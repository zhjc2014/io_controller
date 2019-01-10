//#define DEF_DBG_I
//#define DEF_DBG_W
//#define DEF_DBG_E
//#include "debug.h"

//#include "bsp_usb_interface.h"
//       
////#include "usb_usr.h"
////#include "usbd_custom_hid_core.h"
////#include "usb_cdc_core.h"
////#include "usbd_hid_cdc_wrapper.h"


////USB_CORE_HANDLE  USB_Device_dev ;

//#include "hw_config.h"

//void bsp_usb_init(void)
//{
//    static uint8_t flag_init = 0;
//    if(flag_init==0)
//    {
//        flag_init = 1;
//        USB_Config();
//    }
//}

////#define HID_RX_SIZE  256
////#define HID_TX_SIZE  256
////#define VCP_RX_SIZE  256
////#define VCP_TX_SIZE  256

////uint32_t hid_rx_buf[HID_RX_SIZE/4]={0};
////uint32_t hid_tx_buf[HID_TX_SIZE/4]={0};
////uint32_t vcp_rx_buf[VCP_RX_SIZE/4]={0};
////uint32_t vcp_tx_buf[VCP_TX_SIZE/4]={0};

////kfifo_t hid_rx_fifo = {"",HID_RX_SIZE,(uint8_t*)hid_rx_buf,0,0};
////kfifo_t hid_tx_fifo = {"",HID_TX_SIZE,(uint8_t*)hid_tx_buf,0,0};
////kfifo_t vcp_rx_fifo = {"",VCP_RX_SIZE,(uint8_t*)vcp_rx_buf,0,0};
////kfifo_t vcp_tx_fifo = {"",VCP_TX_SIZE,(uint8_t*)vcp_tx_buf,0,0};


////int32_t bsp_hid_send(uint8_t* pbuf,int32_t len)
////{
////    uint32_t real_len = 0;
////    real_len =  kfifo_push_in(&hid_tx_fifo,pbuf,len);
////	return (int32_t)(real_len);
////}

////int32_t bsp_hid_read(uint8_t* p_dest,int32_t len)
////{
////	uint32_t real_len = 0;
////    real_len =  kfifo_pull_out(&hid_rx_fifo,p_dest,len);
////	return (int32_t)(real_len);
////}

////int32_t bsp_vcp_send(uint8_t* pbuf,int32_t len)
////{
////    uint32_t real_len = 0;
////    real_len =  kfifo_push_in(&vcp_tx_fifo,pbuf,len);
////	return (int32_t)(real_len);
////}

////int32_t bsp_vcp_read(uint8_t* p_dest,int32_t len)
////{
////	uint32_t real_len = 0;
////    real_len =  kfifo_pull_out(&vcp_rx_fifo,p_dest,len);
////	return (int32_t)(real_len);
////}

////void HID_SEND_TASK(void)
////{
////   uint8_t send_hid_buf[64]={0};  
////   uint32_t real_len = 0;
////   real_len = kfifo_pull_out(&hid_tx_fifo,&send_hid_buf[1],63);
////   send_hid_buf[0] = real_len&0xff;
////   if(real_len>0)
////   {
////     USBD_HID_SendReport(&USB_Device_dev,send_hid_buf,64);
////   }
////   
////}
////REG_TASK(ID_TASK,1,HID_SEND_TASK);

////void USB_IRQHandler(void)
////{
////  USB_Istr();
////}


