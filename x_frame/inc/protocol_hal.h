#ifndef __PROTOCOL_HAL_H__
#define __PROTOCOL_HAL_H__

#include "linebuf.h"


#define MAX_PROTCOL_NUM     4
#define MAX_LINK_NUM        20
#define MAX_PACK_LEN        256
#define MAX_SEND_PACK_LEN     (MAX_PACK_LEN+20)
#define MAX_RECV_PACK_LEN     (MAX_PACK_LEN+20)
#define PROTOCOL_RUN_PERIO     (5)   //5ms


#define UNPACK_HEAD     0
#define UNPACK_BODY        1

#define DEFAUT_V2_CHECK_TYPE  1

#define EN_ROUTE        0x01
#define DIS_ROUTE        0x00

#define IS_HOST_PACK    (0)
#define IS_ROUTE_PACK    (1)
#define UN_KNOW_PACK    (2)
#define DIRECT_PACK        (3)
#define ERR_ADDR_PACK    (4)
#define IS_LOOP_PACK    (5)
//route relative 
#define ROUTE_ADDR        0x01
#define ROUTE_CMD        0x02
#define ROUTE_ALL        0x03

#define EN_ROUTE        0x01
#define DIS_ROUTE        0x00

#define UN_USE_CMD        0xff
#define UN_USE_ADDR        0xff

#define ERR_LINK_ID        (0xff)

typedef enum{
    CHECK_TYPE_SUM      =0,
    CHECK_TYPE_8CRC     ,
    CHECK_TYPE_16CRC    ,
    CHECK_TYPE_32CRC    ,
}check_type_e;
typedef uint8_t (*pf_opt_route)(uint8_t dest);

typedef struct {
    uint8_t            route_type;//  是地址路由還是cmd路由
    uint8_t         opt;

    uint8_t         dest;

    uint8_t            cmd_set;
    uint8_t            cmd_id;

    uint8_t         tag_link_id;
    pf_opt_route    func_opt;
} link_route_t;

typedef struct {
    uint32_t  link_num;
    list_t    link_head;
} link_ctrl_t;



typedef struct {
    uint8_t     link_id;
    uint8_t     src;
    uint8_t     dest;
    uint8_t     enc_type;
    uint8_t         check_type;
    uint16_t     cmd_func;
    uint16_t     cmd_id;
    uint16_t     data_len;
    uint8_t     need_ack;
    uint8_t        is_ack;
    uint8_t     ack_type;
    uint8_t*     pdata;
    uint32_t     seq; /*只读信息*/
    uint32_t     time_snap; /*只读信息*/
    uint8_t     is_rw_cmd;
    uint8_t     is_queue;

} hal_frame_info_t;

typedef int32_t         (*pf_action_func)       (hal_frame_info_t* p_frame);
typedef struct {

    uint8_t         cmd_func;
    uint8_t         cmd_id;
    pf_action_func  action_func;
    uint32_t        link_id;
    const char*     func_name;
    list_t          action_list;
} action_tab_t;


typedef int32_t            (*pf_check_route)       (hal_frame_info_t* p_frame, uint8_t* p_tag_link_id);
typedef pf_action_func  (*pf_user_action_func)  (hal_frame_info_t* p_frame);
typedef int32_t         (*pf_user_action)       (hal_frame_info_t* p_frame, action_tab_t* action_tab, uint32_t max_item);


typedef struct {
    const char*             name;
    uint32_t                link_id;
    uint32_t                prio;       //链路运行在哪个任务的优先级上面
    uint32_t                read_dev;
    uint32_t                write_dev;
    uint32_t                protocol_type;
    uint8_t*                p_buf;
    uint16_t                buf_len;
    uint8_t                    en_route;
    uint8_t                 en_debug;
    uint8_t                 flag_init;
    uint8_t                    unpack_step;
    uint32_t                send_seq;
    uint32_t                recv_seq;
    linebuf_t               line_buf_obj;
    list_t                  link_list;
    list_t                  action_list_head;

    /*  增加链路的统计信息  */
} link_hal_item;



/*p_raw_buf 表示包头的缓冲区地址*/
typedef uint16_t  (*pf_get_header_len)    (uint8_t* p_raw_buf);
typedef uint16_t  (*pf_get_data_len)    (uint8_t* p_raw_buf);
typedef uint8_t   (*pf_check_header)    (uint8_t* p_raw_buf);
typedef uint8_t   (*pf_check_data)        (uint8_t* p_raw_buf);
typedef uint16_t  (*pf_pack_data)        (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info);
typedef uint16_t  (*pf_unpack_data)        (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info);



typedef struct {
    const char*            protocol_name;
    uint32_t             protocol_num;
    pf_get_header_len    get_head_len;
    pf_get_data_len        get_pack_len;
    pf_check_header        check_header;
    pf_check_data        check_data;
    pf_pack_data        pack_data;
    pf_unpack_data        unpack_data;
} protocol_obj_t;

#define ADD_ADDR_ROUTE(en,dest,tag_link_id,opt_func) {ROUTE_ADDR,en,dest,UN_USE_CMD, UN_USE_CMD,tag_link_id,opt_func}

#define ADD_LINK(link_id,prio,dev,protocol_ver,buf_size,en_route,en_debug)  \
            PACK_4 uint8_t reg_link_buf_##link_id[buf_size]; \
            link_hal_item    reg_link_##link_id = {#link_id,link_id,prio,dev,dev,protocol_ver,reg_link_buf_##link_id,buf_size,en_route,en_debug};\
            AUTO_REG_ITEM(LINK_OBJ_##link_id,AUTO_REG_TYPE_LINK,(void*)&reg_link_##link_id)

#define ADD_ROUTE(route_func) \
        AUTO_REG_ITEM(route_tab,AUTO_REG_ROUTE_CFG,(void*)&route_func)

#define ADD_PROTOCOL(protocol_id,get_head_len,get_pack_len,check_head,check_pack,pack_data,unpack_data) \
    const protocol_obj_t protocol_id##_obj={#protocol_id,protocol_id,get_head_len,get_pack_len,check_head,check_pack,pack_data,unpack_data};\
    AUTO_REG_ITEM(protocol_id##_entity,AUTO_REG_PROTOCOL,(void*)&protocol_id##_obj)


#define ADD_LINK_ACTION(link_id,cmd_set,cmd_id,func)  \
                action_tab_t  action_##link_id##_##cmd_set##_##cmd_id={cmd_set,cmd_id,func,link_id,#func};\
                AUTO_REG_ITEM(link_id##_##cmd_set##_##cmd_id ,AUTU_REG_TYPE_LINK_ACTION,(void*)&action_##link_id##_##cmd_set##_##cmd_id)


//============================================外可以调用的函数---start
void     protocol_init    (void);
int32_t  protocol_send    (hal_frame_info_t* p_frame_info);
uint8_t calc_check(uint8_t* p_raw,uint16_t len,uint8_t check_type,void* p_result);

//============================================外可以调用的函数---end





#endif //__PROTOCOL_HAL_H__
