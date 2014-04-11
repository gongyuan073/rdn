/**********************************************************************************
 * 实验名  :RDN V1.0
 * 描述    :如果更改SDA或者SCL引脚则需更改key.c中SDA/SCL/SDA_IN()/SDA_OUT()的宏定义
 *            特别是SDA的输入输出配置(注意CRH/CRL).
 * 平台	   :stm32f103zet6
 * 库版本  :ST3.5.0
 *
 * 作者    ：曹星辉
 * e-mail  ：caoxinghui@foxmail.com
 * 调试	   : 1. 20140227 定时器定时时间严重不正确, 更改 system_stm32f10x.c中 1000行处, 但是定时还是不准, 晶振的波形全在0v以下. 
				解决: 电路板的晶振未接地
			 2. 20140321 基本功能添加完毕, SD\USB\WTD\TOUCH等尚未添加
**********************************************************************************/
#include "stm32f10x.h" 
#include "led.h"
#include "key.h"
#include "exti.h"
#include "usart1.h"
#include "dds.h"
#include "dcp_max5483.h"
#include "timer.h"
#include "decoder.h"
#include "usart1.h"
#include "ad_max186.h"
#include "control.h"


//volatile unsigned char knob_f = 0;		//旋钮的标记位, 1 : 旋钮被旋转  0 : 旋钮未被旋转

volatile unsigned char sec, min;		//秒, 分 计时
volatile unsigned char tm_base, ms100_f;//tm_base: 每100ms递增1; ms100_f: 每到100ms的时候ms100_f会置1; 
volatile unsigned int treat_tm;			//treat_tm: 放电治疗的总时间
volatile unsigned char treat_f = 0;		//待机状态: 0; 贴壁检测: 1;  正式治疗: 2; 
volatile unsigned char stop_f = 0;		//停止标志: 1 停止键被按下, 0 停止键未被按下
volatile unsigned char start_f = 0;		//开始标志: 1 开始键按下, 0 开始键未按下

volatile unsigned char W_Set_f = 0;		//设置模式时用来标识正在调节温度值	
volatile unsigned char T_Set_f = 0;		//设置模式时用来标示正在调节时间值
volatile unsigned char P_Set_f = 0;		//设置模式时用来标示正在调节功率值

volatile unsigned int P_Set_Val = 0;  	//功率设置值(w)
volatile unsigned int W_Set_Val = 70;	//温度设置值(°C)
volatile unsigned int T_Set_Val = 60;	//时间设置值(s)

unsigned int P_High_Warning = 50;		//功率的上限值
unsigned int P_Low_Warning  = 0;		//功率的下限值
unsigned int W_High_Warning = 80;		//温度的上限值
unsigned int W_Low_Warning  = 35;		//温度的下限值
unsigned int T_High_Warning  = 180;		//时间的上限值

unsigned char control_mode = 0;	//控制模式: 0 功率控制 ; 1 温度控制;

unsigned int Measure_P_Float[6];	//功率检测值    /* 因为显示屏只能识别整形, 所以不定义为float */
unsigned int Measure_R_Float[6];	//阻抗检测值
unsigned int Measure_W_Float[6];	//温度检测值

unsigned int dcp_pos_arry[6] = {5, 5, 5, 5, 5, 5};//每个电极 dcp应该在的档位

volatile unsigned char electrode_num = 0;//用来遍历电极


/*
 * 函数名：NVIC_Configuration
 * 描述  ：配置嵌套向量中断控制器NVIC
 */
void NVIC_Configuration(void)
{
	/* 一个抢占优先级, 八个响应优先级 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}


void bsp_init(void)
{
	/************* NVIC ************/
	NVIC_Configuration();			//初始化嵌套异常向量中断控制器
	
	/********* 译码器接口 **********/
	DECODER_GPIO_Config();			//初始化SN74译码器
	
	/************* KEY *************/
	KEY_GPIO_Init();				//初始化按键ic的接口
	KEY_I2c_Config();				//配置CH452
	EXTI_PD2_Config(); 				//配置按键的外部中断 


	/************** ADC*************/
	SPI_MAX186_Init();
	
	/************ USART3 ***********/
	USART3_Config();
	
	/************* TIM2 ************/
	TIM2_NVIC_Configuration();		//TIM2中断初始化
	TIM2_Configuration();			//TIM2初始化(100ms中断一次)
//	START_TIMER;					//启动定时器
	STOP_TIMER;						//停止定时器
	
	/************* DCP *************/
	SPI_MAX5483_Init();				//数字电位器初始化 
	Write_WREG(800);				//设置DCP的初始值 	
	
// 	/************* DDS *************/
// 	DDS_Init();						//初始化dds模块(AD9834)
// 	Set_Phase(PHASE_0, PI/2);       //设置相位
//     DDS_Select_Wave(Sine_Wave); 	//正弦波
//     DDS_Set_Freq(FREQ_0,465000);    //设置频率465KHz	
	
	
}


/*
 * 函数名：main
 * 描述  ：主函数
 */
int main(void)
{	
	unsigned int i = 0, j = 0; 
	bsp_init();

	/* wait interrupt */
	while(1)
	{
// 		//按下开始按键
// 		if (start_f)
// 		{
// 			switch (treat_f)
// 			{
// 			case 0 : 
// 				treat_f = 1; 
// 				break;//当前如果是待机模式, 则进入贴壁检测
// 			case 1 : 
// 				treat_f = 2; 
// 				START_TIMER;
// 				treat_tm = 0;
// 				//继电器打开
// 				GPIO_ResetBits(GPIOF, GPIO_Pin_4); //PF4拉低
// 				break;//如果当前是贴壁检测模式, 则进入射频治疗(必须贴壁检测完成以后才可以进行射频治疗)
// 			default: break;
// 			}
// 		}
// 		//如果在贴壁检测或是放电治疗时: 1. 按下了停止按键 2. 指标超标
// 		if (stop_f)
// 		{
// 			//dcp归零
// 			Write_WREG(0);
// 			//关闭输出继电器
// 			GPIO_SetBits(GPIOF, GPIO_Pin_4); //PF4拉高
// 			//关闭定时器
// 			STOP_TIMER;
// 			
// 			stop_f = 0;
// 			treat_f = 0;
// 		}
// 		Measure_W_Enable();
// 		//遍历电极
/********************多电极测试*******************************************
		for (electrode_num = 0; electrode_num < 2; electrode_num++)
		{
			DECODER_Chanal_Select(electrode_num);//选中电极
			for (i = 0; i < 5000; i++)
			{
				for (j = 0; j < 200; j++);
			}	
			W_Measure(electrode_num);
			LCD_Flush(electrode_num);	
			for (i = 0; i < 5000; i++)
			{
				for (j = 0; j < 200; j++);
			}				
		}
***************************************************************************/		
/********************单个电极测试******************************************/
////			DECODER_Chanal_Select(3);//选中电极

			while (1)
			{
// 				DECODER_Chanal_Select(electrode_num);//选中电极
// 				GPIO_SetBits(GPIOB, GPIO_Pin_1);	//PWM负赋值为1
// 				for (i = 0; i < 5000; i++)
// 				{
// 					for (j = 0; j < 200; j++);
// 				}
				
				
//				W_Measure(electrode_num);
				W_Measure(0);
				//LCD_Flush(electrode_num);
				LCD_Flush(0);
// 				LCD_Flush(1);
// 				LCD_Flush(2);
// 				LCD_Flush(3);
// 				LCD_Flush(4);
// 				LCD_Flush(5);
				
// 				GPIO_ResetBits(GPIOB, GPIO_Pin_1);	//PWM负赋值为0
// 				for (i = 0; i < 5000; i++)
// 				{
// 					for (j = 0; j < 200; j++);
// 				}
			}

/***************************************************************************/		
// 			DECODER_Chanal_Select(1);//选中电极
// 			W_Measure(1);
// 			LCD_Flush(1);	
// 			for (i = 0; i < 5000; i++)
// 			{
// 				for (j = 0; j < 200; j++);
//			}				
// 		//贴壁检测
// 		if (treat_f == 1)
// 		{
// 			Adherent_Detect();
// 		}
// 		//放电治疗
// 		if (treat_f == 2)
// 		{
// 			electrode_num++;
// 			if (electrode_num > 5)
// 			{
// 				electrode_num = 0;
// 			}
// 			if (control_mode)
// 			{
// 				//温度控制
// 				Temperature_Control();
// 			}
// 			else
// 			{
// 				//功率控制
// 				Power_Control();
// 			}
// 		}
		
		
		//检测是否超时
// 		if (treat_tm > T_Set_Val)
// 		{
// 			//dcp归零
// 			Write_WREG(0);
// 			//关闭输出继电器
// 			GPIO_SetBits(GPIOF, GPIO_Pin_4); //PF4拉高
// 			//关闭定时器
// 			STOP_TIMER;
// 			
// 			stop_f = 0;
// 			treat_f = 0;
// 		}
// 		//检测阻抗是否超标
// 	
// 		//检测温度是否超标
// 		
// 		//检测功率是否超标
// 		

//  	if (tmp != sec)
//  	{
// 			LCD_Flush();
//  			tmp = sec;
// 		}
	}
}


/**********************************************************************************/
