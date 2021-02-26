#ifndef MEC_CALIB_H
#define MEC_CALIB_H
/*==============================================================================
  @file mec_calib.h
  @brief This file contains MEC API
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/21/17   pag     Added h2xml comments.
 ========================================================================== */

#include "mmdefs.h"
/** @h2xml_title1           {MEC}
    @h2xml_title_agile_rev  {MEC}
    @h2xml_title_date       {February 2, 2017} */


/* ID of the Multichannel Echo Canceler module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_MEC_ENABLE
    - #AUDPROC_PARAM_ID_MEC_SYS
    - #AUDPROC_PARAM_ID_MEC_DELAY
    - #AUDPROC_PARAM_ID_MEC_AEC
 */
#define AUDPROC_MODULE_ID_MEC                  0x00010931
/**@h2xmlm_module       {"AUDPROC_MODULE_ID_MEC",
                          AUDPROC_MODULE_ID_MEC}
    @h2xmlm_displayName  {"AUDIO MEC"}
	@h2xmlm_description	 {ID of the Multichannel Echo Canceler module.


    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_MEC_ENABLE
    - #AUDPROC_PARAM_ID_MEC_SYS
    - #AUDPROC_PARAM_ID_MEC_DELAY
    - #AUDPROC_PARAM_ID_MEC_AEC}

   @h2xmlm_toolPolicy              {Calibration; RTC}
   @{                   <-- Start of the Module -->  */

#define  AUDPROC_PARAM_ID_MEC_ENABLE          0x00010932
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MEC_ENABLE", AUDPROC_PARAM_ID_MEC_ENABLE}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_MEC_ENABLE parameter}  */


/* Payload of the AUDPROC_PARAM_ID_MEC_ENABLE parameter
 */
struct audproc_mec_enable_t
{

   uint32_t enable_flag;
   /**< @h2xmle_description  {Specifies whether MEC is enabled.}
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}
	    @h2xmle_policy {basic}			*/

}
#include "adsp_end_pack.h"
;


/* ID of the system configuration parameter used by #AUDPROC_MODULE_ID_MEC.

 */

#define  AUDPROC_PARAM_ID_MEC_SYS          0x0001092E
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MEC_SYS", AUDPROC_PARAM_ID_MEC_SYS}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_MEC_SYS parameter}
    @h2xmlp_version     {0}    */

/* Payload of the AUDPROC_PARAM_ID_MEC_SYS parameter
 */
struct audproc_mec_sys_t
{
   int32_t    version;
   /**< @h2xmle_description  {Specifies the current version of this parameter.}
   @h2xmle_default {0}
   @h2xmle_readonly	{true}
    @h2xmle_isVersion    {true}    */

   int32_t    mode;
   /**< @h2xmle_description  {Specifies the mode word for setting MEC features. }
   @h2xmle_default {0x1009}
   @h2xmle_policy {basic}
        */

   int16_t    prim_in_chan_L16;
   /**< @h2xmle_description  {Index for the primary channel (0)}
		@h2xmle_policy {advanced}
		@h2xmle_default {0}
        */

   int16_t    mic_ch_map_L16[4];
   /**< @h2xmle_description  {Mic channel mapping indexed by the microphone index. }
		@h2xmle_policy {advanced}
		@h2xmle_default {0}
        */


   int16_t    input_gain_L16Q13[4];
   /**< @h2xmle_description  {Input gain per mic channel. }
		@h2xmle_policy {basic}
		@h2xmle_default {0}
        */


   int16_t    out_gain_L16Q11;
    /**< @h2xmle_description  {Output gain. }
		@h2xmle_policy {basic}
		@h2xmle_default {0}
        */


   int16_t    fb_cfg_force_max_bw_flag;
   /**< @h2xmle_description  {Flag for the bandwidth limit.}
		@h2xmle_policy {advanced}
		@h2xmle_default {0}
        */


   int16_t    fb_cfg_primary_mic_aec_bw_limit;
   /**< @h2xmle_description  {Maximum bandwidth to process for the primary mic.\n - 0.5 * sample rate -- 4 kHz for NB, 8 kHz for WB, 16 kHz for SWB, and
          24 kHz for FB)\n
        - Otherwise, use fb_cfg_primary_mic_aec_bw_limit and
          fb_cfg_nonPrimary_mic_aec_bw_limit values}
		@h2xmle_policy {advanced}
		@h2xmle_default {0x1F40}
        */

   int16_t    fb_cfg_nonPrimary_mic_aec_bw_limit;
   /**< @h2xmle_description  {Maximum bandwidth to process for the non-primary mic.}
		@h2xmle_policy {advanced}
		@h2xmle_default {0x1F40}
        */

   int16_t    reserved;
	  /**< @h2xmle_description  {Clients must set this field to 0.}
		@h2xmle_default {0x0}
		@h2xmle_visibility	{hide}
        */

   int32_t    aec_f_vad_lth_en_L32Q20;
   /**< @h2xmle_description  {Far-end VAD for AEC.}
		@h2xmle_policy {basic}
		@h2xmle_default {0x03}
        */
}
#include "adsp_end_pack.h"
;

#define AUDPROC_PARAM_ID_MEC_DELAY                 0x0001092F

/* Structure for the MEC delay parameter. */
typedef struct audproc_mec_delay_param_t audproc_mec_delay_param_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MEC_DELAY",AUDPROC_PARAM_ID_MEC_DELAY}
    @h2xmlp_description {ID of the Echo Canceller Delay parameter used by #AUDPROC_MODULE_ID_MEC.}  */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_MEC_DELAY
 parameter used by the  echo canceller module.
 */
struct audproc_mec_delay_param_t
{
   int16_t ec_delay_L16;
   /**< @h2xmle_description  {Rx echo-to-Tx delay in samples at 8 kHz. }
		@h2xmle_policy {basic}
		@h2xmle_default {0x0}
        */

   int16_t reserved;
     /**< @h2xmle_description  {Clients must set this field to 0.}
		@h2xmle_default {0x0}
		@h2xmle_visibility	{hide}
        */

}
#include "adsp_end_pack.h"
;


/* Following payload will be repeated for num_tx_channel
    times
 */

/** @h2xmlp_subStruct */

typedef struct audproc_mec_aec_config_t
{
   int16_t    aec_cfg_clip_flag_frac_L16Q15;
		/**< @h2xmle_description  {Path change detector for the foreground output-to-mic energy
        ratio parameter.}
		@h2xmle_default {0xA4}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_pathchange_flag_frac_L16Q15;
   /**< @h2xmle_description  {Path change detector for the filter coefficients ratio parameter.}
		@h2xmle_default {0x6AA9}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_pathchange_wgt_enfact_d_L16Q15;
	/**< @h2xmle_description  {Path change detector for when the background filter has better convergence than the foreground filter. }
		@h2xmle_default {0x3333}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_sflen_L16;
   /**< @h2xmle_description  {Adaptive filter length (number of taps) at low band. }
		@h2xmle_default {0x0A}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_sflen_L16_hb;
   /**< @h2xmle_description  {Adaptive filter length (number of taps) at high band.}
		@h2xmle_default {0x0A}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_wgtq_L16;
   /**< @h2xmle_description  {Q factor for filter coefficients.}
		@h2xmle_default {0x03}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_r_update_L16;
		/**< @h2xmle_description  { Period (number of far-end active frames) of foreground filter updates.}
		@h2xmle_default {0x0}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_bg_update_DT;
   /**< @h2xmle_description  { Flag that enables a background filter update during double talk.}
		@h2xmle_default {0x01}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_r_lrate_L16;
   /**< @h2xmle_description  {Right shift on sLrate for a foreground filter update.}
		@h2xmle_default {0x04}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_sQShift_max_L16;
   /**< @h2xmle_description  {SQshift upper limit for a foreground filter update.}
		@h2xmle_default {0xFFF6}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_sbg_mic_frac_L16Q15;
   /**< @h2xmle_description  {Convergence download test of the ERLE threshold.}
		@h2xmle_default {0x4000}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_saec_full_learn_frames_L16;
	/**< @h2xmle_description  {Number of initial far-end active (no double talk) frames that enforce a download regardless of tests.}
		@h2xmle_default {0x28}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_saec_num_frames_L16;
   /**< @h2xmle_description  {Convergence download test of the pass frame counter.}
		@h2xmle_default {0x04}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_dtd_threshold_lec__L16Q15;
   /**< @h2xmle_description  {Double-talk detector threshold.}
		@h2xmle_default {0x6CCC}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_dtd_threshold_postp__L16Q15;
   /**< @h2xmle_description  {Double-talk detector threshold for echo canceler pre/postprocessing.}
		@h2xmle_default {0x6CCC}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_l_rate_L16;
   /**< @h2xmle_description  {Left shift that approximates the AEC filter learning rate.}
		@h2xmle_default {0xFFFF}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_l_rate_L16_DT;
   /**< @h2xmle_description  {SLrate offset (right shift) during a double-talk detected frame.}
		@h2xmle_default {0x0}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_bg_fg_frac_L16Q15;
   /**< @h2xmle_description  {Convergence download test of the background or foreground threshold.}
		@h2xmle_default {0x4000}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_divg_mic_fg_frac_L16Q15;
   /**< @h2xmle_description  {Divergence download test of the background or foreground threshold.}
		@h2xmle_default {0x4000}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_divg_bg_mic_frac_L16Q15;
   /**< @h2xmle_description  {Divergence download test of the background or mic threshold.}
		@h2xmle_default {0x4000}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_divg_frame_trig_L16;
   /**< @h2xmle_description  {Divergence download test of the frame trigger threshold.}
		@h2xmle_default {0x05}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_dtd_hangover_max_L16;
   /**< @h2xmle_description  {Double-talk detector for the hangover duration.}
		@h2xmle_default {0x14}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_dtd_ni_scalefactor_L16;
		/**< @h2xmle_description  {Double-talk detector that uses the scaled far-end excitation energy to compensate for the noise level in the numerator.}
		@h2xmle_default {0x29}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_dtd_bg_mic_frac_L16Q15;
   /**< @h2xmle_description  {Double-talk detector for the background or mic threshold.}
		@h2xmle_default {0x0290}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_dtd_good_frames_L16;
   /**< @h2xmle_description  {Double-talk is detected in the frame counter.}
		@h2xmle_default {0xC8}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_sEn_sm_bit;
   /**< @h2xmle_description  {Right-shift for energy smoothing.}
		@h2xmle_default {0x02}
		@h2xmle_policy {advanced}
        */

   int32_t    aec_cfg_lAec_sc2_Q31;
   /**< @h2xmle_description  {Regularization parameter to stablize NLMS updates.}
		@h2xmle_default {0x106260}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_sTestSuccess;
   /**< @h2xmle_description  {Convergence download test of the minimum passes.}
		@h2xmle_default {0x03}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_sDtd_Al_Bitshift;
	/**< @h2xmle_description  {Right-shift that approximates double-talk detector second order statistics smoothing.}
		@h2xmle_default {0x03}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_sDtd_Trigger;
   /**< @h2xmle_description  {Frames of double-talk that trigger a hangover period.}
		@h2xmle_default {0x01}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_saec_full_learn_frames_DT_L16;
	/**< @h2xmle_description  {Number of initial far-end active (ignoring double talk) frames that enforce a download.}
		@h2xmle_default {0x0}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_pathchange_Al_Bitshift;
	/**< @h2xmle_description  {Smoothing factor used when computing second order statistics for the echo canceler path change flag.}
		@h2xmle_default {0x04}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_pathchange_sHangoverMax;
   /**< @h2xmle_description  {Hangover frames for the path change flag.}
		@h2xmle_default {0x0F}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_clip_flag_Al_Bitshift;
	/**< @h2xmle_description  {Smoothing factor used when computing second order statistics for the echo canceler clip flag.}
		@h2xmle_default {0x03}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_vssCfg_sVss_Al_Bitshift;
	/**< @h2xmle_description  {Smoothing factor used when computing second order statistics for the echo canceler VSS. }
		@h2xmle_default {0x09}
		@h2xmle_policy {advanced}
        */

   int32_t    aec_cfg_vssCfg_lVss_Eps_Q31;
   /**< @h2xmle_description  {VSS regularization factor.}
		@h2xmle_default {0x0864}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_vssCfg_sVss_Min_Mu_Q15;
	/**< @h2xmle_description  {Lower limit of the VSS mu.}
		@h2xmle_default {0x7FFF}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_vssCfg_sVss_Min_Statistic_Blk_Size;
   /**< @h2xmle_description  {Number of frames in a MinStat block.}
		@h2xmle_default {0x14}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_vssCfg_sVss_Min_Statistic_Num_Blk;
   /**< @h2xmle_description  {Number of MnStat blocks.}
		@h2xmle_default {0x0A}
		@h2xmle_policy {advanced}
        */

   int16_t    aec_cfg_vssCfg_sVss_Min_Statistic_Num_Frames;
   /**< @h2xmle_description  {Number of frames used for minimum statistics.}
		@h2xmle_default {0xC8}
		@h2xmle_policy {advanced}
        */

} audproc_mec_aec_config_t
#include "adsp_end_pack.h"
;

#define AUDPROC_PARAM_ID_MEC_AEC                 0x00010930

/* Structure for the MEC AEC parameter. */
typedef struct audproc_mec_aec_param_t audproc_mec_aec_param_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MEC_AEC",AUDPROC_PARAM_ID_MEC_AEC}
    @h2xmlp_description {ID of the acoustic echo canceller parameter
    used by #AUDPROC_MODULE_ID_MEC.}
     @h2xmlp_version     {0}
     @h2xmlx_expandStructs	{false}  */
#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_MEC_AEC
 parameter used by the  echo canceller module
 */
/* Immediately following this structure are num_tx_channel of the
    configuration payload (%audproc_mec_aec_config_t).
*/
struct audproc_mec_aec_param_t
{
   int32_t version;
   /**< @h2xmle_description  {Specifies the current version of this parameter.}
   @h2xmle_policy {basic}
   @h2xmle_default {0}
   @h2xmle_readonly	{true}
    @h2xmle_isVersion    {true}    */

   int16_t num_tx_channel;
   /**< @h2xmle_description  {Number of Tx mics for AEC.}
   @h2xmle_policy {basic}
   @h2xmle_default {0x04}
        */

   int16_t reserved;
	  /**< @h2xmle_description  {Clients must set this field to 0.}
		@h2xmle_default {0x0}
		@h2xmle_readOnly	{true}
        */



    audproc_mec_aec_config_t Aec_param_struct[0];
   /**< @h2xmle_description  { Aec_param_struct}
		@h2xmle_policy {advanced}
		@h2xmle_variableArraySize {num_tx_channel}
        */
}
#include "adsp_end_pack.h"
;


/**
   @}                   <-- End of the Module -->*/
#endif
