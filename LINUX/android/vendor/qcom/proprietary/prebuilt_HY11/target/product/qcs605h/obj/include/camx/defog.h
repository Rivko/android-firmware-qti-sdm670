////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  defog.h
/// @brief DefogLib class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DEFOG_H__
#define __DEFOG_H__

#include "camxdefs.h"
#include "iqcommondefs.h"
#include "camxchinodeutil.h"
#include "chiiqmoduledefines.h"
#include <cutils/properties.h>
#include "shdr3.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFOG_CE_VERSION "1.5.1"

	/// @brief The group tag for defog parameter update
	typedef UINT32 DefogConfParamGroup;
	static const DefogConfParamGroup DefogConfParamGroupNone                    = (1 << 0);     ///< Generic group
	static const DefogConfParamGroup DefogConfParamGroupDefogEn                 = (1 << 1);     ///< Enable Defog module
	static const DefogConfParamGroup DefogConfParamGroupCEEn                    = (1 << 2);     ///< Enable CE module
	static const DefogConfParamGroup DefogConfParamGroupSpeed                   = (1 << 3);     ///< Speed for Defog and CE modules
	static const DefogConfParamGroup DefogConfParamGroupConvMode                = (1 << 4);     ///< Convergence mode for Defog and CE modules
	static const DefogConfParamGroup DefogConfParamGroupSDEn                    = (1 << 5);     ///< Enable fog/CE scene detectors
	static const DefogConfParamGroup DefogConfParamGroupSD2AEn                  = (1 << 6);     ///< Fog/CE scene detector with AEC/AWB info
	static const DefogConfParamGroup DefogConfParamGroupAlgo                    = (1 << 7);     ///< Defog algorithm
	static const DefogConfParamGroup DefogConfParamGroupStrength                = (1 << 8);     ///< Defog strength
	static const DefogConfParamGroup DefogConfParamGroupLPAlgoDecisionMode      = (1 << 9);     ///< Decision mode for LP defog
	static const DefogConfParamGroup DefogConfParamGroupLPColorCompensationGain = (1 << 10);    ///< Color compensation gain for LP defog
	static const DefogConfParamGroup DefogConfParamGroupABCEn                   = (1 << 11);    ///< Enable ABC module
	static const DefogConfParamGroup DefogConfParamGroupACCEn                   = (1 << 12);    ///< Enable ACC module
	static const DefogConfParamGroup DefogConfParamGroupDefogDarkThres          = (1 << 13);    ///< Threshold for dark region
	static const DefogConfParamGroup DefogConfParamGroupDefogBrightThres        = (1 << 14);    ///< Threshold for bright detion
	static const DefogConfParamGroup DefogConfParamGroupDarkLimit               = (1 << 15);    ///< Limit Max defog strength on dark region
	static const DefogConfParamGroup DefogConfParamGroupBrightLimit             = (1 << 16);    ///< Limit Max defog strength on bright region
	static const DefogConfParamGroup DefogConfParamGroupDarkPreserve            = (1 << 17);    ///< Preserve more details on dark region
	static const DefogConfParamGroup DefogConfParamGroupBrightPreserve          = (1 << 18);    ///< Preserve more details on bright region
	static const DefogConfParamGroup DefogConfParamGroupABCStr                  = (1 << 19);    ///< ABC strength
	static const DefogConfParamGroup DefogConfParamGroupACCDarkStr              = (1 << 20);    ///< ACC strength for dark region
	static const DefogConfParamGroup DefogConfParamGroupACCBrightStr            = (1 << 21);    ///< ACC strength for bright region
	static const DefogConfParamGroup DefogConfParamGroupAFSDTriggers            = (1 << 22);    ///< Triggers of auto fog scene detection
	static const DefogConfParamGroup DefogConfParamGroupGUCEn                   = (1 << 23);    ///< Enable GUC module
	static const DefogConfParamGroup DefogConfParamGroupDCCEn                   = (1 << 24);    ///< Enable DCC module
	static const DefogConfParamGroup DefogConfParamGroupGUCStr                  = (1 << 25);    ///< GUC Strength
	static const DefogConfParamGroup DefogConfParamGroupDCCDarkStr              = (1 << 26);    ///< DCC Strength for dark region
	static const DefogConfParamGroup DefogConfParamGroupDCCBrightStr            = (1 << 27);    ///< DCC Strength for bright region
	static const DefogConfParamGroup DefogConfParamGroupACESDTriggers           = (1 << 28);    ///< Triggers of auto contrast enhancement scene detection
	static const DefogConfParamGroup DefogConfParamGroupAll                     = 0x1FFFFFFF;   ///< All groups
	static const UINT32              DefogConfParamGroupNum                     = 29;

	/// @brief The group tag for defog input update
	typedef UINT32 DefogInputTypeGroup;
	static const DefogInputTypeGroup DefogInputTypeGroupNone                    = (1 << 0);     ///< Generic group
	static const DefogInputTypeGroup DefogInputTypeGroupAECStats                = (1 << 1);     ///< AEC Stats
	static const DefogInputTypeGroup DefogInputTypeGroupAWBStats                = (1 << 2);     ///< AWB Stats
	static const DefogInputTypeGroup DefogInputTypeGroupSHDR3Stats              = (1 << 3);     ///< SHDR3 Stats
	static const DefogInputTypeGroup DefogInputTypeGroupIHistStats              = (1 << 4);     ///< iHist Stats
	static const DefogInputTypeGroup DefogInputTypeGroupInterGLUT               = (1 << 5);     ///< BPS/IFE Gamma
	static const DefogInputTypeGroup DefogInputTypeGroupDS4                     = (1 << 6);     ///< DS4
	static const DefogInputTypeGroup DefogInputTypeGroupSHDR                    = (1 << 7);     ///< SHDR3
	static const DefogInputTypeGroup DefogInputTypeGroupInterANR                = (1 << 8);     ///< Interpolation IPE ANR
	static const DefogInputTypeGroup DefogInputTypeGroupInterLTM                = (1 << 9);     ///< Interpolation IPE LTM
	static const DefogInputTypeGroup DefogInputTypeGroupInterGamma              = (1 << 10);     ///< Interpolation IPE Gamma
	static const DefogInputTypeGroup DefogInputTypeGroupInterCV                 = (1 << 11);    ///< Interpolation CV
	static const DefogInputTypeGroup DefogInputTypeGroupInterASF                = (1 << 12);    ///< Interpolation ASF
	static const UINT32              DefogInputTypeGroupNum                     = 13;

    /// @brief Defog algorithm
    typedef enum
    {
        DefogAlgoInvalid                                =  -1,
        DefogAlgoHQDefog                                =   0,
        DefogAlgoLPDefog                                =   1,
        DefogAlgoManualHQDefog                          =   2,
        DefogAlgoManualLPDefog                          =   3,
        DefogAlgoHQDefogManualLPDefog                   =   4,
        DefogAlgoLpDefogManualHQDefog                   =   5,
        DefogAlgoMax                                    =   6,
    } DefogAlgoMode;

    /// @brief LP algorithm decision mode
    typedef enum
    {
        LPAlgoDecisionModeInvalid                       =  -1,
        LPAlgoDecisionModeMin                           =   0,
        LPAlgoDecisionModeAvg                           =   1,
        LPAlgoDecisionModeMax                           =   2,
    } LPAlgoDecisionMode;

    /// @brief Scene type of DNR trigger for fog scene detection
    typedef enum
    {
        FogSceneDetectionDNRTriggerModeInvalid          =  -1,
        FogSceneDetectionDNRTriggerModeFlatScene        =   0,
        FogSceneDetectionDNRTriggerModeFogScene         =   1,
        FogSceneDetectionDNRTriggerModeNormalScene      =   2,
        FogSceneDetectionDNRTriggerModeMax              =   3,
    } FogSceneDetectionDNRTriggerMode;

    /// @brief Scene type of Lux trigger for fog scene detection
    typedef enum
    {
        FogSceneDetectionLuxTriggerModeInvalid          =  -1,
        FogSceneDetectionLuxTriggerModeDayLight         =   0,
        FogSceneDetectionLuxTriggerModeNormalLight      =   1,
        FogSceneDetectionLuxTriggerModeLowLight         =   2,
        FogSceneDetectionLuxTriggerModeMax              =   3,
    } FogSceneDetectionLuxTriggerMode;

    /// @brief Scene type of CCT trigger for fog scene detection
    typedef enum
    {
        FogSceneDetectionCCTTriggerModeInvalid          =  -1,
        FogSceneDetectionCCTTriggerModeLow              =   0,
        FogSceneDetectionCCTTriggerModeIndoorAndOutdoor =   1,
        FogSceneDetectionCCTTriggerModeOutdoorAndFog    =   2,
        FogSceneDetectionCCTTriggerModeHigh             =   3,
        FogSceneDetectionCCTTriggerModeMax              =   4,
    } FogSceneDetectionCCTTriggerMode;

    /// @brief Scene type of DRC gain trigger for fog scene detection
	typedef enum
	{
		FogSceneDetectionDRCTriggerModeInvalid          =  -1,
		FogSceneDetectionDRCTriggerModeLowDRCGain       =   0,
		FogSceneDetectionDRCTriggerModeHighDRCGain      =   1,
		FogSceneDetectionDRCTriggerModeMax              =   2,
	} FogSceneDetectionDRCTriggerMode;

    /// @brief Scene type of HDR ratio trigger for fog scene detection
	typedef enum
	{
		FogSceneDetectionHDRTriggerModeInvalid          =  -1,
		FogSceneDetectionHDRTriggerModeLowHDRRatio      =   0,
		FogSceneDetectionHDRTriggerModeHighHDRRatio     =   1,
		FogSceneDetectionHDRTriggerModeMax              =   2,
	} FogSceneDetectionHDRTriggerMode;

    /// @brief Scene type of gain trigger for CE scene detection
	typedef enum
	{
		CESceneDetectionGainTriggerModeInvalid          =  -1,
		CESceneDetectionGainTriggerModeLowGain          =   0,
		CESceneDetectionGainTriggerModeNormalGain       =   1,
		CESceneDetectionGainTriggerModeHighGain         =   2,
		CESceneDetectionGainTriggerModeMax              =   3,
	} CESceneDetectionGainTriggerMode;

    /// @brief Defog/CE convergence mode
	typedef enum
	{
		DefogCEConvModeInvalid                          =  -1,
		DefogCEConvModeStartUp                          =   0,
		DefogCEConvModeSerial                           =   1,
		DefogCEConvModeParallel                         =   2,
		DefogCEDefogConvModeMax                         =   3,
	} DefogCEConvMode;

    /// @brief Defog/CE convergence state
	typedef enum
	{
		DefogCEConvStateInvalid                         =  -1,
		DefogCEConvStateInactive                        =   0,
		DefogCEConvStateConverging                      =   1,
		DefogCEConvStateConverged                       =   2,
		DefogCEConvStateMax                             =   3,
	} DefogCEConvState;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Defog/CE version information
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct SoftwareVersionInfoType
	{
		UINT8  majorVersion;                    ///< Major version number
		UINT8  minorVersion;                    ///< Minor version number
		UINT8  patchVersion;                    ///< Patch version number
		UINT8  newFeatureIndicator;             ///< New feature indicator: 0x01: 2AInfo, 0x02: CE, 0x04: ADRC, 0x08: SHDR3
	};

    /// @brief Defog/CE handle
	typedef VOID* StatsAlgorithmHandle;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Trigger information
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct TriggerParams
	{
		FLOAT start;                            ///< start trigger point
		FLOAT end;                              ///< end trigger point
		UINT32 p;                               ///< probability
	};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Fog scene detection trigger information
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef struct FogSceneDetectionParams
	{
		TriggerParams dnr_trigger[3];           ///< [0]: flat_scene, [1]: fog_scene, [2]: normal_scene, range: 0.0 - 8.0 EV
		TriggerParams lux_trigger[3];           ///< [0]: daylight, [1]: normal light, [2]: low light, range: 0.0 - 1000.0 lux index
		TriggerParams cct_trigger[4];           ///< [0]: low CCT, [1]: indoor/outdoor CCT, [2]: outdoor/fog CCT, [3]: high CCT
		TriggerParams drc_trigger[2];           ///< [0]: low DRC gain, [1] high DRC gain
		TriggerParams hdr_trigger[2];           ///< [0]: low HDR ratio, [1] high HDR ratio
	} FOG_SCENE_DETECTION_PARAMS;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief CE scene detection trigger information
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef struct CESceneDetectionParams
	{
		TriggerParams gain_trigger[3];          ///< [0]: low gain, [1]: normal gain, [2]: high gain, range: 1.0 - 4096.0
	} CE_SCENE_DETECTION_PARAMS;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Defog/CE Control information
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct DefogConfigParams
	{
		UINT32 param_update_mask;                       ///< [0x00000000:0x0000FFFF], refer to 'DefogConfParamGroup' and decide which below parameters should be updated.

		/* Defog/CE enanle */
		BOOL   defog_en;                                ///< 0: disable defog, 1: enable defog                                       (default: 1)
		BOOL   ce_en;                                   ///< 0: disable CE(contrast enahancement), 1: enable CE                      (default: 0)

		/* Defog/CE basic settings */
		UINT32 speed;                                   ///< [0:10], convergence speed, 0: slowest, 10: fastest                      (default: 10)
		UINT32 convergence_mode;                        ///< 1: serial mode, 2: parallel mode                                        (default: 1)

		/* Defog/CE scene detector settings */
		BOOL   sd_en;                                   ///< 0: disable SD(scene detector), 1: enable SD                             (default: 1)
		BOOL   sd_2a_en;                                ///< 0: SD without 2A info, 1: SD with 2A info                               (default: 1)

		/* Defog basic settings */
		UINT32 algo;                                    ///< 0: Gamma Defog, 1: CV Defog, 2: Manual Gamma Defog, 3: Manual CV Defog  (default: 0)
		FLOAT  strength;                                ///< [0.0:100.0], 0.0: no defog effect, 100.0: strongest defog effect        (default: 1.0)
		UINT32 lp_algo_decision_mode;                   ///< 0: min bin decision, 1: avg bin decision, 2: max bin decision           (default: 0)
		FLOAT  lp_algo_color_comp_gain;                 ///< 0.0: disable color compensation, 1.0: 1x color compensation gain        (default: 1.0)

		/* Defog advanced settings */
		BOOL   abc_en;                                  ///< 0: disable ABC(adaptive brightness compensation), 1: enable ABC         (default: 1)
		BOOL   acc_en;                                  ///< 0: disable ACC(advanced contrast control), 1: enable ACC                (default: 1)
		UINT32 defog_dark_thres;                        ///< [1:256]  (default: 20): the two parameters are for defog reprocess. if defog status is settled and dark/bright bins are
		UINT32 defog_bright_thres;                      ///< [1:256]  (default: 80):   within the range of defog_bright_thres and defog_dark_thres, it will keep the previous defog settings.
		UINT32 dark_limit;                              ///< [0:255]: limit max internal defog strength for dark regions             (default: 255)
		UINT32 bright_limit;                            ///< [0:255]: limit max internal defog strength for bright regions           (default: 0)
		UINT32 dark_preserve;                           ///< [0:255]: move dark bin to preserve dark details                         (default: 10)
		UINT32 bright_preserve;                         ///< [0:255]: move bright bin to preserve bright details                     (default: 50)
		FLOAT  abc_str;                                 ///< [0.0:4.0]: strength for adaptive brightness compensation                (default: 2.0)
		FLOAT  acc_dark_str;                            ///< [0.0:4.0]: ACC strength for dark region                                 (default: 2.0)
		FLOAT  acc_bright_str;                          ///< [0.0:4.0]: ACC strength for bright region                               (default: 0.5)
		FOG_SCENE_DETECTION_PARAMS defog_trig_params;   ///< Define DNR/LuxIdx/CCT triggers for fog scene detection                  (default settings are as below)
		/********************** Defog trigger default settings ****************************
		dnr_trigger[FogSceneDetectionDNRTriggerModeFlatScene].start         = 0.0f;
		dnr_trigger[FogSceneDetectionDNRTriggerModeFlatScene].end           = 1.0f;
		dnr_trigger[FogSceneDetectionDNRTriggerModeFogScene].start          = 1.5f;
		dnr_trigger[FogSceneDetectionDNRTriggerModeFogScene].end            = 4.0f;
		dnr_trigger[FogSceneDetectionDNRTriggerModeNormalScene].start       = 4.5f;
		dnr_trigger[FogSceneDetectionDNRTriggerModeNormalScene].end	        = 8.0f;
		dnr_trigger[FogSceneDetectionDNRTriggerModeFlatScene].p	            = 0;
		dnr_trigger[FogSceneDetectionDNRTriggerModeFogScene].p              = 100;
		dnr_trigger[FogSceneDetectionDNRTriggerModeNormalScene].p           = 0;
		lux_trigger[FogSceneDetectionLuxTriggerModeDayLight].start          = 0.0f;
		lux_trigger[FogSceneDetectionLuxTriggerModeDayLight].end            = 50.0f;
		lux_trigger[FogSceneDetectionLuxTriggerModeNormalLight].start       = 80.0f;
		lux_trigger[FogSceneDetectionLuxTriggerModeNormalLight].end         = 450.0f;
		lux_trigger[FogSceneDetectionLuxTriggerModeLowLight].start          = 510.0f;
		lux_trigger[FogSceneDetectionLuxTriggerModeLowLight].end            = 900.0f;
		lux_trigger[FogSceneDetectionLuxTriggerModeDayLight].p              = 0;
		lux_trigger[FogSceneDetectionLuxTriggerModeNormalLight].p           = 100;
		lux_trigger[FogSceneDetectionLuxTriggerModeLowLight].p              = 0;
		cct_trigger[FogSceneDetectionCCTTriggerModeLow].start               = 0.0f;
		cct_trigger[FogSceneDetectionCCTTriggerModeLow].end	                = 2000.0f;
		cct_trigger[FogSceneDetectionCCTTriggerModeIndoorAndOutdoor].start  = 2300.0f;
		cct_trigger[FogSceneDetectionCCTTriggerModeIndoorAndOutdoor].end    = 4500.0f;
		cct_trigger[FogSceneDetectionCCTTriggerModeOutdoorAndFog].start     = 5000.0f;
		cct_trigger[FogSceneDetectionCCTTriggerModeOutdoorAndFog].end       = 8500.0f;
		cct_trigger[FogSceneDetectionCCTTriggerModeHigh].start              = 10000.0f;
		cct_trigger[FogSceneDetectionCCTTriggerModeHigh].end                = 20000.0f;
		cct_trigger[FogSceneDetectionCCTTriggerModeLow].p                   = 0;
		cct_trigger[FogSceneDetectionCCTTriggerModeIndoorAndOutdoor].p      = 0;
		cct_trigger[FogSceneDetectionCCTTriggerModeOutdoorAndFog].p         = 100;
		cct_trigger[FogSceneDetectionCCTTriggerModeHigh].p                  = 0;
		drc_trigger[FogSceneDetectionDRCTriggerModeLowDRCGain].start        = 0.0f;
		drc_trigger[FogSceneDetectionDRCTriggerModeLowDRCGain].end          = 1.0f;
		drc_trigger[FogSceneDetectionDRCTriggerModeHighDRCGain].start       = 2.0f;
		drc_trigger[FogSceneDetectionDRCTriggerModeHighDRCGain].end         = 64.0f;
		drc_trigger[FogSceneDetectionDRCTriggerModeLowDRCGain].p            = 100;
		drc_trigger[FogSceneDetectionDRCTriggerModeHighDRCGain].p           = 0;
		hdr_trigger[FogSceneDetectionHDRTriggerModeLowHDRRatio].start       = 0.0f;
		hdr_trigger[FogSceneDetectionHDRTriggerModeLowHDRRatio].end         = 1.0f;
		hdr_trigger[FogSceneDetectionHDRTriggerModeHighHDRRatio].start      = 2.0f;
		hdr_trigger[FogSceneDetectionHDRTriggerModeHighHDRRatio].end        = 64.0f;
		hdr_trigger[FogSceneDetectionHDRTriggerModeLowHDRRatio].p           = 100;
		hdr_trigger[FogSceneDetectionHDRTriggerModeHighHDRRatio].p          = 0;
		**********************************************************************************/

		// CE advanced settings
		BOOL    guc_en;                                 // 0: disable GUC(Gain-up control), enable GUC                             (default: 1)
		BOOL    dcc_en;                                 // 0: disable DCC(dynamic contrast control), 1: enable DCC                 (default: 1)
		FLOAT   guc_str;                                // [0.0:5.0]: GUC strength on CE mode                                      (default: 1.0)
		FLOAT   dcc_dark_str;                           // [0.0:4.0]: DCC strength for dark region on CE mode                      (default: 1.0)
		FLOAT   dcc_bright_str;                         // [0.0:4.0]: DCC strength for bright region on CE mode                    (default: 1.0)
		CE_SCENE_DETECTION_PARAMS ce_trig_params;       // Define Gain triggers for ce scene detection                             (default settings are as below)
		/********************** CE trigger default settings ****************************
		gain_trigger[CESceneDetectionGainTriggerModeLowGain].start          = 1.0f;
		gain_trigger[CESceneDetectionGainTriggerModeLowGain].end            = 2.0f;
		gain_trigger[CESceneDetectionGainTriggerModeNormalGain].start       = 4.0f;
		gain_trigger[CESceneDetectionGainTriggerModeNormalGain].end         = 8.0f;
		gain_trigger[CESceneDetectionGainTriggerModeHighGain].start         = 16.0f;
		gain_trigger[CESceneDetectionGainTriggerModeHighGain].end           = 4096.0f;
		gain_trigger[CESceneDetectionGainTriggerModeLowGain].p              = 100;
		gain_trigger[CESceneDetectionGainTriggerModeNormalGain].p           = 80;
		gain_trigger[CESceneDetectionGainTriggerModeHighGain].p             = 0;
		**********************************************************************************/
	};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief DS4 structure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct DS4Data
    {
        UINT32                                                      ds4_format;
        UINT32                                                      ds4_stride;
        UINT32                                                      ds4_width;
        UINT32                                                      ds4_height;
        UINT8                                                       *buf;
        INT32                                                       fd;
        VOID*                                                       pDS4Data;
    };


	/// @briefDefog DS4 Formatt
	typedef enum DS4BufferFormat
	{
		DEFOG_DS4_FORMAT_BLOB = 0,
	} SHDRBAYERPATTERN;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Represents Defog/CE input parameter to the algorithm
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct DefogInputData
	{
		UINT32                                                      frameNum;                   ///< Frame number for current request
		UINT32                                                      DefogInputType;             ///< Notify input data type
		AECFrameControl*                                            AECStatsdata;               ///< AEC info
		AWBFrameControl*                                            AWBStatsdata;               ///< AWB info
		ParsedIHistStatsOutput*                                     pIHistStatsOutput;          ///< IHistStats with Y/R/G/B Hist Stats, Y Hist stats will be overrided by SHDR30/35 lib on SHDR mode.
		ParsedIHistStatsOutput*                                     pSefIHistStatsOutput;       ///< IHistStats of short exposure frame(SEF) from the other IFE for SHDR38.
		gamma_1_6_0::mod_gamma16_channel_dataType*                  pInterpolationGamma16Data;  ///< IFE/BPS GLUT (reserved for new feature. store Gamma info for YUV sensor)
		DS4Data*                                                    pDS4Data;                   ///< DS4 frame buffer pointer
		anr_1_0_0::mod_anr10_cct_dataType::cct_dataStruct*          pInterpolationANR10Data;    ///< IPE ANR      (reserved for new feature. noise improvement)
		ltm_1_3_0::ltm13_rgn_dataType*                              pInterpolationLTM13Data;    ///< IPE LTM      (reserved for new feature. image tone improvement)
		gamma15_cct_dataType::cct_dataStruct*                       pInterpolationGamma15Data;  ///< IPE Gamma
		cv_1_2_0::cv12_rgn_dataType*                                pInterpolationCV12Data;	    ///< IPE CV
		asf_3_0_0::asf30_rgn_dataType*                              pInterpolationASF30Data;    ///< IPE ASF      (reserved for new feature. detail improvement)

		/* More AEC/AWB info */
		BOOL                                                        isAECSettled;               ///< AWC settle flag
		BOOL                                                        isAWBSettled;               ///< AWB settle flag
		FLOAT                                                       frameRate;                  ///< current fps

		/* ADRC info */
		BOOL                                                        isADRCEnabled;
		FLOAT                                                       DRCGain;
		FLOAT                                                       ltm_percentage;             ///< Percentage of current DRC gain to be applied to LTM
		ltm_1_3_0::chromatix_ltm13_reserveType*                     pLTM13ReserveData;          ///< IPE LTM reverve data, used for ADRC LTM curve calculation

		/* SHDR3.0 info */
		BOOL                                                        isSHDREnabled;              ///< SHDR3 enable status
		BOOL                                                        isSHDRSettled;              ///< SHDR3 settle status
		FLOAT                                                       sHDRRatio;                  ///< SHDR3 ratio
		SoftwareVersionInfoType                                     sHDRVer;                    ///< SHDR3 version
		shdr3_tuning_tm_t*                                          pInterpolationTMData;       ///< SHDR3 GTM and LTM data
		shdr3_tuning_de_t*                                          pInterpolationDEData;       ///< SHDR3.0 DE   (reserved for new feature. detail enahancement on SHDR3.0 mode)
	};

    /// @brief DS4 frame buffer
    DS4Data                                                         m_DS4_buffer;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Represents Defog/CE output from the algorithm
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct DefogOutputData
	{
		UINT32                                                      version;                    ///< Defog Version(reserved for notification between differnt libs or apps)
		UINT32                                                      isSettled;                  ///< Defog Status
		UINT32                                                      fog_scene_probability;      ///< Fog Scene Probability
		anr_1_0_0::mod_anr10_cct_dataType::cct_dataStruct*          pProcessedANR10Data;        ///< IPE ANR      (reserved for new feature. noise improvement)
		ltm_1_3_0::ltm13_rgn_dataType*								pProcessedLTM13Data;	    ///< IPE LTM      (reserved for new feature. image tone improvement)
		gamma15_cct_dataType::cct_dataStruct*						pProcessedGamma15Data;	    ///< IPE Gamma
		cv_1_2_0::cv12_rgn_dataType*								pProcessedCV12Data;		    ///< IPE CV
		asf_3_0_0::asf30_rgn_dataType*                              pProcessedASF30Data;        ///< IPE ASF      (reserved for new feature. detail improvement)
		/* processed data to SHDR3.0 lib */
		//FLOAT                                                     shdrDarkBoost;              ///< Boost brightness on dark region (reserved for new feature. use SHDR3.0 for dark boost)
		shdr3_tuning_de_t*                                          pProcessedDEData;           ///< SHDR3.0 DE   (reserved for new feature. use SHDR3.0 for contrast enhancement)
	};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Represents Defog/CE input and output data for the algorithm
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct DefogData
    {
        DefogInputData                                              defogInputData;             ///< Defog input data
        DefogOutputData                                             defogOutputData;            ///< Defog output data
    };

    CAMX_VISIBILITY_PUBLIC StatsAlgorithmHandle DefogInitialize();
    CAMX_VISIBILITY_PUBLIC UINT32 DefogGetParams(StatsAlgorithmHandle handler, DefogConfigParams *pDefogConfigParams);
    CAMX_VISIBILITY_PUBLIC UINT32 DefogSetParams(StatsAlgorithmHandle handler, DefogConfigParams *pDefogConfigParams);
    CAMX_VISIBILITY_PUBLIC UINT32 DefogProcess(StatsAlgorithmHandle handler, DefogData *pDefogData);
    CAMX_VISIBILITY_PUBLIC UINT32 DefogUninitialize(StatsAlgorithmHandle handler);

#ifdef __cplusplus
}
#endif

#endif /* __DEFOG_H__ */
