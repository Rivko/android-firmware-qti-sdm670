/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/src/AFETdmGroupDevice.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     

DESCRIPTION: Main Interface to the AFE TDM Group driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/src/AFETdmGroupDevice.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEGroupDeviceDriver.h"
#include "AFETdmGroupDevice_i.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"
#include "AFETdmDriver_i.h"
#include "AFELoggingUtils.h"

const uint16_t primiary_rx_tdm_member_port_list[] = {
  AFE_PORT_ID_PRIMARY_TDM_RX,   AFE_PORT_ID_PRIMARY_TDM_RX_1, AFE_PORT_ID_PRIMARY_TDM_RX_2, AFE_PORT_ID_PRIMARY_TDM_RX_3,
  AFE_PORT_ID_PRIMARY_TDM_RX_4, AFE_PORT_ID_PRIMARY_TDM_RX_5, AFE_PORT_ID_PRIMARY_TDM_RX_6, AFE_PORT_ID_PRIMARY_TDM_RX_7
};

const uint16_t secondary_rx_tdm_member_port_list[] = {
  AFE_PORT_ID_SECONDARY_TDM_RX,   AFE_PORT_ID_SECONDARY_TDM_RX_1, AFE_PORT_ID_SECONDARY_TDM_RX_2, AFE_PORT_ID_SECONDARY_TDM_RX_3,
  AFE_PORT_ID_SECONDARY_TDM_RX_4, AFE_PORT_ID_SECONDARY_TDM_RX_5, AFE_PORT_ID_SECONDARY_TDM_RX_6, AFE_PORT_ID_SECONDARY_TDM_RX_7
};
    
const uint16_t tertiary_rx_tdm_member_port_list[] = {
  AFE_PORT_ID_TERTIARY_TDM_RX,   AFE_PORT_ID_TERTIARY_TDM_RX_1, AFE_PORT_ID_TERTIARY_TDM_RX_2, AFE_PORT_ID_TERTIARY_TDM_RX_3,
  AFE_PORT_ID_TERTIARY_TDM_RX_4, AFE_PORT_ID_TERTIARY_TDM_RX_5, AFE_PORT_ID_TERTIARY_TDM_RX_6, AFE_PORT_ID_TERTIARY_TDM_RX_7
};

const uint16_t quaternary_rx_tdm_member_port_list[] = {
  AFE_PORT_ID_QUATERNARY_TDM_RX,   AFE_PORT_ID_QUATERNARY_TDM_RX_1, AFE_PORT_ID_QUATERNARY_TDM_RX_2, AFE_PORT_ID_QUATERNARY_TDM_RX_3,
  AFE_PORT_ID_QUATERNARY_TDM_RX_4, AFE_PORT_ID_QUATERNARY_TDM_RX_5, AFE_PORT_ID_QUATERNARY_TDM_RX_6, AFE_PORT_ID_QUATERNARY_TDM_RX_7
};

const uint16_t quinary_rx_tdm_member_port_list[] = {
    AFE_PORT_ID_QUINARY_TDM_RX,   AFE_PORT_ID_QUINARY_TDM_RX_1, AFE_PORT_ID_QUINARY_TDM_RX_2, AFE_PORT_ID_QUINARY_TDM_RX_3,
    AFE_PORT_ID_QUINARY_TDM_RX_4, AFE_PORT_ID_QUINARY_TDM_RX_5, AFE_PORT_ID_QUINARY_TDM_RX_6, AFE_PORT_ID_QUINARY_TDM_RX_7
};

const uint16_t primiary_tx_tdm_member_port_list[] = {
  AFE_PORT_ID_PRIMARY_TDM_TX,   AFE_PORT_ID_PRIMARY_TDM_TX_1, AFE_PORT_ID_PRIMARY_TDM_TX_2, AFE_PORT_ID_PRIMARY_TDM_TX_3,
  AFE_PORT_ID_PRIMARY_TDM_TX_4, AFE_PORT_ID_PRIMARY_TDM_TX_5, AFE_PORT_ID_PRIMARY_TDM_TX_6, AFE_PORT_ID_PRIMARY_TDM_TX_7
};

const uint16_t secondary_tx_tdm_member_port_list[] = {
  AFE_PORT_ID_SECONDARY_TDM_TX,   AFE_PORT_ID_SECONDARY_TDM_TX_1, AFE_PORT_ID_SECONDARY_TDM_TX_2, AFE_PORT_ID_SECONDARY_TDM_TX_3,
  AFE_PORT_ID_SECONDARY_TDM_TX_4, AFE_PORT_ID_SECONDARY_TDM_TX_5, AFE_PORT_ID_SECONDARY_TDM_TX_6, AFE_PORT_ID_SECONDARY_TDM_TX_7
};
    
const uint16_t tertiary_tx_tdm_member_port_list[] = {
  AFE_PORT_ID_TERTIARY_TDM_TX,   AFE_PORT_ID_TERTIARY_TDM_TX_1, AFE_PORT_ID_TERTIARY_TDM_TX_2, AFE_PORT_ID_TERTIARY_TDM_TX_3,
  AFE_PORT_ID_TERTIARY_TDM_TX_4, AFE_PORT_ID_TERTIARY_TDM_TX_5, AFE_PORT_ID_TERTIARY_TDM_TX_6, AFE_PORT_ID_TERTIARY_TDM_TX_7
};

const uint16_t quaternary_tx_tdm_member_port_list[] = {
  AFE_PORT_ID_QUATERNARY_TDM_TX,   AFE_PORT_ID_QUATERNARY_TDM_TX_1, AFE_PORT_ID_QUATERNARY_TDM_TX_2, AFE_PORT_ID_QUATERNARY_TDM_TX_3,
  AFE_PORT_ID_QUATERNARY_TDM_TX_4, AFE_PORT_ID_QUATERNARY_TDM_TX_5, AFE_PORT_ID_QUATERNARY_TDM_TX_6, AFE_PORT_ID_QUATERNARY_TDM_TX_7
};

const uint16_t quinary_tx_tdm_member_port_list[] = {
  AFE_PORT_ID_QUINARY_TDM_TX,   AFE_PORT_ID_QUINARY_TDM_TX_1, AFE_PORT_ID_QUINARY_TDM_TX_2, AFE_PORT_ID_QUINARY_TDM_TX_3,
  AFE_PORT_ID_QUINARY_TDM_TX_4, AFE_PORT_ID_QUINARY_TDM_TX_5, AFE_PORT_ID_QUINARY_TDM_TX_6, AFE_PORT_ID_QUINARY_TDM_TX_7
};


const uint16_t group_id_list[] = {
  AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX,    AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX,
  AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX,  AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX,
  AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX,   AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX,
  AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX, AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX,
  AFE_GROUP_DEVICE_ID_QUINARY_TDM_RX,    AFE_GROUP_DEVICE_ID_QUINARY_TDM_TX
};


/* TDM group device structure to track TDM specific info */
static afe_tdm_group_device_state_struct_t group_device_rx_primary_tdm;
static afe_tdm_group_device_state_struct_t group_device_tx_primary_tdm;

static afe_tdm_group_device_state_struct_t group_device_rx_secondary_tdm;
static afe_tdm_group_device_state_struct_t group_device_tx_secondary_tdm;

static afe_tdm_group_device_state_struct_t group_device_rx_tertiary_tdm;
static afe_tdm_group_device_state_struct_t group_device_tx_tertiary_tdm;

static afe_tdm_group_device_state_struct_t group_device_rx_quaternary_tdm;
static afe_tdm_group_device_state_struct_t group_device_tx_quaternary_tdm;

static afe_tdm_group_device_state_struct_t group_device_rx_quinary_tdm;
static afe_tdm_group_device_state_struct_t group_device_tx_quinary_tdm;

static bool_t afe_tdm_validate_id(uint16_t id, const uint16_t supported_list[], uint16_t num_items)
{
   uint16_t i;

   for(i = 0; i < num_items; i++)
   {
      if(supported_list[i] == id)
      {
         return true;
      }
   }
   return false;
}


static uint32_t afe_group_get_int_samples_per_period(uint32_t sample_rate)
{
   uint32_t int_samples_per_period = 0;


    switch (sample_rate)
    {
       case 48000:
       case 32000:
       case 24000:
       case 16000:
       case 8000:
          int_samples_per_period = sample_rate/1000;
          break;
       default:
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported sample rate: %lu", sample_rate);
          break;
    }


    return int_samples_per_period;
}

static bool_t afe_validate_tdm_group_member_port_id(uint16_t group_dev_id, uint16_t member_port_id)
{
   bool_t is_valid_port_id = FALSE;

   /* Check if the member port ID is supported for this group */
   switch (group_dev_id)
   {
      case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX:
         return afe_tdm_validate_id(member_port_id, primiary_rx_tdm_member_port_list, 
            sizeof(primiary_rx_tdm_member_port_list)/sizeof(uint16_t));
         break;
      case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX:
         return afe_tdm_validate_id(member_port_id, primiary_tx_tdm_member_port_list, 
             sizeof(primiary_tx_tdm_member_port_list)/sizeof(uint16_t));
         break;
      case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX:
          return afe_tdm_validate_id(member_port_id, secondary_rx_tdm_member_port_list, 
              sizeof(secondary_rx_tdm_member_port_list)/sizeof(uint16_t));
          break;
      case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX:
          return afe_tdm_validate_id(member_port_id, secondary_tx_tdm_member_port_list, 
              sizeof(secondary_tx_tdm_member_port_list)/sizeof(uint16_t));
          break;
      case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX:
          return afe_tdm_validate_id(member_port_id, tertiary_rx_tdm_member_port_list, 
              sizeof(tertiary_rx_tdm_member_port_list)/sizeof(uint16_t));
          break;
      case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX:
          return afe_tdm_validate_id(member_port_id, tertiary_tx_tdm_member_port_list, 
              sizeof(tertiary_tx_tdm_member_port_list)/sizeof(uint16_t));
          break;
      case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX:
          return afe_tdm_validate_id(member_port_id, quaternary_rx_tdm_member_port_list, 
              sizeof(quaternary_rx_tdm_member_port_list)/sizeof(uint16_t));
          break;
      case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX:
          return afe_tdm_validate_id(member_port_id, quaternary_tx_tdm_member_port_list, 
              sizeof(quaternary_tx_tdm_member_port_list)/sizeof(uint16_t));
          break;
      case AFE_GROUP_DEVICE_ID_QUINARY_TDM_RX:
          return afe_tdm_validate_id(member_port_id, quinary_rx_tdm_member_port_list,
              sizeof(quinary_rx_tdm_member_port_list)/sizeof(uint16_t));
          break;
      case AFE_GROUP_DEVICE_ID_QUINARY_TDM_TX:
          return afe_tdm_validate_id(member_port_id, quinary_tx_tdm_member_port_list,
              sizeof(quinary_tx_tdm_member_port_list)/sizeof(uint16_t));
          break;

      default:
         break;
   } /* End of group ID switch */

   return is_valid_port_id;
}

static ADSPResult afe_get_tdm_group_device_state_object(afe_group_device_state_struct_t **p_group_device,
      afe_tdm_group_device_state_struct_t **p_tdm_group_device, uint16_t group_dev_id)
{
   ADSPResult result = ADSP_EOK;

   switch (group_dev_id)
   {
      case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX:
         {
            *p_group_device = &group_device_rx_primary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_rx_primary_tdm;
         }
         break;
      case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX:
         {
            *p_group_device = &group_device_tx_primary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_tx_primary_tdm;
         }
         break;
      case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX:
         {
            *p_group_device = &group_device_rx_secondary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_rx_secondary_tdm;
         }
         break;
      case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX:
         {
            *p_group_device = &group_device_tx_secondary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_tx_secondary_tdm;
         }
         break;
      case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX:
         {
            *p_group_device = &group_device_rx_tertiary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_rx_tertiary_tdm;
         }
         break;
      case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX:
         {
            *p_group_device = &group_device_tx_tertiary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_tx_tertiary_tdm;
         }
         break;
      case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX:
         {
            *p_group_device = &group_device_rx_quaternary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_rx_quaternary_tdm;
         }
         break;
      case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX:
         {
            *p_group_device = &group_device_tx_quaternary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_tx_quaternary_tdm;
         }
         break;
      case AFE_GROUP_DEVICE_ID_QUINARY_TDM_RX:
         {
            *p_group_device = &group_device_rx_quinary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_rx_quinary_tdm;
         }
         break;
      case AFE_GROUP_DEVICE_ID_QUINARY_TDM_TX:
         {
            *p_group_device = &group_device_tx_quinary_tdm.group_device_common_state;
            *p_tdm_group_device = &group_device_tx_quinary_tdm;
         }
         break;
      default:
         result = ADSP_EFAILED;
         break;
   }

   return result;
}


static ADSPResult afe_configure_tdm_group_device (afe_param_id_group_device_tdm_cfg_t *p_group_dev_tdm_cfg)
{
   ADSPResult                             result = ADSP_EOK;
   uint16_t                               port_idx=0;
   afe_group_device_state_struct_t        *p_group_device;
   afe_tdm_group_device_state_struct_t    *p_tdm_group_device;
   afe_dev_port_t                         *p_dev_port;

   // Add DEBUG PRINT for CFG.
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "TDM group config version: %d with group id: 0x%x", 
     p_group_dev_tdm_cfg->group_device_cfg_minor_version, p_group_dev_tdm_cfg->group_id);
   MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "With port id: [0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x]",
     p_group_dev_tdm_cfg->port_id[0], p_group_dev_tdm_cfg->port_id[1], p_group_dev_tdm_cfg->port_id[2], 
     p_group_dev_tdm_cfg->port_id[3], p_group_dev_tdm_cfg->port_id[4], p_group_dev_tdm_cfg->port_id[5], 
     p_group_dev_tdm_cfg->port_id[6], p_group_dev_tdm_cfg->port_id[7]);
   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "With num-chns: %d samp-rate: %d bitwidth: %d nslots: %d slotwidth: %d slotmask: %x",
     p_group_dev_tdm_cfg->num_channels, p_group_dev_tdm_cfg->sample_rate, p_group_dev_tdm_cfg->bit_width,
     p_group_dev_tdm_cfg->nslots_per_frame, p_group_dev_tdm_cfg->slot_width, p_group_dev_tdm_cfg->slot_mask);


   /* Fetch the state struct object for this group ID */
   if( ADSP_EOK != (result = afe_get_tdm_group_device_state_object(&p_group_device, &p_tdm_group_device, 
     p_group_dev_tdm_cfg->group_id)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get group device object, group_id: 0x%x", 
        p_group_dev_tdm_cfg->group_id);
      return ADSP_EFAILED;
   }

   /* Check if this group device is already in RUN state */
   if (AFE_GROUP_DEVICE_STATE_RUN == p_group_device->group_state)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group config failed, all member ports are not in stop state, group_id: 0x%x", 
        p_group_dev_tdm_cfg->group_id);
      return ADSP_EFAILED;
   }

   /* Check if this group device is already enabled */
   if (TRUE == p_group_device->is_enabled)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group config failed, already enabled, group_id: 0x%x", p_group_dev_tdm_cfg->group_id);
      return ADSP_EALREADY;
   }

   /* Group is either in CONFIG or DISABLE state now, and command is sent to (re)configure the group */
   memset((void *)p_group_device, 0, sizeof(afe_group_device_state_struct_t));

   for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
   {
      /* Skip the invalid entries */
      if(0 == p_group_dev_tdm_cfg->port_id[port_idx])
      {
        continue;
      }
      if(AFE_PORT_ID_INVALID == p_group_dev_tdm_cfg->port_id[port_idx])
      {
        continue;
      }

      if ( ADSP_EOK != (afe_svc_get_port(p_group_dev_tdm_cfg->port_id[port_idx], &p_group_device->p_dev_port[port_idx])) )
      {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device failed to get the obj for port_id: 0x%x",
                 p_group_dev_tdm_cfg->port_id[port_idx]);

            return ADSP_EFAILED;
      }

      p_dev_port = p_group_device->p_dev_port[port_idx];

      /* Check if this port ID is valid member of this group */
      if ( FALSE == afe_validate_tdm_group_member_port_id(p_group_dev_tdm_cfg->group_id, p_dev_port->intf) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device invalid member port_id: 0x%x, group_id 0x%x",
                  p_dev_port->intf, p_group_dev_tdm_cfg->group_id);
            return ADSP_EFAILED;
      }

      /* Update group data member pointer to point to this port */
      p_group_device->p_dev_port[port_idx] = p_dev_port;

      /* Clear the dev port pointer pointer to group device object
         This will be set when group device is enabled */
      p_dev_port->p_group_device_state = NULL;

      /* Both group and member points to same device driver object */
      p_group_device->p_dev_drv_state = p_dev_port->afe_drv;
   }

   /* sample rate for this group */
   p_group_device->sample_rate = p_group_dev_tdm_cfg->sample_rate;

   /* Error check for bit width */
   if ((16 != p_group_dev_tdm_cfg->bit_width)&&
       (24 != p_group_dev_tdm_cfg->bit_width) &&
       (32 != p_group_dev_tdm_cfg->bit_width))
   {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit_width: %d for group id 0x%x", p_group_dev_tdm_cfg->bit_width, p_group_dev_tdm_cfg->group_id);
     return ADSP_EUNSUPPORTED;
   }

   /* bit width of this group */
   p_group_device->bit_width = p_group_dev_tdm_cfg->bit_width;

   // Validate number of channels
   if(p_group_dev_tdm_cfg->num_channels > 16)
   {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM num channels: %lu, group id 0x%x", p_group_dev_tdm_cfg->num_channels, p_group_dev_tdm_cfg->group_id);
     return ADSP_EUNSUPPORTED;
   }

   /* Num channels for this group */
   p_group_device->num_channels = p_group_dev_tdm_cfg->num_channels;

   /* Bytes per sample initialized */
   p_group_device->bytes_per_channel = (p_group_dev_tdm_cfg->bit_width > 16) ? 4 : 2;

   /* Default samples per period initialized to 48 */
   p_group_device->int_samples_per_period = afe_group_get_int_samples_per_period(p_group_dev_tdm_cfg->sample_rate);

   /* Derive the frame size and number of group */
   /* note ; there is no use case that has different sampling rate for group and member port */
   p_group_device->frame_size = p_group_device->num_channels * p_group_device->bytes_per_channel;
   /* tdm interface is handling only 1 frame */
   p_group_device->num_frame_in_group = 1;

   /* Save the group id for future ref */
   p_group_device->group_id = p_group_dev_tdm_cfg->group_id;
   
   /* Save the TDM specific group info for future ref */
   p_tdm_group_device->nslots_per_frame = p_group_dev_tdm_cfg->nslots_per_frame;
   p_tdm_group_device->slot_width = p_group_dev_tdm_cfg->slot_width;
   p_tdm_group_device->slot_mask = p_group_dev_tdm_cfg->slot_mask;
   
   /* validate active slot mask with num_channels*/
   if (ADSP_FAILED(afe_tdm_validate_slotmask_num_channels(p_group_dev_tdm_cfg->slot_mask, p_group_dev_tdm_cfg->num_channels ))) 
   {
     return ADSP_EBADPARAM;
   }
   /* Init the DMA signal for the group */
   if (ADSP_FAILED(result = qurt_elite_signal_init(&(p_group_device->dma_signal))))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to init signal for group device, group_dev_id: 0x%x, result: %d",
            p_group_device->group_id, result);
      return result;
   }

   /* Init group state to stop */
   p_group_device->group_state = AFE_GROUP_DEVICE_STATE_STOP;

   /* Pointer to working buffer for the group init to NULL */
   p_group_device->p_group_io_buf = NULL;

   /* Update the group config state to CONFIG */
   p_group_device->is_config_rcvd = TRUE;

   
   return result;
}


ADSPResult afe_tdm_set_group_device_module_params(uint32_t param_id,
                                                          int8_t *param_buffer_ptr,
                                                          uint32_t param_size)
{
    uint32_t group_dev_cfg_version;
    ADSPResult  result = ADSP_EOK;
    afe_param_id_group_device_tdm_cfg_t *p_group_dev_tdm_cfg = NULL;
    uint16_t                            port_idx = 0;
    afe_dev_port_t                      *p_dev_port = NULL;
    afe_group_device_state_struct_t     *p_group_device = NULL;
    afe_tdm_group_device_state_struct_t *p_tdm_group_device = NULL;
    uint16_t                            enable_flag;
    afe_param_id_group_device_enable_t  *p_group_device_cfg_enable;

    switch (param_id)
    {
       case AFE_PARAM_ID_GROUP_DEVICE_TDM_CONFIG:
          {
             group_dev_cfg_version = (uint32_t) *(param_buffer_ptr);

             if(group_dev_cfg_version > AFE_API_VERSION_GROUP_DEVICE_TDM_CONFIG)
             {
               group_dev_cfg_version = AFE_API_VERSION_GROUP_DEVICE_TDM_CONFIG;
             }
             if(AFE_API_VERSION_GROUP_DEVICE_TDM_CONFIG != group_dev_cfg_version)
             {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device Cfg enable/disable, invalid payload size: %lu", param_size);
               return ADSP_EBADPARAM;
             }

             /* Check for the valid param size */
             if (param_size < sizeof(afe_param_id_group_device_tdm_cfg_t))
             {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM Group Device Cfg enable/disable, invalid payload size: %lu", param_size);
                return ADSP_EBADPARAM;
             }

             p_group_dev_tdm_cfg =  (afe_param_id_group_device_tdm_cfg_t *)param_buffer_ptr;

             /* Validate group device ID */
             if(FALSE == afe_tdm_validate_id(p_group_dev_tdm_cfg->group_id, group_id_list, sizeof(group_id_list)/sizeof(uint16_t)))
             {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM Group Device Cfg invalid group_dev_id: 0x%x", p_group_dev_tdm_cfg->group_id);
                   return ADSP_EBADPARAM;
             }
             
             /* Setup member ports info for group and init group state to STOP */
             if ( ADSP_EOK != (result = afe_configure_tdm_group_device(p_group_dev_tdm_cfg)))
             {
                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to set TDM group device config, group_dev_id: 0x%x, result: %d",
                      p_group_dev_tdm_cfg->group_id, result);
                return result;
             }
             MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "TDM Group device config success, group_dev_id: 0x%x", p_group_dev_tdm_cfg->group_id);
          }
          break;
      case AFE_PARAM_ID_GROUP_DEVICE_ENABLE:
         {
            p_group_device_cfg_enable =  (afe_param_id_group_device_enable_t *)param_buffer_ptr;

            if(FALSE == afe_tdm_validate_id(p_group_device_cfg_enable->group_id, group_id_list, sizeof(group_id_list)/sizeof(uint16_t)))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM Group Device enable/disable invalid group_dev_id: 0x%x", p_group_device_cfg_enable->group_id);
                  return ADSP_EBADPARAM;
            }

            /* Fetch the state struct object for this group ID */
            if( ADSP_EOK != (result = afe_get_tdm_group_device_state_object(&p_group_device, &p_tdm_group_device, 
                 p_group_device_cfg_enable->group_id) ) )
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get group device object, group_dev_id: 0x%x", p_group_device_cfg_enable->group_id);
               return ADSP_EFAILED;
            }

            /* Check if this group device is already in RUN state */
            if (AFE_GROUP_DEVICE_STATE_RUN == p_group_device->group_state)
            {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device enable/disable failed, all member ports are not in stop state, group_id: 0x%x",
                     p_group_device_cfg_enable->group_id);
                return ADSP_EFAILED;
            }
               
            enable_flag = p_group_device_cfg_enable->enable;

            /* If group is enabled before configuring the group device */
            if ( (FALSE == p_group_device->is_config_rcvd) && (TRUE == enable_flag) )
            {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device enable failed, not configured, group_id: 0x%x",
                   p_group_device_cfg_enable->group_id);

                return ADSP_ENOTREADY;
            }

            for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
            {
               if (NULL != (p_dev_port = p_group_device->p_dev_port[port_idx]))
               {
                  /* If group disable, decouple the group object from the port */
                  if (FALSE == enable_flag)
                  {
                     p_dev_port->p_group_device_state = NULL;
                  }
                  else /* Group enable, restore the group coupling with port */
                  {
                     p_dev_port->p_group_device_state = p_group_device;
                  }
               }
            }

            /* Set the group device enable / disable state */
            p_group_device->is_enabled = enable_flag;
              
            if(FALSE == enable_flag)
            {
               afe_group_data_logging_deinit(p_group_device_cfg_enable->group_id, p_group_device);
            }
            else
            {
               afe_group_data_logging_init(p_group_device_cfg_enable->group_id, p_group_device);
            }

              
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Group device enable/disable success, group_dev_id: 0x%x, enable: %u",
                     p_group_device_cfg_enable->group_id, enable_flag);
         }
         break;
       default:
          {
             MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Un-supported group device param_id: 0x%lx",
                  param_id);
          }
          break;
    } /* End of group_dev_cfg_version switch case*/

   return result;
}

/**
  @brief save tdm group port config and error check for the params.

  @param[in] p_dev_state pointer for AFE dev port structure
  @param[in] tdm_cfg_ptr   pointer to the configuration structure

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_dev_group_config_tdm_port(tdm_port_state_t * p_dev_state, afe_dev_grp_config_info_t * tdm_grp_cfg_ptr)
{
   ADSPResult result = ADSP_EOK;
   afe_group_device_state_struct_t        *p_group_device;
   afe_tdm_group_device_state_struct_t    *p_tdm_group_device;

   /* Fetch the state struct object for this group ID */
   if( ADSP_EOK != (result = afe_get_tdm_group_device_state_object(&p_group_device, &p_tdm_group_device, 
     tdm_grp_cfg_ptr->group_id)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get group device object, group_id: 0x%x", 
        tdm_grp_cfg_ptr->group_id);
      return ADSP_EFAILED;
   }

   // Printing the device group configuration information
   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "tdm grp id:0x%x, grp sample_rate:%d, grp bitwitdth %d, grp num_channels:%d, bytes_per_channels:%d",\
            p_group_device->group_id, p_group_device->sample_rate, p_group_device->bit_width, p_group_device->num_channels, \
            p_group_device->bytes_per_channel);

   
   
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "grp bytes_ nslots[%d] slot_width[%d] slotmask[%x] Qfactor to 0", \
     p_tdm_group_device->nslots_per_frame, p_tdm_group_device->slot_width, p_tdm_group_device->slot_mask);

   /* If group is not enabled, return */
   if(0 == p_group_device->is_enabled)
   {
      return ADSP_EFAILED;
   }

   afe_param_id_tdm_cfg_t *tdm_cfg_ptr =  (afe_param_id_tdm_cfg_t *)tdm_grp_cfg_ptr->param_buffer_ptr;

   if (tdm_cfg_ptr->tdm_cfg_minor_version > AFE_API_VERSION_TDM_CONFIG)
   {
      tdm_cfg_ptr->tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
   }

   switch (tdm_cfg_ptr->tdm_cfg_minor_version)
   {
      case AFE_API_VERSION_TDM_CONFIG:
      {
         afe_param_id_tdm_cfg_t *tdm_cfg_ptr =  (afe_param_id_tdm_cfg_t *)tdm_grp_cfg_ptr->param_buffer_ptr;

         if (tdm_grp_cfg_ptr->param_size < sizeof(afe_param_id_tdm_cfg_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM Group get prop fail: Invalid payload size: %ld, i2s port index:%d, dir:%d",\
                     tdm_grp_cfg_ptr->param_size, p_dev_state->intf, p_dev_state->direction);
            result = ADSP_EBADPARAM;
         }
         else
         {
            // Printing the device configuration information
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe tdm dev config per port ");
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Device config Tdm Port Interface index:%d, direction:%d, cfg version:%lu", p_dev_state->intf, p_dev_state->direction, tdm_cfg_ptr->tdm_cfg_minor_version);
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "num_channels:%d, bit_width:%d, sample_rate:%d, data_format:%d", tdm_cfg_ptr->num_channels, tdm_cfg_ptr->bit_width, tdm_cfg_ptr->sample_rate, tdm_cfg_ptr->data_format);
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "sync_mode: %u sync_src:%u, nslots_per_frame:%d", tdm_cfg_ptr->sync_mode, tdm_cfg_ptr->sync_src, tdm_cfg_ptr->nslots_per_frame);
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ctrl_data_out_enable:%d, ctrl_invert_sync_pulse:%d, ctrl_sync_data_delay:%d",tdm_cfg_ptr->ctrl_data_out_enable, tdm_cfg_ptr->ctrl_invert_sync_pulse, tdm_cfg_ptr->ctrl_sync_data_delay);
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "slot_width: %lu slot_mask:%lu",tdm_cfg_ptr->slot_width, tdm_cfg_ptr->slot_mask);
    
            tdm_grp_cfg_ptr->port_prop.sample_rate = tdm_cfg_ptr->sample_rate; 
            tdm_grp_cfg_ptr->port_prop.channels = tdm_cfg_ptr->num_channels;
            tdm_grp_cfg_ptr->port_prop.bytes_per_channel = (tdm_cfg_ptr->bit_width > 16) ? 4 : 2;
            tdm_grp_cfg_ptr->port_prop.bit_width = tdm_cfg_ptr->bit_width;
            tdm_grp_cfg_ptr->port_prop.is_interleaved = TRUE;
            /* Determining the qformat shift factor based on bit width. We use this shift factor for 24bit only*/
            if (24 == tdm_cfg_ptr->bit_width)
            {
               tdm_grp_cfg_ptr->port_prop.q_format_shift_factor = QFORMAT_SHIFT_FACTOR;
            }
            else
            {
               tdm_grp_cfg_ptr->port_prop.q_format_shift_factor = 0;
            }

            // port data sturtcure to be filled.
            tdm_grp_cfg_ptr->port_prop.port_data_type = AFE_UNCOMPRESSED_DATA_PORT;
            if ((AFE_NON_LINEAR_DATA == tdm_cfg_ptr->data_format) || (AFE_NON_LINEAR_DATA_PACKED_60958 == tdm_cfg_ptr->data_format))
            {
               tdm_grp_cfg_ptr->port_prop.port_data_type = AFE_IEC61937_COMPRESSED_DATA_PORT;
            }
            
            //override tdm configs with group and static configuration data.
            tdm_cfg_ptr->data_format = AFE_LINEAR_PCM_DATA;

            tdm_grp_cfg_ptr->port_prop.frame_idx = 0;

            tdm_cfg_ptr->num_channels = p_group_device->num_channels;
            tdm_cfg_ptr->sample_rate = p_group_device->sample_rate;
            tdm_cfg_ptr->bit_width =  p_group_device->bit_width;
            tdm_cfg_ptr->nslots_per_frame = p_tdm_group_device->nslots_per_frame;
            tdm_cfg_ptr->slot_width = p_tdm_group_device->slot_width;
            tdm_cfg_ptr->slot_mask = p_tdm_group_device->slot_mask; 

         }
     }
     break;
     default:
        return ADSP_EUNSUPPORTED;
   }
   return result;
}



