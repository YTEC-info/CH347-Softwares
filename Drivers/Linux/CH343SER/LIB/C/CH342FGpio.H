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
// USB���߽ӿ�оƬ�˿ڹ����ӿڿ� V1.5
// �Ͼ��ߺ�΢����
// CH34X-Port-DLL  V1.5
// ���л���: Windows 98/ME, Windows 2000/XP,Windows 7/8/10/11
// support USB chip: CH342
// USB => Single/Multi serial port
// 

#ifndef		_CH342_PORT_H
#define		_CH342_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

//��������˳��
//1.CH342F_GpioInit	CH342F_GetGpioConfig
//2.CH342F_GpioSet/CH342F_GpioGet
//3.CH342F_GpioDeInit

//����GPIO����ʼ��,���ô˽ŵ�MODEM�źŹ���ʧЧ��ʹ��GPIOǰ��������ִ�д˺�����IO���г�ʼ�������������ʼֵ���˺���ִ�к󣬲����ٵ�����Modem�źŵĺ���
UCHAR	WINAPI	CH342F_GpioInit(HANDLE	   iPortH,         //����0/1���
	BOOL       IsSetInitBit,                               //��ʼ��ʱ�Ƿ���Ҫ�ı�IO����͵�ѹ(���)
	USHORT     GpioDirBit,                                 //��ʼ������λ��Bit˳����GPIO˳��һ�¡�λ1Ϊ�����λ0Ϊ����
	USHORT     GpioStaBit);                                //��ʼ������ŵ�ƽ״̬λ��Bit˳����GPIO˳��һ�¡���Ϊ����ʱ����Ӧλֵ����Ϊ0

//�ر�GPIO���ܣ�����ر�ʱ������ô˺���������Ӱ�촮��MODEMʹ�á�
UCHAR	WINAPI	CH342F_GpioDeInit(HANDLE	   iPortH);


//GPIO����ŵ�ƽ����
UCHAR  WINAPI	WINAPI	CH342F_GpioSet(HANDLE	  iPortH,         //����0/1���
                               ULONG      iEnable,        //������Ч��־,λ�����GPIOx����Ŷ�Ӧ,λֵΪ1��ʾiSetDataOut�Ķ�Ӧλ������Ч,Ϊ0��ʾ������Ч;
		                       ULONG	  iSetDataOut);   //IO�����ƽ,���GPIO����Ϊ���,��ôĳλ��0ʱ��Ӧ��������͵�ƽ,ĳλ��1ʱ��Ӧ��������ߵ�ƽ	            

//��ȡGPIO��������:���ŷ��򡢵�ƽ��iSetDirOut��iSetDataOutλ�����GPIO��Ŷ�Ӧ
UCHAR WINAPI	CH342F_GetGpioConfig(HANDLE		iPortH,         //����0/1���
		                                 PULONG			SetDirOut,      // GPIO�ŷ�������:ĳλΪ0��ʾ��Ӧ���ŷ���Ϊ����;ĳλΪ1��ʾ��Ӧ���ŷ���Ϊ���
		                                 PULONG         SetDataOut);    // GPIO�ŵ�ƽֵ:1��ʾ�ߵ�ƽ��0��ʾ�͵�ƽ

//��ȡGPIO���ŵ�ƽ״̬
UCHAR	WINAPI	CH342F_GpioGet(HANDLE			iPortH,      //����0/1���
		                       PULONG			iStatus);    // ����״̬,��GPIOx��Ŷ�Ӧ����λ�š���GPIO1����ӦiStatusֵ�е�bit1,����CH910x_GPIO_xxλ����ȡֵ

#ifdef __cplusplus
}
#endif

#endif		// _CH342_PORT_H