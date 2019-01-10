#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_driver_usb.h"
#include "stm32f10x.h"                  // Device header
#include "kfifo.h"
#include "usb_init.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"

#define USB_KFIFO_SIZE 1024//1024
static uint8_t kfifo_buf[USB_KFIFO_SIZE]={0};
static kfifo_t sg_usb_tx_fifo={"usb_tx_fifo",USB_KFIFO_SIZE,kfifo_buf};

static uint8_t kfifo_rx_buf[USB_KFIFO_SIZE]={0};
static kfifo_t sg_usb_rx_fifo={"usb_rx_fifo",USB_KFIFO_SIZE,kfifo_rx_buf};


void USB_Interrupts_Config(void)
{

#ifdef STM32F10X_CL
  /* Enable the USB Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USB Wake-up interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_Init(&NVIC_InitStructure);  
#else
	
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

#endif /* STM32F10X_CL */

}


void Set_USBClock(void)
{
#ifdef STM32F10X_CL
  /* Select USBCLK source */
  RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLLVCO_Div3);

  /* Enable the USB clock */ 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, ENABLE) ;
#else
  /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
#endif /* STM32F10X_CL */
}

void usb_hid_hw_init(void)
{
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
}

void bsp_usb_init(void)
{
    usb_hid_hw_init();
}

int32_t bsp_usb_write	(uint8_t* pSrc,uint32_t len)
{
    int32_t real_len = 0;
	real_len = kfifo_push_in(&sg_usb_tx_fifo,pSrc,len);
	return real_len;
}

int32_t bsp_usb_read	(uint8_t* pDest,uint32_t len)
{
    int32_t real_len = 0;
	real_len = kfifo_pull_out(&sg_usb_rx_fifo,pDest,len);
	return real_len;
}


int32_t usb_push_data_to_rx_fifo(uint8_t* pbuf,uint16_t len)
{
	int32_t real_len = 0;
	real_len = kfifo_push_in(&sg_usb_rx_fifo,pbuf,len);
	return real_len;
}

int32_t usb_pull_data_from_tx_fifo(uint8_t* pbuf,uint16_t len)
{
	int32_t real_len = 0;
	real_len = kfifo_pull_out(&sg_usb_tx_fifo,pbuf,len);
	return real_len;
}
extern void usb_send_data(uint8_t *pbuf,uint8_t len);

void usb_run_ontick(void)
{
    uint8_t usb_send_buf[64]={0};
	int32_t real_len = 0;
	real_len = usb_pull_data_from_tx_fifo(&usb_send_buf[1],64-1);
	usb_send_buf[0] = real_len;
	if(real_len>0)
	{
		//show_buf(usb_send_buf,64);
		usb_send_data(usb_send_buf,64);
		//DBG_I("usb hid send:%d",real_len);
	}
}



void USB_Cable_Config (uint8_t NewState)
{
#ifdef STM32F10X_CL  
  if (NewState != DISABLE)
  {
    USB_DevConnect();
  }
  else
  {
    USB_DevDisconnect();
  }
#else /* USE_STM3210B_EVAL or USE_STM3210E_EVAL */
  if (NewState != DISABLE)
  {
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
  else
  {
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
#endif /* USE_STM3210C_EVAL */
}

void Enter_LowPowerMode(void)
{

}
void Leave_LowPowerMode(void)
{

}

static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}

void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);

  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &CustomHID_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &CustomHID_StringSerial[18], 4);
  }
}

