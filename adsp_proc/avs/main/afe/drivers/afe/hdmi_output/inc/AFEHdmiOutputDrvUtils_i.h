/*========================================================================
   This file contains declerations to program HDMI hardware registers

  Copyright (c) 2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/inc/AFEHdmiOutputDrvUtils_i.h#1 $
 ====================================================================== */

#ifndef AFE_HDMI_OUTPUT_DRV_UTILS_I_H
#define AFE_HDMI_OUTPUT_DRV_UTILS_I_H

/*==========================================================================
  Macro Defines
  ========================================================================== */

/*==========================================================================
  Structure definitions
  ========================================================================== */
typedef struct hdmi_out_group_node hdmi_out_group_node_t;

struct hdmi_out_group_node
{
   uint16_t port_id;
   uint16_t stream_id;
   uint16_t action_id;
   uint16_t valid;    //1 - valid, 0 - invalid

   hdmi_out_group_node_t*    next_ptr;
};



typedef struct hdmi_out_group_list 
{

    hdmi_out_group_node_t* list_ptr;

    int16_t num_node;
}hdmi_out_group_list_t;


ADSPResult hdmi_output_group_list_node_create(afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg, hdmi_out_group_list_t* list_ptr);
ADSPResult hdmi_output_group_list_node_build(afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg, hdmi_out_group_list_t* src_list_ptr, hdmi_out_group_list_t* dst_list_ptr);
ADSPResult hdmi_output_group_list_node_destroy(hdmi_out_group_list_t* p_group_list);

ADSPResult hdmi_output_group_list_merge(hdmi_out_group_list_t* p_group_target_list);

ADSPResult hdmi_output_group_list_error_check(hdmi_out_group_list_t* p_group_list);
ADSPResult hdmi_output_group_cfg_update_validation(void);

ADSPResult hdmi_output_group_cfg_sanity_check(afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg);

ADSPResult hdmi_output_group_list_valid_update(uint16_t port_id, hdmi_out_group_list_t* p_group_list);  
bool_t hdmi_output_group_list_is_last_item_query(hdmi_out_group_list_t* p_group_list_1, hdmi_out_group_list_t* p_group_list_2);
bool_t hdmi_output_group_list_is_first_item_query(hdmi_out_group_list_t* p_group_list_1, hdmi_out_group_list_t* p_group_list_2);

ADSPResult hdmi_output_group_list_stream_route_update_all(hdmi_out_group_list_t* p_start_list);
uint16_t hdmi_output_group_list_get_stream_id(uint16_t port_id, hdmi_out_group_list_t* p_group_list);


ADSPResult hdmi_output_group_list_cfg_list_match_check(afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg, hdmi_out_group_list_t* check_list_ptr);

ADSPResult hdmi_output_group_list_full_release(void);

#endif //AFE_HDMI_OUTPUT_DRV_UTILS_I_H



