
#ifndef __SPIHANDLER_H__
#define __SPIHANDLER_H__

#define W5500_SPI                       SPI1
#define W5500_SPI_CLK                   RCC_APB2Periph_SPI1

#define W5500_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.5 */
#define W5500_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define W5500_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOA

#define W5500_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.6 */
#define W5500_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define W5500_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOA

#define W5500_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.7 */
#define W5500_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define W5500_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOA

#define W5500_CS_PIN                    GPIO_Pin_4                  /* PA.4 */
#define W5500_CS_GPIO_PORT              GPIOA                       /* GPIOA */
#define W5500_CS_GPIO_CLK               RCC_APB2Periph_GPIOA

#define W5500_DUMMY_BYTE				0xFF
#define W5500_RESET_PIN                 GPIO_Pin_8
#define W5500_RESET_PORT                GPIOA
#define INT_W5500_PIN					GPIO_Pin_5	//in
#define INT_W5500_PORT					GPIOC

void W5500_SPI_Init(void);
void W5500_Init(void);

#endif
