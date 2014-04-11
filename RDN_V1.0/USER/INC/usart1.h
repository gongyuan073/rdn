#ifndef __USART1_H__
#define	__USART1_H__

#include "stm32f10x.h"
#include <stdio.h>

void USART3_Config(void);
int fputc(int ch, FILE *f);
void USART3_printf(USART_TypeDef* USARTx, uint8_t *Data,...);


#endif /* __USART1_H */
