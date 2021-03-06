#ifndef __RCODE_F_H__
#define __RCODE_F_H__

#include	"QRCode.h"
#include	"Define.h"
#include	"Timer.h"
#include	"Motor_Fun.h"
#include	"DateTime.h"

#define	MAX_QR_CODE_LENGHT	150					//二维码最大长度
#define	MAX_SCAN_QR_TIME	10					//扫描二维码时间

typedef struct ReadQRCodeBuffer_tag {
	char originalcode[MAX_QR_CODE_LENGHT];					//原始的二维码
	unsigned short originalCodeLen;								//原始数据长度
	char decryptcode[MAX_QR_CODE_LENGHT];						//解密后的二维码
	ScanCodeResult scanResult;									//读取结果
	char tempbuf[64];											//临时缓存
	char * pbuf1;												//临时指针1
	char * pbuf2;												//临时指针2
	QRCode * cardQR;											//保存二维码
	Timer timer;												//计时器
	unsigned char motorDir;										//电机方向
	unsigned short tempV1;
	MotorAction motorAction;
	DateTime dateTime;
}ReadQRCodeBuffer;

#define	ReadQRCodeBufferStructSize	sizeof(ReadQRCodeBuffer)

ScanCodeResult ScanCodeFun(QRCode * cardQR);

#endif

