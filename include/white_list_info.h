#ifndef __WHITE_LIST_INFO_H__
#define __WHITE_LIST_INFO_H__
#include "sqlite3.h"
#include "DatabaseMgr.h"
#define NAMELENS        	32

/*白名单数据*/
typedef struct white_list_data_s{
	char plate_number[20];	//车牌号码
	unsigned char time_match;//时间段匹配
	char create_time[20];//创建时间
	char start_time[20];//开始时间
	char end_time[20];	//结束时间
	unsigned char isblack_list;//是否黑名单
} white_list_data_s;

/*
函数名称:
	get_http_white_list_info
函数功能：
	从白名单中获取相应的信息
参数:
	result[in/out]识别结果结构体
返回值:
	-1失败
	0没有该车辆信息
	1成功
	
*/
int get_http_white_list_info(Reco_Result *result);

#endif
