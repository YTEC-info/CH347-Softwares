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
// USB bus interface chip port management interface library V1.5
// Nanjing Qinheng Microelectronics Co., Ltd
// CH34X-Port-DLL  V1.5
// Runtime environment: Windows 98/ME, Windows 2000/XP,Windows 7/8/10/11
// support USB chip: CH342
// USB => Single/Multi serial port
// 

#ifndef		_CH342_PORT_H
#define		_CH342_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

//The order of function calls
//1.CH342F_GpioInit	CH342F_GetGpioConfig
//2.CH342F_GpioSet/CH342F_GpioGet
//3.CH342F_GpioDeInit

// Enable GPIO and initialize, the MODEM signal function is invalid which the pin share one . Before using GPIO, the function must be executed first to initialize IO: direction, output initial value. After this function is executed, the function of the serial Modem signal cannot be adjusted.
UCHAR	WINAPI	CH342F_GpioInit(HANDLE	   iPortH,         //UART 0/1 handle
	BOOL       IsSetInitBit,                               //Need or not to change IO direction and voltage(output) when initialization 
	USHORT     GpioDirBit,                                 //Initialize direction bit, Bit order is consistent with GPIO order. Bit 1 is output, Bit 0 is input
	USHORT     GpioStaBit);                                //Initialize output pin level status bit, Bit order is consistent with GPIO order. the corresponding bit value must be set to 0 when input.

//Close GPIO function, must be called when program close, otherwise serial port MODEM is affected.
UCHAR	WINAPI	CH342F_GpioDeInit(HANDLE	   iPortH);


//GPIO output pin level setting 
UCHAR  WINAPI	WINAPI	CH342F_GpioSet(HANDLE	  iPortH,         //UART 0/1 handle
                               ULONG      iEnable,        //Data bit valid flag, bit number corresponding to GPIOx serial number,value of 1 indicates corresponding bit data of iSetDataOut is valid. value of 0 indicates data invalid.
		                       ULONG	  iSetDataOut);   //IO output level£¬If GPIO direction is output, then the bit clear 0 corresponds the pin output low level, the bit set to 1 corresponds the pin output high level            

//Get the GPIO pin configuration: pin direction, level. iSetDirOut, iSetDataOut bit number corresponds to GPIO number
UCHAR WINAPI	CH342F_GetGpioConfig(HANDLE		iPortH,         //UART 0/1 handle
		                                 PULONG			SetDirOut,      // GPIO pin direction configuration: value of the bit is 0 indicates that the corresponding pin direction is input. Value of 1 indicates that the corresponding pin direction is output
		                                 PULONG         SetDataOut);    // GPIO pin level configuration: value of 1 indicates high level, value of 0 indicates low level

//Read GPIO pin level status
UCHAR	WINAPI	CH342F_GpioGet(HANDLE			iPortH,      //UART 0/1 handle
		                       PULONG			iStatus);    // Pin status, GPIOx serial number corresponds to the data bit number. For example, GPIO1 corresponds to bit1 in iStatus. The value can be CH910x_GPIO_xx bit mask

#ifdef __cplusplus
}
#endif

#endif		// _CH342_PORT_H
