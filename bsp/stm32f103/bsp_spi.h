#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

//SPI pin definition
#define SPI_NSS_GPIO_PORT            GPIOA
#define SPI_NSS_GPIO_CLK            RCC_APB2Periph_GPIOA
#define SPI_NSS_GPIO_PIN            GPIO_Pin_4

#define SPI_CLK_GPIO_PORT            GPIOA
#define SPI_CLK_GPIO_CLK            RCC_APB2Periph_GPIOA
#define SPI_CLK_GPIO_PIN            GPIO_Pin_5

#define SPI_MISO_GPIO_PORT            GPIOA
#define SPI_MISO_GPIO_CLK            RCC_APB2Periph_GPIOA
#define SPI_MISO_GPIO_PIN            GPIO_Pin_6

#define SPI_MOSI_GPIO_PORT            GPIOA
#define SPI_MOSI_GPIO_CLK            RCC_APB2Periph_GPIOA
#define SPI_MOSI_GPIO_PIN            GPIO_Pin_7


#define spi_set_nss_high()            SPI_NSS_GPIO_PORT->ODR |= SPI_NSS_GPIO_PIN                                //sel high
#define spi_set_nss_low()            SPI_NSS_GPIO_PORT->ODR &= (uint32_t)( ~((uint32_t)SPI_NSS_GPIO_PIN ))    //sel low



//SPI interface definition
#define SPI_PORT                    SPI1                        //SPI interface
#define SPI_PORT_CLK                RCC_APB2Periph_SPI1            //SPI clock


void drv_spi_init( void );
uint8_t drv_spi_read_write_byte( uint8_t TxByte );
void drv_spi_read_write_string( uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length );

#endif