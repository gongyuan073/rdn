/**********************************************************************************
 * �ļ���  ��led.c
 * ����    ��led Ӧ�ú�����         
 * ʵ��ƽ̨��stm32f103zet6
 * Ӳ�����ӣ�-----------------
 *          |   PE7 - LED1     |
 *          |   PE8 - LED2     |
 *          |   PE9 - LED3     |
 *           ----------------- 
 * ��汾  ��ST3.5.0
 * ����    :���ǻ�
 * email   :caoxinghui@foxmail.com
**********************************************************************************/
#include "led.h"

/*
 * ��������LED_GPIO_Config
 * ����  ������LED�õ���I/O��
 * ����  ����
 * ���  ����
 */
void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_5);	 // turn off all led
}


/*************************************END OF FILE************************************/
