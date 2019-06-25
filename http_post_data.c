#include <stdlib.h>
#include <string.h>

#include "include/http_post_data.h"

size_t write_data(void *ptr, size_t size, size_t nmemb, void *userptr) 
{
   
	char *p1 = (char *)ptr;
	
	http_recive_data_s *p = (http_recive_data_s *)userptr;
	if(nmemb*size < (HTTP_RECIVE_MAX_SIZE - p->datalen))  
	{		
		memcpy(&(p->data[p->datalen]),(unsigned char*)ptr,nmemb*size);
		p->datalen += nmemb*size;
	}

	return size*nmemb;
}

CURL * http_init()
{
	CURL *curl = NULL;
	curl = curl_easy_init();
	return curl;
}
void http_clean(CURL *curl)
{
	if(curl)
	{
		 curl_easy_cleanup(curl);
		 curl = NULL;
	}
	 
}

int http_post(CURL *curl,char* url_str,char*data,http_recive_data_s *recivedata,int session_timeout,int characters_type)
{
	 CURLcode res;
	 char head_str[256] = {0};
 	 struct curl_slist* headers = NULL;
	 if(!curl || !url_str || !data || !recivedata || session_timeout < 0)
	 {	
		return -1;
	 }
		
	memset(recivedata,0,sizeof(http_recive_data_s));
	

	if(characters_type == CHARCTER_TYPE_GB2312)
	{
		sprintf(head_str,"Content-Type:text/plain;charset=utf-8");
	}else if (characters_type == CHARCTER_TYPE_UTF8)
	{
		sprintf(head_str,"Content-Type:text/plain;charset=gb2312");
	}else
	{
		sprintf(head_str,"Content-Type:text/plain");
	}
	
	headers=curl_slist_append(headers, head_str);
	curl_easy_setopt( curl,CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt( curl, CURLOPT_TIMEOUT,session_timeout);//接收数据时超时设置，如果n秒内数据未接收完，直接退出
	//curl_easy_setopt( curl, CURLOPT_VERBOSE, 1L ); //在屏幕打印请求连接过程和返回http数据
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);//匿名ssl
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
   
	curl_easy_setopt(curl, CURLOPT_POST, 1);//设置为非0表示本次操作为POST
   	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L); //开启长连接标志
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 30L);
	//设置url
	curl_easy_setopt(curl, CURLOPT_URL, url_str);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)recivedata);//设置写数据

	// 设置接收数据的处理函数和存放变量
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//设置回调函数
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,data);
	res = curl_easy_perform(curl);
	
	curl_slist_free_all(headers); /* free the list again */
	headers = NULL;	
	return res;
	
}
