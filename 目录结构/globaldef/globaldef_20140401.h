//////////////////////////////////////////////////////////////////////////////////////////
//		本地数据定义头文件 globaldef.h
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DEIRLYM_VIDEO_CAPTURE_LOCAL_DEFINE_HEAD_FILE__
#define __DEIRLYM_VIDEO_CAPTURE_LOCAL_DEFINE_HEAD_FILE__

#include <list>
#include <map>
#include <vector>
#include "IUnknownEx.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned __int64 UNDWORD;

///////////////////////////////
//版本号
#define MAKEVER(m,s)	((m<<8)|(s))	//生成版本号
//程序版本1.2
#define DVR_CAP_VER		MAKEVER(1,2)	//DVR 版本
#define CURR_NET_VER	MAKEVER(1,4)	//当前网络版本

//计算数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
//计算字符长度
#define CountString(String) ((UINT)((lstrlen(String)+1)*sizeof(TCHAR)))

//协议包头结构
typedef struct CMD_Head
{
	DWORD					dwTotalLength;						//数据包总长
	union
	{
		DWORD					dwCommandID;					//命令字
		struct
		{
			WORD				wMainCmd;						//主命令
			WORD				wSubCmd;						//字命令
		};
	};
	DWORD					dwSequenceID;						//序列号

}CMDHEAD;

//命令结构
typedef struct CMD_Command
{
	union
	{
		DWORD					dwCommandID;					//命令字
		struct
		{
            WORD				wMainCmd;						//主命令
			WORD				wSubCmd;						//字命令
		};
	};
	DWORD					dwSequenceID;						//序列号

}CMDCOMMAND;

//////////////////////////////////////////////////////////////////////////////////////////
//	网络数据定义
#define SOCKET_VER					2									//数据包版本
#define SOCKET_PACKAGE				(1024*10)    						//最大网络数据包
#define SOCKET_BUFFER				(SOCKET_PACKAGE+sizeof(CMD_Head))	//网络缓冲大小

#define SOCKVALUE(v,i,r) {(v)=(i);(v)<<=16;(v)|=(r);}
#define SOCKINDEX(v,i,r) {(i)=(WORD)((v)>>16);(r)=(WORD)(v);}
////////////////////////////////////////////////////////////////////////////////////////
//定义网络命令字
//主命令字
#define ALL_MAIN_CMD				0x5000						//通用主命令
#define LOGIN_MAIN_CMD				0x5001						//登录服务器命令
#define CTRL_MAIN_CMD				0x5002						//控制服务器命令
#define TEACH_MAIN_CMD				0x5003						//教学服务器命令

//////////////////////////////////////////////////////////////////////////////////////////
//子命令定义
//所有心跳测试子命令定义
#define ACITVE_TEST					0x0000						//心跳测试子命令
#define ACITVE_TEST_RESP			0x1000						//心跳测试子命令

///////////////////////////////////////////////////////////////////////////
//登录服务器子命令
#define LSVR_TEACHER_LOGIN			0x0001						//教师登录到登录服务器
#define LSVR_TEACHER_LOGIN_RESP		0x1001						//教师登录到登录服务器Response
#define LSVR_STUDENT_LOGIN			0x0002						//用户登录到服务器
#define LSVR_STUDENT_LOGIN_RESP		0x1002						//用户登录到服务器Response
#define LSVR_ROOMCLASS_DATA			0x0003						//教室数据
#define LSVR_ROOMCLASS_DATA_RESP	0x1003						//教室数据Response

//控制服务器子命令
#define CSVR_TEACHER_LOGIN			0x0101						//教师登录到控制服务器
#define CSVR_TEACHER_LOGIN_RESP		0x1101						//教师登录到控制服务器Response
#define CSVR_STUDENT_LOGIN			0x0102						//学生登录到控制服务器
#define CSVR_STUDENT_LOGIN_RESP		0x1102						//学生登录到控制服务器
#define CSVR_CTRLCMD_TRANS			0x0103						//控制指令传输
#define CSVR_CTRLCMD_TRANS_RESP		0x1103						//控制指令传输Response
#define CSVR_SLOGIN_NOTIFY			0x0104						//学生登录通知
#define CSVR_SLOGIN_NOTIFY_RESP		0x1104						//学生登录通知

//教学服务器子命令
#define TSVR_TEACHER_LOGIN			0x0201						//教师登录到教学服务器
#define TSVR_TEACHER_LOGIN_RESP		0x1201						//教师登录到教学服务器Response
#define TSVR_STUDENT_LOGIN			0x0202						//学生登录到教学服务器
#define TSVR_STUDENT_LOGIN_RESP		0x1202						//学生登录到教学服务器Response
#define TSVR_TEACHER_TRACK			0x0203						//教师书写轨迹传输
#define TSVR_TEACHER_TRACK_RESP		0x1203						//教师书写轨迹传输Response
#define TSVR_STUDEND_TRACK			0x0204						//学生写轨迹
#define TSVR_STUDEND_TRACK_RESP		0x1204						//学生写轨迹Response
#define TSVR_TEACHER_AUDIO			0x0205						//教师音频
#define TSVR_TEACHER_AUDIO_RESP		0x1205						//教师音频Response
#define TSVR_STUDENT_AUDIO			0x0206						//学生音频
#define TSVR_STUDENT_AUDIO_RESP		0x1206						//学生音频Response


//负载均衡服务器子命令
#define LBS_REQUEST_SVRIP			0x0301						//请求IP地址
#define LBS_REQUEST_SVRIP_RESP		0x1301						//请求IP地址Response
#define LBS_SVR_LB_DATA				0x0302						//服务器负载数据
#define LBS_SVR_LB_DATA_RESP		0x1302						//服务器负载数据Response
#define LBS_BSVR_REGISTER			0x0303						//业务服务器注册
#define LBS_BSVR_REGISTER_RESP		0x1303						//业务服务器注册Response

///////////////////////////////////////////////////////////////////////////////////////
//错误码
#define DB_SVR_EXPECT				-1							//服务器异常
#define NAME_PASS_ERR				-2							//用户名或密码错误
#define CTRL_LOGIN_ERR				-3							//控制服务器验证错误
#define ADD_BSVR_FAILD				-4							//添加业务服务器失败
#define CREATE_ROOM_FAILD			-5							//创建教室失败
#define ADD_TO_ROOM_FAILD			-6							//添加到教室失败

/////////////////////////////////////////////////////////////////////////////////////////
#ifndef AfxDebugBreak
#ifndef MYASSERT
#define MYASSERT
#define AfxDebugBreak()    _asm { int 3 }
#define ASSERT(f) \
	if (!(f)) \
		AfxDebugBreak();
#define VERIFY(f)          ASSERT(f)
#endif
#endif

//////////////////////////////////////////////////////////////
#define RECTWIDTH(rc) (rc.right-rc.left)
#define RECTHEIGHT(rc) (rc.bottom-rc.top)
/////////////////////////////////////////////////////////////////////
//字符串结构
struct my_str_t
{
	int      len;
    BYTE     *data;
};

#define CLEAR_MY_STR(str) {(str).data=NULL;(str).len=0;}
#define FREE_MY_STR(str) {if(str.data!=NULL){delete []str.data;str.data=NULL;str.len=0;}}
#define NEW_MY_STR(str,size) {if(str.len<size){if(str.data!=NULL){delete[] str.data;str.data=NULL;	\
	str.len=0;}str.data=new BYTE[size];if(str.data==NULL) return; str.len=size;}}
#define SET_MY_STR(str,dat,size) {if(str.data!=NULL){	delete []str.data;	str.data=NULL;str.len=0;} \
	str.data=new BYTE[size+1];if(str.data!=NULL){str.len=size; memcpy(str.data,dat,size);str.data[size]=0;}}

//////////////////////////////////////////////////////////////////////////////////////////

#ifndef INVALID_DATA_DFINE
#define INVALID_DATA_DFINE
#define INVALID_WORD				((WORD)(-1))						//无效WORD 数据
#define INVALID_DWORD				((DWORD)(-1))						//无效DWORD 数据
#endif

//64bit网络字节序操作函数
UNDWORD htonl64(UNDWORD ulSrc);
UNDWORD ntohl64(UNDWORD ulSrc);
int EncCheckCode(LPCSTR pszUserName,int nLength,CHAR * pCheckCode,int nMonth,int nDay,int nHour,int nMinute,int nSecond);
int EncCheckCode(LPCSTR pszUserName,int nLength,CHAR * pCheckCode);
int EncCheckCode(DWORD dwID,LPCSTR pszUserName,int nLength,CHAR * pCheckCode);
void EncCheckCode(UNDWORD undwMobile,CHAR * pCheckCode);
void DecCheckCode(const CHAR * pCheckCode,UNDWORD &undwMobile);
bool DecCheckCode(const CHAR * pCheckCode,LPSTR pszUserName);
bool DecCheckCode(DWORD dwID,const CHAR * pCheckCode,LPSTR pszUserName);

//UTF8 转为 GBK
int UTF8ToGBK(const char * utf8Str,char * pGBKBuffer,int nGBKBuffSize);
//GBK 转为 UTF8
int GBKToUTF8(const char * gbkStr,char *pUTF8Buffer,int nUTF8Size);

void OutTextByColor(LPCTSTR lpszText,WORD wColor);
////////// 类型定义 /////////////////////////////////////////////////////////////////////////////
typedef map<UNDWORD,DWORD>							CUNDwordMap;					//长整型列表
typedef map<UNDWORD,DWORD>::iterator				CUNDwordMapIt;					//长整型列表

#define NAME_LEN				32
#define CHECKCODE_MAXLEN		128		
#define BUFF_SIZE				64
///////////////////////////////////////////////////////////////////////
#define SHARE_MEM_NAME		TEXT("deir_ite_app_share")
#define MAX_BUFF_SIZE		1356

/////////////////////////////////////////////////////////////////////
//服务器类型
typedef enum enIteServerType
{
	ST_LOGINSVR,				//登录服务器
	ST_CTRLSVR,					//控制服务器
	ST_TEACHSVR,				//教学服务器
	ST_MAX_SVR,					//最大服务器数
}ITESERVERTYPE;

//客户端类型
typedef enum emIteClientType
{
	CT_UNKNOWN = 0,					//未知客户端
	CT_TEACH = 1,					//教学客户端
	CT_STUDENT = 2,					//学生PC客户端
	CT_STUDENT_IOS = 4,				//学生ios客户端
	CT_STUDENT_ANDR = 8,			//学生ANDROID客户端
	CT_STUCENT_ALL = (CT_STUDENT | CT_STUDENT_IOS | CT_STUDENT_ANDR),
	
}ITECLIENTTYPE;

//教室状态
typedef enum emIteRoomStatus
{
	RS_INVALID,				//无效
	RS_VALID,				//有效
}ITEROOMSTATUC;


//课件数据类型
typedef enum emCWDataType
{
	CWDT_LINE=1,			//线段数据
	CWDT_AUDIO,				//音频数据
	CWDT_VIDEO,				//音频数据
	CWDT_LINEWIDTH,			//修改线段宽
	CWDT_LINECOLOR,			//修改线段颜色
	CWDT_UNDO,				//撤销线段
	CWDT_CLEAR,				//擦除黑板内容
	CWDT_NEW_BB,			//新黑板
	CWDT_PREV_BB,			//上一页黑板
	CWDT_NEXT_BB,			//下一页黑板
	CWDT_ASK_BEGIN,			//答题开始
	CWDT_ASK_END,			//答题结束

}CWDATATYPE;

//控制指令枚举
typedef enum emTeachCtrlCmd
{
	TCC_UNDO_LINE = CWDT_UNDO,			//撤销线画线
	TCC_CLS_BLACKBOARD =CWDT_CLEAR,		//擦除黑板
	TCC_LINE_WIDTH =CWDT_LINEWIDTH,		//修改线宽
	TCC_LINE_COLOR =CWDT_LINECOLOR,		//修改线颜色

}TEACHCTRLCMD;


//共享数据结构
typedef struct tagShareMemData
{
	HWND					hMainWnd;			//主进程主窗口句柄
	HWND					hParentWnd;			//ie 父窗口句柄
	HWND					hWebWnd;			//ie进程Web主窗口句柄
	WORD					wDataSize;			//数据大小
	BYTE					cbBuffer[MAX_BUFF_SIZE];//数据缓冲区
}SHAREMEMDATA;

//负荷信息
struct tagBurthenInfo
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwBufferSize;						//缓冲区长度
	DWORD							dwDataPacketCount;					//数据包数目
};

//负载数据
typedef struct tagLoadData
{
	DWORD			dwConnCount;			//服务连接数
	DWORD			dwRecvCount;			//收到记录数
	DWORD			dwTransCount;			//转发记录数
	DWORD			dwFaildCount;			//失败记录数
}LOADDATA;

//教师教室数据
typedef struct tagClassRoomData
{
	DWORD			dwRoomCount;		//教室数(1/3)
	DWORD			dwRoomID;			//教室ID
	DWORD			dwTeacherID;		//教师ID
	DWORD			dwStartTime;		//开始时间
	DWORD			dwEndTime;			//结束时间
	int				nTotalPeriod;		//总课时
	int				nUsedPeriod;		//已用课时
	int				nRoomStatus;		//教室状态
	WORD			wCtrlSvrPort;		//控制服务器端口
	WORD			wChatSvrPort;		//聊天服务器端口
	CHAR			szCtrlSvrUrl[BUFF_SIZE];	//控制服务器Url
	CHAR			szChatSvrUrl[BUFF_SIZE];	//聊天服务器Url
	TCHAR			szRoomName[BUFF_SIZE];		//教室名
	TCHAR			szTeacherName[BUFF_SIZE];	//教师名
	TCHAR			szRoomDiscrible[1024];	//教室描述
}CLASSROOMDATA;

//教室数据
typedef struct tagClassRoomInfo
{
	DWORD			dwRoomID;				//教室ID
	DWORD			dwTeacherID;			//教师ID
	DWORD			wUserCount;				//用户数
	WORD			wIndex;					//教师Socket索引
	WORD			wRoundID;				//教师Socket重用计数
	SIZE			blackboardSize;			//黑板大小
}CLASSROOMINFO;

//////////////////////////////////////////////////
#ifdef _UNICODE
#define mysprintf _swprintf_c
#else
#define mysprintf _snprintf_c
#endif
//////////////////////////////////////////////////////
//获取负载数据接口
struct IClassRoomData
{
	//获取用户总数和教室数
	virtual bool __cdecl GetClassRoomData(DWORD &dwUserCount,DWORD &dwRoomCount)=0;
};

#endif //__DEIRLYM_VIDEO_CAPTURE_LOCAL_DEFINE_HEAD_FILE__
