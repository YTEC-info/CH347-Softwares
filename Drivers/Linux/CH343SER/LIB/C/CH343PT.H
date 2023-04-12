//
//*******************************************
//**  Copyright  (C)  WCH  2004-2023       **
//**  Web:  http://wch.cn                  **
//*******************************************
//**  DLL for USB to uart chip CH34X/CH910X**
//**  C, VC6.0                             **
//*******************************************
//
// USB总线接口芯片端口管理接口库 V1.5
// 南京沁恒微电子
// CH34X-Port-DLL  V1.5
// 运行环境: Windows 98/ME, Windows 2000/XP,Windows 7/8/10/11
// support USB chip: CH34X/CH910X/CH344/CH347/CH348
// USB => Single/Multi serial port
//

#ifndef		_CH343_PORT_H
#define		_CH343_PORT_H

#ifdef __cplusplus
extern "C" {
#endif
	
//WCH串口型号
//未知型号
#define USER_TYPE_Unknown   0xFF
//USB转单串1
#define USER_TYPE_CH341     0x10
//USB转单串2
#define USER_TYPE_CH340     0x20
#define USER_TYPE_CH340K    0x21
#define USER_TYPE_CH330     0x22
#define USER_TYPE_CH9340    0x23
#define USER_TYPE_CH9340K   0x24
#define USER_TYPE_CH9340C   0x25
#define USER_TYPE_CH34E     0x26
#define USER_TYPE_CH34X     0x27
//USB转单串3
#define USER_TYPE_CH343K    0x30
#define USER_TYPE_CH343J    0x31
#define USER_TYPE_CH343G    0x32 //CH343G/P
#define USER_TYPE_CH343P    0x33
//USB转单串4,CH9101U/H相同
#define USER_TYPE_CH9101U   0x40
#define USER_TYPE_CH9101H   0x40
//USB转单串4,CH9101R/Y相同
#define USER_TYPE_CH9101R   0x41
#define USER_TYPE_CH9101Y   0x41
#define USER_TYPE_CH9101N   0x42
//USB转单串5
#define USER_TYPE_CH9102F   0x50
#define USER_TYPE_CH9102X   0x51
//USB转单串6
#define USER_TYPE_CH9103M   0x60

//USB转双串
#define USER_TYPE_CH342F    0xA0
#define USER_TYPE_CH342K    0xA1
#define USER_TYPE_CH342J    0xA2
#define USER_TYPE_CH342G    0xA3
#define USER_TYPE_CH347T    0xA4
//USB转四串
#define USER_TYPE_CH9344    0xD0
#define USER_TYPE_CH344L    0xD1
#define USER_TYPE_CH344Q    0xD2
#define USER_TYPE_CH9104L   0xD3

//USB转八串
#define USER_TYPE_CH348L     0xE0
#define USER_TYPE_CH348Q     0xE1

// Return value
#define     CH910x_SUCCESS                      0x00
#define     CH910x_INVALID_HANDLE               0x01
#define     CH910x_INVALID_PARAMETER            0x02
#define     CH910x_DEVICE_IO_FAILED             0x03
#define     CH910x_FUNCTION_NOT_SUPPORTED       0x04
#define     CH910x_NOT_INIT                     0x05

// GPIO mask bit definitions
#define     CH910x_GPIO_0         0x0001
#define     CH910x_GPIO_1         0x0002
#define     CH910x_GPIO_2         0x0004
#define     CH910x_GPIO_3         0x0008
#define     CH910x_GPIO_4         0x0010
#define     CH910x_GPIO_5         0x0020
#define     CH910x_GPIO_6         0x0040
#define     CH910x_GPIO_7         0x0080
#define     CH910x_GPIO_8         0x0100
#define     CH910x_GPIO_9         0x0200
#define     CH910x_GPIO_10        0x0400
#define     CH910x_GPIO_11        0x0800
#define     CH910x_GPIO_12        0x1000
#define     CH910x_GPIO_13        0x2000
#define     CH910x_GPIO_14        0x4000
#define     CH910x_GPIO_15        0x8000

//芯片功能信息结构
typedef struct _USBSER_Property
{
	UCHAR    ChipType;              //芯片型号,USER_TYPE_CHxxx	
	CHAR     ChipTypeStr[32];       //芯片型号字符串
	CHAR     FwVerStr[32];          //固件版本字符串
	UCHAR    GpioCount;             //GPIO脚数,如果大于0，此型号有专用GPIO脚或复用脚;为0表示芯片不支持GPIO引脚
	BOOL     IsEmbbedEeprom;        //是否内置EEPROM,如支持，可进行设备信息的配置	
	BOOL     IsSupportMcuBootCtrl;  //是否支持Modem引脚作为103 MCU下载控制
	CHAR     ManufacturerString[64];//USB描述符内厂商字符串
	CHAR     ProductString[64];     //USB描述符内产品字符串
	USHORT   bcdDevice;             //USB描述符内bcdDevice值
	UCHAR    PortIndex;             //单串为0，如多串则为芯片第几个号串口
	BOOL     IsSupportGPIOInit;     //是否支持GPIO上电初始化设置
	CHAR     PortName[32];          //串口号
	ULONG    ResvD[8];              //预留数据
}ChipPropertyS,*pChipPropertyS;

//除CH340和CH341之外的所有USB串口芯片配置EEPROM
typedef struct _USERCFG_343
{
	UCHAR SIG;      //对于 CH340B：内部配置信息有效标志，必须是 5BH。
	//对于 CH340H/S：外部配置芯片有效标志，必须是 53H。
	//对于 CH343??：外部配置芯片有效标志，必须是 53H。
	//其它值则配置无效
	UCHAR MODE;     //串口模式，必须是23H
	UCHAR CFG;      //芯片功能配置字节
	//BIT7：USB设备序列号字符串控制位；1=有效；0=无效；
	//BIT6：USB产品信息字符串控制位；1=有效；0=无效；
	//BIT5：USB厂商信息字符串控制位；1=有效；0=无效；
	//BIT4：CDC模式下是否启用硬件流控；1=启用；0=禁止；
	//BIT3：芯片引脚是否启用EEPROM的默认配置；如果启用则芯片上电后会根据EEPROM中的配置初始化引脚方向及电平状态，以及使能状态。具体配置值见后面说明。 1=启用；0=禁止；
	UCHAR WP;       //内部配置信息写保护标志，为 57H 则只读，否则可改写
	UCHAR Vid[2];   //Vendor ID，厂商识别码，高字节在后，任意值。
	UCHAR Pid[2];   //Product ID，产品识别码，高字节在后，任意值
	UCHAR BCD[2];   //高字节在后,任意值
	UCHAR Power;    //Max Power，USB电流配置字,以2mA为单位的最大电源电流
	UCHAR Attributes;//	USB供电方式、睡眠唤醒等功能配置信息
	//BIT7：必须设置成1；
	//BIT6：1= Self-Powered；
	//0=Bus-Powered；
	//BIT5：1= Remote Wakeup Enable；
	//0= Remote Wakeup Disable；
	//BIT4-BIT0：必须设置成0；
	UCHAR Reserved[4]; //保留字节00H或FFH
	CHAR  SN[24];    //USB设备序列号字符串.首字节是全部字节数(不超过16H)，次字节开始为标准的USB字符串描述符格式(第1个字节为总长度，第2个字节固定为03H，第3个字节开始奇数为Unicode字符,偶数为0x00)，如果不符合上述特征则使用厂商默认说明。
	UCHAR PROD[40];  //USB产品信息字符串.首字节是全部字节数(不超过28H)，次字节开始为标准的USB字符串描述符格式(第1个字节为总长度，第2个字节固定为03H，第3个字节开始奇数为Unicode字符,偶数为0x00)，如果不符合上述特征则使用厂商默认说明。
	UCHAR MAU[40];   //USB厂商信息字符串.首字节是全部字节数(不超过28H)，次字节开始为标准的USB字符串描述符格式(第1个字节为总长度，第2个字节固定为03H，第3个字节开始奇数为Unicode字符,偶数为0x00)，如果不符合上述特征则使用厂商默认说明。
}USERCFG_343,*pUSERCFG_343;

//CH340和CH341
typedef struct _USERCFG_340
{
	UCHAR SIG;        //对于 CH340B：内部配置信息有效标志，必须是 5BH。
	//其它值则配置无效
	UCHAR MODE;       //串口模式，必须是23H 
	UCHAR CFG;        //芯片功能配置字节
	//位 5 用于配置产品序列号字符串：0=有效；1=无效
	UCHAR WP;         //内部配置信息写保护标志，为 57H 则只读，否则可改写
	UCHAR Vid[2];     //Vendor ID，厂商识别码，高字节在后，任意值。设置为 0000H 或 0FFFFH 则 VID 和 PID 使用厂商默认值
	UCHAR Pid[2];     //Product ID，产品识别码，高字节在后，任意值
	UCHAR Reserve1[2];//0x08-0x09保留段
	UCHAR Power;      //Max Power，USB电流配置字,以2mA为单位的最大电源电流
	UCHAR Reserve2[5];//0x0B-0x0F保留段
	CHAR  SN[8];      //Serial Number，产品序列号 ASCII 字符串，长度为 8。首字节不是 ASCII 字符（21H～7FH）则禁用序列号
	UCHAR Reserve3[2];//0x18-0x19保留段
	UCHAR PROD[38];   //对于 CH340B：Product String，产品说明 Unicode 字符串。首字节是全部字节数（不超过 26H），次字节是 03H，之后是 Unicode 字符串，不符合上述特征则使用厂商默认说明	
}USERCFG_340,*pUSERCFG_340;

//USER_XX适用于所有芯片CH34X、CH910X
//获取芯片功能信息,返回芯片型号。未知型号则返回USER_TYPE_Unknown
UCHAR   WINAPI   CH343PT_GetChipProperty(HANDLE	       iPortHandle,   //串口句柄值
										pChipPropertyS ChipProperty); //返回芯片GPIO、EEPROM等功能信息

ULONG	WINAPI	CH343PT_GetVersion( );  // 获得DLL版本号,返回版本号

BOOL	WINAPI CH343PT_HandleIsCH34x(  // 检查已经打开的端口是否为CH341端口
									 HANDLE		iPortHandle );  // 指定已经打开的端口句柄


BOOL	WINAPI	CH343PT_NameIsCH34x(  // 检查指定名称的端口是否为CH341端口
								   PUCHAR		iPortName );  // 指定端口名称,例如"COM3","COM15"等,该端口必须未被占用(指未被其它程序打开)


typedef		VOID	( CALLBACK	* mPCH343PT_NOTIFY_ROUTINE ) (  // 端口设备事件通知回调程序
															  LONG			iDevIndexAndEvent );  // 端口设备序号和事件及当前状态(参考下行说明)
// iDevIndexAndEvent: 正数说明是设备插入事件/已经插入, 负数说明是设备拔出事件/已经拔出, 其绝对值是设备序号(1到255)


BOOL	WINAPI	CH343PT_SetDevNotify(  // 设定端口设备事件通知程序
									PCHAR						iDeviceID,  // 可选参数,指向以\0终止的字符串,指定被监控的设备的ID或者不含序号的主名称,对于串口该参数必须为NULL或者"COM"
									mPCH343PT_NOTIFY_ROUTINE	iNotifyRoutine );  // 指定端口设备事件回调程序,为NULL则取消事件通知,否则在检测到事件时调用该程序
/* 即插即用设备的应用程序编程参考
	1. 主程序启动，默认是禁止数据传输的，只有在确定有CH341端口可用并且打开端口后才进行数据传输
	2. 调用CH343PtSetDevNotify设置插拔监视，如果将来发生CH341端口的插拔事件DLL将会自动调用iNotifyRoutine所指定的子程序或者函数
	3. 从端口1到端口255（正常到20即可）逐个调用CH343PtNameIsCH341确定是否为CH341端口，如果返回是则记忆其端口号并打开后开始传输，如果返回否则休息
	4. 如果iNotifyRoutine收到事件通知，那么可以在保存事件参数后通知主程序处理，也可以在该子程序中处理，
	分析事件参数，如果是正数则说明有一个端口插入，需要打开时应该首先用CH343PtNameIsCH341判断是否为CH341端口，是则记忆其端口号并打开后开始传输，
	如果是负数则说明有一个端口拔出，判断其端口号（用0减去该负数得端口号）是否与记忆的已打开的端口号相同，是则应该及时关闭
	5. 当前已经打开的端口号应该保存为全局变量, 如果端口未打开或者被关闭，那么应用程序应该停止数据传输
	6. 如果事先未用CH343PtNameIsCH34X确定是否为CH34x端口，那么在端口已经打开后，也可以调用CH343PtHandleIsCH34X判断是否为CH343端口
*/

//进入芯片配置模式:300bpsp;可进行非标准波特率配置和配置空间读写
BOOL WINAPI	CH343PT_EnterConfigMode(HANDLE iPortH);

//退出芯片配置模式:300bps
BOOL WINAPI	CH343PT_ExitConfigMode(HANDLE iPortH);

//通过串口读取芯片EERPOM内设备配置数据
BOOL    WINAPI	 CH343PT_ReadDevConfig(HANDLE          iPortH,     // 指定CH341设备序号
									   PULONG          DataLen,    // 要读取的数据长度
									   PUCHAR          DataBuf);    // 数据缓冲区


//通过串口向芯片EERPOM内写入设备的配置数据,写入前需确保写位保护位已关闭。
BOOL    WINAPI	 CH343PT_WriteDevConfig(HANDLE          iPortH,
										ULONG           BufferSize,
										PUCHAR          DataBuf);

//通过串口向芯片内置的EEPROM内写入配置数据，每次只写一个字节
BOOL	WINAPI	CH343PT_WriteCfgEeprom_Byte(HANDLE         iPortH,     // 指定CH341设备序号	
											ULONG			iAddr,      // 指定数据单元的地址
											UCHAR			iByte );    // 待写入的字节数据
//EEPROM字节读
BOOL	WINAPI	CH343PT_ReadCfgEeprom_Byte(  // 从I2C接口读取一个字节数据
										   HANDLE          iPortH,    // 指定CH341设备序号	
										   ULONG			iAddr,     // 指定数据单元的地址
										   PUCHAR			oByte );   // 指向一个字节单元,用于保存读取的字节数据

/*USB转串芯片GPIO配置和操作函数,适用USB转串口系列芯片CH910X*/
/* ***** 谨慎使用GPIO API, 防止修改I/O方向使输入引脚变为输出引脚导致与其它输出引脚之间短路而损坏芯片 ***** */

//初始化GPIO,操作前必须先执行
UCHAR	WINAPI	CH910x_GpioInit(HANDLE		iPortH);


// 配置GPIO的功能和I/O方向
/* ***** 谨慎使用该API, 防止修改I/O方向使输入引脚变为输出引脚导致与其它输出引脚之间短路而损坏芯片 ***** */
UCHAR	WINAPI	CH910x_GpioConfig(
								  HANDLE			iPortH,         // 串口句柄值
								  ChipPropertyS*  iChipProperty,
								  ULONG			iEnable,        // 位使能:以位对应引脚号,位0对应GPIO0,位1对应GPIO1;某位为1,表示iGpioFunc,iSetDirOut对应位有效											 
								  ULONG			iFuncSet,       // 功能设置:某位值为0,表示对应引脚为芯片默认功能使用;为1,表示对应引脚作为GPIO功能使用;
								  ULONG			iSetDirOut);    // GPIO脚方向设置:某位为0表示对应引脚方向为输入;某位为1表示对应引脚方向为输出

//获取GPIO引脚配置:引脚功能、方向、电平。iFuncSet，iSetDirOut，iSetDataOut位序号与GPIO序号对应
UCHAR	WINAPI	CH910x_GetGpioConfig(
									 HANDLE			iPortH,         // 串口句柄值
									 ChipPropertyS   *iChipProperty, // 芯片属性信息，可为NULL
									 PULONG			FuncSet,        // 功能设置:某位值为0,表示对应引脚为芯片默认功能使用;为1,表示对应引脚作为GPIO功能使用;
									 PULONG			SetDirOut,      // GPIO脚方向设置:某位为0表示对应引脚方向为输入;某位为1表示对应引脚方向为输出
									 PULONG          SetDataOut);    // GPIO脚电平值:1表示高电平；0表示低电平

// 读取设备描述符
BOOL	WINAPI	CH343PT_GetDeviceDescr(  // 读取设备描述符
									   HANDLE			iPortHandle,        // 指定CH341设备序号
									   PVOID			oBuffer,            // 指向一个足够大的缓冲区,用于保存描述符
									   PULONG			ioLength );          // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度

// 设置GPIO引脚电平状态
UCHAR	WINAPI	CH910x_GpioSet(  
							   HANDLE			iPortH,         // 串口句柄值
							   ChipPropertyS*  iChipProperty,
							   ULONG			iEnable,        // 数据有效标志,位序号与GPIOx的序号对应,位值为1表示iSetDataOut的对应位数据有效,为0表示数据无效;
							   ULONG			iSetDataOut );  // IO输出数据,如果GPIO方向为输出,那么某位清0时对应引脚输出低电平,某位置1时对应引脚输出高电平

//读取GPIO引脚电平状态
UCHAR	WINAPI	CH910x_GpioGet(   
							   HANDLE			iPortH,      // 串口句柄值
							   ChipPropertyS*   iChipProperty,
							   PULONG			iStatus );   // 引脚状态,其GPIOx序号对应数据位号。如GPIO1，对应iStatus值中的bit1,可用CH910x_GPIO_xx位掩码取值

//使能GPIO上电初始化配置
BOOL  WINAPI	CH910x_EnableGpioEepromConfig(
											  HANDLE		iPortH,             // 串口句柄值
											  BOOL         iEnable); //TRUE:启用，FALSE:禁用

// 配置GPIO上电时的初始状态:默认功能、GPIO方向和电平状态。此配置值存于内置的EEPROM内，上电后芯片会按此配置值进行设置。
// 此配置值请谨慎使用，确认好引脚方向和电平，防止短路。
BOOL  WINAPI	CH910x_SetGpioEepromConfig(  										 
										   HANDLE		iPortH,             // 串口句柄值
										   ULONG		iEnable,            // 位使能:以位对应引脚号,位0对应GPIO0,位1对应GPIO1;某位为1,表示iGpioFunc,iSetDirOut,iSetDataOut对应位有效											 
										   ULONG      iFuncSet,          // 功能设置:某位值为0,表示对应引脚为芯片默认功能使用;为1,表示对应引脚作为GPIO功能使用;
										   ULONG		iSetDirOut,         // GPIO脚方向设置:某位为0表示对应引脚方向为输入;某位为1表示对应引脚方向为输出
										   ULONG		iSetDataOut,        // GPIO输出脚电平状态:某位为1表示高电平，为0表示低电平
										   BOOL       IsImmediatelySet,   // 是否立即设置到EEPROM内
										   PUCHAR     EepromData);        // 生成的GPIO上电配置的EEPROM数据,可以为NULL

//获取Eeprom内GPIO引脚配置:引脚功能、方向、电平。iFuncSet，iSetDirOut，iSetDataOut位序号与GPIO序号对应
UCHAR	WINAPI	CH910x_GetGpioEepromConfig(
										   HANDLE			iPortH,         // 串口句柄值	
										   PULONG			iFuncSet,       // 功能设置:某位值为0,表示对应引脚为芯片默认功能使用;为1,表示对应引脚作为GPIO功能使用;
										   PULONG			iSetDirOut,     // GPIO脚方向设置:某位为0表示对应引脚方向为输入;某位为1表示对应引脚方向为输出
										   PULONG           iSetDataOut);   // GPIO脚电平值:1表示高电平；0表示低电平


//获取GPIO配置：功能和I/O方向，输出脚默认值
UCHAR	WINAPI	CH348_GetGpioConfig(
									HANDLE		    	iPortH,         // 串口句柄值
									ChipPropertyS       *ChipProperty,  // 芯片属性值
									ULONGLONG			*FuncSet,       // 功能设置:某位值为0,表示对应引脚为芯片默认功能使用;为1,表示对应引脚作为GPIO功能使用;
									ULONGLONG			*SetDirOut,     // GPIO脚方向设置:某位为0表示对应引脚方向为输入;某位为1表示对应引脚方向为输出	
									ULONGLONG           *SetDataOut);   // IO输出数据,如果GPIO方向为输出,那么某位清0时对应引脚输出低电平,某位置1时对应引脚输出高电平


// 配置GPIO的功能和I/O方向
//iEnable,iFuncSet,iSetDirOut为64位变量，位序号与GPIO引脚号对应。
//建议使用前先调用CH348_GetGpioConfig获取iFuncSet,iSetDirOut初值后再进行设置
UCHAR	WINAPI	CH348_GpioConfig(
								 HANDLE		    	iPortH,         // 串口句柄值
								 ChipPropertyS*      iChipProperty,  // 芯片属性值
								 ULONGLONG			iEnable,        // 位使能:以位对应引脚号,位0对应GPIO0,位1对应GPIO1;某位为1,表示iGpioFunc,iSetDirOut对应位有效											 
								 ULONGLONG			iFuncSet,       // 功能设置:某位值为0,表示对应引脚为芯片默认功能使用;为1,表示对应引脚作为GPIO功能使用;
								 ULONGLONG			iSetDirOut,     // GPIO脚方向设置:某位为0表示对应引脚方向为输入;某位为1表示对应引脚方向为输出	
								 ULONGLONG           iSetDataOut);    // IO输出数据,如果GPIO方向为输出,那么某位清0时对应引脚输出低电平,某位置1时对应引脚输出高电平

// 设置某个GPIO引脚电平状态,相应引脚需开启GPIO功能
UCHAR	WINAPI	CH348_GpioSet(  
							  HANDLE			iPortH,         // 串口句柄值
							  ChipPropertyS*   iChipProperty,  // 芯片属性值
							  UCHAR	        iGpioIndex,     // GPIO脚序号,0-63
							  UCHAR		    iGpioLevel );   // GPIO电平，1为高电平；1：低电平

// 设置多个GPIO引脚电平状态,相应引脚需开启GPIO功能
UCHAR	WINAPI	CH348_GpioMSet(  
							   HANDLE			iPortH,          // 串口句柄值
							   ChipPropertyS*   iChipProperty,   // 芯片属性值
							   ULONGLONG        iGpioMIndex,     // GPIO序号掩码，位0-63
							   ULONGLONG		iGpioMLevel);     // GPIO电平掩码，与上iGpioMIndex位置对应。1：高电平，0：低电平

// 获取某个GPIO引脚电平状态,相应引脚需开启GPIO功能
UCHAR	WINAPI	CH348_GpioGet(  
							  HANDLE			iPortH,         // 串口句柄值
							  ChipPropertyS*   iChipProperty,  // 芯片属性值
							  UCHAR	        iGpioIndex,     // GPIO脚序号 0-63
							  PUCHAR		    GpioLevel );     // GPIO电平，1为高电平；1：低电平

// 获取多个GPIO引脚电平状态,相应引脚需开启GPIO功能
UCHAR	WINAPI	CH348_GpioMGet(  
							   HANDLE			iPortH,          // 串口句柄值
							   ChipPropertyS*   iChipProperty,   // 芯片属性值
							   ULONGLONG        iGpioMIndex,     // GPIO序号掩码，位0-63
							   ULONGLONG*	    GpioMLevel );    // GPIO电平掩码，与上iGpioMIndex位置对应。1：高电平，0：低电平
//向内置的EEPROM内写入一个字节配置数据
BOOL	WINAPI	CH348_WriteCfgEeprom_Byte(HANDLE        iPortH,     // 串口句柄值
										  ChipPropertyS *ChipProperty,  // 芯片属性值
										  UCHAR			iAddr,      // 指定数据单元的地址
										  UCHAR			iByte );    // 待写入的字节数据

//从内置的EEPROM内读出一个字节配置数据
BOOL	WINAPI	CH348_ReadCfgEeprom_Byte(HANDLE         iPortH,    // 串口句柄值	
										 ChipPropertyS  *ChipProperty,  // 芯片属性值
										 UCHAR			iAddr,     // 指定数据单元的地址
										 PUCHAR			oByte );   // 指向一个字节单元,用于保存读取的字节数据
//通过串口读取芯片EERPOM内设备配置数据
BOOL    WINAPI	 CH348_ReadDevConfig(
									 HANDLE          iPortH,     // 串口句柄值
									 ChipPropertyS  *ChipProperty,  // 芯片属性值
									 PULONG          DataLen,    // 要读取的数据长度
									 PUCHAR          DataBuf);    // 数据缓冲区
//通过串口向芯片EERPOM内写入设备的配置数据,写入前需确保写位保护位已关闭。
BOOL    WINAPI	 CH348_WriteDevConfig(
									  HANDLE          iPortH,				// 串口句柄值
									  ChipPropertyS  *ChipProperty,  // 芯片属性值
									  ULONG           BufferSize,    // 要写入的数据长度 
									  PUCHAR          DataBuf);       // 数据缓冲区
//适用于所有芯片CH340/1
ULONG	WINAPI	CH341PtGetVersion( );  // 获得DLL版本号,返回版本号

BOOL	WINAPI	CH341PtHandleIsCH341(  // 检查已经打开的端口是否为CH341端口
									 HANDLE		iPortHandle );  // 指定已经打开的端口句柄


BOOL	WINAPI	CH341PtNameIsCH341(  // 检查指定名称的端口是否为CH341端口
								   PUCHAR		iPortName );  // 指定端口名称,例如"COM3","COM15"等,该端口必须未被占用(指未被其它程序打开)


typedef		VOID	( CALLBACK	* mPCH341PT_NOTIFY_ROUTINE ) (  // 端口设备事件通知回调程序
															  LONG			iDevIndexAndEvent );  // 端口设备序号和事件及当前状态(参考下行说明)
// iDevIndexAndEvent: 正数说明是设备插入事件/已经插入, 负数说明是设备拔出事件/已经拔出, 其绝对值是设备序号(1到255)


BOOL	WINAPI	CH341PtSetDevNotify(  // 设定端口设备事件通知程序
									PCHAR						iDeviceID,  // 可选参数,指向以\0终止的字符串,指定被监控的设备的ID或者不含序号的主名称,对于串口该参数必须为NULL或者"COM"
									mPCH341PT_NOTIFY_ROUTINE	iNotifyRoutine );  // 指定端口设备事件回调程序,为NULL则取消事件通知,否则在检测到事件时调用该程序
																				   /* 即插即用设备的应用程序编程参考
																				   1. 主程序启动，默认是禁止数据传输的，只有在确定有CH341端口可用并且打开端口后才进行数据传输
																				   2. 调用CH341PtSetDevNotify设置插拔监视，如果将来发生CH341端口的插拔事件DLL将会自动调用iNotifyRoutine所指定的子程序或者函数
																				   3. 从端口1到端口255（正常到20即可）逐个调用CH341PtNameIsCH341确定是否为CH341端口，如果返回是则记忆其端口号并打开后开始传输，如果返回否则休息
																				   4. 如果iNotifyRoutine收到事件通知，那么可以在保存事件参数后通知主程序处理，也可以在该子程序中处理，
																				   分析事件参数，如果是正数则说明有一个端口插入，需要打开时应该首先用CH341PtNameIsCH341判断是否为CH341端口，是则记忆其端口号并打开后开始传输，
																				   如果是负数则说明有一个端口拔出，判断其端口号（用0减去该负数得端口号）是否与记忆的已打开的端口号相同，是则应该及时关闭
																				   5. 当前已经打开的端口号应该保存为全局变量, 如果端口未打开或者被关闭，那么应用程序应该停止数据传输
																				   6. 如果事先未用CH341PtNameIsCH341确定是否为CH341端口，那么在端口已经打开后，也可以调用CH341PtHandleIsCH341判断是否为CH341端口
*/


//根据指定ID获取设备驱动程序信息并返回此设备驱动是否已安装
BOOL	WINAPI	CH343PT_GetUsbDevDrvInfor(USHORT PID,             //芯片USB PID
										  BOOL   *DrvIsInstalled, //是否需要安装驱动
										  PCHAR  DriverInfo);     //芯片驱动信息,可为NULL

BOOL	WINAPI	CH343PT_StartFwUpate(HANDLE			iPortH,        // 指定CH341设备序号
									 ULONG			FwSize);       // 固件长度

//获取设备BCD码
BOOL	WINAPI	CH343PT_GetUsbComDevBcd(HANDLE			iPortH, 
                                        USHORT          *oDevBCD);


/*
有线串口ISP自动下载方案--CH342/3
1.无需外围三极管等逻辑控制电路，使用USB转串口芯片CH342/3，即可实现MCU BOOT和APP等模式的IO控制切换和固件下载.
2.本方案为软件方案，需要编写代码和加载驱动，而CH340方案为纯硬件级方案，兼容原软件，无需二次开发
3.软件端可监测CH342/3的USB插入后自动进行下载，拔出自动关闭端口。
4.可实现CH32xxxx/STM32F103等单片机载时序控制和串口通讯。
5.支持此功能的USB转串口型号:CH343G/CH343P/CH342F
/*
操作步骤
1.打开串口
2.配置MCU启动模式
CH343PT_SetMcuStartupMode
McuSeries: 103MCU/CH32xxxxMCU启动方式:BOOT0高电平有效；RST低电平有效
PinCombMode:0:CTS->MCU BOOT0,RTS->MCU RST,BOOT1=GND
1:CTS->MCU BOOT0,RTS->MCU RST,DSR->MCU BOOT1
3.从系统区(BOOT)启动MCU
CH343PT_McuStartupFromSystem();
4.调用ISP下载函数进行下载
5.从Flash区启动MCU，运行用户程序
CH343PT_McuStartupFromFlash();
6.关闭串口
*/

#define LEVEL_LOW    0
#define LEVEL_HIGH   1
#define IOFUNC_GPI   0
#define IOFUNC_GPO   1
/*
McuSeries: 103MCU/CH32xxxxMCU启动方式:BOOT0高电平有效；RST低电平有效
PinCombMode:串口引脚组合，0:CTS->MCU BOOT0,RTS->MCU RST,BOOT1=GND
1:CTS->MCU BOOT0,RTS->MCU RST,DSR->MCU BOOT1
StartupMode	BOOT0	BOOT1	说明
0         0	     X	    FLASH区启动(运行APP代码)
1         1	     0	    系统存储区启动(运行BOOT代码,串口下载)
2         1	     1	    SRAM区启动
*/
//MCU启动模式配置
UCHAR	WINAPI	WINAPI	CH343PT_SetMcuStartupMode(HANDLE PortH,     //串口序号
												  UCHAR McuSeries,  //MCU系列 0:103 MCU,CH32xxxx系列芯片启动方式
												  UCHAR PinCombMode,
												  //串口引脚组合，0:CTS->MCU BOOT0,RTS->MCU RST,BOOT1=GND
												  //              1:CTS->MCU BOOT0,RTS->MCU RST,DSR->MCU BOOT1
												  ULONG ResetHoldTime); //复位保持时间
//从Flash区启动MCU
UCHAR	WINAPI	CH343PT_McuStartupFromFlash();
//从系统区(BOOT)启动MCU
UCHAR	WINAPI	CH343PT_McuStartupFromSystem();
//从SRAM区启动MCU
UCHAR	WINAPI	CH343PT_McuStartupFromSram();

//设置DTR/RTS在串口未打开时的电平保持值，只在驱动加载时有效
BOOL	WINAPI	CH343PT_SetModemOutLevelOnClosed(HANDLE PortH,
												 BOOL   EnRtsKeepSetOnClosed, //TRUE:为TTL低电平;FALSE:为高电平，为默认值
												 BOOL   EnDtrKeepSetOnClosed);//TRUE,为TTL低电平;FALSE:为高电平，为默认值

/*
有线串口ISP自动下载硬件级方案---CH340
CH340E/C支持DTR增强模式，可用于免外围电路的多模式MCU下载,配置方式见官网CH340DS1.PDF第5.3章
1.电路精简可靠，串口上电、掉电、打开、关闭等操作均不影响MCU运行模式。传统一键下载电路需要2个三极管配合使用以保持BOOT常态低电平以及防止串口有动作时MCU误复位或误进BOOT下载模式。
2.兼容MCU下载软件，无需修改。
3.支持一键下载。利用USB的热插拔特性和CH343PT插拔库函数，可在软件端实现插入后自动下载。
4.提供MSOP10、ESSOP10、SOP8等封装，还可定制封装。
5.本方案为芯片硬件方案，无需二次开发，区别于CH342/3方案。
*/
//无增强扩展
#define DTR_EXFUNC_NULL     0  
//CH340E芯片TNOW脚复用DTR，常态低电平，兼容STM32Fxxx/CH32V/Fxxx系列MCU下载控制，无需外围电路
#define DTR_EXFUNC_MODE_L   1  
//CH340E芯片TNOW脚复用DTR/CH340C OUT脚复用DTR，常态高电平，兼容STM32Fxxx/CH32V/Fxxx系列MCU下载控制，无需外围电路
#define DTR_EXFUNC_MODE_H   2
//获取芯片DTR引脚模式,CH340E/C支持DTR增强模式，可用于免外围电路的多模式MCU下载。
BOOL	WINAPI	CH343PT_GetDTRMode(HANDLE			iPortH,          // 串口句柄
							       UCHAR			*DTRExFuncMode); // 模式DTR_EXFUNC_xxx

#ifdef __cplusplus
}
#endif

#endif		// _CH343_PORT_H
