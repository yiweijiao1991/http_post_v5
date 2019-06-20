//用于车牌识别设备的白名单相关的操作

#ifndef _WTYSDK_WHITE_LIST_H_
#define _WTYSDK_WHITE_LIST_H_
#if !defined(WIN32) && !defined(__stdcall)
#define __stdcall
#endif


#ifdef __cplusplus
extern "C" {
#endif

// ==============mdf by zhz

#define IpCheck(a) 0
#define Log_Err 1
#define Log(a,b,c) 1+1
//=======================


//---一些返回值错误信息定义
typedef enum
{
	WTYSDK_WLIST_ERROR_NO_ERROR=0,					//返回成功!
	WTYSDK_WLIST_ERROR_GENERAL=-1,					//调用底层接口库失败
	WTYSDK_WLIST_ERROR_IP_NULL=-2,					//IP地址为空
	WTYSDK_WLIST_ERROR_SIZEOUTOFRANGE=-3,			//每页大小超过100
	WTYSDK_WLIST_ERROR_MALLOCFAIL=-4,				//申请内存失败
	WTYSDK_WLIST_ERROR_IP_ILLEGALITY=-5	,			//IP地址非法
	WTYSDK_WLIST_DEVICEALREADYREGISTER=1,			//设备已经注册成功
	WTYSDK_WLIST_ERROR_DATA=-11,					//导入信息有误
	WTYSDK_WLIST_TIME_OUT=-12,						//select超时
	WTYSDK_WLIST_SERVER_CLOSED=-13,					//服务器端不存在
	WTYSDK_WLIST_WLISTSOCKET_ERROR=-21,				//套接字创建失败
	WTYSDK_WLIST_CONNECT_ERROR=-22,					//连接服务器失败
	WTYSDK_WLIST_SELECT_ERROR=-23,					//select失败
	WTYSDK_WLIST_RECV_ERROR=-24,					//接收数据返回失败
	WTYSDK_WLIST_SEND_ERROR=-25,					//发送数据失败
	WTYSDK_WLIST_DATABASE_ERROR		=1,				//数据库操作失败
	WTYSDK_WLIST_REQ_ERROR			=2,				//请求包格式错误
	WTYSDK_WLIST_RECIVE_TIMEOUT     =3,			    //接受包超时
	WTYSDK_WLIST_DATA_ERROR			=4,				//数据错误

	WTYSDK_WLIST_ERROR_NOREGISTSPACE = -6,  // 无注册预留空间 最多100个 mdf by zhz
	WTYSDK_WLIST_ERROR_QUERYCONDITION = -7 // 查询条件出错(比如查询参数不合法) mdf by zhz
}
CLIENT_LPR_WLIST_ERROR;



/*
*	车牌类型宏定义
*/
#define WTYSDK_PLATETYPE_UNKNOWN        0           ///未知类型
#define WTYSDK_PLATETYPE_BLUE           1           ///蓝牌
#define WTYSDK_PLATETYPE_BLACK          2           ///黑牌
#define WTYSDK_PLATETYPE_SINGLEYELLOW   3           ///单层黄牌
#define WTYSDK_PLATETYPE_DOUBLEYELLOW   4           ///双层黄牌
#define WTYSDK_PLATETYPE_JCPLATE        5			///警车车牌
#define WTYSDK_PLATETYPE_WJPLATE        6			///武警车牌
#define WTYSDK_PLATETYPE_INDIVIDUATION  7			///个性化车牌
#define WTYSDK_PLATETYPE_SINGLEARMY		8			///单层军牌
#define WTYSDK_PLATETYPE_DOUBLEARMY     9			///双层军牌
#define WTYSDK_PLATETYPE_EMBASSY        10			///使馆车牌
#define WTYSDK_PLATETYPE_HKOKATE		11			///香港车牌
#define WTYSDK_PLATETYPE_TRACTOR        12			///拖拉机
#define WTYSDK_PLATETYPE_AOMEN			13 			///澳门牌
#define WTYSDK_PLATETYPE_INTERNAL       14			///内部牌
#define WTYSDK_PLATETYPE_MHPALTE		15			///民航车牌
#define WTYSDK_PLATETYPE_CONSULATE      16			///领馆车牌

/*
*	车辆颜色宏定义
*/
#define WTYSDK_CARCOLOR_UNKNOWN         0			///未知类型
#define WTYSDK_CARCOLOR_BLUE			1			///蓝色
#define WTYSDK_CARCOLOR_YELLOW			2			///黄色
#define WTYSDK_CARCOLOR_WHITE			3			///白色
#define WTYSDK_CARCOLOR_BLACK			4			///黑色
#define WTYSDK_CARCOLOR_GREEN			5			///绿色


/*
*	定义用户、车辆信息结构体里面的数组大小
*/
#define WTYSDK_WLIST_CUSTOMER_NAME_LEN	20   //用户名字符串长度
#define WTYSDK_WLIST_CUSTOMER_CODELEN	20	 //用户编码字符串长度

#define WTYSDK_WLIST_VEHICLE_PLATE_LEN 16	//车牌字符串长度
#define WTYSDK_WLIST_VEHICLE_CODE_LEN  20	//车辆编码字符串长度
#define WTYSDK_WLIST_VEHICLE_TIME_LEN  20	//时间信息字符串长度
#define WTYSDK_WLIST_HTTP_SELECTTRAFFIC_IP_DOMAIN 50    // http 查询 ip 域名长度
#define WTYSDK_WLIST_TIME_SEG_MAX_NUM	8	//时间段字符串长度
#define WTYSDK_WLIST_PERPAGE_MAXSIZE    100 //查询或者导入每页最大值
#define WTYSDK_WLIST_PIC_SAVEPATH     100	//保存图片路径字符串长度
#define WTYSDK_WLIST_STOREMAXVALUE    30000 //白名单保存用户和车辆信息最大值
/*
*	定义客户信息、用户信息、车辆通行信息结构体
*/
//黑白名单记录的客户信息
typedef struct
{
	unsigned	uCustomerID;			//<客户在数据库的ID，用于修改、删除操作（主键）
	char		strName[WTYSDK_WLIST_CUSTOMER_NAME_LEN];	//<客户姓名
	char		strCode[WTYSDK_WLIST_CUSTOMER_CODELEN];	//<客户编码,唯一编码

	char		reserved[256];//保留
}
WTYSDK_WLIST_CUSTOMER;

//---以下一些做api函数参数的定义

// 黑白名单客户信息查询条件
typedef struct{
	int 	UserID;											//客户ID
	char	UserName[WTYSDK_WLIST_CUSTOMER_NAME_LEN];		//客户名字
	char 	UserSerial[WTYSDK_WLIST_CUSTOMER_CODELEN];		//客户编号
	int 	page;											//从0开始算第一页，依次类推
	int 	pageSize;										//每页查询的数目
	char	reserved[100];
}
WTYSDK_CUSTOMERQUERYCONDITION;


//黑白名单中的车辆记录
typedef struct
{
	unsigned	uVehicleID;										//<车辆在数据库的ID
	char		strPlateID[WTYSDK_WLIST_VEHICLE_PLATE_LEN];			//<车牌字符串
	unsigned	uCustomerID;									//<客户在数据库的ID，与WTYSDK_WLIST_CUSTOMER::uCustomerID对应
	unsigned	bEnable;										//<该记录有效标记
	//unsigned	bEnableTMEnable;								//<是否开启生效时间
	//unsigned	bEnableTMOverdule;								//<是否开启过期时间
	char		struTMCreate[WTYSDK_WLIST_VEHICLE_TIME_LEN];									//<该记录创建时间
	char		struTMEnable[WTYSDK_WLIST_VEHICLE_TIME_LEN];									//<该记录生效时间
	char		struTMOverdule[WTYSDK_WLIST_VEHICLE_TIME_LEN];									//<该记录过期时间
	unsigned	bUsingTimeSeg;									//是否使用时间段匹配0不启用1
	char        struTimeSeg[256];									//<周期时间段信息
	int 		WeekTimeMatch;									//是否在时间段内使用周期时间段匹配0不启用1启用
	//int 	TimeMatch;		是否使用时间段匹配0不启用1
	unsigned	bAlarm;											//<是否触发报警（黑名单记录）
	int			iColor;											//<车辆颜色
	int			iPlateType;										//<车牌类型
	int         iBlackList;										//是否黑名单 0不是 1是
	char		strCode[WTYSDK_WLIST_VEHICLE_CODE_LEN];			//<车辆编码
	char		strRemark[100];									//车辆备注说明，可以为空
	char		reserved[256];									//保留
}
WTYSDK_WLIST_VEHICLE;
//车辆 信息查询条件
typedef struct{
	int     PlateinfoID;		//车辆ID
	int 	UserID;				//用户ID
	char	UserName[20];		//客户名字
	char 	UserSerial[20];		//客户编号
	char 	PlateNumber[16];	//车牌号
	char 	CarSerial[20];		//车辆代码
	int 	Blacklist;
	int 	conditionsflag;		//查询条件设置0 根据用户ID查询 1根据用户名查询 2根据用户编码 3根据车辆ID 4根据车牌号 5根据车辆代码  6查询所有
	int 	page;				//查询页
	int 	pageSize;			//每页大小，最大值100
	char	reserved[256];
}
WTY_SDK_VEHICLEQUERYRECORD;


//车辆通行信息
typedef struct
{
	int iTrafficId;												//车辆通行ID，自增
	int iVehicleID;												//车辆ID
	int iCustomerID;											//用户ID
	char strPlateNumber[WTYSDK_WLIST_VEHICLE_PLATE_LEN];		//车牌字符串
	char strPassTime[WTYSDK_WLIST_VEHICLE_TIME_LEN];			//车辆通过时间
	int  iCarSigned;											//车辆标示，0外来车，1内部车
	int iIsOpenSigno;											//是否开启道闸， 0未抬杆，1已抬杆
	char strPicSavePath[WTYSDK_WLIST_PIC_SAVEPATH];				//图片保存路径
	char		reserved[256];									//保留
}
WTYSDK_WLIST_TRAFFIC_INFO;
//Http记录信息
typedef struct
{
	int		TrafficID;				///*ID号：相机自己生成，无需客户指定 */
	char  	PlateNumber[16];		///*车牌号码*/
	char	RecDateTime[20];		///*识别时间*/
	char	TransDateTime[20];		///*发送时间*/
	int 	FullImageSize;			///*大图大小*/
	int 	PlateImageSize;			///*小图大小*/
	int 	IPtype;					///*使用IP还是域名0 IP */
	char 	IP_Domain[50];			///*IP 或 域名*/
	int		Port;					///*端口*/
	char 	Path[200];				///*路径*/
	int 	TransType;				///*传送方式  0 即时推送数据 1 异常数据重传*/
	int 	state;					///*发送结果*/
	int 	ReciveState;				///*接数据状态*/
	int		DoorOpen;				///*是否开闸 0 不开 1 开*/
	int		AbnormalID;				///*续传ID*/
	char		reserved[256];									//保留
}
WTYSDK_WLIST_HTTP_INFO;

//车辆通行信息查询条件
typedef struct
{
	char	StartTime[WTYSDK_WLIST_VEHICLE_TIME_LEN];		//*开始时间
	char 	EndTime[WTYSDK_WLIST_VEHICLE_TIME_LEN];		//*结束时间
	char 	chPlateNumber[WTYSDK_WLIST_VEHICLE_PLATE_LEN];	//*车牌号
	int 	page;											//*当前页数
	int 	pageSize;										//*每页最多查询的大小，100，大于100返回-1
	int     queryFlag;										//*0 时间段查询  1 车牌查询 2 分页查询
	char	reserved[256];
}
WTY_SDK_TRAFFIC_QUERYCONDITION;
//Http记录查询条件
typedef struct
{
	char	StartTime[WTYSDK_WLIST_VEHICLE_TIME_LEN];		//*开始时间
	char 	EndTime[WTYSDK_WLIST_VEHICLE_TIME_LEN];		//*结束时间
	char 	chPlateNumber[WTYSDK_WLIST_VEHICLE_PLATE_LEN];	//*车牌号
	char 	IP[16];		/*IP或者域名*/
	char	Domain[50];//域名
	int     ipOrDomain;//1 ip 2 域名
	int 	state;				/*发送结果*/
	//int     http_select_type;/*查询方式：1：根据ip或者域名查询 可选时间段:2：推送结果查询 可选时间段 3:发送失败重新上传 可选时间段 4:前一天没有上传成功，后一天成功的*/
	int 	page;											//*当前页数
	int 	pageSize;										//*每页最多查询的大小，100，大于100返回-1
	int     queryFlag;										//*0 时间段查询  1 车牌查询 2 分页查询  3域名或者IP查询 4推送结果查询 5发送失败从新上传  6前一天没有成功，后一天成功
	int     timeIsEnabled;///使能是否启用时间段匹配
	char	reserved[256];
}
WTY_SDK_HTTP_QUERYCONDITION;

//返回的信息类型
typedef enum{
	WTYSDK_WLIST_CB_TYPE_NO = 0,								//无效信息
	WTYSDK_WLIST_CB_TYPE_CUSTOMER,								//客户信息
	WTYSDK_WLIST_CB_TYPE_VEHICLE,								//车辆信息
	WTYSDK_WLIST_CB_TYPE_TRAFFIC,								//车辆通行信息
	WTYSDK_WLIST_CB_TYPE_ROWS									//车辆和用户综合信息
}
WTYSDK_WLIST_CB_TYPE;
//车辆和用户信息
typedef struct
{
	//int nSize;													//*批量导入车牌和用户数据条数，每次最多100条
	WTYSDK_WLIST_VEHICLE nCarInfo;								//*批量导入车牌信息结构体数组指针
	WTYSDK_WLIST_CUSTOMER nUserInfo;							//*批量导入用户信息结构体数组指针
}
WTYSDK_IMPORTROWS;
//白名单批量导入失败标志
typedef struct
{
	int		nRow;//导入失败的条数
	char 		Result[WTYSDK_WLIST_STOREMAXVALUE];//0 数据库存储大于上限10000条，-1 用户信心错误，-2 车辆信息错误  -3重复车牌
}
WTYSDK_IMPORT_FAILDREASON;


//车辆通行信息
typedef struct plateTraffic_t {
	int	TrafficID;		/* ID号：相机自己生成，无需客户指定 */
	int	CarID;			/*车辆Id*/
	int 	UserID;			/*用户ID*/
	char  	PlateNumber[16];	/*车牌号码*/
	char	DateTime[20];		/*时间*/
	int 	CarFlag;		/*车辆标识0外来车1内部车*/
	int 	OpenDoor;		/*是否抬杆0未抬杆1已抬杆*/
	char 	ImagePath[100];		/*图片路径*/
	char	 reserved[100];
}plateTraffic_t;
// 车辆白名单信息
typedef struct PlateInfo_t {
	int	CarID;		/* ID号：相机自己生成，无需客户指定 */
	int	UserID;		/*外键，用户的ID号*/
	char	PlateNumber[16];	/*车牌号*/
	char	CarSerial[20];		/*车辆代码*/
	int	PlateType;		/*车牌类型
					0   //未知车牌
					1   //蓝牌
					2   //黑牌
					3   //单排黄牌
					4   //双排黄牌（大车尾牌，农用车）
					5   //警车车牌
					6   //武警车牌
					7   //个性化车牌
					8   //单排军车
					9   //双排军车
					10  //使馆牌
					11  //香港牌
					12  //拖拉机
					13  //澳门牌
					14  //厂内牌
					15  //民航牌
					*/
	int 	Enable;		/*该白名单是否生效 0不生效 1生效*/
	int	CarColor;		/*车辆颜色
					0	// 未知
					1	// 蓝色
					2	// 黄色
					3	// 白色
					4	// 黑色
					5	// 绿色
					*/

	char	CreateTime[20];		/*建档时间*/
	char	StartTime[20];		/*生效时间*/
	char 	Endtime[20];		/*过期时间*/
	int 	TimeMatch;		/*是否使用时间段匹配0不启用1*/
	char 	WeekTime[256];		/*周期时间*/
	int 	WeekTimeMatch;		/*是否在时间段内使用周期时间段匹配0不启用1启用*/
	int 	Blacklist;		/*是否黑名单0不是1是*/
	char	Instructions[100];	/*车辆备注说明，可以为空*/
	char	reserved[100];
}PlateInfo_t;
/*
*  @brief 白名单表、客户信息表、车辆通行信息的查询反馈结果函数
*  @param  [IN] type 回调函数返回的信息类型
*  @param  [IN] pLP 查询的车辆信息记录
*  @param  [IN] pCustomer 查询的客户信息记录
*  @param  [IN] nTotal 单次查询返回的总数
*  @param  [IN] pUserData 回调函数中的上下文
*  @ingroup group_callback
*/
typedef void (*WTYSDK_WLIST_QUERY_CALLBACK)(WTYSDK_WLIST_CB_TYPE type, WTYSDK_WLIST_VEHICLE *pLP,
														   WTYSDK_WLIST_CUSTOMER *pCustomer,
														   WTYSDK_WLIST_TRAFFIC_INFO *pTrafficInfo,
														  int nTotal,
														  void *pUserData);

/*
*  @brief  HTTP的查询反馈结果函数
*  @param  [IN] pTrafficInfo 查询的HTTP信息记录
*  @param  [IN] nTotal 单次查询返回的总数
*  @param  [IN] pUserData 回调函数中的上下文
*  @ingroup group_callback
*/
typedef void (*WTYSDK_WLIST_QUERY_HTTP_CALLBACK)(WTYSDK_WLIST_HTTP_INFO *pTrafficInfo,int nTotal,void *pUserData);



/*
*  @brief 设置白名单表和客户信息表的查询结果回调
*  @param  [IN] func 查询结果回调函数
*  @param  [IN] pUserData 回调函数中的上下文  (用户传入指针)
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，其它表示失败
*  @ingroup group_database
*/
int  __stdcall WTYSDK_WhiteListSetQueryCallBack(WTYSDK_WLIST_QUERY_CALLBACK func, void *pUserData,char *pServerIp);

/*
*  @brief 设置HTTP查询结果回调
*  @param  [IN] func 查询结果回调函数
*  @param  [IN] pUserData 回调函数中的上下文
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，其它表示失败
*  @ingroup group_database
*/
int  __stdcall WTYSDK_WhiteListSetQueryHttpCallBack(WTYSDK_WLIST_QUERY_HTTP_CALLBACK func, void *pUserData,char *pServerIp);


/*
*  @brief  获取所有客户信息记录的条数
*  @param  [IN] pServerIp 服务器IP
*  @return >=0表示成功，<0表示失败
*/
int  __stdcall WTY_WhiteListGetCustomerCount(char *pServerIp);

/*
*  @brief  查询白名单表客户信息数据,客户信息通过回调函数返回
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] numbers 单次每页查询最大值100(1-100)，数据多的话可以分页查询和显示
*  @param  [IN] nPage   查询页数,当前页数从0开始代表第一页
*  @return >=0表示查询到的总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadCustomer(char *pServerIp,int numbers,int nPage);

/*
*  @brief 获取白名单客户信息，通过ID
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] iCustomerId 客户信息的ID
*  @return >=0表示查询到的总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadCustomerById(char *pServerIp,int iCustomerId); // id唯一

/*
*  @brief 获取白名单客户信息，通过客户编码
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strCode 客户信息的唯一编码
*  @param  [IN] numbers 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @param  [IN] nPage   查询页数,当前页数从0开始代表第一页
*  @return >=0表示查询到的总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadCustomerByCode(char *pServerIp,char* strCode,int numbers,int nPage);
/*
*  @brief 获取白名单客户信息，通过客户名称
*  @param  [IN] strName 客户信息的名称
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] numbers 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @param  [IN] nPage   查询页数,当前页数从0开始代表第一页
*  @return >=0表示查询到的总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadCustomerByName(char *pServerIp,char* strName,int numbers,int nPage);
/*
*  @brief 增加用户信息记录
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] pNewCustomer 增加的用户信息的结构体  其中 uCustomerID 为自动生成 填写无用
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListInsertCustomer(char *pServerIp,WTYSDK_WLIST_CUSTOMER  pNewCustomer);
/*
*  @brief 删除白名单客户信息，通过ID
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] iCustomerId 客户信息的ID
*  @return 0表示成功，<0表示失败
*/

int __stdcall WTY_WhiteListDeleteCustomerById(char *pServerIp,int iCustomerId);
/*
*  @brief 删除白名单客户信息，通过用户名
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strName 客户名
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListDeleteCustomerByName(char *pServerIp,char* strName);
/*
*  @brief 删除白名单客户信息，通过客户编码
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strCode 客户信息的唯一编码
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListDeleteCustomerByCode(char *pServerIp,char* strCode);


/*
*  @brief 更新白名单客户信息，通过ID   注：id系统生成，更新时候id必须对应，不能随意填
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] pNewCustomer 客户信息
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListUpdateCustomerById(char *pServerIp,WTYSDK_WLIST_CUSTOMER  pNewCustomer);

/*
*  @brief 更新白名单客户信息，通过用户名  注：id系统生成，更新时候id必须对应，不能随意填
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] pNewCustomer 客户信息
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListUpdateCustomerByName(char *pServerIp,WTYSDK_WLIST_CUSTOMER pNewCustomer);

/*
*  @brief 更新白名单客户信息，通过客户编码  id系统生成，更新时候id必须对应，不能随意填  此函数会忽略id，把所有用户编号相同的都改
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] pNewCustomer 客户信息
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListUpdateCustomerByCode(char *pServerIp,WTYSDK_WLIST_CUSTOMER pNewCustomer);
/*
*  @brief  获取所有车辆通行记录的条数
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，<0表示失败
*/
int  __stdcall WTY_WhiteListGetTrafficCount(char *pServerIp);
/*
*  @brief  查询所有车辆通行记录
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] queryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示查询到的总数，<0表示失败
*/
int __stdcall WTY_WhiteListTrafficLoad(char *pServerIp,WTY_SDK_TRAFFIC_QUERYCONDITION queryConditions);

/*
*  @brief  通过车辆通行ID删除车辆通行信息
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] tracfficId 车辆通行记录的ID
*  @return 0表示成功，-1表示失败
*/
int __stdcall WTY_WhiteListTraffiDelete(char *pServerIp,int tracfficId);

/*
*  @brief  清空车辆通行信息
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListTraffiDeleteAll(char *pServerIp);

/*
*  @brief  批量导出车辆通行信息
*  @param  [IN] pServerIp 服务器IP
*  @param  [OUT] pExportBuf 存储查询到的记录缓冲区
*  @param  [OUT] nSucceedCount 存储查询成功的记录总条数
*  @param  [IN] nIndexTotal  想要获取到的记录总数
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListTraffiExportAll(char *pServerIp,WTYSDK_WLIST_TRAFFIC_INFO *pExportBuf,int *nSucceedCount,int nIndexTotal);

/*
*  @brief  获取所有车辆信息记录的条数
*  @param  [IN] pServerIp 服务器IP
*  @return >=0表示成功获取到的数目，<0表示失败
*/
int  __stdcall WTY_WhiteListGetVehicleCount(char *pServerIp);

/*
*  @brief  查询白名单表车辆信息数据,客户信息通过回调函数返回   注：此函数为统查 只关注WTY_SDK_VEHICLEQUERYRECORD 内page 和pagesize
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] queryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示查询到的总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadVehicle(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD queryConditions);
/*
*  @brief 获取白名单车辆信息，通过车牌号码  注： 只关注WTY_SDK_VEHICLEQUERYRECORD 内page 和pagesize 车牌号码   查找车牌号关键字
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] queryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示成功获取车辆总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadVehicleByPlateId(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD queryConditions);
/*
*  @brief 获取白名单车辆信息，通过车辆自增ID
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] queryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示成功获取车牌总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadVehicleById(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD queryConditions);

/*
*  @brief 获取白名单车辆信息，通过车辆编码
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strCode 车辆信息的编码
*  @param  [IN] queryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示成功获取车辆总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadVehicleByCode(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD queryConditions);
/*
*  @brief 获取白名单车辆信息，通过用户ID
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] queryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示成功获取车辆总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadVehicleByUserId(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD queryConditions);

/*
*  @brief 获取白名单车辆信息，通过车辆用户名字
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] queryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示获取到的车辆总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadVehicleByUserName(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD queryConditions);

/*
*  @brief 获取白名单车辆信息，通过用户编码
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strCode 车辆信息的编码
*  @param  [IN] queryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示获取车牌总数，<0表示失败
*/
int __stdcall WTY_WhiteListLoadVehicleByUserCode(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD queryConditions);
/*
*  @brief 更新白名单车辆信息，通过车牌号码
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strVehicleInfo 车辆信息
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListUpdateVehicleByPlateId(char *pServerIp,WTYSDK_WLIST_VEHICLE strVehicleInfo);
/*
*  @brief 更新白名单车辆信息，通过车辆自增ID
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strVehicleInfo 车辆信息
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListUpdateVehicleById(char *pServerIp,WTYSDK_WLIST_VEHICLE  strVehicleInfo);
/*
*  @brief 更新白名单车辆信息，通过车辆编码
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strVehicleInfo 车辆信息
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListUpdateVehicleByCode(char *pServerIp,WTYSDK_WLIST_VEHICLE strVehicleInfo);
/*
*  @brief 增加白名单车辆信息
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strVehicleInfo 车辆信息
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListInsertVehicle(char *pServerIp,WTYSDK_WLIST_VEHICLE strVehicleInfo);

/*
*  @brief 删除白名单车辆信息，通过车牌号码
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strPlateNumber 车牌号码
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListDeleteVehicleByPlateId(char *pServerIp,char* strPlateNumber);

/*
*  @brief 删除白名单车辆信息，通过车辆自增ID
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] iVehicleId 车辆信息自增ID
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListDeleteVehicleById(char *pServerIp,int iVehicleId);

/*
*  @brief 删除白名单车辆信息，通过车辆编码
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] strVehicleCode 车辆信息编码
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListDeleteVehicleByCode(char *pServerIp,char* strVehicleCode);

/*
*  @brief 	清空白名单车辆信息
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListEmptyVehicle(char *pServerIp);

/*
*  @brief  清空所有车辆和用户信息
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListClearCustomerAndVehicles(char *pServerIp);

/*
*  @brief  从数据库删除客户的车辆信息,通过用户ID
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListDeleteVehicleByCustomerId(char *pServerIp ,int userId);

/*
*  @brief  从数据库删除客户以及客户的车辆信息,通过用户ID
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListDeleteVehicleAndUserByCustomerId(char *pServerIp ,int userId);

/*
*  @brief  获取白名单客户表客户和车辆记录的条数
*  @param  [IN] pServerIp 服务器IP
*  @return >=0表示成功，<0表示失败
*/
int  __stdcall WTY_WhiteListGetRowCount(char *pServerIp);
/*
*  @brief  查询白名单表车辆信息数据,客户信息通过回调函数返回
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] nQueryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示查询到的总数，<0表示失败
*/
int  __stdcall WTY_WhiteListLoadRows(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD nQueryConditions);

/*
*  @brief  查询白名单表车辆信息数据,通过回调函数返回
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] nQueryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示查询到的总数，<0表示失败
*/
int  __stdcall WTY_WhiteListLoadRowsCustomerByUserId(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD nQueryConditions);

/*
*  @brief  查询白名单表车辆信息数据,通过回调函数返回
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] nQueryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示查询到的总数，<0表示失败
*/
int  __stdcall WTY_WhiteListLoadRowsCustomerByUserCode(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD nQueryConditions);

/*
*  @brief  查询白名单表车辆信息数据,通过回调函数返回
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] nQueryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示查询到的总数，<0表示失败
*/
int  __stdcall WTY_WhiteListLoadRowsCustomerByUserName(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD nQueryConditions);

/*
*  @brief  通过车牌查询白名单用户信息和车辆信息,通过回调函数返回
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] pCarPlate 查询的车牌号
*  @return >=0表示查询到的总数，<0表示失败
*/
int  __stdcall WTY_WhiteListLoadRowsCustomerAndCarByPlate(char *pServerIp,WTY_SDK_VEHICLEQUERYRECORD nQueryConditions);

/*
*  @brief  批量导入白名单表车辆信息和客户信息
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] nImportRows 单次导入最大值100，数据多的话可以分次导入,大于100返回-1
*  @param  [IN] MyBatchResult 白名单导入失败记录的位置,相应位置的Result值等于1为失败
*  @param  [IN] importMaxValue 单次导入最多100条
*  @return >=0表示成功的总数，<0表示失败
*/
// pImportRows 导入用户和车匹配信息，单次最多100条
// faildReason [out]导入结果(WTYSDK_IMPORT_FAILDREASON参数nRow表示导入错误多少条，Result数组表示每一条的执行情况)
// importMaxValue [int] 表示pImportRows中单次导入的条数--最大100
int  __stdcall WTY_WhiteListImportRows(char *pServerIp,WTYSDK_IMPORTROWS *pImportRows,WTYSDK_IMPORT_FAILDREASON *faildReason,int importMaxValue);
/*
*  @brief  批量导出白名单客户和车辆信息
*  @param  [IN] pServerIp 服务器IP
*  @param  [OUT] nImportRows 导出的数据记录信息结构体指针，大小是结构体大小*nTotal
*  @param  [OUT] pExportSucceed 导出成功的数目
*  @param  [IN] WTY_WhiteListGetRowCount 获取到的记录的总条数
*  @return >=0表示成功的总数，<0表示失败
*/
// pExportRows[out]  导出内容
// pExportSucceed [out] 实际成功导出多少个
// nTotal [in] 需要导出的数量  以此来分页导出，每页最多100
int  __stdcall WTY_WhiteListExportsRows(char *pServerIp,WTYSDK_IMPORTROWS *pExportRows,int *pExportSucceed,int nTotal);


/*
*  @brief  获取所有Http数据库记录的条数
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，<0表示失败
*/
int  __stdcall WTY_WhiteListGetHttpCount(char *pServerIp);
/*
*  @brief  查询Http数据记录
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] queryConditions 单次每页查询最大值100，数据多的话可以分页查询和显示,大于100返回-1
*  @return >=0表示查询到的总数，<0表示失败
*/
int __stdcall WTY_WhiteListHttpLoad(char *pServerIp,WTY_SDK_HTTP_QUERYCONDITION queryConditions,int *nTotal);

/*
*  @brief  通过Http记录ID删除Http记录
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] tracfficId 车辆通行记录的ID
*  @return 0表示成功，-1表示失败
*/
int __stdcall WTY_WhiteListHttpDelete(char *pServerIp,int tracfficId);

/*
*  @brief  清空Http记录
*  @param  [IN] pServerIp 服务器IP
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListHttpDeleteAll(char *pServerIp);

/*
*  @brief  批量导出Http记录
*  @param  [IN] pServerIp 服务器IP
*  @param  [OUT] pExportBuf 存储查询到的记录缓冲区
*  @param  [OUT] nSucceedCount 存储查询成功的记录总条数
*  @param  [IN] nIndexTotal 希望导出的个数
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListHttpExportAll(char *pServerIp,WTYSDK_WLIST_HTTP_INFO *pExportBuf,int *nSucceedCount,int nIndexTotal);

/*
*  @brief  根据车牌查询车辆信息(用于相机traffic)
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] pPlate 车牌号
*  @param  [OUT] pVehicleInfo 车辆信息
*  @return 0表示成功，1表示操作成功但无此车信息，<0表示失败
*/
int __stdcall WTY_WhiteListTrafficRequestByPlate(char *pServerIp, char *pPlate, PlateInfo_t* pVehicleInfo);

/*
*  @brief  写入通行量(用于相机traffic)
*  @param  [IN] pServerIp 服务器IP
*  @param  [IN] pTrafficInfo 要插入的通信信息
*  @return 0表示成功，<0表示失败
*/
int __stdcall WTY_WhiteListTrafficInsertTrafficInfo(char* pServerIp, plateTraffic_t* pTrafficInfo);

#ifdef __cplusplus
}
#endif

#endif
