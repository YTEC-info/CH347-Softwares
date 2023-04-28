# CH347 USB-HS-Bridge

<p><img alt="" src="https://raw.githubusercontent.com/YTEC-info/CH347-Softwares/main/Datasheet%20%26%20Manual/USB-HS-Bridge/Ch347-1.jpg" style="float:center; height:600px; width:600px" /></p>




# Apresentação 
O CH347 USB-HS-Bridge é uma ferramenta de depuração feita pelo MuseLab com base no CH347 da WCH Com USB 2.0 de alta velocidade, suporta `USB-to-I2C` `SPI` `UART` `JTAG` `GPIO`, pode ser usado para operar com vários dispositivos, incluindo `MCUs` `PFPGAs` `CPLs` `EEPROMs` `SPI-Flash` `LCDs`...



# Modo de trabalho
O modo de trabalho do chip é configurado pelos swiches DTR1 e RTS1

| MODO| DTR1 | RTS1 | INTERFACE
|---|---|---|---|
| `0` | OFF | OFF | **UART0 + UART1** |
| `1` | ON | OFF | **UART1 + I2C + SPI (VCP Mode)** |
| `2` | OFF | ON | **UART1 + I2C + SPI (HID Mode)** |
| `3` | ON | ON | **UART1 + JTAG** |  



## USB para UART
Existem dois periféricos UART na placa, UART0 e UART1, ambos habilitados no modo 0 e apenas UART1 habilitado no modo 1-2-3, UART0 suporta todos os sinais de modem e UART1 suporta alguns sinais de modem, ambos suportam controle de fluxo de hardware.


## USB para I2C
O I2C (nota: I2C Mestre) é ativado no modo 1-2, a diferença entre o modo 1-2 é que o modo 1 precisa instalar o driver fornecido pela WCH e o modo 2 não requer a instalação do driver (atua como "USB HID Device").

## USB para SPI
O SPI (nota: SPI Mestre com 4 canais) está habilitado no modo 1-2, existem dois pinos CS, então você pode controlar dois dispositivos SPI ao mesmo tempo (time-sharing).

## USB para JTAG
O JTAG é ativado no modo 3, inclui `TCK` `TMS` `TDI` `TDO` `TRST`, suporta modo rápido e modo bit-bang, velocidade mais rápida é de até 18Mbits.
