#ifndef __HTTP_RESPONSE_HANDLE_H__
#define __HTTP_RESPONSE_HANDLE_H__


/*
函数说明:
	处理http回应数据
参数:
	data[in]:http回传的数据
	barrier_control_result[out]:道闸是否抬杆控制结果
返回值:
	-1:参数非法
	-2:解析json结构失败
	-3:获取Response节点失败
	0:成功
*/
int response_handle(unsigned char * data,int *barrier_control_result);
#endif