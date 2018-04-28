
#include "stm32f10x.h"
#include "spiHandler.h"
#include "wizchip_conf.h"

void Delay(volatile unsigned int nCount)
{
	for(; nCount!= 0;nCount--);
}

uint8_t wizchip_rw(uint8_t byte)
{
	int retry = 0;

	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_TXE) == RESET)
	{
		retry++;
		if(retry>400)
			return 0;
	}

	/*!< Send byte through the SPI1, SPI2 peripheral */
	SPI_I2S_SendData(W5500_SPI, byte);

	retry = 0;

	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_RXNE) == RESET)
	{
		retry++;
		if(retry>400)
			return 0;
	}

	/*!< Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(W5500_SPI);
}

void  wizchip_select(void)
{
	GPIO_ResetBits(W5500_CS_GPIO_PORT, W5500_CS_PIN);
}

void  wizchip_deselect(void)
{
	GPIO_SetBits(W5500_CS_GPIO_PORT, W5500_CS_PIN);
}

void  wizchip_write(uint8_t wb)
{
	wizchip_rw(wb);
}

uint8_t wizchip_read(void)
{
	return wizchip_rw(0xFF);
}

void W5500_SPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
				RCC_APB2Periph_AFIO, ENABLE);

	/*!< Configure W5500_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = W5500_SPI_SCK_PIN | W5500_SPI_MISO_PIN | W5500_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(W5500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure W5500_CS_PIN pin: W5500 CS pin */
	GPIO_InitStructure.GPIO_Pin = W5500_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W5500_CS_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(W5500_SPI, &SPI_InitStructure);

	/*!< Enable the W5500_SPI  */
	SPI_Cmd(W5500_SPI, ENABLE);
}

static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  // Port C Output
  GPIO_InitStructure.GPIO_Pin = W5500_RESET_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(W5500_RESET_PORT, &GPIO_InitStructure);

  GPIO_SetBits(W5500_RESET_PORT, W5500_RESET_PIN);

  // Port C input
  GPIO_InitStructure.GPIO_Pin = INT_W5500_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(INT_W5500_PORT, &GPIO_InitStructure);
}

void W5500_Init(void)
{
	wizchip_deselect();
    GPIO_Configuration();
	GPIO_ResetBits(W5500_RESET_PORT, W5500_RESET_PIN);
	Delay(500);
	GPIO_SetBits(W5500_RESET_PORT, W5500_RESET_PIN);
	Delay(72000);

	// Wiznet
	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
}
