#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_adc.h"
#include "x_frame.h"

#if ENABLE_RANDOM_DRV

typedef union{
    uint32_t random;
    uint8_t random_byte[4];
}random_seed_u;
static random_seed_u random_seed = {0};

static int32_t drv_random_init		(void)
{
    bsp_adc_init();
	return 0;
}

static int32_t drv_random_read(uint8_t* pSrc,uint32_t len	,uint32_t offset,uint32_t* p_real)
{
    uint32_t random_num;
    srand(random_seed.random);
    random_num = rand();
	if (sizeof(random_seed) == len){
        *(uint32_t*)pSrc = random_num;
    }
    return 0;
}

static void get_random_seed(void)
{
    static int i = 0;
    random_seed.random_byte[i++] = (uint8_t)bsp_adc_read(0);
    i = i % sizeof(random_seed.random_byte);
}
REG_TASK(ID_TASK, 20, get_random_seed);
      
REG_DEV(    
            DRV_RANDOM,                     /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_random_init,                /*driver init func  */
            NULL,                           /*driver write func */
            drv_random_read,                /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  
#endif 

