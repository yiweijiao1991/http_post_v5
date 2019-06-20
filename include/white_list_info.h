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


/*

函数名称:
	white_list_add
函数说明:
	插入一条白名单数据
参数:
	white_list_data[in]:白名单信息
返回值:
	1 成功 其他失败
*/
int white_list_add(white_list_data_s *white_list_data);

/*
 *@function name: 
 	white_list_update
 *@Author: yiweijiao
 *@Date: 2019-06-20 14:28:53
 *@describtion: 
	白名单更新
 *@parameter: 
	white_list_data[in]:白名单数据
 *@return: 
	-1 失败
	1成功
*/
int white_list_update(white_list_data_s *white_list_data);



/*
 *@function name: 
	white_list_delete
 *@Author: yiweijiao
 *@Date: 2019-06-20 14:32:08
 *@describtion: 
	根据车牌号码删除一条白名单数据
 *@parameter: 
	plate_number[in]:车牌号码
 *@return: 
	-1 失败
	1成功
*/
int white_list_delete(char *plate_number);
/*
 *@function name: 
	white_list_delete_all
 *@Author: yiweijiao
 *@Date: 2019-06-20 14:38:49
 *@describtion: 
	清空白名单数据
 *@parameter: 
	无
 *@return: 
	-1 失败
	1成功
*/
int white_list_delete_all();

#endif
