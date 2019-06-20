#include <stdio.h>
#include <string.h>
#include "cameraInterface.h"
#include "result_store.h"
#include "RK_LPRClientSDK.h"
#include "rs485.h"
#include "log.h"
static RkSdkHandle g_rkhandle = 0;
static RkSerialHandle rs485handle = 0;
/*
函数名称:init_interface()
函数功能:对相机接口进行初始化
返回值 0 成功 -1 失败	
*/
int init_interface()
{
	int ret = -1;
	

	//SDK 初始化
	ret = rk_sdk_init();
	if(ret != 0)
	{
		log_write("rk sdk init faile ret = %d",ret);
		return -1;
	}
	//注册回调函数
	rk_sdk_recoResultEvent(result_store_callback);
	//注册IO的回调函数

	//链接相机
	while(g_rkhandle == 0)
	{
		g_rkhandle = rk_sdk_connect_device("127.0.0.1",NULL);
		if(g_rkhandle == 0)
		{
			log_write(" http rk sdk connect device faile ret = %d",g_rkhandle);
			sleep(1);
		}		
	}
	//建立RS485链接
	while(rs485handle == 0)
	{
		rs485handle = rk_sdk_rs485_connect(g_rkhandle,RS485_ROAD_1,NULL);
		if(rs485handle == 0)
		{
			log_write("rs485 connect faile ret = %d",rs485handle);
			sleep(1);
		}
			
	
	}
	return 0;
}

/*道闸开闸*/
void barrier_gate_open()
{
	int ret = 0;
	RK_Gpio gpio_status;
	memset(&gpio_status,-1,sizeof(RK_Gpio));
	
	gpio_status.out1 = 1;
	gpio_status.auto_out1 = 1;

	if(g_rkhandle == 0)
	{
		log_write("barrier_gate_open faile,rk sdk is not connect");
		return ;
	}
	ret = rk_sdk_config_gpio_out_set(g_rkhandle, &gpio_status);
	if(ret < 0)
		log_write("barrier gate open faile,gpio set error");
	else
		log_write("barrier gate open success");

	return;
}


/*道闸关闭*/
void barrier_gate_close()
{
	int ret = 0;
	RK_Gpio gpio_status;
	memset(&gpio_status,-1,sizeof(RK_Gpio));
	
	gpio_status.out2 = 1;
	gpio_status.auto_out2 = 1;

	if(g_rkhandle == 0)
	{
		log_write("barrier_gate_close faile,rk sdk is not connect");
		return ;
	}
	ret = rk_sdk_config_gpio_out_set(g_rkhandle, &gpio_status);
	if(ret < 0)
		log_write("barrier gate close faile,gpio set error");
	else
		log_write("barrier gate close success");

	return;
}

/*gpio输出*/
void gpio_out_set(unsigned char gpio_num,unsigned char out_state)
{
	int ret = 0;
	RK_Gpio gpio_status;
	memset(&gpio_status,-1,sizeof(RK_Gpio));
	
	switch(gpio_num)
	{
		case 1:
			gpio_status.out1 = out_state;
			break;
		case 2:
			gpio_status.out2 = out_state;
			break;
		case 3:
			gpio_status.out3 = out_state;
			break;
		default:
			break;
	}

	if(g_rkhandle == 0)
	{
		log_write("gpio_out set faile ,rk sdk is not connect");
		return ;
	}
	ret = rk_sdk_config_gpio_out_set(g_rkhandle, &gpio_status);
	if(ret < 0)
		log_write("gpio set %d:%d faile",gpio_num,out_state);
	else
		log_write("gpio set %d:%d success",gpio_num,out_state);

	return;
}
	
/*
函数名称：rs485_transparent
函数功能:
	RS485透传数据
参数:
	pdata[in]:数据缓冲区地址
	size[in]:数据大小
返回值：
	null
*/

void rs485_transparent( unsigned char *pdata, int size)
{
	int ret = 0;	
	if(!rs485handle)
	{
		log_write("rs485_transparent faile rs485 is not connected");
		return ;
	}
	if(!pdata || size <= 0)
	{
		log_write("rs485_transparent faile paramter is invalid!");
		return ;
	}
	ret = rk_sdk_rs485_SerialSend(rs485handle,pdata,size);		
	if(ret < 0)
		log_write("rs485 transparent faile");
	else
		log_write("rs485 transparent success");
	return;
}
/*
模拟触发
*/
void triger_send()
{
	int ret = 0;
	if(g_rkhandle == 0)
	{
		log_write("triger send faile,rk sdk is not connect");
		return ;
	}
	ret = rk_sdk_config_trigger(g_rkhandle);

	if(ret < 0)
		log_write("triger send success");
	return ;
}

/*
函数名称:device_info_get
函数功能:
	从相机程序中获取设备信息
参数:
	deviceInfo[out]:设备信息结构题指针
返回值:
	null
*/
void device_info_get(RK_DeviceInfo *deviceInfo)
{
	log_write("get device info from camera");
	if(g_rkhandle <= 0)
	{
		log_write("device_info_get::rk sdk is not connect");
		return ;
	}
	if(!deviceInfo)
	{
		log_write(" device_info_get::Parameter is invalid!");
		return ;
	}
	memset(deviceInfo,0,sizeof(RK_DeviceInfo));
	rk_sdk_config_devinfo_get(g_rkhandle,deviceInfo);
	return ;
}
/*
函数名称:http_config_get
函数功能：
	从相机内部获取http的配置参数
参数：
	httpinfo[out]:http的参数结构指针
返回值：
	null
*/
void http_config_get(http_param_s *http_info)
{

	log_write("get http config info from camera");
	if(g_rkhandle <= 0)
	{
		log_write("http_config_get()::rk sdk is not connect");
		return;
	}
	if(!http_info)
	{
		log_write(" http_config_get()::Parameter is invalid!");
		return;
	}
	//memset(http_info,0,sizeof(RK_HttpParam));
	//rk_sdk_config_http_get(g_rkhandle,http_info);
	memset(http_info,0,sizeof(http_param_s));
	//test
	http_info->is_ssl_connect = 0;
	http_info->characters_type = 0;
	http_info->offline_continuingly_en = 0;
	http_info->http_control_enable =1;
	http_info->main_server.enable =1;
	http_info->main_server.full_image_report_enable = 1;
	http_info->main_server.plate_image_report_enable = 1;
	http_info->push_num = 3;
	http_info->main_server.io_report_enable = 1;
	http_info->main_server.keepalive_enable = 0;
	http_info->main_server.keeplive_interval = 5;
	//strcpy(http_info->main_server.url_string,"allinaent.e1.luyouxia.net:29961/camera/lprcresult");
	strcpy(http_info->main_server.url_string,"172.16.10.88:8080/test1");
	//strcpy(http_info->main_server.url_string,"101.200.182.238:80/cgi-bin/HTTP_Test1.cgi");
	return ;
}
