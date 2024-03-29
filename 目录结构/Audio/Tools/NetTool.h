//
//  NetTool.h
//  REducation
//
//  Created by changxm on 14-3-22.
//  Copyright (c) 2014年 changxm. All rights reserved.
//

#import "IUnknownEx.h"
#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UIColor.h>

@interface NetTool : NSObject

// 将本地字节序转换为网络字节序
+ (UNDWORD) htonl64:(UNDWORD)lluValue;

// 将网络字节序转换为本地字节序
+ (UNDWORD) ntohl64:(UNDWORD)lluSrc;

// 时间
+ (UNDWORD) getNowTime;

// 将任意时间适配为秒
+ (UNDWORD) adaptToSecondsOfTimeSince1970:(UNDWORD)time;

// 是否同一网段
+ (BOOL) isInSameLAN:(DWORD)dwPrivateIP_A privateIP_B:(DWORD)dwprivateIP_B publicIP_A:(DWORD)dwPublicIP_A publicIP_B:(DWORD)dwPublicIP_B mask_A:(DWORD)dwMask_A mask_B:(DWORD)dwMask_B;

// 获取子网掩码
+ (DWORD) getSubnetMaskFromSocket:(int)sock;

// 将点分十进制ip转换为数值ip
+ (DWORD) convertIPAddress:(NSString*)strIPAddr;

// 装饰数值ip转换为点分十进制
+ (NSString*) convertIPToString:(DWORD)dwIP;

// 将ip，端口号转为socket地址
+ (struct sockaddr_in) sockAddressFromIP:(DWORD)dwIP withPort:(WORD)wPort;

// int 转rgb
struct CColor TORGB(int color);

int TOINT(CGFloat r, CGFloat g, CGFloat b);
// rgb转int
int ParseRGB(UIColor* color);

//读双字
BYTE *ReadDword(BYTE* pData,DWORD &dwValue);

//读int32
BYTE * ReadInt32(BYTE * pData,__int32_t & nValue);

//读字
BYTE * ReadWord(BYTE * pData,WORD & wValue);

//
BYTE * ReadInt16(BYTE * pData,__int16_t & nValue);

//读BYTE
BYTE * ReadByte(BYTE * pData,BYTE & cbValue);

//读取指定长度
BYTE * ReadToBuffer(BYTE * pData,void * pDest,WORD wLength);

//WORD高八位
WORD HIBYTE(WORD num);
//WORD低八位
WORD LOBYTE(WORD num);

@end
