#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_key.h"


#if ENABLE_KEY_DRV
static int32_t drv_key_init		(void)
{
    bsp_key_init();
	return 0;
}
      
REG_DEV(    
            DRV_KEY,                        /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_key_init,                   /*driver init func  */
            NULL,                           /*driver write func */
            NULL,                           /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  
#endif 
