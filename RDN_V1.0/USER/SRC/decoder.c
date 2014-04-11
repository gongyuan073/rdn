/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：decoder.c
 * 描述    ：译码芯片SN74CBT3251的驱动       
 * 实验平台：stm32f103zet6
 * 硬件连接：-----------------------
 *          |   PB1 - PWM负     	|
 *          |   PC8 - PWM正    		|
 *          |   PF0 - S0			|
 *			|	PF1 - S1			|
 *			|	PF2 - S2			|
 *			|	PF3 - 两芯片的选通	|							|
 *           ----------------------- 
 * 库版本  ：ST3.5.0
 * 作者    ：曹星辉
 * 邮箱    ：caoxinghui@foxmail.com
**********************************************************************************/
#include "decoder.h"

/*
 * 函数名：DECODER_GPIO_Config
 * 描述  ：配置译码器使用到的引脚
 * 输入  ：无
 * 输出  ：无
 */
void DECODER_GPIO_Config(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOC的外设时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOF, ENABLE); 

	/*选择要控制的GPIOB引脚*/													   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化GPIOB*/
  	GPIO_Init(GPIOB, &GPIO_InitStructure);	//PB1 : PWM负

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	//PC8 : PWM正

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	//PF0~PF2 : 译码接口  ; PF3 两个译码芯片的选通引脚
}


/***********射频与测温不能同时进行*************/
void RF_Enable(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_8);	//PWM正赋值为1
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);	//PWM负赋值为0
	
	//使能OE
	GPIO_ResetBits(GPIOF, GPIO_Pin_0);
}
	
void Measure_W_Enable(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_8);	//PWM正赋值为0
	GPIO_SetBits(GPIOB, GPIO_Pin_1);	//PWM负赋值为1
	
	//使能OE
	GPIO_ResetBits(GPIOF, GPIO_Pin_0);
}
	
/*
 * 函数名：DECODER_Chanal_Select
 * 描述  ：选通引脚A 到 通道x.  0 <= x <= 5;
 * 输入  ：通道1到通道6
 * 输出  ：无
 */
void DECODER_Chanal_Select(unsigned char val)
{
	unsigned char tmp = val;
	//s0
// 	if (tmp & 0x1)
// 	{
// 		GPIO_SetBits(GPIOF, GPIO_Pin_0);//s0
// 	}
// 	else
// 	{
// 		GPIO_ResetBits(GPIOF, GPIO_Pin_0);//s0
// 	}
// 	//s1
// 	if (tmp & 0x2)
// 	{
// 		GPIO_SetBits(GPIOF, GPIO_Pin_1);//s1
// 	}
// 	else
// 	{
// 		GPIO_ResetBits(GPIOF, GPIO_Pin_1);//s1
// 	}
// 	//s2
// 	if (tmp & 0x4)
// 	{
// 		GPIO_SetBits(GPIOF, GPIO_Pin_2);//s2
// 	}
// 	else
// 	{
// 		GPIO_ResetBits(GPIOF, GPIO_Pin_2);//s2
// 	}

	GPIO_SetBits(GPIOF, GPIO_Pin_0);
	if (tmp & 0x1)
	{
		GPIO_SetBits(GPIOF, GPIO_Pin_3);//s0
	}
	else
	{
		GPIO_ResetBits(GPIOF, GPIO_Pin_3);//s0
	}
	//s1
	if (tmp & 0x2)
	{
		GPIO_SetBits(GPIOF, GPIO_Pin_2);//s2
	}
	else
	{
		GPIO_ResetBits(GPIOF, GPIO_Pin_2);//s2
	}
	//s2
	if (tmp & 0x4)
	{
		GPIO_SetBits(GPIOF, GPIO_Pin_1);//s1
	}
	else
	{
		GPIO_ResetBits(GPIOF, GPIO_Pin_1);//s1
	}
	GPIO_ResetBits(GPIOF, GPIO_Pin_0);
}
	
/**********************************END OF FILE************************************/
