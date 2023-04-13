#pragma once
// 2021.4
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
// support USB chip: CH342
// USB => Single/Multi serial port
// 

#ifndef		_CH342_PORT_H
#define		_CH342_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

//函数调用顺序
//1.CH342F_GpioInit	CH342F_GetGpioConfig
//2.CH342F_GpioSet/CH342F_GpioGet
//3.CH342F_GpioDeInit

//启用GPIO并初始化,合用此脚的MODEM信号功能失效。使用GPIO前，必须先执行此函数对IO进行初始化：方向，输出初始值。此函数执行后，不能再调串口Modem信号的函数
UCHAR	WINAPI	CH342F_GpioInit(HANDLE	   iPortH,         //串口0/1句柄
	BOOL       IsSetInitBit,                               //初始化时是否需要改变IO方向和电压(输出)
	USHORT     GpioDirBit,                                 //初始化方向位，Bit顺序与GPIO顺序一致。位1为输出，位0为输入
	USHORT     GpioStaBit);                                //初始化输出脚电平状态位，Bit顺序与GPIO顺序一致。当为输入时，对应位值需置为0

//关闭GPIO功能，程序关闭时必须调用此函数，否则影响串口MODEM使用。
UCHAR	WINAPI	CH342F_GpioDeInit(HANDLE	   iPortH);


//GPIO输出脚电平设置
UCHAR  WINAPI	WINAPI	CH342F_GpioSet(HANDLE	  iPortH,         //串口0/1句柄
                               ULONG      iEnable,        //数据有效标志,位序号与GPIOx的序号对应,位值为1表示iSetDataOut的对应位数据有效,为0表示数据无效;
		                       ULONG	  iSetDataOut);   //IO输出电平,如果GPIO方向为输出,那么某位清0时对应引脚输出低电平,某位置1时对应引脚输出高电平	            

//获取GPIO引脚配置:引脚方向、电平。iSetDirOut，iSetDataOut位序号与GPIO序号对应
UCHAR WINAPI	CH342F_GetGpioConfig(HANDLE		iPortH,         //串口0/1句柄
		                                 PULONG			SetDirOut,      // GPIO脚方向设置:某位为0表示对应引脚方向为输入;某位为1表示对应引脚方向为输出
		                                 PULONG         SetDataOut);    // GPIO脚电平值:1表示高电平；0表示低电平

//读取GPIO引脚电平状态
UCHAR	WINAPI	CH342F_GpioGet(HANDLE			iPortH,      //串口0/1句柄
		                       PULONG			iStatus);    // 引脚状态,其GPIOx序号对应数据位号。如GPIO1，对应iStatus值中的bit1,可用CH910x_GPIO_xx位掩码取值

#ifdef __cplusplus
}
#endif

#endif		// _CH342_PORT_H
