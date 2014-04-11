/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * �ļ���  ��decoder.c
 * ����    ������оƬSN74CBT3251������       
 * ʵ��ƽ̨��stm32f103zet6
 * Ӳ�����ӣ�-----------------------
 *          |   PB1 - PWM��     	|
 *          |   PC8 - PWM��    		|
 *          |   PF0 - S0			|
 *			|	PF1 - S1			|
 *			|	PF2 - S2			|
 *			|	PF3 - ��оƬ��ѡͨ	|							|
 *           ----------------------- 
 * ��汾  ��ST3.5.0
 * ����    �����ǻ�
 * ����    ��caoxinghui@foxmail.com
**********************************************************************************/
#include "decoder.h"

/*
 * ��������DECODER_GPIO_Config
 * ����  ������������ʹ�õ�������
 * ����  ����
 * ���  ����
 */
void DECODER_GPIO_Config(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����GPIOC������ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOF, ENABLE); 

	/*ѡ��Ҫ���Ƶ�GPIOB����*/													   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*������������Ϊ50MHz */
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��GPIOB*/
  	GPIO_Init(GPIOB, &GPIO_InitStructure);	//PB1 : PWM��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	//PC8 : PWM��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	//PF0~PF2 : ����ӿ�  ; PF3 ��������оƬ��ѡͨ����
}


/***********��Ƶ����²���ͬʱ����*************/
void RF_Enable(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_8);	//PWM����ֵΪ1
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);	//PWM����ֵΪ0
	
	//ʹ��OE
	GPIO_ResetBits(GPIOF, GPIO_Pin_0);
}
	
void Measure_W_Enable(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_8);	//PWM����ֵΪ0
	GPIO_SetBits(GPIOB, GPIO_Pin_1);	//PWM����ֵΪ1
	
	//ʹ��OE
	GPIO_ResetBits(GPIOF, GPIO_Pin_0);
}
	
/*
 * ��������DECODER_Chanal_Select
 * ����  ��ѡͨ����A �� ͨ��x.  0 <= x <= 5;
 * ����  ��ͨ��1��ͨ��6
 * ���  ����
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
