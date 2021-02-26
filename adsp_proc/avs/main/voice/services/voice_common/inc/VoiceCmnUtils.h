
/*========================================================================
Elite

This file contains Common Utilities for Voice.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_common/inc/VoiceCmnUtils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/25/10   pg     Initial version

Supported Utils :
1) Q6 Intrinsic Utils
2) Circular Buffer
3) voice_custom_msg_send - used to send custom msg without any payload
4) voice_custom_vt_sub_unsub_msg_send - used to send custom msg - subscribe/unsubscribe to voice timer svc
========================================================================== */
#ifndef VOICECMNUTILS_H
#define VOICECMNUTILS_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "VoiceMsgs.h"
#include "VoiceMacros.h"
#include "VoiceTimerMsgs.h"
#include "voice_delivery.h"
#include "AFEAvtDrift.h"
#include "AFEVfrHandler.h"
#include "adsp_vcmn_api.h" //for vfr parameters
#include "adsp_vpm_api.h" // for topology ids
/* APR/API related */
#include "EliteAprIf.h"
#include "mmpm.h"
#include "stringl.h"      // for memscpy and memsmove


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/********************** start of utils using q6 intrinsics ******************/
#if  defined(__qdsp6__)
#include <hexagon_protos.h>
/** Gets the most significant bit signal position */
#define voice_get_signal_pos(mask)  Q6_R_cl0_R(mask)
/** Gets the first available free bit position starting from LSB */
#define voice_get_free_pos(mask)  Q6_R_ct1_R(mask)
/** Gets the first high bit starting from LSB */
#define voice_get_high_lsb(mask)  Q6_R_ct0_R(mask)
/** Sets a particular bit */
#define voice_set_bit(mask, bitnum)   Q6_R_setbit_RR(mask, bitnum)
/** Tests a particular bit */
#define voice_tst_bit(mask, bitnum)   Q6_p_tstbit_RR(mask, bitnum)
/** Toggles a particular bit */
#define voice_toggle_bit(mask, bitnum)   Q6_R_togglebit_RR(mask, bitnum)
/** Clears a particular bit */
#define voice_clr_bit(mask, bitnum)   Q6_R_clrbit_RR(mask, bitnum)
/** Population Count (popcount) counts the number of bits in mask that are set.*/
#define voice_popcount(mask)  Q6_R_popcount_P(mask)
#endif
/************************ end of utils using q6 intrinsics ******************/

/** Log ID to identify voiceproc tx instance in audio path */
#define VPTX_AUDIO_LOG_ID 0x0200
/** Macro to convert session index to session number and vice versa.

    This number is ORed with the index to generate the session number.
*/
#define VOICE_MAP_SESSION_ID 0x0100

/**** Async Mechanism utils *****/
/** Generates a 32 bit packed token of the session number and array position
    of the async struct
 */
#define VOICE_ASYNC_STUFF_TOKEN(session_number,free_index) (((session_number) << 16) | (free_index))
/** Extracts array position from packed token
    For now we are just using lower 3 bits to extract sessionNum.
    Some extra logic might be needed in VSM/VPM to make sure sessionIndex
    isn't outside bounds since VSM and VPM might support different number
    of sessions
    */
#define VOICE_ASYNC_EXTRACT_INDEX(token) ((token) & (0x001f))
/** Extracts array position from packed token */
#define VOICE_ASYNC_EXTRACT_SESSION(token) (((token) >> 16) & (0x7))

#define  VOICE_ROUNDTO8(x) ((((uint32_t)(x) + 7) >> 3) << 3)

#define VOICE_INVALID_SAMPLING_RATE (0)
#define VOICE_NB_SAMPLING_RATE      (8000)
#define VOICE_WB_SAMPLING_RATE      (16000)
#define VOICE_SWB_SAMPLING_RATE     (32000)
#define VOICE_FB_SAMPLING_RATE      (48000)
#define VOICE_44_1_SAMPLING_RATE    (44100)

#define VOICE_FRAME_SIZE_44_1  (882)
#define VOICE_FRAME_SIZE_FB  (960)
#define VOICE_FRAME_SIZE_SWB  (640)
#define VOICE_FRAME_SIZE_WB  (320)
#define VOICE_FRAME_SIZE_NB  (160)
#define VOICE_FRAME_SIZE_NB_10MS  (80)

#define VOICE_FRAME_SIZE_NB_20MS  (160)
#define VOICE_MAX_14 ((int16_t) 0x1FFF)
#define VOICE_MIN_14 ((int16_t) 0xE000)

#define VOICE_INVALID_TOPOLOGY      (0)

#define VOICE_CACHE_WIDTH (32)
#define NO_OF_BITS_PER_SAMPLE 16
#define TWO_BYTES_PER_SAMPLE (2)

#define VOICE_MODULE_SIZE_ZERO 0  // This is used to check module size is zero or not.


#define VOICE_NB_LOG_ID 0x00
#define VOICE_WB_LOG_ID 0x01
#define VOICE_FB_LOG_ID 0x02
#define VOICE_SWB_LOG_ID 0x03

static const uint32_t MIN_TIMER_OFFSET = 0;
static const uint32_t MAX_TIMER_OFFSET = 20000; //20msec

/*------------------------------------------------------------------*/
/*                      DSP Clock identification                    */
/*                                                                  */
/* processing time in ms =                                          */
/* mpps*1.5/(clock freq per thread*no. of processing frames per sec)*/
/*------------------------------------------------------------------*/
/**********************************************************************/
/* Tx Hard VFR NB Offsets in us (not accounting for Cache factor)     */
/**********************************************************************/
#define VOICE_NB_TXSTREAM_MINCLK_PROCTIME_US (4830)       // Maximum processing time required by NB Voice Stream at Min clock
                                                          // For the use case of HR/AMR-NB Encoding with NONE Topology
#define VOICE_NB_TXSTREAM_MIDCLK_PROCTIME_US (3200)       // Maximum processing time required by NB Voice Stream at Mid clock
                                                          // For the use case of HR/AMR-NB Encoding with SM_ECNS, DM_FLUENCE,
                                                          // SM_FLUENCEV5, DM_FLUENCEV5, TM_FLUENCE Topologies
#define VOICE_NB_TXSTREAM_MAXCLK_PROCTIME_US (2820)       // Maximum processing time required by NB Voice Stream at Max clock
                                                          // For the use case of 4GV-NB Encoding with any Topology

/**********************************************************************/
/* Tx Hard VFR WB Offsets in us (not accounting for Cache factor)     */
/**********************************************************************/
#define VOICE_WB_TXSTREAM_MINCLK_PROCTIME_US (   0)       // Not applicable (WB runs at Mid clk minimum)
#define VOICE_WB_TXSTREAM_MIDCLK_PROCTIME_US (5230)       // Maximum processing time required by WB Voice Stream at Mid clock
                                                          // For the use case of AMR-WB Encoding with SM_ECNS, DM_FLUENCE,
                                                          // SM_FLUENCEV5, DM_FLUENCEV5, TM_FLUENCE Topologies
#define VOICE_WB_TXSTREAM_MAXCLK_PROCTIME_US (4470)       // Maximum processing time required by WB Voice Stream at Max clock
                                                          // For the use case of eAMR+CTM Encoding with any Topology
/**********************************************************************/
/* Tx Hard VFR FB Offsets in us (not accounting for Cache factor)     */
/**********************************************************************/
#define VOICE_FB_TXSTREAM_MINCLK_PROCTIME_US (2870)       // Maximum processing time required by FB Voice Stream at Min clock
#define VOICE_FB_TXSTREAM_MIDCLK_PROCTIME_US (1900)       // Maximum processing time required by FB Voice Stream at Mid clock
                                                          // For the use case of AMR-WB Encoding with SM_ECNS, DM_FLUENCE,
                                                          // SM_FLUENCEV5, DM_FLUENCEV5, TM_FLUENCE Topologies
#define VOICE_FB_TXSTREAM_MAXCLK_PROCTIME_US ( 950)       // Maximum processing time required by FB Voice Stream at Max clock
                                                          // For the use case of eAMR+CTM Encoding with any Topology
//----------------------------------------------------------------------------------------------------------------------

/**********************************************************************/
/* Rx Hard VFR NB Offsets in us (not accounting for Cache factor)     */
/**********************************************************************/
#define VOICE_NB_RXSTREAM_MINCLK_PROCTIME_US (2980)       // Maximum processing time required by NB Voice Stream at Min clock
                                                          // For the use case of HR/AMR-NB Encoding with NONE Topology
#define VOICE_NB_RXSTREAM_MIDCLK_PROCTIME_US (1980)       // Maximum processing time required by NB Voice Stream at Mid clock
                                                          // For the use case of HR/AMR-NB Encoding with SM_ECNS, DM_FLUENCE,
                                                          // SM_FLUENCEV5, DM_FLUENCEV5, TM_FLUENCE Topologies
#define VOICE_NB_RXSTREAM_MAXCLK_PROCTIME_US (2190)       // Maximum processing time required by NB Voice Stream at Max clock
                                                          // For the use case of 4GV-NB Encoding with any Topology
/**********************************************************************/
/* Rx Hard VFR WB Offsets in us (not accounting for Cache factor)     */
/**********************************************************************/
#define VOICE_WB_RXSTREAM_MINCLK_PROCTIME_US (   0)       // Not applicable (WB runs at Mid clk minimum)
#define VOICE_WB_RXSTREAM_MIDCLK_PROCTIME_US (3480)       // Maximum processing time required by WB Voice Stream at Mid clock
                                                          // For the use case of AMR-WB Encoding with SM_ECNS, DM_FLUENCE,
                                                          // SM_FLUENCEV5, DM_FLUENCEV5, TM_FLUENCE Topologies
#define VOICE_WB_RXSTREAM_MAXCLK_PROCTIME_US (3680)       // Maximum processing time required by WB Voice Stream at Max clock
                                                          // For the use case of eAMR+CTM Encoding with any Topology
/**********************************************************************/
/* Rx Hard VFR FB Offsets in us (not accounting for Cache factor)     */
/**********************************************************************/
#define VOICE_FB_RXSTREAM_MINCLK_PROCTIME_US (4660)       // Maximum processing time required by FB Voice Stream at Min clock
#define VOICE_FB_RXSTREAM_MIDCLK_PROCTIME_US (3090)       // Maximum processing time required by FB Voice Stream at Mid clock
                                                          // For the use case of AMR-WB Encoding with SM_ECNS, DM_FLUENCE,
                                                          // SM_FLUENCEV5, DM_FLUENCEV5, TM_FLUENCE Topologies
#define VOICE_FB_RXSTREAM_MAXCLK_PROCTIME_US (1540)       // Maximum processing time required by FB Voice Stream at Max clock
                                                          // For the use case of eAMR+CTM Encoding with any Topology
//----------------------------------------------------------------------------------------------------------------------

/**********************************************************************/
/* Rx NOVFR NB Offsets in us (not accounting for Cache factor)        */
/**********************************************************************/
#define VOICE_NB_RXSTREAM_NOVFR_MINCLK_PROCTIME_US (   0) // Not applicable (NB runs at Mid clk minimum)
                                                          // For the use case of HR/AMR-NB Encoding with NONE Topology
#define VOICE_NB_RXSTREAM_NOVFR_MIDCLK_PROCTIME_US (2250) // Maximum processing time required by NB Voice Stream at Mid clock
                                                          // For the use case of HR/AMR-NB Encoding with SM_ECNS, DM_FLUENCE,
                                                          // SM_FLUENCEV5, DM_FLUENCEV5, TM_FLUENCE Topologies
#define VOICE_NB_RXSTREAM_NOVFR_MAXCLK_PROCTIME_US (1640) // Maximum processing time required by NB Voice Stream at Max clock
                                                          // For the use case of 4GV-NB Encoding with any Topology

/**********************************************************************/
/* Rx NOVFR WB Offsets in us (not accounting for Cache factor)        */
/**********************************************************************/
#define VOICE_WB_RXSTREAM_NOVFR_MINCLK_PROCTIME_US (   0) // Not applicable (WB runs at Mid clk minimum)
#define VOICE_WB_RXSTREAM_NOVFR_MIDCLK_PROCTIME_US (4150) // Maximum processing time required by WB Voice Stream at Mid clock
                                                          // For the use case of AMR-WB Encoding with NONE, SM_ECNS, DM_FLUENCE,
                                                          // SM_FLUENCEV5, DM_FLUENCEV5, TM_FLUENCE Topologies
#define VOICE_WB_RXSTREAM_NOVFR_MAXCLK_PROCTIME_US (2880) // Maximum processing time required by WB Voice Stream at Max clock
                                                          // For the use case of eAMR+CTM Encoding with any Topology
/**********************************************************************/
/* Rx NOVFR FB Offsets in us (not accounting for Cache factor)        */
/**********************************************************************/
#define VOICE_FB_RXSTREAM_NOVFR_MINCLK_PROCTIME_US (   0) // Not applicable (WB runs at Mid clk minimum)
#define VOICE_FB_RXSTREAM_NOVFR_MIDCLK_PROCTIME_US (3770) // Maximum processing time required by WB Voice Stream at Mid clock
                                                          // For the use case of PCM-FB Encoding with NONE, SM_ECNS, DM_FLUENCE,
                                                          // SM_FLUENCEV5, DM_FLUENCEV5, TM_FLUENCE Topologies
#define VOICE_FB_RXSTREAM_NOVFR_MAXCLK_PROCTIME_US (1880) // Maximum processing time required by WB Voice Stream at Max clock
                                                          // For the use case of eAMR+CTM Encoding with any Topology
//----------------------------------------------------------------------------------------------------------------------

/**********************************************************************/
/* Dec NOVFR NB Offsets in us (not accounting for Cache factor)       */
/**********************************************************************/
#define VOICE_NB_DEC_NOVFR_MINCLK_PROCTIME_US (   0)      // Not applicable (PS runs at Mid clk minimum)
#define VOICE_NB_DEC_NOVFR_MIDCLK_PROCTIME_US (1410)      // Maximum processing time required by NB Voice dec thread at Mid clock
                                                          // for AMR-NB/HR use case.
#define VOICE_NB_DEC_NOVFR_MAXCLK_PROCTIME_US (1220)      // Maximum processing time required by NB Voice Stream at Max clock
                                                          // For 4GV-NB use case

/**********************************************************************/
/* Dec NOVFR NB Offsets in us (not accounting for Cache factor)       */
/**********************************************************************/
#define VOICE_WB_DEC_NOVFR_MINCLK_PROCTIME_US (   0)      // Not applicable (PS runs at 196MHz minimum)
#define VOICE_WB_DEC_NOVFR_MIDCLK_PROCTIME_US (2700)      // Maximum processing time required by NB Voice dec thread at Mid clock
                                                          // For AMR-WB use case
#define VOICE_WB_DEC_NOVFR_MAXCLK_PROCTIME_US (2090)      // Maximum processing time required by NB Voice Stream at Max clock
                                                          // For 4GV-WB use case
/**********************************************************************/
/* Dec NOVFR NB Offsets in us (not accounting for Cache factor)       */
/**********************************************************************/
#define VOICE_FB_DEC_NOVFR_MINCLK_PROCTIME_US (   0)      // Not applicable (PS runs at 196MHz minimum)
#define VOICE_FB_DEC_NOVFR_MIDCLK_PROCTIME_US (2310)      // Maximum processing time required by NB Voice dec thread at Mid clock
                                                          // For PCM-FB use case
#define VOICE_FB_DEC_NOVFR_MAXCLK_PROCTIME_US (1150)      // Maximum processing time required by NB Voice Stream at Max clock
                                                          // For PCM-FB use case
//----------------------------------------------------------------------------------------------------------------------
// PROCESSING TIME SELECTION TABLES
static const uint32_t voice_txstream_proc_time[3][3] =
{
   {VOICE_NB_TXSTREAM_MINCLK_PROCTIME_US, VOICE_NB_TXSTREAM_MIDCLK_PROCTIME_US, VOICE_NB_TXSTREAM_MAXCLK_PROCTIME_US},
   {VOICE_WB_TXSTREAM_MINCLK_PROCTIME_US, VOICE_WB_TXSTREAM_MIDCLK_PROCTIME_US, VOICE_WB_TXSTREAM_MAXCLK_PROCTIME_US},
   {VOICE_FB_TXSTREAM_MINCLK_PROCTIME_US, VOICE_FB_TXSTREAM_MIDCLK_PROCTIME_US, VOICE_FB_TXSTREAM_MAXCLK_PROCTIME_US},
};

static const uint32_t voice_rxstream_proc_time[3][3] =
{
   {VOICE_NB_RXSTREAM_MINCLK_PROCTIME_US, VOICE_NB_RXSTREAM_MIDCLK_PROCTIME_US, VOICE_NB_RXSTREAM_MAXCLK_PROCTIME_US},
   {VOICE_WB_RXSTREAM_MINCLK_PROCTIME_US, VOICE_WB_RXSTREAM_MIDCLK_PROCTIME_US, VOICE_WB_RXSTREAM_MAXCLK_PROCTIME_US},
   {VOICE_FB_RXSTREAM_MINCLK_PROCTIME_US, VOICE_FB_RXSTREAM_MIDCLK_PROCTIME_US, VOICE_FB_RXSTREAM_MAXCLK_PROCTIME_US},
};

static const uint32_t voice_rxstream_proc_time_novfr[3][3] =
{
   {VOICE_NB_RXSTREAM_NOVFR_MINCLK_PROCTIME_US, VOICE_NB_RXSTREAM_NOVFR_MIDCLK_PROCTIME_US, VOICE_NB_RXSTREAM_NOVFR_MAXCLK_PROCTIME_US},
   {VOICE_WB_RXSTREAM_NOVFR_MINCLK_PROCTIME_US, VOICE_WB_RXSTREAM_NOVFR_MIDCLK_PROCTIME_US, VOICE_WB_RXSTREAM_NOVFR_MAXCLK_PROCTIME_US},
   {VOICE_FB_RXSTREAM_NOVFR_MINCLK_PROCTIME_US, VOICE_FB_RXSTREAM_NOVFR_MIDCLK_PROCTIME_US, VOICE_FB_RXSTREAM_NOVFR_MAXCLK_PROCTIME_US},
};
//----------------------------------------------------------------------------------------------------------------------


#define VOICE_MAX_NB_STREAM_KPPS (12000) // AMR-NB + CTM
#define VOICE_MAX_WB_STREAM_KPPS (25000) // AMR-WB/4GV-NB/4GV-WB + CTM
#define VOICE_MAX_FB_STREAM_KPPS (43000) // vocoder + CTM
#define VOICE_CACHE_MISS_FACTOR_OVER_1MHZ (1800LL) //cache miss rate - 80%, this value is 1.8 * 1000
#define VOICE_PER_THREAD_MINCLK (127/3)
#define VOICE_PER_THREAD_MIDCLK (193/3)
#define VOICE_PER_THREAD_MAXCLK (390/3)

// VSM requests these clocks for NB/WB
#define VOICE_PER_THREAD_STREAM_NB           (VOICE_PER_THREAD_MINCLK)
#define VOICE_PER_THREAD_STREAM_WB           (VOICE_PER_THREAD_MIDCLK)

/* Voice Decoder Offsets computation */

// kpps for decoders
#define VOICE_DEC_KPPS_8K_MAX (2000) //clock is bumped up for 4GV-NB, hence it's not the worst case
#define VOICE_DEC_KPPS_16K_MAX (9000)
#define VOICE_DEC_KPPS_48K_MAX (9000) //todo: need to correct, for now assume as WB

// kpps for ECALL. margin is added for copies and control overhead
#define VOICE_ECALL_KPPS_8K_MAX (945+500)        //(VOICE_RESAMPLER_KPPS_8K_MAX+VOICE_HOST_PCM_MARGIN_KPPS)
#define VOICE_ECALL_KPPS_16K_MAX (1074+500*2)    //(VOICE_RESAMPLER_KPPS_16K_MAX+VOICE_HOST_PCM_MARGIN_KPPS*2)
#define VOICE_ECALL_KPPS_48K_MAX (1058+500*6)    //(VOICE_RESAMPLER_KPPS_48K_MAX+VOICE_HOST_PCM_MARGIN_KPPS*6)

// kpps for CTM
#define VOICE_CTM_KPPS_8K_MAX (6500)
#define VOICE_CTM_KPPS_16K_MAX (7500)
#define VOICE_CTM_KPPS_48K_MAX (8500)

// kpps for DTMF DET
#define VOICE_DTMF_DET_KPPS_8K_MAX (520)
#define VOICE_DTMF_DET_KPPS_16K_MAX (850)
#define VOICE_DTMF_DET_KPPS_48K_MAX (1300)

// kpps for FENS
#define VOICE_FENS_KPPS_8K_MAX (1700)
#define VOICE_FENS_KPPS_16K_MAX (3150)
#define VOICE_FENS_KPPS_48K_MAX (9550)  // RT : corrected for 48K

// kpps for SLOWTALK
#define VOICE_SLOWTALK_KPPS_8K_MAX (3575)
#define VOICE_SLOWTALK_KPPS_16K_MAX (2875)
#define VOICE_SLOWTALK_KPPS_48K_MAX (5275)

// kpps for WIDEVOICE
#define VOICE_WIDEVOICE_KPPS_8K_MAX (8800)
#define VOICE_WIDEVOICE_KPPS_16K_MAX (0)   //WV is not supported at 16k
#define VOICE_WIDEVOICE_KPPS_48K_MAX (0)   //WV is not supported at 48k

// total KPPS for vdec thread
#define VOICE_DEC_TOTAL_KPPS_8K_MAX (VOICE_DEC_KPPS_8K_MAX + VOICE_ECALL_KPPS_8K_MAX )
#define VOICE_DEC_TOTAL_KPPS_16K_MAX (VOICE_DEC_KPPS_16K_MAX + VOICE_ECALL_KPPS_16K_MAX)
#define VOICE_DEC_TOTAL_KPPS_48K_MAX (VOICE_DEC_KPPS_48K_MAX + VOICE_ECALL_KPPS_48K_MAX)

// total KPPS for vdec pp thread
#define VOICE_DEC_PP_TOTAL_KPPS_8K_MAX (VOICE_CTM_KPPS_8K_MAX + VOICE_DTMF_DET_KPPS_8K_MAX + VOICE_FENS_KPPS_8K_MAX +     \
                                    VOICE_SLOWTALK_KPPS_8K_MAX + VOICE_WIDEVOICE_KPPS_8K_MAX)
#define VOICE_DEC_PP_TOTAL_KPPS_16K_MAX (VOICE_CTM_KPPS_16K_MAX + VOICE_DTMF_DET_KPPS_16K_MAX + VOICE_FENS_KPPS_16K_MAX +     \
                                    VOICE_SLOWTALK_KPPS_16K_MAX + VOICE_WIDEVOICE_KPPS_16K_MAX)
#define VOICE_DEC_PP_TOTAL_KPPS_48K_MAX (VOICE_CTM_KPPS_48K_MAX + VOICE_DTMF_DET_KPPS_48K_MAX + VOICE_FENS_KPPS_48K_MAX +     \
                                    VOICE_SLOWTALK_KPPS_48K_MAX + VOICE_WIDEVOICE_KPPS_48K_MAX)


// process time in usec
// assumptions:
// 1) Bump up the clock when CTM or WV enabled
// 2) ECALL and TW aren't enabled together ?
// 3) Other modules are disabled when CTM is enabled ?
#define VOICE_DEC_PROCESS_TIME_8K ((VOICE_DEC_TOTAL_KPPS_8K_MAX)                  \
                                    * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ * 20)/(VOICE_PER_THREAD_MIDCLK * 1000)
#define VOICE_DEC_PROCESS_TIME_16K ((VOICE_DEC_TOTAL_KPPS_16K_MAX)              \
                                    * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ * 20)/(VOICE_PER_THREAD_MAXCLK * 1000)
#define VOICE_DEC_PROCESS_TIME_48K ((VOICE_DEC_TOTAL_KPPS_48K_MAX)              \
                                    * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ * 20)/(VOICE_PER_THREAD_MAXCLK * 1000)

#define VOICE_STREAMPP_RX_PROCESS_TIME_8K ((VOICE_DEC_PP_TOTAL_KPPS_8K_MAX)                         \
                                    * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ * 20)/(VOICE_PER_THREAD_MAXCLK * 1000)
#define VOICE_STREAMPP_RX_PROCESS_TIME_16K ((VOICE_DEC_PP_TOTAL_KPPS_16K_MAX)                         \
                                    * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ * 20)/(VOICE_PER_THREAD_MAXCLK * 1000)
#define VOICE_STREAMPP_RX_PROCESS_TIME_48K ((VOICE_DEC_PP_TOTAL_KPPS_48K_MAX)                         \
                                    * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ * 20)/(VOICE_PER_THREAD_MAXCLK * 1000)

typedef struct vcmn_topo_clk_t
{
   uint32_t topology_id;
   uint32_t per_thread_clk; //in Mhz
} vcmn_topo_clk_t;

typedef struct vcmn_mediatype_kpps_t
{
   uint32_t media_type;
   uint32_t kpps; //kilo packets per second
} vcmn_mediatype_kpps_t;

typedef struct vcmn_sampling_kpps_t
{
   uint32_t sampling_rate;
   uint32_t kpps; //kilo packets per second
} vcmn_sampling_kpps_t;

typedef struct vcmn_sampling_topo_clk_t
{
   uint32_t sampling_rate;
   const vcmn_topo_clk_t *vcmn_topo_clk;
} vcmn_sampling_topo_clk_t;

typedef struct
{
   uint16_t Volume;
   /**< gain/volume value (in Q13 format) to be applied on the input */
   int16_t reserved;
   /**< reserved parameter for 32 bit alignment */
} voice_volume_cal_param_t;

typedef struct
{
   uint16_t     mute;           // 0 - unmute, 1 - mute
   uint16_t     ramp_duration;  //duration over which mute comes into effect gradually. it takes values from 0 to 5000msec
} voice_soft_mute_cal_param_t;


// TODO: Derived from RTD Calculator.
// These numbers will be reviewed
#define VOICE_AMRNB_MODEM_ENC_KPPS    7880
#define VOICE_AMRNB_IF2_ENC_KPPS      7880
#define VOICE_EAMR_ENC_KPPS          20860  //SA: reducing after BeAMR optimizations.
#define VOICE_AMRWB_ENC_KPPS         13500
#define VOICE_EFR_ENC_KPPS            7550
#define VOICE_FR_ENC_KPPS             3140
#define VOICE_HR_ENC_KPPS             8850
#define VOICE_PCM_8KHZ_ENC_KPPS          0
#define VOICE_PCM_16KHZ_ENC_KPPS         0
#define VOICE_PCM_48KHZ_ENC_KPPS         0
#define VOICE_PCM_44_1KHZ_ENC_KPPS       0
#define VOICE_G711ALAW_ENC_KPPS       2830
#define VOICE_G711MLAW_ENC_KPPS       2830
#define VOICE_G729AB_ENC_KPPS         7650

#define VOICE_AMRNB_MODEM_DEC_KPPS    2000   //Accounting for only AMR. REINIT is required if BeAMR is enabled.
#define VOICE_AMRNB_IF2_DEC_KPPS      2000
#define VOICE_EAMR_DEC_KPPS           10280  //Accounting for eAMR + BeAMR combination. No REINIT is required if BeAMR is enabled.
#define VOICE_AMRWB_DEC_KPPS          4000
#define VOICE_EFR_DEC_KPPS            1650
#define VOICE_FR_DEC_KPPS             1500
#define VOICE_HR_DEC_KPPS             2000
#define VOICE_PCM_8KHZ_DEC_KPPS          0
#define VOICE_PCM_16KHZ_DEC_KPPS         0
#define VOICE_PCM_48KHZ_DEC_KPPS         0
#define VOICE_PCM_44_1KHZ_DEC_KPPS       0
#define VOICE_G711ALAW_DEC_KPPS       2400
#define VOICE_G711MLAW_DEC_KPPS       2400   // new g711 library
#define VOICE_G729AB_DEC_KPPS         2250

#define VOICE_AMRNB_BEAMR_DEC_KPPS   9930   // KPPS values for AMR-NB+BeAMR.

#define VOICE_INVALID_MEDIATYPE        (0)

static const vcmn_mediatype_kpps_t VOICE_ENCODE_KPPS_TABLE[] =
{
    {VSM_MEDIA_TYPE_AMR_NB_MODEM, VOICE_AMRNB_MODEM_ENC_KPPS },
    {VSM_MEDIA_TYPE_AMR_NB_IF2,   VOICE_AMRNB_IF2_ENC_KPPS   },
    {VSM_MEDIA_TYPE_EAMR,         VOICE_EAMR_ENC_KPPS        },
    {VSM_MEDIA_TYPE_AMR_WB_MODEM, VOICE_AMRWB_ENC_KPPS       },
    {VSM_MEDIA_TYPE_EFR_MODEM,    VOICE_EFR_ENC_KPPS         },
    {VSM_MEDIA_TYPE_FR_MODEM,     VOICE_FR_ENC_KPPS          },
    {VSM_MEDIA_TYPE_HR_MODEM,     VOICE_HR_ENC_KPPS          },
    {VSM_MEDIA_TYPE_PCM_8_KHZ,    VOICE_PCM_8KHZ_ENC_KPPS    },
    {VSM_MEDIA_TYPE_PCM_16_KHZ,   VOICE_PCM_16KHZ_ENC_KPPS   },
    {VSM_MEDIA_TYPE_PCM_48_KHZ,   VOICE_PCM_48KHZ_ENC_KPPS   },
    {VSM_MEDIA_TYPE_G711_ALAW,    VOICE_G711ALAW_ENC_KPPS    },
    {VSM_MEDIA_TYPE_G711_MLAW,    VOICE_G711MLAW_ENC_KPPS    },
    {VSM_MEDIA_TYPE_G729AB,       VOICE_G729AB_ENC_KPPS      },
    {VOICE_INVALID_MEDIATYPE,       0                        }
};

static const vcmn_mediatype_kpps_t VOICE_DECODE_KPPS_TABLE[] =
{
    {VSM_MEDIA_TYPE_AMR_NB_MODEM, VOICE_AMRNB_MODEM_DEC_KPPS },
    {VSM_MEDIA_TYPE_AMR_NB_IF2,   VOICE_AMRNB_IF2_DEC_KPPS   },
    {VSM_MEDIA_TYPE_EAMR,         VOICE_EAMR_DEC_KPPS        },
    {VSM_MEDIA_TYPE_AMR_WB_MODEM, VOICE_AMRWB_DEC_KPPS       },
    {VSM_MEDIA_TYPE_EFR_MODEM,    VOICE_EFR_DEC_KPPS         },
    {VSM_MEDIA_TYPE_FR_MODEM,     VOICE_FR_DEC_KPPS          },
    {VSM_MEDIA_TYPE_HR_MODEM,     VOICE_HR_DEC_KPPS          },
    {VSM_MEDIA_TYPE_PCM_8_KHZ,    VOICE_PCM_8KHZ_DEC_KPPS    },
    {VSM_MEDIA_TYPE_PCM_16_KHZ,   VOICE_PCM_16KHZ_DEC_KPPS   },
    {VSM_MEDIA_TYPE_PCM_48_KHZ,   VOICE_PCM_48KHZ_DEC_KPPS   },
    {VSM_MEDIA_TYPE_G711_ALAW,    VOICE_G711ALAW_DEC_KPPS    },
    {VSM_MEDIA_TYPE_G711_MLAW,    VOICE_G711MLAW_DEC_KPPS    },
    {VSM_MEDIA_TYPE_G729AB,       VOICE_G729AB_DEC_KPPS      },
    {VOICE_INVALID_MEDIATYPE,       0                        }
};


/**
 calculate processing time in microsecs per frame given dsp clock, kpps and frame size in ms
 @param [in] frame_size_ms, unit - ms
 @param [in] kpps
 @param [in] dsp_clock, unit - MHz per thread
 @param [out] processing_time, unit - micro secs
*/
ADSPResult vcmn_calc_processing_time(uint32_t dsp_clock,uint32_t kpps,uint32_t frame_size_ms, uint32_t* processing_time);

/**
 Find the kpps from a given table of vocoders vs kpps
 @param [in] table - pointer to vcmn_mediatype_kpps_t
 @param [in] media_type - media_type (vocoder under use)
 @param [out] kpps - pointer to uint32_t
*/
ADSPResult vcmn_find_vockpps_table(const vcmn_mediatype_kpps_t *voc_kpps_table, uint32_t media_type,  uint32_t *kpps);

/**
 Find the kpps from a give table of sampling rate vs kpps
 @param [in] table - pointer to vcmn_sampling_kpps_t
 @param [in] sampling_rate - sampling rate
 @param [out] kpps - pointer to uint32_t
*/
ADSPResult vcmn_find_kpps_table(const vcmn_sampling_kpps_t *sampling_kpps_table, uint32_t sampling_rate, uint32_t *kpps);
/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/** Structure to define a header for a versioned parameter */
typedef struct
{
   uint16_t version;
   uint16_t reserved;  // for alignment
} voice_cmn_versioned_param_header_t;

/** Structure to track time performance information
    used to profile a single method. Time units are arbitrary
 */
typedef struct
{
   uint32_t   num_samples;
   /**< number of timing data points collected */
   uint32_t   max_time;
   /**< maximum time spent in profiled method */
   uint32_t   min_time;
   /**< minimum time spent in profiled method */
   uint64_t   total_time;
   /**< total time spent in profiled method across num_samples data points*/
   uint32_t   max_cycles;
   /**< maximum time spent in profiled method */
   uint32_t   min_cycles;
   /**< minimum time spent in profiled method */
   uint64_t   total_cycles;
   /**< total time spent in profiled method across num_samples data points*/
} voice_cmn_time_profile_t;


/** Resets state of the profiler.

    @param [out] time_profile_ptr Pointer to created profiling struct
*/
void voice_cmn_time_profile_init(voice_cmn_time_profile_t *time_profile_ptr);

/** Reads samples from the circular buffer
    @param [in] time_profile_ptr -- time profiler instance
    @param [in] time -- time spent in method being profiled
    @param [in] cycles -- num cycles spent in method being profiled (units arbitrary, thread/processor)

*/
void voice_cmn_time_profile_add_data(voice_cmn_time_profile_t *time_profile_ptr, uint32_t time, uint32_t cycles);


/** Structure to track asynchronous handling of rx and tx paths for a given
    command
 */
typedef struct
{
   elite_apr_packet_t   *apr_packet_ptr;
   /**< pointer to original APR packet that generated this Async request */
   uint32_t state_rx;
   /**< check for acks from Rx */
   uint32_t state_tx;
   /**< check for acks from Tx */
   uint32_t result_rx;
   /**< Status of the Rx return */
   uint32_t result_tx;
   /**< Status of the Tx return */
}async_status_rxtx_t;

/** Structure to track asynchronous handling of a given command.

    This structure is used in cases when there is a single path to handle
    or if rx and tx paths are handled synchronously
 */
typedef struct
{
   elite_apr_packet_t   *apr_packet_ptr;
   /**< pointer to original APR packet that generated this Async request */
   uint32_t state;
   /**< check for acks from Tx and Rx */
   uint32_t result;
   /**< Status of the return */
}async_status_t;


/** Structure to track asynchronous communication involving across multiple
    commands
 */

typedef struct
{
   uint32_t tracking_mask;
   /**< Tracking mask to find free position that can be assigned to a command */
   async_status_t async_status[32];
   /**< Array that allows aynchronous handling of up to 32 commands */
}async_struct_t;

/** Structure to track asynchronous communication involving across multiple
    commands with asynchronous handling of both tx and rx paths.
 */
typedef struct
{
   uint32_t tracking_mask;
   /**< Tracking mask to find free position that can be assigned to a command */
   async_status_rxtx_t async_status[32];
   /**< Array that allows aynchronous handling of up to 32 commands */
}async_struct_rxtx_t;

/**
   This is the payload of VOICEPROCTX_RECONFIG_CMD and VOICEPROCRX_RECONFIG_CMD
 */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< This is the queue to which this payload
                                         buffer needs to be returned*/
    qurt_elite_queue_t   *pResponseQ;      /**< This is the queue to send the ACK to.
                                        NULL indicates no response is required*/
    uint32_t            unClientToken;    /**< Token that should be given in the ACK. This is different
                                            than the unResponeResult and can be used to identify
                                            who send the ACK back by the server. */
    uint32_t            unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t            sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    uint32_t            topology_id;      /**< this field tells the new topology */
    uint32_t            sampling_rate;    /**< this field tells the new sampling rate 8000/16000 */
    uint16_t            port_id_tx;        /**< this field tells the new afe tx port id*/
    uint16_t            port_id_rx;        /**< this field tells the new afe rx port id */
    uint16_t            num_channels;     /**< Number of channels in mic path */
    uint16_t            num_ref_channels;  /**<number of channels in reference path*/
    uint8_t             channel_map_near[8];     /**<channel mapping information for near path*/
    uint8_t             channel_map_far[8];      /**<channel mapping information for far path*/
} elite_msg_voice_re_config_type;

typedef struct topo_descriptor_struct
{
   uint32_t topology_id;
   uint32_t num_in_channels;
}topo_descriptor_struct;

typedef enum{
   VOICE_CMN_AVT_NO_RESYNC = 0,
   VOICE_CMN_AVT_RESYNC,
   VOICE_CMN_AVT_INVALID
} voice_cmn_avt_resync_t;

// Used in check_guard_band api to find from which path ( TX or RX ) it is called.
enum voice_path{
   TX=0,
   RX
};

// possible values of BBWE flag
typedef enum
{
    VOICE_BBWE_NONE = 0, /**< BBWE disabled */
    VOICE_BBWE_BEAMR,  /**< BeAMR */
    VOICE_BBWE_WV_V2,  /**< WV2.0 */
    VOICE_BBWE_WV_V1   /**< WV1.0 */
} voice_bbwe_feature_t;

#define VOICE_GUARD_BAND_SIZE 8                        // 8 bytes
#define VOICE_GUARD_BAND_CONTENTS 0xDEADBEEFDEADBEEFLL // Used to initialize guard band memory to catch memory corruption

/************APIs for Memory corruption detection using guard band logic*************/
/** 1. Populate Guard band memory locations allocated after each module and update in guard band pointers array.
    2. Initialize guard band memory locations with known value.
    @param [in/out] Array of guard band memory location pointers updated in the function
    @param [in] Ptr points to module_size[] array
    @param [in] No of guard band pointers
    @param [in] Memory address of start of the 1st module.
    @retun An indication of success or failure
*/
ADSPResult voice_init_guard_band(int8_t *guard_band_ptrs[], uint32_t *module_size_ptr, int16_t no_of_modules, int8_t *mem_pointer);

/** Detects guard band memory corruption
    @param [in] Array of guard band memory location pointers
    @param [in] No of guard band pointers
    @param [in] Tx or Rx path call flag
    @param [in] Session no.
    @retun An indication of success or failure
*/
ADSPResult voice_check_guard_band(int8_t *guard_band_ptrs[], int16_t no_of_modules, voice_path tx_rx_flag, uint32_t session_no);

/** ADD guard band size to input size
    @param [in] Input size in bytes
    @param return input size + guard band size
*/
int32_t voice_add_guard_band(uint32_t input_size);

/***************************** HP/AV Timer related apis *****************************/
// one sample in 8k is 125 micro secs
#define VOICE_8K_SAMPLE_MICRO_SEC  125


typedef union {
   afe_client_avt_drift_info_t  avt_drift_info;
   /*
    * drift info that's obtained after reading the 64bit avt_drift_info from AFE.
    */
   afe_client_vfr_drift_info_t  vfr_drift_info;
   /*
    * drift info that's obtained after reading the vfr drift from AFE.
    */
}voice_afe_drift_info_t;

typedef struct voice_cmn_drift_info_t{

   uint8_t                      nb_sampling_rate_factor;
   /*
    * nb sampling rate factor = sampling rate/8000. Need to init in run time
    */
   uint8_t                      is_resync_pending; 
   /*
    * If set, update voice drift conters with afe drift counters
    */
   int64_t                      voice_drift_counter;
   /*
    * internal drift counter. increment in terms of 125 usec for each sample detected irrespective of sampling rate
    */
   union {
   afe_client_avt_drift_info_t  avt_drift_info;
   /*
    * drift info that's obtained after reading the 64bit avt_drift_info from AFE.
    */
   afe_client_vfr_drift_info_t  vfr_drift_info;
   /*
    * drift info that's obtained after reading the vfr drift from AFE.
    */
   }afe_drift_info;
} voice_cmn_drift_info_t;

/** accumulates the drift
    @param [out] adds the drift info to this location, units = 8k samples
    @param [io] reads previous drift into and updates drift info for next calculation
    @param [in] drift memory ptr allocated by afe
    @param [in] VFR source in case of VFR drift
    @param [in] client requesting the drift update calculation
    @param [in] VFR mode to determine whether to use AV timer or VFR drift
    @param [in] session no. of the client

    @retun void
*/
void voice_cmn_accu_drift(int32_t *ss_info_counter_ptr, voice_cmn_drift_info_t *voice_cmn_drift_info_ptr, void *afe_drift_ptr, uint8_t vfr_src, voice_cmn_timer_clients_t client, uint16_t mode, uint32_t session, uint8_t timing_ver, uint32_t vsid);
void voice_calc_inst_drift(voice_afe_drift_info_t *voice_cmn_drift_info_ptr,void *afe_drift_ptr,
      uint8_t vfr_mode, voice_cmn_timer_clients_t client, uint16_t mode,
      uint32_t session, uint8_t timing_ver, uint32_t vsid, int32_t *inst_drift_us);
static inline uint8_t voice_cmn_get_vfr_source(uint16_t vfr_mode)
{
   if(VFR_HARD_EXT == vfr_mode)
   {
      return AFE_VFR_SOURCE_EXTERNAL;
   }
   else if(VFR_HARD == vfr_mode)
   {
      return AFE_VFR_SOURCE_INTERNAL;
   }
   else
   {
      return 0; //vfr_none case, don't care value
   }
}


/** function returns the result integer result of a/b
    @param [in] numerator
    @param [in] demoninator

    @retun (a/b) if b!=0, else (0)
*/
int32_t voice_cmn_int_div(int32_t a, int32_t b);

/***************************** start of circ buf *****************************/
/** Structure of circular buffer instance */
typedef struct {
   uint32_t sample_size;
   /**< Size of each sample */
   uint32_t unread_samples;
   /**< Number of samples currently in the buffer */
   uint32_t max_samples;
   /**< Maximum number of samples the buffer can hold */
   int8_t *read_ptr;
   /**< Pointer to next sample to read */
   int8_t *write_ptr;
   /**< Pointer to next sample to write */
   int8_t *buf_ptr;
   /**< Start of circular buffer */
   int8_t *end_ptr;
   /**< End of circular buffer */
} circbuf_struct;

/** Enumerations to indicate number of channels */
typedef enum {
   MONO_VOICE = 1,
   /**< Single channel */
   STEREO_VOICE,
   /**< Two channels */
   THREE_CHANNEL_VOICE,
   /**< Three channels */
   QUAD_VOICE,
   /**< Four channels */
} num_channels_enum;

/** Enumerations for return values of circular buffer operations */
typedef enum {
   CIRCBUF_SUCCESS = 0,
   /**< Operation was successful */
   CIRCBUF_FAIL,
   /**< General failure */
   CIRCBUF_OVERFLOW,
   /**< Buffer overflowed */
   CIRCBUF_UNDERFLOW
   /**< Buffer underflowed */
} circbuf_status_enum;

/** Initializes a new circular buffer instance.

    @param [out] structPtr Pointer to created circular buffer
    @param [in] Pointer to memory to use for the buffer
    @param [in] max_samples Maximum number of samples the buffer can accomodate
    @param [in] num_channels Number of channels, mono or stereo
    @param [in] bitsPerChannel Bitwidth of samples per channel

    @return An indication of success or failure
*/
circbuf_status_enum voice_circbuf_init(circbuf_struct *structPtr,
      int8_t *buf_ptr, int32_t max_samples, num_channels_enum \
      num_channels, int32_t bitsPerChannel);

/** Reads samples from the circular buffer
    @param [in] structPtr Circular buffer instance
    @param [out] toPtr Pointer to memory to write samples to
    @param [in] num_samples Number of samples to read

    @retun An indication of success or failure
*/
circbuf_status_enum voice_circbuf_read(circbuf_struct *structPtr,
      int8_t *toPtr, uint32_t num_samples, uint32_t to_buff_size);

/** Writes samples to the circular buffer
    @param [in] structPtr Circular buffer instance
    @param [in] fromPtr Pointer to memory to read samples from
    @param [in] num_samples Number of samples to write

    @retun An indication of success or failure
*/
circbuf_status_enum voice_circbuf_write(circbuf_struct *structPtr,
      int8_t *fromPtr, uint32_t num_samples);

/** Returns a count of the number of available samples for read
    @param [in] structPtr Circular buffer instance
    @param [in] num_samples Number of samples required
    @param [out] availableSamplesPtr Count of samples actually available

    @return Success if number of samples required is greater than or equal to
    number of samples available, failure otherwise
*/
circbuf_status_enum voice_circbuf_read_request(circbuf_struct *structPtr,
       uint32_t num_samples, uint32_t *availableSamplesPtr);

/** Returns a count of the number of available samples/locations for write
    @param [in] structPtr Circular buffer instance
    @param [in] num_samples Number of samples required
    @param [out] availableSamplesPtr Count of amount of locations available
    for writing

    @return Success if number of samples required to be written is lesser than
    or equal to number of samples available to be written, failure otherwise
*/
circbuf_status_enum voice_circbuf_write_request(circbuf_struct *structPtr,
       uint32_t num_samples, uint32_t *availableSamplesPtr);

/** Resets the circular buffer instance by flushing all samples
    @param [in] structPtr Circular buffer instance

    @retun An indication of success or failure
 */
circbuf_status_enum voice_circbuf_reset(circbuf_struct *structPtr);
/***************************** end of circ buf *****************************/

/***************************** start of msg utils *****************************/
/**  */
ADSPResult voice_custom_msg_send(qurt_elite_queue_t *respQ, qurt_elite_queue_t *destQ, uint32_t unSecOpCode,
      uint32_t unClientToken, bool_t waitForAck);
// Helper function to send a custom msg to voice timer for subscribe and unsubscribe
/**  */
ADSPResult voice_custom_vt_sub_unsub_msg_send(qurt_elite_queue_t *respQ, qurt_elite_queue_t *destQ, uint32_t unSecOpCode,
      uint32_t unClientToken, bool_t waitForAck, Vtm_SubUnsubMsg_t *pData);
/***************************** end of msg utils *****************************/

/** Initialize an asynchronous control structure for a command
    @param [out] asyncStruct Created instance
    @param [in] apr_packet_ptr APR packet corresponding to command
    @param [out] free_index Array index of instance returned

    @return An indication of success or failure
*/
ADSPResult voice_init_async_control( async_struct_t *asyncStruct, elite_apr_packet_t *apr_packet_ptr, uint32_t *free_index);
/** Finish asynchronous handling of a given command
    @param [in] asyncStruct Instance of asynchronous control structure to be ended
    @param [in] free_index Array index of instance to be ended

    @return An indication of success or failure
*/
ADSPResult voice_end_async_control( async_struct_t *asyncStruct, uint32_t free_index);

/** Initialize an asynchronous control structure for a command with separate
    handling for tx and rx paths
    @param [out] asyncStruct Created instance
    @param [in] apr_packet_ptr APR packet corresponding to command
    @param [out] free_index Array index of instance returned

    @return An indication of success or failure
*/
ADSPResult voice_init_async_control_rx_tx( async_struct_rxtx_t *asyncStruct, elite_apr_packet_t *apr_packet_ptr, uint32_t *free_index);
/** Finish asynchronous handling of a given command
    @param [in] asyncStruct Instance of asynchronous control structure to be ended
    @param [in] free_index Array index of instance to be ended

    @return An indication of success or failure
*/
ADSPResult voice_end_async_control_rx_tx( async_struct_rxtx_t *asyncStruct, uint32_t free_index);

// Memory Map Utilities
/** Maps a memory region
    @param [in] nMemMapClient qurt_elite memory mapping client
    @param [in] hAprHandle APR handle to use for sending response
    @param [in] pPkt APR packet with mapping parameters

    @return An indication of success or failure
*/
ADSPResult voice_shared_memory_map_regions_process(uint32_t mem_map_client,
                                      elite_apr_handle_t apr_handle,
                                      elite_apr_packet_t *apr_pkt_ptr);

/** Unmaps a memory region
    @param [in] nMemMapClient qurt_elite memory mapping client
    @param [in] hAprHandle APR handle to use for sending response
    @param [in] pPkt APR packet with unmapping parameters

    @return An indication of success or failure
*/
ADSPResult voice_shared_memory_un_map_regions_process(uint32_t mem_map_client,
                                        elite_apr_handle_t apr_handle,
                                        elite_apr_packet_t *apr_pkt_ptr);

/** Returns sampling rate corresponding to a particular media type
    @param [in] unMediaTypeFormat Media type to determine sampling rate for
    @param [in] beamr_flag - BeAMR flag for eAMR/AMR-NB decoders.
                Should be zero for when called to get encoder sampling rate.

    @return Sampling rate of input media type
*/
uint16_t voice_get_sampling_rate( uint32_t unMediaTypeFormat,int16_t beamr_flag, uint16_t evs_sampling_rate );

/** Returns logging id corresponding to a  particular sampling rate
    @param [in] sampling_rate:sampling rate of particular session

    @return logging id corresponding to Sampling rate
*/
uint32_t voice_get_sampling_rate_log_id( uint16_t sampling_rate);

/** Provides index to handle mapping.

    Forces index to have non-zero port to make valid APR port/handle
    @param [in] sessionIndex Index to convert
    @return Generated APR handle
*/
static inline uint16_t voice_map_session_index_to_handle( uint16_t sessionIndex)
{
    return (VOICE_MAP_SESSION_ID | sessionIndex);
}

/** Provides handle to index mapping.

    @param [in] handle Index to convert
    @return Generated session index
*/
static inline uint16_t voice_map_handle_to_session_index( uint16_t handle)
{
   /* for now we are just using lower 3 bits to extract sessionNum.
      Some extra logic might be needed in VSM/VPM to make sure sessionIndex
      isn't outside bounds since VSM and VPM might support different number
      of sessions
    */
   return (handle & 0x7);
}

/** Provides VFR ID from VSID 32-bit number.

    @param [in] VSID 32-bit number
    @return Extracted VFR ID
*/
static inline uint8_t voice_get_vfrid_from_vsid( uint32_t vsid)
{
   /* bits[21:18] in 32-bit VSID represents VFR ID
      refer 80-NF711-1 for details on VSID.
    */
   return ((vsid & 0x003C0000) >> 18);
}

/** Enumerations to indicate version of timing command */
typedef enum {
   VOICE_TIMING_VERSION_1 = 0,
   /**< First version of timing command where VCP does offset computation */
   VOICE_TIMING_VERSION_2,
   /**< Second version of timing command where CVD computes all offsets and passes down */
   VOICE_TIMING_VERSION_3,
   /**< Third version of timing command with VSID support on top of version 2 */
} voice_timing_version_enum;

typedef enum {
    VOICE_COMMON_MMPM_STATUS_INIT = 0,
    VOICE_COMMON_MMPM_STATUS_REGISTERED,
    VOICE_COMMON_MMPM_STATUS_RESOURCES_REQUESTED,
    VOICE_COMMON_MMPM_STATUS_RESOURCES_RELEASED
} voice_common_mmpm_status_t;

typedef struct voice_commom_mmpm_t {

    voice_common_mmpm_status_t  status;        // requests bundled, so one status for now
    uint32                      client_id;
    MmpmRegParamType            reg_param;
    MmpmGenBwValType            bw_val;
    MmpmMipsReqType             mips_req;
    uint32                      sleep_microsec;
} voice_commom_mmpm_t;



/** Total MIPS requirement for VPM sessions (minus 10000 to compensate for VSM NB) in thousands of instructions per second */
#define VPM_NB_TOTAL_MIPS     39
/** Total MIPS requirement for VPM sessions in millions of
 *  instructions per second */
#define VPM_WB_TOTAL_MIPS     49
/** Total MIPS requirement for WB VPM sessions in millions of
 *  instructions per second */

#define VPM_MIN_CLK_TOTAL_MIPS  39
/** Total MIPS requirement for min use case in millions of
 *  instructions per second */
#define VPM_MID_CLK_TOTAL_MIPS  49
/** Total MIPS requirement for mid use case in millions of
 *  instructions per second */
#define VPM_MAX_CLK_TOTAL_MIPS  59
/** Total MIPS requirement for max use case in millions of
 *  instructions per second */

#define VSM_MIN_CLK_TOTAL_MIPS  39
/** Total MIPS requirement for min use case in millions of
 *  instructions per second */
#define VSM_MID_CLK_TOTAL_MIPS  49
/** Total MIPS requirement for mid use case in millions of
 *  instructions per second */
#define VSM_MAX_CLK_TOTAL_MIPS  59
/** Total MIPS requirement for max use case in millions of
 *  instructions per second */

/** Average AXI bus bandwidth requirement, interface is in
 * bytes per second. Left shift by 20 for Mega conversion
 * BW = 15 MBytes/s.  Unused but kept to show how AXI_USAGE is derived
 */
#define VOICE_DEFAULT_AXI_AB    (15<<20)
/** Instantenous AXI bus bandwidth requirement, interface is in
 * bytes per second. Left shift by 20 for Mega conversion
 * BW = 100 MBytes/s
 */
#define VOICE_DEFAULT_AXI_IB    (100<<20)

/* Usage percentage for requiring instantaneous BW value.  First order estimate is
 * (VOICE_DEFAULT_AXI_AB / VOICE_DEFAULT_AXI_IB) as a percentage X.  This is like needing
 * VOICE_DEFAULT_AXI_IB for X% of the time, and 0 bytes/sec for (1-X)% of the time, to give
 * an overall average of VOICE_DEFAULT_AXI_AB.
 */
#define VOICE_DEFAULT_AXI_USAGE (15)     // indicates require peak bandwidth 15% of the time.

/* Latency value for default use case.  Value chosen to prevent power collapse */
#define VOICE_DEFAULT_SLEEP_LATENCY_MICROSEC   (20)

/** Requests mips/bandwidth from the multimedia power manager
    @param [in] mmpm Pointer to voice service's related mmpm
           data, which represents single client/single core
           instance.
    @param [in] clientName Name of client being registered
    @param [in] coreId Client registering to request resources
           on this coreId.

    @return An indication of success or failure
*/
ADSPResult voice_mmpm_register(  voice_commom_mmpm_t *mmpm,
                                 const char *clientName,
                                 MmpmCoreIdType coreId);

/** Requests mips/bandwidth from the multimedia power manager
    @param [in] mmpm Pointer to voice service's related mmpm
           data
    @param [in] uTotalThreadMips Total MIPS required by current application
    @param [in] uFloorThreadMips Maximum MIPS required by any particular thread
    @param [in] Required Instantaneous/Peak AXI bandwidth
    @param [in] Usage percentage of peak BW
    @param [in] uSleepLatency latency acceptable in microseconds
           for this use case

    @return An indication of success or failure
*/
ADSPResult voice_mmpm_config(  voice_commom_mmpm_t *mmpm,
                               uint32_t uTotalThreadMips,
                               uint32_t uFloorThreadMips,
                               uint32_t uAxiIBw,
                               uint32_t uAxiUsage,
                               uint32_t uSleepLatency);

/** Releases previous requets given to MMPM.  Must request (or
    update previous request) prior to release
    @param [in] mmpm Pointer to voice service's related mmpm
           data

    @return An indication of success or failure
*/
ADSPResult voice_mmpm_release(
      voice_commom_mmpm_t *mmpm
);

ADSPResult voice_cmn_check_align_size(
            uint32_t payload_address_lsw,
            uint32_t payload_address_msw,
            uint32_t payload_size,
            uint32_t alignment_check
      );
int16_t voice_get_dec_process_time(uint16_t samp_rate);


ADSPResult voice_cmn_send_vds_apr_request(elite_svc_handle_t* vds_handle_ptr, vds_deliver_apr_packet_t* vds_packet_ptr, uint32_t session_num);


ADSPResult voice_cmn_send_vds_elite_request(uint32_t vds_client_id, uint32_t vds_client_token, uint32_t buffer_id, qurt_elite_queue_t* peer_ptr,
                                            elite_svc_handle_t* vds_handle_ptr, elite_msg_any_t* msg_ptr, uint32_t session_num);

ADSPResult voice_cmn_send_vds_command(uint32_t* vds_client_id, uint32_t vds_client_token, qurt_elite_queue_t* respq_ptr, uint32_t sec_opcode,
                                      qurt_elite_signal_t** signal_ptr, qurt_elite_signal_t* error_signal_ptr, elite_svc_handle_t* vds_handle_ptr,
                                      uint32_t session_num, bool_t is_delivery_tick_reqd, qurt_elite_signal_t*resync_response_signal_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef VOICETIMERSVC_H

