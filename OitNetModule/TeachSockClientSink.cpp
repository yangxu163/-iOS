///////////////////////////////////////////////////////////////////////////////////
//			��ѧ����ص���ʵ���ļ� TeachSockClientSink.cpp
//			2014-3-12 hubo
///////////////////////////////////////////////////////////////////////////////////
#include "TeachSockClientSink.h"
#include "localdef.h"

///////////////////////////////////////////////////////////////////////////////////
CTeachSockClientSink::CTeachSockClientSink(void)
{
	m_bLogined=FALSE;
	m_pINetDataWndSink=NULL;
	m_pIMediaDataManage=NULL;
}

CTeachSockClientSink::~CTeachSockClientSink(void)
{
}

//��ʼ����ѧ����ͻ���
bool CTeachSockClientSink::InitTeachSockClient(IBlackboardManage *pINetDataWndSink,IMediaDataManage *pIMediaDataManage)
{
	m_pINetDataWndSink=pINetDataWndSink;
	m_pIMediaDataManage=pIMediaDataManage;
	return true;
}

//����������Ϣ
bool CTeachSockClientSink::OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocket)
{
	if(iErrorCode!=0)	//����ʧ��
	{
		NetMsgAutoParam(OT_CONNECT_FAILD,TEXT("���ӽ�ѧ������ʧ��,%s\n"),pszErrorDesc);
		return false;
	}

	return LoginToTeachServer();
}

//�����ȡ��Ϣ
bool CTeachSockClientSink::OnSocketRead(CMD_Command Command, void * pBuffer,WORD wDataSize, IClientSocket * pIClientSocket)
{
	switch(Command.wSubCmd)
	{
	case TSVR_TEACHER_AUDIO:
	case TSVR_STUDENT_AUDIO:
		return OnProcStudentAudioData(Command.dwSequenceID,pBuffer,wDataSize);
	case TSVR_TEACHER_VIDEO:
		return OnProcUserVideoData(Command.dwSequenceID,pBuffer,wDataSize);
	case TSVR_T_BB_EVENT:
		return OnProcStudentBoardData(Command.wSubCmd,Command.dwSequenceID,pBuffer,wDataSize);
	case TSVR_T_OPT_EVENT:
		return OnProcOperateEventData(Command.wSubCmd,Command.dwSequenceID,pBuffer,wDataSize);
	case ACITVE_TEST:
		return OnProcActiveTest(pIClientSocket);
	case TSVR_H264_SPS:
		return OnProcH264SpsPpsData(pBuffer,wDataSize);
	case TSVR_H264_SPS_RESP:
		return true;
	case TSVR_STUDENT_LOGIN_RESP:
		return OnProcLoginSvrResp(Command.dwSequenceID,pBuffer,wDataSize);
	case TSVR_TEACH_EXIT_ROOM:
		return OnProcTeacherExitRoom(Command.dwSequenceID,pBuffer,wDataSize);
	#if TEST_RECV_DAT
	case TSVR_TEACHER_VIDEO:
		return OnProcTeacherVideoData(Command.dwSequenceID,pBuffer,dwDataSize);
	#endif
	}

	return true;
}

#if TEST_RECV_DAT
//������ʦ����Ƶ��
bool CTeachSockClientSink::OnProcTeacherVideoData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	TRACE(TEXT("�յ���Ƶ���ݣ���С��%ld\n"),dwDataSize);
	return true;
}
#endif

//����ر���Ϣ
bool CTeachSockClientSink::OnSocketClose(IClientSocket * pIClientSocket, bool bCloseByServer)
{
	if(!bCloseByServer) return false;

	OnProcTeacherExitRoom(0,NULL,0);
	return true;
}

//���ӵ���ѧ������
bool CTeachSockClientSink::ConnectToTeachServer()
{
	ASSERT(m_pIClientSocket!=NULL);
	if(m_pIClientSocket==NULL) return false;
    
    if (strlen(g_ConfigParam.szTeachSvrUrl) <= 0) {
        NetMsgNotify(TEXT("Teacher Url is NULL"));
        return false;
    }
	NetMsgNotify(TEXT("Start connect to teaching server...\n"));
	return m_pIClientSocket->ConnectToServer(g_ConfigParam.szTeachSvrUrl,g_ConfigParam.wTeachSvrPort);
}

//�ر�����
bool CTeachSockClientSink::CloseTeachConnect()
{
	if(m_pIClientSocket!=NULL)
		m_pIClientSocket->CloseSocket(false);

	return true;
}

//��¼����ѧ������
bool CTeachSockClientSink::LoginToTeachServer()
{
	TCHAR szMsgTxt[128]="";
	sprintf(szMsgTxt,TEXT("------Start login to Teaching server ClienType:%d\n"),g_ConfigParam.wClientType);
	NetMsgNotify(szMsgTxt);

	m_bLogined=FALSE;

	BYTE cbBuffer[256];
	BYTE *pData=cbBuffer;

	WORD wSendSize=0;
	WORD wLength=(WORD)strlen(g_ConfigParam.szUserName);

	pData=WriteDword(pData,g_ConfigParam.dwDstRoomID,wSendSize);
	pData=WriteDword(pData,g_ConfigParam.dwUserID,wSendSize);
	pData=WriteWord(pData,g_ConfigParam.wClientType,wSendSize);
	pData=WriteWord(pData,wLength,wSendSize);
	pData=WriteFromBuffer(pData,g_ConfigParam.szUserName,wLength,wSendSize);
	wLength=(WORD)strlen(g_ConfigParam.szCheckCode);
	pData=WriteWord(pData,wLength,wSendSize);
	pData=WriteFromBuffer(pData,g_ConfigParam.szCheckCode,wLength,wSendSize);

	return m_pIClientSocket->SendData(TEACH_MAIN_CMD,TSVR_STUDENT_LOGIN,0,cbBuffer,wSendSize);
}

//������¼Response
bool CTeachSockClientSink::OnProcLoginSvrResp(DWORD dwSequenceID,void *pDataBuffer,WORD wDataSize)
{
	ASSERT(wDataSize>=8);
	if(wDataSize<8) return false;

	int nResult=0;
	WORD wLength=0;

	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadInt32(pData,nResult);

	TCHAR szMsgTxt[128]="";
	sprintf(szMsgTxt,TEXT("------Teaching server OnProcLoginSvrResp,wDataSize:%d,Result:%d"),wDataSize,nResult);
	NetMsgNotify(szMsgTxt);
	if(nResult!=0)
	{
		m_pIMediaDataManage->OnErrMsgNotify(nResult,TEXT("��¼��ѧ������ʧ��"));
		return false;
	}
	pData=ReadDword(pData,g_ConfigParam.dwRoomIndex);
	
	m_bLogined=TRUE;
	//���Ӽ�ⶨʱ��
	m_dwRecvTime=::GetTickCount();
	wDataSize-=8;
	return m_pINetDataWndSink->OnBlackboardEventData(BEID_BOARD_SIZE,pData,wDataSize,0);
	 
	NetMsgNotify(TEXT("Login to teaching server success\n"),OT_LOGIN_TEACHSVR_SUCCESS);
	
	return true;
}

//����ѧ����Ƶ����
bool CTeachSockClientSink::OnProcStudentAudioData(DWORD dwSequenceID,void *pDataBuffer,WORD wDataSize)
{ 
	if(m_pIMediaDataManage!=NULL)
	{
		m_pIMediaDataManage->OnAudioRTData(pDataBuffer,wDataSize,0);
	}
	return true;
}

//����ѧ���켣����
bool CTeachSockClientSink::OnProcStudentBoardData(WORD wSubCmd,DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	//���Ƶ�����
	if(m_pINetDataWndSink==NULL) return false;
	BOARDEVENTID beid;
	WORD wBoardCmd=0;
	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadWord(pData,wBoardCmd);
	beid=(BOARDEVENTID)wBoardCmd;
	dwDataSize-=2;
	return m_pINetDataWndSink->OnBlackboardEventData(beid,pData,dwDataSize,0);
}

//���������¼�����
bool CTeachSockClientSink::OnProcOperateEventData(WORD wSubCmd,DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	//���Ƶ�����
	ASSERT(m_pINetDataWndSink != NULL);
	if(m_pINetDataWndSink==NULL) return false;
	OPEREVENTID oeid;
	WORD wBoardCmd=0;
	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadWord(pData,wBoardCmd);
	oeid=(OPEREVENTID)wBoardCmd;
	dwDataSize-=2;
	return m_pINetDataWndSink->OnRevOperateEventData(oeid,pData,dwDataSize,0);
}


//������ƵH264SpsPps����
bool CTeachSockClientSink::OnProcH264SpsPpsData(void *pDataBuffer,DWORD dwDataSize)
{
	if(pDataBuffer==NULL || dwDataSize<5) return true;
	if(m_pIMediaDataManage==NULL) return true;

	const int MAXDATA=1024;
	BYTE cbBuffer[MAXDATA];
	DWORD dwChannelID=0;
	WORD wDataLen=0;
	BYTE *pData=(BYTE *)pDataBuffer;
	DWORD dwCount=pData[0];
	pData++;
	for(DWORD i=0;i<dwCount;i++)
	{
		pData=ReadDword(pData,dwChannelID);
		pData=ReadWord(pData,wDataLen);
		if(wDataLen>=MAXDATA) return true;
		pData=ReadToBuffer(pData,cbBuffer,wDataLen);
		m_pIMediaDataManage->OnH264SpsPpsData(dwChannelID,cbBuffer,wDataLen);
	}

	return true;
}

//�����û���Ƶ����
bool CTeachSockClientSink::OnProcUserVideoData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	if(m_pIMediaDataManage==NULL) return true;

	DWORD dwChannelID=0;
	BYTE *pDstData=(BYTE *)pDataBuffer;
	memcpy(&dwChannelID,pDstData,sizeof(DWORD));
	dwChannelID=ntohl(dwChannelID);
	pDstData+=sizeof(DWORD);
	dwDataSize-=sizeof(DWORD);

	m_pIMediaDataManage->OnVideoH264CodeData(dwChannelID,pDstData,dwDataSize);

	return true;
}

//������ʦ�˳�����
bool CTeachSockClientSink::OnProcTeacherExitRoom(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	m_pIMediaDataManage->OnErrMsgNotify(-11,TEXT("��ʦ�˳����ң�����������ӶϿ�"));
	return false;	//���� false �Ͽ�����
}