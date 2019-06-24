#ifndef __IO_REPORT_H__
#define __IO_REPORT_H__


#include "http_post_data.h"
#include "RK_LPRClientSDK.h"

/*
	IO 上报指针结果体
*/

typedef struct io_report_ptr_s{
	
	 http_recive_data_s *http_recive_data;
	 CURL 			    *http_curl_handle ;
	 http_send_buff_t   *http_send_buff;	

}io_report_ptr_s;


/*
IO上报初始化
说明:
分配所需要的资源
参数:
	io_report_ptr[in/out]:参数指针
返回值: 
	0成功 -1失败
*/



int  io_report_init(io_report_ptr_s *io_report_ptr);
/*
函数说明:
	释放资源
参数:
	io_report_ptr[in/out]:参数指针

*/
void io_report_free(io_report_ptr_s *io_report_ptr);

void  io_report_callback (RkSdkHandle handle, RK_Gpio *gpio, void *usePtr);

#endif
