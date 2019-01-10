//#define DEF_DBG_I
//#define DEF_DBG_W
//#define DEF_DBG_E
//#include "debug.h"
#include "ucprobe_action.h"

#if DISABLE_UCPROBE_MOD == 0
//ADD_LINK_ACTION(PC_LINK_ID      ,UCPROBE_CMD_SET    ,PROBE_COM_FMT_RX_QUERY     ,uc_probe_query_RX_MAX);
//ADD_LINK_ACTION(PC_LINK_ID      ,UCPROBE_CMD_SET    ,PROBE_COM_FMT_RX_RD        ,uc_probe_query_RX_RD);
//ADD_LINK_ACTION(PC_LINK_ID      ,UCPROBE_CMD_SET    ,PROBE_COM_FMT_RX_WR        ,uc_probe_query_RX_WR);
//ADD_LINK_ACTION(PC_LINK_ID      ,UCPROBE_CMD_SET    ,PROBE_COM_FMT_RX_RD_MULTI  ,uc_probe_query_RX_RD_MULTI);

void ucprobe_send_ack(hal_frame_info_t* p_frame_info,uint8_t* pbuf,uint16_t len)
{
    uint8_t addr = p_frame_info->dest;
    p_frame_info->dest = p_frame_info->src;
    p_frame_info->src  = addr;
	p_frame_info->is_ack = 1;
    p_frame_info->pdata = pbuf;
    p_frame_info->data_len = len;
    
    protocol_send(p_frame_info);
}


int32_t         uc_probe_query_RX_MAX       (hal_frame_info_t* p_frame_info)
{
    
    uint32_t query_cmd_id = 0;
    memcpy(&query_cmd_id,p_frame_info->pdata,2);
    //DBG_I("uc_probe_query_rx_max!  -- len:%d  query_cmd_id:0x%x\r\n",p_frame_info->data_len,query_cmd_id);
    
    switch(query_cmd_id)
    {
        case PROBE_COM_QUERY_MAX_RX_SIZE: 
        case PROBE_COM_QUERY_MAX_TX_SIZE:
            {
                max_rx_size_ack_t get_max_rx_size_ack={0};
                get_max_rx_size_ack.status = 1;
                get_max_rx_size_ack.modify = 0x07;
                get_max_rx_size_ack.max_rx_size = 256;
                ucprobe_send_ack(p_frame_info,(uint8_t*)&get_max_rx_size_ack,sizeof(max_rx_size_ack_t));
            }
            break;
        
        case PROBE_COM_QUERY_ENDIANNESS_TEST:
            {
                end_test_ack_t end_test_ack={0};
                end_test_ack.status = 1;
                end_test_ack.modify = 0xf;
                end_test_ack.end_test = 0x12345678;
                ucprobe_send_ack(p_frame_info,(uint8_t*)&end_test_ack,sizeof(end_test_ack_t));
            }
            break;
		case PROBE_COM_QUERY_FMT_SUPPORT:
            {
                fmt_support_ack_t fmt_support_ack={0};
                fmt_support_ack.status = 1;
                fmt_support_ack.modify = 0xf;
                fmt_support_ack.support_rx_query = PROBE_COM_FMT_RX_QUERY;
				fmt_support_ack.support_rx_rd= PROBE_COM_FMT_RX_RD;
				fmt_support_ack.support_rx_rd_multi= PROBE_COM_FMT_RX_RD_MULTI;
				fmt_support_ack.support_rx_wr= PROBE_COM_FMT_RX_WR;
				fmt_support_ack.support_rx_wr_multi= PROBE_COM_FMT_RX_WR_MULTI;
				fmt_support_ack.support_rx_str_in= PROBE_COM_FMT_RX_STR_IN;
				fmt_support_ack.support_rx_str_out= PROBE_COM_FMT_RX_STR_OUT;
				fmt_support_ack.support_rx_terminal_exe= PROBE_COM_FMT_RX_TERMINAL_EXEC;
				fmt_support_ack.support_rx_terminal_in= PROBE_COM_FMT_RX_TERMINAL_IN;
				fmt_support_ack.support_rx_terminal_out= PROBE_COM_FMT_RX_TERMINAL_OUT;

                ucprobe_send_ack(p_frame_info,(uint8_t*)&fmt_support_ack,sizeof(fmt_support_ack_t));

				//DBG_I("PROBE_COM_QUERY_FMT_SUPPORT!!!");
            }
            break;
        
        
        default:break;
    }
    
    return 0;
}


int32_t         uc_probe_query_RX_RD       (hal_frame_info_t* p_frame_info)
{
    read_req_t req_read={0};
    read_ack_t read_ack={0};
    memcpy(&req_read,p_frame_info->pdata,sizeof(read_req_t));
    
    //DBG_I("uc_probe_query_RX_RD!  -- len:%d  read_bytes:%d addr:0x%x.\r\n",p_frame_info->data_len,req_read.read_bytes,req_read.start_addr);
    
    read_ack.status = 1;
    read_ack.modify = 0xf;
    memcpy(read_ack.read_dat,(void*)req_read.start_addr,req_read.read_bytes);
    
    ucprobe_send_ack(p_frame_info,(uint8_t*)&read_ack,req_read.read_bytes+2);

    return 0;
}

int32_t         uc_probe_query_RX_WR       (hal_frame_info_t* p_frame_info)
{
    write_req_t req_write={0};
    write_ack_t write_ack={0};
    memcpy(&req_write,p_frame_info->pdata,p_frame_info->data_len);
    
    memcpy((void*)req_write.start_addr,(void*)req_write.write_dat,req_write.write_bytes);
    //DBG_I("uc_probe_query_RX_WR!  -- len:%d  read_bytes:%d addr:0x%x.\r\n",p_frame_info->data_len,req_write.write_bytes,req_write.start_addr);
    
    write_ack.status = 1;
    write_ack.modify = 0xf;
    
    
    ucprobe_send_ack(p_frame_info,(uint8_t*)&write_ack,sizeof(write_ack));

    return 0;
}





int32_t         uc_probe_query_RX_RD_MULTI       (hal_frame_info_t* p_frame_info)
{
    read_multi_req_t req_read_multi={0};
    read_multi_ack_t read_multi_ack={0};
    
    uint32_t i = 0;
    uint32_t item_num = p_frame_info->data_len / sizeof(read_req_t);
    uint8_t* pbuf = read_multi_ack.read_dat;
    uint32_t total_bytes = 0;
    
    memcpy(&req_read_multi,p_frame_info->pdata,sizeof(read_req_t));
    
    //DBG_I("uc_probe_query_RX_RD_MULTI!");
    //DBG_I("uc_probe_query_RX_RD!  -- len:%d  read_bytes:%d addr:0x%x.\r\n",p_frame_info->data_len,req_read.read_bytes,req_read.start_addr);
    
    read_multi_ack.status = 1;
    read_multi_ack.modify = 0xf;
    
    for(i=0;i<item_num;i++)
    {
        memcpy(&pbuf[total_bytes],(void*)req_read_multi.read_item[i].start_addr,req_read_multi.read_item[i].read_bytes);
        total_bytes+=req_read_multi.read_item[i].read_bytes;
    }
    
    
    ucprobe_send_ack(p_frame_info,(uint8_t*)&read_multi_ack,total_bytes+2);

    return 0;
}
#endif
