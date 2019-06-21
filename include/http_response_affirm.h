#ifndef __HTTP_RESPONSE_AFFIRM_H__
#define __HTTP_RESPONSE_AFFIRM_H__
#include "http_post_data.h"
#include "generat_http_data.h"

#define RESPONSE_SUCESS_AFFIRM  1       //HTTP回应数据正确标识
#define RESPONSE_FAILE_AFFIRM   0       //HTTP回应数据错误标识

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
                            int status);
#endif