#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_ht1621b.h"


#if ENABLE_BLOOD_BAR_DRV
static int32_t drv_blood_bar_init		(void)
{
    ht1621b_init();
    return 0;
}

int32_t drv_blood_bar_write		(uint8_t* pSrc, uint32_t len	, uint32_t offset, uint32_t* p_real)
{
    uint32_t dat = 0;
    if(len == sizeof(uint32_t)) {
        memcpy(&dat, pSrc, sizeof(uint32_t));
        show_led((uint8_t)dat);
    }

    return 0;
}



REG_DEV(
    DRV_BLOOD,                      /*driver id         */
    DEV_AUTO_INIT,                  /*driver auto init  */
    drv_blood_bar_init,                 /*driver init func  */
    drv_blood_bar_write,                /*driver write func */
    NULL,                            /*driver read  func */
    NULL,                           /*driver ctrl  func */
    NULL                            /*driver close func */
);

#endif
