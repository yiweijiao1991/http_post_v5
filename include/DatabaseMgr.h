#ifndef  _DATABASE_MGR_20180821_
#define _DATABASE_MGR_20180821_

#if !defined(WIN32) && !defined(__stdcall)
#define __stdcall
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define LOCAL_SERVER_IP 	"127.0.0.1"



/*HTTP传输记录表***************
******************************/
typedef struct HttpTransRecord_t
{
    int 		 HttpID;		//唯一的ID
    char		PlateNumber[16];	//车牌号码
    char 		RecDateTime[20];	//识别时间
    char		TransDateTime[20];	//传输时间
    int 		FullImageSize;		//全景图大小
    int 		PlateImageSize;		//\u8f66\u724c\u56fe\u5927\u5c0f
    int 		IPtype;			//\u4f7f\u7528IP\u8fd8\u662f\u57df\u540d 0 IP  1\u57df\u540d
    char 		IP_Domain[50];		//IP \u6216 \u57df\u540d
    int 		Port;			//\u7aef\u53e3
    char 		Path[200];		//\u670d\u52a1\u7a0b\u5e8f\u8def\u5f84
    int 		TransType;		//\u4f20\u9001\u65b9\u5f0f  0 \u5373\u65f6\u63a8\u9001\u6570\u636e 1 \u5f02\u5e38\u6570\u636e\u91cd\u4f20
    int		Sendstate;		//\u53d1\u9001\u7ed3\u679c
    int 		ReciveState;		//\u63a5\u6570\u636e\u72b6\u6001 0 \Uffffffff
    int 		DoorOpen;		//是否开闸 0 不开 1 开
    int 		AbnormalID;		//续传ID
} HttpTransRecord_t;


//add by yiweijioa@2019-06-17
typedef struct http_trans_record_t
{
    int 		http_id;			//唯一的ID
    char		plate_number[16];	//车牌号码
    char 		reco_time[20];		//识别时间
    char		trans_time[20];		//传输时间
    int 		full_image_size;	//全景图大小
    int 		plate_image_size;	//车牌图像大小
    char 		url_string[256];	//url
    int 		result_type;		//识别结果类型
    int			request_result;		//请求结果
    int 		respond_state;		//回应数据状态
    int 		barrier_control;		//道闸控制
    int 		abnormal_id;		//异常续传ID
} http_trans_record_t;




/*识别结果暂存表***************
******************************/
typedef struct Reco_Result
{
    int 		 ID;
    int          carBright;              //车身亮度（预留）
    int          carColor;               //车身颜色（预留）
    int          colorValue;             //颜色值（预留）
    int          confidence;             //可信度
    int          direction;              //行驶方向
    char    	 imagePath[256];              //图片路径（预留）
    char     	 license[16];                //车牌号，(不可为空)
    int          RECT_bottom;            //车牌在图片中的矩形坐标,下
    int          RECT_left;              //车牌在图片中的矩形坐标,左
    int          RECT_right;             //车牌在图片中的矩形坐标,右
    int          RECT_top;               //车牌在图片中的矩形坐标,上
    int          timeUsed;               //识别耗时
    int          triggerType;            //触发类型
    int          type;                   //车牌类型
    char     	 platecolor[6];             //车牌颜色
    int          carLogo;                //车标
    int          CarType;                //车型
    int          plateBright;            //车牌亮度
    char     	 recotime[20];               //识别时间 (不可为空)
    char   		 FullImagePath[256];          //大图存储路径
    char   		 PlateImagePath[256];         //小图存储路径
    int          WhiteListEnable;        //白名单是否启用  0未启用  1启用
    int          TimeMatchEable;         //白名单是否启用时间匹配  0未启用1启用
    char     	 WhiteListCreateTime[20];        //白名单创建时间
    char     	 WhiteListStartTime[20];        //白名单开始生效时间
    char     	 WhiteListEndTime[20];        //白名单过期时间
    int          BlackList;              //是否被设置为黑名单
    int          OpenDoor;               //白名单判断结果，是否自动抬杆放行
    int          TimeCheck;              //白名单判断结果，是否在有效期内
    int			 InteriorCar;		    //是否为内部车
} Reco_Result;

/*异常信息表***************
******************************/
typedef struct Abnormal_Result
{
    Reco_Result * pRecoResult;   // 识别暂存信息
    int nServerNum; // 服务器编码
} Abnormal_Result;

/////////////////////////////////////////////以下是操作函数


//=============================http 通行数据==================

//插入HTTP传输记录
/*
YBDB_HttpTransRecord_Insert
函数说明：向数据库中插入用户信息(通行细信)
参数说明：
transeRecord[in]：要插入的通行信息
pServerIp[in]: 服务器ip

返回值：0成功  <0 失败
*/
int __stdcall YBDB_HttpTransRecord_Insert(char* pServerIp, HttpTransRecord_t *transeRecord);


//=============================识别暂存==================

//插入识别暂存信息
/*
YBDB_HttpTransRecord_Insert
函数说明：向数据库中插入识别暂存信息
参数说明：
recoResult[in]：要插入的识别暂存信息
pServerIp[in]: 服务器ip

返回值：0成功  <0 失败
*/
int __stdcall YBDB_Reco_Result_Insert(char* pServerIp, Reco_Result *recoResult);


/*
函数名:Reco_Result_DeleteByID
函数说明：删除识别暂存信息表中的数据,通过自增ID
参数说明：nID[in]需要删除的ID
pServerIp[in]: 服务器ip
返回值：0成功  <0 失败
*/
int __stdcall YBDB_Reco_Result_DeleteByID(char* pServerIp, int nID);


/*
函数名:Reco_Result_SelectByTime
函数说明：根据时间段recotime查询最老的一条数据
参数说明：
			pServerIp[in] 服务器ip
			chTime[in]时间段
		  recoResult[out]查询到的数据
返回值：大于0 有数据 0无数据  <0 失败
*/
int __stdcall YBDB_Reco_Result_SelectByTime(char* pServerIp, char *chTime, Reco_Result *recoResult);


/*
函数名:Reco_Result_SelectOldest
函数说明：查询识别暂存信息表中存放时间最长的数据，即ID最小的数据
参数说明：recoResult[out]查询到的结果
					pServerIp[in] 服务器ip
返回值：大于0 有数据 0无数据  <0 失败
*/
int __stdcall YBDB_Reco_Result_SelectOldest(char* pServerIp, Reco_Result *recoResult);


/*
函数名:Reco_Result_Empty
函数说明：清空识别结果暂存表
参数说明：无
返回值：0成功  <0 失败
*/
int __stdcall YBDB_Reco_Result_Empty(char* pServerIp);


//==================================插入异常信息=====================
//插入异常信息
/*
Abnormal_Result_Insert
函数说明：向数据库中插入异常信息
参数说明：
abnormalResult[in]：待插入信息
pServerIp[in] 服务器ip
nServerNum[in] 服务器编码
返回值：0成功  其它 失败
*/
int __stdcall YBDB_Abnormal_Result_Insert(char* pServerIp, Reco_Result *abnormalResult, int nServerNum);


/*
函数名:Abnormal_Result_DeleteByID
函数说明：删除异常信息表中的数据,通过自增ID
参数说明：nID[in]需要删除的ID
pServerIp[in] 服务器ip
返回值：0成功  -1 失败
*/
int __stdcall YBDB_Abnormal_Result_DeleteByID(char* pServerIp, int nID);


/*
函数名:Abnormal_Result_SelectByTime
函数说明：根据时间段Abnormaltime查询最老的一条数据
参数说明：chTime[in]时间段
		  abnormalResult[out]查询到的数据
		  pServerIp[in] 服务器ip
返回值：大于0 有数据 0无数据  -1 失败
*/
int __stdcall YBDB_Abnormal_Result_SelectByTime(char* pServerIp, char *chTime, Reco_Result *abnormalResult);


/*
函数名:Abnormal_Result_SelectOldest
函数说明：查询异常信息表中存放时间最长的数据，即ID最小的数据
参数说明：abnormalResult[out]查询到的结果
		  pServerIp[in] 服务器ip
返回值：大于0 有数据 0无数据  -1 失败
*/
int __stdcall YBDB_Abnormal_Result_SelectOldest(char* pServerIp, Reco_Result *abnormalResult);



/*
函数名:Abnormal_Result_SelectByServ
函数说明：根据服务器编码ServNum查询最老的一条数据
参数说明：ServNum[in]服务器编码
		  abnormalResult[out]查询到的数据
		   pServerIp[in] 服务器ip
返回值：大于0 有数据 0无数据  <0 失败
*/
int __stdcall YBDB_Abnormal_Result_SelectByServ(char* pServerIp, int ServNum, Reco_Result *abnormalResult);


/*
函数名:Abnormal_Result_JudgeByPath
函数说明：根据大图路径判断数据是否存在
参数说明：
chPath[int]  路径信息
 pServerIp[in] 服务器ip
返回值：0 不存在  1  存在
返回值：大于0 有数据 0无数据  <0 失败
*/
int __stdcall YBDB_Abnormal_Result_JudgeByPath(char* pServerIp, char *chPath);



/*
函数名:Abnormal_Result_Empty
函数说明：清空识别结果暂存表
参数说明：无
返回值：0成功  <0 失败
*/
int __stdcall YBDB_Abnormal_Result_Empty(char* pServerIp);



#ifdef __cplusplus
}
#endif

#endif
