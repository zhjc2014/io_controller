#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "mpu6500.h"
#include "x_frame.h"

#if ENABLE_MPU6500_DRV


static int32_t drv_mpu_init        (void)
{
    return MPU6500Init();
}

static int32_t drv_mpu_write    (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    return 0;
}

static int32_t drv_mpu_read    (uint8_t* p_dest, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    #if  ACCX_READ_EN
    *(int16_t*)p_dest     = mpu_accx_read(offset);
    #endif
    #if  ACCY_READ_EN
    *(int16_t*)(p_dest+2) = mpu_accy_read(offset);
    #endif
    #if  ACCZ_READ_EN
    *(int16_t*)(p_dest+4) = mpu_accz_read(offset);
    #endif
    
//    DBG_I("%s", __FUNCTION__);
//    DBG_I("mpuNum is %d, accx is %d, accy is %d, accz is %d", offset, \
//    *(int16_t*)p_dest, *(int16_t*)(p_dest+2), *(int16_t*)(p_dest+4));
    
//    DBG_I("%s", __FUNCTION__);
//    DBG_I("mpuNum is %d, accx is %d, accy is %d, accz is %d", offset, \
//    ((int16_t*)p_dest)[0], ((int16_t*)p_dest)[1], ((int16_t*)p_dest)[2]);
    
    if (p_real){
        *p_real = 6;
    }
    return 0;
}

REG_DEV(
    DRV_MPU6500,                    /*driver id         */
    DEV_AUTO_INIT,                  /*driver auto init  */
    drv_mpu_init,                   /*driver init func  */
    drv_mpu_write,                  /*driver write func */
    drv_mpu_read,                   /*driver read  func */
    NULL,                           /*driver ctrl  func */
    NULL                            /*driver close func */
);

#endif


