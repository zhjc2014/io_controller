#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_pwm.h"


#if ENABLE_LED_DRV
static int32_t drv_led_init        (void)
{
    bsp_pwm_init();
    return 0;
}
int32_t drv_led_write        (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    uint32_t val = 0;
    if(len==sizeof(uint32_t))
    {
        memcpy(&val,pSrc,len);
        bsp_pwm_write((uint8_t)offset,val);
    }
    return 0;
}
        
REG_DEV(    
            DRV_LED,                        /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_led_init,                   /*driver init func  */
            drv_led_write,                  /*driver write func */
            NULL,                           /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  
#endif 
