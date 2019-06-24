#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include "generat_http_data.h"
#include "cJSON.h"
#include "http_post_data.h"
#include "code_cover.h"
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
							int characters_type)
{
	//定义json节点
	cJSON * root=NULL; //根节点
	cJSON *plate_info=NULL;//车牌信息
	cJSON *result=NULL;//识别结果
	cJSON *plate_result=NULL;//车牌结果
	cJSON *location=NULL;//车牌位置
	cJSON *time_stamp=NULL;//时间
	cJSON *rect=NULL;
	cJSON *time_val=NULL;
	//白名单信息
	cJSON *white_list_control = NULL;//白名单控制结果
	cJSON *white_list_car_info = NULL;//车辆白名单信息
	cJSON *white_list_info = NULL;	//白名单那信息节点
	int ret = 0;
	//json输出数据的临时传冲区
	char * p_data = NULL;
	if(!reco_result || !device_info || !http_cfg || !json_buff|| json_buff_max_size <= 0)
	{
		//输入参数错误
		return -1;
	}
	root =cJSON_CreateObject();
	plate_info =cJSON_CreateObject();
	result =cJSON_CreateObject();
	plate_result =cJSON_CreateObject();
	//创建接送节点
	if(root == NULL ||plate_info == NULL ||result == NULL ||plate_result == NULL)
	{
		//创建节点失败
		if(root)
			cJSON_Delete(root);
		if(plate_info)
			cJSON_Delete(plate_info);
		if(result)
			cJSON_Delete(result);
		if(plate_result)
			cJSON_Delete(plate_result);
		return -2;	
	}
	
	
	//组织plate_result
	if(http_cfg->data_type == 0)
	{

			cJSON_AddNumberToObject(plate_result,"bright",0);
			cJSON_AddNumberToObject(plate_result,"carBright",reco_result->plate_info.car_birght);
			cJSON_AddNumberToObject(plate_result,"carColor",reco_result->plate_info.car_color);

			cJSON_AddNumberToObject(plate_result,"colorType",reco_result->plate_info.color_value);
	}

	if(http_cfg->data_type == 0)
		cJSON_AddNumberToObject(plate_result,"colorValue",reco_result->plate_info.color_value);
	if(http_cfg->data_type == 0 || http_cfg->data_type == 1|| http_cfg->data_type == 2)
		cJSON_AddNumberToObject(plate_result,"confidence",reco_result->plate_info.confidence);


	if(http_cfg->data_type == 0 || http_cfg->data_type == 1|| http_cfg->data_type == 2)
		cJSON_AddNumberToObject(plate_result,"direction",reco_result->plate_info.direction);
	if(http_cfg->data_type == 0)
		cJSON_AddStringToObject(plate_result,"imagePath","");
	cJSON_AddStringToObject(plate_result,"license",reco_result->plate_info.license);

	if(http_cfg->data_type == 0 || http_cfg->data_type == 1)
	{
		location =cJSON_CreateObject();
		if(location != NULL)
		{
			rect =cJSON_CreateObject();
			if(rect != NULL)
			{
				//组织rect
				cJSON_AddNumberToObject(rect,"bottom",reco_result->plate_info.location.bottom);
				cJSON_AddNumberToObject(rect,"left",reco_result->plate_info.location.left);
				cJSON_AddNumberToObject(rect,"right",reco_result->plate_info.location.right);
				cJSON_AddNumberToObject(rect,"top",reco_result->plate_info.location.top);
				//组织location
				cJSON_AddItemToObject(location,"RECT",rect);
			}
			cJSON_AddItemToObject(plate_result,"location",location);
		}
		
	}		
	if(http_cfg->data_type == 0)
	{
		
		time_stamp =cJSON_CreateObject();
		if(time_stamp != NULL)
		{
			
			time_val =cJSON_CreateObject();
			if(time_val != NULL)
			{
				//组织time_val
				cJSON_AddNumberToObject(time_val,"sec",0);
				cJSON_AddNumberToObject(time_val,"usec",0);
				//组织time_stamp
				cJSON_AddItemToObject(time_stamp,"TimeVal",time_val);
			}
			cJSON_AddItemToObject(plate_result,"timeStamp",time_stamp);
		}

	}
		
	if(http_cfg->data_type == 0 || http_cfg->data_type == 1)
		cJSON_AddNumberToObject(plate_result,"timeUsed",reco_result->plate_info.reco_time);


	if(http_cfg->data_type == 0 || http_cfg->data_type == 1|| http_cfg->data_type == 2)
	{
		cJSON_AddNumberToObject(plate_result,"triggerType",reco_result->plate_info.triger_type);
		cJSON_AddNumberToObject(plate_result,"type",reco_result->plate_info.plate_type);
	}
	cJSON_AddStringToObject(plate_result,"platecolor",reco_result->plate_info.color);
	if(http_cfg->data_type == 0)
	{
		cJSON_AddNumberToObject(plate_result,"carLogo",reco_result->plate_info.car_logo);
		cJSON_AddNumberToObject(plate_result,"CarType",reco_result->plate_info.car_type);
		cJSON_AddNumberToObject(plate_result,"plateBright",reco_result->plate_info.bright);
	}

	cJSON_AddStringToObject(plate_result,"recotime",reco_result->recotime);

	//图片	
	if(full_image)
	{
		cJSON_AddStringToObject(plate_result,"imageFile",full_image);
		cJSON_AddNumberToObject(plate_result,"imageFileLen",strlen(full_image));
	}else
	{
		cJSON_AddStringToObject(plate_result,"imageFile","");
		cJSON_AddNumberToObject(plate_result,"imageFileLen",0);
	}
	
	if(plate_image)
	{
		cJSON_AddStringToObject(plate_result,"imageFragmentFile",plate_image);
		cJSON_AddNumberToObject(plate_result,"imageFragmentFileLen",strlen(plate_image));		
	}else
	{
		cJSON_AddStringToObject(plate_result,"imageFragmentFile","");
		cJSON_AddNumberToObject(plate_result,"imageFragmentFileLen",0);		
	}

	//组织result
	cJSON_AddItemToObject(result,"PlateResult",plate_result);

	//组织plate_info

	cJSON_AddNumberToObject(plate_info,"resultType",reco_result->result_type);

	if(http_cfg->data_type == 0 || http_cfg->data_type == 1|| http_cfg->data_type == 2)
	{
		cJSON_AddNumberToObject(plate_info,"channel",http_cfg->direction);
		cJSON_AddStringToObject(plate_info,"deviceName",device_info->devname);
		cJSON_AddStringToObject(plate_info,"ipaddr",reco_result->devip);
	}

	cJSON_AddItemToObject(plate_info,"result",result);
	cJSON_AddStringToObject(plate_info,"seriaIno",device_info->roomID);

	if(http_cfg->data_type == 0 || http_cfg->data_type == 1|| http_cfg->data_type == 2)
	{
		cJSON_AddNumberToObject(plate_info,"nParkID",atol(http_cfg->park_id));
		cJSON_AddStringToObject(plate_info,"ParkID",http_cfg->park_id);
		cJSON_AddStringToObject(plate_info,"ParkName",http_cfg->park_name);
		cJSON_AddStringToObject(plate_info,"ParkDoor",http_cfg->park_location);
	}

	//组织root
	cJSON_AddItemToObject(root,"AlarmInfoPlate",plate_info);
	if(http_cfg->data_type == 0 || http_cfg->data_type == 1)
	{
		//组织白名单数据

		white_list_info = cJSON_CreateObject();
		if(white_list_info != NULL)
		{
			//白名单信息
			white_list_car_info = cJSON_CreateObject();
			if(white_list_car_info != NULL)
			{
				cJSON_AddNumberToObject(white_list_car_info,"WhiteListEnable",reco_result->white_list_data.enable);
				cJSON_AddNumberToObject(white_list_car_info,"TimeMatchEable",reco_result->white_list_data.time_match_enable);
				cJSON_AddStringToObject(white_list_car_info,"CreateTime",reco_result->white_list_data.create_time);
				cJSON_AddStringToObject(white_list_car_info,"StartTime",reco_result->white_list_data.start_time);	
				cJSON_AddStringToObject(white_list_car_info,"EndTime",reco_result->white_list_data.end_time);
				cJSON_AddNumberToObject(white_list_car_info,"BlackList",reco_result->white_list_data.is_black);
				cJSON_AddItemToObject(white_list_info,"Info",white_list_car_info);
			}
			white_list_control = cJSON_CreateObject();
			if(white_list_control != NULL)
			{
				//白名单控制信息
				cJSON_AddNumberToObject(white_list_control,"OpenDoor",reco_result->white_list_data.barrier_control);
				cJSON_AddNumberToObject(white_list_control,"TimeCheck",reco_result->white_list_data.time_validity);
				cJSON_AddNumberToObject(white_list_control,"InteriorCar",reco_result->white_list_data.is_interior_car);		
				cJSON_AddItemToObject(white_list_info,"WhiteListControl",white_list_control);
			}	
			cJSON_AddItemToObject(root,"WhiteListInfo",white_list_info);	
		}	
	}
	p_data = cJSON_Print(root);
	if(NULL == p_data)
	{
		//生成json字符串失败
		cJSON_Delete(root);
		return -3;
	}
	cJSON_Delete(root);
	
	if(json_buff_max_size < strlen(p_data) -1)
	{
		//存储内存开辟的过小
		if(p_data)
		{
			free(p_data);
			p_data = NULL;
		}
		return -4;
	}

	memset(json_buff,0,json_buff_max_size);
	if(characters_type == CHARCTER_TYPE_UTF8)
	{
		ret = code_convert("gb2312","utf-8",p_data,strlen(p_data),json_buff,json_buff_max_size);
		if(ret == -1)
		{
			if(p_data)
			{
				free(p_data);
				p_data = NULL;
			}
			return -5;
		}else if(ret == -2)
		{
			if(p_data)
			{
				free(p_data);
				p_data = NULL;
			}
			return -6;
		}
	}else
	{
		memcpy(json_buff,p_data,strlen(p_data));
	}


	if(p_data)
	{
		free(p_data);
		p_data = NULL;
	}
	//成功
	return 0;
}

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
int get_keepalive_json(RK_DeviceInfo *device_info,char * json_buff,int json_buff_max_size)
{
	cJSON * root=NULL;
	cJSON *keep_alive=NULL;
	char  *p_data = NULL;
	
	if(!json_buff || !device_info || json_buff_max_size <= 0)
	{
		return -1;
	}

	root =cJSON_CreateObject();

	if(NULL == root)
	{
		return -2;	
	}
	keep_alive =cJSON_CreateObject();
	if(NULL == keep_alive)
	{
		cJSON_Delete(root);
		return -2;
	}
	
	//获取SN
	cJSON_AddStringToObject(keep_alive,"ROMID",device_info->roomID);
	cJSON_AddStringToObject(keep_alive,"SN",device_info->sn);

	//组织root
	cJSON_AddItemToObject(root,"KeepAlive",keep_alive);
	p_data = cJSON_Print(root);
	if(NULL == p_data)
	{
		cJSON_Delete(root);
		return -3;
	}
	cJSON_Delete(root);
	
	
	if(json_buff_max_size < strlen(p_data) -1)
	{
		//存储内存开辟的过小
		if(p_data)
		{
			free(p_data);
			p_data = NULL;
		}
		return -4;
	}

	memset(json_buff,0,json_buff_max_size);
	memcpy(json_buff,p_data,strlen(p_data));
	if(p_data)
	{
		free(p_data);
		p_data = NULL;
	}	
	return 0;	
}


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
int get_io_state_json(RK_DeviceInfo *device_info,RK_Gpio * io_state,char * json_buff,int json_buff_max_size)
{
	cJSON * root=NULL;
	cJSON *gpio_state =NULL;
	char  *p_data = NULL;
	
	if(!json_buff || !device_info || !io_state || json_buff_max_size <= 0)
	{
		return -1;
	}

	root =cJSON_CreateObject();

	if(NULL == root)
	{
		return -2;	
	}
	gpio_state =cJSON_CreateObject();
	if(NULL == gpio_state)
	{
		cJSON_Delete(root);
		return -2;
	}
	
	//获取SN
	cJSON_AddStringToObject(gpio_state,"rom_id",device_info->roomID);
	cJSON_AddStringToObject(gpio_state,"sn",device_info->sn);
	cJSON_AddNumberToObject(gpio_state,"gpio_in1_statue",io_state->in1);
	cJSON_AddNumberToObject(gpio_state,"gpio_in2_statue",io_state->in2);
	cJSON_AddNumberToObject(gpio_state,"gpio_in3_statue",io_state->in3);
	cJSON_AddNumberToObject(gpio_state,"gpio_in4_statue",io_state->in4);
	cJSON_AddNumberToObject(gpio_state,"gpio_userkey_statue",io_state->userkey);
	//时间戳后面添加
	
	//组织root
	cJSON_AddItemToObject(root,"gpio_state",gpio_state);
	
	p_data = cJSON_Print(root);
	if(NULL == p_data)
	{
		cJSON_Delete(root);
		return -3;
	}
	cJSON_Delete(root);
	
	
	if(json_buff_max_size < strlen(p_data) -1)
	{
		//存储内存开辟的过小
		if(p_data)
		{
			free(p_data);
			p_data = NULL;
		}
		return -4;
	}

	memset(json_buff,0,json_buff_max_size);
	memcpy(json_buff,p_data,strlen(p_data));
	if(p_data)
	{
		free(p_data);
		p_data = NULL;
	}	
	return 0;	
}

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
int get_response_json(RK_DeviceInfo *device_info,char * json_buff,int status,int json_buff_max_size)
{
	cJSON * root=NULL;
	cJSON *commond_respond=NULL;
	char  *p_data = NULL;
	
	if(!json_buff || !device_info || json_buff_max_size <= 0)
	{
		return -1;
	}

	root =cJSON_CreateObject();
	if(NULL == root)
	{
		return -2;	
	}
	
	commond_respond =cJSON_CreateObject();
	if(commond_respond == NULL)
	{
		if(root)
			cJSON_Delete(root);	
		return -2;
	}

	cJSON_AddStringToObject(commond_respond,"ROMID",device_info->roomID);
	cJSON_AddStringToObject(commond_respond,"SN",device_info->sn);

	cJSON_AddNumberToObject(commond_respond,"statue",status);
	//组织root
	cJSON_AddItemToObject(root,"CommondRespond",commond_respond);

		
	p_data = cJSON_Print(root);
	if(NULL == p_data)
	{
		cJSON_Delete(root);
		return -3;
	}
	cJSON_Delete(root);
		
	if(json_buff_max_size < strlen(p_data) -1)
	{
		//存储内存开辟的过小
		if(p_data)
		{
			free(p_data);
			p_data = NULL;
		}
		return -4;
	}

	memset(json_buff,0,json_buff_max_size);
	memcpy(json_buff,p_data,strlen(p_data));
	if(p_data)
	{
		free(p_data);
		p_data = NULL;
	}	
	return 0;	

}

