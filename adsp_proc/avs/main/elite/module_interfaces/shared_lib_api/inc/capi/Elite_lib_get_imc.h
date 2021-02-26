#ifndef ELITE_LIB_GET_IMC_H
#define ELITE_LIB_GET_IMC_H

/**
  @file Elite_lib_get_imc.h

  @brief A CAPIv2 supporting library for inter-module communication with other CAPIv2 modules
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
=============================================================================*/
/*=============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_lib_get_imc.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  06/05/15    sw      (Tech Pubs) Edited Doxygen markup/comments for 8952 (2.6)
  1/21/15     sabdagir   Created file.
=============================================================================*/

/** @weakgroup weakf_capiv2_lib_imc_intro

The Intermodule Communication library is used to communicate with other CAPIv2
modules that are available in the system.

To establish intermodule communication between a destination and one or more
sources, the sources must use a key to register with the IMC database.

The key must be unique for each communication instance, and it must be same
for both destination and sources to communicate with each other. 
 - The first 4 bytes of the key correspond to the communication ID specific to
   the type of the modules communicating.
 - The bytes following the first 4 bytes must distinguish between different
   instances of the same communication.

When registering for IMC, use the following information to determine the key
(id0 = first 4 bytes, id1&nbsp;=&nbsp;next 4 bytes, etc.).

@lib_get_imc_table{
One destination and one source & Unique \newline communication ID &
Destination module ID & Source module ID & Distinguishes between different instances of the same communication. \newline
A service session ID can be used. @tblendline

One destination and multiple sources & Unique \newline communication ID &
Destination module ID & Session ID &   -- @tblendline

Multiple destinations & \multicolumn{4}{l|}{Source module must open communication with different destinations using} \tn
and one source & \multicolumn{4}{l|}{corresponding keys.} @tblendline}
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"


/** @addtogroup capiv2_lib_imc
@{ */

/** Unique identifier that represents the IMC library interface. @newpage */
#define ELITE_LIB_GET_IMC 0x00010E5D

/*------------------------------------------------------------------------------
 * Interface definition
 *----------------------------------------------------------------------------*/
typedef struct elite_lib_get_imc_t elite_lib_get_imc_t;
typedef struct imc_src_info_t  imc_src_info_t;
typedef struct imc_dest_info_t imc_dest_info_t;

/**
  Virtual table of the IMC library interface.
  To use this library, follow the steps for the source and destination modules.

  @subhead{For the source module}

  -# Get an object of type elite_lib_get_imc_t using 
     #CAPI_V2_EVENT_GET_LIBRARY_INSTANCE with id =  ELITE_LIB_GET_IMC
  -# Register with the IMC database to get the source handle. \n @vertspace{2}
     Provide the source buffer queue length (it must be a power of 2).
  -# Open communication with a key and get the destination handle. \n
     @vertspace{2}
     Once a source is registered, it can open multiple connections with
     different destinations.
  -# Allocate and transfer buffers to the destination queue in one of two ways:
      - Allocate a single buffer and push it to the destination. \n
        @vertspace{2}
        When the destination returns the buffer, the source must free or
        resend the buffer with the new payload to the destination.
      - Allocate a required number of buffers with a buffer size, and push
        them to their own queue. \n @vertspace{2}
        The required number of buffers must be less than the source queue
        length. \n @vertspace{2}
        When required, pop the buffers from their queue and send them to the
        destination number of buffers. If the source failed to push the buffers
        to the destination, it must push the buffers to their own queue.
  -# Once communication is done, close the connection for each destination.
  -# Deregister from the IMC database.
  -# Call end() to return the elite_lib_get_imc_t object.

  @subhead{For the destination module}

  -# Get an object of type elite_lib_get_imc_t using
     #CAPI_V2_EVENT_GET_LIBRARY_INSTANCE with id = ELITE_LIB_GET_IMC.
  -# Register with the IMC database to get a destination handle.
  -# Pop the buffers from the queue.
  -# Process the buffers.
  -# Return the buffers to the source with the results.
  -# Deregister from the IMC database.
  -# Call end() to return the elite_lib_get_imc_t object.
 */
typedef struct elite_lib_get_imc_vtable_t
{
   capi_v2_library_base_t b;
   capi_v2_err_t (*src_register)(imc_src_info_t **src_handle, uint32_t src_bufq_len);
   capi_v2_err_t (*src_open_comm)(imc_src_info_t *src_handle, imc_dest_info_t **dst_handle,const int8_t *keyptr,const uint32_t keylen);
   capi_v2_err_t (*src_allocate_push_self_buf)(imc_src_info_t *src_handle, uint32_t payload_size, uint32_t num_buffers);
   capi_v2_err_t (*src_allocate_packet)(imc_src_info_t *src_handle, void **payload_ptr, uint32_t payload_size);
   capi_v2_err_t (*src_pop_buf)(imc_src_info_t *src_handle, void **payload_ptr, capi_v2_err_t *resp_result_ptr,bool_t *is_queue_empty);
   capi_v2_err_t (*src_push_buf)(imc_src_info_t *src_handle,imc_dest_info_t *dst_handle, void *payload_ptr,uint32_t payload_size);
   capi_v2_err_t (*src_return_buf)(imc_src_info_t *src_handle, void *payload_ptr,capi_v2_err_t response_result);
   capi_v2_err_t (*src_close_connection)(imc_src_info_t *src_handle, imc_dest_info_t *dst_handle);
   capi_v2_err_t (*src_deregister)(imc_src_info_t *src_handle);
   capi_v2_err_t (*dest_register)(imc_dest_info_t **dst_handle, const int8_t *keyptr,const uint32_t keylen,uint32_t dest_dataq_len);
   capi_v2_err_t (*dest_pop_buf)(imc_dest_info_t *dst_handle, void **payload_ptr,uint32_t *payload_actual_size_ptr,uint32_t *payload_max_size_ptr,bool_t *is_queue_empty);
   capi_v2_err_t (*dest_return_buf)(imc_dest_info_t *dst_handle, void *payload_ptr,capi_v2_err_t response_result);
   capi_v2_err_t (*dest_deregister)(imc_dest_info_t *dst_handle);
}/** @cond */elite_lib_get_imc_vtable_t/** @endcond */;


/** Structure that contains a pointer to the IMC virtual function table
  defined in elite_lib_get_imc_vtable_t.
*/
typedef struct elite_lib_get_imc_t
{
   const elite_lib_get_imc_vtable_t *vtable;  /**< Pointer to the virtual
                                                   function table. */
}/** @cond */elite_lib_get_imc_t/** @endcond */;

/** @} *//* end_addtogroup capiv2_lib_imc */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* ELITE_LIB_GET_IMC_H */
