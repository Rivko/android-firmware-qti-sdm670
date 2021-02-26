/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHdmiOutputGroupDevice.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEHdmiOutputGroupDevice.cpp

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHdmiOutputGroupDevice.cpp#1 $

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

#include "AFEHdmiOutputGroupDevice.h"

#include "AFEHdmiOutputDrv_i.h"


//overall group_list management
//Group Cfg
//1. ACTION_START, build a start_list with allocate memeory for each node 
//                 set valid for each node to 1 in start_list
//2. ACTION_REMOVE, get the right node from start_list and build a remove_list
//                 set valid for each node to 1 in remove_list
//3. ACTION_ADD, build a add_list with allocate memeory for each node
//                 set valid for each node to 1 in add_list
//4. ACTION_STOP, get the right node from start_list and build a stop_list
//                 set valid for each node to 1 in stop_list

//AFE port start
//1. Start AFE ports after ACTION_START group cfg
//   1.1 Check if it is the first AFE port start in ACTION_START group
//   1.2 Start this port and mark "valid" to 0 in start_list
//   1.3 Check if it is the last AFE port start in ACTION_START group

//2. Start AFE ports after ACTION_ADD group cfg
//   2.1 Check if it is the first AFE port operation (start or stop) in ACTION_ADD and ACTION_REMOVE group
//       If it is, basing current started AFE port in start_list, to_add and to_remove AFE ports in add_list and remove_list, do the necessary stream route and save the final result in start_list
//   2.2 Start this port and mark "valid" to 0 in add_list
//   2.3 Check if it is the last AFE port operation in remove_list and add_list
//       if it is, fully reelase all node in remove_list and reset add_list to null. Note, the node allocated for add_list are added to start_list

//3. Stop AFE ports after ACTION_REMOVE group cfg
//   2.1 Check if it is the first AFE port operation (start or stop) in ACTION_ADD and ACTION_REMOVE group
//       If it is, basing current started AFE port in start_list, to_add and to_remove AFE ports in add_list and remove_list, do the necessary stream route and save the final result in start_list
//   2.2 Stop this port and mark "valid" to 0 in remove_list
//   2.3 Check if it is the last AFE port operation in remove_list and add_list
//       if it is, fully reelase all node in remove_list and reset add_list to null. Note, the node allocated for add_list are added to start_list

//3. Stop AFE ports after ACTION_STOP group cfg
//   1.1 Check if it is the first AFE port stop in ACTION_STOP group
//   1.2 Stop this port and mark "valid" to 0 in stop_list
//   1.3 Check if it is the last AFE port stop in ACTION_STOP group
//       if it is, fully reelase all node in start_list and reset stop_list to null.

afe_hdmi_output_group_device_state_struct_t  hdmi_output_group_device_state;

afe_hdmi_output_group_device_state_struct_t* afe_get_hdmi_output_group_device_state_object(void)
{
 
   return &hdmi_output_group_device_state;
}

static ADSPResult afe_hdmi_output_configure_group_device (afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg);


ADSPResult afe_hdmi_output_set_group_device_module_params(uint32_t param_id,
                                                          int8_t *param_buffer_ptr,
                                                          uint32_t param_size)
{
   ADSPResult                          result = ADSP_EOK;
   uint32_t                            group_dev_cfg_version;
   afe_param_id_group_device_hdmi_multistream_cfg_t     *p_group_dev_cfg;
   afe_param_id_group_device_enable_t  *p_group_device_cfg_enable;

   switch(param_id)
   {
      case AFE_PARAM_ID_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG:
         {
            /* Choose the appropriate config version */
            group_dev_cfg_version = (uint32_t) *(param_buffer_ptr);

            if(group_dev_cfg_version > AFE_API_VERSION_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG)
            {
               group_dev_cfg_version = AFE_API_VERSION_GROUP_DEVICE_HDMI_MULTISTREAM_CONFIG;
            }

            switch (group_dev_cfg_version)
            {
               case GROUP_DEVICE_CFG_V1:
                  {
                     /* Check for the valid param size */
                     if (param_size < sizeof(afe_param_id_group_device_hdmi_multistream_cfg_t) )
                     {
                        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device Cfg enable/disable, invalid payload size: %lu", param_size);
                        return ADSP_EBADPARAM;
                     }

                     p_group_dev_cfg =  (afe_param_id_group_device_hdmi_multistream_cfg_t *)param_buffer_ptr;

                     /* Validate group device ID */
                     if(FALSE == afe_svc_validate_group_dev_id(p_group_dev_cfg->group_id))
                     {
                        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device Cfg invalid group_dev_id: 0x%x", p_group_dev_cfg->group_id);
                           return ADSP_EBADPARAM;
                     }
                    
                     if ( ADSP_EOK != (result = afe_hdmi_output_configure_group_device(p_group_dev_cfg)) )
                     {
                        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to set group device config, group_dev_id: 0x%x, result: %d",
                              p_group_dev_cfg->group_id, result);
                        return result;
                     }

                     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Group device config success, group_dev_id: 0x%x", p_group_dev_cfg->group_id);
                  }
                  break;
               default:
                  {
                     MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Un-supported group device config ver: %lu, param_id: 0x%lx",
                           group_dev_cfg_version, param_id);
                  }
                  break;
            } /* End of group_dev_cfg_version switch case*/
         }
         break;

      case AFE_PARAM_ID_GROUP_DEVICE_ENABLE:
         {
            afe_hdmi_output_group_device_state_struct_t        *p_group_device;

            p_group_device = afe_get_hdmi_output_group_device_state_object();

            p_group_device_cfg_enable =  (afe_param_id_group_device_enable_t *)param_buffer_ptr;

            if(((AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_START == p_group_device->group_state) && (TRUE == p_group_device_cfg_enable->enable)) ||
               ((AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP == p_group_device->group_state) && (FALSE == p_group_device_cfg_enable->enable)))
            {
               p_group_device->is_enabled = p_group_device_cfg_enable->enable;
            }
            else
            {
               MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HDMI Group ID: %d enable: %d group state %d", p_group_device_cfg_enable->group_id, p_group_device_cfg_enable->enable,p_group_device->group_state);
               // This error return will be surpressed as ADSP_EOK by caller as long as it is not bad parameter
               result = ADSP_EUNSUPPORTED;
            }

         }
         break;
       default:
         result = ADSP_EUNSUPPORTED;
         break;
   } /* End of param_id switch case */
         

   return result;
}



ADSPResult afe_hdmi_output_configure_group_device (afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg)
{
   ADSPResult                             result = ADSP_EOK;
   afe_hdmi_output_group_device_state_struct_t        *p_group_device;
   int i;

   p_group_device = afe_get_hdmi_output_group_device_state_object();

   // Printing the device configuration information
   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"Group Device configuration for HDMI Multi-Stream Output Port");
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"cfg minor version: %lu group ID: %d action_id: %d",p_group_dev_cfg->minor_version, p_group_dev_cfg->group_id, p_group_dev_cfg->action_id);

   for(i = 0; i < AFE_GROUP_DEVICE_NUM_PORTS; i++)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"Port ID: %x Stream ID: %x",p_group_dev_cfg->port_id[i], p_group_dev_cfg->stream_id[i]);
   }

   if(ADSP_EOK != (result = hdmi_output_group_cfg_sanity_check(p_group_dev_cfg)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi_output_group_cfg_sanity_check() failed,  group_dev_id: 0x%x", p_group_dev_cfg->group_id);
      result = ADSP_EBADPARAM;
      goto __bailout;
   }

   switch(p_group_dev_cfg->action_id)
   {
   case AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_START:

      //This API policy
      //
      //If client sneds two ACTION_ID_START one by one, AFE only accepts the first one and return error for the 2nd one.
      if(AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP != p_group_device->group_state)
      {
         result = ADSP_EBADPARAM;

         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Group Device configuration failed as the wrong group state");

         goto __bailout;
      }

      //Do sanity check against the below valid cfg and return ERROR if not
      //                        stream_id_0, stream_id_1
      //                        stream_id_0, stream_id_1, stream_id_2
      //                        stream_id_0, stream_id_1, stream_id_2, stream_id_3

      //reset all lists
      memset(&p_group_device->start_list, 0, sizeof(hdmi_out_group_list_t));
      memset(&p_group_device->stop_list, 0, sizeof(hdmi_out_group_list_t));
      memset(&p_group_device->add_list, 0, sizeof(hdmi_out_group_list_t));
      memset(&p_group_device->remove_list, 0, sizeof(hdmi_out_group_list_t));

      //Copy the cfg to p_group_device->start_list
      //Fully build p_group_device->start_list
      //allocate memory for each node
      //p_group_device->start_list will be updated basing on ADD and REMOVE actions
      //It will be fully reelased (include node memory) once the last port in group stopped
      hdmi_output_group_list_node_create(p_group_dev_cfg, &p_group_device->start_list);

      if( ADSP_EOK != (result = hdmi_output_group_list_error_check(&p_group_device->start_list)))
      {
         //hdmi_output_group_list_node_destroy(&p_group_device->start_list);

         result = ADSP_EBADPARAM;
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi_output_group_list_error_check() failed,  group_dev_id: 0x%x", p_group_dev_cfg->group_id);
         goto __bailout;
      }
      else
      {
         //mark the group state to START
         p_group_device->group_state = AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_START;
      }

      break;

   case AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_STOP:

      //This API policy
      //
      //If client sneds two ACTION_ID_STOP one by one, AFE only accepts the first one and return error for the 2nd one.
      if(AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP == p_group_device->group_state)
      {
         result = ADSP_EBADPARAM;

         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Group Device configuration failed as the wrong group state");

         goto __bailout;
      }

      memset(&p_group_device->stop_list, 0, sizeof(hdmi_out_group_list_t));

      //check the STOP group cfg against current start_list
      if(ADSP_EOK != (result = hdmi_output_group_list_cfg_list_match_check(p_group_dev_cfg, &p_group_device->start_list)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi_output_group_list_cfg_list_match_check() failed,  group_dev_id: 0x%x", p_group_dev_cfg->group_id);

         result = ADSP_EBADPARAM;
         goto __bailout;
      }
      else
      {
         //All stopped ports are in updated p_group_device->start_list
         //Build p_group_device->stop_list via query the node in p_group_device->start_list
         hdmi_output_group_list_node_build(p_group_dev_cfg, &p_group_device->start_list, &p_group_device->stop_list);

         //Mark the group state to STOP
         p_group_device->group_state = AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP;
      }
      break;
   case AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_ADD:

      //could not do error check agnist current cfg as ADD and REMOVE maybe come in one shot
      //will do error check once the first port start or stop.

      if(AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP == p_group_device->group_state) 
      {
         result = ADSP_EBADPARAM;

         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Group Device configuration failed as the wrong group state");

         goto __bailout;
      }

      memset(&p_group_device->add_list, 0, sizeof(hdmi_out_group_list_t));

      //Copy the cfg to p_group_device->add_list
      //Fully build p_group_device->add_list
      //allocate memory for each node
      //p_group_device->start_list will be updated basing on ADD and REMOVE actions
      //It will be fully reelased (include node memory) once the last port in group stopped
      hdmi_output_group_list_node_create(p_group_dev_cfg, &p_group_device->add_list);

      //Mark the group state to UPDATE
      p_group_device->group_state = AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_UPDATE;

      break;
   case AFE_HDMI_MULTISTREAM_GROUP_UPDATE_ACTION_ID_REMOVE:

      //could not do error check agnist current cfg as ADD and REMOVE maybe come in one shot
      //will do error check once the first port start or stop.


      if(AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP == p_group_device->group_state) 
      {
         result = ADSP_EBADPARAM;

         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Group Device configuration failed as the wrong group state");

         goto __bailout;
      }

       memset(&p_group_device->remove_list, 0, sizeof(hdmi_out_group_list_t));

      //Copy the cfg to p_group_device->remove_list
      //the items to be removed should be in p_group_device->start_list already
      //search p_group_device->start_list to build p_group_device->remove_list
      if(ADSP_EOK != (result = hdmi_output_group_list_node_build(p_group_dev_cfg, &p_group_device->start_list, &p_group_device->remove_list)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi_output_group_list_node_build() failed for REMOVE,  group_dev_id: 0x%x", p_group_dev_cfg->group_id);
         result = ADSP_EBADPARAM;
         goto __bailout;
      }

      //Mark the group state to UPDATE
      p_group_device->group_state = AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_UPDATE;

      break;

   default:
      break;
   }
  
   /* Update the group config state to CONFIG */
   //p_group_device->is_config_rcvd = TRUE;
   
   return result;

   __bailout:

   //release all allocated memory
   hdmi_output_group_list_full_release();

   //set the group_device to default state
   p_group_device->group_state = AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP;

   p_group_device->is_enabled = FALSE;

   return result;
}


