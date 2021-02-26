#ifndef MSG_I_H
#define MSG_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  Diagnostic packet processing internal declarations.

Copyright (c) 2007-2014,2016 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/common/src/msgi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/16   ph      SMD references removal and code clean up.
04/01/14   rs      Added support for build mask centralization. 
11/22/13   sr      Removed PACK from pointer references 
02/11/13   sr      Mainling Diag central routing  
11/15/12   is      Support for preset masks
08/31/12   sr      Support for mask retrieval command  
08/14/12   ra      Added prototype for msg_mask_binary_search
07/05/12   ra      Features for F3 Mask Check Optimization   
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
12/23/10   mad     Added prototype for msg_get_ssid_masks_adv 
05/10/10   sg      moved msg_mask type from msg.c to msgi.h
01/15/09   mad     defined MSG_I_H, updated copyright
12/02/08   mad     Created file.
===========================================================================*/

#include "diagcomm_ctrl.h" /* For diag_ctrl_msg_type */

/* This structure is used to transfer message mask
to Diag_LSM. 
*/
typedef struct
{
    uint16 ssid_first;      /* Start of range of supported SSIDs */
    uint16 ssid_last;       /* Last SSID in range */
    /* Array of (ssid_last - ssid_first + 1) masks */
    uint32* rt_mask_array;
}
msg_mask_read_buffer_type;

typedef struct{
  uint32 args[10];
}msg_large_args;

#define MSG_LARGE_ARGS(X) (((msg_large_args *)X)->args)

typedef enum
{
  MSG_MASK_BT,          /* Build-time mask */
  MSG_MASK_RT           /* Run-time mask */
} msg_mask_type;

uint8 msg_mask_binary_search (uint16 curr_ssid);

boolean msg_get_ssid_masks_adv (msg_mask_type mask_type, uint16 ssid_start,
                                uint16 ssid_end, uint32 * mask, byte stream_id, uint8 preset_id);

void * msg_pkt_get_local_masks(void * req_pkt, uint16 pkt_len);
 
void * msg_pkt_get_local_ssid_range(void * req_pkt, uint16 pkt_len);     

void msg_set_all_rt_masks_adv (uint32 mask, byte stream_id, uint8 preset_id);
                                
void diag_switch_msg_preset_mask( void );
                                
void diag_ctrl_update_msg_mask( diag_ctrl_msg_type * ctrl_msg );
void diag_ctrl_update_msg_preset_mask( diag_ctrl_msg_type * ctrl_msg );
void diag_ctrl_send_msg_ssid_range_ctrl_pkt(void);
void diag_ctrl_send_msg_bt_mask_ctrl_pkt(void);


/* For F3 Mask Check Optimization - Used to set
   corresponding bits in msg_enabled_mask.
   These denote if a msg mask / listener is turned
   "on" or "off"
*/
#define DIAG_MSG_NONE (0x00)
#define DIAG_MSG_STREAM_1 (0x01)
#define DIAG_MSG_STREAM_2 (0x02)
#define DIAG_MSG_LISTENER (0x04)
#define DIAG_MSG_EXT_LISTENER (0x08)


#endif /* MSG_I_H */
