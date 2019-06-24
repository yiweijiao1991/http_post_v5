#include <stdio.h>
#include <signal.h>
#include "RK_LPRClientSDK.h"
#include "devinfo.h"
#include "cameraInterface.h"
#include "thr_http_keep_alive.h"
#include "thr_http_report.h"
#include "io_report.h"
#include "pthread.h"
#include "log.h"

#define _VERSION_ "3.0"


http_param_s g_http_cfg;//HTTP配置信息
RK_DeviceInfo g_deviceInfo;
int g_thread_quit = 0;//线程退出标志 0 不退出 1 退出
io_report_ptr_s g_gpio_report_mem_ptr;;

void config_signal_deal(int signal_num)
{
	log_write("recive config signal");
	//HTTP配置信息读取
	http_config_get(&g_http_cfg);
	//设备信息读取
	device_info_get(&g_deviceInfo);	
	return;
}

int main()
{
	int ret = 0;
	pthread_t thr_http_keepalive_pid;
	pthread_t thr_http_report_pid;
	void *status;
	
	sleep(3);
	ret = log_init();
	if(ret == -1)
	{
		printf("log init faile");
		return -1;
	}
	log_write("********http_proc(%s) start*********",_VERSION_);



	ret = io_report_init(&g_gpio_report_mem_ptr);
	if(ret != 0)
	{
		log_write("rk sdk gpio report init  faile ret = %d",ret);
		return -1;
	}

	ret = init_interface();
	if(ret < 0)
	{
		log_write("init camera interface faile");
		g_thread_quit = 1;
		return -1;
	}
	
	//HTTP配置信息读取
	http_config_get(&g_http_cfg);
	//设备信息读取
	device_info_get(&g_deviceInfo);
	//注册信号
	signal(SIGUSR1,config_signal_deal);
	
	//启动心跳
	ret = pthread_create(&thr_http_keepalive_pid,NULL,thr_http_keep_alive,NULL);
	if(ret !=0 )
	{
		log_write("http keepalive thread create faile ret = %d",ret);
		g_thread_quit = 1;
		return -1;
	}
	//启动推送
	ret = pthread_create(&thr_http_report_pid,NULL,thr_http_report,NULL);
	if(ret != 0)
	{
		log_write("http report thread create faile ret = %d",ret);
		pthread_join(thr_http_keepalive_pid,&status);
		g_thread_quit = 1;
		return -1;
	}
	//join
	
	pthread_join(thr_http_keepalive_pid,&status);
	pthread_join(thr_http_report_pid,&status);

	log_write("exit");
	log_exit();
	io_report_free(&g_gpio_report_mem_ptr);
	return 0;
}
