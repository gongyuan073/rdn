#include "control.h"
#include "decoder.h"
#include "ad_max186.h"
#include "dcp_max5483.h"
#include "usart1.h"

volatile unsigned int dcp_level;
unsigned int Target_P;

extern volatile unsigned char sec, min;		//��, �� ��ʱ
extern volatile unsigned int P_Set_Val; //��������ֵ(w)
extern volatile unsigned int W_Set_Val;	//�¶�����ֵ(��C)
extern volatile unsigned int T_Set_Val;	//ʱ������ֵ(s)
extern unsigned int Measure_P_Float[6];	//���ʼ��ֵ
extern unsigned int Measure_R_Float[6];	//�迹���ֵ
extern unsigned int Measure_W_Float[6];	//�¶ȼ��ֵ

extern unsigned char electrode_num; 
extern unsigned int dcp_pos_arry[];
extern unsigned int treat_tm;

/*
*������	: R_P_Measure
*����	: �Ե�ѹ�͵����ֱ�ȡ10��ADֵ. ���������P. ��P������迹, Ȼ������迹�������� P
*  			Measure_P : �������Ĺ���    Measure_R : ���������迹 
*/
void R_P_Measure(void) 
{
	unsigned char i;
	unsigned int tmp = 0;
	float RPM_V = 0, RPM_I = 0;
	//����ѹ
	for(i = 0; i < 10; i++)
	{
		tmp += MAX186_ChannelN(1);  		
	}
	//��ѹ:(RPM_V /4096) * ��׼��ѹ
	RPM_V = tmp / 10 / 4096.0 * 89.5;
	//������
	for(i = 0; i < 10; i++)
	{
		tmp += MAX186_ChannelN(0);  								
	}
	//����:(RPM_I /4096) * ��׼��ѹ
	RPM_I = tmp / 10.0 / 4096.0 * 446.9; //RPM_I Ӧ��Ϊ10�εĵ���ƽ��ֵ(��λΪmA)
	
	Measure_P_Float[electrode_num] = RPM_V * RPM_I;		
	Measure_R_Float[electrode_num] = 1.0 * RPM_V / RPM_I;		
}


/*
*������	: W_Measure
*����	: ����¶�
*����	: T1...T6, [0~5]
*/
void W_Measure(unsigned char num)
{
	unsigned int i, j;
	unsigned int tmp = 0;

	Measure_W_Enable();
	
	for(i = 0; i < 10; i++)
	{
		tmp += MAX186_ChannelN(num + 2);//tmp ���10��ad�ĺ�, ͨ��2��ӦT1 ...
	}

// 	for (i = 0; i < 50000; i++)
// 	{
// 		for (j = 0; j < 20; j++);
// 	}
	//tmp = tmp / 10.0 / 4096 * 4.096 * 100; --> ��ʵ���¶����� = tmp / 100

//	Measure_W_Float[electrode_num] = tmp / 5.0 / 4096.0 * 4.096 * 100.0;
//	Measure_W_Float[electrode_num] = tmp / 100.0;
	Measure_W_Float[num] = tmp / 100.0;
//	Measure_W_Float[num] = 	MAX186_ChannelN(num + 2) / 10.0; 
//	Measure_W_Float[num] = tmp / 5.0 / 4096.0 * 4.16 * 100.0;;
// 	k = MAX186_ChannelN(2);
// 	uart_cmd[4] = 0x00; 
// 	uart_cmd[6] = k >> 8;
// 	uart_cmd[7] = k;
// 	for (i = 0; i < 8; i++)
// 	{
// 		printf("%c", uart_cmd[i]);
// 	}
// 	
// 	tmp = k * 4.096 * 100.0 / 4096.0 ;
// 	uart_cmd[4] = 0x0C; 
// 	uart_cmd[6] = (unsigned char)(tmp >> 8);
// 	uart_cmd[7] = (unsigned char)tmp;
// 	for (i = 0; i < 8; i++)
// 	{
// 		printf("%c", uart_cmd[i]);
// 	}
	
//	RF_Enable();
}


/*
 * ��������Power_Control
 * ����  ������迹,���ʺ��¶�, ���ݵ�ǰ���ʺ�Ŀ�깦�ʽ��п���
 */
void Power_Control()
{
	R_P_Measure();		//����迹, ����
	Adjust_P((unsigned int)Measure_P_Float, P_Set_Val);//������Ŀ�깦��, ����dcp��ͷλ�ô����

//	W_Measure();		//����¶�	

	electrode_num++;
	if (electrode_num > 5)
	{
		electrode_num = 0;
	}
	DECODER_Chanal_Select(electrode_num);		//�¸�λ�õĵ缫ѡ��
	Write_WREG(dcp_pos_arry[electrode_num]);	//д���ϴ�adjust_p֮��dcp����ֵ
}


/*
*������	: Temperature_Control 
*����	: �¶ȿ����㷨
*���ݵ�ǰ�¶����趨�¶������ڹ��ʵ�Ŀ��ֵ(Adjust_T)
*���ݵ�ǰ������Ŀ�깦��������DCP�ĳ�ͷλ��(Adjust_P)
*
*/
void Temperature_Control(void) 
{
	R_P_Measure();	//�迹 ���ʼ��
//	W_Measure();	//�¶ȼ��

	Adjust_T(Measure_W_Float[electrode_num], W_Set_Val);//���ݵ�ǰ�¶Ⱥ������¶����������ʵ�Ŀ��ֵ
	Adjust_P((unsigned int)Measure_P_Float, Target_P);//���ݵ�ǰ���ʺ�Ŀ�깦��������dcp
}



/*
*������: Adjust_P
*ÿ�ε��ڶ����ڵ�ʵ�����趨֮���һ�봦
*
*���ʵ��ֵ�����趨ֵ:DCP��С(ʵ��ֵ���趨ֵ֮���һ��)���ȼ�
*���ʵ��ֵС���趨ֵ:
*ʵ��ֵС���趨ֵ1~12w,  DCP ����1 
*ʵ��ֵС���趨ֵ13~24w, DCP ����2 
*ʵ��ֵС���趨ֵ25~36w, DCP ����4 
*ʵ��ֵС���趨ֵ37~48w, DCP ����8 
*/
void Adjust_P(unsigned int Measure,unsigned int Set)
{
	unsigned char i = 0;

	if(Measure != Set)
	{
		if(Measure > Set)
		{
			i = (Measure - Set + 1) / 2;//��ֵ��һ��

			if(i != 0)
			{
				dcp_level -= i;
				//AD5231_Write_RDAC(dcp_level);
				dcp_pos_arry[electrode_num] = dcp_level;
			}
		}
		else
		{
			i = (Set - Measure + 11) / 12;

			if(i != 0)
			{
				dcp_level += i;
				//AD5231_Write_RDAC(dcp_level);
				dcp_pos_arry[electrode_num] = dcp_level;
			}
		}
	}
}



/*
*������	: Adjust_T
*����	: ���ݼ���¶Ⱥ��趨�¶������Ĺ��ʵ�Ŀ��ֵtarget
*/
void Adjust_T(float Measure_W,float Set_W)
{
	float Distance, Speed;
	static float Measure_W_Last_Float;
	unsigned int Measure_P = (unsigned int)Measure_P_Float;
	Distance = Set_W - Measure_W; 

	Speed=(Measure_W - Measure_W_Last_Float)/0.2;//�仯��= (��ǰ����ֵ - �ϴβ���ֵ)  / �¶ȼ�������(TIM2, 0.2s)

	if((Distance>0.5)||(Distance<-0.5))  //���  (��ǰ����ֵ < ����ֵ - 0.5)   ��     (��ǰ����ֵ > ����ֵ + 0.5)
	{
		if(Distance<-1.5)		//   ��ǰ����ֵ > ����ֵ + 1.5
		{
			if(Measure_P > 0)	//���ʵ�ʹ��ʴ���0
			{
				Target_P=Measure_P-1;		//�¶ȿ���ģʽ�µĹ��ʳ�ʼֵ��ֵΪ( ��ǰ����ֵ - 1)
			}
		}
		else 					//   ��ǰ����ֵ <= ����ֵ + 1.5
		{
			if(Distance>8)		// ��ǰ����ֵ < ����ֵ - 8
			{
				if(Speed>16)	//������μ���ֵ����3.2��
				{
					if(Measure_P>0)
					{
						Target_P=Measure_P-1;
					}
				}
				else 			//���μ���ֵС�ڵ���3.2��
				{
					if(Speed<8)	//�仯ֵ ��0~1.6 ֮��
					{
						Target_P=Measure_P+1;
					}
					else			//�仯ֵ ��1.6~3.2 ֮��
					{
						Target_P=Measure_P;
					}
				}
			}
			else// (��ǰ����ֵ+ 1.5)  >= ����ֵ>= (����ֵ-8)
			{
				if(Distance>4)//(��ǰ����ֵ - 4) > ����ֵ>= (����ֵ-8)
				{
					if(Speed>8)
					{
						if(Measure_P>0)
						{
							Target_P=Measure_P-1;
						}
					}
					else
					{
						if(Speed<4)
						{
							Target_P=Measure_P+1;
						}
						else
						{
							Target_P=Measure_P;
						}
					}
				}
				else //(����ֵ+ 1.5) >= ��ǰֵ>= (����ֵ- 4)
				{
					if(Distance>2)//(����ֵ- 2) > ��ǰֵ>= (����ֵ- 4)
					{
						if(Speed>4)
						{
							if(Measure_P>0)
							{
								Target_P=Measure_P-1;
							}
						}
						else
						{
							if(Speed<2)
							{
								Target_P=Measure_P+1;
							}
							else
							{
								Target_P=Measure_P;
							}
						}
					}
					else//(����ֵ+ 1.5 ) > ��ǰֵ>= (����ֵ- 2)
					{
						if(Speed>2)
						{
							if(Measure_P>0)
							{
								Target_P=Measure_P-1;
							}
						}
						else
						{
							if(Speed<1)
							{
								Target_P = Measure_P+1;
							}
							else
							{
								Target_P = Measure_P;
							}
						}
					}
				}
			}
		}
	}

	Measure_W_Last_Float=Measure_W;  //����⵽�����ݱ���Ϊ�ϴμ�������

	if(Target_P > P_Set_Val)//���Ŀ�깦��ֵ�������ù���, ��Ŀ�깦�ʸ�ֵΪ���ù���
	{
		Target_P = P_Set_Val;
	}
}

/*
*������	: Adherent_Detect
*����	: ���ڼ��׶�ÿ���缫�Ϸ���0.5w�Ĺ���, Ȼ��������. ע��: �ȴ򿪵�һ���缫, ���´�200ms����֮���ȼ��õ缫�ϵĲ���, 
*	Ȼ���л����¸��缫. dcp�������, �ܶ���ʼ
*
*/
void Adherent_Detect(void)
{
	static unsigned char first_RF = 1;
	
	if (first_RF)
	{
		Write_WREG(5);
		first_RF = 0;
		
		return;
	}
	R_P_Measure();
	
	electrode_num++;
	if (electrode_num > 5)
	{
		electrode_num = 0;
	}
	DECODER_Chanal_Select(electrode_num);
	Write_WREG(5);
}

/*
*������	: LCD_flush
*����	: ��LCD��Ļ��ˢ�²ɼ�������������
*
*/
void LCD_Flush(unsigned char electrode_num)
{
	unsigned char i = 0;
	unsigned char uart_cmd[9] = {0x5A, 0xA5, 0x05, 0x82, 0x25, 0x00, 0x00, 0x00, 0x00};
	
	switch (electrode_num)
	{
	case 0: 
		uart_cmd[4] = 0x0C; 
		uart_cmd[6] = Measure_W_Float[0] >> 8;
		uart_cmd[7] = Measure_W_Float[0];
		break;
	case 1: 
		uart_cmd[4] = 0x0D; 
		uart_cmd[6] = Measure_W_Float[1] >> 8;
		uart_cmd[7] = Measure_W_Float[1];
		break;
	case 2:
		uart_cmd[4] = 0x0E; 
		uart_cmd[6] = Measure_W_Float[2] >> 8;
		uart_cmd[7] = Measure_W_Float[2];
		break;
	case 3:
		uart_cmd[4] = 0x1C; 
		uart_cmd[6] = Measure_W_Float[3] >> 8;
		uart_cmd[7] = Measure_W_Float[3];
		break;
	case 4:
		uart_cmd[4] = 0x1D; 
		uart_cmd[6] = Measure_W_Float[4] >> 8;
		uart_cmd[7] = Measure_W_Float[4];
		break;
	case 5:
		uart_cmd[4] = 0x1E; 
		uart_cmd[6] = Measure_W_Float[5] >> 8;
		uart_cmd[7] = Measure_W_Float[5];
	break;
	default: break;
	}

	for (i = 0; i < 8; i++)
	{
		printf("%c", uart_cmd[i]);
	}
}

