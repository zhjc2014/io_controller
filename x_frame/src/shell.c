#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "shell.h"
#include "linebuf.h"

#include "tasks.h"
#include "hal_app_interface.h"
#include "x_frame.h"


#if DISABLE_SHELL_MOD == 0

extern uint32_t     sys_debug_drv_id;
extern void*   auto_reg_get_func_cont_by_index(uint8_t func_type,int32_t index);
extern int32_t auto_reg_get_func_type_cont(uint8_t func_type,void* p_cont[],uint8_t max_read_item);


#define SHELL_BUF_SIZE  256
#define SHELL_DEV_IN    sys_debug_drv_id
#define SHELL_MAX_ARGS  16
#define SHELL_X_MAX_ITEM  16
#define SHELL_MAX_ITEM    32



static uint8_t         sg_buf[SHELL_BUF_SIZE];
static linebuf_t     sg_shell_line_buf={"shell_linebuf",sg_buf,SHELL_BUF_SIZE};

int32_t parse_line (uint8_t* line, uint8_t *argv[],uint8_t* p_args,uint8_t max_len);
uint8_t*     shell_dec_line    (linebuf_t*   p_lb);
static shell_cmd_t* find_cmd        (uint8_t * pcmd);
static uint8_t find_EOF(uint8_t* pbuf,uint32_t len);


uint8_t shell_action(uint8_t* pbuf_start,uint8_t max_len)
{
    uint8_t  cur_args = 0;
    uint8_t  read_len = 0;
    uint8_t* shell_args[SHELL_MAX_ARGS];
    shell_cmd_t*  cur_cmd = NULL;
    
    read_len = parse_line(pbuf_start,shell_args,&cur_args,max_len);
            
    /*3:寻找对应的cmd，并且执行*/

    if(cur_args)
    {
        cur_cmd = find_cmd(shell_args[0]);
        if(!cur_cmd)
        {
            SHELL_WRITE("can not find the cmd:%s",shell_args[0]);
        }
        else
        {
            cur_cmd->cmd_func(cur_args,shell_args);
        }
    }
    
    
    return read_len;
}


void shell_run_on_tick(void)
{
    uint16_t lb_rest_len = 0;
    uint32_t real_len = 0;
    uint8_t  lb_state = 0;
    int32_t  ret = 0;
    uint8_t  temp_buf[SHELL_BUF_SIZE];
    uint8_t* pbuf_start = NULL;
    uint8_t  read_len = 0;
    
    
    do
    {
        /*1:从fifo中读取数据到linebuf*/
        
        lb_state    = lb_get_statues(&sg_shell_line_buf);
        if(lb_state&(LB_STATE_IS_R|LB_STATE_IS_M|LB_STATE_IS_R)){break;}
        
        lb_rest_len = lb_get_rest_len(&sg_shell_line_buf);
        ret = hal_app_read(SHELL_DEV_IN,temp_buf,lb_rest_len,0,&real_len);
        
        if( (ret>=0) && (real_len>0) )
        {
            uint32_t cur_len = 0;
            lb_push(&sg_shell_line_buf,temp_buf,real_len);
            /*2:不断的解析linebuf*/

            cur_len = lb_get_data_len(&sg_shell_line_buf);
            if(cur_len==sg_shell_line_buf.size){lb_reset(&sg_shell_line_buf);}
            
            if(find_EOF(sg_shell_line_buf.pbuf,cur_len))
            {
                pbuf_start = shell_dec_line(&sg_shell_line_buf);
                read_len = shell_action(pbuf_start,cur_len);
            }
            /*4:移除已经处理的过的linebuf数据*/
            lb_move(&sg_shell_line_buf,read_len);
        }
    }while(real_len);
}

static uint8_t find_EOF(uint8_t* pbuf,uint32_t len)
{
    uint32_t i = 0;

    for(i=0; i<len; i++)
    {
        if((pbuf[i]=='\r') || ((pbuf[i]=='\n')))
        {
            return 1;
        }
    }

    return 0;
}





int32_t  is_ascii(uint8_t ch)
{
    if( ( (ch>'a')&&(ch<'z') ) ||
        ( (ch>'A')&&(ch<'Z') ) ||
        ( (ch>'0')&&(ch<'9') ) 
    )
    {
        return 1;
    }
    return 0;
}

uint8_t* shell_dec_line(linebuf_t*   p_lb)
{
    uint16_t     buf_len=0;
    uint8_t*    cur_ptr=NULL;
    uint16_t     cur_index = 0;
    
    buf_len = lb_get_data_len(p_lb);
    cur_ptr = lb_get_r_ptr(p_lb);
    
    while(cur_index<buf_len)
    {
        if(is_ascii(cur_ptr[cur_index]))
        {
            return &cur_ptr[cur_index];
        }
        cur_index++;
        p_lb->r_index++;
    }

    return NULL;
}


int32_t parse_line (uint8_t* line, uint8_t *argv[],uint8_t* p_args,uint8_t max_len)
{
    int nargs = 0;
    int16_t read_len = 0;
    while ((nargs < SHELL_MAX_ARGS)&&(read_len<max_len)) {

        /* skip any white space */
        while ( ((*line == ' ') || (*line == '\t') || (*line == '\r') || (*line == '\n')) && (read_len<max_len)) {
            ++line;
            read_len++;
        }

        if (*line == '\0') {    /* end of line, no more args    */
            argv[nargs] = NULL;
            *p_args = nargs;
            return (read_len);
        }

        if(read_len<max_len)
        {
            argv[nargs++] = line;    /* begin of argument string    */
        }
        

        /* find end of string */
        while (*line && (*line != ' ') && (*line != '\t') && (*line != '\r') && (*line != '\n') && (read_len<max_len) ) 
        {
            ++line;
            read_len++;
        }
        
        *p_args = nargs;

        if ((*line == '\0') ) {    /* end of line, no more args    */
            argv[nargs] = NULL;
            return (read_len);
        }
        
        if(read_len<max_len)
        {
            *line++ = '\0';        /* terminate current arg     */
            read_len++;
        }
    }

    //SHELL_WRITE ("** Too many args (max. %d) **\n", SHELL_MAX_ARGS);
    return (read_len);
}





uint8_t real_sys_shell_x_item = 0;
static shell_cmd_t*  sg_shell_x_item[SHELL_X_MAX_ITEM]={0};

uint8_t real_sys_shell_item = 0;
static shell_cmd_t*  sg_shell_item[SHELL_MAX_ITEM]={0};

static uint8_t flag_shell_init = 0;

void init_shell_item(void)
{
    if(flag_shell_init==0)
    {
        flag_shell_init = 1;
        real_sys_shell_x_item  = auto_reg_get_func_type_cont(AUTO_REG_TYPE_SHELL_X,(void**)sg_shell_x_item,SHELL_X_MAX_ITEM);
        real_sys_shell_item    = auto_reg_get_func_type_cont(AUTO_REG_TYPE_SHELL  ,(void**)sg_shell_item  ,SHELL_MAX_ITEM  );
    }
}


//REG_INIT_MOD(init_shell_item);


shell_cmd_t* find_cmd(uint8_t * pcmd)
{
    shell_cmd_t* pShell_cmd=NULL;
    uint32_t i = 0;
   
    for(i=0;i<real_sys_shell_item;i++)
    {
        pShell_cmd = (shell_cmd_t*)(sg_shell_item[i]);
        if(strcmp((const char*)pShell_cmd->name,(const char*)pcmd) == 0)
        {
            return pShell_cmd;
        }
    }
    
    for(i=0;i<real_sys_shell_x_item;i++)
    {
        pShell_cmd = (shell_cmd_t*)(sg_shell_x_item[i]);
        if(strcmp((const char*)pShell_cmd->name,(const char*)pcmd) == 0)
        {
            return pShell_cmd;
        }
    }
    return NULL;
}


REG_TASK(LW_TASK,10,shell_run_on_tick);


int32_t shell_helpx(uint8_t argc, uint8_t *argv[])
{
    shell_cmd_t* pShell_cmd=NULL;
    uint32_t i = 0;
    
    /*help 后面还有参数，说明是显示具体函数的帮助信息所有就不要显示list信息了*/
    if(argc==1)
    {
        SHELL_WRITE("[sys reg cmd list]:");
        SHELL_WRITE(" --name--    ----usestage----");
    }
    
    for(i=0;i<real_sys_shell_x_item;i++)
    {
        pShell_cmd = (shell_cmd_t*)sg_shell_x_item[i];
        if(argc>1)
        {
            if(strcmp((const char*)pShell_cmd->name,(const char*)argv[1]) == 0)
            {
                SHELL_WRITE("%-10s  :\r\n  %s",pShell_cmd->name,pShell_cmd->help);
                return RET_OK;
            }
        }
        else
        {
            SHELL_WRITE("%-15s  : %-15s",pShell_cmd->name,pShell_cmd->usage);
        }
    }
    
    if(argc>1)
    {
        SHELL_WRITE("Not find the shell cmd:%s",argv[1]);
    }
    
    return RET_OK;
}

REG_SHELL_CMD_X(helpx,0,shell_helpx,"help to list sys_cmds",
"this is the dtail help for help \r\n \
    help - list all func.\r\n \
    help - cmd_name  show the detail info for cmd");





int32_t shell_help(uint8_t argc, uint8_t *argv[])
{
    shell_cmd_t* pShell_cmd=NULL;
    uint32_t i = 0;
    
    /*help 后面还有参数，说明是显示具体函数的帮助信息所有就不要显示list信息了*/
    if(argc==1)
    {
        SHELL_WRITE("[user reg cmd list]:");
        SHELL_WRITE(" --name--    ----usestage----");
    }
    
    for(i=0;i<real_sys_shell_item;i++)
    {
        pShell_cmd = (shell_cmd_t*)sg_shell_item[i];
        if(argc>1)
        {
            if(strcmp((const char*)pShell_cmd->name,(const char*)argv[1]) == 0)
            {
                SHELL_WRITE("%-10s  :\r\n  %s",pShell_cmd->name,pShell_cmd->help);
                return RET_OK;
            }
        }
        else
        {
            SHELL_WRITE("%-15s  : %-15s",pShell_cmd->name,pShell_cmd->usage);
        }
    }
    
    if(argc>1)
    {
        SHELL_WRITE("Not find the shell cmd:%s",argv[1]);
    }
    
    return RET_OK;
}

REG_SHELL_CMD(help,0,shell_help,"help to list user_cmds",
"this is the dtail help for help \r\n \
    help - list all func.\r\n \
    help - cmd_name  show the detail info for cmd");

#else
void init_shell_item(void)
{
}
#endif

