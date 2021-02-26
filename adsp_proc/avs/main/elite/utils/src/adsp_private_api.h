/*========================================================================
adsp_privatedefs.h

This file contains private ADSP APIs

Copyright (c) 2010 Qualcomm Technologies, Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/src/adsp_private_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/25/11   RP      Created

========================================================================== */
/**
@file adsp_privatedefs.h

@brief This file contains private ADSP APIs
*/

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

/** @ingroup audio_mtmx_strtr_module_ids
    This module be used for DTS down-mixer related parameters.

    This module supports the following parameter IDs:
       - #DOWNMIXER_PARAM_ID_MIX_LFE_TO_FRONT 
       - #DOWNMIXER_PARAM_ID_STEREO_MODE 
*/
#define MTMX_MODULE_ID_DTS_DOWNMIXER                                  0x00010337

/* Structure payload for:
 * DOWNMIXER_PARAM_ID_MIX_LFE_TO_FRONT*/
 
/** If enabled, this parameter configures the down-mixer to mix the LFE channel
       to the front channel during down-mixing.

       By default, it is disabled

    @msgpayload{downmixer_param_id_mix_lfe_to_front_t}
*/
#define DOWNMIXER_PARAM_ID_MIX_LFE_TO_FRONT              0x00010338

typedef struct downmixer_param_id_mix_lfe_to_front_t downmixer_param_id_mix_lfe_to_front_t;

#include "adsp_begin_pack.h"

struct downmixer_param_id_mix_lfe_to_front_t
{
    uint32_t                  mix_lfe_to_front_enable;
	/* 0: Disabled, 1: Enabled*/
}
#include "adsp_end_pack.h"
;

/* Structure payload for:
 * DOWNMIXER_PARAM_ID_STEREO_MODE*/
 
/** This parameter specifies the stereo mode of the down-mixer.

    @msgpayload{downmixer_param_id_stereo_mode_t}
*/
#define DOWNMIXER_PARAM_ID_STEREO_MODE              0x00010339

typedef struct downmixer_param_id_stereo_mode_t downmixer_param_id_stereo_mode_t;

#include "adsp_begin_pack.h"

struct downmixer_param_id_stereo_mode_t
{
    uint32_t                  stereo_mode;
	/* 0: LO, RO mode (left only, right only)
	     1: LT, RT mode (left total, right total)
     */
}
#include "adsp_end_pack.h"
;
/** @endcond */



/** @ingroup asm_svc_register_for_adspp_vote_notif
    Registers with ASM for any ADSPPM votes.

  @apr_hdr_fields
    Opcode -- ASM_CMD_REGISTER_FOR_ADSPPM_VOTE_EVENT \n
    Dst_port -- 0

  @apr_msgpayload none

  @detdesc
     Registration enables ASM to send events upon any ADSPPM voting.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    The handle is defined by the client as input.

  @dependencies
    None.
*/
#define ASM_CMD_REGISTER_FOR_ADSPPM_VOTE_EVENT    0x00012F2C


/** @ingroup asm_svc_deregister_for_adspp_vote_notif
    Deregisters with ASM for any ADSPPM votes.

  @apr_hdr_fields
    Opcode -- ASM_CMD_DEREGISTER_FROM_ADSPPM_VOTE_EVENT \n
    Dst_port -- 0

  @apr_msgpayload none

  @detdesc
     Deregistration stops ASM from sending events upon any ADSPPM voting.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    The handle is defined by the client as input.

  @dependencies
    None.
*/
#define ASM_CMD_DEREGISTER_FROM_ADSPPM_VOTE_EVENT    0x00012F2D

/** @ingroup asm_event_adsppm_vote_done
    This is an event raised whenever ADSPPM votes are requested/released
    and client has registered.

  @apr_hdr_fields
    Opcode -- ASM_EVENT_ADSPPM_VOTE_DONE \n
    Src_port: - zero

    @apr_msg_payload{asm_event_adsppm_vote_done_t}
    @table{weak_asm_event_adsppm_vote_done_t}

  @return
    None

  @dependencies
    The session/stream must be a valid and opened write or read/write
    session/stream.
*/
#define ASM_EVENT_ADSPPM_VOTE_DONE                           0x00012F2E
/* Structure for a ASM_EVENT_ADSPPM_VOTE_DONE event. */
typedef struct asm_event_adsppm_vote_done_t asm_event_adsppm_vote_done_t;

#define ASM_EVENT_ADSPPM_VOTE_MASK_MIPS      (1)
#define ASM_EVENT_ADSPPM_VOTE_MASK_BW        (1<<1)
#define ASM_EVENT_ADSPPM_VOTE_MASK_DML_BW    (1<<2)
#define ASM_EVENT_ADSPPM_VOTE_MASK_LATENCY   (1<<3)

#include "adsp_begin_pack.h"
/** @weakgroup weak_asm_event_adsppm_vote_done_t
@{ */
/* Payload of the #ASM_EVENT_ADSPPM_VOTE_DONE command*/
struct asm_event_adsppm_vote_done_t
{
   uint32_t event_mask;
   /** which event is raised. Masks are defined above as
    *  ASM_EVENT_ADSPPM_VOTE_MASK_MIPS etc*/

   uint32_t mips_per_thread;
   /** MIPs per thread */
   uint32_t total_mips;
   /** total MIPs*/

   uint32_t bw_dml_ddr_Bps_lsw;
   /** LSW of DML-DDR bandwidth */
   uint32_t bw_dml_ddr_Bps_msw;
   /** MSW of DML-DDR bandwidth */
   uint32_t bw_dml_lpm_Bps_lsw;
   /** LSW of DML-LPM bandwidth */
   uint32_t bw_dml_lpm_Bps_msw;
   /** MSW of DML-LPM bandwidth */

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

/**
  Internal API. Sets loggingg cfg file. This cmd is useful to enable/disable logging in ADSP.
  Some tests fail due to enablement of logs. if logs are enabled, they pass.
  this cmd works only on SIM. On tgt it returns success, but does nothing.
  */
#define AVCS_CMD_SET_LOG_CFG_FILE    0x0001291C

#define AVCS_CMD_LOG_CFG_FILE_LEN    256
/* AVCS cmd to set loggig cfg file */
typedef struct avcs_cmd_set_log_cfg_file_t avcs_cmd_set_log_cfg_file_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_set_log_cfg_file_t
@{ */
/* Payload of the AVCS_CMD_SET_LOG_CFG_FILE message
*/
struct avcs_cmd_set_log_cfg_file_t
{
	uint8_t log_cfg_file_name[AVCS_CMD_LOG_CFG_FILE_LEN];
}
#include "adsp_end_pack.h"
;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_PRIVATEAPI_H_ */
