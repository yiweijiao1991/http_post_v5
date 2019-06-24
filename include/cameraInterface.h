#ifndef __CAMERA_INTERFACE_H__
#define __CAMERA_INTERFACE_H__
#include "RK_LPRClientSDK.h"
#include "io_report.h"
//HTTP 图片存储路径
#define FLASH_FULLIMAGE_PATH  "/root/ipc/HTTP_DIR/HTTPImage/FullImage/"
#define FLASH_PLATEIMAGE_PATH "/root/ipc/HTTP_DIR/HTTPImage/PlatelImage/"
#define TF_FULLIMAGE_PATH     "/mnt/sdcard/HTTPImage/FullImage/"
#define TF_PLATEIMAGE_PATH     "/mnt/sdcard/HTTPImage/PlatelImage/"

#define TFCARD_PATH "/mnt/sdcard"
#define TFCARD_DEVICE "/dev/mmcblk0p1"



/*
函数名称:init_interface()
函数功能:对相机接口进行初始化
返回值 0 成功 -1 失败	
*/
int init_interface();
/*道闸打开*/
void barrier_gate_open();
/*道闸关闭*/
void barrier_gate_close();
/*
函数名称：RS485_transparent
函数功能:
	RS485透传数据
参数:
	pdata[in]:数据缓冲区地址
	size[in]:数据大小
返回值：
	null
*/
void rs485_transparent( unsigned char *pdata, int size);
/*
函数名称:device_info_get
函数功能:
	从相机程序中获取设备信息
参数:
	deviceInfo[out]:设备信息结构题指针
返回值:
	null
*/
void device_info_get(RK_DeviceInfo *deviceInfo);
/*
函数名称:http_config_get
函数功能：
	从相机内部获取http的配置参数
参数：
	httpinfo[out]:http的参数结构指针
返回值：
	null
*/
void http_config_get(http_param_s *http_info);


/*gpio输出*/
void gpio_out_set(unsigned char gpio_num,unsigned char out_state);
/*
模拟触发
*/
void triger_send();
#endif
