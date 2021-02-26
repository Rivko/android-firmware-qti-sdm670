/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/capi_v2_libraries/capi_v2_library_get_imc/inc/inter_module_comm.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/19/2014 sabdagir       Created

==========================================================================*/

/*-----------------------------------------------------------------------
   This file contains intermodule communication private apis/structures .

   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef _INTER_MODULE_COMM_H_
#define _INTER_MODULE_COMM_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "uthash.h"
#include "Elite.h"
#include "EliteMsg_Util.h"
#include "Elite_lib_get_imc.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
     Macro Defines
========================================================================== */
static const uint32_t NAME_LEN = QURT_ELITE_DEFAULT_NAME_LEN;

//Max supported length of the key
#define IMC_MAX_KEY_LENGTH 16

//IMC packet id. Hex for "ImcP" string characters without NULL
#define IMC_PACKET_ID       0x50636d49L

//Intermodule communication packet header structure
typedef struct imc_data_packet_header_t
{
   uint32_t         packet_id;
   /**< Magic number to indicate that this packet is allocated by the intermodule communication framework. */
   imc_dest_info_t  *dest_handle;
   /**< Destination handle */
   imc_src_info_t   *src_handle;
   /**< Source handle */
   uint32_t        actual_size;
   /**< Actual size of the packet. payload_size+header_size*/
   uint32_t        max_size;
   /**< Max size of the packet allocated. payload_size+header_size */
   capi_v2_err_t   response_result;
   /**<response result from the destination. */
}imc_data_packet_header_t;

//Intermodule communication packet structure
typedef struct imc_data_packet_t
{
   imc_data_packet_header_t header;
   /**<header of the data packet */
   /**< Actual payload follows here */
}imc_data_packet_t;

// Intermodule communication destination structure
typedef struct imc_dest_info_t
{
   qurt_elite_queue_t  *dest_dataq_ptr;
   /**< Data Queue for handling input data */
   char_t               dataq_name[NAME_LEN];
   /**< data queue name   */
   qurt_elite_channel_t dest_channel;
   /**< Channel for the queues */
   qurt_elite_mutex_t  dest_mutex;
   /**< mutex for destination structure */
   uint32_t            num_pending_ack;
   /**< number of acknowledgments pending from the destination */
   uint32_t            ref_counter;
   /**< reference counter representing total number of
    * destination and sources for each communication instance
    * */
   bool_t              is_active;
   /**< Flag to indication destination is accepting data.
    * This flag is made true when destination registers and
    * false when destination deregisters from the data base
    * */
}imc_dest_info_t;

// Intermodule communication source structure
typedef struct imc_src_info_t
{
   qurt_elite_queue_t  *src_bufq_ptr;
   /**< buffer queue for ack from dest */
   char_t               bufq_name[NAME_LEN];
   /**< buffer queue name   */
   qurt_elite_channel_t src_channel;
   /**< Channel for the queues */
   qurt_elite_mutex_t  src_mutex;
   /**< mutex for source structure */
   uint32_t            num_pending_ack;
   /**< number of pending acknowledgments to the source*/
   bool_t              is_active;
   /**< Flag to indication source is registers to database.
    * This flag is made true when source registers and
    * false when deregisters from the data base
    * */
}imc_src_info_t;

typedef struct imc_db_entry_t
{
   imc_dest_info_t     imc_dest;
   /**< destination info */
   int8_t              key[IMC_MAX_KEY_LENGTH];
   /**< unique key representing intermodule communication */
   uint32              keylen;
   /**< key length. max value of key length is IMC_MAX_KEY_LENGTH  */
   UT_hash_handle       hh;
   /**< handle to the hash table*/
}imc_db_entry_t;

typedef struct imc_db_t
{
   qurt_elite_mutex_t  imc_db_mutex;
   /**< mutex to the imc data base*/
   imc_db_entry_t      *hh_imc_head;
   /**< first element(head) of the database*/
}imc_db_t;


/*==========================================================================
     Function declaration
========================================================================== */
/**
 *  function called by dest to register for intermodule communication
 *  @param [in] dst_handle: pointer to dst handle. Memory will be allocated by
 *   the function. input shouldn't be NULL
 *  @param [in] keyptr: pointer to key used for query into the database.
 *   This needs to be unique for each destination/communication instance. Key value
 *   needs to be same for both destination and source to communicate. First 4 bytes
 *   of the key corresponds to communication Id specific to the type of the modules
 *   communicating and bytes after that need to distinguish different instances of
 *   the same communication.
 *  @param [in] keylen: length of the key. Max key length is limited to 16 bytes
 *  @param [in] dest_dataq_len: length of dest data queue. Need to be power of 2.
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_dest_register(imc_dest_info_t **dst_handle, const int8_t *keyptr,const uint32_t keylen,uint32_t dest_dataq_len);

/**
 *  function called by source to register for intermodule communication
 *  @param [in] src_handle: pointer to source handle. Memory will be allocated by
 *   the function. input shouldn't be NULL
 *  @param [in] src_bufq_len: length of src buf queue length. Need to be power of 2.
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_src_register(imc_src_info_t **src_handle, uint32_t src_bufq_len);

/**
 *  function called by source to allocate buffers and push these buffers to its own queue.
 *  Need to be called after source is registered.
 *  @param [in] src_handle: pointer to source handle.
 *  @param [in] payload_size: payload size.
 *  @param [in] num_buffers: number of buffers to be allocated and pushed to source queue.
 *        This value needs to be less than max queue length(src_bufq_len).
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_src_allocate_push_self_buf(imc_src_info_t *src_handle, uint32_t payload_size, uint32_t num_buffers);

/**
 *  function called by source to open communication.
 *  Need to be called once both source and destinations are registered.
 *  @param [in] src_handle: pointer to source handle.
 *  @param [in] dst_handle: pointer to dst handle to be updated. ptr shouldn't be NULL.
 *   Function returns the destunation pointer from the data base with same key.
 *  @param [in] keyptr: pointer to key used for query into the database.
 *   This needs to be unique for each destination/communication instance. Key value
 *   needs to be same for both destination and source to communicate. First 4 bytes
 *   of the key corresponds to communication Id specific to the type of the modules
 *   communicating and bytes after that need to distinguish different instances of
 *   the same communication.
 *  @param [in] keylen: length of the key. Max key length is limited to 16 bytes
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_src_open_comm(imc_src_info_t *src_handle, imc_dest_info_t **dst_handle,const int8_t *keyptr,const uint32_t keylen);

/**
 *  function called by source to push buffers to destination data queue after successful registration.
 *  @param [in] src_handle: pointer to source handle.
 *  @param [in] dst_handle: pointer to dst handle to be updated.
 *  @param [in] payload_ptr: pointer to the payload.
 *  @param [in] payload_size: payload size.
 *  @return result of type capi_v2_err_t.
 *  This API returns CAPI_V2_EUNSUPPORTED when dst has already deregistered.
 *  In this case imc_src_return_buf() API needs to be called to return the buffer
 *  if it is allocated using imc_src_allocate_push_self_buf API.
 */
capi_v2_err_t imc_src_push_buf(imc_src_info_t *src_handle,imc_dest_info_t *dst_handle, void *payload_ptr,uint32_t payload_size);

/**
 *  function called by dest to pop buffers from its data queue.
 *  @param [in] dst_handle: pointer to dst handle.
 *  @param [in] payload_ptr: pointer to the payload.
 *  @param [in] payload_actual_size_ptr: pointer to the actual payload size filled by source.
 *  @param [in] payload_actual_size_ptr: pointer to the max payload size allocated by source.
 *  @param [in] is_queue_empty: pointer to the is_queue_empty flag which indicates if the dest queue is empty.
 *  When this flag is set, the returned result/payload_ptr can be ignored.
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_dest_pop_buf(imc_dest_info_t *dst_handle, void **payload_ptr,uint32_t *payload_actual_size_ptr,uint32_t *payload_max_size_ptr,bool_t *is_queue_empty);

/**
 *  function called by src to return the buffer popped from its queue in case it fails to push the buffer to destination
 *  @param [in] src_handle: pointer to source handle.
 *  @param [in] payload_ptr: pointer to the payload.
 *  @param [in] response_result: Packet processing result by source
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_src_return_buf(imc_src_info_t *src_handle, void *payload_ptr,capi_v2_err_t response_result);

/**
 *  function called by dest to return buffers to source after processing.
 *  @param [in] dst_handle: pointer to dst handle.
 *  @param [in] payload_ptr: pointer to the payload.
 *  @param [in] response_result: Packet processing result by destination
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_dest_return_buf(imc_dest_info_t *dst_handle, void *payload_ptr,capi_v2_err_t response_result);

/**
 *  function called by src to pop buffers from its buffer queue
 *  @param [in] src_handle: pointer to src handle.
 *  @param [in] payload_ptr: pointer to the payload.
 *  @param [in] resp_result: pointer to the response result from destination.
 *  @param [in] is_queue_empty: pointer to the is_queue_empty flag which indicates if the source queue is empty.
 *  When this flag is set, the returned result can be ignored.
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_src_pop_buf(imc_src_info_t *src_handle, void **payload_ptr, capi_v2_err_t *resp_result_ptr,bool_t *is_queue_empty);

/**
 *  function called by src to allocate packet memory.
 *  @param [in] src_handle: pointer to src handle.
 *  @param [in] payload_ptr: pointer to the payload allocated by the function. Input should not be NULL.
 *  @param [in] payload_size: payload_size.
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_src_allocate_packet(imc_src_info_t *src_handle, void **payload_ptr, uint32_t payload_size);

/**
 *  function called by source to close intermodule communication
 *  @param [in] src_handle: pointer to src handle.
 *  @param [in] dst_handle: pointer to dst handle.
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_src_close_connection(imc_src_info_t *src_handle, imc_dest_info_t *dst_handle);

/**
 *  function called by source to deregister from intermodule communication
 *  @param [in] src_handle: pointer to source handle.
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_src_deregister(imc_src_info_t *src_handle);

/**
 *  function called by destination to deregister from intermodule communication
 *  @param [in] dst_handle: pointer to dest handle.
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_dest_deregister(imc_dest_info_t *dst_handle);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _INTER_MODULE_COMM_H_ */
