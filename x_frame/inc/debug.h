#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "platform_define.h"
#include "tasks.h"

extern uint32_t flag_enable_cpf ;

#define filename(x) strrchr(x,'\\')?strrchr(x,'\\')+1:x


#define __FILE_NAME__	filename(__FILE__)


#define _PREFIX_I	"[INFO]: "
#define _PREFIX_W	"[WARN]: "
#define _PREFIX_E	"[ERR ]: "

#define _APPEND_FIX "\r\n"

#define _FILE_INFO	"[%-5d][%8s][%-3d]"	
//#define _FILE_INFO_ARG	__LINE__,__FILE_NAME__
//#define _FILE_INFO_ARG	get_cur_ms_cnt(),__FILE_NAME__
#define _FILE_INFO_ARG	SYS_MS_FUNC,__FILE_NAME__,__LINE__

#define ENABLE_PROTOCOL 1
#define DISABLE_PROTOCOL 0

#define ENABLE_SD_LOG 1
#define DISABLE_SD_LOG 0

#define FIX_PARAM           ENABLE_PROTOCOL,ENABLE_SD_LOG

extern uint8_t flag_en_shell_protocol_out ;

extern int SEGGER_RTT_printf(unsigned BufferIndex, const char * sFormat, ...);

void dbg(uint8_t enable_protocol,uint8_t enable_sd_log,char *fmt,...);
void protocol_dbg(uint8_t enable_protocol,uint8_t enable_sd_log,char *fmt,...);

#define NOP_FUNC do{}while(0)

#define PRINTF_FUNC dbg//SEGGER_RTT_printf
    
#define SYS_MS_FUNC  get_sys_ms()
    
	
#define DBG_RAW(fmt,args...) 		PRINTF_FUNC(ENABLE_PROTOCOL,ENABLE_SD_LOG,fmt,##args)
#define DBG_RAW_SER(fmt,args...) 	PRINTF_FUNC(DISABLE_PROTOCOL,DISABLE_SD_LOG,fmt,##args)
#define PRF(fmt,args...) 			PRINTF_FUNC(ENABLE_PROTOCOL,ENABLE_SD_LOG, fmt _APPEND_FIX,##args)

#define RUN_BY_LIMIT_FUNC(ms,func)  {\
        static uint32_t last_exec_ms = 0;\
		uint32_t cur_ms_tick = SYS_MS_FUNC;\
		if(cur_ms_tick>=(last_exec_ms+ms)){\
			func();\
			last_exec_ms = cur_ms_tick;\
			}}

#define RUN_BY_LIMIT_BLOCK(ms,BLOCK)  {\
        static uint32_t last_exec_ms = 0;\
		uint32_t cur_ms_tick = SYS_MS_FUNC;\
		if(cur_ms_tick>=(last_exec_ms+ms)){\
			BLOCK;\
			last_exec_ms = cur_ms_tick;\
			}}	

#define COLOR_RED  "\033[1;31;40m"
#define COLOR_GRE  "\033[1;32;40m"
#define COLOR_BLU  "\033[1;34;40m"
#define COLOR_WHI  "\033[1;37;40m"
#define COLOR_YEL  "\033[1;33;40m"
#define COLOR_END  "\033[0m"        

#ifdef DEF_DBG_I
	#define DBG_I(fmt,args...) 								  	do{ if(flag_enable_cpf==0) {PRINTF_FUNC(FIX_PARAM,	_FILE_INFO 	_PREFIX_I	fmt _APPEND_FIX,	_FILE_INFO_ARG,##args);}\
                                                                    else                   {PRINTF_FUNC(FIX_PARAM,	"%s" _FILE_INFO 	_PREFIX_I	fmt COLOR_END _APPEND_FIX,	COLOR_GRE,_FILE_INFO_ARG,##args);}}while(0)
    #define DBG_I_STRP(fmt,args...) 							do{PRINTF_FUNC(FIX_PARAM,	fmt,	_FILE_INFO_ARG,##args);PRINTF_FUNC(FIX_PARAM,	"\r\n");}while(0) 
    
	#define DBG_I_LIMIT(ms,fmt,args...) RUN_BY_LIMIT_BLOCK(ms,	DBG_I(fmt, ##args))
#else
	#define DBG_I(fmt,args...) 				NOP_FUNC
	#define DBG_I_LIMIT(ms,fmt,args...)		NOP_FUNC
#endif
	
#ifdef DEF_DBG_W
	#define DBG_W(fmt,args...) 									do{ if(flag_enable_cpf==0) {PRINTF_FUNC(FIX_PARAM,	_FILE_INFO 	_PREFIX_W	fmt _APPEND_FIX,	_FILE_INFO_ARG,##args);}\
                                                                    else                   {PRINTF_FUNC(FIX_PARAM,	"%s" _FILE_INFO 	_PREFIX_W	fmt COLOR_END _APPEND_FIX,	COLOR_YEL,_FILE_INFO_ARG,##args);}}while(0)
	#define DBG_W_LIMIT(ms,fmt,args...) RUN_BY_LIMIT_BLOCK(ms,	DBG_W(fmt, ##args))
#else
	#define DBG_W(fmt,args...) 				NOP_FUNC
	#define DBG_W_LIMIT(ms,fmt,args...)		NOP_FUNC
#endif
	
#ifdef DEF_DBG_E
	#define DBG_E(fmt,args...) 									do{ if(flag_enable_cpf==0) {PRINTF_FUNC(FIX_PARAM,	_FILE_INFO 	_PREFIX_E	fmt _APPEND_FIX,	_FILE_INFO_ARG,##args);}\
                                                                    else                   {PRINTF_FUNC(FIX_PARAM,	"%s" _FILE_INFO 	_PREFIX_E	fmt COLOR_END _APPEND_FIX,	COLOR_RED,_FILE_INFO_ARG,##args);}}while(0)
	#define DBG_E_LIMIT(ms,fmt,args...) RUN_BY_LIMIT_BLOCK(ms,	DBG_E(fmt, ##args))
#else
	#define DBG_E(fmt,args...) 				NOP_FUNC
	#define DBG_E_LIMIT(ms,fmt,args...)		NOP_FUNC
#endif

#define _PREFIX_ASSERT	"[ASSERT]: "
//#define ASSERT_LOG(exp)   ((exp) ? (void)0 : 					PRINTF_FUNC(	_FILE_INFO 	_PREFIX_E _PREFIX_ASSERT  #exp _APPEND_FIX,	_FILE_INFO_ARG))
//#define ASSERT_LOG(exp)   ((void)0)

#define ENABLE_ASSERT  1
                                                                                                                                       
#if ENABLE_ASSERT
#define ASSERT_LOG(exp) \
        do { if (!(exp)) {\
            PRINTF_FUNC(FIX_PARAM,	_FILE_INFO 	_PREFIX_E _PREFIX_ASSERT  #exp _APPEND_FIX,_FILE_INFO_ARG	);\
        while(1);}} while (0)
#else
  #define ASSERT_LOG(exp)   ((void)0)          
#endif

void show_buf(uint8_t* pbuf,uint8_t len);
void show_buf_limit(uint32_t ms,uint8_t* pbuf,uint8_t len);


#endif //__DEBUG_H__
