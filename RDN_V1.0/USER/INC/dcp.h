#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f10x.h"

#define SPI_AD5231_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_15)
#define SPI_AD5231_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_15)

void SPI_AD5231_Init(void);
void AD5231_Write_RDAC(unsigned int val);

u8 SPI_AD5231_SendByte(u8 byte);

#endif /* __SPI_FLASH_H */

