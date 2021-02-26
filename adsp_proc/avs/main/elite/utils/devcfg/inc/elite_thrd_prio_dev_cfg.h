/*========================================================================

*//** @file lpm_prop.h
This file contains structure definition for LPM hardware properties.

Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: 

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/27/16   mk     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "mmdefs.h"
#include "adsp_error_codes.h"

#ifndef ELITE_THRD_PRIO_DEVCFG_H
#define ELITE_THRD_PRIO_DEVCFG_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum elite_thrd_prio_id_t
{
  ELITETHREAD_STAT_AFE_PORT_MANAGER_PRIO_ID = 0,
  ELITETHREAD_STAT_AFE_FBSP_EXCUR_PRIO_ID,
  ELITETHREAD_AVTIMER_SERVER_PRIO_ID,
  ELITETHREAD_DEFAULT_IST_PRIO_ID,
  ELITETHREAD_DYNA_AFE_INTERNAL_BT_PRIO_ID,
  ELITETHREAD_STAT_AFE_VFR_PRIO_ID,
  ELITETHREAD_STAT_VOICE_TIMER_SVC_PRIO_ID,
  ELITETHREAD_STAT_VOICE_DELIVERY_SVC_PRIO_ID,
  ELITETHREAD_MVM_HIGH_PRIO_ID,
  ELITETHREAD_DYNA_AFE_INTERNAL_FM_PRIO_ID,
  ELITETHREAD_STAT_VOICE_MIXER_SVC_PRIO_ID,
  ELITETHREAD_STAT_AFE_DEV_SVC_PRIO_ID,
  ELITETHREAD_STAT_VOICE_STREAM_SVC_PRIO_ID,
  ELITETHREAD_STAT_AUDIO_STREAM_SVC_PRIO_ID,
  ELITETHREAD_STAT_AUDIO_DEV_SVC_PRIO_ID,
  ELITETHREAD_STAT_ADSP_CORE_SVC_PRIO_ID,
  ELITETHREAD_STAT_AFE_CDC_INT_SVC_PRIO_ID,
  ELITETHREAD_DYNA_MIXER_SVC_PRIO_ID,
  ELITETHREAD_DYNA_MTMX_PSPD_SVC_PRIO_ID,
  ELITETHREAD_DYNA_STREAM_ROUTER_SVC_PRIO_ID,
  ELITETHREAD_MVM_MED_PRIO_ID,
  ELITETHREAD_CVD_VFR_PRIO_ID,
  ELITETHREAD_CVP_LOW_PRIO_ID,
  ELITETHREAD_CVS_MAILBOX_TIMER_PRIO_ID,
  ELITETHREAD_DYNA_LL_PP_SVC_PRIO_ID,
  ELITETHREAD_DYNA_LL_DEC_SVC_PRIO_ID,
  ELITETHREAD_DYNA_LL_ENC_SVC_PRIO_ID,
  ELITETHREAD_DYNA_VOICE_DEC_PRIO_ID,
  ELITETHREAD_DYNA_VOICE_PROC_RX_PRIO_ID,
  ELITETHREAD_DYNA_VOICE_PROC_TX_PRIO_ID,
  ELITETHREAD_MVM_LOW_PRIO_ID,
  ELITETHREAD_DYNA_VOICE_ENC_PRIO_ID,
  ELITETHREAD_DYNA_PP_SVC_PRIO_ID,
  ELITETHREAD_DYNA_DEC_SVC_PRIO_ID,
  ELITETHREAD_DYNA_SWITCH_SVC_PRIO_ID,
  ELITETHREAD_DYNA_ENC_SVC_PRIO_ID,
  ELITETHREAD_DYNA_LSM_SVC_PRIO_ID,
  ELITETHREAD_AUDENCDEC_TEST_PRIO_ID,
  ELITETHREAD_COMMON_TEST_PRIO_ID,
  ELITETHREAD_AMDB_THREADPOOL_PRIO_ID,
  ELITETHREAD_STAT_LSM_SVC_PRIO_ID,
  ELITETHREAD_STAT_AFE_SW_MAD_PRIO_ID,
  ELITETHREAD_STAT_AFE_AANC_PRIO_ID,
  ELITETHREAD_STAT_AFE_SW_DUTY_CYCLE_PRIO_ID,
  ELITETHREAD_CVD_CAL_LOGGER_PRIO_ID,
  ELITETHREAD_THRD_PRIO_ID_MAX
}elite_thrd_prio_id_t;

/**< For information related to thread priority */
typedef struct  elite_thrd_prio_struct_t
{
  elite_thrd_prio_id_t id;
  /**< Priority ID for this thread */

  uint32_t prio;
  /**< Priority of this thread */

}elite_thrd_prio_struct_t;

/** Thread priority Properties Structure
*/
typedef struct  elite_thrd_prio_prop_struct_t
{
  uint32_t num_threads;
  /**< number of threads */
  elite_thrd_prio_struct_t *thrd_prio_ptr;
  /**<  */
} elite_thrd_prio_prop_struct_t;

/**< Unsupported thread priority */
#define ELITETHREAD_UNSUPPORTED_PRIO 256


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //ELITE_THRD_PRIO_DEVCFG_H


