/* ***************************
 Website:  http://wch.cn
 Email:    tech@wch.cn
 Author:   W.ch 2005.4
 Description:
	1.按键控制芯片CH452接线方式(标准两线方式):
	 ---------------
	|	PD0 -- CLK	|
	|	PD1 -- SDA	|
	|	PD2 -- INT	|
	 ---------------
	2. KEY 的两线接口子程序(不含按键中断, 若含按键中断为3线)
	3. 更改delay可以修改传输速度, 每次读取键值在110~140us之间
*****************************/

#include "CH452CMD.H"
#include "key.h"

/* 初始化IIC接口的引脚 */
void KEY_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1); 		//PD0,PD1 输出高
	SCL_OUT();
}

/*	配置按键控制ic, 打开按键和显示功能 */
void KEY_I2c_Config(void)
{
	KEY_Write(CH452_RESET);	//芯片复位
// 	delay();
// 	delay();
// 	delay();
// 	delay();
	//	KEY_Write(CH452_BCD);
	delay();
	delay();
	KEY_Write(CH452_SYSON2);//开启按键和led

}
/* 模拟iic总线的操作起始 */
void KEY_I2c_Start(void)
{
	DISABLE_KEY_INTERRUPT;  //禁止键盘中断,防止一开始就被CH452中断
	SDA_OUT();   			/* 设置SDA为输出方向 */
	KEY_SDA_W = 1;   		/*发送起始条件的数据信号*/
	
	KEY_SCL = 1;
							/* 设置SCL为输出方向 */
	delay();
	KEY_SDA_W = 0;   		/* SDA输出低电平, 发送起始信号 */
	delay();      
	KEY_SCL = 0;   			/* 钳住iic总线, 准备发送或接收数据 */
	delay();
}
/* 模拟iic总线的操作结束 */
void KEY_I2c_Stop(void)
{
	SDA_OUT();   			/* 设置SDA为输出方向 */
	KEY_SDA_W = 0;

	delay();
	KEY_SCL = 1;
	delay();
	KEY_SDA_W = 1;  		/* 发送iic总线结束信号 */
	delay();
	SDA_IN();   			/* 设置SDA为输入方向 */
	delay();
  	ENABLE_KEY_INTERRUPT;
}

/*  通过模拟iic总线写一个字节数据	*/
void KEY_I2c_WrByte(unsigned char dat)	
{
	unsigned char i;
	SDA_OUT();   			/* 设置SDA为输出方向 */
	for(i=0;i!=8;i++)  	// 输出8位数据
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
	//SDA_IN();  	 /* 设置SDA为输入方向 */
	KEY_SDA_W = 1;	//????????????????//
	delay();
	KEY_SCL = 1;  	// 接收应答
	delay();
	delay();
	KEY_SCL = 0;
	delay();
}

/*	通过iic总线读取一个字节数据	*/
unsigned char  KEY_I2c_RdByte(void)
{
	unsigned char dat = 0;
	unsigned char i;
	
	SDA_OUT();		/* 设置SDA为输出方向 */
	KEY_SDA_W = 1;	// SDA置高

	SDA_IN();   		/* 设置SDA为输入方向 */
	for(i=0;i!=8;i++)  	// 输入8位数据
	{
		
		KEY_SCL = 1;	// SCL置高
		delay();
		delay();
		dat<<=1;

		if(KEY_SDA_R) dat++;  	// 输入1位
		KEY_SCL = 0;
		delay();
		delay();
	}
	SDA_OUT();
	KEY_SDA_W = 1;
	delay();
	KEY_SCL = 1;  		// 发出无效应答
	delay();
	delay();
	KEY_SCL = 0;
	delay();
	return(dat);
}
/*	向按键控制芯片发送命令字	*/
void KEY_Write(unsigned short cmd)	
{
	KEY_I2c_Start();               /*启动总线*/
   	KEY_I2c_WrByte((unsigned char)(cmd>>7) & KEY_I2C_MASK | KEY_I2C_ADDR1);  // KEY?ADDR=1时(默认)
   	KEY_I2c_WrByte((unsigned char)cmd);     /*发送数据*/
  	KEY_I2c_Stop();                 /*结束总线*/ 
}

/*	读取键值	*/
unsigned char KEY_Read(void)		
{
	unsigned char keycode;

   	KEY_I2c_Start();                /*启动总线*/
   	KEY_I2c_WrByte((unsigned char)(CH452_GET_KEY>>7)&KEY_I2C_MASK|0x01|KEY_I2C_ADDR1);    // 若有两个CH452并联, 当ADDR=0, 需更改为CH452_I2C_ADDR0
   	keycode=KEY_I2c_RdByte();      	/*读取数据*/
	delay();
	delay();
	KEY_I2c_Stop();                	/*结束总线*/ 
	
	return(keycode);
}

















