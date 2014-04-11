#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"
#include "bitbank.h"


/***************键值定义(更改各项设置值)*****************/
//选择设置功率按钮
#define P_SET_K		0x41
//选择设置温度按钮
#define W_SET_K		0x42
//选择设置时间按钮
#define T_SET_K		0x43
//增加
#define UP_K	0x45
//减少
#define DOWN_K	0x46

//控制模式选择: 功率控制 or 温度控制
#define Control_Mode_K	0xdb
//
#define CLEAR_K 		0xe7
#define Start_K			0x41
#define Stop_K			0x52

#define Enter_K			0xeb

			



//IO方向设置
//PB3的设置如下
// #define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
// #define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
//本实验中使用的端口
#define SDA_IN()  {GPIOD->CRL&=0XFFFFFF0F;GPIOD->CRL|=8<<4;}
#define SDA_OUT() {GPIOD->CRL&=0XFFFFFF0F;GPIOD->CRL|=3<<4;}

#define SCL_OUT() {GPIOD->CRL&=0XFFFFFFF0;GPIOD->CRL|= 3;}

#define DISABLE_KEY_INTERRUPT
#define ENABLE_KEY_INTERRUPT

#define delay() {char i = 3; while(i--);}

//IO操作函数
//SCL 时钟线
#define KEY_SCL    PDout(0)

//如果是真正的2线接口,SDA是作为中断源使用而不用INT引脚. 标准2线接口使用INT引脚(共3根线)
//SDA输出
#define KEY_SDA_W   PDout(1)
//输入SDA
#define KEY_SDA_R   PDin(1)


void KEY_GPIO_Init(void);	//初始化按键ic的接口引脚
void KEY_I2c_Config(void);	//配置按键ic
void KEY_I2c_Start(void);	//I2C总线的起始条件
void KEY_I2c_Stop(void);	//I2C总线的停止条件
void KEY_I2c_WrByte(unsigned char dat);	//I2C总线写一个数据
unsigned char  KEY_I2c_RdByte(void);	//I2C总线读取一个数据
void KEY_Write(unsigned short cmd);		//向按键ic写命令
unsigned char KEY_Read(void);			//获得按键值


#endif
