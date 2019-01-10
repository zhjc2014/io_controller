#ifndef __CSM_H__
#define __CSM_H__


#include "platform_define.h"


#define FLAG_INIT_KEY  0x12345678

typedef struct __csm_t_             csm_t;                            
typedef int32_t (*pf_step_func)    (csm_t* p_cur_csm);       
                                                          
typedef struct                                            
{                                                         
    uint32_t         step_num;                                     
    pf_step_func    step_func;                                  
}csm_item_t;                                              
                                                          
struct __csm_t_                                           
{                
    const char*     name;
    const csm_item_t*        p_csm_tab;                                  
    uint32_t         csm_item_num;                                 
    uint32_t        last_run_index;                                
    uint32_t        cur_run_index;   
    uint32_t        start_index;
    uint32_t         flag_init;
    uint32_t        need_tragle;  //状态刚切换的第一次会置为1 ,第二次之后就为0
    uint32_t        is_first_in;  //状态刚切换的第一次会置为1 ,第二次之后就为0
};                

#define CSM_OK               0
#define ERR_CSM_ITEM_NULL    1
#define ERR_CSM_CHANGE_STAT  2


int32_t csm_run(csm_t* p_csm)      ;
int32_t change_csm_stat(csm_t* p_csm,uint32_t next_state)  ;

#endif //__CSM_H__

