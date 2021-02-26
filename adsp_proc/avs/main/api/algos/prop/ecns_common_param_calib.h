#ifndef ECNS_COMMON_PARAM_CALIB_H
#define ECNS_COMMON_PARAM_CALIB_H

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc. All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/11/17   shrani     Added h2xml comments.
 11/28/17   pag      Version 8 parameter addition for param VOICE_PARAM_FLUENCE_NS.
 ========================================================================== */
#include "mmdefs.h"



#define VOICE_PARAM_FLUENCE_FLEC 0x00010EAB

#include "adsp_begin_pack.h"

typedef struct voice_param_fluence_flec_t voice_param_fluence_flec_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_FLEC", VOICE_PARAM_FLUENCE_FLEC}

    @h2xmlp_description  {Multi-mic frequency domain echo canceler.
Maximum array size for this parameter\n
Version 0 -- 68 bytes\n}
@h2xmlp_version     {0} */

struct voice_param_fluence_flec_t {

    int16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion    {true}
}

         @h2xmle_readOnly       {true} */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_readOnly       {true} */

    int32_t flec_flag;
    /**< @h2xmle_description {flec_flag}
         @h2xmle_default     {13}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t flec_force_adapt_thresh_L16Q15;
    /**< @h2xmle_description {flec_force_adapt_thresh_L16Q15}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t flec_allow_adapt_thresh_L16Q15[4];
    /**< @h2xmle_description {flec_allow_adapt_thresh_L16Q15[4]}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t flec_local_adapt_thresh_L16Q15;
    /**< @h2xmle_description {flec_local_adapt_thresh_L16Q15}
         @h2xmle_default     {19660}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t flec_div_pre_protect_thresh_L16Q15;
    /**< @h2xmle_description {flec_div_pre_protect_thresh_L16Q15}
         @h2xmle_default     {16423}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t flec_div_post_protect_thresh_L16Q15;
    /**< @h2xmle_description {flec_div_post_protect_thresh_L16Q15}
         @h2xmle_default     {8231}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t flec_coeff_Qfac_L16;
    /**< @h2xmle_description {flec_coeff_Qfac_L16}
         @h2xmle_default     {27}
         @h2xmle_range       {0..31}

         @h2xmle_policy       {Advanced} */

    int16_t flec_adapt_constant_L16Q15;
    /**< @h2xmle_description {flec_adapt_constant_L16Q15}
         @h2xmle_default     {26214}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t flec_backoff_max_freq_Hz_L16;
    /**< @h2xmle_description {flec_backoff_max_freq_Hz_L16}
         @h2xmle_default     {400}
         @h2xmle_range       {0..1000}

         @h2xmle_policy       {Advanced} */

    int16_t flec_backoff_coeff_env_tolerance_L16;
    /**< @h2xmle_description {flec_backoff_coeff_env_tolerance_L16}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t flec_ne_flag_max_floor_dB_L16Q8;
    /**< @h2xmle_description {flec_ne_flag_max_floor_dB_L16Q8}
         @h2xmle_default     {-7680}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t flec_ne_flag_vad_thr_dB_L16Q8;
    /**< @h2xmle_description {flec_ne_flag_vad_thr_dB_L16Q8}
         @h2xmle_default     {5120}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t flec_fe_flag_max_floor_dB_L16Q8;
    /**< @h2xmle_description {flec_fe_flag_max_floor_dB_L16Q8}
         @h2xmle_default     {-7680}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t flec_fe_flag_vad_thr_dB_L16Q8;
    /**< @h2xmle_description {flec_fe_flag_vad_thr_dB_L16Q8}
         @h2xmle_default     {5120}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int32_t flec_f_vad_lth_en_L32Q20;
    /**< @h2xmle_description {flec_f_vad_lth_en_L32Q20}
         @h2xmle_default     {0x7D00}
         @h2xmle_range       {0x0..0x7FFFFFFF}

         @h2xmle_policy       {Basic} */

    int32_t flec_f_vad_pf_lth_en_L32Q20;
    /**< @h2xmle_description {flec_f_vad_pf_lth_en_L32Q20}
         @h2xmle_default     {0x7D00}
         @h2xmle_range       {0x0..0x7FFFFFFF}

         @h2xmle_policy       {Basic} */

    int32_t flec_f_vad_ns_lth_en_L32Q20;
    /**< @h2xmle_description {flec_f_vad_ns_lth_en_L32Q20}
         @h2xmle_default     {0x7FFFFFFF}
         @h2xmle_range       {0x0..0x7FFFFFFF}

         @h2xmle_policy       {Basic} */

    int16_t flec_f_vad_pf_hangover_L16;
    /**< @h2xmle_description {flec_f_vad_pf_hangover_L16}
         @h2xmle_default     {5}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t flec_dtd_stats_thresh_L16Q15;
    /**< @h2xmle_description {flec_dtd_stats_thresh_L16Q15}
         @h2xmle_default     {6553}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t flec_dtd_pf_ratio_thresh_L16Q15;
    /**< @h2xmle_description {flec_dtd_pf_ratio_thresh_L16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t flec_dtd_ns_ratio_thresh_L16Q15;
    /**< @h2xmle_description {flec_dtd_ns_ratio_thresh_L16Q15}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t flec_pcd_pf_ratio_threshL16Q15;
    /**< @h2xmle_description {flec_pcd_pf_ratio_threshL16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Basic} */

    int16_t flec_sclip_flag_frac_L16Q15;
    /**< @h2xmle_description {flec_sclip_flag_frac_L16Q15}
         @h2xmle_default     {3277}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t flec_sclip_flag_frac_L16Q15_hb;
    /**< @h2xmle_description {flec_sclip_flag_frac_L16Q15_hb}
         @h2xmle_default     {3277}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t Reserved1;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

}
#include "adsp_end_pack.h"
;

#define VOICE_PARAM_AF_COEFFS 0x00010E19

#include "adsp_begin_pack.h"

typedef struct voice_param_af_coeffs_t voice_param_af_coeffs_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_AF_COEFFS", VOICE_PARAM_AF_COEFFS}
    @h2xmlp_description {Returns the Adaptive Filter (AF) coefficients of the currently configured echo canceler. This parameter is read only.\n \n
	AFCoeffData data structures.\nAFCoeffData is defined in the following topology AF data structure tables and is based on the voice
	processing topology. The structure length is variable based on the number of taps the module uses at the
	time of the query.}
    @h2xmlp_readOnly        {true}
    @h2xmlp_toolPolicy      {RTM}    */

typedef struct voice_param_af_coeffs_sm_ecns_t  {

	uint32_t Voice_processing_topology ;
	/**< @h2xmle_description {Topology ID that defines the mode of
echo cancellation.\n Supported
topology IDs  \n VSS_IVOCPROC_TOPOLOGY_ID_TX_SM_ECNS Single mic ECNS\n
} */

	uint16_t NumAFTaps;
	/**< @h2xmle_description {Number of taps.}
         @h2xmle_default     {0x01}
         @h2xmle_range       {0..400}  */

	uint16_t Reserved ;
	/**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}  */

	int16_t AFCoeffs[0];
	/**< @h2xmle_description {Array of AF coefficients.}

		 @h2xmle_variableArraySize  {"NumAFTaps"}
		 @h2xmle_policy       {Basic} */

}voice_param_af_coeffs_sm_ecns_t
#include "adsp_end_pack.h"
;
/** @h2xmlp_parameter    {"VOICE_PARAM_AF_COEFFS", VOICE_PARAM_AF_COEFFS}
    @h2xmlp_description {Returns the Adaptive Filter (AF) coefficients of the currently configured echo canceler. This parameter is read only.}
     @h2xmlp_readOnly        {true}
     @h2xmlp_toolPolicy      {RTM}    */

#include "adsp_begin_pack.h"

typedef struct voice_param_af_coeffs_dm_fluence_t  {

	uint32_t Voice_processing_topology ;
	/**< @h2xmle_description {Topology ID that defines the mode of
echo cancellation.\n Supported
topology IDs  \n
 VSS_IVOCPROC_TOPOLOGY_ID_TX_DM_FLUENCE Dual mic Fluence\n
} */

	uint16_t NumAF1Taps;
	/**< @h2xmle_description {Number of mic1 adaptive filter taps.}
         @h2xmle_default     {0x01}
         @h2xmle_range       {0..400}  */

	uint16_t NumAF2Taps ;
	/**< @h2xmle_description {Number of mic2 adaptive filter taps}
         @h2xmle_default     {0}
         @h2xmle_range       {0..400}  */

	int16_t AF1Coeffs[0];
	/**< @h2xmle_description {Array of adaptive filter coefficients
corresponding to mic1.}


		 @h2xmle_variableArraySize  {"NumAF1Taps"}
		 @h2xmle_policy       {Basic} */

	int16_t AF2Coeffs[0];
	/**< @h2xmle_description {Array of adaptive filter coefficients
corresponding to mic2.}

		 @h2xmle_variableArraySize  {"NumAF2Taps"}
		 @h2xmle_policy       {Basic} */

}voice_param_af_coeffs_dm_fluence_t
#include "adsp_end_pack.h"
;

/** @h2xmlp_subStruct */

#include "adsp_begin_pack.h"

typedef struct voice_param_aec_path_struct_t {

	uint16_t AFLen;
	/**< @h2xmle_description {Number of AEC paths (0 to 5).}
         @h2xmle_default     {4}
         @h2xmle_range       {4..512}  */

	uint16_t wgtQ ;
	/**< @h2xmle_description {Q factor.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..15}  */

	int16_t wgtFg[0];
	/**< @h2xmle_description {AFLen taps for the wgtFg filter for
a specific AecPath.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..0x7FFF}
		 @h2xmle_variableArraySize  {"AFLen"}
		 @h2xmle_policy       {Basic} */

	int16_t wgtBg[0];
	/**< @h2xmle_description {AFLen taps for the wgtBg filter for
a specific AecPath.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..0x7FFF}
		 @h2xmle_variableArraySize  {"AFLen"}
		 @h2xmle_policy       {Basic} */

}voice_param_aec_path_struct_t
#include "adsp_end_pack.h"
;
/** @h2xmlp_parameter    {"VOICE_PARAM_AF_COEFFS", VOICE_PARAM_AF_COEFFS}
    @h2xmlp_description {Returns the Adaptive Filter (AF) coefficients of the currently configured echo canceler. This parameter is read only.\n\nAecPathStruct variable length payload\nThe total number of taps over all filters for both Fg and Bg arrays must not exceed 2000. Fg and Bg filters
for a particular filter index i use the same length AFLen[i]. Therefore, the sum of AFLen[i] over i from 0 to
(NumAfFilter-1) must not exceed (2000 / 2) = 1000. The maximum size is 4024 bytes.\n
BASE represents the byte offset of the current AecPath i, which has filter length AFLen[i]. The next
AecPathStruct for index (i+1) starts at offset BASE+4+4*AFLen[i].\n\nMaximum size of VOICE_PARAM_AF_COEFFS\nThe memory allocated to get the AF coefficients must be large enough to accommodate the maximum size
of the VOICE_PARAM_AF_COEFFS structure. The maximum size depends on the maximum number of
AF taps allowed for the current topology configuration.}
    @h2xmlp_readOnly        {true}
    @h2xmlp_toolPolicy      {RTM}
    @h2xmlx_expandStructs	{false}    */

#include "adsp_begin_pack.h"

struct voice_param_af_coeffs_t {

	uint32_t Voice_processing_topology ;
	/**< @h2xmle_description {Topology ID that defines the mode of
echo cancellation.\n Supported
topology IDs : \n
- VSS_IVOCPROC_TOPOLOGY_ID_TX_SM_ECNS_V2  Single-mic ECNS v2\n
- VSS_IVOCPROC_TOPOLOGY_ID_TX_QM_FLUENCE_PRO  Quad-mic Fluence Pro\n
- VSS_IVOCPROC_TOPOLOGY_ID_TX_QM_FLUENCE_PROV2  Quad-mic Fluence Pro v2\n
- VSS_IVOCPROC_TOPOLOGY_ID_TX_SM_FLUENCEV5  Single-mic Fluence v5\n
- VSS_IVOCPROC_TOPOLOGY_ID_TX_DM_FLUENCEV5  Dual-mic Fluence v5\n
- VSS_IVOCPROC_TOPOLOGY_ID_TX_DM_FLUENCEV_BROADSIDE\n} */

	uint16_t NumAecPaths;
	/**< @h2xmle_description {Number of AEC paths (0 to 5).}
         @h2xmle_default     {0x01}
         @h2xmle_range       {0..5}  */

	uint16_t Reserved ;
	/**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}  */

	struct voice_param_aec_path_struct_t AecPathData[0];
	/**< @h2xmle_description {AecPathStruct variable length
payload.}
		 @h2xmle_variableArraySize  {"NumAecPaths"}
		 @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_ECHO_PATH_DELAY   0x00010E1E

#include "adsp_begin_pack.h"

typedef struct voice_param_echo_path_delay_t voice_param_echo_path_delay_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_ECHO_PATH_DELAY", VOICE_PARAM_ECHO_PATH_DELAY}

    @h2xmlp_description  {Fixed echo path delay in the firmware. The delay is in multiples of 8 kHz samples. \n
Maximum array size for this parameter \n
Version 0 - 4 bytes \n} */

struct voice_param_echo_path_delay_t {

    int16_t echo_path_delay_L16;

    /**< @h2xmle_description {echo_path_delay_L16}
         @h2xmle_default     {0x01}
         @h2xmle_range       {-512..512}


        @h2xmle_policy       {Basic} */

    int16_t reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */
}
#include "adsp_end_pack.h"
;



#define VOICE_PARAM_FLUENCE_FLEC_STATE 0x00010EAC

#include "adsp_begin_pack.h"

typedef struct voice_param_fluence_flec_state_t voice_param_fluence_flec_state_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_FLEC_STATE", VOICE_PARAM_FLUENCE_FLEC_STATE}
    @h2xmlp_description {Multi mic frequency domain echo canceler monitor.\n
Maximum array size for this parameter \n
Version 0 - 56 bytes \n\n}

    @h2xmlp_version     {0}
    @h2xmlp_toolPolicy  {RTM}

    @h2xmlp_readOnly    {true} */

struct voice_param_fluence_flec_state_t {

    int16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_isVersion    {true}    */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.} */

    int16_t flec_f_vad_flag;
    /**< @h2xmle_description {flec_f_vad_flag} */

    int16_t flec_f_vad_pf_flag;
    /**< @h2xmle_description {flec_f_vad_pf_flag} */

    int16_t flec_f_vad_ns_flag;
    /**< @h2xmle_description {flec_f_vad_ns_flag} */

    int16_t flec_dtd_flag;
    /**< @h2xmle_description {flec_dtd_flag} */

    int16_t flec_dtd_pf_flag;
    /**< @h2xmle_description {flec_dtd_pf_flag} */

    int16_t flec_dtd_ns_flag;
    /**< @h2xmle_description {flec_dtd_ns_flag} */

    int16_t flec_pcd_flag;
    /**< @h2xmle_description {flec_pcd_flag} */

    int16_t flec_sclip_Flag;
    /**< @h2xmle_description {flec_sclip_Flag} */

    int16_t flec_sclip_FlagHB;
    /**< @h2xmle_description {flec_sclip_FlagHB} */

    int16_t flec_allow_adapt_flag [4];
    /**< @h2xmle_description {flec_allow_adapt_flag [4]} */

    int16_t flec_dtd_stats_Q15 [4];
    /**< @h2xmle_description {flec_dtd_stats_Q15 [4]} */

    int16_t flec_adapt_flag_ratio_Q15 [4];
    /**< @h2xmle_description {flec_adapt_flag_ratio_Q15 [4]} */

    int16_t flec_fe_flag_ratio_L16Q15;
    /**< @h2xmle_description {flec_fe_flag_ratio_L16Q15} */

    int16_t flec_pc_flag_ratio_L16Q15;
    /**< @h2xmle_description {flec_pc_flag_ratio_L16Q15} */

    int16_t flec_erle_lin_L16Q5;
    /**< @h2xmle_description {flec_erle_lin_L16Q5} */

    int32_t flec_f_vad_en_L32Q20;
    /**< @h2xmle_description {flec_f_vad_en_L32Q20} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FLUENCE_NS_SPK_STATE 0x00010E40

#include "adsp_begin_pack.h"

typedef struct voice_param_fluence_ns_spk_state_t voice_param_fluence_ns_spk_state_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_NS_SPK_STATE", VOICE_PARAM_FLUENCE_NS_SPK_STATE}
    @h2xmlp_description {Provides internal status monitoring of the noise suppressor for the following speakerphone modes - \n
Fluence version 5 dual-mic broadside
Fluence Pro version 2 quad-mic
Maximum array size for this read-only parameter \n
Version 0 - 980 bytes \n
Version 1 extension - 984 bytes \n
Version 2 extension - 992 bytes \n}

    @h2xmlp_toolPolicy  {RTM}
    @h2xmlp_readOnly    {true}
    @h2xmlp_version     {2}    */

struct voice_param_fluence_ns_spk_state_t {

    uint16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default      {2}
         @h2xmle_isVersion    {true}         */

    uint16_t Reserved;
    /**< @h2xmle_description {Reserved} */

    uint32_t lib_version;
    /**< @h2xmle_description {lib_version}
         @h2xmle_default      {0x70010200}  */

    int16_t refine_vad;
    /**< @h2xmle_description {refine_vad} */

    int16_t sc_vad;
    /**< @h2xmle_description {sc_vad} */

    int32_t sc_vad_snr;
    /**< @h2xmle_description {sc_vad_snr} */

    int16_t onset_vad;
    /**< @h2xmle_description {onset_vad} */

    int16_t offset_vad;
    /**< @h2xmle_description {offset_vad} */

    int16_t onset_score;
    /**< @h2xmle_description {onset_score} */

    int16_t offset_score;
    /**< @h2xmle_description {offset_score} */

    uint8_t polarActGUI [360];
    /**< @h2xmle_description {polarActGUI} */

    int32_t vad_quad;
    /**< @h2xmle_description {vad_quad} */

    int16_t vad_all;
    /**< @h2xmle_description {vad_all} */

    int16_t ang_vad [4];
    /**< @h2xmle_description {ang_vad} */

    int16_t target_sec;
    /**< @h2xmle_description {target_sec} */

    int16_t max_quad;
    /**< @h2xmle_description {max_quad} */

    int16_t non_targ_vad;
    /**< @h2xmle_description {non_targ_vad} */

    int32_t llh_quad [4];
    /**< @h2xmle_description {llh_quad} */

    int32_t llh_max;
    /**< @h2xmle_description {llh_max} */

    int16_t max_doa_2d_sum[2];
    /**< @h2xmle_description {max_doa_2d_sum} */

    int16_t fevad;
    /**< @h2xmle_description {fevad} */

    int16_t single_tar_sec_en;
    /**< @h2xmle_description {single_tar_sec_en} */

    int16_t max_doa_2d [255];
    /**< @h2xmle_description {max_doa_2d} */

    int16_t music_stat;
    /**< @h2xmle_description {music_stat} */

    int16_t spk_is;
    /**< @h2xmle_description {spk_is} */

    int16_t set_num;
    /**< @h2xmle_description {set_num} */

    int32_t norm_harm;
    /**< @h2xmle_description {norm_harm} */

    int32_t rest_ratio;
    /**< @h2xmle_description {rest_ratio} */

    int32_t min_min_stat_noise;
    /**< @h2xmle_description {min_min_stat_noise} */

    int32_t hps_snr;
    /**< @h2xmle_description {hps_snr} */

    int32_t hps_noise_ref;
    /**< @h2xmle_description {hps_noise_ref} */

    int32_t hps_noise_floor;
    /**< @h2xmle_description {hps_noise_floor} */

    int16_t agm_gain_L16Q13 [4];
    /**< @h2xmle_description {agm_gain_L16Q13} */

    int32_t ag_gain32;
    /**< @h2xmle_description {ag_gain32} */

    int32_t hpScVad_snrL32Q8;
    /**< @h2xmle_description {hpScVad_snrL32Q8} */

    int16_t hp_ScVAD;
    /**< @h2xmle_description {hp_ScVAD} */

    int16_t avgMaskGainL16Q14;
    /**< @h2xmle_description {avgMaskGainL16Q14} */


}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FLUENCE_NS 0x00010E26

#include "adsp_begin_pack.h"

typedef struct voice_param_fv5ecns_t voice_param_fv5ecns_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_NS", VOICE_PARAM_FLUENCE_NS}

    @h2xmlp_description  {Fluence version 5 ECNS.\n
Maximum array size for this parameter \n
Version 0 - 904 bytes \n
Version 1 extension  908 bytes \n
Version 2 extension  912 bytes \n
Version 3 extension  1108 bytes \n
Version 4 extension  5380 bytes \n
Version 5 extension  5496 bytes \n
Version 6 extension  5576 bytes \n
Version 7 extension  5584 bytes \n
Version 8 extension  7400 bytes \n \n}
@h2xmlp_version     {8} */

struct voice_param_fv5ecns_t {

    int16_t version;
    /**< @h2xmle_description {READ ONLY parameter to check the  version}
         @h2xmle_default     {8}
         @h2xmle_range       {0..32767}
          @h2xmle_isVersion    {true}


         @h2xmle_readOnly       {true} */

    int16_t reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t mic_dist_L16Q0;
    /**< @h2xmle_description {Distance between two mics in mm}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t ph_vad_lb_idx_L16;
    /**< @h2xmle_description {Lower bound frequency index for Dir VAD computation}
         @h2xmle_default     {16}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t ph_vad_ub_idx_L16;
    /**< @h2xmle_description {Upper bound frequency index for Dir VAD computation}
         @h2xmle_default     {38}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t prox_vad_lb_idx_L16;
    /**< @h2xmle_description {Lower bound frequency index for DM VAD computation}
         @h2xmle_default     {10}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t prox_vad_ub_idx_L16;
    /**< @h2xmle_description {Upper bound frequency index for DM VAD computation}
         @h2xmle_default     {128}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t lf_prox_vad_lb_idx_L16;
    /**< @h2xmle_description {Lower bound frequency index for DM VAD2 computation}
         @h2xmle_default     {1}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t lf_prox_vad_ub_idx_L16;
    /**< @h2xmle_description {Upper bound frequency index for DM VAD2 computation}
         @h2xmle_default     {25}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t do_qsf_lb_idx_L16;
    /**< @h2xmle_description {Lower bound frequency index for masking (QSF) processing}
         @h2xmle_default     {16}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t do_qsf_ub_idx_L16;
    /**< @h2xmle_description {Upper bound frequency index for masking (QSF) processing}
         @h2xmle_default     {38}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t reserved1;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t fp_vad_flags;
    /**< @h2xmle_description {VAD Flags mode word for enabling different VADs in Fluence V5}
         @h2xmle_default     {515}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t sm_en_L16Q14;
    /**< @h2xmle_description {Smoothing factor for the input energy before single-mic VAD noise floor tracking (Higher value means more smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_thres_dual_L16Q8;
    /**< @h2xmle_description {Single-mic VAD threshold for IS in dual-mic mode}
         @h2xmle_default     {512}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t snr_thres_single_L16Q8;
    /**< @h2xmle_description {Single-mic VAD threshold for IS in single-mic mode}
         @h2xmle_default     {512}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm_vad_hangover_max_dual_L16;
    /**< @h2xmle_description {Single-mic VAD hangover for IS in dual-mic mode (in frames)}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_vad_hangover_max_single_L16;
    /**< @h2xmle_description {Single-mic VAD hangover for IS in single-mic mode (in frames)}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_vad_init_alpha_L16Q15;
    /**< @h2xmle_description {Single-mic VAD input energy smoothing factor for noise floor tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_vad_u_L16;
    /**< @h2xmle_description {Single-mic VAD noise floor tracking window length}
         @h2xmle_default     {10}
         @h2xmle_range       {1..12}


         @h2xmle_policy       {Advanced} */

    int16_t sm_vad_v_L16;
    /**< @h2xmle_description {Single-mic VAD noise floor tracking sub-window length}
         @h2xmle_default     {10}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t sm_vad_m_d_L16Q15;
    /**< @h2xmle_description {Single-mic VAD noise floor tracking bias correction parameter}
         @h2xmle_default     {30147}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm_vad_m_v_L16Q15;
    /**< @h2xmle_description {Single-mic VAD noise floor tracking bias correction parameter-II}
         @h2xmle_default     {21889}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int32_t sig_energy_thres_l32;
    /**< @h2xmle_description {Thermal noise floor threshold for Dir VAD update}
         @h2xmle_default     {180}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t start_idx_sd_L16;
    /**< @h2xmle_description {Lower bound frequency index for Spectral deviation (SD) computation}
         @h2xmle_default     {7}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t end_idx_sd_L16;
    /**< @h2xmle_description {Upper bound frequency index for Spectral Deviation (SD) computation}
         @h2xmle_default     {71}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t ub_sd_L16Q12;
    /**< @h2xmle_description {Upper bound scaling factor on the noise magnitude for establishing the upper threshold of SD score update}
         @h2xmle_default     {12288}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t lb_sd_L16Q12;
    /**< @h2xmle_description {Lower bound scaling factor on the noise magnitude for establishing the lower threshold of SD score update}
         @h2xmle_default     {1365}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_sds_L16Q15;
    /**< @h2xmle_description {Decay time smoothing factor for the Spectral deviation score}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thres_sds_nc_L16Q7;
    /**< @h2xmle_description {Threshold for SD score for updating the maxima tracking in the normalization of VAD scores}
         @h2xmle_default     {-1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thres_sds_is_L16Q7;
    /**< @h2xmle_description {Threshold for SD score for IS decision update}
         @h2xmle_default     {4000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t init_int_n_score_with_c_L16;
    /**< @h2xmle_description {Initial time period to allow for convergence of VAD normalization}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t init_min_dm_score_L16Q11;
    /**< @h2xmle_description {Minimal value of the DM VAD score to be used during the initial period}
         @h2xmle_default     {-50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t init_max_dm_score_L16Q11;
    /**< @h2xmle_description {Maximal value of the DM VAD score to be used during the initial period}
         @h2xmle_default     {1900}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t init_max_dir_score_L16Q14;
    /**< @h2xmle_description {Maximal value of the Dir VAD score to be used during the initial period}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dir_score_att_L16Q14;
    /**< @h2xmle_description {Attack time smoothing parameter for updating the Dir VAD score (Higher value means less smoothing)}
         @h2xmle_default     {9830}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dir_score_dec_L16Q14;
    /**< @h2xmle_description {Decay time smoothing parameter for updating the Dir VAD score (Higher value means less smoothing)}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_ltm_dir_score_L16Q14;
    /**< @h2xmle_description {Smoothing parameter for updating the long term Dir VAD score used in IS update (Higher value means less smoothing)}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t cos_look_dir_lb_L16[4];
    /**< @h2xmle_description {Lower bound for Cosine of the angle defining Sector []}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t cos_look_dir_ub_L16[4];
    /**< @h2xmle_description {Upper bound for Cosine of the angle defining Sector []}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t use_dm_score4_is_L16;
    /**< @h2xmle_description {Unused parameter}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t hangover_max_is_L16;
    /**< @h2xmle_description {Hangover duration for IS switching from dual-mic mode to single-mic mode}
         @h2xmle_default     {75}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t init_interval_leng_is_L16;
    /**< @h2xmle_description {Number of active speech frames after the call start up during which IS is forced to be in dual-mic mode}
         @h2xmle_default     {30}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm_thres1_is_L16Q11;
    /**< @h2xmle_description {Lower threshold for proximity Score in order to let IS switch to single-mic mode (IS Approach - II)}
         @h2xmle_default     {-100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm_thres2_is_L16Q11;
    /**< @h2xmle_description {Upper threshold for proximity Score in order to let IS switch to dual-mic mode (IS Approach - II)}
         @h2xmle_default     {600}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dir_thres_is_L16Q14;
    /**< @h2xmle_description {Dir VAD threshold for controlling IS switching decisions}
         @h2xmle_default     {4000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dir_c_comp_L16Q2;
    /**< @h2xmle_description {Compression factor for the Dir VAD score contrast computation}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_c_dir_L16Q14;
    /**< @h2xmle_description {Minimum bound for the contrast of Dir VAD score}
         @h2xmle_default     {2458}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dir_vad_init_alpha_L16Q15;
    /**< @h2xmle_description {Dir VAD score smoothing factor for maxima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dir_vad_u_L16;
    /**< @h2xmle_description {Dir VAD maxima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t dir_vad_v_L16;
    /**< @h2xmle_description {Dir VAD maxima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}


         @h2xmle_policy       {Advanced} */

    int16_t dir_vad_m_d_L16Q15;
    /**< @h2xmle_description {Dir VAD maxima tracking bias correction parameter}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dir_vad_m_v_L16Q15;
    /**< @h2xmle_description {Dir VAD maxima tracking bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dir_mf_sm2_L16Q11;
    /**< @h2xmle_description {dir_mf_sm2_L16Q11}
         @h2xmle_default     {5596}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dir_vad_thres_L16Q12;
    /**< @h2xmle_description {Directionality VAD threshold. Lower value allows less directional speech to be detected at the expense of false alarms.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dir_vad_hangover_max_L16;
    /**< @h2xmle_description {dir_vad_hangover_max_L16}
         @h2xmle_default     {10}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t init_dm_c_L16Q11;
    /**< @h2xmle_description {init_dm_c_L16Q11}
         @h2xmle_default     {1400}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t slope_dm_c2_mb_snr_L16Q13;
    /**< @h2xmle_description {slope_dm_c2_mb_snr_L16Q13}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mic_gain_tolerance_L16Q11;
    /**< @h2xmle_description {mic_gain_tolerance_L16Q11}
         @h2xmle_default     {500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dm_score_att_L16Q14;
    /**< @h2xmle_description {sm_dm_score_att_L16Q14}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dm_score_dec_L16Q14;
    /**< @h2xmle_description {sm_dm_score_dec_L16Q14}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dm2_score_att_L16Q14;
    /**< @h2xmle_description {sm_dm2_score_att_L16Q14}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dm2_score_dec_L16Q14;
    /**< @h2xmle_description {sm_dm2_score_dec_L16Q14}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm_c_comp_L16Q2;
    /**< @h2xmle_description {dm_c_comp_L16Q2}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm2_c_comp_L16Q2;
    /**< @h2xmle_description {dm2_c_comp_L16Q2}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t min_c_dm_L16Q11;
    /**< @h2xmle_description {min_c_dm_L16Q11}
         @h2xmle_default     {1500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_c_dm2_L16Q11;
    /**< @h2xmle_description {min_c_dm2_L16Q11}
         @h2xmle_default     {1500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm_norm1_init_alpha_L16Q15;
    /**< @h2xmle_description {dm_norm1_init_alpha_L16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm_norm1_u_L16;
    /**< @h2xmle_description {dm_norm1_u_L16}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t dm_norm1_v_L16;
    /**< @h2xmle_description {dm_norm1_v_L16}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}


         @h2xmle_policy       {Advanced} */

    int16_t dm_norm1_m_d_L16Q15;
    /**< @h2xmle_description {dm_norm1_m_d_L16Q15}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm_norm1_m_v_L16Q15;
    /**< @h2xmle_description {dm_norm1_m_v_L16Q15}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm_norm2_init_alpha_L16Q15;
    /**< @h2xmle_description {dm_norm2_init_alpha_L16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm_norm2_u_L16;
    /**< @h2xmle_description {dm_norm2_u_L16}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t dm_norm2_v_L16;
    /**< @h2xmle_description {dm_norm2_v_L16}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}


         @h2xmle_policy       {Advanced} */

    int16_t dm_norm2_m_d_L16Q15;
    /**< @h2xmle_description {dm_norm2_m_d_L16Q15}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm_norm2_m_v_L16Q15;
    /**< @h2xmle_description {dm_norm2_m_v_L16Q15}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm2_norm1_init_alpha_L16Q15;
    /**< @h2xmle_description {DM VAD2 score smoothing factor for minima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm2_norm1_u_L16;
    /**< @h2xmle_description {DM VAD2 minima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t dm2_norm1_v_L16;
    /**< @h2xmle_description {DM VAD2 minima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}


         @h2xmle_policy       {Advanced} */

    int16_t dm2_norm1_m_d_L16Q15;
    /**< @h2xmle_description {DM VAD2 minima tracking bias correction parameter}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm2_vad1_m_v_L16Q15;
    /**< @h2xmle_description {DM VAD2 minima tracking bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm2_norm2_init_alpha_L16Q15;
    /**< @h2xmle_description {DM VAD2 score smoothing factor for maxima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm2_norm2_u_L16;
    /**< @h2xmle_description {DM VAD2 maxima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t dm2_norm2_v_L16;
    /**< @h2xmle_description {DM VAD2 maxima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}


         @h2xmle_policy       {Advanced} */

    int16_t dm2_norm2_m_d_L16Q15;
    /**< @h2xmle_description {DM VAD2 maxima tracking bias correction parameter}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm2_norm2_m_v_L16Q15;
    /**< @h2xmle_description {DM VAD2 maxima tracking bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm_mf_sm2_L16Q11;
    /**< @h2xmle_description {DM VAD score multiplication factor during Onset/Offset VAD activity}
         @h2xmle_default     {3496}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm_vad_thres_L16Q12;
    /**< @h2xmle_description {DM VAD threshold}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm_vad2_thres_L16Q12;
    /**< @h2xmle_description {DM VAD2 threshold}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm_vad_hangover_max_L16;
    /**< @h2xmle_description {DM VAD hangover duration (in frames)}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dm_vad2_hangover_max_L16;
    /**< @h2xmle_description {DM VAD2 hangover duration (in frames)}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_mag_att_sm21_L16Q14;
    /**< @h2xmle_description {Attack time smoothing parameter for the bark scale magnitude used in Onset detection (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_mag_dec_sm21_L16Q14;
    /**< @h2xmle_description {Decay time smoothing parameter for the bark scale magnitude used in Onset detection (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_mag_att_sm22_L16Q14;
    /**< @h2xmle_description {Attack time smoothing parameter for the bark scale magnitude used in Offset detection (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_mag_dec_sm22_L16Q14;
    /**< @h2xmle_description {Decay time smoothing parameter for the bark scale magnitude used in Offset detection (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_diff_sm21_L16Q14;
    /**< @h2xmle_description {Smoothing parameter for the difference of average magnitudes used in Onset detection (Higher value means less smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm_diff_sm22_L16Q14;
    /**< @h2xmle_description {Smoothing parameter for the difference of average magnitudes used in Offset detection (Higher value means less smoothing)}
         @h2xmle_default     {1638}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_score_sm21_L16Q14;
    /**< @h2xmle_description {Smoothing parameter for updating the final Onset VAD score (Higher value means less smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_score_sm22_L16Q14;
    /**< @h2xmle_description {Smoothing parameter for updating the final Offset VAD score (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t band_sm21_thresh_L16Q12[19];
    /**< @h2xmle_description {Onset VAD threshold for bark scale band []}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t band_sm22_thresh_L16Q12[19];
    /**< @h2xmle_description {Offset VAD threshold for bark scale band []}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm21_thresh_L16Q11;
    /**< @h2xmle_description {Onset VAD threshold}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm22_thresh_L16Q11;
    /**< @h2xmle_description {Offset VAD threshold}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t hangover_max_sm21_L16;
    /**< @h2xmle_description {Onset VAD hangover duration (in frames)}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t hangover_max_sm22_L16;
    /**< @h2xmle_description {Offset VAD hangover duration (in frames)}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm21_norm1_init_alpha_L16Q15;
    /**< @h2xmle_description {Onset VAD score smoothing factor for minima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm21_norm1_u_L16;
    /**< @h2xmle_description {Onset VAD minima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t sm21_norm1_v_L16;
    /**< @h2xmle_description {Onset VAD minima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}


         @h2xmle_policy       {Advanced} */

    int16_t sm21_norm1_m_d_L16Q15;
    /**< @h2xmle_description {Onset VAD minima tracking bias correction parameter}
         @h2xmle_default     {26752}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm21_norm1_m_v_L16Q15;
    /**< @h2xmle_description {Onset VAD minima tracking bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm21_norm2_init_alpha_L16Q15;
    /**< @h2xmle_description {Onset VAD score smoothing factor for maxima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm21_norm2_u_L16;
    /**< @h2xmle_description {Onset VAD maxima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t sm21_norm2_v_L16;
    /**< @h2xmle_description {Onset VAD maxima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}


         @h2xmle_policy       {Advanced} */

    int16_t sm21_norm2_m_d_L16Q15;
    /**< @h2xmle_description {Onset VAD maxima tracking bias correction parameter}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm21_norm2_m_v_L16Q15;
    /**< @h2xmle_description {Onset VAD maxima tracking bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm22_norm1_init_alpha_L16Q15;
    /**< @h2xmle_description {Offset VAD score smoothing factor for minima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm22_norm1_u_L16;
    /**< @h2xmle_description {Offset VAD minima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t sm22_norm1_v_L16;
    /**< @h2xmle_description {Offset VAD minima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}


         @h2xmle_policy       {Advanced} */

    int16_t sm22_norm1_m_d_L16Q15;
    /**< @h2xmle_description {Offset VAD minima tracking bias correction parameter}
         @h2xmle_default     {26752}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm22_norm1_m_v_L16Q15;
    /**< @h2xmle_description {Offset VAD minima tracking bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm22_norm2_init_alpha_L16Q15;
    /**< @h2xmle_description {Offset VAD score smoothing factor for maxima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm22_norm2_u_L16;
    /**< @h2xmle_description {Offset VAD maxima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t sm22_norm2_v_L16;
    /**< @h2xmle_description {Offset VAD maxima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}


         @h2xmle_policy       {Advanced} */

    int16_t sm22_norm2_m_d_L16Q15;
    /**< @h2xmle_description {Offset VAD maxima tracking bias correction parameter}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm22_norm2_m_v_L16Q15;
    /**< @h2xmle_description {Offset VAD maxima tracking bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t hangover_max_dir_vad_plus_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t eps_ext_cos_look_dir_lb_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t eps_shr_cos_look_dir_lb_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved2;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t c2m_high_L32Q43;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t c0m_high_L32Q27;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {39321600}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t hangover_max_dm_vad_plus_L16[3];
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t bias_v1_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {91}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t bias_v0_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm_vad_plus_thres_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm_dm_plus_score_att_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dm_plus_score_dec_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c1_low_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-2638}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c0_low_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {2749}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c11_mid_L16Q20;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c10_mid_L16Q22;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {21723}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c01_mid_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-137}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c00_mid_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c21_high_L16Q28;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-11576}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c20_high_L16Q28;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c01_high_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-7330}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t c00_high_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {17474}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t knee_high_idx_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {91}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved3;
    /**< @h2xmle_description {client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t fp_nr_flags;
    /**< @h2xmle_description {NR Flags mode word for enabling different noise references in Fluence V5}
         @h2xmle_default     {-1073704438}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t debug_flags;
    /**< @h2xmle_description {Debug flags mode word for debugging noise references issues in Fluence V5}
         @h2xmle_default     {0}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t th_dir_v_upd_L16Q12;
    /**< @h2xmle_description {Dir VAD threshold for Spatial filter update}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t th_dm_v_upd_L16Q12;
    /**< @h2xmle_description {DM VAD threshold for Spatial filter update}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t th_sm_v_upd_L16Q8;
    /**< @h2xmle_description {Single-mic VAD threshold for Spatial filter update}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t two_path_flag;
    /**< @h2xmle_description {Enable/disable two path flag update for ABF adaptive filter coefficient download control}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mu1_L16Q18;
    /**< @h2xmle_description {ABF adaptive filter step size for low frequency range}
         @h2xmle_default     {1024}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mu2_L16Q18;
    /**< @h2xmle_description {ABF adaptive filter step size for mid frequency range}
         @h2xmle_default     {25000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mu3_L16Q18;
    /**< @h2xmle_description {ABF adaptive filter step size for High frequency range}
         @h2xmle_default     {21000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mu2_lb_idx_L16;
    /**< @h2xmle_description {Starting frequency index of mid frequency range for ABF update control (default: 1500 Hz)}
         @h2xmle_default     {4}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t mu3_lb_idx_L16;
    /**< @h2xmle_description {Starting frequency index of high frequency range for ABF update control (default: 3000 Hz}
         @h2xmle_default     {96}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t bf_start_idx_L16;
    /**< @h2xmle_description {Starting frequency index for speech reference generation in ABF}
         @h2xmle_default     {1}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t nbf_start_idx_L16;
    /**< @h2xmle_description {Starting frequency index for noise reference generation in ABF}
         @h2xmle_default     {1}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t mu_phase_cal_L16Q18;
    /**< @h2xmle_description {Step size for the first stage calibration phase filter in ABF processing}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t nbf_th_bf_L16Q15;
    /**< @h2xmle_description {Threshold for ABF foreground filter output-to-background filter output for updating the foreground null beamformer filter}
         @h2xmle_default     {25000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t nbf_th_oi_L16Q15;
    /**< @h2xmle_description {Threshold for ABF background filter output-to-Mic input for updating the foreground null beamformer filter}
         @h2xmle_default     {21000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t bf_th_bf_L16Q15;
    /**< @h2xmle_description {Threshold for ABF foreground filter output-to-background filter output for updating the foreground beamformer filter}
         @h2xmle_default     {25000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t bf_th_oi_L16Q15;
    /**< @h2xmle_description {Threshold for ABF background filter output-to-Mic input for updating the foreground beamformer filter}
         @h2xmle_default     {21000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_flags;
    /**< @h2xmle_description {Mode word for masking (QSF processing)}
         @h2xmle_default     {62}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_qsf_gain_att_L16Q14;
    /**< @h2xmle_description {Attack time smoothing parameter for the Masking gain (Higher value means less smoothing)}
         @h2xmle_default     {13107}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_qsf_gain_dec_L16Q14;
    /**< @h2xmle_description {Decay time smoothing parameter for the Masking gain (Higher value means less smoothing)}
         @h2xmle_default     {3277}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_qsf_n_gain_att_L16Q14;
    /**< @h2xmle_description {Attack time smoothing parameter for the Complimentary Masking gain (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_qsf_n_gain_dec_L16Q14;
    /**< @h2xmle_description {Decay time smoothing parameter for the Complimentary Masking gain (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t relax_hf_L16Q14;
    /**< @h2xmle_description {Relaxing factor for the masking gain in high frequencies [ lbQsfGainExtLbL16, doQsfLbIdxL16 ] (Higher value means less aggressive)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t relax_lf_L16Q14;
    /**< @h2xmle_description {Relaxing factor for the masking gain in low frequencies [doQsfUbIdxL16, UbQsfGainExtUbL16 ] (Higher value means less aggressive)}
         @h2xmle_default     {11469}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t rolloff_start_angle_r_L16;
    /**< @h2xmle_description {Starting Roll off angle (in degrees) for the right edge of the masking beam}
         @h2xmle_default     {65}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Advanced} */

    int16_t rolloff_end_angle_r_L16;
    /**< @h2xmle_description {Ending Roll off angle (in degrees) for the right edge of the masking beam}
         @h2xmle_default     {85}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Advanced} */

    int16_t rolloff_start_angle_l_L16;
    /**< @h2xmle_description {Starting Roll off angle (in degrees) for the left edge of the masking beam}
         @h2xmle_default     {0}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Advanced} */

    int16_t rolloff_end_angle_l_L16;
    /**< @h2xmle_description {Ending Roll off angle (in degrees) for the left edge of the masking beam}
         @h2xmle_default     {0}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Advanced} */

    int16_t n_rolloff_start_angle_r_L16;
    /**< @h2xmle_description {Starting Roll off angle (in degrees) for the right edge of the complimentary masking beam}
         @h2xmle_default     {180}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Advanced} */

    int16_t n_rolloff_end_angle_r_L16;
    /**< @h2xmle_description {Ending Roll off angle (in degrees) for the right edge of the complimentary masking beam}
         @h2xmle_default     {180}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Advanced} */

    int16_t n_rolloff_start_angle_l_L16;
    /**< @h2xmle_description {Starting Roll off angle (in degrees) for the left edge of the complimentary masking beam}
         @h2xmle_default     {120}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Advanced} */

    int16_t n_rolloff_end_angle_l_L16;
    /**< @h2xmle_description {Ending Roll off angle (in degrees) for the left edge of the complimentary masking beam}
         @h2xmle_default     {90}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Advanced} */

    int16_t min_qsf_agg_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness parameter for the Masking NS}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t slope_qsf_agg2_dir_c_L16Q12;
    /**< @h2xmle_description {Slope factor for the Masking NS (aggressiveness will be proportional to slope*phase contrast)}
         @h2xmle_default     {9830}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_qsf_n_agg_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness parameter for the Complimentary Masking NS}
         @h2xmle_default     {20480}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t slope_qsf_n_agg2_dir_c_L16Q12;
    /**< @h2xmle_description {Slope factor for the Complimentary Masking NS (aggressiveness will be proportional to slope*phase contrast)}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t avg_qsf_gain4_ext_range_L16[2];
    /**< @h2xmle_description {Lower bound of the frequency range for computing the average masking gain}
         @h2xmle_default     {10}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t lb_qsf_gain_ext_lb_L16;
    /**< @h2xmle_description {Extend the masking gain to the frequency range: [ lbQsfGainExtLbL16, doQsfLbIdxL16 ] if the mode bit, Mask_Ext_LB, is enabled}
         @h2xmle_default     {1}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t ub_qsf_gain_ext_ub_L16;
    /**< @h2xmle_description {Extend the masking gain to the frequency range: [ doQsfUbIdxL16, UbQsfGainExtUbL16 ] if the mode bit, Mask_Ext_UB, is enabled}
         @h2xmle_default     {128}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t n_gain4_missing_L16Q14;
    /**< @h2xmle_description {Default gain for the complimentary masking if the phase information from the mic signals is not reliable}
         @h2xmle_default     {655}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t scale_factor4_qsf_nref_L16Q13;
    /**< @h2xmle_description {Scaling factor for the complimentary masking noise reference}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved4;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t min_gain_lfns_L32Q28;
    /**< @h2xmle_description {Target noise reduction for the LFNS processing}
         @h2xmle_default     {67427938}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t min_dm2_c4_lfns_L16Q11;
    /**< @h2xmle_description {Minimum threshold for the DM VAD2 score contrast in order to do LFNS processing}
         @h2xmle_default     {1600}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t slope_dm2_c4_lfns_L16Q8;
    /**< @h2xmle_description {Slope factor for controlling the aggressiveness of LFNS processing}
         @h2xmle_default     {2621}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_cal_L16Q15;
    /**< @h2xmle_description {Smoothing parameter for the inter-mic calibration factor that is applied on the second mic signal (Higher value means more smoothing)}
         @h2xmle_default     {32604}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w0_noise_cal_L16Q14;
    /**< @h2xmle_description {Smoothing parameter across frequencies for averaging the mic energies before inter-mic calibration (Higher value means more smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w0_end_noise_cal_L16Q14;
    /**< @h2xmle_description {Smoothing parameter for the extreme frequencies (end frequencies) when averaging the mic energies (Higher value means more smoothing)}
         @h2xmle_default     {9830}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t beta_voice_cal_L16Q18;
    /**< @h2xmle_description {Step size for updating the voice calibration of the low frequency speech signal}
         @h2xmle_default     {5243}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t beta_init_voice_cal_L16Q18;
    /**< @h2xmle_description {Initial step size for updating the voice calibration of the low frequency speech signal}
         @h2xmle_default     {13107}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t init_n_frames_lfns_L16;
    /**< @h2xmle_description {Initial period (in number of frames) during which the step size for updating the voice calibration can be set to a higher value}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t start_ind_lfns_L16;
    /**< @h2xmle_description {Starting frequency index for the LFNS processing}
         @h2xmle_default     {1}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t end_ind_lfns_L16;
    /**< @h2xmle_description {Ending frequency index for the LFNS processing}
         @h2xmle_default     {16}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t start_ind_voice_cal_L16;
    /**< @h2xmle_description {Starting frequency index for the LFNS voice calibration}
         @h2xmle_default     {7}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t end_ind_voice_cal_L16;
    /**< @h2xmle_description {Ending frequency index for the LFNS voice calibration}
         @h2xmle_default     {22}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t voice_cal_n_bins_bit_shift_L16;
    /**< @h2xmle_description {Bit shift required to calculate block energy estimates for performing voice calibration}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t slope_voice_cal_L16Q17;
    /**< @h2xmle_description {Slope factor to boost the voice calibration in LFNS processing}
         @h2xmle_default     {8738}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t lfns_freq_smooth_flag;
    /**< @h2xmle_description {Enables smoothing of the LFNS gain across frequencies}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w0_lfns_gain_L16Q14;
    /**< @h2xmle_description {Smoothing parameter across frequencies for averaging the final LFNS gain (Higher value means more smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w0_end_lfns_gain_L16Q14;
    /**< @h2xmle_description {Smoothing parameter for the extreme frequencies (end frequencies) when averaging the final LFNS gain (Higher value means more smoothing)}
         @h2xmle_default     {10923}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t coeff_dm_c_L16Q12;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {6061}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_ref_min_stat_cfg_init_alpha_L16Q15;
    /**< @h2xmle_description {Minstat noise reference smoothing factor for primary mic noise floor tracking (Higher value means more smoothing)}
         @h2xmle_default     {27853}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_ref_min_stat_cfg_u_L16;
    /**< @h2xmle_description {Minstat noise reference noise floor tracking window length}
         @h2xmle_default     {12}
         @h2xmle_range       {1..12}


         @h2xmle_policy       {Advanced} */

    int16_t n_ref_min_stat_cfg_v_L16;
    /**< @h2xmle_description {Minstat noise reference noise floor tracking sub-window length}
         @h2xmle_default     {15}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t n_ref_min_stat_cfg_m_d_L16Q15;
    /**< @h2xmle_description {Minstat noise reference noise floor tracking bias correction parameter}
         @h2xmle_default     {26752}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_ref_min_stat_cfg_m_v_L16Q15;
    /**< @h2xmle_description {Minstat noise reference noise floor tracking bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t init_interval_vad_n_ref_L16;
    /**< @h2xmle_description {Initial interval (in frames) for using the Init smoothing parameters to update the VAD based noise references}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_init_dmic_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during initial speech inactivity portions in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_long_init_dmic_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during initial speech activity portions in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_dmic_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during speech inactivity portions in dual-mic mode (Higher value means mores smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_long_dmic_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during speech activity portions in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_init_smic_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during initial speech inactivity portions in single-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_long_init_smic_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during initial speech activity portions in single-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_smic_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during speech inactivity portions in single-mic mode (Higher value means mores smoothing)}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_long_smic_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during speech activity portions in single-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t init_val_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Parameter value for initializing the short term and long term noise estimates}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t over_est_fact_vad_n_ref_L16Q13;
    /**< @h2xmle_description {Over-estimation factor (scaling factor) for the VAD based noise reference}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_noise_act_speech_vad_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for switching from short term noise ref to long term noise ref during speech activity (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_sm_plus_nref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the IRM based noise reference (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thresh_sm_plus_L16Q15;
    /**< @h2xmle_description {SNR threshold for updating the IRM based noise reference}
         @h2xmle_default     {21299}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t over_est_fact_sm_plus_n_ref_L16Q13;
    /**< @h2xmle_description {Over-estimation factor (scaling factor) for the IRM noise reference}
         @h2xmle_default     {9830}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_ref_dir_thres1_L16Q12;
    /**< @h2xmle_description {Lower threshold for the dir VAD score to determine with certainty if the desired speech is absent}
         @h2xmle_default     {5000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_ref_dir_thres2_L16Q12;
    /**< @h2xmle_description {Upper threshold for the dir VAD score to determine with certainty if the desired speech is absent}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t spatial_n_ref_lb_idx_L16;
    /**< @h2xmle_description {Lower bound frequency index for the ABF noise reference to be used in the NS post processing}
         @h2xmle_default     {1}
         @h2xmle_range       {0..128}


         @h2xmle_policy       {Advanced} */

    int16_t spatial_n_ref_ub_idx_L16;
    /**< @h2xmle_description {Upper bound frequency index for the ABF noise reference to be used in the NS post processing}
         @h2xmle_default     {128}
         @h2xmle_range       {0..128}


         @h2xmle_policy       {Advanced} */

    int16_t w_spatial_n_ref_r0_L16Q14;
    /**< @h2xmle_description {Weighting factor for the ABF Noise ref when dir VAD score is less tham nRefDirThres1L16Q12}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w_spatial_n_ref_r2_L16Q14;
    /**< @h2xmle_description {Weighting factor for the ABF Noise ref when dir VAD score is greater than nRefDirThres2L16Q12}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w_min_stat_n_ref_r0_w_spatial_L16Q14;
    /**< @h2xmle_description {Weighting factor for the minStat Noise ref when dir VAD score is less than nRefDirThres1L16Q12 and ABF noise ref is also present}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w_min_stat_n_ref_r2_w_spatial_L16Q14;
    /**< @h2xmle_description {Weighting factor for the minStat Noise ref when dir VAD score is greater than nRefDirThres2L16Q12 and ABF noise ref is also present}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w_min_stat_n_ref_r0_d_m_L16Q14;
    /**< @h2xmle_description {Weighting factor for the minStat Noise ref when dir VAD score is less than nRefDirThres1L16Q12 and ABF noise ref is NOT present in the dual-mic mode}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w_min_stat_n_ref_r2_d_m_L16Q14;
    /**< @h2xmle_description {Weighting factor for the minStat Noise ref when dir VAD score is greater than nRefDirThres2L16Q12 and ABF noise ref is NOT present in the dual-mic mode}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w_min_stat_n_ref_s_m_L16Q14;
    /**< @h2xmle_description {Weighting factor for the minStat Noise ref in the single-mic mode}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t w_sm_plus_nref_s_m_L16Q14;
    /**< @h2xmle_description {Weighting factor for the IRM Noise ref in the single-mic mode}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_comb_n_ref_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the combined noise reference in order to generate the final noise reference (Higher value means more smoothing)}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_dev_init_int_L16;
    /**< @h2xmle_description {Initial period in frames during which the noise deviation is not computed and noise deviation noise ref is set to be equal to the long term noise reference}
         @h2xmle_default     {9}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_dev_sm_n_dev_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the noise deviation estimate during speech inactivity portions}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_dev_sm_n_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the long term noise estimate during speech inactivity portions}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_dev_sm_n_init_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the long term noise estimate during initial period of speech inactivity portions}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t over_est_fact_n_dev_n_ref_L16Q13;
    /**< @h2xmle_description {Over-estimation factor (scaling factor) for the noise deviation based noise reference}
         @h2xmle_default     {9200}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_dev_n_ref_decay_active_L16Q15;
    /**< @h2xmle_description {Decaying factor for updating the noise deviation estimate and long term noise estimate during speech activity portions}
         @h2xmle_default     {32700}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_r1_lb_idx_L16;
    /**< @h2xmle_description {Lower bound frequency index of the R1 band for SNR PP processing}
         @h2xmle_default     {32}
         @h2xmle_range       {0..256}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_r2_lb_idx_L16;
    /**< @h2xmle_description {Lower bound frequency index of the R2 band for SNR PP processing}
         @h2xmle_default     {65}
         @h2xmle_range       {0..256}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_r3_lb_idx_L16;
    /**< @h2xmle_description {Lower bound frequency index of the R3 band for SNR PP processing}
         @h2xmle_default     {128}
         @h2xmle_range       {0..256}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_min_agg_r0_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for the R0 frequency band}
         @h2xmle_default     {16000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t snr_pp_max_agg_r0_L16Q12;
    /**< @h2xmle_description {Maximum aggressiveness control for R0 frequency band}
         @h2xmle_default     {16000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t snr_pp_agg_slope_r0_L16Q10;
    /**< @h2xmle_description {Aggressiveness slope control for the R0 frequency band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_agg_offset_r0_L16Q11;
    /**< @h2xmle_description {Aggressiveness offset control for the R0 frequency band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_min_agg_r1_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for the R1 frequency band}
         @h2xmle_default     {13500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t snr_pp_max_agg_r1_L16Q12;
    /**< @h2xmle_description {Maximum aggressiveness control for the R1 frequency band}
         @h2xmle_default     {14000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t snr_pp_agg_slope_r1_L16Q11;
    /**< @h2xmle_description {Aggressiveness slope control for the R1 frequency band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_min_agg_r3_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for the R3 frequency band}
         @h2xmle_default     {12500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t snr_pp_agg_slope_r3_L16Q11;
    /**< @h2xmle_description {Aggressiveness slope control for the R3 frequency band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_int_decay_factor_L16Q15;
    /**< @h2xmle_description {Decay time smoothing parameter for the SNR-PP gain update}
         @h2xmle_default     {19661}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_int_delta_offset_L16Q14;
    /**< @h2xmle_description {Multiplication factor for the current gain estimate in SNR-PP}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_int_delta_mult_L16Q13;
    /**< @h2xmle_description {Delta offset for the current gain estimate in SNR-PP}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_min_gain_L16Q14;
    /**< @h2xmle_description {Target noise reduction (minimum gain) to be achieved with the SNR-PP module when IS is in dual-mic mode}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_smooth_L16Q15;
    /**< @h2xmle_description {Basicd ENS PP parameter - NOT used in FV5}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t offset_a_L16Q8;
    /**< @h2xmle_description {Basicd ENS PP parameter - NOT used in FV5}
         @h2xmle_default     {64}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t avg_log_snr_max_L16Q8;
    /**< @h2xmle_description {Basicd ENS PP parameter - NOT used in FV5}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t pp_agg_low_L16Q12;
    /**< @h2xmle_description {Basicd ENS PP parameter - NOT used in FV5}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t pp_agg_highto_low_ratio_L16Q12;
    /**< @h2xmle_description {Basicd ENS PP parameter - NOT used in FV5}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_pp_agg_low_L16Q12;
    /**< @h2xmle_description {Basicd ENS PP parameter - NOT used in FV5}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_slope_m1_low_L16Q12;
    /**< @h2xmle_description {Basicd ENS PP parameter - NOT used in FV5}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_slope_m1_high_L16Q12;
    /**< @h2xmle_description {Basicd ENS PP parameter - NOT used in FV5}
         @h2xmle_default     {-4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thres_low;
    /**< @h2xmle_description {LSB 16 bits of the noise floor threshold for updating AGM calibration}
         @h2xmle_default     {21474}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thres_high;
    /**< @h2xmle_description {MSB 16 bits of the noise floor threshold for updating AGM calibration}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t relax_lf_pp_L16Q14;
    /**< @h2xmle_description {Relaxing factor (averaging factor) for the SNR-pp gain in lower frequencies}
         @h2xmle_default     {13926}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_pp_gain_att_L16Q15;
    /**< @h2xmle_description {Attack time smoothing parameter for the SNR-PP gain (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_pp_gain_dec_L16Q15;
    /**< @h2xmle_description {Decay time smoothing parameter for the SNR-PP gain (Higher value means more smoothing)}
         @h2xmle_default     {10}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_comb_gain_att_L16Q15;
    /**< @h2xmle_description {Attack time smoothing parameter for the combined gain (Higher value means more smoothing)}
         @h2xmle_default     {3277}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_comb_gain_dec_L16Q15;
    /**< @h2xmle_description {Decay time smoothing parameter for the combined gain (Higher value means more smoothing)}
         @h2xmle_default     {3500}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_log_snr_ltm_snr_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_mag_ltm_snr_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_log_snr_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_band_log_snr_lb_idx_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {16}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t min_band_log_snr_ub_idx_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {64}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t reserved5;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t min_n_eng_ntm_L32Q31;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {214748}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sm_ntm_act_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_ntm_inact_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ntd_thres_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {4915}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_min_gain_vad_rns4_nt1_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {4915}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_min_gain_vad_rns4_nt2_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {4915}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_min_snr4_min_min_gain4_nt1_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_min_snr4_no_supp4_nt1_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_min_snr4_min_min_gain4_nt2_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-4915}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_min_snr4_no_supp4_nt2_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-6144}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved6;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t clean_ratio_r22_c0_L32Q22;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {1644167168}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t clean_ratio_r21_c0_L32Q22;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {34183578}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t clean_ratio_r20_c0_L32Q24;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {136734310}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t clean_ratio_r12_c0_L32Q21;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {277243494}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t clean_ratio_r11_c0_L32Q24;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {75234070}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t clean_ratio_r02_c10_L32Q28;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {100931731}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t clean_ratio_r02_c00_L32Q22;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {61530440}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t clean_ratio_r01_c10_L32Q31;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t clean_ratio_r01_c00_L32Q24;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {83047219}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t max_ratio_L32Q24;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {2147483647}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t min_ratio_L32Q24;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {16777216}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_th1_idx_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {83}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_th2_idx_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {112}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_dm_c_th1_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {2200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_dm_c_th2_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {2850}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_r22_c1_L16Q10;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_r21_c1_L16Q10;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_r12_c1_L16Q9;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_r11_c1_L16Q12;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_r02_c11_L16Q16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_r02_c01_L16Q10;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_r01_c11_L16Q19;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t clean_ratio_r01_c01_L16Q12;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ratio_comp_fact_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {14582}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_ratio_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_min_gain_dm_plus_rns_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {1638}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_snr4_min_min_gain4_nt1_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_snr4_no_supp4_nt1_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_snr4_no_supp4_nt1_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_snr4_no_supp4_nt2_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_min_snr4_no_supp4_nt1_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {5000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_min_snr4_no_supp4_nt2_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {5000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dm_c_act_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dm_c_inact_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_dm_c4_less_effect_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {1056}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t gainfunction_flag;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_min_gain_dir_rns_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {4915}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_snr4_min_min_gain4_dir_rns4_nt1_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_snr4_no_supp4_dir_rns4_nt1_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_snr4_no_supp4_dir_rns4_nt1_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t coeff_dir_c4_dir_rns_L16Q13;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {6383}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_plus_rns_gain_att_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_plus_rns_gain_dec_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t relax_lf_plus_rns_gain_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {11469}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved7;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true}*/

    int32_t vad_rns_target_gain_c0_dm_c_L32Q23;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {11744051}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t vad_rns_target_gain_c1_dm_c_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-6910}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_vad_rns_gain_inact_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_vad_rns_gain_act_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_final_gain_att_sm_L16Q15;
    /**< @h2xmle_description {Attack time smoothing of the final gain during single-mic mode or non-flat noise in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_final_gain_dec_sm_L16Q15;
    /**< @h2xmle_description {Decay time smoothing of the final gain during single-mic mode or non-flat noise in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thres_snr_rest_c1_L16Q18;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-16004}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thres_snr_rest_c0_L16Q9;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {5620}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_freq_flat_thres_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_freq_flat_thres_L16;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {128}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thres_dm_plus_rns_gain_L16Q14;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {16220}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t offset_pow_dual_wb_L32Q25;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-1724300000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t offset_pow_dual_nb_L32Q25;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-1724300000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t offset_pow_single_wb_L32Q25;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-1724300000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t offset_pow_single_nb_L32Q25;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-1724300000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t offset_dir_chpg_L32Q26;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {489894707}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t offset_dm_c_r0_hpg_L32Q22;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {31378847}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t offset_dm_c_r1_hpg_L32Q23;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {62757693}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t slope_dir_chpg_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-14175}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_dir_chpg_L16Q12;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {4847}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thres1_dm_chpg_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {1600}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t slope_dm_c_r0_hpg_L16Q10;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-7726}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t slope_dm_c_r1_hpg_L16Q11;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {-15452}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_dm_chpg_L16Q12;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {11495}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dir_hpg_single_L16Q12;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {4847}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dm_hpg_single_L16Q12;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dir_chpg_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {31130}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_dm_chpg_L16Q15;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {31130}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t min_log_eng4_dm_L32Q27;
    /**< @h2xmle_description {Thermal noise floor energy threshold for updating the DM VAD and DM VAD2}
         @h2xmle_default     {-350000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t dm_score4_sil_L16Q11;
    /**< @h2xmle_description {Default dual-mic VAD score to be used during silent portions (when the noise floor energy is under the thermal noise floor threshold)}
         @h2xmle_default     {-250}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved8;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t max_n4_is_L32Q0;
    /**< @h2xmle_description {Noise level threshold for the primary mic noise floor level above which IS will be forced to dual-mic mode}
         @h2xmle_default     {3500}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t max_is_hangover4_lfns_L16;
    /**< @h2xmle_description {IS hangover duration during which LFNS aggressiveness is reduced}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved9;
    /**< @h2xmle_description {reserved9}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t max_u_dirs4_inst_score_L16;
    /**< @h2xmle_description {Threshold on the number of frequencies with unreliable DOA estimate values for the Sector[0] phase computation}
         @h2xmle_default     {8}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved10;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t csd_tot_snr_thresh_L16Q5;
    /**< @h2xmle_description {Tuning this parameter is not recommended}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t reserved11;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true}*/

    int16_t is_evaluation_mode_L16;
    /**< @h2xmle_description {Mode word for IS configuration}
         @h2xmle_default     {89}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t is_eval_length_L16;
    /**< @h2xmle_description {Block window length for IS Mode Block Decision}
         @h2xmle_default     {75}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ims_thresh1_is_L16Q15;
    /**< @h2xmle_description {Lower threshold for Inter-mic subtraction level difference}
         @h2xmle_default     {16417}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ims_thresh2_is_L16Q15;
    /**< @h2xmle_description {Higher threshold for Inter-mic subtraction level difference}
         @h2xmle_default     {22000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_act_frames_is_L16;
    /**< @h2xmle_description {Minimum number of activity frames (single/dual-mic) to affect IS decision}
         @h2xmle_default     {7}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t margin_vot_smIs_L16;
    /**< @h2xmle_description {Minimum number of (single-mic frames - dual-mic frames) to force IS to single-mic mode}
         @h2xmle_default     {3}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_att_sds_L16Q15;
    /**< @h2xmle_description {Attack time smoothing parameter for the Spectral deviation score}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sds_boost_high_n_flat_L16Q13;
    /**< @h2xmle_description {Boost factor for Spectral deviation score during high level of Flat noise (stationary noise)}
         @h2xmle_default     {15000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sds_boost_high_n_non_flat_L16Q13;
    /**< @h2xmle_description {Boost factor for Spectral deviation score during high level of non-Flat noise (Non-stationary noise)}
         @h2xmle_default     {10923}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t target_noise_floor_L16Q15;
    /**< @h2xmle_description {NS CNI level for setting the target noise floor (in freq domain)}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t slope_noise_floor_L16Q15;
    /**< @h2xmle_description {Spectral slope (dB/Hz) of the injected comfort noise for NS}
         @h2xmle_default     {32416}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t lb_n_ref_constraint_L16;
    /**< @h2xmle_description {Lower bound frequency index above which noise ref can be forced to not exceed Mic1 input level}
         @h2xmle_default     {255}
         @h2xmle_range       {0..256}


         @h2xmle_policy       {Advanced} */

    int16_t sm_gain_peak_boost_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the Peak boosting gain across time}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t peak_boost_gain_L16Q8;
    /**< @h2xmle_description {Gain factor for boosting the speech peaks in order to minimize speech attenuation}
         @h2xmle_default     {512}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t peak_supp_gain_L16Q8;
    /**< @h2xmle_description {Gain factor for suppressing the peaks in order to avoid speech amplification}
         @h2xmle_default     {128}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ub_peak_supp_L16;
    /**< @h2xmle_description {Upper bound frequency below which speech peaks may be suppressed}
         @h2xmle_default     {6}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t lb_peak_boost_L16;
    /**< @h2xmle_description {Lower bound frequency above which speech peaks may be boosted}
         @h2xmle_default     {10}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t thres_boost_high_n_L16Q12;
    /**< @h2xmle_description {Noise level threshold for the noise ref magnitude to determine if output level should be boosted in high noise or not}
         @h2xmle_default     {1500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_stm_mag_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the noise reference magnitude used in long-term SNR estimation and high noise output boosting (Higher value means more smoothing)}
         @h2xmle_default     {26214}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_gain_att_boost_high_n_L16Q15;
    /**< @h2xmle_description {Attack time smoothing parameter for the high noise gain update (Higher value means more smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_gain_dec_boost_high_n_L16Q15;
    /**< @h2xmle_description {Release time smoothing parameter for the high noise gain (Higher value means more smoothing)}
         @h2xmle_default     {31130}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t gain_boost_high_n_L16Q12;
    /**< @h2xmle_description {Gain factor for boosting the output level during high noise}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t thres_hf_boost_high_n_L16Q12;
    /**< @h2xmle_description {Noise level threshold for the noise ref magnitude to determine if the high frequency output (and Spec deviation score) should be boosted in high noise or not}
         @h2xmle_default     {1500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t hangover_max_hf_boost_high_n_L16;
    /**< @h2xmle_description {Hangover period for boosting the high frequency output level during high noise}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t band_thres_hf_boost_high_n_L16[4];
    /**< @h2xmle_description {Lower bound Frequency index of the band over which output should be boosted during high noise situations}
         @h2xmle_default     {7}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t band_gain_hf_boost_high_n_L16Q12[5];
    /**< @h2xmle_description {Gain factor to boost in the band during high noise situations}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t alpha_L16Q15;
    /**< @h2xmle_description {Fixed scale factor for Mic1 to be used in HPS VAD computations}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t alpha2_L16Q12;
    /**< @h2xmle_description {Fixed scale factor for (Mic2 - alpha*Mic1) to be used in HPS VAD computations}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t alpha3_L16Q15;
    /**< @h2xmle_description {Fixed scale factor for Mic2 before computing the IMS level difference for IS update}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t lb_ratioII_L16;
    /**< @h2xmle_description {Lower bound of frequency index for computing the IMS level difference used in IS update}
         @h2xmle_default     {7}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ub_ratioII_L16;
    /**< @h2xmle_description {Upper bound of frequency index for computing the IMS level difference used in IS update}
         @h2xmle_default     {29}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ub_pit_range_L16;
    /**< @h2xmle_description {Upper bound of frequency index to search for the pitch frequency}
         @h2xmle_default     {20}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t pit_buffer_size_L16;
    /**< @h2xmle_description {Buffer size for holding the pitch buffer used in HPS VAD computation}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t pit_hist_init_L16;
    /**< @h2xmle_description {Pitch Frequency index to initialize the pitch buffer used in HPS VAD computation}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t pit_hist_threshold_L16;
    /**< @h2xmle_description {Threshold for pitch frequency histogram used in HPS VAD computation}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t harm_u_L16;
    /**< @h2xmle_description {Minstat block window size for harmonicity tracking and normalization}
         @h2xmle_default     {12}
         @h2xmle_range       {1..12}


         @h2xmle_policy       {Advanced} */

    int16_t harm_v_L16;
    /**< @h2xmle_description {Minstat block window counts for harmonicity tracking and normalization}
         @h2xmle_default     {15}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t harm_init_alpha_L16Q15;
    /**< @h2xmle_description {Smoothing parameter for harmonicity power estimates before performing minstat tracking}
         @h2xmle_default     {24576}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t norm_flag;
    /**< @h2xmle_description {Flag for enabling the normalization of Harmonicity product sum (HPS) used in HPS Vad}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t final_confidence_sm_L16;
    /**< @h2xmle_description {Smoothing factor for Harmonicty VAD final confidence statistic (Higher value means more smoothing)}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t hps_flags;
    /**< @h2xmle_description {Second NR Flags (Flag2) mode word for enabling new functionalities such as AGM, peak restoration, high noise boost etc. in Fluence V5}
         @h2xmle_default     {-12236}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t level_diff_L16;
    /**< @h2xmle_description {Threshold for the level diff VAD used in HPS computation}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ub_bin_L16;
    /**< @h2xmle_description {Upper bound of frequency index used in the level diff VAD computation}
         @h2xmle_default     {128}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t lb_bin_L16;
    /**< @h2xmle_description {Lower bound of frequency index used in the level diff VAD computation}
         @h2xmle_default     {0}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t harm_thresold_L16;
    /**< @h2xmle_description {Threshold for capturing pitch peaks - higher means less sensitive HPS VAD}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mask_gain_smoothing_L16;
    /**< @h2xmle_description {Smoothing factor for the Masking gain used in the HPS NS}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_level_diff_L16;
    /**< @h2xmle_description {Maximum bound for the level difference (between the two mics) above which the HPS NS gain is set to unity}
         @h2xmle_default     {-7000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_level_diff_L16;
    /**< @h2xmle_description {Minimum bound for the level difference (between the two mics) below which the HPS NS gain is set to the gain floor.}
         @h2xmle_default     {-10000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t hang_over_limit_L16;
    /**< @h2xmle_description {Hangover period for the HPS NS gain}
         @h2xmle_default     {150}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_dm_2_c_flat_4_lfns_L16Q11;
    /**< @h2xmle_description {Minimum DM VAD2 score contrast value to do inter-mic subtraction for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {1600}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t slope_dm_2_c_flat_4_lfns_L16Q8;
    /**< @h2xmle_description {Slope of decreasing inter-mic subtraction gain according to the low freq ProxScore contrast (Tuning NOT needed)}
         @h2xmle_default     {2621}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_min_agg_r0_flat_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for the R0 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {11000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_max_agg_r0_flat_L16Q12;
    /**< @h2xmle_description {Maximum aggressiveness control for the R0 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {18432}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_agg_slope_r0_flat_L16Q10;
    /**< @h2xmle_description {Aggressiveness slope control for the R0 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {16008}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_agg_offset_r0_flat_L16Q11;
    /**< @h2xmle_description {Aggressiveness offset control for the R0 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {-24793}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_min_agg_r1_flat_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for the R1 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_max_agg_r1_flat_L16Q12;
    /**< @h2xmle_description {Maximum aggressiveness control for the R1 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {18432}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_agg_slope_r1_flat_L16Q11;
    /**< @h2xmle_description {Aggressiveness slope control for the R1 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {7314}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_min_agg_r3_flat_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for the R3 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_agg_slope_r3_flat_L16Q11;
    /**< @h2xmle_description {Aggressiveness slope control for the R3 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {3500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_min_gain_flat_L16Q14;
    /**< @h2xmle_description {Target noise reduction (minimum gain) to be achieved with the SNR-PP module when IS is in dual-mic mode for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {1000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t snr_pp_agg_r0_sm_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for the R0 frequency band for IS in single-mic mode or single-mic Fluence V5}
         @h2xmle_default     {12500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_agg_r1_sm_L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for the R1 frequency band for IS in single-mic mode or single-mic Fluence V5}
         @h2xmle_default     {10500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_agg_r3_sm_L16Q12;
    /**< @h2xmle_description {Target noise reduction (minimum gain) to be achieved with the SNR-PP module when IS is in single-mic mode or for single-mic Fluence V5}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snr_pp_min_gain_sm_L16Q14;
    /**< @h2xmle_description {Target noise reduction (minimum gain) to be achieved with the SNR-PP module when IS is in single-mic mode or for single-mic Fluence V5}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int32_t pit_confidence_threshold_L32;
    /**< @h2xmle_description {Threshold for harmonicity based pitch estimation - Higher is less sensitive}
         @h2xmle_default     {1932735283}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t harm_min_L32Q24;
    /**< @h2xmle_description {Min bound for the harmonicity statistic to be used in the HPS VAD computation}
         @h2xmle_default     {-550000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t harm_max_L32Q24;
    /**< @h2xmle_description {Max bound for the harmonicity statistic to be used in the HPS VAD computation}
         @h2xmle_default     {-250000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t harm_vad_threshold_L32;
    /**< @h2xmle_description {Threshold for the harmonicity VAD}
         @h2xmle_default     {1424000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t normalization_min_L32;
    /**< @h2xmle_description {Minimum normalization factor to be used for normalizing the HPS in HPS VAD computation}
         @h2xmle_default     {40000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t suppression_gain_L32;
    /**< @h2xmle_description {Suppression gain for the HPS NS}
         @h2xmle_default     {536870912}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t gain_floor_L32;
    /**< @h2xmle_description {HPS NS gain is set to gain floor if the level difference is less than the lower bound}
         @h2xmle_default     {536870912}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t n_log_cal_over_est_l_fns_L16Q15;
    /**< @h2xmle_description {Log gain overestimate factor for the LFNS calibration}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t n_cal_lbIdx_L16;
    /**< @h2xmle_description {Lower bound of frequency index for calculating the IMS calibration}
         @h2xmle_default     {10}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t n_cal_ubIdx_L16;
    /**< @h2xmle_description {Upper bound of frequency index for calculating the IMS calibration}
         @h2xmle_default     {44}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t eng_thres_n_cal_L16;
    /**< @h2xmle_description {Upper bound energy threshold for the primary mic energy in order to update the noise calibration for the IMS ratio computation}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t init_int_n_cal_L16;
    /**< @h2xmle_description {Initial period in number of frames during which the smOverallNoiseCalInitL16Q15 smoothing parameter is used for IMS cal update}
         @h2xmle_default     {500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_band_noise_cal_L16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the instantaneous IMS calibration factors in different frequency bands (Higher value means mores smoothing)}
         @h2xmle_default     {32604}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_overall_noise_cal_init_L16Q15;
    /**< @h2xmle_description {Initial overall smoothing factor for updating the instantaneous IMS calibration factor}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm_overall_noise_cal_L16Q15;
    /**< @h2xmle_description {Overall smoothing factor for updating the instantaneous IMS calibration factor that is applied on the second mic energy}
         @h2xmle_default     {32604}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t over_est_factor_n_cal_hps_L16Q14;
    /**< @h2xmle_description {Over-estimation factor for the second mic calibration in the IMS ratio computation}
         @h2xmle_default     {17896}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ims_flags;
    /**< @h2xmle_description {Mode word for controlling IMS ratio processing}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t hps_nr_flags;
    /**< @h2xmle_description {Mode word for controlling different sub-modules inside the HPS NS processing}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t start_sm_L16;
    /**< @h2xmle_description {Lower bound index of the frequency range over which bark scale smoothing should be applied}
         @h2xmle_default     {23}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t agm_num_chan;
    /**< @h2xmle_description {agm_num_chan}
         @h2xmle_default     {2}
         @h2xmle_range       {1..4}


         @h2xmle_policy       {Basic} */

    int16_t agm_feature;
    /**< @h2xmle_description {agm_feature}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_data_width;
    /**< @h2xmle_description {agm_data_width}
         @h2xmle_default     {16}
         @h2xmle_range       {0..32}


         @h2xmle_policy       {Basic} */

    int16_t agm_feFlag_cfg;
    /**< @h2xmle_description {agm_feFlag_cfg}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_mode;
    /**< @h2xmle_description {agm_mode}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t reserved12;
    /**< @h2xmle_description {reserved12}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int32_t agm_power_thresh_4nflr_L32Q20;
    /**< @h2xmle_description {agm_power_thresh_4nflr_L32Q20}
         @h2xmle_default     {1024}
         @h2xmle_range       {50..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t agm_snr_low_thresh_L16Q10;
    /**< @h2xmle_description {agm_snr_low_thresh_L16Q10}
         @h2xmle_default     {1500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_snr_high_thresh_L16Q10;
    /**< @h2xmle_description {agm_snr_high_thresh_L16Q10}
         @h2xmle_default     {7000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_eig_ratio_thresh;
    /**< @h2xmle_description {agm_eig_ratio_thresh}
         @h2xmle_default     {1000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_gain_variation_indB_L16Q9;
    /**< @h2xmle_description {agm_gain_variation_indB_L16Q9}
         @h2xmle_default     {920}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_hist_flush_rate;
    /**< @h2xmle_description {agm_hist_flush_rate}
         @h2xmle_default     {-15536}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t agm_bin_cnt_dym_thresh_max;
    /**< @h2xmle_description {agm_bin_cnt_dym_thresh_max}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t agm_bin_cnt_dym_thresh_min;
    /**< @h2xmle_description {agm_bin_cnt_dym_thresh_min}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t agm_scVAD_blklen;
    /**< @h2xmle_description {agm_scVAD_blklen}
         @h2xmle_default     {20}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t agm_scVAD_numblk;
    /**< @h2xmle_description {agm_scVAD_numblk}
         @h2xmle_default     {20}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t agm_nfl_blklen;
    /**< @h2xmle_description {agm_nfl_blklen}
         @h2xmle_default     {10}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t agm_nfl_numblk;
    /**< @h2xmle_description {agm_nfl_numblk}
         @h2xmle_default     {10}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t marginVotSmIS2L16;
    /**< @h2xmle_description {marginVotSmIS2L16}
         @h2xmle_default     {25}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t proximityRatioL16;
    /**< @h2xmle_description {proximityRatioL16}
         @h2xmle_default     {13107}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved13;
    /**< @h2xmle_description {reserved13}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true}*/

    int32_t restorationRatioL32;
    /**< @h2xmle_description {restorationRatioL32}
         @h2xmle_default     {1288490188}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t dirThres2ISL16Q14;
    /**< @h2xmle_description {dirThres2ISL16Q14}
         @h2xmle_default     {2000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved14;
    /**< @h2xmle_description {reserved14}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int32_t pitch_confidence_flag_high_thresholdL32;
    /**< @h2xmle_description {pitch_confidence_flag_high_thresholdL32}
         @h2xmle_default     {805306368}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t pitch_confidence_flag_low_thresholdL32;
    /**< @h2xmle_description {pitch_confidence_flag_low_thresholdL32}
         @h2xmle_default     {322120000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t harmonicity_offsetL32Q24;
    /**< @h2xmle_description {harmonicity_offsetL32Q24}
         @h2xmle_default     {50331645}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t nonspeech_peak_count_thresholdL16;
    /**< @h2xmle_description {nonspeech_peak_count_thresholdL16}
         @h2xmle_default     {10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t min_peak_distanceL16;
    /**< @h2xmle_description {min_peak_distanceL16}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t peak_threshold_exp1L16;
    /**< @h2xmle_description {peak_threshold_exp1L16}
         @h2xmle_default     {-5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t peak_threshold_exp2L16;
    /**< @h2xmle_description {peak_threshold_exp2L16}
         @h2xmle_default     {-2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t nonspeech_peak_relative_gainL16;
    /**< @h2xmle_description {nonspeech_peak_relative_gainL16}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t scalebackL16Q15;
    /**< @h2xmle_description {scalebackL16Q15}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t HFC_thresholdL32;
    /**< @h2xmle_description {HFC_thresholdL32}
         @h2xmle_default     {20000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_low_thresholdL32Q13;
    /**< @h2xmle_description {SNR_low_thresholdL32Q13}
         @h2xmle_default     {37683}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_low_step_thresholdL32Q13;
    /**< @h2xmle_description {SNR_low_step_thresholdL32Q13}
         @h2xmle_default     {22118}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_high_thresholdL32Q13;
    /**< @h2xmle_description {SNR_high_thresholdL32Q13}
         @h2xmle_default     {81920}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_high_step_thresholdL32Q13;
    /**< @h2xmle_description {SNR_high_step_thresholdL32Q13}
         @h2xmle_default     {10240}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_onset_low_thresholdL32Q13;
    /**< @h2xmle_description {SNR_onset_low_thresholdL32Q13}
         @h2xmle_default     {113049}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_onset_low_step_thresholdL32Q13;
    /**< @h2xmle_description {SNR_onset_low_step_thresholdL32Q13}
         @h2xmle_default     {4146}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_onset_high_thresholdL32Q13;
    /**< @h2xmle_description {SNR_onset_high_thresholdL32Q13}
         @h2xmle_default     {245760}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_onset_high_step_thresholdL32Q13;
    /**< @h2xmle_description {SNR_onset_high_step_thresholdL32Q13}
         @h2xmle_default     {11520}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t harmonic_structure_thresholdL32Q30;
    /**< @h2xmle_description {harmonic_structure_thresholdL32Q30}
         @h2xmle_default     {536870912}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t restoration_ratio_thresholdL32;
    /**< @h2xmle_description {restoration_ratio_thresholdL32}
         @h2xmle_default     {858990000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t smrmt_mode;
    /**< @h2xmle_description {smrmt_mode}
         @h2xmle_default     {14}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smrmt_wlenMax;
    /**< @h2xmle_description {smrmt_wlenMax}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t smrmt_thrB1;
    /**< @h2xmle_description {smrmt_thrB1}
         @h2xmle_default     {210000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t smrmt_band1I;
    /**< @h2xmle_description {smrmt_band1I}
         @h2xmle_default     {24}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t reserved15;
    /**< @h2xmle_description {reserved15}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t smrmt_thrB2;
    /**< @h2xmle_description {smrmt_thrB2}
         @h2xmle_default     {200000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t smrmt_band2I;
    /**< @h2xmle_description {smrmt_band2I}
         @h2xmle_default     {64}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t reserved16;
    /**< @h2xmle_description {reserved16}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t smrmt_thrB3;
    /**< @h2xmle_description {smrmt_thrB3}
         @h2xmle_default     {150000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t smrmt_alphaP;
    /**< @h2xmle_description {smrmt_alphaP}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smrmt_alphaD;
    /**< @h2xmle_description {smrmt_alphaD}
         @h2xmle_default     {27852}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smrmt_overest_factL16Q12;
    /**< @h2xmle_description {smrmt_overest_factL16Q12}
         @h2xmle_default     {5300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smrmt_vad_thre_dm;
    /**< @h2xmle_description {smrmt_vad_thre_dm}
         @h2xmle_default     {1000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t smrmt_vad_thre_sm;
    /**< @h2xmle_description {smrmt_vad_thre_sm}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t abf_mode;
    /**< @h2xmle_description {abf_mode}
         @h2xmle_default     {193}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t overEstFactABFVNrefL16Q13;
    /**< @h2xmle_description {overEstFactABFVNrefL16Q13}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t thSmVUpdL16Q12;
    /**< @h2xmle_description {thSmVUpdL16Q12}
         @h2xmle_default     {1500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int32_t spf2P[512];
    /**< @h2xmle_description {];}
         @h2xmle_default     {26785312}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t spf2N[512];
    /**< @h2xmle_description {];}
         @h2xmle_default     {19780096}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t restNRefAttnL16;
    /**< @h2xmle_description {restNRefAttnL16}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t smTotalBoostLInd;
    /**< @h2xmle_description {smTotalBoostLInd}
         @h2xmle_default     {7}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t smTotalBoost;
    /**< @h2xmle_description {smTotalBoost}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smBoostLInd;
    /**< @h2xmle_description {smBoostLInd}
         @h2xmle_default     {64}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t smBoostLEnd;
    /**< @h2xmle_description {smBoostLEnd}
         @h2xmle_default     {128}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t smBoostRatioL16Q14;
    /**< @h2xmle_description {smBoostRatioL16Q14}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPMinGainLbL16Q14;
    /**< @h2xmle_description {snrPPMinGainLbL16Q14}
         @h2xmle_default     {128}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved17;
    /**< @h2xmle_description {reserved17}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int32_t sr_flag3;
    /**< @h2xmle_description {sr_flag3}
         @h2xmle_default     {384}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sr_pa_max_npk_L16Q0;
    /**< @h2xmle_description {sr_pa_max_npk_L16Q0}
         @h2xmle_default     {40}
         @h2xmle_range       {0..40}


         @h2xmle_policy       {Advanced} */

    int16_t sr_pa_rel_thr_L16Q15;
    /**< @h2xmle_description {sr_pa_rel_thr_L16Q15}
         @h2xmle_default     {184}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_pa_spacing_L16Q1;
    /**< @h2xmle_description {sr_pa_spacing_L16Q1}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_max_npk_L16Q0;
    /**< @h2xmle_description {sr_ha_max_npk_L16Q0}
         @h2xmle_default     {10}
         @h2xmle_range       {0..10}


         @h2xmle_policy       {Advanced} */

    int32_t sr_pa_lmin_L32Q15;
    /**< @h2xmle_description {sr_pa_lmin_L32Q15}
         @h2xmle_default     {1}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_f0_min_L16Q15;
    /**< @h2xmle_description {sr_ha_f0_min_L16Q15}
         @h2xmle_default     {532}
         @h2xmle_range       {512..4096}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_f0_max_L16Q15;
    /**< @h2xmle_description {sr_ha_f0_max_L16Q15}
         @h2xmle_default     {2458}
         @h2xmle_range       {512..4096}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_fh_min_L16Q15;
    /**< @h2xmle_description {sr_ha_fh_min_L16Q15}
         @h2xmle_default     {410}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_fh_max_L16Q15;
    /**< @h2xmle_description {sr_ha_fh_max_L16Q15}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_hmin_L16Q15;
    /**< @h2xmle_description {sr_ha_hmin_L16Q15}
         @h2xmle_default     {22938}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_hpw_minr_L16Q15;
    /**< @h2xmle_description {sr_ha_hpw_minr_L16Q15}
         @h2xmle_default     {1147}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_min_tonality_L16Q15;
    /**< @h2xmle_description {sr_ha_min_tonality_L16Q15}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_hp_bias_L16Q15;
    /**< @h2xmle_description {sr_ha_hp_bias_L16Q15}
         @h2xmle_default     {1311}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_cp_bias_L16Q15;
    /**< @h2xmle_description {sr_ha_cp_bias_L16Q15}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_hp_stable_L16Q15;
    /**< @h2xmle_description {sr_ha_hp_stable_L16Q15}
         @h2xmle_default     {27853}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t sr_ha_hidx_max_L32Q15;
    /**< @h2xmle_description {sr_ha_hidx_max_L32Q15}
         @h2xmle_default     {268698}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_apw_minr_L16Q15;
    /**< @h2xmle_description {sr_ha_apw_minr_L16Q15}
         @h2xmle_default     {262}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_fb_L16Q15;
    /**< @h2xmle_description {sr_ha_fb_L16Q15}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_bpw_minr_L16Q15;
    /**< @h2xmle_description {sr_ha_bpw_minr_L16Q15}
         @h2xmle_default     {1638}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_cm_dmax_L16Q15;
    /**< @h2xmle_description {sr_ha_cm_dmax_L16Q15}
         @h2xmle_default     {12288}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ha_cm_max_L16Q15;
    /**< @h2xmle_description {sr_ha_cm_max_L16Q15}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_ltl_recharge_L16Q15;
    /**< @h2xmle_description {sr_fs_ltl_recharge_L16Q15}
         @h2xmle_default     {4242}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t sr_fs_ltl_init_L32Q15;
    /**< @h2xmle_description {sr_fs_ltl_init_L32Q15}
         @h2xmle_default     {327680}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t sr_fs_ltl_min_L32Q15;
    /**< @h2xmle_description {sr_fs_ltl_min_L32Q15}
         @h2xmle_default     {32768}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_ltl_discharge_L16Q15;
    /**< @h2xmle_description {sr_fs_ltl_discharge_L16Q15}
         @h2xmle_default     {451}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_ust_minr_L16Q15;
    /**< @h2xmle_description {sr_fs_ust_minr_L16Q15}
         @h2xmle_default     {10486}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_ust_binstart_L16Q0;
    /**< @h2xmle_description {sr_fs_ust_binstart_L16Q0}
         @h2xmle_default     {96}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_ust_binmax_L16Q0;
    /**< @h2xmle_description {sr_fs_ust_binmax_L16Q0}
         @h2xmle_default     {128}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_vcount_max_L16Q0;
    /**< @h2xmle_description {sr_fs_vcount_max_L16Q0}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_vcount_inc_L16Q0;
    /**< @h2xmle_description {sr_fs_vcount_inc_L16Q0}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_vucount_max_L16Q0;
    /**< @h2xmle_description {sr_fs_vucount_max_L16Q0}
         @h2xmle_default     {12}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_vucount_vinc_L16Q0;
    /**< @h2xmle_description {sr_fs_vucount_vinc_L16Q0}
         @h2xmle_default     {12}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_fs_vucount_uinc_L16Q0;
    /**< @h2xmle_description {sr_fs_vucount_uinc_L16Q0}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_bin_max_L16Q0;
    /**< @h2xmle_description {sr_ss_bin_max_L16Q0}
         @h2xmle_default     {0}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_fv_L16Q15;
    /**< @h2xmle_description {sr_ss_fv_L16Q15}
         @h2xmle_default     {12288}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_v_hpk_rthr_L16Q15;
    /**< @h2xmle_description {sr_ss_v_hpk_rthr_L16Q15}
         @h2xmle_default     {328}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_v_floor_rthr_L16Q15;
    /**< @h2xmle_description {sr_ss_v_floor_rthr_L16Q15}
         @h2xmle_default     {130}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_suppress_pk_mingain_L16Q15;
    /**< @h2xmle_description {sr_ss_suppress_pk_mingain_L16Q15}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t sr_ss_suppress_pk_fgwidth_L32Q15;
    /**< @h2xmle_description {sr_ss_suppress_pk_fgwidth_L32Q15}
         @h2xmle_default     {251658}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t sr_ss_suppress_pk_bfwidth_L32Q15;
    /**< @h2xmle_description {sr_ss_suppress_pk_bfwidth_L32Q15}
         @h2xmle_default     {838861}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_vt_max_npk_L16Q0;
    /**< @h2xmle_description {sr_ss_vt_max_npk_L16Q0}
         @h2xmle_default     {4}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_uv_max_npk_L16Q0;
    /**< @h2xmle_description {sr_ss_uv_max_npk_L16Q0}
         @h2xmle_default     {6}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_uv_bin_max_L16Q0;
    /**< @h2xmle_description {sr_ss_uv_bin_max_L16Q0}
         @h2xmle_default     {48}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_sl_bin_max_L16Q0;
    /**< @h2xmle_description {sr_ss_sl_bin_max_L16Q0}
         @h2xmle_default     {0}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_sl_max_npk_L16Q0;
    /**< @h2xmle_description {sr_ss_sl_max_npk_L16Q0}
         @h2xmle_default     {20}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_sl_mingain_L16Q15;
    /**< @h2xmle_description {sr_ss_sl_mingain_L16Q15}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_sl_gainstep_L16Q15;
    /**< @h2xmle_description {sr_ss_sl_gainstep_L16Q15}
         @h2xmle_default     {26029}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_level_lambda_L16Q15;
    /**< @h2xmle_description {sr_ss_level_lambda_L16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t sr_ss_level_gainmax_L32Q15;
    /**< @h2xmle_description {sr_ss_level_gainmax_L32Q15}
         @h2xmle_default     {46286}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_fw_snr_L16Q15;
    /**< @h2xmle_description {sr_ss_fw_snr_L16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_v_mixed_scale_L16Q15;
    /**< @h2xmle_description {sr_ss_v_mixed_scale_L16Q15}
         @h2xmle_default     {1036}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_v_override_pthr_L16Q15;
    /**< @h2xmle_description {sr_ss_v_override_pthr_L16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_v_override_athr_L16Q15;
    /**< @h2xmle_description {sr_ss_v_override_athr_L16Q15}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t sr_ss_uv_snr_thr_L32Q15;
    /**< @h2xmle_description {sr_ss_uv_snr_thr_L32Q15}
         @h2xmle_default     {24576}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t sr_ss_uv_snr_scale_L32Q15;
    /**< @h2xmle_description {sr_ss_uv_snr_scale_L32Q15}
         @h2xmle_default     {524288}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_sl_bin_silent_L16Q0;
    /**< @h2xmle_description {sr_ss_sl_bin_silent_L16Q0}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_sl_bin_attenuate_L16Q0;
    /**< @h2xmle_description {sr_ss_sl_bin_attenuate_L16Q0}
         @h2xmle_default     {128}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_intraPhase_fh_max_L16Q15;
    /**< @h2xmle_description {sr_ss_intraPhase_fh_max_L16Q15}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_intraPhase_max_npk_L16Q0;
    /**< @h2xmle_description {sr_ss_intraPhase_max_npk_L16Q0}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t sr_ss_interPhase_dphi_D0_L32Q15;
    /**< @h2xmle_description {sr_ss_interPhase_dphi_D0_L32Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t sr_ss_interPhase_dphi_D1_L32Q15;
    /**< @h2xmle_description {sr_ss_interPhase_dphi_D1_L32Q15}
         @h2xmle_default     {-16384}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_interPhase_dphi_rthr_L16Q15;
    /**< @h2xmle_description {sr_ss_interPhase_dphi_rthr_L16Q15}
         @h2xmle_default     {29639}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_interPhase_phw_bw_L16Q15;
    /**< @h2xmle_description {sr_ss_interPhase_phw_bw_L16Q15}
         @h2xmle_default     {1638}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_envModel_fe_L16Q15;
    /**< @h2xmle_description {sr_ss_envModel_fe_L16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_envModel_spd_width_L16Q15;
    /**< @h2xmle_description {sr_ss_envModel_spd_width_L16Q15}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_envModel_alpha_L16Q15;
    /**< @h2xmle_description {sr_ss_envModel_alpha_L16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_envModel_m_L16Q0;
    /**< @h2xmle_description {sr_ss_envModel_m_L16Q0}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_envModel_niter_L16Q0;
    /**< @h2xmle_description {sr_ss_envModel_niter_L16Q0}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_formant_fw_L16Q0;
    /**< @h2xmle_description {sr_ss_formant_fw_L16Q0}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t sr_ss_envModel_max_scale_L32Q15;
    /**< @h2xmle_description {sr_ss_envModel_max_scale_L32Q15}
         @h2xmle_default     {327680}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_formant_max_nm_L16Q0;
    /**< @h2xmle_description {sr_ss_formant_max_nm_L16Q0}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_ss_formant_rthr_L16Q15;
    /**< @h2xmle_description {sr_ss_formant_rthr_L16Q15}
         @h2xmle_default     {1036}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_am_bin_min_L16Q0;
    /**< @h2xmle_description {sr_am_bin_min_L16Q0}
         @h2xmle_default     {48}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sr_am_gain_higher_band_codebook_linear_L16Q11;
    /**< @h2xmle_description {sr_am_gain_higher_band_codebook_linear_L16Q11}
         @h2xmle_default     {2893}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t sr_am_max_gain_linear_L32Q15;
    /**< @h2xmle_description {sr_am_max_gain_linear_L32Q15}
         @h2xmle_default     {146368}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t sr_am_min_gain_linear_L32Q15;
    /**< @h2xmle_description {sr_am_min_gain_linear_L32Q15}
         @h2xmle_default     {32768}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t sr_am_thresh_vad_low_band_dist_log_L32Q15;
    /**< @h2xmle_description {sr_am_thresh_vad_low_band_dist_log_L32Q15}
         @h2xmle_default     {29491}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t sr_am_thresh_vad_high_band_dist_log_L32Q15;
    /**< @h2xmle_description {sr_am_thresh_vad_high_band_dist_log_L32Q15}
         @h2xmle_default     {0}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t sr_am_thresh_vad_high_band_dist_delta_log_L32Q15;
    /**< @h2xmle_description {sr_am_thresh_vad_high_band_dist_delta_log_L32Q15}
         @h2xmle_default     {32768}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t bf_epsL32Q30;
    /**< @h2xmle_description {bf_epsL32Q30}
         @h2xmle_default     {107}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t bf_init_framesL16;
    /**< @h2xmle_description {bf_init_framesL16}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t bf_init_shiftL16;
    /**< @h2xmle_description {bf_init_shiftL16}
         @h2xmle_default     {-2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sfVAD_sthresholdL16;
    /**< @h2xmle_description {threshold for spatial VAD tracking movement}
         @h2xmle_default     {20000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sfVAD_anthresholdL16;
    /**< @h2xmle_description {threshold for ABF noise path adaptation}
         @h2xmle_default     {25000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sfVAD_avthresholdL16;
    /**< @h2xmle_description {threshold for ABF voice path adaptation}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */
		 
    int16_t sfVAD_numStrvL16;
    /**< @h2xmle_description {Number of filter weights for spatial VAD}
         @h2xmle_default     {7}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sfVAD_filterCoeffL16[896];
    /**< @h2xmle_description {Spatial VAD filter weights}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */
		 
		 
    int16_t sf_fb_oi_distVL16Q15;
    /**< @h2xmle_description {abf reinforncement learning output vs input distance measure for speech classifier}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */
		 
    int16_t sf_fb_oi_distNL16Q15;
    /**< @h2xmle_description {abf reinforncement learning output vs input distance measure for non-speech classifier}
         @h2xmle_default     {2047}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */
		 
    int16_t fremix_nfloor_det_thresholdL16Q15;
    /**< @h2xmle_description {remix stage CNI noise floor detector threshold}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t fremix_nfloor_det_hangoverL16Q0;
    /**< @h2xmle_description {remix stage CNI noise floor detector hangover avoid capturing short pauses/unvoiced partials}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t fremix_nfloor_strengthL16Q15;
    /**< @h2xmle_description {remix stage CNI/speech strength (linear gain format)}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t fremix_tonal_thresholdL16Q15;
    /**< @h2xmle_description {remix stage dominant tonal ratio}
         @h2xmle_default     {32}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t micocn_dist_ratioL16Q8;
    /**< @h2xmle_description {mic occlusion detector separation distance ratio}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved18;
    /**< @h2xmle_description {reserved18}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */
		 
}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FLUENCE_SPF_COEFF 0x00010E2D

#include "adsp_begin_pack.h"

typedef struct voice_param_fv5_spf_coeff_t voice_param_fv5_spf_coeff_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_SPF_COEFF", VOICE_PARAM_FLUENCE_SPF_COEFF}
    @h2xmlp_description {Provides Fluence version 5 spatial filter coefficients (internal status monitoring of single-mic and dual-mic
ECNS).\n
Maximum array size for this read-only parameter \n
Version 0 - 4100 bytes \n}
@h2xmlp_version     {0}
@h2xmlp_readOnly    {true}
@h2xmlp_toolPolicy  {RTM} */

struct voice_param_fv5_spf_coeff_t {

    uint16_t Version;
    /**< @h2xmle_description {Read only.}
         @h2xmle_isVersion   {true}     */

    uint16_t Reserved;
    /**< @h2xmle_description {Read only.} */

    int32_t spf2P[512];
    /**< @h2xmle_description {Read only.} */

    int32_t spf2N[512];
    /**< @h2xmle_description {Read only.} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FLUENCE_NS_STATE 0x00010E31

#include "adsp_begin_pack.h"

typedef struct voice_param_fv5_state_t voice_param_fv5_state_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_NS_STATE", VOICE_PARAM_FLUENCE_NS_STATE}
    @h2xmlp_description {Provides internal status monitoring of Fluence version 5 single-mic and dual-mic ECNS.\n
Maximum array size for this read-only parameter\n
Version 0  116 bytes\n
Version 1 extension  180 bytes \n
Version 2 extension  228 bytes \n
Version 3 extension  284 bytes. \n}
     @h2xmlp_version     {3}
    @h2xmlp_toolPolicy  {RTM}
    @h2xmlp_readOnly    {true} */

struct voice_param_fv5_state_t {

    uint16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {3}
         @h2xmle_readOnly    {true}
         @h2xmle_isVersion    {true}  */

    uint16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.} */

    uint32_t lib_version;
    /**< @h2xmle_description {lib_version}
          @h2xmle_default      {0x70010200} */

    int32_t inst_energy_L32;
    /**< @h2xmle_description {inst_energy_L32} */

    int32_t snr_L32Q8;
    /**< @h2xmle_description {snr_L32Q8} */

    int16_t sm_vad;
    /**< @h2xmle_description {sm_vad} */

    int16_t dir_vad;
    /**< @h2xmle_description {dir_vad} */

    int16_t sm_vad_21;
    /**< @h2xmle_description {sm_vad_21} */

    int16_t sm_vad_22;
    /**< @h2xmle_description {sm_vad_22} */

    int16_t sds_L16Q7;
    /**< @h2xmle_description {sds_L16Q7} */

    int16_t ltm_dir_vad_score_L16Q14[4];
    /**< @h2xmle_description {ltm_dir_vad_score_L16Q14[4]} */

    int16_t is_hangover;
    /**< @h2xmle_description {is_hangover} */

    int16_t dm_vad_score_L16Q11;
    /**< @h2xmle_description {dm_vad_score_L16Q11} */

    int16_t dir_vad_score_L16Q14[4];
    /**< @h2xmle_description {dir_vad_score_L16Q14[4]} */

    int16_t dir_vad_score_normf_L16Q12;
    /**< @h2xmle_description {dir_vad_score_normf_L16Q12} */

    int16_t dir_vad_c_L16Q14;
    /**< @h2xmle_description {dir_vad_c_L16Q14} */

    int16_t dm_vad_score_normf_L16Q12;
    /**< @h2xmle_description {dm_vad_score_normf_L16Q12} */

    int16_t dm_vad_score_c_L16Q11;
    /**< @h2xmle_description {dm_vad_score_c_L16Q11} */

    int16_t min_dm_vad_score_L16Q11;
    /**< @h2xmle_description {min_dm_vad_score_L16Q11} */

    int16_t dm_vad2_score_L16Q11;
    /**< @h2xmle_description {dm_vad2_score_L16Q11} */

    int16_t dm_vad2_score_norm_L16Q12;
    /**< @h2xmle_description {dm_vad2_score_norm_L16Q12} */

    int16_t sm_vad21_score_L16Q11;
    /**< @h2xmle_description {sm_vad21_score_L16Q11} */

    int16_t bandsm_vad21_score_L16Q12[19];
    /**< @h2xmle_description {bandsm_vad21_score_L16Q12[19]} */

    int16_t sm_vad22_score_L16Q11;
    /**< @h2xmle_description {sm_vad22_score_L16Q11} */

    int16_t g_vad;
    /**< @h2xmle_description {g_vad} */

    int16_t dm_vad;
    /**< @h2xmle_description {dm_vad} */

    int16_t dm_vad2;
    /**< @h2xmle_description {dm_vad2} */

    int16_t nr_state;
    /**< @h2xmle_description {nr_state} */

    int16_t is_state;
    /**< @h2xmle_description {is_state} */

    int16_t nts_L16Q15;
    /**< @h2xmle_description {nts_L16Q15} */

    int16_t reserved1;
    /**< @h2xmle_description {reserved1} */

    int32_t is_rest_ratio;
    /**< @h2xmle_description {is_rest_ratio} */

    int32_t is_mag_sum_in_L32;
    /**< @h2xmle_description {is_mag_sum_in_L32} */

    int32_t is_mag_sum_diff_L32;
    /**< @h2xmle_description {is_mag_sum_diff_L32} */

    int32_t smrmt_ratio_log_L32[3];
    /**< @h2xmle_description {smrmt_ratio_log_L32[3]} */

    int32_t rest_norm_harmonicity_L32Q30;
    /**< @h2xmle_description {rest_norm_harmonicity_L32Q30} */

    int32_t hps_ns_level_diff_L32;
    /**< @h2xmle_description {hps_ns_level_diff_L32} */

    int32_t hps_harmonicity_L32Q24;
    /**< @h2xmle_description {hps_harmonicity_L32Q24} */

    int32_t hps_norm_harmonicity_L32Q30;
    /**< @h2xmle_description {hps_norm_harmonicity_L32Q30} */

    int32_t hps_final_confidence_L32Q30;
    /**< @h2xmle_description {hps_final_confidence_L32Q30} */

    int16_t highnoise_stm_n_mag_sum_L16;
    /**< @h2xmle_description {highnoise_stm_n_mag_sum_L16} */

    int16_t highnoise_hf_stm_n_mag_sum_L16;
    /**< @h2xmle_description {highnoise_hf_stm_n_mag_sum_L16} */

    int16_t hps_harmonicity_vad;
    /**< @h2xmle_description {hps_harmonicity_vad} */

    int16_t sf_download_flag;
    /**< @h2xmle_description {sf_download_flag} */

    int16_t agmgain;
    /**< @h2xmle_description {agmgain} */

    int16_t td_is_en_diff;
    /**< @h2xmle_description {td_is_en_diff} */

    int16_t is_soft_state;
    /**< @h2xmle_description {is_soft_state} */

    int16_t high_n_flag;
    /**< @h2xmle_description {high_n_flag} */

    int16_t is_rest_ratio_count;
    /**< @h2xmle_description {is_rest_ratio_count} */

    uint8_t v_upd_flag;
    /**< @h2xmle_description {v_upd_flag} */

    uint8_t v_upd_sc_flag;
    /**< @h2xmle_description {v_upd_sc_flag} */

    int32_t sr_stlL32Q15;
    /**< @h2xmle_description {sr_stlL32Q15} */

    int32_t sr_df0_rateL32Q15;
    /**< @h2xmle_description {sr_df0_rateL32Q15} */

    int32_t sr_ltlL32Q15;
    /**< @h2xmle_description {sr_ltlL32Q15} */

    int32_t sr_v_strengthL32Q15;
    /**< @h2xmle_description {sr_v_strengthL32Q15} */

    int32_t sr_u_strengthL32Q15;
    /**< @h2xmle_description {sr_u_strengthL32Q15} */

    int32_t sr_frameType;
    /**< @h2xmle_description {sr_frameType} */

    int16_t sr_f0L16Q15;
    /**< @h2xmle_description {sr_f0L16Q15} */

    int16_t sr_NpL16Q0;
    /**< @h2xmle_description {sr_NpL16Q0} */

    int16_t sr_hpL16Q0;
    /**< @h2xmle_description {sr_hpL16Q0} */

    int16_t sr_cpL16Q0;
    /**< @h2xmle_description {sr_cpL16Q0} */

    int16_t sr_NhpL16Q0;
    /**< @h2xmle_description {sr_NhpL16Q0} */

    int16_t sr_v_stateL16Q0;
    /**< @h2xmle_description {sr_v_stateL16Q0} */

    int16_t sr_vu_stateL16Q0;
    /**< @h2xmle_description {sr_vu_stateL16Q0} */

    int16_t sr_vcountL16Q0;
    /**< @h2xmle_description {sr_vcountL16Q0} */

    int16_t sr_vucountL16Q0;
    /**< @h2xmle_description {sr_vucountL16Q0} */

    int8_t sr_NK;
    /**< @h2xmle_description {sr_NK} */

    int8_t sr_Np;
    /**< @h2xmle_description {sr_Np} */

    int8_t sr_f0_anchor;
    /**< @h2xmle_description {sr_f0_anchor} */

    int8_t sr_pat_anchor;
    /**< @h2xmle_description {sr_pat_anchor} */

    int8_t sr_f0_stable;
    /**< @h2xmle_description {sr_f0_stable} */

    int8_t sr_pat_stable;
    /**< @h2xmle_description {sr_pat_stable} */

    int16_t sr_num_fmnt_L16Q0;
    /**< @h2xmle_description {sr_num_fmnt_L16Q0} */

    int16_t sr_num_coeffs_L16Q0;
    /**< @h2xmle_description {sr_num_coeffs_L16Q0} */

    int16_t sr_q_coeffs_L16Q0;
    /**< @h2xmle_description {sr_q_coeffs_L16Q0} */

    int16_t reserved2;
    /**< @h2xmle_description {reserved2} */

    int32_t sr_a_L32Q0[7];
    /**< @h2xmle_description {sr_a_L32Q0[7]} */

    int32_t sr_dis_low_L32Q15;
    /**< @h2xmle_description {sr_dis_low_L32Q15} */

    int32_t sr_dis_high_L32Q15;
    /**< @h2xmle_description {sr_dis_high_L32Q15} */

    int32_t sr_dis_high_delta_L32Q15;
    /**< @h2xmle_description {sr_dis_high_delta_L32Q15} */

    int32_t sr_ratio_lt_L32Q15;
    /**< @h2xmle_description {sr_ratio_lt_L32Q15} */

    int32_t sr_snr;
    /**< @h2xmle_description {sr_snr} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FLUENCE_EC 0x00010E1C

#include "adsp_begin_pack.h"

typedef struct voice_param_fpecns_t voice_param_fpecns_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_EC", VOICE_PARAM_FLUENCE_EC}

    @h2xmlp_description  {Fluence Pro (quad-mic) and Fluence version 5 (single-mic and dual-mic) ECNS.\n
Maximum array size for this parameter \n
Version 0  904 bytes
Version 1 extension  908 bytes\n
Version 2 extension  920 bytes\n
Version 3 extension  932 bytes\n
Version 4 extension  976 bytes\n
Version 5 extension  984 bytes\n
Version 6 extension  992 bytes\n
Version 7 extension  1000 bytes\n \n}

@h2xmlp_version     {7}  */

struct voice_param_fpecns_t {

    int16_t version;
    /**< @h2xmle_description {READ ONLY parameter to check the FPECNS version}
         @h2xmle_default     {7}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion   {true}


         @h2xmle_readOnly       {true} */

    int16_t reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t mode;
    /**< @h2xmle_description {Mode word for enabling/disabling sub-modules in Fluence V5}
         @h2xmle_default     {25166637}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "25166636"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "25166637"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "25166638"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "58721068"}

         @h2xmle_policy       {Basic} */

    int16_t prim_in_chan_L16;
    /**< @h2xmle_description {Channel Map for the primary mic path (hardcoded)}
         @h2xmle_default     {0}
         @h2xmle_range       {0..0}


         @h2xmle_policy       {Advanced} */

    int16_t mic_ch_map_L16[4];
    /**< @h2xmle_description {Channel Map for Mic 1 path to Fluence V5 (hardcoded)}
         @h2xmle_default     {0}
         @h2xmle_range       {0..3}


         @h2xmle_policy       {Advanced} */

    int16_t input_gain_L16q13[4];
    /**< @h2xmle_description {Input Gain for Mic input}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_bp_sections;
    /**< @h2xmle_description {Number of BPF sections used for filtering the input signals before Wind Noise Detection (WND)}
         @h2xmle_default     {2}
         @h2xmle_range       {0..2}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_bp_numq_L16[4];
    /**< @h2xmle_description {Numerator coeffcient's integer part of Q factor (WND BPF)}
         @h2xmle_default     {2}
         @h2xmle_range       {-8..8}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_bp_denq_L16[4];
    /**< @h2xmle_description {Denominator coeffcient's integer part of Q factor (WND BPF)}
         @h2xmle_default     {2}
         @h2xmle_range       {-8..8}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_calph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the cross-covariance estimation in WND}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_cov_bit_shf_L16;
    /**< @h2xmle_description {Right shift (bit-shift) for the cross-covariance estimate in WND}
         @h2xmle_default     {6}
         @h2xmle_range       {-31..31}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_ws_alph_inc_L16Q15;
    /**< @h2xmle_description {Attack time smoothing factor for the WND score (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_ws_alph_dec_L16Q15;
    /**< @h2xmle_description {Decay time smoothing factor for the WND score (Higher value means less smoothing)}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_pri_wind_boost_L16Q10;
    /**< @h2xmle_description {Boost factor for self RMS value of the primary mic wind indicator}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_pri_wind_fact_L16Q15;
    /**< @h2xmle_description {Normalization factor for computing the primary mic wind indicator}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int32_t wnd_bp_numcoeff_L32[12];
    /**< @h2xmle_description {Numerator coeffcient for WND BPF}
         @h2xmle_default     {2975724}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t wnd_bp_dencoeff_L32[8];
    /**< @h2xmle_description {Denominator coefficient for WND BPF}
         @h2xmle_default     {-954895665}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t wnd_det_thresh_L32Q31[2];
    /**< @h2xmle_description {Threshold for wind noise detection}
         @h2xmle_default     {644218880}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t wnd_cov_eps_L32Q31;
    /**< @h2xmle_description {Epsilon factor to add to the cross-covariance estimate for stabilizing the value}
         @h2xmle_default     {21}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved1;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t wnr_dfc_max_L16;
    /**< @h2xmle_description {Step size increment in WNR HPF cut-off frequency}
         @h2xmle_default     {20}
         @h2xmle_range       {10..200}


         @h2xmle_policy       {Advanced} */

    int32_t wnr_agg_c_L32;
    /**< @h2xmle_description {Maximum cut-off frequency of the HPF used in WNR}
         @h2xmle_default     {800}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t reserved2;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t aec_clip_flag_lb_frac_L16Q15;
    /**< @h2xmle_description {Threshold for the low band NLMS output error to mic ratio for applying extra clipping}
         @h2xmle_default     {656}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_clip_flag_hb_frac_L16Q15;
    /**< @h2xmle_description {Threshold for the high band NLMS output error to mic ratio for applying extra clipping}
         @h2xmle_default     {1092}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pathchange_flag_frac_L16Q15;
    /**< @h2xmle_description {Foreground to background NLMS output error ratio threshold for path change detection}
         @h2xmle_default     {27305}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pathchange_wgt_enfact_L16Q15;
    /**< @h2xmle_description {NLMS weight energy factor for path change detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pathchange_wgt_fact_L16Q15;
    /**< @h2xmle_description {NLMS weight change factor for path change detection}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pathchange_wgt_alph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the background filter energy (higher value means less smoothing)}
         @h2xmle_default     {26216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pathchange_wgt_enfact_d_L16Q15;
    /**< @h2xmle_description {NLMS weight energy factor for coeff download}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pathchange_wgt_trig_d_L16;
    /**< @h2xmle_description {Number of frames with weight increase for triggering AF coeff download during path change detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_sflen_L16;
    /**< @h2xmle_description {LEC1 - Adaptive filter length for the Mic I LEC filter}
         @h2xmle_default     {100}
         @h2xmle_range       {4..400}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg0_wgtq_L16;
    /**< @h2xmle_description {LEC1 - Q-factor for the AF weights for the Mic1 LEC filter}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_r_update;
    /**< @h2xmle_description {LEC1 -Force update foreground AF at every fgUpdate th sample if this parameter is non zero}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_r_lrate;
    /**< @h2xmle_description {LEC1 -Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to mic ratio to trigger weight download}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg0_saec_full_learn_frames_L16;
    /**< @h2xmle_description {LEC1 -Initial number of frames to do force download of AF coefficients}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_threshold_lec__L16Q15;
    /**< @h2xmle_description {LEC1 -Double talk detection threshold for Linear EC adaptation}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg0_dtd_threshold_postp__L16Q15;
    /**< @h2xmle_description {LEC1 -Double talk detection threshold for EC PF processing}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_l_rate_L16;
    /**< @h2xmle_description {LEC1 -Left bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to foreground AF error to trigger weight download}
         @h2xmle_default     {32000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to foreground AF error to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to mic ratio to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC1 -Number of divergence download frames to trigger divergence download}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_hangover_max_L16;
    /**< @h2xmle_description {LEC1 -Hangover duration for the double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC1 -Correction scale factor to account for background noise during EC adaptation}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to mic ratio for double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC1 -Initial number of frames in double talk detection to allow weight adaptation}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_sflen_L16;
    /**< @h2xmle_description {LEC2 - Adaptive filter length for the Mic 2 LEC filter}
         @h2xmle_default     {100}
         @h2xmle_range       {4..400}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_wgtq_L16;
    /**< @h2xmle_description {LEC2 - Q-factor for the AF weights for the Mic2 LEC filter}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_r_update;
    /**< @h2xmle_description {LEC2 - Force update foreground AF at every fgUpdate th sample if this parameter is non zero}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_r_lrate;
    /**< @h2xmle_description {LEC2 - Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC2 - Threshold for background AF error to mic ratio to trigger weight download}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_saec_full_learn_frames;
    /**< @h2xmle_description {LEC2 - Initial number of frames to do force download of AF coefficients}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_threshold_lec_L16Q15;
    /**< @h2xmle_description {LEC2 - Double talk detection threshold for Linear EC adaptation}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_threshold_postp_L16Q15;
    /**< @h2xmle_description {LEC2 - Double talk detection threshold for EC PF processing}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_l_rate_L16;
    /**< @h2xmle_description {LEC2 - Left bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC2 - Threshold for background AF error to foreground AF error to trigger weight download}
         @h2xmle_default     {32000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC2 - Threshold for background AF error to foreground AF error to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC2 - Threshold for background AF error to mic ratio to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC2 - Number of divergence download frames to trigger divergence download}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_hangover_max_L16;
    /**< @h2xmle_description {LEC2 - Hangover duration for the double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC2 - Correction scale factor to account for background noise during EC adaptation}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC2 - Threshold for background AF error to mic ratio for double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC2 - Initial number of frames in double talk detection to allow weight adaptation}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_sflen_L16;
    /**< @h2xmle_description {LEC3 - Adaptive filter length for the Mic 3 LEC filter}
         @h2xmle_default     {100}
         @h2xmle_range       {4..400}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_wgtq_L16;
    /**< @h2xmle_description {LEC3 - Q-factor for the AF weights for the Mic3 LEC filter}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_r_update;
    /**< @h2xmle_description {LEC3 - Force update foreground AF at every fgUpdate th sample if this parameter is non zero}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_r_lrate;
    /**< @h2xmle_description {LEC3 - Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC3 - Threshold for background AF error to mic ratio to trigger weight download}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_saec_full_learn_frames_L16;
    /**< @h2xmle_description {LEC3 - Initial number of frames to do force download of AF coefficients}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_threshold_lec_L16Q15;
    /**< @h2xmle_description {LEC3 - Double talk detection threshold for Linear EC adaptation}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_threshold_postp_L16Q15;
    /**< @h2xmle_description {LEC3 - Double talk detection threshold for EC PF processing}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_l_rate_L16;
    /**< @h2xmle_description {LEC3 - Left bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC3 - Threshold for background AF error to foreground AF error to trigger weight download}
         @h2xmle_default     {32000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC3 - Threshold for background AF error to foreground AF error to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC3 - Threshold for background AF error to mic ratio to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC3 - Number of divergence download frames to trigger divergence download}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_hangover_max_L16;
    /**< @h2xmle_description {LEC3 - Hangover duration for the double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC3 - Correction scale factor to account for background noise during EC adaptation}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC3 - Threshold for background AF error to mic ratio for double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC3 - Initial number of frames in double talk detection to allow weight adaptation}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_sflen_L16;
    /**< @h2xmle_description {LEC4 - Adaptive filter length for the Mic 4 LEC filter}
         @h2xmle_default     {100}
         @h2xmle_range       {4..400}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_wgtq_L16;
    /**< @h2xmle_description {LEC4 - Q-factor for the AF weights for the Mic 4 LEC filter}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_r_update;
    /**< @h2xmle_description {LEC4 - Force update foreground AF at every fgUpdate th sample if this parameter is non zero}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_r_lrate;
    /**< @h2xmle_description {LEC4 - Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC4 - Threshold for background AF error to mic ratio to trigger weight download}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_saec_full_learn_frames_L16;
    /**< @h2xmle_description {LEC4 - Initial number of frames to do force download of AF coefficients}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_threshold_lec_L16Q15;
    /**< @h2xmle_description {LEC4 - Double talk detection threshold for Linear EC adaptation}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_threshold_postp_L16Q15;
    /**< @h2xmle_description {LEC4 - Double talk detection threshold for EC PF processing}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_l_rate_L16;
    /**< @h2xmle_description {LEC4 - Left bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC4 - Threshold for background AF error to foreground AF error to trigger weight download}
         @h2xmle_default     {32000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC4 - Threshold for background AF error to foreground AF error to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC4 - Threshold for background AF error to mic ratio to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC4 - Number of divergence download frames to trigger divergence download}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_hangover_max;
    /**< @h2xmle_description {LEC4 - Hangover duration for the double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC4 - Correction scale factor to account for background noise during EC adaptation}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC4 - Threshold for background AF error to mic ratio for double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC4 - Initial number of frames in double talk detection to allow weight adaptation}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_sflen_L16;
    /**< @h2xmle_description {LEC5 - Adaptive filter length for the Mic I HB LEC filter (Wideband)}
         @h2xmle_default     {100}
         @h2xmle_range       {4..400}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_wgtq_L16;
    /**< @h2xmle_description {LEC5 - Q-factor for the AF weights for the Mic1 HB LEC filter (Wideband)}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_r_update;
    /**< @h2xmle_description {LEC5 -Force update foreground AF at every fgUpdate th sample if this parameter is non zero (Wideband)}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_r_lrate;
    /**< @h2xmle_description {LEC5 -Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update (Wideband)}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to mic ratio to trigger weight download (Wideband)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_saec_full_learn_frames_L16;
    /**< @h2xmle_description {LEC5 -Initial number of frames to do force download of AF coefficients (Wideband)}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_threshold_lec_L16Q15;
    /**< @h2xmle_description {LEC5 -Double talk detection threshold for Linear EC adaptation (Wideband)}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_threshold_postp_L16Q15;
    /**< @h2xmle_description {LEC5 -Double talk detection threshold for EC PF processing (Wideband).}
         @h2xmle_default     {29000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_l_rate_L16;
    /**< @h2xmle_description {LEC5 -Left bit shift for accelerating the learning of AF coefficients in the LEC weight update (Wideband)}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to foreground AF error to trigger weight download (Wideband)}
         @h2xmle_default     {32000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to foreground AF error to activate divergence download (Wideband)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to mic ratio to activate divergence download (Wideband)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC5 -Number of divergence download frames to trigger divergence download (Wideband)}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_hangover_max_L16;
    /**< @h2xmle_description {LEC5 -Hangover duration for the double talk detection (Wideband)}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC5 -Correction scale factor to account for background noise during EC adaptation (Wideband)}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to mic ratio for double talk detection (Wideband)}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC5 -Initial number of frames in double talk detection to allow weight adaptation (Wideband)}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t aec_f_vad_lth_en_L32Q20;
    /**< @h2xmle_description {Far-end VAD threshold for low band LEC control}
         @h2xmle_default     {32000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t aec_f_vad_hb_lth_en_L32Q20;
    /**< @h2xmle_description {Far-end VAD threshold for high band LEC control}
         @h2xmle_default     {32000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t aec_f_vad_pf_lth_en_L32Q20;
    /**< @h2xmle_description {Far-end VAD threshold for EC PF control}
         @h2xmle_default     {32000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t aec_f_vad_ns_lth_en_L32Q20;
    /**< @h2xmle_description {Far-end VAD threshold for NS control}
         @h2xmle_default     {2147483647}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_thresh_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD threshold}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_thresh2_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD threshold-II}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_pwr_scale_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD noise power scale factor}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_hangover_max_L16;
    /**< @h2xmle_description {EC CNI near-end VAD hangover duration}
         @h2xmle_default     {15}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_alpha_snr_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD probability estimation smoothing factor (higher value means more smoothing)}
         @h2xmle_default     {3278}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_avar_scale_L16Q13;
    /**< @h2xmle_description {EC CNI near-end VAD noise variance scaling factor}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_sub_nc_L16;
    /**< @h2xmle_description {EC CNI near-end VAD noise floor tracking window length}
         @h2xmle_default     {25}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_snr_diff_max_L16Q8;
    /**< @h2xmle_description {EC CNI near-end VAD SNR difference max threshold}
         @h2xmle_default     {3072}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_snr_diff_min_L16Q8;
    /**< @h2xmle_description {EC CNI near-end VAD SNR difference min threshold}
         @h2xmle_default     {2560}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_init_length_L16;
    /**< @h2xmle_description {EC CNI near-end VAD initial frames length for power initialization}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_max_val_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD max bound on noise estimate}
         @h2xmle_default     {648}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_init_bound_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD initial bound for noise estimation}
         @h2xmle_default     {50}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_reset_bound_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD reset bound for noise estimation}
         @h2xmle_default     {290}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_spow_min_L16Q15;
    /**< @h2xmle_description {EC CNI near-end minimum bound for VAD speech power estimate}
         @h2xmle_default     {410}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int32_t qsf_flags;
    /**< @h2xmle_description {qsf_flags}
         @h2xmle_default     {30}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t qsf_stateover_wrtflag;
    /**< @h2xmle_description {qsf_stateover_wrtflag}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_stateover_wrt;
    /**< @h2xmle_description {qsf_stateover_wrt}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_min_gain_l_L16Q15;
    /**< @h2xmle_description {qsf_min_gain_l_L16Q15}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_min_gain_ml_L16Q15;
    /**< @h2xmle_description {qsf_min_gain_ml_L16Q15}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_min_gain_mu_L16Q15;
    /**< @h2xmle_description {qsf_min_gain_mu_L16Q15}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_min_gain_postu_L16Q15;
    /**< @h2xmle_description {qsf_min_gain_postu_L16Q15}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_min_gain_post_L16Q15;
    /**< @h2xmle_description {qsf_min_gain_post_L16Q15}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_min_gain_postx_L16Q15;
    /**< @h2xmle_description {qsf_min_gain_postx_L16Q15}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_min_gain_postup_L16Q15;
    /**< @h2xmle_description {qsf_min_gain_postup_L16Q15}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_startf_L16;
    /**< @h2xmle_description {qsf_startf_L16}
         @h2xmle_default     {0}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_endf_L16;
    /**< @h2xmle_description {qsf_endf_L16}
         @h2xmle_default     {128}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_distfb_L16;
    /**< @h2xmle_description {qsf_distfb_L16}
         @h2xmle_default     {40}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_distlr_L16;
    /**< @h2xmle_description {qsf_distlr_L16}
         @h2xmle_default     {35}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_indfb_p_L16;
    /**< @h2xmle_description {qsf_indfb_p_L16}
         @h2xmle_default     {0}
         @h2xmle_range       {0..4}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_indfb_s_L16;
    /**< @h2xmle_description {qsf_indfb_s_L16}
         @h2xmle_default     {2}
         @h2xmle_range       {0..4}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_indlr_p_L16;
    /**< @h2xmle_description {qsf_indlr_p_L16}
         @h2xmle_default     {0}
         @h2xmle_range       {0..4}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_indlr_s_L16;
    /**< @h2xmle_description {qsf_indlr_s_L16}
         @h2xmle_default     {1}
         @h2xmle_range       {0..4}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_gain_cdyn_L16;
    /**< @h2xmle_description {qsf_gain_cdyn_L16}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_gain_clr_shift_L16;
    /**< @h2xmle_description {qsf_gain_clr_shift_L16}
         @h2xmle_default     {10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_gain_cfb_shift_L16;
    /**< @h2xmle_description {qsf_gain_cfb_shift_L16}
         @h2xmle_default     {10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_omega_fb_lbnd_L16;
    /**< @h2xmle_description {qsf_omega_fb_lbnd_L16}
         @h2xmle_default     {30}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_omega_fb_hbnd_L16;
    /**< @h2xmle_description {qsf_omega_fb_hbnd_L16}
         @h2xmle_default     {100}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_vad_phbeg_L16;
    /**< @h2xmle_description {qsf_vad_phbeg_L16}
         @h2xmle_default     {16}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_vad_phend_L16;
    /**< @h2xmle_description {qsf_vad_phend_L16}
         @h2xmle_default     {64}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_vad_fact_L16Q15;
    /**< @h2xmle_description {qsf_vad_fact_L16Q15}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_vad_attack_L16Q15;
    /**< @h2xmle_description {qsf_vad_attack_L16Q15}
         @h2xmle_default     {24576}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_vad_decay_L16Q15;
    /**< @h2xmle_description {qsf_vad_decay_L16Q15}
         @h2xmle_default     {19661}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_staswitsm_fact_L16Q15;
    /**< @h2xmle_description {qsf_staswitsm_fact_L16Q15}
         @h2xmle_default     {327}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_contrastsm_fact_L16Q15;
    /**< @h2xmle_description {qsf_contrastsm_fact_L16Q15}
         @h2xmle_default     {8500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_owstate_thrd_L16Q15;
    /**< @h2xmle_description {qsf_owstate_thrd_L16Q15}
         @h2xmle_default     {18022}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_numstate_L16;
    /**< @h2xmle_description {qsf_numstate_L16}
         @h2xmle_default     {9}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_vad_thrd_L16Q15;
    /**< @h2xmle_description {qsf_vad_thrd_L16Q15}
         @h2xmle_default     {8000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_vad_hang_L16;
    /**< @h2xmle_description {qsf_vad_hang_L16}
         @h2xmle_default     {20}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_dtstatelearn_thrdL16;
    /**< @h2xmle_description {qsf_dtstatelearn_thrdL16}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_mingaincomp_L16Q15;
    /**< @h2xmle_description {qsf_mingaincomp_L16Q15}
         @h2xmle_default     {1310}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_bound_table_l_L16[9];
    /**< @h2xmle_description {];}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_bound_table_r_L16[9];
    /**< @h2xmle_description {];}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_bound_wid1_L16;
    /**< @h2xmle_description {qsf_bound_wid1_L16}
         @h2xmle_default     {15}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_bound_wid2_L16;
    /**< @h2xmle_description {qsf_bound_wid2_L16}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_sm_attack_L16Q15;
    /**< @h2xmle_description {qsf_sm_attack_L16Q15}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_sm_decay_L16Q15;
    /**< @h2xmle_description {qsf_sm_decay_L16Q15}
         @h2xmle_default     {21299}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_comp_sm_L16Q15;
    /**< @h2xmle_description {qsf_comp_sm_L16Q15}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_ang_to_lfb1_L16;
    /**< @h2xmle_description {qsf_ang_to_lfb1_L16}
         @h2xmle_default     {75}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_ang_to_lfb2_L16;
    /**< @h2xmle_description {qsf_ang_to_lfb2_L16}
         @h2xmle_default     {90}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_comp_ang_to_lfb1_L16;
    /**< @h2xmle_description {qsf_comp_ang_to_lfb1_L16}
         @h2xmle_default     {90}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_comp_ang_to_lfb2_L16;
    /**< @h2xmle_description {qsf_comp_ang_to_lfb2_L16}
         @h2xmle_default     {105}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_over_estfact_lb_L16Q15;
    /**< @h2xmle_description {qsf_over_estfact_lb_L16Q15}
         @h2xmle_default     {11469}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_over_estfact_hb_L16Q15;
    /**< @h2xmle_description {qsf_over_estfact_hb_L16Q15}
         @h2xmle_default     {11469}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_over_est_qfact_L16;
    /**< @h2xmle_description {qsf_over_est_qfact_L16}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_freqsm1_L16Q15;
    /**< @h2xmle_description {qsf_freqsm1_L16Q15}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_freqsm2_L16Q15;
    /**< @h2xmle_description {qsf_freqsm2_L16Q15}
         @h2xmle_default     {27853}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_freqsm3_L16Q15;
    /**< @h2xmle_description {qsf_freqsm3_L16Q15}
         @h2xmle_default     {24576}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_freqsm4_L16Q15;
    /**< @h2xmle_description {qsf_freqsm4_L16Q15}
         @h2xmle_default     {21299}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_statnse_sm_L16Q15;
    /**< @h2xmle_description {qsf_statnse_sm_L16Q15}
         @h2xmle_default     {3276}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_nonse_sm_L16Q15;
    /**< @h2xmle_description {qsf_nonse_sm_L16Q15}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_omega_l_L16[9];
    /**< @h2xmle_description {];}
         @h2xmle_default     {87}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_omega_r_L16[9];
    /**< @h2xmle_description {];}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_pp_const1__L16Q15;
    /**< @h2xmle_description {qsf_pp_const1__L16Q15}
         @h2xmle_default     {13107}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t qsf_pp_const2_L16Q15;
    /**< @h2xmle_description {qsf_pp_const2_L16Q15}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_pp_const3_L16Q15;
    /**< @h2xmle_description {qsf_pp_const3_L16Q15}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_pp_const4_L16Q15;
    /**< @h2xmle_description {qsf_pp_const4_L16Q15}
         @h2xmle_default     {9830}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_pp_filtconst1_L16Q15;
    /**< @h2xmle_description {qsf_pp_filtconst1_L16Q15}
         @h2xmle_default     {819}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_pp_filtconst2_L16Q15;
    /**< @h2xmle_description {qsf_pp_filtconst2_L16Q15}
         @h2xmle_default     {31130}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_pp_filtconst3_L16Q15;
    /**< @h2xmle_description {qsf_pp_filtconst3_L16Q15}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t qsf_pp_filtconst4_L16Q15;
    /**< @h2xmle_description {qsf_pp_filtconst4_L16Q15}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pf_saec_pathchange_factor_L16Q15;
    /**< @h2xmle_description {Path change factor for extra gain clipping in EC}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_num_spectral_bands_L16;
    /**< @h2xmle_description {Number of spectral bands for EC PF}
         @h2xmle_default     {32}
         @h2xmle_range       {8..128}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_lin_log_freq_L16;
    /**< @h2xmle_description {Linear-log boundary frequency for Mel scale in EC PF processing}
         @h2xmle_default     {700}
         @h2xmle_range       {700..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_pc_add_agg_q_L16;
    /**< @h2xmle_description {Additional Q-factor for EC PF aggressiveness during path change}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_gain_floor_L16Q15;
    /**< @h2xmle_description {Minimum EC PF gain for each band}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pf_nlp_st_agg_L16Q15;
    /**< @h2xmle_description {EC PF aggressiveness during single talk (far-end alone)}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pf_nlp_dt_agg_L16Q15;
    /**< @h2xmle_description {EC PF aggressiveness during double talk}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pf_agg_q_L16;
    /**< @h2xmle_description {Q-factor for EC PF aggressiveness}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pf_nlp_boost_L16Q12;
    /**< @h2xmle_description {Slope Boost factor for the non-linear EC PF processing}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pf_rx_ec_factor_L16Q13;
    /**< @h2xmle_description {Factor for applying Rx energy to estimated echo energy}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_energy_t_factor_L16Q15[32];
    /**< @h2xmle_description {EC PF energy factor to apply for spectral band[]}
         @h2xmle_default     {819}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pf_salph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the Mic energy input in EC PF (Higher value means less smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_salph_edown_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the echo energy input in EC PF (Higher value means less smoothing)}
         @h2xmle_default     {9830}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_galph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the EC PF gain (Higher value means less smoothing)}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_lin_log_L16Q13;
    /**< @h2xmle_description {Normalized transition frequency in EC PF}
         @h2xmle_default     {716}
         @h2xmle_range       {700..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_mela_L16Q8;
    /**< @h2xmle_description {Multiplication factor in EC PF}
         @h2xmle_default     {5120}
         @h2xmle_range       {640..5120}


         @h2xmle_policy       {Advanced} */

    int32_t aec_pf_nl_echo_en_thsd_b_L16[32];
    /**< @h2xmle_description {Non-linear echo energy threshold energy factor to apply for spectral band []}
         @h2xmle_default     {0}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t aec_pf_clip_saec_clip_factor2_L16Q15;
    /**< @h2xmle_description {EC PF gain clip factor to be applied during initial duration after call start}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_clip_sinit_frames_L16;
    /**< @h2xmle_description {Initial duration for applying the EC PF gain clip factor}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_clip_saec_clip_factor_L16Q15;
    /**< @h2xmle_description {EC PF gain clip factor to be applied in normal cases}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cn_norm_const_L16Q15;
    /**< @h2xmle_description {EC CNI scaling factor}
         @h2xmle_default     {18022}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cn_norm_const_q_L16;
    /**< @h2xmle_description {EC CNI estimate Q factor}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cn_salph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the EC CNI noise estimate estimate (Higher value means less smoothing)}
         @h2xmle_default     {22938}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cn_ms_frames_L16;
    /**< @h2xmle_description {EC CNI noise floor estimation window lenth}
         @h2xmle_default     {200}
         @h2xmle_range       {1..1000}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cn_ms_num_blk_L16;
    /**< @h2xmle_description {EC CNI minimum statistics bulk size}
         @h2xmle_default     {10}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_noise_level_min_gain_L16Q15[4];
    /**< @h2xmle_description {];}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t s3_pp_noise_level_agg_L16Q12[4];
    /**< @h2xmle_description {];}
         @h2xmle_default     {300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_noise_level_agg2_L16Q15[4];
    /**< @h2xmle_description {];}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_int_decay_factor_L16Q15;
    /**< @h2xmle_description {s3_pp_int_decay_factor_L16Q15}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_int_delta_offset_L16Q15;
    /**< @h2xmle_description {s3_pp_int_delta_offset_L16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_int_delta_mult_L16Q13;
    /**< @h2xmle_description {s3_pp_int_delta_mult_L16Q13}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_sm_gain_alph_inc_L16Q15;
    /**< @h2xmle_description {s3_pp_sm_gain_alph_inc_L16Q15}
         @h2xmle_default     {20480}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_sm_gain_alph_dec_L16Q15;
    /**< @h2xmle_description {s3_pp_sm_gain_alph_dec_L16Q15}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_noise_ref_factor_L16Q14;
    /**< @h2xmle_description {s3_pp_noise_ref_factor_L16Q14}
         @h2xmle_default     {20000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_lt_vndev_factor_L16Q14;
    /**< @h2xmle_description {s3_pp_lt_vndev_factor_L16Q14}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_lt_vnfactor_L16Q14;
    /**< @h2xmle_description {s3_pp_lt_vnfactor_L16Q14}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_sm_ltalph_L16Q15;
    /**< @h2xmle_description {s3_pp_sm_ltalph_L16Q15}
         @h2xmle_default     {328}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_sm_ltdev_alph_L16Q15;
    /**< @h2xmle_description {s3_pp_sm_ltdev_alph_L16Q15}
         @h2xmle_default     {1638}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_sm_ltalph_init_L16Q15;
    /**< @h2xmle_description {s3_pp_sm_ltalph_init_L16Q15}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_freq_dep_cutoff_L16[4];
    /**< @h2xmle_description {];}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t s3_pp_freq_dep_min_gain_L16Q15[5];
    /**< @h2xmle_description {];}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t s3_pp_noise_min_thresh_L32[4];
    /**< @h2xmle_description {];}
         @h2xmle_default     {687865856}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t out_gain_L16Q11;
    /**< @h2xmle_description {Output gain for Fluence V5}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t reserved3;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t aec_cfg0_sQShift_max_L16;
    /**< @h2xmle_description {LEC 1: Upper bound on the Q-factor shift of the Foreground filter during EC adapation}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_sQShift_max_L16;
    /**< @h2xmle_description {LEC 2: Upper bound on the Q-factor shift of the Foreground filter during EC adapation}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_sQShift_max_L16;
    /**< @h2xmle_description {LEC 3: Upper bound on the Q-factor shift of the Foreground filter during EC adapation}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_sQShift_max_L16;
    /**< @h2xmle_description {LEC 4: Upper bound on the Q-factor shift of the Foreground filter during EC adapation}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_sQShift_max_L16;
    /**< @h2xmle_description {LEC 5: Upper bound on the Q-factor shift of the Foreground filter during EC adapation (Wideband)}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved4;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t aec_cfg0_sNumframes;
    /**< @h2xmle_description {LEC1: Number of consecutive good frames before activating weight download for LEC}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg1_sNumframes;
    /**< @h2xmle_description {LEC2: Number of consecutive good frames before activating weight download for LEC}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg2_sNumframes;
    /**< @h2xmle_description {LEC3: Number of consecutive good frames before activating weight download for LEC}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg3_sNumframes;
    /**< @h2xmle_description {LEC4: Number of consecutive good frames before activating weight download for LEC}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg4_sNumframes;
    /**< @h2xmle_description {LEC5: Number of consecutive good frames before activating weight download for LEC (Wideband)}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t reserved5;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t aecCfg_abf_echo_shift;
    /**< @h2xmle_description {aecCfg_abf_echo_shift}
         @h2xmle_default     {-4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_aecSfCfg_vssCfg_sVss_Min_Mu_Q15;
    /**< @h2xmle_description {aec_cfg0_aecSfCfg_vssCfg_sVss_Min_Mu_Q15}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_aecSfCfg_vssCfg_sVss_Min_Mu_Q15;
    /**< @h2xmle_description {aec_cfg1_aecSfCfg_vssCfg_sVss_Min_Mu_Q15}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_aecSfCfg_vssCfg_sVss_Min_Mu_Q15;
    /**< @h2xmle_description {aec_cfg4_aecSfCfg_vssCfg_sVss_Min_Mu_Q15}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aecCfg_aecSfCfg_Ch0_dtdCfg_sDtd_Al_Bitshift;
    /**< @h2xmle_description {aecCfg_aecSfCfg_Ch0_dtdCfg_sDtd_Al_Bitshift}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_aecSfCfg_filtAdpCfg_sTestSuccess;
    /**< @h2xmle_description {aec_cfg0_aecSfCfg_filtAdpCfg_sTestSuccess}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_aecSfCfg_filtAdpCfg_sTestSuccess;
    /**< @h2xmle_description {aec_cfg1_aecSfCfg_filtAdpCfg_sTestSuccess}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_aecSfCfg_filtAdpCfg_sTestSuccess;
    /**< @h2xmle_description {aec_cfg4_aecSfCfg_filtAdpCfg_sTestSuccess}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t aec_cfg0_aecSfCfg_filtAdpCfg_lAec_sc2_Q31;
    /**< @h2xmle_description {aec_cfg0_aecSfCfg_filtAdpCfg_lAec_sc2_Q31}
         @h2xmle_default     {687865856}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t aec_cfg1_aecSfCfg_filtAdpCfg_lAec_sc2_Q31;
    /**< @h2xmle_description {aec_cfg1_aecSfCfg_filtAdpCfg_lAec_sc2_Q31}
         @h2xmle_default     {687865856}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t aec_cfg4_aecSfCfg_filtAdpCfg_lAec_sc2_Q31;
    /**< @h2xmle_description {aec_cfg4_aecSfCfg_filtAdpCfg_lAec_sc2_Q31}
         @h2xmle_default     {687865856}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t aecCfg_farEndVadPFCfg_sHangoverMax;
    /**< @h2xmle_description {aecCfg_farEndVadPFCfg_sHangoverMax}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_saec_full_learn_frames_L16_DT;
    /**< @h2xmle_description {aec_cfg0_saec_full_learn_frames_L16_DT}
         @h2xmle_default     {300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_l_rate_L16_DT;
    /**< @h2xmle_description {aec_cfg0_l_rate_L16_DT}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_saec_full_learn_frames_L16_DT;
    /**< @h2xmle_description {aec_cfg1_saec_full_learn_frames_L16_DT}
         @h2xmle_default     {300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_l_rate_L16_DT;
    /**< @h2xmle_description {aec_cfg1_l_rate_L16_DT}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_saec_full_learn_frames_L16_DT;
    /**< @h2xmle_description {aec_cfg4_saec_full_learn_frames_L16_DT}
         @h2xmle_default     {300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_l_rate_L16_DT;
    /**< @h2xmle_description {aec_cfg4_l_rate_L16_DT}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t Reserved6;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int16_t post_gain_L16q11[4];
    /**< @h2xmle_description {];}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_LB_clipFlag_Al_BitShift;
    /**< @h2xmle_description {aec_LB_clipFlag_Al_BitShift}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_HB_clipFlag_Al_BitShift;
    /**< @h2xmle_description {aec_HB_clipFlag_Al_BitShift}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t EC_Ref_HPF_Enable;
    /**< @h2xmle_description {EC_Ref_HPF_Enable}
         @h2xmle_default     {0}
         @h2xmle_range       {0..1}


         @h2xmle_policy       {Advanced} */

    int16_t EC_Ref_HPF_Cutoff;
    /**< @h2xmle_description {EC_Ref_HPF_Cutoff}
         @h2xmle_default     {100}
         @h2xmle_range       {50..1000}


         @h2xmle_policy       {Advanced} */

    int32_t aec_rx_channel_flag;
    /**< @h2xmle_description {aec_rx_channel_flag}
         @h2xmle_default     {3}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t aec_feature_mode;
    /**< @h2xmle_description {aec_feature_mode}
         @h2xmle_default     {0}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;

#define VOICE_PARAM_FLUENCE_EC_SPK 0x00010E35

#include "adsp_begin_pack.h"

typedef struct voice_param_fluence_ec_t voice_param_fluence_ec_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_EC_SPK", VOICE_PARAM_FLUENCE_EC_SPK}

    @h2xmlp_description  {Fluence version 5 broadside and Fluence Pro version 2 ECNS.\n
Maximum array size for this parameter \n
Version 0  716 bytes \n
Version 1 extension  724 bytes \n
Version 2 extension  732 bytes \n
Version 3 extension  740 bytes \n\n}
@h2xmlp_version     {3} */

struct voice_param_fluence_ec_t {

    uint16_t Version;
    /**< @h2xmle_description {Version.}
         @h2xmle_default     {3}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion   (true)


         @h2xmle_readOnly       {true} */

    int16_t reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t mode;
    /**< @h2xmle_description {Mode word for enabling/disabling sub-modules in Fluence V5}
         @h2xmle_default     {276824877}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "276824876"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "276824877"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "276824878"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "310379308"}

         @h2xmle_policy       {Basic} */

    int16_t prim_in_chan_L16;
    /**< @h2xmle_description {Channel Map for the primary mic path (hardcoded)}
         @h2xmle_default     {0}
         @h2xmle_range       {0..0}


         @h2xmle_policy       {Advanced} */

    int16_t mic_ch_map_L16[4];
    /**< @h2xmle_description {Channel Map for Mic 1 path to Fluence V5 (hardcoded)}
         @h2xmle_default     {0}
         @h2xmle_range       {0..3}


         @h2xmle_policy       {Advanced} */

    int16_t input_gain_L16q13[4];
    /**< @h2xmle_description {Input Gain for Mic-I input}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_bp_sections;
    /**< @h2xmle_description {Number of BPF sections used for filtering the input signals before wind noise detection (WND)}
         @h2xmle_default     {2}
         @h2xmle_range       {0..2}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_bp_numq_L16[4];
    /**< @h2xmle_description {Numerator coeffcient's integer part of Q factor (WND BPF)}
         @h2xmle_default     {2}
         @h2xmle_range       {-8..8}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_bp_denq_L16[4];
    /**< @h2xmle_description {Denominator coeffcient's integer part of Q factor (WND BPF)}
         @h2xmle_default     {2}
         @h2xmle_range       {-8..8}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_calph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the cross-covariance estimation in WND}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_cov_bit_shf_L16;
    /**< @h2xmle_description {Right shift (bit-shift) for the cross-covariance estimate in WND}
         @h2xmle_default     {6}
         @h2xmle_range       {-31..31}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_ws_alph_inc_L16Q15;
    /**< @h2xmle_description {Attack time smoothing factor for the WND score (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_ws_alph_dec_L16Q15;
    /**< @h2xmle_description {Decay time smoothing factor for the WND score (Higher value means less smoothing)}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_pri_wind_boost_L16Q10;
    /**< @h2xmle_description {Boost factor for self RMS value of the primary mic wind indicator}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t wnd_pri_wind_fact_L16Q15;
    /**< @h2xmle_description {Normalization factor for computing the primary mic wind indicator}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int32_t wnd_bp_numcoeff_L32[12];
    /**< @h2xmle_description {Numerator coeffcient for WND BPF}
         @h2xmle_default     {2975724}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t wnd_bp_dencoeff_L32[8];
    /**< @h2xmle_description {Denominator coefficient for WND BPF}
         @h2xmle_default     {-954895665}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t wnd_det_thresh_L32Q31[2];
    /**< @h2xmle_description {Lower threshold for Wind noise detection (no wind detected if wind score is below this threshold)}
         @h2xmle_default     {644218880}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t wnd_cov_eps_L32Q31;
    /**< @h2xmle_description {Epsilon factor to add to the cross-covariance estimate for stabilizing the value}
         @h2xmle_default     {21}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t reserved1;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t wnr_dfc_max_L16;
    /**< @h2xmle_description {Step size increment in WNR HPF cut-off frequency}
         @h2xmle_default     {20}
         @h2xmle_range       {10..200}


         @h2xmle_policy       {Basic} */

    int32_t wnr_agg_c_L32;
    /**< @h2xmle_description {Maximum cut-off frequency of the HPF used in WNR}
         @h2xmle_default     {800}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t reserved2;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int16_t aec_clip_flag_lb_frac_L16Q15;
    /**< @h2xmle_description {Threshold for the low band NLMS output error to mic ratio for applying extra clipping}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "10000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "10000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "10000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1092"}

         @h2xmle_policy       {Basic} */

    int16_t aec_clip_flag_hb_frac_L16Q15;
    /**< @h2xmle_description {Threshold for the high band NLMS output error to mic ratio for applying extra clipping}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "10000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "10000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "10000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1092"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pathchange_flag_frac_L16Q15;
    /**< @h2xmle_description {Foreground to background NLMS output error ratio threshold for path change detection}
         @h2xmle_default     {27305}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_pathchange_wgt_enfact_L16Q15;
    /**< @h2xmle_description {NLMS weight energy factor for path change detection}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "22938"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pathchange_wgt_fact_L16Q15;
    /**< @h2xmle_description {NLMS weight change factor for path change detection}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pathchange_wgt_alph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the Background filter energy (Higher value means less smoothing)}
         @h2xmle_default     {26216}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26216"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26216"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26216"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "6554"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pathchange_wgt_enfact_d_L16Q15;
    /**< @h2xmle_description {NLMS weight energy factor for coeff download}
         @h2xmle_default     {9830}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "9830"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "9830"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "9830"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "13107"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_pathchange_wgt_trig_d_L16;
    /**< @h2xmle_description {Number of frames with weight increase for triggering AF coeff download during path change detection}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg0_sflen_L16;
    /**< @h2xmle_description {LEC1 - Adaptive filter length for the Mic  LEC filter}
         @h2xmle_default     {128}
         @h2xmle_range       {4..400}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_wgtq_L16;
    /**< @h2xmle_description {LEC1 - Q-factor for the AF weights for the Mic LEC filter}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_r_update;
    /**< @h2xmle_description {LEC1 -Force update foreground AF at every fgUpdate th sample if this parameter is non zero}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_r_lrate;
    /**< @h2xmle_description {LEC1 -Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg0_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to mic ratio to trigger weight download}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1092"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_saec_full_learn_frames_L16;
    /**< @h2xmle_description {LEC1 -Initial number of frames to do force download of AF coefficients}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Basic} */

    int16_t aec_cfg0_dtd_threshold_lec__L16Q15;
    /**< @h2xmle_description {LEC1 -Double talk detection threshold for Linear EC adaptation}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_threshold_postp__L16Q15;
    /**< @h2xmle_description {LEC1 -Double talk detection threshold for EC PF processing}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_l_rate_L16;
    /**< @h2xmle_description {LEC1 -Left bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "8"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to foreground AF error to trigger weight download}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29789"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to foreground AF error to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to mic ratio to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC1 -Number of divergence download frames to trigger divergence download}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_hangover_max_L16;
    /**< @h2xmle_description {LEC1 -Hangover duration for the double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "20"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC1 -Correction scale factor to account for background noise during EC adaptation}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "41"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC1 -Threshold for background AF error to mic ratio for double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "656"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC1 -Initial number of frames in double talk detection to allow weight adaptation}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "200"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_sflen_L16;
    /**< @h2xmle_description {LEC2 - Adaptive filter length for the Mic 2 LEC filter}
         @h2xmle_default     {128}
         @h2xmle_range       {4..400}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_wgtq_L16;
    /**< @h2xmle_description {LEC2 - Q-factor for the AF weights for the Mic2 LEC filter}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_r_update;
    /**< @h2xmle_description {LEC2 -Force update foreground AF at every fgUpdate th sample if this parameter is non zero}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_r_lrate;
    /**< @h2xmle_description {LEC2 -Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC2 -Threshold for background AF error to mic ratio to trigger weight download}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1092"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_saec_full_learn_frames;
    /**< @h2xmle_description {LEC2 -Initial number of frames to do force download of AF coefficients}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_threshold_lec_L16Q15;
    /**< @h2xmle_description {LEC2 -Double talk detection threshold for Linear EC adaptation}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_threshold_postp_L16Q15;
    /**< @h2xmle_description {LEC2 -Double talk detection threshold for EC PF processing}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_l_rate_L16;
    /**< @h2xmle_description {LEC2 -Left bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "8"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC2 -Threshold for background AF error to foreground AF error to trigger weight download}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29789"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC2 -Threshold for background AF error to foreground AF error to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC2 -Threshold for background AF error to mic ratio to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC2 -Number of divergence download frames to trigger divergence download}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_hangover_max_L16;
    /**< @h2xmle_description {LEC2 -Hangover duration for the double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "20"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC2 -Correction scale factor to account for background noise during EC adaptation}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "41"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC2 -Threshold for background AF error to mic ratio for double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "656"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC2 -Initial number of frames in double talk detection to allow weight adaptation}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "200"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_sflen_L16;
    /**< @h2xmle_description {LEC3 - Adaptive filter length for the Mic 3 LEC filter}
         @h2xmle_default     {128}
         @h2xmle_range       {4..400}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_wgtq_L16;
    /**< @h2xmle_description {LEC3 - Q-factor for the AF weights for the Mic3 LEC filter}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_r_update;
    /**< @h2xmle_description {LEC3 -Force update foreground AF at every fgUpdate th sample if this parameter is non zero}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_r_lrate;
    /**< @h2xmle_description {LEC3 -Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC3 -Threshold for background AF error to mic ratio to trigger weight download}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1092"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_saec_full_learn_frames_L16;
    /**< @h2xmle_description {LEC3 -Initial number of frames to do force download of AF coefficients}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_threshold_lec_L16Q15;
    /**< @h2xmle_description {LEC3 -Double talk detection threshold for Linear EC adaptation}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_threshold_postp_L16Q15;
    /**< @h2xmle_description {LEC3 -Double talk detection threshold for EC PF processing}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_l_rate_L16;
    /**< @h2xmle_description {LEC3 -Left bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "8"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC3 -Threshold for background AF error to foreground AF error to trigger weight download}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29789"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC3 -Threshold for background AF error to foreground AF error to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC3 -Threshold for background AF error to mic ratio to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC3 -Number of divergence download frames to trigger divergence download}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_hangover_max_L16;
    /**< @h2xmle_description {LEC3 -Hangover duration for the double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "20"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC3 -Correction scale factor to account for background noise during EC adaptation}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "41"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC3 -Threshold for background AF error to mic ratio for double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "656"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC3 -Initial number of frames in double talk detection to allow weight adaptation}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "200"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_sflen_L16;
    /**< @h2xmle_description {LEC4 - Adaptive filter length for the Mic 4 LEC filter}
         @h2xmle_default     {128}
         @h2xmle_range       {4..400}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_wgtq_L16;
    /**< @h2xmle_description {LEC4 - Q-factor for the AF weights for the Mic 4 LEC filter}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_r_update;
    /**< @h2xmle_description {LEC4 -Force update foreground AF at every fgUpdate th sample if this parameter is non zero}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_r_lrate;
    /**< @h2xmle_description {LEC4 -Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC4 -Threshold for background AF error to mic ratio to trigger weight download}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1092"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_saec_full_learn_frames_L16;
    /**< @h2xmle_description {LEC4 -Initial number of frames to do force download of AF coefficients}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_threshold_lec_L16Q15;
    /**< @h2xmle_description {LEC4 -Double talk detection threshold for Linear EC adaptation}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_threshold_postp_L16Q15;
    /**< @h2xmle_description {LEC4 -Double talk detection threshold for EC PF processing}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_l_rate_L16;
    /**< @h2xmle_description {LEC4 -Left bit shift for accelerating the learning of AF coefficients in the LEC weight update}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "8"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC4 -Threshold for background AF error to foreground AF error to trigger weight download}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29789"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC4 -Threshold for background AF error to foreground AF error to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC4 -Threshold for background AF error to mic ratio to activate divergence download}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC4 -Number of divergence download frames to trigger divergence download}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_hangover_max;
    /**< @h2xmle_description {LEC4 -Hangover duration for the double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "20"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC4 -Correction scale factor to account for background noise during EC adaptation}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "41"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC4 -Threshold for background AF error to mic ratio for double talk detection}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "656"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC4 -Initial number of frames in double talk detection to allow weight adaptation}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "200"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_sflen_L16;
    /**< @h2xmle_description {LEC5 - Adaptive filter length for the Mic I HB LEC filter (Wideband)}
         @h2xmle_default     {128}
         @h2xmle_range       {4..400}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "128"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_wgtq_L16;
    /**< @h2xmle_description {LEC5 - Q-factor for the AF weights for the Mic1 HB LEC filter (Wideband)}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_r_update;
    /**< @h2xmle_description {LEC5 -Force update foreground AF at every fgUpdate th sample if this parameter is non zero (Wideband)}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_r_lrate;
    /**< @h2xmle_description {LEC5 -Reduced bit shift for accelerating the learning of AF coefficients in the LEC weight update (Wideband)}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_sbg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to mic ratio to trigger weight download (Wideband)}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1092"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_saec_full_learn_frames_L16;
    /**< @h2xmle_description {LEC5 -Initial number of frames to do force download of AF coefficients (Wideband)}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "20"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_threshold_lec_L16Q15;
    /**< @h2xmle_description {LEC5 -Double talk detection threshold for Linear EC adaptation (Wideband)}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_threshold_postp_L16Q15;
    /**< @h2xmle_description {LEC5 -Double talk detection threshold for EC PF processing (Wideband)}
         @h2xmle_default     {26000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "26000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29490"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_l_rate_L16;
    /**< @h2xmle_description {LEC5 -Left bit shift for accelerating the learning of AF coefficients in the LEC weight update (Wideband)}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "8"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to foreground AF error to trigger weight download (Wideband)}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "29789"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_divg_bg_fg_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to foreground AF error to activate divergence download (Wideband)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_divg_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to mic ratio to activate divergence download (Wideband)}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "16384"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_divg_frame_trig_L16;
    /**< @h2xmle_description {LEC5 -Number of divergence download frames to trigger divergence download (Wideband)}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_hangover_max_L16;
    /**< @h2xmle_description {LEC5 -Hangover duration for the double talk detection (Wideband)}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "20"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_ni_scalefactor_L16;
    /**< @h2xmle_description {LEC5 -Correction scale factor to account for background noise during EC adaptation (Wideband)}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "400"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "41"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_bg_mic_frac_L16Q15;
    /**< @h2xmle_description {LEC5 -Threshold for background AF error to mic ratio for double talk detection (Wideband)}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "656"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_dtd_good_frames_L16;
    /**< @h2xmle_description {LEC5 -Initial number of frames in double talk detection to allow weight adaptation (Wideband)}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "200"}

         @h2xmle_policy       {Basic} */

    int32_t aec_f_vad_lth_en_L32Q20;
    /**< @h2xmle_description {Far-end VAD threshold for low band LEC control}
         @h2xmle_default     {8000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "32000"}

         @h2xmle_policy       {Basic} */

    int32_t aec_f_vad_hb_lth_en_L32Q20;
    /**< @h2xmle_description {Far-end VAD threshold for high band LEC control}
         @h2xmle_default     {8000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "20480"}

         @h2xmle_policy       {Basic} */

    int32_t aec_f_vad_pf_lth_en_L32Q20;
    /**< @h2xmle_description {Far-end VAD threshold for EC PF control}
         @h2xmle_default     {4000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "4000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "4000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "4000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "20480"}

         @h2xmle_policy       {Advanced} */

    int32_t aec_f_vad_ns_lth_en_L32Q20;
    /**< @h2xmle_description {Far-end VAD threshold for NS control}
         @h2xmle_default     {2147483647}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "2147483647"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "2147483647"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "2147483647"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "163845"}

         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_thresh_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD threshold}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "16384"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "16384"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "16384"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "26214"}

         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_thresh2_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD threshold-II}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "16384"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "16384"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "16384"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "26214"}

         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_pwr_scale_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD noise power scale factor}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_hangover_max_L16;
    /**< @h2xmle_description {EC CNI near-end VAD hangover duration}
         @h2xmle_default     {30}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "30"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "30"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "30"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "15"}

         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_alpha_snr_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD probability estimation smoothing factor (higher value means more smoothing)}
         @h2xmle_default     {3278}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_avar_scale_L16Q13;
    /**< @h2xmle_description {EC CNI near-end VAD noise variance scaling factor}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_sub_nc_L16;
    /**< @h2xmle_description {EC CNI near-end VAD noise floor tracking window length}
         @h2xmle_default     {25}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_snr_diff_max_L16Q8;
    /**< @h2xmle_description {EC CNI near-end VAD SNR difference max threshold}
         @h2xmle_default     {3072}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_snr_diff_min_L16Q8;
    /**< @h2xmle_description {EC CNI near-end VAD SNR difference min threshold}
         @h2xmle_default     {2560}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_init_length_L16;
    /**< @h2xmle_description {EC CNI near-end VAD initial frames length for power initialization}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_max_val_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD max bound on noise estimate}
         @h2xmle_default     {648}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_init_bound_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD initial bound for noise estimation}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_reset_bound_L16Q15;
    /**< @h2xmle_description {EC CNI near-end VAD reset bound for noise estimation}
         @h2xmle_default     {290}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ec_vad_spow_min_L16Q15;
    /**< @h2xmle_description {EC CNI near-end minimum bound for VAD speech power estimate}
         @h2xmle_default     {410}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ecns_dtstatelearn_thrdL16;
    /**< @h2xmle_description {Double talk detection threshold for controlling NS module updates (Higher value means less sensitive detection)}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_saec_pathchange_factor_L16Q15;
    /**< @h2xmle_description {Path change factor for extra gain clipping in EC}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_num_spectral_bands_L16;
    /**< @h2xmle_description {Number of spectral bands for EC PF}
         @h2xmle_default     {32}
         @h2xmle_range       {8..128}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "20"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_lin_log_freq_L16;
    /**< @h2xmle_description {Linear-log boundary frequency for Mel scale in EC PF processing}
         @h2xmle_default     {700}
         @h2xmle_range       {700..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_pc_add_agg_q_L16;
    /**< @h2xmle_description {Additional Q-factor for EC PF aggressiveness during path change}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "3"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "3"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "3"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pf_gain_floor_L16Q15;
    /**< @h2xmle_description {Minimum EC PF gain for each band}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "98"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pf_nlp_st_agg_L16Q15;
    /**< @h2xmle_description {EC PF aggressiveness during single talk (far-end alone)}
         @h2xmle_default     {32000}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "18432"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pf_nlp_dt_agg_L16Q15;
    /**< @h2xmle_description {EC PF aggressiveness during double talk}
         @h2xmle_default     {30000}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "30000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "30000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "30000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "18432"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pf_agg_q_L16;
    /**< @h2xmle_description {Q-factor for EC PF aggressiveness}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "6"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "2"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pf_nlp_boost_L16Q12;
    /**< @h2xmle_description {Slope Boost factor for the non-linear EC PF processing}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_rx_ec_factor_L16Q13;
    /**< @h2xmle_description {Factor for applying Rx energy to estimated echo energy}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "4096"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pf_energy_t_factor_L16Q15[32];
    /**< @h2xmle_description {EC PF energy factor to apply for spectral band 1}
         @h2xmle_default     {800}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "800"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "800"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "800"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "819"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pf_salph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the Mic energy input in EC PF  (Higher value means less smoothing)}
         @h2xmle_default     {29490}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "29490"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "29490"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "29490"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "32440"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_salph_edown_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the echo energy input in EC PF  (Higher value means less smoothing)}
         @h2xmle_default     {9830}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "9830"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "9830"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "9830"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "24576"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_galph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the EC PF gain (Higher value means less smoothing)}
         @h2xmle_default     {32540}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32540"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32540"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32540"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "26214"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_lin_log_L16Q13;
    /**< @h2xmle_description {Normalized transition frequency in EC PF}
         @h2xmle_default     {700}
         @h2xmle_range       {700..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "700"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "700"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "700"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "716"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_mela_L16Q8;
    /**< @h2xmle_description {Multiplication factor in EC PF}
         @h2xmle_default     {2560}
         @h2xmle_range       {640..5120}


         @h2xmle_policy       {Basic} */

    int32_t aec_pf_nl_echo_en_thsd_b_L16[32];
    /**< @h2xmle_description {Non-linear echo energy threshold energy factor to apply for spectral band 1}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "20000"}

         @h2xmle_policy       {Basic} */

    int16_t aec_pf_clip_saec_clip_factor2_L16Q15;
    /**< @h2xmle_description {EC PF gain clip factor to be applied during initial duration after call start}
         @h2xmle_default     {327}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "327"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "327"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "327"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "3277"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_clip_sinit_frames_L16;
    /**< @h2xmle_description {Initial duration for applying the EC PF gain clip factor}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "200"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "200"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "200"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_pf_clip_saec_clip_factor_L16Q15;
    /**< @h2xmle_description {EC PF gain clip factor to be applied in normal cases}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cn_norm_const_L16Q15;
    /**< @h2xmle_description {EC CNI scaling factor}
         @h2xmle_default     {24576}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "24576"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "24576"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "24576"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "18022"}

         @h2xmle_policy       {Basic} */

    int16_t aec_cn_norm_const_q_L16;
    /**< @h2xmle_description {EC CNI estimate Q factor}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "3"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cn_salph_L16Q15;
    /**< @h2xmle_description {Smoothing factor for the EC CNI noise estimate estimate (Higher value means less smoothing)}
         @h2xmle_default     {13107}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cn_ms_frames_L16;
    /**< @h2xmle_description {EC CNI noise floor estimation window lenth}
         @h2xmle_default     {200}
         @h2xmle_range       {1..1000}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cn_ms_num_blk_L16;
    /**< @h2xmle_description {EC CNI minimum statistics bulk size}
         @h2xmle_default     {10}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Basic} */

    int16_t out_gain_L16Q11;
    /**< @h2xmle_description {Output gain for Fluence}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_sQShift_max_L16;
    /**< @h2xmle_description {LEC 1: Upper bound on the Q-factor shift of the Foreground filter during EC adapation}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_sQShift_max_L16;
    /**< @h2xmle_description {LEC 2: Upper bound on the Q-factor shift of the Foreground filter during EC adapation}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_sQShift_max_L16;
    /**< @h2xmle_description {LEC 3: Upper bound on the Q-factor shift of the Foreground filter during EC adapation}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_sQShift_max_L16;
    /**< @h2xmle_description {LEC 4: Upper bound on the Q-factor shift of the Foreground filter during EC adapation}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_sQShift_max_L16;
    /**< @h2xmle_description {LEC 5: Upper bound on the Q-factor shift of the Foreground filter during EC adapation (Wideband)}
         @h2xmle_default     {-10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_cfg0_sNumframes;
    /**< @h2xmle_description {LEC1: Number of consecutive good frames before activating weight download for LEC}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "4"}

         @h2xmle_policy       {Basic} */

    int16_t aec_cfg1_sNumframes;
    /**< @h2xmle_description {LEC2: Number of consecutive good frames before activating weight download for LEC}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "4"}

         @h2xmle_policy       {Basic} */

    int16_t aec_cfg2_sNumframes;
    /**< @h2xmle_description {LEC3: Number of consecutive good frames before activating weight download for LEC}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "4"}

         @h2xmle_policy       {Basic} */

    int16_t aec_cfg3_sNumframes;
    /**< @h2xmle_description {LEC4: Number of consecutive good frames before activating weight download for LEC}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "4"}

         @h2xmle_policy       {Basic} */

    int16_t aec_cfg4_sNumframes;
    /**< @h2xmle_description {LEC5: Number of consecutive good frames before activating weight download for LEC (Wideband)}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "4"}

         @h2xmle_policy       {Advanced} */

    int16_t aecCfg_abf_echo_shift;
    /**< @h2xmle_description {Q0 shift factor for scaling echo energy computed from Rx signal}
         @h2xmle_default     {-8}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "-8"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "-8"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "-8"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "-4"}

         @h2xmle_policy       {Advanced} */

    int16_t aecCfg_aecSfCfg_vssCfg_sVss_Min_Mu_Q15[5];
    /**< @h2xmle_description {Q15 minimum value of step size for Mic 1 LEC adaptation}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "327"}

         @h2xmle_policy       {Advanced} */

    int16_t aecCfg_aecSfCfg_Ch0_dtdCfg_sDtd_Al_Bitshift;
    /**< @h2xmle_description {Q0 shift factor for smoothing double talk detection likelihood}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "3"}

         @h2xmle_policy       {Advanced} */

    int16_t aecCfg_aecSfCfg_filtAdpCfg_sTestSuccess[5];
    /**< @h2xmle_description {No. of test conditions to satisfy for allowing Mic 1 LEC download}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "2"}

         @h2xmle_policy       {Advanced} */

    int16_t aecCfg_farEndVadPFCfg_sHangoverMax;
    /**< @h2xmle_description {EC Postprocesing far-end VAD hangover in number of frames}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "5"}

         @h2xmle_policy       {Advanced} */

    int32_t aecCfg_aecSfCfg_filtAdpCfg_lAec_sc2_Q31[5];
    /**< @h2xmle_description {Adaptation stabilization factor for Mic 1 LEC adaptation}
         @h2xmle_default     {655360000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "655360000"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "655360000"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "655360000"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "21495808"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_saec_full_learn_frames_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow Mic 1 LEC download}
         @h2xmle_default     {300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg0_l_rate_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow faster Mic1 LEC adaptation}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_saec_full_learn_frames_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow Mic 2 LEC download}
         @h2xmle_default     {300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg1_l_rate_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow faster Mic2 LEC adaptation}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_saec_full_learn_frames_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow Mic 3 LEC download}
         @h2xmle_default     {300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg2_l_rate_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow faster Mic3 LEC adaptation}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_saec_full_learn_frames_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow Mic 4 LEC download}
         @h2xmle_default     {300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg3_l_rate_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow faster Mic4 LEC adaptation}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_saec_full_learn_frames_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow Mic 1 high band LEC download}
         @h2xmle_default     {300}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_cfg4_l_rate_L16_DT;
    /**< @h2xmle_description {Initial number of double talk frames to allow faster Mic1 high band LEC adaptation}
         @h2xmle_default     {4}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "4"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Basic} */

    int16_t post_gain_L16q11 [4];
    /**< @h2xmle_description {Q4.11 gain applied on Mic 1 signal after LEC processing}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t aec_LB_clipFlag_Al_BitShift;
    /**< @h2xmle_description {Q0 shift factor for smoothing Mic1 clipper detection likelihood}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t aec_HB_clipFlag_Al_BitShift;
    /**< @h2xmle_description {Q0 shift factor for smoothing Mic1 high band clipper detection threshold}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t EC_Ref_HPF_Enable;
    /**< @h2xmle_description {Q0 flag for enabling HPF on the EC reference path}
         @h2xmle_default     {0}
         @h2xmle_range       {0..1}


         @h2xmle_policy       {Advanced} */

    int16_t EC_Ref_HPF_Cutoff;
    /**< @h2xmle_description {Cutoff frequency of the HPF on the EC reference path}
         @h2xmle_default     {100}
         @h2xmle_range       {50..1000}


         @h2xmle_policy       {Basic} */

    int32_t aec_rx_channel_flag;
    /**< @h2xmle_description {Rx echo reference channel selection flag for stereo LEC configuration}
         @h2xmle_default     {3}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t aec_feature_mode;
    /**< @h2xmle_description {Feature selection flag to choose time domain LEC or Frequency domain LEC}
         @h2xmle_default     {0}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;



#define VOICE_PARAM_FLUENCE_NS_SPK 0x00010E36

#include "adsp_begin_pack.h"

typedef struct voice_param_fluence_ns_spk_t voice_param_fluence_ns_spk_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_NS_SPK", VOICE_PARAM_FLUENCE_NS_SPK}

    @h2xmlp_description  {Fluence version 5 broadside and Fluence Pro version 2 ECNS.
Maximum array size for this parameter:
Version 0  1064 bytes \n
Version 1 extension  1088 bytes \n
Version 2 extension  1224 bytes \n\n}
@h2xmlp_version     {2}  */

struct voice_param_fluence_ns_spk_t {

    uint16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion   {true}


         @h2xmle_readOnly       {true} */

    int16_t reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_readOnly       {true} */

    int32_t fp_VAD_flags;
    /**< @h2xmle_description {VAD Flags mode word for enabling different VADs in Fluence V5}
         @h2xmle_default     {529}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "529"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "529"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "529"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "513"}

         @h2xmle_policy       {Advanced} */

    int16_t smEnL16Q14;
    /**< @h2xmle_description {Smoothing factor for the input energy before single-mic VAD noise floor tracking (Higher value means more smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Basic} */

    int16_t snrThresDualL16Q8;
    /**< @h2xmle_description {Single-mic VAD threshold for IS in dual-mic mode}
         @h2xmle_default     {512}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "512"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "512"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "512"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1022"}

         @h2xmle_policy       {Basic} */

    int16_t snrThresSingleL16Q8;
    /**< @h2xmle_description {Single-mic VAD threshold for IS in single-mic mode}
         @h2xmle_default     {512}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "512"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "512"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "512"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1022"}

         @h2xmle_policy       {Advanced} */

    int16_t smVadHangoverMaxDualL16;
    /**< @h2xmle_description {Single-mic VAD hangover for IS in dual-mic mode (in frames)}
         @h2xmle_default     {5}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "5"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "5"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "19"}

         @h2xmle_policy       {Advanced} */

    int16_t smVadHangoverMaxSingleL16;
    /**< @h2xmle_description {Single-mic VAD hangover for IS in single-mic mode (in frames)}
         @h2xmle_default     {5}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "5"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "5"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "11"}

         @h2xmle_policy       {Advanced} */

    int16_t smVad_initAlphaL16Q15;
    /**< @h2xmle_description {Single-mic VAD input energy smoothing factor for noise floor tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smVad_uL16;
    /**< @h2xmle_description {Single-mic VAD noise floor tracking window length}
         @h2xmle_default     {8}
         @h2xmle_range       {1..12}


         @h2xmle_policy       {Advanced} */

    int16_t smVad_vL16;
    /**< @h2xmle_description {Single-mic VAD noise floor tracking sub-window length}
         @h2xmle_default     {12}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Basic} */

    int16_t smVad_mDL16Q15;
    /**< @h2xmle_description {Single-mic VAD noise floor tracking bias correction parameter}
         @h2xmle_default     {28672}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Basic} */

    int16_t smVad_mVL16Q15;
    /**< @h2xmle_description {Single-mic VAD noise floor tracking bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int32_t sigEnergyThresL32;
    /**< @h2xmle_description {Thermal noise floor threshold for Dir VAD update}
         @h2xmle_default     {3500}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t smMagAttSm21L16Q14;
    /**< @h2xmle_description {Attack time smoothing parameter for the bark scale magnitude used in Onset detection (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t smMagDecSm21L16Q14;
    /**< @h2xmle_description {Decay time smoothing parameter for the bark scale magnitude used in Onset detection (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smMagAttSm22L16Q14;
    /**< @h2xmle_description {Attack time smoothing parameter for the bark scale magnitude used in Offset detection (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smMagDecSm22L16Q14;
    /**< @h2xmle_description {Decay time smoothing parameter for the bark scale magnitude used in Offset detection (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t smDiffSm21L16Q14;
    /**< @h2xmle_description {Smoothing parameter for the difference of average magnitudes used in Onset detection (Higher value means less smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smDiffSm22L16Q14;
    /**< @h2xmle_description {Smoothing parameter for the difference of average magnitudes used in Offset detection (Higher value means less smoothing)}
         @h2xmle_default     {1638}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smScoreSm21L16Q14;
    /**< @h2xmle_description {Smoothing parameter for updating the final Onset VAD score (Higher value means less smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smScoreSm22L16Q14;
    /**< @h2xmle_description {Smoothing parameter for updating the final Offset VAD score (Higher value means less smoothing)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t bandSm21ThreshL16Q12[19];
    /**< @h2xmle_description {Onset VAD threshold for bark scale band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t bandSm22ThreshL16Q12[19];
    /**< @h2xmle_description {Offset VAD threshold for bark scale band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm21ThreshL16Q11;
    /**< @h2xmle_description {Onset VAD threshold}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "14000"}

         @h2xmle_policy       {Advanced} */

    int16_t sm22ThreshL16Q11;
    /**< @h2xmle_description {Offset VAD threshold}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "3000"}

         @h2xmle_policy       {Advanced} */

    int16_t hangoverMaxSm21L16;
    /**< @h2xmle_description {Onset VAD hangover duration (in frames)}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t hangoverMaxSm22L16;
    /**< @h2xmle_description {Offset VAD hangover duration (in frames)}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t sm21Norm1_initAlphaL16Q15;
    /**< @h2xmle_description {Onset VAD score smoothing factor for minima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm21Norm1_uL16;
    /**< @h2xmle_description {Onset VAD minima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "48"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "48"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "48"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "6"}

         @h2xmle_policy       {Advanced} */

    int16_t sm21Norm1_vL16;
    /**< @h2xmle_description {Onset VAD minima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "30"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "30"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "30"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "8"}

         @h2xmle_policy       {Basic} */

    int16_t sm21Norm1_mDL16Q15;
    /**< @h2xmle_description {Onset VAD minima tracking  bias correction parameter}
         @h2xmle_default     {26752}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm21Norm1_mVL16Q15;
    /**< @h2xmle_description {Onset VAD minima tracking  bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t sm21Norm2_initAlphaL16Q15;
    /**< @h2xmle_description {Onset VAD score smoothing factor for maxima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm21Norm2_uL16;
    /**< @h2xmle_description {Onset VAD maxima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}

         @h2xmle_policy       {Advanced} */

    int16_t sm21Norm2_vL16;
    /**< @h2xmle_description {Onset VAD maxima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "30"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "30"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "30"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "20"}

         @h2xmle_policy       {Basic} */

    int16_t sm21Norm2_mDL16Q15;
    /**< @h2xmle_description {Onset VAD maxima tracking  bias correction parameter}
         @h2xmle_default     {32604}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t sm21Norm2_mVL16Q15;
    /**< @h2xmle_description {Onset VAD maxima tracking  bias correction parameter-II}
         @h2xmle_default     {23101}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm22Norm1_initAlphaL16Q15;
    /**< @h2xmle_description {Offset VAD score smoothing factor for minima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t sm22Norm1_uL16;
    /**< @h2xmle_description {Offset VAD minima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "48"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "48"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "48"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "6"}

         @h2xmle_policy       {Advanced} */

    int16_t sm22Norm1_vL16;
    /**< @h2xmle_description {Offset VAD minima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}



         @h2xmle_policy       {Basic} */

    int16_t sm22Norm1_mDL16Q15;
    /**< @h2xmle_description {Offset VAD minima tracking  bias correction parameter}
         @h2xmle_default     {26752}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t sm22Norm1_mVL16Q15;
    /**< @h2xmle_description {Offset VAD minima tracking  bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm22Norm2_initAlphaL16Q15;
    /**< @h2xmle_description {Offset VAD score smoothing factor for maxima tracking (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t sm22Norm2_uL16;
    /**< @h2xmle_description {Offset VAD maxima tracking window length}
         @h2xmle_default     {48}
         @h2xmle_range       {1..48}


         @h2xmle_policy       {Advanced} */

    int16_t sm22Norm2_vL16;
    /**< @h2xmle_description {Offset VAD maxima tracking sub-window length}
         @h2xmle_default     {30}
         @h2xmle_range       {1..150}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "30"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "30"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "30"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "20"}

         @h2xmle_policy       {Basic} */

    int16_t sm22Norm2_mDL16Q15;
    /**< @h2xmle_description {Offset VAD maxima tracking  bias correction parameter}
         @h2xmle_default     {32604}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t sm22Norm2_mVL16Q15;
    /**< @h2xmle_description {Offset VAD maxima tracking  bias correction parameter-II}
         @h2xmle_default     {23101}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int32_t fp_nr_flags;
    /**< @h2xmle_description {NR Flags mode word for enabling different noise references in Fluence V5}
         @h2xmle_default     {-1073704384}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t debugFlags;
    /**< @h2xmle_description {Debug flags mode word for debugging noise references issues in Fluence V5}
         @h2xmle_default     {0}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t nRefMinStatCfg_initAlphaL16Q15;
    /**< @h2xmle_description {Minstat noise reference smoothing factor for primary mic noise floor tracking (Higher value means more smoothing)}
         @h2xmle_default     {27853}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t nRefMinStatCfg_uL16;
    /**< @h2xmle_description {Minstat noise reference noise floor tracking window length}
         @h2xmle_default     {12}
         @h2xmle_range       {1..12}



         @h2xmle_policy       {Advanced} */

    int16_t nRefMinStatCfg_vL16;
    /**< @h2xmle_description {Minstat noise reference noise floor tracking sub-window length}
         @h2xmle_default     {15}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t nRefMinStatCfg_mDL16Q15;
    /**< @h2xmle_description {Minstat noise reference noise floor tracking  bias correction parameter}
         @h2xmle_default     {28672}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t nRefMinStatCfg_mVL16Q15;
    /**< @h2xmle_description {Minstat noise reference noise floor tracking  bias correction parameter-II}
         @h2xmle_default     {19005}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t initIntervalVADNRefL16;
    /**< @h2xmle_description {Initial interval (in frames) for using the Init smoothing parameters to update the VAD based noise references}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t smNoiseInitDmicVADNRefL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during initial speech inactivity portions in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smNoiseLongInitDmicVADNRefL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during initial speech activity portions in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t smNoiseDmicVADNRefL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during speech inactivity portions in dual-mic mode (Higher value means mores smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smNoiseLongDmicVADNRefL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during speech activity portions in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smNoiseInitSmicVADNRefL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during initial speech inactivity portions in single-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smNoiseLongInitSmicVADNRefL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during initial speech activity portions in single-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smNoiseSmicVADNRefL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during speech inactivity portions in single-mic mode (Higher value means mores smoothing)}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smNoiseLongSmicVADNRefL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the VAD based noise reference during speech activity portions in single-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t initValVADNRefL16Q15;
    /**< @h2xmle_description {Parameter value for initializing the short term and long term noise estimates}
         @h2xmle_default     {819}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t overEstFactVADNRefL16Q13;
    /**< @h2xmle_description {Over-estimation factor (scaling factor) for the VAD based noise reference}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smNoiseActSpeechVADNRefL16Q15;
    /**< @h2xmle_description {Smoothing factor for switching from short term noise ref to long term noise ref during speech activity (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t smSmPlusNrefL16Q15;
    /**< @h2xmle_description {Smoothing factor for the IRM based noise reference (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t threshSmPlusL16Q15;
    /**< @h2xmle_description {SNR threshold for updating the IRM based noise reference}
         @h2xmle_default     {23100}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t overEstFactSmPlusNRefL16Q13;
    /**< @h2xmle_description {Over-estimation factor (scaling factor) for the IRM noise reference}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "8192"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "8192"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "8192"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "9830"}

         @h2xmle_policy       {Advanced} */

    int16_t nDevInitIntL16;
    /**< @h2xmle_description {Initial period in frames during which the noise deviation is not computed and noise deviation noise ref is set to be equal to the long term noise reference}
         @h2xmle_default     {9}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t nDevSmNDevL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the noise deviation estimate during speech inactivity portions}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t nDevSmNL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the long term noise estimate during speech inactivity portions}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t nDevSmNInitL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the long term noise estimate during initial period of speech inactivity portions}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t overEstFactNDevNRefL16Q13;
    /**< @h2xmle_description {Over-estimation factor (scaling factor) for the noise deviation based noise reference}
         @h2xmle_default     {9000}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "9000"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "9000"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "9000"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "9830"}

         @h2xmle_policy       {Advanced} */

    int16_t nDevNRefDecayActiveL16Q15;
    /**< @h2xmle_description {Decaying factor for updating the noise deviation estimate and long term noise estimate during speech activity portions}
         @h2xmle_default     {32700}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "32700"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "32700"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "32700"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "32440"}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPR1LbIdxL16;
    /**< @h2xmle_description {Lower bound frequency index of the R1 band  for SNR PP processing}
         @h2xmle_default     {16}
         @h2xmle_range       {0..256}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPR2LbIdxL16;
    /**< @h2xmle_description {Lower bound frequency index of the R2 band  for SNR PP processing}
         @h2xmle_default     {65}
         @h2xmle_range       {0..256}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPR3LbIdxL16;
    /**< @h2xmle_description {Lower bound frequency index of the R3 band  for SNR PP processing}
         @h2xmle_default     {128}
         @h2xmle_range       {0..256}



         @h2xmle_policy       {Basic} */

    int16_t snrPPMinAggR0L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for  the R0 frequency band}
         @h2xmle_default     {13500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "13500"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "13500"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "13500"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "8192"}

         @h2xmle_policy       {Basic} */

    int16_t snrPPMaxAggR0L16Q12;
    /**< @h2xmle_description {Maximum aggressiveness control for the R0 frequency band}
         @h2xmle_default     {18432}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggSlopeR0L16Q10;
    /**< @h2xmle_description {Aggressiveness slope control for the R0 frequency band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "16008"}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggOffsetR0L16Q11;
    /**< @h2xmle_description {Aggressiveness offset control for the R0 frequency band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "-22793"}

         @h2xmle_policy       {Basic} */

    int16_t snrPPMinAggR1L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for  the R1 frequency band}
         @h2xmle_default     {12500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "12500"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "12500"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "12500"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "8602"}

         @h2xmle_policy       {Basic} */

    int16_t snrPPMaxAggR1L16Q12;
    /**< @h2xmle_description {Maximum aggressiveness control for the R1 frequency band}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggSlopeR1L16Q11;
    /**< @h2xmle_description {Aggressiveness slope control for the R1 frequency band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "7314"}

         @h2xmle_policy       {Basic} */

    int16_t snrPPMinAggR3L16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for  the R3 frequency band}
         @h2xmle_default     {11500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "11500"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "11500"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "11500"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "6144"}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggSlopeR3L16Q11;
    /**< @h2xmle_description {Aggressiveness slope control for the R3 frequency band}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "9092"}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPIntDecayFactorL16Q15;
    /**< @h2xmle_description {Decay time smoothing parameter for the SNR-PP gain update}
         @h2xmle_default     {19661}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPIntDeltaOffsetL16Q14;
    /**< @h2xmle_description {Multiplication factor for the current gain estimate in SNR-PP}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPIntDeltaMultL16Q13;
    /**< @h2xmle_description {Delta offset for the current gain estimate in SNR-PP}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPMinGainL16Q14;
    /**< @h2xmle_description {Target noise reduction (minimum gain) to be achieved with the SNR-PP module when IS is in dual-mic mode}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "256"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "256"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "256"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1638"}

         @h2xmle_policy       {Advanced} */

    int16_t relaxLfPpL16Q14;
    /**< @h2xmle_description {Relaxing factor (averaging factor) for the SNR-pp gain in lower frequencies}
         @h2xmle_default     {15565}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "15565"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "15565"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "15565"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t smPPGainAttL16Q15;
    /**< @h2xmle_description {Attack time smoothing parameter for the SNR-PP gain (Higher value means more smoothing)}
         @h2xmle_default     {6400}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "6400"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "6400"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "6400"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t smPPGainDecL16Q15;
    /**< @h2xmle_description {Decay time smoothing parameter for the SNR-PP gain (Higher value means more smoothing)}
         @h2xmle_default     {8000}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "8000"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "8000"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "8000"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t smCombGainAttL16Q15;
    /**< @h2xmle_description {Attack time smoothing parameter for the combined gain (Higher value means more smoothing)}
         @h2xmle_default     {3277}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t smCombGainDecL16Q15;
    /**< @h2xmle_description {Decay time smoothing parameter for the combined gain (Higher value means more smoothing)}
         @h2xmle_default     {3500}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smLogSnrLtmSnrL16Q15;
    /**< @h2xmle_description {smLogSnrLtmSnrL16Q15}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t smMagLtmSnrL16Q15;
    /**< @h2xmle_description {smMagLtmSnrL16Q15}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t minLogSnrL16Q13;
    /**< @h2xmle_description {minLogSnrL16Q13}
         @h2xmle_default     {-16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t minBandLogSnrLbIdxL16;
    /**< @h2xmle_description {minBandLogSnrLbIdxL16}
         @h2xmle_default     {16}
         @h2xmle_range       {0..257}



         @h2xmle_policy       {Advanced} */

    int16_t minBandLogSnrUbIdxL16;
    /**< @h2xmle_description {minBandLogSnrUbIdxL16}
         @h2xmle_default     {64}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int32_t minNEngNtmL32Q31;
    /**< @h2xmle_description {minNEngNtmL32Q31}
         @h2xmle_default     {214748}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t smNtmActL16Q15;
    /**< @h2xmle_description {smNtmActL16Q15}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smNtmInactL16Q15;
    /**< @h2xmle_description {smNtmInactL16Q15}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t ntdThresL16Q15;
    /**< @h2xmle_description {ntdThresL16Q15}
         @h2xmle_default     {12000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smFinalGainAttSmL16Q15;
    /**< @h2xmle_description {Attack time smoothing of the final gain during single-mic mode or non-flat noise in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smFinalGainDecSmL16Q15;
    /**< @h2xmle_description {Decay time smoothing of the final gain during single-mic mode or non-flat noise in dual-mic mode (Higher value means more smoothing)}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved1;
    /**< @h2xmle_description {reserved1}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int32_t offsetPowDualWBL32Q25;
    /**< @h2xmle_description {offsetPowDualWBL32Q25}
         @h2xmle_default     {-603979776}
         @h2xmle_range       {-2147483648..2147483647}



         @h2xmle_policy       {Advanced} */

    int32_t offsetPowDualNBL32Q25;
    /**< @h2xmle_description {offsetPowDualNBL32Q25}
         @h2xmle_default     {-553648128}
         @h2xmle_range       {-2147483648..2147483647}



         @h2xmle_policy       {Advanced} */

    int32_t offsetPowSingleWBL32Q25;
    /**< @h2xmle_description {offsetPowSingleWBL32Q25}
         @h2xmle_default     {-553648128}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t offsetPowSingleNBL32Q25;
    /**< @h2xmle_description {offsetPowSingleNBL32Q25}
         @h2xmle_default     {-553648128}
         @h2xmle_range       {-2147483648..2147483647}
         @h2xmle_policy       {Advanced} */

    int16_t targetNoiseFloorL16Q15;
    /**< @h2xmle_description {NS CNI level for setting the target noise floor (in freq domain)}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t slopeNoiseFloorL16Q15;
    /**< @h2xmle_description {Spectral slope (dB/Hz) of the injected comfort noise for NS}
         @h2xmle_default     {32416}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t lbNRefConstraintL16;
    /**< @h2xmle_description {Lower bound frequency index above which noise ref can be forced to not exceed Mic1 input level}
         @h2xmle_default     {255}
         @h2xmle_range       {0..256}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "255"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "255"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "255"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Advanced} */

    int16_t thresBoostHighNL16Q12;
    /**< @h2xmle_description {Noise level threshold for the noise ref magnitude to determine if the high frequency output (Spec deviation score) should be boosted in high noise or not}
         @h2xmle_default     {12000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smStmMagL16Q15;
    /**< @h2xmle_description {Smoothing factor for the noise reference magnitude used in long-term SNR estimation and high noise output boosting (Higher value means more smoothing)}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smGainAttBoostHighNL16Q15;
    /**< @h2xmle_description {Attack time smoothing parameter for the high noise gain update (Higher value means more smoothing)}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smGainDecBoostHighNL16Q15;
    /**< @h2xmle_description {Release time smoothing parameter for the high noise gain (Higher value means more smoothing)}
         @h2xmle_default     {31130}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t gainBoostHighNL16Q12;
    /**< @h2xmle_description {Gain factor for boosting the output level during high noise}
         @h2xmle_default     {6000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t thresHFBoostHighNL16Q12;
    /**< @h2xmle_description {Noise level threshold for the noise ref magnitude to determine if the high frequency output (and Spec deviation score) should be boosted in high noise or not}
         @h2xmle_default     {29464}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t hangoverMaxHFBoostHighNL16;
    /**< @h2xmle_description {Hangover period for boosting the high frequency output level during high noise}
         @h2xmle_default     {150}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t bandThresHFBoostHighNL16[4];
    /**< @h2xmle_description {Lower bound Frequency index of the bands}
         @h2xmle_default     {45}
         @h2xmle_range       {0..257}

         @h2xmle_policy       {Advanced} */

    int16_t bandGainHFBoostHighNL16Q12[5];
    /**< @h2xmle_description {Gain factor to boost in the band during high noise situations}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t alphaL16Q15;
    /**< @h2xmle_description {Fixed scale factor for Mic1 to be used in HPS VAD computations}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t alpha2L16Q12;
    /**< @h2xmle_description {Fixed scale factor for (Mic2 - alpha*Mic1) to be used in HPS VAD computations}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t alpha3L16Q15;
    /**< @h2xmle_description {Fixed scale factor for Mic2 before computing the IMS level difference for IS update}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t lbRatioIIL16;
    /**< @h2xmle_description {Lower bound of frequency index for computing the IMS level difference used in IS update}
         @h2xmle_default     {7}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ubRatioIIL16;
    /**< @h2xmle_description {Upper bound of frequency index for computing the IMS level difference used in IS update}
         @h2xmle_default     {29}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t ub_pit_rangeL16;
    /**< @h2xmle_description {Upper bound of frequency index to search for the pitch frequency}
         @h2xmle_default     {20}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t pit_buffer_sizeL16;
    /**< @h2xmle_description {Buffer size for holding the pitch buffer used in HPS VAD computation}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t pit_hist_initL16;
    /**< @h2xmle_description {Pitch Frequency index to initialize the pitch buffer used in HPS VAD computation}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t pit_hist_thresholdL16;
    /**< @h2xmle_description {Threshold for pitch frequency histogram used in HPS VAD computation}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t harm_uL16;
    /**< @h2xmle_description {Minstat block window size for harmonicity tracking and normalization}
         @h2xmle_default     {12}
         @h2xmle_range       {1..12}


         @h2xmle_policy       {Advanced} */

    int16_t harm_vL16;
    /**< @h2xmle_description {Minstat block window counts for harmonicity tracking and normalization}
         @h2xmle_default     {15}
         @h2xmle_range       {1..100}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "15"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "15"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "15"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "10"}

         @h2xmle_policy       {Advanced} */

    int16_t harm_initAlphaL16Q15;
    /**< @h2xmle_description {Smoothing parameter for harmonicity power estimates before performing minstat tracking}
         @h2xmle_default     {24576}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t norm_flag;
    /**< @h2xmle_description {Flag for enabling the normalization of Harmonicity product sum (HPS) used in HPS Vad}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t final_confidence_smL16;
    /**< @h2xmle_description {Smoothing factor for  Harmonicty VAD final confidence statistic (Higher value means more smoothing)}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Basic} */

    int16_t hps_flags;
    /**< @h2xmle_description {Second NR Flags (Flag2) mode word for enabling new functionalities such as AGM, peak restoration, high noise boost etc. in Fluence V5}
         @h2xmle_default     {24576}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t level_diffL16;
    /**< @h2xmle_description {Threshold for the level diff VAD used in HPS computation}
         @h2xmle_default     {26214}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t ub_binL16;
    /**< @h2xmle_description {Upper bound of frequency index used in the level diff VAD computation}
         @h2xmle_default     {128}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t lb_binL16;
    /**< @h2xmle_description {Lower bound of frequency index used in the level diff VAD computation}
         @h2xmle_default     {0}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t harm_thresoldL16;
    /**< @h2xmle_description {Threshold for capturing pitch peaks - higher means less sensitive HPS VAD}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t mask_gain_smoothingL16;
    /**< @h2xmle_description {Smoothing factor for the Masking gain used in the HPS NS}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t max_level_diffL16;
    /**< @h2xmle_description {Maximum bound for the level difference (between the two mics) above which the HPS NS gain is set to unity}
         @h2xmle_default     {-10000}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t min_level_diffL16;
    /**< @h2xmle_description {Minimum bound for the level difference (between the two mics) below which the HPS NS gain is set to the gain floor.}
         @h2xmle_default     {-14000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t hang_over_limitL16;
    /**< @h2xmle_description {Hangover period for the HPS NS gain}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t snrPPMinAggR0FlatL16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for  the R0 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {11000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPMaxAggR0FlatL16Q12;
    /**< @h2xmle_description {Maximum aggressiveness control for the R0 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {18432}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggSlopeR0FlatL16Q10;
    /**< @h2xmle_description {Aggressiveness slope control for the R0 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {16008}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggOffsetR0FlatL16Q11;
    /**< @h2xmle_description {Aggressiveness offset control for the R0 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {-24793}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPMinAggR1FlatL16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for  the R1 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPMaxAggR1FlatL16Q12;
    /**< @h2xmle_description {Maximum aggressiveness control for the R1 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {18432}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggSlopeR1FlatL16Q11;
    /**< @h2xmle_description {Aggressiveness slope control for the R1 frequency band for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {7314}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPMinAggR3FlatL16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for  the R3 frequency band  for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggSlopeR3FlatL16Q11;
    /**< @h2xmle_description {Aggressiveness slope control for the R3 frequency band  for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {9092}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Basic} */

    int16_t snrPPMinGainFlatL16Q14;
    /**< @h2xmle_description {Target noise reduction (minimum gain) to be achieved with the SNR-PP module when IS is in dual-mic mode for Flat noise (Tuning NOT needed)}
         @h2xmle_default     {1000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggR0SmL16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for  the R0 frequency band for IS in single-mic mode or single-mic Fluence V5}
         @h2xmle_default     {13500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "13500"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "13500"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "13500"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "7000"}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggR1SmL16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for  the R1 frequency band for IS in single-mic mode or single-mic Fluence V5}
         @h2xmle_default     {12500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "12500"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "12500"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "12500"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "8192"}

         @h2xmle_policy       {Advanced} */

    int16_t snrPPAggR3SmL16Q12;
    /**< @h2xmle_description {Minimum aggressiveness control for  the R3 frequency band for IS in single-mic mode or single-mic Fluence V5}
         @h2xmle_default     {11500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "11500"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "11500"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "11500"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "8192"}

         @h2xmle_policy       {Basic} */

    int16_t snrPPMinGainSmL16Q14;
    /**< @h2xmle_description {Target noise reduction (minimum gain) to be achieved with the SNR-PP module when IS is in single-mic mode or for single-mic Fluence V5}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "256"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "256"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "256"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "3000"}

         @h2xmle_policy       {Advanced} */

    int32_t pit_confidence_thresholdL32;
    /**< @h2xmle_description {Threshold for harmonicity based pitch estimation - Higher is less sensitive}
         @h2xmle_default     {193273283}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t harm_minL32Q24;
    /**< @h2xmle_description {Min bound for the harmonicity statistic to be used in the HPS VAD computation}
         @h2xmle_default     {-550000000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t harm_maxL32Q24;
    /**< @h2xmle_description {Max bound for the harmonicity statistic to be used in the HPS VAD computation}
         @h2xmle_default     {-270000000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t harm_vad_thresholdL32;
    /**< @h2xmle_description {Threshold for the harmonicity VAD}
         @h2xmle_default     {1019000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t normalization_minL32;
    /**< @h2xmle_description {Minimum normalization factor to be used for normalizing the HPS in HPS VAD computation}
         @h2xmle_default     {40000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t suppression_gainL32;
    /**< @h2xmle_description {Suppression gain for the HPS NS}
         @h2xmle_default     {536870912}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t gain_floorL32;
    /**< @h2xmle_description {HPS NS gain is set to gain floor if the level difference is less than the lower bound}
         @h2xmle_default     {536870912}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t gain_floorNBL32;
    /**< @h2xmle_description {gain_floorNBL32}
         @h2xmle_default     {336870912}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t gain_floorWBL32;
    /**< @h2xmle_description {gain_floorWBL32}
         @h2xmle_default     {1036870912}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t nCalLbIdxL16;
    /**< @h2xmle_description {Lower bound of frequency index for calculating the IMS calibration}
         @h2xmle_default     {10}
         @h2xmle_range       {0..257}

         @h2xmle_policy       {Advanced} */

    int16_t nCalUbIdxL16;
    /**< @h2xmle_description {Upper bound of frequency index for calculating the IMS calibration}
         @h2xmle_default     {44}
         @h2xmle_range       {0..257}

         @h2xmle_policy       {Advanced} */

    int16_t engThresNCalL16;
    /**< @h2xmle_description {Upper bound energy threshold for the primary mic energy  in order to update the noise calibration for the IMS ratio computation}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t initIntNCalL16;
    /**< @h2xmle_description {Initial period in number of frames during which the smOverallNoiseCalInitL16Q15 smoothing parameter is used for IMS cal update}
         @h2xmle_default     {500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smBandNoiseCalL16Q15;
    /**< @h2xmle_description {Smoothing factor for updating the instantaneous IMS calibration factors in different frequency bands (Higher value means mores smoothing)}
         @h2xmle_default     {32604}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smOverallNoiseCalInitL16Q15;
    /**< @h2xmle_description {Initial overall smoothing factor for updating the instantaneous IMS calibration factor}
         @h2xmle_default     {31130}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smOverallNoiseCalL16Q15;
    /**< @h2xmle_description {Overall smoothing factor for updating the instantaneous IMS calibration factor that is applied on the second mic energy}
         @h2xmle_default     {32604}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t overEstFactorNCalHpsL16Q14;
    /**< @h2xmle_description {Over-estimation factor for the second mic calibration in the IMS ratio computation}
         @h2xmle_default     {17896}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t ims_flags;
    /**< @h2xmle_description {Mode word for controlling IMS ratio processing}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Basic} */

    int16_t hps_nr_flags;
    /**< @h2xmle_description {Mode word for controlling different sub-modules inside the HPS NS processing}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t startSmL16;
    /**< @h2xmle_description {Lower bound  index of the frequency range over which bark scale smoothing should be applied}
         @h2xmle_default     {46}
         @h2xmle_range       {0..257}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "46"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "46"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "46"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Basic} */

    int16_t agm_num_chan;
    /**< @h2xmle_description {Number of microphone channels to configure in AGM processing}
         @h2xmle_default     {2}
         @h2xmle_range       {1..4}


         @h2xmle_policy       {Basic} */

    int16_t agm_feature;
    /**< @h2xmle_description {Feature mode to configure AGM processing}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_data_width;
    /**< @h2xmle_description {Bit-width information for AGM processing}
         @h2xmle_default     {16}
         @h2xmle_range       {0..32}


         @h2xmle_policy       {Basic} */

    int16_t agm_feFlag_cfg;
    /**< @h2xmle_description {Far-end flag enablement for AGM processing}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_mode;
    /**< @h2xmle_description {Mode word for AGM processing}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int32_t agm_power_thresh_4nflr_L32Q20;
    /**< @h2xmle_description {Q20 Noise floor threshold for updating power estimate in AGM processing}
         @h2xmle_default     {1024}
         @h2xmle_range       {50..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t agm_snr_low_thresh_L16Q10;
    /**< @h2xmle_description {Q10 Low bound for SNR estimation in AGM processing}
         @h2xmle_default     {1500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_snr_high_thresh_L16Q10;
    /**< @h2xmle_description {Q10 High bound for SNR estimation in AGM processing}
         @h2xmle_default     {7000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t agm_eig_ratio_thresh;
    /**< @h2xmle_description {Threshold for Microphone level ratio computation in AGM processing}
         @h2xmle_default     {1000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t agm_gain_variation_indB_L16Q9;
    /**< @h2xmle_description {Q9 AGM gain variation in dB}
         @h2xmle_default     {920}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t agm_hist_flush_rate;
    /**< @h2xmle_description {AGM histogram buffer flushing rate}
         @h2xmle_default     {-15536}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t agm_bin_cnt_dym_thresh_max;
    /**< @h2xmle_description {Max counter threshold for AGM processing}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t agm_bin_cnt_dym_thresh_min;
    /**< @h2xmle_description {Min counter threshold for AGM processing}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t agm_scVAD_blklen;
    /**< @h2xmle_description {Block length for AGM single channel VAD noise floor estimation}
         @h2xmle_default     {20}
         @h2xmle_range       {1..100}

         @h2xmle_policy       {Advanced} */

    int16_t agm_scVAD_numblk;
    /**< @h2xmle_description {Number of blocks used in AGM single-channel VAD noise floor estimation}
         @h2xmle_default     {20}
         @h2xmle_range       {1..100}

         @h2xmle_policy       {Advanced} */

    int16_t agm_nfl_blklen;
    /**< @h2xmle_description {Block length for AGM noise floor estimation}
         @h2xmle_default     {10}
         @h2xmle_range       {1..100}

         @h2xmle_policy       {Advanced} */

    int16_t agm_nfl_numblk;
    /**< @h2xmle_description {Number of blocks used in AGM noise floor estimation}
         @h2xmle_default     {10}
         @h2xmle_range       {1..100}

         @h2xmle_policy       {Advanced} */

    int16_t proximityRatioL16;
    /**< @h2xmle_description {Dual-mic proximity ratio used in HPS IS decision making}
         @h2xmle_default     {13107}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int32_t restorationRatioL32;
    /**< @h2xmle_description {Restoration ratio for allowing IS switching in cleaner conditions}
         @h2xmle_default     {1288490188}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t pitch_confidence_flag_high_thresholdL32;
    /**< @h2xmle_description {Higher confidence threshold for pitch computation in HPS processing}
         @h2xmle_default     {805306368}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t pitch_confidence_flag_low_thresholdL32;
    /**< @h2xmle_description {Lower confidence threshold for pitch computation in HPS processing}
         @h2xmle_default     {322120000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t harmonicity_offsetL32Q24;
    /**< @h2xmle_description {Frequency offset in harmonicity computation in HPS processing}
         @h2xmle_default     {50331645}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t nonspeech_peak_count_thresholdL16;
    /**< @h2xmle_description {Threshold count for nonspeech detection}
         @h2xmle_default     {10}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t min_peak_distanceL16;
    /**< @h2xmle_description {Minimum separation between harmonic peaks}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t peak_threshold_exp1L16;
    /**< @h2xmle_description {Threshold1 for harmonic peak estimation}
         @h2xmle_default     {-5}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t peak_threshold_exp2L16;
    /**< @h2xmle_description {Threshold2 for harmonic peak estimation}
         @h2xmle_default     {-2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t nonspeech_peak_relative_gainL16;
    /**< @h2xmle_description {Counter for harmonic peaks corresponding to non speech}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t scalebackL16Q15;
    /**< @h2xmle_description {Gain scale back employed in HPS processing}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t HFC_thresholdL32;
    /**< @h2xmle_description {Speech restoration threshold in HPS processing}
         @h2xmle_default     {20000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_low_thresholdL32Q13;
    /**< @h2xmle_description {Low SNR threshold for speech restoration in HPS processing}
         @h2xmle_default     {37683}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_low_step_thresholdL32Q13;
    /**< @h2xmle_description {Low SNR step size for speech restortion in HPS processing}
         @h2xmle_default     {22118}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_high_thresholdL32Q13;
    /**< @h2xmle_description {High SNR threshold for speech restoration in HPS processing}
         @h2xmle_default     {81920}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_high_step_thresholdL32Q13;
    /**< @h2xmle_description {High SNR step size for speech restortion in HPS processing}
         @h2xmle_default     {10240}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_onset_low_thresholdL32Q13;
    /**< @h2xmle_description {Lower threshold factor for speech onset estimation in HPS processing}
         @h2xmle_default     {113049}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_onset_low_step_thresholdL32Q13;
    /**< @h2xmle_description {Lower Step size for speech onset estimation in HPS processing}
         @h2xmle_default     {4146}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_onset_high_thresholdL32Q13;
    /**< @h2xmle_description {Higher threshold factor for speech onset estimation in HPS processing}
         @h2xmle_default     {245760}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SNR_onset_high_step_thresholdL32Q13;
    /**< @h2xmle_description {Higher Step size for speech onset estimation in HPS processing}
         @h2xmle_default     {11520}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t harmonic_structure_thresholdL32Q30;
    /**< @h2xmle_description {Threshold for harmonic peak computation in HPS processing}
         @h2xmle_default     {536870912}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t restoration_ratio_thresholdL32;
    /**< @h2xmle_description {Ratio for speech restoration computation in HPS processing}
         @h2xmle_default     {858990000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t smrmt_mode;
    /**< @h2xmle_description {Mode word for single-mic SMRMT noise reference estimation}
         @h2xmle_default     {14}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "14"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "14"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "14"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "30"}

         @h2xmle_policy       {Advanced} */

    int16_t smrmt_wlenMax;
    /**< @h2xmle_description {Window length for SMRMT noise reference estimation}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "50"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "50"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "50"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "35"}

         @h2xmle_policy       {Basic} */

    int32_t smrmt_thrB1;
    /**< @h2xmle_description {Band 1 speech energy threshold employed in SMRMT noise reference estimation}
         @h2xmle_default     {250000000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "250000000"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "250000000"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "250000000"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "210000000"}

         @h2xmle_policy       {Basic} */

    int32_t smrmt_thrB2;
    /**< @h2xmle_description {Band 2 speech energy threshold employed in SMRMT noise reference estimation}
         @h2xmle_default     {200000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t smrmt_thrB3;
    /**< @h2xmle_description {Band 3 speech energy threshold employed in SMRMT noise reference estimation}
         @h2xmle_default     {150000000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t smrmt_band1I;
    /**< @h2xmle_description {Frequency bin upper bound for Band 1 in SMRMT processing}
         @h2xmle_default     {48}
         @h2xmle_range       {0..257}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "48"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "48"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "48"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "24"}

         @h2xmle_policy       {Advanced} */

    int16_t smrmt_band2I;
    /**< @h2xmle_description {Frequency bin upper bound for Band 2 in SMRMT processing}
         @h2xmle_default     {128}
         @h2xmle_range       {0..257}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "128"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "128"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "128"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "64"}

         @h2xmle_policy       {Advanced} */

    int16_t smrmt_alphaP;
    /**< @h2xmle_description {Smoothing factor for voice estimate computation in SMRMT processing}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smrmt_alphaD;
    /**< @h2xmle_description {Smoothing factor for noise reference computation in SMRMT processing}
         @h2xmle_default     {27852}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smrmt_overest_factL16Q12;
    /**< @h2xmle_description {Q12 over-estimation factor for SMRMT noise reference}
         @h2xmle_default     {5500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "5500"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5500"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "5500"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "5300"}

         @h2xmle_policy       {Advanced} */

    int16_t smrmt_vad_thre_dm;
    /**< @h2xmle_description {Dual-mic VAD threshold for voice estimate computation in SMRMT processing}
         @h2xmle_default     {1000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t smrmt_vad_thre_sm;
    /**< @h2xmle_description {Single-mic VAD threshold for voice estimate computation in SMRMT processing}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t restNRefAttnL16;
    /**< @h2xmle_description {Noise reference attenuation shift factor for speech restoration}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t smTotalBoostLInd;
    /**< @h2xmle_description {Starting frequency index for speech boost processing}
         @h2xmle_default     {7}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t smTotalBoost;
    /**< @h2xmle_description {Speech gain boost in single-mic NS processing}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "8192"}

         @h2xmle_policy       {Advanced} */

    int16_t smBoostLInd;
    /**< @h2xmle_description {Starting frequency index for slope filter in speech boost procesing}
         @h2xmle_default     {64}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t smBoostLEnd;
    /**< @h2xmle_description {Ending frequency index for slope filter in speech boost procesing}
         @h2xmle_default     {128}
         @h2xmle_range       {0..257}


         @h2xmle_policy       {Advanced} */

    int16_t smBoostRatioL16Q14;
    /**< @h2xmle_description {Gain ratio for slope filter in speech boost processing}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t snrPPMinGainLbL16Q14;
    /**< @h2xmle_description {Min gain for low frequency band in the NS post-processing}
         @h2xmle_default     {128}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_flags;
    /**< @h2xmle_description {Mode word for configuring FluencePro spatial filtering}
         @h2xmle_default     {40}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "40"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "40"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "40"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "8744"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_del_ang;
    /**< @h2xmle_description {Angular resolution for FluencePro directionality estimation}
         @h2xmle_default     {6}
         @h2xmle_range       {0..6}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "6"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "6"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "6"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "3"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_sz_ang;
    /**< @h2xmle_description {Number of angular bins to employ in directionality estimation}
         @h2xmle_default     {31}
         @h2xmle_range       {0..0x3D}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_start_cutL16;
    /**< @h2xmle_description {Starting frequency index for directionality estimation}
         @h2xmle_default     {1}
         @h2xmle_range       {0..126}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_sum_cutL16;
    /**< @h2xmle_description {Ending frequency index for directionality estimation}
         @h2xmle_default     {80}
         @h2xmle_range       {2..128}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "80"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "80"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "80"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_startMask_cutL16;
    /**< @h2xmle_description {Starting frequency index for FluencePro spatial filter II processing}
         @h2xmle_default     {1}
         @h2xmle_range       {0..126}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_endMask_cutL16;
    /**< @h2xmle_description {Ending frequency index for FluencePro spatial filter II processing}
         @h2xmle_default     {80}
         @h2xmle_range       {2..128}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "80"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "80"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "80"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "100"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_mic_distance1L16[2];
    /**< @h2xmle_description {Microphone spacing for the mic pair of the first set}
         @h2xmle_default     {20971}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "20971"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "20971"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "20971"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "18350"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_mic_distance2L16[2];
    /**< @h2xmle_description {Microphone spacing for the first mic pair of the second set}
         @h2xmle_default     {20316}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "20316"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "20316"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "20316"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "32767"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_mic_distanceQfactor1L16[2];
    /**< @h2xmle_description {Q-factor for Microphone spacing for the  mic pair of the first set}
         @h2xmle_default     {0xFFFE}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_mic_distanceQfactor2L16[2];
    /**< @h2xmle_description {Q-factor for Microphone spacing for the  mic pair of the second set}
         @h2xmle_default     {0xFFFD}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_pair_channel1L16[4];
    /**< @h2xmle_description {Mic channel for configuring  channel pair of first mic set}
         @h2xmle_default     {1}
         @h2xmle_range       {0..3}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_pair_channel2L16[4];
    /**< @h2xmle_description {Mic channel for configuring second channel pair of second mic set}
         @h2xmle_default     {3}
         @h2xmle_range       {0..3}


         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_offset_cos_sqL32Q30;
    /**< @h2xmle_description {Offest for DOA estimation in FluencePro processing}
         @h2xmle_default     {100}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_var_regL32Q31;
    /**< @h2xmle_description {Regularization factor employed in DOA estimation}
         @h2xmle_default     {21474836}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_freqbinL16Q10;
    /**< @h2xmle_description {Inverse frequency bin factor employed in DOA Estimation}
         @h2xmle_default     {32000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_forget_factorL16Q15;
    /**< @h2xmle_description {Smoothing factor for DoA tracking}
         @h2xmle_default     {29491}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_vad_quadrantL32Q2;
    /**< @h2xmle_description {VAD scaling factor employed in Anglogram updates}
         @h2xmle_default     {500}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_sect[4];
    /**< @h2xmle_description {Beginning angle for Sectors in Anglogram processing}
         @h2xmle_default     {45}
         @h2xmle_range       {0..400}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "45"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "45"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "45"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "70"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_ec_sec;
    /**< @h2xmle_description {Sector containing loud speaker echo direction}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t reserved2;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true}*/

    int16_t SPKPhoneCfg_target_flag;
    /**< @h2xmle_description {Flag to configure fixed target employed in DOA estimation}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_target_margin;
    /**< @h2xmle_description {Margin for target located in the sector boundary}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_set_threshold[2];
    /**< @h2xmle_description {Threshold  for sensor based switching in FluencePro processing}
         @h2xmle_default     {300000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_set_flag;
    /**< @h2xmle_description {Flag to enable sensor based processing in FluencePro NS}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_smoothing_filterL16Q15[5];
    /**< @h2xmle_description {Smoothing filter for filling gaps in DOA Estimation}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_GUI_qfactor;
    /**< @h2xmle_description {Smoothing factor for the source traking plot in GUI display}
         @h2xmle_default     {-4}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_hangover;
    /**< @h2xmle_description {Hangover for the DOA vad in FluencePro processing}
         @h2xmle_default     {5}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "5"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "5"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "3"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_target_sec_en[4];
    /**< @h2xmle_description {Enablement flag for Sectors in FluencePro Processing}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_ang_margin[2];
    /**< @h2xmle_description {Angle margin  for DOA tilt estimation}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_set_number;
    /**< @h2xmle_description {Default mic set to use at the beginning of FluencePro processing}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_set_threshold_zL16Q11;
    /**< @h2xmle_description {Threshold for mic set switching}
         @h2xmle_default     {6144}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_sz_pair;
    /**< @h2xmle_description {Number of mic pairs to use in FluencePro NS processing}
         @h2xmle_default     {2}
         @h2xmle_range       {0..2}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_sector_buffer_sizeL16;
    /**< @h2xmle_description {Buffer size to use in target sector switching in FluencePro processing}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_sector_buffer_size2L16;
    /**< @h2xmle_description {Buffer size for IS processing in FluencePro NS}
         @h2xmle_default     {10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_sector_hist_initL16;
    /**< @h2xmle_description {Initialization value for histogram processing in target sector estimation}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_sector_hist_max;
    /**< @h2xmle_description {Threshold for histrogram processing in target sector estimation}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_stationarity_threL16;
    /**< @h2xmle_description {Number of frames to use in Noise session tracking}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_nonStatThreL16;
    /**< @h2xmle_description {Threshold for histrogram processing in noise session estimation}
         @h2xmle_default     {-320}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_dirVadallMultL16;
    /**< @h2xmle_description {Threshold for directionality VAD in FluencePro processing}
         @h2xmle_default     {10000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "10000"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "10000"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "10000"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "24576"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_dirVadallMultQL16;
    /**< @h2xmle_description {Qfactor of the threshold for directionality VAD in FluencePro processing}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_vadMaxThreL16;
    /**< @h2xmle_description {Max Factor threshold employed in directionality VAD computation}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_harmonicityThreL32;
    /**< @h2xmle_description {Normalized harmonicity threshold for directionality VAD computation}
         @h2xmle_default     {2147483647}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "2147483647"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "2147483647"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "2147483647"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1500000000"}

         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_restRatioThreL32;
    /**< @h2xmle_description {Restoration ratio employed in directionality VAD computation}
         @h2xmle_default     {2147483647}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "2147483647"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "2147483647"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "2147483647"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1288490188"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_dirVadSectMultL16;
    /**< @h2xmle_description {Threshold for target sector switching processing in FluencePro NS}
         @h2xmle_default     {28672}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_dirVadSectMultQL16;
    /**< @h2xmle_description {Q factor of the Threshold for target sector switching processing in FluencePro NS}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_harmonicityHistThreL32;
    /**< @h2xmle_description {Normalized harmonicity threshold for the conservative VAD in FluencePro NS}
         @h2xmle_default     {-2147483647}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_harmonicitySectThreL32;
    /**< @h2xmle_description {Normalized harmonicity threshold for the histogram processing in FluencePro NS}
         @h2xmle_default     {-2147483648}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "-2147483648"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "-2147483648"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "-2147483648"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1500000000"}

         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_restRatioSectThreL32;
    /**< @h2xmle_description {Restoration ratio threshold for target sector switching in FluencePro NS}
         @h2xmle_default     {-2147483648}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "-2147483648"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "-2147483648"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "-2147483648"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_dirVadIntSectMultL16;
    /**< @h2xmle_description {Interference sector switching threshold in FluencePro processing}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_dirVadIntSectMultQL16;
    /**< @h2xmle_description {Q-factor for the interference sector switching threshold in FluencePro processing}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_restRatioISThreL32;
    /**< @h2xmle_description {Restoration ratio threshold for IS switching in FluencePro processing}
         @h2xmle_default     {1610612736}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_lbISL16;
    /**< @h2xmle_description {Low frequency bin bound for IS processing in FluencePro NS}
         @h2xmle_default     {1}
         @h2xmle_range       {0..256}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_ubISL16;
    /**< @h2xmle_description {Higher frequency bin bound for IS processing in FluencePro NS}
         @h2xmle_default     {128}
         @h2xmle_range       {0..256}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_minHistL16;
    /**< @h2xmle_description {Minimum buffer length for enabling reset condition in target sector switching}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_minHist2L16;
    /**< @h2xmle_description {Minimum buffer length for enabling reset condition in noise session estimation}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_lbVadnrefL16;
    /**< @h2xmle_description {Lower bound of aggressive VAD based noise reference}
         @h2xmle_default     {25}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_positiveGainDiffThreL16;
    /**< @h2xmle_description {Gain difference factor 1 for using in Mic2 noise reference}
         @h2xmle_default     {2457}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_negativeGainDiffThreL16;
    /**< @h2xmle_description {Gain difference factor 2 for using in Mic2 noise reference}
         @h2xmle_default     {-6554}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_overEstFactVADNRefL16Q13;
    /**< @h2xmle_description {Over-estimation factor 1 for VAD based noise reference in FluencePro NS}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_overEstFact2VADNRefL16Q13;
    /**< @h2xmle_description {Over-estimation factor 2 for VAD based noise reference in FluencePro NS}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_overEstFactCSD_VADNRefL16Q13;
    /**< @h2xmle_description {Over-estimation factor in clean condition for VAD based noise reference in FluencePro NS}
         @h2xmle_default     {9830}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_musicStat_threL16;
    /**< @h2xmle_description {Threshold1 for music noise detection in Fluence broadside processing}
         @h2xmle_default     {400}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_musicStatThreL16;
    /**< @h2xmle_description {Threshold2 for music noise detection in Fluence broadside processing}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_likelihood_max_limitL16;
    /**< @h2xmle_description {Limiting threshold for likelihood estimate employed in DOA computation}
         @h2xmle_default     {2000}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_gain_shift;
    /**< @h2xmle_description {Shift factor for gain application in FluencePro NS processing}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_ec_doa;
    /**< @h2xmle_description {DoA initialization for loud speaker echo direction}
         @h2xmle_default     {42}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_snrPPAggR0SmL16Q12;
    /**< @h2xmle_description {Band 1 aggressiveness factor for FluencePro NS processing}
         @h2xmle_default     {13500}
         @h2xmle_range       {-32768..32767}
         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_snrPPAggR1SmL16Q12;
    /**< @h2xmle_description {Band 2 aggressiveness factor for FluencePro NS processing}
         @h2xmle_default     {12500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_snrPPAggR3SmL16Q12;
    /**< @h2xmle_description {Band 3 aggressiveness factor for FluencePro NS processing}
         @h2xmle_default     {11500}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t reserved3;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_readOnly       {true}*/

    int16_t SPK_LinSpaProc_overEstFactBFNFNrefL16Q13;
    /**< @h2xmle_description {Over-estimation factor for the spatial filter in FluencePro NS processing}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_bfnfEndIndex;
    /**< @h2xmle_description {End frequency index for spatial filter processing in FluencePro NS}
         @h2xmle_default     {128}
         @h2xmle_range       {0..256}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "128"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "128"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "128"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "64"}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_channelGainL16Q15[3];
    /**< @h2xmle_description {Channel gain  in spatial filtering in FluencePro NS}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_nbfStartIdxL16;
    /**< @h2xmle_description {Start frequency index for spatial filter noise reference}
         @h2xmle_default     {1}
         @h2xmle_range       {0..256}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_bfnfStartIndex;
    /**< @h2xmle_description {Start frequency index for spatial filter processing in FluencePro NS}
         @h2xmle_default     {1}
         @h2xmle_range       {0..256}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_overEstShift;
    /**< @h2xmle_description {Shift factor for spatial filtering overestimation}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_diagLoadLimit;
    /**< @h2xmle_description {Upper frequency limit for spatial filter stabilization}
         @h2xmle_default     {128}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_pos1L16Q18[4];
    /**< @h2xmle_description {Microphone reference}
         @h2xmle_default     {30140}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "30140"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "30140"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "30140"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "18350"}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_pos2L16Q18[4];
    /**< @h2xmle_description {Microphone reference}
         @h2xmle_default     {18694}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "18694"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "18694"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "18694"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "32767"}

         @h2xmle_policy       {Advanced} */

    int32_t SPK_LinSpaProc_diagonal_load_r;
    /**< @h2xmle_description {Stabilization factor for the spatial filtering in FluencePro NS processing}
         @h2xmle_default     {100000000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t SPK_LinSpaProc_diagonal_load_i;
    /**< @h2xmle_description {Stabilization factor2 for the spatial filtering in FluencePro NS processing}
         @h2xmle_default     {0}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_flags;
    /**< @h2xmle_description {Enablement flag for spatial filter II in FluencePro NS}
         @h2xmle_default     {51}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_smMaskGainAttL16Q14;
    /**< @h2xmle_description {Attack smoothing factor for spatial filter II processing}
         @h2xmle_default     {13107}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_smMaskGainDecL16Q14;
    /**< @h2xmle_description {Decay smoothing factor for spatial filter II processing}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_smCompMaskGainAttL16Q14;
    /**< @h2xmle_description {Attack smoothing factor for spatial filter II noise reference}
         @h2xmle_default     {13107}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_smCompMaskGainDecL16Q14;
    /**< @h2xmle_description {Decay smoothing factor for spatial filter II noise reference}
         @h2xmle_default     {3277}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_relaxHfL16Q14;
    /**< @h2xmle_description {High frequency relaxation factor for spatial filter II processing}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "8192"}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_relaxLfL16Q14;
    /**< @h2xmle_description {Low frequency relaxation factor for spatial filter II processing}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "11469"}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_rolloffStartAngleL16;
    /**< @h2xmle_description {Starting angle for spatial filter II gain processing}
         @h2xmle_default     {25}
         @h2xmle_range       {0..400}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_rolloffEndAngleL16;
    /**< @h2xmle_description {Ending angle for spatial filter II gain processing}
         @h2xmle_default     {45}
         @h2xmle_range       {0..400}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_minMaskAggL16Q12;
    /**< @h2xmle_description {Minimum aggressiveness factof for spatial filter II gain processing}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_slopeMaskAgg2PhaseContL16Q12;
    /**< @h2xmle_description {Slope factor for gain computation in spatial filter II processing}
         @h2xmle_default     {7291}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_minCompMaskAggL16Q12;
    /**< @h2xmle_description {Minimum aggressiveness factof for spatial filter II noise reference computation}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_slopeCompMaskAgg2PhaseContL16Q12;
    /**< @h2xmle_description {Slope factor for gain computation in spatial filter II noise reference computation}
         @h2xmle_default     {7291}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_lbMaskGainExtLbL16;
    /**< @h2xmle_description {Lower frequency bound for spatial filter II gain computation}
         @h2xmle_default     {1}
         @h2xmle_range       {0..256}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_avgMaskGain4ExtRangeL16[2];
    /**< @h2xmle_description {Frequency index for spatial filter}
         @h2xmle_default     {20}
         @h2xmle_range       {0..256}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_ubMaskGainExtUbL16;
    /**< @h2xmle_description {Upper frequency bound for spatial filter II gain computation}
         @h2xmle_default     {256}
         @h2xmle_range       {0..256}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_gain4MissingDoAL16Q14;
    /**< @h2xmle_description {Default gain for filling gaps in the spatial filter II gain computation}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_compGain4MissingDoAL16Q14;
    /**< @h2xmle_description {Default gain for filling gaps in the spatial filter II noise reference}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_scaleFactor4CompMaskNrefL16Q13;
    /**< @h2xmle_description {Scaling factor for spatial filter II noise reference computation}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_NrefrolloffStartAngleL16;
    /**< @h2xmle_description {Starting angle for spatial filter II noise reference}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_NrefrolloffEndAngleL16;
    /**< @h2xmle_description {Ending angle for spatial filter II noise reference}
         @h2xmle_default     {6}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_NotchrolloffStartAngleL16;
    /**< @h2xmle_description {Starting angle for notch filtering in spatial filter II noise reference}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_NonLinSpaProc_NotchrolloffEndAngleL16;
    /**< @h2xmle_description {Ending angle for notch filtering in spatial filter II noise reference}
         @h2xmle_default     {21}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int32_t SPK_PreprocCfg_HPS_NSCfg_restThreHPSL32;
    /**< @h2xmle_description {Restoration threshold for spatial filtering in FluencePro NS}
         @h2xmle_default     {1266980377}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_NSCfg_transition_gain_floorL16;
    /**< @h2xmle_description {Transitional gain floor for spatial filter II processing}
         @h2xmle_default     {30945}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_NSCfg_lowFreqL16;
    /**< @h2xmle_description {Low frequency bound for isolated peak suppression}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_NSCfg_highFreqL16;
    /**< @h2xmle_description {High frequency bound for isolated peak suppression}
         @h2xmle_default     {235}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_NSCfg_neighbor_rangeL16;
    /**< @h2xmle_description {Neighboring frequency range for searching isolated peaks}
         @h2xmle_default     {15}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_NSCfg_local_rangeL16;
    /**< @h2xmle_description {Local frequency range for doing isolated peak suppression}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_NSCfg_local_range2L16;
    /**< @h2xmle_description {Local frequency range 2 for doing isolated peak suppression}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_NSCfg_peak_q_threshold1L16;
    /**< @h2xmle_description {Threshold for peak estimation in isolated peak suppression}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_NSCfg_peak_q_threshold2L16;
    /**< @h2xmle_description {Threshold 2 for peak estimation in isolated peak suppression}
         @h2xmle_default     {18918}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_NSCfg_hangover_sizeL16;
    /**< @h2xmle_description {Hangover for gain application in isolated peak suppression}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t reserved4;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_readOnly       {true} */

    int32_t SPK_PreprocCfg_HPS_NSCfg_ns_ratio_thresholdL32Q31;
    /**< @h2xmle_description {Threshold for NS ratio employed in isolated peak suppression}
         @h2xmle_default     {644245094}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t SPK_PreprocCfg_HPS_NSCfg_suppression_gainL32Q31;
    /**< @h2xmle_description {Gain factor employed in isolated peak suppression}
         @h2xmle_default     {134217728}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_CSDCfg_clean_mode_enter_countL16Q0;
    /**< @h2xmle_description {Counter for entering clean speech mode in FluencePro processing}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_CSDCfg_clean_mode_exit_countL16Q0;
    /**< @h2xmle_description {Counter for exiting clean speech mode in FluencePro processing}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_CSDCfg_silent_mode_enter_countL16Q0;
    /**< @h2xmle_description {Counter for entering silent mode in FluencePro processing}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPK_PreprocCfg_HPS_CSDCfg_silent_mode_exit_countL16Q0;
    /**< @h2xmle_description {Counter for exiting silent mode in FluencePro processing}
         @h2xmle_default     {40}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int32_t SPK_PreprocCfg_HPS_CSDCfg_csd_restoration_ratio_low_thresholdL32Q31;
    /**< @h2xmle_description {Low threshold for restoration ratio employed in clean speech detection}
         @h2xmle_default     {1481800000}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SPK_PreprocCfg_HPS_CSDCfg_csd_restoration_ratio_high_thresholdL32Q31;
    /**< @h2xmle_description {High threshold for restoration ratio employed in clean speech detection}
         @h2xmle_default     {1546200000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t SPK_PreprocCfg_HPS_CSDCfg_smrmt_sum_enter_CS_thresholdL32;
    /**< @h2xmle_description {Entering threshold for SMRMT processing in clean speech detection}
         @h2xmle_default     {38000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t SPK_PreprocCfg_HPS_CSDCfg_smrmt_sum_exit_CS_thresholdL32;
    /**< @h2xmle_description {Exiting threshold for SMRMT processing in clean speech detection}
         @h2xmle_default     {90000}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t SPK_PreprocCfg_HPS_CSDCfg_snr_thresholdL32Q24;
    /**< @h2xmle_description {SNR threshold employed in clean speech detection}
         @h2xmle_default     {18000000}
         @h2xmle_range       {-2147483648..2147483647}
         @h2xmle_policy       {Advanced} */

    int32_t SPK_PreprocCfg_HPS_CSDCfg_csd_norm_harmonicity_thresholdL32Q30;
    /**< @h2xmle_description {Normalized harmonicity threshold employed in clean speech detection}
         @h2xmle_default     {536870912}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_CFG_FilterL16[10];
    /**< @h2xmle_description {SPK_CFG_Filter}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Advanced} */

    int32_t SPK_CFG_DOABufferL32[10];
    /**< @h2xmle_description {Buffers for DOA processing}
         @h2xmle_default     {0}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t agMode;
    /**< @h2xmle_description {Mode word for enabling audio zoom in FluencePro and broadside processing}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t agGainAttSmL16Q15;
    /**< @h2xmle_description {Attack time smoothing parameter for audio zoom gain}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t agGainDecSmL16Q15;
    /**< @h2xmle_description {Decay time smoothing parameter for audio zoom gain}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int32_t agTargetGainL32Q15;
    /**< @h2xmle_description {Target gain default for audio zoom gain}
         @h2xmle_default     {32768}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t agMaxGainL32Q15;
    /**< @h2xmle_description {Max bound for audio zoom gain}
         @h2xmle_default     {524288}
         @h2xmle_range       {0..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t agMinGainL32Q15;
    /**< @h2xmle_description {Min bound for audio zoom gain}
         @h2xmle_default     {32768}
         @h2xmle_range       {0..2147483647}

         @h2xmle_policy       {Advanced} */

    int32_t agStepGainL32Q15;
    /**< @h2xmle_description {Step size for audio zoom gain}
         @h2xmle_default     {36768}
         @h2xmle_range       {32768..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t smBoostFFGainUBQ14;
    /**< @h2xmle_description {Upper frequency bound for single-mic gain boost in FluencePro NS processing}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smrmtR0;
    /**< @h2xmle_description {Upper bound of R0 band in SMRMT processing}
         @h2xmle_default     {16}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Advanced} */

    int16_t smrmtR1;
    /**< @h2xmle_description {Upper bound of R1 band in SMRMT processing}
         @h2xmle_default     {45}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smrmtR0overEstL16Q13;
    /**< @h2xmle_description {Over-estimation factor for SMRMT R0 band in FluencePro processing}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t smrmtR1overEstL16Q13;
    /**< @h2xmle_description {Over-estimation factor for SMRMT R1 band in FluencePro processing}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t smrmtR2overEstL16Q13;
    /**< @h2xmle_description {Over-estimation factor for SMRMT R2 band in FluencePro processing}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_snd_threshold_noSpatL32;
    /**< @h2xmle_description {Energy threshold for noise type classification}
         @h2xmle_default     {2147483647}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t SPKPhoneCfg_biasFlagThresholdL32Q2;
    /**< @h2xmle_description {Bias Flag for DoA reliability estimation}
         @h2xmle_default     {0}
         @h2xmle_range       {0..2147483647}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_spkPrimCh;
    /**< @h2xmle_description {Primary channel flag for Spkrphone processing}
         @h2xmle_default     {0}
         @h2xmle_range       {0..3}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_DOASmL16Q15;
    /**< @h2xmle_description {Smoothing Factor for DoA estimation}
         @h2xmle_default     {30000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_minDOADiff_InDegree;
    /**< @h2xmle_description {Minimum DoA difference for switching}
         @h2xmle_default     {30}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_biasFlagHangoverL16;
    /**< @h2xmle_description {Hangover period for bias flag computation}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_snglsrcHangoverFrm;
    /**< @h2xmle_description {Hangover period for single source detection}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_snglsrcMultL16;
    /**< @h2xmle_description {Multiplication factor for single source detection}
         @h2xmle_default     {24000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_snglsrcMultQ;
    /**< @h2xmle_description {Q-factor shift of multiplication factor for single source detection}
         @h2xmle_default     {3}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_perpen_sec;
    /**< @h2xmle_description {Sector selection for broadside direction of arrival}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_hangoverTgtSecSwtchLimit;
    /**< @h2xmle_description {Hangover period for enabling target sector switching}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_noiseDOASmL16Q15;
    /**< @h2xmle_description {Smoothing factor for noise DoA estimation}
         @h2xmle_default     {22000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_polarCalFrmFreq;
    /**< @h2xmle_description {Frame count for Polar activity calculation}
         @h2xmle_default     {5}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_doaHistLen;
    /**< @h2xmle_description {Histogram length for DoA estimation}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_sameDOAIndDist;
    /**< @h2xmle_description {Distance indication for DOA classification}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_convCntIncDecStepSize;
    /**< @h2xmle_description {Step size for DoA tracking and update}
         @h2xmle_default     {3}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SPKPhoneCfg_eachAggStageCnt;
    /**< @h2xmle_description {Adaptive spatial processing aggressiveness control factor}
         @h2xmle_default     {5}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t Reserved5;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t SPK_LinSpaProc_Qmflags;
    /**< @h2xmle_description {Spkrphone Linear Spatial processing Control Flag}
         @h2xmle_default     {3}
         @h2xmle_range       {0..2147483647}

         @h2xmle_policy       {Basic} */

    int16_t SPK_LinSpaProc_bfnf_nbfEndIdxL16;
    /**< @h2xmle_description {Noise reference spatial filtering End frequency index}
         @h2xmle_default     {64}
         @h2xmle_range       {0..129}


         @h2xmle_policy       {Basic} */

    int16_t SPK_LinSpaProc_longMicPair_bfnfEndIndex;
    /**< @h2xmle_description {Long Mic pair noise reference processing end frequency index}
         @h2xmle_default     {45}
         @h2xmle_range       {0..129}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_BestInputSNRChId;
    /**< @h2xmle_description {Mic channel index with best input SNR}
         @h2xmle_default     {1}
         @h2xmle_range       {0..3}


         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_longMicPairId;
    /**< @h2xmle_description {Long mic pair index}
         @h2xmle_default     {0}
         @h2xmle_range       {0..3}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_lowFreqNoiseRefInputChId;
    /**< @h2xmle_description {Mic channel index for low frequency noise reference}
         @h2xmle_default     {2}
         @h2xmle_range       {0..3}


         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_smMaskingCompressorThrdL16Q14;
    /**< @h2xmle_description {Threshold for time-frequency spatial processing compression}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_smMaskingCompressorAttL16Q14;
    /**< @h2xmle_description {Attenuation factor for time-frequency spatial processing compression}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Advanced} */

    int16_t SPK_LinSpaProc_aliasDOAThreshL16Q10;
    /**< @h2xmle_description {DoA threshold for time-frequency spatial processing}
         @h2xmle_default     {1300}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dynamicPayLoadVersion;
    /**< @h2xmle_description {Payload version for dynamic noise reference spatial processsing parameters}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t dynamicInt16PayLoad [11];
    /**< @h2xmle_description {Parameter buffer for dynamic noise reference spatial processing}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int32_t dynamicInt32Payload [10];
    /**< @h2xmle_description {Parameter buffer for dynamic noise reference spatial processing}
         @h2xmle_default     {0}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_MUTE_DET 0x00010E82

#include "adsp_begin_pack.h"

typedef struct voice_param_mute_det_t voice_param_mute_det_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_MUTE_DET", VOICE_PARAM_MUTE_DET}

    @h2xmlp_description  {Tunes mute detection. Fluence V5 and Fluence Pro modules support this parameter.\n
Maximum array size for this parameter \n
Version 0 - 24 bytes \n\n}
@h2xmlp_version     {0} */

struct voice_param_mute_det_t {

    int16_t version;
    /**< @h2xmle_description {version}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly    {true}
         @h2xmle_isVersion   {true}


         @h2xmle_policy       {Basic} */

    int16_t reserved1;
    /**< @h2xmle_description {reserved1}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly    {true}


         @h2xmle_policy       {Basic} */

    int16_t mute_det_mode;
    /**< @h2xmle_description {mute_det_mode}
         @h2xmle_default     {0}
         @h2xmle_range       {0..1}


         @h2xmle_policy       {Advanced} */

    int16_t mute_det_cnf_block;
    /**< @h2xmle_description {mute_det_cnf_block}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mute_det_svad_threshQ15;
    /**< @h2xmle_description {mute_det_svad_threshQ15}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mute_det_th_shift;
    /**< @h2xmle_description {mute_det_th_shift}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mute_det_hangover_max;
    /**< @h2xmle_description {mute_det_hangover_max}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t mic_break_hangover_max;
    /**< @h2xmle_description {mic_break_hangover_max}
         @h2xmle_default     {400}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int32_t mute_det_clean_thresh32;
    /**< @h2xmle_description {mute_det_clean_thresh32}
         @h2xmle_default     {6800}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t mic_break_thresh32;
    /**< @h2xmle_description {mic_break_thresh32}
         @h2xmle_default     {22}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FLUENCE_EC_STATE 0x00010E30

#include "adsp_begin_pack.h"

typedef struct voice_param_fp_state_t voice_param_fp_state_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_EC_STATE", VOICE_PARAM_FLUENCE_EC_STATE}
    @h2xmlp_description {Provides internal status monitoring of Fluence Pro (quad-mic) and Fluence version 5 (single-mic and
dual-mic) ECNS.\n
Maximum array size for this read-only parameter \n
Version 0  860 bytes \n
Version 1 extension  872 bytes \n }
@h2xmlp_version     {1}

    @h2xmlp_toolPolicy  {RTM}
    @h2xmlp_readOnly    {true} */

struct voice_param_fp_state_t {

    uint16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default      {1}
         @h2xmle_isVersion     {true}         */

    uint16_t Reserved;
    /**< @h2xmle_description {Read only.} */

    uint32_t lib_version;
    /**< @h2xmle_description {lib_version}
         @h2xmle_default      {0x70010200}*/

    int32_t wnd_std[4];
    /**< @h2xmle_description {wnd_std[4]} */

    int32_t wnd_wind_meas [4];
    /**< @h2xmle_description {wnd_wind_meas [4]} */

    int32_t wnd_wind_sc_sm_Q30 [4];
    /**< @h2xmle_description {wnd_wind_sc_sm_Q30 [4]} */

    int32_t wnd_pri_wind1_Q14 [4];
    /**< @h2xmle_description {wnd_pri_wind1_Q14 [4]} */

    int32_t wnr_freqc;
    /**< @h2xmle_description {wnr_freqc} */

    int16_t wnd_wind_sc_adj_sm_Q30 [4];
    /**< @h2xmle_description {wnd_wind_sc_adj_sm_Q30 [4]} */

    int16_t wnd_pri_wind2_Q14 [4];
    /**< @h2xmle_description {wnd_pri_wind2_Q14 [4]} */

    int16_t aec_s_download_flag [5];
    /**< @h2xmle_description {aec_s_download_flag [5]} */

    int16_t aec_divg_dl_frame [5];
    /**< @h2xmle_description {aec_divg_dl_frame [5]} */

    int16_t aec_pc_s_pathchange_flag [5];
    /**< @h2xmle_description {aec_pc_s_pathchange_flag [5]} */

    int16_t aec_pc_s_alert [5];
    /**< @h2xmle_description {aec_pc_s_alert [5]} */

    int16_t aec_pc_dl_alert [5];
    /**< @h2xmle_description {aec_pc_dl_alert [5]} */

    int16_t aec_bg_fg_flag [5];
    /**< @h2xmle_description {aec_bg_fg_flag [5]} */

    int16_t aec_erle_dtd_flag [5];
    /**< @h2xmle_description {aec_erle_dtd_flag [5]} */

    int16_t aec_erle_only_flag [5];
    /**< @h2xmle_description {aec_erle_only_flag [5]} */

    int16_t aec_f_vad_s_flag_vad;
    /**< @h2xmle_description {aec_f_vad_s_flag_vad} */

    int16_t aec_f_vad_hb_s_flag_vad;
    /**< @h2xmle_description {aec_f_vad_hb_s_flag_vad} */

    int16_t aec_f_vad_pf_s_flag_vad;
    /**< @h2xmle_description {aec_f_vad_pf_s_flag_vad} */

    int16_t aec_s_dtd_flag_lec [5];
    /**< @h2xmle_description {aec_s_dtd_flag_lec [5]} */

    int16_t aec_s_clip_flag;
    /**< @h2xmle_description {aec_s_clip_flag} */

    int16_t aec_s_clip_flag_hb;
    /**< @h2xmle_description {aec_s_clip_flag_hb} */

    int32_t aec_pf_a_lp_voice_en_b [32];
    /**< @h2xmle_description {aec_pf_a_lp_voice_en_b [32]} */

    int32_t aec_pf_a_lp_echo_en_b [32];
    /**< @h2xmle_description {aec_pf_a_lp_echo_en_b [32]} */

    int32_t aec_pf_a_lp_rx_en_b [32];
    /**< @h2xmle_description {aec_pf_a_lp_rx_en_b [32]} */

    int32_t aec_pf_a_nl_echo_en_b [32];
    /**< @h2xmle_description {aec_pf_a_nl_echo_en_b [32]} */

    int16_t aec_pf_a_lp_gain_b [32];
    /**< @h2xmle_description {aec_pf_a_lp_gain_b [32]} */

    int16_t aec_cn_a_ec_voice_en_ms_amp_b [32];
    /**< @h2xmle_description {aec_cn_a_ec_voice_en_ms_amp_b [32]} */

    int16_t aec_swb_cn_a_ec_voice_en_ms_amp_Q14 [2];
    /**< @h2xmle_description {aec_swb_cn_a_ec_voice_en_ms_amp_Q14 [2]} */

    int16_t qsf_dir_vad;
    /**< @h2xmle_description {qsf_dir_vad} */

    int16_t qsf_dir_vad_score;
    /**< @h2xmle_description {qsf_dir_vad_score} */

    int16_t qsf_state_ind;
    /**< @h2xmle_description {qsf_state_ind} */

    int16_t qsf_contrast [9];
    /**< @h2xmle_description {qsf_contrast [9]} */

    int16_t aec_dtd_stat[5];
    /**< @h2xmle_description {aec_dtd_stat[5]} */

    int16_t aec_dtd_flag_pp;
    /**< @h2xmle_description {aec_dtd_flag_pp} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FNS 0x00010E0B

#include "adsp_begin_pack.h"

typedef struct voice_param_fns_t voice_param_fns_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FNS", VOICE_PARAM_FNS}

    @h2xmlp_description  {FNS algorithm.\n
Fixed size for this parameter \n
Version 0 - 76 bytes \n\n }
    @h2xmlp_deprecated              {true}*/

struct voice_param_fns_t {

    int16_t fnsMode;
    /**< @h2xmle_description {Mode for enabling/disabling submodules}
         @h2xmle_default     {242}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "242"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Basic} */

    int16_t fnsInputGain;
    /**< @h2xmle_description {Input gain to FENS module}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsOutputGain;
    /**< @h2xmle_description {fnsOutputGain}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsTargetNS;
    /**< @h2xmle_description {Target noise suppression level in dB}
         @h2xmle_default     {4608}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "4608"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "4096"}

         @h2xmle_policy       {Basic} */

    int16_t fnsSalpha;
    /**< @h2xmle_description {Over-subtraction factor for stationary NS}
         @h2xmle_default     {5120}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5120"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "4096"}

         @h2xmle_policy       {Basic} */

    int16_t fnsNalpha;
    /**< @h2xmle_description {Over-subtraction factor for nonstationary NS}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Basic} */

    int16_t fnsNalphaMax;
    /**< @h2xmle_description {Maximum over-subtraction factor for nonstationary NS}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fnsEalpha;
    /**< @h2xmle_description {Scaling factor for excess noise suppression}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsNSNRmax;
    /**< @h2xmle_description {Upper bound in dB for SNR estimation}
         @h2xmle_default     {5120}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Basic} */

    int16_t fnsSNblock;
    /**< @h2xmle_description {Quarter block size for stationary NS}
         @h2xmle_default     {50}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fnsNi;
    /**< @h2xmle_description {Initialization block size for nonstationary NS}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fnsNPscale;
    /**< @h2xmle_description {Power scale factor for nonstationary noise update}
         @h2xmle_default     {2560}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsNLambda;
    /**< @h2xmle_description {Smoothing factor for nonstationary noise update}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fnsNLambdaf;
    /**< @h2xmle_description {Smoothing factor for higher level nonstationary noise update}
         @h2xmle_default     {32736}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsGsBias;
    /**< @h2xmle_description {Bias factor in dB for gain calculation}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fnsGsMax;
    /**< @h2xmle_description {SNR lower bound in dB for aggressive gain calculation}
         @h2xmle_default     {2000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsSalphaHB;
    /**< @h2xmle_description {Over-subtraction factor for high-band stationary NS}
         @h2xmle_default     {5120}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsNalphaMaxHB;
    /**< @h2xmle_description {Maximum over-subtraction factor for high-band nonstationary NS}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}

\

         @h2xmle_policy       {Basic} */

    int16_t fnsEalphaHB;
    /**< @h2xmle_description {Scaling factor for high-band excess noise suppression}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsNLambda0;
    /**< @h2xmle_description {Smoothing factor for nonstationary noise update during speech activity}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsGsFast;
    /**< @h2xmle_description {Fast smoothing factor for postprocessor gain}
         @h2xmle_default     {9830}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsGsMed;
    /**< @h2xmle_description {Medium smoothing factor for postprocessor gain}
         @h2xmle_default     {22938}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fnsGsSlow;
    /**< @h2xmle_description {Slow smoothing factor for postprocessor gain}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fnsSwbNalpha;
    /**< @h2xmle_description {Over-subtraction factor for super wide band stationary NS}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fnsSwbSalpha;
    /**< @h2xmle_description {fnsSwbSalpha}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t thresh;
    /**< @h2xmle_description {Threshold for generating a binary VAD decision}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t pwrScale;
    /**< @h2xmle_description {Indirectly lower-bounds the noise level estimate}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t hangoverMax;
    /**< @h2xmle_description {Avoids mid-speech clipping and reliably detects weak speech bursts at the end of speech activity}
         @h2xmle_default     {20}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t alphaSNR;
    /**< @h2xmle_description {Controls responsiveness of the VAD}
         @h2xmle_default     {3278}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t snrDiffMax;
    /**< @h2xmle_description {Decreasing this parameter value may help in making correct decisions during abrupt changes; however, decreasing too much may increase false alarms during long pauses/silences}
         @h2xmle_default     {3072}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t snrDiffMin;
    /**< @h2xmle_description {Decreasing this parameter value may help in making correct decisions during abrupt changes; however, decreasing too much may increase false alarms during long pauses/silences}
         @h2xmle_default     {2560}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t initLength;
    /**< @h2xmle_description {Defines the number of frames for which noise level estimate is set to a fixed value}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t maxVal;
    /**< @h2xmle_description {Defines the upper limit of the noise level}
         @h2xmle_default     {648}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t initBound;
    /**< @h2xmle_description {Defines the initial bounding value for the noise level estimate; used during the initial segment defined by the parameter initLength}
         @h2xmle_default     {10}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "50"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "10"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "50"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "50"}

         @h2xmle_policy       {Basic} */

    int16_t resetBound;
    /**< @h2xmle_description {Reset bound for noise tracking}
         @h2xmle_default     {290}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t avarScale;
    /**< @h2xmle_description {Defines the bias factor in noise estimation}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t sub_Nc;
    /**< @h2xmle_description {Defines the window length for noise estimation}
         @h2xmle_default     {25}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t spowMin;
    /**< @h2xmle_description {Defines the minimum signal power required to update the bounds for the noise floor estimate}
         @h2xmle_default     {1310}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;



#define VOICE_PARAM_MUTE_DET_STATE 0x00010EA1

#include "adsp_begin_pack.h"

/** @h2xmlp_subStruct */

typedef struct mic_low_det_struct {

    int16_t mic_low_det ;

    int16_t mic_low_det_hang;
} mic_low_det_struct;

typedef struct voice_param_mute_det_state_t voice_param_mute_det_state_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_MUTE_DET_STATE", VOICE_PARAM_MUTE_DET_STATE}
    @h2xmlp_description {Monitors the mute detection state. Fluence V5 and Fluence Pro modules support this read-only parameter. \n
Maximum array size for this parameter \n
Version 0 – 12+num_mic*4 bytes \n\n}
    @h2xmlp_version     {0}
    @h2xmlp_readOnly    {true}
    @h2xmlp_toolPolicy  {RTM}
    @h2xmlx_expandStructs	{false}*/


struct voice_param_mute_det_state_t {

    int16_t version;
    /**< @h2xmle_description {Version}
         @h2xmle_isVersion    {true}
          */

     int16_t reserved1;
    /**< @h2xmle_description {Client must set this field to zero.}
          */

     int16_t mute_det_counter;
    /**< @h2xmle_description {mute_det_counter}
          */

     int16_t mute_det;
    /**< @h2xmle_description {mute_det}
          */

     int16_t mute_det_hang;
    /**< @h2xmle_description {mute_det_hang}
          */

     int16_t num_mics;
    /**< @h2xmle_description {num_mics}
          */

     mic_low_det_struct mic[0] ;
     /**< @h2xmle_description {Dynamic array depending on the size of mic.}
          @h2xmle_variableArraySize   {"num_mics"} */


 }
#include "adsp_end_pack.h"
;


#endif

