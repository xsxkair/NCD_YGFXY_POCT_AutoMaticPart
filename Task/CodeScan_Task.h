#ifndef __CODE_T_H__
#define __CODE_T_H__

#include	"Define.h"
#include	"QRCode.h"

char StartCodeScanTask(void);
MyRes StartScanQRCode(QRCode * parm);
ScanCodeResult getScanResult(void);

#endif

