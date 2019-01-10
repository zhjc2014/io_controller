#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "linebuf.h"


int32_t lb_init(const char*     name,
                          linebuf_t*    p_lb,
                          uint8_t*        pbuf,
                          uint16_t        size)
{
    
    p_lb->name      = name;
    p_lb->pbuf      = pbuf;
    p_lb->size      = size;
    p_lb->r_index      = 0;
    p_lb->w_index      = 0;
    p_lb->state      = 0;
    return 0;
}

void lb_reset(linebuf_t*    p_lb)
{
    p_lb->r_index = p_lb->w_index = 0;
}


int32_t lb_get_rest_len(linebuf_t*    p_lb)
{
    return (p_lb->size - p_lb->w_index);
}

int32_t lb_get_data_len(linebuf_t*    p_lb)
{
    return (p_lb->w_index - p_lb->r_index);
}

uint8_t lb_get_statues(linebuf_t*    p_lb)
{
    return p_lb->state;
}

uint8_t* lb_get_r_ptr(linebuf_t*    p_lb)
{
    return &p_lb->pbuf[p_lb->r_index];
}



int32_t lb_push(linebuf_t*    p_lb,uint8_t* p_src,uint16_t len)
{
    int16_t real_len = 0;
    uint16_t rest_len = 0;
    
    //DBG_I("lb_push-s++++++++++:%d",len);
    //show_buf(p_lb->pbuf,p_lb->size);

    if(p_lb->state&LB_STATE_IS_M){return -1;}
    if(p_lb->state&LB_STATE_IS_W){return -1;}
    
    rest_len = lb_get_rest_len(p_lb);
    real_len = (rest_len>len)?len:rest_len;
    
    p_lb->state |=LB_STATE_IS_W;
        memcpy(&p_lb->pbuf[p_lb->w_index],p_src,real_len);
        p_lb->w_index+=real_len;
    p_lb->state &=~LB_STATE_IS_W;
    //DBG_I("lb_push-a_+++++++++++++++++++");
    //show_buf(p_lb->pbuf,p_lb->size);
    
    
    return real_len;    
}


int32_t lb_pull(linebuf_t*    p_lb,uint8_t* p_dest,uint16_t len)
{
    int16_t real_len = 0;
    uint16_t data_len = 0;
    
    if(p_lb->state&LB_STATE_IS_M){return -1;}
    if(p_lb->state&LB_STATE_IS_R){return -1;}
    
    //DBG_I("lb_pull-s:==================%D",len);
    //show_buf(p_lb->pbuf,p_lb->size);
    data_len = lb_get_data_len(p_lb);
    real_len = (data_len>len)?len:data_len;
    
    p_lb->state |=LB_STATE_IS_R;
        memcpy(p_dest,&p_lb->pbuf[p_lb->r_index],real_len);
        p_lb->r_index+=real_len;
    p_lb->state &=~LB_STATE_IS_R;
    //DBG_I("lb_pull-a=================");
    //show_buf(p_lb->pbuf,p_lb->size);
    
    return real_len;    
}

/*
三种情况
1： len 小于 r_index,w_index
2： len 大于 r_index,小于 w_index
3： len 大于 r_index,w_index
*/

int32_t lb_move(linebuf_t*    p_lb,uint16_t len)
{

    #if 1
    int16_t real_len = 0;
    uint8_t* pdest = NULL;
    uint8_t* pfrom= NULL;
    uint16_t move_len = 0;
    
    
    if(p_lb->state&LB_STATE_IS_R){return -1;}
    if(p_lb->state&LB_STATE_IS_W){return -1;}
    if(p_lb->state&LB_STATE_IS_M){return -1;}
    
    p_lb->state |=LB_STATE_IS_M;
    
    
    //DBG_I("lb_pull-M--------------:%d",len);
    //show_buf(p_lb->pbuf,p_lb->size);
    
    if(len<p_lb->r_index)
    {
        pdest         = &p_lb->pbuf[p_lb->r_index - len];
        pfrom         = &p_lb->pbuf[p_lb->r_index];
        move_len     = p_lb->w_index - p_lb->r_index;
        memmove(pdest,pfrom,move_len);
        p_lb->r_index -= len;
        p_lb->w_index -= len;
    }
    else if(len>p_lb->w_index)
    {    
        p_lb->r_index = 0;
        p_lb->w_index = 0;
    }
    else
    {
        pdest         = p_lb->pbuf;
        pfrom         = &p_lb->pbuf[len];
        move_len     = p_lb->w_index - len;
        memmove(pdest,pfrom,move_len);
        p_lb->r_index = 0;
        p_lb->w_index -=len;
    }

    //开启memset之后非常消耗资源，目前在1224 bytes消耗cpu在40%，关掉在9%    
    //memset(&p_lb->pbuf[p_lb->w_index],0,(p_lb->size-p_lb->w_index));
    
    //DBG_I("lb_pull-M--------------A");
    //show_buf(p_lb->pbuf,p_lb->size);
    
    p_lb->state &=~LB_STATE_IS_M;
    
    return real_len;    

    #else
    uint8_t*   dest_prt;
    uint8_t*   src_prt;
    uint16_t     data_len;
    dest_prt = &p_lb->pbuf[0];
    src_prt  = &p_lb->pbuf[p_lb->r_index];
    data_len = lb_get_data_len(p_lb);

    memmove(dest_prt,src_prt,data_len);

    p_lb->w_index -= p_lb->r_index;
    p_lb->r_index = 0;
    return data_len;
    
    #endif
}




