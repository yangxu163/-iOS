//////////////////////////////////////////////////////////////////////////////////////
//		���Ʒ���������ͻ��˻ص���ͷ�ļ� CtrlSockClientSink.h
//		bowen 2014-2-20
///////////////////////////////////////////////////////////////////////////////////////
#ifndef __BOWEN_HU_MSG_SOCKET_CLIENT_SINK_HEAD_FILE__
#define __BOWEN_HU_MSG_SOCKET_CLIENT_SINK_HEAD_FILE__

#include "SockClientSink.h"

//////////////////////////////////////////////////////////////////////////////////////
class CMsgSockClientSink: public CSockClientSink
{
private:
	MYBOOL						m_bLogining;					//��¼��־
	CONNECTSVRTYPE				m_cst;							//���ӷ���������

private:
	IKernelDataSink				*m_pIKernelDataSink;				//��Ϣ�ص��ӿ�
	ISockSinkManage				*m_pISockSinkManage;			//�����ӿ�
	IMediaDataManage			*m_pIMediaDataManage;		//ý�����ݹ����ӿ�

public:
	void						*m_pTimerHandle;				//��Ϣ��������ʱ��
	void						*m_pTeachSvrTime;				//��ѧ��������ʱ��
	int							m_nTimerID;						//��ʱ��ID

public:
	CMsgSockClientSink(void);
	~CMsgSockClientSink(void);

public:
	//��ʼ����������ӿ�
	bool InitMsgSocketClient(ISockSinkManage *pISockSinkManage,IMediaDataManage *pIMediaDataManage);
	//���ӵ�������
	bool ConnectToServer(CONNECTSVRTYPE cst);
	//��������ӿ�
	bool SetKernelDataSink(IKernelDataSink *pIKernelDataSink);
	//�ر�����
	bool CloseMsgConnect();

	//�ص��ӿ�
public:
	//����������Ϣ
	virtual bool OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocket);
	//�����ȡ��Ϣ
	virtual bool OnSocketRead(CMD_Command Command,void * pBuffer,WORD wDataSize,IClientSocket * pIClientSocket);
	//����ر���Ϣ
	virtual bool OnSocketClose(IClientSocket *pIClientSocket, bool bCloseByServer);

private:
	//�����������ַ
	bool RequestSvrIPPort();
	//��¼����¼������
	bool LoginToLoginSvr();
	//��¼����Ϣ������
	bool LoginToMsgSvr();
	//����Json��Ϣ
	bool OnProcJsonMsg(int nMsgType,LPCSTR lpszUtf8Txt,int nMsgSize);
	//��ȡ��ʱ��ID
	int GetTimerID(){return m_nTimerID;}

public:
	static bool OnMyTimer(void *pObject);
	static bool OnTeachSvrTime(void *pObject);
	static bool OnConnectMsgSvr(void *pObject);
	

private:
	//������¼Response
	bool OnProcLoginResponse(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//������¼����Ϣ������Response
	bool OnProcLoginMsgSvrResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//���������������ַResponse
	bool OnProcRequestSvrIPPortResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//����������ת����Ϣ
	bool OnProcServerSwitchMsg(WORD wCmdID,DWORD dwSequenceID,const void *pDataBuffer,WORD wDataSize);
	//����������ϢResponse
	bool OnProcSendMsgResp(DWORD dwSequenceID,const void *pDataBuffer,WORD wDataSize);

private:
	//������ʦ�������
	bool OnProcTeacherInRoom(LPCTSTR lpszJsonTxt);
	//����ѧ���������
	bool OnProcStudentInRoom(LPCTSTR lpszJsonTxt);

};

#endif //__BOWEN_HU_MSG_SOCKET_CLIENT_SINK_HEAD_FILE__