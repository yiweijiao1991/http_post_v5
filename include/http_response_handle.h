#ifndef __HTTP_RESPONSE_HANDLE_H__
#define __HTTP_RESPONSE_HANDLE_H__


/*
函数说明:
	处理http回应数据
参数:
	data[in]:http回传的数据
	barrier_control_result[out]:道闸是控制结果 0落杆 1抬杆 -1 不操作
	rs485_delay[in]:rs485延时,单位毫秒
返回值:
	-1:参数非法
	-2:解析json结构失败
	-3:获取Response节点失败
	0:成功
*/
int response_handle(unsigned char * data,int *barrier_control_result,int rs485_delay);
#endif