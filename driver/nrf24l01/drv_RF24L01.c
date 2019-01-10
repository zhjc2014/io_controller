/**
  ******************************************************************************
  * @author  ��ҫ�Ƽ� ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   NRF24L01����C�ļ�
  ******************************************************************************
  * @attention
  *
  * ����	:	http://www.ashining.com
  * �Ա�	:	https://shop105912646.taobao.com
  * ����Ͱ�:	https://cdzeyao.1688.com
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
  * @brief :NRF24L01���Ĵ���
  * @param :
           @Addr:�Ĵ�����ַ
  * @note  :��ַ���豸����Ч
  * @retval:��ȡ������
  */
uint8_t NRF24L01_Read_Reg( uint8_t RegAddr )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );			//Ƭѡ
	
    drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	//������ ��ַ
    btmp = drv_spi_read_write_byte( 0xFF );				//������
	
    RF24L01_SET_CS_HIGH( );			//ȡ��Ƭѡ
	
    return btmp;
}

/**
  * @brief :NRF24L01��ָ�����ȵ�����
  * @param :
  *			@reg:��ַ
  *			@pBuf:���ݴ�ŵ�ַ
  *			@len:���ݳ���
  * @note  :���ݳ��Ȳ�����255����ַ���豸����Ч
  * @retval:��ȡ״̬
  */
void NRF24L01_Read_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );			//Ƭѡ
	
    drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	//������ ��ַ
    for( btmp = 0; btmp < len; btmp ++ )
    {
        *( pBuf + btmp ) = drv_spi_read_write_byte( 0xFF );	//������
    }
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :NRF24L01д�Ĵ���
  * @param :��
  * @note  :��ַ���豸����Ч
  * @retval:��д״̬
  */
void NRF24L01_Write_Reg( uint8_t RegAddr, uint8_t Value )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//д���� ��ַ
    drv_spi_read_write_byte( Value );			//д����
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :NRF24L01дָ�����ȵ�����
  * @param :
  *			@reg:��ַ
  *			@pBuf:д������ݵ�ַ
  *			@len:���ݳ���
  * @note  :���ݳ��Ȳ�����255����ַ���豸����Ч
  * @retval:д״̬
  */
void NRF24L01_Write_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t i;
	
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//д���� ��ַ
    for( i = 0; i < len; i ++ )
    {
        drv_spi_read_write_byte( *( pBuf + i ) );		//д����
    }
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :���TX������
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Flush_Tx_Fifo ( void )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( FLUSH_TX );	//��TX FIFO����
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :���RX������
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Flush_Rx_Fifo( void )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( FLUSH_RX );	//��RX FIFO����
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :����ʹ����һ������
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Reuse_Tx_Payload( void )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( REUSE_TX_PL );		//����ʹ����һ������
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :NRF24L01�ղ���
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Nop( void )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( NOP );		//�ղ�������
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :NRF24L01��״̬�Ĵ���
  * @param :��
  * @note  :��
  * @retval:RF24L01״̬
  */
uint8_t NRF24L01_Read_Status_Register( void )
{
    uint8_t Status;
	
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    Status = drv_spi_read_write_byte( NRF_READ_REG + STATUS );	//��״̬�Ĵ���
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
	
    return Status;
}

/**
  * @brief :NRF24L01���ж�
  * @param :
           @IRQ_Source:�ж�Դ
  * @note  :��
  * @retval:�����״̬�Ĵ�����ֵ
  */
uint8_t NRF24L01_Clear_IRQ_Flag( uint8_t IRQ_Source )
{
    uint8_t btmp = 0;

    IRQ_Source &= ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT );	//�жϱ�־����
    btmp = NRF24L01_Read_Status_Register( );			//��״̬�Ĵ���
			
    RF24L01_SET_CS_LOW( );			//Ƭѡ
    drv_spi_read_write_byte( NRF_WRITE_REG + STATUS );	//д״̬�Ĵ�������
    drv_spi_read_write_byte( IRQ_Source | btmp );		//����Ӧ�жϱ�־
    RF24L01_SET_CS_HIGH( );			//ȡ��Ƭѡ
	
    return ( NRF24L01_Read_Status_Register( ));			//����״̬�Ĵ���״̬
}

/**
  * @brief :��RF24L01�ж�״̬
  * @param :��
  * @note  :��
  * @retval:�ж�״̬
  */
uint8_t RF24L01_Read_IRQ_Status( void )
{
    return ( NRF24L01_Read_Status_Register( ) & (( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT )));	//�����ж�״̬
}
 
 /**
  * @brief :��FIFO�����ݿ��
  * @param :��
  * @note  :��
  * @retval:���ݿ��
  */
uint8_t NRF24L01_Read_Top_Fifo_Width( void )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( R_RX_PL_WID );	//��FIFO�����ݿ������
    btmp = drv_spi_read_write_byte( 0xFF );	//������
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
	
    return btmp;
}

 /**
  * @brief :�����յ�������
  * @param :��
  * @note  :��
  * @retval:
           @pRxBuf:���ݴ�ŵ�ַ�׵�ַ
  */
uint8_t NRF24L01_Read_Rx_Payload( uint8_t *pRxBuf )
{
    uint8_t Width, PipeNum;
	
    PipeNum = ( NRF24L01_Read_Reg( STATUS ) >> 1 ) & 0x07;	//������״̬
    Width = NRF24L01_Read_Top_Fifo_Width( );		//���������ݸ���

    RF24L01_SET_CS_LOW( );		//Ƭѡ
    drv_spi_read_write_byte( RD_RX_PLOAD );			//����Ч��������
	
    for( PipeNum = 0; PipeNum < Width; PipeNum ++ )
    {
        *( pRxBuf + PipeNum ) = drv_spi_read_write_byte( 0xFF );		//������
    }
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
    NRF24L01_Flush_Rx_Fifo( );	//���RX FIFO
	
    return Width;
}

 /**
  * @brief :�������ݣ���Ӧ��
  * @param :
  *			@pTxBuf:�������ݵ�ַ
  *			@len:����
  * @note  :һ�β�����32���ֽ�
  * @retval:��
  */
void NRF24L01_Write_Tx_Payload_Ack( uint8_t *pTxBuf, uint8_t len )
{
    uint8_t btmp;
    uint8_t length = ( len > 32 ) ? 32 : len;		//���ݳ����Լ32 ��ֻ����32��

    NRF24L01_Flush_Tx_Fifo( );		//��TX FIFO
	
    RF24L01_SET_CS_LOW( );			//Ƭѡ
    drv_spi_read_write_byte( WR_TX_PLOAD );	//��������
	
    for( btmp = 0; btmp < length; btmp ++ )
    {
        drv_spi_read_write_byte( *( pTxBuf + btmp ) );	//��������
    }
    RF24L01_SET_CS_HIGH( );			//ȡ��Ƭѡ
}

 /**
  * @brief :�������ݣ�����Ӧ��
  * @param :
  *			@pTxBuf:�������ݵ�ַ
  *			@len:����
  * @note  :һ�β�����32���ֽ�
  * @retval:��
  */
void NRF24L01_Write_Tx_Payload_NoAck( uint8_t *pTxBuf, uint8_t len )
{
    if( len > 32 || len == 0 )
    {
        return ;		//���ݳ��ȴ���32 ���ߵ���0 ��ִ��
    }
	
    RF24L01_SET_CS_LOW( );	//Ƭѡ
    drv_spi_read_write_byte( WR_TX_PLOAD_NACK );	//��������
    while( len-- )
    {
        drv_spi_read_write_byte( *pTxBuf );			//��������
		pTxBuf++;
    }
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

 /**
  * @brief :�ڽ���ģʽ����TX FIFOд����(��ACK)
  * @param :
  *			@pData:���ݵ�ַ
  *			@len:����
  * @note  :һ�β�����32���ֽ�
  * @retval:��
  */
void NRF24L01_Write_Tx_Payload_InAck( uint8_t *pData, uint8_t len )
{
    uint8_t btmp;
	
	len = ( len > 32 ) ? 32 : len;		//���ݳ��ȴ���32����ֻд32���ֽ�

    RF24L01_SET_CS_LOW( );			//Ƭѡ
    drv_spi_read_write_byte( W_ACK_PLOAD );		//����
    for( btmp = 0; btmp < len; btmp ++ )
    {
        drv_spi_read_write_byte( *( pData + btmp ) );	//д����
    }
    RF24L01_SET_CS_HIGH( );			//ȡ��Ƭѡ
}

 /**
  * @brief :���÷��͵�ַ
  * @param :
  *			@pAddr:��ַ��ŵ�ַ
  *			@len:����
  * @note  :��
  * @retval:��
  */
void NRF24L01_Set_TxAddr( uint8_t *pAddr, uint8_t len )
{
	len = ( len > 5 ) ? 5 : len;					//��ַ���ܴ���5���ֽ�
    NRF24L01_Write_Buf( TX_ADDR, pAddr, len );	//д��ַ
}

 /**
  * @brief :���ý���ͨ����ַ
  * @param :
  *			@PipeNum:ͨ��
  *			@pAddr:��ַ����ŵ�ַ
  *			@Len:����
  * @note  :ͨ��������5 ��ַ���Ȳ�����5���ֽ�
  * @retval:��
  */
void NRF24L01_Set_RxAddr( uint8_t PipeNum, uint8_t *pAddr, uint8_t Len )
{
    Len = ( Len > 5 ) ? 5 : Len;
    PipeNum = ( PipeNum > 5 ) ? 5 : PipeNum;		//ͨ��������5 ��ַ���Ȳ�����5���ֽ�

    NRF24L01_Write_Buf( RX_ADDR_P0 + PipeNum, pAddr, Len );	//д���ַ
}

 /**
  * @brief :����ͨ���ٶ�
  * @param :
  *			@Speed:�ٶ�
  * @note  :��
  * @retval:��
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
  * @brief :���ù���
  * @param :
  *			@Speed:�ٶ�
  * @note  :��
  * @retval:��
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
  * @brief :����Ƶ��
  * @param :
  *			@FreqPoint:Ƶ�����ò���
  * @note  :ֵ������127
  * @retval:��
  */
void RF24LL01_Write_Hopping_Point( uint8_t FreqPoint )
{
    NRF24L01_Write_Reg(  RF_CH, FreqPoint & 0x7F );
//    DBG_I("%s" ,__FUNCTION__);
}

/**
  * @brief :NRF24L01���
  * @param :��
  * @note  :��
  * @retval:��
  */ 
void NRF24L01_check( void )
{
	uint8_t buf[5]={ 0XA5, 0XA5, 0XA5, 0XA5, 0XA5 };
	uint8_t read_buf[ 5 ] = { 0 };
	 
    NRF24L01_Write_Buf( TX_ADDR, buf, 5 );			//д��5���ֽڵĵ�ַ
    NRF24L01_Read_Buf( TX_ADDR, read_buf, 5 );		//����д��ĵ�ַ  
    if (memcmp(read_buf, buf, sizeof buf)){
        DBG_I("%s error", __FUNCTION__);
    }
}

 /**
  * @brief :����ģʽ
  * @param :
  *			@Mode:ģʽ����ģʽ�����ģʽ
  * @note  :��
  * @retval:��
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
  * @brief :NRF24L01����һ������
  * @param :
  *			@txbuf:�����������׵�ַ
  *			@Length:�������ݳ���
  * @note  :��
  * @retval:
  *			MAX_TX���ﵽ����ط�����
  *			TX_OK���������
  *			0xFF:����ԭ��
  */ 
uint8_t NRF24L01_TxPacket( uint8_t *txbuf, uint8_t Length )
{
	uint8_t l_Status = 0;
	uint16_t l_MsTimes = 0;
	
	RF24L01_SET_CS_LOW( );		//Ƭѡ
	drv_spi_read_write_byte( FLUSH_TX );
	RF24L01_SET_CS_HIGH( );
	
	RF24L01_SET_CE_LOW( );		
	NRF24L01_Write_Buf( WR_TX_PLOAD, txbuf, Length );	//д���ݵ�TX BUF 32�ֽ�  TX_PLOAD_WIDTH
	RF24L01_SET_CE_HIGH( );			//��������
	while( 0 != RF24L01_GET_IRQ_STATUS( ))
	{
		drv_delay_ms( 1 );
		if( 500 == l_MsTimes++ )						//500ms��û�з��ͳɹ������³�ʼ���豸
		{
			NRF24L01_Gpio_Init( );
			RF24L01_Init( );
			RF24L01_Set_Mode( MODE_TX );
			break;
		}
	}
	l_Status = NRF24L01_Read_Reg(STATUS);						//��״̬�Ĵ���
	NRF24L01_Write_Reg( STATUS, l_Status );						//���TX_DS��MAX_RT�жϱ�־
	if (l_Status & RX_OK){  //received package when sending package
        NRF24L01_Write_Reg( FLUSH_RX,0xff );	                //���TX FIFO�Ĵ���
    }
	if( l_Status & MAX_TX )	//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg( FLUSH_TX,0xff );	                //���TX FIFO�Ĵ���
        NRF24L01_Write_Reg( FLUSH_RX,0xff );                    //clear rx fifo to avoid that can't receive data
		return MAX_TX; 
	}
	if( l_Status & TX_OK )	//�������
	{
        NRF24L01_Write_Reg( FLUSH_RX,0xff );
		return TX_OK;
	}
	NRF24L01_Write_Reg( FLUSH_RX,0xff );                        //clear rx fifo to avoid that can't receive data
	return 0xFF;	//����ԭ����ʧ��
}

uint8_t NRF24L01_TxPacket_Channel(uint8_t channel, uint8_t *txbuf, uint8_t Length )
{
    uint8_t result;
    NRF24L01_Set_TxAddr( tx_rx_addr[channel], 5 );          //����TX��ַ
    #if EN_ACK_WAIT
    NRF24L01_Set_RxAddr( 0, tx_rx_addr[channel], 5 );       //����RX��ַ    for auto ack   
    #endif
    result = NRF24L01_TxPacket(txbuf, Length );
    #if EN_ACK_WAIT
    NRF24L01_Set_RxAddr( 0, tx_rx_addr[0], 5 );             //�ָ�ͨ��0Ĭ�ϵ�ַ
    #endif
    return result;
}

uint8_t NRF24L01_RxAddr_Change(uint8_t channel)
{
    if (channel < MAX_CHANNEL_NUM){
        NRF24L01_Set_RxAddr( 0, tx_rx_addr[channel], 5 );   //����RX��ַ
        return 0;
    }else{
        return 1;
    }
}


/**
  * @brief :NRF24L01��������
  * @param :
  *			@rxbuf:�������ݴ�ŵ�ַ
  * @note  :��
  * @retval:���յ����ݸ���
  */ 
uint8_t NRF24L01_RxPacket( uint8_t *rxbuf )
{
	uint8_t l_Status = 0, l_RxLength = 0, l_100MsTimes = 0;
	
	RF24L01_SET_CS_LOW( );		//Ƭѡ
	drv_spi_read_write_byte( FLUSH_RX );
	RF24L01_SET_CS_HIGH( );
	
	while( 0 != RF24L01_GET_IRQ_STATUS( ))
	{
		drv_delay_ms( 100 );
		
		if( 30 == l_100MsTimes++ )		//3sû���չ����ݣ����³�ʼ��ģ��
		{
			NRF24L01_Gpio_Init( );
			RF24L01_Init( );
			RF24L01_Set_Mode( MODE_RX );
			break;
		}
	}
	
	l_Status = NRF24L01_Read_Reg( STATUS );		//��״̬�Ĵ���
	NRF24L01_Write_Reg( STATUS,l_Status );		//���жϱ�־
	if( l_Status & RX_OK)	//���յ�����
	{
		l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//��ȡ���յ������ݸ���
		NRF24L01_Read_Buf( RD_RX_PLOAD,rxbuf,l_RxLength );	//���յ����� 
		NRF24L01_Write_Reg( FLUSH_RX,0xff );				//���RX FIFO
        DBG_I("rx channel num is %d", l_Status >> 1 & 0x07);
		return l_RxLength; 
	}	
	return 0;				//û���յ�����	
}
    
    /**
  * @brief :NRF24L01��������
  * @param :
  *			@rxbuf:�������ݴ�ŵ�ַ
  * @note  :��
  * @retval:���յ����ݸ���
  */ 
void NRF24L01_RxPacket_int(void)
{
	uint8_t l_Status = 0, l_RxLength = 0;
	uint8_t pipeNum;
	l_Status = NRF24L01_Read_Reg( STATUS );		//��״̬�Ĵ���
	pipeNum = (l_Status >> 1) & 0x07;           //get the data pipe number
    NRF24L01_Write_Reg( STATUS,l_Status );		//���жϱ�־
	if( l_Status & RX_OK)	//���յ�����
	{
		l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//��ȡ���յ������ݸ���
		if (l_RxLength > MAX_PACKAGE_LEN){
			l_RxLength = MAX_PACKAGE_LEN;
		}
		if (pipeNum < MAX_CHANNEL_NUM){
			rf_rec[pipeNum].cnt = l_RxLength; 
			NRF24L01_Read_Buf( RD_RX_PLOAD,rf_rec[pipeNum].buf, rf_rec[pipeNum].cnt );	//���յ����� 	
		}
        NRF24L01_Write_Reg( FLUSH_RX,0xff );				//���RX FIFO
        #if EN_ACK_WAIT
//        NRF24L01_Write_Reg( FLUSH_TX,0xff );                //clear the tx fifo
//        NRF24L01_Write_Tx_Payload_InAck(ackData, sizeof ackData);   //send ack payload
        #endif
	}else{
		if (pipeNum < MAX_CHANNEL_NUM){
			rf_rec[pipeNum].cnt = 0; //û���յ�����	
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
  * @brief :RF24L01���ų�ʼ��
  * @param :��
  * @note  :��
  * @retval:��
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
  * @brief :RF24L01ģ���ʼ��
  * @param :��
  * @note  :��
  * @retval:��
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
                                    1 << DPL_P0) ); 	//ʹ��ͨ��0��̬���ݳ���
    NRF24L01_Write_Reg( FEATRUE, 0x07 );
    NRF24L01_Read_Reg( DYNPD );
    NRF24L01_Read_Reg( FEATRUE );
	
#elif DYNAMIC_PACKET == 0
    
    NRF24L01_Write_Reg( RX_PW_P0, FIXED_PACKET_LEN );	//�̶����ݳ���
	
#endif	//DYNAMIC_PACKET

    NRF24L01_Write_Reg( CONFIG, /*( 1<<MASK_RX_DR ) |*/		//�����ж�
                                      ( 1 << EN_CRC ) |     //ʹ��CRC 
                                      ( 1 << CRCO   ) |     //CRC 2 �ֽ�
                                      ( 1 << PWR_UP ) );    //�����豸
    NRF24L01_Write_Reg( EN_RXADDR, (    1 << ERX_P0 | \
                                        1 << ERX_P1 | \
                                        1 << ERX_P2 | \
                                        1 << ERX_P3 | \
                                        1 << ERX_P4 | \
                                        1 << ERX_P5 ));		
    NRF24L01_Write_Reg( SETUP_AW, AW_5BYTES );     			//��ַ��� 5���ֽ�
#if EN_ACK_WAIT
    NRF24L01_Write_Reg( EN_AA, (    1 << ENAA_P0 | \
                                    1 << ENAA_P1 | \
                                    1 << ENAA_P2 | \
                                    1 << ENAA_P3 | \
                                    1 << ENAA_P4 | \
                                    1 << ENAA_P5) );        //ͨ��0�Զ�Ӧ��
    NRF24L01_Write_Reg( SETUP_RETR, ACK_TIMOUT_THR |
                        ( REPEAT_CNT & 0x0F ) );         	//�ظ��ȴ�ʱ�� 
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

