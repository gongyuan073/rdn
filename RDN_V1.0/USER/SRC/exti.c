/**********************************************************************************
 * �ļ���  ��exti.c
 * ����    ��I/O���ж�Ӧ�ú�����         
 * ʵ��ƽ̨��stm32f103zet6
 * Ӳ�����ӣ�-------------------------
 *          | PE5 - key1              |
 *          | PE6 - key2(��ʵ��û�õ�)|
 *           -------------------------
 * ��汾  :ST3.5.0
 * ����    :���ǻ�
 * email   :caoxinghui@foxmail.com
**********************************************************************************/
#include "exti.h"



/*
 * ��������EXTI_PE5_Config
 * ����  ������ PE5 Ϊ���жϿڣ��������ж����ȼ�
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 // ��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* EXTI line(PD2) mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
	/* ����P[A|B|C|D|E]2Ϊ�ж�Դ */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/************************************END OF FILE********************************/

