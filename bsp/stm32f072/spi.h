#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f0xx.h"
#include "x_frame.h"


void SPI1_init(void);
void SPI2_init(void);
uint8_t SPI_WriteReadByte(uint8_t spi_num,uint8_t dat);
void SPI_Set_speed(uint8_t spi_num, uint8_t newSpeed);


#endif
