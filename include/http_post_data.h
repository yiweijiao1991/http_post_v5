#ifndef __HTTP_POST_DATA_H__
#define __HTTP_POST_DATA_H__
#include "curl/curl.h"

#define CHARCTER_TYPE_GB2312 	0		//汉字编码GB2312
#define CHARCTER_TYPE_UTF8		1		//汉字编码UTF8

#define HTTP_SEND_BUFF_MAX_SIZE 1024*1024*2//发送缓冲区大小


//图片暂存结构体
typedef struct http_image_t{
	int full_image_len;	   //大图的长度
	unsigned char full_image_buff[1024*1024];//大图的数据
	int plate_image_len;	   //小图的长度
	unsigned char plate_image_buff[1024*500];//小图的数据
}http_image_t;
//数据发送缓冲区
typedef struct http_send_buff_t{
	unsigned char data_buff[HTTP_SEND_BUFF_MAX_SIZE];//数据缓冲区
	int data_len;	   //数据大小
}http_send_buff_t;


#define HTTP_RECIVE_MAX_SIZE 1024*1024*5 ///接受缓冲区大小
/*接收回应结构体 */
typedef struct http_recive_data_s{
	int datalen;
	unsigned char data[HTTP_RECIVE_MAX_SIZE];	
}http_recive_data_s;

/*
 *@function name: 
	http_init
 *@Author: yiweijiao
 *@Date: 2019-06-28 16:58:13
 *@describtion: 
	初始化curl
 *@parameter: 
	无
 *@return: 
	curl操作句柄
	null失败
*/
CURL * http_init();

/*
 *@function name: 
	http_clean
 *@Author: yiweijiao
 *@Date: 2019-06-28 16:59:02
 *@describtion: 
	关闭curl并释放资源
 *@parameter: 
	curl[in]:curl句柄
 *@return: 
*/
void http_clean(CURL *curl);

/*
 *@function name: 
	http_post
 *@Author: yiweijiao
 *@Date: 2019-06-28 16:58:52
 *@describtion: 
	推送http post请求 并获取回应信息（连接服务器超时时间为5秒回话时间用户自己设定）
 *@parameter: 
	curl[in]:curl句柄
	url_str[in]:url
	data[in]:要发送的数据
	recivedata[in]:接受数据结构体
	session_timeout[in]:连接成功之后的回话时间
	characters_type[in]:字符编码 0 GB2312  1UTF-8
 *@return: 
	-1参数错误
	0成功
	>0错误代码
*/
int http_post(CURL *curl,char* url_str,char*data,http_recive_data_s *recivedata,int session_timeout,int characters_type);

#endif

