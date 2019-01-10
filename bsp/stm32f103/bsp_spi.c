#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "stm32f10x.h"
#include "bsp_spi.h"

/** SPI init */
#define SPI_WAIT_TIMEOUT			((uint16_t)0xFFFF)

/**
  * @brief :SPI init
  * @param :none
  * @note  :none
  * @retval:none
  */ 
void drv_spi_init( void )
{
	GPIO_InitTypeDef	SpiGpioInitStructer;
	SPI_InitTypeDef		SpiInitStructer;
	
	/** SPI pin config */
	RCC_APB2PeriphClockCmd( SPI_CLK_GPIO_CLK | SPI_MISO_GPIO_CLK | SPI_MOSI_GPIO_CLK | SPI_NSS_GPIO_CLK, ENABLE );	//??????
	
	//SCK MOSI MISO pin config
	SpiGpioInitStructer.GPIO_Speed = GPIO_Speed_10MHz;
	SpiGpioInitStructer.GPIO_Mode = GPIO_Mode_AF_PP;
	
	SpiGpioInitStructer.GPIO_Pin = SPI_CLK_GPIO_PIN;
	GPIO_Init( SPI_CLK_GPIO_PORT, &SpiGpioInitStructer );		//SCK init
	
	SpiGpioInitStructer.GPIO_Pin = SPI_MOSI_GPIO_PIN;
	GPIO_Init( SPI_MOSI_GPIO_PORT, &SpiGpioInitStructer );		//MOSI init
	
	SpiGpioInitStructer.GPIO_Pin = SPI_MISO_GPIO_PIN;
	GPIO_Init( SPI_MISO_GPIO_PORT, &SpiGpioInitStructer );		//MISO init
	
	//NSS pull push output
	SpiGpioInitStructer.GPIO_Mode = GPIO_Mode_Out_PP;
	SpiGpioInitStructer.GPIO_Pin = SPI_NSS_GPIO_PIN;
	GPIO_Init( SPI_NSS_GPIO_PORT, &SpiGpioInitStructer );		//NSS init
	GPIO_SetBits( SPI_NSS_GPIO_PORT, SPI_NSS_GPIO_PIN );		//set

	/** SPI config */
	SPI_I2S_DeInit( SPI_PORT );			//SPI reset
	
	if( SPI1 == SPI_PORT )				
	{
		RCC_APB2PeriphClockCmd( SPI_PORT_CLK, ENABLE );			//SPI1
	}
	else
	{
		RCC_APB1PeriphClockCmd( SPI_PORT_CLK, ENABLE );			//SPI2
	}
	
	SPI_Cmd( SPI_PORT, DISABLE );		//close the spi peripheral
	
	SpiInitStructer.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//full duplex
	SpiInitStructer.SPI_Mode = SPI_Mode_Master;							//master mode
	SpiInitStructer.SPI_CPOL = SPI_CPOL_Low;							//low in idle mode
	SpiInitStructer.SPI_CPHA = SPI_CPHA_1Edge;							//get data at first edge 
	SpiInitStructer.SPI_DataSize = SPI_DataSize_8b;						//8bit data
	SpiInitStructer.SPI_NSS = SPI_NSS_Soft;								//soft nss control
	SpiInitStructer.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;	//32 division
	SpiInitStructer.SPI_FirstBit = SPI_FirstBit_MSB;					//high bit first
	SpiInitStructer.SPI_CRCPolynomial = 7;								//default crc 
	
	SPI_Init( SPI_PORT, &SpiInitStructer );
	SPI_Cmd( SPI_PORT, ENABLE );
}

/**
  * @brief :SPI 
  * @param :
  *			@TxByte: tx/rx 1 byte
  * @note  :none-blocking
  * @retval:received byte
  */
static uint8_t drv_spi_read_write_byte( uint8_t TxByte )
{
	uint8_t l_Data = 0;
	uint16_t l_WaitTime = 0;
	
	while( RESET == SPI_I2S_GetFlagStatus( SPI_PORT, SPI_I2S_FLAG_TXE ) )		//waiting for the send buff to be cleared
	{
		if( SPI_WAIT_TIMEOUT == ++l_WaitTime )
		{
			break;			//timeout 
		}
	}
	l_WaitTime = SPI_WAIT_TIMEOUT / 2;		//reset the timeout value
	SPI_PORT->DR = TxByte;	//send data
	
	while( RESET == SPI_I2S_GetFlagStatus( SPI_PORT, SPI_I2S_FLAG_RXNE ) )		//waiting for the receive buff is not emputy
	{
		if( SPI_WAIT_TIMEOUT == ++l_WaitTime )
		{
			break;			//timeout
		}
	}
	
	l_Data = (uint8_t)SPI_PORT->DR;		//read the received data
	
	return l_Data;		//return data
}

/**
  * @brief :SPI write/read buffer 
  * @param :
  *			@ReadBuffer: received buffer addr
  *			@WriteBuffer: send buffer addr
  *			@Length:byte length
  * @note  :none blocking
  * @retval:none
  */
void drv_spi_read_write_string( uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length )
{
//	GPIO_ResetBits( SPI_NSS_GPIO_PORT, SPI_NSS_GPIO_PIN);			//sel low
    spi_set_nss_low();
	while( Length-- )
	{
		*ReadBuffer = drv_spi_read_write_byte( *WriteBuffer );		//rx/tx data
		ReadBuffer++;
		WriteBuffer++;		
	}
//	GPIO_SetBits( SPI_NSS_GPIO_PORT, SPI_NSS_GPIO_PIN);				//sel high
    spi_set_nss_high();
}
