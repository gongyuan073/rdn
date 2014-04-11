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
extern volatile unsigned char sec, min;		//秒, 分 计时
extern volatile unsigned char tm_base, ms100_f, treat_tm;
extern volatile unsigned char treat_f;		//贴壁检测 : 0;  正式治疗 : 1
extern volatile unsigned char stop_f, start_f;		
extern volatile unsigned char ssart_f;

extern volatile unsigned char W_Set_f;		//设置模式时用来标识正在调节温度值	
extern volatile unsigned char T_Set_f;		//设置模式时用来标示正在调节时间值
extern volatile unsigned char P_Set_f;		//设置模式时用来标示正在调节功率值

extern volatile unsigned int P_Set_Val;  	//功率设置值(w)
extern volatile unsigned int W_Set_Val;	//温度设置值(°C)
extern volatile unsigned int T_Set_Val;	//时间设置值(s)

extern unsigned int P_High_Warning;		//功率的上限值
extern unsigned int P_Low_Warning;		//功率的下限值
extern unsigned int W_High_Warning;		//温度的上限值
extern unsigned int W_Low_Warning;		//温度的下限值
extern unsigned int T_High_Warning;		//时间的上限值

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
  * @brief  This function handles USART1 interrupt request. 接收中断
  * @param  None
  * @retval : None
  */
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		//测试代码: 直接将接收到的数据返回
		//printf("%c",USART_ReceiveData(USART3));
	}
}

/**
  * @brief  This function handles TIM2 interrupt request. 100ms中断一次
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

			if(treat_f == 2)//计算放电治疗的总时间
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
  * @brief  This function handles EXTI2 interrupt request. I/O线中断，中断线为PD2
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
	unsigned char key_val = 0;

	if(EXTI_GetITStatus(EXTI_Line2) != RESET) 	//查询是否为Line2中断
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
// 		if(key_val < 0x40 || key_val > 0x80)	//键盘控制ic的键值应该在0x40~0x7F之间
// 		{
// 			return ;
// 		}
// 		else
// 		{
// 			// "开始/结束射频消融"按键
// 			if (key_val == Start_K || key_val == Stop_K)
// 			{
// 				//如果是停止键则进入待机状态
// 				if (key_val == Stop_K)
// 				{
// 					stop_f = 1;
// 					treat_f = 0;
// 				}
// 				//如果是开始键
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
// 				//"温度/功率控制选择"按键
// 				if (key_val == Control_Mode_K)
// 				{
// 					if (control_mode)
// 					{
// 						//功率控制
// 						control_mode = 0;
// 					}
// 					else
// 					{
// 						//温度控制
// 						control_mode = 1;
// 					}
// 					return ;
// 				}
// 				//判断控制模式: 温度控制/功率控制
// 				if (control_mode)
// 				{
// 					//"设置功率"按键
// 					if (key_val == P_SET_K)
// 					{
// 						P_Set_f = 1;//设置功率调节标志位(如果旋钮开始调整大小, 则是调节功率P)
// 						W_Set_f = 0;
// 						T_Set_f = 0;
// 						
// 						return ;
// 					}
// 				}
// 				else
// 				{
// 					//"设置温度"按键
// 					if (key_val == W_SET_K)
// 					{
// 						P_Set_f = 0;
// 						W_Set_f = 1;//设置温度调节标志位(如果旋钮开始调整大小, 则只能是调节温度W)	
// 						T_Set_f = 0;
// 						
// 						return ;
// 					}
// 				}

// 				//"时间设置"按键 (单位: 秒)
// 				if (key_val == T_SET_K)
// 				{
// 					P_Set_f = 0;
// 					W_Set_f = 0;
// 					T_Set_f = 1;//设置时间调节标志位(如果下次旋钮开始调整大小, 则只能是调节时间T)

// 					return ;
// 				}

// 				//"增加"按键
// 				if (key_val == UP_K)
// 				{
// 					if (P_Set_f)
// 					{
// 						//功率设置值增加
// 						if (P_Set_Val < P_High_Warning)
// 						{
// 							P_Set_Val++;
// 						}
// 					}						
// 					if (W_Set_f)
// 					{
// 						//温度设置值增加
// 						if (W_Set_Val < W_High_Warning)
// 						{
// 							W_Set_Val++;
// 						}
// 					}
// 					if (T_Set_f)
// 					{
// 						//时间设置值增加
// 						if (T_Set_Val < T_High_Warning)
// 						{
// 							T_Set_Val++;
// 						}
// 					}
// 						
// 					return ;
// 				}
// 				//"减少"按键
// 				if (key_val == DOWN_K)
// 				{
// 					if (P_Set_f)
// 					{
// 						//功率设置值减小
// 						if (P_Set_Val > P_Low_Warning)
// 						{
// 							P_Set_Val--;
// 							return ;
// 						}
// 					}				
// 					if (W_Set_f)
// 					{
// 						//温度设置值减小
// 						if (P_Set_Val > P_Low_Warning)
// 						{
// 							P_Set_Val--;
// 							return ;
// 						}
// 					}
// 					if (T_Set_f)
// 					{
// 						//时间设置值减小
// 						if (P_Set_Val > 0)
// 						{
// 							P_Set_Val--;
// 							return ;
// 						}
// 					}
// 				}
// 				
// 				
// 				//"显示归零"按键:所有测量值归零
// 				if (CLEAR_K)
// 				{
// 					
// 				}
//			}

//		}
/******************************************************************/
		EXTI_ClearITPendingBit(EXTI_Line2);     //清除中断标志位
	}
}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
