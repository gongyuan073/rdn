/**********************************************************************************
 * 文件名  :dds.c
 * 描述    :dds芯片AD9834的底层驱动(使用CORTEX-M3系列芯片的位带操作完成)
 * 平台    :stm32f103vet6
 * 硬件连接 -------------------------
 *         | PC6    : AD9834-FSYNC  |
 *         | PC7    : AD9834-SCLK   |
 *         | PC8    : AD9834-SDATA  |
 *         | PC9    : AD9834-RESET  |
 *		   /AVDD	: 5V			/
 *		   /AGND	: GND			/
 *          -------------------------
 * 库版本  :ST3.5.0
 * 作者    :曹星辉
 * email   :caoxinghui@foxmail.com
**********************************************************************************/
#include "stm32f10x.h"
#include "dds.h"

/****************************************************************
function name   : AD9834_Write_16Bits
description     : 向AD9834发送16bit数据
input           : data - 要写入到寄存器里的16bit数据
return          : None
*****************************************************************/
void DDS_Write_16Bits(unsigned int data)
{
    unsigned char i = 0 ;
    
    SCLK = 1 ;
    FSYNC = 0 ;
    for(i=0 ;i<16 ;i++) 
    {
        if(data & 0x8000)
          SDATA = 1; 
        else
          SDATA = 0; 
        
        SCLK = 0;
        data <<= 1;
        SCLK = 1;
    }
    
    FSYNC = 1;
}

/****************************************************************
function name   : DDS_Select_Wave
description     : 选择IOUT引脚输出的波形(正弦波/三角波)
                    IOUT正弦波 ,SIGNBITOUT方波 ,写FREQREG0 ,写PHASE0
                    ad9834_write_16bit(0x2028)   一次性写FREQREG0
                    ad9834_write_16bit(0x0038)   单独改写FREQREG0的LSB
                    ad9834_write_16bit(0x1038)   单独改写FREQREG0的MSB
                    --------------------------------------------------
                    IOUT三角波 ,写PHASE0
                    ad9834_write_16bit(0x2002)   一次性写FREQREG0
                    ad9834_write_16bit(0x0002)   单独改写FREQREG0的LSB
                    ad9834_write_16bit(0x1008)   单独改写FREQREG0的MSB
input           : initdata - Triangle_Wave(三角波) 或 Sine_Wave(正弦波)
return          : None
*****************************************************************/
void DDS_Select_Wave(unsigned int initdata)
{
    FSYNC = 1;;
    SCLK = 1;
    
    RESET = 1;
    RESET = 1;
    RESET = 0;
     
    DDS_Write_16Bits(initdata);
}
/****************************************************************
function name   : DDS_Init
description     : 初始化AD9834与芯片的接口
input           : None
return          : None
*****************************************************************/
void DDS_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//使能PE端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
   // GPIO_Init(AD9834_Control_Port ,&GPIO_InitStructure) ;
     GPIO_Init(GPIOE, &GPIO_InitStructure) ;
}

/****************************************************************
function name   : Set_Phase
description     : 设置相位
input           : phase_number  - 要设置的相位寄存器
                  Phase_value   - 要向相位寄存器中写入的值
return          : None
*****************************************************************/
void Set_Phase(unsigned char phase_number, unsigned long Phase_value)
{
    unsigned int PHASEREG = ((unsigned int)(4096.0 / (2 * PI) * Phase_value)) & 0xFFFF;
    
    PHASEREG |= (1 << 15);
    PHASEREG |= (1 << 14);
    
    if (phase_number)
    {
        PHASEREG |= (1 << 13);
    }
    else
    {
        PHASEREG &= ~(1 << 13);
    }
    
	DDS_Write_16Bits(PHASEREG);//设置初相位为0
}

/****************************************************************
function name   : DDS_Set_Freq
description     : 设置频率
input           : freq_number   - 要设置的频率寄存器
                  freq          - 要向频率寄存器中写入的值
return          : None
*****************************************************************/
void DDS_Set_Freq(unsigned char freq_number, unsigned long freq)
{
    unsigned long FREQREG = (unsigned long)(268435456.0/FCLK*freq);
    
    unsigned int FREQREG_LSB_14BIT = (unsigned int)FREQREG;
    unsigned int FREQREG_MSB_14BIT = (unsigned int)(FREQREG>>14);
    
    if(freq_number == FREQ_0)
    {
        FREQREG_LSB_14BIT &= ~(1U<<15);
        FREQREG_LSB_14BIT |= 1<<14;
        FREQREG_MSB_14BIT &= ~(1U<<15);
        FREQREG_MSB_14BIT |= 1<<14;
    }
    else
    {
        FREQREG_LSB_14BIT &= ~(1<<14);
        FREQREG_LSB_14BIT |= 1U<<15;
        FREQREG_MSB_14BIT &= ~(1<<14);
        FREQREG_MSB_14BIT |= 1U<<15;
    }
    
    DDS_Write_16Bits(FREQREG_LSB_14BIT);
    DDS_Write_16Bits(FREQREG_MSB_14BIT);
}

/********************end of file*******************************/
