#define DEF_DBG_I
//#define DEF_DBG_W
//#define DEF_DBG_E
#include "debug.h"

#include "hal_app_interface.h"
#include "hal_core.h"
#include "x_frame.h"

extern hal_obj_t    sg_sys_hal;  

#define P_DEV  sg_sys_hal.p_dev_tab[dev_handle&0xff]

int32_t  hal_app_write(uint32_t dev_handle,uint8_t* pbuf, uint32_t len, uint32_t offset,uint32_t* p_real_bytes)
{
    if(dev_handle>MAX_DRV_NUM)                      {return -1;}
	if( (P_DEV->dev_flag & DEV_READY) != DEV_READY) {return -1;}
    return P_DEV->dev_write(pbuf,len,offset,p_real_bytes);
}


int32_t  hal_app_read(uint32_t dev_handle,uint8_t* pbuf, uint32_t len, uint32_t offset,uint32_t* p_real_bytes)
{
	if(dev_handle>MAX_DRV_NUM)                      {return -1;}
    if( (P_DEV->dev_flag & DEV_READY) != DEV_READY) {return -1;}
    return P_DEV->dev_read(pbuf,len,offset,p_real_bytes);
}

int32_t  hal_app_ctrl(uint32_t dev_handle,uint32_t ctrl_cmd,uint8_t* pbuf,uint32_t len,uint32_t* p_real_bytes)
{
	if(dev_handle>MAX_DRV_NUM)                      {return -1;}
    if( (P_DEV->dev_flag & DEV_READY) != DEV_READY) {return -1;}
	return P_DEV->dev_ctrl(ctrl_cmd,pbuf,len,p_real_bytes);
}


#if TINY_FUNC_CODE == 0

static int32_t w_dev(uint8_t argc, uint8_t *argv[])
{
	if(argc>=3)
    {
        uint32_t dev_id = 0;
        uint32_t w_len = strlen((const char*)argv[2]);
        uint32_t real_write_bytes = 0;
        int32_t  write_state = 0; 
        sscanf((const char*)argv[1],"%d",&dev_id);
        
        write_state = hal_app_write(dev_id,argv[2],w_len,0,&real_write_bytes);
        DBG_I("[write result:%d]  dev_id:%d  - %d bytes real_write:%d bytes (%s)",write_state,dev_id,w_len,real_write_bytes,(const char*)argv[2]);

    }
    else
    {
        DBG_I("input param err! eg:w_dev 1 123 ");
    }
	return RET_OK;
}

REG_SHELL_CMD_X(w_dev,0,w_dev,"w_dev", "w_dev 1 123 \r\n ");
#endif


#undef P_DEV
