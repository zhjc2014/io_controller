#ifndef __KFIFO2_H__
#define __KFIFO2_H__


#include "platform_define.h"

typedef struct
{
    const char*     fifo_name;
    uint32_t        fifo_size;
    uint32_t        data_size;
    uint8_t*        p_buf;
    uint32_t        write_index;
    uint32_t        read_index;
    //volatile uint8_t lock;
}kfifo2_t;

#define FIFO_IS_OK     0
#define FIFO_IS_EMPTY  1
#define FIFO_IS_FULL   2

//int32_t     kfifo2_init      (char* name,kfifo2_t* pfifo,uint8_t* pbuf,uint32_t size);
int32_t     kfifo2_init      (char* name,kfifo2_t* pfifo,uint8_t* pbuf,uint32_t size, uint32_t data_size);
uint32_t    kfifo2_push_in   (kfifo2_t* pfifo,uint8_t* pbuf, uint32_t len);
uint32_t    kfifo2_pull_out  (kfifo2_t* pfifo,uint8_t* pbuf, uint32_t len);
uint32_t    kfifo2_status    (kfifo2_t* pfifo);
void        kfifo2_reset(kfifo2_t* pfifo);
uint32_t    kfifo2_get_w_index(kfifo2_t* pfifo);
uint32_t    kfifo2_get_r_index(kfifo2_t* pfifo);

uint32_t    kfifo2_have_len(kfifo2_t* pfifo);


#define     SYS_ENTRY_CRITICAL  
#define     SYS_EXIT_CRITICAL

#endif  //__KFIFO_H__
