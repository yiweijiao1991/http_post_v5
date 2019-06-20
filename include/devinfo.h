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

// �������ڽṹ��
typedef struct _keeplive_cycle{
#ifdef WIN32
	SYSTEMTIME		time;					// ������һ�������ķ���ʱ��
#else
	struct timeval	time;
#endif
}_keeplive_cycle;

typedef struct _device_info{
	int				sockfd;
	char			ip[16];
	int				port;
	int				re_conn;			// �Ƿ���Ҫ������0����������1������
	void			*usePtr;
	int				abnormal_number;	// �����쳣����
	_keeplive_cycle	last_refresh_time;	// ��������
	int				is_delay;			// ��ǰ��������������ʱ����һ�η��ͣ�0������ʱ��1����ʱ
	unsigned int	handle;
	unsigned int	serial_handle;
	int				serial_port;		//ָ��ʹ���豸�Ĵ�����ţ�0:��ʾ��һ�����ڣ�1:��ʾ�ڶ�������

	mutex_t			*mutex;				/* Mutex to protect the global data */
}_device_info;

typedef struct device_info{
	_device_info	devinfo[MAX_CONNECT_DEVICE_NUM];
	int				thread_exit;		// 0: �߳�û���˳���1���߳��Ѿ��˳�
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
	����ֵ��
		-1������
		0��û�г������ʱ�䣻
		1���������ʱ��
*/
extern int calculate_keeplive_interval(_keeplive_cycle *prev_time, int interval_time);

#endif