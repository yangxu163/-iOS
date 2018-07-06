///////////////////////////////////////////////////////////////////////////////////////
//			ȫ�ֶ���ͷ�ļ� localdef.h
///////////////////////////////////////////////////////////////////////////////////////
#ifndef __BOWEN_HU_JNI_LOCALDEF_HEAD_FILE__
#define __BOWEN_HU_JNI_LOCALDEF_HEAD_FILE__
#define MACIOS	1

#include <iostream>
#if MACIOS
#include "../globaldef/globaldef.h"
#include "../globaldef/MacThread.h"
#include "../globaldef/ProtocolFunction.h"
#include "custom_log.h"
#else
#include "../../../../globaldef/globaldef.h"
#include "../../../../globaldef/LinuxThread.h"
#include "../../../../globaldef/ProtocolFunction.h"
#include "../../../../globaldef/MemManage.h"
#endif

#if !MACIOS
#define ANDROID_OS
#endif

using namespace std;
    
#define IS_ANDROID			1				//Andoirdϵͳ
///////////////////////////////////////////////////////////////////////////////////////////////

//������Ϣ�ṹ
typedef struct tagConfigData
{
	MYBOOL		bUpHanding;					//��ʦҪ����ֱ�־
	DWORD		dwUserID;					//�û�ID
	DWORD		dwDstRoomID;				//Ŀ�����ID
	DWORD		dwRoomIndex;				//����SOCK����
	DWORD		dwTeacherID;				//��ʦID
	MYBOOL		bIsSavePwd;					//�Ƿ񱣴�����
	MYBOOL		bOpenGlEs20;				//�Ƿ�ʹ��GLES
	WORD		wUserType;					//�û�����(0=ѧ��,1=��ʦ,2=�ҳ�)
	WORD		wClientType;				//�ͻ�������
	WORD		wServerPort;				//��¼�������˿ں�
	WORD		wCtrlSvrPort;				//���Ʒ������˿�
	WORD		wTeachSvrPort;				//��ѧ�������˿�
	WORD		wLBSvrPort;					//���ؾ���������˿�
	DWORD		dwCtrlSvrIP;				//���Ʒ�����IP��ַ
	CHAR		szCtrlSvrUrl[BUFF_SIZE];	//���Ʒ�����Url
	CHAR		szTeachSvrUrl[BUFF_SIZE];	//��ѧ������Url
	CHAR		szLBSvrUrl[BUFF_SIZE];		//���ؾ��������URL
	CHAR		szCheckCode[CHECKCODE_MAXLEN];	//CheckCode
	CHAR		szServerUrl[BUFF_SIZE];		//��¼������URL		
	CHAR		szUserName[BUFF_SIZE];		//�û���
	CHAR		szPassword[BUFF_SIZE];		//��¼����
	CHAR		szRealName[NAME_LEN];		//�û���ʵ����
	TCHAR		szwRealName[NAME_LEN];		//�û���ʵ����

}CONFIGDATA,*PCONFIGDATA;

///////////////////////////////////////////////////////////////////////
#if !MACIOS
//����Java ����Ϣ��������
bool OnJavaMsgSink(const char *pMsgJson,int nMsgType);
//����Java ��ڰ��¼���������
bool OnJavaBoardSink(const char *pMsgJson,int nMsgType);
//����Java ������¼���������
bool OnJavaOperateSink(const char *pMsgJson,int nMsgType);
//�ϴ�������(nSrcType=0��ʦ,nSrcType=1ѧ��)
bool OnJavaLineData(const void * pLineData,int nDataSize,int nSrcType);
//������Ƶ���ݵ�Java��
bool OnJavaVideoData(const void * pVideoData,int nDataSize,int nWidth,int nHeight);
//������ʾ֪ͨJava��
bool OnJavaErrMsg(const char *pMsgJson);
#endif

//��־��ӡ
void NetMsgNotify(LPCTSTR lpszMsg,ITEOPTIONTYPE nOptType=OT_NOOPTION);
//��Ϣ֪ͨ
void NetMsgAutoParam(ITEOPTIONTYPE nOptType,LPCTSTR lpszFormate,...);
/////////////////////////////////////////////////////////////////////////
//�ڰ�����ӿ�
struct IBlackboardManage
{
	//�ڰ��¼�����
	virtual bool __cdecl OnBlackboardEventData(WORD beid,void *pEventData,DWORD dwDataSize,DWORD dwTimeStamp)=0;
	//�����¼�����
	virtual bool __cdecl OnRevOperateEventData(OPEREVENTID beid,void *pEventData,DWORD dwDataSize,DWORD dwTimeStamp)=0;
};

//����Ƶ���ݽӿ�
struct IMediaDataManage
{
	//��Ƶ����
	virtual bool __cdecl OnAudioRTData(const void *pAudioData,DWORD dwDataSize,DWORD dwTimeStamp)=0;
	//��ƵH264SpsPps����
	virtual bool __cdecl OnH264SpsPpsData(DWORD dwChannelID,const void * pSpsPps,DWORD dwDataSize)=0;
	//H264��Ƶ��������
	virtual bool __cdecl OnVideoH264CodeData(DWORD dwChannelID,const void *pH264Data,DWORD dwDataSize)=0;
	//����H264��������
	virtual bool __cdecl SendH264CodeData(const void *pH264Data,DWORD dwDataSize)=0;
	//������ʾ��Ϣ
	virtual bool __cdecl OnErrMsgNotify(int nErrCode,LPCTSTR lpszMsgTxt)=0;
};

//////////////////////////////////////////////////////////////////////////////////
extern CONFIGDATA	g_ConfigParam;			//���ò���

#endif //__BOWEN_HU_JNI_LOCALDEF_HEAD_FILE__