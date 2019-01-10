#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "spi.h"

#define SPI1_CLK   GPIO_Pin_5
#define SPI1_MISO  GPIO_Pin_6
#define SPI1_MOSI  GPIO_Pin_7

#define SPI1_CLK_SOURCE  GPIO_PinSource5
#define SPI1_MISO_SOURCE GPIO_PinSource6
#define SPI1_MOSI_SOURCE GPIO_PinSource7

#define SPI2_CLK   GPIO_Pin_13
#define SPI2_MISO  GPIO_Pin_14
#define SPI2_MOSI  GPIO_Pin_15

#define SPI2_CLK_SOURCE  GPIO_PinSource13
#define SPI2_MISO_SOURCE GPIO_PinSource14
#define SPI2_MOSI_SOURCE GPIO_PinSource15


void SPI1_init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);

    GPIO_PinAFConfig(GPIOA, SPI1_CLK_SOURCE , GPIO_AF_0);
    GPIO_PinAFConfig(GPIOA, SPI1_MISO_SOURCE, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOA, SPI1_MOSI_SOURCE, GPIO_AF_0);


    GPIO_InitStructure.GPIO_Pin =  SPI1_CLK | SPI1_MISO | SPI1_MOSI;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //----------spi
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge; // SPI_CPHA_2Edge 好像不行
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // 最高不能超过8M 的SPI  clk 
    //SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;
    SPI_Init(SPI1,&SPI_InitStructure);
    
    SPI_RxFIFOThresholdConfig(SPI1,SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPI1,ENABLE);
}

void SPI2_init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

    GPIO_PinAFConfig(GPIOB, SPI2_CLK_SOURCE , GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, SPI2_MISO_SOURCE, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, SPI2_MOSI_SOURCE, GPIO_AF_0);


    GPIO_InitStructure.GPIO_Pin =  SPI2_CLK | SPI2_MISO | SPI2_MOSI;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //----------spi
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge; // SPI_CPHA_2Edge 好像不行
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // 最高不能超过8M 的SPI  clk 
    //SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;
    SPI_Init(SPI2,&SPI_InitStructure);
    
    SPI_RxFIFOThresholdConfig(SPI2,SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPI2,ENABLE);
}


//uint8_t SPI_WriteReadByte(uint8_t dat)
//{
//    while( (SPI1->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET );
//    SPI_SendData8(SPI1,dat);
//    
//    while( (SPI1->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET );
//    return SPI_ReceiveData8(SPI1);
//}

uint8_t SPI_WriteReadByte(uint8_t spi_num,uint8_t dat)
{
  SPI_TypeDef* SPIx = SPI1;
  if(spi_num==2) {SPIx =  SPI2;}
    
  while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
  SPI_SendData8(SPIx, dat);

  while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_ReceiveData8(SPIx);
}


void SPI_Set_speed(uint8_t spi_num, uint8_t newSpeed)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    SPI_TypeDef* SPIx = SPI1;
    if(spi_num==2) {SPIx =  SPI2;}


    //----------spi
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge; // SPI_CPHA_2Edge 好像不行
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = newSpeed; // 最高不能超过8M 的SPI  clk 
    //SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;
    SPI_Init(SPIx,&SPI_InitStructure);
    
    SPI_RxFIFOThresholdConfig(SPIx,SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPIx,ENABLE);
} 

