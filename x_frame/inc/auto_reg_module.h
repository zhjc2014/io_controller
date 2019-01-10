#ifndef __AUTO_REG_MODULE_H__
#define __AUTO_REG_MODULE_H__

#include "platform_define.h"




enum
{
    AUTO_REG_TYPE_RESV          = 0x0   ,
    AUTO_REG_TYPE_DRIVER                ,
    AUTO_REG_TYPE_DRIVER_PUSH           ,
    AUTO_REG_TYPE_DBG_DEV               ,
    AUTO_REG_TYPE_SHELL_X               ,
    AUTO_REG_TYPE_TASK                  ,
    AUTO_REG_TYPE_LINK                  ,
    AUTU_REG_TYPE_LINK_ACTION           ,
    AUTO_REG_ROUTE_CFG                  ,
    AUTO_REG_PROTOCOL                   ,
    AUTO_REG_INIT_MOD_BEFORE            ,
    AUTO_REG_INIT_MOD                   ,
    AUTO_REG_INIT_DELAY                 ,
    AUTO_REG_TYPE_SHELL                 ,
    AUTO_REG_TYPE_PARAM_CFG             ,
    AUTO_REG_TYPE_TEST_CASE             ,
    
    
    //-----------------------------------------
    AUTO_REG_TYPE_ERR                   ,
};



typedef struct __auto_reg_ auto_reg_t;

void*   auto_reg_get_func_cont_by_index(uint8_t func_type,int32_t index);

#endif //__AUTO_REG_MODULE_H__
