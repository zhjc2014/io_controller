
#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "x_frame.h"

extern unsigned char Load$$AUTO_REG_SECTION$$Base[];
extern unsigned char Load$$AUTO_REG_SECTION$$Length[];



void* auto_reg_get_func_cont_by_index(uint8_t func_type, int32_t index)
{
    auto_reg_t* p_auto_reg = NULL;
    uint32_t i;
    uint32_t match_cur_type_cnt = 0;
    /*加载所有需要自动初始化的模块所有设备*/
    unsigned long auto_reg_section_addr = (unsigned long)Load$$AUTO_REG_SECTION$$Base;
    unsigned long auto_reg_section_len  = (unsigned long)Load$$AUTO_REG_SECTION$$Length;
    uint32_t   auto_reg_item = (auto_reg_section_len / sizeof(auto_reg_t));


    if(index > auto_reg_item) {
        return NULL;
    }
    p_auto_reg = (auto_reg_t*)(auto_reg_section_addr);

    for(i = 0; i < auto_reg_item; i++) {
        if(p_auto_reg[i].type == func_type) {
            if(match_cur_type_cnt == index) {
                //DBG_I("[%d]: %s type:%d addr:0x%x",i,p_auto_reg->name,p_auto_reg->type,p_auto_reg->p_user_reg_data);
                return p_auto_reg[i].p_user_reg_data;
            }
            match_cur_type_cnt++;
        }
    }
    return NULL;
}

int32_t auto_reg_get_func_type_cont(uint8_t func_type, void* p_cont[], uint8_t max_read_item)
{
    auto_reg_t* p_auto_reg = NULL;
    uint32_t i;
    int32_t     match_cur_type_cnt = 0;
    /*加载所有需要自动初始化的模块所有设备*/
    unsigned long auto_reg_section_addr = (unsigned long)Load$$AUTO_REG_SECTION$$Base;
    unsigned long auto_reg_section_len  = (unsigned long)Load$$AUTO_REG_SECTION$$Length;
    uint32_t   auto_reg_item = (auto_reg_section_len / sizeof(auto_reg_t));


    p_auto_reg = (auto_reg_t*)(auto_reg_section_addr);
    for(i = 0; i < auto_reg_item; i++) {
        if(p_auto_reg[i].type == func_type) {
            p_cont[match_cur_type_cnt] = p_auto_reg[i].p_user_reg_data;
            match_cur_type_cnt++;
            //DBG_I("[%d]: %s type:%d addr:0x%x",i,p_auto_reg->name,p_auto_reg->type,p_auto_reg->p_user_reg_data);
        }
        if(match_cur_type_cnt >= max_read_item) {
            break;
        }
    }
    return match_cur_type_cnt;
}



typedef void (*pf_init_func)(void);

void auto_init_modules(void)
{
    auto_reg_t* p_auto_reg = NULL;
    uint32_t i;
    pf_init_func p_init_func = NULL;
    unsigned long auto_reg_section_addr = (unsigned long)Load$$AUTO_REG_SECTION$$Base;
    unsigned long auto_reg_section_len  = (unsigned long)Load$$AUTO_REG_SECTION$$Length;
    uint32_t   auto_reg_item = (auto_reg_section_len / sizeof(auto_reg_t));



    p_auto_reg = (auto_reg_t*)(auto_reg_section_addr);
    DBG_I("\r\n ----------------init modules---------\r\n");
    for(i = 0; i < auto_reg_item; i++) {
        if(p_auto_reg[i].type == AUTO_REG_INIT_MOD) {
            DBG_I("init mod [%d]: %15s type:%d addr:0x%x", i, p_auto_reg[i].name, p_auto_reg[i].type, p_auto_reg[i].p_user_reg_data);
            p_init_func = (pf_init_func)p_auto_reg[i].p_user_reg_data;
            p_init_func();
        }

    }
}


#if TINY_FUNC_CODE == 0
int32_t show_modules(uint8_t argc, uint8_t *argv[])
{
    auto_reg_t* p_auto_reg = NULL;
    uint32_t i;
    unsigned long auto_reg_section_addr = (unsigned long)Load$$AUTO_REG_SECTION$$Base;
    unsigned long auto_reg_section_len  = (unsigned long)Load$$AUTO_REG_SECTION$$Length;
    uint32_t   auto_reg_item = (auto_reg_section_len / sizeof(auto_reg_t));

    (void)p_auto_reg;

    p_auto_reg = (auto_reg_t*)(auto_reg_section_addr);
    for(i = 0; i < auto_reg_item; i++) {
        DBG_I("[%2d]:type:%3d addr:0x%8x - %s ", i, p_auto_reg[i].type, p_auto_reg[i].p_user_reg_data, p_auto_reg[i].name);
    }


    return RET_OK;
}

REG_SHELL_CMD_X(moduls, 0, show_modules, "demo show_modulesfault", "show all auto reg modules \r\n ");
#endif
