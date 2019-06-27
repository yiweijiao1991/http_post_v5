#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
 #include <unistd.h>
#ifdef SQLLIST
#include <sys/time.h>
#endif
#include "white_list_info.h"
#include "WTYSDK_WhiteList.h"
#include "log.h"
//用于暂存白名单查询出的车辆信息
typedef struct carinfo_temp_t
{
	WTYSDK_WLIST_VEHICLE carinfo;//车辆信息
	int is_select_ok;//查询是否结束
}carinfo_temp_t;

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


/*
 *@function name: 
	select_white_list_info_call
 *@Author: yiweijiao
 *@Date: 2019-06-27 11:34:19
 *@describtion: 
	查询车辆信息的回调函数
 *@parameter: 
 *@return: 
*/

void select_white_list_info_call(WTYSDK_WLIST_CB_TYPE type, WTYSDK_WLIST_VEHICLE *pLP,
								WTYSDK_WLIST_CUSTOMER *pCustomer,
								WTYSDK_WLIST_TRAFFIC_INFO *pTrafficInfo,
								int nTotal,
								void *pUserData)

{
	carinfo_temp_t *p = (carinfo_temp_t *)pUserData;
	if(p)
	{
		if(type == WTYSDK_WLIST_CB_TYPE_VEHICLE)
		{
			if(nTotal > 0 && pLP)
			{
				memcpy(&(p->carinfo),pLP,sizeof(WTYSDK_WLIST_VEHICLE));
			}
		}
		p->is_select_ok = 1;
	}
} 

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


int white_list_update(white_list_data_s *white_list_data)
{
	WTYSDK_WLIST_VEHICLE white_list_updata_info;//更新后的车辆信息
	static carinfo_temp_t search_carinfo;	//暂存查询出的车辆信息
	WTY_SDK_VEHICLEQUERYRECORD search_condition;	//查询车辆信息条件
	int ret = 0;
	int i = 0;
	static int is_init = 0;
	memset(&search_carinfo,0,sizeof(carinfo_temp_t));
	memset(&search_condition,0,sizeof(WTY_SDK_VEHICLEQUERYRECORD));

	//注册回调函数
	if(!is_init)
	{
		ret = WTYSDK_WhiteListSetQueryCallBack(select_white_list_info_call,(void*)&search_carinfo,LOCAL_SERVER_IP);
		if(ret != 0)
		{

				return -1;
		}
		is_init = 1;
	}

	strcpy(search_condition.PlateNumber,white_list_data->plate_number);
	search_condition.conditionsflag = 4;
	search_condition.page = 0;
	search_condition.pageSize = WTYSDK_WLIST_PERPAGE_MAXSIZE;
	//查询
	ret = WTY_WhiteListLoadVehicleByPlateId(LOCAL_SERVER_IP,search_condition);
	if(ret < 0)
	{
		return -1;
	}
		
	//等候数据到达
	for(i = 0;i<10;i++)
	{
		if(!search_carinfo.is_select_ok)
		{
			usleep(20000);

		}else
		{
			break;
		}
	}
	//更改
	memset(&white_list_updata_info,0,sizeof(WTYSDK_WLIST_VEHICLE));
	white_list_updata_info.bEnable = 1;
	strcpy(white_list_updata_info.strPlateID,white_list_data->plate_number);

	white_list_updata_info.bUsingTimeSeg = white_list_data->time_match;

	strcpy(white_list_updata_info.struTMCreate,white_list_data->create_time);

	strcpy(white_list_updata_info.struTMEnable,white_list_data->start_time);

	strcpy(white_list_updata_info.struTMOverdule,white_list_data->end_time);

	white_list_updata_info.iBlackList = white_list_data->isblack_list;

	white_list_updata_info.uCustomerID = search_carinfo.carinfo.uCustomerID;

	ret = WTY_WhiteListUpdateVehicleByPlateId(LOCAL_SERVER_IP,white_list_updata_info);

	if(ret == 0)
		return 1;
	return -1;
}
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
int white_list_delete(char *plate_number)
{
	int ret = 0;
	ret  = WTY_WhiteListDeleteVehicleByPlateId(LOCAL_SERVER_IP,plate_number);
	if(ret == 0)
		return 1;
	return -1;
}
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
int white_list_delete_all()
{
	int ret = 0;
	ret = WTY_WhiteListEmptyVehicle(LOCAL_SERVER_IP);
	if(ret == 0)
		return 1;
	return -1;

}
