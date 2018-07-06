///////////////////////////////////////////////////////////////////////////////////
//			����ص�����ͷ�ļ� SockClientSink.h
//			2014-3-12 hubo
///////////////////////////////////////////////////////////////////////////////////
#ifndef __BOWEN_HU_SOCK_CLIENT_SINK_HEAD_FILE__
#define __BOWEN_HU_SOCK_CLIENT_SINK_HEAD_FILE__

#include "ClientSocket.h"

//////////////////////////////////////////////////////////////////////////////////
class CSockClientSink: public IClientSocketSink 
{
protected:
	CClientSocket				m_ClientSocket;					//���Ӷ���
	IClientSocket				*m_pIClientSocket;				//�������ӽӿ�

protected:
	DWORD						m_dwRecvTime;					//���һ�ν���ʱ��

public:
	CSockClientSink(void);
	~CSockClientSink(void);


public:
	//��ʼ���ص���
	bool InitSocketSink();
	//��ȡ����ӿ�
	IClientSocket *GetSocketInterface(){return m_pIClientSocket;}
	//��������
	bool TestNetConnect();
	//��������
	bool OnProcActiveTest(IClientSocket * pIClientSocket);
};

#endif //__BOWEN_HU_SOCK_CLIENT_SINK_HEAD_FILE__