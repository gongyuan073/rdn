/**********************************************************************************
 * �ļ���  :dds.c
 * ����    :ddsоƬAD9834�ĵײ�����(ʹ��CORTEX-M3ϵ��оƬ��λ���������)
 * ƽ̨    :stm32f103vet6
 * Ӳ������ -------------------------
 *         | PC6    : AD9834-FSYNC  |
 *         | PC7    : AD9834-SCLK   |
 *         | PC8    : AD9834-SDATA  |
 *         | PC9    : AD9834-RESET  |
 *		   /AVDD	: 5V			/
 *		   /AGND	: GND			/
 *          -------------------------
 * ��汾  :ST3.5.0
 * ����    :���ǻ�
 * email   :caoxinghui@foxmail.com
**********************************************************************************/
#include "stm32f10x.h"
#include "dds.h"

/****************************************************************
function name   : AD9834_Write_16Bits
description     : ��AD9834����16bit����
input           : data - Ҫд�뵽�Ĵ������16bit����
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
description     : ѡ��IOUT��������Ĳ���(���Ҳ�/���ǲ�)
                    IOUT���Ҳ� ,SIGNBITOUT���� ,дFREQREG0 ,дPHASE0
                    ad9834_write_16bit(0x2028)   һ����дFREQREG0
                    ad9834_write_16bit(0x0038)   ������дFREQREG0��LSB
                    ad9834_write_16bit(0x1038)   ������дFREQREG0��MSB
                    --------------------------------------------------
                    IOUT���ǲ� ,дPHASE0
                    ad9834_write_16bit(0x2002)   һ����дFREQREG0
                    ad9834_write_16bit(0x0002)   ������дFREQREG0��LSB
                    ad9834_write_16bit(0x1008)   ������дFREQREG0��MSB
input           : initdata - Triangle_Wave(���ǲ�) �� Sine_Wave(���Ҳ�)
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
description     : ��ʼ��AD9834��оƬ�Ľӿ�
input           : None
return          : None
*****************************************************************/
void DDS_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//ʹ��PE�˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
   // GPIO_Init(AD9834_Control_Port ,&GPIO_InitStructure) ;
     GPIO_Init(GPIOE, &GPIO_InitStructure) ;
}

/****************************************************************
function name   : Set_Phase
description     : ������λ
input           : phase_number  - Ҫ���õ���λ�Ĵ���
                  Phase_value   - Ҫ����λ�Ĵ�����д���ֵ
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
    
	DDS_Write_16Bits(PHASEREG);//���ó���λΪ0
}

/****************************************************************
function name   : DDS_Set_Freq
description     : ����Ƶ��
input           : freq_number   - Ҫ���õ�Ƶ�ʼĴ���
                  freq          - Ҫ��Ƶ�ʼĴ�����д���ֵ
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
