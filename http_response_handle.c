#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>

#include "http_response_handle.h"
#include "cJSON.h"
#include "cameraInterface.h"
#include "base64.h"
#include "log.h"
#include "white_list_info.h"


#define BARRIER_OPERATION_OPEN  1	//抬杆
#define BARRIER_OPERATION_CLOSE 0	//落杆

//白名单操作指令
enum  WHITE_LIST_OPERATION{
	WHITE_LIST_ADD = 1, 	//增加
	WHITE_LIST_UPDATE,		//修改
	WHTTE_LIST_DELETE,		//删除
	WHITE_LIST_DELETE_ALL	//清空
};

/*
函数说明:
	控制道闸起落
参数:
	operation[in]:道闸控制指令
		BARRIER_OPERATION_OPEN:开闸
		BARRIER_OPERATION_CLOSE:落杆
返回值:
	0 陈功
	-1失败


*/
static int barrier_control_func(int operation)
{
	int ret = 0;
	switch(operation)
	{
		case BARRIER_OPERATION_OPEN:
			//抬杆
			barrier_gate_open();
			ret = 0;
			break;
		case BARRIER_OPERATION_CLOSE:
			//落杆
			 barrier_gate_close();
			 ret = 0;
			break;
		default:
			//异常
			log_write("barrier control error operation cmd invalid ,cmd = %d",operation);
			ret = -1;
	}
	return ret;
}
/*
函数说明:
	控制GPIO输出
参数:
	gpio_num[in]:ip编号
	state[in]:状态
返回值:
	0 成功
	-1失败
*/
static int gpio_out_func(int gpio_num,int state)
{
	if(gpio_num >= 1 && gpio_num<= 3)
	{
		if(state)
			 gpio_out_set(gpio_num,1);
		 else
			 gpio_out_set(gpio_num,0);
		return 0;
	}
	log_write("gpio out set error gpio number error,number = %d",gpio_num);
	return -1;
}
/*
函数说明:
	控制rs485的输出
参数:
	data[in]:需要透传的数据(base64编码)
返回值:
	-1失败
	0成功
	
*/
static int rs485_out(char * data)
{
	unsigned char send_buff[1024*2] = {0};
	int send_data_len = 0;
	if(strlen((char *)data) > 1024*2)
	{
		log_write("rs485 out error, this rs485 data len is too long	,length = %d",strlen((char *)data) );
		return -1;
	}
		
	send_data_len = base64_decode(data,send_buff);
	if(send_data_len > 0)
	{
		 rs485_transparent(send_buff,send_data_len);
		 return 0;
	}
	else
	{
		log_write("rs485 out error,base64 decode error");
		return -1;
	}
		
}
/*
模拟触发
*/
static int triger()
{
	triger_send();
	return 0;
}
/*
函数说明:
	白名单处理
参数:
	operation[in]:操作命令
	white_list_data[in]:白名单数据
返回值:
	-1 失败
	0 成功
*/
static int white_list_handle(int operation,white_list_data_s *white_list_data)
{
	int ret = 0;
	switch(operation)
	{
		case WHITE_LIST_ADD:
			if(1 != white_list_add(white_list_data))
			{
				log_write("white list add faile,platenumber:%s",white_list_data->plate_number);
				ret = -1;
			}else
			{
				log_write("white list add success,platenumber:%s",white_list_data->plate_number);
				ret = 0;
			}

			break;
		case WHITE_LIST_UPDATE:
			if(1 != white_list_update(white_list_data))
			{
				log_write("white list update faile,plate number:%s",white_list_data->plate_number);
				ret = -1;
			}else
			{
				log_write("white list update success,plate number:%s",white_list_data->plate_number);
				ret = 0;
			}
			break;
		case WHTTE_LIST_DELETE:
			if(1 != white_list_delete(white_list_data->plate_number))
			{
				log_write("white list delete faile,plate number:%s",white_list_data->plate_number);
				ret = -1;
			}else
			{
				log_write("white list delete success,plate number:%s",white_list_data->plate_number);
				ret = 0;
			}
			break;
		case WHITE_LIST_DELETE_ALL:
			if(1 != white_list_delete_all())
			{
				log_write("white list delete all faile");
				ret = -1;
			}else
			{
				log_write("white list delete all success");
				ret = 0;
			}
			break;
		default:
			log_write("white list operation faile, cmd error ,cmd = %d\n",operation);
			ret = -1;
			break;
	}
	return ret;
}
/*
函数说明:
	处理http回应数据
参数:
	data[in]:http回传的数据
	barrier_control_result[out]:道闸是控制结果
返回值:
	-1:参数非法
	-2:解析json结构失败
	-3:获取Response节点失败
	0:成功
*/
int response_handle(unsigned char * data,int *barrier_control_result)
{	
	cJSON * root = NULL;
	cJSON * response=NULL;
	cJSON * barrier_control=NULL;
	cJSON * gpio_out=NULL;
	cJSON * rs485_data=NULL;
	cJSON * trigger_data=NULL;
	cJSON * whitelist_data=NULL;
	/*老版本协议处理*/
	cJSON * barrier_control_old=NULL;
	cJSON * rs485_data_old = NULL;
	
	int ret_value = 0;
	int array_number = 0;
	int i = 0;
	
	*barrier_control_result = -1;
	
	if(!data)
	{
		ret_value =  -1;
		goto out;
	} 
		
	root = cJSON_Parse((char *)data); 
	if(root == NULL)
	{
		ret_value =  -2;
		goto out;
	}
	response = cJSON_GetObjectItem(root,"Response");
	if(response == NULL)
	{
		ret_value = -3;
		goto out;
	}
	//道闸控制
	barrier_control = cJSON_GetObjectItem(response,"barrier_control");
	if(barrier_control)
	{
		cJSON * action = cJSON_GetObjectItem(barrier_control,"action");
		if(action)
		{
			if(cJSON_IsString(action))
			{
				if(strcmp("open",action->valuestring) == 0)
				{
					//抬杆
					barrier_control_func(BARRIER_OPERATION_OPEN);
					*barrier_control_result = BARRIER_OPERATION_OPEN;
				}else if(strcmp("close",action->valuestring) == 0)
				{
					//落杆
					barrier_control_func(BARRIER_OPERATION_CLOSE);
					*barrier_control_result = BARRIER_OPERATION_CLOSE;
				}
			}	
		}
	}

	//gpio 输出控制
	gpio_out = cJSON_GetObjectItem(response,"gpio_out");
	if(gpio_out)
	{
		if(cJSON_IsArray(gpio_out))
		{
			cJSON *gpio_subitem =NULL; 
			array_number = cJSON_GetArraySize(gpio_out);
			for(i = 0;i < array_number;i++)
			{
				gpio_subitem =  cJSON_GetArrayItem(gpio_out, i);
				if(gpio_subitem)
				{
					if(cJSON_IsNumber(cJSON_GetObjectItem(gpio_subitem,"io_num")))
					{
						if((cJSON_IsNumber(cJSON_GetObjectItem(gpio_subitem,"data"))))
						{
							gpio_out_func(cJSON_GetObjectItem(gpio_subitem,"io_num")->valueint,
										  cJSON_GetObjectItem(gpio_subitem,"data")->valueint);
						}		
					}

				}
			}
		}
	}
	//rs485控制
	rs485_data = cJSON_GetObjectItem(response,"rs485_data");
	if(rs485_data)
	{
		
		if(cJSON_IsArray(rs485_data))
		{
			cJSON *rs485_subitem =NULL;
			array_number = cJSON_GetArraySize(rs485_data);
			for(i = 0;i< array_number;i++)
			{
				rs485_subitem = cJSON_GetArrayItem(rs485_data, i);
				if(rs485_subitem)
				{
					if(cJSON_IsString(cJSON_GetObjectItem(rs485_subitem,"data")))
					{	
						//调用接口进行透明传输
						rs485_out(cJSON_GetObjectItem(rs485_subitem,"data")->valuestring);
						//延时	
						usleep(50000);
					}				
				}

			}	
		}	
	}
	//模拟触发
	trigger_data = cJSON_GetObjectItem(response,"trigger_data");
	if(trigger_data)
	{
		if(cJSON_IsString(cJSON_GetObjectItem(trigger_data,"action")))
		{
			if(strcmp("on",cJSON_GetObjectItem(trigger_data,"action")->valuestring) == 0)
			{
				//强制模拟触发
				triger();
			}
		}
	}
	//白名单处理
	whitelist_data = cJSON_GetObjectItem(response,"whitelist_data");
	if(whitelist_data)
	{
		if(cJSON_IsArray(whitelist_data))
		{
			cJSON *whitelist_subitem =NULL;
			white_list_data_s white_list_temp;
			array_number = cJSON_GetArraySize(rs485_data);
			int white_list_operation = 0;
			for(i = 0;i < array_number; i++)
			{
					whitelist_subitem = cJSON_GetArrayItem(whitelist_data, i);
					if(whitelist_subitem)
					{
						white_list_operation = 0;
						memset(&white_list_temp,0,sizeof(white_list_temp));
						//操作命令
						if(cJSON_IsString(cJSON_GetObjectItem(whitelist_subitem,"action")))
						{
							if(strcmp("add",cJSON_GetObjectItem(whitelist_subitem,"action")->valuestring) == 0)
								white_list_operation = WHITE_LIST_ADD;
							else if(strcmp("update",cJSON_GetObjectItem(whitelist_subitem,"action")->valuestring) == 0)
								white_list_operation = WHITE_LIST_UPDATE;
							else if(strcmp("delete",cJSON_GetObjectItem(whitelist_subitem,"action")->valuestring) == 0)
								white_list_operation = WHTTE_LIST_DELETE;
							else if(strcmp("delete_all",cJSON_GetObjectItem(whitelist_subitem,"action")->valuestring) == 0)
								white_list_operation = WHITE_LIST_DELETE_ALL;
						}
						//车牌号码
						if(cJSON_IsString(cJSON_GetObjectItem(whitelist_subitem,"plate_number")))
						{
							strcpy(white_list_temp.plate_number,
									cJSON_GetObjectItem(whitelist_subitem,"plate_number")->valuestring);
						}
						//时间匹配使能
						if(cJSON_IsString(cJSON_GetObjectItem(whitelist_subitem,"time_match")))
						{
							if(strcmp("enable",cJSON_GetObjectItem(whitelist_subitem,"time_match")->valuestring))
								white_list_temp.time_match = 1;
							else
								white_list_temp.time_match = 0;
						}
						//创建时间
						if(cJSON_IsString(cJSON_GetObjectItem(whitelist_subitem,"create_time")))
						{
							strcpy(white_list_temp.create_time,
									cJSON_GetObjectItem(whitelist_subitem,"create_time")->valuestring);
						}
						//开始时间
						if(cJSON_IsString(cJSON_GetObjectItem(whitelist_subitem,"start_time")))
						{
							strcpy(white_list_temp.start_time,
									cJSON_GetObjectItem(whitelist_subitem,"start_time")->valuestring);
						}
						//结束时间
						if(cJSON_IsString(cJSON_GetObjectItem(whitelist_subitem,"end_time")))
						{
							strcpy(white_list_temp.end_time,
									cJSON_GetObjectItem(whitelist_subitem,"end_time")->valuestring);
						}
						//黑名单
						if(cJSON_IsString(cJSON_GetObjectItem(whitelist_subitem,"is_black_list")))
						{
							if(strcmp("yes",cJSON_GetObjectItem(whitelist_subitem,"is_black_list")->valuestring))
								white_list_temp.isblack_list = 1;
							else
								white_list_temp.isblack_list = 0;
						}
						//处理白名单数据
						white_list_handle(white_list_operation,&white_list_temp);	
					}
			}
		}

	}
	/*
	老版本协议处理
	*/
	//抬杆
	barrier_control_old = cJSON_GetObjectItem(response,"Open");
	if(cJSON_IsNumber(barrier_control_old))
	{
		if(barrier_control_old->valueint == 1)
		{
			*barrier_control_result = BARRIER_OPERATION_OPEN;
			barrier_control_func(BARRIER_OPERATION_OPEN);
		}
				
	}
	/*
	透传
	*/
	rs485_data_old =  cJSON_GetObjectItem(response,"SerialData");
	if(rs485_data_old)
	{
			if(cJSON_IsNumber(cJSON_GetObjectItem(rs485_data_old,"datalen")))
			{
				if(cJSON_IsString(cJSON_GetObjectItem(rs485_data_old,"data")))
				{
					rs485_out(cJSON_GetObjectItem(rs485_data_old,"data")->valuestring);
				}
			}
	}
out:

	if(!root)
	{
		cJSON_Delete(root);
		root = NULL;
	}
	return ret_value;
}
