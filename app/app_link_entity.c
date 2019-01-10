#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "x_frame.h"

extern int HOST_ADDR; 
extern int HOST_GADDR;

//static link_route_t sg_route_tab[] = {
//    //               enable                 distination addr            link id, retransmission by this link
//    ADD_ADDR_ROUTE(EN_ROUTE,                PC_ADDR,                    PC_LINK_ID,                        NULL        ),
//};

//int32_t pc_link_route_check(hal_frame_info_t* p_frame, uint8_t* p_tag_link_id)
//{
//    if(p_frame->dest != HOST_ADDR && p_frame->dest != HOST_GADDR) {
//        uint16_t i = 0;
//        for(i = 0; i < ARRY_ITEMS_NUM(sg_route_tab); i++) {
//            switch(sg_route_tab[i].route_type) {
//            case ROUTE_ADDR:

//                if( (p_frame->dest == sg_route_tab[i].dest) && (sg_route_tab[i].opt == EN_ROUTE) ) {
//                    //DBG_I("find route tag link id:0x%x",sg_route_tab[i].tag_link_id);
//                    *p_tag_link_id = sg_route_tab[i].tag_link_id;

//                    //is the main board changed the route settings
//                    if(sg_route_tab[i].func_opt != NULL) {
//                        uint8_t  modify_link_id = sg_route_tab[i].func_opt(p_frame->dest);
//                        if(modify_link_id != ERR_LINK_ID) {
//                            *p_tag_link_id = modify_link_id;
//                            //DBG_I("modify route for main board!! link:%d",*p_tag_link_id);
//                        }
//                    }
//                    return IS_ROUTE_PACK;
//                }
//                break;
//            default:
//                break;
//            }
//        }
//        return UN_KNOW_PACK; //no route info, discard the package
//    }
//    return IS_HOST_PACK;
//}

//ADD_ROUTE(pc_link_route_check);

