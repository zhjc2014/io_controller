#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"

#include "csm.h"

#if DISABLE_CSM_MOD == 0
static csm_item_t* find_cur_run_item(csm_t* p_csm)                                      
{                                                                                
    uint32_t i = 0;                                                                 
    for(i=0;i<p_csm->csm_item_num;i++)                                              
    {                                                                               
        if(p_csm->cur_run_index==p_csm->p_csm_tab[i].step_num)                         
        {                                                                              
            return (csm_item_t*)&(p_csm->p_csm_tab[i]);                                                  
        }                                                                              
    }                                                                               
    return NULL;                                                                    
}                                                                                
                                                                                 
int32_t csm_run(csm_t* p_csm)                                                   
{                                                                                
    csm_item_t* cur_run_item = NULL;   

    if(p_csm->flag_init!=FLAG_INIT_KEY)
    {
        p_csm->flag_init = FLAG_INIT_KEY;
        change_csm_stat(p_csm,p_csm->start_index);
    }
    
    cur_run_item = find_cur_run_item(p_csm);                                        
                                                                                    
    if(cur_run_item==NULL){return -ERR_CSM_ITEM_NULL;}     

    p_csm->is_first_in = 0;
    if(p_csm->need_tragle)
    {
        p_csm->is_first_in = 1;
        p_csm->need_tragle = 0;
    }

                                                                                    
    return cur_run_item->step_func(p_csm);                                          
}                                                                                
                                                                                 
int32_t change_csm_stat(csm_t* p_csm,uint32_t next_state)                       
{                                                                                
    if(next_state<p_csm->csm_item_num)                                              
    {                                                                               
        p_csm->last_run_index = p_csm->cur_run_index;                                  
        p_csm->cur_run_index  = next_state;       
        p_csm->need_tragle =1;
        return next_state;                                                             
    }                                                                               
    else                                                                            
    {                                                                               
        p_csm->last_run_index = 0;                                                     
        p_csm->cur_run_index  = 0;                                                     
        DBG_I("error!!! next:%d max:%d",next_state,p_csm->csm_item_num);   
        return -ERR_CSM_CHANGE_STAT;
    }                                                                                                                                                  
}                                                                                
#endif
