#ifndef __AFE_DEVICE_EVENT_UTIL_H__
#define __AFE_DEVICE_EVENT_UTIL_H__
/**
@file AFEDeviceEventUtil.h **
This module contains AFE Port Event handling common function.

Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI)
*/
/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/inc/AFEDeviceEventUtil.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/03/13  YJ      AFE Port Event Handling common code functions
========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#if defined(__cplusplus)
extern "C" {
#endif


/**
 * Struct for apr source and destination address information 
 */
typedef struct afe_port_event_client_info
{
   elite_apr_addr_t              apr_self_addr;
   elite_apr_port_t              apr_self_port;
   elite_apr_addr_t              apr_client_addr;
   elite_apr_port_t              apr_client_port;
   uint32_t                      param_id;
   uint16_t                      port_id;
   uint16_t                      event_type;  // event enumeration starting from 1,
} afe_port_event_client_info_t;


/**
 * Struct for apr source and destination address information 
 */
typedef struct afe_port_event_client_list_info
{
   elite_apr_addr_t              apr_self_addr;
   elite_apr_port_t              apr_self_port;
   elite_apr_addr_t              apr_client_addr;
   elite_apr_port_t              apr_client_port;
   uint32_t                      param_id;
   uint16_t                      port_id;
   uint64_t                      event_type64;  // event bit mappnig for maximum 64 event 
} afe_port_event_client_list_info_t;


/** @brief Node for storing the linked list of clients. */
typedef struct afe_event_client_list
{
   afe_port_event_client_list_info_t    client_info;    /**< This element. */
   struct afe_event_client_list         *next;                 /**< Pointer to the next node. */
} afe_event_client_list_t;

ADSPResult afe_port_event_register(afe_event_client_list_t **pp_client_list, afe_port_event_client_info_t *p_client_info);

ADSPResult afe_port_event_unregister(afe_event_client_list_t **pp_client_list, afe_port_event_client_info_t *p_client_info);

ADSPResult afe_port_event_send_client(afe_event_client_list_t **pp_client_list, uint16_t event_type, uint16_t *p_payload, uint16_t payload_size);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif // !defined(__AFE_DEVICE_EVENT_UTIL_H__)

