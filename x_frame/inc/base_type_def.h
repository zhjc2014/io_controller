#ifndef __BASE_TYPE_DEF_H__
#define __BASE_TYPE_DEF_H__



#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "stdlib.h"
#include "list.h"

struct __auto_reg_
{  
    const char *    name;
    uint8_t            type;
    void*            p_user_reg_data;        
};



//----dev 
typedef int32_t (*pf_dev_init)         (void);
typedef int32_t (*pf_dev_write)         (uint8_t* pSrc     ,uint32_t len  ,uint32_t offset,uint32_t * real_w_bytes);
typedef int32_t (*pf_dev_read)         (uint8_t* pDest    ,uint32_t len  ,uint32_t offset,uint32_t * real_r_bytes);
typedef int32_t (*pf_dev_ctrl)         (uint32_t ctrl_cmd ,uint8_t* pDest,uint32_t len   ,uint32_t * real_c_bytes);
typedef int32_t (*pf_dev_close)         (void);
typedef int32_t (*pf_dev_push)       (uint32_t code,uint8_t* pbuf, uint32_t len);

struct  _dev_push_t
{
    const char*     cb_func_name;
    uint32_t        dev_id;
    pf_dev_push     cb_func;
};

struct _dev_obj_
{
    const char*     dev_name;
    uint32_t        dev_id;             /*约束在0-63*/

    uint32_t        is_auto_init;
    
    pf_dev_init        dev_init;
    pf_dev_write    dev_write;
    pf_dev_read        dev_read;
    pf_dev_ctrl        dev_ctrl;
    pf_dev_close    dev_close;
    
    struct _dev_push_t*     p_dev_push_obj;
    uint32_t        dev_status;
    uint32_t        dev_flag;           /* 是否已经注册，是否已经初始化*/
    
};

#define MAX_DRV_NUM     64
#define HAL_NAME        "EF HAL platform"
#define HAL_VER         "V1.0"
#define HAL_PLATFORM    "stm32 serial "


#define DEV_FLAG_REG    (0x1<<0)
#define DEV_FLAG_INIT   (0x1<<1)

#define DEV_READY       (DEV_FLAG_REG|DEV_FLAG_INIT)

#define DEV_AUTO_INIT   (0x01)

struct _hal_obj_t
{
    const char*     hal_name;
    const char*     hal_version;
    const char*     hal_platform_info;
    struct _dev_obj_*      p_dev_tab[MAX_DRV_NUM+1];
};


//===========shell
typedef int32_t (*pf_cmd)(uint8_t argc, uint8_t *argv[]);
struct __shell_cmd
{  
    const char *    name;
    uint8_t            max_args;
    pf_cmd            cmd_func;
    const char        *usage;        /* Usage message    (short)    */
    const char        *help;        /* Help  message    (long)    */
        
};


#endif //__BASE_TYPE_DEF_H__
