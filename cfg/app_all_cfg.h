#ifndef __APP_ALL_CFG_H__
#define __APP_ALL_CFG_H__

#ifdef X_BOT_MAIN_APP
	#include "./x_bot_main_app_cfg/app_cfg.h"
	#include "./x_bot_main_app_cfg/app_def_param.h"
	#include "./x_bot_main_app_cfg/app_driver_cfg.h"
    #include "./x_bot_main_app_cfg/protocol_cmd_cfg.h" 
    #include "./x_bot_main_app_cfg/public_define.h"
#elif defined X_BOT_BLOOD_APP
    #include "./x_bot_blood_app_cfg/app_cfg.h"
	#include "./x_bot_blood_app_cfg/app_def_param.h"
	#include "./x_bot_blood_app_cfg/app_driver_cfg.h"
    #include "./x_bot_blood_app_cfg/protocol_cmd_cfg.h" 
#endif  
#endif //__APP_ALL_CFG_H__
