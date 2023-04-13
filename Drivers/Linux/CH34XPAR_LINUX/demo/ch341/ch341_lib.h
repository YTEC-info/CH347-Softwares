#ifndef _CH34X_LIB_H
#define _CH34X_LIB_H

typedef enum _EEPROM_TYPE {
	ID_24C01,
	ID_24C02,
	ID_24C04,
	ID_24C08,
	ID_24C16,
	ID_24C32,
	ID_24C64,
	ID_24C128,
	ID_24C256,
	ID_24C512,
	ID_24C1024,
	ID_24C2048,
	ID_24C4096
} EEPROM_TYPE;

typedef enum _CHIP_TYPE {
	CHIP_CH341 = 0,
	CHIP_CH347
} CHIP_TYPE;

typedef enum {
	TYPE_TTY = 0,
	TYPE_HID,
	TYPE_VCP,
} FUNCTYPE;

/**
 * CH34XOpenDevice - open device
 * @pathname: device path in /dev directory
 *
 * The function return positive file descriptor if successful, others if fail.
 */
extern int CH34xOpenDevice(const char *pathname);

/**
 * CH34XCloseDevice - close device
 * @fd: file descriptor of device
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xCloseDevice(int fd);

/**
 * CH34x_GetDriverVersion - get vendor driver version
 * @fd: file descriptor of device
 * @Drv_Version: pointer to version string
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34x_GetDriverVersion(int fd, unsigned char *Drv_Version);

/**
 * CH34x_GetChipVersion - get chip version
 * @fd: file descriptor of device
 * @Version: pointer to version
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34x_GetChipVersion(int fd, unsigned char *Version);

/**
 * CH34x_GetChipType - get chip type
 * @fd: file descriptor of device
 * @Version: pointer to type
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34x_GetChipType(int fd, unsigned char *ChipType);

/**
 * CH34X_GetDeviceID - get device vid and pid
 * @fd: file descriptor of device
 * @id: pointer to store id which contains vid and pid
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34X_GetDeviceID(int fd, uint32_t *id);

/**
 * CH34xSetParaMode - set chip parrallel work mode
 * @fd: file descriptor of device
 * @Mode: work mode, 0/1->EPP mode, 2->MEM mode
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xSetParaMode(int fd, uint8_t Mode);

/**
 * CH34xInitParallel - initial chip parrallel work mode
 * @fd: file descriptor of device
 * @Mode: work mode, 0/1->EPP mode, 2->MEM mode
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xInitParallel(int fd, uint8_t Mode);

/**
 * CH34xEppRead - read data or addr in parrallel EPP mode
 * @fd: file descriptor of device
 * @oBuffer: pointer to read buffer
 * @ioLength: read length
 * @PipeMode: 0->read pipe0 data, 1->read pipe1 addr
 *
 * The function return read 0 if successful, others if fail.
 */
extern int CH34xEppRead(int fd, uint8_t *oBuffer, uint32_t ioLength, uint8_t PipeMode);

/**
 * CH34xEppWrite - write data or addr in parrallel EPP mode
 * @fd: file descriptor of device
 * @iBuffer: pointer to write buffer
 * @ioLength: write length
 * @PipeMode: 0->write pipe0 data, 1->write pipe1 addr
 *
 * The function return 0 if successful, others if fail.
 */
extern int CH34xEppWrite(int fd, uint8_t *iBuffer, uint32_t ioLength, uint8_t PipeMode);

/**
 * CH34xEppWriteData - write data in parrallel EPP mode
 * @fd: file descriptor of device
 * @iBuffer: pointer to write buffer
 * @ioLength: write length
 *
 * The function return 0 if successful, others if fail.
 */
extern int CH34xEppWriteData(int fd, uint8_t *iBuffer, uint32_t ioLength);

/**
 * CH34xEppReadData - read data in parrallel EPP mode
 * @fd: file descriptor of device
 * @oBuffer: pointer to read buffer
 * @ioLength: read length
 *
 * The function return 0 if successful, others if fail.
 */
extern int CH34xEppReadData(int fd, uint8_t *oBuffer, uint32_t ioLength);

/**
 * CH34xEppWriteAddr - write addr in parrallel EPP mode
 * @fd: file descriptor of device
 * @iBuffer: pointer to write buffer
 * @ioLength: write length
 *
 * The function return 0 if successful, others if fail.
 */
extern int CH34xEppWriteAddr(int fd, uint8_t *iBuffer, uint32_t ioLength);

/**
 * CH34xEppReadAddr - read addr in parrallel EPP mode
 * @fd: file descriptor of device
 * @oBuffer: pointer to read buffer
 * @ioLength: read length
 *
 * The function return 0 if successful, others if fail.
 */
extern int CH34xEppReadAddr(int fd, uint8_t *oBuffer, uint32_t ioLength);

/**
 * CH34xEppSetAddr - set addr in parrallel EPP mode
 * @fd: file descriptor of device
 * @iAddr: addr data
 *
 * The function return 0 if successful, others if fail.
 */
extern int CH34xEppSetAddr(int fd, uint32_t iAddr);

/**
 * CH34xSetTimeout - set USB data read and write timeout
 * @fd: file descriptor of device
 * @iWriteTimeout: data download timeout in milliseconds
 * @iReadTimeout: data upload timeout in milliseconds
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xSetTimeout(int fd, uint32_t iWriteTimeout, uint32_t iReadTimeout);

/**
 * CH34xInitMEM - initial chip in parrallel MEM mode
 * @fd: file descriptor of device
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xInitMEM(int fd);

/**
 * CH34xMEMReadData - read data in parrallel MEM mode
 * @fd: file descriptor of device
 * @oBuffer: pointer to read buffer
 * @ioLength: read length
 * @PipeMode: 0->read pipe0, 1->read pipe1
 *
 * The function return 0 if successful, others if fail.
 */
extern int CH34xMEMReadData(int fd, uint8_t *oBuffer, uint32_t ioLength, uint8_t PipeMode);

/**
 * CH34xMEMWriteData - write data in parrallel MEM mode
 * @fd: file descriptor of device
 * @iBuffer: pointer to write buffer
 * @ioLength: write length
 * @PipeMode: 0->write pipe0, 1->write pipe1
 *
 * The function return 0 if successful, others if fail.
 */
extern int CH34xMEMWriteData(int fd, uint8_t *iBuffer, uint32_t ioLength, uint32_t PipeMode);

/**
 * CH34xSetStream - configure spi/i2c interface in stream mode
 * @fd: file descriptor of device
 * @Mode: stream mode
 * ->bit0~1: set I2C SCL rate
 * 			   --> 00 :	low rate 20KHz
 * 			   --> 01 : standard rate 100KHz
 * 			   --> 10 : fast rate 400KHz
 * 			   --> 11 : high rate 750KHz
 * ->bit2: set spi mode
 * 			   --> 0 : one in one out(D3: clk, D5: out, D7: in)
 * 			   --> 1 : two in two out(D3 :clk, D4/D5: out, D6/D7 :in)
 * ->bit7: set spi data mode
 * 			   --> 0 : low bit first
 *       	   --> 1 : high bit first
 * other bits must keep 0
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xSetStream(int fd, uint8_t Mode);

/**
 * CH34xSetDelaymS - delay operation
 * @fd: file descriptor of device
 * @iDelay: delay time in millseconds
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xSetDelaymS(int fd, uint32_t iDelay);

/**
 * CH34xReadData - read for spi/i2c operation
 * @fd: file descriptor of device
 * @oReadBuffer: pointer to read buffer
 * @oReadLength: pointer to read length
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xReadData(int fd, void *oReadBuffer, uint32_t *oReadLength);

/**
 * CH34xWriteData - write data for spi/i2c operation
 * @fd: file descriptor of device
 * @iWriteBuffer: pointer to write buffer
 * @iWriteLength: pointer to write length
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xWriteData(int fd, void *iWriteBuffer, uint32_t *iWriteLength);

/**
 * CH34xWriteRead - write data then read for spi/i2c operation
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
extern bool CH34xWriteRead(int fd, uint32_t iWriteLength, void *iWriteBuffer, uint32_t iReadStep, uint32_t iReadTimes,
			   uint32_t *oReadLength, void *oReadBuffer);

/**
 * CH34xSetOutput - set direction and output data of CH341
 * @iEnbale: set direction and data enable
 * 			   --> Bit16 High :	effect on Bit15~8 of iSetDataOut
 * 			   --> Bit17 High :	effect on Bit15~8 of iSetDirOut
 * 			   --> Bit18 High :	effect on Bit7~0 of iSetDataOut
 * 			   --> Bit19 High :	effect on Bit7~0 of iSetDirOut
 *			   --> Bit20 High :	effect on Bit23~16 of iSetDataOut
 * @iSetDirOut: set io direction
 *			  -- > Bit High : Output
 *			  -- > Bit Low : Input
 * @iSetDataOut: set io data
 * 			 Output:
 *			  -- > Bit High : High level
 *			  -- > Bit Low : Low level
 * Note:
 * Bit7~Bit0<==>D7-D0, Bit8<==>ERR#, Bit9<==>PEMP, Bit10<==>INT#
 * Bit11<==>SLCT, Bit13<==>WAIT#, Bit14<==>DATAS#/READ#, Bit15<==>ADDRS#/ADDR/ALE
 *
 * The pins below can only be used in output mode:
 * Bit16<==>RESET#, Bit17<==>WRITE#, Bit18<==>SCL, Bit29<==>SDA
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xSetOutput(int fd, uint32_t iEnable, uint32_t iSetDirOut, uint32_t iSetDataOut);

/**
 * CH34xSet_D5_D0 - set direction and output data of D5-D0 on CH341
 * @fd: file descriptor of device
 * @iSetDirOut: set io direction
 *			  -- > Bit High : Output
 *			  -- > Bit Low : Input
 * @iSetDataOut: set io data
 * 			 Output:
 *			  -- > Bit High : High level
 *			  -- > Bit Low : Low level
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xSet_D5_D0(int fd, uint8_t iSetDirOut, uint8_t iSetDataOut);

/**
 * CH34xStreamI2C - write/read i2c in stream mode
 * @fd: file descriptor of device
 * @iWriteLength: write length
 * @iWriteBuffer: pointer to write buffer
 * @iReadLength: read length
 * @oReadBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xStreamI2C(int fd, uint32_t iWriteLength, void *iWriteBuffer, uint32_t iReadLength, void *oReadBuffer);

/**
 * CH34xReadEEPROM - read data from eeprom
 * @fd: file descriptor of device
 * @iEepromID: eeprom type
 * @iAddr: address of eeprom
 * @iLength: read length
 * @oBuffer: pointer to read buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xReadEEPROM(int fd, EEPROM_TYPE iEepromID, uint32_t iAddr, uint32_t iLength, uint8_t *oBuffer);

/**
 * CH34xWriteEEPROM - write data to eeprom
 * @fd: file descriptor of device
 * @iEepromID: eeprom type
 * @iAddr: address of eeprom
 * @iLength: write length
 * @iBuffer: pointer to write buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xWriteEEPROM(int fd, EEPROM_TYPE iEepromID, uint32_t iAddr, uint32_t iLength, uint8_t *iBuffer);

/**
 * CH34xStreamSPIx - write/read spi in stream mode
 * @fd: file descriptor of device
 * @iChipSelect: cs enable
 * @iLength: the length of data
 * @ioBuffer: one in one out buffer
 * @ioBuffer2: two in two out buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xStreamSPIx(int fd, uint32_t iChipSelect, uint32_t iLength, void *ioBuffer, void *ioBuffer2);

/**
 * CH34xStreamSPI4 - write/read spi in 4-line stream mode
 * @fd: file descriptor of device
 * @iChipSelect: cs enable
 * @iLength: the length of data
 * @ioBuffer: one in one out buffer
 *
 * The function return true if successful, false if fail.
 */
extern bool CH34xStreamSPI4(int fd, uint32_t iChipSelect, uint32_t iLength, void *ioBuffer);

#endif
