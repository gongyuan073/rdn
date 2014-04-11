#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"
#include "bitbank.h"


/***************��ֵ����(���ĸ�������ֵ)*****************/
//ѡ�����ù��ʰ�ť
#define P_SET_K		0x41
//ѡ�������¶Ȱ�ť
#define W_SET_K		0x42
//ѡ������ʱ�䰴ť
#define T_SET_K		0x43
//����
#define UP_K	0x45
//����
#define DOWN_K	0x46

//����ģʽѡ��: ���ʿ��� or �¶ȿ���
#define Control_Mode_K	0xdb
//
#define CLEAR_K 		0xe7
#define Start_K			0x41
#define Stop_K			0x52

#define Enter_K			0xeb

			



//IO��������
//PB3����������
// #define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
// #define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
//��ʵ����ʹ�õĶ˿�
#define SDA_IN()  {GPIOD->CRL&=0XFFFFFF0F;GPIOD->CRL|=8<<4;}
#define SDA_OUT() {GPIOD->CRL&=0XFFFFFF0F;GPIOD->CRL|=3<<4;}

#define SCL_OUT() {GPIOD->CRL&=0XFFFFFFF0;GPIOD->CRL|= 3;}

#define DISABLE_KEY_INTERRUPT
#define ENABLE_KEY_INTERRUPT

#define delay() {char i = 3; while(i--);}

//IO��������
//SCL ʱ����
#define KEY_SCL    PDout(0)

//�����������2�߽ӿ�,SDA����Ϊ�ж�Դʹ�ö�����INT����. ��׼2�߽ӿ�ʹ��INT����(��3����)
//SDA���
#define KEY_SDA_W   PDout(1)
//����SDA
#define KEY_SDA_R   PDin(1)


void KEY_GPIO_Init(void);	//��ʼ������ic�Ľӿ�����
void KEY_I2c_Config(void);	//���ð���ic
void KEY_I2c_Start(void);	//I2C���ߵ���ʼ����
void KEY_I2c_Stop(void);	//I2C���ߵ�ֹͣ����
void KEY_I2c_WrByte(unsigned char dat);	//I2C����дһ������
unsigned char  KEY_I2c_RdByte(void);	//I2C���߶�ȡһ������
void KEY_Write(unsigned short cmd);		//�򰴼�icд����
unsigned char KEY_Read(void);			//��ð���ֵ


#endif
