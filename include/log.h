#ifndef __LOG_H__
#define __LOG_H__

/*
函数名称:log_init#endif
函数功能:
	日志初始化
返回值：
	0成功
	-1失败
*/
int log_init(void);
/*
函数名称:
	log_exit
函数功能:
	日志退出
返回值:
	无
*/
void log_exit(void);
/*
函数名称：
	log_write
函数功能：
	写日志
返回值：
	0 成功
	-1 失败
*/

int log_write(const char *pFormat, ...);
#endif
