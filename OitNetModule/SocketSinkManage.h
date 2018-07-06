//////////////////////////////////////////////////////////////////////////////////////
//		����ͻ��˻ص�������ͷ�ļ� SocketSinkManage.h
//		bowen 2014-2-20
//////////////////////////////////////////////////////////////////////////////////////
#ifndef __BOWEN_HU_SOCKET_SINK_MANAGE_HEAD_FILE__
#define __BOWEN_HU_SOCKET_SINK_MANAGE_HEAD_FILE__

#include "MsgSockClientSink.h"
#include "TeachSockClientSink.h"
#include "BoardEventData.h"
#include "MediaDataManage.h"
#include "../PlayCwdFile/PlayCWThread.h"

#define HAVE_BBEVETN		1				//�кڰ��¼���������
//////////////////////////////////////////////////////////////////////////////////////
class CSocketSinkManage: public ISockSinkManage
#ifdef ANDROID_OS
	,public IAudioDataSink
#endif
{
private:
	IClientSocket				*m_pMsgSockClient;				//��Ϣ����������ӿ�
	IClientSocket				*m_pTeachSockClient;			//��ѧ����������ӿ�
	CMsgSockClientSink			m_MsgSocketSink;				//��Ϣ�������ͻ��˻ص�����
	CTeachSockClientSink		m_TeachSocketSink;				//��ѧ�������ͻ��˻ص�����
	IKernelDataSink				*m_pIKernelDataSink;				//��Ϣ�ص��ӿ�
private:
	#if HAVE_BBEVETN
	CBoardEventData				m_BoardEventData;				//�ڰ��¼�����
	#endif
	CMediaDataManage			m_MediaDataManage;				//ý�����ݹ���

private:
	static CSocketSinkManage	*m_pSelfObject;					//�Լ�����

private:
	DWORD						m_dwLastTime;					//���ֲ���ʱ��

#ifdef ANDROID_OS
private:
	CAudioManager				m_AudioManager;					//��Ƶ����
#endif

private:
	CPlayCWThread				m_PlayCWThread;					//���ſμ�����

public:
	CSocketSinkManage(void);
	~CSocketSinkManage(void);

public:
	//����ʵ��
	static CSocketSinkManage *GetInstance();
	//�ͷ�ʵ��
	static void ReleaseInstance();

public:
	//��ʼ�������
	virtual bool StartNetService();
	//ֹͣ�������
	virtual bool StopNetService();
	//��ʼ��SockSink
	virtual bool InitSocketSinkManage(LPCTSTR lpszLBSvrIp,WORD wLBSvrPort);
	//��ʼ��������
	virtual bool ConnectToServer(CONNECTSVRTYPE cst=CST_LOGINSVR);
	//�������ݵ�AVSERVER
	virtual bool SendDataToAVServer(WORD wSubCmd,void * pData,WORD wDataSize,DWORD dwSequenceID=INVALID_DWORD);
	//�������ݵ���Ϣ������
	virtual bool SendDataToMsgServer(WORD wSubCmd,void * pData,WORD wDataSize,DWORD dwSequenceID=INVALID_DWORD);
	//�ر���������
	virtual void CloseSocketConnect();
	//�����ı���Ϣ
	virtual bool SendTextMsgToMsgSvr(DWORD dwDstID,WORD wDstType,WORD wRelationType,WORD wMsgType,
		DWORD dwFileSize,DWORD dwDuration,DWORD dwSequenceID,const void * pMsgData,WORD wDataSize);
	//���ú��Ļص��ӿ�
    virtual bool SetKernelDataSink(void *pIKernelDataSink);
	//���úڰ����ݻص��ӿ�
    virtual bool SetBoardDataSink(void *pIBoardEventSink);
    //�ͷ��������
    virtual void FreeSockSinkManage();
	//����ڰ����
	virtual float ComputeBoardScale(int scx,int scy,int dcx,int dcy,POINT &offset);
	//�����û���Ϣ
	virtual bool SetUserInfoData(DWORD dwUserID,WORD wUserType,LPCTSTR lpszUserName,LPCTSTR lpszRealName,LPCTSTR lpszCheckCode);
	//���ý�����Ϣ
	virtual bool SetDstRoomData(DWORD dwDstRoomID,DWORD dwTeacherID);
	//����ý��ص��ӿ�
	virtual bool SetMediaDataSink(void *pIMediaDataSink);
	#ifdef ANDROID_OS
	//��ʼ¼��
	virtual bool StartRecord();
	//ֹͣ¼��
	virtual bool StopRecord();
	#endif
	//���;���Э��
	virtual bool SendUpHandData();
	//���ſμ�
	virtual bool PlayCwdFile(LPCTSTR lpszFileName);
	//��ͣ���ſμ�
	virtual bool PausePlayCwd(bool bPuased);
	//ֹͣ���ſμ�
	virtual bool StopPlayCwd();
	//�����������
	virtual void CheckNetConnect();
	//�˳���¼
	virtual void LoginOut();

public:
	#ifdef ANDROID_OS
	//������Ƶ�����С(nType=0 ΪRGB����,nType=1ΪYUV����)
	bool SetVideoEncodeSize(int cx,int cy,int nType);
	//��ʼ��Ƶ����
	bool StartVideoEncode();
	//ֹͣ��Ƶ����
	bool StopVideoEncode();
	//�����Ӳ���Ƶ����
	int AddVideoData(const void *pVideoData,int nDataSize);
	#endif

public:
	//H264SpsPps����
	virtual bool OnH264SpsPpsData(const void * pHeadData,DWORD dwDataSize);

#ifdef ANDROID_OS
public:
	//¼�����ݻص�
	virtual bool __cdecl OnAacAudioData(const void * pAacData,DWORD dwDataSize);
#endif

private:
	//���ͺڰ��¼�
	bool SendBoardEventData(BOARDEVENTID beid,void *pEventData,DWORD dwDataSize);
	//���Ͳ����¼�
	bool SendOperateEventData(OPEREVENTID oeid,void *pEventData,DWORD dwDataSize,DWORD dwSequenceID=INVALID_DWORD);
};

#endif //__BOWEN_HU_SOCKET_SINK_MANAGE_HEAD_FILE__