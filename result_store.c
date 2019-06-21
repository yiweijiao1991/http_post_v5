#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include <dirent.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/vfs.h>  

#include <sys/stat.h>
#include <sys/types.h>
#include "RK_LPRClientSDK.h"
#include "gbl.h"
#include "cameraInterface.h"
#include "white_list_info.h"
#include "log.h"

#define FLASH_SAVE_IMAGE_MAX 100 //flash中最大存储图片的数量
extern http_param_s g_http_cfg;//HTTP配置信息
/*
函数名称：get_file_count
函数功能：获取目录下文件的数量
参数：
root[in]:路径
返回值 ： 文件的数量

*/
int get_file_count(char *root)
{
	DIR *dir;
	struct dirent * ptr;
	int total = 0;
	createDir(root);
	dir = opendir(root); /* 打开目录*/
	if(dir == NULL)
	{
		printf("fail to open dir %s\n",root);
		return -1;
	}
	while((ptr = readdir(dir)) != NULL)
	{
		//顺序读取每一个目录项；
		//跳过“..”和“.”两个目录
		if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
		{
			continue;
		} 
		total++;	
 	}
	closedir(dir);
	return total;
}

/*
	写文件
*/
static int write_file(char *file_name, unsigned char *image_data, int image_size)
{
	int ret = 0;
	if((file_name == NULL)
		|| (image_data == NULL)
		|| (image_size <= 0))
		return -1;
	
	// 自动创建目录
	if(createDir(file_name) == -1)
	{
		log_write("save image faile ,create dir faile");
		return -1;
	}
	
	// 保存文件
	FILE *file;
	file = fopen(file_name, "wb+");
	if(file == NULL)
	{
		log_write("save image faile , \
				open %s faile datalen = %d errno = %d",file_name,image_size,errno);
		return -1;
	}
		
	ret = fwrite(image_data, image_size, 1, file);
	if(ret <= 0)
	{
		log_write("save image faile ,write %s faile datalen = %d",file_name,image_size);
	}
	fclose(file);
	return 0;
}
/*
函数名称：http_save_image
函数功能：存储图片到Flash或者TF卡
参数 ： result[in] 识别结果结构
	full_image_path[out]:大图路径
	plate_image_path[out]:小图路径
返回值 ：
	0成功
	-1失败
*/
int http_save_image(RK_PlateResult *result,char * full_image_path,char *plate_image_path)
{
	char	save_file_path[256] = {0};//存储路径
	/*用于获取当前时间*/
	time_t tt;
	struct tm *t;
	time(&tt);

	t = localtime(&tt); 
	//获取毫秒
	struct timeval tv;
   	gettimeofday(&tv,NULL);
	struct statfs diskInfo;  //磁盘状态
	unsigned long long blocksize = 0; //块大小
	unsigned long long availableDisk = 0; //可用空间大小

	//判断FLASH中的图片数量是否超过了最大值
	if(get_file_count(FLASH_FULLIMAGE_PATH) < FLASH_SAVE_IMAGE_MAX)
	{
		
		//存储图片到Flash
		if(result->nPlateCount > 0)
		{
			//大图
			if(result->fullImage.imageSize > 0)
			{
		
				memset(save_file_path, 0, sizeof(save_file_path));
				sprintf(save_file_path, "%s%04d%02d%02d%02d%02d%02d%03d_%sFULL.jpg", 
										FLASH_FULLIMAGE_PATH,
										result->strRecTime.nYear, 
										result->strRecTime.nMonth,
										result->strRecTime.nDay, 
										result->strRecTime.nHour,
										result->strRecTime.nMinute,
										result->strRecTime.nSecond,
										tv.tv_usec/1000,
										result->strRecResultItem.license);
				sprintf(full_image_path,"%s",save_file_path);
				write_file(save_file_path,result->fullImage.pBuffer,result->fullImage.imageSize);
				
			}
			//小图
			if(result->plateImage.imageSize > 0)
			{
				memset(save_file_path, 0, sizeof(save_file_path));
				sprintf(save_file_path, "%s%04d%02d%02d%02d%02d%02d%03d_%sPlate.jpg", 
										FLASH_PLATEIMAGE_PATH,
										result->strRecTime.nYear,
										result->strRecTime.nMonth,
										result->strRecTime.nDay, 
										result->strRecTime.nHour,
										result->strRecTime.nMinute,
										result->strRecTime.nSecond,
										tv.tv_usec/1000,
										result->strRecResultItem.license);
				sprintf(plate_image_path,"%s",save_file_path);
				write_file(save_file_path,result->plateImage.pBuffer,result->plateImage.imageSize);
			}
		}else
		{	//没有识别结果仅仅存储大图
			if(result->fullImage.imageSize > 0)
			{
				memset(save_file_path, 0, sizeof(save_file_path));
				sprintf(save_file_path, "%s%04d%02d%02d%02d%02d%02d%03d_NoPlateFULL.jpg", 
										FLASH_FULLIMAGE_PATH,
										t->tm_year + 1900, 
										t->tm_mon+1,
										t->tm_mday,
										t->tm_hour,
										t->tm_min,
										t->tm_sec,
										tv.tv_usec/1000);
				sprintf(full_image_path,"%s",save_file_path);
				write_file(save_file_path,result->fullImage.pBuffer,result->fullImage.imageSize);
			}	
		}
	
		
	}else
	{	//存储图片到TF CARD
		//判断是否插入了TF卡
		if((access(TFCARD_DEVICE,F_OK))==-1)  
		{  
			log_write("save image in tf card faile,No TF Card ,%s not exist",TFCARD_DEVICE);
			return -1;
		}

		//判断剩余空间
		if(-1 == statfs(TFCARD_PATH, &diskInfo))
		{
			log_write("save image in tf card faile,get tf card avaliable size faile");
			return -1;
		}
		blocksize = diskInfo.f_bsize;  //每个block里包含的字节数  
		availableDisk = diskInfo.f_bavail * blocksize;   //可用空间大小  
	
		
		if((availableDisk>>20) < 10)//必须剩下10M 的可用空间
		{
			log_write("save image in tf card faile, TF Card avaliable size is  too small");
			return -1;
		}

		//存储图片到TF卡
		if(result->nPlateCount > 0)
		{
			if(result->fullImage.imageSize > 0)
			{
				memset(save_file_path, 0, sizeof(save_file_path));
				sprintf(save_file_path, "%s%04d%02d%02d%02d%02d%02d%03d_%sFULL.jpg", 
										TF_FULLIMAGE_PATH,
										result->strRecTime.nYear, 
										result->strRecTime.nMonth,
										result->strRecTime.nDay, 
										result->strRecTime.nHour,
										result->strRecTime.nMinute,
										result->strRecTime.nSecond,
										tv.tv_usec/1000,
										result->strRecResultItem.license);
				sprintf(full_image_path,"%s",save_file_path);
				write_file(save_file_path,result->fullImage.pBuffer,result->fullImage.imageSize);
			}
			if(result->plateImage.imageSize > 0)
			{
				memset(save_file_path, 0, sizeof(save_file_path));
				sprintf(save_file_path, "%s%04d%02d%02d%02d%02d%02d%03d_%sPlate.jpg", 
										TF_PLATEIMAGE_PATH,
										result->strRecTime.nYear, 
										result->strRecTime.nMonth,
										result->strRecTime.nDay, 
										result->strRecTime.nHour,
										result->strRecTime.nMinute,
										result->strRecTime.nSecond,
										tv.tv_usec/1000,
										result->strRecResultItem.license);
				sprintf(plate_image_path,"%s",save_file_path);
				write_file(save_file_path,result->plateImage.pBuffer,result->plateImage.imageSize);


			}
		}else
		{
			if(result->fullImage.imageSize > 0)
			{
				memset(save_file_path, 0, sizeof(save_file_path));
				sprintf(save_file_path, "%s%04d%02d%02d%02d%02d%02d%03d_NoPlateFULL.jpg", 
										TF_FULLIMAGE_PATH,
										t->tm_year + 1900, 
										t->tm_mon+1, 
										t->tm_mday,
										t->tm_hour, 
										t->tm_min, 
										t->tm_sec,
										tv.tv_usec/1000);
				sprintf(full_image_path,"%s",save_file_path);
				write_file(save_file_path,result->fullImage.pBuffer,result->fullImage.imageSize);
			}

		}
		
	}

	return 0;
}
/*
save_http_result
函数功能
	1.判断是否获取到数据
	2.存储图片
	3.存储数据到数据库
参数:
	plate_result[in]:识别结果的结构体指针
返回值 null
*/
void save_http_result(RK_PlateResult *plate_result)
{   
	Reco_Result http_reco_result = {0};//数据库存储识别结果的结构

	int ret = 0;
	/*
	用于获取当前时间
	*/
	time_t tt;
	struct tm *t;
	if(plate_result->nPlateCount > 0)
	{
		log_write("http get result form camera plate number is %s",plate_result->strRecResultItem.license);
	}else
	{
		log_write("http get result from camera ,no plate number");
	}
	//存储图片
	http_save_image(plate_result,http_reco_result.FullImagePath,http_reco_result.PlateImagePath);

	//组织数据
	http_reco_result.carBright = plate_result->strRecResultItem.nCarBright;
	http_reco_result.carColor = plate_result->strRecResultItem.nCarColor;
	http_reco_result.colorValue = plate_result->strRecResultItem.nCarType;
	http_reco_result.confidence = plate_result->strRecResultItem.nConfidence;
	http_reco_result.direction = plate_result->strRecResultItem.nDirection;
	strcpy(http_reco_result.license,plate_result->strRecResultItem.license);
	http_reco_result.RECT_bottom = plate_result->strRecResultItem.rcLocation.nBottom;
	http_reco_result.RECT_left = plate_result->strRecResultItem.rcLocation.nLeft;
	http_reco_result.RECT_top =  plate_result->strRecResultItem.rcLocation.nTop;
	http_reco_result.RECT_right =  plate_result->strRecResultItem.rcLocation.nRight;
	http_reco_result.timeUsed = plate_result->strRecResultItem.nTime;
	http_reco_result.triggerType = plate_result->trigger_signal;

	http_reco_result.type = plate_result->strRecResultItem.nType;
	strcpy(http_reco_result.platecolor,plate_result->strRecResultItem.color);
	http_reco_result.carLogo = plate_result->strRecResultItem.nCarLogo;
	http_reco_result.CarType = plate_result->strRecResultItem.nCarType;
	//http_reco_result.plateBright = plate_result->strRecResultItem.nBright;

	if(plate_result->nPlateCount > 0)	
	{
		sprintf(http_reco_result.recotime,"%04d-%02d-%02d %02d:%02d:%02d"
							,plate_result->strRecTime.nYear
							,plate_result->strRecTime.nMonth
							,plate_result->strRecTime.nDay
							,plate_result->strRecTime.nHour
							,plate_result->strRecTime.nMinute
							,plate_result->strRecTime.nSecond);
	}
	else
	{
		time(&tt);
		t = localtime(&tt); 
		sprintf(http_reco_result.recotime,"%04d-%02d-%02d %02d:%02d:%02d"
									,t->tm_year + 1900
									, t->tm_mon+1
									, t->tm_mday
									,t->tm_hour
									, t->tm_min
									, t->tm_sec);
	} 
	
	//白名单
	if(plate_result->nPlateCount > 0)
	{
		
		ret = get_http_white_list_info(&http_reco_result);
		if(ret < 0)
		{
			log_write("http get white list info error ret = %d",ret);
		}
	}
	ret = YBDB_Reco_Result_Insert(LOCAL_SERVER_IP, &http_reco_result);

	//存储数据倒数据库
	if( ret != 0)
	{
		log_write("http result store into database error ret = %d",ret);
	}
	
	return;
}
/*
函数名称:
	result_store_callback
函数功能
	接收到识别结果的回调函数
	将识别结果整理后存储到http的数据苦中
参数
	handle[in]:SDK的句柄
	plate_result[in]:识别结果结构体
	usePtr[in]:用户自用指针
返回值
	null
*/
void  result_store_callback (RkSdkHandle handle, RK_PlateResult *plate_result, void *use_ptr)
{
	//判断是否由HTTP服务打开
	if(g_http_cfg.main_server.enable ||g_http_cfg.standby_server[0].enable ||g_http_cfg.standby_server[1].enable)
		save_http_result(plate_result);//存储识别结果到数据库中
	return ;
}

