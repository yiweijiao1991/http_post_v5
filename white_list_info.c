#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#ifdef SQLLIST
#include <sys/time.h>
#endif
#include "white_list_info.h"
#include "WTYSDK_WhiteList.h"
#include "log.h"


/*
函数名：whiteListInfo_search
函数说明：查询用表中的数据
参数说明：

	recoResult[in/out]：识别结果结构体


返回值:-1 失败 0成功 1 未找到

*/

static int get_white_list_serach(Reco_Result *result)

{
	int ret = 0;
	PlateInfo_t white_list_info;
	ret = WTY_WhiteListTrafficRequestByPlate(LOCAL_SERVER_IP,result->license,&white_list_info);
	if(ret == 1)
	{

		result->WhiteListEnable = white_list_info.Enable;//白名单是否启用
		memcpy(result->WhiteListCreateTime, white_list_info.CreateTime, strlen(white_list_info.CreateTime));//白名单创建时间
		memcpy(result->WhiteListStartTime,white_list_info.StartTime, strlen(white_list_info.StartTime));//白名单生效时间
		memcpy(result->WhiteListEndTime, white_list_info.Endtime, strlen( white_list_info.Endtime));//白名单失效时间
		result->TimeMatchEable = white_list_info.TimeMatch;//是否启用时间段匹配
		result->BlackList =  white_list_info.Blacklist;//是否黑名单
	}

	return ret;
	
}
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
int get_http_white_list_info(Reco_Result *result)
{
	int ret = 0;
	char nowtimestr[25] ={0};
	struct tm tm_time_start;
	struct tm tm_time_end;
	struct tm tm_now;

	long long start = 0;
	long long end = 0;
	long long now = 0;
		
	//查询
	ret = get_white_list_serach(result);
	if(ret < 0)
	{
		return -1;
	}
	if(ret == 0)
	{
		return 0;
	}

	result->OpenDoor = 0;//////////////////待补充
	
	strptime(result->WhiteListStartTime, "%Y-%m-%d %H:%M:%S", &tm_time_start);
	start = mktime(&tm_time_start);
	strptime(result->WhiteListEndTime, "%Y-%m-%d %H:%M:%S", &tm_time_end);
	end = mktime(&tm_time_end);
	strptime(result->recotime, "%Y-%m-%d %H:%M:%S", &tm_now);
	now = mktime(&tm_now);	
	if(now >= start && now <= end)
	{
		result->TimeCheck = 1;
	}else
	{
		result->TimeCheck = 0; 
	}
	result->InteriorCar = 1;
	return 1;	
}
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
int white_list_add(white_list_data_s *white_list_data)
{
	WTYSDK_IMPORTROWS import_white_list_info;//插入数据库白名单变量
	WTYSDK_IMPORT_FAILDREASON import_result;//导入数据库的结果
	int ret = 0;

	memset(&import_white_list_info,0,sizeof(WTYSDK_IMPORTROWS));
	memset(&import_result,0,sizeof(WTYSDK_IMPORT_FAILDREASON));
	//组织结构体
	import_white_list_info.nCarInfo.bEnable = 1;
	strcpy(import_white_list_info.nCarInfo.strPlateID,white_list_data->plate_number);
	import_white_list_info.nCarInfo.bUsingTimeSeg = white_list_data->time_match;
	strcpy(import_white_list_info.nCarInfo.struTMCreate,white_list_data->create_time);
	strcpy(import_white_list_info.nCarInfo.struTMEnable,white_list_data->start_time);
	strcpy(import_white_list_info.nCarInfo.struTMOverdule,white_list_data->end_time);
	import_white_list_info.nCarInfo.iBlackList = white_list_data->isblack_list;
	//导入数据
	ret = WTY_WhiteListImportRows(LOCAL_SERVER_IP,&import_white_list_info,&import_result,1);
	if(ret == 1)
	{
		return 1;
	}
	return ret;
}

int white_list_update(white_list_data_s *white_list_data)
{
	WTYSDK_WLIST_VEHICLE white_list_updata_info;
	int ret = 0;
	memset(&white_list_updata_info,0,sizeof(WTYSDK_WLIST_VEHICLE));

}
