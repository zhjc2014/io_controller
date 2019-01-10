#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"
#include "hw_config.h"

#if ENABLE_USB_VCP_DRV
static int32_t drv_vcp_init        (void)
{
    USB_Config();
    return 0;
}

int32_t drv_vcp_write        (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    USB_TxWrite(pSrc, len);
    return 0;
}
int32_t drv_vcp_read        (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    len = USB_RxRead(pSrc, len);
    if (p_real){
        *p_real = len;
    }
    return 0;
}
        
REG_DEV(    
            DRV_USB_VCP,                    /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_vcp_init ,                  /*driver init func  */
            drv_vcp_write,                  /*driver write func */
            drv_vcp_read,                   /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  

#endif 


#if ENABLE_USB_HID_DRV
static int32_t drv_hid_init        (void)
{
    bsp_usb_init();
    return 0;
}

int32_t drv_hid_write        (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    uint32_t write_len = 0;
    write_len = bsp_hid_send(pSrc,len);
    if(p_real!=NULL) {*p_real = write_len; }
    return 0;
}
int32_t drv_hid_read        (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    uint32_t read_len = 0;
    read_len = bsp_hid_read(pSrc,len);
    if(p_real!=NULL) {*p_real = read_len; }
    return 0;
}

REG_DEV(    
            DRV_USB_HID,                    /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_hid_init ,                  /*driver init func  */
            drv_hid_write,                  /*driver write func */
            drv_hid_read,                   /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  
#endif 
