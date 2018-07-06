#ifndef CLIENT_MODULE_H_H_
#define CLIENT_MODULE_H_H_

#include "localdef.h"

/////////////////////////////////////////////////////////////////////
//define thread state
#define THREAD_WAIT                 0
#define THREAD_PREPARE              1
#define THREAD_OK                   2
#define THREAD_UNPREPARE            3

#define NET_TIMEOUT			60000			//������Գ�ʱʱ��

///////////////////////////////////////////////////////////////
//����״̬
enum enSocketState
{
	SocketState_NoConnect,					//û������
	SocketState_Connecting,					//��������
	SocketState_Connected,					//�ɹ�����
};

///////////////////////////////////////////////////////////////////////
struct IClientSocket
{
	//set sink interface
	virtual bool SetSocketSink(void * pISocketSink)=0;
	//��ȡ����״̬
	virtual enSocketState GetConnectState()=0;
	//conect to server
	virtual bool ConnectToServer(DWORD dwServerIP,WORD wPort) = 0;
	virtual bool ConnectToServer(LPCTSTR lpszServerIP,WORD wPort) = 0;
	//Send data function
	virtual bool SendData(WORD mainCmd,WORD subCmd,DWORD dwSequenceID,void * pData,WORD wDataSize) = 0;
	//Close socket
	virtual bool CloseSocket(bool bNotify) = 0;
	//��ʼ�����߳�
	virtual bool StartSocketThread()=0;
	//ֹͣ�����߳�
	virtual bool StopSocketThread()=0;
	
};

struct IClientSocketSink
{
	//����������Ϣ
	virtual bool OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocket)=0;
	//�����ȡ��Ϣ
	virtual bool OnSocketRead(CMD_Command Command, void * pBuffer, WORD wDataSize, IClientSocket * pIClientSocket)=0;
	//����ر���Ϣ
	virtual bool OnSocketClose(IClientSocket * pIClientSocket, bool bCloseByServer)=0;
};

//����ص������ӿ�
struct ISockSinkManage
{
	//��ʼ�������
	virtual bool StartNetService()=0;
	//ֹͣ�������
	virtual bool StopNetService()=0;
	//��ʼ��SockSink
	virtual bool InitSocketSinkManage(LPCTSTR lpszLBSvrIp,WORD wLBSvrPort)=0;
	//��ʼ��������
	virtual bool ConnectToServer(CONNECTSVRTYPE cst=CST_LOGINSVR)=0;
	//�������ݵ�AVSERVER
	virtual bool SendDataToAVServer(WORD wSubCmd,void * pData,WORD wDataSize,DWORD dwSequenceID=INVALID_DWORD)=0;
	//�������ݵ���Ϣ������
	virtual bool SendDataToMsgServer(WORD wSubCmd,void * pData,WORD wDataSize,DWORD dwSequenceID=INVALID_DWORD)=0;
	//�ر���������
	virtual void CloseSocketConnect()=0;
	//�����ı���Ϣ
	virtual bool SendTextMsgToMsgSvr(DWORD dwDstID,WORD wDstType,WORD wRelationType,WORD wMsgType,
		DWORD dwFileSize,DWORD dwDuration,DWORD dwSequenceID,const void * pMsgData,WORD wDataSize)=0;
	//���ú��Ļص��ӿ�
    virtual bool SetKernelDataSink(void *pIKernelDataSink)=0;
	//���úڰ����ݻص��ӿ�
    virtual bool SetBoardDataSink(void *pIBoardEventSink)=0;
    //�ͷ��������
    virtual void FreeSockSinkManage()=0;
	//����ڰ����
	virtual float ComputeBoardScale(int scx,int scy,int dcx,int dcy,POINT &offset)=0;
	//�����û���Ϣ
	virtual bool SetUserInfoData(DWORD dwUserID,WORD wUserType,LPCTSTR lpszUserName,LPCTSTR lpszRealName,LPCTSTR lpszCheckCode)=0;
	//���ý�����Ϣ
	virtual bool SetDstRoomData(DWORD dwDstRoomID,DWORD dwTeacherID)=0;
	//����ý��ص��ӿ�
	virtual bool SetMediaDataSink(void *pIMediaDataSink)=0;
	#if !MACIOS
	//��ʼ¼��
	virtual bool StartRecord()=0;
	//ֹͣ¼��
	virtual bool StopRecord()=0;
	#endif
	//���;���Э��
	virtual bool SendUpHandData()=0;
	//���ſμ�
	virtual bool PlayCwdFile(LPCTSTR lpszFileName)=0;
	//��ͣ���ſμ�
	virtual bool PausePlayCwd(bool bPuased)=0;
	//ֹͣ���ſμ�
	virtual bool StopPlayCwd()=0;
	//�����������
	virtual void CheckNetConnect()=0;
	//�˳���¼
	virtual void LoginOut()=0;
};

//�ڰ����ݻص��ӿ�
struct IBoardEventSink
{
	//���պڰ�������
	virtual bool OnRevBoardLineData(const void *pEventData,DWORD dwDataSize)=0;
	//���պڰ��ı�����
	virtual bool OnRevBoardTextData(int x,int y,int cx,int cy,LPCTSTR lpszMsgTxt,int nTxtLength,int nFontSize)=0;
	//���պڰ�ͼ������
	virtual bool OnRevBoardShapeData(int nShapeType,int x,int y,int cx,int cy)=0;
	//���պڰ���������
	virtual bool OnRevBoardLineStyle(int nPageCount,int nLineWidth,DWORD dwLineColor,int nLineStyle)=0;
	//���պڰ�����ͼƬ
	virtual bool OnRevBoardZoomImage(DWORD dwImgID,int x,int y,int cx,int cy,int nPageIndex)=0;
	//���Ӻڰ�ͼƬ
	virtual bool OnRevBoardAddImage(DWORD dwImgID,int x,int y,int cx,int cy,LPCTSTR lpszUrl,int nPageIndex)=0;
	//ɾ���ڰ�ͼƬ
	virtual bool OnRevBoardDelImage(DWORD dwImgID)=0;
	//���պڰ��޲����¼�
	virtual bool OnProcNoDataEvent(int nEventID)=0;
	//���պڰ����Ͳ����¼�
	virtual bool OnProcIntDataEvent(int nEventID,int nValue)=0;
	//���պڰ�DWORD�Ͳ����¼�
	virtual bool OnProcDwordDataEvent(int nEventID,DWORD dwValue)=0;
	//���ɺڰ��С�¼�
	virtual bool OnProcBoardSize(int cx,int cy,int nPageCount)=0;
	//���տͻ��˲����¼�
	virtual bool OnRecvClientPlayEvent(int nResType,LPCTSTR lpszResName,LPCTSTR lpszResUrl)=0;
	//��ʦѡ��ѧ���ش��¼�
	virtual bool OnTeacherSelecStudent(DWORD dwUserID,LPCTSTR lpszRealName)=0;
	//��ʦѡ��ѧ���ش��¼�
	virtual bool OnBeginClassEvent(DWORD dwDuratoin,MYBOOL bCanSpeak,MYBOOL bCanWrite,MYBOOL bUpHanding,MYBOOL bClassPause,int nBBPageCount,int nCurrentPage,int nCurrPeriod,int nscType)=0;
};

//����Ƶ���ݽӿ�
struct IMediaDataSink
{
	//��Ƶ����
	virtual bool OnAudioRTData(const void *pAudioData,DWORD dwDataSize,DWORD dwSequenceID)=0;
	//��ƵH264SpsPps����
	virtual bool OnH264SpsPpsData(WORD wChannelID,const void * pSpsPps,DWORD dwDataSize)=0;
	//H264��Ƶ��������
	virtual bool OnVideoH264CodeData(WORD wChannelID,const void *pH264Data,DWORD dwDataSize)=0;
};

#ifndef __IPHONE_4_0
//�ײ����ݻص��ӿ�
struct IKernelDataSink
{
	//��Ϣ֪ͨ
	virtual bool OnRecvMsgData(LPCTSTR lpszSrcName,LPCTSTR lpszMsg,WORD wMsgType,DWORD dwTime)=0;
};

#else
//�ײ����ݻص��ӿ�
struct IKernelDataSink
{
	//��Ϣ֪ͨ
	virtual bool OnRecvMsgData(DWORD dwSrcID,DWORD dwDstID,WORD wSrcType,WORD wDstType,WORD wRelationType,WORD wMsgType,LPCSTR lpszMsgBody,DWORD dwFileSize,DWORD dwDuration,DWORD dwMsgID,DWORD dwTimpStap)=0;
    //������ϢResponse
    virtual bool OnRecvMsgResp(int nResult,DWORD dwMsgID,DWORD dwSendTime,DWORD dwSequenceID)=0;
	//������ʾ��Ϣ
	virtual bool OnErrMsgNotify(int nErrCode,LPCSTR lpszErrMsg)=0;
};

///////////////////////////////////////////////////////////////////
ISockSinkManage *AfxCreateSinkManage();
#endif

#endif