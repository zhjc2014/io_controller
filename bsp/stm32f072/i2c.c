#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "i2c.h"


uint32_t I2C_Timeout;

#define IIC_TIMING  0x60821A37

#define IIC_MODE  I2C_Mode_SMBusHost

#define I2C1_SCL_PIN          GPIO_Pin_6 
#define I2C1_SDA_PIN          GPIO_Pin_7 
#define I2C1_SCL_PIN_SOURCE   GPIO_PinSource6
#define I2C1_SDA_PIN_SOURCE   GPIO_PinSource7
#define I2C1_PORT             GPIOB


static void I2C1_GPIO_Config(void)
{
      
    GPIO_InitTypeDef GPIO_InitStructure;
 
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);  

    GPIO_PinAFConfig(I2C1_PORT, I2C1_SCL_PIN_SOURCE, GPIO_AF_1);
    GPIO_PinAFConfig(I2C1_PORT, I2C1_SDA_PIN_SOURCE, GPIO_AF_1);

    /* Configure I2C1 pins: SCL and SDA ----------------------------------------*/
    GPIO_InitStructure.GPIO_Pin =  I2C1_SCL_PIN | I2C1_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(I2C1_PORT, &GPIO_InitStructure);
}


void I2c1Init(void)
{
  I2C_InitTypeDef I2C_InitStructure;
  
  RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  I2C1_GPIO_Config();
  
  I2C_InitStructure.I2C_Mode = IIC_MODE;//I2C_Mode_I2C;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x01;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_Timing = IIC_TIMING;//0x30E32E44;
  I2C_Init(I2C1, &I2C_InitStructure);
  
  I2C_Cmd(I2C1, ENABLE);
}

I2C_Status I2C1_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer)
{
  uint8_t read_Num;
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_TransferHandling(I2C1, driver_Addr, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_SendData(I2C1, start_Addr);
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TC) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_TransferHandling(I2C1, driver_Addr, number_Bytes,  I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  
  for(read_Num = 0; read_Num < number_Bytes; read_Num++)
  {
    I2C_Timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)
    {
      if((I2C_Timeout--) == 0)
      {
        return I2C_FAIL;
      }
    }
    
    read_Buffer[read_Num] = I2C_ReceiveData(I2C1);
  }
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }

  return I2C_OK;
}

I2C_Status I2C1_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer)
{
  uint8_t write_Num;
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_TransferHandling(I2C1, driver_Addr, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_SendData(I2C1, start_Addr);
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TCR) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }

  I2C_TransferHandling(I2C1, driver_Addr, number_Bytes, I2C_AutoEnd_Mode, I2C_No_StartStop);
  
  for(write_Num = 0; write_Num < number_Bytes; write_Num++)
  {
    I2C_Timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET)
    {
      if((I2C_Timeout--) == 0)
      {
        return I2C_FAIL;
      }
    }
    
    I2C_SendData(I2C1, write_Buffer[write_Num]);
  }

  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  
  return I2C_OK;
}

void I2c2Init(void)
{
  I2C_InitTypeDef I2C_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  
  I2C_InitStructure.I2C_Mode = IIC_MODE;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x01;
  I2C_InitStructure.I2C_OwnAddress1 = 0x01;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_Timing =  IIC_TIMING;//0x30E32E44;
  I2C_Init(I2C2, &I2C_InitStructure);
  
  I2C_Cmd(I2C2, ENABLE);
}

I2C_Status I2C2_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer)
{
  uint8_t read_Num;
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_TransferHandling(I2C2, driver_Addr, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_SendData(I2C2, start_Addr);
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TC) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_TransferHandling(I2C2, driver_Addr, number_Bytes,  I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  
  for(read_Num = 0; read_Num < number_Bytes; read_Num++)
  {
    I2C_Timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
    {
      if((I2C_Timeout--) == 0)
      {
        return I2C_FAIL;
      }
    }
    
    read_Buffer[read_Num] = I2C_ReceiveData(I2C2);
  }
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }

  return I2C_OK;
}

I2C_Status I2C2_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer)
{
  uint8_t write_Num;
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_TransferHandling(I2C2, driver_Addr, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  I2C_SendData(I2C2, start_Addr);
  
  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TCR) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }

  I2C_TransferHandling(I2C2, driver_Addr, number_Bytes, I2C_AutoEnd_Mode, I2C_No_StartStop);
  
  for(write_Num = 0; write_Num < number_Bytes; write_Num++)
  {
    I2C_Timeout = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET)
    {
      if((I2C_Timeout--) == 0)
      {
        return I2C_FAIL;
      }
    }
    
    I2C_SendData(I2C2, write_Buffer[write_Num]);
  }

  I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF) == RESET)
  {
    if((I2C_Timeout--) == 0)
    {
      return I2C_FAIL;
    }
  }
  
  
  return I2C_OK;
}



void iic_init(void)
{
    I2c1Init();
}

REG_INIT_MOD(iic_init);


int32_t iic_r(uint8_t argc, uint8_t *argv[])
{
    if(argc>=3)
    {
        uint32_t addr;
        uint32_t cmd = 0;
        uint32_t read_len = 0;
        uint8_t buf_rd[100]={0};
        
        sscanf((const char*)argv[1],"%x",&addr);
        sscanf((const char*)argv[2],"%x",&cmd);
        sscanf((const char*)argv[3],"%d",&read_len);
        
        I2C1_Read_NBytes((uint8_t)addr,(uint8_t)cmd,(uint8_t)read_len,buf_rd);
        
        
        DBG_I("smbus_read :addr:0x%x  cmd:0x%x  len:%d",addr,cmd,read_len);
        show_buf(buf_rd,read_len);
        
    }
    else
    {
        DBG_W("warn the smbus_r param is error: eg:iic_r cmd len");
    }
	return RET_OK;
}

REG_SHELL_CMD(iic_r,0,iic_r,"smbus_r", "eg:iic_r cmd  len\r\n ");





