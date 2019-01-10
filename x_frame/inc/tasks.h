#ifndef __TASK_H__
#define __TASK_H__

#include "auto_reg_module.h"
#include "list.h"
//-------------sys_task cfg

typedef void (*pf_exe_func)(void);

typedef struct {
    const char*     func_name;
    uint32_t        group;
    uint32_t        perio;
    pf_exe_func     exe_func;
    uint32_t        last_exe_time;
    uint32_t        task_used_time;
    uint8_t         flag_init;
    uint8_t         flag_sem;
    list_t          list;    
}task_item_t;

typedef struct
{
    uint32_t  task_num;
    list_t    task_head;
}task_ctrl_t;


uint32_t get_sys_ms(void);
void task_init(void);
void task_sch (uint8_t group);
void block_ms(uint32_t ms);

#define MAX_TASKS_GROUP 4   /*very hi    hi   low  idle*/
#define MAX_TASKS_NUM   20



    

#endif //__TASK_H__
