/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/common/src/AFEI2sGroupDevice.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/common/src/AFEI2sGroupDevice.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEGroupDeviceDriver.h"
#include "AFEI2sGroupDevice_i.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"
#include "AFEI2sDriver_i.h"
#include "AFELoggingUtils.h"

const uint16_t primiary_i2s_rx_member_port_list[] = {
  AFE_PORT_ID_PRIMARY_MI2S_RX,   AFE_PORT_ID_PRIMARY_MI2S_RX_1, AFE_PORT_ID_PRIMARY_MI2S_RX_2, AFE_PORT_ID_PRIMARY_MI2S_RX_3,
};

const uint16_t primiary_i2s_tx_member_port_list[] = {
  AFE_PORT_ID_PRIMARY_MI2S_TX,   AFE_PORT_ID_PRIMARY_MI2S_TX_1, AFE_PORT_ID_PRIMARY_MI2S_TX_2, AFE_PORT_ID_PRIMARY_MI2S_TX_3,
};


const uint16_t quaternary_i2s_rx_member_port_list[] = {
  AFE_PORT_ID_QUATERNARY_MI2S_RX,   AFE_PORT_ID_QUATERNARY_MI2S_RX_1, AFE_PORT_ID_QUATERNARY_MI2S_RX_2, AFE_PORT_ID_QUATERNARY_MI2S_RX_3,
  AFE_PORT_ID_QUATERNARY_MI2S_RX_4, AFE_PORT_ID_QUATERNARY_MI2S_RX_5, AFE_PORT_ID_QUATERNARY_MI2S_RX_6, AFE_PORT_ID_QUATERNARY_MI2S_RX_7
};

const uint16_t quaternary_i2s_tx_member_port_list[] = {
  AFE_PORT_ID_QUATERNARY_MI2S_TX,   AFE_PORT_ID_QUATERNARY_MI2S_TX_1, AFE_PORT_ID_QUATERNARY_MI2S_TX_2, AFE_PORT_ID_QUATERNARY_MI2S_TX_3,
  AFE_PORT_ID_QUATERNARY_MI2S_TX_4, AFE_PORT_ID_QUATERNARY_MI2S_TX_5, AFE_PORT_ID_QUATERNARY_MI2S_TX_6, AFE_PORT_ID_QUATERNARY_MI2S_TX_7
};


const uint16_t group_id_list[] = {
  AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX,    AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX,
  AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX,  AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX,
};


/* I2S group device structure to track TDM specific info */
static afe_i2s_group_device_state_struct_t group_device_rx_primary_i2s;
static afe_i2s_group_device_state_struct_t group_device_tx_primary_i2s;

static afe_i2s_group_device_state_struct_t group_device_rx_quaternary_i2s;
static afe_i2s_group_device_state_struct_t group_device_tx_quaternary_i2s;

static bool_t afe_i2s_validate_id(uint16_t id, const uint16_t supported_list[], uint16_t num_items)
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
       case 192000:
       case 96000:
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

static bool_t afe_validate_i2s_group_member_port_id(uint16_t group_dev_id, uint16_t member_port_id)
{
   /* Check if the member port ID is supported for this group */
   switch (group_dev_id)
   {
      case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX:
         return afe_i2s_validate_id(member_port_id, primiary_i2s_rx_member_port_list, 
            sizeof(primiary_i2s_rx_member_port_list)/sizeof(uint16_t));
         break;
      case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX:
         return afe_i2s_validate_id(member_port_id, primiary_i2s_tx_member_port_list, 
             sizeof(primiary_i2s_tx_member_port_list)/sizeof(uint16_t));
         break;
      case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX:
          return afe_i2s_validate_id(member_port_id, quaternary_i2s_rx_member_port_list, 
              sizeof(quaternary_i2s_rx_member_port_list)/sizeof(uint16_t));
          break;
      case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX:
          return afe_i2s_validate_id(member_port_id, quaternary_i2s_tx_member_port_list, 
              sizeof(quaternary_i2s_tx_member_port_list)/sizeof(uint16_t));
          break;
      default:
         return FALSE;
         break;
   } /* End of group ID switch */

}

ADSPResult afe_get_i2s_group_device_state_object(afe_group_device_state_struct_t **p_group_device,
      afe_i2s_group_device_state_struct_t **p_i2s_group_device, uint16_t group_dev_id)
{
   ADSPResult result = ADSP_EOK;

   switch (group_dev_id)
   {
      case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX:
         {
            *p_group_device = &group_device_rx_primary_i2s.group_device_common_state;
            *p_i2s_group_device = &group_device_rx_primary_i2s;
         }
         break;
      case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX:
         {
            *p_group_device = &group_device_tx_primary_i2s.group_device_common_state;
            *p_i2s_group_device = &group_device_tx_primary_i2s;
         }
         break;
      case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX:
         {
            *p_group_device = &group_device_rx_quaternary_i2s.group_device_common_state;
            *p_i2s_group_device = &group_device_rx_quaternary_i2s;
         }
         break;
      case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX:
         {
            *p_group_device = &group_device_tx_quaternary_i2s.group_device_common_state;
            *p_i2s_group_device = &group_device_tx_quaternary_i2s;
         }
         break;
      default:
         result = ADSP_EFAILED;
         break;
   }

   return result;
}

uint16_t afe_group_get_mi2s_num_channel(uint16_t channel_mode)
{
   uint16_t num_channels = 0;

   switch (channel_mode)
    {
       case AFE_PORT_I2S_SD0:
       case AFE_PORT_I2S_SD1:
       case AFE_PORT_I2S_SD2:
       case AFE_PORT_I2S_SD3:
          {
             num_channels = 2;
          }
          break;
       case AFE_PORT_I2S_QUAD01:
       case AFE_PORT_I2S_QUAD23:
          {
             num_channels = 4;
          }
          break;
       case AFE_PORT_I2S_6CHS:
          {
             num_channels = 6;
          }
          break;
       case AFE_PORT_I2S_8CHS:
          {
             num_channels = 8;
          }
          break;
       default:
          {
             MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported i2s channel_mode: 0x%x", channel_mode);
           }
    }

   return num_channels;
}


static ADSPResult afe_configure_i2s_group_device (afe_param_id_group_device_i2s_cfg_v1_t *p_group_dev_i2s_cfg)
{
   ADSPResult                             result = ADSP_EOK;
   uint16_t                               port_idx=0;
   afe_group_device_state_struct_t        *p_group_device;
   afe_i2s_group_device_state_struct_t    *p_i2s_group_device;
   afe_dev_port_t                         *p_dev_port;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "I2S group config version: %d with group id: 0x%x", 
     p_group_dev_i2s_cfg->minor_version, p_group_dev_i2s_cfg->group_id);
   MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "With port id: [0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x]",
     p_group_dev_i2s_cfg->port_id[0], p_group_dev_i2s_cfg->port_id[1], p_group_dev_i2s_cfg->port_id[2], 
     p_group_dev_i2s_cfg->port_id[3], p_group_dev_i2s_cfg->port_id[4], p_group_dev_i2s_cfg->port_id[5], 
     p_group_dev_i2s_cfg->port_id[6], p_group_dev_i2s_cfg->port_id[7]);
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "samp-rate: %d bitwidth: %d channel mode: %d",
     p_group_dev_i2s_cfg->sample_rate, p_group_dev_i2s_cfg->bit_width, p_group_dev_i2s_cfg->channel_mode);


   /* Fetch the state struct object for this group ID */
   if( ADSP_EOK != (result = afe_get_i2s_group_device_state_object(&p_group_device, &p_i2s_group_device, 
     p_group_dev_i2s_cfg->group_id)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get group device object, group_id: 0x%x", 
        p_group_dev_i2s_cfg->group_id);
      return ADSP_EFAILED;
   }

   /* Check if this group device is already in RUN state */
   if (AFE_GROUP_DEVICE_STATE_RUN == p_group_device->group_state)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group config failed, all member ports are not in stop state, group_id: 0x%x", 
        p_group_dev_i2s_cfg->group_id);
      return ADSP_EFAILED;
   }

   /* Check if this group device is already enabled */
   if (TRUE == p_group_device->is_enabled)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group config failed, already enabled, group_id: 0x%x", p_group_dev_i2s_cfg->group_id);
      return ADSP_EALREADY;
   }

   /* Group is either in CONFIG or DISABLE state now, and command is sent to (re)configure the group */
   memset((void *)p_group_device, 0, sizeof(afe_group_device_state_struct_t));

   for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
   {
      if (0 != p_group_dev_i2s_cfg->port_id[port_idx]) 
      {
         if ( ADSP_EOK != (afe_svc_get_port(p_group_dev_i2s_cfg->port_id[port_idx], &p_group_device->p_dev_port[port_idx])) )
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device failed to get the obj for port_id: 0x%x",
                 p_group_dev_i2s_cfg->port_id[port_idx]);

            return ADSP_EFAILED;
         }

         p_dev_port = p_group_device->p_dev_port[port_idx];

         /* Check if this port ID is valid member of this group */
         if ( FALSE == afe_validate_i2s_group_member_port_id(p_group_dev_i2s_cfg->group_id, p_dev_port->intf) )
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device invalid member port_id: 0x%x, group_id 0x%x",
                     p_dev_port->intf, p_group_dev_i2s_cfg->group_id);
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
   }

   /* sample rate for this group */
   p_group_device->sample_rate = p_group_dev_i2s_cfg->sample_rate;

   if ((AFE_BITS_PER_SAMPLE_16 != p_group_dev_i2s_cfg->bit_width)&&
       (AFE_BITS_PER_SAMPLE_24 != p_group_dev_i2s_cfg->bit_width) &&
       (AFE_BITS_PER_SAMPLE_32 != p_group_dev_i2s_cfg->bit_width))
   {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit_width: %d for group id 0x%x",\
           p_group_dev_i2s_cfg->bit_width, p_group_dev_i2s_cfg->group_id);
     return ADSP_EUNSUPPORTED;
   }

   /* bit width of this group */
   p_group_device->bit_width = p_group_dev_i2s_cfg->bit_width;

   /* Num channels for this group */
   p_group_device->num_channels =afe_group_get_mi2s_num_channel(p_group_dev_i2s_cfg->channel_mode);

   /* Bytes per sample initialized */
   p_group_device->bytes_per_channel = (p_group_dev_i2s_cfg->bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;

   /* Default samples per period initialized to 48 */
   p_group_device->int_samples_per_period = afe_group_get_int_samples_per_period(p_group_dev_i2s_cfg->sample_rate);

   /* Derive the frame size and number of group */
   /* note ; there is no use case that has different sampling rate for group and member port */
   p_group_device->frame_size = p_group_device->bytes_per_channel * 2;

   /* There are use cases for 1 and 2 frames in i2s and number of frame means the number of data line in i2s cases*/
   p_group_device->num_frame_in_group = p_group_device->num_channels >> 1;
   
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[i2s group num_frame:%d]  [i2s group frame_size: %d]", \
      p_group_device->num_frame_in_group, p_group_device->frame_size);

   /* Save the group id for future ref */
   p_group_device->group_id = p_group_dev_i2s_cfg->group_id;

   /* Save the I2S specific group info for future ref */
   p_i2s_group_device->channel_mode = p_group_dev_i2s_cfg->channel_mode;

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


ADSPResult afe_i2s_set_group_device_module_params(uint32_t param_id,
                                                          int8_t *param_buffer_ptr,
                                                          uint32_t param_size)
{
    uint32_t group_dev_cfg_version;
    ADSPResult  result = ADSP_EOK;
    afe_param_id_group_device_i2s_cfg_v1_t *p_group_dev_i2s_cfg = NULL;
    uint16_t                            port_idx = 0;
    afe_dev_port_t                      *p_dev_port = NULL;
    afe_group_device_state_struct_t     *p_group_device = NULL;
    afe_i2s_group_device_state_struct_t *p_i2s_group_device = NULL;
    uint16_t                            enable_flag;
    afe_param_id_group_device_enable_t  *p_group_device_cfg_enable;

    switch (param_id)
    {
       case AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG:
          {
             group_dev_cfg_version = (uint32_t) *(param_buffer_ptr);

             if(group_dev_cfg_version > AFE_API_VERSION_GROUP_DEVICE_I2S_CONFIG)
             {
               group_dev_cfg_version = AFE_API_VERSION_GROUP_DEVICE_I2S_CONFIG;
             }
             if(AFE_API_VERSION_GROUP_DEVICE_I2S_CONFIG != group_dev_cfg_version)
             {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device Cfg enable/disable, invalid payload size: %lu", param_size);
               return ADSP_EBADPARAM;
             }

             /* Check for the valid param size */
             if (param_size < sizeof(afe_param_id_group_device_i2s_cfg_v1_t))
             {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S Group Device Cfg enable/disable, invalid payload size: %lu", param_size);
                return ADSP_EBADPARAM;
             }

             p_group_dev_i2s_cfg =  (afe_param_id_group_device_i2s_cfg_v1_t *)param_buffer_ptr;

             /* Validate group device ID */
             if(FALSE == afe_i2s_validate_id(p_group_dev_i2s_cfg->group_id, group_id_list, sizeof(group_id_list)/sizeof(uint16_t)))
             {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S Group Device Cfg invalid group_dev_id: 0x%x", p_group_dev_i2s_cfg->group_id);
                   return ADSP_EBADPARAM;
             }

             /* Setup member ports info for group and init group state to STOP */
             if ( ADSP_EOK != (result = afe_configure_i2s_group_device(p_group_dev_i2s_cfg)))
             {
                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to set I2S group device config, group_dev_id: 0x%x, result: %d",
                      p_group_dev_i2s_cfg->group_id, result);
                return result;
             }
             MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "I2S Group device config success, group_dev_id: 0x%x", p_group_dev_i2s_cfg->group_id);
          }
          break;
      case AFE_PARAM_ID_GROUP_DEVICE_ENABLE:
         {
            p_group_device_cfg_enable =  (afe_param_id_group_device_enable_t *)param_buffer_ptr;

            if(FALSE == afe_i2s_validate_id(p_group_device_cfg_enable->group_id, group_id_list, sizeof(group_id_list)/sizeof(uint16_t)))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S Group Device enable/disable invalid group_dev_id: 0x%x", p_group_device_cfg_enable->group_id);
                  return ADSP_EBADPARAM;
            }

               /* Fetch the state struct object for this group ID */
            if( ADSP_EOK != (result = afe_get_i2s_group_device_state_object(&p_group_device, &p_i2s_group_device, 
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
  @brief save i2s group port config and error check for the params.

  @param[in] p_dev_state pointer for AFE dev port structure
  @param[in] i2s_cfg_ptr   pointer to the configuration structure
  @param[in] number of channels for group device

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_dev_group_config_i2s_port(i2s_port_state_t * p_dev_state, afe_dev_grp_config_info_t * i2s_grp_cfg_ptr)
{
   ADSPResult result = ADSP_EOK;
   afe_group_device_state_struct_t        *p_group_device;
   afe_i2s_group_device_state_struct_t    *p_i2s_group_device;

   /* Fetch the state struct object for this group ID */
   if( ADSP_EOK != (result = afe_get_i2s_group_device_state_object(&p_group_device, &p_i2s_group_device, 
     i2s_grp_cfg_ptr->group_id)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get group device object, group_id: 0x%x", 
        i2s_grp_cfg_ptr->group_id);
      return ADSP_EFAILED;
   }

   // Printing the device group configuration information
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "i2s grp id:0x%x, grp sample_rate:%d, grp bitwitdth %d, grp channel mode:%d",\
            p_group_device->group_id, p_group_device->sample_rate, p_group_device->bit_width, p_i2s_group_device->channel_mode);

   afe_param_id_i2s_cfg_t *i2s_cfg_ptr =  (afe_param_id_i2s_cfg_t *)i2s_grp_cfg_ptr->param_buffer_ptr;

   if (i2s_cfg_ptr->i2s_cfg_minor_version > AFE_API_VERSION_I2S_CONFIG)
   {
      i2s_cfg_ptr->i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
   }

   switch (i2s_cfg_ptr->i2s_cfg_minor_version)
   {
      case AFE_API_VERSION_I2S_CONFIG:
      {
         afe_param_id_i2s_cfg_t *i2s_cfg_ptr =  (afe_param_id_i2s_cfg_t *)i2s_grp_cfg_ptr->param_buffer_ptr;

         if (i2s_grp_cfg_ptr->param_size < sizeof(afe_param_id_i2s_cfg_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S Group get prop fail: Invalid payload size: %ld, i2s port index:%d, dir:%d",\
                     i2s_grp_cfg_ptr->param_size, p_dev_state->intf, p_dev_state->direction);
            result = ADSP_EBADPARAM;
         }
         else
         {
            // Printing the device configuration information
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe i2s dev config per port ");
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Device config I2S Port Interface index:%d, direction:%d, cfg version:%lu", p_dev_state->intf, p_dev_state->direction, i2s_cfg_ptr->i2s_cfg_minor_version);
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Bit width:%d, Channel Mode:%d, Data Format:%d", i2s_cfg_ptr->bit_width, i2s_cfg_ptr->channel_mode, i2s_cfg_ptr->data_format);
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MonoStereo: %u Sample Rate:%lu, MSM mode:%d", i2s_cfg_ptr->mono_stereo, i2s_cfg_ptr->sample_rate, i2s_cfg_ptr->ws_src);

            // need to check line capabilities here.
            switch (i2s_cfg_ptr->channel_mode)
            {
               //supported for all i2s interfaces
               case AFE_PORT_I2S_SD0:
               case AFE_PORT_I2S_SD1:
               case AFE_PORT_I2S_SD2:
               case AFE_PORT_I2S_SD3:
               {
                  if (i2s_cfg_ptr->mono_stereo == AFE_PORT_I2S_STEREO)
                     i2s_grp_cfg_ptr->port_prop.channels = 2;
                  else if (i2s_cfg_ptr->mono_stereo == AFE_PORT_I2S_MONO)
                     i2s_grp_cfg_ptr->port_prop.channels = 1;
                  else
                     result = ADSP_EBADPARAM;
               }
               break;
               case AFE_PORT_I2S_QUAD01:
               case AFE_PORT_I2S_QUAD23:
               {
                  i2s_grp_cfg_ptr->port_prop.channels = 4;
               }
                  break;
               default:
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported group device channel_mode: i2s intf %lu",  p_dev_state->intf);
                  result = ADSP_EBADPARAM;

               }
            }
            
            // override mono_stereo; group device should be stereo
            i2s_cfg_ptr->mono_stereo = AFE_PORT_I2S_STEREO; 

            /* Configure DMA buffer offset of this member port */
            switch (p_i2s_group_device->channel_mode)
            {
               case AFE_PORT_I2S_SD0:
               case AFE_PORT_I2S_SD1:
               case AFE_PORT_I2S_SD2:
               case AFE_PORT_I2S_SD3:
               {  /* This is only for TDM case */
                  i2s_grp_cfg_ptr->port_prop.frame_idx = 0;
               }   
               break;  
               case AFE_PORT_I2S_QUAD01:
               {
                  switch (i2s_cfg_ptr->channel_mode)
                  {
                     case AFE_PORT_I2S_SD0:
                     {
                        i2s_grp_cfg_ptr->port_prop.frame_idx = 0;
                     }
                     break;
                     case AFE_PORT_I2S_SD1:
                     {
                        i2s_grp_cfg_ptr->port_prop.frame_idx = 1;
                     }
                     break;
                     default:
                     {
                        result = ADSP_EUNSUPPORTED;
                     }
                     break;
                  }
               }   
               break;
               case AFE_PORT_I2S_QUAD23:
               {
                  switch (i2s_cfg_ptr->channel_mode)
                  {
                     case AFE_PORT_I2S_SD2:
                     {
                         i2s_grp_cfg_ptr->port_prop.frame_idx = 0;
                     }
                     break;
                     case AFE_PORT_I2S_SD3:
                     {
                         i2s_grp_cfg_ptr->port_prop.frame_idx = 1;
                     }
                     break;
                     default:
                     {
                        result = ADSP_EUNSUPPORTED;
                     }
                     break;
                  }
               }
               break;
               case    AFE_PORT_I2S_6CHS:
               {
                  switch (i2s_cfg_ptr->channel_mode)
                  {
                     case AFE_PORT_I2S_SD0:
                     case AFE_PORT_I2S_QUAD01:
                     {
                        i2s_grp_cfg_ptr->port_prop.frame_idx = 0;
                     }
                     break;
                     case AFE_PORT_I2S_SD1:
                     {
                        i2s_grp_cfg_ptr->port_prop.frame_idx = 1;
                     }
                     break;
                     case AFE_PORT_I2S_SD2:
                     {
                        i2s_grp_cfg_ptr->port_prop.frame_idx = 2;
                     }
                     break;
                     default:
                     {
                        result = ADSP_EUNSUPPORTED;
                     }
                     break;
                  }
               }
               case AFE_PORT_I2S_8CHS:   
               {
                  switch (i2s_cfg_ptr->channel_mode)
                  {
                     case AFE_PORT_I2S_SD0:
                     case AFE_PORT_I2S_QUAD01:
                     case AFE_PORT_I2S_6CHS:
                     {
                        i2s_grp_cfg_ptr->port_prop.frame_idx = 0;
                     }
                     break;
                     case AFE_PORT_I2S_SD1:
                     {
                        i2s_grp_cfg_ptr->port_prop.frame_idx = 1;
                     }
                     break;
                     case AFE_PORT_I2S_SD2:
                     case AFE_PORT_I2S_QUAD23:
                     {
                        i2s_grp_cfg_ptr->port_prop.frame_idx = 2;
                     }
                     break;
                     case AFE_PORT_I2S_SD3:
                     {
                        i2s_grp_cfg_ptr->port_prop.frame_idx = 3;
                     }
                     break;
                     default:
                     {
                        result = ADSP_EUNSUPPORTED;
                     }
                     break;
                  }
               }
            }

            /* Override the i2s channel mode with group device config */
            i2s_cfg_ptr->channel_mode = p_i2s_group_device->channel_mode;
         }
      }
      break;
      default:
         return ADSP_EUNSUPPORTED;
   }

   i2s_grp_cfg_ptr->port_prop.sample_rate = i2s_cfg_ptr->sample_rate;
   /* Override i2s sample_rate with group device config */
   i2s_cfg_ptr->sample_rate = p_group_device->sample_rate;

   i2s_grp_cfg_ptr->port_prop.bit_width = i2s_cfg_ptr->bit_width;

   /* Determining the qformat shift factor based on bit width. We use this shift factor for 24bit only*/
   if (AFE_BITS_PER_SAMPLE_24 == i2s_cfg_ptr->bit_width)
   {
      i2s_grp_cfg_ptr->port_prop.q_format_shift_factor = QFORMAT_SHIFT_FACTOR;
   }
   else
   {
      i2s_grp_cfg_ptr->port_prop.q_format_shift_factor = 0;
   }

   i2s_grp_cfg_ptr->port_prop.bytes_per_channel = (i2s_cfg_ptr->bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;
   /* Override i2s bit width with group device config */
   i2s_cfg_ptr->bit_width = p_group_device->bit_width;

   i2s_grp_cfg_ptr->port_prop.is_interleaved = TRUE;

   // port data sturtcure to be filled.
   i2s_grp_cfg_ptr->port_prop.port_data_type = AFE_UNCOMPRESSED_DATA_PORT;
   if ((AFE_NON_LINEAR_DATA == i2s_cfg_ptr->data_format) || (AFE_NON_LINEAR_DATA_PACKED_60958 == i2s_cfg_ptr->data_format))
   {
      i2s_grp_cfg_ptr->port_prop.port_data_type = AFE_IEC61937_COMPRESSED_DATA_PORT;
   }
   else if(AFE_DSD_DOP_DATA == p_dev_state->data_format)
   {
      i2s_grp_cfg_ptr->port_prop.port_data_type = AFE_DSD_DOP_COMPRESSED_DATA_PORT;
   }
   
   //override data format 
   i2s_cfg_ptr->data_format = AFE_LINEAR_PCM_DATA;

   return result;
}



