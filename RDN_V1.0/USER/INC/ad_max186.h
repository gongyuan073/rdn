#ifndef __AD_MAX186_H
#define __AD_MAX186_H

#include "stm32f10x.h"

#define SPI_MAX186_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_MAX186_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_4)

void SPI_MAX186_Init(void);
unsigned int MAX186_ChannelN( unsigned char channel );
u8 SPI_MAX186_SendByte(u8 byte);


#endif /* __SPI_FLASH_H */

