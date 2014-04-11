#ifndef __DCP_MAX5483_H__
#define __DCP_MAX5482_H__

#include "stm32f10x.h"

#define SPI_MAX5483_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_12)
#define SPI_MAX5483_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_12)

void SPI_MAX5483_Init(void);
void Write_WREG(unsigned int val);
u8 SPI_MAX5483_SendByte(u8 byte);

#endif /* __SPI_FLASH_H */

