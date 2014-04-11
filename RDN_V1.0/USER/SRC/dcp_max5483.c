/**********************************************************************************
 * 文件名  ：dcp_max5483.c
 * 描述    ：spi 片上设备驱动  
 * 实验平台：STM32F103VET6
 * 硬件连接 -----------------------------
 *         | PA15-SPI3-NSS : MAX5483-CS   |
 *         | PB3-SPI3-SCK  : MAX5483-SCLK |
 *         | PB4-SPI3-MISO : MAX5483-DOUT |
 *         | PB5-SPI3-MOSI : MAX5483-DIN  |
 *          -----------------------------
 * 库版本  ：ST3.5.0
 * 
 * 
**********************************************************************************/
#include "dcp_max5483.h"



/* Private typedef -----------------------------------------------------------*/

#define Dummy_Byte                0x0

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_MAX5483_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	/*开启复用时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	

  /*< SPI_FLASH_SPI Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//spi3与JLINK引脚复用 ，因此需禁止JTAG . JTAG-DP Disabled and SW-DP Enabled
  
  /*!< Configure SPI_FLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  SPI_MAX5483_CS_HIGH();

  /* SPI3 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI3, &SPI_InitStructure);

  /* Enable SPI3  */
  SPI_Cmd(SPI3, ENABLE);
}



void delay_ms(unsigned char val)
{
	unsigned char i;
	for (i = 10*val; i > 0; i--)
	{
		;
	}
}

/*******************************************************************************
* Function Name  : Write_WREG
* Description    : 向DCP的RDAC写入数据, 由此更改抽头位置
* Input          : DCP抽头位置值
* Output         : None
* Return         : None
*******************************************************************************/
void Write_WREG(unsigned int val)
{
	unsigned char tmp1, tmp2;
	
	SPI_MAX5483_CS_LOW();
//	delay_ms(1);
	
	tmp1 = (unsigned char)(val >> 2);	//高字节
	tmp2 = (unsigned char)((val << 6) & 0xC0);	//低字节
	SPI_MAX5483_SendByte(0x00);

	SPI_MAX5483_SendByte(tmp1);

	SPI_MAX5483_SendByte(tmp2);
	
//	delay_ms(1);
	SPI_MAX5483_CS_HIGH();
}


/*******************************************************************************
* Function Name  : SPI_MAX186_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_MAX5483_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI3, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI3);
}







/******************************END OF FILE*****************************/
