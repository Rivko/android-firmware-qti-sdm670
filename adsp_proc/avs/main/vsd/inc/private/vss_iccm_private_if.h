#ifndef __VSS_ICCM_PRIVATE_IF_H__
#define __VSS_ICCM_PRIVATE_IF_H__

/*
   Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/vss_iccm_private_if.h#2 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"

/****************************************************************************
 * CVD CONCURRENCY MONITOR CORE APIS                                        *
 ****************************************************************************/

/**
  Event broadcast by the CCM to all active MVM sessions in the system,
  indicating the total number of nb/wb+ streams and vocprocs in the system.
 
  There are two possible events that can trigger the CCM to broadcast this event
  to all active MVM sessions:
 
    1. A #VSS_IMVM_EVT_VOICE_SESSION_ACTIVE event is receieved from an MVM
    session indicating to the CCM that it is going to voice active state.
 
    2. A #VSS_IMVM_EVT_VOICE_SESSION_INACTIVE event is received from an MVM
    session indicating to the CCM that it has gone to voice inactive state.
 
  Each MVM session receving this event utilizes the infomation provided to
  configure the voice path timing offsets for the streams/vocprocs connected
  to it.
 
  @par Payload
  #vss_iccm_evt_active_sessions_t
  
  @return 
  None.

  @dependencies
  None.
 
  @comments
  This event is sent only to sessions whose last status is known to CCM
  as active (i.e. their last notification was #VSS_IMVM_EVT_VOICE_SESSION_ACTIVE).
  
*/
#define VSS_ICCM_EVT_ACTIVE_SESSIONS ( 0x00012E6A )

/** @brief Type definition for #vss_iccm_evt_active_sessions_t.
*/
typedef struct vss_iccm_evt_active_sessions_t vss_iccm_evt_active_sessions_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICCM_EVT_ACTIVE_SESSIONS event.
*/
struct vss_iccm_evt_active_sessions_t
{
  uint16_t num_voice_sessions;
    /**< Total number of voice sesssions in the system. */
  uint16_t num_nb_streams;
    /**< Total number of narrow-band (8 KHz) streams in the system. */
  uint16_t num_wb_streams;
    /**< Total number of wide-band (16 KHz) streams in the system. */
  uint16_t num_swb_streams;
    /**< Total number of super wide-band (32 KHz) streams in the system. */
  uint16_t num_fb_plus_streams;
    /**< Total number of full-band (48 KHz) or higher streams in the system. */
  uint16_t num_nb_vocprocs;
    /**< Total number of narrow-band (8 KHz) vocprocs in the system. */
  uint16_t num_wb_vocprocs;
    /**< Total number of wide-band (16 Khz) vocprocs in the system. */
  uint16_t num_swb_vocprocs;
    /**< Total number of super wide-band (32 KHz) vocprocs in the system. */
  uint16_t num_fb_plus_vocprocs;
    /**< Total number of full-band (48 Khz) or higher vocprocs in the system. */
  uint32_t total_core_kpps;
    /**< VDSP current core KPPS. */
  uint32_t vptx_preemption_safety_margin;
    /**< VPTX Preemption safety margin read from target devcfg  */
  uint32_t vprx_preemption_safety_margin;
    /**< VPRX Preemption safety margin read from target devcfg  */
  uint32_t venc_preemption_safety_margin;
    /**< VENC Preemption safety margin read from target devcfg  */
  uint32_t vdec_preemption_safety_margin; 
    /**< VDEC Preemption safety margin read from target devcfg  */
  uint16_t vptx_proc;
  uint16_t vprx_proc;
  uint16_t venc_proc;
  uint16_t vdec_proc;
}
#include "apr_pack_end.h"
;

#endif /* __VSS_ICCM_PRIVATE_IF_H__ */

