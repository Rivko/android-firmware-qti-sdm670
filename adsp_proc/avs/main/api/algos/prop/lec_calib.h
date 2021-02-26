#ifndef LEC_CALIB_H
#define LEC_CALIB_H
/*==============================================================================
  @file lec_calib.h
  @brief This file contains LEC API
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
/** @h2xml_title1           {Linear Echo Canceler (LEC)}
    @h2xml_title_agile_rev  {Linear Echo Canceler (LEC)}
    @h2xml_title_date       {March 8, 2017} */

/**
   @h2xmlx_xmlNumberFormat {int}
*/

/** ID of the Linear Echo Canceler (LEC) module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_LEC_ENABLE
    - #AUDPROC_PARAM_ID_LEC_SYS
    - #AUDPROC_PARAM_ID_LEC_DELAY
    - #AUDPROC_PARAM_ID_LEC_MON_STATE
    - #AUDPROC_PARAM_ID_LEC_AF_COEFFS
 */
#define AUDPROC_MODULE_ID_LEC                  0x00010926
/** @h2xmlm_module       {"AUDPROC_MODULE_ID_LEC",
                          AUDPROC_MODULE_ID_LEC}

    @h2xmlm_displayName  {"AUDIO LEC"}
	@h2xmlm_description	 {ID of the Linear Echo Canceler (LEC) module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_LEC_ENABLE\n
    - #AUDPROC_PARAM_ID_LEC_SYS\n
    - #AUDPROC_PARAM_ID_LEC_DELAY\n
    - #AUDPROC_PARAM_ID_LEC_MON_STATE\n
    - #AUDPROC_PARAM_ID_LEC_AF_COEFFS\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
 */

/* ID of the LEC enable parameter used by AUDPROC_MODULE_ID_LEC.

 */
#define  AUDPROC_PARAM_ID_LEC_ENABLE         0x00010927

/* Structure for the LEC enable parameter. */
typedef struct audproc_lec_enable_t audproc_lec_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_LEC_ENABLE", AUDPROC_PARAM_ID_LEC_ENABLE}
    @h2xmlp_description {ID of the LEC enable parameter used by AUDPROC_MODULE_ID_LEC.} */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_LEC_ENABLE parameter in the EANS module.
 */
 struct audproc_lec_enable_t
{

   uint32_t enable_flag;
   /**< @h2xmle_description  {Specifies whether the LEC is enabled. }
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}  */

}
#include "adsp_end_pack.h"
;


/* ID of the LEC system configuration parameter used by #AUDPROC_MODULE_ID_LEC.
*/
#define AUDPROC_PARAM_ID_LEC_SYS                 0x00010928

/* Structure for the LEC enable parameter. */
typedef struct audproc_lec_sys_cfg_param_t audproc_lec_sys_cfg_param_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_LEC_SYS", AUDPROC_PARAM_ID_LEC_SYS}
    @h2xmlp_description {ID of the LEC system configuration parameter used by #AUDPROC_MODULE_ID_LEC.}
     @h2xmlp_version     {0}   */
#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_LEC_SYS
 parameter used by the  linear echo canceller module
 */
 struct audproc_lec_sys_cfg_param_t
 {
   int16_t version;
   /**< @h2xmle_description  {Specifies the current version of this parameter.}
		@h2xmle_default	{0}
		@h2xmle_policy	{basic}
        @h2xmle_isVersion    {true}        */


   int16_t reserved;
   /**< @h2xmle_description  {Clients must set this field to 0.}
		@h2xmle_default	{0}
		@h2xmle_readOnly	{true}		*/


   int32_t mode;
   /**< @h2xmle_description  {Specifies the mode word for setting LEC features. }
		@h2xmle_default	{0x1E}
		@h2xmle_policy	{basic} */

   int16_t prim_in_chan_L16;
   /**< @h2xmle_description  {Index for the primary channel (0).}
		@h2xmle_default	{0}
		@h2xmle_policy	{advanced} */


   int16_t input_gain_L16q13;
   /**< @h2xmle_description  {Tx input gain in Q13 format.}
	@h2xmle_default	{0}
	@h2xmle_dataFormat {Q15}
		@h2xmle_policy	{advanced}*/


   int16_t out_gain_L16Q11;
   /**< @h2xmle_description  {Tx output gain in Q13 format.}
	@h2xmle_default	{0}
	@h2xmle_dataFormat {Q15}
		@h2xmle_policy	{basic}*/


   int16_t aec_pathchange_flag_frac_L16Q15;
   /**< @h2xmle_description  {Path change detector for the foreground output-to-mic energy ratio
        parameter.}
		@h2xmle_default	{0x6AA9}
		@h2xmle_policy	{basic} */


   int16_t aec_pathchange_wgt_enfact_L16Q15;
   /**< @h2xmle_description  {Path change detector for when the background filter has better
        convergence than the foreground filter.}
		@h2xmle_default	{0x599A}
		@h2xmle_policy	{basic} */


   int16_t aec_pathchange_wgt_fact_L16Q15;
   /**< @h2xmle_description  {Path change detector for the filter coefficients ratio parameter. }
		@h2xmle_default	{0x4000}
		@h2xmle_policy	{basic} */


   int16_t aec_pathchange_wgt_alph_L16Q15;
   /**< @h2xmle_description  {Path change detector for the background filter coefficients smoothing
        parameter.}
		@h2xmle_default	{0x199A}
		@h2xmle_policy	{advanced} */


   int16_t aec_pathchange_wgt_enfact_d_L16Q15;
   /**< @h2xmle_description  {Path change detector download alert for the foreground-to-background
        output energy ratio parameter.}
		@h2xmle_default	{0x3333}
		@h2xmle_policy	{basic} */


   int16_t aec_pathchange_wgt_trig_d_L16;
   /**< @h2xmle_description  {Path change detector for the alerts threshold.}
		@h2xmle_default	{0x03}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg_lec_convergence_extra_run;
   /**< @h2xmle_description  {Extra number of lec_aec_process() calls during the intial far-end
        frames to speed up convergence (increases peak MPPS).}
		@h2xmle_default	{0x0}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_sflen_L16;
   /**< @h2xmle_description  {Adaptive filter length (number of taps) of the Rx left channel
       (or Rx mono) at low band.}
		@h2xmle_default	{0x78}
		@h2xmle_policy	{basic}		*/


   int16_t aec_cfg0_wgtq_L16;
   /**< @h2xmle_description  {Q factor for filter coefficients.}
		@h2xmle_default	{0x01}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_r_update_L16;
   /**< @h2xmle_description  {Period (number of far-end active frames) of foreground filter
        updates.}
		@h2xmle_default	{0}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_bg_update_DT;
   /**< @h2xmle_description  {Flag that enables a background filter update during double talk.}
		@h2xmle_default	{0x01}
		@h2xmle_policy	{basic}		*/


   int16_t aec_cfg0_r_lrate_L16;
   /**< @h2xmle_description  {Right-shift on sLrate for a foreground filter update.}
		@h2xmle_default	{0x04}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_sbg_mic_frac_L16Q15;
   /**< @h2xmle_description  {Convergence download test of the echo return loss enhancement (ERLE)
        threshold.}
		@h2xmle_default	{0x2000}
		@h2xmle_policy	{basic}*/


   int16_t aec_cfg0_saec_full_learn_frames_L16;
   /**< @h2xmle_description  {Number of initial far-end active (no DT) frames that enforce a download
        regardless of tests.}
		@h2xmle_default	{0x40}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_dtd_threshold_lec_L16Q15;
   /**< @h2xmle_description  {Double-talk detector threshold.}
		@h2xmle_default	{0x4A38}
		@h2xmle_policy	{basic}		*/


   int16_t aec_cfg0_sdtd_al_bit_shift;
   /**< @h2xmle_description  {Right shift that approximates second order statistics smoothing for the
        double-talk detector.}
		@h2xmle_default	{0x03}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_l_rate_L16;
   /**< @h2xmle_description  {Left shift that approximates the AEC filter learning rate. }
		@h2xmle_default	{0x06}
		@h2xmle_policy	{basic}*/


   int16_t aec_cfg0_bg_fg_frac_L16Q15;
   /**< @h2xmle_description  {Convergence download test of the background or foreground threshold.}
		@h2xmle_default	{0x7D00}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_divg_bg_fg_frac_L16Q15;
   /**< @h2xmle_description  {Divergence download test of the background or foreground threshold.}
		@h2xmle_default	{0x4000}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_divg_bg_mic_frac_L16Q15;
   /**< @h2xmle_description  {Divergence download test of the background or mic threshold.}
		@h2xmle_default	{0x4000}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_divg_frame_trig_L16;
   /**< @h2xmle_description  {Divergence download test of the frame trigger threshold.}
		@h2xmle_default	{0x05}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_dtd_hangover_max_L16;
   /**< @h2xmle_description  {Double-talk detector for the hangover duration.}
		@h2xmle_default	{0x14}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_dtd_ni_scalefactor_L16;
   /**< @h2xmle_description  {Double-talk detector that uses the scaled far-end excitation energy
        to compensate for the noise level in the numerator.}
		@h2xmle_default	{0x29}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_dtd_bg_mic_frac_L16Q15;
   /**< @h2xmle_description  {Double-talk detector for the background or mic threshold.}
		@h2xmle_default	{0x0290}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_dtd_good_frames_L16;
   /**< @h2xmle_description  {Double-talk is detected in the frame counter.}
		@h2xmle_default	{0xC8}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_sQShift_max_L16;
   /**< @h2xmle_description  {SQshift upper limit for the foreground filter update.}
		@h2xmle_default	{0x7FFF}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_snum_frames;
   /**< @h2xmle_description  {Convergence download test of the pass frame counter.}
		@h2xmle_default	{0x02}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_svss_min_mu_Q15;
   /**< @h2xmle_description  {Lower limit of the variable-step size (VSS).}
		@h2xmle_default	{0x7FFF}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_stest_success;
   /**< @h2xmle_description  {Convergence download to test the minimum passes.}
		@h2xmle_default	{0x03}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg0_l_rate_L16_DT;
   /**< @h2xmle_description  {SLrate offset (right shift) during a double-talk detected frame.}
		@h2xmle_default	{0}
		@h2xmle_policy	{basic} */


   int16_t aec_cfg0_saec_full_learn_frames_L16_DT;
   /**< @h2xmle_description  { Number of initial far-end active (ignoring double talk) frames that
        enforce a download.}
		@h2xmle_default	{0}
		@h2xmle_policy	{advanced} */


   int32_t aec_cfg0_laec_sc2_Q31;
   /**< @h2xmle_description  {Regularization parameter used to stabilize normalized least mean square
        (NLMS) updates.}
		@h2xmle_default	{0x14800000}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_sflen_L16;
   /**< @h2xmle_description  {Adaptive filter length (number of taps) of the Rx left channel (or Rx
        mono) at high band.}
		@h2xmle_default	{0x78}
		@h2xmle_policy	{basic} */


   int16_t aec_cfg1_wgtq_L16;
   /**< @h2xmle_description  { Q factor for filter coefficients. }
		@h2xmle_default	{0x01}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_r_update_L16;
   /**< @h2xmle_description  {Period (number of far-end active frames) of foreground filter
        updates. }
		@h2xmle_default	{0}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_bg_update_DT;
   /**< @h2xmle_description  {Flag that enables a background filter update during double talk.}
		@h2xmle_default	{0x01}
		@h2xmle_policy	{basic} */

   int16_t aec_cfg1_r_lrate_L16;
   /**< @h2xmle_description  {Right shift on sLrate for an foreground filter update.}
		@h2xmle_default	{0x04}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_sbg_mic_frac_L16Q15;
   /**< @h2xmle_description  {Convergence download test of the ERLE threshold.}
		@h2xmle_default	{0x2000}
		@h2xmle_policy	{basic} */


   int16_t aec_cfg1_saec_full_learn_frames;
   /**< @h2xmle_description  {Number of initial far-end active (no double talk) frames that enforce
        a download regardless of tests.}
		@h2xmle_default	{0x40}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_dtd_threshold_lec_L16Q15;
   /**< @h2xmle_description  {Double-talk detector threshold. }
		@h2xmle_default	{0x4A38}
		@h2xmle_policy	{basic} */


   int16_t aec_cfg1_sdtd_al_bit_shift;
   /**< @h2xmle_description  {Right shift that approximates second order statistics smoothing for the
        double-talk detector.}
		@h2xmle_default	{0x03}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_l_rate_L16;
   /**< @h2xmle_description  {Left shift that approximates the AEC filter learning rate.}
		@h2xmle_default	{0x06}
		@h2xmle_policy	{basic} */


   int16_t aec_cfg1_bg_fg_frac_L16Q15;
   /**< @h2xmle_description  {Convergence download test of the background or foreground threshold.}
		@h2xmle_default	{0x7D00}
		@h2xmle_policy	{basic} */


   int16_t aec_cfg1_divg_bg_fg_frac_L16Q15;
   /**< @h2xmle_description  {Divergence download test of the background or foreground threshold.}
		@h2xmle_default	{0x4000}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_divg_bg_mic_frac_L16Q15;
   /**< @h2xmle_description  {Divergence download test of the background or mic threshold.}
		@h2xmle_default	{0x4000}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_divg_frame_trig_L16;
   /**< @h2xmle_description  {Divergence download test of the frame trigger threshold.}
		@h2xmle_default	{0x05}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_dtd_hangover_max_L16;
   /**< @h2xmle_description  {Double-talk detector for hangover duration.}
		@h2xmle_default	{0x14}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_dtd_ni_scalefactor_L16;
   /**< @h2xmle_description  {Double-talk detector that uses the scaled far-end excitation energy
        to compensate for the noise level in the numerator.}
		@h2xmle_default	{0x29}
		@h2xmle_policy	{advanced} */

   int16_t aec_cfg1_dtd_bg_mic_frac_L16Q15;
   /**< @h2xmle_description  {Double-talk detector for the bg/mic threshold.}
		@h2xmle_default	{0x0290}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_dtd_good_frames_L16;
   /**< @h2xmle_description  {Double-talk is detected in the frame counter.}
		@h2xmle_default	{0xC8}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_sQShift_max_L16;
   /**< @h2xmle_description  {SQshift upper limit for a foreground filter update.}
		@h2xmle_default	{0x7FFF}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_snum_frames;
   /**< @h2xmle_description  { Convergence download to test the pass frame counter.}
		@h2xmle_default	{0x02}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_svss_min_mu_Q15;
   /**< @h2xmle_description  {Lower limit of the VSS.}
		@h2xmle_default	{0x7FFF}
		@h2xmle_policy	{advanced} */


   int16_t aec_cfg1_stest_success;
   /**< @h2xmle_description  {Convergence download to test the minimum passes.}
		@h2xmle_default	{0x03}
		@h2xmle_policy	{basic} */


   int16_t aec_cfg1_l_rate_L16_DT;
   /**< @h2xmle_description  {SLrate offset (right shift) during a double-talk detected frame.}
		@h2xmle_default	{0}
		@h2xmle_policy	{basic} */

   int16_t aec_cfg1_saec_full_learn_frames_L16_DT;
   /**< @h2xmle_description  {Number of initial far-end active (ignoring double talk) frames that
        enforce a download.}
		@h2xmle_default	{0}
		@h2xmle_policy	{advanced} */


   int32_t aec_cfg1_laec_sc2_Q31;
   /**< @h2xmle_description  {Regularization parameter used to stabilize NLMS updates.}
		@h2xmle_default	{0x14800000}
		@h2xmle_policy	{advanced} */


   int32_t aec_f_vad_lth_en_L32Q20;
   /**< @h2xmle_description  {Far-end energy-based VAD threshold (low band).}
		@h2xmle_default	{0x5DC0}
		@h2xmle_policy	{basic} */


   int32_t aec_f_vad_hb_lth_en_L32Q20;
   /**< @h2xmle_description  {Far-end energy-based VAD threshold (high band).}
		@h2xmle_default	{0x5DC0}
		@h2xmle_policy	{basic} */


   int16_t aec_cfg0_sflen1_L16;
   /**< @h2xmle_description  {Adaptive filter length (number of taps) of the Rx right channel at low
        band.}
		@h2xmle_default	{0x78}
		@h2xmle_policy	{basic} */


   int16_t aec_cfg1_sflen1_L16;
   /**< @h2xmle_description  {Adaptive filter length (number of taps) of the Rx right channel at high
        band.}
		@h2xmle_default	{0x78}
		@h2xmle_policy	{basic} */


 }
#include "adsp_end_pack.h"
;


/* ID of the LEC delay parameter used by #AUDPROC_MODULE_ID_LEC.

*/
#define AUDPROC_PARAM_ID_LEC_DELAY                 0x00010929

 /* Structure for the LEC enable parameter. */
typedef struct audproc_lec_delay_param_t audproc_lec_delay_param_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_LEC_DELAY", AUDPROC_PARAM_ID_LEC_DELAY}
    @h2xmlp_description {ID of the LEC delay parameter used by #AUDPROC_MODULE_ID_LEC.} */
 #include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_LEC_DELAY
 parameter used by the linear echo canceller module
 */
struct audproc_lec_delay_param_t
 {
    int16_t ec_delay_L16;
   /**< @h2xmle_description  {Rx left/mono channel-to-Tx delay in samples at 8 kHz.}
		@h2xmle_default	{0}
		@h2xmle_policy	{basic} */


    int16_t ec_delay1_L16;
   /**< @h2xmle_description  {Rx right channel-to-Tx delay in samples at 8 kHz.}
		@h2xmle_default	{0}
		@h2xmle_policy	{basic} */


}
#include "adsp_end_pack.h"
;


/* ID of the LEC state monitor parameter used by #AUDPROC_MODULE_ID_LEC.

*/
#define AUDPROC_PARAM_ID_LEC_MON_STATE                 0x0001092A

/** Maximum number of channels supported by LEC near-end input. */
#define AUDPROC_LEC_MAX_TX_LEC_CHS (1)

/** Maximum number of subframes supported by the LEC. */
#define AUDPROC_LEC_MAX_AEC_SUBFRAMES        (AUDPROC_LEC_MAX_TX_LEC_CHS + 1)

/** Maximum number of channels supported by LEC far-end input. */
#define AUDPROC_LEC_MAX_RX_LEC_CHS (2)

  /* Structure for the LEC Monitor state parameter. */
  typedef struct audproc_lec_mon_state_t audproc_lec_mon_state_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_LEC_MON_STATE", AUDPROC_PARAM_ID_LEC_MON_STATE}
    @h2xmlp_description {ID of the LEC state monitor parameter used by #AUDPROC_MODULE_ID_LEC.}
	@h2xmlp_toolPolicy	{RTM}
    @h2xmlp_readOnly    {true}
    @h2xmlp_version     {0}    */
  #include "adsp_begin_pack.h"


struct audproc_lec_mon_state_t
{
   int16_t version;
   /**< @h2xmle_description  {Specifies the current version of this parameter. }
		@h2xmle_default	{0}
         @h2xmle_isVersion    {true}*/


   int16_t reserved;
   /**< @h2xmle_description  {Clients must set this field to 0.}
		@h2xmle_default	{0} */


   uint32_t lib_version;
   /**< @h2xmle_description  {Specifies the current version of the library. }
		@h2xmle_default	{0} */


   int16_t aec_sDownloadFlag[AUDPROC_LEC_MAX_AEC_SUBFRAMES][AUDPROC_LEC_MAX_RX_LEC_CHS];
   /**< @h2xmle_description  {Flag for a background-onto-foreground download.}
		@h2xmle_range {-32768..32767}
		@h2xmle_default	{0x0000} */


   int16_t aec_divgDlFrame[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  {Flag for a divergence download. }
		@h2xmle_default	{0} */


   int16_t aec_pc_sPathchange_Flag[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  {Flag for a path change.}
		@h2xmle_default	{0} */


   int16_t aec_pc_sAlert[AUDPROC_LEC_MAX_AEC_SUBFRAMES][AUDPROC_LEC_MAX_RX_LEC_CHS];
   /**< @h2xmle_description  { Counter for a path change alert.}
		@h2xmle_default	{0} */


   int16_t aec_pc_dlAlert[AUDPROC_LEC_MAX_AEC_SUBFRAMES][AUDPROC_LEC_MAX_RX_LEC_CHS];
   /**< @h2xmle_description  {Counter for a path change download alert.}
		@h2xmle_default	{0} */


   int16_t aec_BGFGFlag[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  {Convergence download flag for a background-foreground-only test.}
		@h2xmle_default	{0} */


   int16_t aec_ERLEDTDFlag[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  {Convergence download flag for a double-talk detector test.}
		@h2xmle_default	{0} */


   int16_t aec_ERLEonlyflag[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  {Convergence download flag for an ERLE test.}
		@h2xmle_default	{0} */


   int16_t aec_f_vad_sFlagVAD[AUDPROC_LEC_MAX_RX_LEC_CHS];
   /**< @h2xmle_description  {Far-end low-band VAD. }
		@h2xmle_default	{0} */


   int16_t aec_f_vad_hb_sFlagVAD[AUDPROC_LEC_MAX_RX_LEC_CHS];
   /**< @h2xmle_description  {Far-end high-band VAD.}
		@h2xmle_default	{0} */


   int16_t aec_sDtd_Flag_Lec[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  { DTD flag.}
		@h2xmle_default	{0} */


   int32_t aec_lEn_back_sm[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  {Smoothed energy of the back filter output.}
		@h2xmle_default	{0} */


   int32_t aec_lEn_front_sm[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  {Smoothed energy of the front filter output. }
		@h2xmle_default	{0} */


   int32_t aec_lEn_mic_sm[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  {Smoothed energy of mic input.}
		@h2xmle_default	{0} */


   int32_t aec_sDtd_statQ31[AUDPROC_LEC_MAX_AEC_SUBFRAMES];
   /**< @h2xmle_description  {Double-talk detector statistics.}
		@h2xmle_default	{0} */


}
#include "adsp_end_pack.h"
;


/** @h2xmlp_subStruct */

 #include "adsp_begin_pack.h"
typedef struct AecPathStruct
{
	uint16_t sFlen;
	/**< @h2xmle_description {Number of taps in left channel of plWgtFg and plWgtBg filters for a particular Aec path .Values are [0-200].It will be repeated if the value of NumRxActiveChannel will be 2.\n } */

    uint16_t sWgtQ;
	/**< @h2xmle_description {Q factor [0-15]} */

	int16_t plWgtFg[0];
	/**< @h2xmle_description {Taps for plWgtFg filter for the particular AecPath.It will be repeated if the value of NumRxActiveChannel will be 2.\n}
	     @h2xmle_variableArraySize {"sFlen"}  */

    int16_t plWgtBg[0];
	/**< @h2xmle_description {Taps for plWgtBg filter for the particular AecPath.It will be repeated if the value of NumRxActiveChannel will be 2.\n}
	     @h2xmle_variableArraySize {"sFlen"}  */


}AecPathStruct
#include "adsp_end_pack.h"
;

/* ID of the LEC AF coefficients used by #AUDPROC_MODULE_ID_LEC.

    The payload structure is variable, depending on the number of channels.
  payload is dynamic based on number of channels and such structures cannot be defined in code. Refer to the CCB.*/
#define AUDPROC_PARAM_ID_LEC_AF_COEFFS                 0x0001092B

typedef struct audproc_lec_af_coeff_param_t audproc_lec_af_coeff_param_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_LEC_AF_COEFFS", AUDPROC_PARAM_ID_LEC_AF_COEFFS}
    @h2xmlp_description {ID of the LEC AF coefficients used by #AUDPROC_MODULE_ID_LEC.\n
    The payload structure is variable, depending on the number of channels.\n
  payload is dynamic based on number of channels and such structures cannot be defined in code. Refer to the CCB.\n}
    @h2xmlx_expandStructs	{false}
    @h2xmlp_readOnly    {true}
    @h2xmlp_toolPolicy  {RTM} */


 #include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_LEC_DELAY
 parameter used by the linear echo canceller module
 */
struct audproc_lec_af_coeff_param_t
 {
    int16_t NumAecPaths;
   /**< @h2xmle_description  {Number of Aec Paths  [0-1].\n} */



    int16_t NumActiveRxChannels;
   /**< @h2xmle_description  {Number of active reference channels. \n} */


    AecPathStruct Aec_Path_Data[1];
   /**< @h2xmle_description  {NumAecPaths structures defined by table below.\n}  */


}
#include "adsp_end_pack.h"
;


/**  @}                   <-- End of the Module --> */

#endif
