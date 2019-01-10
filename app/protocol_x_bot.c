#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"

static uint16_t 	v1_get_header_len		(uint8_t* pbuf);
static uint16_t 	v1_get_data_len		    (uint8_t* pbuf);
static uint8_t 	    v1_check_header		    (uint8_t* pbuf);
static uint8_t 	    v1_check_data			(uint8_t* pbuf);
static uint16_t     v1_pack_data			(uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info);
static uint16_t     v1_unpack_data			(uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info);

ADD_PROTOCOL(   PROTOCOL_X_BOT,
                v1_get_header_len,
                v1_get_data_len,
                v1_check_header,
                v1_check_data,
                v1_pack_data,
                v1_unpack_data
            ) ;


#pragma pack(1)
typedef struct {
    uint8_t 	sof1;                //1 byte
    uint8_t     sof2;                //1 byte
    uint8_t 	datalen;             //1 byte
    uint8_t		version		    : 4; //1 byte
    uint8_t 	resv		    : 4;
    uint8_t		need_ack		: 1; //1 byte
    uint8_t		is_ack			: 1;
    uint8_t		check_type		: 2;
    uint8_t		enc_type		: 3;
    uint8_t		seq_type		: 1;
    uint8_t   	seq;                 //1 byte
    uint8_t		src;                 //1 byte
    uint8_t		dest;                //1 byte
    uint8_t 	cmdfunc;             //1 byte
    uint8_t 	cmdid;               //1 byte
    uint8_t     headcheck;           //1 byte
    uint8_t 	pdata[1];            //1 byte
} v1_data_struct_t;
#pragma pack()


#define v1_HEAD_SOF1	(0xAA)
#define v1_HEAD_SOF2	(0xBB)
#define v1_VERSION		(2)
#define v1_HEAD_LEN		(sizeof(v1_data_struct_t)-1)

//this is the header content + header check
static uint16_t v1_get_header_len(uint8_t* pbuf)
{
    return v1_HEAD_LEN;
}

static uint16_t v1_get_data_len(uint8_t* pbuf)
{
    v1_data_struct_t* p_v1_pack = (v1_data_struct_t*)pbuf;
    uint16_t all_pack_len = 0;
    all_pack_len = v1_HEAD_LEN + p_v1_pack->datalen + 2; //2 is the package check len
    return all_pack_len;
}

static uint8_t v1_check_header(uint8_t* pbuf)
{
    v1_data_struct_t* p_v1_pack = (v1_data_struct_t*)pbuf;
    if( (p_v1_pack->sof1 == v1_HEAD_SOF1) && \
            (p_v1_pack->sof2 == v1_HEAD_SOF2) && \
            (p_v1_pack->version == v1_VERSION)) {
        uint8_t check_cal_result;
        calc_check(pbuf, v1_HEAD_LEN - 1, CHECK_TYPE_8CRC, &check_cal_result);
        if (p_v1_pack->headcheck == check_cal_result) {
            return 1;
        }
    }
    return 0;
}

static uint8_t v1_check_data(uint8_t* pbuf)
{
    v1_data_struct_t* p_v1_pack = (v1_data_struct_t*)pbuf;
    uint16_t check;
    memcpy(&check, &pbuf[v1_HEAD_LEN + p_v1_pack->datalen], 2);
    if (CHECK_TYPE_16CRC == p_v1_pack->check_type) {
        uint16_t check_cal_result;
        calc_check(pbuf, v1_HEAD_LEN + p_v1_pack->datalen, CHECK_TYPE_16CRC, &check_cal_result);
        if (check == check_cal_result) {
            return 1;
        }
        return 0;
    }
    return 0;
}

static uint16_t  v1_pack_data	(uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info)
{
    uint8_t * pcheck_buf = NULL;
    uint16_t pack_len = 0;
    uint16_t check_cal_result;
    v1_data_struct_t* p_v1_pack     = (v1_data_struct_t*)p_raw_buf;

    p_v1_pack->sof1				    = v1_HEAD_SOF1;
    p_v1_pack->sof2				    = v1_HEAD_SOF2;
    p_v1_pack->need_ack		        = (p_frame_info->need_ack > 0) ? 1 : 0;
    p_v1_pack->is_ack			    = (p_frame_info->is_ack > 0)	? 1 : 0;
    p_v1_pack->check_type		    = p_frame_info->check_type ;
    p_v1_pack->enc_type		        = p_frame_info->enc_type	& (0x07);
    p_v1_pack->version			    = v1_VERSION;
    p_v1_pack->resv			        = 0;

    p_v1_pack->datalen				= p_frame_info->data_len;
    //------------------data
    p_v1_pack->seq					= p_frame_info->seq         & 0xff;
    p_v1_pack->cmdid				= p_frame_info->cmd_id      & 0xff ;
    p_v1_pack->cmdfunc				= p_frame_info->cmd_func    & 0xff;
    p_v1_pack->src					= p_frame_info->src         & 0xff;
    p_v1_pack->dest					= p_frame_info->dest        & 0xff;
    memcpy(p_v1_pack->pdata, p_frame_info->pdata,  p_frame_info->data_len);

    //----------------check
    pack_len = v1_HEAD_LEN + p_v1_pack->datalen + 2;
    pcheck_buf = &p_raw_buf[pack_len - 2];
    calc_check((uint8_t*)p_v1_pack, v1_HEAD_LEN - 1, CHECK_TYPE_8CRC, &p_v1_pack->headcheck );
    calc_check((uint8_t*)p_v1_pack, v1_HEAD_LEN + p_v1_pack->datalen, CHECK_TYPE_16CRC, &check_cal_result);
    memcpy(pcheck_buf, &check_cal_result, sizeof check_cal_result);
    return pack_len;
}


static uint16_t  v1_unpack_data	(uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info)
{
    v1_data_struct_t* p_v1_pack = (v1_data_struct_t*)p_raw_buf;

    //p_frame_info->link_id 			= 0; /*由上层去填充*/
    p_frame_info->enc_type			= p_v1_pack->enc_type;
    p_frame_info->check_type		= p_v1_pack->check_type;
    p_frame_info->cmd_id			= p_v1_pack->cmdid;
    p_frame_info->data_len			= p_v1_pack->datalen;

    p_frame_info->is_ack			= p_v1_pack->is_ack;
    p_frame_info->need_ack			= p_v1_pack->need_ack;

    p_frame_info->src				= p_v1_pack->src;
    p_frame_info->dest				= p_v1_pack->dest;
    p_frame_info->cmd_func			= p_v1_pack->cmdfunc;
    p_frame_info->seq				= p_v1_pack->seq;
    p_frame_info->pdata				= p_v1_pack->pdata;

    return 1;
}
