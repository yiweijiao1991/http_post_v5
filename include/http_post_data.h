#ifndef __HTTP_POST_DATA_H__
#define __HTTP_POST_DATA_H__
#include "curl/curl.h"

#define CHARCTER_TYPE_GB2312 	0		//汉字编码GB2312
#define CHARCTER_TYPE_UTF8		1		//汉字编码UTF8

#define HTTP_SEND_BUFF_MAX_SIZE 1024*1024*2


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




#define HTTP_RECIVE_MAX_SIZE 1024*1024*5
typedef struct http_recive_data_s{
	int datalen;
	unsigned char data[HTTP_RECIVE_MAX_SIZE];	
}http_recive_data_s;

CURL * http_init();
void http_clean(CURL *curl);
int http_post(CURL *curl,char* url_str,char*data,http_recive_data_s *recivedata,int session_timeout,int characters_type);

#endif

