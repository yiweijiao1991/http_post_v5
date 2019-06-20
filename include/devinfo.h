#ifndef  __DEV_INFO_H__
#define  __DEV_INFO_H__

#ifdef WIN32
	#include <windows.h>
#else
	#include<time.h>
	#include<sys/time.h>
#endif

#include "mutex.h"
#include "gbl.h"

// 心跳周期结构体
typedef struct _keeplive_cycle{
#ifdef WIN32
	SYSTEMTIME		time;					// 保存上一次心跳的发送时间
#else
	struct timeval	time;
#endif
}_keeplive_cycle;

typedef struct _device_info{
	int				sockfd;
	char			ip[16];
	int				port;
	int				re_conn;			// 是否需要重连。0：不重连；1：重连
	void			*usePtr;
	int				abnormal_number;	// 心跳异常次数
	_keeplive_cycle	last_refresh_time;	// 心跳周期
	int				is_delay;			// 当前心跳发送数据延时到下一次发送，0：不延时；1：延时
	unsigned int	handle;
	unsigned int	serial_handle;
	int				serial_port;		//指定使用设备的串口序号：0:表示第一个串口，1:表示第二个串口

	mutex_t			*mutex;				/* Mutex to protect the global data */
}_device_info;

typedef struct device_info{
	_device_info	devinfo[MAX_CONNECT_DEVICE_NUM];
	int				thread_exit;		// 0: 线程没有退出，1：线程已经退出
}device_info;

extern device_info devinfo;
extern device_info reConnectDevinfo;
extern device_info recoResultDevinfo;
extern device_info reConnectRecoResultDevinfo;
extern device_info rs485Devinfo;
extern device_info reConnectRs485Devinfo;

extern int set_cur_time(_keeplive_cycle *prev_time);
extern int get_cur_time(_keeplive_cycle *prev_time);
/* 
	返回值：
		-1：错误；
		0：没有超过间隔时间；
		1：超过间隔时间
*/
extern int calculate_keeplive_interval(_keeplive_cycle *prev_time, int interval_time);

#endif