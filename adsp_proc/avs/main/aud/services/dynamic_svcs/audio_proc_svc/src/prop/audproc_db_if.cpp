/*==========================================================================
ELite Source File

This file implements the setting of static topologies and modules into the
PP databases.

Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/16/10   DG      Created file.
=========================================================================== */

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "EliteTopology_db_if.h"
#include "EliteCmnTopology_db.h"
#include "audproc_comdef.h"
#include "adsp_core_api.h"
#include "adsp_amdb_static.h"
#include "AudDynaPPSvc.h"
#include "AudDevMgr_PrivateDefs.h"
#include "adsp_internal_api.h"
#include "ds1_calib.h"
#include "dts_hpx_calib.h"

#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

static avcs_module_info_t def_audproc_channel_mixer __attribute__ ((unused)) = {
AUDPROC_MODULE_ID_CHMIXER,
};

static avcs_module_info_t vol_ctrl_def = {
AUDPROC_MODULE_ID_VOL_CTRL,
};

static avcs_module_info_t vol_ctrl_instance_2_def __attribute__ ((unused)) = {
AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2,
};

static avcs_module_info_t resampler_stream_def = {
AUDPROC_MODULE_ID_RESAMPLER,
};

static avcs_module_info_t resampler_device_def = {
AUDPROC_MODULE_ID_RESAMPLER,
};

static avcs_module_info_t sampleslip_rx_def = {
AUDPROC_MODULE_ID_SAMPLESLIP,
};

static avcs_module_info_t sampleslip_tx_def = {
AUDPROC_MODULE_ID_SAMPLESLIP,
};

static avcs_module_info_t pbe_def = {
AUDPROC_MODULE_ID_PBE,
};

static avcs_module_info_t iir_def = {
AUDPROC_MODULE_ID_IIR_TUNING_FILTER,
};

static avcs_module_info_t mbdrc_def = {
AUDPROC_MODULE_ID_MBDRC,
};

static avcs_module_info_t rx_codec_gain_def = {
AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL,
};

static avcs_module_info_t mchan_iir_def = {
AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER,
};

static avcs_module_info_t peak_meter_def = {
AUDPROC_MODULE_ID_PEAK_METER,
};

static avcs_module_info_t mbdrc_v2_def = {
AUDPROC_MODULE_ID_MBDRCV2,
};

static avcs_module_info_t mbdrc_v3_def = {
AUDPROC_MODULE_ID_MBDRCV3,
};

static avcs_module_info_t left_iir_def = {
AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER,
};

static avcs_module_info_t right_iir_def = {
AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER,
};

static avcs_module_info_t srs_def = {
APPI_SRS_TRUMEDIA_MODULE_ID,
};

static avcs_module_info_t tx_mic_gain_def = {
AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL,
};

static avcs_module_info_t hpf_iir_def = {
AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER,
};

static avcs_module_info_t eans_def = {
AUDPROC_MODULE_ID_EANS,
};

static avcs_module_info_t tx_iir_def = {
AUDPROC_MODULE_ID_TX_IIR_FILTER,
};

static avcs_module_info_t ds1ap_def = {
AUDPROC_MODULE_ID_DS1AP,
};

static avcs_module_info_t dts_hpx_postmix_def = {
AUDPROC_MODULE_ID_DTS_HPX_POSTMIX,
};

static avcs_module_info_t dts_hpx_premix_def = {
AUDPROC_MODULE_ID_DTS_HPX_PREMIX,
};

static avcs_module_info_t aig_def = {
AUDPROC_MODULE_ID_AIG,
};

static avcs_module_info_t dts_dialnorm_def = {
AUDPROC_MODULE_ID_DTS_DIALNORM,
};

static avcs_module_info_t dts_drc_def = {
AUDPROC_MODULE_ID_DTS_DRC,
};

static avcs_module_info_t compressed_latency_def = {
AUDPROC_MODULE_ID_COMPRESSED_LATENCY,
};

static avcs_module_info_t compressed_mute_def = {
AUDPROC_MODULE_ID_COMPRESSED_MUTE,
};

static avcs_module_info_t lpcm_delay_def = {
AUDPROC_MODULE_ID_DELAY,
};

static avcs_module_info_t virtualizer_def = {
AUDPROC_MODULE_ID_VIRTUALIZER,
};

static avcs_module_info_t reverb_def = {
AUDPROC_MODULE_ID_REVERB,
};

static avcs_module_info_t bass_boost_def = {
AUDPROC_MODULE_ID_BASS_BOOST,
};

static avcs_module_info_t popless_equalizer_def = {
AUDPROC_MODULE_ID_POPLESS_EQUALIZER,
};

static avcs_module_info_t visualizer_def = {
AUDPROC_MODULE_ID_VISUALIZER,
};

static avcs_module_info_t mfc_def __attribute__ ((unused)) = {
AUDPROC_MODULE_ID_MFC,
};

static avcs_topology_definition_t none_uncompressed_topo_def = {
		0,
		AUDPROC_TOPOLOGY_ID_UNCOMPRESSED_NONE,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		0
};

static avcs_topology_definition_t none_compressed_topo_def = {
		0,
		AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		0
};

static avcs_topology_definition_t none_copp_topo_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_NONE_AUDIO_COPP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		3
};

static avcs_module_info_t none_copp_topo_mod_def[]= {
            resampler_device_def,
            vol_ctrl_def,
		sampleslip_rx_def
};

static avcs_topology_definition_t none_coprep_topo_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_NONE_AUDIO_COPP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		3
};

static avcs_module_info_t none_coprep_topo_mod_def[]= {
            sampleslip_tx_def,
            resampler_device_def,
		vol_ctrl_def
};

static avcs_topology_definition_t copp_default_def = {
		0,
		ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_RX,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		8
};

static avcs_module_info_t copp_default_mod_def[]= {
            lpcm_delay_def,
            resampler_device_def,
            vol_ctrl_def,
            pbe_def,
            iir_def,
            mbdrc_def,
            rx_codec_gain_def,
            sampleslip_rx_def
};


static avcs_topology_definition_t copp_default_mbdrc_v3_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_MBDRCV3,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		7
};

static avcs_module_info_t copp_default_mbdrc_v3_mod_def[]= {
            resampler_device_def,
            vol_ctrl_def,
            pbe_def,
            iir_def,
            mbdrc_v3_def,
            rx_codec_gain_def,
            sampleslip_rx_def,
};

static avcs_topology_definition_t copp_spk_stereo_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		7
};

static avcs_module_info_t copp_spk_stereo_mod_def[]= {
            resampler_device_def,
            vol_ctrl_def,
            pbe_def,
            iir_def,
            mbdrc_def,
            rx_codec_gain_def,
            sampleslip_rx_def,
};

static avcs_topology_definition_t copp_mchan_def = {
		0,
      AUDPROC_COPP_TOPOLOGY_ID_MCHAN_V2_DEFAULT,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		7
};

static avcs_module_info_t copp_mchan_mod_def[]= {
            resampler_device_def,
            vol_ctrl_def,
            pbe_def,
            mchan_iir_def,
            mbdrc_def,
            rx_codec_gain_def,
            sampleslip_rx_def,
};

static avcs_topology_definition_t copp_peakmeter_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_PEAKMETER_AUDIO_COPP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		8
};

static avcs_module_info_t copp_peakmeter_mod_def[]= {
            resampler_device_def,
            peak_meter_def,
            vol_ctrl_def,
            pbe_def,
            iir_def,
            mbdrc_def,
            rx_codec_gain_def,
            sampleslip_rx_def,
};

static avcs_topology_definition_t copp_mbdrc_v2_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP_MBDRCV2,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		7
};

static avcs_module_info_t copp_mbdrc_v2_mod_def[]= {
            resampler_device_def,
            vol_ctrl_def,
            pbe_def,
            iir_def,
            mbdrc_v2_def,
            rx_codec_gain_def,
            sampleslip_rx_def,
};

static avcs_topology_definition_t copp_stereo_iir_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		8
};

static avcs_module_info_t copp_stereo_iir_mod_def[]= {
            resampler_device_def,
            vol_ctrl_def,
            pbe_def,
            left_iir_def,
            right_iir_def,
            mbdrc_def,
            rx_codec_gain_def,
            sampleslip_rx_def,
};

static avcs_topology_definition_t copp_stereo_iir_mbdrc_v2_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP_MBDRCV2,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		8
};

static avcs_module_info_t copp_stereo_iir_mbdrc_v2_mod_def[]= {
            resampler_device_def,
            vol_ctrl_def,
            pbe_def,
            left_iir_def,
            right_iir_def,
            mbdrc_v2_def,
            rx_codec_gain_def,
            sampleslip_rx_def,
};

static avcs_topology_definition_t copp_spkr_mono_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		7
};

static avcs_module_info_t copp_spkr_mono_mod_def[]= {
            resampler_device_def,
            vol_ctrl_def,
            pbe_def,
            iir_def,
            mbdrc_def,
            rx_codec_gain_def,
            sampleslip_rx_def,
};

static avcs_topology_definition_t copp_spkr_mono_mbdrc_v2_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP_MBDRCV2,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		7
};

static avcs_module_info_t copp_spkr_mono_mbdrc_v2_mod_def[]= {
         resampler_device_def,
         vol_ctrl_def,
         pbe_def,
         iir_def,
         mbdrc_v2_def,
         rx_codec_gain_def,
         sampleslip_rx_def,
};

static avcs_topology_definition_t copp_def_SRS_TruMedia = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SRS_TRUMEDIA_TOPOLOGY,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		5
};

static avcs_module_info_t copp_def_SRS_TruMedia_mod[]= {
            resampler_device_def,
            srs_def,
            peak_meter_def,
            vol_ctrl_def,
            rx_codec_gain_def,
};

static avcs_topology_definition_t coprep_default_def = {
		0,
		ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_TX,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		9
};

static avcs_module_info_t coprep_default_mod_def[]= {
            sampleslip_tx_def,
            vol_ctrl_def,
            tx_mic_gain_def,
            hpf_iir_def,
            eans_def,
            tx_iir_def,
            aig_def,
            mbdrc_def,
            resampler_device_def,
};

static avcs_topology_definition_t coprep_mic_mono_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_MONO_AUDIO_COPP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		9
};

static avcs_module_info_t coprep_mic_mono_mod_def[]= {
            sampleslip_tx_def,
            vol_ctrl_def,
            tx_mic_gain_def,
            hpf_iir_def,
            eans_def,
            tx_iir_def,
            aig_def,
            mbdrc_def,
            resampler_device_def,
};

static avcs_topology_definition_t coprep_default_mic_mbdrc_v3_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_MIC_AUDIO_COPP_MBDRCV3,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		9
};

static avcs_module_info_t coprep_default_mic_mbdrc_v3_mod_def[]= {
            sampleslip_tx_def,
            vol_ctrl_def,
            tx_mic_gain_def,
            hpf_iir_def,
            eans_def,
            tx_iir_def,
            aig_def,
            mbdrc_v3_def,
            resampler_device_def,
};

static avcs_topology_definition_t coprep_mic_stereo_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_STEREO_AUDIO_COPP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		9
};

static avcs_module_info_t coprep_mic_stereo_mod_def[]= {
            sampleslip_tx_def,
            vol_ctrl_def,
            tx_mic_gain_def,
            hpf_iir_def,
            eans_def,
            tx_iir_def,
            aig_def,
            mbdrc_def,
            resampler_device_def,
};

static avcs_topology_definition_t coprep_mchan_def = {
		0,
      AUDPROC_COPP_TOPOLOGY_ID_MCHAN_V2_DEFAULT,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		8
};

static avcs_module_info_t coprep_mchan_mod_def[]= {
            vol_ctrl_def,
            tx_mic_gain_def,
            hpf_iir_def,
            eans_def,
            mchan_iir_def,
            mbdrc_def,
            sampleslip_tx_def,
            resampler_device_def,
};

static avcs_topology_definition_t coprep_mchan_iir_def = {
		0,
      AUDPROC_COPP_TOPOLOGY_ID_MCHAN_IIR_AUDIO,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		1
};

static avcs_module_info_t coprep_mchan_iir_mod_def[]= {
            mchan_iir_def,
};

static avcs_topology_definition_t popp_def = {
		0,
      ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		2
};

static avcs_module_info_t popp_mod_def[]= {
            resampler_stream_def,
            vol_ctrl_def,
};

static avcs_topology_definition_t popp_peakmeter_def = {
		0,
      ASM_STREAM_POSTPROC_TOPO_ID_PEAKMETER,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		3
};

static avcs_module_info_t popp_peakmeter_mod_def[]= {
            resampler_stream_def,
            vol_ctrl_def,
            peak_meter_def,
};

static avcs_topology_definition_t popp_none_def = {
		0,
      ASM_STREAM_POSTPROC_TOPO_ID_NONE,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		2
};

static avcs_module_info_t popp_none_mod_def[]= {
            resampler_stream_def,
            vol_ctrl_def,
};

static avcs_topology_definition_t poprep_none_def __attribute__ ((unused)) = {
		0,
      ASM_STREAM_PREPROC_TOPO_ID_NONE,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		2
};

static avcs_module_info_t poprep_none_mod_def[] __attribute__ ((unused)) = {
            resampler_stream_def,
            vol_ctrl_def,
};

static avcs_topology_definition_t poprep_default_def = {
		0,
      ASM_STREAM_PREPROC_TOPO_ID_DEFAULT,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		2
};

static avcs_module_info_t poprep_default_mod_def[]= {
            resampler_stream_def,
            vol_ctrl_def,
};

static avcs_topology_definition_t multichannel_popp_def = {
		0,
      ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		3
};

static avcs_module_info_t multichannel_popp_mod_def[]= {
            resampler_stream_def,
            peak_meter_def,
            vol_ctrl_def,
};

static avcs_topology_definition_t multichannel_copp_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MCH_PEAK_VOL,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		3
};

static avcs_module_info_t multichannel_copp_mod_def[]= {
         resampler_device_def,
         peak_meter_def,
         vol_ctrl_def,
};

static avcs_topology_definition_t ds1ap_copp_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DS1AP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		1
};

static avcs_module_info_t ds1ap_copp_mod_def[]= {
            ds1ap_def,
};

static avcs_topology_definition_t dts_hpx_postmix_copp_def = {
		0,
      ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DTS_HPX,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		1
};

static avcs_module_info_t dts_hpx_postmix_copp_mod_def[]= {
            dts_hpx_postmix_def,
};

static avcs_topology_definition_t ds1ap_popp_def = {
		0,
      ASM_STREAM_POSTPROC_TOPO_ID_DS1AP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		1
};

static avcs_module_info_t ds1ap_popp_mod_def[]= {
            ds1ap_def,
};

static avcs_topology_definition_t dts_hpx_premix_popp_def = {
		0,
		ASM_STREAM_POSTPROC_TOPO_ID_DTS_HPX_PREMIX,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		2
};

static avcs_module_info_t dts_hpx_premix_popp_mod_def[]= {
            resampler_stream_def,
            dts_hpx_premix_def,
};

static avcs_topology_definition_t dts_hpx_postmix_popp_def = {
		0,
		ASM_STREAM_POSTPROC_TOPO_ID_DTS_HPX_POSTMIX,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		1
};

static avcs_module_info_t dts_hpx_postmix_popp_mod_def[]= {
            dts_hpx_postmix_def,
};

static avcs_topology_definition_t dts_hpx_pipeline_popp_def = {
		0,
		ASM_STREAM_POSTPROC_TOPO_ID_DTS_HPX_PIPELINE,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		2
};

static avcs_module_info_t dts_hpx_pipeline_popp_mod_def[]= {
            dts_hpx_premix_def,
            dts_hpx_postmix_def,
};

static avcs_topology_definition_t dts_popp_def = {
		0,
		AUDPROC_POPP_TOPOLOGY_ID_DTS_AUDIO,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		4
};

static avcs_module_info_t dts_popp_mod_def[]= {
            dts_dialnorm_def,
            dts_drc_def,
            resampler_stream_def,
            vol_ctrl_def,
};

static avcs_topology_definition_t compressed_default_def = {
		0,
      AUDPROC_TOPOLOGY_ID_COMPRESSED_DEFAULT,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		2
};

static avcs_module_info_t compressed_default_mod_def[]= {
            compressed_latency_def,
            compressed_mute_def,
};

static avcs_topology_definition_t copp_audio_plus_headphone_topo_def = {
		0,
      AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_HEADPHONE,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		4
};

static avcs_module_info_t copp_audio_plus_headphone_topo_mod_def[]= {
            virtualizer_def,
            reverb_def,
            bass_boost_def,
            visualizer_def,
};

static avcs_topology_definition_t copp_audio_plus_speaker_topo_def = {
		0,
      AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_SPEAKER,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		4
};

static avcs_module_info_t copp_audio_plus_speaker_topo_mod_def[]= {
            virtualizer_def,
            reverb_def,
            pbe_def,
            visualizer_def,
};

static avcs_topology_definition_t popp_audio_plus_def = {
		0,
      AUDPROC_POPP_TOPOLOGY_ID_AUDIO_PLUS,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		3
};

static avcs_module_info_t popp_audio_plus_mod_def[]= {
            resampler_stream_def,
            vol_ctrl_def,
            popless_equalizer_def,
};

static avcs_topology_definition_t avcs_audio_static_topos[] = {
      popp_def,
      popp_none_def,
      popp_peakmeter_def,
      multichannel_popp_def,
      dts_popp_def,
      ds1ap_popp_def,
      dts_hpx_premix_popp_def,
      dts_hpx_postmix_popp_def,
      dts_hpx_pipeline_popp_def,
		popp_audio_plus_def,
      poprep_default_def,
      popp_none_def,
      copp_mchan_def,
      none_copp_topo_def,
      copp_default_def,
      copp_peakmeter_def,
      copp_spkr_mono_def,
      copp_spk_stereo_def,
      copp_stereo_iir_def,
      copp_spkr_mono_mbdrc_v2_def,
      copp_mbdrc_v2_def,
      copp_stereo_iir_mbdrc_v2_def,
      copp_default_mbdrc_v3_def,
      multichannel_copp_def,
      copp_def_SRS_TruMedia,
      ds1ap_copp_def,
      compressed_default_def,
      dts_hpx_postmix_copp_def,
      copp_audio_plus_headphone_topo_def,
		copp_audio_plus_speaker_topo_def,
      coprep_mchan_def,
      coprep_default_def,
      none_coprep_topo_def,
      coprep_mic_mono_def,
      coprep_default_mic_mbdrc_v3_def,
      coprep_mic_stereo_def,
		coprep_mchan_iir_def,
		none_uncompressed_topo_def,
		none_compressed_topo_def
};

static avcs_module_info_t *avcs_audio_static_topos_mods_ptr[] = {
		&popp_mod_def[0],
		&popp_none_mod_def[0],
		&popp_peakmeter_mod_def[0],
		&multichannel_popp_mod_def[0],
		&dts_popp_mod_def[0],
		&ds1ap_popp_mod_def[0],
		&dts_hpx_premix_popp_mod_def[0],
		&dts_hpx_postmix_popp_mod_def[0],
		&dts_hpx_pipeline_popp_mod_def[0],
		&popp_audio_plus_mod_def[0],
		&poprep_default_mod_def[0],
		&popp_none_mod_def[0],
		&copp_mchan_mod_def[0],
		&none_copp_topo_mod_def[0],
		&copp_default_mod_def[0],
		&copp_peakmeter_mod_def[0],
		&copp_spkr_mono_mod_def[0],
		&copp_spk_stereo_mod_def[0],
		&copp_stereo_iir_mod_def[0],
		&copp_spkr_mono_mbdrc_v2_mod_def[0],
		&copp_mbdrc_v2_mod_def[0],
		&copp_stereo_iir_mbdrc_v2_mod_def[0],
		&copp_default_mbdrc_v3_mod_def[0],
		&multichannel_copp_mod_def[0],
		&copp_def_SRS_TruMedia_mod[0],
		&ds1ap_copp_mod_def[0],
		&compressed_default_mod_def[0],
		&dts_hpx_postmix_copp_mod_def[0],
		&copp_audio_plus_headphone_topo_mod_def[0],
		&copp_audio_plus_speaker_topo_mod_def[0],
		&coprep_mchan_mod_def[0],
		&coprep_default_mod_def[0],
		&none_coprep_topo_mod_def[0],
		&coprep_mic_mono_mod_def[0],
		&coprep_default_mic_mbdrc_v3_mod_def[0],
		&coprep_mic_stereo_mod_def[0],
		&coprep_mchan_iir_mod_def[0],
		NULL,
		NULL
};

// Module definition table for CAPIv2 modules
typedef struct _capi_v2_module_definition_t
{
   uint32_t module_id;
   capi_v2_get_static_properties_f get_static_properties_f;
   capi_v2_init_f init_f;
} capi_v2_module_definition_t;

ADSPResult AudPP_pptype_to_topo_db_client_type(AudPP_AudProcType pp_type, elite_topo_db_client_type* client_type)
{
   ADSPResult result = ADSP_EOK;

   switch (pp_type)
   {
      case DYNA_SVC_PP_TYPE_POPP:
         *client_type = TOPO_DB_CLIENT_TYPE_POPP;
         break;
      case DYNA_SVC_PP_TYPE_COPP:
         *client_type = TOPO_DB_CLIENT_TYPE_COPP;
         break;
      case DYNA_SVC_PP_TYPE_POPREP:
         *client_type = TOPO_DB_CLIENT_TYPE_POPREP;
         break;
      case DYNA_SVC_PP_TYPE_COPREP:
         *client_type = TOPO_DB_CLIENT_TYPE_COPREP;
         break;
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported PP type %ld", (uint32_t)pp_type);
         *client_type = TOPO_DB_CLIENT_TYPE_INVALID;
         result = ADSP_EUNSUPPORTED;
   }

   return result;
}

ADSPResult AudPP_AddStaticTopologies(void)
{
	ADSPResult result = ADSP_EOK;
      return result;
   }

ADSPResult AudPP_AddStaticTopologiesAndModules(void)
   {
	ADSPResult result = ADSP_EOK;
      return result;
   }

ADSPResult aud_add_static_topos_and_mods_to_cmn_db(void)
   {
	ADSPResult result = ADSP_EOK;

	//Add the audio PP static topologies to the common global database
	result = aud_add_static_topos_to_cmn_db();
   if (ADSP_FAILED(result))
   {
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "aud_add_static_topos_to_cmn_db failed with %d", result);
      return result;
   }
   //adding static modules to AMDB is now done in module_mgr
    return result;
}

ADSPResult aud_add_static_topos_to_cmn_db(void)
{
	ADSPResult result = ADSP_EOK;

	for (uint32_t i=0; i< SIZE_OF_ARRAY(avcs_audio_static_topos); i++)
	{
		avcs_topology_definition_t *topo_def_ptr = &avcs_audio_static_topos[i];
		avcs_module_info_t *mod_def_ptr = avcs_audio_static_topos_mods_ptr[i];

		//Add single topology
		result = elite_cmn_topo_db_add_topology(topo_def_ptr, mod_def_ptr, ELITE_CMN_STATIC_TOPOLOGIES);
		if (ADSP_FAILED(result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_add_topology failed with %d", result);
			return result;
		}
	}
	return result;
}
