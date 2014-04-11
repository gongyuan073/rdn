#ifndef __DDS_H__
 #define __DDS_H__

#include "stm32f10x.h"
#include "bitbank.h"

//DDS AD9834�ӿ� 
#define FSYNC   PEout(10)
#define SCLK    PEout(11)
#define SDATA   PEout(12)
#define RESET   PEout(13)

#define FCLK 75000000UL
#define FREQ_0 0
#define FREQ_1 1 
#define PHASE_0 0
#define PHASE_1 1

//���ǲ�
#define Triangle_Wave    0x2002
//���Ҳ�
#define Sine_Wave  0x2028

#define PI 3.14

void DDS_Write_16Bits(unsigned int data);
void DDS_Select_Wave(unsigned int initdata);
void DDS_Init(void);
void DDS_Set_Freq(unsigned char freq_number, unsigned long freq);
void Set_Phase(unsigned char phase_number, unsigned long Phase_value);


 #endif
