#ifndef _I2C_H_
#define _I2C_H_

#include "stm32f0xx.h"
#include "x_frame.h"
typedef enum
{
  I2C_OK                                          = 0,
  I2C_FAIL                                        = 1
}I2C_Status;

/* Exported constants --------------------------------------------------------*/
#define I2C_TIMING                                0x00210507
#define I2C_TIMEOUT                               0x1000

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void I2c1Init(void);
I2C_Status I2C1_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer); 
I2C_Status I2C1_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer); 

void I2c2Init(void);
I2C_Status I2C2_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer); 
I2C_Status I2C2_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer); 
#endif
