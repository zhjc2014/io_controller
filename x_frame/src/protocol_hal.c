#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "x_frame.h"
extern uint32_t nrf_lost_cnt;

typedef uint8_t (*pf_calc_check)(uint8_t* pbuf, uint32_t len, void* p_result);
typedef struct {
    int check_type;
    pf_calc_check check_func;
} pf_calc_check_t;

extern void*   auto_reg_get_func_cont_by_index(uint8_t func_type, int32_t index);
extern int32_t auto_reg_get_func_type_cont(uint8_t func_type, void* p_cont[], uint8_t max_read_item);

static protocol_obj_t*  sg_protocol_tab[MAX_PROTCOL_NUM]        = {0};
static link_hal_item*   sg_link_tab[MAX_LINK_NUM + 1]           = {0};
static pf_check_route   sg_sys_check_route                      = NULL;
static uint8_t          real_reg_link_num                       = 0;
static link_ctrl_t      sg_link_prio[MAX_TASKS_GROUP]           = {0};

int32_t protocol_send_by_id(hal_frame_info_t* p_frame_info)
{
    static volatile uint8_t lock = 0;
    uint8_t send_buf[MAX_SEND_PACK_LEN] = {0};
    protocol_obj_t* p_protocol;
    link_hal_item*  p_link ;
    int32_t ret = 0;
    uint16_t pack_len = 0;

    if(lock) {
        return -1;
    }
    lock = 1;

    p_link     = sg_link_tab[p_frame_info->link_id];
    p_protocol = sg_protocol_tab[p_link->protocol_type];

    if( (p_link == NULL) || (p_protocol == NULL)) {
        goto end;
    }

    p_frame_info->seq  = p_link->send_seq++;
    pack_len = p_protocol->pack_data(send_buf, p_frame_info);
    ret = hal_app_write(p_link->write_dev, send_buf, pack_len, 0, NULL);
	
//	if (p_frame_info->src == PC_ADDR && p_frame_info->dest >= 50 && p_frame_info->dest <= 55){
//		DBG_I("from pc to car %d:", p_frame_info->dest - 50);
//		show_buf(send_buf, pack_len);
//	}
end:
    lock = 0;
    return ret;
}


int32_t  protocol_send(hal_frame_info_t* p_frame_info)
{
#if DISABLE_ROUTE == 0
    link_hal_item* p_link = sg_link_tab[p_frame_info->link_id];
    uint8_t     pack_type = ERR_ADDR_PACK;
    uint8_t     tag_link_id = 0;

    if(p_link == NULL) {
        return -1;
    }
    if(p_frame_info->link_id > MAX_LINK_NUM) {
        return -1;
    }

    if(p_frame_info->check_type == 0) {
        p_frame_info->check_type = DEFAUT_V2_CHECK_TYPE;
    }


    /*

      路由逻辑

            链路支持路由  |     ok    |    ok    |    no    |    no     |
    ----------------------+-----------+----------+----------+-----------+
        系统已经注册路由表|     ok    |    no    |    ok    |    no     |
    ----------------------+-----------+----------+----------+-----------+
           此包的发送结果 |    route  | dis_route| dis_route| dis_route |

    */

    if((p_link->en_route == EN_ROUTE) && (sg_sys_check_route != NULL)) {
        //---需要进行路由的包
        pack_type = sg_sys_check_route(p_frame_info, &tag_link_id);
        if(pack_type == IS_ROUTE_PACK) {
            p_frame_info->link_id = tag_link_id;
        }
    }
#endif
    return protocol_send_by_id(p_frame_info);
}


static void find_and_exec_action(hal_frame_info_t* p_frame_info)
{
    list_t* tag_list_head = &sg_link_tab[p_frame_info->link_id]->action_list_head;
    uint8_t en_debug      =  sg_link_tab[p_frame_info->link_id]->en_debug;
    list_t  *cur_list;
    action_tab_t  * p_action_item = NULL;
    uint8_t flag_is_exec = 0;

    //DBG_I("recv [cmd_set:0x%02x ][cmd_id:0x%02x ]",p_frame_info->cmd_func,p_frame_info->cmd_id);
    if(sg_link_tab[p_frame_info->link_id]->flag_init) {
        list_for_each(cur_list, tag_list_head) {
            p_action_item = list_entry(cur_list, action_tab_t, action_list);
            if( (p_action_item->cmd_func == p_frame_info->cmd_func ) &&
                    (p_action_item->cmd_id   == p_frame_info->cmd_id   ) ) {
                p_action_item->action_func(p_frame_info);
                flag_is_exec = 1;
                //DBG_I("action [cmd_set:%4d ][cmd_id:%4d ]: %s",p_frame_info->cmd_func,p_frame_info->cmd_id,p_action_item->func_name);
                break;
            }
        }
    }
    if(en_debug) {
        if(flag_is_exec == 0) {
            DBG_W("unknow pack [cmd_set:0x%02x cmd_id:0x%02x]", p_frame_info->cmd_func, p_frame_info->cmd_id);
        } else {
            DBG_I("recv [cmd_set:0x%02x ][cmd_id:0x%02x ] : %s", p_frame_info->cmd_func, p_frame_info->cmd_id, p_action_item->func_name);
        }
    }
}

static void run_unpack(link_hal_item* p_link)
{
    protocol_obj_t* p_protocol = sg_protocol_tab[p_link->protocol_type];
    uint32_t read_len = 0;
    uint16_t lb_rest_len = 0;
    uint16_t lb_have_len = 0;
    int32_t  ret = 0;
    uint8_t* p_buf = NULL;
    static volatile uint8_t lock = 0;
    uint8_t read_buf[MAX_RECV_PACK_LEN];

    if(lock) {
        return;
    }
    lock = 1;

    lb_move(&p_link->line_buf_obj, p_link->line_buf_obj.r_index);	//move the not read data left for length(param2)
    lb_rest_len = lb_get_rest_len(&p_link->line_buf_obj);			//get the idle space 
    read_len = (lb_rest_len < MAX_RECV_PACK_LEN) ? lb_rest_len : MAX_RECV_PACK_LEN;	//the length want to read

    if(read_len > 0) {
        ret = hal_app_read(p_link->read_dev, read_buf, read_len, 0, &read_len);//the bottom interface is ringbuffer
    }

    if( (ret >= 0) && (read_len > 0) ){
        lb_push(&p_link->line_buf_obj, read_buf, read_len);

		//for debug
//		if (p_link->link_id >= RF_LINK_0_ID && p_link->link_id <= RF_LINK_5_ID){
//			DBG_I("car%d data:", p_link->link_id - RF_LINK_0_ID);
//			show_buf(read_buf, read_len);
//		}
    }

    lock = 0;
	
	//for debug
    do {
        lb_have_len = lb_get_data_len(&p_link->line_buf_obj);
        switch(p_link->unpack_step) {
        case UNPACK_HEAD:
            if(lb_have_len >= p_protocol->get_head_len(NULL)) {
                p_buf = lb_get_r_ptr(&p_link->line_buf_obj);
                if(p_protocol->check_header(p_buf)) {
                    p_link->unpack_step = UNPACK_BODY;
                } else {
                    DBG_E_LIMIT(1000, "[%s]just head err pack!!", p_link->name);
//                    show_buf(p_buf,11);
                    p_link->line_buf_obj.r_index++;
                }
            } else {
                goto end;   //字节数不够,等待下一个周期
            }
            break;

        case UNPACK_BODY:
            p_buf = lb_get_r_ptr(&p_link->line_buf_obj);
            if(lb_have_len >= p_protocol->get_pack_len(p_buf)) {
                p_link->unpack_step = UNPACK_HEAD;
                if(p_protocol->check_data(p_buf)) {
                    hal_frame_info_t frame_info = {0};
                    uint8_t tag_link_id = 0;
                    p_protocol->unpack_data(p_buf, &frame_info);
                    p_link->recv_seq = frame_info.seq;
                    frame_info.link_id = p_link->link_id;


                    if((p_link->en_route == EN_ROUTE) && (sg_sys_check_route != NULL)) {
                        uint8_t pack_type;
                        //---需要进行路由的包
                        pack_type = sg_sys_check_route(&frame_info, &tag_link_id);
                        if(pack_type == IS_ROUTE_PACK) {
                            frame_info.link_id = tag_link_id;
                            protocol_send_by_id(&frame_info);
							//data from car
							if (frame_info.src >= CAR0_ADDR && frame_info.src <= CAR4_ADDR){
								nrf_lost_cnt = 0;
							}
							
                        }else if (pack_type == IS_HOST_PACK){
                            //查找并且调用响应函数
                            find_and_exec_action(&frame_info);
                        }else if (pack_type == IS_LOOP_PACK){
							DBG_I("loop package, tx rx is shorted?!");
						}else{
                            DBG_I("unknown package");
                        }
                    } else {
                        //p_link->user_action(&frame_info,p_link->p_action__tab,p_link->remap_tab_num);
                        //查找并且调用响应函数
                        find_and_exec_action(&frame_info);
                    }

                    p_link->line_buf_obj.r_index += p_protocol->get_pack_len(p_buf);

                } else {
                    DBG_E_LIMIT(1000, "[%s]just have err pack!!####! -len:%d ", p_link->name, p_protocol->get_pack_len(p_buf));
                    //show_buf(p_buf,p_protocol->get_pack_len(p_buf));
                    p_link->line_buf_obj.r_index += p_protocol->get_head_len(p_buf);
                }
            } else {
                goto end;   //字节数不够,等待下一个周期
            }
            break;

        default:
            DBG_E("unpack err!!!");
            while(1) ;  //此处不会出现
        }

    } while(1);

end:
    return;
}


static int32_t  protocol_ontick(uint32_t prio)
{
    /*运行不同优先级的解包*/
    list_t *     p_link_list_head = NULL;
    link_hal_item*  p_cur_link;
    list_t    *cur_node = NULL;

    if(prio >= MAX_TASKS_GROUP) {
        return -1;
    }

    p_link_list_head = &sg_link_prio[prio].link_head;

    list_for_each(cur_node, p_link_list_head) {
        p_cur_link = list_entry(cur_node, link_hal_item, link_list);
        if(p_cur_link->buf_len < 3) {
            return 0;   /// 此链路不需要解包
        }
        run_unpack(p_cur_link);
//        DBG_I("run_unpack :%s",p_cur_link->name);
    }
    return 0;
}


#define VH_TASK   0
#define HI_TASK   1
#define LW_TASK   2
#define ID_TASK   3

void run_protocol_vh(void)
{
    protocol_ontick(VH_TASK);
}
void run_protocol_hi(void)
{
    protocol_ontick(HI_TASK);
}
void run_protocol_lw(void)
{
    protocol_ontick(LW_TASK);
}
void run_protocol_id(void)
{
    protocol_ontick(ID_TASK);
}

REG_TASK(VH_TASK, PROTOCOL_RUN_PERIO, run_protocol_vh);
REG_TASK(HI_TASK, PROTOCOL_RUN_PERIO, run_protocol_hi);
REG_TASK(LW_TASK, PROTOCOL_RUN_PERIO, run_protocol_lw);
REG_TASK(ID_TASK, PROTOCOL_RUN_PERIO, run_protocol_id);

void  protocol_init(void)
{
    uint32_t i = 0;
    protocol_obj_t* p_cur_protocol;
    link_hal_item*  p_cur_link;

    for(i = 0; i < MAX_PROTCOL_NUM; i++) {
        sg_protocol_tab[i] = NULL;
    }

    for(i = 0; i < MAX_TASKS_GROUP; i++) {
        sg_link_prio[i].link_num = 0;
        INIT_LIST_HEAD(&sg_link_prio[i].link_head);
    }

    /*1:加载协议*/
    for(i = 0; i < MAX_PROTCOL_NUM; i++) {
        p_cur_protocol = (protocol_obj_t*)auto_reg_get_func_cont_by_index(AUTO_REG_PROTOCOL, i);
        if( (p_cur_protocol != NULL) && (p_cur_protocol->protocol_num < MAX_PROTCOL_NUM) ) {
            sg_protocol_tab[p_cur_protocol->protocol_num] = p_cur_protocol;
        }
    }

    /*2:加载链路*/
    for(i = 0; i < MAX_LINK_NUM; i++) {
        p_cur_link = (link_hal_item*)auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_LINK, i);
        if( (p_cur_link != NULL)                          &&
                (p_cur_link->link_id < MAX_LINK_NUM)        &&
                (p_cur_link->prio < MAX_TASKS_GROUP)          &&
                (p_cur_link->protocol_type < MAX_PROTCOL_NUM)
          ) {
            sg_link_tab[p_cur_link->link_id] = p_cur_link;
            sg_link_tab[p_cur_link->link_id]->flag_init  = 1;
            list_add_tail(&p_cur_link->link_list, &sg_link_prio[p_cur_link->prio].link_head);
            lb_init(NULL, &p_cur_link->line_buf_obj, p_cur_link->p_buf, p_cur_link->buf_len);
            real_reg_link_num++;
            INIT_LIST_HEAD(&p_cur_link->action_list_head);
        }
    }

    /*3:加载action_tab*/
    for(i = 0; i < 10000; i++) { //
        action_tab_t* p_action_item = NULL;
        p_action_item = (action_tab_t*)auto_reg_get_func_cont_by_index(AUTU_REG_TYPE_LINK_ACTION, i);

        if(p_action_item == NULL) {
            break;
        }

//        if(p_action_item->link_id <= real_reg_link_num) {
        if(p_action_item->link_id <= MAX_LINK_NUM && \
                sg_link_tab[p_action_item->link_id]->flag_init) {
            INIT_LIST_HEAD(&p_action_item->action_list);
            list_add_tail(&p_action_item->action_list, &sg_link_tab[p_action_item->link_id]->action_list_head);
            //DBG_I("cur action item:%s",p_action_item->func_name);
        } else {
            DBG_W("ADD action item- cmd_set:%d cmd_id:%d link_id:%d is err!!!", p_action_item->cmd_func, p_action_item->cmd_id, p_action_item->link_id);
        }
    }

    DBG_I("system max interface action:%d", i);

    /*4:加载路由信息*/
    auto_reg_get_func_type_cont(AUTO_REG_ROUTE_CFG, (void**)&sg_sys_check_route, 1);

    /*5:初始化链路*/
    for(i = 0; i < MAX_LINK_NUM; i++) {
        if(sg_link_tab[i] != NULL) {
            sg_link_tab[i]->send_seq 		= 0;
            sg_link_tab[i]->recv_seq 		= 0;
            sg_link_tab[i]->unpack_step     = UNPACK_HEAD;
        }
    }
}

//package check////////////////////////////////////////
////////////////for crc8 check/////////////////////////
#define CRC8(CRC, C)  ((CRC = crc8Table[CRC ^ C]) & 0xFF)
static uint8_t crc8Table[256];  /* 8-bit crc table */
static void initCrc8()
{
    int i;
    int j;
    uint8_t crc;
    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 0; j < 8; j++) {
            crc = (crc << 1) ^ ((crc & 0x80) ? 0x07 : 0);
        }
        crc8Table[i] = crc & 0xFF;
    }
}
REG_INIT_MOD(initCrc8);

static uint8_t crc8Buff(uint8_t *buff, uint8_t len)
{
    uint8_t i = 0;
    uint8_t crc = 0;
    for (i = 0; i < len; i++) {
        CRC8(crc, buff[i]);
    }
    return crc;
}
///////////////for crc8 check end////////////////////////

///////////////for crc16 check////////////////////////
/*
 * CRC lookup table for bytes, generating polynomial is 0x8005
 * input: reflexed (LSB first)
 * output: reflexed also...
 */
static const uint16_t crc_ibm_table[256] = {
  0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
  0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
  0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
  0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
  0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
  0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
  0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
  0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
  0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
  0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
  0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
  0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
  0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
  0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
  0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
  0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
  0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
  0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
  0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
  0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
  0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
  0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
  0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
  0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
  0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
  0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
  0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
  0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
  0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
  0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
  0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
  0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};

static uint16_t crc_ibm_byte(uint16_t crc, const uint8_t c)
{
	const unsigned char lut = (crc ^ c) & 0xFF;
	return (crc >> 8) ^ crc_ibm_table[lut];
}

/**
 * crc_ibm - recompute the CRC for the data buffer
 * @crc - previous CRC value
 * @buffer - data pointer
 * @len - number of bytes in the buffer
 */
uint16_t crc_ibm(uint8_t const *buffer, uint16_t len)
{
	uint16_t crc = 0x0000;
	while (len--)
		crc = crc_ibm_byte(crc, *buffer++);
	return crc;
}
///////////////for crc16 check end////////////////////


static uint8_t sum_check(uint8_t* pbuf, uint32_t len, void* p_result)
{
    int i;
    uint8_t sum = 0;
    for (i = 0; i < len; i++) {
        sum += pbuf[i];
    }
    *(uint8_t*)p_result = sum;
    return 0;
}
static uint8_t crc8_check(uint8_t* pbuf, uint32_t len, void* p_result)
{
    *(uint8_t*)p_result = crc8Buff(pbuf, len);
    return 0;
}

static uint8_t crc16_check(uint8_t* pbuf, uint32_t len, void* p_result)
{
    *(uint16_t*)p_result = crc_ibm(pbuf, len);
    return 0;
}

static uint8_t crc32_check(uint8_t* pbuf, uint32_t len, void* p_result)
{
    *(uint32_t*)p_result = 0;
    return 0;
}

static const pf_calc_check_t g_check_func_sets[] = {
    {CHECK_TYPE_SUM, sum_check   },
    {CHECK_TYPE_8CRC, crc8_check  },
    {CHECK_TYPE_16CRC, crc16_check },
    {CHECK_TYPE_32CRC, crc32_check },
};

uint8_t calc_check(uint8_t* p_raw, uint16_t len, uint8_t check_type, void* p_result)
{
    int i;
    for (i = 0; i < ARRY_ITEMS_NUM(g_check_func_sets); i++) {
        if (check_type == g_check_func_sets[i].check_type \
                && g_check_func_sets[i].check_func != NULL) {
            g_check_func_sets[check_type].check_func(p_raw, len, p_result);
            return 0;
        }
    }
    return 1;
}
///////////////////////////////////////////////////////


#if TINY_FUNC_CODE == 0

int32_t list_links(uint8_t argc, uint8_t *argv[])
{
    uint32_t i;
    DBG_I("*cur system reg  %d links ", real_reg_link_num);
    for(i = 0; i < MAX_LINK_NUM; i++) {
        if(sg_link_tab[i] != NULL) {
            DBG_I("[link_id:%d] prio:%d -  %-12s  protocol-type:%d   bufsize:%d ", sg_link_tab[i]->link_id, sg_link_tab[i]->prio, sg_link_tab[i]->name, sg_link_tab[i]->protocol_type, sg_link_tab[i]->buf_len);
        }
    }

    return RET_OK;
}

REG_SHELL_CMD_X(ls_links, 0, list_links, "list_links", "list_links.  eg list_links \r\n ");


int32_t list_action(uint8_t argc, uint8_t *argv[])
{
    if(argc >= 2) {
        uint32_t link_id = 0;
        sscanf((const char*)argv[1], "%d", &link_id);
        if(link_id > real_reg_link_num) {
            DBG_W("link id is err! max_link_id[%d]", real_reg_link_num);
        } else {

            list_t *        p_link_list_head = &sg_link_tab[link_id]->action_list_head;
            list_t          *cur_list;
            action_tab_t    * p_action_item = NULL;

            if(sg_link_tab[link_id]->flag_init) {
                DBG_I("The Link ID:%d action function as follow:", link_id);
                list_for_each(cur_list, p_link_list_head) {
                    p_action_item = list_entry(cur_list, action_tab_t, action_list);
                    DBG_I("[cmd_set:0x%02x ][cmd_id:0x%02x ]: %s", p_action_item->cmd_func, p_action_item->cmd_id, p_action_item->func_name);
                }
            } else {
                DBG_W("LINK_ID %d is not exist!!!", link_id);
            }
        }
    } else {
        DBG_W("param is error, [ eg. ] list_action link_id ");
    }
    return RET_OK;
}

REG_SHELL_CMD_X(list_action, 0, list_action, "list_action", "list_action.  eg list_action link_id\r\n ");
#endif

