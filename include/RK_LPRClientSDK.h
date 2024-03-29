#ifndef _RK_LPRCLIENTSDK_H
#define _RK_LPRCLIENTSDK_H

#if !defined(WIN32) && !defined(__stdcall)
#define __stdcall
#endif

typedef unsigned int RkSdkHandle;
typedef unsigned int RkSerialHandle;

/* ÁœÂ·RS485 */
#define RS485_ROAD_1      1
#define RS485_ROAD_2      2

typedef struct 
{
	int				nYear;					/* Year		*/
	int				nMonth;					/* Month	*/
	int				nDay;					/* Day		*/
	int				nHour;					/* Hour		*/
	int				nMinute;				/* Minute	*/
	int				nSecond;				/* Second	*/
}RK_DeviceTime;

/* Ê¶±ðœá¹û×ø±ê */
typedef struct 
{
	int 			nLeft;					/* left		*/
	int 			nTop;					/* top		*/
	int 			nRight;					/* right	*/
	int 			nBottom;				/* bottom	*/
}RK_PlateLocation;

/* ŸßÌåµÄÊ¶±ðœá¹û */
typedef struct 
{	
	char			license[16];			/* ³µÅÆ×Ö·ûŽ® */
	char			color[8];				/* ³µÅÆÑÕÉ«	  */
	int				nColor;					/* ³µÅÆÑÕÉ«   */
	int				nType;					/* ³µÅÆÀàÐÍ   */
	int				nConfidence;			/* ÕûÅÆ¿ÉÐÅ¶È */
	int				nTime;					/* Ê¶±ðºÄÊ±   */
	RK_PlateLocation	rcLocation;			/* ³µÅÆ×ø±ê   */
	int				nCountry;				/* ¹ú±ð		  */
	unsigned char	nCarBright;				/* ³µµÄÁÁ¶È   */
	unsigned char	nCarColor;				/* ³µµÄÑÕÉ«   */
	unsigned char	nCarLogo;				/* ³µ±êÀàÐÍ   */
	unsigned char	nCarType;				/* ³µÁŸÀàÐÍ   */
	int				nDirection;				/* ³µÅÆ·œÏò	  0 unknown, 1 left, 2 right, 3 up, 4 down*/
	RK_PlateLocation	DetectRect;			/* ÊÓÆµÊ¶±ðŒì²âÇøÓò   */

	unsigned char	nCarModel;				/* ³µ±êÀàÐÍ   */

	char			reserved[128-1];		/* ±£Áô		  */
}RK_RecResultInfo;

typedef struct {
	unsigned char	*pBuffer;			// ÍŒÏñµØÖ·
	unsigned int	imageSize;			// ÍŒÏñŽóÐ¡
	int				imageWidth;			// ÍŒÏñµÄ¿í¶È
	int				imageHeight;		// ÍŒÏñµÄžß¶È
}RK_ImageData;

/* ³µÅÆÊ¶±ðœá¹û*/
typedef struct{
	char				chIp[16];
	RK_DeviceTime		strRecTime;									/* Éè±žÊ¶±ð³µÅÆµÄÊ±Œä					*/
	int					nPlateCount;								/* Ò»·ùÍŒÏñÊ¶±ð³öµÄ³µÅÆžöÊý				*/
	int					trigger_signal;								/* Ž¥·¢ÐÅºÅÀàÐÍ 
																			0:ÎÞŽ¥·¢ÐÅºÅ 
																			1:µØžÐŽ¥·¢ÐÅºÅ 
																			2:ÊÖ¶¯Ž¥·¢ÐÅºÅ 
																			3:ÊÓÆµŽ¥·¢ÐÅºÅ
																	*/
	RK_RecResultInfo	strRecResultItem;							/* Ê¶±ðœá¹ûÐÅÏ¢							*/
	RK_ImageData		fullImage;									/* È«Ÿ°ÍŒ								*/
	RK_ImageData		plateImage;									/* ³µÅÆÐ¡ÍŒ								*/
	RK_ImageData		twoImage;									/* ³µÅÆÐ¡ÍŒ								*/
	
	char				reserved[128];								/* ±£Áô									*/
}RK_PlateResult;

/* ³µÅÆÊ¶±ðœá¹û*/
typedef struct{
	int					nSerialPort;								/* Öž¶šÊ¹ÓÃÉè±žµÄŽ®¿ÚÐòºÅ£º0:±íÊŸµÚÒ»žöŽ®¿Ú£¬1:±íÊŸµÚ¶þžöŽ®¿Ú */
	unsigned char		*pData;										/* œÓÊÕµÄÊýŸÝµÄÊ×µØÖ· */
	unsigned			size;										/* œÓÊÕµÄÊýŸÝµÄ³ßŽç */
	
	char				reserved[128];								/* ±£Áô									*/
}RK_Rs485Result;

/*
GPIO ÊäÈë 
*/
typedef struct  RK_Gpio{
	unsigned int in1;			//GPIO IN1 	0µÍµçÆœ 1 žßµçÆœ
	unsigned int in2;			//GPIO IN2 	0µÍµçÆœ 1 žßµçÆœ
	unsigned int in3;			//GPIO IN3 	0µÍµçÆœ 1 žßµçÆœ
	unsigned int in4;			//GPIO IN4 	0µÍµçÆœ 1 žßµçÆœ
	unsigned int userkey;		//GPIO userkey 	0µÍµçÆœ 1 žßµçÆœ

	unsigned int out1;			//GPIO out1 0µÍµçÆœ 1 žßµçÆœ
	unsigned int out2;			//GPIO out2 0µÍµçÆœ 1 žßµçÆœ
	unsigned int out3;			//GPIO out3 0µÍµçÆœ 1 žßµçÆœ
	unsigned int led1;			//GPIO led1 0µÍµçÆœ 1 žßµçÆœ
	unsigned int led2;			//GPIO led2	0µÍµçÆœ 1 žßµçÆœ

	unsigned int auto_out1;			//GPIO out1 ×Ô¶¯žŽÎ» 0²»×Ô¶¯žŽÎ» 1×Ô¶¯žŽÎ»£šŒäžô1.5Ãë£©
	unsigned int auto_out2;			//GPIO out2 ×Ô¶¯žŽÎ» 0²»×Ô¶¯žŽÎ» 1×Ô¶¯žŽÎ»£šŒäžô1.5Ãë£©
	unsigned int auto_out3;			//GPIO out3 ×Ô¶¯žŽÎ» 0²»×Ô¶¯žŽÎ» 1×Ô¶¯žŽÎ»£šŒäžô1.5Ãë£©
}RK_Gpio;


/*³µÅÆÀàÐÍ*/
typedef struct{
	char			individual;
	char			two_yellow;
	char			arm_police;
	char			two_army;
	char			tractor;
	char			only_tworow_yellow;
	char			embassy;
	char			arm_police2;
	char			factory;
	char			minhang;
	char			consular;
	char			new_energy;
	char			emergency;

	char			reserved[128-1];
}RK_LicensType;

typedef struct{
	int				x;
	int				y;
}RK_Point;

typedef struct{
	RK_Point		point0;
	RK_Point		point1;
	RK_Point		point2;
	RK_Point		point3;

	char			reserved[64];
}RK_PointArea;

// Ê¡·ÝŸÀÕý
typedef struct{
	char			bProvinceCorrect;
	char			default_province[10];
	float			ProvinceRatio;
	int				ProvinceScore;

	char			reserved[64];
}RK_ProvincesCorrect;

/* Ê¶±ðËã·š²ÎÊý */
typedef struct{
	char			en;
	int				min_width;
	int				max_width;
	int				moving_image;
	int				multi_output;
	int				output_order;
	int				lean_correction;
	int				plate_cnt;
	int				car_color;
	int				locate_thr;
	int				ocr_thr;
	int				car_logo;
	int				lot_detect;
	int				shadow;
	int				shield_railing;
	int				improve_speed;
	int				param_type;
	int				output_direction;
	int				no_plate_check;
	int				location_zoom;
	int				capture_delay;
	int				videoRec_delay;
	int				reco_log_save;
	RK_LicensType	plate_type;
	RK_PointArea	reco_area;	// Ê¶±ðÇøÓò
	RK_PointArea	out_area;	// Êä³öÇøÓò
	RK_ProvincesCorrect provinces_correct;

	int				car_model;
	
	char			reserved[256-4];
}RK_AlgoParam;


/* Camera device infomation */
typedef struct 
{
	char 			devname[256];			/* device name		*/
	char			softver[20];			/* software version	*/
	char 			hardver[20];			/* hardware	version	*/
	char 			sysver[20];				/* System	version	*/
	int				sdkport;				/* sdk port			*/
	char 			ip[16];					/* IP address		*/
	char 			gateway[16];			/* gateway address	*/
	char 			netmask[16];			/* Netmask address	*/
	char            dns[16];			    /* DNS				*/
	char 			mac[18];				/* Mac address		*/
	char            roomID[20];				/* RoomID           */
	char            sn[20];					/* SN				*/

	char            algover[20];			/* algo version		*/
	char            type[20];				/* product type		*/

	char			reserved[256-20-20];	/* ±£Áô				*/
}RK_DeviceInfo;


/* sntp of camera device */
typedef struct 
{
	char			en;						/* Whether to enable the network time checks. YES:y NO:n */
	char			serverip[16];			/* Ntp server ip address					 */
	int				serverport;				/* Ntp server port							 */
	int				period;					/* Interval period							 */

	char			reserved[32];			/* ±£Áô		  */
}RK_SntpParam;

/* Manual setting device time */
typedef struct 
{
	int				year;					/* Year		*/
	int				month;					/* Month	*/
	int				day;					/* Day		*/
	int				hour;					/* Hour		*/
	int				minute;					/* Minute	*/
	int				second;					/* Second	*/
}RK_TimeParam;

typedef struct RK_VideoParam{
	int				compressionType;		// ÊÓÆµÑ¹ËõÀàÐÍ 0:H.264, 1:M-JPEG
	int				bitRate;				/* ÊÓÆµÊä³öÂëÂÊ
												0:1920x1080(1080P)
												1:1280x720(720P)
												2:704x576(D1)
												3:352x288(CIF)
											*/
	int				frameRate;				// Ö¡ÂÊ 1~25
	int				quality;				// ÊÓÆµÖÊÁ¿ 0:žß, 1:ÖÐ, 2:µÍ

	char			reserved[64];			/* ±£Áô		  */
}RK_VideoParam;

typedef struct RK_VideoImageParam{
	int				image_brightness;		// ÊÓÆµÁÁ¶È(-100~100)
	int				image_contrast;			// ÊÓÆµ¶Ô±È¶È(-100~100)
	int				image_hue;				// ÊÓÆµÉ«µ÷(-100~100)
	int				image_saturation;		// ÊÓÆµ±¥ºÍ¶È(-100~100)

	char			reserved[64];			/* ±£Áô		  */
}RK_VideoImageParam;

typedef struct _OsdParam{
	int				en;
	int				left;
	int				top;
	char			text[50];

	char			reserved[30];			/* ±£Áô		  */
}_OsdParam;

typedef struct RK_OsdParam{
	char			en;
	_OsdParam		video_osd[2];
	_OsdParam		image_osd[4];

	char			reserved[64];			/* ±£Áô		  */
}RK_OsdParam;

/* IPºÍÓòÃûµÄÑ¡Ôñ */
typedef struct _HttpIpDomain{
	int 				select;	 			// ÅäÖÃÑ¡ÔñIP»¹ÊÇÓòÃûÍÆËÍ 0£ºIP£¬1£ºÓòÃû
	char 				ip[16];				// ·þÎñÆ÷IPµØÖ·
	char 				domain[50];			// ÓòÃû
}_HttpIpDomain;

/* HTTPÍÆËÍÊ¶±ðœá¹ûÅäÖÃÐÅÏ¢ */
typedef struct _HttpRecoResult{
	int  				full_image;			//·¢ËÍÈ«Ÿ°ÍŒ£¬1·¢ËÍ£¬0²»·¢
	int  				plate_image;		//·¢ËÍ³µÅÆÍŒÆ¬£¬1·¢ËÍ£¬0²»·¢
		
	int 				response;			// œÓÊÕÊ¶±ðœá¹û»ØÓŠ°ü 0 ²»œÓÊÕ 1 œÓÊÕ
	int 				response_timeout;	// œÓÊÕÊ¶±ðœá¹û»ØÓŠ°üµÄ³¬Ê±Ê±Œä µ¥Î»Ãë
	int 				response_affirm;	// ·¢ËÍÊ¶±ðœá¹û»ØÓŠ°üµÄÈ·ÈÏ°ü 0 ²»·¢ËÍ 1 ·¢ËÍ
	
	char				reserved[32];
}_HttpRecoResult;

/* HTTPÍÆËÍÐÄÌøÅäÖÃÐÅÏ¢ */
typedef struct _HttpKeepLive{
	int 				enable;				// ÍÆËÍÐÄÌøÊ¹ÄÜ¿ª¹Ø 0:²»ÍÆËÍ 1:·¢ËÍ
	
	int 				response;			// œÓÊÕÐÄÌø»ØÓŠ°ü 0 ²»œÓÊÕ 1 œÓÊÕ
	int 				response_timeout;	// œÓÊÕ»ØÓŠ³¬Ê±Ê±Œä
	int 				response_affirm;		// ·¢ËÍÐÄÌø»ØÓŠ°üµÄÈ·ÈÏ°ü 0 ²»·¢ËÍ 1 ·¢ËÍ
	int 				keeplive_interval;	// ÍÆËÍÐÄÌøŒäžôÊ±Œä µ¥Î»Ãë
	
	char				reserved[32];
}_HttpKeepLive;

//·þÎñÆ÷ÅäÖÃÐÅÏ¢
typedef struct _HttpServerParam
{
	int 				enable;			//Ê¹ÄÜ¿ª¹Ø 0:²»¿ªÆô 1:¿ªÆô
	_HttpIpDomain		ip_domain;
	int  				port;			//·þÎñÆ÷¶Ë¿ÚºÅ£¬Ä¬ÈÏ80
	char 				push_path[100];	//±£ŽæÂ·Ÿ¶
	_HttpRecoResult		reco_result;
	_HttpKeepLive		keeplive;
	
	char				reserved[32];
}_HttpServerParam;
/*****************************************************************/

/*HTTPÅäÖÃ²ÎÊý*/
typedef struct RK_HttpParam
{
	int					offline_continuingly_en;	// ¶ÏÍøÐøŽ«ÊÇ·ñÆôÓÃ
	int					direction;					// ³öÈë¿Ú·œÏò±êÊ¶£¬0Èë¿Ú£¬1³ö¿Ú
	char				park_id[20];				// Í£³µ³¡ID£¬Ä¬ÈÏ3
	int					push_num;					// ÍÆËÍÊ§°Ü£¬×î¶à¿ÉÒÔÁ¬ÐøÍÆËÍµÄŽÎÊý,×î¶à5ŽÎ
	char				park_name[40];				// ³µ³¡Ãû×Ö
	char				park_location[20];			// ³µ³¡Î»ÖÃ
	int					data_type; 					/* ÍÆËÍÊýŸÝµÄžñÊœÀàÐÍ£º
														0 È«ÊýŸÝÄ£Êœ  
														1 ÆÕÍšÄ£Êœ  
														2 Ÿ«ŒòÄ£Êœ  
														3 Œ«ŒòÄ£Êœ
													*/
	_HttpServerParam 	main_server;
	_HttpServerParam 	standby_server1;
	_HttpServerParam 	standby_server2;
	
	char				reserved[64];
}RK_HttpParam;

//服务器配置信息
typedef struct http_server_info_s
{
	 int 				enable;			//使能开关 0:不开启 1:开启
	char			    url_string[1024];
	int 				keepalive_enable;
	int 				keeplive_interval;	// 推送心跳间隔时间 单位秒
	int  				io_report_enable;
	int  				full_image_report_enable;
	int  				plate_image_report_enable;

	char	 			reserved[32];
} http_server_info_s;
/*HTTP配置参数*/
typedef struct http_param_s{
	int					offline_continuingly_en;		// 断网续传是否启用
	int					direction;						// 出入口方向标识，0入口，1出口
	char				park_id[20];					// 停车场ID，默认3
	int					push_num;						// 推送失败，最多可以连续推送的次数,最多5次
	char				park_name[40];					// 车场名字
	char				park_location[20];			    // 车场位置
	int					data_type; 							/* 推送数据的格式类型：
															0 全数据模式  
															1 普通模式  
															2 精简模式  
															3 极简模式
														*/
	 int session_timeout;								//会话超时时间
	 int characters_type;								//汉字字符编码 0 GB2312 1 UTF-8
	 int is_ssl_connect;								//是否启用ssl
	 int http_control_enable;							//http控制输出是否启用
	 int rs485_delay;									//rs485单条透传的延时
	 int response_affirm_enable;						//回应确认包使能
	 http_server_info_s		main_server;
	 http_server_info_s 	standby_server[2];
	char				reserved[64];
} http_param_s;














/*RS485*/
typedef struct _RS485
{
	int					enable;	
	int					baudrate;					
	char				databit;
	char				stopbit;
	char				checkbit;
	char				workmode;
	
	char				reserved[32];
}_RS485;

/*RS485²ÎÊýÅäÖÃ*/
typedef struct RK_Rs485Param
{
	_RS485				rs485_1;
	_RS485				rs485_2;

	char				reserved[64];
}RK_Rs485Param;

/*RS485Ð­ÒéÅäÖÃ*/
typedef struct RK_Rs485ProtocolParam
{
	int				data_head_size;				// ÊýŸÝÍ·ŽóÐ¡
	int				data_head_byte_1;			// ÊýŸÝÍ·µÚ1žö×ÖœÚ
	int				data_head_byte_2;			// ÊýŸÝÍ·µÚ2žö×ÖœÚ
	int				data_head_byte_3;			// ÊýŸÝÍ·µÚ3žö×ÖœÚ
	int				data_head_byte_4;			// ÊýŸÝÍ·µÚ4žö×ÖœÚ

	int				data_end_size;				// ÊýŸÝÎ²ŽóÐ¡
	int				data_end_type_1;			// ÊýŸÝÎ²µÄµÚ1žö×ÖœÚ
	int				data_end_type_2;			// ÊýŸÝÎ²µÄµÚ2žö×ÖœÚ
	int				data_end_type_3;			// ÊýŸÝÎ²µÄµÚ3žö×ÖœÚ
	int				data_end_type_4;			// ÊýŸÝÎ²µÄµÚ4žö×ÖœÚ

	int 			is_verify;					// ÊÇ·ñ¿ªÆôÐ£Ñé

	int 			data_size;					// ÕûžöÐ­ÒéµÄŽóÐ¡(°üº¬Ð­ÒéÍ·¡¢ÊýŸÝ¡¢Ð­ÒéÎ²)
	int				trigger_type;				// Ž¥·¢ÀàÐÍ(0µØžÐ 1ÊÓÆµ 2Ä£Äâ)
	int				reco_time;					// Ê¶±ðÊ±Œä£¬×Ö·ûŽ®Èç:2017-02-28 21:14:12
	int				have_no_palte;				// ÓÐÎÞ³µÅÆ(AAÓÐ³µÅÆ;BBÎÞ³µÅÆ)
	int				plate_number;				// ÊÇ·ñŽ«Êä³µÅÆºÅ
	int 			plate_color;				// ÊÇ·ñŽ«Êä³µÅÆÑÕÉ«
	int				plate_type;					// ³µÅÆÀàÐÍ
	int				confidence;					// ÖÃÐÅ¶È
	int				reco_use_time;				// Ê¶±ðºÄÊ±
	int 			direction;					// ÔË¶¯·œÏò
	int				location;					// ³µÅÆ×ø±ê
	int				car_color;					// ³µÉíÑÕÉ«
	int             car_bright;					// ³µÉíÁÁ¶È
	int 			car_logo;					// ³µ±ê
	int 			car_type;					// ³µÐÍ
	int				serial;						// ÐòÁÐºÅ
	int				is_vip_plate;				// ÊÇ·ñÎªÄÚ²¿³µ
	int				is_indate;					// ÊÇ·ñÔÚÓÐÐ§ÆÚÄÚ
	int				is_open_door;				// ÊÇ·ñ¿ªÕ¢
	int				whitelist_enable;			// ÊÇ·ñÆôÓÃ°×Ãûµ¥
	int				whitelist_create_time;		// °×Ãûµ¥ŽŽœšÊ±Œä
	int				whitelist_start_time;		// °×Ãûµ¥ÉúÐ§Ê±Œä
	int 			whitelist_end_time;			// °×Ãûµ¥Ê§Ð§Ê±Œä
	int				whitelist_time_match;		// ÊÇ·ñÆôÓÃÊ±Œä¶ÎÆ¥Åä
	int				is_blacklist;				// ÊÇ·ñºÚÃûµ¥

	char			reserved[64];
}RK_Rs485ProtocolParam;


/*LEDÒ»ÌåÏä²ÎÊýÅäÖÃ*/
typedef struct RK_LedBoxParam
{
	int				led_box_type;				/* Ò»ÌåÏäÐÍºÅ 
													0£ºEP-YTX030	(2ÐÐÆÁ)
													1£ºPT-LED03		(2ÐÐÆÁ)
													2£ºEP-G200		(4ÐÐÆÁ£¬»ÛºÅÍš)
													3£ºPT-V500		(4ÐÐÆÁ£¬»ÛºÅÍš)
												*/
	int				welcome_speech;				// »¶Ó­Óï 0:»¶Ó­¹âÁÙ£¬1£ºÒ»Â·Ë³·ç
	char			line_1_show[50];			// ÏÐÊ±µÚ1ÐÐÏÔÊŸÄÚÈÝ
	char			line_2_show[50];			// ÏÐÊ±µÚ2ÐÐÏÔÊŸÄÚÈÝ
	char			line_3_show[50];			// ÏÐÊ±µÚ3ÐÐÏÔÊŸÄÚÈÝ
	char			line_4_show[50];			// ÏÐÊ±µÚ4ÐÐÏÔÊŸÄÚÈÝ

	char			reserved[32];
}RK_LedBoxParam;

/*ÍÑ»ú°×Ãûµ¥¿ØÖÆÌ§žË£¬³µÅÆÆ¥ÅäµÄÄ£Êœ*/
typedef struct RK_LicenseMatchingParam
{			
	int				license_matching_model;		/* ³µÅÆÆ¥ÅäÄ£Êœ£¬
															0£ºÈ«×ÖÆ¥Åä£»
															1£º²»Æ¥Åäºº×ÖŒ°Ê×Î²×Ö·û£»
															2£º²»Æ¥Åäºº×ÖŒ°ÊÕÎ²×Ö·û,Æ¥ÅäÏàËÆ×Ö·û (ÏàËÆ×Ö·û:0-D,8-B,0-Q,E-F) 
												*/
	char			reserved[32];
}RK_LicenseMatchingParam;

/*ÍÑ»ú²ÎÊýÅäÖÃ*/
typedef struct RK_OfflineParam
{
	int							offline_mode;				// ÍÑ»úÄ£Êœ£º0£º±ê×ŒÄ£Êœ£¬1£ºÍÑ»úÄ£Êœ£¬2£ºÊ¶±ðºóÁ¢ŒŽÌ§žË
	RK_LedBoxParam				led_box;					// LEDÒ»ÌåÏäÅäÖÃ²ÎÊý
	RK_LicenseMatchingParam		license_matching;			// ³µÅÆÆ¥Åä²ÎÊý

	char			reserved[64];
}RK_OfflineParam;


/*LED²¹¹âµÆ²ÎÊýÅäÖÃ*/
typedef struct RK_LedParam
{
	unsigned int				on_off;				// ¿ªµÆ¡¢¹ØµÆ£º0£º¹Ø£¬1£º¿ª, 2,×Ô¶¯
	unsigned int				brightness;			// ÁÁ¶ÈÖµ£º1~30000

	char			reserved[64];
}RK_LedParam;

typedef struct _4G_cfg_t{
	int							hw_model;		// Ó²ŒþÐÍºÅ:0:ÎÞ,1:ÒÆÔ¶,2:ÓÐ·œ
	int							_operator;		// ÔËÓªÉÌÍøÂç,1:µçÐÅ;2:ÒÆ¶¯;3ÁªÍš
	char						ping1[32];		// ipµØÖ·,ÓÃpingµÄ·œÊœ,ÀŽÈ·ÈÏÒÑŸ­Á¬ÉÏÍø
	char						ping2[32];		// ipµØÖ·,ÓÃpingµÄ·œÊœ,ÀŽÈ·ÈÏÒÑŸ­Á¬ÉÏÍø

	char						reserved[128];
}_4G_cfg_t;

typedef struct _4G_status_t{
	int							module_insert;		// 4GÄ£¿éÊÇ·ñ²åÈë:0:ÎŽ²åÈë,1:²åÈë
	int							sim_insert;			// SIM¿šÊÇ·ñŽæÔÚ,0:²»ŽæÔÚ,1:ŽæÔÚ
	int							signal_strength;	// ÐÅºÅµÄÇ¿¶È
	int							dialing;			// ÊÇ·ñÒÑŸ­²ŠºÅ³É¹Š,0:»¹ÎŽ²ŠºÅ,1:ÒÑŸ­²ŠºÅ
	int							access_internet;	// ÊÇ·ñ¿ÉÒÔ·ÃÎÊÍâÍø,0:²»¿ÉÒÔ,1:¿ÉÒÔ

	char						reserved[128];
}_4G_status_t;

/*4G²ÎÊýÅäÖÃ*/
typedef struct RK_4GParam
{
	_4G_cfg_t					_4G_cfg;			// 4GµÄÅäÖÃ
	_4G_status_t				_4G_status;			// 4GµÄÊ¹ÓÃ×ŽÌ¬

	char						reserved[128];
}RK_4GParam;


typedef void (__stdcall *RKConnectCallback)(RkSdkHandle handle, int status, void *usePtr);
typedef void (__stdcall *RKRecoResultCallback)(RkSdkHandle handle, RK_PlateResult *plateResult, void *usePtr);
typedef void (__stdcall *RKRs485ResultCallback)(RkSerialHandle handle, RK_Rs485Result *rs485Rsult, void *usePtr);
typedef void (__stdcall *RKConfigResultCallback)(RkSdkHandle handle, RK_Gpio *gpio, void *usePtr);

extern int __stdcall rk_sdk_init();
extern int __stdcall rk_sdk_destory();
extern RkSdkHandle __stdcall rk_sdk_connect_device(char *ip, void *usePtr);
extern int __stdcall rk_sdk_disconnect_device(RkSdkHandle handle);
extern void __stdcall rk_sdk_connEvent(RKConnectCallback callback);
extern void __stdcall rk_sdk_recoResultEvent(RKRecoResultCallback callback);
extern void __stdcall rk_sdk_rs485ResultEvent(RKRs485ResultCallback callback);
extern void __stdcall rk_sdk_configResultEvent(RKConfigResultCallback callback);

// ÉÏÃæµÄÀàËÆº¯Êý£¬¿ÉÒÔÍ¬Ê±ÓÐ¶àžöÁŽÂ·ºÍÏà»úÁ¬œÓ£¬ÏÂÃæ4žöº¯Êý£¬œöÓÃÓÚÅäÖÃ²ÎÊýÁŽÂ·µÄÁ¬œÓ£¬ÓÃÓÚÌáÉýÅäÖÃ²ÎÊý»ñÈ¡µÄËÙ¶È
extern int __stdcall rk_config_init();
extern int __stdcall rk_config_destory();
extern RkSdkHandle __stdcall rk_config_connect_device(char *ip, void *usePtr);
extern int __stdcall rk_config_disconnect_device(RkSdkHandle handle);

// RS485ÍžÃ÷Ž«Êä
extern RkSerialHandle rk_sdk_rs485_connect(RkSdkHandle handle, int serialPort, void *usePtr); //serialPort: RS485_ROAD_1\RS485_ROAD_2
extern int rk_sdk_rs485_SerialSend(RkSerialHandle handle, unsigned char *pdata, int size);
extern int rk_sdk_rs485_disconnect(RkSerialHandle handle);

// Ê¶±ð²ÎÊýÉèÖÃ
extern int __stdcall rk_sdk_config_algo_get(RkSdkHandle handle, RK_AlgoParam *algo);
extern int __stdcall rk_sdk_config_algo_set(RkSdkHandle handle, RK_AlgoParam *algo);
// Éè±žÐÅÏ¢²ÎÊý
extern int __stdcall rk_sdk_config_devinfo_get(RkSdkHandle handle, RK_DeviceInfo *devinfo);
extern int __stdcall rk_sdk_config_devinfo_set(RkSdkHandle handle, RK_DeviceInfo *devinfo);
// Ê±Œä²ÎÊý
extern int __stdcall rk_sdk_config_time_set(RkSdkHandle handle, RK_TimeParam *timeinfo);
extern int __stdcall rk_sdk_config_sntp_get(RkSdkHandle handle, RK_SntpParam *sntpinfo);
extern int __stdcall rk_sdk_config_sntp_set(RkSdkHandle handle, RK_SntpParam *sntpinfo);
// ÊÓÆµÂëÁ÷²ÎÊý
extern int __stdcall rk_sdk_config_video_get(RkSdkHandle handle, RK_VideoParam *videoinfo);
extern int __stdcall rk_sdk_config_video_set(RkSdkHandle handle, RK_VideoParam *videoinfo);
// ÊÓÆµÍŒÏñ²ÎÊý
extern int __stdcall rk_sdk_config_video_image_param_get(RkSdkHandle handle, RK_VideoImageParam *videoinfo);
extern int __stdcall rk_sdk_config_video_image_param_set(RkSdkHandle handle, RK_VideoImageParam *videoinfo);
// GPIOÊä³öÉèÖÃ
extern int __stdcall rk_sdk_config_gpio_out_set(RkSdkHandle handle, RK_Gpio *gpio);
// OSD²ÎÊý
extern int __stdcall rk_sdk_config_osd_get(RkSdkHandle handle, RK_OsdParam *osdinfo);
extern int __stdcall rk_sdk_config_osd_set(RkSdkHandle handle, RK_OsdParam *osdinfo);
// HTTP²ÎÊý
extern int __stdcall rk_sdk_config_http_get(RkSdkHandle handle, RK_HttpParam *httpinfo);
extern int __stdcall rk_sdk_config_http_set(RkSdkHandle handle, RK_HttpParam *httpinfo);
// RS485²ÎÊý
extern int __stdcall rk_sdk_config_rs485_param_get(RkSdkHandle handle, RK_Rs485Param *rs485info);
extern int __stdcall rk_sdk_config_rs485_param_set(RkSdkHandle handle, RK_Rs485Param *rs485info);
extern int __stdcall rk_sdk_config_rs485_protocol_get(RkSdkHandle handle, RK_Rs485ProtocolParam *rs485Protocol);
extern int __stdcall rk_sdk_config_rs485_protocol_set(RkSdkHandle handle, RK_Rs485ProtocolParam *rs485Protocol);
// ÍÑ»ú²ÎÊý
extern int __stdcall rk_sdk_config_offline_get(RkSdkHandle handle, RK_OfflineParam *offlineinfo);
extern int __stdcall rk_sdk_config_offline_set(RkSdkHandle handle, RK_OfflineParam *offlineinfo);
// ²¹¹âµÆ²ÎÊý
extern int __stdcall rk_sdk_config_led_get(RkSdkHandle handle, RK_LedParam *ledinfo);
extern int __stdcall rk_sdk_config_led_set(RkSdkHandle handle, RK_LedParam *ledinfo);
// ·¢ËÍÄ£ÄâŽ¥·¢ÖžÁî
extern int __stdcall rk_sdk_config_trigger(RkSdkHandle handle);
// ·¢ËÍÇå¿Õ×Ô¶¯Í³ŒÆÊ¡·ÝÖžÁî
extern int __stdcall rk_sdk_config_clear_province(RkSdkHandle handle);
// ×é²¥ŒìË÷Éè±žÐÅÏ¢
extern int __stdcall rk_sdk_config_multicast_search(RK_DeviceInfo devinfo[], int max_num, int *num, int wait_time);
// ×é²¥ÐÞžÄÉè±žÐÅÏ¢
extern int __stdcall rk_sdk_config_multicast_devinfo_set(RK_DeviceInfo devinfo, int wait_time);
// ×é²¥»ÖžŽÄ¬ÈÏÉè±žÐÅÏ¢
extern int __stdcall rk_sdk_config_multicast_devinfo_default_set();
// 4G²ÎÊý
extern int __stdcall rk_sdk_config_4G_get(RkSdkHandle handle, RK_4GParam *_4GParam);
extern int __stdcall rk_sdk_config_4G_set(RkSdkHandle handle, RK_4GParam *_4GParam);


// ÉèÖÃŒÓÃÜÐŸÆ¬µÄÃÜÂë	(key 8žö×ÖœÚ)
extern int __stdcall rk_sdk_config_security_key_set(RkSdkHandle handle, unsigned char *key);
// ÖØÖÃŒÓÃÜÐŸÆ¬µÄÃÜÂëºÍÓÃ»§ÊýŸÝ
extern int __stdcall rk_sdk_config_security_reset(RkSdkHandle handle);
// »ñÈ¡ŒÓÃÜÐŸÆ¬µÄÓÃ»§ÊýŸÝ(key 8žö×ÖœÚ£¬ userdata 24×ÖœÚ)
extern int __stdcall rk_sdk_config_security_userdata_get(RkSdkHandle handle, unsigned char *key, unsigned char *userdata);
// ÉèÖÃŒÓÃÜÐŸÆ¬µÄÓÃ»§ÊýŸÝ(key 8žö×ÖœÚ£¬ userdata 24×ÖœÚ)
extern int __stdcall rk_sdk_config_security_userdata_set(RkSdkHandle handle, unsigned char *key, unsigned char *userdata);

// ÍŒÆ¬±£ŽæÂ·Ÿ¶
extern void __stdcall rk_sdk_set_save_path(char *save_path);

#endif
