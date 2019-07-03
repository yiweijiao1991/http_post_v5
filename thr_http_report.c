
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>

#include "gbl.h"
#include "thr_http_report.h"
#include "DatabaseMgr.h"
#include "http_post_data.h"
#include "cameraInterface.h"
#include "http_response_handle.h"
#include "generat_http_data.h"
#include "base64.h"
#include "log.h"
#include "http_response_affirm.h"


#define RESULT_TYPE_REALTIME	0 //实时传输识别结果
#define RESULT_TYPE_ABNORMAL	1 //异常重传识别结果

#define RECIVE_STATE_SUCCESS   			0		//接收回应状态->成功
#define RECIVE_STATE_INVALID_ARGUMENT   1		//接收回应状态->参数异常
#define RECIVE_STATE_DATA_STRUCT_ERROR  2		//接收回应状态->数据结构错误
#define RECIVE_STATE_DATA_SIZE_ZERO		3		//接收回应状态->数据大小为0
#define RECIVE_STATE_DATA_DONT_NEED_RESPOND 4	//接收回应状态->不处理回应数据
#define RECIVE_STATE_UNKNOWN 			-1		//接收回应状态->未知

#define CLEAN_DATABASE_INTERVAL	1		//清理数据库的间隔单位：秒
#define CLEAN_IMAGE_INTERVAL	3600	//清理垃圾图片的间隔：秒



extern RK_DeviceInfo g_deviceInfo;	//设备信息
extern http_param_s g_http_cfg;//HTTP配置信息
extern int g_thread_quit;//线程退出标志 0 不退出 1 退出




/*
such as system() without fork
*/
int my_system(const char *cmd)  
{  
	pid_t pid;  
	int status;  
	if(cmd == NULL)  
	{  
	    return 1;  
	}
	if((pid = vfork()) < 0)  
	{  
	    status = -1;  
	}  
	else if(pid == 0)  
	{  
	    execl("/bin/sh", "sh", "-c",cmd, (char*)0);  
	    _exit(127);  
	}  
	else  
	{  
	    while(waitpid(pid, &status, 0) < 0)  
	    {  
		if(errno != EINTR)  
		{  
		    status = -1;  
		    break;  
		}  
	    }  
	}  
	return status;  
} 
/*
函数名：remove_file
函数功能：
删除指定的文件
*/
int remove_file(char * filename)
{
	int ret = -1;
	char system_cmd[1024] = {0};
	if(filename ==NULL || strlen(filename) <=0 )
	{
		log_write("remove error filename is null");
		return ret;
	}
	if(remove(filename) != 0 )
	{
		log_write("remove %s faile,try use system cmd  <rm -rf %s>",filename,filename);
		sprintf(system_cmd,"rm -rf %s",filename);
		my_system(system_cmd);
	}else
	{
		ret = 0;
	}
	return ret;
	
}
/*
 * 函数名称：read_image
 * 函数功能：
 * 读取图片文件内容到缓冲区
 * 参数：path图片路径
 * outbuff 输出缓冲区，需要外部开辟
 * 返回值 0 失败  >0 读取到的数据大小
 */
int read_image(char * path,unsigned char * outbuff)
{
	FILE *fp = NULL;
	int count = 0;
	if(path == NULL || outbuff == NULL || strlen(path) == 0)
	{
		log_write("http read_image Error! invalid paramer");
		return 0;
	}
	fp = fopen(path,"rb");
	int ret = 0;
	if(fp == NULL)
	{
		log_write("http read_image  Error!  open %s Faile",path);
		return 0;
	}

	while(!feof(fp))
	{
		ret = fread(outbuff+count,1,1024*100,fp);
		count+=ret;
	}
	fclose(fp);
	return count;
}


/*
函数名称
	http_trans_record
函数功能
	存储http的发送记录到数据库
参数
	http_server_info[in]:http配置信息
	platenum[in]:车牌号
	reco_time[in]:识别时间
	full_image_size[in]:大图大小
	plate_image_size[in]:小图大小
	request_result[in]:请求结果
	respond_state[in]:回应数据状态
	barrier_state[in]:开闸情况
	result_type[in]:传送方式
	abnormal_id[in]:续传的ID 
返回值
	0
*/

int http_trans_record(http_server_info_s *http_server_info,
					  char *platenum,
					  char * reco_time,
					  int full_image_size,
		      		  int plate_image_size,
		      		  int request_result,
					  int respond_state,
		      		  int barrier_control,
		      		  int result_type,
		      		  int abnormal_id)
{
#if 0
	http_trans_record_t http_trans_data ={0};//数据库存储数据
	/****获取时间***/
	time_t tt;
	struct tm *t;
	char timebuf[64];

	time(&tt);
	t = localtime(&tt); 
	sprintf(&timebuf[0],"%04d-%02d-%02d %02d:%02d:%02d",
			t->tm_year + 1900, t->tm_mon+1, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec);

	if(strlen(platenum) > 0)
		strcpy(http_trans_data.plate_number,platenum);//车牌
	else
		strcpy(http_trans_data.plate_number,"NOPlate");//车牌

	strcpy(http_trans_data.reco_time,reco_time);//识别时间

	strcpy(http_trans_data.trans_time,timebuf);//当前传送时间
	http_trans_data.full_image_size = full_image_size;//大图大小
	http_trans_data.plate_image_size = plate_image_size;//小图大小

	strcpy(http_trans_data.url_string,http_server_info->url_string);// 路径
	http_trans_data.result_type = result_type;//传送类型  0 即时推送数据 1 异常数据重传
	http_trans_data.request_result = request_result;//请求结果 ，curl的返回值
	http_trans_data.respond_state = respond_state;//回应数据的解析状态
	http_trans_data.barrier_control = barrier_control;	//开闸情况
	http_trans_data.abnormal_id = abnormal_id;
	//插入数据库	后面添加
#endif
	HttpTransRecord_t record_data ={0};//数据库存储数据
	/****获取时间***/
	time_t tt;
	struct tm *t;
	char timebuf[64];
	char temp_ip_domain[512] = {0}; //IP域名存储
	char path_str[512] = {0};//推送路径
	char port_str[512] = {0};//端口
	char *p = NULL;//临时指针
	int a,b,c,d;//临时变量
	int ip_type = 0;//0 ip 1 域名

	time(&tt);
	t = localtime(&tt); 
	sprintf(&timebuf[0],"%04d-%02d-%02d %02d:%02d:%02d",
			t->tm_year + 1900, t->tm_mon+1, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec);

	if(strlen(platenum) > 0)
		strcpy(record_data.PlateNumber,platenum);//车牌
	else
		strcpy(record_data.PlateNumber,"NOPlate");//车牌

	strcpy(record_data.RecDateTime,reco_time);//识别时间
	strcpy(record_data.TransDateTime,timebuf);//当前传送时间
	record_data.FullImageSize = full_image_size;//大图大小
	record_data.PlateImageSize = plate_image_size;//小图大小

//分解url

	p = strchr(http_server_info->url_string,'/');
	if(p)
	{
		memcpy(temp_ip_domain,http_server_info->url_string,(p-http_server_info->url_string));
		memcpy(path_str,p,strlen(p));	
		p = strchr(temp_ip_domain,':');
		if(p)
		{
			memcpy(port_str,p+1,strlen(p+1));
			*p = 0;
		}
	}
	//判断是IP还是域名

	if(sscanf(temp_ip_domain, "%d.%d.%d.%d ",&a,&b,&c,&d)==4 &&  a>=0  &&  a<=255 &&  b>=0  &&  b<=255 &&  c>=0  &&  c<=255 &&  d>=0  &&  d<=255) 
	{
		ip_type = 0;
	}else 
	{
		ip_type = 1;
	}

	record_data.IPtype = ip_type;//域名还是IP
	strcpy(record_data.IP_Domain,temp_ip_domain);
	record_data.Port = atoi(port_str);//端口
	strcpy(record_data.Path,path_str);// 路径
	record_data.TransType = result_type;//传送类型  0 即时推送数据 1 异常数据重传
	record_data.Sendstate = request_result;	//发送结果
	record_data.ReciveState = respond_state;	//接受结果
	record_data.DoorOpen = barrier_control;	//开闸情况
	record_data.AbnormalID = abnormal_id;
	//插入数据库	
	if(0 > YBDB_HttpTransRecord_Insert(LOCAL_SERVER_IP,&record_data))
	{
		log_write("http save trans record into database error");
	}

	return 0;
}
/*
函数名称:
	generate_http_result
函数说明:
	组织用于发送的数据结构
参数:
	http_result[in]:识别结果结构体
	result[out]:数据库查询出的识别结果结构体
	result_type[in]:识别结果类型 0 实时传输识别结果 1异常重传数据
	http_image_bin[in]:二进制图片存储区
	http_image_base64[out]:base64图片数据存储区
返回值:
	无
*/
void generate_http_result(http_reco_result_s *http_result,
						  Reco_Result *result,
						  int result_type,
						  http_image_t	*http_image_bin,
						  http_image_t	*http_image_base64
						  )
{


	//车牌识别结果
	//清空
	memset(http_result,0,sizeof(http_reco_result_s));
	//结果类型
	http_result->result_type = result_type;
	if (strlen(result->license) > 0)
	{
		//识别区域
		http_result->plate_info.location.left = result->RECT_left;
		http_result->plate_info.location.top = result->RECT_top;
		http_result->plate_info.location.right = result->RECT_right;
		http_result->plate_info.location.bottom = result->RECT_bottom;

		strcpy(http_result->plate_info.license,result->license);
		strcpy(http_result->plate_info.color,result->platecolor);
		
		http_result->plate_info.color_value = result->colorValue;
		http_result->plate_info.plate_type = result->type;
		http_result->plate_info.confidence = result->confidence;
		http_result->plate_info.reco_time = result->timeUsed;
		http_result->plate_info.bright = result->plateBright;
		http_result->plate_info.direction = result->direction;
		http_result->plate_info.car_birght = result->carBright;
		http_result->plate_info.car_logo = result->carLogo;
		http_result->plate_info.car_color = result->carColor;
		http_result->plate_info.car_type = result->CarType;
		http_result->plate_info.triger_type = result->triggerType;

		strcpy(http_result->devip,g_deviceInfo.ip);
		strcpy(http_result->softver,g_deviceInfo.sysver);

		http_result->platecount = 1;
		sprintf(http_result->recotime,"%s",result->recotime);
		//白名单
		http_result->white_list_data.enable = result->WhiteListEnable;
		http_result->white_list_data.time_match_enable = result->TimeMatchEable;
		strcpy(http_result->white_list_data.create_time,result->WhiteListCreateTime);
		strcpy(http_result->white_list_data.start_time,result->WhiteListStartTime);		
		strcpy(http_result->white_list_data.end_time,result->WhiteListEndTime);
		http_result->white_list_data.is_black = result->BlackList;
		http_result->white_list_data.barrier_control = result->OpenDoor;
		http_result->white_list_data.time_validity = result->TimeCheck;
		http_result->white_list_data.is_interior_car = result->InteriorCar;		
	}else
	{
		http_result->platecount = 0;
		http_result->plate_info.reco_time =  result->timeUsed;
		strcpy(http_result->devip,g_deviceInfo.ip);
		strcpy(http_result->softver,g_deviceInfo.sysver);
		http_result->plate_info.car_logo = result->carLogo;
		http_result->plate_info.car_type = result->CarType;
		http_result->plate_info.triger_type = result->triggerType;
		sprintf(http_result->recotime, "%s",result->recotime);	
	}
	
	//图片
	memset(http_image_bin,0,sizeof(http_image_t));
	if(strlen(result->FullImagePath) > 0)
	{
		http_image_bin->full_image_len= read_image(result->FullImagePath,
							   http_image_bin->full_image_buff);
	}
	if(strlen(result->PlateImagePath) > 0)
	{
		http_image_bin->plate_image_len = read_image(result->PlateImagePath,
							     http_image_bin->plate_image_buff);
	}
	memset(http_image_base64,0,sizeof(http_image_t));
	//图片转base64
	if(http_image_bin->full_image_len > 0)
	{
		base64_encode(http_image_bin->full_image_buff,
					 (char *)http_image_base64->full_image_buff,
					 http_image_bin->full_image_len);
		http_image_base64->full_image_len = strlen((char *)http_image_base64->full_image_buff);
		
	}
	if(http_image_bin->plate_image_len > 0)
	{
		base64_encode(http_image_bin->plate_image_buff,
		              (char *)http_image_base64->plate_image_buff,
					  http_image_bin->plate_image_len);
		http_image_base64->plate_image_len = strlen((char *)http_image_base64->plate_image_buff);
	}

	return;
}

/*
函数名称:
	http_send_func
函数说明:
	发送http请求
返回值:
	-1:生成json数据失败
	其他 CURL返回值
*/
int http_send_func(http_reco_result_s  *http_result,
				   http_image_t	*http_image_base64,
				   http_send_buff_t    *http_send_buff,
				   http_recive_data_s  *http_recive_data,
				   CURL 			   *http_curl_handle,
				   http_server_info_s	   *servre_info,  
				   http_param_s        *http_config)
{
	char url_str[1024] = {0};		//url
	char *p_full_image_base64 = NULL;	//base64大图指针
	char *p_plate_image_base64 = NULL;	//base64小图指针
	int res;	//http请求返回值
	int ret = 0;


	//是否使用ssl
	if(http_config->is_ssl_connect)
		sprintf(url_str,"https://%s",servre_info->url_string);
	else
		sprintf(url_str,"http://%s",servre_info->url_string);

	//获取图片指针
	p_full_image_base64 = NULL;
	p_plate_image_base64 = NULL;

	if(servre_info->full_image_report_enable && http_image_base64->full_image_len > 0)
	{
		p_full_image_base64 = (char *)http_image_base64->full_image_buff;
	}

	if(servre_info->plate_image_report_enable&& http_image_base64->plate_image_len> 0)
	{
		p_plate_image_base64= (char *)http_image_base64->plate_image_buff;
	}

	if(http_result->result_type == RESULT_TYPE_REALTIME)
	{
		log_write("http report ready send realtime result to %s ,plate number:%s",url_str,http_result->plate_info.license);
	}else
	{
		log_write("http report ready send abnormal result to %s ,plate number:%s",url_str,http_result->plate_info.license);
	}


	//生成json串
	memset(http_send_buff,0,sizeof(http_send_buff_t));
	
	ret = get_result_json_string(http_config,
				     http_result,
				     p_full_image_base64,
				     p_plate_image_base64,
				    &g_deviceInfo,
				    (char *)http_send_buff->data_buff,
				    HTTP_SEND_BUFF_MAX_SIZE,
				    http_config->characters_type);

	if(ret != 0)
	{
		//生成json失败具体原因后面添加日志
		if(ret == -1)
			log_write("http report send result [%s] to %s generate json string error:invalid paramer",http_result->plate_info.license,url_str);
		else if(ret == -2)
			log_write("http report send result [%s] to %s generate json string error:create json section error",http_result->plate_info.license,url_str);
		else if(ret == -3)
			log_write("http report send result [%s] to %s generate json string error:et json string error",http_result->plate_info.license,url_str);
		else if(ret == -4)
			log_write("http report send result [%s] to %s generate json string error:storage too small",http_result->plate_info.license,url_str);
		else if(ret == -5)
			log_write("http report send result [%s] to %s generate json string error:iconv open faile",http_result->plate_info.license,url_str);
		else if(ret == -6)
			log_write("http report send result [%s] to %s generate json string error:iconv faile",http_result->plate_info.license,url_str);
		else
			log_write("http report send result [%s] to %s generate json string error:unknown error",http_result->plate_info.license,url_str);
		return -1;	
	}
	
	http_send_buff->data_len = strlen((char *)http_send_buff->data_buff);

	memset(http_recive_data,0,sizeof(http_recive_data_s));
	

	//发送请求
	res = http_post(http_curl_handle,
					url_str,
					(char *)http_send_buff->data_buff,
					http_recive_data,
					http_config->session_timeout,
					http_config->characters_type);

	return res;
}


/*
函数名称:
	http_realtime_result_send
函数说明:
	发送实时识别结果并处理回应
参数:
	result[in]:识别结果
	http_image_bin[in]:二进制图片存储指针
	http_image_base64[in]:base64图片内存指针
	http_recive_data[in]:数据接收存储内存指针
	http_send_buff[in]:发送区失真
	http_curl_handle[in]:http发送模块句柄
	http_config[in]:配置参数
返回值:
	0成功 -1失败


*/
int http_realtime_result_send(Reco_Result    *result,
					 http_image_t	*http_image_bin,
					 http_image_t	*http_image_base64,
					 http_recive_data_s *http_recive_data,
					 http_send_buff_t    *http_send_buff,
					 CURL 			*http_curl_handle,
					 http_param_s    http_config)
{

	http_reco_result_s  http_result;		//车牌数据	
	int i = 0;
	int j = 0;
	int ret = 0;
	int  res;
    	int  barrier_control_result  = -1;
	int  ret_value = 0;
	int  recive_state = -1;
	//组织发送结果结果体

	generate_http_result(&http_result,
						 result,
						 RESULT_TYPE_REALTIME,
						 http_image_bin,
						 http_image_base64);

	if(http_config.main_server.enable)
	{
		
		for(j = 0;j < http_config.push_num;j++)
		{
			res = http_send_func(&http_result,
								  http_image_base64,
								  http_send_buff,
								  http_recive_data,
								  http_curl_handle,
								  &http_config.main_server,
								  &http_config);
			if(res == CURLE_OK)
			{
				//处理回应数据
				log_write("http send realtime result [%s] to main server[%s] success[%d/%d]",http_result.plate_info.license,
													     http_config.main_server.url_string,
													     j+1,
													     http_config.push_num);
				if(http_config.http_control_enable)
				{
					if(http_recive_data->datalen > 0)
					{
						if(strlen( http_recive_data->data) > 1024*3 - 512)
						{
							log_write("http send realtime result [%s] to main server[%s] get response data len is %d\n",http_result.plate_info.license,
									http_config.main_server.url_string,
									http_recive_data->datalen);
						}else
						{
							log_write("http send realtime result [%s] to main server[%s] get response :\n%s",http_result.plate_info.license,
									http_config.main_server.url_string,
									http_recive_data->data);
						}
						ret = response_handle(http_recive_data->data,&barrier_control_result,http_config.rs485_delay);
						//根据http处理结果进行插入数据库写日志之类的操作
						if(ret == 0)
						{
							//成功
							log_write("http realtime result[%s] of main server[%s] response handle success",
										http_result.plate_info.license,http_config.main_server.url_string);
							recive_state = RECIVE_STATE_SUCCESS;
						}else
						{
							if(ret == -1)
								log_write("http realtime result[%s] of main server[%s] response handle faile,paramer invalid",http_result.plate_info.license,http_config.main_server.url_string);
							else if(ret == -2)
								log_write("http realtime result[%s] of main server[%s] response handle faile,get json struct error",http_result.plate_info.license,http_config.main_server.url_string);

							else if(ret == -3)
								log_write("http realtime result[%s] of main server[%s] response handle faile,get response section in json error",http_result.plate_info.license,	http_config.main_server.url_string);

							else
								log_write("http realtime result[%s] of main server[%s] response handle faile,unknown error",http_result.plate_info.license,http_config.main_server.url_string);

							//处理失败
							if(ret== -1)
							{
								recive_state = RECIVE_STATE_INVALID_ARGUMENT;
							}else
							{
								recive_state = RECIVE_STATE_DATA_STRUCT_ERROR;
							}
						}
						//发送确认包
						if(http_config.response_affirm_enable)
						{
							char url_str[1024] = {0};
							if(http_config.is_ssl_connect)
								sprintf(url_str,"https://%s",http_config.main_server.url_string);
							else
								sprintf(url_str,"http://%s",http_config.main_server.url_string);
							if(ret == 0)
							{
								http_send_response_affirm(http_recive_data,
														   http_send_buff,
														   http_curl_handle,
														   http_config,
														   url_str,
														   RESPONSE_SUCESS_AFFIRM);
							}else
							{
								http_send_response_affirm(http_recive_data,
														  http_send_buff,
														  http_curl_handle,
														  http_config,
														  url_str,
														  RESPONSE_FAILE_AFFIRM);
							}
						}
					}else
					{
						log_write("http realtime result[%s] of main server[%s] get response length is 0,so no operation",http_result.plate_info.license,http_config.main_server.url_string);
						
						recive_state = RECIVE_STATE_DATA_SIZE_ZERO;
						//接收数据为0
					}
				}else
				{
					//不处理回应
					recive_state = RECIVE_STATE_DATA_DONT_NEED_RESPOND;
					
				}
				break;				
			}else
			{
				log_write("http send realtime result [%s] to main server[%s] faile[%d/%d],ret = %d",http_result.plate_info.license,
													   	    http_config.main_server.url_string,
														    j+1,
														    http_config.push_num,
														    res);
				if(j == (http_config.push_num -1))
				{
					ret_value = -1;
					recive_state = RECIVE_STATE_UNKNOWN;
					//发送失败插入异常数据库
					if(http_config.offline_continuingly_en)
					{
						ret =  YBDB_Abnormal_Result_Insert(LOCAL_SERVER_IP,result,0);
						if(0 != ret)
						{
							log_write("http realtime result [%s] of main server insert to abnormal database faile,ret = %d",http_result.plate_info.license,ret);
						}
					}
						

				}
			}
		}
		//这条记录插入到数据库中，数据库接口后面定义
		http_trans_record(&(http_config.main_server),
						  http_result.plate_info.license,
						  http_result.recotime,
						  http_image_bin->full_image_len,
						  http_image_bin->plate_image_len,
						  res,
						  recive_state,
						  barrier_control_result,
						  RESULT_TYPE_REALTIME,
						  0
						  );
	}
	//备用服务器
	for(i = 0;i < 2;i++)
	{
		
		if(http_config.standby_server[i].enable)
		{

			for(j = 0;j < http_config.push_num;j++)
			{
				res = http_send_func(&http_result,
									 http_image_base64,
									 http_send_buff,
									 http_recive_data,
									 http_curl_handle,
									 &(http_config.standby_server[i]),
									 &http_config);
				if(res == CURLE_OK)
				{
					//不处理回应,写日志
					log_write("http send realtime result [%s] to standby server[%d]-[%s] success[%d/%d]",http_result.plate_info.license,
						     						i+1,
												http_config.standby_server[i].url_string,
												j+1,
												http_config.push_num);
					recive_state = RECIVE_STATE_DATA_DONT_NEED_RESPOND;
					break;
				}else
				{
					log_write("http send realtime result [%s]to standby server[%d]-[%s] faile[%d/%d] ret = %d",http_result.plate_info.license,
						   							  i+1,
													  http_config.standby_server[i].url_string,
													  j+1,
													  http_config.push_num,
													  res);
					if(j == (http_config.push_num -1))
					{
						//发送失败，后面处理
						if(http_config.offline_continuingly_en)
						{
							ret =  YBDB_Abnormal_Result_Insert(LOCAL_SERVER_IP,result,i+1);;
							if(0 != ret)
							{
								log_write("http realtime result [%s] of standby server[%d] insert to abnormal database faile,ret = %d",http_result.plate_info.license,i+1,ret);
							}
						}	
						recive_state = RECIVE_STATE_UNKNOWN;
						ret_value = -1;
					}
				}
			}
			barrier_control_result = -1;
			//这条记录插入到数据库中，数据库接口后面定义	
			http_trans_record(&(http_config.standby_server[i]),
					http_result.plate_info.license,
					http_result.recotime,
					http_image_bin->full_image_len,
					http_image_bin->plate_image_len,
					res,
					recive_state,
					barrier_control_result,
					RESULT_TYPE_REALTIME,
					0
					);	
		}	
	}

	//删除图片
	if(ret_value == 0)
	{
		if(strlen(result->FullImagePath) > 0)
		{
			remove_file(result->FullImagePath);	
		}
		if(strlen(result->PlateImagePath) > 0)		
		{			
			remove_file(result->PlateImagePath);		
		}
	}
	//删除数据库中的数据
	ret = YBDB_Reco_Result_DeleteByID(LOCAL_SERVER_IP,result->ID);
	if(0 != ret)
	{
		log_write("http realtime result [%s] delete from database error,ret = %d",http_result.plate_info.license,ret);
	}
	return ret_value;
}

/*
函数名称:
	http_abnormal_result_send
函数说明:
	发送异常重传识别结果
参数:
	result[in]:识别结果
	http_image_bin[in]:二进制图片存储指针
	http_image_base64[in]:base64图片内存指针
	http_recive_data[in]:数据接收存储内存指针
	http_send_buff[in]:发送区失真
	http_curl_handle[in]:http发送模块句柄
	http_config[in]:配置参数
返回值:
	0成功 -1失败
*/

int http_abnormal_result_send(http_image_t	*http_image_bin,
							 http_image_t	*http_image_base64,
							 http_recive_data_s *http_recive_data,
							 http_send_buff_t    *http_send_buff,
							 CURL 			*http_curl_handle,
							 http_param_s    http_config)
{
	Reco_Result    result;				//识别结果
	http_reco_result_s  http_result;		//车牌数据	
	int i  = 0;
	http_server_info_s	server_info;	//服务器信息
	int ret = 0;
	int  res;
	int ret_value = 0;				//返回值
	int  recive_state = -1;			//接收回应结果
	
	if(!http_config.offline_continuingly_en)
		return 0;
	
	for(i = 0;i < 3;i++)
	{
		//获取服务器信息
		if(i == 0)
			memcpy(&server_info,&(http_config.main_server),sizeof(http_server_info_s));
		else if(i == 1)
			memcpy(&server_info,&(http_config.standby_server[0]),sizeof(http_server_info_s));
		else if(i == 2)
			memcpy(&server_info,&(http_config.standby_server[1]),sizeof(http_server_info_s));
		else
				return -1;
		if(server_info.enable)
		{
			//根据服务器信息查询异常数据
			memset(&result,0,sizeof(Reco_Result));
			ret = YBDB_Abnormal_Result_SelectByServ(LOCAL_SERVER_IP,i,&result);
			if(ret > 0)
			{
				//组织发送结果结果体
				generate_http_result(&http_result,&result,RESULT_TYPE_ABNORMAL,http_image_bin,http_image_base64);
				//发送请求
				res = http_send_func(&http_result,
									 http_image_base64,
									 http_send_buff,
									 http_recive_data,
									 http_curl_handle,
									 &server_info,
									 &http_config);
	
				if(res == CURLE_OK)
				{
					//不处理回应,写日志		
					log_write("http send abnormal result [%s] to [%s] success",http_result.plate_info.license,
																				server_info.url_string);
					//数据库中删除该数据
					if(0 == YBDB_Abnormal_Result_DeleteByID(LOCAL_SERVER_IP,result.ID))
					{
						//判断图片是否还有用
						if(0 == YBDB_Abnormal_Result_JudgeByPath(LOCAL_SERVER_IP,result.FullImagePath))
						{
							//删除图片
							if(strlen(result.FullImagePath) > 0)
							{
								remove_file(result.FullImagePath);	
							}
							if(strlen(result.PlateImagePath) > 0)		
							{			
								remove_file(result.PlateImagePath);		
							}
						}
					}else
					{
							log_write("http abnormal result [%s] deleted from database error",result.license);
					}
					
					recive_state = RECIVE_STATE_DATA_DONT_NEED_RESPOND;	
				}else
				{

					//发送失败，后面处理
					log_write("http send abnormal result [%s] to [%s] faile ret = %d",http_result.plate_info.license,
																					  server_info.url_string,
																					  res);
					if(res == 1)
						res = -999;
					else if(res > 0)
						res = 0-res;
					recive_state = RECIVE_STATE_UNKNOWN;
					ret_value = -1;
				}
				//插入数据库中做记录(如果库中已经存在该记录那么就不存，要求接口自动实现)
				http_trans_record(&server_info,
						http_result.plate_info.license,
						http_result.recotime,
						http_image_bin->full_image_len,
						http_image_bin->plate_image_len,
						res,
						recive_state,
						-1,
						RESULT_TYPE_ABNORMAL,
						result.ID
						);
			}

		
		}		
	}

	return ret_value;	
}
/*
 *@function name: 
	clean_dir
 *@Author: yiweijiao
 *@Date: 2019-07-02 16:03:13
 *@describtion: 
	清理目录下的图片
 *@parameter: 
	root[in]:根目录
 *@return: 
	-1:失败
	0:成功
*/
int clean_dir(char *root)
{
	DIR *dir;
	struct dirent * ptr;
	struct stat file_state; //文件的信息
	int ret = 0;//返回值
	char path[256] = {0};
	time_t ts;
	time_t yestoday;
	createDir(root);
	dir = opendir(root); /* 打开目录*/
	//获取当前时间
	ts = time(NULL);
	//一天前的时间
	yestoday = ts -3600*24;

	if(dir == NULL)
	{
		log_write("clean http dir fail, open dir %s error",root);
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
		 //获得文件状态信息
		memset(path,0,256);
		sprintf(path,"%s%s",root,ptr->d_name);
		//获取文件信息
   		ret =stat(path, &file_state);
		if(ret == 0)
		{
			//判断文件是否超时
			if((file_state.st_ctime < yestoday ) || (file_state.st_ctime > ts ))	
			{
				//删除文件
				remove_file(path);	
			}
		}else
		{
			log_write("clean http dir faile, get file %s status faile",path);
		}	
 	}
	closedir(dir);
	return 0;
}

void clean_garbage_image()
{
	//删除Flash中一天之前图片的数据		
	clean_dir(FLASH_PLATEIMAGE_PATH);
	clean_dir(FLASH_FULLIMAGE_PATH);
	if((access(TFCARD_DEVICE,F_OK))==-1)  
	{  
		return;
	}
	//删除TF 卡中一天之前的图片
	clean_dir(TF_PLATEIMAGE_PATH);
	clean_dir(TF_FULLIMAGE_PATH);	
}

void clean_database()
{
	time_t ts;
	struct   tm     *ptm; 
	char timestr[20] = {0};
	int ret = 0;
	Reco_Result result = {0};

	//获取当前时间
	ts = time(NULL);
	//1天前的时间
	ts -= 3600*24;
	//转化成为字符串
	ptm =  localtime(&ts); 
	sprintf(timestr,"%04d-%02d-%02d %02d:%02d:%02d",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_hour,ptm->tm_sec);
	/*
	清理数据库中数据清理时间小于timestr的数据 
	*/
	//异常表的清理
	memset(&result,0,sizeof(Reco_Result));
	ret = YBDB_Abnormal_Result_SelectByTime(LOCAL_SERVER_IP,timestr,&result);
	
	if(ret > 0)
	{
		//1从数据库中移除
		ret = YBDB_Abnormal_Result_DeleteByID(LOCAL_SERVER_IP,result.ID);
		if(ret == 0)
		{
			//2.根据图片路径查找异常表中的数据
			ret = YBDB_Abnormal_Result_JudgeByPath(LOCAL_SERVER_IP,result.FullImagePath);
			if(ret == 0)
			{
				//删除图片
				if(strlen(result.PlateImagePath) > 0)
				{
					remove_file(result.PlateImagePath);	
				}
				if(strlen(result.FullImagePath) > 0)
				{
					remove_file(result.FullImagePath);		
				}
			}
		}	
	}
	//缓冲表的清理
	memset(&result,0,sizeof(Reco_Result));
	ret =  YBDB_Reco_Result_SelectByTime(LOCAL_SERVER_IP,timestr,&result);
	if(ret > 0)
	{
		ret = YBDB_Reco_Result_DeleteByID(LOCAL_SERVER_IP,result.ID);
		if(ret == 0)
		{
			//删除图片
			if(strlen(result.PlateImagePath) > 0)
			{
				remove_file(result.PlateImagePath);	
			}
			if(strlen(result.FullImagePath) > 0)
			{
				remove_file(result.FullImagePath);	
			}
		}
		
	}
}


/*
函数名称：Thr_httpReport
函数功能:http推送识别结果线程函数
参数
返回值
*/
void *thr_http_report(void *arg)
{
	void 			*status = NULL;
	Reco_Result		 result = {0};			//从数据库中读取出来的数据
	int 			 ret = 0;
	time_t 			 abnormal_send_last_time;//断网续传使用的时间值
	time_t 			 nowtime;				//当前时间
	time_t			 last_clean_database_time;//上次清理数据库的时间
	time_t			 last_clean_image_time;		//上次清理图片的时间
	http_image_t *	 http_image_bin = NULL;		//http二进制图片缓冲区
	http_image_t * 	 http_image_base64 = NULL;	//http base64图片缓冲区
	http_recive_data_s *http_recive_data = NULL;//接收回应数据缓冲区	
	CURL 			*http_curl_handle = NULL;	//curl句柄
	http_send_buff_t * http_send_buff = NULL;	//发送缓冲区

	log_write("************ http report thread start *****************");

	//分配内存和资源
	http_image_bin = (http_image_t *)malloc(sizeof(http_image_t));
	http_image_base64 = (http_image_t *)malloc(sizeof(http_image_t));
	http_recive_data = (http_recive_data_s *)malloc(sizeof(http_recive_data_s));
	http_send_buff = (http_send_buff_t *)malloc(sizeof(http_send_buff_t));
	if(!http_image_bin || !http_image_base64 || !http_recive_data || !http_send_buff)
	{
		log_write("http report malloc resource faile");
		goto http_report_out;
	}

	http_curl_handle = http_init();
	if(!http_curl_handle)
	{
		log_write("http report  create curl handle faile");
		goto http_report_out;
	}
	
	nowtime = time(NULL);
	abnormal_send_last_time = time(NULL);
	last_clean_database_time = time(NULL);
	last_clean_image_time = time(NULL);

	while(!g_thread_quit)
	{
		nowtime = time(NULL);
		//1.判断是配置文件中启用了HTTP功能
		if(g_http_cfg.main_server.enable ||g_http_cfg.standby_server[0].enable ||g_http_cfg.standby_server[1].enable)
		{
			//2.从缓存表中获取一条数据
			ret = YBDB_Reco_Result_SelectOldest(LOCAL_SERVER_IP,&result);
			if(ret >0)
			{
				//获取到
				http_realtime_result_send(&result,
							  http_image_bin,
							  http_image_base64,
							  http_recive_data,
							  http_send_buff,
							  http_curl_handle,
							  g_http_cfg);
			}else 
			{
				if(ret < 0)
					log_write("http get realtime result from database error");

				//没有获取到，如果没有数据那么就每两秒进行异常数据推送
				if((nowtime - abnormal_send_last_time) >1 || (nowtime - abnormal_send_last_time) < 0)
				{
					http_abnormal_result_send(http_image_bin,
								  http_image_base64,
								  http_recive_data,
								  http_send_buff,
								  http_curl_handle,
								  g_http_cfg);
					abnormal_send_last_time = nowtime; 
				}
			}	
		}
		//定时进行过期数据清理，删除超过24小时以外的数据，
		if((nowtime - last_clean_database_time) >= CLEAN_DATABASE_INTERVAL || (nowtime - last_clean_database_time) < 0)
		{
			//清理数据
			clean_database();	
			last_clean_database_time = nowtime;
		}
		//定时进行图片清理，删除过期的图片
		if((nowtime - last_clean_image_time) >= CLEAN_IMAGE_INTERVAL || (nowtime - last_clean_image_time) < 0)
		{
			//清理图片
			clean_garbage_image();	
			last_clean_image_time = nowtime;
		}	
		usleep(1000000);
	}
http_report_out:
	//释放资源
	if(http_image_bin)
	{
		free(http_image_bin);
		http_image_bin = NULL;
	}
	if(http_image_base64)
	{
		free(http_image_base64);
		http_image_base64 = NULL;
	}
	if(http_recive_data)
	{
		free(http_recive_data);
		http_recive_data = NULL;
	}
	if(http_send_buff)
	{
		free(http_send_buff);
		http_send_buff = NULL;
	}
	if(http_curl_handle)
	{
		http_clean(http_curl_handle);
		http_curl_handle = NULL;
	}
	return  status;
}

