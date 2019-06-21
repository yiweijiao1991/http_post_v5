#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "thr_http_keep_alive.h"
#include "log.h"
#include"http_response_affirm.h"

extern RK_DeviceInfo g_deviceInfo;//设备信息
/*
 *@function name: 
    http_send_response_affirm
 *@Author: yiweijiao
 *@Date: 2019-06-21 10:40:02
 *@describtion: 
    发送http回应确认包
 *@parameter: 
    http_recive_data[in]:接收缓冲区
    http_send_buff[in]:发送缓冲区
    http_curl_handle[in]:http发送句柄
    url[in]:http的url
    status:状态
 *@return: 
    无
*/

void http_send_response_affirm(http_recive_data_s *http_recive_data,
					 		http_send_buff_t    *http_send_buff,
					 		CURL 			*http_curl_handle,
		  				 	http_param_s    http_config,
                            char * url,
                            int status)
{
    int ret = 0;
    int res;
    if(!http_recive_data || !http_send_buff || !http_curl_handle || !url)
    {
         log_write("send http response affirm package faile,invalid paramer");
         return;
    }
    memset(http_send_buff,0,sizeof(http_send_buff));
    //获取json数据
    log_write("ready send http response affirm package to %s",url);
    //组织json数据
    ret = get_response_json(&g_deviceInfo,http_send_buff->data_buff,status,HTTP_SEND_BUFF_MAX_SIZE);
    if(ret == 0)
    {
        http_send_buff->data_len = strlen((char *)http_send_buff->data_buff);
        memset(http_recive_data,0,sizeof(http_recive_data_s));
        //发送请求
		res = http_post(http_curl_handle,
                        url,
                        (char *)http_send_buff->data_buff,
                        http_recive_data,
                        http_config.session_timeout,
                        http_config.characters_type);
        if(res == CURLE_OK)
		{
            //成功
            log_write("http send respond affirm request success,url is %s",url);
        }else
        {
            /*失败*/
              log_write("http send respond affirm request faile,url is %s",url);
        }
    }else 
    {
        //获取json失败
		if(ret == -1)
			log_write("http send respond affirm request faile, \
                    generate json string faile,paramer invalid,url is %s ret = %d",url,ret);
		else if(ret == -2)
			log_write("http send respond affirm request faile ,\
                        generate json string faile,create json section faile,url is %s ret = %d",url,ret);
		else if(ret == -3)
			log_write("http send respond affirm request faile, \
                        generate json string faile,get string error,url is %s ret = %d",url,ret);
		else if(ret == -4)
				log_write("http send respond affirm request faile, \
                            generate json string faile,memory too small,url is %s ret = %d",url,ret);
		else
				log_write("http send respond affirm request faile, \
                            generate json string faile,unknown error ,url is %s ret = %d",url,ret);
    }
    return;
}
