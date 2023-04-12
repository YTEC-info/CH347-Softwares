# **Introduction**

​ CH347FPGADownloader is an FPGA download software dedicated to CH347, which is implemented in combination with the OpenOCD open source project.

​ The currently supported FPGA models are mainly xilinx, and the specific models are as follows:

![image-20221027190604695](README.assets/image-20221027190604695.png)

​ If you encounter problems during use, please email: tech@wch.cn

# **Software Instructions**

## **Interface display**

![image-20221027190616419](README.assets/image-20221027190616419.png)

## **Download Settings Options**

![image-20221027190632574](README.assets/image-20221027190632574.png)

​ 1. "Select FPGA model": select the FPGA model to be operated this time, the selection box can be edited, and the support list can be matched according to the input content;

​ 2. "Select download file type":

​ A. BIT file download: This option will download the BIT file to the FPGA RAM by default, and it will be lost when the power is turned off, and it needs to be downloaded again when the power is turned on;

​ B. BIN file download: This option will download the BIN file to the FLASH connected to the FPGA by default, and the program will be solidified to the FLASH. After the download is complete, the software will automatically reset the FPGA running program;

​ 3. "Select hardware clock": Select CH347 as the TCK clock frequency of the JTAG interface. Here, the default settings are 10M, 15M, and 30M. If you need other frequencies, you can manually input them;

4. "About": View software version information;

​ 5. "Select download file": Click to select the bit or bin file required for this operation according to the current download method, and the selected file path will be remembered by default, and enter this path when selecting next time;

6. "Download": The download progress bar will be displayed in the BIN download mode, and it will be automatically hidden after the download is completed. When the BIN file download mode is selected, the download progress bar will also be displayed automatically;

![image-20221027190647377](README.assets/image-20221027190647377.png)

## **Information output**

![image-20221027190701769](README.assets/image-20221027190701769.png)

​ 1. "Device Information": Dynamically display CH347-JTAG interface plug-in information

​ 2. "Clear screen": the content in the current black display box of the situation

# **Software acquisition address**

[WCHSoftGroup/ch347: ch347 480Mbps high-speed USB to Jtag/I2C/SPI/Uart/GPIO etc. (github.com)](https://github.com/WCHSoftGroup/ch347/CH347FPGADownloader)