/*========================================================================
adsp_privatedefs.h

This file contains private ADSP APIs

Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/inc/adsp_private_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/25/11   RP      Created

========================================================================== */
/**
@file adsp_privatedefs.h

@brief This file contains private ADSP APIs
*/

#include "adsp_private_api_ext.h"
#include "adsp_prv_avcs_api.h"


#ifndef _ADSP_PRIVATEAPI_H_
#define _ADSP_PRIVATEAPI_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup adsp_privatedefs
    @{ */

/**  Internal API. Opcode for exiting the premordial main
 *   thread */
#define AVCS_CMD_EXIT_PREMORDIAL_THREAD     0x00012910

/**  Internal API. Opcode for deleting all the stored licenses. */
#define AVCS_CMD_DELETE_LICENSES             0x00012911

/**  Internal API. Opcode for deleting all the custom topologies. */
#define AVCS_CMD_CLEAR_PP_DBS   0x00012913


/**
    ID of the AVSYNC Test module.for collecting AVSYNC statistics

    This module supports the following parameter IDs:
    - #AFE_PRV_PARAM_ID_AVSYNC_STATS
    - #AFE_PARAM_ID_ENABLE

    Opcode -- AFE_PRV_MODULE_AVSYNC_TEST
  @newpage
*/
#define AFE_PRV_MODULE_AVSYNC_TEST			0x0001021A

#define AFE_PRV_PARAM_ID_AVSYNC_STATS		0x0001021B

typedef struct afe_prv_avsync_stats_param_t afe_prv_avsync_stats_param_t;

#include "adsp_begin_pack.h"
struct afe_prv_avsync_stats_param_t
{
   uint32_t 	  non_zero_ts_lsw;
   /**< LSW of the timestamp of the nonzero sample */

   uint32_t 	  non_zero_ts_msw;
   /**< MSW of the timestamp of the nonzero sample. the 64 bit value formed by LSW and MSW is interpreted as an unsigned number */

   uint16_t       is_valid;
   /**< whether above TS is valid. 1-> valid, 0-> invalid.*/

   uint16_t       reserved;
   /**< must be set to zero */
}
#include "adsp_end_pack.h"
;

/** @ingroup audio_strm_mgr_module_ids
    This module is alias of AUDPROC_MODULE_ID_MFC.

*/
#define AUDPROC_MODULE_ID_MFC_FAR                                  0x0001092C

/** @ingroup audio_strm_mgr_module_ids
    This module is alias of AUDPROC_MODULE_ID_SAMPLESLIP.

*/
#define AUDPROC_MODULE_ID_SAMPLESLIP_FAR                            0x0001092D

#include "adsp_end_pack.h"
;





/** @ingroup asm_svc_register_for_adspp_vote_notif
    Registers with ASM for any ADSPPM votes.

  @apr_hdr_fields
    Opcode -- ASM_CMD_REGISTER_FOR_ADSPPM_VOTES \n
    Dst_port -- 0

  @apr_msgpayload none

  @detdesc
     Registration enables ASM to send track ADSPPM voting & enable query.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    The handle is defined by the client as input.

  @dependencies
    None.
*/
#define ASM_CMD_REGISTER_FOR_ADSPPM_VOTES    0x00012F36


/** @ingroup asm_svc_deregister_for_adspp_vote_notif
    Deregisters with ASM for any ADSPPM votes.

  @apr_hdr_fields
    Opcode -- ASM_CMD_DEREGISTER_FROM_ADSPPM_VOTE_EVENT \n
    Dst_port -- 0

  @apr_msgpayload none

  @detdesc
     Deregistration stops ASM from tracking ADSPPM voting.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    The handle is defined by the client as input.

  @dependencies
    None.
*/
#define ASM_CMD_DEREGISTER_FROM_ADSPPM_VOTES    0x00012F35

/** @ingroup asm_event_adsppm_vote_done
    This is an cmd to query ADSPPM votes from ADSP

  @apr_hdr_fields
    Opcode -- ASM_CMD_GET_ADSPPM_VOTES \n
    Src_port: - zero

    @apr_msg_payload{asm_event_adsppm_vote_done_t}
    @table{weak_asm_event_adsppm_vote_done_t}

  @return
    ASM_CMDRSP_GET_ADSPPM_VOTES

  @dependencies
    The session/stream must be a valid and opened write or read/write
    session/stream.
*/
#define ASM_CMD_GET_ADSPPM_VOTES                           0x00012F34


/** @ingroup asm_event_adsppm_vote_done
    This is an cmd to query ADSPPM votes from ADSP

  @apr_hdr_fields
    Opcode -- ASM_CMD_GET_ADSPPM_VOTES \n
    Src_port: - zero

    @apr_msg_payload{asm_event_adsppm_vote_done_t}
    @table{weak_asm_event_adsppm_vote_done_t}

  @return
    None

  @dependencies
    The session/stream must be a valid and opened write or read/write
    session/stream.
*/
#define ASM_CMDRSP_GET_ADSPPM_VOTES                        0x00012F33

/* Structure for a ASM_CMD_GET_ADSPPM_VOTES. */
typedef struct asm_cmdrsp_get_adsppm_votes_t asm_cmdrsp_get_adsppm_votes_t;

#define ASM_CMDRSP_ADSPPM_VOTE_MASK_MIPS      (1)
#define ASM_CMDRSP_ADSPPM_VOTE_MASK_BW        (1<<1)
#define ASM_CMDRSP_ADSPPM_VOTE_MASK_LATENCY   (1<<2)

#include "adsp_begin_pack.h"
/** @weakgroup weak_asm_cnd_get_adsppm_votes_t
@{ */
/* Payload of the #ASM_CMD_GET_ADSPPM_VOTES command*/
struct asm_cmdrsp_get_adsppm_votes_t
{
   uint32_t event_mask;
   /** which event is raised. Masks are defined above as
    *  ASM_EVENT_ADSPPM_VOTE_MASK_MIPS etc*/

   uint32_t mips_per_thread;
   /** MIPs per thread */
   uint32_t total_mips;
   /** total MIPs*/

   uint32_t bw_adsp_ddr_Bps_lsw;
   /** LSW of ADSP-DDR bandwidth */
   uint32_t bw_adsp_ddr_Bps_msw;
   /** MSW of ADSP-DDR bandwidth */
   uint32_t bw_adsp_lpm_Bps_lsw;
   /** LSW of ADSP-LPM bandwidth */
   uint32_t bw_adsp_lpm_Bps_msw;
   /** MSW of ADSP-LPM bandwidth */

   uint32_t latency_us;
   /** Latency vote */
   uint32_t latency_request;
   /** Latency is released (0) or requested (1) */
}

#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_event_adsppm_vote_done_t */

/** @} */ /* end_addtogroup adsp_privatedefs */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_PRIVATEAPI_H_ */
