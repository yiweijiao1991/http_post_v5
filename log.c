#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/time.h>

#define LOG_OPEN	1
#define  LOG_FILE    "/root/log/http_server/"


static pthread_rwlock_t rwlock;
static int  rwlock_initflag = 0;

/*
函数名称:log_init
函数功能:
	日志初始化
返回值：
	0成功
	-1失败
*/
int log_init(void)
{
	int ret;

	ret = pthread_rwlock_init(&rwlock, NULL);
	if(ret < 0)
	{
		rwlock_initflag = 0;
		return -1;
	}
	rwlock_initflag = 1;

	return 0;
}
/*
函数名称:
	log_exit
函数功能:
	日志退出
返回值:
	无
*/
void log_exit(void)
{
	if(rwlock_initflag)
	{
		pthread_rwlock_destroy(&rwlock);
	}
}

/*
函数名称：
	get_time_string
函数功能：
	获取当前时间的字符串
返回值：
	当前时间的字符串
*/
static char *get_time_string(void)   
{   
	time_t tt;
	struct tm *t;
	static char timebuf[64];

	time(&tt);
	t = localtime(&tt); 
	sprintf(&timebuf[0],"%04d-%02d-%02d %02d:%02d:%02d ",
			t->tm_year + 1900, t->tm_mon+1, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec);

	return  &timebuf[0];  
}



static int createDir(char *fileName)
{
	if((fileName == NULL) || (strlen(fileName) == 0)){
		return -1;
	}
	char	*tag = NULL;
	char	buf[1000],path[1000];
	int	flag = 0;
	int	ret;
	
	for(tag=fileName;*tag;tag++)
	{
		if (*tag=='/')
		{
			memset(buf, 0, 1000);
			memset(path, 0, 1000);
			strcpy(buf,fileName);
			buf[strlen(fileName)-strlen(tag)+1]='\0';
			strcpy(path,buf);
			if (access(path,0)==-1)
			{
				ret = mkdir(path, S_IRWXU);
				if(ret < 0)
				{
					return -1;
				}else{
					flag = 1;
				}
			}
		}
	}
	if(flag == 1){
		return 0;
	}else{
		return 1;
	}
}

/*
函数名称：
	log_write
函数功能：
	写日志
返回值：
	0 成功
	-1 失败
*/

int log_write(const char *pFormat, ...)
{	

	int 		ret;
	int 		fd;
	struct stat 	sgbuf;
	char 		*timestr;
	char 		log_file_name[100] = {0};
	char		loginfo[1024] = {0};
	va_list		args;
	time_t tt;
	struct tm *t;


	va_start(args, pFormat);
	vsprintf(loginfo,pFormat,args);
	va_end(args);

	printf("%s\n", loginfo);
#if LOG_OPEN
	pthread_rwlock_wrlock(&rwlock);
	time(&tt);
	t = localtime(&tt); 
	sprintf(log_file_name,"%s%04d-%02d-%02d.txt",LOG_FILE,t->tm_year + 1900, t->tm_mon+1, t->tm_mday);
	// 自动创建目录
	if(createDir(log_file_name) != -1)
	{
		fd = open(log_file_name, O_WRONLY | O_CREAT | O_APPEND, 644);
		if(fd < 0)
		{
			pthread_rwlock_unlock(&rwlock);
			return -1;
		}
		timestr = get_time_string();
		ret = write(fd, timestr, strlen(timestr));
		ret = write(fd, loginfo, strlen(loginfo));
		ret = write(fd, "\r\n", strlen("\r\n"));
		close(fd);
	}
	pthread_rwlock_unlock(&rwlock);
#endif
	return 0;	
}
