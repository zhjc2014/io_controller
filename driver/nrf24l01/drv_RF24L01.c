/**
  ******************************************************************************
  * @author  泽耀科技 ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   NRF24L01配置C文件
  ******************************************************************************
  * @attention
  *
  * 官网	:	http://www.ashining.com
  * 淘宝	:	https://shop105912646.taobao.com
  * 阿里巴巴:	https://cdzeyao.1688.com
  ******************************************************************************
  */
#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"  
  
#include "drv_RF24L01.h"
#include "drv_delay.h"

typedef struct{
    int cnt;    //rev number
    uint8_t buf[MAX_PACKAGE_LEN];
}rf_rec_t;
    

const char *g_ErrorString = "RF24L01 is not find !...";
static rf_rec_t rf_rec[MAX_CHANNEL_NUM];
static uint8_t tx_rx_addr[MAX_CHANNEL_NUM][5] = {
    {0x00, DEFAULT_BASE_ADDR},
    {0x01, DEFAULT_BASE_ADDR},
    {0x02, DEFAULT_BASE_ADDR},
    {0x03, DEFAULT_BASE_ADDR},
    {0x04, DEFAULT_BASE_ADDR},
    {0x05, DEFAULT_BASE_ADDR}
};

/**
  * @brief :NRF24L01读寄存器
  * @param :
           @Addr:寄存器地址
  * @note  :地址在设备中有效
  * @retval:读取的数据
  */
uint8_t NRF24L01_Read_Reg( uint8_t RegAddr )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );			//片选
	
    drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	//读命令 地址
    btmp = drv_spi_read_write_byte( 0xFF );				//读数据
	
    RF24L01_SET_CS_HIGH( );			//取消片选
	
    return btmp;
}

/**
  * @brief :NRF24L01读指定长度的数据
  * @param :
  *			@reg:地址
  *			@pBuf:数据存放地址
  *			@len:数据长度
  * @note  :数据长度不超过255，地址在设备中有效
  * @retval:读取状态
  */
void NRF24L01_Read_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );			//片选
	
    drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	//读命令 地址
    for( btmp = 0; btmp < len; btmp ++ )
    {
        *( pBuf + btmp ) = drv_spi_read_write_byte( 0xFF );	//读数据
    }
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :NRF24L01写寄存器
  * @param :无
  * @note  :地址在设备中有效
  * @retval:读写状态
  */
void NRF24L01_Write_Reg( uint8_t RegAddr, uint8_t Value )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//写命令 地址
    drv_spi_read_write_byte( Value );			//写数据
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :NRF24L01写指定长度的数据
  * @param :
  *			@reg:地址
  *			@pBuf:写入的数据地址
  *			@len:数据长度
  * @note  :数据长度不超过255，地址在设备中有效
  * @retval:写状态
  */
void NRF24L01_Write_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t i;
	
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//写命令 地址
    for( i = 0; i < len; i ++ )
    {
        drv_spi_read_write_byte( *( pBuf + i ) );		//写数据
    }
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :清空TX缓冲区
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Flush_Tx_Fifo ( void )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( FLUSH_TX );	//清TX FIFO命令
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :清空RX缓冲区
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Flush_Rx_Fifo( void )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( FLUSH_RX );	//清RX FIFO命令
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :重新使用上一包数据
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Reuse_Tx_Payload( void )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( REUSE_TX_PL );		//重新使用上一包命令
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :NRF24L01空操作
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Nop( void )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( NOP );		//空操作命令
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :NRF24L01读状态寄存器
  * @param :无
  * @note  :无
  * @retval:RF24L01状态
  */
uint8_t NRF24L01_Read_Status_Register( void )
{
    uint8_t Status;
	
    RF24L01_SET_CS_LOW( );		//片选
	
    Status = drv_spi_read_write_byte( NRF_READ_REG + STATUS );	//读状态寄存器
	
    RF24L01_SET_CS_HIGH( );		//取消片选
	
    return Status;
}

/**
  * @brief :NRF24L01清中断
  * @param :
           @IRQ_Source:中断源
  * @note  :无
  * @retval:清除后状态寄存器的值
  */
uint8_t NRF24L01_Clear_IRQ_Flag( uint8_t IRQ_Source )
{
    uint8_t btmp = 0;

    IRQ_Source &= ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT );	//中断标志处理
    btmp = NRF24L01_Read_Status_Register( );			//读状态寄存器
			
    RF24L01_SET_CS_LOW( );			//片选
    drv_spi_read_write_byte( NRF_WRITE_REG + STATUS );	//写状态寄存器命令
    drv_spi_read_write_byte( IRQ_Source | btmp );		//清相应中断标志
    RF24L01_SET_CS_HIGH( );			//取消片选
	
    return ( NRF24L01_Read_Status_Register( ));			//返回状态寄存器状态
}

/**
  * @brief :读RF24L01中断状态
  * @param :无
  * @note  :无
  * @retval:中断状态
  */
uint8_t RF24L01_Read_IRQ_Status( void )
{
    return ( NRF24L01_Read_Status_Register( ) & (( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT )));	//返回中断状态
}
 
 /**
  * @brief :读FIFO中数据宽度
  * @param :无
  * @note  :无
  * @retval:数据宽度
  */
uint8_t NRF24L01_Read_Top_Fifo_Width( void )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( R_RX_PL_WID );	//读FIFO中数据宽度命令
    btmp = drv_spi_read_write_byte( 0xFF );	//读数据
	
    RF24L01_SET_CS_HIGH( );		//取消片选
	
    return btmp;
}

 /**
  * @brief :读接收到的数据
  * @param :无
  * @note  :无
  * @retval:
           @pRxBuf:数据存放地址首地址
  */
uint8_t NRF24L01_Read_Rx_Payload( uint8_t *pRxBuf )
{
    uint8_t Width, PipeNum;
	
    PipeNum = ( NRF24L01_Read_Reg( STATUS ) >> 1 ) & 0x07;	//读接收状态
    Width = NRF24L01_Read_Top_Fifo_Width( );		//读接收数据个数

    RF24L01_SET_CS_LOW( );		//片选
    drv_spi_read_write_byte( RD_RX_PLOAD );			//读有效数据命令
	
    for( PipeNum = 0; PipeNum < Width; PipeNum ++ )
    {
        *( pRxBuf + PipeNum ) = drv_spi_read_write_byte( 0xFF );		//读数据
    }
    RF24L01_SET_CS_HIGH( );		//取消片选
    NRF24L01_Flush_Rx_Fifo( );	//清空RX FIFO
	
    return Width;
}

 /**
  * @brief :发送数据（带应答）
  * @param :
  *			@pTxBuf:发送数据地址
  *			@len:长度
  * @note  :一次不超过32个字节
  * @retval:无
  */
void NRF24L01_Write_Tx_Payload_Ack( uint8_t *pTxBuf, uint8_t len )
{
    uint8_t btmp;
    uint8_t length = ( len > 32 ) ? 32 : len;		//数据长达大约32 则只发送32个

    NRF24L01_Flush_Tx_Fifo( );		//清TX FIFO
	
    RF24L01_SET_CS_LOW( );			//片选
    drv_spi_read_write_byte( WR_TX_PLOAD );	//发送命令
	
    for( btmp = 0; btmp < length; btmp ++ )
    {
        drv_spi_read_write_byte( *( pTxBuf + btmp ) );	//发送数据
    }
    RF24L01_SET_CS_HIGH( );			//取消片选
}

 /**
  * @brief :发送数据（不带应答）
  * @param :
  *			@pTxBuf:发送数据地址
  *			@len:长度
  * @note  :一次不超过32个字节
  * @retval:无
  */
void NRF24L01_Write_Tx_Payload_NoAck( uint8_t *pTxBuf, uint8_t len )
{
    if( len > 32 || len == 0 )
    {
        return ;		//数据长度大于32 或者等于0 不执行
    }
	
    RF24L01_SET_CS_LOW( );	//片选
    drv_spi_read_write_byte( WR_TX_PLOAD_NACK );	//发送命令
    while( len-- )
    {
        drv_spi_read_write_byte( *pTxBuf );			//发送数据
		pTxBuf++;
    }
    RF24L01_SET_CS_HIGH( );		//取消片选
}

 /**
  * @brief :在接收模式下向TX FIFO写数据(带ACK)
  * @param :
  *			@pData:数据地址
  *			@len:长度
  * @note  :一次不超过32个字节
  * @retval:无
  */
void NRF24L01_Write_Tx_Payload_InAck( uint8_t *pData, uint8_t len )
{
    uint8_t btmp;
	
	len = ( len > 32 ) ? 32 : len;		//数据长度大于32个则只写32个字节

    RF24L01_SET_CS_LOW( );			//片选
    drv_spi_read_write_byte( W_ACK_PLOAD );		//命令
    for( btmp = 0; btmp < len; btmp ++ )
    {
        drv_spi_read_write_byte( *( pData + btmp ) );	//写数据
    }
    RF24L01_SET_CS_HIGH( );			//取消片选
}

 /**
  * @brief :设置发送地址
  * @param :
  *			@pAddr:地址存放地址
  *			@len:长度
  * @note  :无
  * @retval:无
  */
void NRF24L01_Set_TxAddr( uint8_t *pAddr, uint8_t len )
{
	len = ( len > 5 ) ? 5 : len;					//地址不能大于5个字节
    NRF24L01_Write_Buf( TX_ADDR, pAddr, len );	//写地址
}

 /**
  * @brief :设置接收通道地址
  * @param :
  *			@PipeNum:通道
  *			@pAddr:地址存肥着地址
  *			@Len:长度
  * @note  :通道不大于5 地址长度不大于5个字节
  * @retval:无
  */
void NRF24L01_Set_RxAddr( uint8_t PipeNum, uint8_t *pAddr, uint8_t Len )
{
    Len = ( Len > 5 ) ? 5 : Len;
    PipeNum = ( PipeNum > 5 ) ? 5 : PipeNum;		//通道不大于5 地址长度不大于5个字节

    NRF24L01_Write_Buf( RX_ADDR_P0 + PipeNum, pAddr, Len );	//写入地址
}

 /**
  * @brief :设置通信速度
  * @param :
  *			@Speed:速度
  * @note  :无
  * @retval:无
  */
void NRF24L01_Set_Speed( nRf24l01SpeedType Speed )
{
	uint8_t btmp = 0;
	
	btmp = NRF24L01_Read_Reg( RF_SETUP );
	btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );
	
	if( Speed == SPEED_250K )		//250K
	{
		btmp |= ( 1<<5 );
	}
	else if( Speed == SPEED_1M )   //1M
	{
   		btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );
	}
	else if( Speed == SPEED_2M )   //2M
	{
		btmp |= ( 1<<3 );
	}

	NRF24L01_Write_Reg( RF_SETUP, btmp );
}

 /**
  * @brief :设置功率
  * @param :
  *			@Speed:速度
  * @note  :无
  * @retval:无
  */
void NRF24L01_Set_Power( nRf24l01PowerType Power )
{
    uint8_t btmp;
	
	btmp = NRF24L01_Read_Reg( RF_SETUP ) & ~0x07;
    switch( Power )
    {
        case POWER_F18DBM:
            btmp |= PWR_18DB;
            break;
        case POWER_F12DBM:
            btmp |= PWR_12DB;
            break;
        case POWER_F6DBM:
            btmp |= PWR_6DB;
            break;
        case POWER_0DBM:
            btmp |= PWR_0DB;
            break;
        default:
            break;
    }
    NRF24L01_Write_Reg( RF_SETUP, btmp );
}

 /**
  * @brief :设置频率
  * @param :
  *			@FreqPoint:频率设置参数
  * @note  :值不大于127
  * @retval:无
  */
void RF24LL01_Write_Hopping_Point( uint8_t FreqPoint )
{
    NRF24L01_Write_Reg(  RF_CH, FreqPoint & 0x7F );
//    DBG_I("%s" ,__FUNCTION__);
}

/**
  * @brief :NRF24L01检测
  * @param :无
  * @note  :无
  * @retval:无
  */ 
void NRF24L01_check( void )
{
	uint8_t buf[5]={ 0XA5, 0XA5, 0XA5, 0XA5, 0XA5 };
	uint8_t read_buf[ 5 ] = { 0 };
	 
    NRF24L01_Write_Buf( TX_ADDR, buf, 5 );			//写入5个字节的地址
    NRF24L01_Read_Buf( TX_ADDR, read_buf, 5 );		//读出写入的地址  
    if (memcmp(read_buf, buf, sizeof buf)){
        DBG_I("%s error", __FUNCTION__);
    }
}

 /**
  * @brief :设置模式
  * @param :
  *			@Mode:模式发送模式或接收模式
  * @note  :无
  * @retval:无
  */
void RF24L01_Set_Mode( nRf24l01ModeType Mode )
{
    uint8_t controlreg = 0;
	controlreg = NRF24L01_Read_Reg( CONFIG );
	
    if( Mode == MODE_TX )       
	{
		controlreg &= ~( 1<< PRIM_RX );
	}
    else 
	{
		if( Mode == MODE_RX )  
		{ 
			controlreg |= ( 1<< PRIM_RX ); 
		}
	}

    NRF24L01_Write_Reg( CONFIG, controlreg );
}

/**
  * @brief :NRF24L01发送一次数据
  * @param :
  *			@txbuf:待发送数据首地址
  *			@Length:发送数据长度
  * @note  :无
  * @retval:
  *			MAX_TX：达到最大重发次数
  *			TX_OK：发送完成
  *			0xFF:其他原因
  */ 
uint8_t NRF24L01_TxPacket( uint8_t *txbuf, uint8_t Length )
{
	uint8_t l_Status = 0;
	uint16_t l_MsTimes = 0;
	
	RF24L01_SET_CS_LOW( );		//片选
	drv_spi_read_write_byte( FLUSH_TX );
	RF24L01_SET_CS_HIGH( );
	
	RF24L01_SET_CE_LOW( );		
	NRF24L01_Write_Buf( WR_TX_PLOAD, txbuf, Length );	//写数据到TX BUF 32字节  TX_PLOAD_WIDTH
	RF24L01_SET_CE_HIGH( );			//启动发送
	while( 0 != RF24L01_GET_IRQ_STATUS( ))
	{
		drv_delay_ms( 1 );
		if( 500 == l_MsTimes++ )						//500ms还没有发送成功，重新初始化设备
		{
			NRF24L01_Gpio_Init( );
			RF24L01_Init( );
			RF24L01_Set_Mode( MODE_TX );
			break;
		}
	}
	l_Status = NRF24L01_Read_Reg(STATUS);						//读状态寄存器
	NRF24L01_Write_Reg( STATUS, l_Status );						//清除TX_DS或MAX_RT中断标志
	if (l_Status & RX_OK){  //received package when sending package
        NRF24L01_Write_Reg( FLUSH_RX,0xff );	                //清除TX FIFO寄存器
    }
	if( l_Status & MAX_TX )	//达到最大重发次数
	{
		NRF24L01_Write_Reg( FLUSH_TX,0xff );	                //清除TX FIFO寄存器
        NRF24L01_Write_Reg( FLUSH_RX,0xff );                    //clear rx fifo to avoid that can't receive data
		return MAX_TX; 
	}
	if( l_Status & TX_OK )	//发送完成
	{
        NRF24L01_Write_Reg( FLUSH_RX,0xff );
		return TX_OK;
	}
	NRF24L01_Write_Reg( FLUSH_RX,0xff );                        //clear rx fifo to avoid that can't receive data
	return 0xFF;	//其他原因发送失败
}

uint8_t NRF24L01_TxPacket_Channel(uint8_t channel, uint8_t *txbuf, uint8_t Length )
{
    uint8_t result;
    NRF24L01_Set_TxAddr( tx_rx_addr[channel], 5 );          //设置TX地址
    #if EN_ACK_WAIT
    NRF24L01_Set_RxAddr( 0, tx_rx_addr[channel], 5 );       //设置RX地址    for auto ack   
    #endif
    result = NRF24L01_TxPacket(txbuf, Length );
    #if EN_ACK_WAIT
    NRF24L01_Set_RxAddr( 0, tx_rx_addr[0], 5 );             //恢复通道0默认地址
    #endif
    return result;
}

uint8_t NRF24L01_RxAddr_Change(uint8_t channel)
{
    if (channel < MAX_CHANNEL_NUM){
        NRF24L01_Set_RxAddr( 0, tx_rx_addr[channel], 5 );   //设置RX地址
        return 0;
    }else{
        return 1;
    }
}


/**
  * @brief :NRF24L01接收数据
  * @param :
  *			@rxbuf:接收数据存放地址
  * @note  :无
  * @retval:接收的数据个数
  */ 
uint8_t NRF24L01_RxPacket( uint8_t *rxbuf )
{
	uint8_t l_Status = 0, l_RxLength = 0, l_100MsTimes = 0;
	
	RF24L01_SET_CS_LOW( );		//片选
	drv_spi_read_write_byte( FLUSH_RX );
	RF24L01_SET_CS_HIGH( );
	
	while( 0 != RF24L01_GET_IRQ_STATUS( ))
	{
		drv_delay_ms( 100 );
		
		if( 30 == l_100MsTimes++ )		//3s没接收过数据，重新初始化模块
		{
			NRF24L01_Gpio_Init( );
			RF24L01_Init( );
			RF24L01_Set_Mode( MODE_RX );
			break;
		}
	}
	
	l_Status = NRF24L01_Read_Reg( STATUS );		//读状态寄存器
	NRF24L01_Write_Reg( STATUS,l_Status );		//清中断标志
	if( l_Status & RX_OK)	//接收到数据
	{
		l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//读取接收到的数据个数
		NRF24L01_Read_Buf( RD_RX_PLOAD,rxbuf,l_RxLength );	//接收到数据 
		NRF24L01_Write_Reg( FLUSH_RX,0xff );				//清除RX FIFO
        DBG_I("rx channel num is %d", l_Status >> 1 & 0x07);
		return l_RxLength; 
	}	
	return 0;				//没有收到数据	
}
    
    /**
  * @brief :NRF24L01接收数据
  * @param :
  *			@rxbuf:接收数据存放地址
  * @note  :无
  * @retval:接收的数据个数
  */ 
void NRF24L01_RxPacket_int(void)
{
	uint8_t l_Status = 0, l_RxLength = 0;
	uint8_t pipeNum;
	l_Status = NRF24L01_Read_Reg( STATUS );		//读状态寄存器
	pipeNum = (l_Status >> 1) & 0x07;           //get the data pipe number
    NRF24L01_Write_Reg( STATUS,l_Status );		//清中断标志
	if( l_Status & RX_OK)	//接收到数据
	{
		l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//读取接收到的数据个数
		if (l_RxLength > MAX_PACKAGE_LEN){
			l_RxLength = MAX_PACKAGE_LEN;
		}
		if (pipeNum < MAX_CHANNEL_NUM){
			rf_rec[pipeNum].cnt = l_RxLength; 
			NRF24L01_Read_Buf( RD_RX_PLOAD,rf_rec[pipeNum].buf, rf_rec[pipeNum].cnt );	//接收到数据 	
		}
        NRF24L01_Write_Reg( FLUSH_RX,0xff );				//清除RX FIFO
        #if EN_ACK_WAIT
//        NRF24L01_Write_Reg( FLUSH_TX,0xff );                //clear the tx fifo
//        NRF24L01_Write_Tx_Payload_InAck(ackData, sizeof ackData);   //send ack payload
        #endif
	}else{
		if (pipeNum < MAX_CHANNEL_NUM){
			rf_rec[pipeNum].cnt = 0; //没有收到数据	
		}
    }
}


//return num
int get_nrf24l01_data(uint8_t* buf, uint8_t pipeNum)
{
    int cnt = 0;
    static int lost_cnt;
    if (pipeNum < MAX_CHANNEL_NUM){
        //DBG_I("data pipe is %d", pipeNum);
        //DBG_I("data len is %d", rf_rec[pipeNum].cnt);
        memcpy(buf, rf_rec[pipeNum].buf, rf_rec[pipeNum].cnt);
        cnt = rf_rec[pipeNum].cnt;
        rf_rec[pipeNum].cnt = 0;
        ////////////if no data received for 10 times, reset the chip
//        if (!cnt){
//            lost_cnt++;
//            if (lost_cnt > 10 * MAX_CHANNEL_NUM){
//				lost_cnt = 0;
//                RF24L01_Init( );
//                RF24L01_Set_Mode( MODE_RX ); 
//            }
//        }else{
//            lost_cnt = 0;
//        }
    } 
    return cnt;
}

 /**
  * @brief :RF24L01引脚初始化
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Gpio_Init( void )
{
	GPIO_InitTypeDef	RF24L01_GpioInitStructer;
	
    
	RCC_APB2PeriphClockCmd(RF24L01_POW_EN__CLK | RF24L01_CE_GPIO_CLK | RF24L01_IRQ_GPIO_CLK, ENABLE );	//?? CE IRQ????  CS SCK MISO MOSI?SPI?????
	//power pin config
	RF24L01_GpioInitStructer.GPIO_Mode = GPIO_Mode_Out_PP;
	RF24L01_GpioInitStructer.GPIO_Speed = GPIO_Speed_10MHz;
	RF24L01_GpioInitStructer.GPIO_Pin = RF24L01_POW_EN_PIN;
	GPIO_Init( RF24L01_POW_EN_PORT, &RF24L01_GpioInitStructer );
	//power on
	GPIO_SetBits( RF24L01_POW_EN_PORT, RF24L01_POW_EN_PIN);
	
	//ce pin config
	RF24L01_GpioInitStructer.GPIO_Mode = GPIO_Mode_Out_PP;
	RF24L01_GpioInitStructer.GPIO_Speed = GPIO_Speed_10MHz;
	RF24L01_GpioInitStructer.GPIO_Pin = RF24L01_CE_GPIO_PIN;
	GPIO_Init( RF24L01_CE_GPIO_PORT, &RF24L01_GpioInitStructer );
	//ce enable
	GPIO_SetBits( RF24L01_CE_GPIO_PORT, RF24L01_CE_GPIO_PIN);
	
	//irq pin config
	RF24L01_GpioInitStructer.GPIO_Mode = GPIO_Mode_IPU;
	RF24L01_GpioInitStructer.GPIO_Speed = GPIO_Speed_10MHz;
	RF24L01_GpioInitStructer.GPIO_Pin = RF24L01_IRQ_GPIO_PIN;
	GPIO_Init( RF24L01_IRQ_GPIO_PORT, &RF24L01_GpioInitStructer );
	//irq pin set
	GPIO_SetBits( RF24L01_IRQ_GPIO_PORT, RF24L01_IRQ_GPIO_PIN);
	
    
    //exti config
    {
        EXTI_InitTypeDef exti;
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, 1);
        /* Configure EXTI lines. */
        exti.EXTI_Mode    = EXTI_Mode_Interrupt;
        exti.EXTI_Trigger = EXTI_Trigger_Falling;
        exti.EXTI_LineCmd = ENABLE;
        exti.EXTI_Line    = EXTI_Line1;
        EXTI_Init(&exti);
        NVIC_SetPriority(EXTI1_IRQn, 2);
        NVIC_EnableIRQ(EXTI1_IRQn);
    }
    
	RF24L01_SET_CE_LOW( );		//??24L01
	RF24L01_SET_CS_HIGH( );		//??SPI??
}

static uint32_t uid[3];
static void get_uid(void)
{
    uid[0]=*(uint32_t*)(0x1ffff7e8);
    uid[1]=*(uint32_t*)(0x1ffff7ec);
    uid[2]=*(uint32_t*)(0x1ffff7f0);
}

 /**
  * @brief :RF24L01模块初始化
  * @param :无
  * @note  :无
  * @retval:无
  */
void RF24L01_Init( void )
{    
	RF24L01_SET_POWER_OFF();
	drv_delay_ms( 10 );
	RF24L01_SET_POWER_ON();
	drv_delay_ms( 100 );

    NRF24L01_Write_Reg( CONFIG, 0x00);      //set power down
    NRF24L01_Write_Reg( FLUSH_RX,0xff );    //flush rx fifo
    NRF24L01_Write_Reg( FLUSH_TX,0xff );    //flush tx fifo
    
    RF24L01_SET_CE_HIGH( );
    NRF24L01_Clear_IRQ_Flag( IRQ_ALL );
#if DYNAMIC_PACKET == 1

    NRF24L01_Write_Reg( DYNPD, (    1 << DPL_P5 | \
                                    1 << DPL_P4 | \
                                    1 << DPL_P3 | \
                                    1 << DPL_P2 | \
                                    1 << DPL_P1 | \
                                    1 << DPL_P0) ); 	//使能通道0动态数据长度
    NRF24L01_Write_Reg( FEATRUE, 0x07 );
    NRF24L01_Read_Reg( DYNPD );
    NRF24L01_Read_Reg( FEATRUE );
	
#elif DYNAMIC_PACKET == 0
    
    NRF24L01_Write_Reg( RX_PW_P0, FIXED_PACKET_LEN );	//固定数据长度
	
#endif	//DYNAMIC_PACKET

    NRF24L01_Write_Reg( CONFIG, /*( 1<<MASK_RX_DR ) |*/		//接收中断
                                      ( 1 << EN_CRC ) |     //使能CRC 
                                      ( 1 << CRCO   ) |     //CRC 2 字节
                                      ( 1 << PWR_UP ) );    //开启设备
    NRF24L01_Write_Reg( EN_RXADDR, (    1 << ERX_P0 | \
                                        1 << ERX_P1 | \
                                        1 << ERX_P2 | \
                                        1 << ERX_P3 | \
                                        1 << ERX_P4 | \
                                        1 << ERX_P5 ));		
    NRF24L01_Write_Reg( SETUP_AW, AW_5BYTES );     			//地址宽度 5个字节
#if EN_ACK_WAIT
    NRF24L01_Write_Reg( EN_AA, (    1 << ENAA_P0 | \
                                    1 << ENAA_P1 | \
                                    1 << ENAA_P2 | \
                                    1 << ENAA_P3 | \
                                    1 << ENAA_P4 | \
                                    1 << ENAA_P5) );        //通道0自动应答
    NRF24L01_Write_Reg( SETUP_RETR, ACK_TIMOUT_THR |
                        ( REPEAT_CNT & 0x0F ) );         	//重复等待时间 
#else
    NRF24L01_Write_Reg( SETUP_RETR, ARC_DISABLE);         	
#endif
    NRF24L01_Write_Reg( RF_CH, DEFAULT_CHANNEL);            //2.4G + xM
    NRF24L01_Write_Reg( RF_SETUP, 0x0e );                   //2Mb 0dBm    
    NRF24L01_Set_TxAddr( tx_rx_addr[0], 5 );
    NRF24L01_Set_RxAddr( 0, tx_rx_addr[0], 5 );             //set the channel 0 to be the default addr
    NRF24L01_Set_RxAddr( 1, tx_rx_addr[1], 5 );             //set the channel 1 to be the default addr
    NRF24L01_Set_RxAddr( 2, &tx_rx_addr[2][0], 1 );         //set the channel 2 to be the default addr
    NRF24L01_Set_RxAddr( 3, &tx_rx_addr[3][0], 1 );         //set the channel 3 to be the default addr
    NRF24L01_Set_RxAddr( 4, &tx_rx_addr[4][0], 1 );         //set the channel 4 to be the default addr
    NRF24L01_Set_RxAddr( 5, &tx_rx_addr[5][0], 1 );         //set the channel 5 to be the default addr
}

