/**********************************************************************************
 * 文件名  ：ad_max186.c
 * 描述    ：spi 片上设备驱动  
 * 实验平台：STM32F103VET6
 * 硬件连接 -----------------------------
 *         | PA4-SPI1-NSS  : MAX186-CS   |
 *         | PA5-SPI1-SCK  : MAX186-SCLK |
 *         | PA6-SPI1-MISO : MAX186-DOUT |
 *         | PA7-SPI1-MOSI : MAX186-DIN  |
 *          -----------------------------
 * 库版本  ：ST3.5.0
 * 
 *
**********************************************************************************/
#include "ad_max186.h"



/* Private typedef -----------------------------------------------------------*/

#define Dummy_Byte                0x7e

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_MAX186_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /*< SPI_FLASH_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
 
  
  /*!< Configure SPI_FLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  SPI_MAX186_CS_HIGH();

  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}



static void delay_ms(unsigned char val)
{
	unsigned char i;
	for (i = 10*val; i > 0; i--)
	{
		;
	}
}

/*******************************************************************************
* Function Name  : MAX186_ChannelN
* Description    : 实际电压 = (返回值 / 4096) * 4.096v
* Input          : 通道: 可选参数为 0 ~ 7.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
unsigned int MAX186_ChannelN( unsigned char channel )
{
	unsigned short temp = 0;
	unsigned char cmd = 0, val = 0;
	
	switch(channel)
	{
	case 0:	cmd = 0x8E; break;
	case 1:	cmd = 0xCE; break;
	case 2:	cmd = 0x9E; break;
	case 3:	cmd = 0xDE; break;
	case 4:	cmd = 0xAE; break;
	case 5:	cmd = 0xEE; break;
	case 6:	cmd = 0xBE; break;
	case 7:	cmd = 0xFE; break;
	default:	break;
	}
	
	SPI_MAX186_CS_LOW();     //低电平选通允许操作
	
	temp  = SPI_MAX186_SendByte(cmd);
	delay_ms(1);   
	temp  = SPI_MAX186_SendByte(Dummy_Byte);
	temp  = temp << 8; 
	//temp += SPI_MAX186_SendByte(Dummy_Byte);
	val = SPI_MAX186_SendByte(Dummy_Byte);
	temp = temp | val;
	
	SPI_MAX186_CS_HIGH();     //高电平禁止操作
	temp  = (temp >>3) & 0xFFF;
	
	return temp;
}



/*******************************************************************************
* Function Name  : SPI_MAX186_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_MAX186_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send Half Word through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, HalfWord);

  /* Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}


// /*******************************************************************************
// * Function Name  : SPI_FLASH_WaitForWriteEnd
// * Description    : Polls the status of the Write In Progress (WIP) flag in the
// *                  FLASH's status  register  and  loop  until write  opertaion
// *                  has completed.
// * Input          : None
// * Output         : None
// * Return         : None
// *******************************************************************************/
// void SPI_FLASH_WaitForWriteEnd(void)
// {
//   u8 FLASH_Status = 0;

//   /* Select the FLASH: Chip Select low */
//   SPI_FLASH_CS_LOW();

//   /* Send "Read Status Register" instruction */
//   SPI_FLASH_SendByte(W25X_ReadStatusReg);

//   /* Loop as long as the memory is busy with a write cycle */
//   do
//   {
//     /* Send a dummy byte to generate the clock needed by the FLASH
//     and put the value of the status register in FLASH_Status variable */
//     FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	 
//   }
//   while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

//   /* Deselect the FLASH: Chip Select high */
//   SPI_FLASH_CS_HIGH();
// }



/******************************END OF FILE*****************************/
