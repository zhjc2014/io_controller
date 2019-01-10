#ifndef __SHELL_H__
#define __SHELL_H__

#include "auto_reg_module.h"



typedef struct __shell_cmd shell_cmd_t;



                
#define  SHELL_WRITE    PRF
            
            
uint8_t shell_action(uint8_t* pbuf_start,uint8_t max_len);
            
            
#endif //__SHELL_H__
