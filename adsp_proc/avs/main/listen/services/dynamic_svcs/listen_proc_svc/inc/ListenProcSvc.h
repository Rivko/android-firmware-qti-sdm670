/**
  @file ListenProcSvc.h
  @brief Listen processing service public header.
*/

/*==============================================================================
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/inc/ListenProcSvc.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  07/22/14    Unni    Topology Support
  11/26/12    Sudhir   Initial version
==============================================================================*/
#ifndef ELITE_LISTEN_PROCSVC_H
#define ELITE_LISTEN_PROCSVC_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "Elite.h"
#include "ListenStreamMgr.h"
#include "adsp_lsm_api.h"

/*------------------------------------------------------------------------------
 * Global definitions/forward declarations
 *----------------------------------------------------------------------------*/

enum lsm_proc_revision_t
{
  LISTEN_PROC_OPEN_USING_APP_ID = 0,
  /**< Session was opened using LSM_SESSION_CMD_OPEN_TX.
       -APP_ID is internally converted to compatible Module IDs
        LSM_VOICE_WAKEUP_APP -> LSM_MODULE_VOICE_WAKEUP_V1
        LSM_VOICE_WAKEUP_APP -> LSM_MODULE_VOICE_WAKEUP_V2
       -Use of New Module Ids using this session during set_param is detected
        and error is thrown. i.e. any other module other than LSM_MODULE_VOICE
        is unsupported during set_param */

  LISTEN_PROC_OPEN_USING_TOPOLOGY_ID
  /**< Session was opened using LSM_SESSION_CMD_OPEN_TX_v2
       -Use of deprecated Module Id LSM_MODULE_VOICE_WAKEUP is detected and
        error is thrown */
};


/**< @brief Data structure for initializing Listen processing service on open
   command */
typedef struct
{
  uint16_t bits_per_sample;
  /**< Number of bits per sample */

  uint32_t sampling_rate;
  /**< sampling rate of the listen block */

  lsm_callback_handle_t *callback_data_ptr;
  /**< LSM callback handle for event generation/ack */

  uint16_t num_channels;
  /**< Number of channels */

  uint32_t topology_id;
  /**< Indicates the list of modules(libraries) using this session */

  lsm_proc_revision_t revision;
  /**< Session revision defining and used to address compatibility details */

  uint16_t session_id;
  /**< Session Id of the session*/

} listen_proc_svc_init_params_t;

/*------------------------------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------------------------*/

/**
  Creates an instance of the  Listen Processing service.

  @param [in] init_param_ptr: Pointer to listen_proc_svc_init_params_t
  @param [out] handle_ptr_ptr: Service entry handle returned to the caller/LSM.

  @return
  Success or failure of the instance creation.

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_create(const listen_proc_svc_init_params_t* init_param_ptr,
                                  void **handle_ptr_ptr);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*#ifndef ELITE_LISTEN_PROCSVC_H*/

