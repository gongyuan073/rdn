/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */




/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x_it.h"
#include "key.h"
#include "led.h"
#include "string.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
   
  
extern volatile unsigned char knob_f;
extern volatile unsigned int  dcp_wiper;
extern volatile unsigned char sec, min;		//��, �� ��ʱ
extern volatile unsigned char tm_base, ms100_f, treat_tm;
extern volatile unsigned char treat_f;		//���ڼ�� : 0;  ��ʽ���� : 1
extern volatile unsigned char stop_f, start_f;		
extern volatile unsigned char ssart_f;

extern volatile unsigned char W_Set_f;		//����ģʽʱ������ʶ���ڵ����¶�ֵ	
extern volatile unsigned char T_Set_f;		//����ģʽʱ������ʾ���ڵ���ʱ��ֵ
extern volatile unsigned char P_Set_f;		//����ģʽʱ������ʾ���ڵ��ڹ���ֵ

extern volatile unsigned int P_Set_Val;  	//��������ֵ(w)
extern volatile unsigned int W_Set_Val;	//�¶�����ֵ(��C)
extern volatile unsigned int T_Set_Val;	//ʱ������ֵ(s)

extern unsigned int P_High_Warning;		//���ʵ�����ֵ
extern unsigned int P_Low_Warning;		//���ʵ�����ֵ
extern unsigned int W_High_Warning;		//�¶ȵ�����ֵ
extern unsigned int W_Low_Warning;		//�¶ȵ�����ֵ
extern unsigned int T_High_Warning;		//ʱ�������ֵ

extern unsigned char control_mode;

extern unsigned int Measure_W_Float[6];

extern volatile unsigned char electrode_num;




/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
/**
  * @brief  This function handles USART1 interrupt request. �����ж�
  * @param  None
  * @retval : None
  */
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		//���Դ���: ֱ�ӽ����յ������ݷ���
		//printf("%c",USART_ReceiveData(USART3));
	}
}

/**
  * @brief  This function handles TIM2 interrupt request. 100ms�ж�һ��
  * @param  None
  * @retval : None
  */
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);   
  		tm_base++;
		if (tm_base > 9)
		{
			tm_base = 0;
			sec++;

			if(treat_f == 2)//����ŵ����Ƶ���ʱ��
			{
				treat_tm++;
			}

			if (sec > 59)
			{
				sec = 0;
				min++;
				if (min > 59)
				{
					min = 0;
				}
			}
		}
		ms100_f = 1;
	}
}

/**
  * @brief  This function handles EXTI2 interrupt request. I/O���жϣ��ж���ΪPD2
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
	unsigned char key_val = 0;

	if(EXTI_GetITStatus(EXTI_Line2) != RESET) 	//��ѯ�Ƿ�ΪLine2�ж�
	{
		key_val = KEY_Read();

		if (key_val == Start_K)
		{
			memset(Measure_W_Float, 0, 24);
			electrode_num++;
			if (electrode_num > 5)
			{
				electrode_num = 0;
			}
		}
/******************************************************************/
// 		if(key_val < 0x40 || key_val > 0x80)	//���̿���ic�ļ�ֵӦ����0x40~0x7F֮��
// 		{
// 			return ;
// 		}
// 		else
// 		{
// 			// "��ʼ/������Ƶ����"����
// 			if (key_val == Start_K || key_val == Stop_K)
// 			{
// 				//�����ֹͣ����������״̬
// 				if (key_val == Stop_K)
// 				{
// 					stop_f = 1;
// 					treat_f = 0;
// 				}
// 				//����ǿ�ʼ��
// 				if (key_val == Start_K)
// 				{
// 					start_f = 1;
// 				}
// 			}
// 			else if (treat_f != 0)
// 			{
// 				return ;
// 			}
// 			else
// 			{
// 				//"�¶�/���ʿ���ѡ��"����
// 				if (key_val == Control_Mode_K)
// 				{
// 					if (control_mode)
// 					{
// 						//���ʿ���
// 						control_mode = 0;
// 					}
// 					else
// 					{
// 						//�¶ȿ���
// 						control_mode = 1;
// 					}
// 					return ;
// 				}
// 				//�жϿ���ģʽ: �¶ȿ���/���ʿ���
// 				if (control_mode)
// 				{
// 					//"���ù���"����
// 					if (key_val == P_SET_K)
// 					{
// 						P_Set_f = 1;//���ù��ʵ��ڱ�־λ(�����ť��ʼ������С, ���ǵ��ڹ���P)
// 						W_Set_f = 0;
// 						T_Set_f = 0;
// 						
// 						return ;
// 					}
// 				}
// 				else
// 				{
// 					//"�����¶�"����
// 					if (key_val == W_SET_K)
// 					{
// 						P_Set_f = 0;
// 						W_Set_f = 1;//�����¶ȵ��ڱ�־λ(�����ť��ʼ������С, ��ֻ���ǵ����¶�W)	
// 						T_Set_f = 0;
// 						
// 						return ;
// 					}
// 				}

// 				//"ʱ������"���� (��λ: ��)
// 				if (key_val == T_SET_K)
// 				{
// 					P_Set_f = 0;
// 					W_Set_f = 0;
// 					T_Set_f = 1;//����ʱ����ڱ�־λ(����´���ť��ʼ������С, ��ֻ���ǵ���ʱ��T)

// 					return ;
// 				}

// 				//"����"����
// 				if (key_val == UP_K)
// 				{
// 					if (P_Set_f)
// 					{
// 						//��������ֵ����
// 						if (P_Set_Val < P_High_Warning)
// 						{
// 							P_Set_Val++;
// 						}
// 					}						
// 					if (W_Set_f)
// 					{
// 						//�¶�����ֵ����
// 						if (W_Set_Val < W_High_Warning)
// 						{
// 							W_Set_Val++;
// 						}
// 					}
// 					if (T_Set_f)
// 					{
// 						//ʱ������ֵ����
// 						if (T_Set_Val < T_High_Warning)
// 						{
// 							T_Set_Val++;
// 						}
// 					}
// 						
// 					return ;
// 				}
// 				//"����"����
// 				if (key_val == DOWN_K)
// 				{
// 					if (P_Set_f)
// 					{
// 						//��������ֵ��С
// 						if (P_Set_Val > P_Low_Warning)
// 						{
// 							P_Set_Val--;
// 							return ;
// 						}
// 					}				
// 					if (W_Set_f)
// 					{
// 						//�¶�����ֵ��С
// 						if (P_Set_Val > P_Low_Warning)
// 						{
// 							P_Set_Val--;
// 							return ;
// 						}
// 					}
// 					if (T_Set_f)
// 					{
// 						//ʱ������ֵ��С
// 						if (P_Set_Val > 0)
// 						{
// 							P_Set_Val--;
// 							return ;
// 						}
// 					}
// 				}
// 				
// 				
// 				//"��ʾ����"����:���в���ֵ����
// 				if (CLEAR_K)
// 				{
// 					
// 				}
//			}

//		}
/******************************************************************/
		EXTI_ClearITPendingBit(EXTI_Line2);     //����жϱ�־λ
	}
}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
