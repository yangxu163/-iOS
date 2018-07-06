////////////////////////////////////////////////////////////////////////////////////
//			�ڰ��¼�������ͷ�ļ� BoardEventData.h
//			2016-08-13 Bowen.hu
/////////////////////////////////////////////////////////////////////////////////////
#ifndef __BOWEN_HU_BOARD_EVENT_DATA_HEAD_FILE__
#define __BOWEN_HU_BOARD_EVENT_DATA_HEAD_FILE__

#include "ClientModule.h"
#include "localdef.h"

/////////////////////////////////////////////////////////////////////////////////////
class CBoardEventData: public IBlackboardManage
{
private:
	IBoardEventSink				*m_pIBoardEventSink;		//�ڰ��¼��ӿ�

public:
	CBoardEventData(void);
	~CBoardEventData(void);

public:
	//���úڰ����ݻص��ӿ�
    bool SetBoardDataSink(IBoardEventSink *pIBoardEventSink);

public:
	//�ڰ��¼�����
	virtual bool __cdecl OnBlackboardEventData(WORD beid,void *pEventData,DWORD dwDataSize,DWORD dwTimeStamp);
	//�����¼�����
	virtual bool __cdecl OnRevOperateEventData(OPEREVENTID beid,void *pEventData,DWORD dwDataSize,DWORD dwTimeStamp);

private:
	//�������Ӻڰ�Ԫ��
	void NetAddBoardElement(const void *pEventData,DWORD dwDataSize);
	//������������Ԫ��
	void NetChangelLineStyle(const void *pEventData,DWORD dwDataSize);
	//ͼƬ����
	void NetProcImageData(BOARDEVENTID beid,const void *pEventData,DWORD dwDataSize);
	//�����ڰ��¼�
	void NetProcNoDataEvent(int beid);
	//�������Ͳ����¼�
	void NetProcIntEvent(int beid,const void *pEventData,DWORD dwDataSize);
	//�����ڰ��С
	void NetProcBoardSize(BOARDEVENTID beid,const void *pEventData,DWORD dwDataSize);
	//����DWORD�Ͳ����¼�
	void NetProcDwordEvent(int beid,const void *pEventData,DWORD dwDataSize);
	//����ѡ��ѧ�������¼�
	void NetProcSelectedUser(int beid,const void *pEventData,DWORD dwDataSize);
	//�����ͻ��˲����¼�
	void NetProcClientPlay(int eid,const void *pEventData,DWORD dwDataSize);
	//������ʼ�Ͽβ���
	void OnProcStartClass(int eid,const void *pEventData,DWORD dwDataSize);
	//�μ����ſ�ʼ�Ͽ�
	bool OnPlaycwdStartClass(const void *pEventData,DWORD dwDataSize);
};

#endif	//__BOWEN_HU_BOARD_EVENT_DATA_HEAD_FILE__

////////////////////////////////////////////////////////////////////////////////////
