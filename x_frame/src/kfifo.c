#include "kfifo.h"


#define KFIFO_MIN(a,b) ((a)<(b)?(a):(b))
#define KFIFO_MAX(a,b) ((a)>(b)?(a):(b))

static uint32_t calc_real_fifo_len(uint16_t len)
{
    uint16_t i = 0;
    
    for(i=0; len>0; i++)
    {
        len = len >> 1;
    }
    
    return (0x1<<(i-1));
}

int32_t kfifo_init(char* name,kfifo_t* pfifo,uint8_t* pbuf,uint32_t size)
{
    pfifo->fifo_name    = name;
    pfifo->p_buf        = pbuf;
    pfifo->fifo_size    = calc_real_fifo_len(size);    
    pfifo->write_index  = 0;
    pfifo->read_index   = 0;
    //pfifo->lock         = 0;
    return 1;
}

void kfifo_reset(kfifo_t* pfifo)
{
	pfifo->write_index  = 0;
    pfifo->read_index   = 0;
}


/*
      avaiable                   avaiable
    |*******|-----------------|************|
       |     \___             |        |
      PART_B     |            |      PART_A
                 |            |
                 |             \
               read_index       write_index
*/

uint32_t kfifo_push_in(kfifo_t* pfifo,uint8_t* pbuf, uint32_t len)
{   
    unsigned int l;   
  
    len = KFIFO_MIN(len, pfifo->fifo_size - pfifo->write_index + pfifo->read_index);   
    SYS_ENTRY_CRITICAL   
  
    l = KFIFO_MIN(len, pfifo->fifo_size - (pfifo->write_index & (pfifo->fifo_size - 1)));   
    memcpy(pfifo->p_buf + (pfifo->write_index & (pfifo->fifo_size - 1)), pbuf, l);   
    memcpy(pfifo->p_buf, pbuf + l, len - l);   
  

    pfifo->write_index += len;  

    SYS_EXIT_CRITICAL  
    
    return len;   
}  
/*
      avaiable                   avaiable
    |*******|-----------------|************|
       |     \___             |        |
      PART_B     |            |      PART_A
                 |            |
                 |             \
               write_index       read_index
*/ 
uint32_t kfifo_pull_out(kfifo_t* pfifo,uint8_t* pbuf, uint32_t len)
{   
    unsigned int l;   
    
    if(pbuf!=NULL)
    {
  
    len = KFIFO_MIN(len, pfifo->write_index - pfifo->read_index);   
  
    SYS_ENTRY_CRITICAL   
  
    l = KFIFO_MIN(len, pfifo->fifo_size - (pfifo->read_index & (pfifo->fifo_size - 1)));   
    
    
        memcpy(pbuf, pfifo->p_buf + (pfifo->read_index & (pfifo->fifo_size - 1)), l);   
        memcpy(pbuf + l, pfifo->p_buf, len - l);   
    }

  
    pfifo->read_index += len;   
  
    SYS_EXIT_CRITICAL 

    return len;   
}   



static uint32_t kfifo_have_len(kfifo_t* pfifo)
{
    return (pfifo->write_index - pfifo->read_index);
}

uint32_t kfifo_rest_len(kfifo_t* pfifo)
{
	return pfifo->fifo_size - kfifo_have_len(pfifo);
}

uint32_t kfifo_get_w_index(kfifo_t* pfifo)
{
    return (pfifo->write_index & (pfifo->fifo_size - 1));
}

uint32_t kfifo_get_r_index(kfifo_t* pfifo)
{
    return (pfifo->read_index & (pfifo->fifo_size - 1));
}
