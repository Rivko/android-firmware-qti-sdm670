#ifndef __SPEAKER_PROTECTION_H__
#define __SPEAKER_PROTECTION_H__

/*==============================================================================
  @file speaker_protection.h
  @brief This file contains APIs for Speaker Protection Module

  Copyright (c) 2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or
  software is regulated by the U.S. Government, Diversion contrary to U.S.
  law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/prop/speaker_protection.h#3 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
==============================================================================*/

/** @h2xml_title1           {Speaker Protection}
    @h2xml_title_date       {September 11, 2017} */

#include "mmdefs.h"
#include "afe_common.h"

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Module ID */
#define AFE_MODULE_SPEAKER_PROTECTION_RX    0x0001025F

/** @h2xmlm_module       {"AFE_MODULE_SPEAKER_PROTECTION_RX",
                          AFE_MODULE_SPEAKER_PROTECTION_RX}
    @h2xmlm_toolPolicy   {Calibration;RTC_READONLY}
    @h2xmlm_description  {Speaker Protection Module}
    @{                   <-- Start of the Module --> */

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_PTONE_RAMP_DN_CFG   0x00010260

/*==============================================================================
   Type definitions
==============================================================================*/

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_PTONE_RAMP_DN_CFG",
                         AFE_PARAM_ID_SP_RX_PTONE_RAMP_DN_CFG}
    @h2xmlp_description {Used to configure the pilot tone Ramp Down mode of
                         speaker protection Rx processing. This parameter must
                         be sent at least 15 ms before a port stop.}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */

/*==============================================================================
   Constants
==============================================================================*/

/* Maximum number of stages in the notch filter. */
#define SP_NCH_FILT_STAGES_MAX 5

/* Maximum number of taps in the FIR filter. */
#define SP_MAX_FIR_TAP 48

/* Maximum number of speakers supported in speaker protection Rx
    processing. */
#define SP_NUM_MAX_SPKRS 2

/* Number of plotting samples in one thermal Rx output packet. */
#define SP_TH_RX_DEMO_SMPL_PER_PKT 10

/* Number of plotting samples in one excursion Rx output packet. */
#define SP_EX_RX_DEMO_SMPL_PER_PKT  10

/* Number of plotting samples in one audio clip manager Rx output packet. */
#define SP_ACM_SMPL_PER_DEMO_PKT 10

/* Number of plotting samples in one output packet. */
#define SP_AGC_DEMO_SMPL_PER_PKT     10

/* Number of FFT bins in Real-Time Monitoring (RTM) statistics. */
#define SP_NUM_FFT_BINS 256

/* Maximum number of stages in the IIR filter. */
#define SP_IIR_TDF2_STAGES_MAX 5

/* Number of numerators per stage in the IIR filter. */
#define SP_IIR_TDF2_NUM_NUM_PER_STAGE 3

/* Number of denominators per stage in the IIR filter. */
#define SP_IIR_TDF2_NUM_DEN_PER_STAGE 2

/* Factor used to represent a complex value. */
#define SP_FACT_CMPLX 2

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_STATIC_CFG        0x00010261

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_STATIC_CFG     0x2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_static_cfg_v2_t afe_sp_rx_static_cfg_v2_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_STATIC_CFG",
                         AFE_PARAM_ID_SP_RX_STATIC_CFG}
    @h2xmlp_description {parameter used to configure the static configuration of
                         speaker protection Rx processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_STATIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_static_cfg_v2_t
{
  uint32_t sp_rx_static_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_STATIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_STATIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t features;
  /**< @h2xmle_description {Features enabled in the speaker protection
                            algorithm.}
       @h2xmle_range       {0..0x3f}

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"Notch high-pass filter"}
       @h2xmle_description {Notch high-pass filter}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"Thermal protection"}
       @h2xmle_description {Thermal protection}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000004}
       @h2xmle_bitName     {"Feedback excursion control"}
       @h2xmle_description {Feedback excursion control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000008}
       @h2xmle_bitName     {"Analog clip manager"}
       @h2xmle_description {Analog clip manager}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000010}
       @h2xmle_bitName     {"Feedforward excursion control"}
       @h2xmle_description {Feedforward excursion control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000020}
       @h2xmle_bitName     {"Automatic gain control"}
       @h2xmle_description {Automatic gain control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd  */

  uint32_t tuning_mode_en_flag;
  /**< @h2xmle_description {Specifies whether RTM tuning mode is enabled.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0} */

  uint32_t ctrl_rate;
  /**< @h2xmle_description {Thermal control rate.}
       @h2xmle_rangeList   {"1000hz"=1000}
       @h2xmle_default     {1000} */

  uint32_t num_nch_filter_stages;
  /**< @h2xmle_description {Number of notch filter stages.}
       @h2xmle_range       {0..SP_NCH_FILT_STAGES_MAX}
       @h2xmle_default     {2} */

  int16_t temp_loop_dly_ms;
  /**< @h2xmle_description {Loop delay from the predicted temperature to the
                            temperature information coming back.}
       @h2xmle_range       {0..1000}
       @h2xmle_default     {120} */

  int16_t pow_est_smooth_flag;
  /**< @h2xmle_description {Specifies whether to use IIR filtering to smooth the
                            power estimation.}
       @h2xmle_rangeList   {"Do not use IIR filtering"=0;
                            "Use IIR filtering"=1}
       @h2xmle_default     {1} */

  int16_t num_bs_filter_stages;
  /**< @h2xmle_description {Number of stages for band split LPF and HPF.}
       @h2xmle_range       {1..10}
       @h2xmle_default     {3} */

  int16_t num_ex_filter_taps;
  /**< @h2xmle_description {Number of taps for the FIR excursion prediction
                            filter.}
       @h2xmle_rangeList   {"24"=24; "32"=32; "40"=40; "48"=48}
       @h2xmle_default     {48} */

  uint16_t pt_lvl_switch_en;
  /**< @h2xmle_description {Specifies whether to enable the dynamic pilot tone
                            level switch.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0} */


  uint16_t pt_masking_delay_ms;
  /**< @h2xmle_description {Delay in ramping down the pilot tone.}
       @h2xmle_range       {0..1000}
       @h2xmle_default     {13} */

  int32_t pt_masking_thr_q27;
  /**< @h2xmle_description {Specifies the input level threshold below which the
                            pilot tone is disabled.}
       @h2xmle_range       {0..134217727}
       @h2xmle_dataFormat  {Q27}
       @h2xmle_default     {189813} */


  uint32_t agc_delay_ms;
  /**< @h2xmle_description {Specifies the agc delay in millisecond (ms).}
       @h2xmle_range       {0..0xa}
       @h2xmle_default     {0xa} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_STATIC_CFG_V1     0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_static_cfg_v1_t afe_sp_rx_static_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_STATIC_CFG_V1",
                         AFE_PARAM_ID_SP_RX_STATIC_CFG}
    @h2xmlp_description {parameter used to configure the static configuration of
                         speaker protection v2 Rx processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_STATIC_CFG_V1}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_static_cfg_v1_t
{
  uint32_t sp_rx_static_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_STATIC_CFG_V1}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_STATIC_CFG_V1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t features;
  /**< @h2xmle_description {Features enabled in the speaker protection
                            algorithm.}
       @h2xmle_range       {0..0x1f}

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"Notch high-pass filter"}
       @h2xmle_description {Notch high-pass filter}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"Thermal protection"}
       @h2xmle_description {Thermal protection}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000004}
       @h2xmle_bitName     {"Feedback excursion control"}
       @h2xmle_description {Feedback excursion control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000008}
       @h2xmle_bitName     {"Analog clip manager"}
       @h2xmle_description {Analog clip manager}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000010}
       @h2xmle_bitName     {"Feedforward excursion control"}
       @h2xmle_description {Feedforward excursion control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd */

  uint32_t tuning_mode_en_flag;
  /**< @h2xmle_description {Specifies whether RTM tuning mode is enabled.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0} */

  uint32_t ctrl_rate;
  /**< @h2xmle_description {Thermal control rate.}
       @h2xmle_rangeList   {"1000hz"=1000}
       @h2xmle_default     {1000} */

  uint32_t num_nch_filter_stages;
  /**< @h2xmle_description {Number of notch filter stages.}
       @h2xmle_range       {0..SP_NCH_FILT_STAGES_MAX}
       @h2xmle_default     {2} */

  int16_t temp_loop_dly_ms;
  /**< @h2xmle_description {Loop delay from the predicted temperature to the
                            temperature information coming back.}
       @h2xmle_range       {0..1000}
       @h2xmle_default     {120} */

  int16_t pow_est_smooth_flag;
  /**< @h2xmle_description {Specifies whether to use IIR filtering to smooth the
                            power estimation.}
       @h2xmle_rangeList   {"Do not use IIR filtering"=0;
                            "Use IIR filtering"=1}
       @h2xmle_default     {1} */

  int16_t num_bs_filter_stages;
  /**< @h2xmle_description {Number of stages for band split LPF and HPF.}
       @h2xmle_range       {1..10}
       @h2xmle_default     {3} */

  int16_t num_ex_filter_taps;
  /**< @h2xmle_description {Number of taps for the FIR excursion prediction
                            filter.}
       @h2xmle_rangeList   {"24"=24; "32"=32; "40"=40; "48"=48}
       @h2xmle_default     {48} */

  uint16_t pt_lvl_switch_en;
  /**< @h2xmle_description {Specifies whether to enable the dynamic pilot tone
                            level switch.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0} */


  uint16_t pt_masking_delay_ms;
  /**< @h2xmle_description {Delay in ramping down the pilot tone.}
       @h2xmle_range       {0..1000}
       @h2xmle_default     {13} */

  int32_t pt_masking_thr_q27;
  /**< @h2xmle_description {Specifies the input level threshold below which the
                            pilot tone is disabled.}
       @h2xmle_range       {0..134217727}
       @h2xmle_dataFormat  {Q27}
       @h2xmle_default     {189813} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_FBSP_MODE_RX_CFG       0x0001021D

/* Minor Version of this PARAM */
#define AFE_API_VERSION_FBSP_MODE_RX_CFG    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_fbsp_mode_rx_cfg_v1_t afe_fbsp_mode_rx_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_FBSP_MODE_RX_CFG",
                         AFE_PARAM_ID_FBSP_MODE_RX_CFG}
    @h2xmlp_description {parameter used to configure the mode of feedback
                         speaker protection Rx processing}
    @h2xmlp_version     {AFE_API_VERSION_FBSP_MODE_RX_CFG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_fbsp_mode_rx_cfg_v1_t
{
  uint32_t sp_rx_mode_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_FBSP_MODE_RX_CFG}
       @h2xmle_default     {AFE_API_VERSION_FBSP_MODE_RX_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t mode;
  /**< @h2xmle_description {Mode of feedback speaker protection Rx processing}
       @h2xmle_rangeList   {"Normal processing"=0;"Thermal calibration"=1} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_DYNAMIC_CFG       0x00010262

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_DYNAMIC_CFG    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_dynamic_cfg_v1_t afe_sp_rx_dynamic_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_DYNAMIC_CFG",
                         AFE_PARAM_ID_SP_RX_DYNAMIC_CFG}
    @h2xmlp_description {parameter used to configure dynamic parameters for
                         Speaker Protection Rx module processing}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_DYNAMIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_dynamic_cfg_v1_t
{
  uint32_t  sp_rx_dynamic_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_DYNAMIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_DYNAMIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t  speakers_link_mode;
  /**< @h2xmle_description {Specifies whether the gains applied to the two
                            speakers are linked.}
       @h2xmle_rangeList   {Unlinked=0; Linked=1}
       @h2xmle_default     {0} */

  uint32_t  left_pi_scale_u16q18;
  /**< @h2xmle_description {Left speaker: scale down a full scale 40 Hz pilot
                            tone by this value.}
       @h2xmle_dataFormat  {Q18}
       @h2xmle_range       {0..65535}
       @h2xmle_default     {5230} */

  uint32_t  left_rx_scale_u16q16;
  /**< @h2xmle_description {Left speaker: scale down the outgoing signal by this
                            value so when the pilot tone is added, the signal
                            does not exceed full scale.}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_range       {32768..65535}
       @h2xmle_default     {64228} */

  uint32_t  right_pi_scale_u16q18;
  /**< @h2xmle_description {Right speaker: scale down a full scale 40 Hz pilot
                            tone by this value.}
       @h2xmle_dataFormat  {Q18}
       @h2xmle_range       {0..65535}
       @h2xmle_default     {5230} */

  uint32_t  right_rx_scale_u16q16;
  /**< @h2xmle_description {Right speaker: scale down the outgoing signal by
                            this value so when the pilot tone is added, the
                            signal does not exceed full scale.}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_range       {32768..65535}
       @h2xmle_default     {64228} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_NOTCH_DYNAMIC_CFG       0x00010263

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_NOTCH_DYNAMIC_CFG    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_notch_dynamic_cfg_v1_t
               afe_sp_rx_notch_dynamic_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_NOTCH_DYNAMIC_CFG",
                         AFE_PARAM_ID_SP_RX_NOTCH_DYNAMIC_CFG}
    @h2xmlp_description {Parameter used to configure notch dynamic parameters
                         for Feedback Speaker Protection Rx module
                         processing}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_NOTCH_DYNAMIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_notch_dynamic_cfg_v1_t
{
  uint32_t sp_rx_notch_dynamic_cfg_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_NOTCH_DYNAMIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_NOTCH_DYNAMIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t num[SP_NCH_FILT_STAGES_MAX * SP_IIR_TDF2_NUM_NUM_PER_STAGE];
  /**< @h2xmle_description {Numerator coefficients for notch filter
                            configuration.}
       @h2xmle_range       {-2147483648..2147483647}
       @h2xmle_defaultList {2126619312, -2126590850, 0, 1063193941, -2126402111,
                            1063208171, 0, 0, 0, 0, 0, 0, 0, 0, 0} */

  int32_t den[SP_NCH_FILT_STAGES_MAX * SP_IIR_TDF2_NUM_DEN_PER_STAGE];
  /**< @h2xmle_description {Denominator coefficients for notch filter
                            configuration.}
       @h2xmle_defaultList {-2105726514, 0, -2126197149, 1052865249, 0, 0, 0, 0,
                            0, 0} */

  int16_t num_q[SP_NCH_FILT_STAGES_MAX];
  /**< @h2xmle_description {Q factor of numerator coefficients for notch filter
                            configuration.}
       @h2xmle_range       {-32..32}
       @h2xmle_defaultList {0, 1, 0, 0, 0} */

  int16_t den_q[SP_NCH_FILT_STAGES_MAX];
  /**< @h2xmle_description {Q factor of denominator coefficients for notch
                            filter configuration.}
       @h2xmle_defaultList {0, 1, 0, 0, 0} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_EX_DYNAMIC_CFG         0x00010264

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_EX_DYNAMIC_CFG      0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_ex_dynamic_cfg_v1_t
               afe_sp_rx_ex_dynamic_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_EX_DYNAMIC_CFG",
                         AFE_PARAM_ID_SP_RX_EX_DYNAMIC_CFG}
    @h2xmlp_description {Parameter used to configure dynamic parameters for
                         Feedback Speaker Protection Rx module processing
                         related to exertion}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_EX_DYNAMIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_ex_dynamic_cfg_v1_t
{
  uint32_t sp_rx_ex_dynamic_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_EX_DYNAMIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_EX_DYNAMIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t excursion_gain_recovery_constant_q31;
  /**< @h2xmle_description {Excursion gain release constant.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q31}
       @h2xmle_default     {0x7A3D70A4} */

  int32_t limiter_gain_recovery_constant_q31;
  /**< @h2xmle_description {Limiter gain release constant.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q31}
       @h2xmle_default     {0x7CCCCCCD} */

  int32_t left_amp_gain_q24;
  /**< @h2xmle_description {Left speaker: amplifier gain used when scaling the
                            Tx Vsens signal to an Rx signal.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {0x05A82799} */

  uint32_t left_ptone_excursion_q16;
  /**< @h2xmle_description {Left speaker: excursion for the pilot tone. This
                            excursion is added to the computed peak excursion.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {0x02E4} */

  int32_t left_xmax_q24;
  /**< @h2xmle_description {Left speaker: maximum excursion value.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {0x451EB8} */

  int16_t left_fir_taps;
  /**< @h2xmle_description {Left FIR: Number of taps.}
       @h2xmle_rangeList   {"24"=24; "32"=32; "40"=40; "48"=48}
       @h2xmle_default     {48} */

  int16_t left_fir_coeffq;
  /**< @h2xmle_description {Left FIR: Q factor of coefficients.}
       @h2xmle_default     {0x23} */

  int32_t left_fir_coeffs[SP_MAX_FIR_TAP];
  /**< @h2xmle_description {Left FIR: coefficients.}
       @h2xmle_range       {-2147483648..2147483647}
       @h2xmle_defaultList {0x33A1EC, 0xDC758B19, 0xC23F48CF, 0xBD7E314C,
                            0xC9018DEA, 0xDFD7FDD3, 0xF9E191EB, 0x1113ED0E,
                            0x206540CC, 0x264FE900, 0x2321907E, 0x19AABF6E,
                            0x0D1A2C79, 0x011719BE, 0xF804AE90, 0xF37C5AFE,
                            0xF36623EF, 0xF7012767, 0xFC9037BA, 0x028BC4ED,
                            0x075B93FC, 0x0A4402FF, 0x0ADA19E1, 0x098FCE76,
                            0x06EC3E53, 0x03E65E3D, 0x011C1AA9, 0xFF389DD3,
                            0xFE56021E, 0xFE87F241, 0xFF67E79B, 0xB40B67,
                            0x01E994EB, 0x02DAE219, 0x033E83F9, 0x032DE14D,
                            0x02A43075, 0x01EB95F1, 0x01179952, 0x6F9011,
                            0xFFF15ADA, 0xFFC29504, 0xFFBF3E37, 0xFFF0D2BD,
                            0x23C3B7, 0x5DEBDE, 0x73E377, 0x79CB74} */

  int16_t left_lpf_num_ch;
  /**< @h2xmle_description {Left LPF: number of channels.}
       @h2xmle_range       {1..2}
       @h2xmle_default     {0x01} */

  int16_t left_lpf_stages;
  /**< @h2xmle_description {Left LPF: number of stages.}
       @h2xmle_range       {1..10}
       @h2xmle_default     {0x03} */

  int32_t left_lpf_num[SP_IIR_TDF2_STAGES_MAX *
                       SP_IIR_TDF2_NUM_NUM_PER_STAGE];
  /**< @h2xmle_description {Left LPF: numerator.}
       @h2xmle_defaultList {0x508BEC28, 0x508BEC28, 0x0, 0x40000000, 0xA8AD098A,
                            0x40000000, 0x40000000, 0x918AEB3C, 0x40000000, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0 } */

  int32_t left_lpf_den[SP_IIR_TDF2_STAGES_MAX *
                       SP_IIR_TDF2_NUM_DEN_PER_STAGE];
  /**< @h2xmle_description {Left LPF: denominator.}
       @h2xmle_defaultList {0x9DC83AB1, 0x0, 0x97F51086, 0x2BB683A5, 0x8B8CD517,
                            0x388ECAB0, 0x0, 0x0, 0x0, 0x0 } */

  int16_t left_lpf_num_q[SP_IIR_TDF2_STAGES_MAX];
  /**< @h2xmle_description {Left LPF: Q factor of numerator coefficients.}
       @h2xmle_defaultList {0xFFF8, 0x01, 0x01, 0x0, 0x0 } */

  int16_t left_lpf_den_q[ SP_IIR_TDF2_STAGES_MAX];
  /**< @h2xmle_description {Left LPF: Q factor of denominator coefficients.}
       @h2xmle_defaultList {0x0, 0x01, 0x01, 0x0, 0x0 } */

  int16_t left_hpf_num_ch;
  /**< @h2xmle_description {Left HPF: number of channels.}
       @h2xmle_range       {1..2}
       @h2xmle_default     {0x01} */

  int16_t left_hpf_stages;
  /**< @h2xmle_description {Left HPF: number of stages.}
       @h2xmle_range       {1..10}
       @h2xmle_default     {0x03} */

  int32_t left_hpf_num[SP_IIR_TDF2_STAGES_MAX *
                       SP_IIR_TDF2_NUM_NUM_PER_STAGE];
  /**< @h2xmle_description {Left HPF: numerator.}
       @h2xmle_defaultList {0xA8E384A2, 0x571C7B5E, 0x0, 0x40000000, 0x80680DFC,
                            0x40000000, 0x40000000, 0x810A8708, 0x40000000, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0 } */

  int32_t left_hpf_den[SP_IIR_TDF2_STAGES_MAX *
                       SP_IIR_TDF2_NUM_DEN_PER_STAGE];
  /**< @h2xmle_description {Left HPF: denominator.}
       @h2xmle_defaultList {0x9DC83AB1 , 0x0 , 0x97F51086 , 0x2BB683A5 ,
                            0x8B8CD517 , 0x388ECAB0 , 0x0 , 0x0 , 0x0 , 0x0} */

  int16_t left_hpf_num_q[SP_IIR_TDF2_STAGES_MAX];
  /**< @h2xmle_description {Left HPF: Q factor of numerator coefficients.}
       @h2xmle_defaultList {0x0 , 0x01 , 0x01 , 0x0 , 0x0 } */

  int16_t left_hpf_den_q[SP_IIR_TDF2_STAGES_MAX];
  /**< @h2xmle_description {Left HPF: Q factor of denominator coefficients.}
       @h2xmle_defaultList {0x0, 0x01, 0x01, 0x0, 0x0 } */

  int32_t left_xmax_margin_q24;
  /**< @h2xmle_description {Left speaker: margin used to modify the overall
                            limiter gain.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {0x01000000} */

  int32_t left_lim_thr_q27;
  /**< @h2xmle_description {Left speaker: limiter threshold.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q27}
       @h2xmle_default     {0x069DACAD} */

  int32_t right_amp_gain_q24;
  /**< @h2xmle_description {Right speaker: amplifier gain used to scale the Tx
                            Vsens signal to the Rx signal.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {0x05A82799} */

  uint32_t right_ptone_excursion_q16;
  /**< @h2xmle_description {Right speaker: excursion for the pilot tone. This
                            excursion is added to  the computed peak excursion.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {0x02AC} */

  int32_t right_xmax_q24;
  /**< @h2xmle_description {Right speaker: xmax value.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {0x451EB8} */

  int16_t right_fir_taps;
  /**< @h2xmle_description {Right FIR: Number of taps in the right FIR.}
       @h2xmle_rangeList   {"24"=24; "32"=32; "40"=40; "48"=48}
       @h2xmle_default     {48} */

  int16_t right_fir_coeffq;
  /**< @h2xmle_description {Right FIR: Q factor of coefficients.}
       @h2xmle_default     {0x24} */

  int32_t right_fir_coeffs[SP_MAX_FIR_TAP];
  /**< @h2xmle_description {Right FIR: coefficients.}
       @h2xmle_defaultList {0xFFED957D, 0xBEFB0A61, 0x9087A025, 0x8A21AF35,
                            0xA19703D4, 0xCCE06CC7, 0xFC69AC77, 0x24E6B496,
                            0x3D82B6DA, 0x4447E828, 0x3AD01A14, 0x2727E125,
                            0x0FAC4AA7, 0xFB101B72, 0xED3EBD89, 0xE85BCEA2,
                            0xEB5E4FD9, 0xF4288BBB, 0xFF226E69, 0x09750FE6,
                            0x108795B6, 0x138D07C5, 0x126062EF, 0x0E5744FD,
                            0x08BBCF90, 0x0360F963, 0xFF3CA96D, 0xFD316B74,
                            0xFD0B721F, 0xFE945344, 0xD6DC8C, 0x03474877,
                            0x050B43C4, 0x05FE56FE, 0x05D70A56, 0x04FB2858,
                            0x038A48A5, 0x021BA338, 0xCCACE7, 0x06466E,
                            0xFFA33897, 0xFFC5FB3A, 0x13E7A8, 0x910D83,
                            0xE0B065, 0x0118F396, 0xFD728C, 0xC5CFF9 } */

  int16_t right_lpf_num_ch;
  /**< @h2xmle_description {Right LPF: number of channels.}
       @h2xmle_range       {1..2}
       @h2xmle_default     {0x01} */

  int16_t right_lpf_stages;
  /**< @h2xmle_description {Right LPF: number of stages.}
       @h2xmle_range       {1..10}
       @h2xmle_default     {0x03} */

  int32_t right_lpf_num[SP_IIR_TDF2_STAGES_MAX *
                        SP_IIR_TDF2_NUM_NUM_PER_STAGE];
  /**< @h2xmle_description {Right LPF: numerator.}
       @h2xmle_defaultList {0x508BEC28, 0x508BEC28, 0x0, 0x40000000, 0xA8AD098A,
                            0x40000000, 0x40000000, 0x918AEB3C, 0x40000000, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0 } */

  int32_t right_lpf_den[SP_IIR_TDF2_STAGES_MAX *
                        SP_IIR_TDF2_NUM_DEN_PER_STAGE];
  /**< @h2xmle_description {Right LPF: denominator.}
       @h2xmle_defaultList {0x9DC83AB1, 0x0, 0x97F51086, 0x2BB683A5, 0x8B8CD517,
                            0x388ECAB0, 0x0, 0x0, 0x0, 0x0 } */

  int16_t right_lpf_num_q[SP_IIR_TDF2_STAGES_MAX];
  /**< @h2xmle_description {Right LPF: Q factor of numerator coefficients.}
       @h2xmle_defaultList {0xFFF8, 0x01, 0x01, 0x0, 0x0 } */

  int16_t right_lpf_den_q[SP_IIR_TDF2_STAGES_MAX];
  /**< @h2xmle_description {Right LPF: Q factor of denominator coefficients.}
       @h2xmle_defaultList {0x0, 0x01, 0x01, 0x0, 0x0 } */

  int16_t right_hpf_num_ch;
  /**< @h2xmle_description {Right HPF: number of channels.}
       @h2xmle_range       {1..2}
       @h2xmle_default     {0x01} */

  int16_t right_hpf_stages;
  /**< @h2xmle_description {Right HPF: number of stages.}
       @h2xmle_range       {1..10}
       @h2xmle_default     {0x03} */

  int32_t right_hpf_num[SP_IIR_TDF2_STAGES_MAX *
                        SP_IIR_TDF2_NUM_NUM_PER_STAGE];
  /**< @h2xmle_description {Right HPF: numerator.}
       @h2xmle_defaultList {0xA8E384A2, 0x571C7B5E, 0x0, 0x40000000, 0x80680DFC,
                            0x40000000, 0x40000000, 0x810A8708, 0x40000000, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0 } */

  int32_t right_hpf_den[SP_IIR_TDF2_STAGES_MAX *
                        SP_IIR_TDF2_NUM_DEN_PER_STAGE];
  /**< @h2xmle_description {Right HPF: denominator.}
       @h2xmle_defaultList {0x9DC83AB1, 0x0, 0x97F51086, 0x2BB683A5, 0x8B8CD517,
                            0x388ECAB0, 0x0, 0x0, 0x0, 0x0 } */

  int16_t right_hpf_num_q[SP_IIR_TDF2_STAGES_MAX];
  /**< @h2xmle_description {Right HPF: Q factor of numerator coefficients.}
       @h2xmle_defaultList {0x0, 0x01, 0x01, 0x0, 0x0 } */

  int16_t right_hpf_den_q[SP_IIR_TDF2_STAGES_MAX];
  /**< @h2xmle_description {Right HPF: Q factor of denominator coefficients.}
       @h2xmle_defaultList {0x0, 0x01, 0x01, 0x0, 0x0 } */

  int32_t right_xmax_margin_q24;
  /**< @h2xmle_description {Right HPF: margin used to modify overall limiter
                            gain.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {0x01000000} */


  int32_t right_lim_thr_q27;
  /**< @h2xmle_description {Right speaker: limiter threshold.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q27}
       @h2xmle_default     {0x069DACAD} */

}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_TH_DYNAMIC_CFG        0x00010265

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_TH_DYNAMIC_CFG     0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_th_dynamic_cfg_v1_t
               afe_sp_rx_th_dynamic_cfg_v1_t;


/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_TH_DYNAMIC_CFG",
                         AFE_PARAM_ID_SP_RX_TH_DYNAMIC_CFG}
    @h2xmlp_description {Parameter used to configure dynamic parameters for
                         Feedback Speaker Protection Rx module processing
                         related to thermal}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_TH_DYNAMIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_th_dynamic_cfg_v1_t
{
  uint32_t sp_rx_th_dynamic_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_TH_DYNAMIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_TH_DYNAMIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t temp_control_mthd;
  /**< @h2xmle_description {Temperature control method.}
       @h2xmle_rangeList   {"Use feedback temperature only"=0;
                            "Feedback with low pass"=1;
                            "Use feedforward estimated temperature"=2}
       @h2xmle_default     {2} */

  int32_t temp_nominal_q6;
  /**< @h2xmle_description {Assumed voice coil temperature at startup. (in
                            degree C)}
       @h2xmle_range       {-1920..12800}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {3200} */

  int32_t temp_min_q6;
  /**< @h2xmle_description {Minimum reasonable measured temperature. A
                            temperature lower than this value is ignored. (in
                            degree C)}
       @h2xmle_range       {-1920..12800}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {-1920} */

  int32_t temp_max_q6;
  /**< @h2xmle_description {Maximum reasonable measured temperature. A
                            temperature higher than this value is ignored.(in
                            degree C)}
       @h2xmle_range       {-1920..12800}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {12800} */

  int32_t temp_max_chg_sec_q6;
  /**< @h2xmle_description {Maximum temperature change per second. A new
                            temperature is ignored if it falls outside this
                            range.(Set 0 to disables this feature)}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {4096} */

  int32_t temp_smoothing_factor_q31;
  /**< @h2xmle_description {Smoothing of the temperature to compute the gain.}
       @h2xmle_dataFormat  {Q31}
       @h2xmle_default     {6735944} */

  int32_t max_attenuation_db_q7;
  /**< @h2xmle_description {Minimum temperature control gain.}
       @h2xmle_range       {-7680..0}
       @h2xmle_dataFormat  {Q7}
       @h2xmle_default     {-1536} */

  int32_t attack_gain_smoothing_factor;
  /**< @h2xmle_description {Smoothing factor for adjusting gain during the
                            attack phase (current gain less than previous
                            gain).}
       @h2xmle_range       {328..32767}
       @h2xmle_default     {32767} */

  int32_t release_gain_smoothing_factor;
  /**< @h2xmle_description {Smoothing factor for adjusting gain during the
                            release phase (current gain greater than previous
                            gain).}
       @h2xmle_range       {328..32767}
       @h2xmle_default     {32767} */

  int32_t temp_dead_zone_q6;
  /**< @h2xmle_description {Dead zone (region near the target temperature) for a
                            reduced proportional term.(in degree C)}
       @h2xmle_range       {2..2048}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {64} */

  int32_t dead_zone_scaling_q15;
  /**< @h2xmle_description {Scaling factor to modify the proportional term.}
       @h2xmle_range       {328..32767}
       @h2xmle_dataFormat  {Q15}
       @h2xmle_default     {32767} */

  int32_t clip_time_out_ms;
  /**< @h2xmle_description {Timeout in milli seconds used for processing a clip
                            interrupt.(value greater than 0 enables the clip
                            interrupt timeout)}
       @h2xmle_range       {0..4000}
       @h2xmle_default     {0} */

  int32_t vbat_time_out_ms;
  /**< @h2xmle_description {Timeout in milli seconds used to process a VBAT
                            interrupt. (value greater than 0 enables the VBAT
                            interrupt timeout)}
       @h2xmle_range       {0..1000000}
       @h2xmle_default     {300000} */

  int32_t left_r_spk_coil_q8;
  /**< @h2xmle_description {Left speaker: typical resistance.}
       @h2xmle_range       {512..16384}
       @h2xmle_dataFormat  {Q8}
       @h2xmle_default     {2048} */

  int32_t left_v_full_scale_q10;
  /**< @h2xmle_description {Left speaker: voltage for a digitally full scale
                            signal.}
       @h2xmle_range       {512..15360}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {5477} */

  int32_t left_thermal_resistance_q6;
  /**< @h2xmle_description {Left speaker: total thermal resistance (such as
                            voice coil + magnet).}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {3200} */

  int32_t left_thermal_time_constant_low_q10;
  /**< @h2xmle_description {Left speaker: low estimate of the voice coil thermal
                            time constant. (in seconds)}
       @h2xmle_range       {51..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {1024} */

  int32_t left_thermal_time_constant_high_q10;
  /**< @h2xmle_description {Left speaker: high estimate of voice coil thermal
                            time constant. (in seconds)}
       @h2xmle_range       {51..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {3482} */

  int32_t left_temp_tgt_q6;
  /**< @h2xmle_description {Left speaker: target temperature.(in degrees C)}
       @h2xmle_range       {-1920..12800}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {5120} */

  int32_t left_pi_ctrl_prop_value_q12;
  /**< @h2xmle_description {Left speaker: value of the proportional term in the
                            PI controller.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q12}
       @h2xmle_default     {5734} */

  int32_t left_pi_ctrl_intg_value_q12;
  /**< @h2xmle_description {Left speaker: value of the integral term in the PI
                            controller.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q12}
       @h2xmle_default     {2294} */

  int32_t right_r_spk_coil_q8;
  /**< @h2xmle_description {Right speaker: typical resistance.}
       @h2xmle_range       {512..16384}
       @h2xmle_dataFormat  {Q8}
       @h2xmle_default     {2048} */

  int32_t right_v_full_scale_q10;
  /**< @h2xmle_description {Right speaker: speaker voltage for digitally full
                            scale signal.}
       @h2xmle_range       {512..15360}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {5477} */

  int32_t right_thermal_resistance_q6;
  /**< @h2xmle_description {Right speaker: total thermal resistance (such as
                            voice coil + magnet).}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {3200} */


  int32_t right_thermal_time_constant_low_q10;
  /**< @h2xmle_description {Right speaker: low estimate of the voice coil
                            thermal time constant.}
       @h2xmle_range       {51..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {1024} */

  int32_t right_thermal_time_constant_high_q10;
  /**< @h2xmle_description {Right speaker: high estimate of the voice coil
                            thermal time constant.}
       @h2xmle_range       {51..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {3482} */

  int32_t right_temp_tgt_q6;
  /**< @h2xmle_description {Right speaker: target temperature.(in degrees C)}
       @h2xmle_range       {-1920..12800}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {5120} */

  int32_t right_pi_ctrl_prop_value_q12;
  /**< @h2xmle_description {Right speaker: value of the proportional term in
                            PI controller.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q12}
       @h2xmle_default     {5734} */

  int32_t right_pi_ctrl_intg_value_q12;
  /**< @h2xmle_description {Right speaker: value of the integral term in the PI
                            controller.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q12}
       @h2xmle_default     {2294} */

}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_ACM_DYNAMIC_CFG           0x00010266

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_ACM_DYNAMIC_CFG        0x1


/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_acm_dynamic_cfg_v1_t
               afe_sp_rx_acm_dynamic_cfg_v1_t;


/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_ACM_DYNAMIC_CFG",
                         AFE_PARAM_ID_SP_RX_ACM_DYNAMIC_CFG}
    @h2xmlp_description {Parameter used to configure dynamic parameters for
                         Feedback Speaker Protection Rx module processing
                         related to audio clip manager}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_ACM_DYNAMIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_acm_dynamic_cfg_v1_t
{
  uint32_t sp_rx_acm_dynamic_cfg_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_TH_DYNAMIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_TH_DYNAMIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t cdc_buf_q_format;
  /**< @h2xmle_description {Q-format of the clip PCM values returned by the
                            codec.}
       @h2xmle_range       {1..15}
       @h2xmle_default     {9} */

  uint32_t clip_pcm_buf_len;
  /**< @h2xmle_description {Length of buffer containing clip PCM values.}
       @h2xmle_range       {1..16}
       @h2xmle_default     {16} */

  int32_t lim_thr_to_clip_level_ratio_q15;
  /**< @h2xmle_description {Limiter threshold =
                            lim_thr_to_clip_level_ratio_q15 * clip PCM value.}
       @h2xmle_range       {16384..32767}
       @h2xmle_dataFormat  {Q15}
       @h2xmle_default     {32113} */

  int32_t lowest_acc_clip_level_Q15;
  /**< @h2xmle_description {Lowest acceptable clip level. Any clipping detecting
                            below this level is ignored.}
       @h2xmle_range       {0..23281}
       @h2xmle_dataFormat  {Q15}
       @h2xmle_default     {14551} */

  int32_t thr_inc_step_in_db_Q15;
  /**< @h2xmle_description {Limiter threshold increment per step_size_in_msec.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q15}
       @h2xmle_default     {1638} */

  uint32_t thr_inc_period_in_msec;
  /**< @h2xmle_description {Step (in milli seconds) size for the limiter
                            threshold increment.}
       @h2xmle_range       {1..10000}
       @h2xmle_default     {2000} */

  uint32_t loudness_boost_en;
  /**< @h2xmle_description {Enable or disable Loudness Boost mode.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {1} */

  int32_t loudness_threshold_q27;
  /**< @h2xmle_description {Maximum limiter threshold in Loudness Boost mode.}
       @h2xmle_range       {1..134217727}
       @h2xmle_default     {108470060} */

  int32_t loudness_temp_margin_q6;
  /**< @h2xmle_description {Temperature prediction error window. An error is
                            caused by corrupt Isen samples.}
       @h2xmle_range       {0..1280}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {640} */
}
#include "adsp_end_pack.h"
;


/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_AGC_DYNAMIC_CFG           0x000102b0

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_AGC_DYNAMIC_CFG        0x1


/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_agc_dynamic_cfg_v1_t
               afe_sp_rx_agc_dynamic_cfg_v1_t;


/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_AGC_DYNAMIC_CFG",
                         AFE_PARAM_ID_SP_RX_AGC_DYNAMIC_CFG}
    @h2xmlp_description {Parameter used to configure dynamic parameters for
                         Feedback Speaker Protection Rx module processing
                         related to Automatic Gain Control.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_AGC_DYNAMIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_agc_dynamic_cfg_v1_t
{
  uint32_t sp_rx_agc_dynamic_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_AGC_DYNAMIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_AGC_DYNAMIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t channelLinked;
  /**< @h2xmle_description {Parameter to set channel link mode.}
       @h2xmle_rangeList   {NotLinked=0;Linked=1}
       @h2xmle_default     {0} */

  uint32_t rmsTavUL16Q16;
  /**< @h2xmle_description {Parameter for time Constant used to compute Input RMS.}
       @h2xmle_range       {0x0..0xffffffff}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {0x01DF} */

  uint32_t makeupGainUL32Q27;
  /**< @h2xmle_description {Parameter to set Makeup Gain Value.}
       @h2xmle_range       {0x0..0xffffffff}
       @h2xmle_default     {0x141857EA} */

  int32_t dncompThresholdDBL32Q23;
  /**< @h2xmle_description {Parameter to set downward compression threshold.}
       @h2xmle_range       {0x80000000..0x7fffffff}
       @h2xmle_dataFormat  {Q23}
       @h2xmle_default     {0x24278495} */

  uint32_t dncompSlopeUL16Q16;
  /**< @h2xmle_description {Parameter to set downward compression slope.}
       @h2xmle_range       {0x0..0xffffffff}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {0xE666} */

  int32_t upcompThresholdDBL32Q23;
  /**< @h2xmle_description {Parameter to set upward compression threshold.}
       @h2xmle_range       {0x80000000..0x7fffffff}
       @h2xmle_dataFormat  {Q23}
       @h2xmle_default     {0x1E278495} */

  uint32_t upcompSlopeUL16Q16;
  /**< @h2xmle_description {Parameter to set upward compression slope.}
       @h2xmle_range       {0x0..0xffffffff}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {0xE666} */

  int32_t expThresholdDBL32Q23;
  /**< @h2xmle_description {Parameter to set expansion threshold.}
       @h2xmle_range       {0x80000000..0x7fffffff}
       @h2xmle_dataFormat  {Q23}
       @h2xmle_default     {0x19278495} */

  int32_t expSlopeL16Q8;
  /**< @h2xmle_description {Parameter to set expansion slope.}
       @h2xmle_range       {0x80000000..0x7fffffff}
       @h2xmle_dataFormat  {Q8}
       @h2xmle_default     {0x00F3} */

  uint32_t attackUL32Q31;
  /**< @h2xmle_description {Parameter to set attack constant.}
       @h2xmle_range       {0x0..0xffffffff}
       @h2xmle_dataFormat  {Q31}
       @h2xmle_default     {0x2D9036ED} */

  uint32_t releaseUL32Q31;
  /**< @h2xmle_description {Parameter to set release constant.}
       @h2xmle_range       {0x0..0xffffffff}
       @h2xmle_dataFormat  {Q31}
       @h2xmle_default     {0x02C90623} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_LIMITER_TH           0x000102b1

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_LIMITER_TH        0x1


/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_limiter_th_v1_t
               afe_sp_rx_limiter_th_v1_t;


/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_LIMITER_TH",
                         AFE_PARAM_ID_SP_RX_LIMITER_TH}
    @h2xmlp_description {Parameter used to configure dynamic parameters for
                         Feedback Speaker Protection Rx module processing
                         related to limiter threshold per calib.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_LIMITER_TH} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_limiter_th_v1_t
{
  uint32_t sp_rx_limiter_th_per_calib_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_LIMITER_TH}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_LIMITER_TH}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t lim_thr_per_calib_q27[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {This limiter threshold whenever present will
                            overwrite the limiter threshold setting from
                            ACDB.}
       @h2xmle_range       {0x80000000..0x7fffffff}
       @h2xmle_dataFormat  {Q27} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Structure definition for Parameter */
#define AFE_PARAM_ID_SP_RX_TH_STATS          0x00010267

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_TH_STATS       0x2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_th_stats_v2_t afe_sp_rx_th_stats_v2_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_TH_STATS",
                         AFE_PARAM_ID_SP_RX_TH_STATS}
    @h2xmlp_description {parameter used for logging the algorithm internal
                         variables related to thermal.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_TH_STATS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_th_stats_v2_t
{
  uint32_t sp_rx_th_stats_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_TH_STATS}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_TH_STATS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration in milliseconds for Thermal Rx
                            statistics.} */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the Thermal Rx statistics.} */

  int32_t vc_resis_q24[SP_NUM_MAX_SPKRS][SP_TH_RX_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Voice coil resistance array (in Ohms). }
       @h2xmle_dataFormat  {Q24}
       @h2xmle_range       {-2147483648..2147483647}   */


  int32_t vc_temp_q22[SP_NUM_MAX_SPKRS][SP_TH_RX_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Voice coil temperature array (in degrees C)}
       @h2xmle_dataFormat  {Q22} */

  int32_t th_gain_lnk_db_q23[SP_NUM_MAX_SPKRS][SP_TH_RX_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Gain control array (in deciBels)}
       @h2xmle_dataFormat  {Q23} */

  int32_t target_temp_q22[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Targeted temperature at which temperature
                            control kicks in.}
       @h2xmle_dataFormat  {Q22} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_TH_STATS_V1       0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_th_stats_v1_t afe_sp_rx_th_stats_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_TH_STATS_V1",
                         AFE_PARAM_ID_SP_RX_TH_STATS}
    @h2xmlp_description {parameter used for logging the algorithm internal
                         variables related to thermal.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_TH_STATS_V1}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_th_stats_v1_t
{
  uint32_t sp_rx_th_stats_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_TH_STATS_V1}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_TH_STATS_V1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration in milliseconds for Thermal Rx
                            statistics.} */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the Thermal Rx statistics.} */

  int32_t vc_resis_q24[SP_NUM_MAX_SPKRS][SP_TH_RX_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Voice coil resistance array (in Ohms). }
       @h2xmle_dataFormat  {Q24}
       @h2xmle_range       {-2147483648..2147483647}   */


  int32_t vc_temp_q22[SP_NUM_MAX_SPKRS][SP_TH_RX_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Voice coil temperature array (in degrees C)}
       @h2xmle_dataFormat  {Q22} */

  int32_t th_gain_lnk_db_q23[SP_NUM_MAX_SPKRS][SP_TH_RX_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Gain control array (in deciBels)}
       @h2xmle_dataFormat  {Q23} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_EX_STATS      0x00010268

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_EX_STATS   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_ex_stats_v1_t afe_sp_rx_ex_stats_v1_t;
/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_EX_STATS",
                         AFE_PARAM_ID_SP_RX_EX_STATS}
    @h2xmlp_description {parameter used for logging the algorithm internal
                         variables related to exertion.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_EX_STATS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_ex_stats_v1_t
{
  uint32_t sp_rx_ex_stats_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                           this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_EX_STATS}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_EX_STATS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */


  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration in milliseconds for Excursion Rx
                            statistics.} */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the Excursion Rx statistics.} */

  int32_t xmax_q14[SP_NUM_MAX_SPKRS ];
  /**< @h2xmle_description {Maximum excursion limit. }
       @h2xmle_dataFormat  {Q14}
       @h2xmle_range       {-2147483648..2147483647} */


  int32_t total_gain_q31[SP_NUM_MAX_SPKRS ][SP_EX_RX_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Excursion control gain. }
       @h2xmle_dataFormat  {Q31} */


  uint16_t pred_peak_exc_q14[SP_NUM_MAX_SPKRS ][SP_EX_RX_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Predicted excursion. }
       @h2xmle_dataFormat  {Q14} */

  uint16_t ctrl_peak_exc_q14[SP_NUM_MAX_SPKRS ][SP_EX_RX_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Controlled excursion. }
       @h2xmle_dataFormat  {Q14} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_ACM_STATS       0x00010269

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_ACM_STATS    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_acm_stats_v1_t afe_sp_rx_acm_stats_v1_t;
/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_ACM_STATS",
                         AFE_PARAM_ID_SP_RX_ACM_STATS}
    @h2xmlp_description {parameter used for logging the algorithm internal
                         variables related to audio clip manager.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_ACM_STATS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_acm_stats_v1_t
{
  uint32_t sp_rx_acm_stats_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                           this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_EX_STATS}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_EX_STATS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */


  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration in milliseconds for ACM statistics.}
                            */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the ACM statistics.} */

  int32_t clip_intr_status[SP_NUM_MAX_SPKRS][SP_ACM_SMPL_PER_DEMO_PKT];
  /**< @h2xmle_description {Clip interrupt status (high or low). }
       @h2xmle_range       {-2147483648..2147483647}   */

  uint16_t clip_level_q15[SP_NUM_MAX_SPKRS][SP_ACM_SMPL_PER_DEMO_PKT];
  /**< @h2xmle_description {Clip level. }
       @h2xmle_dataFormat  {Q15} */

  int32_t lim_threshold_q27[SP_NUM_MAX_SPKRS][SP_ACM_SMPL_PER_DEMO_PKT];
  /**< @h2xmle_description {Limiter threshold. }
       @h2xmle_dataFormat  {Q27} */
}
#include "adsp_end_pack.h"
;


/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_AGC_STATS       0x000102b2

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_AGC_STATS    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_agc_stats_v1_t afe_sp_rx_agc_stats_v1_t;
/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_AGC_STATS",
                         AFE_PARAM_ID_SP_RX_AGC_STATS}
    @h2xmlp_description {parameter used for logging the algorithm internal
                         variables related to automatic gain control.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_AGC_STATS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_rx_agc_stats_v1_t
{
  uint32_t sp_rx_agc_stats_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID.}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_AGC_STATS}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_AGC_STATS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */


  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration in milliseconds for AGC statistics.}
       @h2xmle_dataFormat  {Q15}
       @h2xmle_default     {0}
       @h2xmle_range       {0x0..0xffffffff} */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the AGC statistics.}
       @h2xmle_dataFormat  {Q15}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_AGC_STATS}
       @h2xmle_range       {0x0..0xffffffff} */

  int32_t dwn_compression_thr_db_q23;
  /**< @h2xmle_description {downward compression threshold, in dB. }
       @h2xmle_dataFormat  {Q23}
       @h2xmle_default     {0}
       @h2xmle_range       {0x80000000..0x7fffffff}  */

  int32_t up_compression_thr_db_q23;
  /**< @h2xmle_description {upward compression threshold, in dB. }
       @h2xmle_dataFormat  {Q23}
       @h2xmle_default     {0}
       @h2xmle_range       {0x80000000..0x7fffffff} */

  int32_t exp_thr_db_q23;
  /**< @h2xmle_description {Expansion threshold, in dB. }
       @h2xmle_dataFormat  {Q23}
       @h2xmle_default     {0}
       @h2xmle_range       {0x80000000..0x7fffffff} */

  int32_t max_input_rms_db_q23[SP_NUM_MAX_SPKRS][SP_AGC_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {RMS level of the input, in dB (max over the frequency
                            of generating RTM packets. }
       @h2xmle_dataFormat  {Q23}
       @h2xmle_default     {0}
       @h2xmle_range       {0x80000000..0x7fffffff} */

  uint32_t max_smoothed_agc_gain_q27[SP_NUM_MAX_SPKRS][SP_AGC_DEMO_SMPL_PER_PKT];
  /**< @h2xmle_description {Smoothed AGC gain applied to the signal
                            (max over the frequency of generating RTM packets). }
       @h2xmle_dataFormat  {Q27}
       @h2xmle_default     {0}
       @h2xmle_range       {0x0..0xffffffff} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_RX_STATS         0x00010284


/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_STATS      0x2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_stats_v2_t afe_sp_rx_stats_v2_t;
/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_STATS",
                         AFE_PARAM_ID_SP_RX_STATS}
    @h2xmlp_description {parameter used for logging the algorithm internal
                         variables.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_STATS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */

#include "adsp_begin_pack.h"
struct afe_sp_rx_stats_v2_t
{
  uint32_t sp_rx_stats_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                           this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_STATS}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_STATS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */


  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration in milliseconds for Rx statistics.}*/

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the Rx statistics.} */

  uint32_t num_speakers;
  /**< @h2xmle_description {Number of speakers.} */

  uint32_t features;
  /**< @h2xmle_description {Features enabled in the speaker protection
                            algorithm.}

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"Notch high-pass filter"}
       @h2xmle_description {Notch high-pass filter}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"Thermal protection"}
       @h2xmle_description {Thermal protection}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000004}
       @h2xmle_bitName     {"Feedback excursion control"}
       @h2xmle_description {Feedback excursion control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000008}
       @h2xmle_bitName     {"Analog clip manager"}
       @h2xmle_description {Analog clip manager}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000010}
       @h2xmle_bitName     {"Feedforward excursion control"}
       @h2xmle_description {Feedforward excursion control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000020}
       @h2xmle_bitName     {"Automatic gain control"}
       @h2xmle_description {Automatic gain control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_RX_STATS_V1      0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_rx_stats_v1_t afe_sp_rx_stats_v1_t;
/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_RX_STATS_V1",
                         AFE_PARAM_ID_SP_RX_STATS}
    @h2xmlp_description {parameter used for logging the algorithm internal
                         variables.}
    @h2xmlp_version     {AFE_API_VERSION_SP_RX_STATS_V1}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */

#include "adsp_begin_pack.h"
struct afe_sp_rx_stats_v1_t
{
  uint32_t sp_rx_stats_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                           this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_RX_STATS_V1}
       @h2xmle_default     {AFE_API_VERSION_SP_RX_STATS_V1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */


  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration in milliseconds for Rx statistics.}*/

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the Rx statistics.} */

  uint32_t num_speakers;
  /**< @h2xmle_description {Number of speakers.} */

  uint32_t features;
  /**< @h2xmle_description {Features enabled in the speaker protection
                            algorithm.}

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"Notch high-pass filter"}
       @h2xmle_description {Notch high-pass filter}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"Thermal protection"}
       @h2xmle_description {Thermal protection}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000004}
       @h2xmle_bitName     {"Feedback excursion control"}
       @h2xmle_description {Feedback excursion control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000008}
       @h2xmle_bitName     {"Analog clip manager"}
       @h2xmle_description {Analog clip manager}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000010}
       @h2xmle_bitName     {"Feedforward excursion control"}
       @h2xmle_description {Feedforward excursion control}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_bitFieldEnd */
}
#include "adsp_end_pack.h"
;

/** @h2xml_Select         {afe_mod_enable_param_t}
    @h2xmlm_InsertParameter */

/** @} <-- End of the Module --> */

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Module ID */
#define AFE_MODULE_SPEAKER_PROTECTION_TH_VI    0x0001026A

/* Input Pin Name - '0' */
#define SP_PROT_TH_VI_INPUT_PIN_0      0

/* Input Pins Max */
#define SP_PROT_TH_VI_INPUT_PINS_MAX   1

/* Output Pins Max */
#define SP_PROT_TH_VI_OUTPUT_PINS_MAX  0

/** @h2xmlm_module       {"AFE_MODULE_SPEAKER_PROTECTION_TH_VI",
                          AFE_MODULE_SPEAKER_PROTECTION_TH_VI}
    @h2xmlm_InputPins    {IN0=SP_PROT_TH_VI_INPUT_PIN_0}
    @h2xmlm_toolPolicy   {Calibration;RTC_READONLY}
    @h2xmlm_description  {Speaker Protection Thermal VI Module}
    @{                   <-- Start of the Module --> */

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_TH_VI_MODE_CFG       0x0001026B

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_TH_VI_MODE_CFG    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_th_vi_mode_cfg_v1_t afe_sp_th_vi_mode_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_TH_VI_MODE_CFG",
                         AFE_PARAM_ID_SP_TH_VI_MODE_CFG}
    @h2xmlp_description {parameter used process Vsens and Isens data for
                         extracting the speaker temperature.}
    @h2xmlp_version     {AFE_API_VERSION_SP_TH_VI_MODE_CFG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_th_vi_mode_cfg_v1_t
{
  uint32_t sp_th_vi_mode_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                          this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_TH_VI_MODE_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_TH_VI_MODE_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t operation_mode;
  /**< @h2xmle_description {Operation mode of thermal VI module.}
       @h2xmle_rangeList   {"Normal Running mode"=0;
                            "Calibration mode"=1;
                            "Factory Test Mode"=2} */

  uint32_t r0t0_selection_flag[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Specifies which set of R0, T0 values the algorithm
                            will use. This field is valid only in Normal mode
                            (operation_mode = 0).}
       @h2xmle_rangeList   {"Use calibrated R0, T0 value"=0;
                            "Use safe R0, T0 value"=1}
       @h2xmlx_expandArray {true} */

  int32_t r0_cali_q24[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Calibration point resistance per device (in Ohms).
                            This field is valid only  in Normal mode
                            (operation_mode = 0).}
       @h2xmle_range       {33554432..1073741824}
       @h2xmle_default     {33554432}
       @h2xmle_dataFormat  {Q24} */

  int16_t t0_cali_q6[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Calibration point temperature per device (in
                            degrees C). This field is valid in both Normal mode
                            and Calibration mode.}
       @h2xmle_range       {-1920..5120 }
       @h2xmle_dataFormat  {Q6} */

  uint32_t quick_calib_flag;
  /**< @h2xmle_description {Indicates whether calibration is to be done in quick
                             mode or not. This field is valid only in
                            Calibration mode (operation_mode = 1).
       @h2xmle_rangeList   {disabled=0;enabled=1} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_TH_VI_STATIC_CFG     0x0001026C

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_TH_VI_STATIC_CFG  0x2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_th_vi_static_cfg_v2_t afe_sp_th_vi_static_cfg_v2_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_TH_VI_STATIC_CFG",
                         AFE_PARAM_ID_SP_TH_VI_STATIC_CFG}
    @h2xmlp_description {parameter used to configure static parameters of
                         thermal VI processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_TH_VI_STATIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_th_vi_static_cfg_v2_t
{
  uint32_t sp_th_vi_static_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_TH_VI_STATIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_TH_VI_STATIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t ctrl_rate;
  /**< @h2xmle_description {Thermal control rate.}
       @h2xmle_rangeList   {"500Hz"=500; "1000Hz"=1000}
       @h2xmle_default     {1000} */

  int32_t pow_supply_freq;
  /**< @h2xmle_description {Specifies the power supply frequency.}
       @h2xmle_rangeList   {"disabled"=0; "50Hz"=50; "60Hz"=60}
       @h2xmle_default     {0} */

  uint16_t pt_lvl_switch_en;
  /**< @h2xmle_description {Specifies whether to enable the dynamic pilot tone
                            level switch.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0} */

  uint16_t pt_masking_delay_ms;
  /**< @h2xmle_description {Delay in ramping down the pilot tone (in milli
                            seconds).}
       @h2xmle_range       {0..1000}
       @h2xmle_default     {13} */

  int32_t pt_masking_thr_q27;
  /**< @h2xmle_description {Specifies the input level threshold below which the
                            pilot tone is disabled.}
       @h2xmle_range       {0..134217727}
       @h2xmle_dataFormat  {Q27}
       @h2xmle_default     {189813} */

  uint32_t temp_est_selection_flag;
  /**< @h2xmle_description {Temperature estimation selection flag.}
       @h2xmle_rangeList   {OldMethod=0;NewMethod=1}
       @h2xmle_default     {1} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_TH_VI_STATIC_CFG_V1  0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_th_vi_static_cfg_v1_t afe_sp_th_vi_static_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_TH_VI_STATIC_CFG_V1",
                         AFE_PARAM_ID_SP_TH_VI_STATIC_CFG}
    @h2xmlp_description {parameter used to configure static parameters of
                         thermal VI processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_TH_VI_STATIC_CFG_V1}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_th_vi_static_cfg_v1_t
{
  uint32_t sp_th_vi_static_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_TH_VI_STATIC_CFG_V1}
       @h2xmle_default     {AFE_API_VERSION_SP_TH_VI_STATIC_CFG_V1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t ctrl_rate;
  /**< @h2xmle_description {Thermal control rate.}
       @h2xmle_rangeList   {"500Hz"=500; "1000Hz"=1000}
       @h2xmle_default     {1000} */

  int32_t pow_supply_freq;
  /**< @h2xmle_description {Specifies the power supply frequency.}
       @h2xmle_rangeList   {"disabled"=0; "50Hz"=50; "60Hz"=60}
       @h2xmle_default     {0} */

  uint16_t pt_lvl_switch_en;
  /**< @h2xmle_description {Specifies whether to enable the dynamic pilot tone
                            level switch.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0} */

  uint16_t pt_masking_delay_ms;
  /**< @h2xmle_description {Delay in ramping down the pilot tone (in milli
                            seconds).}
       @h2xmle_range       {0..1000}
       @h2xmle_default     {13} */

  int32_t pt_masking_thr_q27;
  /**< @h2xmle_description {Specifies the input level threshold below which the
                            pilot tone is disabled.}
       @h2xmle_range       {0..134217727}
       @h2xmle_dataFormat  {Q27}
       @h2xmle_default     {189813} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_TH_VI_DYNAMIC_CFG        0x0001026D

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_TH_VI_DYNAMIC_CFG     0x2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_th_vi_dynamic_cfg_v2_t
               afe_sp_th_vi_dynamic_cfg_v2_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_TH_VI_DYNAMIC_CFG",
                         AFE_PARAM_ID_SP_TH_VI_DYNAMIC_CFG}
    @h2xmlp_description {parameter used to configure dynamic thermal parameters
                         for feedback speaker protection VI processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_TH_VI_DYNAMIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_th_vi_dynamic_cfg_v2_t
{
  uint32_t sp_th_vi_dynamic_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_TH_VI_DYNAMIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_TH_VI_DYNAMIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t left_pilot_tone_scale_u16q18;
  /**< @h2xmle_description {Left speaker: scale of the pilot tone relative to
                            0 dBFS (for example, -40 dB from the full scale).}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q18}
       @h2xmle_default     {5230} */

  int32_t left_wait_time_ms;
  /**< @h2xmle_description {Left speaker: wait time for initial settling before
                            collecting statistics (in milli seconds).}
       @h2xmle_range       {100..5000}
       @h2xmle_default     {200} */

  int32_t left_cali_time_ms;
  /**< @h2xmle_description {Left speaker: calibration time  (in milli seconds).}
       @h2xmle_range       {500..30000}
       @h2xmle_default     {2000} */

  int32_t left_r0_safe_q24;
  /**< @h2xmle_description {Left speaker: backup calibration point resistance
                            when per-device calibration is not available (in
                            Ohms.)}
       @h2xmle_range       {33554432..1073741824}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {104857600} */

  int16_t left_t0_safe_q6;
  /**< @h2xmle_description {Left speaker: backup calibration point temperature
                            when per-device calibration is not available (in
                            degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {1600} */

  int16_t left_r_spk_coil_typ_q8;
  /**< @h2xmle_description {Typical resistance of the left speaker (in Ohms).}
       @h2xmle_range       {512..32767}
       @h2xmle_default     {2048}
       @h2xmle_dataFormat  {Q8} */

  int16_t left_r_spk_coil_factor_q10;
  /**< @h2xmle_description {Left speaker: factor used to determine the threshold
                            for speaker coil resistance (threshold for speaker
                            coil resistance = factor * speaker coil resistance).
                            After this threshold, the speaker is considered
                            open.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {2048} */

  int16_t left_v_full_sc_q10;
  /**< @h2xmle_description {Left speaker: voltage for digitally full scale
                            signal.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {5477} */

  uint16_t left_spk_resistivity_coeff_u16q7;
  /**< @h2xmle_description {Left speaker: resistivity coefficient i.e:
                            1/degree C (in degrees C).}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q7}
       @h2xmle_default     {32570} */

  uint16_t left_vsen_2_v_u16q11;
  /**< @h2xmle_description {Left speaker: conversion factor from Vsens digital
                            to volts.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q11}
       @h2xmle_default     {21466} */

  uint16_t left_isen_2_a_u16q11;
  /**< @h2xmle_description {Left speaker: conversion factor from Isens digital
                            to amps.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q11}
       @h2xmle_default     {14143} */

  uint16_t left_v_pilot_thsd_u16q16;
  /**< @h2xmle_description {Left speaker: fraction of the expected pilot tone.
                            Below this value, the  resistance update is
                            unreliable.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {26214} */

  uint16_t left_r0_und_est_fac_q16;
  /**< @h2xmle_description {Left speaker: underestimate R0 if it is quick
                            calibration.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {0} */

  int16_t left_r0_rng_fac_q10;
  /**< @h2xmle_description {Left speaker: factor used to determine the threshold
                             for calculated calibrated resistance (threshold for
                             calibrated resistance = factor * calibrated
                            resistance). After this threshold, calibrated
                            resistance is considered as failed.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {1536} */

  int16_t left_t0_min_q6;
  /**< @h2xmle_description {Left speaker: minimum expected temperature from
                            calibration (in degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {0} */

  int16_t left_t0_max_q6;
  /**< @h2xmle_description {Left speaker: maximum expected temperature from
                            calibration (in degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {3200} */

  uint32_t right_pilot_tone_scale_u16q18;
  /**< @h2xmle_description {Right speaker: scale of the pilot tone relative to
                            0 dBFS (for example, -40 dB from the full scale).}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q18}
       @h2xmle_default     {5230} */

  int32_t right_wait_time_ms;
  /**< @h2xmle_description {Right speaker: wait time for initial settling
                            before collecting statistics (in milli seconds).}
       @h2xmle_range       {100..5000}
       @h2xmle_default     {200} */

  int32_t right_cali_time_ms;
  /**< @h2xmle_description {Right speaker: calibration time (in milli seconds).}
       @h2xmle_range       {500..30000}
       @h2xmle_default     {2000} */

  int32_t right_r0_safe_q24;
  /**< @h2xmle_description {Right speaker: backup calibration point resistance
                            when per-device calibration is not available (in
                            Ohms).}
       @h2xmle_range       {33554432..1073741824}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {104857600} */

  int16_t right_t0_safe_q6;
  /**< @h2xmle_description {Right speaker: backup calibration point temperature
                            when per-device calibration is not available (in
                            degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {1600} */

  int16_t right_r_spk_coil_typ_q8;
  /**< @h2xmle_description {Right speaker: typical resistance of the speaker (in
                            ohms).}
       @h2xmle_range       {512..32767}
       @h2xmle_dataFormat  {Q8}
       @h2xmle_default     {2048} */

  int16_t right_r_spk_coil_factor_q10;
  /**< @h2xmle_description {Right speaker: factor used to determine the
                            threshold for speaker coil resistance (threshold for
                            speaker coil resistance = factor * speaker coil
                            resistance). After this threshold, the speaker is
                            considered open.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {2048} */

  int16_t right_v_full_sc_q10;
  /**< @h2xmle_description {Right speaker: speaker voltage for a digitally
                            full-scale signal.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {5477} */

  uint16_t right_spk_resistivity_coeff_u16q7;
  /**< @h2xmle_description {Right speaker: resistivity coefficient i.e
                            1/degree C (in degrees C).}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q7}
       @h2xmle_default     {32570} */

  uint16_t right_vsen_2_v_u16q11;
  /**< @h2xmle_description {Right speaker: conversion factor from Vsens digital
                            to volts.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q11}
       @h2xmle_default     {21449} */

  uint16_t right_isen_2_a_u16q11;
  /**< @h2xmle_description {Right speaker: conversion factor from Isens digital
                            to amps.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q11}
       @h2xmle_default     {14195} */

  uint16_t right_v_pilot_thsd_u16q16;
  /**< @h2xmle_description {Right speaker: fraction of the expected pilot tone.
                            Below this value, the resistance update is
                            unreliable.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {26214} */

  uint16_t right_r0_und_est_fac_q16;
  /**< @h2xmle_description {Right speaker: underestimate R0 if it is quick
                            calibration.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {0} */

  int16_t right_r0_rng_fac_q10;
  /**< @h2xmle_description {Right speaker: factor used to determine the
                            threshold for calculated calibrated resistance
                            (threshold for calibrated resistance = factor *
                            calibrated resistance). After this threshold,
                            calibrated resistance is considered as failed.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {1536} */

  int16_t right_t0_min_q6;
  /**< @h2xmle_description {Right speaker: minimum expected temperature from
                            calibration (in degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {0} */

  int16_t right_t0_max_q6;
  /**< @h2xmle_description {Right speaker: maximum expected temperature from
                            calibration (in degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {3200} */

  uint16_t left_spk_delta_t_delta_r_ratio_u16q9;
  /**< @h2xmle_description {speaker deltaT/deltaR = 1/(change in resistance
                            per unit change in temperature)}
       @h2xmle_range       {0x0..0xffff}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {0x4286} */

  uint16_t right_spk_delta_t_delta_r_ratio_u16q9;
  /**< @h2xmle_description {speaker deltaT/deltaR = 1/(change in resistance
                            per unit change in temperature).}
       @h2xmle_range       {0x0..0xffff}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {0x4286} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_TH_VI_DYNAMIC_CFG_V1     0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_th_vi_dynamic_cfg_v1_t
               afe_sp_th_vi_dynamic_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_TH_VI_DYNAMIC_CFG_V1",
                         AFE_PARAM_ID_SP_TH_VI_DYNAMIC_CFG}
    @h2xmlp_description {parameter used to configure dynamic thermal parameters
                         for feedback speaker protection VI processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_TH_VI_DYNAMIC_CFG_V1}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_th_vi_dynamic_cfg_v1_t
{
  uint32_t sp_th_vi_dynamic_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_TH_VI_DYNAMIC_CFG_V1}
       @h2xmle_default     {AFE_API_VERSION_SP_TH_VI_DYNAMIC_CFG_V1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t left_pilot_tone_scale_u16q18;
  /**< @h2xmle_description {Left speaker: scale of the pilot tone relative to
                            0 dBFS (for example, -40 dB from the full scale).}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q18}
       @h2xmle_default     {5230} */

  int32_t left_wait_time_ms;
  /**< @h2xmle_description {Left speaker: wait time for initial settling before
                            collecting statistics (in milli seconds).}
       @h2xmle_range       {100..5000}
       @h2xmle_default     {200} */

  int32_t left_cali_time_ms;
  /**< @h2xmle_description {Left speaker: calibration time  (in milli seconds).}
       @h2xmle_range       {500..30000}
       @h2xmle_default     {2000} */

  int32_t left_r0_safe_q24;
  /**< @h2xmle_description {Left speaker: backup calibration point resistance
                            when per-device calibration is not available (in
                            Ohms.)}
       @h2xmle_range       {33554432..1073741824}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {104857600} */

  int16_t left_t0_safe_q6;
  /**< @h2xmle_description {Left speaker: backup calibration point temperature
                            when per-device calibration is not available (in
                            degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {1600} */

  int16_t left_r_spk_coil_typ_q8;
  /**< @h2xmle_description {Typical resistance of the left speaker (in Ohms).}
       @h2xmle_range       {512..32767}
       @h2xmle_default     {2048}
       @h2xmle_dataFormat  {Q8} */

  int16_t left_r_spk_coil_factor_q10;
  /**< @h2xmle_description {Left speaker: factor used to determine the threshold
                            for speaker coil resistance (threshold for speaker
                            coil resistance = factor * speaker coil resistance).
                            After this threshold, the speaker is considered
                            open.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {2048} */

  int16_t left_v_full_sc_q10;
  /**< @h2xmle_description {Left speaker: voltage for digitally full scale
                            signal.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {5477} */

  uint16_t left_spk_resistivity_coeff_u16q7;
  /**< @h2xmle_description {Left speaker: resistivity coefficient i.e:
                            1/degree C (in degrees C).}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q7}
       @h2xmle_default     {32570} */

  uint16_t left_vsen_2_v_u16q11;
  /**< @h2xmle_description {Left speaker: conversion factor from Vsens digital
                            to volts.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q11}
       @h2xmle_default     {21466} */

  uint16_t left_isen_2_a_u16q11;
  /**< @h2xmle_description {Left speaker: conversion factor from Isens digital
                            to amps.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q11}
       @h2xmle_default     {14143} */

  uint16_t left_v_pilot_thsd_u16q16;
  /**< @h2xmle_description {Left speaker: fraction of the expected pilot tone.
                            Below this value, the  resistance update is
                            unreliable.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {26214} */

  uint16_t left_r0_und_est_fac_q16;
  /**< @h2xmle_description {Left speaker: underestimate R0 if it is quick
                            calibration.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {0} */

  int16_t left_r0_rng_fac_q10;
  /**< @h2xmle_description {Left speaker: factor used to determine the threshold
                             for calculated calibrated resistance (threshold for
                             calibrated resistance = factor * calibrated
                            resistance). After this threshold, calibrated
                            resistance is considered as failed.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {1536} */

  int16_t left_t0_min_q6;
  /**< @h2xmle_description {Left speaker: minimum expected temperature from
                            calibration (in degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {0} */

  int16_t left_t0_max_q6;
  /**< @h2xmle_description {Left speaker: maximum expected temperature from
                            calibration (in degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {3200} */

  uint32_t right_pilot_tone_scale_u16q18;
  /**< @h2xmle_description {Right speaker: scale of the pilot tone relative to
                            0 dBFS (for example, -40 dB from the full scale).}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q18}
       @h2xmle_default     {5230} */

  int32_t right_wait_time_ms;
  /**< @h2xmle_description {Right speaker: wait time for initial settling
                            before collecting statistics (in milli seconds).}
       @h2xmle_range       {100..5000}
       @h2xmle_default     {200} */

  int32_t right_cali_time_ms;
  /**< @h2xmle_description {Right speaker: calibration time (in milli seconds).}
       @h2xmle_range       {500..30000}
       @h2xmle_default     {2000} */

  int32_t right_r0_safe_q24;
  /**< @h2xmle_description {Right speaker: backup calibration point resistance
                            when per-device calibration is not available (in
                            Ohms).}
       @h2xmle_range       {33554432..1073741824}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {104857600} */

  int16_t right_t0_safe_q6;
  /**< @h2xmle_description {Right speaker: backup calibration point temperature
                            when per-device calibration is not available (in
                            degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {1600} */

  int16_t right_r_spk_coil_typ_q8;
  /**< @h2xmle_description {Right speaker: typical resistance of the speaker (in
                            ohms).}
       @h2xmle_range       {512..32767}
       @h2xmle_dataFormat  {Q8}
       @h2xmle_default     {2048} */

  int16_t right_r_spk_coil_factor_q10;
  /**< @h2xmle_description {Right speaker: factor used to determine the
                            threshold for speaker coil resistance (threshold for
                            speaker coil resistance = factor * speaker coil
                            resistance). After this threshold, the speaker is
                            considered open.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {2048} */

  int16_t right_v_full_sc_q10;
  /**< @h2xmle_description {Right speaker: speaker voltage for a digitally
                            full-scale signal.}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {5477} */

  uint16_t right_spk_resistivity_coeff_u16q7;
  /**< @h2xmle_description {Right speaker: resistivity coefficient i.e
                            1/degree C (in degrees C).}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q7}
       @h2xmle_default     {32570} */

  uint16_t right_vsen_2_v_u16q11;
  /**< @h2xmle_description {Right speaker: conversion factor from Vsens digital
                            to volts.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q11}
       @h2xmle_default     {21449} */

  uint16_t right_isen_2_a_u16q11;
  /**< @h2xmle_description {Right speaker: conversion factor from Isens digital
                            to amps.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q11}
       @h2xmle_default     {14195} */

  uint16_t right_v_pilot_thsd_u16q16;
  /**< @h2xmle_description {Right speaker: fraction of the expected pilot tone.
                            Below this value, the resistance update is
                            unreliable.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {26214} */

  uint16_t right_r0_und_est_fac_q16;
  /**< @h2xmle_description {Right speaker: underestimate R0 if it is quick
                            calibration.}
       @h2xmle_range       {0..65535}
       @h2xmle_dataFormat  {Q16}
       @h2xmle_default     {0} */

  int16_t right_r0_rng_fac_q10;
  /**< @h2xmle_description {Right speaker: factor used to determine the
                            threshold for calculated calibrated resistance
                            (threshold for calibrated resistance = factor *
                            calibrated resistance). After this threshold,
                            calibrated resistance is considered as failed.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_dataFormat  {Q10}
       @h2xmle_default     {1536} */

  int16_t right_t0_min_q6;
  /**< @h2xmle_description {Right speaker: minimum expected temperature from
                            calibration (in degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {0} */

  int16_t right_t0_max_q6;
  /**< @h2xmle_description {Right speaker: maximum expected temperature from
                            calibration (in degrees C).}
       @h2xmle_range       {-1920..5120}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {3200} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_TH_VI_CALIB_RES_CFG         0x0001026E

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_TH_VI_CALIB_RES_CFG      0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_th_vi_calib_res_cfg_v1_t
              afe_sp_th_vi_calib_res_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_TH_VI_CALIB_RES_CFG",
                         AFE_PARAM_ID_SP_TH_VI_CALIB_RES_CFG}
    @h2xmlp_description {Parameter used to get the calibrated resistance value
                         from the feedback speaker VI processing module.}
    @h2xmlp_version     {AFE_API_VERSION_SP_TH_VI_CALIB_RES_CFG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_th_vi_calib_res_cfg_v1_t
{
  uint32_t sp_th_vi_calib_res_cfg_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter}
       @h2xmle_range       {1..AFE_API_VERSION_SP_TH_VI_CALIB_RES_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_TH_VI_CALIB_RES_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */


  int32_t r0_cali_q24[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Calibration point resistance per device.
                            In Calibration mode, this resistance is not valid.}
       @h2xmle_range       {33554432..1073741824}
       @h2xmle_default     {33554432}
       @h2xmle_dataFormat  {Q24} */

  uint32_t th_vi_ca_state;
  /**< @h2xmle_description {Represents the calibration state for both speakers.
                            The state of each speaker is tied together.}
       @h2xmle_rangeList   {"Incorrect operation mode (isn't normal mode)"=0;
                            "Inactive mode (port is not started)"=1;
                            "Wait state"=2;
                            "Calibration state"=3;
                            "Success"=4;
                            "Failed"=5} */
}
#include "adsp_end_pack.h"
;

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_TH_VI_FTM_CFG        0x0001029F

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_TH_VI_FTM_CFG     0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_th_vi_ftm_cfg_v1_t afe_sp_th_vi_ftm_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_TH_VI_FTM_CFG",
                         AFE_PARAM_ID_SP_TH_VI_FTM_CFG}
    @h2xmlp_description {parameter used to set the FTM configuration in the
                         feedback speaker thermal VI processing module.}
    @h2xmlp_version     {AFE_API_VERSION_SP_TH_VI_FTM_CFG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_th_vi_ftm_cfg_v1_t
{
  uint32_t sp_th_vi_ftm_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                          this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_TH_VI_FTM_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_TH_VI_FTM_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */


  uint32_t wait_time_ms[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Wait time to heat up the speaker before collecting
                            statistics for FTM mode (in milli seconds).}
       @h2xmle_range       {0..4294967295} */

  uint32_t ftm_time_ms[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Time period when FTM statistics are collected (in
                            milli seconds).}
       @h2xmle_range       {0..2000} */
}
#include "adsp_end_pack.h"
;

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_TH_VI_FTM_PARAMS       0x000102A0

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_TH_VI_FTM_PARAMS    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_th_vi_ftm_params_v1_t afe_sp_th_vi_ftm_params_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_TH_VI_FTM_PARAMS",
                         AFE_PARAM_ID_SP_TH_VI_FTM_PARAMS}
    @h2xmlp_description {parameter used to get the FTM statistics from the
                         Thermal VI processing module.}
    @h2xmlp_version     {AFE_API_VERSION_SP_TH_VI_FTM_PARAMS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_th_vi_ftm_params_v1_t
{
  uint32_t sp_th_vi_ftm_params_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                          this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_TH_VI_FTM_PARAMS}
       @h2xmle_default     {AFE_API_VERSION_SP_TH_VI_FTM_PARAMS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t ftm_dc_res_q24[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {DC resistance value (in Ohms).}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t ftm_temp_q22[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Temperature value (in degrees C).}
       @h2xmle_range       {-125829120..2147483647}
       @h2xmle_dataFormat  {Q22} */

  uint32_t status[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {FTM packet status.}
       @h2xmle_rangeList   {"Incorrect operation mode"=0;
                            "Inactive mode (port is not started)"=1;
                            "Wait state"=2;
                            "In progress state"=3;
                            "Success"=4;
                            "Failed"=5} */

}
#include "adsp_end_pack.h"
;

/** @h2xml_Select         {afe_mod_enable_param_t}
    @h2xmlm_InsertParameter */

/** @} <-- End of the Module --> */

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Module ID */
#define AFE_MODULE_SPEAKER_PROTECTION_EX_VI    0x0001026F

/* Input Pin Name - '0' */
#define SP_PROT_EX_VI_INPUT_PIN_0      0

/* Input Pins Max */
#define SP_PROT_EX_VI_INPUT_PINS_MAX   1

/* Output Pins Max */
#define SP_PROT_EX_VI_OUTPUT_PINS_MAX  0

/** @h2xmlm_module       {"AFE_MODULE_SPEAKER_PROTECTION_EX_VI",
                          AFE_MODULE_SPEAKER_PROTECTION_EX_VI}
    @h2xmlm_InputPins    {IN0=SP_PROT_TH_VI_INPUT_PIN_0}
    @h2xmlm_toolPolicy   {Calibration;RTC_READONLY}
    @h2xmlm_description  {Speaker Protection Excursion VI Module, which is used
                          to process Vsens and Isens data for extracting speaker
                          excursion information.}
    @{                   <-- Start of the Module --> */

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_EX_VI_STATIC_CFG     0x00010270

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_EX_VI_STATIC_CFG  0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_ex_vi_static_cfg_v1_t afe_sp_ex_vi_static_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_EX_VI_STATIC_CFG",
                         AFE_PARAM_ID_SP_EX_VI_STATIC_CFG}
    @h2xmlp_description {parameter used configure the static parameters of
                         feedback Speaker Protection Excursion VI processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_EX_VI_STATIC_CFG} */
#include "adsp_begin_pack.h"
struct afe_sp_ex_vi_static_cfg_v1_t
{
  uint32_t sp_ex_vi_static_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                          this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_EX_VI_STATIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_EX_VI_STATIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t ex_filter_taps;
  /**< @h2xmle_description {Number of taps in the excursion prediction.}
       @h2xmle_rangeList   {"24"=24; "32"=32; "40"=40; "48"=48}
       @h2xmle_default     {48} */

  uint32_t exc_fine_tune_en_flag;
  /**< @h2xmle_description {Specifies whether the Tuning mode is enabled.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0} */

  uint32_t tuning_mode_en_flag;
  /**< @h2xmle_description {Specifies whether RTM tuning mode is enabled.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0} */

  uint16_t pt_lvl_switch_en;
  /**< @h2xmle_description {Specifies whether to enable the dynamic pilot tone
                            level switch.}
       @h2xmle_rangeList   {disabled=0;enabled=1}
       @h2xmle_default     {0} */

  uint16_t pt_masking_delay_ms;
  /**< @h2xmle_description {Delay in ramping down the pilot tone (in milli
                            seconds).}
       @h2xmle_range       {0..1000}
       @h2xmle_default     {13} */

  int32_t pt_masking_thr_q27;
  /**< @h2xmle_description {Specifies the input level threshold below which the
                            pilot tone is disabled.}
       @h2xmle_range       {0..134217727}
       @h2xmle_dataFormat  {Q27}
       @h2xmle_default     {189813} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_EX_VI_DYNAMIC_CFG      0x00010271

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_EX_VI_DYNAMIC_CFG   0x2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_ex_vi_dynamic_cfg_v2_t
               afe_sp_ex_vi_dynamic_cfg_v2_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_EX_VI_DYNAMIC_CFG",
                         AFE_PARAM_ID_SP_EX_VI_DYNAMIC_CFG}
    @h2xmlp_description {parameter used to configure the dynamic parameters of
                         excursion VI processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_EX_VI_DYNAMIC_CFG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_ex_vi_dynamic_cfg_v2_t
{
  uint32_t  sp_ex_vi_dynamic_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_EX_VI_DYNAMIC_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_EX_VI_DYNAMIC_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t impedance_convergence_level_thr_q15;
  /**< @h2xmle_description {Impedance convergence level threshold.}
       @h2xmle_range       {16384..32767}
       @h2xmle_default     {29491}
       @h2xmle_dataFormat  {Q15} */

  int32_t ex_filter_stable_level_thr_q31;
  /**< @h2xmle_description {Excursion prediction filter stability threshold.}
       @h2xmle_range       {1073741824..2147483647}
       @h2xmle_default     {1932735283}
       @h2xmle_dataFormat  {Q31} */

  int32_t ex_filter_energy_level_thr_q31;
  /**< @h2xmle_description {Excursion prediction filter energy threshold as
                            compared to the shape of the reference filter.}
       @h2xmle_range       {1073741824..2147483647}
       @h2xmle_default     {1503238553}
       @h2xmle_dataFormat  {Q31} */

  int32_t impedance_convergence_speed_ctrl_cnst_q31;
  /**< @h2xmle_description {Impedance convergence speed control parameter. The
                            higher the value, the faster the convergence, but
                            with more noise.}
       @h2xmle_range       {0..1073741824}
       @h2xmle_default     {21474836}
       @h2xmle_dataFormat  {Q31} */

  int32_t left_re_typ_q24;
  /**< @h2xmle_description {Left speaker: typical resistance of the speaker.
                            (in Ohm)}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {134217728}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_vsen_2_v_q24;
  /**< @h2xmle_description {Left speaker: Vsens scaling constant.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {176294986}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_isen_2_a_q24;
  /**< @h2xmle_description {Left speaker: Isens scaling constant.}
       @h2xmle_range       {-2147483648..2147483647}
       @h2xmle_default     {-117406957}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_amp_gain_q24;
  /**< @h2xmle_description {Left speaker: amplifier gain used when scaling a Tx
                            Vsens signal to an Rx signal.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {92438702}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_re_q24;
  /**< @h2xmle_description {Left speaker: dc resistance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {134217728} */

  int32_t left_le_q24;
  /**< @h2xmle_description {Left speaker: inductance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {838861} */

  int32_t left_bl_q24;
  /**< @h2xmle_description {Left speaker: force factor parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {13421773} */

  int32_t left_fct_q20;
  /**< @h2xmle_description {Left speaker: resonance frequency parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q20}
       @h2xmle_default     {838860800} */

  int32_t left_qmct_q24;
  /**< @h2xmle_description {Left speaker: Q factor parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {67108864} */

  int32_t left_rect_q24;
  /**< @h2xmle_description {Left speaker: mechanical resistance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {67108864} */

  int32_t left_pi_scale_q31;
  /**< @h2xmle_description {Left speaker: scale of the pilot tone relative to
                            0 dBFS (for example, -40 dB from the full scale).}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q31}
       @h2xmle_default     {42847932} */

  int32_t left_tol_range_q15;
  /**< @h2xmle_description {Left speaker: tolerance range for speaker
                            parameters (for example, 20%).}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q15}
       @h2xmle_default     {6554} */

  int32_t right_re_typ_q24;
  /**< @h2xmle_description {Right speaker: typical resistance of the speaker (in
                            Ohms).}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {134217728} */

  int32_t right_vsen_2_v_q24;
  /**< @h2xmle_description {Right speaker: Vsens scaling constant.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {176294986} */

  int32_t right_isen_2_a_q24;
  /**< @h2xmle_description {Right speaker: Isens scaling constant.}
       @h2xmle_range       {-2147483648..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {-117406957} */

  int32_t right_amp_gain_q24;
  /**< @h2xmle_description {Right speaker: amplifier gain used when scaling a
                            Tx Vsens signal to an Rx signal.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {92438702} */

  int32_t right_re_q24;
  /**< @h2xmle_description {Right speaker: dc resistance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {134217728} */

  int32_t right_le_q24;
  /**< @h2xmle_description {Right speaker: inductance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {838861} */

  int32_t right_bl_q24;
  /**< @h2xmle_description {Right speaker: force factor parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {13421773} */

  int32_t right_fct_q20;
  /**< @h2xmle_description {Right speaker: resonance frequency parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q20}
       @h2xmle_default     {838860800} */

  int32_t right_qmct_q24;
  /**< @h2xmle_description {Right speaker: Q factor parameters.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {67108864} */

  int32_t right_rect_q24;
  /**< @h2xmle_description {Right speaker: mechanical resistance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24}
       @h2xmle_default     {67108864} */

  int32_t right_pi_scale_q31;
  /**< @h2xmle_description {Right speaker: scale of the pilot tone relative to
                            0 dBFS (for example, for -40 dB).}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q31}
       @h2xmle_default     {42847932} */

  int32_t right_tol_range_q15;
  /**< @h2xmle_description {Right speaker: tolerance range for speaker
                            parameters (for example, 20%).}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q15}
       @h2xmle_default     {6554} */

  int16_t left_fir_taps;
  /**< @h2xmle_description {Left FIR: Number of taps.}
       @h2xmle_rangeList   {"24"=24; "32"=32; "40"=40; "48"=48}
       @h2xmle_default     {48} */

  int16_t left_fir_coeffq;
  /**< @h2xmle_description {Left FIR: Q factor of coefficients.} */

  int32_t left_fir_coeffs[SP_MAX_FIR_TAP];
  /**< @h2xmle_description {Left FIR: coefficients.}
       @h2xmle_range       {-2147483648..2147483647} */

  int16_t right_fir_taps;
  /**< @h2xmle_description {Right FIR: Number of taps.}
       @h2xmle_rangeList   {"24"=24; "32"=32; "40"=40; "48"=48}
       @h2xmle_default     {48} */

  int16_t right_fir_coeffq;
  /**< @h2xmle_description {Right FIR: Q factor of coefficients.}*/

  int32_t right_fir_coeffs[SP_MAX_FIR_TAP];
  /**< @h2xmle_description {Right FIR: coefficients.} */

}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_EX_VI_DYNAMIC_CFG_V1   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_ex_vi_dynamic_cfg_v1_t
               afe_sp_ex_vi_dynamic_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_EX_VI_DYNAMIC_CFG_V1",
                         AFE_PARAM_ID_SP_EX_VI_DYNAMIC_CFG}
    @h2xmlp_description {parameter used to configure the dynamic parameters of
                         excursion VI processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_EX_VI_DYNAMIC_CFG_V1} */
#include "adsp_begin_pack.h"
struct afe_sp_ex_vi_dynamic_cfg_v1_t
{
  uint32_t sp_ex_vi_dynamic_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_EX_VI_DYNAMIC_CFG_V1}
       @h2xmle_default     {AFE_API_VERSION_SP_EX_VI_DYNAMIC_CFG_V1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t impedance_convergence_level_thr_q15;
  /**< @h2xmle_description {Impedance convergence level threshold.}
       @h2xmle_range       {16384..32767}
       @h2xmle_default     {16384}
       @h2xmle_dataFormat  {Q15} */

  int32_t ex_filter_stable_level_thr_q31;
  /**< @h2xmle_description {Excursion prediction filter stability threshold.}
       @h2xmle_range       {1073741824..2147483647}
       @h2xmle_default     {1073741824}
       @h2xmle_dataFormat  {Q31} */

  int32_t ex_filter_energy_level_thr_q31;
  /**< @h2xmle_description {Excursion prediction filter energy threshold as
                            compared to the shape of the reference filter.}
       @h2xmle_range       {1073741824..2147483647}
       @h2xmle_default     {1073741824}
       @h2xmle_dataFormat  {Q31} */

  int32_t impedance_convergence_speed_ctrl_cnst_q31;
  /**< @h2xmle_description {Impedance convergence speed control parameter. The
                            higher the value, the faster the convergence, but
                            with more noise.}
       @h2xmle_range       {0..1073741824}
       @h2xmle_dataFormat  {Q31} */

  int32_t left_re_typ_q24;
  /**< @h2xmle_description {Left speaker: typical resistance of the speaker.}
       @h2xmle_range       {0..2147483647} Ohms
       @h2xmle_dataFormat  {Q24} */

  int32_t left_vsen_2_v_q24;
  /**< @h2xmle_description {Left speaker: Vsens scaling constant.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_isen_2_a_q24;
  /**< @h2xmle_description {Left speaker: Isens scaling constant.}
       @h2xmle_range       {-2147483648..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_amp_gain_q24;
  /**< @h2xmle_description {Left speaker: amplifier gain used when scaling a Tx
                            Vsens signal to an Rx signal.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_re_q24;
  /**< @h2xmle_description {Left speaker: dc resistance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_le_q24;
  /**< @h2xmle_description {Left speaker: inductance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_bl_q24;
  /**< @h2xmle_description {Left speaker: force factor parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_fct_q20;
  /**< @h2xmle_description {Left speaker: resonance frequency parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q20} */

  int32_t left_qmct_q24;
  /**< @h2xmle_description {Left speaker: Q factor parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_rect_q24;
  /**< @h2xmle_description {Left speaker: mechanical resistance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t left_pi_scale_q31;
  /**< @h2xmle_description {Left speaker: scale of the pilot tone relative to
                            0 dBFS (for example, -40 dB from the full scale).}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q31} */

  int32_t left_tol_range_q15;
  /**< @h2xmle_description {Left speaker: tolerance range for speaker
                            parameters (for example, 20%).}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q15} */

  int32_t right_re_typ_q24;
  /**< @h2xmle_description {Right speaker: typical resistance of the speaker (in
                            Ohms).}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t right_vsen_2_v_q24;
  /**< @h2xmle_description {Right speaker: Vsens scaling constant.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t right_isen_2_a_q24;
  /**< @h2xmle_description {Right speaker: Isens scaling constant.}
       @h2xmle_range       {-2147483648..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t right_amp_gain_q24;
  /**< @h2xmle_description {Right speaker: amplifier gain used when scaling a
                            Tx Vsens signal to an Rx signal.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t right_re_q24;
  /**< @h2xmle_description {Right speaker: dc resistance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t right_le_q24;
  /**< @h2xmle_description {Right speaker: inductance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t right_bl_q24;
  /**< @h2xmle_description {Right speaker: force factor parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t right_fct_q20;
  /**< @h2xmle_description {Right speaker: resonance frequency parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q20} */

  int32_t right_qmct_q24;
  /**< @h2xmle_description {Right speaker: Q factor parameters.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t right_rect_q24;
  /**< @h2xmle_description {Right speaker: mechanical resistance parameter.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t right_pi_scale_q31;
  /**< @h2xmle_description {Right speaker: scale of the pilot tone relative to
                            0 dBFS (for example, for -40 dB).}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q31} */

  int32_t right_tol_range_q15;
  /**< @h2xmle_description {Right speaker: tolerance range for speaker
                            parameters (for example, 20%).}
       @h2xmle_range       {0..32767}
       @h2xmle_dataFormat  {Q15} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_EX_VI_STATS          0x00010272

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_EX_VI_STATS       0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_ex_vi_stats_v1_t afe_sp_ex_vi_stats_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_EX_VI_STATS",
                         AFE_PARAM_ID_SP_EX_VI_STATS}
    @h2xmlp_description {Parameter used to log excursion VI algorithm internal
                         variables.}
    @h2xmlp_version     {AFE_API_VERSION_SP_EX_VI_STATS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_ex_vi_stats_v1_t
{
  uint32_t sp_ex_vi_stats_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter}
       @h2xmle_range       {1..AFE_API_VERSION_SP_EX_VI_STATS}
       @h2xmle_default     {AFE_API_VERSION_SP_EX_VI_STATS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration in milliseconds for Excursion VI
                            statistics.} */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the Excursion VI statistics. This
                            field is reset to zero after crossing the maximum
                            limit for uint32 variables.} */

  uint32_t exc_fine_tune_en_flag;
  /**< @h2xmle_description {Specifies whether fine tuning is enabled for
                            Excursion statistics.}
       @h2xmle_rangeList   {disabled=0;enabled=1} */

  int32_t impedance_mag_response[SP_NUM_MAX_SPKRS][SP_NUM_FFT_BINS];
  /**< @h2xmle_description {Impedance magnitude response.}
       @h2xmle_range       {-2147483648..2147483647}   */

  int32_t excursion_mag_response[SP_NUM_MAX_SPKRS][SP_NUM_FFT_BINS];
  /**< @h2xmle_description {Excursion magnitude response.}
       @h2xmle_range       {-2147483648..2147483647}   */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_ADV_EX_VI_STATS      0x0001029A

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_ADV_EX_VI_STATS   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_adv_ex_vi_stats_v1_t afe_sp_adv_ex_vi_stats_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_ADV_EX_VI_STATS",
                         AFE_PARAM_ID_SP_ADV_EX_VI_STATS}
    @h2xmlp_description {parameter used o log advanced excursion VI algorithm
                         internal variables.}
    @h2xmlp_version     {AFE_API_VERSION_SP_ADV_EX_VI_STATS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_adv_ex_vi_stats_v1_t
{
  uint32_t sp_adv_ex_vi_stats_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_ADV_EX_VI_STATS}
       @h2xmle_default     {AFE_API_VERSION_SP_ADV_EX_VI_STATS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration in milliseconds for advanced
                            Excursion VI statistics.} */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the advanced Excursion VI
                            statistics. This field is  reset to zero after it
                            crosses the maximum limit for uint32 variables.} */

  int32_t impedance_response[SP_NUM_MAX_SPKRS]
                            [SP_NUM_FFT_BINS * SP_FACT_CMPLX];
  /**< @h2xmle_description {Impedance response.}
       @h2xmle_range       {-2147483648..2147483647}   */

  int32_t re[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Predicted DC voice coil resistance.} */

  int32_t le[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Predicted voice coil inductance.} */

  int32_t bl[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Predicted voice coil force factor.} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_EX_VI_MODE_CFG       0x000102A1

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_EX_VI_MODE_CFG    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_ex_vi_mode_cfg_v1_t afe_sp_ex_vi_mode_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_EX_VI_MODE_CFG",
                         AFE_PARAM_ID_SP_EX_VI_MODE_CFG}
    @h2xmlp_description {parameter used to configure the mode of Feedback
                         Speaker Protection Excursion VI processing.}
    @h2xmlp_version     {AFE_API_VERSION_SP_EX_VI_MODE_CFG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_ex_vi_mode_cfg_v1_t
{
  uint32_t sp_ex_vi_mode_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_EX_VI_MODE_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_EX_VI_MODE_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t operation_mode;
  /**< @h2xmle_description {Operation mode of the Excursion VI module.}
       @h2xmle_rangeList   {"Normal Running mode"=0;
                            "FTM mode"=1} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_EX_VI_FTM_CFG        0x000102A2

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_EX_VI_FTM_CFG     0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_ex_vi_ftm_cfg_v1_t afe_sp_ex_vi_ftm_cfg_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_EX_VI_FTM_CFG",
                         AFE_PARAM_ID_SP_EX_VI_FTM_CFG}
    @h2xmlp_description {parameter used to set the FTM configuration in the
                         Excursion VI processing module.}
    @h2xmlp_version     {AFE_API_VERSION_SP_EX_VI_FTM_CFG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_ex_vi_ftm_cfg_v1_t
{
  uint32_t sp_ex_vi_ftm_cfg_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_EX_VI_FTM_CFG}
       @h2xmle_default     {AFE_API_VERSION_SP_EX_VI_FTM_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t wait_time_ms[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Wait time to heat up the speaker before collecting
                            statistics for FTM mode (in milli seconds).}
       @h2xmle_range       {0..4294967295} */

  uint32_t ftm_time_ms[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Period of time when FTM statistics are collected (in
                            milli seconds).}
       @h2xmle_range       {0..2000} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SP_EX_VI_FTM_PARAMS     0x000102A3

/* Minor Version of this PARAM */
#define AFE_API_VERSION_SP_EX_VI_FTM_PARAMS  0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sp_ex_vi_ftm_params_v1_t afe_sp_ex_vi_ftm_params_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SP_EX_VI_FTM_PARAMS",
                         AFE_PARAM_ID_SP_EX_VI_FTM_PARAMS}
    @h2xmlp_description {parameter used to get the FTM statistics from the
                         Excursion VI module.}
    @h2xmlp_version     {AFE_API_VERSION_SP_EX_VI_FTM_PARAMS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_sp_ex_vi_ftm_params_v1_t
{
  uint32_t sp_ex_vi_ftm_params_minor_version;
  /**< @h2xmle_description {Minor version used for tracking the version of
                            this parameter ID}
       @h2xmle_range       {1..AFE_API_VERSION_SP_EX_VI_FTM_PARAMS}
       @h2xmle_default     {AFE_API_VERSION_SP_EX_VI_FTM_PARAMS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t ftm_fct_q20[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Resonance frequency (in Hz).}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q20} */

  int32_t ftm_rect_q24[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Mechanical resistance.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  int32_t ftm_qmct_q24[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {Mechanical Qfactor.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_dataFormat  {Q24} */

  uint32_t status[SP_NUM_MAX_SPKRS];
  /**< @h2xmle_description {FTM packet status.}
       @h2xmle_rangeList   {"Incorrect operation mode (isn't normal mode)"=0;
                            "Inactive mode (port is not started)"=1;
                            "Wait state"=2;
                            "In Progress state"=3;
                            "Success"=4;
                            "Failed"=5} */
}
#include "adsp_end_pack.h"
;

/** @h2xml_Select         {afe_mod_enable_param_t}
    @h2xmlm_InsertParameter */

/** @} <-- End of the Module --> */

#endif /* __SPEAKER_PROTECTION_H__ */
