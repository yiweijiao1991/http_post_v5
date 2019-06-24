#ifndef _CJSON_CLIENT_H
#define _CJSON_CLIENT_H

#include "RK_LPRClientSDK.h"
#include "cJSON.h"

//0 即时推送数据 1 异常数据重传
#define ABNORMAL_DATA  1  //1 异常数据重传
#define REALTIME_DATA  0  //0 即时推送数据

typedef struct
{
	int left;
	int top;
	int right;
	int bottom;
}plate_location_s;

typedef struct
{
	char license[16];
	char color[8];
	int color_value;
	int plate_type;
	int confidence;
	int reco_time;
	int bright;
	int direction;
	plate_location_s  location;
	int car_birght;
	int car_color;
	int car_logo;
	int car_type;
	int triger_type;
}plate_info_s;
 typedef struct http_whitelist_info_s{
	int enable;	//白名单是否使能
	int time_match_enable;	//是否启用时间断匹配
	char create_time[20];	//白名单创建时间	
	char start_time[20];	//白名单生效时间
	char end_time[20];	//白名单过期时间 
	int is_black;		//是否被设置为黑名单
	int barrier_control;		//是否自动抬杆放行
	int time_validity;		//是否再有效期内部
	int is_interior_car;	      //是否为能内部车
} http_whitelist_info_s;
typedef struct http_reco_result_s
{
	char devip[16];
	char softver[16];
	char algover[16];
	int  platecount;
	char recotime[32];
	int result_type;	//识别结果类型  0 即时推送数据 1 异常数据重传
	plate_info_s  plate_info;
	http_whitelist_info_s white_list_data;
} http_reco_result_s;

/******************************************************
		接口
*******************************************************/
/*
函数名称：get_result_json_string
函数功能:
	组织识别结果json数据用于http上报
参数：
	http_cfg[in]:http配置参数
	reco_result[in]：识别结果结构体
	full_image[in]:大图base64数据
	plate_image[in]:小图的base64数据
	device_info[in]:设备信息
	json_buff[in/out]:json数据的存储区
	json_buff_max_size：json存储区大小
	characters_type[in]:输出汉字编码 0 GB2312 1 UTF8
返回值：
	-1 参数非法
	-2 json节点创建失败
	-3 生成json字符串失败
	-4 json 存储区 大小不够
	-5 iconv 打开失败
	-6 iconv转码失败
    0成功
	
*/
int  get_result_json_string(http_param_s *http_cfg,
							http_reco_result_s *reco_result,
							char * full_image,
							char *plate_image,
							RK_DeviceInfo *device_info,
							char * json_buff,
							int json_buff_max_size,
							int characters_type);
/*

get_keepalive_json
函数功能:
	组织心跳json数据用于http上报
参数：
	device_info[in]:设备信息
	json_buff[in/out]:json数据的存储区
	json_buff_max_size：json存储区大小
返回值：
	-1 参数非法
	-2 json节点创建失败
	-3 生成json字符串失败
	-4 json 存储区 大小不够
    0成功
*/
int get_keepalive_json(RK_DeviceInfo *device_info,char * json_buff,int json_buff_max_size);

/*
get_response_json
函数功能:
	组织回应json数据用于http上报
参数：
	device_info[in]:设备信息
	json_buff[in/out]:json数据的存储区
	json_buff_max_size：json存储区大小
返回值：
	-1 参数非法
	-2 json节点创建失败
	-3 生成json字符串失败
	-4 json 存储区 大小不够
    0成功
*/
int get_response_json(RK_DeviceInfo *device_info,char * json_buff,int status,int json_buff_max_size);


/*
get_io_state_json
函数功能:
	组织IO上报json数据用于http上报
参数：
	device_info[in]:设备信息
	io_state[in]:gpio数据
	json_buff[in/out]:json数据的存储区
	json_buff_max_size：json存储区大小
返回值：
	-1 参数非法
	-2 json节点创建失败
	-3 生成json字符串失败
	-4 json 存储区 大小不够
    0成功
*/
int get_io_state_json(RK_DeviceInfo *device_info,RK_Gpio *io_state,char * json_buff,int json_buff_max_size);

#endif
