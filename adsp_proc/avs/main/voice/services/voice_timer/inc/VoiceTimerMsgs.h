
/*========================================================================

*//** @file VoiceTimerMsgs.h

Copyright (c) 2009-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_timer/inc/VoiceTimerMsgs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/21/09           creation
========================================================================== */

#ifndef VOICETIMERMSGS_H
#define VOICETIMERMSGS_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"

/** @brief Enumerations for Voice Timer custom msg id's.
*/
enum VOICE_TIMER_MSGS_TYPE
{
   VOICE_TIMER_SUBSCRIBE,
   /**< Msg ID to subscribe to Voice Timer */

   VOICE_TIMER_UNSUBSCRIBE,
   /**< Msg ID to unsubscribe to Voice Timer */

   VOICE_TIMER_HARD_VFR,
   /**< Msg ID to send hard VFR msg to Voice Timer */

   VOICE_TIMER_START_ONESHOT,
   /**< Msg ID to start one shot timer to Voice Timer for given VFR and client subscription */

   VOICE_TIMER_MSGS_END
   /**< Enum to indicate end of msgs */
};

typedef enum{
   VOICEPROC_TX_NEAR=0,
   VOICEPROC_TX_NEAR_OUT,
   VOICEPROC_TX_NEAR_IN,
   VOICEPROC_TX_FAR,
   VOICEPROC_RX,
   VOICE_ENC,
   VOICE_DEC,
   VOICE_ENC_REC,
   VOICE_DEC_REC,
   VOICE_DEC_PKT_REQ,
   VOICE_DEC_PP,
   VOICE_DEC_VDS_PKT_REQ,
   VOICE_DRIVER, /*CVD client to VTM*/
   VOICEPROC_TX_VENC_DELIVERY
} voice_cmn_timer_clients_t;

/** @brief Structure of secondary payload used in Subscribe and Unsubscribe msgs.
        This payload acts as a communication channel between Voice Timer and
        the client. The client must allocated memory and should not destroy
        until Unsubsribe.
*/
typedef struct
{
   qurt_elite_signal_t      *signal_ptr;
   /**< This signal is created by the client and is used for signalling the client
        when time offset expires */

   qurt_elite_signal_t      *signal_end_ptr;
   /**< This signal is created by the client and is used by voice timer to ack back
        to the client when unsubscribe is completed */

   qurt_elite_signal_t      *resync_signal_ptr;
   /**< Re-sync detection from AFE device. This will be asserted when there is too much drift between
        VFR and DMA. Client must clear this signal once handled. If a client does not want to be
	signalled, it can set this to NULL */

   uint32_t                  offset;
   /**< Time offset in usec from hard VFR tick. Client initializes it */

   uint8_t                  signal_enable;
   /**< Flag used by the client to request if signal is needed or not
        0 == not needed, 1 == needed */

   uint8_t                  vfr_mode;
   /**< Indicates the mode of VFR as defined in voice api enum voice_vfr_mode_t.
    MODE VFR_NONE implies soft VFR generated using internal timer*/

   uint32_t                vfr_cycle;
   /**< Indicates the time duration between two vfrs. */

   uint32_t                 client_id;
   /**< Indicates client ID. */

   uint32_t                 vsid;
   /**< Indicates Voice System ID used to device hard VFR timing. */

   uint64_t                 avtimer_timestamp_us;
   /**< Indicates avtimer timestamp corresponding to Soft VFR tick. And is used only for VFR_NONE.
        This parameter is updated at every soft vfr tick and shall be set to 0 by client at the
        time of subscription. */

   uint8_t                  timing_ver;
   /**< Indicates version of timing used for backwards compatibility purposes.
        timing_ver parameter is ignored for VFR_NONE vfr_mode and shall be set to 0 by client.
        Version macros are defined in AFE domain.
        VFR_CLIENT_INFO_VER_1 - timing mechanism before VSID. used for these timing cmds
                                VOICE_CMD_SET_TIMING_PARAMS,
                                VSM_CMD_SET_TIMING_PARAMS
        VFR_CLIENT_INFO_VER_2 - timing mechanism with VSID. used for these timing cmds
                                VSM_CMD_SET_TIMING_PARAMS_V2 */

} Vtm_SubUnsubMsg_t;

/** @brief Structure of primary payload used in Subscribe and Unsubscribe custom msgs.
*/
typedef struct
{
    qurt_elite_queue_t      *pBufferReturnQ;
    /**< This is the queue to which this payload buffer needs to be returned */

    qurt_elite_queue_t      *pResponseQ;
    /**< This is the queue to send the ACK back. NULL indicates no response is required */

    uint32_t                unClientToken;
    /**< Token that should be given in the ACK. This is different than the
         unResponeResult and can be used to identify who send the ACK back by
         the server. */

    uint32_t                unResponseResult;
    /**< This is to be filled with ACK result by the client. */

    uint32_t                unSecOpCode;
    /**< This is the secondary opcode indicating the format for the rest of payload */

    Vtm_SubUnsubMsg_t       *vtm_sub_unsub_payload_ptr;
    /**< Pointer to subscribe or unsubscribe struct. */

} EliteMsg_CustomVtSubUnsubHeaderType;

#endif /* VOICETIMERMSGS_H */
