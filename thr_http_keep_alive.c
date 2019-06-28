#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "thr_http_keep_alive.h"
#include "http_post_data.h"
#include "log.h"
#include "http_response_handle.h"
#include "generat_http_data.h"
#include "http_response_affirm.h"

//extern RK_HttpParam g_http_cfg;//HTTP配置信息
extern http_param_s g_http_cfg;//HTTP配置信息

extern RK_DeviceInfo g_deviceInfo;//设备信息
extern int g_thread_quit;//线程退出标志 0 不退出 1 退出

/*

函数说明:
	发送心跳并处理回应
参数:
	http_recive_data[in]:接收回应结构体
	http_send_buff[in]:发送缓冲结构体
	http_curl_handle[in]:http发送模块句柄
	http_config[in]:配置参数
返回值:
	无

*/

void http_keep_alive_handle(http_recive_data_s *http_recive_data,
					 		http_send_buff_t    *http_send_buff,
					 		CURL 			*http_curl_handle,
		  				 	http_param_s    http_config)
{
	int ret = 0;
	int res;//http请求结果
	int  barrier_control_result  = 0;//道闸控制结果
	char url_str[1024] = {0};//url字符串

	//是否使用ssl 生成http url
	if(http_config.is_ssl_connect)
		sprintf(url_str,"https://%s",http_config.main_server.url_string);
	else
		sprintf(url_str,"http://%s",http_config.main_server.url_string);

	log_write("http keepalive ready send request to %s",url_str);
	//获取json数据
	memset(http_send_buff,0,sizeof(http_send_buff));
	ret = get_keepalive_json(&g_deviceInfo,
							(char *)http_send_buff->data_buff,
							HTTP_SEND_BUFF_MAX_SIZE);
	if(ret == 0)
	{
		//http请求
		http_send_buff->data_len = strlen((char *)http_send_buff->data_buff);
		memset(http_recive_data,0,sizeof(http_recive_data_s));
		res = http_post(http_curl_handle,
						url_str,
						(char *)http_send_buff->data_buff,
						http_recive_data,
						http_config.session_timeout,
						http_config.characters_type);
		
		if(res == CURLE_OK)
		{
			//处理回应数据
			log_write("http keepalive request success,url is %s",url_str);
			if(http_config.http_control_enable)
			{
				if(http_recive_data->datalen > 0)
				{
					ret = response_handle(http_recive_data->data,&barrier_control_result,http_config.rs485_delay);
					if(ret == 0)
					{
						//成功
						log_write("http keepalive response handle success,url is %s",url_str);
					}else
					{
						//数据处理失败
						if(ret == -1)
							log_write("http keepalive response handle faile,paramer invalid,url is %s",url_str);
						else if(ret == -2)
							log_write("http keepalive response handle faile,get json struct error,url is %s",url_str);
						else if(ret == -3)
							log_write("http keepalive response handle faile,get response section in json error,url is %s",url_str);
						else
							log_write("http keepalive response handle faile,nknown error,url is %s",url_str);
					}
					//发送确认包
					if(http_config.response_affirm_enable)
					{
						if(ret == 0)
						{
							http_send_response_affirm(http_recive_data,
													  http_send_buff,
													  http_curl_handle,
													  http_config,
													  url_str,
													  RESPONSE_SUCESS_AFFIRM);
						}else
						{
							http_send_response_affirm(http_recive_data,
													 http_send_buff,
													 http_curl_handle,
													 http_config,
													 url_str,
													 RESPONSE_FAILE_AFFIRM);
						}
					}

				}else
				{
					//接收数据为0
					log_write("http keepalive get response length is 0,so no operation,url is %s",url_str);
				}
			}
					
		}else
		{
			//发送失败
			log_write("http keepalive request faile ,ret = %d,url is %s",res,url_str);
		}
	}else
	{
		//获取json失败
		if(ret == -1)
			log_write("http keepalive request faile ,generate json string faile,paramer invalid,url is %s ret = %d",url_str,ret);
		else if(ret == -2)
			log_write("http keepalive request faile ,generate json string faile,create json section faile,url is %s ret = %d",url_str,ret);
		else if(ret == -3)
			log_write("http keepalive request faile ,generate json string faile,get string error,url is %s ret = %d",url_str,ret);
		else if(ret == -4)
				log_write("http keepalive request faile ,generate json string faile,memory too small,url is %s ret = %d",url_str,ret);
		else
				log_write("http keepalive request faile ,generate json string faile,unknown error ,url is %s ret = %d",url_str,ret);
		
	}
		
}

/*
函数名称:Thr_httpKeepAlive
函数功能:
	http推送心跳功能线程函数
返回值：
	线程状态
参数
	null
*/
void *thr_http_keep_alive(void *arg)
{
	void *status = NULL;
	
	time_t nowtime;		//当前时间
	time_t last_keepalive;//用于发送心跳的上次时间

	http_recive_data_s *http_recive_data = NULL;//接收结构体指针
	CURL 			   *http_curl_handle = NULL;//http请求句柄
	http_send_buff_t * http_send_buff = NULL;	//发送缓冲区结构体指针

	log_write("************ http keepalive thread start *****************");
	
	//开辟空间
	http_recive_data = (http_recive_data_s *)malloc(sizeof(http_recive_data_s));
	http_send_buff = (http_send_buff_t *)malloc(sizeof(http_send_buff_t));
	if( !http_recive_data || !http_send_buff)
	{
		log_write("http keepalive malloc resources error");
		goto http_keepalive_out;
	}
		
	//初始化http请求模块
	http_curl_handle = http_init();
	if(!http_curl_handle)
	{
		log_write("http keepalive create curl handle faile");
		goto http_keepalive_out;
	}

	last_keepalive = time(NULL);


	while(!g_thread_quit)
	{
		nowtime = time(NULL);
		//发送心跳首先需要启动主服务器 其次需要启动发送心跳
		if((g_http_cfg.main_server.enable == 1) && (g_http_cfg.main_server.keepalive_enable == 1))
		{
			if((nowtime - last_keepalive) >= g_http_cfg.main_server.keeplive_interval || (nowtime - last_keepalive) < 0)
			{
				 //进行心跳请求
				 http_keep_alive_handle(http_recive_data,http_send_buff,http_curl_handle,g_http_cfg);
				 last_keepalive = nowtime;
			}
		}
		usleep(500000);

	}


http_keepalive_out:
	
	//释放资源
	if(http_recive_data)
	{
		free(http_recive_data);
		http_recive_data = NULL;
	}
	if(http_send_buff)
	{
		free(http_send_buff); 
		http_send_buff = NULL;
	}
	if(http_curl_handle)
	{
		http_clean(http_curl_handle);
		http_curl_handle = NULL;

	}


	return  status;
}


