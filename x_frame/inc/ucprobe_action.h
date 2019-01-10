#ifndef __UCPROBE_ACTION_H__
#define __UCPROBE_ACTION_H__

#include "x_frame.h"




int32_t         uc_probe_query_RX_MAX       (hal_frame_info_t* p_frame_info);
int32_t         uc_probe_query_RX_RD       (hal_frame_info_t* p_frame_info);
int32_t         uc_probe_query_RX_WR       (hal_frame_info_t* p_frame_info);
int32_t         uc_probe_query_RX_RD_MULTI       (hal_frame_info_t* p_frame_info);



#define MAX_TRANS_BYTES  128


#define  PROBE_COM_FMT_RX_QUERY                       0x0001u   /* Query setup parameter or capability.                 */
                                                                        /* ------------------- CONFIGURATION ------------------ */
    #define  PROBE_COM_QUERY_MAX_RX_SIZE                  0x0101u   /* Get data size of largest pkt target can rx.          */
    #define  PROBE_COM_QUERY_MAX_TX_SIZE                  0x0102u   /* Get data size of largest pkt target can tx.          */

 	#define  PROBE_COM_QUERY_FMT_SUPPORT                  0x1001u   /* Get list of supported req's.                         */
    #define  PROBE_COM_QUERY_ENDIANNESS_TEST              0x0201u   /* Test endianness of target.                           */
    #define  PROBE_COM_QUERY_STATUS                       0x0202u   /* Target status.     */
   

#define  PROBE_COM_FMT_RX_RD                                0x0002u   /* Query setup parameter or capability.                 */
#define  PROBE_COM_FMT_RX_WR                          		0x0003u   /* Write data at memory address.                        */
#define  PROBE_COM_FMT_RX_RD_MULTI                          0x0007u   /* Read data at multiple memory addresses.              */

#define  PROBE_COM_FMT_RX_WR_MULTI                    		0x0008u   /* Write data at multiple memory addresses.             */
#define  PROBE_COM_FMT_RX_STR_OUT                     		0x0009u   /* Transmit output string to Probe.                     */
#define  PROBE_COM_FMT_RX_STR_IN                      		0x000Au   /* Receive input string from Probe.                     */

#define  PROBE_COM_FMT_RX_TERMINAL_EXEC               		0x000Bu   /* Execute terminal command.                            */
#define  PROBE_COM_FMT_RX_TERMINAL_OUT                		0x000Cu   /* Transmit output data to Probe.                       */
#define  PROBE_COM_FMT_RX_TERMINAL_IN                 		0x000Du   /* Receive input data from Probe.                       */

#pragma pack(1)
typedef struct
{
    uint8_t     status;
    uint8_t     modify;
    uint16_t    max_rx_size;
    uint16_t    resv;
}max_rx_size_ack_t;
    
typedef struct
{
    uint8_t     status;
    uint8_t     modify;
    uint32_t    end_test;
}end_test_ack_t;

    
typedef struct
{
    uint8_t     status;
    uint8_t     modify;
    uint16_t    support_rx_query;
	uint16_t    support_rx_rd;
	uint16_t    support_rx_rd_multi;
	uint16_t    support_rx_wr;
	uint16_t    support_rx_wr_multi;
	uint16_t    support_rx_str_in;
	uint16_t    support_rx_str_out;
	uint16_t    support_rx_terminal_exe;
	uint16_t    support_rx_terminal_in;
	uint16_t    support_rx_terminal_out;
}fmt_support_ack_t;



typedef struct
{
    uint16_t     read_bytes;
    uint32_t     start_addr;
}read_req_t;

typedef struct
{
    uint8_t     status;
    uint8_t     modify;
    uint8_t     read_dat[MAX_TRANS_BYTES];
}read_ack_t;


typedef struct
{
    uint16_t     write_bytes;
    uint32_t     start_addr;
    uint8_t      write_dat[MAX_TRANS_BYTES];
}write_req_t;

typedef struct
{
    uint8_t     status;
    uint8_t     modify;
}write_ack_t;





typedef struct
{
    uint8_t      read_bytes;
    uint32_t     start_addr;
}read_multi_item_t;

typedef struct
{
    read_multi_item_t read_item[64];
}read_multi_req_t;

typedef struct
{
    uint8_t     status;
    uint8_t     modify;
    uint8_t     read_dat[MAX_TRANS_BYTES];
}read_multi_ack_t;



//shell exe
typedef struct
{
	uint16_t	resv;
	uint8_t		*pada;
}shell_exe_req_t;

typedef struct
{
	uint8_t     status;
    uint8_t     modify;
}shell_exe_ack_t;


//shell in
typedef struct
{
	uint16_t	resv;
	uint8_t		*pada;
}shell_in_req_t;

typedef struct
{
	uint8_t     status;
    uint8_t     modify;
}shell_in_ack_t;


//shell out
typedef struct
{
	uint8_t     status;
    uint8_t     modify;
}shell_out_req_t;


typedef struct
{
	uint8_t     status;
    uint8_t     modify;
	uint8_t*	pdata;
}shell_out_ack_t;
#pragma pack()

#endif //__UCPROBE_ACTION_H__
