#ifndef __RESULT_STRORE_H__
#define __RESULT_STRORE_H__
#include "RK_LPRClientSDK.h"
/*
函数名称:
	result_store_callback
函数功能
	接收到识别结果的回调函数
	将识别结果整理后存储到http的数据苦中
参数
	handle[in]:SDK的句柄
	plate_result[in]:识别结果结构体
	usePtr[in]:用户自用指针
返回值
	null
*/
void  result_store_callback (RkSdkHandle handle, RK_PlateResult *plate_result, void *use_ptr);
#endif
