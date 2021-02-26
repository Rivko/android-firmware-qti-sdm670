/*========================================================================*/
/**
 @file adsp_audproc_api_prop.h

 This file contains the structure definitions for Audio Post Processing
 Topology IDs, Module IDs, Parameter IDs.
 */
/*===========================================================================
 NOTE: The @brief description above does not appear in the PDF.
 The description that displays in the PDF is maintained in the
 audio_mainpage.dox file. Contact Tech Pubs for assistance.
 ===========================================================================*/
/*===========================================================================
  Copyright (c) 2011-2018 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
============================================================================ */
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 08/20/17   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8 (SDM845).
 03/21/17   pag     Removed Doxygen comments and added h2xml comments for modules.
                    Kept those modules in separate file and corresponding header
                    files are included here.
 08/23/16   sw      (Tech Pubs) Merged Doxygen comments from 8953.2.7;
                    edited Doxygen comments for AVS 2.8 (MSM8998).
 08\17\16   HB      Create File for properietary information.
 ========================================================================== */

#ifndef _ADSP_AUDPROC_API_PROP_H_
#define _ADSP_AUDPROC_API_PROP_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "audiosphere_calib.h"
#include "bass_boost_calib.h"
#include "cfcm_calib.h"
#include "compressed_latency_calib.h"
#include "delay_calib.h"
#include "eans_calib.h"
#include "eans_v2_calib.h"
#include "iir_tuning_filter_calib.h"
#include "lec_calib.h"
#include "mec_calib.h"
#include "mchan_iir_tuning_filter_calib.h"
#include "peak_meter_calib.h"
#include "reverb_calib.h"
#include "left_iir_tuning_filter_calib.h"
#include "right_iir_tuning_filter_calib.h"
#include "shoebox_calib.h"
#include "srs_trumedia_calib.h"
#include "hpf_iir_tx_filter_calib.h"
#include "tx_iir_filter_calib.h"
#include "virtualizer_calib.h"
#include "visualizer_calib.h"
#include "multichan_fir_tuning_filter_calib.h"
#include "aig_calib.h"
#include "mbdrc_calib.h"
#include "iir_mbdrc_calib.h"
#include "pbe_calib.h"
#include "compressed_mute_calib.h"
#include "popless_equalizer_calib.h"
#include "dts_drc_calib.h"
#include "dts_dialnorm_calib.h"
#include "mic2amb_calib.h"

/** @addtogroup audiopp_topos_asm
@{ */

/** ID of the default audio postprocessing stream topology.

    @inputfig{1,topo_id_asm_0x00010BE4_postproc.png}

    - Resampler                 (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control       (AUDPROC_MODULE_ID_VOL_CTRL) @newpage
 */
#define ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT                         0x00010BE4

/** ID of the default audio preprocessing stream topology.

    @inputfig{1,topo_id_asm_0x00010BE4_preproc.png}

    - Resampler                 (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control       (AUDPROC_MODULE_ID_VOL_CTRL)
 */
#define ASM_STREAM_PREPROC_TOPO_ID_DEFAULT   ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT

/** ID of the default postprocessing topology with peak meter.

    @inputfig{1,topo_id_asm_0x00010D83.png}

    - Resampler                 (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control       (AUDPROC_MODULE_ID_VOL_CTRL)
    - Peak Meter                (AUDPROC_MODULE_ID_PEAK_METER)
 */
#define ASM_STREAM_POSTPROC_TOPO_ID_PEAKMETER                       0x00010D83

/** ID of the audo postprocessing topology with multichannel resampler, peak
    meter, and volume control.

    @inputfig{1,topo_id_asm_0x00010D8B.png}

    - Resampler                 (AUDPROC_MODULE_ID_RESAMPLER)
    - Peak Meter                (AUDPROC_MODULE_ID_PEAK_METER)
    - Soft Volume Control       (AUDPROC_MODULE_ID_VOL_CTRL) @newpage
 */
#define ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL                     0x00010D8B

/** ID of the DTS audio postprocessing topology.

    @inputfig{1,topo_id_audproc_0x0001071F.png}

    - DTS Dynamic Range Control  (AUDPROC_MODULE_ID_DTS_DRC)
    - DTS DialNorm               (AUDPROC_MODULE_ID_DTS_DIALNORM)
    - Resampler                  (AUDPROC_MODULE_ID_RESAMPLER)
    - Volume Control             (AUDPROC_MODULE_ID_VOL_CTRL)
 */
#define AUDPROC_POPP_TOPOLOGY_ID_DTS_AUDIO   0x0001071F

/** @} *//* end_addtogroup audiopp_topos_asm */

/** @addtogroup audiopp_topos_adm
@{ */

/** ID of the default COPP audio postprocessing device topology.

    @inputfig{1,topo_id_adm_0x00010BE3.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Delay                             (AUDPROC_MODULE_ID_DELAY)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (AUDPROC_MODULE_ID_MBDRC)
    - Rx Codec Gain                     (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    - Sample Slip                       (AUDPROC_MODULE_ID_SAMPLESLIP)
    @newpage
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP           0x00010BE3

/** ID of SPEAKER_MONO in the COPP audio postprocessing device topology.

    @inputfig{1,topo_id_adm_0x00010313.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (AUDPROC_MODULE_ID_MBDRC)
    - Rx Codec Gain                     (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    - Sample Slip                       (AUDPROC_MODULE_ID_SAMPLESLIP)
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP        0x00010313

/** ID of SPEAKER_STEREO in the COPP audio postprocessing device topology.

    @inputfig{1,topo_id_adm_0x00010314.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (AUDPROC_MODULE_ID_MBDRC)
    - Rx Codec Gain                     (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    - Sample Slip                       (AUDPROC_MODULE_ID_SAMPLESLIP) @newpage
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP      0x00010314

/** ID of SPEAKER_STEREO_IIR ID in the COPP audio postprocessing device
    topology.

    @inputfig{1,topo_id_adm_0x00010704.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter Left            (AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER)
    - IIR Tuning Filter Right          (AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (AUDPROC_MODULE_ID_MBDRC)
    - Rx Codec Gain                     (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    - Sample Slip                       (AUDPROC_MODULE_ID_SAMPLESLIP)
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP  0x00010704

/** ID of the COPP topology of SPEAKER_MONO for MBDRC version 2.

    @inputfig{1,topo_id_adm_0x0001070D.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control version 2 (AUDPROC_MODULE_ID_MBDRCV2)
    - Rx Codec Gain                     (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    - Sample Slip                       (AUDPROC_MODULE_ID_SAMPLESLIP) @newpage
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP_MBDRCV2 0x0001070D

/** IID of the COPP topology of SPEAKER_STEREO for MBDRC version 2.

    @inputfig{1,topo_id_adm_0x0001070E.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control version 2 (AUDPROC_MODULE_ID_MBDRCV2)
    - Rx Codec Gain                     (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    - Sample Slip                       (AUDPROC_MODULE_ID_SAMPLESLIP)
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP_MBDRCV2 0x0001070E

/** ID of the COPP topology of STEREO_IIR for MBDRC version 2.
    This topology is only used on the Rx path.

    @inputfig{1,topo_id_adm_0x0001070F.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter Left            (AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER)
    - IIR Tuning Filter Right          (AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control version 2 (AUDPROC_MODULE_ID_MBDRCV2)
    - Rx Codec Gain                     (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    - Sample Slip                       (AUDPROC_MODULE_ID_SAMPLESLIP)

    The SPEAKER_STEREO_IIR topology is almost the same as the SPEAKER_STEREO
    topology. In the SPEAKER_STEREO_IIR, the IIR filter module is replaced by
    two independent per-channel IIR tuning filter modules. @newpage
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP_MBDRCV2 0x0001070F

/** ID of the COPP topology with multichannel peak meter and volume control.

    @inputfig{1,topo_id_adm_0x0001031B.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Peak Meter                        (AUDPROC_MODULE_ID_PEAK_METER)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MCH_PEAK_VOL          0x0001031B

/** ID of Tx MIC_MONO in the COPP audio postprocessing device topology.

    @inputfig{1,topo_id_adm_0x00010315.png}

    - Sample Slip                       (AUDPROC_MODULE_ID_SAMPLESLIP)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Tx Mic Gain                       (AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL)
    - High Pass Filter                  (AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER)
    - Enhanced Audio Noise Suppression  (AUDPROC_MODULE_ID_EANS)
    - Tx IIR Tuning Filter              (AUDPROC_MODULE_ID_TX_IIR_FILTER)
    - Adaptive Input Gain               (AUDPROC_MODULE_ID_AIG)
    - Multiband Dynamic Range Control   (AUDPROC_MODULE_ID_MBDRC)
    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER) @newpage
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_MONO_AUDIO_COPP           0x00010315

/** ID of Tx MIC_STEREO in the COPP audio postprocessing device topology.

    @inputfig{1,topo_id_adm_0x00010316.png}

    - Sample Slip                       (AUDPROC_MODULE_ID_SAMPLESLIP)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Tx Mic Gain                       (AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL)
    - High Pass Filter                  (AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER)
    - Enhanced Audio Noise Suppression  (AUDPROC_MODULE_ID_EANS)
    - Tx IIR Tuning Filter              (AUDPROC_MODULE_ID_TX_IIR_FILTER)
    - Adaptive Input Gain               (AUDPROC_MODULE_ID_AIG)
    - Multiband Dynamic Range Control   (AUDPROC_MODULE_ID_MBDRC)
    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_STEREO_AUDIO_COPP         0x00010316

/** ID of Tx MIC_DEFAULT in the COPP audio postprocessing device topology using
    MBDRC version 3.

    @inputfig{1,topo_id_adm_0x00010355.png}

    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Tx Mic Gain                       (AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL)
    - High Pass Filter                  (AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER)
    - Enhanced Audio Noise Suppression  (AUDPROC_MODULE_ID_EANS)
    - Tx IIR Tuning Filter              (AUDPROC_MODULE_ID_TX_IIR_FILTER)
    - Adaptive Input Gain               (AUDPROC_MODULE_ID_AIG)
    - Multiband Dynamic Range Control version 3 (AUDPROC_MODULE_ID_MBDRCV3)
    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER) @newpage
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_MIC_AUDIO_COPP_MBDRCV3   0x00010355

/** ID of Tx Multichannel IIR in the COPP audio postprocessing device topology.

    @inputfig{1,topo_id_audproc_0x00010715.png}

    - Tx Multichannel IIR Tuning Filter (AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER)
 */
#define AUDPROC_COPP_TOPOLOGY_ID_MCHAN_IIR_AUDIO                    0x00010715

/** ID of the default COPP audio postprocessing device topology using MBDRC
    version 3.

    @inputfig{1,topo_id_adm_0x00010354.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (AUDPROC_MODULE_ID_PBE)
    - IIR Tuning filter                 (AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control version 3 (AUDPROC_MODULE_ID_MBDRCV3)
    - Rx Codec Gain                     (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_MBDRCV3   0x00010354

/** ID of the default COPP audio postprocessing device topology using MBDRC
    version 3.

·    @inputfig{1,topo_id_adm_0x00010317.png}

    - Resampler                         (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control version 3 (AUDPROC_MODULE_ID_MBDRCV3)
    - Rx Codec Gain                     (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    @newpage
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_PEAKMETER_AUDIO_COPP          0x00010317


/** @weakgroup weakf_audproc_custom_topo_recommendations

  @trhead{TR1 -- Placement of volume control in any topology containing
  Snapdragon Audio+ (SA+) modules}

  @trsubhead{Guideline:} Place one volume control (AUDPROC_MODULE_ID_VOL_CTRL)
  before the entire SA+ module chain, and another volume control
  (AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2) after the chain.

  @trsubhead{Reason:} The two volume control instances are required to
  correctly control the headroom and provide soft pause facility. If only one
  volume control is used, there might be a glitch during soft pause.

  @trsubhead{Mandatory or recommended:} Recommended
 *
  @trhead{TR2 -- Any topology using more than two SA+ modules}

  @trsubhead{Guideline:} Place the SA+ modules in the following order, with no
  other modules between them:
   -# AUDPROC_MODULE_ID_RESAMPLER
   -# AUDPROC_MODULE_ID_VOL_CTRL
   -# AUDPROC_MODULE_ID_POPLESS_EQUALIZER
   -# AUDPROC_MODULE_ID_VIRTUALIZER
   -# AUDPROC_MODULE_ID_CHMIXER
   -# AUDPROC_MODULE_ID_REVERB
   -# AUDPROC_MODULE_ID_BASS_BOOST
   -# AUDPROC_MODULE_ID_PBE
   -# AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2

  @trsubhead{Reason:} This ordering is recommended for optimum processing of
  SA+ algorithms. It also avoids known issues observed due to various
  interactions between the modules.

  @trsubhead{Mandatory or recommended:} Recommended
 *
  @trhead{TR3 -- Topologies in Non-Tunneled mode}

  @trsubhead{Guideline:} Topologies in Non-Tunneled mode must have the media
  format converter module (AUDPROC_MODULE_ID_MFC).

  @trsubhead{Reason:} The MFC module is required to convert the incoming stream
  media format to the media format set on the encoder.

  @trsubhead{Mandatory or recommended:} Mandatory @newpage
 *
  @trhead{TR4 -- Modules that convert the media format}

  @trsubhead{Guideline:} If a module is expected to change the media format and
  reduce the number of channels or sampling rate, place it close to the
  beginning of the chain.

  @trsubhead{Reason:} Downstream modules from this module require less
  processing power.

  @trsubhead{Mandatory or recommended:} Recommended
 *
  @trhead{TR5 -- Multi-device use cases}

  @trsubhead{Guideline:} If a single stream is being routed to multiple devices
  that might be running from different clock sources, place the sample slip
  module (AUDPROC_MODULE_ID_SAMPLESLIP) in the device path for each device.

  @trsubhead{Reason:} The sample slip module matches the rate of one device to
  the rate of the other device.

  @trsubhead{Mandatory or recommended:} Recommended
 *
  @trhead{TR6 -- Combination topologies}

  These topologies have two processing chains in a cascade, for example,
  SA+ modules followed by DTS HPX. In this case, enable only one chain at a
  time.

  @trsubhead{Guideline:} Both processing chains must be able to output the same
  media format. Use the media format converter module (AUDPROC_MODULE_ID_MFC)
  if one of the topologies does not have a module that can change the media
  format.

  @trsubhead{Reason:} If effects are enabled or disabled during playback,
  downstream services should not see a change in the media format.

  @trsubhead{Mandatory or recommended:} Recommended
 *
  @trhead{TR7 -- PBE module}

  @trsubhead{Guideline:} Use the PBE module (AUDPROC_MODULE_ID_PBE) only for
  speakers. For headphones, use the Bass Boost module
  (AUDPROC_MODULE_ID_BASS_BOOST).

  @trsubhead{Reason:} The PBE algorithm is designed only for speakers.

  @trsubhead{Mandatory or recommended:} Recommended
 *
  @trhead{TR8 -- Bass Boost module}

  @trsubhead{Guideline:} Use the Bass Boost module
  (AUDPROC_MODULE_ID_BASS_BOOST) only for headphones. For speakers, use the
  PBE module (AUDPROC_MODULE_ID_PBE).

  @trsubhead{Reason:} The BassBoost algorithm is designed only for headphones.

  @trsubhead{Mandatory or recommended:} Recommended @newpage
 *
  @trhead{TR9 -- Equalizer module}

  @trsubhead{Guideline:} Place a volume control module just before
  the equalizer module (AUDPROC_MODULE_ID_POPLESS_EQUALIZER) in the topology.
  There should be no modules between this volume control module and the
  equalizer.

  @trsubhead{Reason:} The volume control module is used to provide headroom that
  is required for the equalizer processing.

  @trsubhead{Mandatory or recommended:} Mandatory
 *
  @trhead{TR10 -- Audiosphere module}

  @trsubhead{Guideline:} Disable the virtualizer module
  (AUDPROC_MODULE_ID_VIRTUALIZER) when the audiosphere module
  (AUDPROC_MODULE_ID_AUDIOSPHERE) is enabled. Other processing should also be
  minimal.

  @trsubhead{Reason:} The virtualizer processing interferes with the processing
  of audiosphere.

  @trsubhead{Mandatory or recommended:} Recommended
 *
  @trhead{TR11 -- Resampler module}

  @trsubhead{Guideline:} Place the resampler module
  (AUDPROC_MODULE_ID_RESAMPLER) at the beginning of the topology.

  @trsubhead{Reason:} The resampler converts the stream sampling rate to the
  device sampling rate. If the resampler is at the beginning of the topology,
  the modules following it can be tuned based on the device.

  @trsubhead{Mandatory or recommended:} Recommended
 *
  @trhead{TR12 -- Modules with significant delay}

  @trsubhead{Guideline:} Place AUDPROC_MODULE_ID_VOL_CTRL before any modules
  that have significant delay. Place AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2
  after any modules that have significant delay. This recommendation is
  applicable only for paths where a pause command is expected.

  @trsubhead{Reason:} The two volume control instances interact during soft
  pause to ensure that ramp down and ramp up during pause and run are smooth.

  @trsubhead{Mandatory or recommended:} Recommended
*/

/** ID of the default COPP open topology for compressed data.

    @inputfig{1,topo_id_audproc_0x0001076B.png}

    - Mute (compressed)         (AUDPROC_MODULE_ID_COMPRESSED_MUTE)
    - Latency (compressed)      (AUDPROC_MODULE_ID_COMPRESSED_LATENCY)
 */
#define AUDPROC_TOPOLOGY_ID_COMPRESSED_DEFAULT                      0x0001076B

/** ID of the topology that contains no modules for compressed data. */
#define AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE                         0x00010774

/** @} *//* end_addtogroup audiopp_topos_adm */


/** @ingroup audiopp_topos_adm
    ID of the COPP topology for the SRS TruMedia module.

    @inputfig{1,topo_id_adm_0x00010D90.png}

    - Resampler                       (AUDPROC_MODULE_ID_RESAMPLER)
    - SRS TruMedia                    (APPI_SRS_TRUMEDIA_MODULE_ID)
    - Peak Meter                      (AUDPROC_MODULE_ID_PEAK_METER)
    - Soft Volume Control             (AUDPROC_MODULE_ID_VOL_CTRL)
    - Rx Codec Gain                   (AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SRS_TRUMEDIA_TOPOLOGY          0x00010D90


/** @addtogroup audiopp_topos_adm
@{ */
/** ID of the COPP audio postprocessing headphone topology that includes
    AudioPlus.

    @inputfig{1,topo_id_audproc_0x0001089E.png}

    - Virtualizer     (AUDPROC_MODULE_ID_VIRTUALIZER)
    - Reverberation   (AUDPROC_MODULE_ID_REVERB)
    - Bass Boost      (AUDPROC_MODULE_ID_BASS_BOOST)
    - Visualizer      (AUDPROC_MODULE_ID_VISUALIZER)
 */
#define AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_HEADPHONE           0x0001089E

/** ID of the COPP audio postprocessing speaker topology that includes
    AudioPlus.

    @inputfig{1,topo_id_audproc_0x0001089F.png}

    - Virtualizer        (AUDPROC_MODULE_ID_VIRTUALIZER)
    - Reverberation      (AUDPROC_MODULE_ID_REVERB)
    - PBE                (AUDPROC_MODULE_ID_PBE)
    - Visualizer         (AUDPROC_MODULE_ID_VISUALIZER)
 */
#define AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_SPEAKER             0x0001089F

/** ID of the POPP audio postprocessing topology that includes AudioPlus.

    @inputfig{1,topo_id_audproc_0x000108A0.png}

    - Resampler                (AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control      (AUDPROC_MODULE_ID_VOL_CTRL)
    - Popless Equalizer        (AUDPROC_MODULE_ID_POPLESS_EQUALIZER)
 */
#define AUDPROC_POPP_TOPOLOGY_ID_AUDIO_PLUS                      0x000108A0

/** @} *//* end_addtogroup audiopp_topos_adm */

/** @ingroup audiopp_module_dtmf
    ID of the DTMF Detection module on the audio path.

    This module supports the following parameter IDs:
    - VOICE_PARAM_MOD_ENABLE
    - VOICE_PARAM_DTMF_DETECTION

    For information on these parameter IDs, refer to @xhyperref{Q7,80-NF774-2}.
 */
#define AUDPROC_MODULE_ID_DTMF_DETECTION               0x00010940

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_AUDPROC_API_PROP_H_ */
