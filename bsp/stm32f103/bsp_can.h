#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#include <stdint.h>

#define CAN_FIFO_SIZE 16

//#pragma pack(1)
typedef struct {
    uint32_t id;
    uint32_t len;
    uint8_t data[8];
}can_data_t;
//#pragma pack()

void bsp_can_init(void);
int bsp_can_read(can_data_t *can_rxdata);
int bsp_can_write(can_data_t *can_txdata);
void bsp_can_start_send(void);

#endif

