/**
  @file ListenProcSvc_mmpm.h
  @brief This file declares APIs used to invoke LSM power manager.
*/

/*==============================================================================
  Edit History

  $Header:

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  12/12/14   unni    Moving LSM MMPM from static to dynamic service

==============================================================================*/

/*==============================================================================
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/
#ifndef LSM_MMPM_H
#define LSM_MMPM_H
/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "adsp_error_codes.h"
#include "topo_utils.h"
#include "qurt_elite_adsppm_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *----------------------------------------------------------------------------*/
#if defined (__XCC__)
/* No Bandwidth control on xtensa process types solution */
#define LSM_MMPM_DISABLE_KBPS_VOTING
#endif

/*------------------------------------------------------------------------------
 * Type Declarations
 *----------------------------------------------------------------------------*/
typedef enum {
   LSM_MMPM_VOTE_NONE=0,
   LSM_MMPM_VOTE_UPDATE_BPS_ONLY,
   LSM_MMPM_VOTE_UPDATE_KPPS_ONLY,
   LSM_MMPM_VOTE_ALL,
}lsm_mmpm_vote_t;


/**
 * MMPM interaction parameter related structure
 */
typedef struct
{
  uint32_t client_id;
  /* mmpm client id used for registration */

  qurt_elite_adsppm_client_t           *mmpm_client_ptr;
  /* mmpm client ptr, this will be used only for profiling cases*/

  /* Note: Runtime mmpm changes comprises of supporting runtime failures. Below
   * variables reflects last successful vote values. They don't impact listen
   * call flow */
  uint32_t dbg_current_mpps_per_thread;
  /* Last know successful mips per thread  vote value */
  uint32_t dbg_current_bw_bytes_per_sec;
  /* Last know successful bw byptes per sec vote value */
}lsm_mmpm_info_t;


/*------------------------------------------------------------------------------
 * Function Declarations and Documentation
 *----------------------------------------------------------------------------*/

/**
 * Registers a lsm session with mmpm and populate mmpm client info
 * @param mmpm_info_ptr - lsm session mmpm state variables
 * @param session_id - lsm session id
 *
 * @return adsp result status
 */
ADSPResult lsm_mmpm_register(lsm_mmpm_info_t *mmpm_info_ptr,
                             uint16_t session_id);

/**
 * De-registers from mmpm using mmpm client info
 * @param mmpm_info_ptr - lsm session mmpm state variables
 *
 * @return adsp result status
 */
ADSPResult lsm_mmpm_deregister(lsm_mmpm_info_t *mmpm_info_ptr);

/**
 * Configure mmpm based on type of vote
 * @param mmpm_info_ptr - lsm session mmpm state variables
 * @param topo_ptr - lsm session topology state variables.
 * @param vote - Which voting mechanism to use
 *
 * @return adsp result status
 */
ADSPResult lsm_mmpm_config(lsm_mmpm_info_t* mmpm_info_ptr,
                           topo_t* topo_ptr,
                           lsm_mmpm_vote_t vote);

#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif /*LSM_MMPM_H*/


