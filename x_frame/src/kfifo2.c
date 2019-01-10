#include "kfifo2.h"


#define kfifo2_MIN(a,b) ((a)<(b)?(a):(b))
#define kfifo2_MAX(a,b) ((a)>(b)?(a):(b))

static uint32_t calc_real_fifo_len(uint16_t len)
{
    uint16_t i = 0;
    
    for(i=0; len>0; i++)
    {
        len = len >> 1;
    }
    
    return (0x1<<(i-1));
}

int32_t kfifo2_init(char* name,kfifo2_t* pfifo,uint8_t* pbuf,uint32_t size, uint32_t data_size)
{
    pfifo->fifo_name    = name;
    pfifo->p_buf        = pbuf;
    pfifo->fifo_size    = calc_real_fifo_len(size);    
    pfifo->data_size    = data_size;
    pfifo->write_index  = 0;
    pfifo->read_index   = 0;
    //pfifo->lock         = 0;
    return 1;
}

void kfifo2_reset(kfifo2_t* pfifo)
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

uint32_t kfifo2_push_in(kfifo2_t* pfifo,uint8_t* pbuf, uint32_t len)
{   
    unsigned int length;   
  
    len = kfifo2_MIN(len, pfifo->fifo_size - pfifo->write_index + pfifo->read_index);   
    SYS_ENTRY_CRITICAL   
  
    length = kfifo2_MIN(len, pfifo->fifo_size - (pfifo->write_index & (pfifo->fifo_size - 1)));   
    memcpy(pfifo->p_buf + (pfifo->write_index & (pfifo->fifo_size - 1)) * pfifo->data_size, pbuf, length * pfifo->data_size);   
    memcpy(pfifo->p_buf, pbuf + length * pfifo->data_size, (len - length) * pfifo->data_size);   
  

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
uint32_t kfifo2_pull_out(kfifo2_t* pfifo,uint8_t* pbuf, uint32_t len)
{   
    unsigned int length;   
    
    if(pbuf!=NULL)
    {
  
    len = kfifo2_MIN(len, pfifo->write_index - pfifo->read_index);   
  
    SYS_ENTRY_CRITICAL   
  
    length = kfifo2_MIN(len, pfifo->fifo_size - (pfifo->read_index & (pfifo->fifo_size - 1)));   
    
    
        memcpy(pbuf, pfifo->p_buf + (pfifo->read_index & (pfifo->fifo_size - 1)) * pfifo->data_size, length * pfifo->data_size);   
        memcpy(pbuf + length * pfifo->data_size, pfifo->p_buf, (len - length) *  pfifo->data_size);   
    }

  
    pfifo->read_index += len;   
  
    SYS_EXIT_CRITICAL 

    return len;   
}   



uint32_t kfifo2_have_len(kfifo2_t* pfifo)
{
    return (pfifo->write_index - pfifo->read_index);
}

uint32_t kfifo2_rest_len(kfifo2_t* pfifo)
{
    return pfifo->fifo_size - kfifo2_have_len(pfifo);
}

uint32_t kfifo2_get_w_index(kfifo2_t* pfifo)
{
    return (pfifo->write_index & (pfifo->fifo_size - 1));
}

uint32_t kfifo2_get_r_index(kfifo2_t* pfifo)
{
    return (pfifo->read_index & (pfifo->fifo_size - 1));
}
