#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "x_frame.h"

//主要是先hal数据类型的抽象和存储


hal_obj_t    sg_sys_hal={0};
static uint32_t     real_reg_devs = 0;
uint32_t            sys_debug_drv_id = ERR_DRV_ID;

const char* build_data  = __DATE__;
const char* build_time  = __TIME__;


extern void*   auto_reg_get_func_cont_by_index(uint8_t func_type,int32_t index);
extern void auto_init_modules(void);


 

static uint32_t hal_reg_driver(dev_obj_t* pdev)
{
    uint32_t i = 0;
    dev_push_t* p_cur_dev_push;
    if(pdev->dev_id > MAX_DRV_NUM)
    {
        DBG_E("[%s]reg faild,the drv_id[%d] is err,max:%d! ",pdev->dev_name,pdev->dev_id,MAX_DRV_NUM);
        return 0;
    }
    //检查是否此驱动已经注册
    if(pdev->dev_flag & DEV_FLAG_REG)
    {
        DBG_E("[%s] is exsis!!",pdev->dev_name);
        return  0;
    }
    
    real_reg_devs++;
    pdev->dev_flag |= DEV_FLAG_REG;

    pdev->p_dev_push_obj = NULL;
    
    /*检查是否需要加载dev push cb的func*/
    for(i=0;i<=MAX_DRV_NUM;i++)
    {
         p_cur_dev_push = (dev_push_t*)auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_DRIVER_PUSH,i);
         if( (p_cur_dev_push!=NULL) && (p_cur_dev_push->dev_id == pdev->dev_id))
         {
            pdev->p_dev_push_obj  = p_cur_dev_push;
            break;  //只注册第一个
         }
    }
    
    
    /*加入驱动管理tab表中*/
    sg_sys_hal.p_dev_tab[pdev->dev_id] = pdev;
    DBG_I("[%s]-id:%d is reg.",pdev->dev_name,pdev->dev_id);
    
    return RET_OK;
}

static void init_all_driver(void)
{
    uint32_t  i;
    uint32_t*  p_debug_id;
    dev_obj_t* cur_dev_obj=NULL;
    /*首先初始化console ,保证打印信息不会丢失*/
    p_debug_id = (uint32_t*)auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_DBG_DEV,0);
    if(p_debug_id!=NULL)
    {
        memcpy(&sys_debug_drv_id,p_debug_id,sizeof(uint32_t));
        
    }
    
    
    
    
    for(i=0;i<=MAX_DRV_NUM;i++)
    {
        cur_dev_obj = (dev_obj_t*)auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_DRIVER,i);
        if((cur_dev_obj!=NULL) && (cur_dev_obj->dev_id == sys_debug_drv_id))
        {
            hal_reg_driver(cur_dev_obj);
            cur_dev_obj->dev_init();
            cur_dev_obj->dev_flag|=DEV_FLAG_INIT;
            break;
        }
    }
    
    /*然后注册所有驱动*/
    for(i=0;i<=MAX_DRV_NUM;i++)
    {
        cur_dev_obj = (dev_obj_t*)auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_DRIVER,i);
        if(cur_dev_obj!=NULL)
        {
            hal_reg_driver(cur_dev_obj);
        } 
    }
    
    /*保证初始化是按照driverid 的从小到大的顺序初始化*/
    for(i=0;i<MAX_DRV_NUM;i++)
    {
       if(sg_sys_hal.p_dev_tab[i]==NULL){continue;} 
       if( (sg_sys_hal.p_dev_tab[i]->dev_flag & DEV_FLAG_INIT) == 0)
       {
           if( sg_sys_hal.p_dev_tab[i]->is_auto_init )
           {
                sg_sys_hal.p_dev_tab[i]->dev_init();
                sg_sys_hal.p_dev_tab[i]->dev_flag|=DEV_FLAG_INIT;
                DBG_I("[%s]-id:%d is init.",sg_sys_hal.p_dev_tab[i]->dev_name,sg_sys_hal.p_dev_tab[i]->dev_id);
           }
       }
    }
}




extern void init_shell_item(void);
int32_t	hal_init(void)
{
    sg_sys_hal.hal_name             = HAL_NAME;
    sg_sys_hal.hal_platform_info    = HAL_PLATFORM;
    sg_sys_hal.hal_version          = HAL_VER;
    
    memset(sg_sys_hal.p_dev_tab,0,sizeof(sg_sys_hal.p_dev_tab));
    
    
    /*注册所有设备*/
    init_all_driver();
    protocol_init();
    task_init();
    init_shell_item();
    auto_init_modules();
    
    return 0;
}

void hal_dev_notify(uint32_t dev_id,uint32_t code,uint8_t* pbuf, uint32_t len)
{
    if(dev_id<MAX_DRV_NUM)
    {
        if(  ((sg_sys_hal.p_dev_tab[dev_id]->dev_flag & DEV_READY) == DEV_READY) && 
             (sg_sys_hal.p_dev_tab[dev_id]->p_dev_push_obj!=NULL) 
          )        
        {
            sg_sys_hal.p_dev_tab[dev_id]->p_dev_push_obj->cb_func(code,pbuf,len);
        }
    }
}

#if TINY_FUNC_CODE == 0

#define BigEndian 1  
#define LittleEndian 0  
uint32_t is_big_endian(void)  
{  
    const uint16_t n = 1;  
    if(*(char *)&n)  
    {  
        return LittleEndian;  
    }  
    return BigEndian;  
} 


static void show_all_devs(void)
{
    uint32_t i=0;
    
    DBG_I("\t name           \t dev-id         [total_dev:%d]",real_reg_devs);
    DBG_I("\t--------------- \t-------");
    for(i=0;i<MAX_DRV_NUM;i++)
    {
        if( (sg_sys_hal.p_dev_tab[i]->dev_flag & DEV_READY) == DEV_READY)
        {
            DBG_I("\t %-20s \t %-02d",sg_sys_hal.p_dev_tab[i]->dev_name,sg_sys_hal.p_dev_tab[i]->dev_id);
        }
    }
}

int32_t list_devs(uint8_t argc, uint8_t *argv[])
{
    DBG_I("*cur system reg  %d devices ",real_reg_devs);
    show_all_devs();
    
    return RET_OK;
}

REG_SHELL_CMD_X(ls_devs,0,list_devs,"list_devices", "list_devices.  eg ls_devs \r\n ");
#endif
    
