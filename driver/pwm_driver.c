#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_pwm.h"


#if ENABLE_PWM_DRV
static int32_t drv_pwm_init        (void)
{
    bsp_pwm_init();
    return 0;
}
int32_t drv_pwm_write        (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    int16_t val = 0;
    if(len == sizeof(val))
    {
        memcpy(&val,pSrc,len);
        bsp_pwm_write((uint8_t)offset,val);
    }
    return 0;
}
        
REG_DEV(    
            DRV_PWM,                        /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_pwm_init,                   /*driver init func  */
            drv_pwm_write,                  /*driver write func */
            NULL,                           /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  
#endif 
