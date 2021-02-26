#ifndef __AANC_H__
#define __AANC_H__

/*==============================================================================
  @file aanc.h
  @brief This file contains APIs for Adaptive Active Noise Cancellation Module

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.  Export of this
  technology or software is regulated by the U.S. Government, Diversion contrary
  to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/prop/aanc.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  05/20/17    sw        (Tech Pubs) Edited descriptions and values
==============================================================================*/

/** @h2xml_title1           {Adaptive Active Noise Cancellation Module}
    @h2xml_title_date       {January 3, 2017} */

#include "mmdefs.h"
#include "afe_common.h"

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Module ID */
#define AFE_MODULE_AANC             0x00010214

/** @h2xmlm_module      {"AFE_MODULE_AANC",AFE_MODULE_AANC}
    @h2xmlm_toolPolicy  {Calibration;RTC}
    @h2xmlm_description {Supports the Adaptive Active Noise Cancellation
                         (AANC) feature.}
    @{                  <-- Start of the Module --> */

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_AANC_PORT_CONFIG    0x00010215

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_PORT_CONFIG 0x1

/* Enumeration for indicating if a channel is unused */
#define AANC_TX_MIC_UNUSED      0

/* Enumeration for indicating if a channel contains voice samples */
#define AANC_TX_VOICE_MIC       1

/* Enumeration for indicating if a channel contains error mic samples */
#define AANC_TX_ERROR_MIC       2

/* Enumeration for indicating if a channel contains noise mic samples */
#define AANC_TX_NOISE_MIC       3

/* Max number of AANC channel map count */
#define AANC_TX_CHANNEL_MAP_CNT 8

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_aanc_port_cfg_param_v1_t afe_aanc_port_cfg_param_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_PORT_CONFIG",
                         AFE_PARAM_ID_AANC_PORT_CONFIG}
    @h2xmlp_description {Configures a port for the AANC algorithm.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_PORT_CONFIG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_port_cfg_param_v1_t
{
  uint32_t aanc_port_cfg_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_PORT_CONFIG}
       @h2xmle_default     {AFE_API_VERSION_AANC_PORT_CONFIG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t tx_port_sample_rate;
  /**< @h2xmle_description {Sampling rate of the source Tx port.}
       @h2xmle_rangeList   {"SAMPLE_RATE_8K"=8000,
                            "SAMPLE_RATE_16K"=16000,
                            "SAMPLE_RATE_48K"=48000,
                            "SAMPLE_RATE_96K"=96000,
                            "SAMPLE_RATE_192K"=192000}
       @h2xmle_default     {8000} */

  uint8_t tx_port_channel_map[AANC_TX_CHANNEL_MAP_CNT];
  /**< @h2xmle_description {Array of channel mapping for the Tx port signal
                            carrying noise (X), error (E), and voice (V)
                            signals.}
       @h2xmle_rangeList   {"AANC_TX_MIC_UNUSED"=AANC_TX_MIC_UNUSED,
                            "AANC_TX_VOICE_MIC"=AANC_TX_VOICE_MIC,
                            "AANC_TX_ERROR_MIC"=AANC_TX_ERROR_MIC,
                            "AANC_TX_NOISE_MIC"=AANC_TX_NOISE_MIC}
       @h2xmlx_expandArray {true}
       @h2xmle_default     {AANC_TX_MIC_UNUSED} */

  uint16_t tx_port_num_channels;
  /**< @h2xmle_description {Number of channels on the source Tx port.}
       @h2xmle_range       {1..AANC_TX_CHANNEL_MAP_CNT}
       @h2xmle_default     {3} */

  uint16_t rx_path_ref_port_id;
  /**< @h2xmle_description {AFE port ID of the Rx path reference signal.} */

  uint32_t ref_port_sample_rate;
  /**< @h2xmle_description {Sampling rate of the reference port.}
       @h2xmle_rangeList   {"SAMPLE_RATE_8K"=8000,
                            "SAMPLE_RATE_16K"=16000,
                            "SAMPLE_RATE_48K"=48000,
                            "SAMPLE_RATE_96K"=96000,
                            "SAMPLE_RATE_192K"=192000}
       @h2xmle_default     {8000} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_AANC_CFG1          0x00010216

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_CFG1       0x2

/* Enumeration for indicating the maximum length of the IIR filter */
#define ADAP_ANC_IIR_MAX_FILT_LEN       24

/* Enumeration for indicating the maximum length of the prefilter */
#define ADAP_ANC_PREFILT_MAX_FILT_LEN   5

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_aanc_cfg1_param_v2_t afe_aanc_cfg1_param_v2_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_CFG1_V2", AFE_PARAM_ID_AANC_CFG1}
    @h2xmlp_description {Static configuration for the AANC algorithm.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_CFG1} */
#include "adsp_begin_pack.h"
struct afe_aanc_cfg1_param_v2_t
{
  uint32_t aanc_cfg1_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_CFG1}
       @h2xmle_default     {AFE_API_VERSION_AANC_CFG1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t frame_size;
  /**< @h2xmle_description {Input frame size for voice, error, noise, and
                            Rx path signals.}
       @h2xmle_rangeList   {"80"=80, "160"=160, "240"=240}
       @h2xmle_default     {160} */

  int32_t ad_gain_default;
  /**< @h2xmle_description {Default adaptive gain.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x2000} */

  uint16_t ad_skip_k;
  /**< @h2xmle_description {Number of samples to skip from the start of the
                            frame.}
       @h2xmle_range       {0..240}
       @h2xmle_default     {0} */

  uint16_t vad_gate_hist_len;
  /**< @h2xmle_description {Length of the energy estimate history buffer (for
                            minimum statistics).}
       @h2xmle_range       {1..1024}
       @h2xmle_default     {0x32} */

  int32_t vad_gate_hist_init_val;
  /**< @h2xmle_description {Initial value of the energy history buffer.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x7FFF} */

  uint16_t prefilt_biquad_iir_num_len;
  /**< @h2xmle_description {Length of the numerator coefficient for the
                            prefilter biquad IIR filter.}
       @h2xmle_range       {1..5}
       @h2xmle_default     {3} */

  uint16_t prefilt_biquad_iir_deno_len;
  /**< @h2xmle_description {Length of the denominator coefficient for the
                            prefilter biquad IIR filter.}
       @h2xmle_range       {1..5}
       @h2xmle_default     {3} */

  int32_t prefilt_biquad_iir_num[ADAP_ANC_PREFILT_MAX_FILT_LEN];
  /**< @h2xmle_description {Numerator coefficient for the prefilter biquad
                            IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {0x04AB, 0x0, 0xFFFFFB55, 0x0, 0x0} */

  int32_t prefilt_biquad_iir_deno[ADAP_ANC_PREFILT_MAX_FILT_LEN];
  /**< @h2xmle_description {Denominator coefficient for the prefilter biquad
                            IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {0x4000, 0xFFFF9208, 0x36A9, 0x0, 0x0} */

  uint16_t spath_biquad_iir_num_len;
  /**< @h2xmle_description {Length of the numerator coefficient for the
                            speaker transfer function biquad IIR filter.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {8} */

  uint16_t spath_biquad_iir_deno_len;
  /**< @h2xmle_description {Length of the denominator coefficient for the
                            speaker transfer function biquad IIR filter.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {8} */

  int32_t spath_biquad_iir_num[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Numerator coefficient for the speaker transfer
                            function biquad IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {0xFFFFFB0E, 0xFFFFDB5B, 0x3119, 0x0BAA, 0xFFFFFC39,
                            0xFFFFF387, 0xFFFFFC86, 0x97, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0} */

  int32_t spath_biquad_iir_deno[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Denominator coefficient for the speaker transfer
                            function biquad IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {0x4000, 0xFFFFBE58, 0xFFFFFA62, 0x159E, 0xFFFFE1E5,
                            0x06C1, 0x1546, 0xFFFFFA6C, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0} */

  uint16_t wfilt_biquad_iir_num_len;
  /**< @h2xmle_description {Length of the numerator coefficient for the
                           adaptive LMS biquad IIR filter.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {8} */

  uint16_t wfilt_biquad_iir_deno_len;
 /**< @h2xmle_description {Length of the denominator coefficient for the
                           adaptive LMS biquad IIR filter.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {8} */

  int32_t wfilt_biquad_iir_num[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Numerator coefficient for the adaptive LMS biquad
                            IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {0x3183, 0xFFFFEDCC, 0x3119, 0xFFFFF362, 0xFFFFF6D4,
                            0xFFFFEAEA, 0xFFFFFC86, 0x0DA1, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0} */

  int32_t wfilt_biquad_iir_deno[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Denominator coefficient for the adaptive LMS biquad
                            IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {0x4000, 0xFFFF9B7D, 0x6758, 0xFFFF9F56, 0x1106,
                            0x091E, 0xFFFFF13D, 0x130A, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0} */

  uint32_t fb_gate_static_enable;
  /**< @h2xmle_description {Enables the feedback gate on the static path. When
                            disabled, this field completely frees memory and
                            bypasses computation.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  uint32_t fb_gate_sample_frequency;
  /**< @h2xmle_description {Sample frequency of the feedback gate. This
                            frequency is static; it is related to memory
                            requirements.}
       @h2xmle_rangeList   {"8 kHz"=8000; "16 kHz"=16000}
       @h2xmle_default     {8000} */

  uint16_t fb_gate_fpath_iir_num_len;
  /**< @h2xmle_description {Length of the numerator coefficient for the
                            feedback path IIR filter of the feedback gate.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {1} */

  uint16_t fb_gate_fpath_iir_deno_len;
  /**< @h2xmle_description {Length of the denominator coefficient for the
                            feedback path IIR filter of the feedback gate.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {1} */

  int32_t fb_gate_fpath_iir_num[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Numerator coefficient for the feedback path IIR
                            filter of the feedback gate.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {16384, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0} */

  int32_t fb_gate_fpath_iir_deno[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Denominator coefficient for the feedback path IIR
                            filter of the feedback gate.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {16384, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0} */

  uint16_t fb_gate_spath_iir_num_len;
  /**< @h2xmle_description {Length of the numerator coefficient for the
                            speaker path IIR filter of the feedback gate.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {1} */

  uint16_t fb_gate_spath_iir_deno_len;
  /**< @h2xmle_description {Length of the denominator coefficient for the
                            speaker path IIR filter of the feedback gate.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {1} */

  int32_t fb_gate_spath_iir_num[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Numerator coefficient for the speaker path IIR
                            filter of the feedback gate.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {16384, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0} */

  int32_t fb_gate_spath_iir_deno[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Denominator coefficient for the speaker path IIR
                            filter of the feedback gate.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {16384, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/
/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_CFG1_V1       0x1

/*==============================================================================
   Type definitions
==============================================================================*/
/* Deprecated. Structure definition for Parameter */
typedef struct afe_aanc_cfg1_param_v1_t afe_aanc_cfg1_param_v1_t;

/** Deprecated. Use #afe_aanc_cfg1_param_v2_t.

    @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_CFG1_V1", AFE_PARAM_ID_AANC_CFG1}
    @h2xmlp_description {Static configuration for the AANC algorithm.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_CFG1_V1}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_cfg1_param_v1_t
{
  uint32_t aanc_cfg1_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_CFG1_V1}
       @h2xmle_default     {AFE_API_VERSION_AANC_CFG1_V1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {AFE_API_VERSION_AANC_CFG1_V1} */

  uint32_t frame_size;
  /**< @h2xmle_description {Input frame size for voice, error, noise, and
                            Rx path signals.}
       @h2xmle_rangeList   {"80"=80; "160"=160; "240"=240}
       @h2xmle_default     {80} */

  int32_t ad_gain_default;
  /**< @h2xmle_description {Default adaptive gain.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767} */

  uint16_t ad_skip_k;
  /**< @h2xmle_description {Number of samples to skip from the start of the
                            frame.}
       @h2xmle_range       {0..240} */

  uint16_t vad_gate_hist_len;
  /**< @h2xmle_description {Length of the energy estimate history buffer (for
                            minimum statistics).}
       @h2xmle_range       {1..1024}
       @h2xmle_default     {1} */

  int32_t vad_gate_hist_init_val;
  /**< @h2xmle_description {Initial value of the energy history buffer.}
       @h2xmle_range       {0..32767} */

  uint16_t prefilt_biquad_iir_num_len;
  /**< @h2xmle_description {Length of the numerator coefficient for the
                            prefilter biquad IIR filter.}
       @h2xmle_range       {1..5}
       @h2xmle_default     {1} */

  uint16_t prefilt_biquad_iir_deno_len;
  /**< @h2xmle_description {Length of the denominator coefficient for the
                            prefilter biquad IIR filter.}
       @h2xmle_range       {1..5}
       @h2xmle_default     {1} */

  int32_t prefilt_biquad_iir_num[ADAP_ANC_PREFILT_MAX_FILT_LEN];
  /**< @h2xmle_description {Numerator coefficient for the prefilter biquad
                            IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767} */

  int32_t prefilt_biquad_iir_deno[ADAP_ANC_PREFILT_MAX_FILT_LEN];
  /**< @h2xmle_description {Denominator coefficient for the prefilter biquad
                            IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767} */

  uint16_t spath_biquad_iir_num_len;
  /**< @h2xmle_description {Length of the numerator coefficient for the
                            speaker transfer function biquad IIR filter.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {1} */

  uint16_t spath_biquad_iir_deno_len;
  /**< @h2xmle_description {Length of the denominator coefficient for the
                            speaker transfer function biquad IIR filter.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {1} */

  int32_t spath_biquad_iir_num[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Numerator coefficient for the speaker transfer
                            function biquad IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767} */

  int32_t spath_biquad_iir_deno[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Denominator coefficient for the speaker transfer
                            function biquad IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767} */

  uint16_t wfilt_biquad_iir_num_len;
  /**< @h2xmle_description {Length of the numerator coefficient for the
                            adaptive LMS biquad IIR filter.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {1} */

  uint16_t wfilt_biquad_iir_deno_len;
  /**< @h2xmle_description {Length of the denominator coefficient for the
                            adaptive LMS biquad IIR filter.}
       @h2xmle_range       {1..24}
       @h2xmle_default     {1} */

  int32_t wfilt_biquad_iir_num[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Numerator coefficient for the adaptive LMS biquad
                            IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767} */

  int32_t wfilt_biquad_iir_deno[ADAP_ANC_IIR_MAX_FILT_LEN];
  /**< @h2xmle_description {Denominator coefficient for the adaptive LMS biquad
                            IIR filter.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767} */
}
#include "adsp_end_pack.h"
;


/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_AANC_CFG2   0x00010217

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_CFG2 0x5

/* Algorithm feature for wind gate, corresponding to bit 0 in the features
   word in wnd_gate_enable of afe_aanc_cfg2_param_v3_t */
#define ADANC_WND_GATE     (1 << 0)

/* Algorithm feature for wind detection, corresponding to bit 1 in the
   features word in wnd_gate_enable of afe_aanc_cfg2_param_v3_t */
#define ADANC_WND_DETECT   (1 << 1)

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_aanc_cfg2_param_v5_t afe_aanc_cfg2_param_v5_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_CFG2_V5", AFE_PARAM_ID_AANC_CFG2}
    @h2xmlp_description {Dynamic configuration for the AANC algorithm.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_CFG2}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_cfg2_param_v5_t
{
  uint32_t aanc_cfg2_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_CFG2}
       @h2xmle_default     {AFE_API_VERSION_AANC_CFG2}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint16_t adapt_anc_enable;
  /**< @h2xmle_description {Enables AANC.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t alc_enable;
  /**< @h2xmle_description {Enables the adaptive-level controller.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t pre_filt_enable;
  /**< @h2xmle_description {Enables the prefilter.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t rx_factor_enable;
  /**< @h2xmle_description {Enables the Rx energy factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t vad_factor_enable;
  /**< @h2xmle_description {Enables the VAD factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t noise_gate_enable;
  /**< @h2xmle_description {Enables the noise gate.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  int32_t ad_alpha;
  /**< @h2xmle_description {Learning rate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x400} */

  int32_t ad_delta_g2_bound;
  /**< @h2xmle_description {Absolute maximum of delta G2.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x400} */

  int32_t ad_gain_bound[2];
  /**< @h2xmle_description {Minimum and maximum values of adaptive gain.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {0x04AB, 0x3FAC} */

  int32_t alc_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the adaptive-level controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x666} */

  int32_t alc_cfg_ftarget;
  /**< @h2xmle_description {Gain curve target of the adaptive-level controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x2B9} */

  int32_t alc_cfg_fthresh;
  /**< @h2xmle_description {Gain curve threshold of the adaptive-level controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x57} */

  int32_t rx_gate_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x199A} */

  int32_t rx_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 of the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x64} */

  int32_t rx_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 of the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x190} */

  int32_t vad_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 of the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x190} */

  int32_t vad_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 of the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x150} */

  int32_t noise_gate_cfg_thresh;
  /**< @h2xmle_description {Noise gate threshold.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x400} */

  int32_t noise_gate_cfg_trstep;
  /**< @h2xmle_description {Noise gate transition step.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x40} */

  uint16_t block_gate_enable;
  /**< @h2xmle_description {Enables the block gate.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  uint16_t block_offset_enable;
  /**< @h2xmle_description {Enables the block offset.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  int32_t block_gate_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0xA4} */

  int32_t block_gate_cfg_discount_factor;
  /**< @h2xmle_description {Discount factor of the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x7EB8} */

  int32_t block_gate_cfg_weight_factor;
  /**< @h2xmle_description {Weighting factor of the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x800} */

  int32_t block_gate_cfg_rx_vad_threshold;
  /**< @h2xmle_description {Rx VAD threshold of the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x40} */

  int32_t block_gate_cfg_rx_offset_factor;
  /**< @h2xmle_description {Factor for generating an offset.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x200} */

  int32_t noise_gate_cfg_thresh2;
  /**< @h2xmle_description {Noise gate threshold 2.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x5000} */

  int32_t noise_gate_cfg_thresh2targetgain;
  /**< @h2xmle_description {Safe target gain for noise gate threshold 2.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0} */

  int32_t block_gate_cfg_discount_gain_threshold;
  /**< @h2xmle_description {Threshold on ANC gain that determines the dynamic
                            discount factor.}
       @h2xmle_dataformat  {Q13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x800} */

  int32_t block_gate_cfg_err_limit_factor;
  /**< @h2xmle_description {Safe target gain for the block gate.}
       @h2xmle_dataformat  {Q13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x3000} */

  int32_t wnd_gate_enable;
  /**< @h2xmle_description {Wind noise detection feature.}
       @h2xmle_range       {0..3}
       
       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"ADANC_WND_GATE"}
       @h2xmle_description {Enables the gate adjustment.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"ADANC_WND_DETECT"}
       @h2xmle_description {Enables wind detection.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd  */

  int32_t wnd_tran_step;
  /**< @h2xmle_description {Transition step for wind noise.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x9C4} */

  int32_t wnd_min_gain;
  /**< @h2xmle_description {Minimum gain when wind is detected.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0} */

  int32_t wnd_hangover;
  /**< @h2xmle_description {After wind is detected, hangover before declaring
                            no wind.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0xC8} */

  int32_t wnd_sec_mic;
  /**< @h2xmle_description {Secondary mic for wind noise detection.}
       @h2xmle_range       {0..2}
       @h2xmle_default     {1} */

  int32_t wnd_cov_eps;
  /**< @h2xmle_description {Small initialization constant for the covariance
                            (per sample).}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x7A120} */

  int32_t wnd_det_th[2];
  /**< @h2xmle_description {Lower and upper thresholds for wind measurement.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_defaultList {0x1999999A, 0x33333333} */

  int32_t wnd_detect_thresh_l32q31;
  /**< @h2xmle_description {Wind detection threshold.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x3B9ACA00} */

  int32_t wnd_silent_thresh_l32q0;
  /**< @h2xmle_description {Wind silence threshold.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x0C3500} */

  int16_t wnd_c_alph;
  /**< @h2xmle_description {Smoothing alpha for the covariance statistics.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x4000} */

  int16_t wnd_cov_bit_shf;
  /**< @h2xmle_description {Right bit shift for correction before converting to
                            int32.}
       @h2xmle_range       {-32..62}
       @h2xmle_default     {0x7} */

  int16_t wnd_ws_alph_inc;
  /**< @h2xmle_description {Smoothing alpha for the wind score when wind
                            increases.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x4000} */

  int16_t wnd_ws_alph_dec;
  /**< @h2xmle_description {Smoothing alpha for the wind score when wind
                            decreases.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x2000} */

  int16_t wnd_boost_s16q10;
  /**< @h2xmle_description {Boosting self-standard deviation for wind
                            determination.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x800} */

  int16_t wnd_fact_s16q15;
  /**< @h2xmle_description {Percentage of low-frequency standard deviation in
                            wind discrimination.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x2000} */

  int32_t sat_gate_enable;
  /**< @h2xmle_description {Enables the saturation gate.}
       @h2xmle_rangeList   {Off=0;On=1}
       @h2xmle_default     {1} */

  int32_t sat_gate_cfg_pcm_thresh;
  /**< @h2xmle_description {Threshold for a PCM sample before saturation
                            (typically 2^15-1).}
       @h2xmle_range       {0..65535}
       @h2xmle_default     {0x6200} */

  int32_t sat_gate_cfg_count_thresh;
  /**< @h2xmle_description {Threshold count to determine if saturation occurs.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x5} */

  int32_t sat_gate_cfg_gain_thresh;
  /**< @h2xmle_description {Threshold on gain to determine if saturation
                            occurs.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x500} */

  int32_t sat_gate_cfg_hangover_len;
  /**< @h2xmle_description {Length of hangover frames when saturation is
                            detected.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x64} */

  int32_t sat_gate_cfg_trans_step;
  /**< @h2xmle_description {Gain reduction delta when saturation is detected.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x400} */

  int32_t sat_gate_cfg_min_gain;
  /**< @h2xmle_description {Lower limit of the gain transition target when
                            saturation is detected.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0} */

  int32_t prox_gate_enable;
  /**< @h2xmle_description {Enables the proximity detector.}
       @h2xmle_rangeList   {Off=0;On=1}
       @h2xmle_default     {0} */

  int32_t prox_gate_trans_step;
  /**< @h2xmle_description {Gain reduction delta when proximity is not
                            detected.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x64} */

  int32_t prox_gate_min_gain;
  /**< @h2xmle_description {Lower limit of gain reduction.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0} */

  int32_t prox_gate_trigger_hangover;
  /**< @h2xmle_description {Hangover before the proximity detector flag is
                            accepted.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {1} */

  int32_t fb_gate_dynamic_enable;
  /**< @h2xmle_description {Dynamically enables the feedback gate while AANC
                            is running.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  int32_t fb_gate_criteria_type;
  /**< @h2xmle_description {Type of normalization criteria for the feedback gate
                            correlation method.}
       @h2xmle_rangeList   {"Fully normalized with error and noise"=0;
                            "Half normalized with error"=1;
                            "Half normalized with noise"=2;
                            "Correlation"=3}
       @h2xmle_default     {0} */

  int32_t fb_gate_threshold;
  /**< @h2xmle_description {Threshold to compare against the feedback criteria.}
       @h2xmle_dataformat  {Q24.8}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {128} */

  int32_t fb_gate_sm_beta1;
  /**< @h2xmle_description {Smoothing beta for the XX, XE, and EE energies.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {26214} */

  int32_t fb_gate_sm_beta2;
  /**< @h2xmle_description {Smoothing beta for XE/sqrt(XX*EE).}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {3276} */

  int32_t fb_gate_rx_discount_factor;
  /**< @h2xmle_description {Factor multiplied with Rx power for subtraction
                            from correlation.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {10} */

  int32_t fb_gate_hangover;
  /**< @h2xmle_description {Hangover frames when feedback is detected.}
       @h2xmle_dataformat  {Q16}
       @h2xmle_range       {0..32768}
       @h2xmle_default     {100} */

  int32_t fb_gate_trans_step;
  /**< @h2xmle_description {Gain reduction delta when feedback is detected.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..32768}
       @h2xmle_default     {128} */

  int32_t fb_gate_min_gain;
  /**< @h2xmle_description {Lower limit of the gain transition target when
                            feedback is detected.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0} */

  int32_t static_gate_enable;
  /**< @h2xmle_description {Specifies whether the output gain is incrementally
                            increased each frame to the default gain.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  int32_t static_gate_trans_step;
  /**< @h2xmle_description {Transition-up step in static ANC mode.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..32768}
       @h2xmle_default     {64} */

  int32_t noise_gate_cfg_min_gain;
  /**< @h2xmle_description {Noise gate minimum gain.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0} */

  int32_t vad_gate_vthresh;
  /**< @h2xmle_description {Threshold for voice activity detection.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {512} */

  int32_t vad_gate_sm_beta1;
  /**< @h2xmle_description {Smoothing on voice activity detection to estimate
                            voice.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {27853} */

  int32_t vad_gate_sm_beta2;
  /**< @h2xmle_description {Smoothing on noise estimates.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {1638} */

  int32_t vad_gate_hist_min_val;
  /**< @h2xmle_description {Lower limit of the minimum energy used to protect
                            division operation.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {1} */

  int32_t noise_gate_cfg_mode;
  /**< @h2xmle_description {Configuration mode of the noise gate.}
       @h2xmle_rangeList   {"Default VAD minimum statistics estimate method"=0;
                            "VAD probability weighted noise estimate method"=1}
       @h2xmle_default     {0} */

  int32_t noise_gate_cfg_thresh1p5;
  /**< @h2xmle_description {Noise gate threshold used to turn on AANC
                            (from the Off state).}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {6} */

  int32_t noise_gate_cfg_thresh2p5;
  /**< @h2xmle_description {Noise gate threshold used to turn off AANC
                            (from the On state).}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {16383} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_CFG2_V4 0x4

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_aanc_cfg2_param_v4_t afe_aanc_cfg2_param_v4_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_CFG2_V4", AFE_PARAM_ID_AANC_CFG2}
    @h2xmlp_description {Dynamic configuration for the AANC algorithm.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_CFG2_V4} */
#include "adsp_begin_pack.h"
struct afe_aanc_cfg2_param_v4_t
{
  uint32_t aanc_cfg2_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_CFG2_V4}
       @h2xmle_default     {AFE_API_VERSION_AANC_CFG2_V4}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint16_t adapt_anc_enable;
  /**< @h2xmle_description {Enables AANC.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t alc_enable;
  /**< @h2xmle_description {Enables the adaptive-level controller.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t pre_filt_enable;
  /**< @h2xmle_description {Enables the prefilter.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t rx_factor_enable;
  /**< @h2xmle_description {Enables the Rx energy factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t vad_factor_enable;
  /**< @h2xmle_description {Enables the VAD factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t noise_gate_enable;
  /**< @h2xmle_description {Enables the noise gate.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  int32_t ad_alpha;
  /**< @h2xmle_description {Learning rate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x400} */

  int32_t ad_delta_g2_bound;
  /**< @h2xmle_description {Absolute maximum of delta G2.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x400} */

  int32_t ad_gain_bound[2];
  /**< @h2xmle_description {Minimum and maximum values of adaptive gain.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_defaultList {0x400, 0x3FAC} */

  int32_t alc_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the adaptive-level controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x666} */

  int32_t alc_cfg_ftarget;
  /**< @h2xmle_description {Gain curve target for the adaptive-level
                            controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x2B9} */

  int32_t alc_cfg_fthresh;
  /**< @h2xmle_description {Gain curve target threshold for the adaptive-level
                            controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x57} */

  int32_t rx_gate_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x199A} */

  int32_t rx_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x64} */

  int32_t rx_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x190} */

  int32_t vad_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 for the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x190} */

  int32_t vad_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 for the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x150} */

  int32_t noise_gate_cfg_thresh;
  /**< @h2xmle_description {Noise gate threshold.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x400} */

  int32_t noise_gate_cfg_trstep;
  /**< @h2xmle_description {Noise gate transition step.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x40} */

  uint16_t block_gate_enable;
  /**< @h2xmle_description {Enables the block gate.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  uint16_t block_offset_enable;
  /**< @h2xmle_description {Enables the block offset.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  int32_t block_gate_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0xA4} */

  int32_t block_gate_cfg_discount_factor;
  /**< @h2xmle_description {Discount factor for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x7EB8} */

  int32_t block_gate_cfg_weight_factor;
  /**< @h2xmle_description {Weighting factor for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x800} */

  int32_t block_gate_cfg_rx_vad_threshold;
  /**< @h2xmle_description {Rx VAD threshold for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x40} */

  int32_t block_gate_cfg_rx_offset_factor;
  /**< @h2xmle_description {Factor for generating an offset.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x200} */

  int32_t noise_gate_cfg_thresh2;
  /**< @h2xmle_description {Noise gate threshold 2.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x5000} */

  int32_t noise_gate_cfg_thresh2targetgain;
  /**< @h2xmle_description {Safe target gain for noise gate threshold 2.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0} */

  int32_t block_gate_cfg_discount_gain_threshold;
  /**< @h2xmle_description {Threshold on ANC gain that determines the dynamic
                            discount factor.}
       @h2xmle_dataformat  {Q13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x800} */

  int32_t block_gate_cfg_err_limit_factor;
  /**< @h2xmle_description {Safe target gain for the block gate.}
       @h2xmle_dataformat  {Q13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x3000} */

  int32_t wnd_gate_enable;
  /**< @h2xmle_description {Wind noise detection feature.}
       @h2xmle_range       {0..3}
       
       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"ADANC_WND_GATE"}
       @h2xmle_description {Enables the gate adjustment.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"ADANC_WND_DETECT"}
       @h2xmle_description {Enables detection.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd  */

  int32_t wnd_tran_step;
  /**< @h2xmle_description {Transition step for wind noise.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0x9C4} */

  int32_t wnd_min_gain;
  /**< @h2xmle_description {Minimum gain when wind is detected.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0} */

  int32_t wnd_hangover;
  /**< @h2xmle_description {After wind is detected, hangover before declaring
                            no wind.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0xC8} */

  int32_t wnd_sec_mic;
  /**< @h2xmle_description {Secondary mic for wind noise detection.}
       @h2xmle_range       {0..2}
       @h2xmle_default     {1} */

  int32_t wnd_cov_eps;
  /**< @h2xmle_description {Small initialization constant for the covariance,
                            per sample.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x7A120} */

  int32_t wnd_det_th[2];
  /**< @h2xmle_description {Lower and upper thresholds for wind measurement.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_defaultList {0x1999999A, 0x33333333} */

  int32_t wnd_detect_thresh_l32q31;
  /**< @h2xmle_description {Wind detection threshold.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x3B9ACA00} */

  int32_t wnd_silent_thresh_l32q0;
  /**< @h2xmle_description {Wind silence threshold.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x0C3500} */

  int16_t wnd_c_alph;
  /**< @h2xmle_description {Smoothing alpha for the covariance statistics.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x4000} */

  int16_t wnd_cov_bit_shf;
  /**< @h2xmle_description {Right bit shift for correction before converting
                            to int32.}
       @h2xmle_range       {-32..62}
       @h2xmle_default     {0x7} */

  int16_t wnd_ws_alph_inc;
  /**< @h2xmle_description {Smoothing alpha for the wind score when wind
                            increases.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x4000} */

  int16_t wnd_ws_alph_dec;
  /**< @h2xmle_description {Smoothing alpha for the wind score when wind
                            decreases.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x2000} */

  int16_t wnd_boost_s16q10;
  /**< @h2xmle_description {Boosting self-standard deviation for wind
                            determination.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x800} */

  int16_t wnd_fact_s16q15;
  /**< @h2xmle_description {Percentage of low-frequency standard deviation in
                            wind discrimination.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x2000} */

  int32_t sat_gate_enable;
  /**< @h2xmle_description {Enables the saturation gate.}
       @h2xmle_rangeList   {Off=0;On=1}
       @h2xmle_default     {1} */

  int32_t sat_gate_cfg_pcm_thresh;
  /**< @h2xmle_description {Threshold for a PCM sample before saturation
                            (typically 2^15-1).}
       @h2xmle_range       {0..65535}
       @h2xmle_default     {0x6200} */

  int32_t sat_gate_cfg_count_thresh;
  /**< @h2xmle_description {Threshold count to determine if saturation occurs.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x5} */

  int32_t sat_gate_cfg_gain_thresh;
  /**< @h2xmle_description {Threshold on gain to determine if saturation
                            occurs.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x500} */

  int32_t sat_gate_cfg_hangover_len;
  /**< @h2xmle_description {Length of hangover frames when saturation is
                            detected.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x64} */

  int32_t sat_gate_cfg_trans_step;
  /**< @h2xmle_description {Gain reduction delta when saturation is detected.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x400} */

  int32_t sat_gate_cfg_min_gain;
  /**< @h2xmle_description {Lower limit of the gain transition target when
                            saturation is detected.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0} */

  int32_t prox_gate_enable;
  /**< @h2xmle_description {Enables the proximity detector.}
       @h2xmle_rangeList   {Off=0;On=1}
       @h2xmle_default     {0} */

  int32_t prox_gate_trans_step;
  /**< @h2xmle_description {Gain reduction delta when proximity is not
                            detected.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {0x64} */

  int32_t prox_gate_min_gain;
  /**< @h2xmle_description {Lower limit of gain reduction.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {0} */

  int32_t prox_gate_trigger_hangover;
  /**< @h2xmle_description {Hangover before the proximity detector flag is
                            accepted.}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {1} */

  int32_t fb_gate_dynamic_enable;
  /**< @h2xmle_description {Dynamically enables the feedback gate
                            while AANC is running.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  int32_t fb_gate_criteria_type;
  /**< @h2xmle_description {Type of normalization criteria for the feedback gate
                            correlation method.}
       @h2xmle_rangeList   {"Fully normalized with error and noise"=0;
                            "Half normalized with error"=1;
                            "Half normalized with noise"=2;
                            "Correlation"=3}
       @h2xmle_default     {0} */

  int32_t fb_gate_threshold;
  /**< @h2xmle_description {Threshold to compare against the feedback criteria.}
       @h2xmle_dataformat  {Q24.8}
       @h2xmle_range       {0..2147483647}
       @h2xmle_default     {128} */

  int32_t fb_gate_sm_beta1;
  /**< @h2xmle_description {Smoothing beta for the XX, XE, and EE energies.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {26214} */

  int32_t fb_gate_sm_beta2;
  /**< @h2xmle_description {Smoothing beta for XE/sqrt(XX*EE).}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {3276} */

  int32_t fb_gate_rx_discount_factor;
  /**< @h2xmle_description {Factor multiplied with Rx power for subtraction
                            from correlation.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767}
       @h2xmle_default     {10} */

  int32_t fb_gate_hangover;
  /**< @h2xmle_description {Hangover frames when feedback is detected.}
       @h2xmle_dataformat  {Q16}
       @h2xmle_range       {0..32768}
       @h2xmle_default     {100} */

  int32_t fb_gate_trans_step;
  /**< @h2xmle_description {Gain reduction delta when feedback is detected.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..32768}
       @h2xmle_default     {128} */

  int32_t fb_gate_min_gain;
  /**< @h2xmle_description {Lower limit of the gain transition target when
                            feedback is detected.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0} */

  int32_t static_gate_enable;
  /**< @h2xmle_description {Specifies whether the output gain is incrementally
                            increased each frame to the default gain.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

  int32_t static_gate_trans_step;
  /**< @h2xmle_description {Transition-up step in static ANC mode.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..32768}
       @h2xmle_default     {64} */

  int32_t noise_gate_cfg_min_gain;
  /**< @h2xmle_description {Noise gate minimum gain.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_CFG2_V3 0x3

/*==============================================================================
   Type definitions
==============================================================================*/


/* Deprecated. Structure definition for Parameter */
typedef struct afe_aanc_cfg2_param_v3_t afe_aanc_cfg2_param_v3_t;

/** Deprecated. Use #afe_aanc_cfg2_param_v4_t and #afe_aanc_cfg2_param_v5_t.

    @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_CFG2_V3", AFE_PARAM_ID_AANC_CFG2}
    @h2xmlp_description {Dynamic configuration for the AANC algorithm.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_CFG2_V3}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_cfg2_param_v3_t
{
   uint32_t aanc_cfg2_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_CFG2_V3}
       @h2xmle_default     {AFE_API_VERSION_AANC_CFG2_V3}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint16_t adapt_anc_enable;
  /**< @h2xmle_description {Enables adaptive AANC.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t alc_enable;
  /**< @h2xmle_description {Enables the adaptive level.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t pre_filt_enable;
  /**< @h2xmle_description {Enables the prefilter.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t rx_factor_enable;
  /**< @h2xmle_description {Enables the Rx energy factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t vad_factor_enable;
  /**< @h2xmle_description {Enables the VAD factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t noise_gate_enable;
  /**< @h2xmle_description {Enables the noise gate.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  int32_t ad_alpha;
  /**< @h2xmle_description {Learning rate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {-32768..32767} */

  int32_t ad_delta_g2_bound;
  /**< @h2xmle_description {Absolute maximum of delta G2.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..32767} */

  int32_t ad_gain_bound[2];
  /**< @h2xmle_description {Minimum and maximum values of adaptive gain.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767} */

  int32_t alc_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the adaptive-level controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t alc_cfg_ftarget;
  /**< @h2xmle_description {Gain curve target for the adaptive-level
                            controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t alc_cfg_fthresh;
  /**< @h2xmle_description {Gain curve target threshold the adaptive-level
                            controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t rx_gate_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t rx_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t rx_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t vad_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 for the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767} */

  int32_t vad_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 for the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767} */

  int32_t noise_gate_cfg_thresh;
  /**< @h2xmle_description {Noise gate threshold.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t noise_gate_cfg_trstep;
  /**< @h2xmle_description {Noise gate transition step.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  uint16_t block_gate_enable;
  /**< @h2xmle_description {Enables the block gate.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t block_offset_enable;
  /**< @h2xmle_description {Enables the block offset.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  int32_t block_gate_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t block_gate_cfg_discount_factor;
  /**< @h2xmle_description {Discount factor for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767} */

  int32_t block_gate_cfg_weight_factor;
  /**< @h2xmle_description {Weighting factor for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767} */

  int32_t block_gate_cfg_rx_vad_threshold;
  /**< @h2xmle_description {Rx VAD threshold for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t block_gate_cfg_rx_offset_factor;
  /**< @h2xmle_description {Factor for generating an offset.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767} */

  int32_t noise_gate_cfg_thresh2;
  /**< @h2xmle_description {Noise gate threshold.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767} */

  int32_t noise_gate_cfg_thresh2targetgain;
  /**< @h2xmle_description {Safe target gain.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767} */

  int32_t block_gate_cfg_discount_gain_threshold;
  /**< @h2xmle_description {Threshold on ANC gain that determines the dynamic 
                            discount factor.}
       @h2xmle_dataformat  {Q13}
       @h2xmle_range       {-32768..32767} */

  int32_t block_gate_cfg_err_limit_factor;
  /**< @h2xmle_description {Block gate safe target gain.}
       @h2xmle_dataformat  {Q13}
       @h2xmle_range       {-32768..32767} */

  int32_t wnd_gate_enable;
  /**< @h2xmle_description {Wind noise detection feature.}
       @h2xmle_range       {0..3}

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"ADANC_WND_GATE"}
       @h2xmle_description {Enables the gate adjustment.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"ADANC_WND_DETECT"}
       @h2xmle_description {Enables detection.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd  */

  int32_t wnd_tran_step;
  /**< @h2xmle_description {Transition step for wind noise.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {-32768..32767} */

  int32_t wnd_min_gain;
  /**< @h2xmle_description {Minimum gain when wind is detected.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767} */

  int32_t wnd_hangover;
  /**< @h2xmle_description {After wind is detected, hangover before declaring
                            no wind.}
       @h2xmle_range       {0..2147483647} */

  int32_t wnd_sec_mic;
  /**< @h2xmle_description {Choice of secondary mic for wind noise detection.}
       @h2xmle_range       {0..2} */

  int32_t wnd_cov_eps;
  /**< @h2xmle_description {Small initialization constant for the covariance
                            (per sample).}
       @h2xmle_range       {0..2147483647} */

  int32_t wnd_det_th[2];
  /**< @h2xmle_description {Lower and upper thresholds for wind measurement.}
       @h2xmle_range       {0..2147483647} */

  int32_t wnd_detect_thresh_l32q31;
  /**< @h2xmle_description {Wind detection threshold.}
       @h2xmle_range       {0..2147483647} */

  int32_t wnd_silent_thresh_l32q0;
  /**< @h2xmle_description {Wind silence threshold.}
       @h2xmle_range       {0..2147483647} */

  int16_t wnd_c_alph;
  /**< @h2xmle_description {Smoothing alpha for the covariance statistics.}
       @h2xmle_range       {0..32767} */

  int16_t wnd_cov_bit_shf;
  /**< @h2xmle_description {Right bit shift for correction before converting to
                            int32.}
       @h2xmle_range       {-32..62} */

  int16_t wnd_ws_alph_inc;
  /**< @h2xmle_description {Smoothing alpha for the wind score when wind
                            increases.}
       @h2xmle_range       {0..32767} */

  int16_t wnd_ws_alph_dec;
  /**< @h2xmle_description {Smoothing alpha for the wind score when wind
                            decreases.}
       @h2xmle_range       {0..32767} */

  int16_t wnd_boost_s16q10;
  /**< @h2xmle_description {Boosting self-standard deviation for wind
                            determination.}
       @h2xmle_range       {0..32767} */

  int16_t wnd_fact_s16q15;
  /**< @h2xmle_description {Percentage of low-frequency standard deviation in
                            wind discrimination.}
       @h2xmle_range       {0..32767} */

  int32_t sat_gate_enable;
  /**< @h2xmle_description {Enables the saturation gate.}
       @h2xmle_rangeList   {Off=0;On=1} */

  int32_t sat_gate_cfg_pcm_thresh;
  /**< @h2xmle_description {Threshold for a PCM sample before saturation
                            (typically 2^15-1).}
       @h2xmle_range       {0..65535} */

  int32_t sat_gate_cfg_count_thresh;
  /**< @h2xmle_description {Threshold count to determine if saturation occurs.}
       @h2xmle_range       {0..2147483647} */

  int32_t sat_gate_cfg_gain_thresh;
  /**< @h2xmle_description {Threshold on gain to determine if saturation
                            occurs.}
       @h2xmle_range       {0..32767} */

  int32_t sat_gate_cfg_hangover_len;
  /**< @h2xmle_description {Length of hangover frames when saturation is
                            detected.}
       @h2xmle_range       {0..2147483647} */

  int32_t sat_gate_cfg_trans_step;
  /**< @h2xmle_description {Gain reduction delta when saturation is detected.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..2147483647} */

  int32_t sat_gate_cfg_min_gain;
  /**< @h2xmle_description {Lower limit of the gain transition target when
                            saturation is detected.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767} */

  int32_t prox_gate_enable;
  /**< @h2xmle_description {Enables the proximity detector feature.}
       @h2xmle_rangeList   {Off=0;On=1} */

  int32_t prox_gate_trans_step;
  /**< @h2xmle_description {Gain reduction delta when proximity is not
                            detected.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..2147483647} */

  int32_t prox_gate_min_gain;
  /**< @h2xmle_description {Lower limit of gain reduction.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767} */

  int32_t prox_gate_trigger_hangover;
  /**< @h2xmle_description {Hangover before the proximity detector flag is
                            accepted.}
       @h2xmle_range       {0..2147483647} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_CFG2_V2 0x2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Deprecated. Structure definition for Parameter */
typedef struct afe_aanc_cfg2_param_v2_t afe_aanc_cfg2_param_v2_t;

/** Deprecated. Use #afe_aanc_cfg2_param_v4_t and #afe_aanc_cfg2_param_v5_t.

    @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_CFG2_V2", AFE_PARAM_ID_AANC_CFG2}
    @h2xmlp_description {Dynamic configuration for the AANC algorithm.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_CFG2_V2}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_cfg2_param_v2_t
{
  uint32_t aanc_cfg2_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_CFG2_V2}
       @h2xmle_default     {AFE_API_VERSION_AANC_CFG2_V2}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint16_t adapt_anc_enable;
  /**< @h2xmle_description {Enables adaptive AANC.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t alc_enable;
  /**< @h2xmle_description {Enables the adaptive-level controller.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t pre_filt_enable;
  /**< @h2xmle_description {Enables the prefilter.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t rx_factor_enable;
  /**< @h2xmle_description {Enables the Rx energy factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t vad_factor_enable;
  /**< @h2xmle_description {Enables the VAD factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t noise_gate_enable;
  /**< @h2xmle_description {Enables the noise gate.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  int32_t ad_alpha;
  /**< @h2xmle_description {Learning rate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {-32768..32767} */

  int32_t ad_delta_g2_bound;
  /**< @h2xmle_description {Absolute maximum of delta G2.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..32767} */

  int32_t ad_gain_bound[2];
  /**< @h2xmle_description {Minimum and maximum values of adaptive gain.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767} */

  int32_t alc_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the adaptive-level controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t alc_cfg_ftarget;
  /**< @h2xmle_description {Gain curve target for the adaptive-level
                            controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t alc_cfg_fthresh;
  /**< @h2xmle_description {Gain curve target threshold the adaptive-level
                            controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t rx_gate_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t rx_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t rx_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t vad_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 for the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767} */

  int32_t vad_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 for the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767} */

  int32_t noise_gate_cfg_thresh;
  /**< @h2xmle_description {Noise gate threshold.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t noise_gate_cfg_trstep;
  /**< @h2xmle_description {Noise gate transition step.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  uint16_t block_gate_enable;
  /**< @h2xmle_description {Enables the block gate.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t block_offset_enable;
  /**< @h2xmle_description {Enables the block offset.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  int32_t block_gate_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t block_gate_cfg_discount_factor;
  /**< @h2xmle_description {Discount factor for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767} */

  int32_t block_gate_cfg_weight_factor;
  /**< @h2xmle_description {Weighting factor for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767} */

  int32_t block_gate_cfg_rx_vad_threshold;
  /**< @h2xmle_description {Rx VAD threshold for the block gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t block_gate_cfg_rx_offset_factor;
  /**< @h2xmle_description {Factor for generating an offset.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {-32768..32767} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_CFG2_V1 0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Deprecated. Structure definition for Parameter */
typedef struct afe_aanc_cfg2_param_v1_t afe_aanc_cfg2_param_v1_t;

/** Deprecated. Use #afe_aanc_cfg2_param_v4_t and #afe_aanc_cfg2_param_v5_t.

    @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_CFG2_V1", AFE_PARAM_ID_AANC_CFG2}
    @h2xmlp_description {Dynamic configuration for the AANC algorithm.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_CFG2_V1}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_cfg2_param_v1_t
{
   uint32_t aanc_cfg2_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_CFG2_V1}
       @h2xmle_default     {AFE_API_VERSION_AANC_CFG2_V1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */
  
uint16_t adapt_anc_enable;
  /**< @h2xmle_description {Enables adaptive AANC.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t alc_enable;
  /**< @h2xmle_description {Enables the adaptive-level controller.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t pre_filt_enable;
  /**< @h2xmle_description {Enables the prefilter.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t rx_factor_enable;
  /**< @h2xmle_description {Enables the Rx energy factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t vad_factor_enable;
  /**< @h2xmle_description {Enables the VAD factor.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  uint16_t noise_gate_enable;
  /**< @h2xmle_description {Enables the noise gate.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */

  int32_t ad_alpha;
  /**< @h2xmle_description {Learning rate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {-32768..32767} */

  int32_t ad_delta_g2_bound;
  /**< @h2xmle_description {Absolute maximum of delta G2.}
       @h2xmle_dataformat  {Q2.14}
       @h2xmle_range       {0..32767} */

  int32_t ad_gain_bound[2];
  /**< @h2xmle_description {Minimum and maximum values of adaptive gain.}
       @h2xmle_dataformat  {Q3.13}
       @h2xmle_range       {-32768..32767} */

  int32_t alc_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the adaptive-level
                            controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t alc_cfg_ftarget;
  /**< @h2xmle_description {Gain curve target for the adaptive-level
                            controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t alc_cfg_fthresh;
  /**< @h2xmle_description {Gain curve target threshold the adaptive-level
                            controller.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t rx_gate_cfg_sm_beta;
  /**< @h2xmle_description {Smoothing beta for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t rx_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t rx_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 for the Rx gate.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t vad_gate_cfg_fthresh1;
  /**< @h2xmle_description {Curve threshold 1 for the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767} */

  int32_t vad_gate_cfg_fthresh2;
  /**< @h2xmle_description {Curve threshold 2 for the VAD gate.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..32767} */

  int32_t noise_gate_cfg_thresh;
  /**< @h2xmle_description {Noise gate threshold.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */

  int32_t noise_gate_cfg_trstep;
  /**< @h2xmle_description {Noise gate transition step.}
       @h2xmle_dataformat  {Q1.15}
       @h2xmle_range       {0..32767} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_AANC_NOISE_REDUCTION   0x000102AB

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_NOISE_REDUCTION   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_aanc_noise_reduce_cfg_param_v1_t 
               afe_aanc_noise_reduce_cfg_param_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_NOISE_REDUCTION",
                         AFE_PARAM_ID_AANC_NOISE_REDUCTION}
    @h2xmlp_description {Controls noise reduction strength.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_NOISE_REDUCTION}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"

struct afe_aanc_noise_reduce_cfg_param_v1_t
{
  uint32_t aanc_noise_reduce_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_NOISE_REDUCTION}
       @h2xmle_default     {AFE_API_VERSION_AANC_NOISE_REDUCTION}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t ad_beta;
  /**< @h2xmle_description {Target noise reduction level.}
       @h2xmle_dataformat  {Q8.8}
       @h2xmle_range       {0..255} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_AANC_PROXIMITY_DETECTION   0x00010247

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_PROXIMITY_DETECTION   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_aanc_proxy_det_cfg_param_v1_t 
               afe_aanc_proxy_det_cfg_param_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_PROXIMITY_DETECTION",
                         AFE_PARAM_ID_AANC_PROXIMITY_DETECTION}
    @h2xmlp_description {Configures proximity sensor information at runtime.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_PROXIMITY_DETECTION}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_proxy_det_cfg_param_v1_t
{
  uint32_t    aanc_prox_det_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_PROXIMITY_DETECTION}
       @h2xmle_default     {AFE_API_VERSION_AANC_PROXIMITY_DETECTION}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t     prox_detect_flag;
  /**< @h2xmle_description {Enables the proximity detector flag.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_AANC_STATS           0x0001024A

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_STATS        0x3

/* Maximum number of wind channels allowed for wind detection calculation */
#define AFE_AANC_WND_MAX_CHANNELS         2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_aanc_stats_v3_t afe_aanc_stats_v3_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_STATS_V3", AFE_PARAM_ID_AANC_STATS}
    @h2xmlp_description {Logs AANC algorithm tuning variables. Currently, this
                         parameter is used for logging the internal variables.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_STATS}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_stats_v3_t
{
  uint32_t aanc_stats_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_STATS}
       @h2xmle_default     {AFE_API_VERSION_AANC_STATS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration for AANC variables (in milliseconds).} */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the AANC variables.} */

  int32_t ad_gain;
  /**< @h2xmle_description {Adaptive gain value.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t delta_g;
  /**< @h2xmle_description {Delta G from the adaptive algorithm.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t out_gain;
  /**< @h2xmle_description {Output gain, which is computed as
                            adaptive gain * noise gate factor.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t alc_data_sm_peak_x;
  /**< @h2xmle_description {ALC smoothed signal peak.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t alc_data_gain_x;
  /**< @h2xmle_description {ALC gain.} */

  int32_t rx_gate_data_sm_peak_x;
  /**< @h2xmle_description {Rx gate smoothed signal peak.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t rx_gate_data_factor;
  /**< @h2xmle_description {Gate factor for the Rx power estimate learning
                            rate.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t vad_gate_data_detect_ratio;
  /**< @h2xmle_description {Voice detection ratio.}
       @h2xmle_dataformat  {Q8.8} */

  int32_t vad_gate_data_factor;
  /**< @h2xmle_description {Learning rate gate factor.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t vad_gate_data_min_eng;
  /**< @h2xmle_description {Minimum energy for noise estimation.} */

  int32_t vad_gate_data_cur_eng;
  /**< @h2xmle_description {Current energy for noise estimation.} */

  int32_t noise_gate_data_nng_state;
  /**< @h2xmle_description {State of the noise gate.} */

  int32_t noise_gate_data_nng_factor;
  /**< @h2xmle_description {Factor multiplied with ANC gain to generate the
                            final gain}
       @h2xmle_dataformat  {Q3.13} */

  int32_t block_gate_data_est_x;
  /**< @h2xmle_description {Peak estimate on X.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t block_gate_data_est_e;
  /**< @h2xmle_description {Peak estimate on E.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t block_gate_data_sm_est_x;
  /**< @h2xmle_description {Smoothed peak estimate on X.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t block_gate_data_sm_est_e;
  /**< @h2xmle_description {Smoothed peak estimate on E.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t block_gate_data_gate_delta;
  /**< @h2xmle_description {Delta factor to be added to the filtered LMS.}
       @h2xmle_dataformat  {Q14} */

  int32_t block_gate_data_gate_offset;
  /**< @h2xmle_description {Offset that is proportional to the estimate X and
                            ratio.}
       @h2xmle_dataformat  {Q14} */

  int32_t sat_gate_data_nsg_sat_count;
  /**< @h2xmle_description {Number of saturations in the current frame.} */

  int32_t sat_gate_data_nsg_sat_hangover;
  /**< @h2xmle_description {Hangover counter, which is positive if the
                            saturation gate is active.} */

  int32_t wnd_gate_data_wnd_state;
  /**< @h2xmle_description {State of the wind gate.} */

  int32_t prox_gate_cfg_prox_detect_flag;
  /**< @h2xmle_description {Proximity detector flag.} */

  int32_t prox_gate_data_prox_state;
  /**< @h2xmle_description {Proximity state.} */

  int32_t prox_gate_data_prox_hangover_count;
  /**< @h2xmle_description {Proximity gate hangover count.} */

  int32_t std[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Standard deviation of the low-pass data.}
       @h2xmle_range       {-2147483648..2147483647} */

  int32_t var_f[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Non-low-pass variance.} */

  int32_t std_f[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Non-low-pass standard deviation.} */

  int32_t std_o[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Lowest standard deviation of the other channel
                            (low pass).} */

  int32_t wind_meas[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Measure of the raw wind strength.} */

  int32_t wind_sc_q30[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Wind score (probability of wind detection). A high
                            number means there is more probability.}
       @h2xmle_dataformat  {Q30} */

  int16_t pri_wind_1q14[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Probability that wind is present in the primary
                            channel (criteria 1).}
       @h2xmle_dataformat  {Q14} */

  int16_t pri_wind_2q14[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Probability that wind is present in the primary
                            channel (criteria 2).}
       @h2xmle_dataformat  {Q14} */

  int16_t pri_wind_q15[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Probability that wind is present in the channel.}
       @h2xmle_dataformat  {Q15} */

  int32_t wind_sc_adj_q30[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Wind score, adjusted for all bad correlations.}
       @h2xmle_dataformat  {Q30} */

  int32_t wind_sc_adj_sm_q30[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Wind score, adjusted for all bad correlations and
                            smoothed.}
       @h2xmle_dataformat  {Q30} */

  int16_t wnd_gate_data_wind_detect_flag;
  /**< @h2xmle_description {Wind detection flag.} */

  int16_t wnd_channels;
  /**< @h2xmle_description {Number of wind channels.} */

  int32_t fb_gate_corr_xe;
  /**< @h2xmle_description {Correlation score for XE.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_var_xx;
  /**< @h2xmle_description {Variance of XX to normalize XY.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_var_ee;
  /**< @h2xmle_description {Variance of EE to normalize XY.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_corr_sm_xe;
  /**< @h2xmle_description {Smooth correlation score for XE.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_var_sm_xx;
  /**< @h2xmle_description {Smooth variance for XX.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_var_sm_ee;
  /**< @h2xmle_description {Smooth variance for EE.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_criteria;
  /**< @h2xmle_description {Final criteria computed from the smooth score and
                            variance.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_hangover_counts;
  /**< @h2xmle_description {Frame counts to turn off ANC when feedback is
                            detected.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_rx_eng;
  /**< @h2xmle_description {Rx mean square energy.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t vad_gate_data_flag;
  /**< @h2xmle_description {Voice detection flag.} */

  int32_t vad_gate_data_prob_est;
  /**< @h2xmle_description {Voice estimate.}
       @h2xmle_dataformat  {Q12} */

  int32_t vad_gate_data_nv_eng;
  /**< @h2xmle_description {Non-voice frame energy estimates with smoothing.}
       @h2xmle_dataformat  {Q15} */

  int32_t vad_gate_data_noise_eng;
  /**< @h2xmle_description {Noise energy estimates based on a
                            multichannel noise reference.}
       @h2xmle_dataformat  {Q15} */

  int32_t vad_gate_data_voice_eng;
  /**< @h2xmle_description {Voice energy estimates based on a
                            multichannel voice reference.}
       @h2xmle_dataformat  {Q15} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_STATS_V2        0x2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */

typedef struct afe_aanc_stats_v2_t afe_aanc_stats_v2_t;

/** Deprecated. Use #afe_aanc_stats_v3_t.

    @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_STATS_V2", AFE_PARAM_ID_AANC_STATS}
    @h2xmlp_description {Logs AANC algorithm tuning variables. Currently, this
                         parameter is used for logging the internal variables.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_STATS_V2}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_stats_v2_t
{
  uint32_t aanc_stats_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_STATS}
       @h2xmle_default     {AFE_API_VERSION_AANC_STATS}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration for AANC variables (in milliseconds).} */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the AANC variables.} */

  int32_t ad_gain;
  /**< @h2xmle_description {Adaptive gain value.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t delta_g;
  /**< @h2xmle_description {Delta G from the adaptive algorithm.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t out_gain;
  /**< @h2xmle_description {Output gain, which is computed as
                            adaptive gain * noise gate factor.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t alc_data_sm_peak_x;
  /**< @h2xmle_description {ALC smoothed signal peak.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t alc_data_gain_x;
  /**< @h2xmle_description {ALC gain.} */

  int32_t rx_gate_data_sm_peak_x;
  /**< @h2xmle_description {Rx gate smoothed signal peak.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t rx_gate_data_factor;
  /**< @h2xmle_description {Gate factor for the Rx power estimate learning
                            rate.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t vad_gate_data_detect_ratio;
  /**< @h2xmle_description {Voice detection ratio.}
       @h2xmle_dataformat  {Q8.8} */

  int32_t vad_gate_data_factor;
  /**< @h2xmle_description {Learning rate gate factor.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t vad_gate_data_min_eng;
  /**< @h2xmle_description {Minimum energy for noise estimation.} */

  int32_t vad_gate_data_cur_eng;
  /**< @h2xmle_description {Current energy for noise estimation.} */

  int32_t noise_gate_data_nng_state;
  /**< @h2xmle_description {State of the noise gate.} */

  int32_t noise_gate_data_nng_factor;
  /**< @h2xmle_description {Factor multiplied with ANC gain to generate the 
                            final gain.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t block_gate_data_est_x;
  /**< @h2xmle_description {Peak estimate on X.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t block_gate_data_est_e;
  /**< @h2xmle_description {Peak estimate on E.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t block_gate_data_sm_est_x;
  /**< @h2xmle_description {Smoothed peak estimate on X.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t block_gate_data_sm_est_e;
  /**< @h2xmle_description {Smoothed peak estimate on E.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t block_gate_data_gate_delta;
  /**< @h2xmle_description {Delta factor to be added to the filtered LMS.}
       @h2xmle_dataformat  {Q14} */

  int32_t block_gate_data_gate_offset;
  /**< @h2xmle_description {Offset that is proportional to the estimate X and
                            ratio.}
       @h2xmle_dataformat  {Q14} */

  int32_t sat_gate_data_nsg_sat_count;
  /**< @h2xmle_description {Number of saturations in the current frame.} */

  int32_t sat_gate_data_nsg_sat_hangover;
  /**< @h2xmle_description {Hangover counter, which is positive if the
                            saturation gate is active.} */

  int32_t wnd_gate_data_wnd_state;
  /**< @h2xmle_description {State of the wind gate.} */

  int32_t prox_gate_cfg_prox_detect_flag;
  /**< @h2xmle_description {Proximity detector flag.} */

  int32_t prox_gate_data_prox_state;
  /**< @h2xmle_description {Proximity state.} */

  int32_t prox_gate_data_prox_hangover_count;
  /**< @h2xmle_description {Proximity gate hangover count.} */

  int32_t std[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Standard deviation of the low-pass data.}
       @h2xmle_range       {-2147483648..2147483647} */

  int32_t var_f[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Non-low-pass variance.} */

  int32_t std_f[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Non-low-pass standard deviation.} */

  int32_t std_o[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Lowest standard deviation of the other channel
                            (low pass).} */

  int32_t wind_meas[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Measure of the raw wind strength.} */

  int32_t wind_sc_q30[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Wind score (probability of wind detection). A high
                            number means there is more probability.}
       @h2xmle_dataformat  {Q30} */

  int16_t pri_wind_1q14[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Probability that wind is present in the primary
                            channel (criteria 1).}
       @h2xmle_dataformat  {Q14} */

  int16_t pri_wind_2q14[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Probability that wind is present in the primary
                            channel (criteria 2).}
       @h2xmle_dataformat  {Q14} */

  int16_t pri_wind_q15[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Probability that wind is present in the channel.}
       @h2xmle_dataformat  {Q15} */

  int32_t wind_sc_adj_q30[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Wind score, adjusted for all bad correlations.}
       @h2xmle_dataformat  {Q30} */

  int32_t wind_sc_adj_sm_q30[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Wind score, adjusted for all bad correlations and
                            smoothed.}
       @h2xmle_dataformat  {Q30} */

  int16_t wnd_gate_data_wind_detect_flag;
  /**< @h2xmle_description {Wind detection flag.} */

  int16_t wnd_channels;
  /**< @h2xmle_description {Number of wind channels.} */

  int32_t fb_gate_corr_xe;
  /**< @h2xmle_description {Correlation score for XE.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_var_xx;
  /**< @h2xmle_description {Variance of XX to normalize XY.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_var_ee;
  /**< @h2xmle_description {Variance of EE to normalize XY.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_corr_sm_xe;
  /**< @h2xmle_description {Smooth correlation score for XE.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_var_sm_xx;
  /**< @h2xmle_description {Smooth variance for XX.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_var_sm_ee;
  /**< @h2xmle_description {Smooth variance for EE.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_criteria;
  /**< @h2xmle_description {Final criteria computed from the smooth score and
                            variance.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_hangover_counts;
  /**< @h2xmle_description {Frame counts to turn off ANC when feedback is
                            detected.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t fb_gate_rx_eng;
  /**< @h2xmle_description {Rx mean square energy.}
       @h2xmle_dataformat  {Q1.30} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Minor Version of this parameter */
#define AFE_API_VERSION_AANC_STATS_V1        0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Deprecated. Structure definition for Parameter */
typedef struct afe_aanc_stats_v1_t afe_aanc_stats_v1_t;

/** Deprecated. Use #afe_aanc_stats_v3_t.

    @h2xmlp_parameter   {"AFE_PARAM_ID_AANC_STATS_V1", AFE_PARAM_ID_AANC_STATS}
    @h2xmlp_description {Logs AANC algorithm tuning variables. Currently, this
                         parameter is used for logging the internal variables.}
    @h2xmlp_version     {AFE_API_VERSION_AANC_STATS_V1}
    @h2xmlx_expandArray {true}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_aanc_stats_v1_t
{
  uint32_t aanc_stats_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_AANC_STATS_V1}
       @h2xmle_default     {AFE_API_VERSION_AANC_STATS_V1}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t frame_duration_in_ms;
  /**< @h2xmle_description {Frame duration for AANC variables (in milliseconds).} */

  uint32_t frame_num;
  /**< @h2xmle_description {Frame number of the AANC variables.} */

  int32_t ad_gain;
  /**< @h2xmle_description {Adaptive gain value.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t delta_g;
  /**< @h2xmle_description {Delta G from the adaptive algorithm.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t out_gain;
  /**< @h2xmle_description {Output gain, which is computed as
                            adaptive gain * noise gate factor.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t alc_data_sm_peak_x;
  /**< @h2xmle_description {ALC smoothed signal peak.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t alc_data_gain_x;
  /**< @h2xmle_description {ALC gain.} */

  int32_t rx_gate_data_sm_peak_x;
  /**< @h2xmle_description {Rx gate smoothed signal peak.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t rx_gate_data_factor;
  /**< @h2xmle_description {Gate factor for the Rx power estimate learning
                            rate.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t vad_gate_data_detect_ratio;
  /**< @h2xmle_description {Voice detection ratio.}
       @h2xmle_dataformat  {Q8.8} */

  int32_t vad_gate_data_factor;
  /**< @h2xmle_description {Learning rate gate factor.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t vad_gate_data_min_eng;
  /**< @h2xmle_description {Minimum energy for noise estimation.} */

  int32_t vad_gate_data_cur_eng;
  /**< @h2xmle_description {Current energy for noise estimation.} */

  int32_t noise_gate_data_nng_state;
  /**< @h2xmle_description {State of the noise gate.} */

  int32_t noise_gate_data_nng_factor;
  /**< @h2xmle_description {Factor multiplied with ANC gain to generate the
                            final gain.}
       @h2xmle_dataformat  {Q3.13} */

  int32_t block_gate_data_est_x;
  /**< @h2xmle_description {Peak estimate on X.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t block_gate_data_est_e;
  /**< @h2xmle_description {Peak estimate on E.}
       @h2xmle_dataformat  {Q1.15} */

  int32_t block_gate_data_sm_est_x;
  /**< @h2xmle_description {Smoothed peak estimate on X.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t block_gate_data_sm_est_e;
  /**< @h2xmle_description {Smoothed peak estimate on E.}
       @h2xmle_dataformat  {Q1.30} */

  int32_t block_gate_data_gate_delta;
  /**< @h2xmle_description {Delta factor to be added to the filtered LMS.}
       @h2xmle_dataformat  {Q14} */

  int32_t block_gate_data_gate_offset;
  /**< @h2xmle_description {Offset that is proportional to the estimate X and
                            ratio.}
       @h2xmle_dataformat  {Q14} */

  int32_t sat_gate_data_nsg_sat_count;
  /**< @h2xmle_description {Number of saturations in the current frame.} */

  int32_t sat_gate_data_nsg_sat_hangover;
  /**< @h2xmle_description {Hangover counter, which is positive if the
                            saturation gate is active.} */

  int32_t wnd_gate_data_wnd_state;
  /**< @h2xmle_description {State of the wind gate.} */

  int32_t prox_gate_cfg_prox_detect_flag;
  /**< @h2xmle_description {Proximity detector flag.} */

  int32_t prox_gate_data_prox_state;
  /**< @h2xmle_description {Proximity state.} */

  int32_t prox_gate_data_prox_hangover_count;
  /**< @h2xmle_description {Proximity gate hangover count.} */

  int32_t std[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Standard deviation of the low-pass data.} */

  int32_t var_f[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Non-low-pass variance.} */

  int32_t std_f[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Non-low-pass standard deviation.} */

  int32_t std_o[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Lowest standard deviation of the other channel
                            (low pass).} */

  int32_t wind_meas[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Measure of the raw wind strength.} */

  int32_t wind_sc_q30[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Wind score (probability of wind detection). A high
                            number means there is more probability.}
       @h2xmle_dataformat  {Q30} */

  int16_t pri_wind_1q14[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Probability that wind is present in the primary
                            channel (criteria 1).}
       @h2xmle_dataformat  {Q14} */

  int16_t pri_wind_2q14[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Probability that wind is present in the primary
                            channel (criteria 2).}
       @h2xmle_dataformat  {Q14} */

  int16_t pri_wind_q15[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Probability that wind is present in the channel.}
       @h2xmle_dataformat  {Q15} */

  int32_t wind_sc_adj_q30[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Wind score, adjusted for all bad correlations.}
       @h2xmle_dataformat  {Q30} */

  int32_t wind_sc_adj_sm_q30[AFE_AANC_WND_MAX_CHANNELS];
  /**< @h2xmle_description {Wind score, adjusted for all bad correlations and
                            smoothed.}
       @h2xmle_dataformat  {Q30} */

  int16_t wnd_gate_data_wind_detect_flag;
  /**< @h2xmle_description {Wind detection flag.} */

  int16_t wnd_channels;
  /**< @h2xmle_description {Number of wind channels.} */
}
#include "adsp_end_pack.h"
;

/** @h2xml_Select          {afe_mod_enable_param_t}
    @h2xmlm_InsertParameter */

/** @} <-- End of the Module --> */

#endif /* __AANC_H__ */
