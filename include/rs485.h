#ifndef  __RS485_RESULT_H__
#define  __RS485_RESULT_H__
#include "devinfo.h"
#include "RK_LPRClientSDK.h"

#define  RS485_1_CMD_BASE            1000
#define  RS485_CMD_CONNECT			(RS485_1_CMD_BASE + 1)
#define  RS485_CMD_KEEPALIVE		(RS485_1_CMD_BASE + 2)
#define  RS485_CMD_DATA				(RS485_1_CMD_BASE + 3)
#define  RS485_CMD_COME_BACK_DATA	(RS485_1_CMD_BASE + 4)

/* Á½Â·RS485 */
#define RS485_ROAD_1      1
#define RS485_ROAD_2      2

#define PORT_RS485		9110

#ifdef WIN32
DWORD __stdcall rs485ResultThread(LPVOID lpParameter);
#else
void *rs485ResultThread(void* lpParameter);
#endif

#ifdef WIN32
DWORD __stdcall reConnectRs485Thread(LPVOID lpParameter);
#else
void *reConnectRs485Thread(void* lpParameter);
#endif

#ifdef WIN32
DWORD __stdcall rs485KeepliveThread(LPVOID lpParameter);
#else
void *rs485KeepliveThread(void* lpParameter);
#endif


extern int rk_sdk_rs485_devinfo_add(_device_info *_devinfo);
extern int rk_sdk_rs485_devinfo_del(RkSerialHandle handle);
extern RkSerialHandle _rk_sdk_rs485_connect(RkSerialHandle serialHandle, RkSdkHandle handle, int serialPort, void *usePtr);
extern RkSerialHandle rk_sdk_rs485_connect(RkSdkHandle handle, int serialPort, void *usePtr); //serialPort: RS485_ROAD_1\RS485_ROAD_2
extern int rk_sdk_rs485_devinfo_find(RkSerialHandle handle, _device_info *_devinfo);
extern int rk_sdk_rs485_devinfo_keeplive_delay(RkSerialHandle handle, int delay);
extern int rk_sdk_rs485_disconnect(RkSerialHandle handle);
extern int rk_sdk_rs485_SerialSend(RkSdkHandle handle, unsigned char *pdata, int size);



#endif
