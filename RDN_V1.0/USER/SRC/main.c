/**********************************************************************************
 * ʵ����  :RDN V1.0
 * ����    :�������SDA����SCL�����������key.c��SDA/SCL/SDA_IN()/SDA_OUT()�ĺ궨��
 *            �ر���SDA�������������(ע��CRH/CRL).
 * ƽ̨	   :stm32f103zet6
 * ��汾  :ST3.5.0
 *
 * ����    �����ǻ�
 * e-mail  ��caoxinghui@foxmail.com
 * ����	   : 1. 20140227 ��ʱ����ʱʱ�����ز���ȷ, ���� system_stm32f10x.c�� 1000�д�, ���Ƕ�ʱ���ǲ�׼, ����Ĳ���ȫ��0v����. 
				���: ��·��ľ���δ�ӵ�
			 2. 20140321 ��������������, SD\USB\WTD\TOUCH����δ���
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


//volatile unsigned char knob_f = 0;		//��ť�ı��λ, 1 : ��ť����ת  0 : ��ťδ����ת

volatile unsigned char sec, min;		//��, �� ��ʱ
volatile unsigned char tm_base, ms100_f;//tm_base: ÿ100ms����1; ms100_f: ÿ��100ms��ʱ��ms100_f����1; 
volatile unsigned int treat_tm;			//treat_tm: �ŵ����Ƶ���ʱ��
volatile unsigned char treat_f = 0;		//����״̬: 0; ���ڼ��: 1;  ��ʽ����: 2; 
volatile unsigned char stop_f = 0;		//ֹͣ��־: 1 ֹͣ��������, 0 ֹͣ��δ������
volatile unsigned char start_f = 0;		//��ʼ��־: 1 ��ʼ������, 0 ��ʼ��δ����

volatile unsigned char W_Set_f = 0;		//����ģʽʱ������ʶ���ڵ����¶�ֵ	
volatile unsigned char T_Set_f = 0;		//����ģʽʱ������ʾ���ڵ���ʱ��ֵ
volatile unsigned char P_Set_f = 0;		//����ģʽʱ������ʾ���ڵ��ڹ���ֵ

volatile unsigned int P_Set_Val = 0;  	//��������ֵ(w)
volatile unsigned int W_Set_Val = 70;	//�¶�����ֵ(��C)
volatile unsigned int T_Set_Val = 60;	//ʱ������ֵ(s)

unsigned int P_High_Warning = 50;		//���ʵ�����ֵ
unsigned int P_Low_Warning  = 0;		//���ʵ�����ֵ
unsigned int W_High_Warning = 80;		//�¶ȵ�����ֵ
unsigned int W_Low_Warning  = 35;		//�¶ȵ�����ֵ
unsigned int T_High_Warning  = 180;		//ʱ�������ֵ

unsigned char control_mode = 0;	//����ģʽ: 0 ���ʿ��� ; 1 �¶ȿ���;

unsigned int Measure_P_Float[6];	//���ʼ��ֵ    /* ��Ϊ��ʾ��ֻ��ʶ������, ���Բ�����Ϊfloat */
unsigned int Measure_R_Float[6];	//�迹���ֵ
unsigned int Measure_W_Float[6];	//�¶ȼ��ֵ

unsigned int dcp_pos_arry[6] = {5, 5, 5, 5, 5, 5};//ÿ���缫 dcpӦ���ڵĵ�λ

volatile unsigned char electrode_num = 0;//���������缫


/*
 * ��������NVIC_Configuration
 * ����  ������Ƕ�������жϿ�����NVIC
 */
void NVIC_Configuration(void)
{
	/* һ����ռ���ȼ�, �˸���Ӧ���ȼ� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}


void bsp_init(void)
{
	/************* NVIC ************/
	NVIC_Configuration();			//��ʼ��Ƕ���쳣�����жϿ�����
	
	/********* �������ӿ� **********/
	DECODER_GPIO_Config();			//��ʼ��SN74������
	
	/************* KEY *************/
	KEY_GPIO_Init();				//��ʼ������ic�Ľӿ�
	KEY_I2c_Config();				//����CH452
	EXTI_PD2_Config(); 				//���ð������ⲿ�ж� 


	/************** ADC*************/
	SPI_MAX186_Init();
	
	/************ USART3 ***********/
	USART3_Config();
	
	/************* TIM2 ************/
	TIM2_NVIC_Configuration();		//TIM2�жϳ�ʼ��
	TIM2_Configuration();			//TIM2��ʼ��(100ms�ж�һ��)
//	START_TIMER;					//������ʱ��
	STOP_TIMER;						//ֹͣ��ʱ��
	
	/************* DCP *************/
	SPI_MAX5483_Init();				//���ֵ�λ����ʼ�� 
	Write_WREG(800);				//����DCP�ĳ�ʼֵ 	
	
// 	/************* DDS *************/
// 	DDS_Init();						//��ʼ��ddsģ��(AD9834)
// 	Set_Phase(PHASE_0, PI/2);       //������λ
//     DDS_Select_Wave(Sine_Wave); 	//���Ҳ�
//     DDS_Set_Freq(FREQ_0,465000);    //����Ƶ��465KHz	
	
	
}


/*
 * ��������main
 * ����  ��������
 */
int main(void)
{	
	unsigned int i = 0, j = 0; 
	bsp_init();

	/* wait interrupt */
	while(1)
	{
// 		//���¿�ʼ����
// 		if (start_f)
// 		{
// 			switch (treat_f)
// 			{
// 			case 0 : 
// 				treat_f = 1; 
// 				break;//��ǰ����Ǵ���ģʽ, ��������ڼ��
// 			case 1 : 
// 				treat_f = 2; 
// 				START_TIMER;
// 				treat_tm = 0;
// 				//�̵�����
// 				GPIO_ResetBits(GPIOF, GPIO_Pin_4); //PF4����
// 				break;//�����ǰ�����ڼ��ģʽ, �������Ƶ����(�������ڼ������Ժ�ſ��Խ�����Ƶ����)
// 			default: break;
// 			}
// 		}
// 		//��������ڼ����Ƿŵ�����ʱ: 1. ������ֹͣ���� 2. ָ�곬��
// 		if (stop_f)
// 		{
// 			//dcp����
// 			Write_WREG(0);
// 			//�ر�����̵���
// 			GPIO_SetBits(GPIOF, GPIO_Pin_4); //PF4����
// 			//�رն�ʱ��
// 			STOP_TIMER;
// 			
// 			stop_f = 0;
// 			treat_f = 0;
// 		}
// 		Measure_W_Enable();
// 		//�����缫
/********************��缫����*******************************************
		for (electrode_num = 0; electrode_num < 2; electrode_num++)
		{
			DECODER_Chanal_Select(electrode_num);//ѡ�е缫
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
/********************�����缫����******************************************/
////			DECODER_Chanal_Select(3);//ѡ�е缫

			while (1)
			{
// 				DECODER_Chanal_Select(electrode_num);//ѡ�е缫
// 				GPIO_SetBits(GPIOB, GPIO_Pin_1);	//PWM����ֵΪ1
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
				
// 				GPIO_ResetBits(GPIOB, GPIO_Pin_1);	//PWM����ֵΪ0
// 				for (i = 0; i < 5000; i++)
// 				{
// 					for (j = 0; j < 200; j++);
// 				}
			}

/***************************************************************************/		
// 			DECODER_Chanal_Select(1);//ѡ�е缫
// 			W_Measure(1);
// 			LCD_Flush(1);	
// 			for (i = 0; i < 5000; i++)
// 			{
// 				for (j = 0; j < 200; j++);
//			}				
// 		//���ڼ��
// 		if (treat_f == 1)
// 		{
// 			Adherent_Detect();
// 		}
// 		//�ŵ�����
// 		if (treat_f == 2)
// 		{
// 			electrode_num++;
// 			if (electrode_num > 5)
// 			{
// 				electrode_num = 0;
// 			}
// 			if (control_mode)
// 			{
// 				//�¶ȿ���
// 				Temperature_Control();
// 			}
// 			else
// 			{
// 				//���ʿ���
// 				Power_Control();
// 			}
// 		}
		
		
		//����Ƿ�ʱ
// 		if (treat_tm > T_Set_Val)
// 		{
// 			//dcp����
// 			Write_WREG(0);
// 			//�ر�����̵���
// 			GPIO_SetBits(GPIOF, GPIO_Pin_4); //PF4����
// 			//�رն�ʱ��
// 			STOP_TIMER;
// 			
// 			stop_f = 0;
// 			treat_f = 0;
// 		}
// 		//����迹�Ƿ񳬱�
// 	
// 		//����¶��Ƿ񳬱�
// 		
// 		//��⹦���Ƿ񳬱�
// 		

//  	if (tmp != sec)
//  	{
// 			LCD_Flush();
//  			tmp = sec;
// 		}
	}
}


/**********************************************************************************/
