#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "stm32f10x.h"                  // Device header
#include "x_frame.h"


#define DEFAULT_BUF_SIZE    128
#define DEF_STOP            USART_StopBits_1            
#define DEF_CHK             USART_Parity_No   

//#define USART_StopBits_1                     ((uint16_t)0x0000)
//#define USART_StopBits_0_5                   ((uint16_t)0x1000)
//#define USART_StopBits_2                     ((uint16_t)0x2000)
//#define USART_StopBits_1_5                   ((uint16_t)0x3000)


//#define USART_Parity_No                      ((uint16_t)0x0000)
//#define USART_Parity_Even                    ((uint16_t)0x0400)
//#define USART_Parity_Odd                     ((uint16_t)0x0600) 

//sdk
#define UART1_DEF_BAUD                  115200//460800//100000//921600            
#define UART1_IRQ                          5                            
#define UART1_TX_BUF_SIZE                  512//DEFAULT_BUF_SIZE                    
#define UART1_RX_BUF_SIZE                  DEFAULT_BUF_SIZE    
#define ENABLE_UARTS1_REMAP             0


//rc
#define UART2_DEF_BAUD                  115200//100000//115200            
#define UART2_IRQ                          9                            
#define UART2_TX_BUF_SIZE                  512                    
#define UART2_RX_BUF_SIZE                  DEFAULT_BUF_SIZE    
#define ENABLE_UARTS2_REMAP             1


//arm_ctrl
#define UART3_DEF_BAUD                  115200            
#define UART3_IRQ                          3                            
#define UART3_TX_BUF_SIZE                  DEFAULT_BUF_SIZE//DEFAULT_BUF_SIZE                    
#define UART3_RX_BUF_SIZE                  DEFAULT_BUF_SIZE    
#define ENABLE_UARTS3_REMAP             0


//IMU
#define UART4_DEF_BAUD                  115200            
#define UART4_IRQ                          1                            
#define UART4_TX_BUF_SIZE                  4                    
#define UART4_RX_BUF_SIZE                  128    

//dbg
#define UART5_DEF_BAUD                  115200            
#define UART5_IRQ                          6                            
#define UART5_TX_BUF_SIZE                  1024//DEFAULT_BUF_SIZE                    
#define UART5_RX_BUF_SIZE                  1024    




void bsp_uart_init          (uint8_t uart_port);
int32_t bsp_uart_send       (uint8_t uart_port,uint8_t* pbuf,int32_t len);
int32_t bsp_uart_read        (uint8_t uart_port,uint8_t* p_dest,int32_t len);
void  bsp_uart_set_mode        (uint8_t uart_port,uint32_t mode);
void bsp_uart_set(uint8_t uart_port,uint32_t baud, uint32_t data_bits,uint8_t is_wait_mod);

#endif //__BSP_UART_H__
