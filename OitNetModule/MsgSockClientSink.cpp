//////////////////////////////////////////////////////////////////////////////////////
//		���Ʒ���������ͻ��˻ص���ͷ�ļ� CtrlSockClientSink.h
//		bowen 2014-2-20
///////////////////////////////////////////////////////////////////////////////////////
#include "MsgSockClientSink.h"
#include "localdef.h"
#include "mul_timer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
///////////////////////////////////////////////////////////////////////////////////////
CMsgSockClientSink::CMsgSockClientSink(void)
{
    m_bLogining=FALSE;
    m_cst=CST_LBSVR;
    m_pIKernelDataSink=NULL;
    m_pTimerHandle=NULL;
    m_pTeachSvrTime=NULL;
    m_pISockSinkManage=NULL;
    m_pIMediaDataManage=0;
}

CMsgSockClientSink::~CMsgSockClientSink(void)
{
}

//��ʼ����������ӿ�
bool CMsgSockClientSink::InitMsgSocketClient(ISockSinkManage *pISockSinkManage,IMediaDataManage *pIMediaDataManage)
{
    m_pISockSinkManage=pISockSinkManage;
    m_pIMediaDataManage=pIMediaDataManage;
    
    return true;
}

//��������ӿ�
bool CMsgSockClientSink::SetKernelDataSink(IKernelDataSink *pIKernelDataSink)
{
    m_pIKernelDataSink =pIKernelDataSink;
    return true;
}

//���ӵ�LB������
bool CMsgSockClientSink::ConnectToServer(CONNECTSVRTYPE cst)
{
    if(cst==CST_TEACHSVR)
    {
        if(m_pISockSinkManage!=NULL)
            m_pISockSinkManage->ConnectToServer(CST_TEACHSVR);
        return true;
    }
    
    ASSERT(m_pIClientSocket!=NULL);
    if(m_pIClientSocket==NULL) return false;
    CHAR szSvrUrl[64]="";
    WORD wSvrPort=0;
    DWORD dwServerIP=0;
    m_cst=cst;
    switch(cst)
    {
        case CST_LBSVR:
            m_bLogining=TRUE;
            //		dwServerIP=ConvertUrlToIpAddr(g_ConfigParam.szLBSvrUrl);
            strcpy(szSvrUrl, g_ConfigParam.szLBSvrUrl);
            wSvrPort=g_ConfigParam.wLBSvrPort;
            NetMsgNotify(TEXT("Start connect to lb server...\n"));
            break;
        case CST_LOGINSVR:
            //		dwServerIP=ConvertUrlToIpAddr(g_ConfigParam.szServerUrl);
            strcpy(szSvrUrl, g_ConfigParam.szServerUrl);
            wSvrPort=g_ConfigParam.wServerPort;
            NetMsgNotify(TEXT("��ʼ���ӵ���¼������...\n"));
            break;
        case CST_MSGSVR:
        {
            m_bLogining=FALSE;
            dwServerIP=g_ConfigParam.dwCtrlSvrIP;
            
            in_addr inaddr;
            //memcpy(&inaddr,&dwServerIP,sizeof(dwServerIP));
            inaddr.s_addr=dwServerIP;
            TCHAR *pIpAddr=inet_ntoa(inaddr);
            strcpy(szSvrUrl, pIpAddr);
            
            
            
            wSvrPort=g_ConfigParam.wCtrlSvrPort;
            NetMsgNotify(TEXT("Start connect to msg server...\n"));
        }
            break;
        default:
            return false;
    }
    
    return m_pIClientSocket->ConnectToServer(szSvrUrl,wSvrPort);
}

//�ر�����
bool CMsgSockClientSink::CloseMsgConnect()
{
    NetMsgAutoParam(OT_CONNECT_FAILD,TEXT("--------*****----ccc----Msg server start close,0x%x\n"),m_pIClientSocket);
    if(m_pIClientSocket!=NULL)
        m_pIClientSocket->CloseSocket(false);
    
    return true;
}

//����������Ϣ
bool CMsgSockClientSink::OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc,IClientSocket * pIClientSocket)
{
    if(iErrorCode!=0)
    {
        switch(m_cst)
        {
            case CST_LBSVR:
                NetMsgAutoParam(OT_CONNECT_FAILD,TEXT("���Ӹ��ؾ��������ʧ��,%s,���Ժ����ԣ�\n"),pszErrorDesc);
                break;
            case CST_LOGINSVR:
                NetMsgAutoParam(OT_CONNECT_FAILD,TEXT("���ӵ�¼������ʧ��,%s,���Ժ����ԣ�\n"),pszErrorDesc);
                break;
            case CST_MSGSVR:
                NetMsgAutoParam(OT_CONNECT_FAILD,TEXT("���ӿ��Ʒ�����ʧ��,%s,���Ժ����ԣ�\n"),pszErrorDesc);
                break;
        }
        if(m_cst==CST_MSGSVR)
        {
            CMulTimer *pMulTimer=CMulTimer::GetMulTimerInstance();
            pMulTimer->StartTimer((int)(3*1000/30),OnConnectMsgSvr,this);
        }
        return true;
    }
    
    bool bSuccess=true;
    switch(m_cst)
    {
        case CST_LBSVR:
            bSuccess=RequestSvrIPPort();
            break;
        case CST_LOGINSVR:
            bSuccess=LoginToLoginSvr();
            break;
        case CST_MSGSVR:
            bSuccess=LoginToMsgSvr();
            break;
    }
    
    return bSuccess;
}

//�����ȡ��Ϣ
bool CMsgSockClientSink::OnSocketRead(CMD_Command Command, void * pBuffer,WORD wDataSize,IClientSocket * pIClientSocket)
{
    //NetMsgAutoParam(OT_NOOPTION,TEXT("------OnSocketRead,SubCmd:%d,datasize:%d"),Command.wSubCmd,wDataSize);
    switch(Command.wSubCmd)
    {
        case MSVR_SEND_MSG:	//������Ϣ
        case MSVR_SYS_MSG:	//ϵͳ��Ϣ
            return OnProcServerSwitchMsg(Command.wSubCmd,Command.dwSequenceID,pBuffer,wDataSize);
        case MSVR_SEND_MSG_RESP:	//����Response
            return OnProcSendMsgResp(Command.dwSequenceID,pBuffer,wDataSize);
        case ACITVE_TEST:
            return OnProcActiveTest(pIClientSocket);
        case MSVR_USER_LOGIN_RESP:
            return OnProcLoginMsgSvrResp(Command.dwSequenceID,pBuffer,wDataSize);
        case LSVR_STUDENT_LOGIN:
            return OnProcLoginResponse(Command.dwSequenceID,pBuffer,wDataSize);
        case LBS_REQUEST_SVRIP_RESP:
            return OnProcRequestSvrIPPortResp(Command.dwSequenceID,pBuffer,wDataSize);
            
    }
    return true;
}

//����ر���Ϣ
bool CMsgSockClientSink::OnSocketClose(IClientSocket * pIClientSocket, bool bCloseByServer)
{
    NetMsgAutoParam(OT_CONNECT_FAILD,TEXT("��Ϣ������[%d]���ӶϿ�,Close by server:%d"),m_cst,bCloseByServer);
    
    if(!bCloseByServer) return true;
    if(m_bLogining)
    {
        NetMsgNotify(TEXT("��¼�������Ͽ�����\n"));
        return true;
    }
    
    NetMsgNotify(TEXT("��Ϣ�������Ͽ�����\n"));
    if(m_cst==CST_MSGSVR)
    {
        CMulTimer *pMulTimer=CMulTimer::GetMulTimerInstance();
        pMulTimer->StartTimer((int)(3*1000/30),OnConnectMsgSvr,this);
    }
    return true;
}

bool CMsgSockClientSink::OnMyTimer(void *pObject)
{
    NetMsgNotify(TEXT("--------OnMyTimer sink call...\n"));
    CMsgSockClientSink *pMsgSocket=(CMsgSockClientSink *)pObject;
    if(pMsgSocket==NULL) return false;
    
    pMsgSocket->ConnectToServer(CST_MSGSVR);
    
    return false;	//false��ɾ����ʱ��
}

bool CMsgSockClientSink::OnTeachSvrTime(void *pObject)
{
    NetMsgNotify(TEXT("--------OnTeachSvrTime sink call...\n"));
    CMsgSockClientSink *pMsgSocket=(CMsgSockClientSink *)pObject;
    if(pMsgSocket==NULL) return false;
    
    pMsgSocket->ConnectToServer(CST_TEACHSVR);
    
    return false;
}

bool CMsgSockClientSink::OnConnectMsgSvr(void *pObject)
{
    CMsgSockClientSink *pMsgSocket=(CMsgSockClientSink *)pObject;
    if(pMsgSocket==NULL) return false;
    pMsgSocket->ConnectToServer(CST_MSGSVR);
    return false;
}

//�����������ַ
bool CMsgSockClientSink::RequestSvrIPPort()
{
    //TRACE(TEXT("���Ӹ��ؾ���������ɹ����������������...\n"));
    NetMsgNotify(TEXT("connected loadbalance server,request msg server ip...\n"));
    
    BYTE cbBuffer[8];
    BYTE *pData=cbBuffer;
    
    WORD wSendSize=0;
    pData=WriteDword(pData,ST_MSGSVR,wSendSize);
    
    return m_pIClientSocket->SendData(ALL_MAIN_CMD,LBS_REQUEST_SVRIP,0,cbBuffer,wSendSize);
}

//���������������ַResponse
bool CMsgSockClientSink::OnProcRequestSvrIPPortResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
    ASSERT(dwDataSize==10);
    if(dwDataSize!=10) return false;
    CHAR *pTmpIP=NULL;
    in_addr addrin;
    DWORD dwSvrType=0;
    DWORD dwSvrIp=0;
    BYTE *pData=(BYTE *)pDataBuffer;
    //pData=ReadDword(pData,dwSvrType);
    //ASSERT((ITESERVERTYPE)dwSvrType & ST_LOGINSVR);	//��¼������
    //pData=ReadDword(pData,dwSvrIp);
    //addrin.s_addr=dwSvrIp;
    //pTmpIP=inet_ntoa(addrin);
    //strcpy(g_ConfigParam.szServerUrl,pTmpIP);
    //pData=ReadWord(pData,g_ConfigParam.wServerPort);
    
    pData=ReadDword(pData,dwSvrType);
    ASSERT((ITESERVERTYPE)dwSvrType & ST_MSGSVR);	//��Ϣ������
    pData=ReadDword(pData,dwSvrIp);
    addrin.s_addr=dwSvrIp;
    pTmpIP=inet_ntoa(addrin);
    strcpy(g_ConfigParam.szCtrlSvrUrl,pTmpIP);
    pData=ReadWord(pData,g_ConfigParam.wCtrlSvrPort);
    
    if(g_ConfigParam.wCtrlSvrPort==0)
    {
        NetMsgNotify(TEXT("Response msg server ip invalid��please reconnect...\n"),OT_LOGIN_FAILD);
        return false;
    }
    
    g_ConfigParam.dwCtrlSvrIP=ConvertUrlToIpAddr(g_ConfigParam.szCtrlSvrUrl);
    
    //���ӵ���¼������
    NetMsgNotify(TEXT("Request msg server ip success��start connect to msg server ...\n"));
    
    CMulTimer *pMulTimer=CMulTimer::GetMulTimerInstance();
    if(pMulTimer!=NULL)
    {
        NetMsgNotify(TEXT("---------Start run connect to msg server timer ...\n"));
        try{
            m_nTimerID=0;
            m_pTimerHandle=pMulTimer->StartTimer(1,OnMyTimer,this);
        }catch(...)
        {
            NetMsgNotify(TEXT("---------Start timer exception\n"));
        }
        NetMsgAutoParam(OT_NOOPTION,TEXT("---------Timer waw add,handle:%x\n"),m_pTimerHandle);
    }
    
    return false;
}

//��¼����¼������
bool CMsgSockClientSink::LoginToLoginSvr()
{
    if(m_pIClientSocket==NULL)
        return false;
    
    NetMsgNotify(TEXT("���ӳɹ�����ʼ��¼...\n"));
    //TRACE(TEXT("���ӳɹ�����ʼ��¼����¼������......\n"));
    
    BYTE cbBuffer[256];
    BYTE *pData=cbBuffer;
    
    WORD wSendSize=0;
    WORD wNameLength=(WORD)strlen(g_ConfigParam.szUserName);
    WORD wPassLength=(WORD)strlen(g_ConfigParam.szPassword);
    
    pData=WriteWord(pData,g_ConfigParam.wClientType,wSendSize);
    pData=WriteWord(pData,wNameLength,wSendSize);
    pData=WriteFromBuffer(pData,(BYTE *)g_ConfigParam.szUserName,wNameLength,wSendSize);
    pData=WriteWord(pData,wPassLength,wSendSize);
    pData=WriteFromBuffer(pData,(BYTE *)g_ConfigParam.szPassword,wPassLength,wSendSize);
    
    return m_pIClientSocket->SendData(LOGIN_MAIN_CMD,LSVR_STUDENT_LOGIN,0,cbBuffer,wSendSize);
}

//������¼Response
bool CMsgSockClientSink::OnProcLoginResponse(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
    ASSERT(dwDataSize>=4);
    if(dwDataSize<4)	return false;
    
    WORD wNameLen=0;
    WORD wCheckCodeLen=0;
    int nResult=0;
    DWORD dwClassRoomID=0;
    DWORD dwTeacherID=0;
    const int nCodeLength=CHECKCODE_MAXLEN;
    g_ConfigParam.szCheckCode[0]=0;
    memset(g_ConfigParam.szRealName,0,sizeof(g_ConfigParam.szRealName));
    
    BYTE *pData=(BYTE *)pDataBuffer;
    pData=ReadInt32(pData,nResult);
    if(nResult==0)
    {
        pData=ReadDword(pData,g_ConfigParam.dwUserID);
        pData=ReadWord(pData,wNameLen);
        if(wNameLen>=NAME_LEN) return false;
        pData=ReadToBuffer(pData,(BYTE *)g_ConfigParam.szRealName,wNameLen);
        g_ConfigParam.szRealName[wNameLen]=0;
        
        pData=ReadWord(pData,wCheckCodeLen);
        if(wCheckCodeLen>=nCodeLength)
            wCheckCodeLen=nCodeLength-1;
        pData=ReadToBuffer(pData,(BYTE *)g_ConfigParam.szCheckCode,wCheckCodeLen);
        g_ConfigParam.szCheckCode[wCheckCodeLen]=0;
        
#ifdef _UNICODE
        ::MultiByteToWideChar(CP_ACP,0,g_ConfigParam.szRealName,-1,g_ConfigParam.szwRealName,CountArray(g_ConfigParam.szwRealName));
#else
        lstrcpy(g_ConfigParam.szwRealName,g_ConfigParam.szRealName);
#endif
        
        NetMsgNotify(TEXT("��¼�ɹ����ȴ���������...\n"));
    }
    else
    {
        NetMsgAutoParam(OT_LOGIN_FAILD,TEXT("��¼ʧ��,�����룺%d,���Ժ�����\n"),nResult);
        return false;
    }
    
    return true;
}

//��¼����Ϣ������
bool CMsgSockClientSink::LoginToMsgSvr()
{
    NetMsgNotify(TEXT("Connect to msg server success,begin login to server...\n"));
    
    WORD wNameLen=0;
    WORD wSendSize=0;
    DWORD dwTimeSpan=::GetTickCount();
    TCHAR szCheckCode[33]=TEXT("");
    BYTE cbBuffer[128];
    BYTE *pData=cbBuffer;
    pData=WriteDword(pData,g_ConfigParam.dwUserID,wSendSize);
    pData=WriteByte(pData,(BYTE)UT_STUDENT,wSendSize);
    wNameLen=strlen(g_ConfigParam.szUserName);
    pData=WriteWord(pData,wNameLen,wSendSize);
    pData=WriteFromBuffer(pData,g_ConfigParam.szUserName,wNameLen,wSendSize);
    pData=WriteFromBuffer(pData,g_ConfigParam.szCheckCode,32,wSendSize);
    pData=WriteDword(pData,dwTimeSpan,wSendSize);
    
    bool bSended=m_pIClientSocket->SendData(MSG_MAIN_CMD,MSVR_USER_LOGIN,0,cbBuffer,wSendSize);
    
    return bSended;
}

//������¼����Ϣ������Response
bool CMsgSockClientSink::OnProcLoginMsgSvrResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
    if(dwDataSize!=4) return false;
    
    int nResult=0;
    WORD wLength=0;
    BYTE cbRedirect=0;
    
    BYTE *pData=(BYTE *)pDataBuffer;
    pData=ReadInt32(pData,nResult);
    
    if(nResult!=0)
    {
        if(nResult==CREATE_ROOM_FAILD)
            NetMsgNotify(TEXT("You relogin on the server,connect will close��\n"),OT_LOGIN_CTRLSVR_FAILD);
        else
            NetMsgNotify(TEXT("Login to msg server waw faild��account was error\n"),OT_LOGIN_CTRLSVR_FAILD);
        return true;
    }
    
    //���Ӽ�ⶨʱ��
    m_dwRecvTime=::GetTickCount();
    NetMsgNotify(TEXT("---Login to msg server success,you can send message to other user\n"),OT_LOGIN_CTRL_SUCCESS);
    
    if(g_ConfigParam.dwDstRoomID>0 && m_pISockSinkManage!=NULL)
    {
        NetMsgNotify(TEXT("---Start send login notify msg,RoomID\n"),OT_LOGIN_CTRL_SUCCESS);
        
        const int MSGMAX=64;
        CHAR szMsgTxt[MSGMAX]="";
        CHAR szRealName[WNAME_LEN]="";
        sprintf(szMsgTxt,"{\"uid\":%u,\"rname\":\"%s\",\"roomid\":%u}",
                g_ConfigParam.dwUserID,g_ConfigParam.szRealName,g_ConfigParam.dwDstRoomID);
        WORD wMsgLen=strlen(szMsgTxt);
        m_pISockSinkManage->SendTextMsgToMsgSvr(g_ConfigParam.dwDstRoomID,1,1,TMT_SINROOM,0,0,0,szMsgTxt,wMsgLen);
        
        NetMsgNotify(szMsgTxt,OT_LOGIN_CTRL_SUCCESS);
    }
    
    if(m_pIMediaDataManage!=NULL)
        m_pIMediaDataManage->OnErrMsgNotify(0,TEXT("Msg server login success"));
    
    return true;
}

//�����ı���Ϣ
bool CMsgSockClientSink::OnProcServerSwitchMsg(WORD wCmdID,DWORD dwSequenceID,const void *pDataBuffer,WORD wDataSize)
{
    if(pDataBuffer==NULL) return true;
    if(wDataSize<=14) return true;
    
    DWORD dwSrcUserID=0;
    DWORD dwDstUserID=0;
    BYTE cbSrcType=0;
    BYTE cbDstType=0;
    BYTE cbRelationType=0;
    BYTE cbMsgType=0;
    DWORD dwFileSize=0;
    DWORD dwDuration=0;
    DWORD dwMsgID=0;
    DWORD dwSendTime=0;
    WORD wMsgLength=0;
    TCHAR szMsgTxt[1024];
    BYTE *pData=NULL;
    pData=(BYTE *)pDataBuffer;
    pData=ReadDword(pData,dwSrcUserID);
    pData=ReadDword(pData,dwDstUserID);
    pData=ReadByte(pData,cbSrcType);
    pData=ReadByte(pData,cbDstType);
    pData=ReadByte(pData,cbRelationType);
    pData=ReadByte(pData,cbMsgType);
    pData=ReadWord(pData,wMsgLength);
    pData=ReadToBuffer(pData,szMsgTxt,wMsgLength);
    pData=ReadDword(pData,dwFileSize);
    pData=ReadDword(pData,dwDuration);
    pData=ReadDword(pData,dwMsgID);
    pData=ReadDword(pData,dwSendTime);
    szMsgTxt[wMsgLength]=0;
    
    TCHAR szMsg[1248]=TEXT("");
    if(wCmdID==MSVR_SYS_MSG)
        sprintf(szMsg,TEXT("----Recv message MsgID:%ld,MsgType:%d,Content[%s]\n"),dwMsgID,cbMsgType,szMsgTxt);
    else
        sprintf(szMsg,TEXT("----Recv message MsgID:%ld,MsgType:%d,Content[%s]\n"),dwMsgID,cbMsgType,szMsgTxt);
    
    NetMsgNotify(szMsg);
    
    if(cbMsgType>=TMT_JSON)
    {
        return OnProcJsonMsg(cbMsgType,szMsgTxt,wMsgLength);
    }
    
    
    NetMsgNotify(szMsg);
    
    if(wCmdID==MSVR_SYS_MSG)
    {
        WORD wSendSize=0;
        BYTE cbBuffer[12];
        pData=cbBuffer;
        pData=WriteInt32(pData,0,wSendSize);
        pData=WriteDword(pData,dwMsgID,wSendSize);
        m_pIClientSocket->SendData(MSG_MAIN_CMD,MSVR_SYS_MSG_RESP,dwSequenceID,cbBuffer,wSendSize);
    }
    
#ifdef __IPHONE_4_0
    if(m_pIKernelDataSink !=NULL)
    {
        m_pIKernelDataSink->OnRecvMsgData(dwSrcUserID,dwDstUserID,cbSrcType,cbDstType,cbRelationType,cbMsgType,szMsgTxt,dwFileSize,dwDuration,dwMsgID,dwSendTime);
    }
#else
    char *jsonBuf = (char*)malloc(wMsgLength+256);
    
    sprintf(jsonBuf,"{\"fromId\":%d,\"toId\":%d,\"fromUserType\":%d,\"toUserId\":%d,\"toType\":%d,\"msgtype\":%d,\"msg\":\"%s\",\"fileSize\":%d,\"timeLen\":%d,\"msgId\":%d,\"time\":%d}",
            dwSrcUserID,dwDstUserID,cbSrcType,cbDstType,cbRelationType,cbMsgType,szMsgTxt,dwFileSize,dwDuration,dwMsgID,dwSendTime);
    //���͵�java��
    OnJavaMsgSink(jsonBuf,TMT_TXT);
    
    free(jsonBuf);
#endif
    return true;
}

//����������ϢResponse
bool CMsgSockClientSink::OnProcSendMsgResp(DWORD dwSequenceID,const void *pDataBuffer,WORD wDataSize)
{
    if(wDataSize<12) return false;
    
    int nResult=0;
    DWORD dwMsgID=0;
    DWORD dwTime=0;
    
    BYTE *pData=(BYTE *)pDataBuffer;
    pData=ReadInt32(pData,nResult);
    pData=ReadDword(pData,dwMsgID);
    pData=ReadDword(pData,dwTime);
    
#ifdef __IPHONE_4_0
    if(m_pIKernelDataSink !=NULL)
    {
        m_pIKernelDataSink->OnRecvMsgResp(nResult,dwMsgID,dwTime,dwSequenceID);
    }
#else
    TCHAR szMsgJson[128]=TEXT("");
    
    sprintf(szMsgJson,"{\"status\":%d,\"msgId\":%d,\"sequence\":%d,\"time\":%d}",nResult,dwMsgID,dwSequenceID,dwTime);
    //���͵�java��
    OnJavaMsgSink(szMsgJson,TMT_MSGRESP);
#endif
    
    return true;
}

//����Json��Ϣ
bool CMsgSockClientSink::OnProcJsonMsg(int nMsgType,LPCSTR lpszUtf8Txt,int nMsgSize)
{
    const int MAXMSG=1024;
    CHAR szMsgTxt[MAXMSG];
    lstrcpy(szMsgTxt,lpszUtf8Txt);
    int nLength=lstrlen(szMsgTxt);	//UTF8ToGBK(lpszUtf8Txt,szMsgTxt,MAXMSG);
    if(nLength<=0) return false;
    
    switch(nMsgType)
    {
        case TMT_TINROOM:
            OnProcTeacherInRoom(szMsgTxt);	break;
        case TMT_SINROOM:
            OnProcStudentInRoom(szMsgTxt); break;
    }
    
    return true;
}

DWORD ReadDwordVal(LPCTSTR lpszJson,LPCTSTR lpszKey)
{
    TCHAR szNumber[NAME_LEN]="";
    LPCTSTR lpszResult=strstr(lpszJson,lpszKey);
    if(lpszResult==NULL) return 0;
    lpszResult+=lstrlen(lpszKey);
    int nLength=lstrlen(lpszResult);
    if(nLength<=1) return 0;
    lpszResult+=1;
    nLength-=1;
    int nIndex=0;
    TCHAR *pDstData=szNumber;
    while(*lpszResult && *lpszResult>='0' && *lpszResult<='9' && nIndex<NAME_LEN)
    {
        *pDstData++=*lpszResult++;
        nIndex++;
    }
    *pDstData=0;
    //CMsgSockClientSink::NetMsgNotify(szNumber);
    
    DWORD dwResult=(DWORD)CharToUInt64(szNumber);
    
    return dwResult;
}

string ReadStringVal(LPCTSTR lpszJson,LPCTSTR lpszKey)
{
    const int MAXVAL=1024;
    TCHAR szValue[MAXVAL]=TEXT("");
    LPCTSTR lpszResult=strstr(lpszJson,lpszKey);
    if(lpszResult==NULL) return "";
    //CMsgSockClientSink::NetMsgNotify(lpszResult);
    lpszResult+=lstrlen(lpszKey);
    int nLength=lstrlen(lpszResult);
    if(nLength<=2) return 0;
    lpszResult+=2;
    nLength-=2;
    
    int nIndex=0;
    TCHAR *pDstData=szValue;
    //CMsgSockClientSink::NetMsgNotify("------------>>>> Start while...");
    while(*lpszResult && *lpszResult!='"' && nIndex<MAXVAL && nLength>0)
    {
        *pDstData++=*lpszResult++;
        nIndex++;
        nLength--;
    }
    *pDstData=0;
    string strValue=szValue;
    
    return strValue;
}

//������ʦ�������
bool CMsgSockClientSink::OnProcTeacherInRoom(LPCTSTR lpszJsonTxt)
{
    try
    {
        const int MAXTXT=512;
        CHAR szMsgTxt[MAXTXT]="";
        
        DWORD dwTeacherID=ReadDwordVal(lpszJsonTxt,"\"tid\"");
        string strSvrIP=ReadStringVal(lpszJsonTxt,"\"tsvrip\"");
        WORD wSvrPort=ReadDwordVal(lpszJsonTxt,"\"tsvrport\"");
        
        sprintf(szMsgTxt,"**********Teaching server info��%ld,%s:%d\n",dwTeacherID,strSvrIP.c_str(),wSvrPort);
        NetMsgNotify(szMsgTxt);
        
        if(dwTeacherID==0 || wSvrPort==0 || wSvrPort>65535) return false;
        
        g_ConfigParam.wTeachSvrPort=wSvrPort;
        strcpy(g_ConfigParam.szTeachSvrUrl,strSvrIP.c_str());
        
        CMulTimer *pMulTimer=CMulTimer::GetMulTimerInstance();
        if(pMulTimer!=NULL)
        {
            NetMsgNotify(TEXT("---->>>-----Start run connect to teaching server timer ...\n"));
            try{
                m_nTimerID=1;
                m_pTeachSvrTime=pMulTimer->StartTimer(1,OnTeachSvrTime,this);
            }catch(...)
            {
                NetMsgNotify(TEXT("---------Start timer exception\n"));
            }
        }
        else
        {
            NetMsgNotify(TEXT("---------<<<<<<<<<< timer object is null ...\n"));
        }
#ifdef __IPHONE_4_0
        if(m_pIKernelDataSink !=NULL)
        {
            DWORD dwSendTime=::time(NULL);
            sprintf(szMsgTxt,TEXT("%u"),dwTeacherID);
            m_pIKernelDataSink->OnRecvMsgData(dwTeacherID,g_ConfigParam.dwDstRoomID,1,0,1,TMT_TINROOM,szMsgTxt,0,0,0,dwSendTime);
        }
#else
        //֪ͨJava��		
        sprintf(szMsgTxt,"{\"userid\":%u}",dwTeacherID);
        OnJavaMsgSink(szMsgTxt,TMT_TINROOM);
#endif
    }
    catch(...){}
    return true;
}

//����ѧ���������
bool CMsgSockClientSink::OnProcStudentInRoom(LPCTSTR lpszJsonTxt)
{
    try
    {
        //��������
        DWORD dwUserID=ReadDwordVal(lpszJsonTxt,"\"sid\"");
        string strName=ReadStringVal(lpszJsonTxt,"\"rname\"");
#ifdef __IPHONE_4_0
        if(m_pIKernelDataSink !=NULL)
        {
            DWORD dwSendTime=::time(NULL);
            m_pIKernelDataSink->OnRecvMsgData(dwUserID,g_ConfigParam.dwDstRoomID,0,0,1,TMT_SINROOM,strName.c_str(),0,0,0,dwSendTime);
        }
#else
        const int MAXTXT=512;
        CHAR szMsgTxt[MAXTXT]="";
        sprintf(szMsgTxt,"{\"userid\":%u,\"rname\":%s}",dwUserID,strName.c_str());
        OnJavaMsgSink(szMsgTxt,TMT_SINROOM);
#endif
    }
    catch(...){}
    return true;
}