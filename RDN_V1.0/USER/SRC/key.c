/* ***************************
 Website:  http://wch.cn
 Email:    tech@wch.cn
 Author:   W.ch 2005.4
 Description:
	1.��������оƬCH452���߷�ʽ(��׼���߷�ʽ):
	 ---------------
	|	PD0 -- CLK	|
	|	PD1 -- SDA	|
	|	PD2 -- INT	|
	 ---------------
	2. KEY �����߽ӿ��ӳ���(���������ж�, ���������ж�Ϊ3��)
	3. ����delay�����޸Ĵ����ٶ�, ÿ�ζ�ȡ��ֵ��110~140us֮��
*****************************/

#include "CH452CMD.H"
#include "key.h"

/* ��ʼ��IIC�ӿڵ����� */
void KEY_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1); 		//PD0,PD1 �����
	SCL_OUT();
}

/*	���ð�������ic, �򿪰�������ʾ���� */
void KEY_I2c_Config(void)
{
	KEY_Write(CH452_RESET);	//оƬ��λ
// 	delay();
// 	delay();
// 	delay();
// 	delay();
	//	KEY_Write(CH452_BCD);
	delay();
	delay();
	KEY_Write(CH452_SYSON2);//����������led

}
/* ģ��iic���ߵĲ�����ʼ */
void KEY_I2c_Start(void)
{
	DISABLE_KEY_INTERRUPT;  //��ֹ�����ж�,��ֹһ��ʼ�ͱ�CH452�ж�
	SDA_OUT();   			/* ����SDAΪ������� */
	KEY_SDA_W = 1;   		/*������ʼ�����������ź�*/
	
	KEY_SCL = 1;
							/* ����SCLΪ������� */
	delay();
	KEY_SDA_W = 0;   		/* SDA����͵�ƽ, ������ʼ�ź� */
	delay();      
	KEY_SCL = 0;   			/* ǯסiic����, ׼�����ͻ�������� */
	delay();
}
/* ģ��iic���ߵĲ������� */
void KEY_I2c_Stop(void)
{
	SDA_OUT();   			/* ����SDAΪ������� */
	KEY_SDA_W = 0;

	delay();
	KEY_SCL = 1;
	delay();
	KEY_SDA_W = 1;  		/* ����iic���߽����ź� */
	delay();
	SDA_IN();   			/* ����SDAΪ���뷽�� */
	delay();
  	ENABLE_KEY_INTERRUPT;
}

/*  ͨ��ģ��iic����дһ���ֽ�����	*/
void KEY_I2c_WrByte(unsigned char dat)	
{
	unsigned char i;
	SDA_OUT();   			/* ����SDAΪ������� */
	for(i=0;i!=8;i++)  	// ���8λ����
	{
		if(dat&0x80) 
		{
			KEY_SDA_W = 1;
		}
		else
		{
			KEY_SDA_W = 0;
		}
		delay();
		KEY_SCL = 1;
		dat<<=1;
		delay();
		delay();
		KEY_SCL = 0;
		delay();
	}
	//SDA_IN();  	 /* ����SDAΪ���뷽�� */
	KEY_SDA_W = 1;	//????????????????//
	delay();
	KEY_SCL = 1;  	// ����Ӧ��
	delay();
	delay();
	KEY_SCL = 0;
	delay();
}

/*	ͨ��iic���߶�ȡһ���ֽ�����	*/
unsigned char  KEY_I2c_RdByte(void)
{
	unsigned char dat = 0;
	unsigned char i;
	
	SDA_OUT();		/* ����SDAΪ������� */
	KEY_SDA_W = 1;	// SDA�ø�

	SDA_IN();   		/* ����SDAΪ���뷽�� */
	for(i=0;i!=8;i++)  	// ����8λ����
	{
		
		KEY_SCL = 1;	// SCL�ø�
		delay();
		delay();
		dat<<=1;

		if(KEY_SDA_R) dat++;  	// ����1λ
		KEY_SCL = 0;
		delay();
		delay();
	}
	SDA_OUT();
	KEY_SDA_W = 1;
	delay();
	KEY_SCL = 1;  		// ������ЧӦ��
	delay();
	delay();
	KEY_SCL = 0;
	delay();
	return(dat);
}
/*	�򰴼�����оƬ����������	*/
void KEY_Write(unsigned short cmd)	
{
	KEY_I2c_Start();               /*��������*/
   	KEY_I2c_WrByte((unsigned char)(cmd>>7) & KEY_I2C_MASK | KEY_I2C_ADDR1);  // KEY?ADDR=1ʱ(Ĭ��)
   	KEY_I2c_WrByte((unsigned char)cmd);     /*��������*/
  	KEY_I2c_Stop();                 /*��������*/ 
}

/*	��ȡ��ֵ	*/
unsigned char KEY_Read(void)		
{
	unsigned char keycode;

   	KEY_I2c_Start();                /*��������*/
   	KEY_I2c_WrByte((unsigned char)(CH452_GET_KEY>>7)&KEY_I2C_MASK|0x01|KEY_I2C_ADDR1);    // ��������CH452����, ��ADDR=0, �����ΪCH452_I2C_ADDR0
   	keycode=KEY_I2c_RdByte();      	/*��ȡ����*/
	delay();
	delay();
	KEY_I2c_Stop();                	/*��������*/ 
	
	return(keycode);
}

















