#ifndef __X_FRAME_H__
#define __X_FRAME_H__


#include "app_all_cfg.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "auto_reg_module.h"
#include "hal_app_interface.h"
#include "tasks.h"
#include "shell.h"
#include "csm.h"
#include "hal_core.h"
#include "kfifo.h"
#include "kfifo2.h"
#include "list.h"
#include "protocol_hal.h"

#ifdef ALIGN
#undef ALIGN
#endif
#define ALIGN(n) __attribute__ ((aligned (n))) 
 
#define ARRY_ITEMS_NUM(n) (sizeof(n)/sizeof(n[0]))
#define PACK_8  __attribute__((aligned(8)))
#define PACK_4  __attribute__((aligned(4)))
                                   

#define AUTO_REG_SECTION  __attribute__ ((used,section("AUTO_REG"))) 
#define AUTO_REG_ITEM(name,type,p_user_data)  const auto_reg_t   __auto_reg_##name   AUTO_REG_SECTION = {#name,type,p_user_data}
#define REG_INIT_MOD(func)  AUTO_REG_ITEM(func,AUTO_REG_INIT_MOD,(void*)func)


#define REG_DEV(id,is_auto_init,init,write,read,ctrl,close) \
                static dev_obj_t driver_##id = {#id,id,is_auto_init,init,write,read,ctrl,close,NULL,0,0};\
                AUTO_REG_ITEM(REG_##id      ,AUTO_REG_TYPE_DRIVER           ,(void*)&driver_##id)

#define REG_DEBUG(dev_id)           AUTO_REG_ITEM(debug_id  ,AUTO_REG_TYPE_DBG_DEV          ,(void*)&dev_id)
    
#define REG_DEV_CB(dev_id,func)     \
                                    dev_push_t    dev_cb_##func  = {#func,dev_id,func};\
                                    AUTO_REG_ITEM(dev_cb_##func,AUTO_REG_TYPE_DRIVER_PUSH,(void*)& dev_cb_##func)


#define REG_SHELL_CMD_X(name,maxargs,cmd,usage,detail)  \
            const shell_cmd_t    cmd_##name = {#name,maxargs,cmd,usage,detail};\
            AUTO_REG_ITEM(name,AUTO_REG_TYPE_SHELL_X,(void*)&cmd_##name)
                
#define REG_SHELL_CMD(name,maxargs,cmd,usage,detail)  \
            const shell_cmd_t    cmd_##name = {#name,maxargs,cmd,usage,detail};\
            AUTO_REG_ITEM(name,AUTO_REG_TYPE_SHELL,(void*)&cmd_##name)
                                    
#define REG_TASK(group,perio,func)  \
            task_item_t    task_##func  = {#func,group,perio,func};\
            AUTO_REG_ITEM(func,AUTO_REG_TYPE_TASK,(void*)&task_##func)
                
#define VH_TASK   0              
#define HI_TASK   1      
#define LW_TASK   2    
#define ID_TASK   3        
                                    

#endif //__X_FRAME_H__
