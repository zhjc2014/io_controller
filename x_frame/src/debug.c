#include "debug.h"
#include "hal_app_interface.h"
#include "x_frame.h"

#define DEBUG_MAX_SEND  256

extern uint32_t     sys_debug_drv_id;
#define _DEBUG_DRV sys_debug_drv_id

extern int SEGGER_RTT_vsprintf(uint8_t* pout_buf,uint32_t size , const char * sFormat, va_list * pParamList);
volatile uint8_t lock = 0;
uint8_t flag_en_shell_protocol_out = 1;
 void dbg(uint8_t enable_protocol,uint8_t enable_sd_log,char *fmt,...)
{
    va_list ap;
	uint16_t len = 0;
    char str[DEBUG_MAX_SEND];
    
    if(lock==1){return;}
    lock =1;
    va_start(ap,fmt);

	#if DEBUG_USE_RTT_VSPRINTF
		len =SEGGER_RTT_vsprintf ((uint8_t*)str,DEBUG_MAX_SEND-1,fmt,&ap);
	#else
		len = vsnprintf(str,DEBUG_MAX_SEND-1,fmt,ap);
	#endif
    
    str[len] = '\0';
    len=len+1;

	hal_app_write(_DEBUG_DRV,(uint8_t*)str,len,0,NULL);
    
    #if SECOND_DBG_PORT
    hal_app_write(SECOND_DBG_PORT,(uint8_t*)str,len,0,NULL);
    #endif


    lock =0;
    va_end(ap);
}



void show_buf(uint8_t* pbuf,uint8_t len)
{
    
	uint8_t i = 0;
	DBG_RAW_SER("\r\n");
	
	for(i=0;i<len;i++)
	{
		DBG_RAW_SER("%02x ",pbuf[i]);
		if(i%16==15){DBG_RAW_SER("\r\n");}
	}
	DBG_RAW_SER("\r\n");
}

void show_buf_limit(uint32_t ms,uint8_t* pbuf,uint8_t len)
{
    static uint32_t last_ms = 0;
	uint8_t i = 0;

	
	DBG_RAW_SER("\r\n");

	if((SYS_MS_FUNC-last_ms)<ms){return ;}

	last_ms = SYS_MS_FUNC;
	
	for(i=0;i<len;i++)
	{
		DBG_RAW_SER("%02x ",pbuf[i]);
		if(i%28==27){DBG_RAW_SER("\r\n");}
	}
	DBG_RAW_SER("\r\n");
}

