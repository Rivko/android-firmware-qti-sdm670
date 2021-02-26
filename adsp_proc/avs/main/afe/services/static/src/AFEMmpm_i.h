/*========================================================================
  This file contains AFE MMPM related apis

  Copyright (c) 2009-2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEMmpm_i.h#2 $
 ====================================================================== */
#ifndef _AFE_MMPM_I_H_
#define _AFE_MMPM_I_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
 Include files
 ========================================================================== */
#include "qurt_elite_types.h"
#include "AFEMmpm.h"
#include "mmpm.h"

/*==========================================================================
 Defines
 ========================================================================== */
/*Below are the macro functions to calculate individual process kpps related to port's process. These functions were derived from the profiled data.
 * Client process is divided into client framework process and client resampler process.
 *
 * AFE_CLIENT_FWK_KPPS(sample_rate)
 *                                   : client framework processing is function of port's sampling rate (sample_rate)
 *                                     In order to reduce absolute difference between profiled kpps and formulated kpps, we have two formulas, that depends on port's sampling rate
 *                                     if(sample_rate<=48000)  {(300*(sample_rate/8000)*(1/3))} else {1000}
 *                                     300kpps is base value (for port sampling rate less than or equal to 48K) taken from the profiled data when port at 8khz 24bit mono                                    1/3 is scaling factor
 *                                     1000kpps is base is base value (for port sampling rate greater than 48K) taken from the profiled data when port at 192khz 24bit stereo and
 *                                     client at 192khz 16 bit mono  */

#define AFE_CLIENT_FWK_KPPS(sample_rate)   ( ((sample_rate)<=48000) ? ((sample_rate)/80) : (1000) )

/* DTMF_KPPS                         : includes dtmf generation and resampler kpps. Considered for calculating kpps only for Rx port */
#define AFE_DTMF_KPPS                             3000

/*AUD_VOC_MIX_KPPS(sample_rate, num_channels)
 *                                  : audio voice mixing is a function of port's sampling rate (sample_rate) and port channels (num_channels), considered only for Rx port
 *                                    ((50*((sample_rate)/8000)*(num_channels))/3)
 *                                    50kpps is base value taken when port at 8Khz
 *                                    1/3 is scaling factor*/
#define AFE_AUD_VOC_MIX_KPPS(sample_rate,num_channels)          ((17*(sample_rate)*(num_channels))/8000)

/*AFE_INTER_DEINTERLEAVE_KPPS(num_channels)
 *                                 : interleaver/de-interleaver process is function of port's channels (num_channels)
 *                                   215kpps is base value taken when port at 192khz 24bit mono. With this formulation, for 8channels, error is less than 2mpps*/
#define AFE_INTER_DEINTERLEAVE_KPPS(num_channels)         (215*(num_channels))

/*AFE_GROUP_DEVICE_KPPS(num_channels)
 *                                 : group device mux/demux process is function of group device channels (num_channels)
 *                                   10000kpps is base value taken when group device at 192khz 24bit mono.*/

#define AFE_GROUP_DEVICE_KPPS(num_channels)            (10000*(num_channels))


/*DEVICE_PROCESS_KPPS(sample_rate, num_channels)
 *                                 : device process is function of port's sampling rate (sample_rate) and port's channels (num_channels)
 * 									                 ((150*((sample_rate)/8000)*(num_channels))/4)
 * 									                 150kpps is base value taken when port at 8Khz 24bit mono
 * 									                 1/4 is scaling factor */
#define AFE_DEVICE_PROCESS_KPPS(sample_rate,num_channels)       ((3*(sample_rate)*(num_channels))/640)

/*FWK_KPPS                         : framework kpps, 200 is average of all the framework kpps from the profiled data (3clients case)*/
#define AFE_FWK_KPPS                              200

/*AFE_ENC_DEC_FWK_KPPS       : encoder/decoder framework kpps, placeholder value, needs profiling*/
#define AFE_ENC_DEC_FWK_KPPS                     500

/*AFE_ENC_DEC_FWK_BW_BPS : encoder/decoder framework BW (Bps), placeholder value, needs profiling*/
#define AFE_ENC_DEC_FWK_BW_BPS               (0.5*1024*1024)

/*RT_PROXY_KPPS                    : 787kpps is base value taken from profiled data, which is maximum of kpps needed for rtproxy read handler
 * 										               kpps value for rtproxy read and write handler is almost same  */
#define AFE_RT_PROXY_KPPS                         787

/*AFE_LIMITER_ACTIVE_KPPS(sample_rate, num_channels)
 *                                  : limiter active process is function of port's sampling rate(sample_rate) and port channels (num_channels), applicable only for Rx port
 *                                    (100*((sample_rate)/8000)*(num_channels))
 *                                    100kpps is base value taken when port at 8Khz 24bit mono*/
/* New limiter is taking twice of previous limiter. So multiplying the MIPS voting with a factor of 2.
      Once new limiter is optimised voting has to be adjusted after profiling*/
#define AFE_LIMITER_ACTIVE_KPPS(sample_rate,num_channels)              (((sample_rate)*(num_channels))/80)*2

/*AFE_LIMITER_ACTIVE_KPPS(sample_rate, num_channels)
 *                                  : limiter bypass process is function of port's sampling rate(sample_rate) and port channels (num_channels), applicable only for Rx port
 *                                    100(1+(sample_rate/(2*8000)))*(num_channels)
 *                                    100kpps is base value taken when port at 8Khz 24bit mono   */
/* New limiter is taking twice of previous limiter. So multiplying the MIPS voting with a factor of 2.
     Once new limiter is optimised voting has to be adjusted after profiling*/
#define AFE_LIMITER_BYPASS_KPPS(sample_rate,num_channels)    ((100+((sample_rate)/160))*(num_channels))*2

/*AFE_LOOPBACK_GAIN_KPPS              : 390 kpps is for loopback gain function, but will be adding this kpps, whenever loopback is enabled, irrespective
 *                                      of whether gain is unity or non-unity*/
#define AFE_LOOPBACK_GAIN_KPPS                   390

/*AFE_SIDETONE_IIR_KPPS               : 500kpps is for IIR resampler function kpps */
#define AFE_SIDETONE_IIR_KPPS                    500

/*8000kpps is sum of worst case AANC algo kpps and AANC framework kpps*/
#define AFE_AANC_ALGO_FWK_KPPS                  8000

// define maximum concurrent clients handled by afe MMPM wrapper
#define AFE_ADSPPM_REQUEST_MAX          3

#define AFE_ADSPPM_BW_REQUEST_MAX      8

/* Max number of MMPM cores that AFE needs,
   * 1) LPASS_CORE - To avoid unclocked access for some targets, this core
   *                 needs to be registered first and de-registered at last.   *
   * 2) ADSP
   * 3) LPM
   * 4) Depending on port either AIF, SPDIF, HDMI Out */
#define AFE_MMPM_MAX_NUM_CORE   4

/* Enum for the core order*/
typedef enum
{
    LPASS_CORE = 0,
    LPM,
    ADSP,
    DEVICE,
}afe_mmpm_core_type_e;


/* Enum for Power and Reg prog requests*/
typedef enum
{
	AFE_MMPM_VOTING_DISABLE = 0,
	AFE_MMPM_VOTING_ENABLE
}afe_mmpm_voting_flag_e;


typedef struct afe_mmpm_core_info_t
{
	MmpmRegParamType     core;
	uint32_t             client_id;
	afe_mmpm_voting_flag_e power_voting;
	afe_mmpm_voting_flag_e reg_access_voting;
}afe_mmpm_core_info_t;


typedef struct afe_mmpm_voting_t
{
	uint32_t ebi_to_qdsp6ss_bw;
	uint32_t adsp_to_lpm_bw;
	uint32_t lpm_to_aif_bw;
	uint32_t lpm_to_sb_bw;
	uint32_t ddr_to_aif_bw;
	uint32_t ddr_to_spdif_bw;
	uint32_t hdmitx_to_lpm_bw;    //HDMI Output
	uint32_t hdmitx_to_ddr_bw;
	uint32_t cycles_in_kpps;
	/**< sleep latency in usec*/
	uint32_t sleep_latency_us;
}afe_mmpm_voting_t;

/**This structure holds the information related to client's dynamic voting*/
typedef struct afe_client_dynamic_voting_info_t
{
	uint32_t          agr_client_kpps;   /**This variable is used to aggregate total client kpps and afe port's dynamic kpps (limiter and audio voice mixer kpps).
                                          This variable will be updated during client connect/disconnect and DTMF presence/absence.
                                          It's unit is kpps*/
	uint32_t          agr_client_bw;     /**This variable is used to aggregate total client bw.
                                          This variable will be updated during client connect/disconnect.
                                          Not accounting for DTMF, limiter and audio voice mixer bandwidth.
                                          It's unit is Bytes per second*/
	uint32_t          prev_limiter_kpps; /**prev_limiter_kpps holds previous updated limiter kpps. During updating port dynamic kpps, this value is deducted from
                                          aggregated client kpps, and limiter kpps is freshly calculated. This calculated value is stored in prev_limiter_kpps
                                          for future reference
                                          It's unit is kpps*/

	uint32_t          prev_mixer_kpps;  /** prev_mixer_kpps holds previous updated audio voice mixer kpps. During updating port dynamic kpps, this value is deducted
                                          from aggregated client kpps, and again audio voice mixer kpps is freshly calculated. This calculated value is stored in
                                          prev_mixer_kpps for future reference
                                          It's unit is kpps*/
}afe_client_dynamic_voting_info_t;


typedef struct afe_mmpm_info_t
{
	/**< Structure for the core registration information */
	afe_mmpm_core_info_t core_info[AFE_MMPM_MAX_NUM_CORE];
	/**< Structure for bw, mcps, sleep latency voting, we are saving the votes just for debug purpose */
	afe_mmpm_voting_t mmpm_voting;
	/**< To keep track of active topologies */
	uint32_t          mmpm_active_topos;
	/**< To keep track of afe client dynamic voting information*/
	afe_client_dynamic_voting_info_t dyn_vot_info;
}afe_mmpm_info_t;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _AFE_MMPM_I_H_ */
