/**
@file ListenProcSvc_Deprecate.h

@brief This file contains functionalities that are potential candidates
       for deprecation. They need to be supported as HLOS clients
       have active solution using these older approach. When there is no
       longer clients to below functions, we ought to remove.

*/

/*========================================================================

 $Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/src/ListenProcSvc_Deprecate.h#1 $

 Edit History

   when       who     what, where, why
  --------    ---       -----------------------------------------------------
 10/29/2015  Unni      Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef LISTENPROCSVC_DEPRECATE_H_
#define LISTENPROCSVC_DEPRECATE_H_

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "ListenProcSvc_Includes.h"

/*------------------------------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------------------------*/

/*
   This function processes  LSM_SESSION_CMD_SET_PARAM command that is sent to a given session.
   (Alternatively, use LSM_SESSION_CMD_SET_PARAM_V2)

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */
ADSPResult listen_proc_svc_set_param_v1_handler(listen_proc_svc_t *me_ptr);

/*
   This function processes  LSM_SESSION_CMD_GET_PARAM command that is sent to a given session.
   (Alternatively, use LSM_SESSION_CMD_GET_PARAM_V2)

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */
ADSPResult listen_proc_svc_get_param_v1_handler(listen_proc_svc_t *me_ptr);

/**
  Function which process the sound model register command
  (Alternatively, use SET_PARAM with LSM_PARAM_ID_REGISTER_SOUND_MODEL)

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_register_sound_model_handler(listen_proc_svc_t *me_ptr);

/**
  Function which process the sound model deregister command
  (Alternatively, use SET_PARAM with LSM_PARAM_ID_DEREGISTER_SOUND_MODEL)

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult  listen_proc_svc_deregister_sound_model_handler(listen_proc_svc_t *me_ptr);


/**
  Function which process the re-mapping LSM_MODULE_VOICE_WAKEUP
  (Alternatively, use SET_PARAM with LSM_MODULE_VOICE_WAKEUP_V2)

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t
  @param param_id [in] Param id associated with the module

  @return appropriate module id

  @dependencies
  None.
 */
uint32_t listen_proc_remap_deprecated_module_id(listen_proc_svc_t* me_ptr,
                                                uint32_t param_id);


/**
  Function which retrieves topology from old database
  (Alternatively, use AVCS_CMD_REGISTER_TOPOLOGIES)

  @param me_ptr [in] Topology id to be instantiated
  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Success/failure

  @dependencies
  None.
 */
ADSPResult  listen_proc_svc_get_topo_old_db(uint32_t topology_id,
                                            listen_proc_svc_t *me_ptr);

#endif /* LISTENPROCSVC_DEPRECATE_H_ */


