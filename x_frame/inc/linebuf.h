#ifndef __LINE_BUF_H__
#define __LINE_BUF_H__


#include "platform_define.h"


typedef struct
{
	const char*	name;
	uint8_t*	pbuf;
	uint16_t 	size;
	uint16_t	r_index;
	uint16_t	w_index;
	uint8_t 	state;
	
}linebuf_t;

/*不能在写入的时候同时读取会导致内存错乱*/

#define LB_STATE_IS_W	(0x1<<0)
#define LB_STATE_IS_R	(0x1<<1)
#define LB_STATE_IS_M	(0x1<<2) //move buf


int32_t lb_init(const char* 	name,
				  linebuf_t*	p_lb,
				  uint8_t*		pbuf,
				  uint16_t		size);
int32_t 	lb_get_rest_len	(linebuf_t*	p_lb);
int32_t 	lb_get_data_len	(linebuf_t*	p_lb);
uint8_t 	lb_get_statues	(linebuf_t*	p_lb);
int32_t 	lb_push			(linebuf_t*	p_lb,uint8_t* p_src,uint16_t len);
int32_t 	lb_pull			(linebuf_t*	p_lb,uint8_t* p_dest,uint16_t len);
int32_t 	lb_move			(linebuf_t*	p_lb,uint16_t len);
uint8_t* 	lb_get_r_ptr	(linebuf_t*	p_lb);
void lb_reset(linebuf_t*	p_lb);


#endif  //__LINE_BUF_H__
