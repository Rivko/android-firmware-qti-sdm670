#ifndef __MVM_API_I_H__
#define __MVM_API_I_H__

/*
   Copyright (c) 2014 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/protected/mvm_api_i.h#1 $
   $Author: pwbldsvc $
*/

#include "mvm_api.h"
#include "cvd_cal_protected_i.h"

/****************************************************************************
 * CVD calibration utility APIs                                             *
 ****************************************************************************/
/* Following utility APIs need to be used with mvm_call() API.

   All the utility APIs are synchronous blocking calls.
*/

/**
  Command used to retrieve the search output tagged to a valid query_handle. 
  If the query_handle is valid, the cal utility will call the call back function 
  registered ( #cvd_cal_query_cb_fn_t ) multiple times for all the mid, pid pairs 
  for all matching entries.

  @par Payload
  #vss_imvm_cmd_cal_query_t

  @return 
  APR_EOK on success

  @dependencies
  None.

  @comments

*/

#define MVM_CMDID_CAL_QUERY ( 0xDEADBE01 )


/** @brief Type definition for #vss_imvm_cmd_cal_query_t.
*/
typedef struct vss_imvm_cmd_cal_query_t vss_imvm_cmd_cal_query_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #MVM_CMDID_CAL_QUERY command.
*/
struct vss_imvm_cmd_cal_query_t
{
  uint32_t query_handle;
  cvd_cal_query_cb_fn_t cb_fn;
  const void* client_data;
}
#include "apr_pack_end.h"
;


/****************************************************************************
 * MVM CSM utility APIs                                                     *
 ****************************************************************************/

typedef enum mvm_csm_client_enum_t{
   MVM_CSM_CLIENT_ENUM_VPTX,
   MVM_CSM_CLIENT_ENUM_VPRX,
   MVM_CSM_CLIENT_ENUM_VENC,
   MVM_CSM_CLIENT_ENUM_VDEC,
   MVM_CSM_CLIENT_ENUM_VSM,
   MVM_CSM_CLIENT_ENUM_VPM,
   MVM_CSM_CLIENT_ENUM_MVM,
   MVM_CSM_CLIENT_ENUM_MAX
 } mvm_csm_client_enum_t;

/** 
  Command used to indicate the vote status of the client. CSM 
  shall update its internal state based off each client vote and 
  trigger MMPM API on need basis. 
   
  Payload #vss_imvm_csm_cmd_request_vote_t 

  @return 
  APR_EOK on success

  @dependencies
  Needs to be used with mvm_call() API. 
  It will be a synchronous blocking call.

  @comments 
  This cmd processing can take upto 1.5ms as it may involve MMPM
  API call. 

*/
#define VSS_IMVM_CSM_CMD_REQUEST_VOTE ( 0x00013280 )

/** @brief Type definition for #vss_imvm_csm_cmd_request_vote_t.
*/
typedef struct vss_imvm_csm_cmd_request_vote_t vss_imvm_csm_cmd_request_vote_t;

#include "apr_pack_begin.h"
/** @brief Payload structure for the
           #VSS_IMVM_CSM_CMD_REQUEST_VOTE command.
*/
struct vss_imvm_csm_cmd_request_vote_t
{
  mvm_csm_client_enum_t client;
  /**< Client enum. */
   
  bool_t vote; 
  /**< Client vote status: 
       TRUE  : indicates client is active and it does need the Q6 clock and BW vote to be UP.
       FALSE : indicates client is inactive and it does not need the Q6 clock and BW vote to be UP.
    */ 
} 
#include "apr_pack_end.h"
;

/**
  Command used to retrieve the number of apr objects used for MVM heap at that time of query.

  @par Payload
  #uint32-t objects_used - the count of objects being used at that point

  @return 
  APR_EOK on success

  @dependencies
  None.

  @comments

*/

#define MVM_CMDID_OBJ_STAT ( 0x0001327B )

#endif /* __MVM_API_I_H__ */

