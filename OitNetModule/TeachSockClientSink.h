///////////////////////////////////////////////////////////////////////////////////
//			��ѧ����ص���ͷ�ļ� TeachSockClientSink.h
//			2014-3-12 hubo
///////////////////////////////////////////////////////////////////////////////////
#ifndef __BOWEN_HU_TEACH_SOCK_CLIENT_SINK_HEAD_FILE__
#define __BOWEN_HU_TEACH_SOCK_CLIENT_SINK_HEAD_FILE__

#include "SockClientSink.h"

//////////////////////////////////////////////////////////////////////////////////
class CTeachSockClientSink: public CSockClientSink
{
private:
	MYBOOL						m_bLogined;					//��¼��־
	IBlackboardManage			*m_pINetDataWndSink;		//���ڽӿ����ݻص�
	IMediaDataManage			*m_pIMediaDataManage;		//ý�����ݹ����ӿ�

public:
	CTeachSockClientSink(void);
	~CTeachSockClientSink(void);

public:
	//��ʼ����ѧ����ͻ���
	bool InitTeachSockClient(IBlackboardManage *pINetDataWndSink,IMediaDataManage *pIMediaDataManage);
	//���ӵ���ѧ������
	bool ConnectToTeachServer();
	//�ر�����
	bool CloseTeachConnect();

	//�ص��ӿ�
public:
	//����������Ϣ
	virtual bool OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocket);
	//�����ȡ��Ϣ
	virtual bool OnSocketRead(CMD_Command Command, void * pBuffer,WORD wDataSize, IClientSocket * pIClientSocket);
	//����ر���Ϣ
	virtual bool OnSocketClose(IClientSocket * pIClientSocket, bool bCloseByServer);

private:
	//��¼����ѧ������
	bool LoginToTeachServer();

private:
	//������¼Response
	bool OnProcLoginSvrResp(DWORD dwSequenceID,void *pDataBuffer,WORD wDataSize);
	//����ѧ����Ƶ����
	bool OnProcStudentAudioData(DWORD dwSequenceID,void *pDataBuffer,WORD wDataSize);
	//����ѧ���ڰ�����
	bool OnProcStudentBoardData(WORD wSubCmd,DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//���������¼�����
	bool OnProcOperateEventData(WORD wSubCmd,DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//������ƵH264SpsPps����
	bool OnProcH264SpsPpsData(void *pDataBuffer,DWORD dwDataSize);
	//�����û���Ƶ����
	bool OnProcUserVideoData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//������ʦ�˳�����
	bool OnProcTeacherExitRoom(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	#if TEST_RECV_DAT
	//������ʦ����Ƶ��
	bool OnProcTeacherVideoData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	#endif
};

#endif //__BOWEN_HU_TEACH_SOCK_CLIENT_SINK_HEAD_FILE__