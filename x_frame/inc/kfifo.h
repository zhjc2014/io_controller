#ifndef __KFIFO_H__
#define __KFIFO_H__


#include "platform_define.h"

typedef struct
{
    const char*     fifo_name;
    uint32_t        fifo_size;
    uint8_t*        p_buf;
    uint32_t        write_index;
    uint32_t        read_index;
    //volatile uint8_t lock;
}kfifo_t;

#define FIFO_IS_OK     0
#define FIFO_IS_EMPTY  1
#define FIFO_IS_FULL   2

int32_t     kfifo_init      (char* name,kfifo_t* pfifo,uint8_t* pbuf,uint32_t size);
uint32_t    kfifo_push_in   (kfifo_t* pfifo,uint8_t* pbuf, uint32_t len);
uint32_t    kfifo_pull_out  (kfifo_t* pfifo,uint8_t* pbuf, uint32_t len);
uint32_t    kfifo_status    (kfifo_t* pfifo);
void         kfifo_reset(kfifo_t* pfifo);
uint32_t    kfifo_get_w_index(kfifo_t* pfifo);
uint32_t    kfifo_get_r_index(kfifo_t* pfifo);

uint32_t kfifo_have_len(kfifo_t* pfifo);


#define     SYS_ENTRY_CRITICAL
#define     SYS_EXIT_CRITICAL

#endif  //__KFIFO_H__
