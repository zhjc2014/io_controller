#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_flash.h"

#define FLASH_PAGE_SIZE  FLASH_BLOCK_SIZE
#define FLASH_MIN_ADDR   MIN_FLASH_ADDRESS
#define FLASH_MAX_ADDR   MAX_FLASH_ADDRESS

FLASH_Status FLASHStatus = FLASH_COMPLETE;

int32_t bsp_flash_write       (uint32_t addr,uint8_t* pbuf,int32_t len)
{
    uint32_t DATA_32 = 0;
    uint32_t i = 0;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
    

    while (i < len)
    {
        if(len>(i+4)) { memcpy(&DATA_32,&pbuf[i],sizeof(uint32_t))  ; }
        else          { memcpy(&DATA_32,&pbuf[i],(len-i) ); }
        
        if (FLASH_ProgramWord(addr+i, DATA_32) == FLASH_COMPLETE)
        {
          i+=4;
        }
        else
        { 
          /* Error occurred while writing data in Flash memory. User can add here some code to deal with this error */
            DBG_E("bsp_flash_write error:0x%x ",addr+i);
            while (1);
        }
    }

    /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    FLASH_Lock(); 
    return len;
}

int32_t bsp_flash_read		 (uint32_t addr,uint8_t* p_dest,int32_t len)
{
    
    memcpy(p_dest, (void *)addr, len);
    return len;
}

int32_t bsp_flash_erase		 (uint32_t address,uint32_t len)
{
    uint32_t EraseCounter = 0;
    uint32_t NbrOfPage = 0;
    
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
    
    
    NbrOfPage = (len) / FLASH_PAGE_SIZE;
    if(len&(FLASH_PAGE_SIZE-1)){NbrOfPage++;}/*如果字节数大于当前块数*/
    
    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        if (FLASH_ErasePage(address + (FLASH_PAGE_SIZE * EraseCounter))!= FLASH_COMPLETE)
        {
         /* Error occurred while sector erase. User can add here some code to deal with this error  */
            DBG_E("bsp_flash_erase error : address:0x%x ",address + (FLASH_PAGE_SIZE * EraseCounter));
            while (1);
        }
    }
    FLASH_Lock(); 
    return 0;
}

#if 0
int32_t flash_w(uint8_t argc, uint8_t *argv[])
{
    uint32_t wr_addr = 0;
    uint32_t len  = 0;
    uint32_t read_len = 0;
    uint32_t read_bytes = 0;
    uint8_t read_buf[100]={0};
    
    if(argc>=3)
    {
        sscanf((const char*)argv[1],"0x%x",&wr_addr);
        len = strlen((const char*)argv[2]); 
        
        DBG_I("flash_w address:0x%x  len:%d",wr_addr,len);
        bsp_flash_erase(wr_addr,len);
        bsp_flash_write(wr_addr,(uint8_t*)argv[2],len+1);
        
        read_len = (len<99)?(len):(99);
        read_bytes = bsp_flash_read(wr_addr,read_buf,read_len);
        DBG_I("flash read:0x%x len:%d str:%s",wr_addr,read_bytes,(char*)read_buf);
    }     
    else
    {
        DBG_W("flash_w param error! eg: flash_w addr str");
    }
	return RET_OK;
}

REG_SHELL_CMD(flash_w,0,flash_w,"flash write!!", "eg:flash_w addr str \r\n ");


int32_t flash_r(uint8_t argc, uint8_t *argv[])
{
    uint32_t rd_addr = 0;
    uint32_t len  = 0;
    uint32_t read_len = 0;
    uint32_t read_bytes = 0;
    uint8_t read_buf[100]={0};
    
    if(argc>=3)
    {
        sscanf((const char*)argv[1],"0x%x",&rd_addr);
        sscanf((const char*)argv[2],"%d"  ,&len);
        
        read_len = (len<99)?(len):(99);
        read_bytes = bsp_flash_read(rd_addr,read_buf,read_len);
        DBG_I("flash read:0x%x len:%d str:%s",rd_addr,read_bytes,(char*)read_buf);
        show_buf(read_buf,read_bytes);
    }     
    else
    {
        DBG_W("flash_r param error! eg: read addr len");
    }
	return RET_OK;
}

REG_SHELL_CMD(flash_r,0,flash_r,"flash read!!", "eg:read addr len \r\n ");
#endif
