#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "x_frame.h"
#include "version.h"
extern void IAP_Set(void);

int main(void)
{
//    #ifdef __IAP__
//        IAP_Set();
//    #endif 
    
    hal_init();    
    
//    #ifdef __IAP__
//        DBG_I("-----------------------this is in the APP program!--------------------------2");
//    #else
//        DBG_I("*****************this is in the LOADER program*****************");
//    #endif 
    
    DBG_I("version is %s", VERSION);
    
    while(1)
    {
        task_sch(0);
        task_sch(1);
        task_sch(2);
        task_sch(3);
    }
}

