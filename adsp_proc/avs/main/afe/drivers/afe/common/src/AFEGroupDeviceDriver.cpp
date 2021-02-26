/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEGroupDeviceDriver.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEDeviceDriver.cpp

DESCRIPTION: Main Interface to the AFE Device driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEGroupDeviceDriver.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/06/07    RA      Created file

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEGroupDeviceDriver.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"
#include "AFETdmGroupDevice.h"
#include "AFEI2sGroupDevice.h"
#include "AFEHdmiOutputGroupDevice.h"


const uint16_t group_port_list[] = {
  AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX, AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX,
  AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX,  AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX,
  AFE_GROUP_DEVICE_ID_HDMI_MULTISTREAM_RX,
  AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX, AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX,
  AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX, AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX,
  AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX, AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX,
  AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX, AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX,
  AFE_GROUP_DEVICE_ID_QUINARY_TDM_RX, AFE_GROUP_DEVICE_ID_QUINARY_TDM_TX

};


bool_t afe_svc_validate_group_dev_id(uint16_t group_dev_id)
{
   uint16_t i;

   for(i = 0; i < sizeof(group_port_list)/sizeof(uint16_t); i++)
   {
      if(group_port_list[i] == group_dev_id)
      {
         return true;
      }
   }

   return false;

}


ADSPResult afe_svc_set_group_device_module_params(uint32_t param_id,
                                                  int8_t *param_buffer_ptr,
                                                  uint32_t param_size)
{
   ADSPResult                          result = ADSP_EOK;
   afe_param_id_group_device_enable_t  *p_group_device_cfg_enable;

   switch(param_id)
   {
      case AFE_PARAM_ID_GROUP_DEVICE_ENABLE:
         {
            /* Check if payload size is valid */
            if (param_size < sizeof(afe_param_id_group_device_enable_t) )
            {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Group Device enable/disable failed, invalid payload size: %lu", param_size);
               return ADSP_EBADPARAM;
            }

            p_group_device_cfg_enable =  (afe_param_id_group_device_enable_t *)param_buffer_ptr;

            switch(p_group_device_cfg_enable->group_id)
            {
               case AFE_GROUP_DEVICE_ID_HDMI_MULTISTREAM_RX:
               {
                 result = afe_hdmi_output_set_group_device_module_params(param_id, param_buffer_ptr, param_size);
               }
               break;
               case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX:
               case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX:
               case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX:
               case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX:  
               case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX:     
               case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX:   
               case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX:  
               case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX:
               case AFE_GROUP_DEVICE_ID_QUINARY_TDM_RX:
               case AFE_GROUP_DEVICE_ID_QUINARY_TDM_TX:
               {
                 result = afe_tdm_set_group_device_module_params(param_id, param_buffer_ptr, param_size);
               }
               break;
               case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX:             
               case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX:                      
               case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX:                   
               case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX:     
               {
                  result = afe_i2s_set_group_device_module_params(param_id, param_buffer_ptr, param_size);
               }
               break;
               default:
               {
                 MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Uunsupported Group Device enable/disable failed, invalid payload size: %lu", param_size);
                  result = ADSP_EUNSUPPORTED;
               }
               break;
               }
         } /*End of AFE_PARAM_ID_ENABLE param */
         break;
      case AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG:
         {
            result = afe_i2s_set_group_device_module_params(param_id, param_buffer_ptr, param_size);
         }
         break;
      case AFE_PARAM_ID_GROUP_DEVICE_TDM_CONFIG:
         {
            result = afe_tdm_set_group_device_module_params(param_id, param_buffer_ptr, param_size);
         }
         break;
      case AFE_PARAM_ID_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG:
         {
            result = afe_hdmi_output_set_group_device_module_params(param_id, param_buffer_ptr, param_size);

         }
         break;
      default:
         result = ADSP_EUNSUPPORTED;
         break;
   } /* End of param_id switch case */

   return result;
}

