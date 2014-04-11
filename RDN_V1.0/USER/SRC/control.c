#include "control.h"
#include "decoder.h"
#include "ad_max186.h"
#include "dcp_max5483.h"
#include "usart1.h"

volatile unsigned int dcp_level;
unsigned int Target_P;

extern volatile unsigned char sec, min;		//秒, 分 计时
extern volatile unsigned int P_Set_Val; //功率设置值(w)
extern volatile unsigned int W_Set_Val;	//温度设置值(°C)
extern volatile unsigned int T_Set_Val;	//时间设置值(s)
extern unsigned int Measure_P_Float[6];	//功率检测值
extern unsigned int Measure_R_Float[6];	//阻抗检测值
extern unsigned int Measure_W_Float[6];	//温度检测值

extern unsigned char electrode_num; 
extern unsigned int dcp_pos_arry[];
extern unsigned int treat_tm;

/*
*函数名	: R_P_Measure
*描述	: 对电压和电流分别取10次AD值. 初步计算出P. 由P计算出阻抗, 然后根据阻抗补偿功率 P
*  			Measure_P : 补偿过的功率    Measure_R : 补偿过的阻抗 
*/
void R_P_Measure(void) 
{
	unsigned char i;
	unsigned int tmp = 0;
	float RPM_V = 0, RPM_I = 0;
	//检测电压
	for(i = 0; i < 10; i++)
	{
		tmp += MAX186_ChannelN(1);  		
	}
	//电压:(RPM_V /4096) * 基准电压
	RPM_V = tmp / 10 / 4096.0 * 89.5;
	//检测电流
	for(i = 0; i < 10; i++)
	{
		tmp += MAX186_ChannelN(0);  								
	}
	//电流:(RPM_I /4096) * 基准电压
	RPM_I = tmp / 10.0 / 4096.0 * 446.9; //RPM_I 应该为10次的电流平均值(单位为mA)
	
	Measure_P_Float[electrode_num] = RPM_V * RPM_I;		
	Measure_R_Float[electrode_num] = 1.0 * RPM_V / RPM_I;		
}


/*
*函数名	: W_Measure
*描述	: 检测温度
*输入	: T1...T6, [0~5]
*/
void W_Measure(unsigned char num)
{
	unsigned int i, j;
	unsigned int tmp = 0;

	Measure_W_Enable();
	
	for(i = 0; i < 10; i++)
	{
		tmp += MAX186_ChannelN(num + 2);//tmp 存放10次ad的和, 通道2对应T1 ...
	}

// 	for (i = 0; i < 50000; i++)
// 	{
// 		for (j = 0; j < 20; j++);
// 	}
	//tmp = tmp / 10.0 / 4096 * 4.096 * 100; --> 真实的温度数据 = tmp / 100

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
 * 函数名：Power_Control
 * 描述  ：检测阻抗,功率和温度, 根据当前功率和目标功率进行控制
 */
void Power_Control()
{
	R_P_Measure();		//检测阻抗, 功率
	Adjust_P((unsigned int)Measure_P_Float, P_Set_Val);//调节了目标功率, 并把dcp抽头位置存放在

//	W_Measure();		//检测温度	

	electrode_num++;
	if (electrode_num > 5)
	{
		electrode_num = 0;
	}
	DECODER_Chanal_Select(electrode_num);		//下个位置的电极选中
	Write_WREG(dcp_pos_arry[electrode_num]);	//写入上次adjust_p之后dcp的数值
}


/*
*函数名	: Temperature_Control 
*描述	: 温度控制算法
*根据当前温度与设定温度来调节功率的目标值(Adjust_T)
*根据当前功率与目标功率来调节DCP的抽头位置(Adjust_P)
*
*/
void Temperature_Control(void) 
{
	R_P_Measure();	//阻抗 功率检测
//	W_Measure();	//温度检测

	Adjust_T(Measure_W_Float[electrode_num], W_Set_Val);//根据当前温度和设置温度来调整功率的目标值
	Adjust_P((unsigned int)Measure_P_Float, Target_P);//根据当前功率和目标功率来调节dcp
}



/*
*函数名: Adjust_P
*每次调节都调节到实际与设定之差的一半处
*
*如果实际值大于设定值:DCP减小(实际值和设定值之差的一半)个等级
*如果实际值小于设定值:
*实际值小于设定值1~12w,  DCP 增加1 
*实际值小于设定值13~24w, DCP 增加2 
*实际值小于设定值25~36w, DCP 增加4 
*实际值小于设定值37~48w, DCP 增加8 
*/
void Adjust_P(unsigned int Measure,unsigned int Set)
{
	unsigned char i = 0;

	if(Measure != Set)
	{
		if(Measure > Set)
		{
			i = (Measure - Set + 1) / 2;//差值的一半

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
*函数名	: Adjust_T
*描述	: 依据检测温度和设定温度来更改功率的目标值target
*/
void Adjust_T(float Measure_W,float Set_W)
{
	float Distance, Speed;
	static float Measure_W_Last_Float;
	unsigned int Measure_P = (unsigned int)Measure_P_Float;
	Distance = Set_W - Measure_W; 

	Speed=(Measure_W - Measure_W_Last_Float)/0.2;//变化率= (当前测试值 - 上次测试值)  / 温度检测的周期(TIM2, 0.2s)

	if((Distance>0.5)||(Distance<-0.5))  //如果  (当前测试值 < 设置值 - 0.5)   或     (当前测试值 > 设置值 + 0.5)
	{
		if(Distance<-1.5)		//   当前测试值 > 设置值 + 1.5
		{
			if(Measure_P > 0)	//如果实际功率大于0
			{
				Target_P=Measure_P-1;		//温度控制模式下的功率初始值赋值为( 当前测试值 - 1)
			}
		}
		else 					//   当前测试值 <= 设置值 + 1.5
		{
			if(Distance>8)		// 当前测试值 < 设置值 - 8
			{
				if(Speed>16)	//如果两次检测的值大于3.2度
				{
					if(Measure_P>0)
					{
						Target_P=Measure_P-1;
					}
				}
				else 			//两次检测的值小于等于3.2度
				{
					if(Speed<8)	//变化值 在0~1.6 之间
					{
						Target_P=Measure_P+1;
					}
					else			//变化值 在1.6~3.2 之间
					{
						Target_P=Measure_P;
					}
				}
			}
			else// (当前测试值+ 1.5)  >= 设置值>= (设置值-8)
			{
				if(Distance>4)//(当前测试值 - 4) > 设置值>= (设置值-8)
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
				else //(设置值+ 1.5) >= 当前值>= (设置值- 4)
				{
					if(Distance>2)//(设置值- 2) > 当前值>= (设置值- 4)
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
					else//(设置值+ 1.5 ) > 当前值>= (设置值- 2)
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

	Measure_W_Last_Float=Measure_W;  //将检测到的数据保存为上次检测的数据

	if(Target_P > P_Set_Val)//如果目标功率值大于设置功率, 则目标功率赋值为设置功率
	{
		Target_P = P_Set_Val;
	}
}

/*
*函数名	: Adherent_Detect
*描述	: 贴壁检测阶段每个电极上发出0.5w的功率, 然后检测数据. 注意: 先打开第一个电极, 在下次200ms到达之后先检测该电极上的参数, 
*	然后切换到下个电极. dcp进行输出, 周而复始
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
*函数名	: LCD_flush
*描述	: 在LCD屏幕上刷新采集到的所有数据
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

