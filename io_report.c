#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "generat_http_data.h"
#include "io_report.h"
#include "cameraInterface.h"
#include "http_response_handle.h"
#include "log.h"
#include "http_response_affirm.h"

extern RK_DeviceInfo g_deviceInfo;
extern http_param_s g_http_cfg;//HTTP配置信息
extern io_report_ptr_s g_gpio_report_mem_ptr;//gpio 内存指针


/*
IO上报初始化
说明:
分配所需要的资源
参数:
	io_report_ptr[in/out]:参数指针
返回值: 
	0成功 -1失败
*/

int  io_report_init(io_report_ptr_s *io_report_ptr)
{

	if(!io_report_ptr)
		return -1;
	//清空
	memset(io_report_ptr,0,sizeof(io_report_ptr_s));

	//开辟接收回应数据结构内存
	if(!io_report_ptr->http_recive_data)
		io_report_ptr->http_recive_data = (http_recive_data_s *)malloc(sizeof(http_recive_data_s));
	//开辟发送缓冲
	if(!io_report_ptr->http_send_buff)
		io_report_ptr->http_send_buff = (http_send_buff_t *)malloc(sizeof(http_send_buff_t));
	if( !io_report_ptr->http_recive_data || !io_report_ptr->http_send_buff)
		goto error_out;
	//初始化发送模块
	if(!io_report_ptr->http_curl_handle)
		io_report_ptr->http_curl_handle = http_init();
	
	if(!io_report_ptr->http_curl_handle)
		goto error_out;

	return 0;
	
error_out:
	//释放
	io_report_free(io_report_ptr);

	return -1;

}
/*
函数说明:
	释放资源
参数:
	io_report_ptr[in/out]:参数指针

*/
void io_report_free(io_report_ptr_s *io_report_ptr)
{
	if(io_report_ptr->http_recive_data)
	{
		free(io_report_ptr->http_recive_data);
		io_report_ptr->http_recive_data = NULL;
	}
	if(io_report_ptr->http_send_buff)
	{
		free(io_report_ptr->http_send_buff);
		io_report_ptr->http_send_buff = NULL;
	}
	if(io_report_ptr->http_curl_handle)
	{
		http_clean(io_report_ptr->http_curl_handle);
		io_report_ptr->http_curl_handle = NULL;

	}
}

/*
函数说明:
	IO状态上报并处理
参数:
	io_report_ptr[in]:存储参数指针
	io_state[in]:输入IO的状态
	http_config[in]:配置参数
返回值:
	无
*/
void io_report(io_report_ptr_s *io_report_ptr,RK_Gpio * io_state,http_param_s    http_config)
{
	int ret = 0;
	int res;
	int  barrier_control_result  = 0;
	char url_str[1024] = {0};
	
	if(!io_report_ptr->http_curl_handle || !io_report_ptr->http_recive_data || !io_report_ptr->http_send_buff)
			return ;

	log_write("get io state from camera,in1:%d in2:%d in3:%d in4:%d userkey:%d",io_state->in1,
																	 io_state->in2,
																	 io_state->in3,
																	 io_state->in4,
																	 io_state->userkey);
	//判断是否使用ssl
	if(http_config.is_ssl_connect)
		sprintf(url_str,"https://%s",http_config.main_server.url_string);
	else
		sprintf(url_str,"http://%s",http_config.main_server.url_string);

	log_write("gpio report ready send gpio info to %s",url_str);
	//组织json数据
	memset(io_report_ptr->http_send_buff,0,sizeof(io_report_ptr->http_send_buff));
	ret = get_io_state_json(&g_deviceInfo,
							io_state,
							(char *)io_report_ptr->http_send_buff->data_buff,
							HTTP_SEND_BUFF_MAX_SIZE);
	
	if(ret == 0)
	{
		//发送请求
		io_report_ptr->http_send_buff->data_len = strlen((char *)io_report_ptr->http_send_buff->data_buff);
		memset(io_report_ptr->http_recive_data,0,sizeof(http_recive_data_s));
		res = http_post(io_report_ptr->http_curl_handle,
						url_str,
						(char *)io_report_ptr->http_send_buff->data_buff,
						io_report_ptr->http_recive_data,
						http_config.session_timeout,
						http_config.characters_type);
		if(res == CURLE_OK)
		{
			log_write("gpio report send gpio info to %s success",url_str);
			//处理回应数据
			if(http_config.http_control_enable)
			{
				if(io_report_ptr->http_recive_data->datalen > 0)
				{
					log_write("gpio report to  %s ready handle response",url_str);

					ret = response_handle(io_report_ptr->http_recive_data->data,
										  &barrier_control_result,http_config.rs485_delay);
					if(ret == 0)
					{
						//成功
						log_write("gpio report to  %s handle response success",url_str);
					}else
					{
						//数据处理失败
						if(ret == -1)
						{
							log_write("gpio report response handle faile,paramer invalid,url is %s",url_str);
						}else if(ret == -2)
						{
							log_write("gpio report response handle faile,get json struct error,url is %s",url_str);
						}else if(ret == -3)
						{
							log_write("gpio report response handle faile,get response section in json error,url is %s",url_str);
						}else
						{
							log_write("gpio report response handle faile,unknown error,url is %s",url_str);
						}
					}
							//发送确认包
					if(http_config.response_affirm_enable)
					{
						if(ret == 0)
						{
							http_send_response_affirm(io_report_ptr->http_recive_data,
													 io_report_ptr->http_send_buff,
													 io_report_ptr->http_curl_handle,
													 http_config,
													 url_str,
													 RESPONSE_SUCESS_AFFIRM);
						}else
						{
							http_send_response_affirm(io_report_ptr->http_recive_data,
													 io_report_ptr->http_send_buff,
													 io_report_ptr->http_curl_handle,
													 http_config,
													 url_str,
													 RESPONSE_FAILE_AFFIRM);
						}
					}
				}else
				{
					//数据大小为0
					log_write("gpio report to  %s get response length is 0,so no operation",url_str);
				}
			}
			
		}else
		{
			//发送失败
			log_write("gpio report request faile ,ret = %d,url is %s",res,url_str);
		}
	}else
	{
		//获取json失败
		if(ret == -1)
			log_write("gpio report request faile , generate json string faile,paramer invalid,url is %s ret = %d",url_str,ret);
		else if(ret == -2)
			log_write("gpio report request faile ,generate json string faile,create json section faile,url is %s ret = %d",url_str,ret);
		else if(ret == -3)
			log_write("gpio report request faile ,generate json string faile,get string error,url is %s ret = %d",url_str,ret);
		else if(ret == -4)
			log_write("gpio report request faile ,generate json string faile,memory too small,url is %s ret = %d",url_str,ret);
		else
			log_write("gpio report request faile ,generate json string faile,unknown error ,url is %s ret = %d",url_str,ret);
	}
}

void  io_report_callback (RkSdkHandle handle, RK_Gpio *gpio, void *usePtr)
{
	//判断是否由HTTP服务打开
	if(g_http_cfg.main_server.enable && g_http_cfg.main_server.io_report_enable)
		io_report(&g_gpio_report_mem_ptr,gpio,g_http_cfg);
	return ;
}


