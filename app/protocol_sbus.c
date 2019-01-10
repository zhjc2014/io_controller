#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"

static uint16_t     sbus_get_header_len        (uint8_t* pbuf);
static uint16_t     sbus_get_data_len            (uint8_t* pbuf);
static uint8_t         sbus_check_header            (uint8_t* pbuf);
static uint8_t         sbus_check_data            (uint8_t* pbuf);
static uint16_t     sbus_pack_data            (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info);
static uint16_t     sbus_unpack_data            (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info);

//0F 00 04 20 00 01 08 40 00 02 10 80 00 04 20 00 01 08 40 00 02 10 80 00 00

//ADD_PROTOCOL(   PROTOCOL_SBUST,
//                sbus_get_header_len,
//                sbus_get_data_len,
//                sbus_check_header,
//                sbus_check_data,
//                sbus_pack_data,
//                sbus_unpack_data
//            ) ;

#define SBUS_CH_DATA (22)
#pragma pack(1)
typedef struct {
    uint8_t     sof;
    /*ctrl 1-------------*/
    uint8_t     ch_data[SBUS_CH_DATA];
    uint8_t     flag;
//    uint16_t    ch1:11;
//    uint16_t    ch2:11;
//    uint16_t    ch3:11;
//    uint16_t    ch4:11;
//    uint16_t    ch5:11;
//    uint16_t    ch6:11;
//    uint16_t    ch7:11;
//    uint16_t    ch8:11;
//    uint16_t    ch9:11;
//    uint16_t    ch10:11;
//    uint16_t    ch11:11;
//    uint16_t    ch12:11;
//    uint16_t    ch13:11;
//    uint16_t    ch14:11;
//    uint16_t    ch15:11;
//    uint16_t    ch16:11;
//    uint8_t     flag;

    uint8_t     chk;
} sbus_data_struct_t;
#pragma pack()
#define sbus_HEAD_SOF        (0x0f)
#define sbus_DATA_CHK        (0x00)

#define sbus_HEAD_HEN            (1)
#define sbus_EXTPACK_LEN    (sizeof(sbus_data_struct_t))


static uint16_t sbus_get_header_len(uint8_t* pbuf)
{
    return sbus_HEAD_HEN;
}


static uint16_t sbus_get_data_len(uint8_t* pbuf)
{
    return sbus_EXTPACK_LEN;
}


static uint8_t sbus_check_header(uint8_t* pbuf)
{
    sbus_data_struct_t* p_sbus_pack = (sbus_data_struct_t*)pbuf;
    if(p_sbus_pack->sof == sbus_HEAD_SOF) {
        return 1;
    }

    return 0;
}

static uint8_t sbus_check_data(uint8_t* pbuf)
{
    sbus_data_struct_t* p_sbus_pack = (sbus_data_struct_t*)pbuf;
    if(p_sbus_pack->chk == sbus_DATA_CHK) {
        return 1;
    }
    return 0;
}

static uint16_t  sbus_pack_data    (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info)
{
    return 0;
}


static uint16_t  sbus_unpack_data    (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info)
{
    sbus_data_struct_t* p_sbus_pack = (sbus_data_struct_t*)p_raw_buf;
    p_frame_info->enc_type            = 0;
    p_frame_info->check_type        = 0;
    p_frame_info->cmd_id            = SBUS_CHANNEL_DATA;
    p_frame_info->data_len            = SBUS_CH_DATA + 1;

    p_frame_info->cmd_func            = SBUS_CMD_SET;
    p_frame_info->pdata                = (uint8_t*)(&p_sbus_pack->ch_data);

    return 1;
}

