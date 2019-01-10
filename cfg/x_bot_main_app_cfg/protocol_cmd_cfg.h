#ifndef __PROTOCOL_CMD_CFG_H__
#define __PROTOCOL_CMD_CFG_H__

#define PROTOCOL_X_BOT     1
#define PROTOCOL_SBUST     2
#define PROTOCOL_IMUM      3

#define G_ADDR_BASE                0x00
#define P_ADDR_BASE                0x64
//global addr
#define PC_ADDR                    (G_ADDR_BASE + 0x1)
#define RC_BOARD_ADDR              (G_ADDR_BASE + 0x2)
#define RX_BOARD_ADDR              (G_ADDR_BASE + 0x3)
#define IO_BOARD_ADDR              (G_ADDR_BASE + 0x20)
#define NO_GADDR                   (G_ADDR_BASE + 0x63)     //invalid global addr

//private addr
///

//通用命令集 --- update  shell  cfg_param
#define COMM_CMD_SET                    0x1

    //----心跳包
    #define CMD_ID_HEART_BEAT           0x00
    #define CMD_ID_PING                 0x01
    #define CMD_ID_LINK_CFG             0x02
    #define CMD_ID_HW_INFO              0x03
    #define CMD_ID_SW_INFO              0x04
    #define CMD_ID_PRODUCT_INFO         0x05

    #define CMD_ID_RESET_DEV            0x06

    //查询是否在bootloader中
    #define ISP_CMD_ID_IS_BOOTING       0x07
    //在
    #define ISP_CMD_ID_GO               0x08
    #define ISP_CMD_ID_GO_ALL           0x09


    //----升级传输的子命令ID
    #define CMD_ID_UPDATE_PREPARE       0x10
    #define CMD_ID_UPDATE_START         0x11
    #define CMD_ID_UPDATE_TRANS         0x12
    #define CMD_ID_UPDATE_CHECK         0x13


    //----shell的子命令ID
    #define CMD_ID_SHELL_IN             0x21
    #define CMD_ID_SHELL_OUT            0x22


    //----参数配置的子命令ID
    #define CMD_ID_GET_PARAM_CNT        0x31
    #define CMD_ID_GET_PARAM_ITEM       0x32
    #define CMD_ID_WR_PARAM_ITEM        0x33
    #define CMD_ID_RD_PARAM_ITEM        0x34
    #define CMD_ID_RS_PARAM_ITEM        0x35
    #define CMD_ID_RS_ALL_PARAM_ITEM    0x36

//ucprobe -v2 ?üá??ˉ ---
#define UCPROBE_CMD_SET                         0xf0
    #define PROBE_COM_FMT_RX_QUERY              0x0001u   /* Query setup parameter or capability.                 */
    #define PROBE_COM_QUERY_MAX_RX_SIZE         0x0101u   /* Get data size of largest pkt target can rx.          */
    #define PROBE_COM_QUERY_MAX_TX_SIZE         0x0102u   /* Get data size of largest pkt target can tx.          */
                                                
    #define PROBE_COM_QUERY_FMT_SUPPORT         0x1001u   /* Get list of supported req's.                         */
    #define PROBE_COM_QUERY_ENDIANNESS_TEST     0x0201u   /* Test endianness of target.                           */
    #define PROBE_COM_QUERY_STATUS              0x0202u   /* Target status.     */
        
    #define PROBE_COM_FMT_RX_RD                 0x0002u   /* Query setup parameter or capability.                 */
    #define PROBE_COM_FMT_RX_WR                 0x0003u   /* Write data at memory address.                        */
    #define PROBE_COM_FMT_RX_RD_MULTI           0x0007u   /* Read data at multiple memory addresses.              */
        
    #define PROBE_COM_FMT_RX_WR_MULTI           0x0008u   /* Write data at multiple memory addresses.             */
    #define PROBE_COM_FMT_RX_STR_OUT            0x0009u   /* Transmit output string to Probe.                     */
    #define PROBE_COM_FMT_RX_STR_IN             0x000Au   /* Receive input string from Probe.                     */
        
    #define PROBE_COM_FMT_RX_TERMINAL_EXEC      0x000Bu   /* Execute terminal command.                            */
    #define PROBE_COM_FMT_RX_TERMINAL_OUT       0x000Cu   /* Transmit output data to Probe.                       */
    #define PROBE_COM_FMT_RX_TERMINAL_IN        0x000Du   /* Receive input data from Probe.                       */
    
//control command set   
#define CONTROL_CMD_SET                     0x02   
    #define BLOOD_BAR_INFO                  0x01          //include battery soc and blood percentage    
    #define CTR_CMD_ID_COM_TYPE_CHECK        0x03        //get the com type
    #define CTR_CMD_ID_JOYSTICK_DATA        0x04        //joystick data
    #define CTR_CMD_ID_CAR_INFO_SET         0x06
    #define CTR_CMD_ID_GET_REQ              0x07
    #define CTR_CMD_ID_BULLET_ADDING        0x0A
    #define CTR_CMD_ID_MOTOR_REVERSE        0x0B        //battle field motor reverse 
//rc command set    
#define SBUS_CMD_SET                        0x10
    #define SBUS_CHANNEL_DATA               0x00
//imu module command set                    
#define IMUM_CMD_SET                        0x11
    #define IMUM_ACC_DATA                   0x51
    #define IMUM_ANG_SPEED                  0x52
    #define IMUM_ANG                        0x53
    
#define CAN_CMD_SET                         0x12
    #define CAN_CTRL_DATA                   0x01
    #define CAN_WARNING_DATA                0x02
    #define CAN_WARNING_CLEAR               0x03
#endif //__PROTOCOL_CMD_CFG_H__
