// 
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
// support USB chip: CH34X/CH910X/CH344/CH347/CH348
// USB => Single/Multi serial port
//

#ifndef		_CH343_PORT_H
#define		_CH343_PORT_H

#ifdef __cplusplus
extern "C" {
#endif
	
//WCH Serial port Part No.
//Unknown Part No.
#define USER_TYPE_Unknown   0xFF
//USB to single serial port 1
#define USER_TYPE_CH341     0x10
//USB to single serial port 2
#define USER_TYPE_CH340     0x20
#define USER_TYPE_CH340K    0x21
#define USER_TYPE_CH330     0x22
#define USER_TYPE_CH9340    0x23
#define USER_TYPE_CH9340K   0x24
#define USER_TYPE_CH9340C   0x25
#define USER_TYPE_CH34E     0x26
#define USER_TYPE_CH34X     0x27
//USB to single serial port 3
#define USER_TYPE_CH343K    0x30
#define USER_TYPE_CH343J    0x31
#define USER_TYPE_CH343G    0x32 //CH343G/P
#define USER_TYPE_CH343P    0x33
//USB to single serial port 4,CH9101U/H is the same
#define USER_TYPE_CH9101U   0x40
#define USER_TYPE_CH9101H   0x40
//USB to single serial port 4,CH9101R/Y is the same
#define USER_TYPE_CH9101R   0x41
#define USER_TYPE_CH9101Y   0x41
#define USER_TYPE_CH9101N   0x42
//USB to single serial port 5
#define USER_TYPE_CH9102F   0x50
#define USER_TYPE_CH9102X   0x51
//USB to single serial port 6
#define USER_TYPE_CH9103M   0x60

//USB to dual serial ports
#define USER_TYPE_CH342F    0xA0
#define USER_TYPE_CH342K    0xA1
#define USER_TYPE_CH342J    0xA2
#define USER_TYPE_CH342G    0xA3
#define USER_TYPE_CH347T    0xA4
//USB to quad serial ports
#define USER_TYPE_CH9344    0xD0
#define USER_TYPE_CH344L    0xD1
#define USER_TYPE_CH344Q    0xD2
#define USER_TYPE_CH9104L   0xD3

//USB to octal serial ports
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

//Chip Function Information Structure
typedef struct _USBSER_Property
{
	UCHAR    ChipType;              //Chip Mode , USER_TYPE_CHxxx	
	CHAR     ChipTypeStr[32];       //Chip Mode string
	CHAR     FwVerStr[32];          //Firmware version string
	UCHAR    GpioCount;             //The number of GPIO pins, if it is greater than 0, this Part No. has dedicated GPIO pins or multiplex pins; if it is 0, the chip does not support GPIO pins
	BOOL     IsEmbbedEeprom;        //Whether chip has built-in EEPROM, if supported, can configure the device information
	BOOL     IsSupportMcuBootCtrl;  //Whether chip supports Modem pin as 103 MCU download control
	CHAR     ManufacturerString[64];//Manufacturer string in USB descriptor
	CHAR     ProductString[64];     //Product string in USB descriptor
	USHORT   bcdDevice;             //bcdDevice value in USB descriptor
	UCHAR    PortIndex;             //Single serial port is 0, if there are multiple serial ports, it is the serial port number of the chip
	BOOL     IsSupportGPIOInit;     //Whether to support GPIO power-on initialization settings
	CHAR     PortName[32];          //serial port number
	ULONG    ResvD[8];              //Reserved data
}ChipPropertyS,*pChipPropertyS;


//All USB to UART chips configuration EEPROM Area except CH340 and CH341
typedef struct _USERCFG_343
{
	UCHAR SIG;      //For CH340B: Internal configuration information valid flag, must be 5BH.
	//For CH340H/S: External configuration chip valid flag, must be 53H.
	//For CH343: External configuration chip valid flag, must be 53H.
	//Invalid for other value
	UCHAR MODE;     //UART mode, must be 23H
	UCHAR CFG;      //Chip function configuration byte
	//BIT7£ºUSB device serial number string control bit; 1=valid; 0=invalid;
	//BIT6£ºUSB product information string control bit; 1=valid; 0=invalid;
	//BIT5£ºUSB manufacturer information string control bit; 1=valid; 0=invalid;
	//BIT4£ºHardware flow control enable switch in CDC mode; 1=enable; 0=disable;
	//BIT3£ºThe default configuration of pins enable switch of the EEPROM; if enabled, the chip will initialize the pin direction, level state, and enable state according to the configuration in the EEPROM after power-on. The specific configuration values are described later. 1=enable; 0=disable;
	UCHAR WP;       //Internal configuration information write protection flag, if it is 57H, it is read-only, otherwise it can be rewritten
	UCHAR Vid[2];   //Vendor ID, high byte at the behind, any value
	UCHAR Pid[2];   //Product ID, high byte at the behind, any value
	UCHAR BCD[2];   //High byte at the behind, any value
	UCHAR Power;    //Max Power, the maximum supply current in 2mA units
	UCHAR Attributes;//Function configuration information such as USB power supply mode, sleep and wakeup, etc.
	//BIT7£ºMust be set to 1;
	//BIT6£º1=Self-Powered;
	//0=Bus-Powered;
	//BIT5£º1=Remote Wakeup Enable;
	//0= Remote Wakeup Disable;
	//BIT4-BIT0£ºMust be set to 0;
	UCHAR Reserved[4]; //Reserved byte 00H or FFH
	CHAR  SN[24];    //USB device serial number string. The first byte is the total number of bytes (not more than 16H), and the second byte starts with the standard USB string descriptor format (the first byte is the total length, and the second byte is fixed It is 03H, the third byte starts with odd numbers of Unicode characters, and even numbers are 0x00). If the above characteristics are not met, the manufacturer's default instructions are used.
	UCHAR PROD[40];  //USB device product string. The first byte is the total number of bytes (not more than 28H), and the second byte starts with the standard USB string descriptor format (the first byte is the total length, and the second byte is fixed It is 03H, the third byte starts with odd numbers of Unicode characters, and even numbers are 0x00). If the above characteristics are not met, the manufacturer's default instructions are used.
	UCHAR MAU[40];   //USB device manufacturer string. The first byte is the total number of bytes (not more than 28H), and the second byte starts with the standard USB string descriptor format (the first byte is the total length, and the second byte is fixed It is 03H, the third byte starts with odd numbers of Unicode characters, and even numbers are 0x00). If the above characteristics are not met, the manufacturer's default instructions are used.
}USERCFG_343,*pUSERCFG_343;

//CH340 and CH341
typedef struct _USERCFG_340
{
	UCHAR SIG;        //For CH340B: Internal configuration information valid flag, must be 5BH.
	//other value is invalid
	UCHAR MODE;       //Serial port mode, must be 23H
	UCHAR CFG;        //Chip function configuration byte
	//Bit 5 is used to configure the product serial number string: 0=valid; 1=invalid
	UCHAR WP;         //Internal configuration information write protection flag, if it is 57H, it is read-only, otherwise it can be rewritten
	UCHAR Vid[2];     //Vendor ID, high byte is end, any value. Set to 0000H or 0FFFFH implies VID and PID using vendor default value
	UCHAR Pid[2];     //Product ID, high byte is end, any value
	UCHAR Reserve1[2];//0x08-0x09 Reserved
	UCHAR Power;      //Max Power, the maximum supply current in 2mA units
	UCHAR Reserve2[5];//0x0B-0x0F Reserved
	CHAR  SN[8];      //Serial Number, the length of ASCII string is 8, disable the Serial number when the first byte is not ASCII character (21H~7FH)
	UCHAR Reserve3[2];//0x18-0x19 Reserved
	UCHAR PROD[38];   //CH340B£ºProduct String, Unicode string for product description. The first byte is by total bytes (less than 26H), the next byte is 03H, Unicode string after that, using vendor default description when do not meet characteristics above.
}USERCFG_340,*pUSERCFG_340;

//USER_XX works with all chips: CH34X¡¢CH910X
//Get the chip function information and return the Chip Mode , unknown Chip Part. return  USER_TYPE_Unknown
UCHAR   WINAPI   CH343PT_GetChipProperty(HANDLE	       iPortHandle,   //Serial port handle value
										pChipPropertyS ChipProperty); //Return chip GPIO, EEPROM and other functional information

ULONG	WINAPI	CH343PT_GetVersion( );  // Get the DLL version number, return the version number

BOOL	WINAPI CH343PT_HandleIsCH34x(  // Check whether the library supports this opened serial port
									 HANDLE		iPortHandle );  // Specifies an open serial port handle


BOOL	WINAPI	CH343PT_NameIsCH34x(  // Check if the port with the specified name is the CH341 seial port
								   PUCHAR		iPortName );  // Specify the serial port name, such as "COM3", "COM15", etc., the serial port must not be occupied (meaning not opened by other programs)


typedef		VOID	( CALLBACK	* mPCH343PT_NOTIFY_ROUTINE ) (  // Serial port device event notification callback routine
															  LONG			iDevIndexAndEvent );  // Serial port device serial number, event and current status (refer to the description below)
// iDevIndexAndEvent: A positive number indicates a device insertion event/has been inserted, a negative number indicates a device unplug event/has been pulled out, and its absolute value is the device serial number (1 to 255)


BOOL	WINAPI	CH343PT_SetDevNotify(  // Configure the Serial port device event notifier
									PCHAR						iDeviceID,  // Optional parameter, pointing to a string terminated by \0, specifying the ID of the monitored device or the main name without serial number, this parameter must be NULL or "COM" for serial ports
									mPCH343PT_NOTIFY_ROUTINE	iNotifyRoutine );  // Specifies the port device event callback program. If it is NULL, the event notification will be canceled. Otherwise, the program will be called when an event is detected.
/* Application Programming Reference for Plug and Play Devices
	1. When the main program starts, data transmission is prohibited by default. Data transmission is only performed after it is determined that the CH341 serial port is available and the serial port is opened.
	2. Call CH343PtSetDevNotify to set plugging monitoring, if the plugging event of CH341 serial port occurs in the future, the DLL will automatically call the subroutine or function specified by iNotifyRoutine.
	3. From port 1 to port 255 (normally to port 20), call CH343PtNameIsCH341 one by one to determine whether it is the CH341 serial port, if it returns yes, then memorize its serial port number and open it to start transmission, if it returns, otherwise rest.
	4. If iNotifyRoutine receives an event notification, it can notify the main program to process it after saving the event parameters, or it can be processed in this subprogram,
	analyze the event parameters, if it is a positive number, it means that there is a serial port inserted. When you need to open it, you should first use CH343PtNameIsCH341 to determine whether it is the CH341 serial port. If it is, then remember its serial port number and open it to start transmission,
	If it is a negative number, it means that a serial port has been pulled out, and judge whether its serial port number (the port number obtained by subtracting the negative number from 0) is the same as the opened serial port number in memory. If it is, it should be closed in time.
	5. The currently opened serial port number should be saved as a global variable, if the serial port is not opened or closed, the application should stop data transmission.
	6. If CH343PtNameIsCH34X is not used to determine whether it is a CH34x port beforehand, after the serial port has been opened, you can also call CH343PtHandleIsCH34X to determine whether it is a CH343 serial port.
*/

//Enter chip configuration mode: 300bps; non-standard baud rate configuration and configuration space can be read and written
BOOL WINAPI	CH343PT_EnterConfigMode(HANDLE iPortH);

//Exit chip configuration mode: 300bps
BOOL WINAPI	CH343PT_ExitConfigMode(HANDLE iPortH);

//Read the device configuration data in the chip EEPROM through the serial port
BOOL    WINAPI	 CH343PT_ReadDevConfig(HANDLE          iPortH,     // Serial port handle value
									   PULONG          DataLen,    // length of data to read
									   PUCHAR          DataBuf);    // data buffer


//Write the configuration data of the device to the chip EEPROM through the serial port, and make sure that the write protection bit is turned off before writing.
BOOL    WINAPI	 CH343PT_WriteDevConfig(HANDLE          iPortH,
										ULONG           BufferSize,
										PUCHAR          DataBuf);

//Write configuration data to the built-in EEPROM of the chip through the serial port, only one byte at a time
BOOL	WINAPI	CH343PT_WriteCfgEeprom_Byte(HANDLE         iPortH,     // Serial port handle value	
											ULONG			iAddr,      // address of specified data unit
											UCHAR			iByte );    // Byte data to be written
//EEPROM byte read
BOOL	WINAPI	CH343PT_ReadCfgEeprom_Byte(  // Read one byte of data from the I2C interface
										   HANDLE          iPortH,    // Serial port handle value
										   ULONG			iAddr,     // address of specified data unit
										   PUCHAR			oByte );   // Refers to a byte unit used to store the read byte data

/*USB to serial chip GPIO configuration and operation function, suitable for USB to serial chip CH910X*/
/* ***** Use the GPIO API carefully to prevent damage to the chip due to short-circuiting with other output pins by modifying the I/O direction so that the input pin becomes an output pin ***** */

//Initialize GPIO, which must be executed before operation
UCHAR	WINAPI	CH910x_GpioInit(HANDLE		iPortH);


// Configure GPIO function and I/O direction
/* ***** Use the GPIO API carefully to prevent damage to the chip due to short-circuiting with other output pins by modifying the I/O direction so that the input pin becomes an output pin ***** */
UCHAR	WINAPI	CH910x_GpioConfig(
								  HANDLE			iPortH,     // Serial port handle value
								  ChipPropertyS*  iChipProperty,
								  ULONG			iEnable,        // Bit enable: bit corresponds to the pin number, bit 0 corresponds to GPIO0, bit 1 corresponds to GPIO1; a bit is 1, indicating that the corresponding bit of iGpioFunc and iSetDirOut is valid											 
								  ULONG			iFuncSet,       // Function setting: the value of a bit is 0, indicating that the corresponding pin is used by the default function of the chip; if it is 1, it indicates that the corresponding pin is used as a GPIO function;
								  ULONG			iSetDirOut);    // GPIO pin direction setting: a bit of 0 indicates that the corresponding pin direction is input; a bit of 1 indicates that the corresponding pin direction is output

//Get GPIO pin configuration: pin function, direction, level. iFuncSet, iSetDirOut, iSetDataOut bit numbers correspond to GPIO numbers
UCHAR	WINAPI	CH910x_GetGpioConfig(
									 HANDLE			iPortH,         // Serial port handle value
									 ChipPropertyS   *iChipProperty, // Chip attribute information, can be NULL
									 PULONG			FuncSet,        // Function setting: the value of a bit is 0, indicating that the corresponding pin is used by the default function of the chip; if it is 1, it indicates that the corresponding pin is used as a GPIO function;
									 PULONG			SetDirOut,      // GPIO pin direction setting: a bit of 0 indicates that the corresponding pin direction is input; a bit of 1 indicates that the corresponding pin direction is output
									 PULONG          SetDataOut);    // GPIO pin level value: 1 means high level; 0 means low level

// Read device descriptor
BOOL	WINAPI	CH343PT_GetDeviceDescr(  // Read device descriptor
									   HANDLE			iPortHandle,        // Serial port handle value
									   PVOID			oBuffer,            // Points to a buffer large enough to hold the descriptor
									   PULONG			ioLength );          // Points to the length unit, the length to be read when input is the length to be read, and the actual read length after return

// Set GPIO pin level state
UCHAR	WINAPI	CH910x_GpioSet(  
							   HANDLE			iPortH,         // Serial port handle value
							   ChipPropertyS*  iChipProperty,
							   ULONG			iEnable,        // Data valid flag, the bit number corresponds to the serial number of GPIOx, the bit value is 1 means the corresponding bit data of iSetDataOut is valid, and 0 means the data is invalid;
							   ULONG			iSetDataOut );  // IO output data, if the GPIO direction is output, then when a bit is cleared to 0, the corresponding pin outputs a low level, and when a certain bit is 1, the corresponding pin outputs a high level

//Read GPIO pin level status
UCHAR	WINAPI	CH910x_GpioGet(   
							   HANDLE			iPortH,      // Serial port handle value
							   ChipPropertyS*   iChipProperty,
							   PULONG			iStatus );   // Pin state, its GPIOx serial number corresponds to the data bit number. For example, GPIO1, corresponding to bit1 in the iStatus value, can use the CH910x_GPIO_xx bit mask to get the value

//Enable GPIO power-on initialization configuration
BOOL  WINAPI	CH910x_EnableGpioEepromConfig(
											  HANDLE		iPortH,             // Serial port handle value
											  BOOL         iEnable); //TRUE: enable£¬FALSE: disable

// Configure the initial state of the GPIO when powered on: default function, GPIO direction, and level state. This configuration value is stored in the built-in EEPROM, and the chip will be set according to this configuration value after power-on.
// Please use this configuration value with caution, and confirm the pin direction and level to prevent short circuit.
BOOL  WINAPI	CH910x_SetGpioEepromConfig(  										 
										   HANDLE		iPortH,             // Serial port handle value
										   ULONG		iEnable,            // Bit enable: bit corresponds to the pin number, bit 0 corresponds to GPIO0, bit 1 corresponds to GPIO1; a bit is 1, indicating that the corresponding bit of iGpioFunc, iSetDirOut and iSetDataOut is valid											 
										   ULONG      iFuncSet,          // Function setting: the value of a bit is 0, indicating that the corresponding pin is used by the default function of the chip; if it is 1, it indicates that the corresponding pin is used as a GPIO function;
										   ULONG		iSetDirOut,         // GPIO pin direction setting: a bit of 0 indicates that the corresponding pin direction is input; a bit of 1 indicates that the corresponding pin direction is output
										   ULONG		iSetDataOut,        // GPIO pin level value: 1 means high level; 0 means low level
										   BOOL       IsImmediatelySet,   // Whether to set to EEPROM immediately
										   PUCHAR     EepromData);        // Generated EEPROM data of GPIO power-on configuration, can be NULL


//Get GPIO pin configuration in Eeprom: pin function, direction, level. iFuncSet, iSetDirOut, iSetDataOut bit numbers correspond to GPIO numbers
UCHAR	WINAPI	CH910x_GetGpioEepromConfig(
										   HANDLE			iPortH,         // Serial port handle value	
										   PULONG			iFuncSet,       // Function setting: the value of a bit is 0, indicating that the corresponding pin is used by the default function of the chip; if it is 1, it indicates that the corresponding pin is used as a GPIO function;
										   PULONG			iSetDirOut,     // GPIO pin direction setting: a bit of 0 indicates that the corresponding pin direction is input; a bit of 1 indicates that the corresponding pin direction is output
										   PULONG           iSetDataOut);   // GPIO pin level value: 1 means high level; 0 means low level


//Get GPIO configuration: function and I/O direction, output pin default value
UCHAR	WINAPI	CH348_GetGpioConfig(
									HANDLE		    	iPortH,         // Serial port handle value	
									ChipPropertyS       *ChipProperty,  // Chip property value
									ULONGLONG			*FuncSet,       // Function setting: the value of a bit is 0, indicating that the corresponding pin is used by the default function of the chip; if it is 1, it indicates that the corresponding pin is used as a GPIO function;
									ULONGLONG			*SetDirOut,     // GPIO pin direction setting: a bit of 0 indicates that the corresponding pin direction is input; a bit of 1 indicates that the corresponding pin direction is output	
									ULONGLONG           *SetDataOut);   // IO output data, if the GPIO direction is output, then when a bit is cleared to 0, the corresponding pin outputs a low level, and when a certain bit is 1, the corresponding pin outputs a high level


// Configure GPIO function and I/O direction
//iEnable, iFuncSet, iSetDirOut are 64-bit variables, and the bit number corresponds to the GPIO pin number.
//It is recommended to call CH348_GetGpioConfig to obtain iFuncSet before use, and then set the initial value of iSetDirOut
UCHAR	WINAPI	CH348_GpioConfig(
								 HANDLE		    	iPortH,         // Serial port handle value
								 ChipPropertyS*      iChipProperty,  // Chip property value
								 ULONGLONG			iEnable,        // Bit enable: bit corresponds to the pin number, bit 0 corresponds to GPIO0, bit 1 corresponds to GPIO1; a bit is 1, indicating that the corresponding bit of iGpioFunc and iSetDirOut is valid											 
								 ULONGLONG			iFuncSet,       // Function setting: the value of a bit is 0, indicating that the corresponding pin is used by the default function of the chip; if it is 1, it indicates that the corresponding pin is used as a GPIO function;
								 ULONGLONG			iSetDirOut,     // GPIO pin direction setting: a bit of 0 indicates that the corresponding pin direction is input; a bit of 1 indicates that the corresponding pin direction is output	
								 ULONGLONG           iSetDataOut);    // IO output data, if the GPIO direction is output, then when a bit is cleared to 0, the corresponding pin outputs a low level, and when a certain bit is 1, the corresponding pin outputs a high level

// To set the level state of a GPIO pin, the corresponding pin needs to enable the GPIO function
UCHAR	WINAPI	CH348_GpioSet(  
							   HANDLE			iPortH,         // Serial port handle value
    						   ChipPropertyS*   iChipProperty,  // Chip property value
							   UCHAR	        iGpioIndex,     // GPIO pin number, 0-63
							   UCHAR		    iGpioLevel );   // GPIO level, 1 is high level; 0: low level

// To set the level state of a GPIO pin, the corresponding pin needs to enable the GPIO function
UCHAR	WINAPI	CH348_GpioMSet(  
							   HANDLE			iPortH,          // Serial port handle value
    						   ChipPropertyS*   iChipProperty,   // Chip property value
							   ULONGLONG        iGpioMIndex,     // GPIO pin number, 0-63
							   ULONGLONG		iGpioMLevel);     // GPIO level mask, corresponding to the previous iGpioMIndex position. 1: High level, 0: Low level

// To get the level status of a GPIO pin, the corresponding pin needs to enable the GPIO function
UCHAR	WINAPI	CH348_GpioGet(  
							   HANDLE			iPortH,         // Serial port handle value
    						   ChipPropertyS*   iChipProperty,  // Chip property value
							   UCHAR	        iGpioIndex,     // GPIO pin number, 0-63
							   PUCHAR		    GpioLevel );     // GPIO level, 1 is high level; 0: low level

// To get the level status of a GPIO pin, the corresponding pin needs to enable the GPIO function
UCHAR	WINAPI	CH348_GpioMGet(  
							   HANDLE			iPortH,          // Serial port handle value
    						   ChipPropertyS*   iChipProperty,   // Chip property value
							   ULONGLONG        iGpioMIndex,     // GPIO pin number, 0-63
							   ULONGLONG*	    GpioMLevel );    // GPIO level mask, corresponding to the previous iGpioMIndex position. 1: High level, 0: Low level
//Write one byte of configuration data to the built-in EEPROM
BOOL	WINAPI	CH348_WriteCfgEeprom_Byte(HANDLE        iPortH,     // Serial port handle value
										  ChipPropertyS *ChipProperty,  // Chip property value
										  UCHAR			iAddr,      // Address of specified data unit
										  UCHAR			iByte );    // Byte data to be written

//Read one byte of configuration data from the built-in EEPROM
BOOL	WINAPI	CH348_ReadCfgEeprom_Byte(HANDLE         iPortH,    // Serial port handle value
										 ChipPropertyS  *ChipProperty,  // Chip property value
										 UCHAR			iAddr,     // Address of specified data unit
										 PUCHAR			oByte );   // Points to a byte unit used to save the read byte data
//Read the device configuration data in the chip EEPROM through the serial port
BOOL    WINAPI	 CH348_ReadDevConfig(
									 HANDLE          iPortH,     // Serial port handle value
									 ChipPropertyS  *ChipProperty,  // Chip property value
									 PULONG          DataLen,    // Length of data to read
									 PUCHAR          DataBuf);    // Data buffer
//Write the configuration data of the device to the chip EEPROM through the serial port,and make sure that the write protection bit is turned off before writing.
BOOL    WINAPI	 CH348_WriteDevConfig(
									  HANDLE          iPortH,				// Serial port handle value
									  ChipPropertyS  *ChipProperty,  // Chip property value
									  ULONG           BufferSize,    // Length of data to write
									  PUCHAR          DataBuf);       // Data buffer
//All chip models supporting CH340 and CH341
ULONG	WINAPI	CH341PtGetVersion( );  // Get the DLL version number, return the version number

BOOL	WINAPI	CH341PtHandleIsCH341(  // Check whether the opened serial port is CH341 seiral port
									 HANDLE		iPortHandle );  // Specifies an open serial port handle


BOOL	WINAPI	CH341PtNameIsCH341(  // Check if the port with the specified name is the CH341 seial port
								   PUCHAR		iPortName );  // Specify the serial port name, such as "COM3", "COM15", etc., the serial port must not be occupied (meaning not opened by other programs)


typedef		VOID	( CALLBACK	* mPCH341PT_NOTIFY_ROUTINE ) (  // Serial port device event notification callback program
															  LONG			iDevIndexAndEvent );  // Serial port device serial number, event and current status (refer to the description below)
// iDevIndexAndEvent: iDevIndexAndEvent: A positive number indicates a device insertion event/has been inserted, a negative number indicates a device unplug event/has been pulled out, and its absolute value is the device serial number (1 to 255)

BOOL	WINAPI	CH341PtSetDevNotify(  // Configure the Serial port device event notifier
									PCHAR						iDeviceID,  // Optional parameter, pointing to a string terminated by \0, specifying the ID of the monitored device or the main name without serial number, this parameter must be NULL or "COM" for serial ports
									mPCH341PT_NOTIFY_ROUTINE	iNotifyRoutine );  // Specifies the port device event callback program. If it is NULL, the event notification will be canceled. Otherwise, the program will be called when an event is detected.
																				   /* Application Programming Reference for Plug and Play Devices
																					1. When the main program starts, data transmission is prohibited by default. Data transmission is only performed after it is determined that the CH341 serial port is available and the serial port is opened.
																					2. Call CH343PtSetDevNotify to set plugging monitoring, if the plugging event of CH341 serial port occurs in the future, the DLL will automatically call the subroutine or function specified by iNotifyRoutine.
																					3. From port 1 to port 255 (normally to port 20), call CH343PtNameIsCH341 one by one to determine whether it is the CH341 serial port, if it returns yes, then memorize its serial port number and open it to start transmission, if it returns, otherwise rest.
																					4. If iNotifyRoutine receives an event notification, it can notify the main program to process it after saving the event parameters, or it can be processed in this subprogram,
																					analyze the event parameters, if it is a positive number, it means that there is a serial port inserted. When you need to open it, you should first use CH343PtNameIsCH341 to determine whether it is the CH341 serial port. If it is, then remember its serial port number and open it to start transmission,
																					If it is a negative number, it means that a serial port has been pulled out, and judge whether its serial port number (the port number obtained by subtracting the negative number from 0) is the same as the opened serial port number in memory. If it is, it should be closed in time.
																					5. The currently opened serial port number should be saved as a global variable, if the serial port is not opened or closed, the application should stop data transmission.
																					6. If CH343PtNameIsCH34X is not used to determine whether it is a CH34x port beforehand, after the serial port has been opened, you can also call CH343PtHandleIsCH34X to determine whether it is a CH343 serial port.
*/


//Get device driver information based on the specified ID and return whether the device driver is installed
BOOL	WINAPI	CH343PT_GetUsbDevDrvInfor(USHORT PID,             //Chip USB PID
										  BOOL   *DrvIsInstalled, //Whether need to install a driver
										  PCHAR  DriverInfo);     //Chip driver information,can be NULL

BOOL	WINAPI	CH343PT_StartFwUpate(HANDLE			iPortH,        // Specify the serial number of the CH341 device
									 ULONG			FwSize);       // Firmware length

//Get the device BCD code
BOOL	WINAPI	CH343PT_GetUsbComDevBcd(HANDLE			iPortH, 
                                        USHORT          *oDevBCD);


/*
The wired serial port ISP automatic download solution
1.Does not require logic control circuits such as peripheral transistors, and uses the USB to serial port chip CH342/3 to realize IO control switching and firmware download in MCU BOOT and APP modes.
2.The solution is a software solution,which requires writing code and loading drivers,while the CH340 solution is a pure hardware-level solution,compatible with the original software,without secondary development.
3.The software side can monitor the USB of CH342/3 and automatically download it after plugging in, and automatically close the port when it is pulled out.
4.It can realize on-board timing control and serial communication of MCU such as CH32xxxx/STM32F103.
5.USB to serial port Part No. that support this function: CH343G/CH343P/CH342F
/*
Operation steps
1.Open serial port
2.Configure MCU boot mode
  CH343PT_SetMcuStartupMode
  McuSeries: 103MCU/CH32xxxxMCU startup mode: BOOT0 active high; RST active low
  PinCombMode:0:CTS->MCU BOOT0,RTS->MCU RST,BOOT1=GND
			    1:CTS->MCU BOOT0,RTS->MCU RST,DSR->MCU BOOT1
3.Boot the MCU from the system area (BOOT)
  CH343PT_McuStartupFromSystem();
4.Use the ISP download function to download
5.Start the MCU from the Flash area and run the user program
  CH343PT_McuStartupFromFlash();
6.Close serial port
*/

#define LEVEL_LOW    0
#define LEVEL_HIGH   1
#define IOFUNC_GPI   0
#define IOFUNC_GPO   1
/*
McuSeries: 103MCU/CH32xxxxMCU startup mode: BOOT0 active high; RST active low
PinCombMode:Serial port pin combination£¬0:CTS->MCU BOOT0,RTS->MCU RST,BOOT1=GND
1:CTS->MCU BOOT0,RTS->MCU RST,DSR->MCU BOOT1
StartupMode	BOOT0	BOOT1	illustrate
0         0	     X	    FLASH area start (run APP code)
1         1	     0	    System storage area startup (run BOOT code, serial port download)
2         1	     1	    SSRAM area start
*/
//MCU Boot Mode Configuration
UCHAR WINAPI WINAPI CH343PT_SetMcuStartupMode(HANDLE PortH,     //Serial port handle value
								UCHAR McuSeries,  //MCU series 0:103 MCU, CH32xxxx series chip startup method
								UCHAR PinCombMode,
								//Serial port pin combination£¬0:CTS->MCU BOOT0,RTS->MCU RST,BOOT1=GND
								//              1:CTS->MCU BOOT0,RTS->MCU RST,DSR->MCU BOOT1
								ULONG ResetHoldTime); //Reset hold time
//Boot the MCU from the Flash area
UCHAR WINAPI CH343PT_McuStartupFromFlash();
//Boot the MCU from the system area (BOOT)
UCHAR WINAPI CH343PT_McuStartupFromSystem();
//Boot MCU from SRAM area
UCHAR WINAPI CH343PT_McuStartupFromSram();

//Set the level hold value of DTR/RTS when the serial port is not open, only valid when the driver is loaded
BOOL WINAPI CH343PT_SetModemOutLevelOnClosed(HANDLE PortH,
									  BOOL   EnRtsKeepSetOnClosed, //TRUE:TTL low level; FALSE: high level, default value
									  BOOL   EnDtrKeepSetOnClosed);//TRUE,TTL low level; FALSE: high level, default value

/*
Wired serial ISP automatic download hardware-level solution---CH340
CH340E/C supports DTR enhanced mode, which can be used for multi-mode MCU download without peripheral circuits. For the configuration method, see chapter 5.3 of the official website CH340DS1.PDF
1. The circuit is simple and reliable, and the serial port power-on, power-off, opening, closing and other operations will not affect the MCU running mode. The traditional one-key download circuit requires two transistors to be used together to keep the BOOT low level normally and prevent the MCU from accidentally resetting or entering the BOOT download mode by mistake when the serial port is active.
2. Compatible with MCU download software without modification.
3. Support one-click download. Using the hot-plug feature of USB and the CH343PT plug-in library function, the software can realize automatic download after plug-in.
4. Provide MSOP10, ESSOP10, SOP8 and other packages, and can also customize packages.
5. This scheme is a chip hardware scheme, and does not require secondary development, which is different from the CH342/3 scheme.
*/
//No enhancement extension
#define DTR_EXFUNC_NULL     0  
//CH340E chip TNOW pin multiplexing DTR, normal low level, compatible with STM32Fxxx/CH32V/Fxxx series MCU download control, no need for peripheral circuits
#define DTR_EXFUNC_MODE_L   1  
//CH340E chip TNOW pin multiplexing DTR/CH340C OUT pin multiplexing DTR, normal high level, compatible with STM32Fxxx/CH32V/Fxxx series MCU download control, no need for peripheral circuits
#define DTR_EXFUNC_MODE_H   2
//Get the chip DTR pin mode, CH340E/C supports DTR enhanced mode, which can be used for multi-mode MCU download without peripheral circuits.
BOOL	WINAPI	CH343PT_GetDTRMode(HANDLE			iPortH,          // Serial port handle
							       UCHAR			*DTRExFuncMode); // DTR_EXFUNC_xxx mode

#ifdef __cplusplus
}
#endif

#endif		// _CH343_PORT_H
