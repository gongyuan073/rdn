/**********************************************************************************
 * 文件名  ：exti.c
 * 描述    ：I/O线中断应用函数库         
 * 实验平台：stm32f103zet6
 * 硬件连接：-------------------------
 *          | PE5 - key1              |
 *          | PE6 - key2(本实验没用到)|
 *           -------------------------
 * 库版本  :ST3.5.0
 * 作者    :曹星辉
 * email   :caoxinghui@foxmail.com
**********************************************************************************/
#include "exti.h"



/*
 * 函数名：EXTI_PE5_Config
 * 描述  ：配置 PE5 为线中断口，并设置中断优先级
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void EXTI_PD2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* config the extiline(PD2) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/* EXTI line gpio config(PD2) */	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 // 上拉输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* EXTI line(PD2) mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
	/* 配置P[A|B|C|D|E]2为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/************************************END OF FILE********************************/

