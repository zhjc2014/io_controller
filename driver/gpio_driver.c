#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_gpio.h"


#if ENABLE_GPIO_DRV
static int32_t drv_gpio_init        (void)
{
    bsp_gpio_init();
    return 0;
}


int32_t drv_gpio_write        (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    uint32_t val = 0;
    if(len==sizeof(uint32_t))
    {
        memcpy(&val,pSrc,len);
        bsp_gpio_write((uint8_t)offset,val);
    }
    return 0;
}

int32_t drv_gpio_read        (uint8_t* dest,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    uint32_t val = 0;
    if(len==sizeof(uint32_t))
    {
        val = bsp_gpio_read((uint8_t)offset);
        memcpy(dest,&val,len);
    }
    return 0;
}  

REG_DEV(    
            DRV_GPIO,                        /*driver id         */
            DEV_AUTO_INIT,                   /*driver auto init  */
            drv_gpio_init,                   /*driver init func  */
            drv_gpio_write,                  /*driver write func */
            drv_gpio_read,                   /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  
#endif 
