//#define DEF_DBG_I
//#define DEF_DBG_W
//#define DEF_DBG_E
//#include "debug.h"
//#include "param_cfg.h"

//#if DISABLE_PARAM_MOD == 0

//static param_core_t param_core={0};

//static void param_core_init(void)
//{
//    /*1:加载param_cfg_item*/
//    param_item_t *p_item=NULL;
//    uint32_t i = 0;
//    float cur_def_val = 0;
//    INIT_LIST_HEAD(&param_core.param_head);
//    while(1)
//    {
//        p_item = (param_item_t*)auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_PARAM_CFG,i);
//        if(p_item==NULL){break;}
//        
//        INIT_LIST_HEAD(&p_item->param_node);
//        list_add_tail(&p_item->param_node,&param_core.param_head);
//        
//        /*  恢复参数为def val*/
//        
//        //cur_def_val = (p_item->p_item_cfg->def_val<=p_item->p_item_cfg->max_val)?p_item->p_item_cfg->def_val:p_item->p_item_cfg->max_val;
//        //cur_def_val = (cur_def_val>=p_item->p_item_cfg->min_val)?cur_def_val:p_item->p_item_cfg->min_val;
//        //p_item->p_item_cfg->def_val = cur_def_val;
//        
//        if(p_item->p_item_cfg->def_val>p_item->p_item_cfg->max_val)
//        {
//            DBG_W("[%18s]:def_val :%f is larger the max_limit:%f",p_item->p_item_cfg->name,p_item->p_item_cfg->def_val,p_item->p_item_cfg->max_val);
//        }
//        else if(p_item->p_item_cfg->def_val<p_item->p_item_cfg->min_val)
//        {
//            DBG_W("[%18s]:def_val :%f is smaller the min_limit:%f",p_item->p_item_cfg->name,p_item->p_item_cfg->def_val,p_item->p_item_cfg->min_val);
//        }
//        else   {} 
//        
//        switch(p_item->p_item_cfg->type)
//        {
//            case  eu_uint8:
//            case eu_uint16:
//            case eu_uint32:
//                {
//                    uint32_t def_val = (uint32_t)cur_def_val;
//                    memcpy((void*)p_item->p_item_cfg->p_param_val_addr,&def_val,p_item->p_item_cfg->size);
//                }break;
//            case   eu_int8:
//            case  eu_int16:
//            case  eu_int32:
//                {
//                    int32_t def_val = (int32_t)cur_def_val;
//                    memcpy((void*)p_item->p_item_cfg->p_param_val_addr,&def_val,p_item->p_item_cfg->size);
//                }break;
//            case    eu_fp32:
//                {
//                    float def_val = (float)cur_def_val;
//                    memcpy((void*)p_item->p_item_cfg->p_param_val_addr,&def_val,p_item->p_item_cfg->size);
//                }break;
//        }
//        
//        i++;
//    }
//    param_core.param_count = i;
//    DBG_I("system have %d param-cfg items",i);
//}

//REG_INIT_MOD(param_core_init);

///*
//    shell 命令接口和pc通信接口如下：
//    +-------+-------------------------+----------------------------------+-------+-------+-------+
//    |  num  |       api_func          |              param               |  ret  | SHELL |  PC   |
//    +-------+-------------------------+----------------------------------+-------+-------+-------+
//    |   1   |    show_all_param       |              NULL                |  tab  |   OK  |       |
//    +-------+-------------------------+----------------------------------+-------+-------+-------+
//    |   2   |    rd_param_item        |              n                   |  val  |   OK  |   OK  |
//    +-------+-------------------------+----------------------------------+-------+-------+-------+
//    |   3   |    wr_param_item        |              n                   |  val  |   OK  |   OK  |
//    +-------+-------------------------+----------------------------------+-------+-------+-------+
//    |   4   |    rs_param_item        |              n                   |  val  |   OK  |   OK  |
//    +-------+-------------------------+----------------------------------+-------+-------+-------+
//    |   5   |    rs_param_all         |              NULL                | is_ok |   OK  |   OK  |
//    +-------+-------------------------+----------------------------------+-------+-------+-------+
//    |   6   |    load_param_item      |              n                   | item  |       |   OK  |
//    +-------+-------------------------+----------------------------------+-------+-------+-------+
//    |   7   |    get_param_count      |              NULL                | num   |       |   OK  |
//    +-------+-------------------------+----------------------------------+-------+-------+-------+
//*/


//static param_item_t*  rd_param_item(uint32_t n_item,float* val)
//{
//    param_item_t *p_item=NULL;
//    uint32_t i = 0;
//    list_t *cur_list=NULL;
//    list_t *tag_list_head = &param_core.param_head;
//    float rd_ret_val = 0;
//    list_for_each(cur_list,tag_list_head)
//    {
//        if(i==n_item)
//        {
//            p_item = list_entry(cur_list,param_item_t,param_node);    
//            switch(p_item->p_item_cfg->type)
//            {
//                case  eu_uint8:
//                case eu_uint16:
//                case eu_uint32:
//                    {
//                        uint32_t rd_val = 0;
//                        memcpy(&rd_val,p_item->p_item_cfg->p_param_val_addr,p_item->p_item_cfg->size);
//                        rd_ret_val = rd_val;
//                    }break;
//                case   eu_int8:
//                case  eu_int16:
//                case  eu_int32:
//                    {
//                        int32_t rd_val = 0;
//                        memcpy(&rd_val,p_item->p_item_cfg->p_param_val_addr,p_item->p_item_cfg->size);
//                        rd_ret_val = rd_val;
//                    }break;
//                case    eu_fp32:
//                    {
//                        float rd_val = 0;
//                        memcpy(&rd_val,p_item->p_item_cfg->p_param_val_addr,p_item->p_item_cfg->size);
//                        rd_ret_val = rd_val;
//                    }break;
//            }
//            memcpy(val,&rd_ret_val,sizeof(float));
//            return p_item;
//        }
//        i++;
//    }
//    return NULL;
//}

//static param_item_t* wr_param_item(uint32_t n_item,float val)
//{
//    param_item_t *p_item=NULL;
//    uint32_t i = 0;
//    list_t *cur_list=NULL;
//    list_t *tag_list_head = &param_core.param_head;
//    list_for_each(cur_list,tag_list_head)
//    {
//        if(i==n_item)
//        {
//            p_item = list_entry(cur_list,param_item_t,param_node); 
//            
//            val = (val<=p_item->p_item_cfg->max_val)?val:p_item->p_item_cfg->max_val;
//            val = (val>=p_item->p_item_cfg->min_val)?val:p_item->p_item_cfg->min_val;
//            
//            switch(p_item->p_item_cfg->type)
//            {
//                case  eu_uint8:
//                case eu_uint16:
//                case eu_uint32:
//                    {
//                        uint32_t u32_val = (uint32_t)val;
//                        memcpy(p_item->p_item_cfg->p_param_val_addr,&u32_val,p_item->p_item_cfg->size);
//                    }break;
//                case   eu_int8:
//                case  eu_int16:
//                case  eu_int32:
//                    {
//                        int32_t i32_val = (int32_t)val;
//                        memcpy(p_item->p_item_cfg->p_param_val_addr,&i32_val,p_item->p_item_cfg->size);
//                    }break;
//                case    eu_fp32:
//                    {
//                        memcpy(p_item->p_item_cfg->p_param_val_addr,&val,p_item->p_item_cfg->size);
//                    }break;
//            }            
//            
//            return p_item;
//        }
//        i++;
//    }
//    return NULL;
//}

//static param_item_t* rs_param_item(uint32_t n_item)
//{
//    param_item_t *p_item=NULL;
//    uint32_t i = 0;
//    list_t *cur_list=NULL;
//    list_t *tag_list_head = &param_core.param_head;
//    float def_val = 0;
//    list_for_each(cur_list,tag_list_head)
//    {
//        if(i==n_item)
//        {
//            p_item = list_entry(cur_list,param_item_t,param_node);    
//            def_val = p_item->p_item_cfg->def_val;
//            
//            DBG_I("def_val :%f",def_val);
//                 if(def_val<p_item->p_item_cfg->min_val){def_val = p_item->p_item_cfg->min_val;}
//            else if(def_val>p_item->p_item_cfg->max_val){def_val = p_item->p_item_cfg->max_val;}       
//            else  {}
//                
//             switch(p_item->p_item_cfg->type)
//            {
//                case  eu_uint8:
//                case eu_uint16:
//                case eu_uint32:
//                    {
//                        uint32_t u32_val = (uint32_t)def_val;
//                        memcpy(p_item->p_item_cfg->p_param_val_addr,&u32_val,p_item->p_item_cfg->size);
//                    }break;
//                case   eu_int8:
//                case  eu_int16:
//                case  eu_int32:
//                    {
//                        int32_t i32_val = (int32_t)def_val;
//                        memcpy(p_item->p_item_cfg->p_param_val_addr,&i32_val,p_item->p_item_cfg->size);
//                    }break;
//                case    eu_fp32:
//                    {
//                        memcpy(p_item->p_item_cfg->p_param_val_addr,&def_val,p_item->p_item_cfg->size);
//                    }break;
//            }            
//            return p_item;
//        }
//        i++;
//    }
//    return NULL;
//}

//static void rs_param_all(void)
//{
//    param_item_t *p_item=NULL;
//    list_t *cur_list=NULL;
//    list_t *tag_list_head = &param_core.param_head;
//    float def_val = 0;
//    list_for_each(cur_list,tag_list_head)
//    {
//        p_item = list_entry(cur_list,param_item_t,param_node);    
//        def_val = p_item->p_item_cfg->def_val;
//             if(def_val<p_item->p_item_cfg->min_val){def_val = p_item->p_item_cfg->min_val;}
//        else if(def_val>p_item->p_item_cfg->max_val){def_val = p_item->p_item_cfg->max_val;}       
//        else  {}
//            
//         switch(p_item->p_item_cfg->type)
//        {
//            case  eu_uint8:
//            case eu_uint16:
//            case eu_uint32:
//                {
//                    uint32_t u32_val = (uint32_t)def_val;
//                    memcpy(p_item->p_item_cfg->p_param_val_addr,&u32_val,p_item->p_item_cfg->size);
//                }break;
//            case   eu_int8:
//            case  eu_int16:
//            case  eu_int32:
//                {
//                    int32_t i32_val = (int32_t)def_val;
//                    memcpy(p_item->p_item_cfg->p_param_val_addr,&i32_val,p_item->p_item_cfg->size);
//                }break;
//            case    eu_fp32:
//                {
//                    memcpy(p_item->p_item_cfg->p_param_val_addr,&def_val,p_item->p_item_cfg->size);
//                }break;
//        }            
//    }
//}
//static int32_t  load_param_item(uint32_t n_item,uint8_t* p_buf,uint32_t max_len)
//{
//    param_item_t *p_item=NULL;
//    uint32_t i = 0;
//    int32_t real_len = 0;
//    list_t *cur_list=NULL;
//    list_t *tag_list_head = &param_core.param_head;
//    protocol_param_item_t* p_protocol_item=(protocol_param_item_t*)p_buf;
//    list_for_each(cur_list,tag_list_head)
//    {
//        if(i==n_item)
//        {
//            p_item = list_entry(cur_list,param_item_t,param_node);    
//            real_len = sizeof(protocol_param_item_t)+strlen((const char*)p_item->p_item_cfg->name);
//            if(max_len<real_len){return -2;}
//            
//            p_protocol_item->status     = 0;
//            p_protocol_item->item_num   = i;
//            p_protocol_item->type       = p_item->p_item_cfg->type;
//            p_protocol_item->attr       = p_item->p_item_cfg->attr;
//            p_protocol_item->size       = p_item->p_item_cfg->size;
//            
//            p_protocol_item->min_val    = p_item->p_item_cfg->min_val;
//            p_protocol_item->max_val    = p_item->p_item_cfg->max_val;
//            p_protocol_item->cur_val    = 0;
//            
//            
//            p_protocol_item->def_val    = p_item->p_item_cfg->def_val;
//            
//            if(p_protocol_item->def_val>p_item->p_item_cfg->max_val)
//            {
//                //DBG_W("[%18s]:def_val :%f is larger the max_limit:%f",p_item->p_item_cfg->name,p_item->p_item_cfg->def_val,p_item->p_item_cfg->max_val);
//                p_protocol_item->def_val = p_item->p_item_cfg->max_val;
//            }
//            else if(p_protocol_item->def_val<p_item->p_item_cfg->min_val)
//            {
//                //DBG_W("[%18s]:def_val :%f is smaller the min_limit:%f",p_item->p_item_cfg->name,p_item->p_item_cfg->def_val,p_item->p_item_cfg->min_val);
//                p_protocol_item->def_val = p_item->p_item_cfg->min_val;
//            }
//            else   {} 
//                
//            
//            switch(p_item->p_item_cfg->type)
//            {
//                case  eu_uint8:
//                case eu_uint16:
//                case eu_uint32:
//                    {
//                        uint32_t u32_val = 0;
//                        memcpy(&u32_val,p_item->p_item_cfg->p_param_val_addr,p_item->p_item_cfg->size);
//                        p_protocol_item->cur_val = u32_val;
//                    }break;
//                case   eu_int8:
//                case  eu_int16:
//                case  eu_int32:
//                    {
//                        int32_t i32_val = 0;
//                        memcpy(&i32_val,p_item->p_item_cfg->p_param_val_addr,p_item->p_item_cfg->size);
//                        p_protocol_item->cur_val = i32_val;
//                    }break;
//                case    eu_fp32:
//                    {
//                        memcpy(&p_protocol_item->cur_val,p_item->p_item_cfg->p_param_val_addr,p_item->p_item_cfg->size);
//                    }break;
//            }            
//            
//           
//            memcpy(p_protocol_item->name,p_item->p_item_cfg->name,strlen((const char*)p_item->p_item_cfg->name));
//            p_buf[real_len-1]='\0';
//            return real_len;
//        }
//        i++;
//    }
//    {return -1;}
//}


//static uint32_t get_param_count(void)
//{
//    return param_core.param_count;
//}



////--------------protocol-api interface


//static void _send_ack(hal_frame_info_t* p_frame_info,uint8_t* pbuf,uint16_t len)
//{
//    uint8_t addr = p_frame_info->dest;
//    p_frame_info->dest = p_frame_info->src;
//    p_frame_info->src  = addr;
//    p_frame_info->is_ack = 1;
//    p_frame_info->pdata = pbuf;
//    p_frame_info->data_len = len;
//    protocol_send(p_frame_info);
//}



//int32_t         param_act_param_cnt       (hal_frame_info_t* p_frame_info)
//{
//    
//    ack_param_cnt_t ack={0};
//    ack.total_item_num =get_param_count();
//    DBG_I_LIMIT(0,"total_item:%d",ack.total_item_num);
//    _send_ack(p_frame_info,(uint8_t*)&ack,sizeof(ack_param_cnt_t));
//    return 0;
//}

//int32_t         param_act_param_item      (hal_frame_info_t* p_frame_info)
//{
//    #define MAX_ITEM_LEN 100
//    int32_t real_len = 0;
//    uint32_t cur_item_num = 0;
//    uint8_t buf_send[MAX_ITEM_LEN]={0};
//    
//    memcpy(&cur_item_num,p_frame_info->pdata,sizeof(uint32_t));
//    //DBG_I("cur_item_num:%d",cur_item_num);
//    real_len = load_param_item(cur_item_num,buf_send,MAX_ITEM_LEN);
//    
//    if(real_len>0)
//    {
//        _send_ack(p_frame_info,(uint8_t*)buf_send,real_len);
//    }
//    else
//    {
//        memcpy(buf_send,&real_len,1);
//        _send_ack(p_frame_info,(uint8_t*)buf_send,1);
//    }
//   
//    return 0;
//}

//int32_t         param_act_param_wr       (hal_frame_info_t* p_frame_info)
//{
//   
//    
//    ack_param_wr_t  ack_wr={0};
//    param_wr_t*      p_wr_dat=NULL;
//    
//    DBG_I("param_act_param_wr!!!");
//    if( p_frame_info->data_len ==  sizeof(param_wr_t) )
//    {
//        param_item_t* param_item = NULL;
//        float    rd_val   = 0;
//        p_wr_dat = (param_wr_t*)(p_frame_info->pdata);
//        
//        DBG_I("wr_param_item,item:%d  val:%f",p_wr_dat->wr_index,p_wr_dat->val);
//        wr_param_item(p_wr_dat->wr_index,p_wr_dat->val);
//        param_item = rd_param_item(p_wr_dat->wr_index,&rd_val);
//        ack_wr.wr_index = p_wr_dat->wr_index;
//        if(param_item==NULL){DBG_W("the item:%d is outof range",p_wr_dat->wr_index); ack_wr.status = -1;}
//        else
//        {
//           ack_wr.ret_val = rd_val;
//           DBG_I("rd val:%f",rd_val);
//        }
//    }
//    else
//    {
//        ack_wr.status = -1;
//    }
//    _send_ack(p_frame_info,(uint8_t*)&ack_wr,sizeof(ack_param_wr_t));
//    
//    return 0;
//}

//int32_t         param_act_param_rd       (hal_frame_info_t* p_frame_info)
//{
//    ack_param_wr_t  ack_wr={0};
//    param_item_t* param_item = NULL;
//    float    rd_val   = 0;
//    uint32_t  nitem = 0;     
//    if( p_frame_info->data_len ==  sizeof(uint32_t) )
//    {
//        memcpy(&nitem,p_frame_info->pdata,sizeof(uint32_t));
//        param_item = rd_param_item(nitem,&rd_val);
//        ack_wr.wr_index = nitem;
//        if(param_item==NULL){DBG_W("the item:%d is outof range",nitem); ack_wr.status = -1;}
//        else
//        {
//           ack_wr.ret_val = rd_val;
//           DBG_I("rd val:%f",rd_val);
//        }
//    }
//    else
//    {
//        ack_wr.status = -1;
//    }
//    _send_ack(p_frame_info,(uint8_t*)&ack_wr,sizeof(ack_param_wr_t));
//    
//    return 0;
//}

//int32_t         param_act_param_rs       (hal_frame_info_t* p_frame_info)
//{
//    ack_param_wr_t  ack_wr={0};
//    param_item_t* param_item = NULL;
//    float    rd_val   = 0;
//    uint32_t  nitem = 0;     
//    if( p_frame_info->data_len ==  sizeof(uint32_t) )
//    {
//        memcpy(&nitem,p_frame_info->pdata,sizeof(uint32_t));
//        param_item = rs_param_item(nitem);
//        param_item = rd_param_item(nitem,&rd_val);
//        ack_wr.wr_index = nitem;
//        if(param_item==NULL){DBG_W("the item:%d is outof range",nitem); ack_wr.status = -1;}
//        else
//        {
//           ack_wr.ret_val = rd_val;
//           DBG_I("rd val:%f",rd_val);
//        }
//    }
//    else
//    {
//        ack_wr.status = -1;
//    }
//    _send_ack(p_frame_info,(uint8_t*)&ack_wr,sizeof(ack_param_wr_t));
//    return 0;
//}

//int32_t         param_act_param_rs_all       (hal_frame_info_t* p_frame_info)
//{
//    rs_param_all();
//    return 0;
//}



//ADD_LINK_ACTION(PC_LINK_ID      ,COMM_CMD_SET    ,CMD_ID_GET_PARAM_CNT        ,param_act_param_cnt);
//ADD_LINK_ACTION(PC_LINK_ID      ,COMM_CMD_SET    ,CMD_ID_GET_PARAM_ITEM       ,param_act_param_item);
//ADD_LINK_ACTION(PC_LINK_ID      ,COMM_CMD_SET    ,CMD_ID_WR_PARAM_ITEM        ,param_act_param_wr);
//ADD_LINK_ACTION(PC_LINK_ID      ,COMM_CMD_SET    ,CMD_ID_RD_PARAM_ITEM        ,param_act_param_rd);
//ADD_LINK_ACTION(PC_LINK_ID      ,COMM_CMD_SET    ,CMD_ID_RS_PARAM_ITEM        ,param_act_param_rs);
//ADD_LINK_ACTION(PC_LINK_ID      ,COMM_CMD_SET    ,CMD_ID_RS_ALL_PARAM_ITEM    ,param_act_param_rs_all);

//#if TINY_FUNC_CODE == 0
////-----------shell  cmd
//static int32_t show_all_param(uint8_t argc, uint8_t *argv[])
//{
//    param_item_t *p_item=NULL;
//    uint32_t i = 0;
//    list_t *cur_list=NULL;
//    list_t *tag_list_head = &param_core.param_head;
//    value_contain_t cur_val={0};
//    //float cur_val = 0;
//    DBG_I("(%d param in tab!)",param_core.param_count);
//    DBG_I("[num]:-------name--------  ---min---   ---max---  ---def---  ---cur---");
//    
//    list_for_each(cur_list,tag_list_head)
//    {
//        p_item = list_entry(cur_list,param_item_t,param_node);
//        memcpy(&cur_val,p_item->p_item_cfg->p_param_val_addr,p_item->p_item_cfg->size);
//        
//        switch(p_item->p_item_cfg->type)
//        {
//            case  eu_uint8:
//            case eu_uint16:
//            case eu_uint32:
//                {
//                    DBG_I("[%3d]:%18s %10.0f %10.0f %10.0f %10d",i,p_item->p_item_cfg->name,p_item->p_item_cfg->min_val,p_item->p_item_cfg->max_val,p_item->p_item_cfg->def_val,cur_val.u32_val);
//                }break;
//            case   eu_int8:
//            case  eu_int16:
//            case  eu_int32:
//                {
//                    DBG_I("[%3d]:%18s %10.0f %10.0f %10.0f %10d",i,p_item->p_item_cfg->name,p_item->p_item_cfg->min_val,p_item->p_item_cfg->max_val,p_item->p_item_cfg->def_val,cur_val.i32_val);
//                }break;
//            case    eu_fp32:
//                {
//                    DBG_I("[%3d]:%18s %10.3f %10.3f %10.3f %10.3f ",i,p_item->p_item_cfg->name,p_item->p_item_cfg->min_val,p_item->p_item_cfg->max_val,p_item->p_item_cfg->def_val,cur_val.fp32_val);
//                }break;
//        }
//        i++;
//    }
//    return RET_OK;
//}

//REG_SHELL_CMD_X(param_show,0,show_all_param,"show_all_param", "show_all_param\r\n ");



//static int32_t param_wr(uint8_t argc, uint8_t *argv[])
//{
//    param_item_t* param_item = NULL;
//    uint32_t nitem = 0;
//    float    val   = 0;
//    float    rd_val   = 0;
//    if(argc<3){DBG_W("param err!! eg:param_wr nitem  val");return RET_OK;}
//    
//    
//    sscanf((const char*)argv[1],"%d",&nitem);
//    sscanf((const char*)argv[2],"%f",&val);
//    wr_param_item(nitem,val);
//    param_item = rd_param_item(nitem,&rd_val);
//    if(param_item==NULL){DBG_W("the item:%d is outof range max:%d",nitem,param_core.param_count);}
//    else
//    {
//        if(param_item->p_item_cfg->type == eu_fp32) {DBG_I("write param [%d]- %s : tag:%.3f  real_val:%.3f",nitem,param_item->p_item_cfg->name,val,rd_val);}
//        else                                        {DBG_I("write param [%d]- %s : tag:%.0f  real_val:%.3f",nitem,param_item->p_item_cfg->name,val,rd_val);}
//    }
//    return RET_OK;
//}
//REG_SHELL_CMD_X(param_wr,0,param_wr,"param_wr nitem  val", "param_wr nitem  val\r\n ");

//static int32_t param_rd(uint8_t argc, uint8_t *argv[])
//{
//    param_item_t* param_item = NULL;
//    uint32_t nitem = 0;
//    float    val   = 0;
//    if(argc<2){DBG_W("param err!! eg:param_rd nitem ");return RET_OK;}
//    
//    sscanf((const char*)argv[1],"%d",&nitem);
//    param_item = rd_param_item(nitem,&val);
//    if(param_item==NULL){DBG_W("the item:%d is outof range max:%d",nitem,param_core.param_count);}
//    else
//    {
//        if(param_item->p_item_cfg->type == eu_fp32) {DBG_I("read param [%d]- %s : %.3f",nitem,param_item->p_item_cfg->name,val);}
//        else                                        {DBG_I("read param [%d]- %s : %.0f",nitem,param_item->p_item_cfg->name,val);}
//    }
//    return RET_OK;
//}
//REG_SHELL_CMD_X(param_rd,0,param_rd,"param_rd nitem ", "param_rd nitem \r\n ");

//static int32_t param_rs(uint8_t argc, uint8_t *argv[])
//{
//    param_item_t* param_item = NULL;
//    uint32_t nitem = 0;
//    float    val   = 0;
//    if(argc<2){DBG_W("param err!! eg:param_rd nitem ");return RET_OK;}
//    
//    sscanf((const char*)argv[1],"%d",&nitem);
//    param_item = rs_param_item(nitem);
//    if(param_item==NULL){DBG_W("the item:%d is outof range max:%d",nitem,param_core.param_count);}
//    else
//    {
//        if(param_item->p_item_cfg->type == eu_fp32) {DBG_I("read param [%d]- %s : %.3f",nitem,param_item->p_item_cfg->name,val);}
//        else                                        {DBG_I("read param [%d]- %s : %.0f",nitem,param_item->p_item_cfg->name,val);}
//    }
//    return RET_OK;
//}
//REG_SHELL_CMD_X(param_rest,0,param_rs,"param_rest nitem  ", "param_wr nitem  \r\n ");

//static int32_t param_rs_all(uint8_t argc, uint8_t *argv[])
//{
//    rs_param_all();
//    return RET_OK;
//}
//REG_SHELL_CMD_X(param_rest_all,0,param_rs_all,"param_rest_all  ", "param_rest_all  \r\n ");
//#endif 

//#endif 
