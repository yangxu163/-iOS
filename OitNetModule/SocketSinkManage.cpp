//////////////////////////////////////////////////////////////////////////////////////
//		����ͻ��˻ص�������ʵ���ļ� SocketSinkManage.cpp
//		bowen 2014-2-20
//////////////////////////////////////////////////////////////////////////////////////
#include "SocketSinkManage.h"
#include "mul_timer.h"

////////////////////////////////////////////////////////////////////////////////////
CONFIGDATA g_ConfigParam;
CSocketSinkManage *CSocketSinkManage::m_pSelfObject=NULL;
//////////////////////////////////////////////////////////////////////////////////////
CSocketSinkManage::CSocketSinkManage(void)
{
	m_pMsgSockClient=NULL;
	m_pTeachSockClient=NULL;
	m_pIKernelDataSink=NULL;
	m_pSelfObject=NULL;
	m_dwLastTime=0;
}

CSocketSinkManage::~CSocketSinkManage(void)
{
	CMulTimer::ReleaseMulTimerInstance();
}

//��ʼ������ص���
bool CSocketSinkManage::InitSocketSinkManage(LPCTSTR lpszLBSvrIp,WORD wLBSvrPort)
{
	g_ConfigParam.wLBSvrPort=wLBSvrPort;
	lstrcpy(g_ConfigParam.szLBSvrUrl,lpszLBSvrIp);

	if(!m_MsgSocketSink.InitSocketSink())
	{
		NetMsgNotify(TEXT("��ʼ�����Ʒ���������ͻ���ʧ��"));
		return false;
	}

	if(!m_TeachSocketSink.InitSocketSink())
	{
		NetMsgNotify(TEXT("��ʼ����ѧ����������ͻ���ʧ��"));
		return false;
	}

	IMediaDataManage *pIMediaDataManage=(IMediaDataManage *)&m_MediaDataManage;

	ISockSinkManage *pISockSinkManage=(ISockSinkManage *)this;
	m_MsgSocketSink.InitMsgSocketClient(pISockSinkManage,pIMediaDataManage);

#if HAVE_BBEVETN
	IBlackboardManage *pIBlackboardManage=(IBlackboardManage *)&m_BoardEventData;
#else
	IBlackboardManage *pIBlackboardManage=NULL;
#endif

	m_TeachSocketSink.InitTeachSockClient(pIBlackboardManage,pIMediaDataManage);
	
	m_pMsgSockClient=m_MsgSocketSink.GetSocketInterface();
	ASSERT(m_pMsgSockClient!=NULL);
	
	m_pTeachSockClient=m_TeachSocketSink.GetSocketInterface();
	ASSERT(m_pTeachSockClient!=NULL);

	g_ConfigParam.wClientType=CT_STUDENT_IOS;

	bool bInited=m_MediaDataManage.InitMediaManage();

	#ifdef ANDROID_OS
	bInited=m_AudioManager.InitAudioManage();
	IAudioManager *pIAudioManager=(IAudioManager *)&m_AudioManager;
	m_MediaDataManage.SetAudioManage(pIAudioManager);
	IAudioDataSink *pIAudioDataSink=(IAudioDataSink *)this;
	m_AudioManager.SetAudioDataSink(pIAudioDataSink);
	#endif

	m_PlayCWThread.InitPlayThread(pIBlackboardManage,pIMediaDataManage);

	return bInited;
}

//�����û���Ϣ
bool CSocketSinkManage::SetUserInfoData(DWORD dwUserID,WORD wUserType,LPCTSTR lpszUserName,LPCTSTR lpszRealName,LPCTSTR lpszCheckCode)
{
	g_ConfigParam.dwUserID=dwUserID;
	g_ConfigParam.wUserType=wUserType;
	
	lstrcpy(g_ConfigParam.szUserName,lpszUserName);
	lstrcpy(g_ConfigParam.szRealName,lpszRealName);
	
	lstrcpy(g_ConfigParam.szCheckCode,lpszCheckCode);
	return true;
}

//���ý�����Ϣ
bool CSocketSinkManage::SetDstRoomData(DWORD dwDstRoomID,DWORD dwTeacherID)
{
	g_ConfigParam.dwDstRoomID=dwDstRoomID;
	g_ConfigParam.dwTeacherID=dwTeacherID;
	if(g_ConfigParam.dwDstRoomID>0)
	{
		NetMsgNotify(TEXT("---Start send login notify msg,RoomID"),OT_LOGIN_CTRL_SUCCESS);

		const int MSGMAX=64;
		CHAR szMsgTxt[MSGMAX]="";
		CHAR szRealName[WNAME_LEN]="";
		sprintf(szMsgTxt,"{\"uid\":%u,\"rname\":\"%s\",\"roomid\":%u}",
			g_ConfigParam.dwUserID,g_ConfigParam.szRealName,g_ConfigParam.dwDstRoomID);
		WORD wMsgLen=strlen(szMsgTxt);
		SendTextMsgToMsgSvr(g_ConfigParam.dwDstRoomID,1,1,TMT_SINROOM,0,0,0,szMsgTxt,wMsgLen);

		NetMsgNotify(szMsgTxt,OT_LOGIN_CTRL_SUCCESS);

		#ifdef ANDROID_OS
		bool bSuccess=m_AudioManager.StartPlaying();		//��������
		if(bSuccess) LOGI(TEXT("------Run OpenSL audio play success"));
		else LOGE(TEXT("------Run OpenSL audio play faild"));
		#endif
	}
	else
	{
		//�رս�������
		if(m_pTeachSockClient!=NULL)
			m_pTeachSockClient->CloseSocket(false);

		#ifdef ANDROID_OS
		if(dwTeacherID>0)
		{
			bool bSuccess=m_AudioManager.StartPlaying();		//��������
			if(bSuccess) LOGI(TEXT("------Run OpenSL audio play success"));
			else LOGE(TEXT("------Run OpenSL audio play faild"));
		}
		else
		{
			m_AudioManager.StopPlaying();
		}
		#endif
	}
	return true;
}

//��ʼ��������
bool CSocketSinkManage::ConnectToServer(CONNECTSVRTYPE cst)
{
	if(m_pMsgSockClient==NULL) return false;

	NetMsgNotify("--------CSocketSinkManage::ConnectToServer....");

	if(cst==CST_TEACHSVR)
	{
		if(m_pTeachSockClient==NULL) return false;
		return m_TeachSocketSink.ConnectToTeachServer();
	}

	return m_MsgSocketSink.ConnectToServer(cst);
}

//��ʼ�������
bool CSocketSinkManage::StartNetService()
{
	if(m_pMsgSockClient==NULL) return false;
	if(m_pTeachSockClient==NULL) return false;
	bool bSuccess=m_pMsgSockClient->StartSocketThread();
	if(!bSuccess) return false;
	bSuccess=m_pTeachSockClient->StartSocketThread();
	if(!bSuccess) return false;
	bSuccess=m_MediaDataManage.StartMediaManage();

	NetMsgAutoParam(OI_EXCEPTION,"---->>>>>>----CSocketSinkManage::StartNetService result:%d",bSuccess);

	return true;
}

//ֹͣ�������
bool CSocketSinkManage::StopNetService()
{
	if(m_pTeachSockClient!=NULL)
	{
		m_pTeachSockClient->StopSocketThread();
	}
	if(m_pMsgSockClient==NULL)
	{
		m_pMsgSockClient->StopSocketThread();
	}
	NetMsgNotify(TEXT("--------->>>>>---Start Stop	MediaDataManage ...... "));
	m_MediaDataManage.StopMediaManage();
	NetMsgNotify(TEXT("--------->>>>>---Start Stop	m_AudioManager ...... "));
	#ifdef ANDROID_OS
	m_AudioManager.StopPlaying();
	m_AudioManager.StopRecording();
	#endif
	NetMsgNotify(TEXT("--------->>>>>---Start Stop	CSocketSinkManage end"));
	return true;
}

//ֹͣ������
void CSocketSinkManage::CloseSocketConnect()
{
	if(m_pMsgSockClient!=NULL && m_pMsgSockClient->GetConnectState()!=SocketState_NoConnect)
	{
		m_pMsgSockClient->CloseSocket(false);
	}
 
	if(m_pTeachSockClient!=NULL && m_pTeachSockClient->GetConnectState()!=SocketState_NoConnect)
	{
		m_pTeachSockClient->CloseSocket(false);
	}
}

//�������ݵ�AVSERVER
bool CSocketSinkManage::SendDataToAVServer(WORD wSubCmd,void * pData,WORD wDataSize,DWORD dwSequenceID)
{
	if(m_pTeachSockClient==NULL) return false;

	return m_pTeachSockClient->SendData(TEACH_MAIN_CMD,wSubCmd,dwSequenceID,pData,wDataSize);
}

//�������ݵ���Ϣ������
bool CSocketSinkManage::SendDataToMsgServer(WORD wSubCmd,void * pData,WORD wDataSize,DWORD dwSequenceID)
{
	if(m_pMsgSockClient==NULL) return false;

	m_pMsgSockClient->SendData(MSG_MAIN_CMD,wSubCmd,dwSequenceID,pData,wDataSize);

	return true;
}

//H264SpsPps����
bool CSocketSinkManage::OnH264SpsPpsData(const void * pHeadData,DWORD dwDataSize)
{
	m_pTeachSockClient->SendData(TEACH_MAIN_CMD,TSVR_H264_SPS,0,(void *)pHeadData,dwDataSize);

	return true;
}

//����ʵ��
CSocketSinkManage *CSocketSinkManage::GetInstance()
{
	if(m_pSelfObject!=NULL) return m_pSelfObject;

	m_pSelfObject=new CSocketSinkManage();

	return m_pSelfObject;
}

//�ͷ�ʵ��
void CSocketSinkManage::ReleaseInstance()
{
	if(m_pSelfObject==NULL) return;

	delete m_pSelfObject;
	m_pSelfObject=NULL;
}

//�����ı���Ϣ
bool CSocketSinkManage::SendTextMsgToMsgSvr(DWORD dwDstID,WORD wDstType,WORD wRelationType,WORD wMsgType,DWORD dwFileSize,DWORD dwDuration,DWORD dwSequenceID,const void * pMsgData,WORD wDataSize)
{
	WORD wSendSize=0;
 
	BYTE cbSrcType=(BYTE)g_ConfigParam.wUserType;
	BYTE cbDstType=(BYTE)wDstType;
	BYTE cbRelationType=(BYTE)wRelationType;
	BYTE cbMsgType=(BYTE)wMsgType;

	BYTE cbBuffer[2048];
	BYTE *pData=cbBuffer;
	pData=WriteDword(pData,g_ConfigParam.dwUserID,wSendSize);
	pData=WriteDword(pData,dwDstID,wSendSize);
	pData=WriteByte(pData,cbSrcType,wSendSize);
	pData=WriteByte(pData,cbDstType,wSendSize);
	pData=WriteByte(pData,cbRelationType,wSendSize);
	pData=WriteByte(pData,cbMsgType,wSendSize);
	pData=WriteWord(pData,wDataSize,wSendSize);	
	pData=WriteFromBuffer(pData,pMsgData,wDataSize,wSendSize);	 
	pData=WriteDword(pData,dwFileSize,wSendSize);
	pData=WriteDword(pData,dwDuration,wSendSize);

	SendDataToMsgServer(MSVR_SEND_MSG,cbBuffer,wSendSize,dwSequenceID);

	return true;
}

//���ú��Ļص��ӿ�
bool CSocketSinkManage::SetKernelDataSink(void *pIKernelDataSink)
{
	m_MediaDataManage.SetKernelDataSink((IKernelDataSink *)pIKernelDataSink);
	return m_MsgSocketSink.SetKernelDataSink((IKernelDataSink *)pIKernelDataSink);
}

//���úڰ����ݻص��ӿ�
bool CSocketSinkManage::SetBoardDataSink(void *pIBoardEventSink)
{
	return m_BoardEventData.SetBoardDataSink((IBoardEventSink *)pIBoardEventSink);
}

//����ý��ص��ӿ�
bool CSocketSinkManage::SetMediaDataSink(void *pIMediaDataSink)
{
	return m_MediaDataManage.SetMediaDataSink((IMediaDataSink *)pIMediaDataSink);
}

//�ͷ��������
void CSocketSinkManage::FreeSockSinkManage()
{
	CSocketSinkManage::FreeSockSinkManage();
}

//����ڰ����
float CSocketSinkManage::ComputeBoardScale(int scx,int scy,int dcx,int dcy,POINT &offset)
{
	int tcx=dcx;
	int tcy=dcy;
	offset.x=0;
	offset.y=0;
	float fScale=1.0f;
	//�������
	if(scx==0 || scy==0) return fScale;
	if(tcx==0 || tcy==0) return fScale;

	double tRate=(double)tcx/(double)tcy;
	double sRate=(double)scx/(double)scy;
	double xyScale=tRate;
	if(tRate>sRate)
	{
		fScale=(float)((double)scx/(double)tcx);
		//����cy
		tcx=scx;
		tcy=(int)((double)scx/xyScale);		
	}
	else
	{
		fScale=(float)((double)scy/(double)tcy);
		tcy=scy;
		tcx=(int)(scy*xyScale);
	}
	offset.x=(scx-tcx)/2;
	offset.y=(scy-tcy)/2;

	return fScale;
}

//���;���Э��
bool CSocketSinkManage::SendUpHandData()
{
	#ifdef ANDROID_OS
	LOGE(TEXT("<<<<<<<<<<<<<-------check---SendUpHandData"));
	#endif
	if(!g_ConfigParam.bUpHanding)
	{
		#ifdef ANDROID_OS
		/*TCHAR szMsg[MAX_PATH]=TEXT("");
		sprintf(szMsg,TEXT("{\"errcode\":%d,\"msg\":\"%s\"}"),-101,TEXT("��ʦû�����ʣ��벻Ҫ��������..."));
		OnJavaErrMsg(szMsg);*/
		LOGE(TEXT("<<<<<<<<<<<<<----------Teacher no question,Please don't up hand"));
		#else
		if(m_pIKernelDataSink!=NULL)
			m_pIKernelDataSink->OnErrMsgNotify(-1,TEXT("��ʦû�����ʣ��벻Ҫ��������..."));
		#endif
		return false;
	}

	//����ѧ��ȡ����Ϣ
	DWORD dwTime=::GetTickCount();
	DWORD dwSpaceTime=dwTime-m_dwLastTime;
	m_dwLastTime=dwTime;
	if(dwSpaceTime<5000)
	{
		#ifdef ANDROID_OS
		LOGE(TEXT("<<<<<<<<<<<<<----------Please don't multi times up hand"));
		/*TCHAR szMsg[MAX_PATH]=TEXT("");		
		sprintf(szMsg,TEXT("{\"errcode\":%d,\"msg\":\"%s\"}"),-101,TEXT("��ҪƵ�����;�������.."));
		OnJavaErrMsg(szMsg);*/
		#else
		if(m_pIKernelDataSink!=NULL)
			m_pIKernelDataSink->OnErrMsgNotify(-1,TEXT("��ҪƵ�����;�������.."));
		#endif
		return false;
	}
	
	#ifdef ANDROID_OS
	LOGE(TEXT("<<<<<<<<<<<<<------start----SendUpHandData"));
	#endif
	WORD wSendSize=0;
	BYTE cbNameLen=0;
	BYTE cbBuffer[64];
	BYTE *pData=cbBuffer;
	cbNameLen=(BYTE)strnlen(g_ConfigParam.szRealName,NAME_LEN);
	pData=WriteDword(pData,g_ConfigParam.dwUserID,wSendSize);
	pData=WriteByte(pData,cbNameLen,wSendSize);
	pData=WriteFromBuffer(pData,g_ConfigParam.szRealName,cbNameLen,wSendSize);

	#ifdef ANDROID_OS
	LOGE(TEXT("<<<<<<<<<<<<<------send data----SendUpHandData"));
	#endif
	return SendOperateEventData(OEID_USER_UPHAND,cbBuffer,wSendSize,g_ConfigParam.dwTeacherID);
	//g_pILogMsgOutput->OutSysLogTxt(TEXT("���;�������"));
}

#ifdef ANDROID_OS
//��ʼ¼��
bool CSocketSinkManage::StartRecord()
{
	bool bSuccess=true;
	m_AudioManager.SetProcess(true);
	bSuccess=m_AudioManager.StartRecording();
	if(!bSuccess)
	{
		LOGE(TEXT("<<<<<<<<<<<<<----------StartRecord,Result:%d"),bSuccess);
	}
	else
	{
		LOGI(TEXT("<<<<<<<<<<<<<----------StartRecord success"));
	}
	
	return bSuccess;
}

//ֹͣ¼��
bool CSocketSinkManage::StopRecord()
{
	bool bSuccess=true;
 
	m_AudioManager.SetProcess(false);
	m_AudioManager.StopRecording();

	return bSuccess;
}

//������Ƶ�����С(nType=0 ΪRGB����,nType=1ΪYUV����)
bool CSocketSinkManage::SetVideoEncodeSize(int cx,int cy,int nType)
{
	return m_MediaDataManage.SetVideoEncodeSize(cx,cy,nType);
}

//��ʼ��Ƶ����
bool CSocketSinkManage::StartVideoEncode()
{
	return m_MediaDataManage.StartVideoEncode();
}

//ֹͣ��Ƶ����
bool CSocketSinkManage::StopVideoEncode()
{
	return m_MediaDataManage.StopVideoEncode();
}

//�����Ӳ���Ƶ����
int CSocketSinkManage::AddVideoData(const void *pVideoData,int nDataSize)
{
	return m_MediaDataManage.AddVideoData(pVideoData,nDataSize);
}

#endif

#ifdef ANDROID_OS
//¼�����ݻص�
bool __cdecl CSocketSinkManage::OnAacAudioData(const void * pAacData,DWORD dwDataSize)
{
	LOGI(TEXT("<<<<<<<<<<<<<--------------OnAacAudioData,DataSize:%d"),dwDataSize);
	if(g_ConfigParam.dwDstRoomID==0) return false;

	return m_pTeachSockClient->SendData(TEACH_MAIN_CMD,TSVR_STUDENT_AUDIO,g_ConfigParam.dwDstRoomID,(void *)pAacData,(WORD)dwDataSize);
}

#endif

//���ͺڰ��¼�
bool CSocketSinkManage::SendBoardEventData(BOARDEVENTID beid,void *pEventData,DWORD dwDataSize)
{
	const int MAXSIZE=2048;
	BYTE cbBuffer[MAXSIZE];
	BYTE *pData=cbBuffer;
	WORD wSendSize=0;
	pData=WriteWord(pData,(WORD)beid,wSendSize);
	if(beid>=BEID_ADD_IMAGE && beid<=BEID_DEL_IMAGE)
	{
		pData=WriteFromBuffer(pData,pEventData,(WORD)dwDataSize,wSendSize);
	}
	else if(beid==BEID_ELEMENT)
	{
		DRAWELEMENT *pde=(DRAWELEMENT*)pEventData;

		pData=WriteByte(pData,(BYTE)pde->et,wSendSize);	//д��Ԫ������
		if(pde->et==ET_LINE)
		{
			/*CPointVector *pNewLine=(CPointVector*)pde->pObjData;			
			CPointVector &lData=*pNewLine;

			DWORD dwCount=lData.size();						
			dwDataSize=dwCount*sizeof(POINT);
			POINT *pPoint=&lData[0];
			pData=WriteFromBuffer(pData,pPoint,(WORD)dwDataSize,wSendSize);*/
		}
		else if(pde->et==ET_TEXT)
		{
			TXTELEMENT *pTxtElement=(TXTELEMENT*)pde->pObjData;
			RECT &rc=pTxtElement->rc;
			if(pTxtElement->nTxtSize<=0) return true;
			if(pTxtElement->nTxtSize>=TXT_MAX_LEN)
			{
				pTxtElement->pTxtData[TXT_MAX_LEN-1]=0;
				pTxtElement->nTxtSize=TXT_MAX_LEN-1;
			}

			int nTxtLength=lstrlen(pTxtElement->pTxtData);
			pData=WriteInt32(pData,rc.left,wSendSize);
			pData=WriteInt32(pData,rc.top,wSendSize);
			pData=WriteInt32(pData,RECTWIDTH(rc),wSendSize);
			pData=WriteInt32(pData,RECTHEIGHT(rc),wSendSize);
			pData=WriteWord(pData,pTxtElement->wFontSize,wSendSize);		
			pData=WriteWord(pData,(WORD)nTxtLength,wSendSize);
			if(nTxtLength>0)
			{		
				pData=WriteFromBuffer(pData,pTxtElement->pTxtData,nTxtLength,wSendSize);
			}
		}
		else if(pde->et==ET_SHAPE)
		{
			SHAPEELEMENT *pShape=(SHAPEELEMENT *)pde->pObjData;
			RECT &rc=pShape->rc;
			pData=WriteWord(pData,(WORD)pShape->st,wSendSize);	
			pData=WriteInt32(pData,rc.left,wSendSize);
			pData=WriteInt32(pData,rc.top,wSendSize);
			pData=WriteInt32(pData,RECTWIDTH(rc),wSendSize);
			pData=WriteInt32(pData,RECTHEIGHT(rc),wSendSize);
		}
	}	
	else
	{
		if(dwDataSize>0)
		{
			pData=WriteFromBuffer(pData,pEventData,(WORD)dwDataSize,wSendSize);
		}
	}
	
	bool bSuccess=SendDataToAVServer(TSVR_T_BB_EVENT,cbBuffer,wSendSize);
	
	return bSuccess;
}

//���Ͳ����¼�
bool CSocketSinkManage::SendOperateEventData(OPEREVENTID oeid,void *pEventData,DWORD dwDataSize,DWORD dwSequenceID)
{
	const int MAXSIZE=2048;
	BYTE cbBuffer[MAXSIZE];
	BYTE *pData=cbBuffer;
	WORD wSendSize=0;
	pData=WriteWord(pData,(WORD)oeid,wSendSize);
	 
	if(dwDataSize>0)
	{
		pData=WriteFromBuffer(pData,pEventData,(WORD)dwDataSize,wSendSize);
	}
	
	return SendDataToAVServer(TSVR_T_OPT_EVENT,cbBuffer,wSendSize,dwSequenceID);
}

//���ſμ�
bool CSocketSinkManage::PlayCwdFile(LPCTSTR lpszFileName)
{
	return m_PlayCWThread.StartPlayThread(lpszFileName);
}

//��ͣ���ſμ�
bool CSocketSinkManage::PausePlayCwd(bool bPuased)
{
	return m_PlayCWThread.PausePlayThead(bPuased);
}

//ֹͣ���ſμ�
bool CSocketSinkManage::StopPlayCwd()
{
	return m_PlayCWThread.StopPlayThread();
}

//�����������
void CSocketSinkManage::CheckNetConnect()
{
	IClientSocket *pIClientSocket=m_MsgSocketSink.GetSocketInterface();
	m_MsgSocketSink.OnProcActiveTest(pIClientSocket);
}

//�˳���¼
void CSocketSinkManage::LoginOut()
{
	m_TeachSocketSink.CloseTeachConnect();
	m_MsgSocketSink.CloseMsgConnect();
}

////////////////////////////////////////////////////////////////
#ifdef __IPHONE_4_0
ISockSinkManage *AfxCreateSinkManage()
{
    CSocketSinkManage *pSockManage=CSocketSinkManage::GetInstance();
    return (ISockSinkManage *)pSockManage;
}
#endif	//__IPHONE_4_0

/////////////////////////////////////////////////////////////////////////////