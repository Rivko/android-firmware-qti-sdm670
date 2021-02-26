/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEDeviceEventUtil.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEDeviceEventUtil.cpp

DESCRIPTION: common functions for AFE Port Event Handling

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI)

==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEDeviceEventUtil.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/03/13  yongseok      Created file

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "mmdefs.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"
#include "AFEDeviceEventUtil.h"
#include "AFEInternal.h"
#include "qurt_elite.h"


/*==========================================================================
  Function definitions
========================================================================== */
/**
 * This function is to add a client to the client list
 *
 * @param[in] pClientList, pointer to the client list,
 * @param[in] pQNode, client to be added 
 * @return None.
 */
ADSPResult afe_port_event_register(afe_event_client_list_t **pp_client_list, afe_port_event_client_info_t *p_client_info)
{
	afe_event_client_list_t *list_node = NULL;
   afe_event_client_list_t *list = NULL;
   ADSPResult result = ADSP_EOK;
   bool_t     registered = FALSE;
   uint64_t event_type64 = 0; 

   if(0 == p_client_info->event_type)
   {
      //MSG for error
      return ADSP_EBADPARAM;
   }
   else
   {
      event_type64 = 1 << (p_client_info->event_type - 1); 
   }   

	list = *pp_client_list;
	if (list != NULL)
	{
      do
      {
	      /*list is not empty, check if the client has already registered*/ 
	      if((p_client_info->apr_client_addr == list->client_info.apr_client_addr) &&
	      (p_client_info->apr_client_port == list->client_info.apr_client_port) && 
	      (p_client_info->param_id == list->client_info.param_id) )
         {

            // 1 << p_client_info->event_type
            /* check if this is new event type or not */
            if(list->client_info.event_type64 & event_type64)
            {
               result = ADSP_EALREADY;
          	   MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: This event has been registered already, client addr: 0x%x, client port: 0x%x, \
                  param id 0x%lx, dst port id 0x%x, event type 0x%x", p_client_info->apr_client_addr, p_client_info->apr_client_port, p_client_info->param_id, 
                  p_client_info->port_id, p_client_info->event_type);
               break;
            }
            else
            {
               list->client_info.event_type64 |=event_type64;
               registered = TRUE;
               break;
            }
         }

         /* if there is only one client and skip the next iteration */
         if(NULL != list->next)
         {  
            list = list->next;
         }
      }while(NULL != list->next);

      /* there is no client with the same address */
      if((ADSP_EOK == result) && (FALSE == registered))
      {
         list_node = (afe_event_client_list_t *) qurt_elite_memory_malloc((sizeof(afe_event_client_list_t)), QURT_ELITE_HEAP_DEFAULT);
         if(NULL == list_node)
         {
            MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: This event register melloc failed, client addr: 0x%x, client port: 0x%x, \
                  param id 0x%lx, dst port id 0x%x, event type 0x%x", p_client_info->apr_client_addr, p_client_info->apr_client_port, p_client_info->param_id, 
                  p_client_info->port_id, p_client_info->event_type);
            result = ADSP_ENOMEMORY;
         }
         else
         {  
            list_node->client_info.apr_client_addr = p_client_info->apr_client_addr;
            list_node->client_info.apr_client_port = p_client_info->apr_client_port;
            list_node->client_info.apr_self_addr = p_client_info->apr_self_addr;
            list_node->client_info.apr_self_port = p_client_info->apr_self_port;
            list_node->client_info.param_id = p_client_info->param_id;
            list_node->client_info.port_id = p_client_info->port_id;
            list_node->client_info.event_type64 = event_type64;
            list->next = list_node;
         }
      }
	}
	else
	{
		/*First element in the node, so update the clientList */
      list_node = (afe_event_client_list_t *) qurt_elite_memory_malloc((sizeof(afe_event_client_list_t)), QURT_ELITE_HEAP_DEFAULT);
      if(NULL == list_node)
      {
         MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: This event register melloc failed, client addr: 0x%x, client port: 0x%x, \
               param id 0x%lx, dst port id 0x%x, event type 0x%x", p_client_info->apr_client_addr, p_client_info->apr_client_port, p_client_info->param_id, 
               p_client_info->port_id, p_client_info->event_type);
         result = ADSP_ENOMEMORY;
      }
      else
      { 
         list_node->client_info.apr_client_addr = p_client_info->apr_client_addr;
         list_node->client_info.apr_client_port = p_client_info->apr_client_port;
         list_node->client_info.apr_self_addr = p_client_info->apr_self_addr;
         list_node->client_info.apr_self_port = p_client_info->apr_self_port;  
         list_node->client_info.param_id = p_client_info->param_id;
         list_node->client_info.port_id = p_client_info->port_id;
         list_node->client_info.event_type64 = event_type64;  
         *pp_client_list = list_node;
      }
	}

   return result;
}

/**
 * This function is to search for a client and remove it from the list
 *
 * @param[in] ppQNode, pointer to the list,
 * @param[in] svc_handle_ptr, client to be removed 
 * @return bufQList, ptr to the node containing the current element, NULL if element not found.
 */
ADSPResult afe_port_event_unregister(afe_event_client_list_t **pp_client_list, afe_port_event_client_info_t *p_client_info)
{
   afe_event_client_list_t *list = NULL, *pre_list = NULL;
   ADSPResult result = ADSP_EOK;
   uint64_t event_type64 = 0; 

   if(0 == p_client_info->event_type)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: This event unregister failed, bad event type, client addr: 0x%x, client port: 0x%x, \
            param id 0x%lx, dst port id 0x%x, event type 0x%x", p_client_info->apr_client_addr, p_client_info->apr_client_port, p_client_info->param_id, 
            p_client_info->port_id, p_client_info->event_type);
      return ADSP_EBADPARAM;
   }
   else
   {
      event_type64 = 1 << (p_client_info->event_type - 1); 
   }   
   
	pre_list = list = *pp_client_list;
	if (list != NULL)
	{
      do
      {
	      /*list is not empty, check if the client has already registered*/ 
	      if((p_client_info->apr_client_addr == list->client_info.apr_client_addr) &&
	      (p_client_info->apr_client_port == list->client_info.apr_client_port) &&
         (p_client_info->param_id == list->client_info.param_id) )
         {
            /* check if this event have been registered or not */
            if(list->client_info.event_type64 & event_type64)
            {
               list->client_info.event_type64 &= ~(event_type64);
               break;
            }
            else
            {
          	   MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: This event has not been registered, client addr: 0x%x, client port: 0x%x, \
                  param id 0x%lx, dst port id 0x%x, event type 0x%x", p_client_info->apr_client_addr, p_client_info->apr_client_port, p_client_info->param_id, 
                  p_client_info->port_id, p_client_info->event_type);
               result = ADSP_ENOTEXIST;
               break;
            }
         }

         pre_list = list;
         /* if there is only one client and skip the next iteration */
         if(NULL != list->next)
         {  
            list = list->next;
         }
      }while(NULL != list->next);

      /*if there is no other event in that client, then release it */
      if((ADSP_EOK == result) && (0 == list->client_info.event_type64))
      {
         if(pre_list == list) 
         {
            *pp_client_list = NULL; /* the last client has been dereigstered */
         }
         else
         {
            pre_list->next = NULL;
         }
         
         qurt_elite_memory_free(list);
      }
	}
	else
	{
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: This client list is null, client addr: 0x%x, client port: 0x%x, \
         param id 0x%lx, dst port id 0x%x, event type 0x%x", p_client_info->apr_client_addr, p_client_info->apr_client_port, p_client_info->param_id, 
         p_client_info->port_id, p_client_info->event_type);
      result = ADSP_ENOTEXIST;
   }

   return result;
}


ADSPResult afe_port_event_send_client(afe_event_client_list_t **pp_client_list, uint16_t event_type, uint16_t *p_payload, uint16_t payload_size)
{
   //afe_event_client_list_t *list_node = NULL;
   afe_event_client_list_t *list = NULL;
   ADSPResult result = ADSP_EOK;
   bool_t     client_found = FALSE;
   afe_port_custom_event_t *p_event_msg;
   uint32_t event_msg_total_size = 0;
   uint8_t *p_event_payload = 0;
   uint64_t event_type64 = 0; 
   
   if(0 == event_type)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: This event send failed, bad event type, event type 0x%x, client list 0x%lx", \
         event_type, (long unsigned int)*pp_client_list);
      return ADSP_EBADPARAM;
   }
   else
   {
      event_type64 = 1 << (event_type - 1); 
   }  

   if(0 == payload_size)
   {
       event_msg_total_size = sizeof(afe_port_custom_event_t);
   }
   else
   {
      if(NULL == p_payload)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: This event send failed, bad payload pointer for this event, \
            event type 0x%x, client list 0x%lx", event_type, (long unsigned int)*pp_client_list);
         return ADSP_EBADPARAM;
      }
      else
      {
         event_msg_total_size = sizeof(afe_port_custom_event_t) + payload_size;
      }
   }
      
   list = *pp_client_list;
   if (list != NULL)
   {
      // move out this wihile loop 
      p_event_msg = (afe_port_custom_event_t *) qurt_elite_memory_malloc(event_msg_total_size, QURT_ELITE_HEAP_DEFAULT);

      if(NULL == p_event_msg)
      {
         MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: port event message mem alloc failed client addr: 0x%x, client port: 0x%x, \
                     param id 0x%lx, dst port id 0x%x, event type 0x%x", list->client_info.apr_client_addr, list->client_info.apr_client_port, list->client_info.param_id, 
                     list->client_info.port_id, event_type);
      }
      else
      {
         do
         {
            /*list is not empty, check if there is the client registered for this event*/ 
            if(list->client_info.event_type64 & event_type64)
            {
               memset(p_event_msg, 0, event_msg_total_size);

               p_event_msg->port_id = (uint32_t)list->client_info.port_id;
               p_event_msg->module_id = AFE_MODULE_CUSTOM_EVENTS;
               p_event_msg->param_id = list->client_info.param_id;
               p_event_msg->event_type = event_type;
               p_event_msg->payload_size = payload_size;

               p_event_payload = (uint8_t *)p_event_msg + sizeof(afe_port_custom_event_t);

               memscpy(p_payload, payload_size, p_event_payload, payload_size);
              
               //send the event to the client
               result = elite_apr_if_alloc_send_event ( afe_svc_get_apr_handle(),
                                       list->client_info.apr_self_addr,
                                       list->client_info.apr_self_port,
                                       list->client_info.apr_client_addr,
                                       list->client_info.apr_client_port,
                                       list->client_info.port_id,
                                       AFE_PORT_CUSTOM_EVENT, //opcode for event
                                       p_event_msg,          //pointer to payload
                                       event_msg_total_size);   //length of payload
               if (ADSP_FAILED(result))
               {
                  MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: This event send apr failed, client addr: 0x%x, client port: 0x%x, \
                     param id 0x%lx, dst port id 0x%x, event type 0x%x", list->client_info.apr_client_addr, list->client_info.apr_client_port, list->client_info.param_id, 
                     list->client_info.port_id, event_type);

               }

               client_found = TRUE;
               
               //move to next list if the next is not null.
               if(NULL != list->next)
               {  
                  list = list->next;
               }
            }
         }while(NULL != list->next);

         qurt_elite_memory_free(p_event_msg);
      }      
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: The event client list is null 0x%x", event_type);
      result = ADSP_ENOTEXIST;
   }

   if(FALSE == client_found)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: There is no client in the event client list for this event type 0x%x", event_type);
      result = ADSP_ENOTEXIST;
   }

   return result;
}



