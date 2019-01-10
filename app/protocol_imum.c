//#define DEF_DBG_I
//#define DEF_DBG_W
//#define DEF_DBG_E
//#include "debug.h"
//#include "x_frame.h"

//static uint16_t     imum_get_header_len            (uint8_t* pbuf);
//static uint16_t     imum_get_data_len            (uint8_t* pbuf);
//static uint8_t         imum_check_header            (uint8_t* pbuf);
//static uint8_t         imum_check_data                (uint8_t* pbuf);
//static uint16_t     imum_pack_data                (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info);
//static uint16_t     imum_unpack_data            (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info);

//ADD_PROTOCOL(   PROTOCOL_IMUM,
//                imum_get_header_len,
//                imum_get_data_len,
//                imum_check_header,
//                imum_check_data,
//                imum_pack_data,
//                imum_unpack_data
//            ) ;


//#pragma pack(1)
//typedef struct {
//    uint8_t     sof;                 //1 byte
//    uint8_t     cmdid;               //1 byte
//    uint8_t     pdata[8];            //1 byte
//    uint8_t     chk;                 //data check
//} imum_data_struct_t;
//#pragma pack()


//#define IMUM_HEAD_SOF                (0x55)
//#define IMUM_VERSION                (PROTOCOL_IMUM)
//#define IMUM_HEAD_HEN                (1)
//#define IMUM_EXTPACK_LEN            (11)
//#define IMUM_CHECK_TYPE             CHECK_TYPE_SUM

//static uint16_t imum_get_header_len(uint8_t* pbuf)
//{
//    return IMUM_HEAD_HEN;
//}

//static uint16_t imum_get_data_len(uint8_t* pbuf)
//{
//    return IMUM_EXTPACK_LEN;
//}

//static uint8_t imum_check_header(uint8_t* pbuf)
//{
//    imum_data_struct_t* p_imum_pack = (imum_data_struct_t*)pbuf;
//    if(p_imum_pack->sof == IMUM_HEAD_SOF && \
//        (p_imum_pack->cmdid == IMUM_ACC_DATA || \
//         p_imum_pack->cmdid == IMUM_ANG_SPEED || \
//         p_imum_pack->cmdid == IMUM_ANG)) {
//        return 1;
//    }
//    return 0;
//}

//static uint8_t imum_check_data(uint8_t* pbuf)
//{
//    uint8_t  *pbuf_check = NULL;
//    uint8_t chk_result;

//    pbuf_check = &pbuf[IMUM_EXTPACK_LEN - 1];

//    calc_check(pbuf, IMUM_EXTPACK_LEN - 1, IMUM_CHECK_TYPE, &chk_result);

//    if(pbuf_check[0] == chk_result) {
//        return 1;
//    }
//    return 0;
//}

//static uint16_t  imum_pack_data    (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info)
//{
//    uint32_t chk_result = 0;
//    uint8_t * pcheck_buf = NULL;
//    uint16_t pack_len = 0;
//    imum_data_struct_t* p_imum_pack = (imum_data_struct_t*)p_raw_buf;

//    p_imum_pack->sof = IMUM_HEAD_SOF;

//    //------------------data
//    p_imum_pack->cmdid                = p_frame_info->cmd_id;
//    memcpy(p_imum_pack->pdata, p_frame_info->pdata,  p_frame_info->data_len);

//    //----------------check
//    pack_len = IMUM_EXTPACK_LEN - 1;
//    calc_check(p_raw_buf, pack_len, IMUM_CHECK_TYPE, &chk_result);

//    pcheck_buf = &p_raw_buf[pack_len - 1];
//    pcheck_buf[0] = chk_result & 0xff;

//    return IMUM_EXTPACK_LEN;
//}


//static uint16_t  imum_unpack_data    (uint8_t* p_raw_buf, hal_frame_info_t* p_frame_info)
//{
//    imum_data_struct_t* p_IMU_pack = (imum_data_struct_t*)p_raw_buf;
//    p_frame_info->cmd_func          =   IMUM_CMD_SET;
//    p_frame_info->cmd_id            =   p_IMU_pack->cmdid;
//    p_frame_info->data_len            =   IMUM_EXTPACK_LEN - 3;
//    p_frame_info->pdata                =   p_IMU_pack->pdata;
//    return 1;
//}

//////////////////////////////protocol action start//////////////////

//typedef struct
//{
//    int16_t    ax;
//    int16_t    ay;
//    int16_t    az;
//    int16_t    temp;
//}imu_acc_info_t;

//typedef struct
//{
//    int16_t    ang_speed_x;
//    int16_t    ang_speed_y;
//    int16_t    ang_speed_z;
//    int16_t    temp;
//}imu_ang_speed_info_t;

//typedef struct
//{
//    int16_t    ang_x;
//    int16_t    ang_y;
//    int16_t    ang_z;
//    int16_t    temp;
//}imu_ang_info_t;

//imu_info_t g_imu_info;

//int32_t recv_acc (hal_frame_info_t* p_frame_info)
//{
//    //DBG_I("imu_link_entity  recv_acc!!");
//    imu_acc_info_t imu_acc_info;
//    memcpy(&imu_acc_info, p_frame_info->pdata, p_frame_info->data_len);
//    g_imu_info.ax        = (imu_acc_info.ax) * 16 * 9.8 / 32768.0f /*/ 10.0f*/;
//    g_imu_info.ay        = (imu_acc_info.ay) * 16 * 9.8 / 32768.0f /*/ 10.0f*/;
//    g_imu_info.az        = (imu_acc_info.az) * 16 * 9.8 / 32768.0f /*/ 10.0f*/;
//    return 0;
//}
//ADD_LINK_ACTION(IMUM_LINK_ID, IMUM_CMD_SET, IMUM_ACC_DATA, recv_acc);

//int32_t recv_ang_speed (hal_frame_info_t* p_frame_info)
//{
//    //DBG_I("imu_link_entity  recv_ang_speed!!");
//    imu_ang_speed_info_t imu_ang_speed_info;
//    memcpy(&imu_ang_speed_info,p_frame_info->pdata,p_frame_info->data_len);
//    g_imu_info.angle_speed_x        = (imu_ang_speed_info.ang_speed_x)*2000/32768.0f;
//    g_imu_info.angle_speed_y        = (imu_ang_speed_info.ang_speed_y)*2000/32768.0f;
//    g_imu_info.angle_speed_z        = (imu_ang_speed_info.ang_speed_z)*2000/32768.0f;
//    g_imu_info.temp                 = (imu_ang_speed_info.temp)/340.0f+36.53;
//    g_imu_info.angle_speed_x = (g_imu_info.angle_speed_x>2000.0f)?(g_imu_info.angle_speed_x-4000):g_imu_info.angle_speed_x;
//    g_imu_info.angle_speed_y = (g_imu_info.angle_speed_y>2000.0f)?(g_imu_info.angle_speed_y-4000):g_imu_info.angle_speed_y;
//    g_imu_info.angle_speed_z = (g_imu_info.angle_speed_z>2000.0f)?(g_imu_info.angle_speed_z-4000):g_imu_info.angle_speed_z;    
//    return 0;
//}
//ADD_LINK_ACTION(IMUM_LINK_ID, IMUM_CMD_SET, IMUM_ANG_SPEED, recv_ang_speed);

//int32_t recv_ang (hal_frame_info_t* p_frame_info)
//{
//    //DBG_I("imu_link_entity  recv_ang!!  len:%d ",p_frame_info->data_len);
//    imu_ang_info_t imu_ang_info;
//    memcpy(&imu_ang_info,p_frame_info->pdata,p_frame_info->data_len);
//    g_imu_info.angle_x        = (imu_ang_info.ang_x)*180/32768.0f;
//    g_imu_info.angle_y        = (imu_ang_info.ang_y)*180/32768.0f;
//    g_imu_info.angle_z        = (imu_ang_info.ang_z)*180/32768.0f;
//    g_imu_info.angle_x -=180.0;
//    g_imu_info.angle_y -=180.0;
//    g_imu_info.angle_z -=180.0;
//    return 0;
//}
//ADD_LINK_ACTION(IMUM_LINK_ID, IMUM_CMD_SET, IMUM_ANG, recv_ang);


//////////////////////////////protocol action end//////////////////

