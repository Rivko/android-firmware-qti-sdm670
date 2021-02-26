
/*========================================================================
Elite

This file contains an example service belonging to the Elite framework.

Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_timer/inc/VoiceTimer.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/21/10   pg      1) Added hard vfr / sample slip support
                              2) change subscribe/unsubscribe to FADD cust msgs
                              3) major cleanup - use function handlers, coding convention
05/14/10   pg      Spin off VoiceTimer from VoiceDevSvc

========================================================================== */
#ifndef VOICETIMER_H
#define VOICETIMER_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "VoiceMsgs.h"
#include "VoiceTimerMsgs.h"
#include "adsp_vcmn_api.h"
#include "avtimer_drv_api.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
/* Macro for maximum number of timers/clients allowed per VFR source*/
#define VOICE_MAX_TIMERS_PER_VFR_SOURCE       32   
/* Macro for total number of timer/clients allowed across all VFR sources
   Limiting to 32 until new requirements come in. Upto 32 is easy to 
   manage due to availability of bit manipulation intrinsics for 32-bit.
   If the requirement goes beyond 32 then some other logic needs to be used.
   Common pool method for one-shot timers is used to avoid malloc and free
   during run-time.*/
#define VOICE_MAX_TIMERS_TOTAL                32   

#define vtm_t VoiceTimer_t
#define vtm_create VoiceTimerCreate

//Global handle to VDS which is created during timer creation
extern elite_svc_handle_t *vds_handle;

/*list of possible VFR ID's contained in VSID received as part of timing commands - 
 *VSM_CMD_SET_TIMING_PARAMS_V2 and VPM_CMD_SET_TIMING_PARAMS_V2 */
enum
{
   VSID_VFR_ID_0 = VFR_MODE_END,
   VSID_VFR_ID_1,
   VSID_VFR_ID_2,
   VSID_VFR_ID_3,
   VSID_VFR_ID_4,
   VSID_VFR_ID_5,
   VSID_VFR_ID_6,
   VSID_VFR_ID_7,
   VSID_VFR_ID_8,
   VSID_VFR_ID_9,
   VSID_VFR_ID_10,
   VSID_VFR_ID_11,
   VSID_VFR_ID_12,
   VSID_VFR_ID_13,
   VSID_VFR_ID_14,
   VSID_VFR_ID_15,
   NUM_VFR_SOURCES
};

typedef struct vfr_source_context_t
{
   uint32_t            sub_mask;                                     
   /**< Mask to manage subscriptions to timer based on a vfr source*/
   Vtm_SubUnsubMsg_t   *client_subscription_ptr[VOICE_MAX_TIMERS_PER_VFR_SOURCE];
   /**< Array of pointers to store client's payload structure address*/   
   int32_t             client_cnt;
   /**< Number of active clients*/
   qurt_elite_timer_t  *oneshot_timer_ptr[VOICE_MAX_TIMERS_PER_VFR_SOURCE];         
   /**< Array of one-shot timer pointers needed for timer create for each client*/
   uint64_t            vfr_time_stamp;
}vfr_source_context_t;

typedef struct vfr_jitter_info_t
{
   uint64_t            vfr_afe_av_timestamp_us;
   /**< Current AV timer timestamp when VFR received at AFE*/
   uint64_t            vfr_vtm_av_timestamp_us;
   /**< Current AV timer timestamp when VFR received at VTM*/
   uint32_t            vfr_curr_jitter;
   /**< Current VFR jitter = vfr_vtm_av_timestamp_us - vfr_afe_av_timestamp_us*/
   uint32_t            vfr_max_jitter;
   /**< Max VFR jitter detected since start of the call*/
}vfr_jitter_info_t;
/** @brief Structure of Voice Timer instance.
*/
typedef struct vtm_t
{
   qurt_elite_thread_t        timer_thread;
   /**< Thread ID*/
   qurt_elite_queue_t         *cmdq_ptr;
   /**< Command Queue*/
   qurt_elite_channel_t       timer_channel;
   /**< Channel for listening to msgs and signals*/
   qurt_elite_signal_t        soft_vfr_signal;
   /**< Signal used for soft VFR ticks*/
   qurt_elite_timer_t         soft_vfr_timer;
   /**< Timer used to for soft VFR, used for ps calls*/
   uint32_t                   wait_mask;
   /**< Mask to wait on the channel*/
   uint32_t                   rcvd_mask;
   /**< Mask received from the channel*/
   int32_t                    bit_pos;
   /**< Channel bit received*/
   qurt_elite_queue_t         *afe_cmdq_ptr;
   /**< Pointer to AFE command queue*/
   qurt_elite_timer_t         oneshot_timer_struct[VOICE_MAX_TIMERS_TOTAL];
   /**< Array of timer structs needed for timer create for each signal/client*/
   uint32_t                   oneshot_timer_mask;
   /**< Mask to manage subscriptions to one-shot timers*/
   avtimer_drv_handle_t       avt_drv;
   /**< AV Timer registration
        Debug flags/signals for one shot timers */
   qurt_elite_signal_t        test_signal;
   uint32_t                   test_flag;
   qurt_elite_timer_t         test_timer;
   uint32_t                   first_hard_vfr_rcvd;
   vfr_jitter_info_t          vfr_jitter_info;
   vfr_source_context_t       vfr_source_context[NUM_VFR_SOURCES];
   /**< Array of context structures for book keeping data per VFR source */
}vtm_t;

/** Function to create an instance of this service
    @param[in] input_param - This can be any value or pointer to structure required by
        service help setup the instance. This input is not used in this service.
    @param[out] handle - Return handle of created instance
    @return Success/failure of instance creation.
*/
ADSPResult vtm_create(uint32_t input_param, void **handle);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef VOICETIMERSVC_H

