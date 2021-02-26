/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHdmiOutputDrvUtils.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hdmi_input_drv.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2013 Qualcomm Technologies, Inc. (QTI).
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHdmiOutputDrvUtils.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
7/30/2012   RP   Created


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEGroupDeviceDriver.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"

#include "AFEHdmiOutputGroupDevice.h"
#include "AFEHdmiOutputDrv_i.h"

#include "AFEHalLpassHdmiOutput.h"

/*=====================================================================
 Globals
 ======================================================================*/

/*=====================================================================
 Static Functions
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

static int16_t hdmi_output_group_list_get_num_valid_items(hdmi_out_group_list_t* p_group_list);


ADSPResult hdmi_out_group_list_insert_node(hdmi_out_group_node_t **list_dptr, hdmi_out_group_node_t *node_ptr)
{
    hdmi_out_group_node_t **curr_dptr;
    uint16_t str_id_in;

    curr_dptr = list_dptr;
    str_id_in = node_ptr->stream_id;

    //insert the node in the ascending order of stream ID
    while ( (NULL != (*curr_dptr)) && (str_id_in >= (*curr_dptr)->stream_id))
    {
        curr_dptr = &(*curr_dptr)->next_ptr;
    }

    node_ptr->next_ptr = *curr_dptr;
    *curr_dptr = node_ptr;

    return ADSP_EOK;
}


ADSPResult hdmi_out_group_list_remove_node(hdmi_out_group_node_t **list_dptr, hdmi_out_group_node_t *node_ptr)
{
    hdmi_out_group_node_t **curr_dptr;

    curr_dptr = list_dptr;

    //find the client timer in the list
    while ( (NULL != (*curr_dptr)) && (node_ptr != (*curr_dptr)))
    {
        curr_dptr = &(*curr_dptr)->next_ptr;
    }

    // if the client timer is found in the list, remove it
    if (node_ptr == *curr_dptr)
    {
        *curr_dptr = node_ptr->next_ptr;
        node_ptr->next_ptr = NULL;
    }
    else {
        return ADSP_ENOTEXIST;

    }

    return ADSP_EOK;
}

//Before 
//    port_id    stream_id
//    A          0
//    C          2
//
//After
//    port_id    stream_id
//    A          0
//    C          1

ADSPResult hdmi_out_group_list_stream_id_update(hdmi_out_group_node_t **list_dptr)
{
    hdmi_out_group_node_t **curr_dptr;
    uint16_t str_id_cur = AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_0;

    curr_dptr = list_dptr;

    //insert the node in the ascending order of stream ID
    while ( NULL != (*curr_dptr))
    {
       (*curr_dptr)->stream_id = str_id_cur;
       str_id_cur++;
       curr_dptr = &(*curr_dptr)->next_ptr;
    }

    return ADSP_EOK;
}


ADSPResult hdmi_output_group_list_node_create(afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg, hdmi_out_group_list_t* list_ptr)
{
   ADSPResult result = ADSP_EFAILED;
   int idx;
   hdmi_out_group_node_t *node_ptr;

   if((p_group_dev_cfg) && (list_ptr))
   {
      for(idx = 0; idx < AFE_GROUP_DEVICE_NUM_PORTS; idx++)
      {
         if((AFE_PORT_ID_INVALID == p_group_dev_cfg->port_id[idx]) || (0 == p_group_dev_cfg->port_id[idx]))
            break;

         //this is a valid slot, add to the list
         node_ptr = (hdmi_out_group_node_t *)qurt_elite_memory_malloc(sizeof(hdmi_out_group_node_t),QURT_ELITE_HEAP_DEFAULT);

         if(node_ptr)
         {
            node_ptr->next_ptr = NULL;

            node_ptr->port_id = p_group_dev_cfg->port_id[idx];
            node_ptr->stream_id = p_group_dev_cfg->stream_id[idx];
            node_ptr->action_id = p_group_dev_cfg->action_id;
            node_ptr->valid = 1;

            hdmi_out_group_list_insert_node(&list_ptr->list_ptr, node_ptr);

            list_ptr->num_node++;

            result = ADSP_EOK;

         }
         else
         {
            result = ADSP_EFAILED;
            break;
         }
      }
   }

   if(ADSP_EFAILED == result)
   {
      //release all memory in this list
      hdmi_output_group_list_node_destroy(list_ptr);

   }

   return result;
}

ADSPResult hdmi_output_group_list_node_build(afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg, hdmi_out_group_list_t* src_list_ptr, hdmi_out_group_list_t* dst_list_ptr)
{
   //basing on port_id, stream_id, get the node from src_list_ptr and build the dst_list_ptr

    ADSPResult result = ADSP_EFAILED;

   if((dst_list_ptr) && (p_group_dev_cfg) && (src_list_ptr))
   {
      int idx, valid_cfg_cnt = 0;
      hdmi_out_group_node_t *node_ptr;

      uint16_t port_id, stream_id;

      memset((void*)dst_list_ptr, 0, sizeof(hdmi_out_group_list_t));

      for(idx = 0; idx < AFE_GROUP_DEVICE_NUM_PORTS; idx++)
      {
         if((AFE_PORT_ID_INVALID == p_group_dev_cfg->port_id[idx]) || (0 == p_group_dev_cfg->port_id[idx]))
            break;

         valid_cfg_cnt++;

         port_id = p_group_dev_cfg->port_id[idx];
         stream_id = p_group_dev_cfg->stream_id[idx];

         node_ptr = src_list_ptr->list_ptr;

         do
         {
            if(node_ptr)
            {
               if((port_id == node_ptr->port_id) && (stream_id == node_ptr->stream_id))
               {
                  //found! Set this node valid again. Also this node to new action_id
                  break;
               }  
               node_ptr = node_ptr->next_ptr;
            }

         } while (NULL != node_ptr);

         if(node_ptr)
         {
            //remove this node from src_list
            hdmi_out_group_list_remove_node(&src_list_ptr->list_ptr, node_ptr);
            src_list_ptr->num_node--;

            //Set this node valid again. Also this node to new action_id
            node_ptr->valid = 1;
            node_ptr->action_id = p_group_dev_cfg->action_id;

            //insert this node to dst_list
            hdmi_out_group_list_insert_node(&dst_list_ptr->list_ptr, node_ptr);
            dst_list_ptr->num_node++;
         }
      }

      if(valid_cfg_cnt == dst_list_ptr->num_node)
      {
         //find right one
         result = ADSP_EOK;
      }
   }


   return result;
}


ADSPResult hdmi_output_group_list_cfg_list_match_check(afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg, hdmi_out_group_list_t* check_list_ptr)
{
   //basing on port_id, stream_id, get the node from src_list_ptr and build the dst_list_ptr

    ADSPResult result = ADSP_EFAILED;

   if((check_list_ptr) && (p_group_dev_cfg))
   {
      int idx;
      hdmi_out_group_node_t *node_ptr;

      uint16_t port_id, stream_id;

      int match_cnt = 0;

      for(idx = 0; idx < AFE_GROUP_DEVICE_NUM_PORTS; idx++)
      {
         if((AFE_PORT_ID_INVALID == p_group_dev_cfg->port_id[idx]) || (0 == p_group_dev_cfg->port_id[idx]))
            break;

         port_id = p_group_dev_cfg->port_id[idx];
         stream_id = p_group_dev_cfg->stream_id[idx];

         node_ptr = check_list_ptr->list_ptr;

         do
         {
            if(node_ptr)
            {
               if((port_id == node_ptr->port_id) && (stream_id == node_ptr->stream_id))
               {
                  //found! 
                  match_cnt++;
                  break;
               }  
               node_ptr = node_ptr->next_ptr;
            }

         } while (NULL != node_ptr);
      }

      if(match_cnt == check_list_ptr->num_node)
      {
         //find right one
         result = ADSP_EOK;
      }
   }

   return result;
}


ADSPResult hdmi_output_group_list_valid_update(uint16_t port_id, hdmi_out_group_list_t* p_group_list)
{
   if(p_group_list)
   {
      hdmi_out_group_node_t* node_header_ptr = p_group_list->list_ptr;

      do
      {
         if(node_header_ptr)
         {
            if(port_id == node_header_ptr->port_id)
               node_header_ptr->valid = 0;
              
            node_header_ptr = node_header_ptr->next_ptr;
         }

      } while (NULL != node_header_ptr);

   }

   return ADSP_EOK;
}



bool_t hdmi_output_group_list_is_first_item_query(hdmi_out_group_list_t* p_group_list_1, hdmi_out_group_list_t* p_group_list_2)
{
   bool_t result = FALSE;

   int cnt_1 = 0;
   int cnt_2 = 0;

   int num_node_1 = 0;
   int num_node_2 = 0;

   if(p_group_list_1)
   {
      cnt_1 = hdmi_output_group_list_get_num_valid_items(p_group_list_1);

      num_node_1 = p_group_list_1->num_node;
   }

   if(p_group_list_2)
   {
      cnt_2 = hdmi_output_group_list_get_num_valid_items(p_group_list_2);
      num_node_2 = p_group_list_2->num_node;
   }

   if((cnt_1 + cnt_2) == (num_node_1 + num_node_2))
      result = TRUE;

   return result;
}

bool_t hdmi_output_group_list_is_last_item_query(hdmi_out_group_list_t* p_group_list_1, hdmi_out_group_list_t* p_group_list_2)
{
   bool_t result = FALSE;

   int cnt_1 = 0;
   int cnt_2 = 0;

   int num_node_1 = 0;
   int num_node_2 = 0;

   if(p_group_list_1)
   {
      cnt_1 = hdmi_output_group_list_get_num_valid_items(p_group_list_1);

      num_node_1 = p_group_list_1->num_node;
   }

   if(p_group_list_2)
   {
      cnt_2 = hdmi_output_group_list_get_num_valid_items(p_group_list_2);
      num_node_2 = p_group_list_2->num_node;
   }

   if((0 == (cnt_1 + cnt_2)) && ( 0 != (num_node_1 + num_node_2)))
      result = TRUE;

   return result;
}

ADSPResult hdmi_output_group_list_node_destroy(hdmi_out_group_list_t* p_group_list)
{
   ADSPResult       result = ADSP_EOK;

   if(p_group_list)
   {
      //go through all node and free the memory
      hdmi_out_group_node_t* node_header_ptr = p_group_list->list_ptr;

      hdmi_out_group_node_t* node_tail_ptr = NULL;

      do
      {
         if(node_header_ptr)
         {
            node_tail_ptr = node_header_ptr->next_ptr;

            qurt_elite_memory_free((void*)node_header_ptr);
         }

         node_header_ptr = node_tail_ptr;

      } while (NULL != node_tail_ptr);

       memset((void*)p_group_list, 0, sizeof(hdmi_out_group_list_t));
   }

   return result;
}

#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)
extern afe_hdmi_output_driver_t    hdmi_output_driver;
#endif //#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)


ADSPResult hdmi_output_group_list_stream_route_update_all(hdmi_out_group_list_t* p_start_list)
{
   uint16_t port_id;
   uint16_t stream_id;

#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)

   for(int i = 0; i < 4; i++)
   {
      hdmi_output_driver.cur_stream_id_array[i] = HDMI_DRIVE_INVALID_VALUE;
      hdmi_output_driver.cur_dma_idx_array[i] = HDMI_DRIVE_INVALID_VALUE;
   }

   hdmi_output_driver.cur_used_idx = 0;

#endif //#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)

   if(p_start_list)
   {
      hdmi_out_group_node_t* node_header_ptr = p_start_list->list_ptr;

      do
      {
         if(node_header_ptr)
         {
            port_id = node_header_ptr->port_id;
            stream_id = node_header_ptr->stream_id;

            afe_hdmi_output_stream_route_update(port_id, stream_id);

            node_header_ptr = node_header_ptr->next_ptr;
         }

      } while (NULL != node_header_ptr);

   }

#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)

   //mapping unused DMA and straem_id
   int unused_start_idx = hdmi_output_driver.cur_used_idx;
   uint16_t unused_stream_id; //1, 2, 3, 4
   uint16_t unused_dma_inx;   //0, 1, 2, 3
   hal_lpass_hdmi_output_ms_pack_trans_cfg_t ms_cfg;

   for(unused_dma_inx = 0; unused_dma_inx < 4; unused_dma_inx++)
   {
      if((unused_dma_inx == hdmi_output_driver.cur_dma_idx_array[0]) ||
         (unused_dma_inx == hdmi_output_driver.cur_dma_idx_array[1]) ||
         (unused_dma_inx == hdmi_output_driver.cur_dma_idx_array[2]) ||
         (unused_dma_inx == hdmi_output_driver.cur_dma_idx_array[3]))
      {
         continue;
      }
      else
      {
         //find unused one: unused_dma_inx

         //look for unused stream_id
         for(unused_stream_id = 1; unused_stream_id <= 4; unused_stream_id++)
         {
            if((unused_stream_id == hdmi_output_driver.cur_stream_id_array[0]) ||
               (unused_stream_id == hdmi_output_driver.cur_stream_id_array[1]) ||
               (unused_stream_id == hdmi_output_driver.cur_stream_id_array[2]) ||
               (unused_stream_id == hdmi_output_driver.cur_stream_id_array[3]))
            {
               continue;
            }
            else
            {
               ////find unused one: unused_stream_id
               hdmi_output_driver.cur_dma_idx_array[hdmi_output_driver.cur_used_idx] = unused_dma_inx;
               hdmi_output_driver.cur_stream_id_array[hdmi_output_driver.cur_used_idx] = unused_stream_id;
               hdmi_output_driver.cur_used_idx++;

               break;
            }
         }
      }
   }

   //configure for unused DMA and stream ID
   for(int i = unused_start_idx; i < 4; i++)
   {
      ms_cfg.dma_idx = hdmi_output_driver.cur_dma_idx_array[i];
      ms_cfg.stream_id = hdmi_output_driver.cur_stream_id_array[i];

      hal_lpass_hdmi_output_ms_cfg_pack_trans(ms_cfg);
   }

#endif //#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)

   return ADSP_EOK;
}


uint16_t hdmi_output_group_list_get_stream_id(uint16_t port_id, hdmi_out_group_list_t* p_group_list)
{
   uint16_t stream_id = AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_INVALID;

   if(p_group_list)
   {
      hdmi_out_group_node_t* node_header_ptr = p_group_list->list_ptr;

      do
      {
         if(node_header_ptr)
         {
            if(port_id == node_header_ptr->port_id)
            {
               stream_id = node_header_ptr->stream_id;
               break;
            }
              
            node_header_ptr = node_header_ptr->next_ptr;
         }

      } while (NULL != node_header_ptr);

   }

   return stream_id;
}

ADSPResult hdmi_output_group_list_error_check(hdmi_out_group_list_t* p_group_list)
{
   ADSPResult result = ADSP_EFAILED;

   if(p_group_list)
   {
      if((MAX_NUM_SUB_STREAM_SUPPORTED >= p_group_list->num_node) && (MIN_NUM_SUB_STREAM_SUPPORTED <= p_group_list->num_node))
      {
         hdmi_out_group_node_t* node_header_ptr;
         uint16_t stream_id = AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_INVALID;
         int i = 0;

         node_header_ptr = p_group_list->list_ptr;

         do
         {
            if(node_header_ptr)
            {
               stream_id = node_header_ptr->stream_id;

               if((AFE_HDMI_MULTISTREAM_SUB_STREAM_ID_0 + i) != stream_id)
                  break;

               node_header_ptr = node_header_ptr->next_ptr;
            }

            i++;

         } while (NULL != node_header_ptr);

         if(i == p_group_list->num_node)
         {
            result = ADSP_EOK;
         }
      }
   }

   return result;
}


ADSPResult hdmi_output_group_cfg_update_validation(void)
{

   ADSPResult                             result = ADSP_EFAILED;
   afe_hdmi_output_group_device_state_struct_t        *p_group_device;

   p_group_device = afe_get_hdmi_output_group_device_state_object();

   int valid_cnt, skip_check = 0;

   //hdmi_out_group_list_t temp_list;

   //check against pending table

   //case 1 ADD and REMOVE in one run

   //case 2 ADD only

   //case 3 REMOVE only

   //Do sanity check against the below valid cfg and return ERROR if not
   //                        stream_id_0, stream_id_1
   //                        stream_id_0, stream_id_1, stream_id_2
   //                        stream_id_0, stream_id_1, stream_id_2, stream_id_3

   //build a group list temp_list based on start_list, add_list, and remove_list

  //1. Check the if all ports in start_list are started
   valid_cnt = hdmi_output_group_list_get_num_valid_items(&p_group_device->start_list);

   if(0 != valid_cnt)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi_output_group_cfg_update_validation() failed,  num of valid cnt: 0x%x", valid_cnt);
      goto __bailout;
   }

   //2. Sanity chack against max 4 and min 2 sub-stream case
   if(
      (MAX_NUM_SUB_STREAM_SUPPORTED < (p_group_device->start_list.num_node + p_group_device->add_list.num_node)) || 
      (MIN_NUM_SUB_STREAM_SUPPORTED > (p_group_device->start_list.num_node + p_group_device->add_list.num_node))
     )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi_output_group_cfg_update_validation() failed,  num of items in group: 0x%x", (p_group_device->start_list.num_node + p_group_device->add_list.num_node));
      goto __bailout;
   }

   //3. add and remove in one shot case
   //check if need to do stream_id update
    if((0 < p_group_device->remove_list.num_node) && 
       (0 < p_group_device->add_list.num_node)    &&
       (p_group_device->add_list.num_node == p_group_device->remove_list.num_node)
      )
    {
            //go through all node and free the memory
       hdmi_out_group_node_t* node_ptr_add;
       hdmi_out_group_node_t* node_ptr_remove;
       int cnt = 0;

       node_ptr_add = p_group_device->add_list.list_ptr;
       node_ptr_remove = p_group_device->remove_list.list_ptr;

       do
       {
          if(node_ptr_add->stream_id != node_ptr_remove->stream_id)
          {
             break;
          }

          cnt++;

          node_ptr_add = node_ptr_add->next_ptr;
          node_ptr_remove = node_ptr_remove->next_ptr;

       } while ((NULL != node_ptr_add) && (NULL != node_ptr_remove));

       if(cnt == p_group_device->add_list.num_node)
       {
          //It is below case
         //if add and remove are pired                   ===> no route required
         //            remove 0 and add 0, 
         //            remove 0 & 1, add 0 & 1
         //            remove 0 & 1 & 2, add 0 & 1 & 2
         //            remove 0 & 1 & 2 & 3, add 0 & 1 & 2 & 3

          skip_check = 1;
       }
       else
       {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi_output_group_cfg_update_validation() failed for ADD/REMOVE in one shot case,  num of add items: 0x%x  num of remove items in group: 0x%x", p_group_device->add_list.num_node, p_group_device->remove_list.num_node);
          goto __bailout;
       }
    }

    if(!skip_check)
    {
       if((0 < p_group_device->add_list.num_node) && (0 == p_group_device->remove_list.num_node))
       {
          //add only case
          //alway add at the end of start_list
          //1. find the max stream_id in start_list
          //2. Go through add_list and check new added stream_id against the max one in start_list

          uint16_t stream_id_max = 0;
          hdmi_out_group_node_t* node_header_ptr;
            
          //1. find the max stream_id in start_list
          node_header_ptr = p_group_device->start_list.list_ptr;

          do
          {
             if(node_header_ptr)
             {
                stream_id_max = node_header_ptr->stream_id;
                node_header_ptr = node_header_ptr->next_ptr;
             }
          } while (NULL != node_header_ptr);

          //2. Go through add_list and check new added stream_id against the max one in start_list

          node_header_ptr = p_group_device->add_list.list_ptr;

          if(node_header_ptr)
          {
             if((stream_id_max + 1) != node_header_ptr->stream_id)
             {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi_output_group_cfg_update_validation() ADD only case failed,  the 1st added stream ID: 0x%x", node_header_ptr->stream_id);
                goto __bailout;
             }
          }
       }
       else if((0 == p_group_device->add_list.num_node) && (0 < p_group_device->remove_list.num_node))
       {
          //remove only case
          //remove_list is generated from start_list
          //the sanity check is done already
       }
       else
       {
          //remove and add in one shot case
          //do nothing
       }
    }

    result = ADSP_EOK;


   return result;

   __bailout:

   return result;
}

//group_dev_cfg should not have duplicated port_id OR stream_id
ADSPResult hdmi_output_group_cfg_sanity_check(afe_param_id_group_device_hdmi_multistream_cfg_t *p_group_dev_cfg)
{
   ADSPResult result = ADSP_EOK;

   if(p_group_dev_cfg)
   {
      int idx1, idx2;
      
      uint16_t port_id, stream_id;

      for(idx1 = 0; idx1 < AFE_GROUP_DEVICE_NUM_PORTS; idx1++)
      {
         if((AFE_PORT_ID_INVALID == p_group_dev_cfg->port_id[idx1]) || (0 == p_group_dev_cfg->port_id[idx1]))
            break;

         port_id = p_group_dev_cfg->port_id[idx1];
         stream_id = p_group_dev_cfg->stream_id[idx1];

         for(idx2 = 0; idx2 < AFE_GROUP_DEVICE_NUM_PORTS; idx2++)
         {
            if((idx2 != idx1) && ((port_id == p_group_dev_cfg->port_id[idx2]) || (stream_id == p_group_dev_cfg->stream_id[idx2])))
            {
               result = ADSP_EFAILED;
               break;
            }
         }

         if(ADSP_EFAILED == result)
            break;
      }
   }
   else
   {
      result = ADSP_EFAILED;
   }

   return result;
}


ADSPResult hdmi_output_group_list_merge(hdmi_out_group_list_t* p_group_target_list)
{

   afe_hdmi_output_group_device_state_struct_t        *p_group_device;
   p_group_device = afe_get_hdmi_output_group_device_state_object();

   if((0 < p_group_device->add_list.num_node) && (0 == p_group_device->remove_list.num_node))
   {
      //add only case, alway add at the end of start_list, no stream_id update needed
      hdmi_out_group_node_t* add_node_ptr = p_group_device->add_list.list_ptr;

      hdmi_out_group_node_t* add_node_next_ptr;

      while(NULL != add_node_ptr)
      {
         add_node_next_ptr = add_node_ptr->next_ptr;
         hdmi_out_group_list_insert_node(&p_group_target_list->list_ptr, add_node_ptr);
         p_group_target_list->num_node++;

         add_node_ptr = add_node_next_ptr;
      }

      //the add_list has been moved to start_list
      //reset add_list
      memset(&p_group_device->add_list, 0, sizeof(hdmi_out_group_list_t));
   }
   else if((0 == p_group_device->add_list.num_node) && (0 < p_group_device->remove_list.num_node))
   {
      //remove only case
      //the start_list has been updated
      //only need to do stream_id route update
      hdmi_out_group_list_stream_id_update(&p_group_device->start_list.list_ptr);
   }
   else
   {
      //add and remove in one shot
      //Note the remove_list is built based start_list. 
      //So only add add_list to start_list here
      //No hdmi_out_group_list_stream_id_update() needed.
      hdmi_out_group_node_t* add_node_ptr = p_group_device->add_list.list_ptr;

      hdmi_out_group_node_t* add_node_next_ptr;

      while(NULL != add_node_ptr)
      {
         add_node_next_ptr = add_node_ptr->next_ptr;
         hdmi_out_group_list_insert_node(&p_group_target_list->list_ptr, add_node_ptr);
         p_group_target_list->num_node++;

         add_node_ptr = add_node_next_ptr;
      }

      //the add_list has been moved to start_list
      //reset add_list
      memset(&p_group_device->add_list, 0, sizeof(hdmi_out_group_list_t));
   }

   return ADSP_EOK;
}
int16_t hdmi_output_group_list_get_num_valid_items(hdmi_out_group_list_t* p_group_list)
{
   int16_t cnt = 0;

   if(p_group_list)
   {
      hdmi_out_group_node_t* node_header_ptr = p_group_list->list_ptr;

      do
      {
         if(node_header_ptr)
         {
            if(node_header_ptr->valid)
               cnt++;

            node_header_ptr = node_header_ptr->next_ptr;
         }

      } while (NULL != node_header_ptr);

   }

   return cnt;
}


ADSPResult hdmi_output_group_list_full_release(void)
{
   afe_hdmi_output_group_device_state_struct_t        *p_group_device;
   p_group_device = afe_get_hdmi_output_group_device_state_object();


   hdmi_output_group_list_node_destroy(&p_group_device->start_list);  //start_list is empty already
   hdmi_output_group_list_node_destroy(&p_group_device->stop_list);
   hdmi_output_group_list_node_destroy(&p_group_device->add_list);  //start_list is empty already
   hdmi_output_group_list_node_destroy(&p_group_device->remove_list);

   //reset all lists
   memset(&p_group_device->start_list, 0, sizeof(hdmi_out_group_list_t));
   memset(&p_group_device->stop_list, 0, sizeof(hdmi_out_group_list_t));
   memset(&p_group_device->add_list, 0, sizeof(hdmi_out_group_list_t));
   memset(&p_group_device->remove_list, 0, sizeof(hdmi_out_group_list_t));

   return ADSP_EOK;
}
