////////////////////////////////////////////////////////////////////////////////////
//			�ڰ��¼�������ʵ���ļ� BoardEventData.cpp
//			2016-08-13 Bowen.hu
/////////////////////////////////////////////////////////////////////////////////////
#include "BoardEventData.h"

CBoardEventData::CBoardEventData(void)
{
	m_pIBoardEventSink=NULL;
}

CBoardEventData::~CBoardEventData(void)
{
}

//���úڰ����ݻص��ӿ�
bool CBoardEventData::SetBoardDataSink(IBoardEventSink *pIBoardEventSink)
{
	m_pIBoardEventSink=pIBoardEventSink;
	return true;
}

//�ڰ��¼�����
bool __cdecl CBoardEventData::OnBlackboardEventData(WORD beid,void *pEventData,DWORD dwDataSize,DWORD dwTimeStamp)
{
	switch(beid)
	{
	case BEID_ELEMENT:
		NetAddBoardElement(pEventData,dwDataSize);	break;
	case BEID_LINE_STYLE:
		NetChangelLineStyle(pEventData,dwDataSize);	break;
	case BEID_ADD_IMAGE:	//����ͼƬ
	case BEID_ZOOM_IMAGE:	//����ͼƬ
	case BEID_DEL_IMAGE:	//ɾ��ͼƬ
		NetProcImageData((BOARDEVENTID)beid,pEventData,dwDataSize); break;
	case BEID_CLS_BOARD:
	case BEID_UNDO_ELEMENT:
	case BEID_REDO_ELEMENT:
	case BEID_END_CWD:
		NetProcNoDataEvent(beid); break;
	case BEID_SWITCH_PAGE:
	case BEID_PLAY_CWD:
		NetProcIntEvent(beid,pEventData,dwDataSize); break;
	case BEID_BOARD_SIZE:
		NetProcBoardSize((BOARDEVENTID)beid,pEventData,dwDataSize); break;
	case OEID_CLASS_BEGIN:			//��ʼ�Ͽ�
		OnPlaycwdStartClass(pEventData,dwDataSize);
	}
	if(dwTimeStamp>0)
	{
		dwTimeStamp=htonl(dwTimeStamp);
		NetProcDwordEvent(BEID_PLAY_TIME,&dwTimeStamp,sizeof(DWORD));
	}

	return true;
}

//�����¼�����
bool __cdecl CBoardEventData::OnRevOperateEventData(OPEREVENTID oeid,void *pEventData,DWORD dwDataSize,DWORD dwTimeStamp)
{
	switch(oeid)
	{
	case OEID_QUESTION:
		NetProcDwordEvent(oeid,pEventData,dwDataSize);
		break;
	case OEID_SELECT_USER:
		NetProcSelectedUser(oeid,pEventData,dwDataSize);
		break;
	case OEID_AUDIO_CTRL:
	case OEID_WRITE_CTRL:
		NetProcIntEvent(oeid,pEventData,dwDataSize);
		break;
	case OEID_CLIENT_PLAY:
		break;
	case OEID_CLASS_BEGIN:
		OnProcStartClass(oeid,pEventData,dwDataSize);
		break;
	case OEID_CLASS_END:
	case OEID_CLASS_PAUSE:
	case OEID_CLASS_RESUME:
		NetProcNoDataEvent(oeid);
		break;
	}
	if(dwTimeStamp>0)
	{
		dwTimeStamp=htonl(dwTimeStamp);
		NetProcDwordEvent(BEID_PLAY_TIME,&dwTimeStamp,sizeof(DWORD));
	}

	return true;
}

////////////////////////////////////////
//������ʼ�Ͽβ���
void CBoardEventData::OnProcStartClass(int eid,const void *pEventData,DWORD dwDataSize)
{
	MYBOOL bCanSpeak=FALSE;
	MYBOOL bCanWrite=FALSE;
	MYBOOL bUpHanding=FALSE;
	MYBOOL bClassPause=FALSE;
	int nBBPageCount=1;
	int nCurrentPage=1;
	int nCurrPeriod=0;
	DWORD dwDuration=0;
	WORD wSCType=SCT_NORMAL;

	BYTE *pData=(BYTE *)pEventData;
	pData=ReadDword(pData,dwDuration);
	pData=ReadInt32(pData,bCanSpeak);
	pData=ReadInt32(pData,bCanWrite);
	pData=ReadInt32(pData,bUpHanding);
	pData=ReadInt32(pData,bClassPause);
	pData=ReadInt32(pData,nBBPageCount);
	pData=ReadInt32(pData,nCurrPeriod);
	pData=ReadInt32(pData,nCurrentPage);
	pData=ReadWord(pData,wSCType);

	#ifndef __IPHONE_4_0
	const int MAXMSG=1024;
	TCHAR szMsgTxt[MAXMSG]="";
	sprintf(szMsgTxt,"{\"duration\":%u,\"speek\":%d,\"write\":%d,\"uphand\":%d,\"pause\":%d,\"pagecount\":%d,\"currperiod\":%d,\"currpage\":%d,\"nscType\":%d}",
		dwDuration,bCanSpeak,bCanWrite,bUpHanding,bClassPause,nBBPageCount,nCurrPeriod,nCurrentPage,wSCType);
	OnJavaOperateSink(szMsgTxt,eid);
	#else
	if(m_pIBoardEventSink!=NULL)
		m_pIBoardEventSink->OnBeginClassEvent(dwDuration,bCanSpeak,bCanWrite,bUpHanding,bClassPause,nBBPageCount,nCurrentPage,nCurrPeriod,wSCType);
	#endif

}

//�μ����ſ�ʼ�Ͽ�
bool CBoardEventData::OnPlaycwdStartClass(const void *pEventData,DWORD dwDataSize)
{
	MYBOOL bCanSpeak=FALSE;
	MYBOOL bCanWrite=FALSE;
	MYBOOL bUpHanding=FALSE;
	MYBOOL bClassPause=FALSE;
	int nBBPageCount=1;
	int nCurrentPage=1;
	int nCurrPeriod=0;
	DWORD dwDuration=0;

	const BYTE *pData=(const BYTE *)pEventData;
	pData=ReadInt32(pData,nBBPageCount);
	pData=ReadInt32(pData,nCurrPeriod);
	pData=ReadInt32(pData,nCurrentPage);
	 
	#ifndef __IPHONE_4_0
	const int MAXMSG=1024;
	TCHAR szMsgTxt[MAXMSG]="";
	sprintf(szMsgTxt,"{\"duration\":%u,\"speek\":%d,\"write\":%d,\"uphand\":%d,\"pause\":%d,\"pagecount\":%d,\"currperiod\":%d,\"currpage\":%d,\"nscType\":%d}",
		dwDuration,bCanSpeak,bCanWrite,bUpHanding,bClassPause,nBBPageCount,nCurrPeriod,nCurrentPage,STC_LOCALCWD);
	OnJavaOperateSink(szMsgTxt,OEID_CLASS_BEGIN);
	#else
	if(m_pIBoardEventSink!=NULL)
		m_pIBoardEventSink->OnBeginClassEvent(dwDuration,bCanSpeak,bCanWrite,bUpHanding,bClassPause,nBBPageCount,nCurrentPage,nCurrPeriod,STC_LOCALCWD);
	#endif

	return true;
}

//�������Ӻڰ�Ԫ��
void CBoardEventData::NetAddBoardElement(const void *pEventData,DWORD dwDataSize)
{
	if(pEventData==NULL) return;
	if(dwDataSize==0) return;
	#ifndef __IPHONE_4_0
	const int MAXMSG=1024;
	TCHAR szMsgTxt[MAXMSG];	
	#endif
	BYTE *pData=(BYTE *)pEventData;
	ELEMENTTYPE et=(ELEMENTTYPE)pData[0];
	pData++;
	dwDataSize--;
	
	if(et==ET_LINE)
	{ 
		#ifndef __IPHONE_4_0
		OnJavaLineData(pData,dwDataSize,0);
		#else
		if(m_pIBoardEventSink!=NULL)
			m_pIBoardEventSink->OnRevBoardLineData(pData,dwDataSize);
		#endif
	}
	else if(et==ET_TEXT)
	{
		const int MAXTXT=512;
		CHAR szInText[MAXTXT]="";
		int left,top,cx,cy;
		WORD wFontSize=12;
		WORD wTxtLen=0;
		pData=ReadInt32(pData,left);
		pData=ReadInt32(pData,top);
		pData=ReadInt32(pData,cx);
		pData=ReadInt32(pData,cy);
		pData=ReadWord(pData,wFontSize);		
		pData=ReadWord(pData,wTxtLen);
		if(wTxtLen>=MAXTXT) return;
		pData=ReadToBuffer(pData,szInText,wTxtLen);
		szInText[wTxtLen]=0;
		#ifndef __IPHONE_4_0
		sprintf(szMsgTxt,"{\"type\":%d,\"x\":%d,\"y\":%d,\"w\":%d,\"h\":%d,\"size\":%d,\"text\":\"%s\",\"fontSize\":%d}",
			et,left,top,cx,cy,wTxtLen,szInText,wFontSize);
		OnJavaBoardSink(szMsgTxt,BEID_ELEMENT);
		#else
		if(m_pIBoardEventSink!=NULL)
			m_pIBoardEventSink->OnRevBoardTextData(left,top,cx,cy,szInText,wTxtLen,wFontSize);
		#endif

	}
	else if(et==ET_SHAPE)
	{
		int left,top,cx,cy;
		WORD wShapeType=0;
		pData=ReadWord(pData,wShapeType);	
		pData=ReadInt32(pData,left);
		pData=ReadInt32(pData,top);
		pData=ReadInt32(pData,cx);
		pData=ReadInt32(pData,cy);
		#ifndef __IPHONE_4_0
		sprintf(szMsgTxt,"{\"type\":%d,\"shapeType\":%d,\"left\":%d,\"top\":%d,\"right\":%d,\"bottom\":%d}",
			et,wShapeType,left,top,left+cx,top+cy);
		OnJavaBoardSink(szMsgTxt,BEID_ELEMENT);
		#else
		if(m_pIBoardEventSink!=NULL)
			m_pIBoardEventSink->OnRevBoardShapeData(wShapeType,left,top,cx,cy);
		#endif
	}
}

//������������Ԫ��
void CBoardEventData::NetChangelLineStyle(const void *pEventData,DWORD dwDataSize)
{
	const int MAXMSG=1024;
	TCHAR szMsgTxt[MAXMSG];

	DWORD dwTeacherID=0;
	int nPageCount=0;
	int nLineWidth=0;
	DWORD dwLineColor=0;
	int nLineStyle=0;
	BYTE *pData=(BYTE *)pEventData;
	pData=ReadDword(pData,dwTeacherID);
	pData=ReadInt32(pData,nPageCount);
	pData=ReadInt32(pData,nLineWidth);
	pData=ReadDword(pData,dwLineColor);
	pData=ReadInt32(pData,nLineStyle);
	
	#ifndef __IPHONE_4_0
	BYTE r=GetRValue(dwLineColor);
	BYTE g=GetGValue(dwLineColor);
	BYTE b=GetBValue(dwLineColor);
	sprintf(szMsgTxt,"{\"cr\":%d,\"cg\":%d,\"cb\":%d,\"width\":%d,\"style\":%d}",r,g,b,nLineWidth,nLineStyle);	
	OnJavaBoardSink(szMsgTxt,BEID_LINE_STYLE);
	#else
	if(m_pIBoardEventSink!=NULL)
		m_pIBoardEventSink->OnRevBoardLineStyle(nPageCount,nLineWidth,dwLineColor,nLineStyle);
	#endif
}

//ͼƬ����
void CBoardEventData::NetProcImageData(BOARDEVENTID beid,const void *pEventData,DWORD dwDataSize)
{
	DWORD dwImgID=0;
	short x,y,cx,cy;
	short nPageIndex=0;
	BYTE cbLength=0;
	BYTE *pData=(BYTE *)pEventData;
	TCHAR szUrl[MAX_PATH]="";
	const int MAXMSG=1024;
	TCHAR szMsgTxt[MAXMSG];

	pData=ReadDword(pData,dwImgID);
	if(beid==BEID_ZOOM_IMAGE || beid==BEID_ADD_IMAGE)
	{
		pData=ReadInt16(pData,nPageIndex);
		pData=ReadInt16(pData,x);
		pData=ReadInt16(pData,y);
		pData=ReadInt16(pData,cx);
		pData=ReadInt16(pData,cy);
		if(beid==BEID_ZOOM_IMAGE)
		{
			#ifndef __IPHONE_4_0
			sprintf(szMsgTxt,"{\"imageId\":%u,\"x\":%d,\"y\":%d,\"w\":%d,\"h\":%d,\"pi\":%d}",
				dwImgID,x,y,cx,cy,nPageIndex);	
			#else
			if(m_pIBoardEventSink!=NULL)
				m_pIBoardEventSink->OnRevBoardZoomImage(dwImgID,x,y,cx,cy,nPageIndex);
			#endif
		}
		if(beid==BEID_ADD_IMAGE)
		{
			pData=ReadByte(pData,cbLength);
			pData=ReadToBuffer(pData,szUrl,cbLength);
			#ifndef __IPHONE_4_0
			sprintf(szMsgTxt,"{\"imageId\":%u,\"x\":%d,\"y\":%d,\"w\":%d,\"h\":%d,\"url\":\"%s\",\"pi\":%d}",
				dwImgID,x,y,cx,cy,szUrl,nPageIndex);
			#else
			if(m_pIBoardEventSink!=NULL)
				m_pIBoardEventSink->OnRevBoardAddImage(dwImgID,x,y,cx,cy,szUrl,nPageIndex);
			#endif
		}
	}
	if(beid==BEID_DEL_IMAGE)
	{
		#ifndef __IPHONE_4_0
		sprintf(szMsgTxt,"{\"imageId\":%u}",dwImgID);	
		#else
		if(m_pIBoardEventSink!=NULL)
			m_pIBoardEventSink->OnRevBoardDelImage(dwImgID);
		#endif
	}

	#ifndef __IPHONE_4_0
	OnJavaBoardSink(szMsgTxt,beid);
	#endif
}

//�����ڰ��¼�
void CBoardEventData::NetProcNoDataEvent(int beid)
{
	#ifndef __IPHONE_4_0
	const int MAXMSG=64;
	TCHAR szMsgTxt[MAXMSG]="{\"desc\":\"no data event\"}";
	if(beid>=OEID_QUESTION)
	{
		OnJavaOperateSink(szMsgTxt,beid);
	}
	else
	{
		OnJavaBoardSink(szMsgTxt,beid);
	}
	#else
	if(m_pIBoardEventSink!=NULL)
	{
		m_pIBoardEventSink->OnProcNoDataEvent(beid);
	}
	#endif
}

//�������Ͳ����¼�
void CBoardEventData::NetProcIntEvent(int beid,const void *pEventData,DWORD dwDataSize)
{
	if(dwDataSize!=sizeof(int))
		return;

	int nValue=0;
	BYTE *pData=(BYTE *)pEventData;
	pData=ReadInt32(pData,nValue);

	#ifndef __IPHONE_4_0
	const int MAXMSG=64;
	TCHAR szMsgTxt[MAXMSG]="";
	sprintf(szMsgTxt,"{\"value\":%d}",nValue);
	if(beid>=OEID_QUESTION)
	{
		OnJavaOperateSink(szMsgTxt,beid);
	}
	else
	{
		OnJavaBoardSink(szMsgTxt,beid);
	}
	#else
	if(m_pIBoardEventSink!=NULL)
		m_pIBoardEventSink->OnProcIntDataEvent(beid,nValue);
	#endif
}

//�����ڰ��С
void CBoardEventData::NetProcBoardSize(BOARDEVENTID beid,const void *pEventData,DWORD dwDataSize)
{
	BYTE *pData=(BYTE*)pEventData;
	int cx=0,cy=0;
	int nPageSize=0;
	pData=ReadInt32(pData,cx);
	pData=ReadInt32(pData,cy);
	pData=ReadInt32(pData,nPageSize);
	#ifndef __IPHONE_4_0
	const int MAXMSG=64;
	TCHAR szMsgTxt[MAXMSG]="";
	sprintf(szMsgTxt,"{\"cx\":%d,\"cy\":%d,\"pagecount\":%d}",cx,cy,nPageSize);
	OnJavaBoardSink(szMsgTxt,beid);
	#else
	if(m_pIBoardEventSink!=NULL)
		m_pIBoardEventSink->OnProcBoardSize(cx,cy,nPageSize);
	#endif
}

//����DWORD�Ͳ����¼�
void CBoardEventData::NetProcDwordEvent(int beid,const void *pEventData,DWORD dwDataSize)
{
	if(dwDataSize!=sizeof(DWORD))
		return;

	DWORD dwValue=0;
	BYTE *pData=(BYTE *)pEventData;
	pData=ReadDword(pData,dwValue);

	if(OEID_QUESTION==beid)
	{
		g_ConfigParam.bUpHanding=(MYBOOL)dwValue;
	}
	#ifndef __IPHONE_4_0
	const int MAXMSG=64;
	TCHAR szMsgTxt[MAXMSG]="";
	sprintf(szMsgTxt,"{\"value\":%u}",dwValue);
	OnJavaBoardSink(szMsgTxt,beid);
	#else
	if(m_pIBoardEventSink!=NULL)
		m_pIBoardEventSink->OnProcDwordDataEvent(beid,dwValue);
	#endif
}

//����ѡ��ѧ�������¼�
void CBoardEventData::NetProcSelectedUser(int beid,const void *pEventData,DWORD dwDataSize)
{
	DWORD dwUserID=0;
	BYTE cbNameLen=0;
	CHAR szUserName[128];
	BYTE *pData=(BYTE *)pEventData;	
	pData=ReadDword(pData,dwUserID);
	pData=ReadByte(pData,cbNameLen);
	pData=ReadToBuffer(pData,szUserName,cbNameLen);
	#ifndef __IPHONE_4_0
	const int MAXMSG=256;
	TCHAR szMsgTxt[MAXMSG]="";
	sprintf(szMsgTxt,"{\"uid\":%u,\"uname\":\"%s\"}",dwUserID,szUserName);
	OnJavaBoardSink(szMsgTxt,beid);
	#else
	if(m_pIBoardEventSink!=NULL)
		m_pIBoardEventSink->OnTeacherSelecStudent(dwUserID,szUserName);
	#endif
}

//�����ͻ��˲����¼�
void CBoardEventData::NetProcClientPlay(int eid,const void *pEventData,DWORD dwDataSize)
{
	WORD wResType=0;
	WORD wLength=0;
	CHAR szResName[WNAME_LEN]="";
	CHAR szResUrl[MAX_PATH]="";
	BYTE *pData=(BYTE *)pEventData;
	pData=ReadWord(pData,wResType);
	pData=ReadWord(pData,wLength);
	pData=ReadToBuffer(pData,szResName,wLength);
	pData=ReadWord(pData,wLength);
	pData=ReadToBuffer(pData,szResUrl,wLength);

	#ifndef __IPHONE_4_0
	const int MAXMSG=512;
	TCHAR szMsgTxt[MAXMSG]="";
	sprintf(szMsgTxt,"{\"resType\":%d,\"resName\":\"%s\",\"url\":\"%s\"}",wResType,szResName,szResUrl);
	OnJavaOperateSink(szMsgTxt,eid);
	#else
	if(m_pIBoardEventSink!=NULL)
		m_pIBoardEventSink->OnRecvClientPlayEvent(wResType,szResName,szResUrl);
	#endif
}
