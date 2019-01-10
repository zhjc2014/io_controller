#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_adc.h"


#if ENABLE_ADC_DRV
static int32_t drv_key_init		(void)
{
    bsp_adc_init();
	return 0;
}

static int32_t drv_adc_read		(uint8_t* pSrc,uint32_t len	,uint32_t offset,uint32_t* p_real)
{
	uint32_t rd_len = 0;
	rd_len = bsp_adc_read(pSrc,len,offset);
    if(p_real!=NULL) {*p_real = rd_len; }
    return 0;
}
      
REG_DEV(    
            DRV_ADC,                        /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_key_init,                   /*driver init func  */
            NULL,                           /*driver write func */
            drv_adc_read,                   /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  
#endif 

