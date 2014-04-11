#ifndef __DECODER_H__
#define	__DECODER_H__

#include "stm32f10x.h"				

void DECODER_GPIO_Config(void);
void DECODER_Chanal_Select(unsigned char val);
void RF_Enable(void);
void Measure_W_Enable(void);


#endif
