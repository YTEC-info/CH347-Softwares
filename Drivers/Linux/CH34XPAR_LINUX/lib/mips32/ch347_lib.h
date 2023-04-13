#ifndef _CH347LIB_H
#define _CH347LIB_H

#include "ch341_lib.h"

#pragma pack(1)

/* SPI setting structure */
typedef struct _SPI_CONFIG {
	uint8_t iMode; /* 0-3: SPI Mode0/1/2/3 */
	uint8_t iClock; /* 0: 60MHz, 1: 30MHz, 2: 15MHz, 3: 7.5MHz, 4: 3.75MHz, 5: 1.875MHz, 6: 937.5KHz，7: 468.75KHz */
	uint8_t iByteOrder;		/* 0: LSB, 1: MSB */
	uint16_t iSpiWriteReadInterval; /* SPI read and write interval, unit: us */
	uint8_t iSpiOutDefaultData;	/* SPI output data by default while read */
	uint32_t iChipSelect;		/* SPI chip select, valid while BIT7 is 1, 00: CS0, 01: CS1 */
	uint8_t CS1Polarity;		/* BIT0：CS1 polar control, 0：low active, 1：high active */
	uint8_t CS2Polarity;		/* BIT0：CS2 polar control, 0：low active, 1：high active */
	uint16_t iIsAutoDeativeCS;	/* automatically undo the CS after operation completed */
	uint16_t iActiveDelay;		/* delay time of read and write operation after setting CS, unit: us */
	uint32_t iDelayDeactive;	/* delay time of read and write operation after canceling CS, unit: us */
} mSpiCfgS, *mPSpiCfgS;

/* SPI Init structure definition */
typedef struct _SPI_InitTypeDef {
	uint16_t SPI_Direction;		/* Specifies the SPI unidirectional or bidirectional data mode.
                                       This parameter can be a value of @ref SPI_data_direction */
	uint16_t SPI_Mode;		/* Specifies the SPI operating mode.
                                       This parameter can be a value of @ref SPI_mode */
	uint16_t SPI_DataSize;		/* Specifies the SPI data size.
                                       This parameter can be a value of @ref SPI_data_size */
	uint16_t SPI_CPOL;		/* Specifies the serial clock steady state.
                                       This parameter can be a value of @ref SPI_Clock_Polarity */
	uint16_t SPI_CPHA;		/* Specifies the clock active edge for the bit capture.
                                       This parameter can be a value of @ref SPI_Clock_Phase */
	uint16_t SPI_NSS;		/* Specifies whether the NSS signal is managed by
                                       hardware (NSS pin) or by software using the SSI bit.
                                       This parameter can be a value of @ref SPI_Slave_Select_management */
	uint16_t SPI_BaudRatePrescaler; /* Specifies the Baud Rate prescaler value which will be
                                       used to configure the transmit and receive SCK clock.
                                       This parameter can be a value of @ref SPI_BaudRate_Prescaler.
                                       @note The communication clock is derived from the master
                                             clock. The slave clock does not need to be set. */
	uint16_t SPI_FirstBit;		/* Specifies whether data transfers start from MSB or LSB bit.
                                       This parameter can be a value of @ref SPI_MSB_LSB_transmission */
	uint16_t SPI_CRCPolynomial;	/* Specifies the polynomial used for the CRC calculation. */
} SPI_InitTypeDef;

typedef struct _StreamUSBCFG {
	SPI_InitTypeDef SPIInitCfg;
	uint16_t SpiWriteReadInterval; /* SPI read and write interval, unit: us */
	uint8_t SpiOutDefaultData;     /* SPI output data by default while read */
	uint8_t OtherCfg;	       /* misc option
                                    BIT7: CS1 polar control, 0：low active, 1：high active
                                    BIT6：CS2 polar control, 0：low active, 1：high active
                                    BIT5：I2C clock stretch control, 0：disable 1: enable
                                    BIT4：generates NACK or not when read the last byte for I2C operation
                                    BIT3-0：reserved
                                   */
	uint8_t Reserved[4];	       /* reserved */
} StreamHwCfgS, *PStreamHwCfgS;

typedef struct _DEV_INFOR {
	int fd;		   /* device file descriptor */
	FUNCTYPE FuncType; /* 0: TTY uart device(/dev/tty*), 1: HID device(/dev/hidraw*), 2: Vendor device(/dev/ch34xpis*) */
	char DeviceID[64];	     /* USB\VID_xxxx&PID_xxxx */
	uint8_t ChipMode;	     /* work mode
                                  0: Mode0(UART0/UART1)
                                  1: Mode1(UART1+SPI+I2C)
                                  2: Mode2(HID UART1+SPI+I2C)
                                  3: Mode3(UART1+JTAG+I2C)
                                 */
	uint16_t BulkOutEndpMaxSize; /* bulk out endpoint sise */
	uint16_t BulkInEndpMaxSize;  /* bulk in endpoint size */
	uint8_t UsbSpeedType;	     /* USB speed, 0: FS, 1:HS, 2:SS */
	uint8_t CH347IfNum;	     /* USB interface: 0:UART, 1:SPI/I2C/JTAG/GPIO */
	char ProductString[64];	     /* USB product string */
	char ManufacturerString[64]; /* USB manufacturer string */
	uint32_t WriteTimeout;	     /* USB write timeout */
	uint32_t ReadTimeout;	     /* USB read timeout */
	uint8_t FirewareVer;	     /* firmware version */
	uint32_t CmdDataMaxSize;

	mSpiCfgS dllUserSpiCfg;
	StreamHwCfgS dllHwCfg;

	int CMDPKT_DATA_MAX_BITS;
	int MaxBitsPerBulk;
	int MaxBytesPerBulk;

} mDeviceInforS, *mPDeviceInforS;

#pragma pack()

/**
 * CH347OpenDevice - open device
 * @pathname: device path in /dev directory
 *
 * The function return positive file descriptor if successful, others if fail.
 */
extern int CH347OpenDevice(const char *pathname);

/**
 * CH347CloseDevice - close device
 * @fd: file descriptor of device
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347CloseDevice(int fd);

/**
 * CH347ReadData - read data
 * @fd: file descriptor of device
 * @oBuffer: pointer to read buffer
 * @ioLength: pointer to read length
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347ReadData(int fd, void *oBuffer, uint32_t *ioLength);

/**
 * CH347WriteData - write data
 * @fd: file descriptor of device
 * @iBuffer: pointer to write buffer
 * @ioLength: pointer to write length
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347WriteData(int fd, void *iBuffer, uint32_t *ioLength);

/**
 * CH347WriteRead - write data then read
 * @fd: file descriptor of device
 * @iWriteLength: write length
 * @iWriteBuffer: pointer to write buffer
 * @iReadStep: per read length
 * @iReadTimes: read times
 * @oReadLength: pointer to read length
 * @oReadBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347WriteRead(int fd, int iWriteLength, void *iWriteBuffer, int iReadStep, int iReadTimes,
			   uint32_t *oReadLength, void *oReadBuffer);

/**
 * CH347SetTimeout - set USB data read and write timeout
 * @fd: file descriptor of device
 * @iWriteTimeout: data download timeout in milliseconds
 * @iReadTimeout: data upload timeout in milliseconds
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347SetTimeout(int fd, uint32_t iWriteTimeout, uint32_t iReadTimeout);

/**
 * CH347SPI_GetHwStreamCfg - get spi setting from hardware
 * @fd: file descriptor of device
 * @StreamCfg: pointer to SPI stream configuration
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347SPI_GetHwStreamCfg(int fd, StreamHwCfgS *StreamCfg);

/**
 * CH347SPI_Init - SPI interface initialization
 * @fd: file descriptor of device
 * @SpiCfg: pointer to SPI configuration
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347SPI_Init(int fd, mSpiCfgS *SpiCfg);

/**
 * CH347SPI_GetCfg - get SPI configuration
 * @fd: file descriptor of device
 * @SpiCfg: pointer to SPI configuration
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347SPI_GetCfg(int fd, mSpiCfgS *SpiCfg);

/**
 * CH347SPI_SetChipSelect - SPI chip selection initialization
 * @fd: file descriptor of device
 * @iEnableSelect: low 8 bits: CS1, high 8 bits: CS2, byte value -> 1: set CS, 0: ignore CS setting
 * @iChipSelect: low 8 bits: CS1, high 8 bits: CS2, CS output, byte value -> 1: set CS, 0: cancel CS
 * @iIsAutoDeativeCS: low 16 bits: CS1, high 16 bits: CS2, automatically undo the CS after operation completed
 * @iActiveDelay: low 16 bits: CS1, high 16 bits: CS2, delay time of read and write operation after setting CS, unit: us
 * @iDelayDeactive: low 16 bits: CS1, high 16 bits: CS2,, delay time of read and write operation after canceling CS, unit: us
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347SPI_SetChipSelect(int fd, uint16_t iEnableSelect, uint16_t iChipSelect, int iIsAutoDeativeCS,
				   int iActiveDelay, int iDelayDeactive);

/**
 * CH347SPI_ChangeCS - SPI CS setting, must call CH347SPI_Init first
 * @fd: file descriptor of device
 * @iStatus: 0: cancel CS, 1: set CS
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347SPI_ChangeCS(int fd, uint8_t iStatus);

/**
 * CH347SPI_Write - write SPI data
 * @fd: file descriptor of device
 * @ignoreCS: ignore SPI chip select while true, else auto set CS
 * @iChipSelect: SPI chip select, ignore while BIT7 is 0, valid while BIT7 is 1
 * @iLength: length to write
 * @iWriteStep: per write length
 * @ioBuffer: pointer to write buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347SPI_Write(int fd, bool ignoreCS, int iChipSelect, int iLength, int iWriteStep, void *ioBuffer);

/**
 * CH347SPI_Read - read SPI data
 * @fd: file descriptor of device
 * @ignoreCS: ignore SPI chip select while true, else auto set CS
 * @iChipSelect: SPI chip select, ignore while BIT7 is 0, valid while BIT7 is 1
 * @iLength: length to write
 * @oLength: pointer to read length
 * @ioBuffer: pointer to buffer, store data to be written from MOSI, and return data to be read from MISO
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347SPI_Read(int fd, bool ignoreCS, int iChipSelect, int iLength, uint32_t *oLength, void *ioBuffer);

/**
 * CH347SPI_WriteRead - write then read SPI data
 * @fd: file descriptor of device
 * @ignoreCS: ignore SPI chip select while true, else auto set CS
 * @iChipSelect: SPI chip select, ignore while BIT7 is 0, valid while BIT7 is 1
 * @iLength: data length to xfer
 * @ioBuffer: pointer to buffer, store data to be written from MOSI, and return data to be read from MISO
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347SPI_WriteRead(int fd, bool ignoreCS, int iChipSelect, int iLength, void *ioBuffer);

/**
 * CH347Jtag_INIT - JTGA interface initialization, mode and speed setting
 * @fd: file descriptor of device
 * @iClockRate: communication speed, valid value is 0-5, the higher the value, the faster the speed
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_INIT(int fd, uint8_t iClockRate);

/**
 * CH347Jtag_GetCfg - get JTGA speed setting
 * @fd: file descriptor of device
 * @iClockRate: pointer to communication speed, valid value is 0-5, the higher the value, the faster the speed
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_GetCfg(int fd, uint8_t *ClockRate);

/**
 * CH347Jtag_WriteRead - bitband mode JTAG IR/DR data read and write which is applicable for a small amount of data. Exp: command operation, state machine switching and other control transmission. For batch data transmission, it is recommended to use CH347Jtag_ WriteRead_Fast
 * Command packets are read and written in batches in 4096 bytes
 * State machine: Run-Test -> Shift-IR/DR.. -> exit IR/DR -> Run-Test
 * @fd: file descriptor of device
 * @IsDR: true: DR data read and write, false: IR data read and write
 * @iWriteBitLength: write length
 * @iWriteBitBuffer: pointer to write buffer
 * @iReadTimes: read times
 * @oReadBitLength: pointer to read length, returns the actual number of bytes read on success
 * @oReadBitBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_WriteRead(int fd, bool IsDR, int iWriteBitLength, void *iWriteBitBuffer, uint32_t *oReadBitLength,
				void *oReadBitBuffer);

/**
 * CH347Jtag_WriteRead_Fast - JTAG IR/DR data read and write in batches for multi-byte continuous operation. Exp: JTAG firmware download operation. Hardware has a 4K buffer, such as write then read, the length should not exceed 4096 bytes. The buffer size can be adjusted.
 * State machine: Run-Test -> Shift-IR/DR.. -> exit IR/DR -> Run-Test
 * @fd: file descriptor of device
 * @IsDR: true: DR data read and write, false: IR data read and write
 * @iWriteBitLength: write length
 * @iWriteBitBuffer: pointer to write buffer
 * @iReadTimes: read times
 * @oReadBitLength: pointer to read length, returns the actual number of bytes read on success
 * @oReadBitBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_WriteRead_Fast(int fd, bool IsDR, int iWriteLength, void *iWriteBuffer, uint32_t *oReadLength,
				     void *oReadBuffer);

/**
 * CH347Jtag_SwitchTapState - switch JTAG state machine
 * @fd: file descriptor of device
 * @TapState: machine state
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_SwitchTapState(int fd, uint8_t TapState);

/**
 * CH347Jtag_ByteWriteDR - JTAG DR write in bytes which used for multi-byte continuous operation. Exp: JTAG firmware download operation.
 * State machine: Run-Test -> Shift-DR.. -> exit DR -> Run-Test
 * @fd: file descriptor of device
 * @iWriteLength: write length
 * @iWriteBuffer: pointer to write buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_ByteWriteDR(int fd, int iWriteLength, void *iWriteBuffer);

/**
 * CH347Jtag_ByteReadDR - JTAG DR read in bytes which used for multi-byte continuous operation.
 * State machine: Run-Test -> Shift-DR.. -> exit DR -> Run-Test
 * @fd: file descriptor of device
 * @oReadLength: pointer to read length, returns the actual number of bytes read on success
 * @oReadBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_ByteReadDR(int fd, uint32_t *oReadLength, void *oReadBuffer);

/**
 * CH347Jtag_ByteWriteIR - JTAG IR write in bytes which used for multi-byte continuous operation.
 * State machine: Run-Test -> Shift-IR.. -> exit IR -> Run-Test
 * @fd: file descriptor of device
 * @iWriteLength: pointer to write length
 * @iWriteBuffer: pointer to write buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_ByteWriteIR(int fd, int iWriteLength, void *iWriteBuffer);

/**
 * CH347Jtag_ByteReadIR - JTAG IR read in bytes which used for multi-byte continuous operation.
 * State machine: Run-Test -> Shift-IR.. -> exit IR -> Run-Test
 * @fd: file descriptor of device
 * @oReadLength: pointer to read length, returns the actual number of bytes read on success
 * @oReadBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_ByteReadIR(int fd, uint32_t *oReadLength, void *oReadBuffer);

/**
 * CH347Jtag_WriteRead - bitband mode JTAG DR data write which is applicable for a small amount of data. Exp: command operation, state machine switching and other control transmission. For batch data transmission, it is recommended to use USB20Jtag_ByeWriteDR
 * State machine: Run-Test -> Shift-DR.. -> exit DR -> Run-Test
 * @fd: file descriptor of device
 * @iWriteBitLength: pointer to write length
 * @iWriteBitBuffer: pointer to write buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_BitWriteDR(int fd, int iWriteBitLength, void *iWriteBitBuffer);

/**
 * CH347Jtag_BitWriteIR - bitband mode JTAG IR data write which is applicable for a small amount of data. Exp: command operation, state machine switching and other control transmission. For batch data transmission, it is recommended to use USB20Jtag_ByeWriteIR
 * State machine: Run-Test -> Shift-IR.. -> exit IR -> Run-Test
 * @fd: file descriptor of device
 * @iWriteBitLength: pointer to write length
 * @iWriteBitBuffer: pointer to write buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_BitWriteIR(int fd, int iWriteBitLength, void *iWriteBitBuffer);

/**
 * CH347Jtag_BitWriteIR - bitband mode JTAG IR data read which is applicable for a small amount of data. Exp: command operation, state machine switching and other control transmission. For batch data transmission, it is recommended to use USB20Jtag_ByteReadIR
 * State machine: Run-Test -> Shift-IR.. -> exit IR -> Run-Test
 * @fd: file descriptor of device
 * @oReadBitLength: pointer to read length, returns the actual number of bytes read on success
 * @oReadBitBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_BitReadIR(int fd, uint32_t *oReadBitLength, void *oReadBitBuffer);

/**
 * CH347Jtag_BitReadDR - bitband mode JTAG DR data read which is applicable for a small amount of data. Exp: command operation, state machine switching and other control transmission. For batch data transmission, it is recommended to use USB20Jtag_ByteReadDR
 * State machine: Run-Test -> Shift-DR.. -> exit DR -> Run-Test
 * @fd: file descriptor of device
 * @oReadBitLength: pointer to read length, returns the actual number of bytes read on success
 * @oReadBitBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Jtag_BitReadDR(int fd, uint32_t *oReadBitLength, void *oReadBitBuffer);

/**
 * CH347GPIO_Get - get gpio status
 * @fd: file descriptor of device
 * @iDir: gpio direction bits, bits0-7 on gpio0-7, 1 on ouput, 0 on input
 * @iData: gpio level bits, bits0-7 on gpio0-7, 1 on high, 0 on low
 *
 * The function return true if success, others if fail.
 */
extern bool CH347GPIO_Get(int fd, uint8_t *iDir, uint8_t *iData);

/**
 * CH347GPIO_Set - gpio setting
 * @fd: file descriptor of device
 * @iEnable: gpio function enable bits, bits0-7 on gpio0-7, 1 on enable
 * @iSetDirOut: gpio direction bits, bits0-7 on gpio0-7, 1 on ouput, 0 on input
 * @iSetDataOut: gpio output bits, bits0-7 on gpio0-7, if gpio direction is output, 1 on high, 0 on low
 *
 * The function return true if success, others if fail.
 */
extern bool CH347GPIO_Set(int fd, uint8_t iEnable, uint8_t iSetDirOut, uint8_t iSetDataOut);

/**
 * CH347Uart_Open - open device
 * @pathname: device path in /dev directory
 *
 * The function return positive file descriptor if successful, others if fail.
 */
extern int CH347Uart_Open(const char *pathname);

/**
 * CH347Uart_Close - close device
 * @fd: file descriptor of device
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Uart_Close(int fd);

/**
 * CH347Uart_GetCfg - read uart setting
 * @fd: file descriptor of device
 * @BaudRate: pointer to uart baudrate
 * @ByteSize: pointer to data bits
 *                  --> 0 : 5 bits
 *                  --> 1 : 6 bits
 *                  --> 2 : 7 bits
 *                  --> 3 : 8 bits
 *                  --> 4 : 16 bits
 * @Parity: pointer parity
 *                  --> 0 : none
 *                  --> 1 : odd
 *                  --> 2 : even
 *                  --> 3 : mark
 *                  --> 4 : space
 * @StopBits: pointer to stop bits
 *                  --> 0 : 1 bit
 *                  --> 1 : 1.5 bits
 *                  --> 2 : 2 bits
 * @ByteTimeout: pointer to receive timeout value, unit: 100us
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Uart_GetCfg(int fd, uint32_t *BaudRate, uint8_t *ByteSize, uint8_t *Parity, uint8_t *StopBits,
			     uint8_t *ByteTimeout);

/**
 * CH347Uart_Init - uart setting
 * @fd: file descriptor of device
 * @BaudRate: uart baudrate
 * @ByteSize: data bits setting
 *                  --> 0 : 5 bits
 *                  --> 1 : 6 bits
 *                  --> 2 : 7 bits
 *                  --> 3 : 8 bits
 *                  --> 4 : 16 bits
 * @Parity: parity setting
 *                  --> 0 : none
 *                  --> 1 : odd
 *                  --> 2 : even
 *                  --> 3 : mark
 *                  --> 4 : space
 * @StopBits: stop bits setting
 *                  --> 0 : 1 bit
 *                  --> 1 : 1.5 bits
 *                  --> 2 : 2 bits
 * @ByteTimeout: receive timeout value, unit: 100us
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Uart_Init(int fd, int BaudRate, uint8_t ByteSize, uint8_t Parity, uint8_t StopBits,
			   uint8_t ByteTimeout);

/**
 * CH347Uart_Read - read for uart operation
 * @fd: file descriptor of device
 * @oBuffer: pointer to read buffer
 * @ioLength: pointer to read length
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Uart_Read(int fd, void *oBuffer, uint32_t *ioLength);

/**
 * CH347Uart_Write - write data for uart operation
 * @fd: file descriptor of device
 * @iBuffer: pointer to write buffer
 * @ioLength: pointer to write length
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347Uart_Write(int fd, void *iBuffer, uint32_t *ioLength);

/**
 * CH347I2C_Set - configure i2c interface in stream mode
 * @fd: file descriptor of device
 * @iMode: stream mode
 * ->bit0~1: set I2C SCL rate
 * 			   --> 00 :	low rate 20KHz
 * 			   --> 01 : standard rate 100KHz
 * 			   --> 10 : fast rate 400KHz
 * 			   --> 11 : high rate 750KHz
 * other bits must keep 0
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347I2C_Set(int fd, int iMode);

/**
 * CH347I2C_SetDelaymS - delay operation
 * @fd: file descriptor of device
 * @iDelay: delay time in millseconds
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347I2C_SetDelaymS(int fd, int iDelay);

/**
 * CH347StreamI2C - write/read i2c in stream mode
 * @fd: file descriptor of device
 * @iWriteLength: write length
 * @iWriteBuffer: pointer to write buffer
 * @iReadLength: read length
 * @oReadBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347StreamI2C(int fd, int iWriteLength, void *iWriteBuffer, int iReadLength, void *oReadBuffer);

/**
 * CH347ReadEEPROM - read data from eeprom
 * @fd: file descriptor of device
 * @iEepromID: eeprom type
 * @iAddr: address of eeprom
 * @iLength: read length
 * @oBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347ReadEEPROM(int fd, EEPROM_TYPE iEepromID, int iAddr, int iLength, uint8_t *oBuffer);

/**
 * CH347WriteEEPROM - write data to eeprom
 * @fd: file descriptor of device
 * @iEepromID: eeprom type
 * @iAddr: address of eeprom
 * @iLength: write length
 * @iBuffer: pointer to write buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH347WriteEEPROM(int fd, EEPROM_TYPE iEepromID, int iAddr, int iLength, uint8_t *iBuffer);

#endif
