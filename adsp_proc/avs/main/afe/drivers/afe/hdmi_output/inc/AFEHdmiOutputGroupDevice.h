/*========================================================================
   This file contains declerations to program HDMI hardware registers

  Copyright (c) 2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/inc/AFEHdmiOutputGroupDevice.h#1 $
 ====================================================================== */

#ifndef _AFE_HDMI_OUTPUT_GROUP_DEVICE_H
#define _AFE_HDMI_OUTPUT_GROUP_DEVICE_H


#include "AFEHdmiOutputDrvUtils_i.h"

/*==========================================================================
  Macro Defines
  ========================================================================== */
#define MAX_NUM_SUB_STREAM_SUPPORTED                     4
#define MIN_NUM_SUB_STREAM_SUPPORTED                     2

/** Possible group device state. */
typedef enum
{
   AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP = 0,   
   AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_START,     
   AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_UPDATE        
} afe_hdmi_output_group_device_state;


typedef struct afe_hdmi_output_group_device_state_struct
{
   afe_hdmi_output_group_device_state        group_state;

   bool_t                        is_enabled;
                                 /**< Flag to indicate if group is in enable or disable state */
   bool_t                        is_config_rcvd;
                                 /**< Flag to indicate if group config is rcvd */

   hdmi_out_group_list_t        add_list;
   hdmi_out_group_list_t        remove_list;
   hdmi_out_group_list_t        start_list;
   hdmi_out_group_list_t        stop_list;
   
}afe_hdmi_output_group_device_state_struct_t;

/*==========================================================================
  Function Declerations
  ========================================================================== */

afe_hdmi_output_group_device_state_struct_t* afe_get_hdmi_output_group_device_state_object(void);

ADSPResult afe_hdmi_output_set_group_device_module_params(uint32_t param_id,
                                                          int8_t *param_buffer_ptr,
                                                          uint32_t param_size);

#endif



