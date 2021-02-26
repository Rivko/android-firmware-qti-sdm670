// NOWHINE NC009 <- Shared file with system team so uses non-CamX file naming
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  iqcommondefs.h
/// @brief IQ Interface Input declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IQCOMMONDEFS_H
#define IQCOMMONDEFS_H

#include "abf_3_4_0.h"
#include "abf_4_0_0.h"
#include "anr_1_0_0.h"
#include "asf_3_0_0.h"
#include "bls_1_2_0.h"
#include "bpcbcc_5_0_0.h"
#include "cac_2_2_0.h"
#include "cc_1_2_0.h"
#include "cc_1_3_0.h"
#include "cst_1_2_0.h"
#include "cs_2_0_0.h"
#include "cv_1_2_0.h"
#include "demosaic_3_6_0.h"
#include "demux_1_3_0.h"
#include "hdr_2_0_0.h"
#include "ica_1_0_0.h"
#include "gamma_1_5_0.h"
#include "gamma_1_6_0.h"
#include "gic_3_0_0.h"
#include "gra_1_0_0.h"
#include "gtm_1_0_0.h"
#include "hdr_2_2_0.h"
#include "ica_1_0_0.h"
#include "iqsettingutil.h"
#include "hnr_1_0_0.h"
#include "linearization_3_3_0.h"
#include "linearization_3_4_0.h"
#include "lsc_3_4_0.h"
#include "ltm_1_3_0.h"
#include "pdpc_1_1_0.h"
#include "pdpc_2_0_0.h"
#include "pedestal_1_3_0.h"
#include "sce_1_1_0.h"
#include "tdl_1_0_0.h"
#include "tintless_2_0_0.h"
#include "tf_1_0_0.h"
#include "tmc_1_0_0.h"
#include "upscale_2_0_0.h"

#include "chitintlessinterface.h"

static const UINT32 H_GRID                              = 25;
static const UINT32 S_GRID                              = 16;
static const UINT32 GammaMask                           = 0x3FF; // last 10 bits are 1s
static const UINT32 MAX_NUM_PASSES                      = 4;

static const UINT32 ANRUNPACKSIZE                       = 15476;
static const UINT32 ANRCHROMATIX                        = 9552;
static const UINT32 ANRMaxNonLeafNode                   = 255;

static const UINT32 TFUNPACKSIZE                        = 5316;
static const UINT32 TFCHROMATIX                         = 4800;
static const UINT32 TFMaxNonLeafNode                    = 255;

static const UINT32 Linearization33MaxmiumNode          = 91; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2 + 24 * 2)
static const UINT32 Linearization33MaxmiumNonLeafNode   = 43; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2)
static const UINT32 Linearization33InterpolationLevel   = 6;

static const UINT32 ABF34MaxmiumNode                    = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2);
static const UINT32 ABF34MaxmiumNonLeafNode             = 7;  // (1 + 1 * 2 + 2 * 2);
static const UINT32 ABF34InterpolationLevel             = 4;

static const UINT32 BPCBCC50MaxmiumNode                 = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2);
static const UINT32 BPCBCC50MaxmiumNonLeafNode          = 7;  // (1 + 1 * 2 + 2 * 2);
static const UINT32 BPCBCC50InterpolationLevel          = 4;

static const UINT32 CC12MaxmiumNode                     = 91; // (1 + 1 * 2 + 2 * 2 +  4 * 3 + 12 * 2 + 24 * 2)
static const UINT32 CC12MaxmiumNonLeafNode              = 43; // (1 + 1 * 2 + 2 * 2 +  4 * 3 + 12 * 2)
static const UINT32 CC12InterpolationLevel              = 6;

static const UINT32 HDR20MaxmiumNode                    = 7;  // (1 + 1 * 2 + 2 * 2);
static const UINT32 HDR20MaxmiumNoNLeafNode             = 3;  // (1 + 1 * 2);
static const UINT32 HDR20InterpolationLevel             = 3;

static const UINT32 MAX_ADRC_LUT_KNEE_LENGTH            = 4;
static const UINT32 MAX_ADRC_LUT_COEF_SIZE              = 6;

static const UINT32 PDPC11MaxmiumNode                   = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2)
static const UINT32 PDPC11MaxmiumNonLeafNode            = 7;  // (1 + 1 * 2 + 2 * 2)
static const UINT32 PDPC11InterpolationLevel            = 4;

static const UINT32 TDL10MaxNode                        = 91; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2 + 24 * 2)
static const UINT32 TDL10MaxNonLeafNode                 = 43; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2)
static const UINT32 TDL10InterpolationLevel             = 6;

static const UINT32 CS20MaxmiumNode                     = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2)
static const UINT32 CS20MaxmiumNonLeafNode              = 7;  // (1 + 1 * 2 + 2 * 2)
static const UINT32 CS20InterpolationLevel              = 4;

static const UINT32 ASF30MaxNode                        = 31; // (1 + 1 * 2 + 2 * 2 + 4 * 2 + 8 * 2)
static const UINT32 ASF30MaxNoLeafNode                  = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2)
static const UINT32 ASF30InterpolationLevel             = 5;  // Core->TotalScaleRatio->DRCGAIN->HDRAEC->AEC

static const UINT32 BLS12MaxmiumNode                    = 91; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2 + 24 * 2);
static const UINT32 BLS12MaxmiumNonLeafNode             = 43; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2);
static const UINT32 BLS12InterpolationLevel             = 6;

static const UINT32 ABF40MaxmiumNode                    = 15; // (1 + 1 * 2 +  2 * 2 + 4 * 2);
static const UINT32 ABF40MaxmiumNoLeafNode              = 7;  // (1 + 1 * 2 +  2 * 2);
static const UINT32 ABF40InterpolationLevel             = 4;

static const UINT32 GIC30MaxmiumNode                    = 7;  // (1 + 1 * 2 + 2 * 2);
static const UINT32 GIC30MaxmiumNonLeafNode             = 3;  // (1 + 1 * 2);
static const UINT32 GIC30InterpolationLevel             = 3;

static const UINT32 HDR22MaxmiumNode                    = 7;  // (1 + 1 * 2 + 2 * 2)
static const UINT32 HDR22MaxmiumNonLeafNode             = 3;  // (1 + 1 * 2)
static const UINT32 HDR22InterpolationLevel             = 3;

static const UINT32 Linearization34MaxmiumNode          = 91; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2 + 24 * 2)
static const UINT32 Linearization34MaxmiumNonLeafNode   = 43; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2)
static const UINT32 Linearization34InterpolationLevel   = 6;

static const UINT32 PDPC20MaxNode                       = 15; // (1 + 1 * 2 + 2 * 2 +  4 * 2)
static const UINT32 PDPC20MaxNonLeafNode                = 7;  // (1 + 1 * 2 +  2 * 2)
static const UINT32 PDPC20InterpolationLevel            = 4;

static const UINT32 CAC22MaxNode                        = 7;  // (1 + 1 * 2 + 2 * 2);
static const UINT32 CAC22MaxNoLeafNode                  = 3;  // (1 + 1 * 2);
static const UINT32 CAC22InterpolationLevel             = 3;  // Core->TotalScaleRatio->AEC

static const UINT32 CC13MaxNode                         = 91; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2 + 24 * 2)
static const UINT32 CC13MaxNonLeafNode                  = 43; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2)
static const UINT32 CC13InterpolationLevel              = 6;

static const UINT32 CV12MaxNode                         = 91; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2 + 24 * 2)
static const UINT32 CV12MaxNonLeafNode                  = 43; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2)
static const UINT32 CV12InterpolationLevel              = 6;  // Core->DRCGain->HDRAEC->LED->AEC->CCT

static const UINT32 Demosaic36MaximumNode               = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2)
static const UINT32 Demosaic36MaximumNonLeafNode        = 7;  // (1 + 1 * 2 + 2 * 2)
static const UINT32 Demosaic36InterpolationLevel        = 4;

static const UINT32 Gamma15MaxNode                      = 91; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2 + 24 * 2)
static const UINT32 Gamma15MaxNonLeafNode               = 43; // (1 + 1 * 2 + 2 * 2 + 4 * 3 + 12 * 2)
static const UINT32 Gamma15InterpolationLevel           = 6;  // Core->DRCGain->HDRAEC->LED->AEC->CCT

static const UINT32 GRA10MaxNode                        = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2)
static const UINT32 GRA10MaxNonLeafNode                 = 7;  // (1 + 1 * 2 + 2 * 2)
static const UINT32 GRA10InterpolationLevel             = 4;

static const UINT32 GTM10MaxmiumNode                    = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2);
static const UINT32 GTM10MaxmiumNonLeafNode             = 7;  // (1 + 2 + 4);
static const UINT32 GTM10InterpolationLevel             = 4;

static const UINT32 HNR10MaxmiumNode                    = 31; // (1 + 1 * 2 + 2 * 2 + 4 * 2 + 8 * 2);
static const UINT32 HNR10MaxmiumNonLeafNode             = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2)
static const UINT32 HNR10InterpolationLevel             = 5;

static const UINT32 ICA10MaxNode                        = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2);
static const UINT32 ICA10MaxNoLeafNode                  = 7;  // (1 + 1 * 2 + 2 * 2);
static const UINT32 ICA10InterpolationLevel             = 4;  // Core->LensPosition->LensZoom->AEC

static const UINT32 LSC34MaxmiumNode                    = 159; // (1 + 1 * 2 + 2 * 2 + 4 * 2 + 8 * 3 + 24 * 2 + 48 * 2)
static const UINT32 LSC34MaxmiumNonLeafNode             = 63;  // (1 + 1 * 2 + 2 * 2 + 4 * 2 + 8 * 3 + 24 * 2)
static const UINT32 LSC34InterpolationLevel             = 7;

static const UINT32 LTM13MaxNode                        = 15; // (1 + 1 * 2 + 2 * 2 + 4 * 2)
static const UINT32 LTM13MaxNoLeafNode                  = 7;  // (1 + 1 * 2 + 2 * 2)
static const UINT32 LTM13InterpolationLevel             = 4;

static const UINT32 Pedestal13MaxmiumNode               = 63; // (1 + 1 * 2 + 2 * 2 + 4 * 2 + 8 * 2 + 16 * 2)
static const UINT32 Pedestal13MaxmiumNonLeafNode        = 31; // (1 + 1 * 2 + 2 * 2 + 4 * 2 + 8 * 2)
static const UINT32 Pedestal13InterpolationLevel        = 6;

static const UINT32 SCE11MaxNode                        = 7;  // (1 + 1 * 2 + 2 * 2)
static const UINT32 SCE11MaxNoLeafNode                  = 3;  // (1 + 1 * 2)
static const UINT32 SCE11InterpolationLevel             = 3;

static const UINT32 TF10MaxNode                         = 511; // (1 + 1 * 2 + 2 * 2 + 4 * 2 + 8 * 2 + 32 + 64 + 128 + 256)
static const UINT32 TF10MaxNonLeafNode                  = 255; // (1 + 1 * 2 + 2 * 2 + 4 * 2 + 8 * 2 + 16 * 2 + 32 * 2 + 64 * 2)
static const UINT32 TF10InterpolationLevel              = 9;   // Root->LensPos->LensZoom->PostScaleRatio->PreScaleRatio
                                                               // ->DRCGain->HDRAEC->AEC->CCT
static const UINT32 Upscale20MaxNode                    = 7;   // (1 + 1 * 2 + 2 * 2)
static const UINT32 Upscale20MaxNoLeafNode              = 3;   // (1 + 1 * 2)
static const UINT32 Upscale20InterpolationLevel         = 3;   // Core->TotalScaleRatio->AEC
static const UINT32 ANRIPEInterpolationSize             = (sizeof(anr_1_0_0::mod_anr10_cct_dataType::cct_dataStruct));
static const UINT32 LTMIPEInterpolationSize             = (sizeof(ltm_1_3_0::ltm13_rgn_dataType));
static const UINT32 ASFIPEInterpolationSize             = (sizeof(asf_3_0_0::asf30_rgn_dataType));
static const UINT32 CV12IPEInterpolationSize             =(sizeof(cv_1_2_0::cv12_rgn_dataType));
static const UINT32 GammaIFEInterpolationSize           = (sizeof(gamma_1_6_0::mod_gamma16_channel_dataType) * 3);

/// @brief ISP Use Case
enum UseCaseMode
{
    CONFIG_STILL = 0,     ///< Use Case STILL Image
    CONFIG_PREVIEW,       ///< Use Case Preview
    CONFIG_VIDEO,         ///< Use Case Video
};

/// @brief ISP Multi-Frame Config
enum ConfigMFMode
{
    MF_CONFIG_NONE = 0,       ///< MF Case: Config None
    MF_CONFIG_PREFILT,        ///< MF Case: Config Prefiltering
    MF_CONFIG_TEMPORAL,       ///< MF Case: Config Temporal Filtering
    MF_CONFIG_POSTPROCESS,    ///< MF Case: Config Post Processing
};

/// @brief: This enumerator is for gamma look up tables
enum GammaLUTChannel
{
    GammaLUTChannel0 = 0, ///< G channel
    GammaLUTChannel1 = 1, ///< B channel
    GammaLUTChannel2 = 2, ///< R channel
    GammaLUTMax      = 3, ///< Max LUTs
};

/// @brief Sensor PDAF Coordinates
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct PDPixelCoordinates
{
    UINT32  PDXCoordinate;  ///< PD pixel X coordinate
    UINT32  PDYCoordinate;  ///< PD pixel Y coordinate
};

/// @brief Input Data to all ISP IQ Module
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct ISPIQTriggerData
{
    FLOAT      AECexposureTime;      ///< Input trigger value:  exposure time ratio
    FLOAT      AECexposureGainRatio; ///< Input trigger value: exposure gain raio
    FLOAT      AECSensitivity;       ///< Input trigger value:  AEC sensitivity data
    FLOAT      AECGain;              ///< Input trigger value:  AEC Gain Value
    FLOAT      AECShortGain;         ///< Input trigger value:  AEC Short Gain Value
    FLOAT      AECLuxIndex;          ///< Input trigger value:  Lux index
    FLOAT      AWBleftGGainWB;       ///< Input trigger value:  AWB G channel gain
    FLOAT      AWBleftBGainWB;       ///< Input trigger value:  AWB B channel gain
    FLOAT      AWBleftRGainWB;       ///< Input trigger value:  AWB R channel gain
    FLOAT      AWBColorTemperature;  ///< Input Trigger value:  AWB CCT value
    FLOAT      DRCGain;              ///< Input trigger value:  DRCGain
    FLOAT      DRCGainDark;          ///< Input trigger value:  DRCGainDark
    FLOAT      lensPosition;         ///< Input trigger value:  Lens position
    FLOAT      lensZoom;             ///< Input trigger value:  Lens Zoom
    FLOAT      postScaleRatio;       ///< Input trigger value:  Post scale ratio
    FLOAT      preScaleRatio;        ///< Input trigger value:  Pre scale ratio
    FLOAT      totalScaleRatio;      ///< Input trigger value:  Total scale ratio
    UINT32     sensorImageWidth;     ///< Current Sensor Image Output Width
    UINT32     sensorImageHeight;    ///< Current Sensor Image Output Height
    UINT32     CAMIFWidth;           ///< Width of CAMIF Output
    UINT32     CAMIFHeight;          ///< Height of CAMIF Output
    UINT16     numberOfLED;          ///< Number of LED
    FLOAT      LEDSensitivity;       ///< LED Sensitivity
    FLOAT      LEDFirstEntryRatio;   ///< Dual LED First Entry Ratio
    UINT8      bayerPattern;         ///< Bayer pattern
    UINT32     sensorOffsetX;        ///< Current Sensor Image Output horizontal offset
    UINT32     sensorOffsetY;        ///< Current Sensor Image Output vertical offset
    UINT32     blackLevelOffset;     ///< Black level offset
    VOID*      pOEMTrigger[1];       ///< OEM Trigger Pointer Array, Place Holder
    VOID*      pLibInitialData;      ///< Customtized Library initial data block

    ParsedBHistStatsOutput*     pParsedBHISTStats;    ///< Pointer to parsed BHIST stats
    UINT32                      maxPipelineDelay;     ///< Maximum pipeline delay
};

/// @ Input FD Data to IQ modules
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct FDData
{
    UINT32                           numberOfFace;                 ///< Number of faces
    UINT32                           faceCenterX[MAX_FACE_NUM];    ///< Face cordinates X
    UINT32                           faceCenterY[MAX_FACE_NUM];    ///< Face coordinates Y
    UINT32                           faceRadius[MAX_FACE_NUM];     ///< Face Radius
};

/// @brief Input Data to all GTM and LTM Module for ADRC.
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct ADRCData
{
    UINT32 enable;                          ///< global enable
    UINT32 gtmEnable;                       ///< gtm enable
    UINT32 ltmEnable;                       ///< ltm enable
    FLOAT  kneeX[MAX_ADRC_LUT_KNEE_LENGTH]; ///< kneex
    FLOAT  kneeY[MAX_ADRC_LUT_KNEE_LENGTH]; ///< kneey
    FLOAT  coef[MAX_ADRC_LUT_COEF_SIZE];    ///< coef
    FLOAT  drcGainDark;                     ///< drcGainDark
    FLOAT  gtmPercentage;                   ///< GtmPercentage
    FLOAT  ltmPercentage;                   ///< LtmPercentage
};

struct _ImageDimension;
struct _WindowRegion;

/// @brief Structure to hold generic data buffer information
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct ISPIQTuningDataBuffer
{
    SIZE_T  size;       ///< Size of the buffer
    VOID*   pBuffer;    ///< Buffer pointer
};

/// @brief Input Data to BPCBCC50 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct BPCBCC50InputData
{
    bpcbcc_5_0_0::chromatix_bpcbcc50Type* pChromatix;          ///< Input Chromatix tunning data
    BOOL                                  symbolIDChanged;     ///< Symbol IOD changed, reload triggers
    UINT32                                blackLevelOffset;    ///< Black Level Offset
    FLOAT                                 DRCGain;             ///< Input trigger value:  DRCGain
    FLOAT                                 exposureTime;        ///< Input trigger value:  exposure time
    FLOAT                                 AECSensitivity;      ///< Input trigger value:  AEC sensitivity data
    FLOAT                                 luxIndex;            ///< Input trigger value:  Lux index
    FLOAT                                 realGain;            ///< Input trigger value:  Real Gain Value
    FLOAT                                 digitalGain;         ///< Input trigger value:  Digital Gain Value
    FLOAT                                 leftGGainWB;         ///< G channel gain to adjust white balance
    FLOAT                                 leftBGainWB;         ///< B channel gain to adjust white balance
    FLOAT                                 leftRGainWB;         ///< R channel gain to adjust white balance
    FLOAT                                 nonHdrMultFactor;    ///< Non HDR MultFactor
    VOID*                                 pInterpolationData;  ///< input memory for interpolation data
};

/// @brief Input Data to CC12 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct CC12InputData
{
    cc_1_2_0::chromatix_cc12Type* pChromatix;         ///< Chromatix data for CC12 module
    FLOAT                         DRCGain;            ///< Input trigger value:  DRCGain
    FLOAT                         exposureTime;       ///< Input trigger value:  exposure time
    FLOAT                         AECSensitivity;     ///< Input trigger value:  AEC sensitivity data
    FLOAT                         luxIndex;           ///< Input trigger value:  Lux index
    FLOAT                         AECGain;            ///< Input trigger value:  Aec Gain Value
    FLOAT                         colorTemperature;   ///< Input trigger value:  AWB colorTemperature
    FLOAT                         LEDTrigger;         ///< Input trigger value:  LED Index
    FLOAT                         LEDFirstEntryRatio; ///< Ratio of Dual LED
    UINT16                        numberOfLED;        ///< Number of LED
    VOID*                         pInterpolationData; ///< input memory for interpolation data
};

/// @brief Input Data to CC13 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct CC13InputData
{
    cc_1_3_0::chromatix_cc13Type* pChromatix;           ///< Chromatix data for CC12 module
    FLOAT                         DRCGain;              ///< Input trigger value:  drcGain
    FLOAT                         exposureTime;         ///< Input trigger value:  exposure time
    FLOAT                         AECSensitivity;       ///< Input trigger value:  AEC sensitivity data
    FLOAT                         luxIndex;             ///< Input trigger value:  Lux index
    FLOAT                         AECGain;              ///< Input trigger value:  AEC Gain Value
    FLOAT                         CCTTrigger;           ///< Input trigger value:  AWB colorTemperature
    FLOAT                         LEDTrigger;           ///< Input trigger value:  LED Index
    FLOAT                         LEDFirstEntryRatio;   ///< Ratio of Dual LED
    UINT16                        numberOfLED;          ///< Number of LED
    FLOAT                         effectsMatrix[3][3];  ///< Matrix to hold effects values
    INT32                         saturation;           ///< Saturation effect value
    VOID*                         pInterpolationData;   ///< input memory for interpolation data
};

/// @brief Input Data to CST12 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct CST12InputData
{
    cst_1_2_0::chromatix_cst12Type* pChromatix; ///< Input Chromatix tunning data
};

/// @brief Input Data to Linearization33 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct Linearization33InputData
{
    linearization_3_3_0::chromatix_linearization33Type*  pChromatix;         ///< Input Chromatix tunning data
    BOOL                                                 symbolIDChanged;    ///< Symbol IOD changed, reload triggers
    FLOAT                                                LEDTrigger;         ///< LED Value
    FLOAT                                                LEDFirstEntryRatio; ///< Ratio of Dual LED
    UINT16                                               numberOfLED;        ///< Number of LED
    FLOAT                                                CCTTrigger;         ///< CCT Value
    FLOAT                                                DRCGain;            ///< DRCGain
    FLOAT                                                exposureTime;       ///< exposure time
    FLOAT                                                AECSensitivity;     ///< AEC sensitivity data
    FLOAT                                                luxIndex;           ///< Lux index
    FLOAT                                                realGain;           ///< Linear Gain Value
    UINT16                                               lutBankSel;         ///< LUT bank need for this frame.
    BOOL                                                 pedestalEnable;     ///< pedestal is enabled or not
    UINT8                                                bayerPattern;       ///< Bayer Pattern
    VOID*                                                pInterpolationData; ///< input memory for interpolation data
};

/// @brief Input Data to BLS12 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct BLS12InputData
{
    bls_1_2_0::chromatix_bls12Type* pChromatix;         ///< Input Chromatix tunning data
    BOOL                            symbolIDChanged;    ///< Symbol IOD changed, reload triggers
    FLOAT                           DRCGain;            ///< Input trigger value:  DRCGain
    FLOAT                           exposureTime;       ///< Input trigger value:  exposure time
    FLOAT                           AECSensitivity;     ///< Input trigger value:  AEC sensitivity data
    FLOAT                           realGain;           ///< Input trigger value:  Digital Gain Value
    FLOAT                           LEDTrigger;         ///< Input trigger value:  Led Index
    FLOAT                           LEDFirstEntryRatio; ///< Ratio of Dual LED
    UINT16                          numberOfLED;        ///< Number of LED
    FLOAT                           CCTTrigger;         ///< Input trigger value:  CCT
    FLOAT                           digitalGain;        ///< Digital gain from Sensor
    FLOAT                           luxIndex;           ///< Input trigger value:  Lux index
    UINT8                           bayerPattern;       ///< Bayer Pattern
    VOID*                           pInterpolationData; ///< input memory for interpolation data
};

/// @brief LSC Calibration Table from EEPROM
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct LSC34CalibrationDataTable
{
    FLOAT* pRGain;    ///< R gain table
    FLOAT* pGRGain;   ///< GR gain table
    FLOAT* pGBGain;   ///< GB gain table
    FLOAT* pBGain;    ///< B gain table
};

/// @brief LSC stripe output
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct LSC34Stripe
{
    UINT32   lx_start_l;         ///< Start block x index, 6u
    UINT32   bx_start_l;         ///< Start subgrid x index within start block, 3u
    UINT32   bx_d1_l;            ///< x coordinate of top left pixel in start block/subgrid, 9u
};

/// @brief Input Data to LSC34 IQ Algorithem
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct LSC34InputData
{
    lsc_3_4_0::chromatix_lsc34Type*           pChromatix;           ///< Input Chromatix tunning data
    BOOL                                      symbolIDChanged;      ///< Symbol IOD changed, reload triggers
    UINT32                                    fullResWidth;         ///< Current Sensor Image Max Output Width
    UINT32                                    fullResHeight;        ///< Current Sensor Image Max Output Height
    UINT32                                    imageWidth;           ///< Current Sensor Image Output Width
    UINT32                                    imageHeight;          ///< Current Sensor Image Output Height
    UINT32                                    offsetX;              ///< Current Sensor Image Output Width
    UINT32                                    offsetY;              ///< Current Sensor Image Output Height
    UINT32                                    scalingFactor;        ///< Scaling factor from sensor
    FLOAT                                     DRCGain;              ///< Input trigger value:  DRCGain
    FLOAT                                     exposureTime;         ///< Input trigger value:  exposure time
    FLOAT                                     AECSensitivity;       ///< Input trigger value:  AEC sensitivity data
    FLOAT                                     luxIndex;             ///< Input trigger value:  Lux index
    FLOAT                                     realGain;             ///< Input trigger value:  real Gain Value
    FLOAT                                     LEDTrigger;           ///< Input trigger value:  LED Value
    FLOAT                                     LEDFirstEntryRatio;   ///< Ratio of Dual LED
    UINT16                                    numberOfLED;          ///< Number of LED
    FLOAT                                     lensPosTrigger;       ///< Input trigger value:  LensPosition Value
    FLOAT                                     CCTTrigger;           ///< Input trigger value:  CCT Value
    FLOAT                                     leftGGainWB;          ///< G channel gain to adjust white balance
    FLOAT                                     leftBGainWB;          ///< B channel gain to adjust white balance
    FLOAT                                     leftRGainWB;          ///< R channel gain to adjust white balance
    UINT16                                    bankSelect;           ///< Bank select for LUT table
    UINT8                                     numTables;            ///< Number of calibration tables
    LSC34CalibrationDataTable*                pCalibrationTable;    ///< pointer to the Calibration tables
    UINT32                                    numberOfEEPROMTable;  ///< number of tables in EEPROM
    BOOL                                      toCalibrate;          ///< Indicates we need to calibrate
    LSC34Stripe                               stripeOut;            ///< LSC changed values from striping lib
    BOOL                                      fetchSettingOnly;     ///< Flag to indicate to fetch settings for striping lib
    BOOL                                      enableCalibration;    ///< Indicates if calibration should be enabled or not
    tintless_2_0_0::chromatix_tintless20Type* pTintlessChromatix;   ///< Pointer to tintless Chromatix
    TintlessConfig*                           pTintlessConfig;      ///< Pointer to the Tintless Algo Configurations
    ParsedTintlessBGStatsOutput*              pTintlessStats;       ///< Pointer to Parsed Tintless data
    CHITintlessAlgorithm*                     pTintlessAlgo;        ///< Pointer to the Tintless Algo resource
    VOID*                                     pInterpolationData;   ///< input memory for interpolation data
};

/// @brief Input Data to Gamma16 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct Gamma16InputData
{
    gamma_1_6_0::chromatix_gamma16Type* pChromatix;         ///< Input Chromatix tunning data
    BOOL                                symbolIDChanged;    ///< Symbol ID changed, reload triggers
    FLOAT                               DRCGain;            ///< Input trigger value:  DRCGain
    FLOAT                               exposureTime;       ///< Input trigger value:  exposure time
    FLOAT                               AECSensitivity;     ///< Input trigger value:  AEC sensitivity data
    FLOAT                               LEDTrigger;         ///< Input trigger value:  Led Index
    FLOAT                               LEDFirstEntryRatio; ///< Ratio of Dual LED
    UINT16                              numberOfLED;        ///< Number of LED
    FLOAT                               luxIndex;           ///< Input trigger value:  Lux index
    FLOAT                               realGain;           ///< Input trigger value:  Real Gain Value
    FLOAT                               CCTTrigger;         ///< Input trigger value:  CCT
    FLOAT                               AECGain;            ///< Input trigger value:  Digital Gain Value
    UINT16                              LUTBankSel;         ///< LUT bank used for the frame
    VOID*                               pLibData;           ///< Pointer to the library specific data
    VOID*                               pInterpolationData; ///< input memory for interpolation data
};

/// @brief Input Data to ABF34 IQ Algorithem
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct ABF34InputData
{
    abf_3_4_0::chromatix_abf34Type*  pChromatix;          ///< Input Chromatix tunning data
    FLOAT                            DRCGain;             ///< Input trigger value:  DRCGain
    FLOAT                            exposureTime;        ///< Input trigger value:  exposure time
    FLOAT                            AECSensitivity;      ///< Input trigger value:  AEC sensitivity data
    FLOAT                            luxIndex;            ///< Input trigger value:  Lux index
    FLOAT                            realGain;            ///< Input trigger value:  Digital Gain Value
    UINT8                            LUTBankSel;          ///< Lut bank
    UINT32                           CAMIFWidth;          ///< Input CAMIF width
    UINT32                           CAMIFHeight;         ///< Input CAMIF Height
    UINT32                           blackResidueOffset;  ///< Black Level Offset
    UINT32                           sensorOffsetX;       ///< Current Sensor Image Output horizontal offset
    UINT32                           sensorOffsetY;       ///< Current Sensor Image Output vertical offset
    VOID*                            pInterpolationData;  ///< input memory for chromatix interpolation data
};

static const UINT32 MAX_CAMIF_PDAF_PIXELS   = 256;
static const UINT32 BPSPDPC20DMILengthDword = 64;

/// @brief Input Data to pdpc11 Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct PDPC11InputData
{
    pdpc_1_1_0::chromatix_pdpc11Type* pChromatix;                             ///< Input Chromatix tunning data
    SensorType                        sensorType;                             ///< Sensor type
    UINT32                            imageWidth;                             ///< Current Sensor Image Output Width
    UINT32                            imageHeight;                            ///< Current Sensor Image Output Height
    UINT32                            PDAFBlockWidth;                         ///< Current Sensor PDAF block Width
    UINT32                            PDAFBlockHeight;                        ///< Current Sensor PDAF block Height
    UINT16                            PDAFGlobaloffsetX;                      ///< Current Sensor PDAF horizontal offset
    UINT16                            PDAFGlobaloffsetY;                      ///< Current Sensor PDAF vertical offset
    FLOAT                             DRCGain;                                ///< Input trigger value:  DRCGain
    FLOAT                             exposureTime;                           ///< Input trigger value:  exposure time
    FLOAT                             AECSensitivity;                         ///< Input trigger value:  AEC sensitivity data
    FLOAT                             luxIndex;                               ///< Input trigger value:  Lux index
    FLOAT                             realGain;                               ///< Input trigger value:  AEC Gain Value
    UINT16                            blackLevelOffset;                       ///< Black Level Offset
    FLOAT                             leftGGainWB;                            ///< G channel gain to adjust white balance
    FLOAT                             leftBGainWB;                            ///< B channel gain to adjust white balance
    FLOAT                             leftRGainWB;                            ///< R channel gain to adjust white balance
    UINT8                             HDRMode;                                ///< ISP or Sensor HDR mode
    PDPixelCoordinates                PDAFPixelCoords[MAX_CAMIF_PDAF_PIXELS]; ///< Pixel coordinates
    UINT32                            PDAFPixelCount;                         ///< PD pixel count
    VOID*                             pInterpolationData;                     ///< input memory for interpolation data
};

/// @brief Input Data to GTM10 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct GTM10InputData
{
    gtm_1_0_0::chromatix_gtm10Type* pChromatix;             ///< Input Chromatix tunning data
    BOOL                            symbolIDChanged;        ///< Symbol IOD changed, reload triggers
    FLOAT                           DRCGain;                ///< Input trigger value:  drcGain
    FLOAT                           exposureTime;           ///< Input trigger value:  exposure time
    FLOAT                           AECSensitivity;         ///< Input trigger value:  AEC sensitivity data
    FLOAT                           AECSensitivityShort;    ///< Input trigger value:  AEC sensitivity data
    FLOAT                           realGain;               ///< Input trigger value:  Digital Gain Value
    FLOAT                           digitalGain;            ///< Digital gain from Sensor
    FLOAT                           luxIndex;               ///< Input trigger value:  Lux index
    UINT8                           LUTBankSel;             ///< LUT bank need for this frame
    /// @todo (CAMX-1843) Need to populate below parameters from Meta data/calculated values
    UINT32*                         pGRHist;                ///< BHIST GR stats
    FLOAT                           HDRExpTimeRatio;        ///< HDR exposure time ratio
    UINT16                          HDRMSBAlign;            ///< HDR MSB alignment
    UINT16                          HDRExpRatio;            ///< HDR exposure ratio
    UINT16                          HDRBlackIn;             ///< Black level input
    UINT32                          totalNumFrames;         ///< Total number of frames
    UINT32                          frameNum;               ///< Current frame number
    FLOAT                           keyPrevious;            ///< Key from previous frame
    FLOAT                           alphaPrevious;          ///< Alpha from previous frame
    INT32                           maxVHistPrevious;       ///< Maximum value in histogram from previous frame
    INT32                           minVHistPrevious;       ///< Minimum value in histogram from previous frame
    VOID*                           pLibData;               ///< Pointer to the library specific data
    ADRCData*                       pAdrcInputData;         ///< Pointer to the input data for adrc algo calculation
    UINT64                          BHistRequestID;         ///< RequestID to which the BHIST stats correspond to
    UINT32                          maxPipelineDelay;       ///< Maximum pipeline delay
    VOID*                           pInterpolationData;     ///< input memory for interpolation data
};

/// @brief Input Data to HNR10 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct HNR10InputData
{
    hnr_1_0_0::chromatix_hnr10Type* pChromatix;                ///< Input Chromatix tunning data
    FLOAT                           totalScaleRatio;           ///< Input trigger value:  TotalScaleRatio
    FLOAT                           DRCGain;                   ///< Input trigger value:  drcGain
    FLOAT                           exposureTime;              ///< Input trigger value:  exporsure time
    FLOAT                           AECSensitivity;            ///< Input trigger value:  AEC sensitivity data
    FLOAT                           luxIndex;                  ///< Input trigger value:  Lux index
    FLOAT                           realGain;                  ///< Input trigger value:  Digital Gain Value
    UINT32                          streamWidth;               ///< Stream Width
    UINT32                          streamHeight;              ///< Stream height
    UINT32                          horizontalOffset;          ///< Horizontal Offset
    UINT32                          verticalOffset;            ///< Vertical Offset
    struct FDData*                  pFDData;                   ///< Face data
    UINT8                           LUTBankSel;                ///< LUT bank need for this frame
    VOID*                           pInterpolationData;        ///< input memory for interpolation data
};

/// @brief Input Data to Demux13 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct Demux13InputData
{
    demux_1_3_0::chromatix_demux13Type* pChromatixInput;       ///< Input Chromatix tuning data
    UINT8                               bayerPattern;          ///< Bayer pattern
    UINT32                              demuxInConfigPeriod;   ///< Configration period
    UINT32                              demuxInConfigConfig;   ///< Determine config for even or odd channel
    FLOAT                               digitalGain;           ///< Digital Gain of in the input stream
    FLOAT                               stretchGainRed;        ///< Stretch Gain of Red Channel
    FLOAT                               stretchGainGreenEven;  ///< Stretch Gain of Green Channel (Even)
    FLOAT                               stretchGainGreenOdd;   ///< Stretch Gain of Green Channel (Odd)
    FLOAT                               stretchGainBlue;       ///< Stretch Gain of Blue Channel
    UINT32                              blackLevelOffset;      ///< Black Level Offset
};

/// @brief Input Data to Demosaic36 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct Demosaic36InputData
{
    demosaic_3_6_0::chromatix_demosaic36Type* pChromatix;           ///< Input Chromatix tuning data
    BOOL                                      symbolIDChanged;      ///< Symbol ID changed, reload triggers
    UINT32                                    imageWidth;           ///< Current Sensor Image Output Width
    UINT32                                    imageHeight;          ///< Current Sensor Image Output Height
    FLOAT                                     DRCGain;              ///< Input trigger value:  DRCGain
    FLOAT                                     exposureTime;         ///< Input trigger value:  exporsure time
    FLOAT                                     AECSensitivity;       ///< Input trigger value:  AEC sensitivity data
    FLOAT                                     luxIndex;             ///< Input trigger value:  Lux index
    FLOAT                                     AECGain;              ///< Input trigger value:  AEC Gain Value
    VOID*                                     pInterpolationData;   ///< input memory for interpolation data
};

/// @brief Input Data to GIC30 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct GIC30InputData
{
    gic_3_0_0::chromatix_gic30Type* pChromatix;             ///< Input Chromatix tunning data
    BOOL                            symbolIDChanged;        ///< Symbol ID changed, reload triggers
    FLOAT                           DRCGain;                ///< Input trigger value:  drcGain
    FLOAT                           exposureTimeTrigger;    ///< Input trigger value:  exporsure time
    FLOAT                           AECSensitivity;         ///< Input trigger value:  AEC sensitivity data
    FLOAT                           luxIndex;               ///< Input trigger value:  Lux index
    FLOAT                           realGain;               ///< Input trigger value:  Real Gain Value
    UINT8                           LUTBankSel;             ///< LUT bank used for the frame
    UINT32                          imageWidth;             ///< Current Sensor Image Output Width
    UINT32                          imageHeight;            ///< Current Sensor Image Output Height
    UINT32                          sensorOffsetX;          ///< Current Sensor Image Output horizontal offset
    UINT32                          sensorOffsetY;          ///< Current Sensor Image Output vertical offset
    VOID*                           pInterpolationData;     ///< input memory for interpolation data
};

/// @brief Input Data to HDR20 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct HDR20InputData
{
    hdr_2_0_0::chromatix_hdr20Type* pChromatix;           ///< Input Chromatix tunning data
    FLOAT                           exposureTime;         ///< Input trigger value:  exposure time
    FLOAT                           AECSensitivity;       ///< Input trigger value:  AEC sensitivity data
    FLOAT                           luxIndex;             ///< Input trigger value:  Lux index
    FLOAT                           AECGain;              ///< Input trigger value:  Digital Gain Value
    UINT32                          blackLevelOffset;     ///< Black Level Offset
    FLOAT                           leftGGainWB;          ///< G channel gain to adjust white balance
    FLOAT                           leftBGainWB;          ///< B channel gain to adjust white balance
    FLOAT                           leftRGainWB;          ///< R channel gain to adjust white balance
    UINT16                          RECONFirstField;      ///< HDR First field 0=T1, 1=T2
    UINT16                          ZZHDRMode;            ///< ZZHDR mode or IHDR mode
    UINT16                          ZZHDRPattern;         ///< 0~3, Bayer starting R1: 0=R1G1G2B1, 1=R1G1G2B2,
                                                          ///<                         2=R1G2G1B1, 3=R1G2G1B2
    UINT16                          ZZHDRFirstRBEXP;      ///< ZZHDR first R/B Exp,  0=T1, 1=T2
    UINT16                          HDRZigzagReconSel;    ///< ZZHDR ReconSel
    VOID*                           pInterpolationData;   ///< input memory for interpolation data
};

/// @brief Input Data to Pedestal13 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct Pedestal13InputData
{
    pedestal_1_3_0::chromatix_pedestal13Type* pChromatix;             ///< Input Chromatix tunning data
    BOOL                                      symbolIDChanged;        ///< Symbol ID changed, reload triggers
    FLOAT                                     DRCGain;                ///< Input trigger value:  drcGain
    FLOAT                                     exposureTimeTrigger;    ///< Input trigger value:  exporsure time
    FLOAT                                     AECSensitivity;         ///< Input trigger value:  AEC sensitivity data
    FLOAT                                     LEDTrigger;             ///< Input trigger value:  Led Index
    FLOAT                                     LEDFirstEntryRatio;     ///< Ratio of Dual LED
    UINT16                                    numberOfLED;            ///< Number of LED
    FLOAT                                     luxIndex;               ///< Input trigger value:  Lux index
    FLOAT                                     CCTTrigger;             ///< Input trigger value:  CCT
    FLOAT                                     AECGain;                ///< Input trigger value:  Digital Gain Value
    UINT16                                    LUTBankSel;             ///< LUT bank used for the frame
    UINT8                                     bayerPattern;           ///< Bayer Pattern
    UINT32                                    imageWidth;             ///< Current Sensor Image Output Width
    UINT32                                    imageHeight;            ///< Current Sensor Image Output Height
    VOID*                                     pInterpolationData;     ///< input memory for interpolation data
};

/// @brief Input Data to Linearization34 IQ Algorithm
/// NOWHINE NC004c: Share code with system team
struct Linearization34IQInput
{
    linearization_3_4_0::chromatix_linearization34Type*  pChromatix;             ///< Input Chromatix tunning data
    BOOL                                                 symbolIDChanged;        ///< Symbol IOD changed, reload triggers
    FLOAT                                                DRCGain;                ///< Input trigger value: DRCGain
    FLOAT                                                exposureTimeTrigger;    ///< Input trigger value: exporsure time
    FLOAT                                                AECSensitivity;         ///< Input trigger value: AEC sensitivity data
    FLOAT                                                LEDTrigger;             ///< Input trigger value: Led Index
    UINT16                                               numberOfLED;            ///< Number of LED
    FLOAT                                                LEDFirstEntryRatio;     ///< Ratio of Dual LED
    FLOAT                                                luxIndexTrigger;        ///< Input trigger value: Lux index
    FLOAT                                                CCTTrigger;             ///< Input trigger value: CCT
    FLOAT                                                realGain;               ///< Input trigger value: Real Gain Value
    UINT16                                               LUTBankSel;             ///< LUT bank need for this frame
    UINT8                                                bayerPattern;           ///< Bayer pattern
    BOOL                                                 pedestalEnable;         ///< pedestal is enabled or not
    VOID*                                                pInterpolationData;     ///< input memory for interpolation data
};

/// @brief Input Data to WhiteBalance12 IQ Algorithem
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct WB12InputData
{
    FLOAT leftGGainWB;  ///< G channel gain to adjust white balance
    FLOAT leftBGainWB;  ///< B channel gain to adjust white balance
    FLOAT leftRGainWB;  ///< R channel gain to adjust white balance
};

/// @brief Input Data to WhiteBalance13 IQ Algorithem
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct WB13InputData
{
    FLOAT leftGGainWB;  ///< G channel gain to adjust white balance
    FLOAT leftBGainWB;  ///< B channel gain to adjust white balance
    FLOAT leftRGainWB;  ///< R channel gain to adjust white balance
};

/// @brief Input Data to ABF40 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct ABF40InputData
{
    abf_4_0_0::chromatix_abf40Type* pChromatix;          ///< Input Chromatix tunning data
    FLOAT                           DRCGain;             ///< Input trigger value:  DRCGain
    FLOAT                           exposureTime;        ///< Input trigger value:  exporsure time
    FLOAT                           AECSensitivity;      ///< Input trigger value:  AEC sensitivity data
    FLOAT                           luxIndex;            ///< Input trigger value:  Lux index
    FLOAT                           realGain;            ///< Input trigger value:  Real Gain Value
    FLOAT                           leftGGainWB;         ///< G channel gain to adjust white balance
    FLOAT                           leftBGainWB;         ///< B channel gain to adjust white balance
    FLOAT                           leftRGainWB;         ///< R channel gain to adjust white balance
    UINT8                           LUTBankSel;          ///< Lut Bank
    BLS12InputData                  BLSData;             ///< Black Level is clubbed with ABF40 in BPS
    UINT32                          sensorOffsetX;       ///< Sensor X offset used for RNR
    UINT32                          sensorOffsetY;       ///< Sensor y offset used for RNR
    UINT32                          sensorWidth;         ///< Sensor output width
    UINT32                          sensorHeight;        ///< Sensor output Height
    VOID*                           pInterpolationData;  ///< input memory for interpolation data
};

/// @brief Input Data to CS20 IQ Algorithm
// NOWHINE NC004c: Share code with system team
struct CS20InputData
{
    cs_2_0_0::chromatix_cs20Type*  pChromatix;          ///< Input Chromatix tunning data
    FLOAT                          DRCGain;             ///< Input trigger value:  DRCGain
    FLOAT                          exposureTime;        ///< Input trigger value:  exposure time
    FLOAT                          AECSensitivity;      ///< Input trigger value:  AEC sensitivity data
    FLOAT                          luxIndex;            ///< Input trigger value:  Lux index
    FLOAT                          linearGain;          ///< Input trigger value:  Digital Gain Value
    VOID*                          pInterpolationData;  ///< input memory for interpolation data
};

/// @brief Input Data to BPSHDR22 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct HDR22InputData
{
    hdr_2_2_0::chromatix_hdr22Type* pChromatix;         ///< Input Chromatix tunning data
    FLOAT                           exposureTime;       ///< Input trigger value:  exposure time
    FLOAT                           AECSensitivity;     ///< Input trigger value:  AEC sensitivity data
    FLOAT                           luxIndex;           ///< Input trigger value:  Lux index
    FLOAT                           AECGain;            ///< Input trigger value:  Digital Gain Value
    FLOAT                           leftGGainWB;        ///< G channel gain to adjust white balance
    FLOAT                           leftBGainWB;        ///< B channel gain to adjust white balance
    FLOAT                           leftRGainWB;        ///< R channel gain to adjust white balance
    UINT32                          blackLevelOffset;   ///< Black Level Offset
    UINT16                          RECONFirstField;    ///< HDR First field 0=T1, 1=T2
    UINT16                          ZZHDRMode;          ///< ZZHDR mode or IHDR mode
    UINT16                          ZZHDRPattern;       ///< 0~3 Bayer starting R1: 0=R1G1G2B1, 1=R1G1G2B2,
                                                        ///<                        2=R1G2G1B1, 3=R1G2G1B2
    UINT16                          ZZHDRFirstRBEXP;    ///< ZZHDR first r/B Exp, 0=T1, 1=T2
    UINT8                           bayerPattern;       ///< Bayer pattern
    VOID*                           pInterpolationData; ///< input memory for interpolation data
};

/// @brief Input Data to PDPC20 IQ Algorithm
/// NOWHINE NC004c: Share code with system team
struct PDPC20IQInput
{
    pdpc_2_0_0::chromatix_pdpc20Type* pChromatix;                               ///< Input Chromatix tunning data
    SensorType                        sensorType;                               ///< Sensor type
    UINT32                            imageWidth;                               ///< Current Sensor Image Output Width
    UINT32                            imageHeight;                              ///< Current Sensor Image Output Height
    UINT32                            PDAFBlockWidth;                           ///< Current Sensor Image Output Width
    UINT32                            PDAFBlockHeight;                          ///< Current Sensor Image Output Height
    UINT16                            PDAFGlobaloffsetX;                        ///< Current Sensor Image Output Width
    UINT16                            PDAFGlobaloffsetY;                        ///< Current Sensor Image Output Height
    UINT16                            LUTBankSel;                               ///< LUT bank need for this frame.
    UINT32                            blackLevelOffset;                         ///< Black Level Offset
    FLOAT                             DRCGain;                                  ///< Input trigger value:  drcGain
    FLOAT                             exposureTime;                             ///< Input trigger value:  exporsure time
    FLOAT                             AECSensitivity;                           ///< Input trigger value:  AEC sensitivity data
    FLOAT                             luxIndex;                                 ///< Input trigger value:  Lux index
    FLOAT                             realGain;                                 ///< Input trigger value:  Digital Gain Value
    FLOAT                             digitalGain;                              ///< Input trigger value:  Digital Gain Value
    FLOAT                             leftGGainWB;                              ///< G channel gain to adjust white balance
    FLOAT                             leftBGainWB;                              ///< B channel gain to adjust white balance
    FLOAT                             leftRGainWB;                              ///< R channel gain to adjust white balance
    UINT8                             bayerPattern;                             ///< Bayer pattern
    UINT8                             HDRMode;                                  ///< ISP or Sensor HDR mode
    PDPixelCoordinates                PDAFPixelCoords[MAX_CAMIF_PDAF_PIXELS];   ///< Pixel coordinates
    UINT32                            PDAFPixelCount;                           ///< PD pixel count
    VOID*                             pInterpolationData;                       ///< input memory for interpolation data
};

/// @brief Input Data to TDL10 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct TDL10InputData
{
    tdl_1_0_0::chromatix_tdl10Type* pChromatix;          ///< Input Chromatix tunning data
    FLOAT                           DRCGain;             ///< Input trigger value:  drcGain
    FLOAT                           exposureTime;        ///< Input trigger value:  exposure time
    FLOAT                           AECSensitivity;      ///< Input trigger value:  AEC sensitivity data
    FLOAT                           LEDTrigger;          ///< Input trigger value:  LED Trigger
    FLOAT                           LEDFirstEntryRatio;  ///< Ratio of Dual LED
    UINT16                          numberOfLED;         ///< Number of LED
    FLOAT                           luxIndex;            ///< Input trigger value:  Lux index
    FLOAT                           CCTTrigger;          ///< Input trigger value:  CCT
    FLOAT                           realGain;            ///< Input trigger value:  real Gain Value
    FLOAT                           AECShortSensitivity; ///< AEC short sensitivity
    VOID*                           pInterpolationData;  ///< input memory for interpolation data
};

/// @brief Input Data ASF30 IQ Algorithm
// NOWHINE NC004c: Share code with system team
struct ASF30InputData
{
    asf_3_0_0::chromatix_asf30Type* pChromatix;                  ///< Input Chromatix tunning data
    FLOAT                           totalScaleRatio;             ///< Input trigger value:  Totalscale ratio
    FLOAT                           DRCGain;                     ///< Input trigger value:  DRC Gain
    FLOAT                           exposureTime;                ///< Input trigger value:  exporsure time
    FLOAT                           AECSensitivity;              ///< Input trigger value:  AEC sensitivity data
    FLOAT                           luxIndex;                    ///< Input trigger value:  Lux index
    FLOAT                           digitalGain;                 ///< Input trigger value:  Digital Gain Value
    UINT16                          edgeAlignEnable;             ///< edge alignment enable flag
    UINT16                          layer1Enable;                ///< layer1 enable flag
    UINT16                          layer2Enable;                ///< layer2 enable flag
    UINT16                          contrastEnable;              ///< contrast enable flag
    UINT16                          radialEnable;                ///< rnr enable flag
    UINT32                          chYStreamInWidth;            ///< Sensor output Channel Y stream in width
    UINT32                          chYStreamInHeight;           ///< Sensor output Channel Y stream in height
    struct FDData*                  pFDData;                      ///< Pointer to FD Data
    UINT16                          faceHorzOffset;              ///< face horizontal offset
    UINT16                          faceVertOffset;              ///< face vertical offset
    UINT8                           specialEffectAbsoluteEnable; ///< special effect; 0 for Emboss,
                                                                 ///< 1: for Sketch when negAbsY1 is 1
                                                                 ///< 1: for Neon when negAbsY1 is 0
    UINT8                           negateAbsoluteY1;            ///< special effect; see comments for spEffAbsen
    UINT8                           specialEffectEnable;         ///< special effect enable flag; 1 to enable special effect
    UINT8                           specialPercentage;           ///< special percentage
    UINT8                           smoothPercentage;            ///< smooth percentage
    UINT8                           nonZero[8];                  ///< non-zero flag; 0x1A90 for special effect
    UINT32                          sensorOffsetX;               ///< Current Sensor Image Output horizontal offset
    UINT32                          sensorOffsetY;               ///< Current Sensor Image Output vertical offset
    UINT32                          edgeMode;                    ///< sharpness control
    FLOAT                           sharpness;                   ///< sharpness strength
    VOID*                           pInterpolationData;          ///< input memory for interpolation data
    VOID*                           pWarpGeometriesOutput;       ///< Pointer to Warp Geometries Output Structure
    VOID*                           pDefogInterpolationData;     ///< Defog interpolation data
    VOID*                           pInternalData;               ///< internal data for ASF live tuning
};

/// @brief Input Data to CV12 IQ Algorithm
// NOWHINE NC004c: Share code with system team
struct CV12InputData
{
    cv_1_2_0::chromatix_cv12Type*       pChromatix;          ///< Input Chromatix tunning data
    FLOAT                               DRCGain;             ///< Input trigger value:  DRCGain
    FLOAT                               exposureTime;        ///< Input trigger value:  exporsure time
    FLOAT                               AECSensitivity;      ///< Input trigger value:  AEC sensitivity data
    FLOAT                               luxIndex;            ///< Input trigger value:  Lux index
    FLOAT                               LEDTrigger;;         ///< Input trigger value:  LED Trigger
    FLOAT                               LEDFirstEntryRatio;  ///< Ratio of Dual LED
    UINT16                              numberOfLED;         ///< Number of LED
    FLOAT                               colorTemperature;    ///< Input trigger value:  cct
    FLOAT                               digitalGain;         ///< Input trigger value:  Digital Gain Value
    VOID*                               pInterpolationData;  ///< input memory for interpolation data
    VOID*                               pDefogInterpolationData;  ///< input memory for defog interpolation data
};

/// @brief Input Data to Gamma15 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct Gamma15InputData
{
    gamma_1_5_0::chromatix_gamma15Type* pChromatix;                ///< Input Chromatix tunning data
    FLOAT                               DRCGain;                   ///< Input trigger value:  drcGain
    FLOAT                               exposureTime;              ///< Input trigger value:  exporsure time
    FLOAT                               AECSensitivity;            ///< Input trigger value:  AEC sensitivity data
    FLOAT                               luxIndex;                  ///< Input trigger value:  Lux index
    FLOAT                               LEDTrigger;;               ///< Input trigger value:  LED Trigger
    FLOAT                               LEDFirstEntryRatio;        ///< Ratio of Dual LED
    UINT16                              numberOfLED;               ///< Number of LED
    FLOAT                               colorTemperature;          ///< Input trigger value:  AWB colorTemperature
    FLOAT                               digitalGain;               ///< Input trigger value:  Digital Gain Value
    VOID*                               pLibData;                  ///< Pointer to the library specific data
    UINT8                               contrastLevel;             ///< contrast level pass down by APP
    BOOL                                isHDR10Mode;               ///< HDR10 mode check
    UseCaseMode                         usecaseMode;               ///< Use case Mode
    VOID*                               pInterpolationData;        ///< input memory for interpolation data
    VOID*                               pDefogInterpolationData;   ///< defog interpolation data
    VOID*                               gammaChannelData;          ///< Gamma channel data    
};

/// @brief Input Data to SCE11 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct SCE11InputData
{
    sce_1_1_0::chromatix_sce11Type* pChromatix;          ///< Input Chromatix tunning data
    FLOAT                           DRCGain;             ///< Input trigger value:  DRCGain
    FLOAT                           exposureTime;        ///< Input trigger value:  exporsure time
    FLOAT                           AECSensitivity;      ///< Input trigger value:  AEC sensitivity data
    FLOAT                           luxIndex;            ///< Input trigger value:  Lux index
    FLOAT                           realGain;            ///< Input trigger value:  Real Gain Value
    FLOAT                           leftGGainWB;         ///< G channel gain to adjust white balance
    FLOAT                           leftBGainWB;         ///< B channel gain to adjust white balance
    FLOAT                           leftRGainWB;         ///< R channel gain to adjust white balanc
    VOID*                           pInterpolationData;  ///< input memory for interpolation data
};

/// @brief Input Data to CAC22 IQ Algorithm
// NOWHINE NC004c: Share code with system team
struct CAC22InputData
{
    cac_2_2_0::chromatix_cac22Type* pChromatix;         ///< Input Chromatix tunning data
    FLOAT                           totalScaleRatio;    ///< Input trigger value:  Total Scale Ratio
    FLOAT                           luxIndex;           ///< Input trigger value:  Lux index
    FLOAT                           digitalGain;        ///< Input trigger value:  Digital Gain Value
    UINT32                          enableSNR;          ///< SNR enable flag
    UINT32                          resolution;         ///< 0 - 420 proessing 1 - 422 processing
    VOID*                           pInterpolationData; ///< input memory for interpolation data
};

/// @brief Input Data to TF10 IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct TF10InputData
{
    tf_1_0_0::chromatix_tf10Type* pChromatix;                   ///< Chromatix data for TF10 module
    FLOAT                         lensPosition;                 ///< Input trigger value:  Lens position
    FLOAT                         lensZoom;                     ///< Input trigger value:  Lens Zoom
    FLOAT                         postScaleRatio;               ///< Input trigger value:  Post scale ratio
    FLOAT                         preScaleRatio;                ///< Input trigger value:  Pre scale ratio
    FLOAT                         DRCGain;                      ///< Input trigger value:  drcGain
    FLOAT                         exposureTime;                 ///< Input trigger value:  exposure time
    FLOAT                         AECSensitivity;               ///< Input trigger value:  AEC sensitivity data
    FLOAT                         luxIndex;                     ///< Input trigger value:  Lux index
    FLOAT                         AECGain;                      ///< Input trigger value:  AEC Gain Value
    FLOAT                         CCTTrigger;                   ///< Input trigger value:  AWB colorTemperature
    FLOAT                         tnrIntensity;                 ///< Input trigger value:  TNR Strength
    FLOAT                         tnrMotionSensitivity;         ///< Input trigger value:  TNR Sensitivity
    FLOAT                         tnrBlendStrength;             ///< Input trigger value:  TNR BlendStrength
    UINT32                        fullPassIcaOutputFrameWidth;  ///< Full Pass ICA Output Frame width
    UINT32                        fullPassIcaOutputFrameHeight; ///< Full Pass ICA Ouput Frame Height
    UINT32                        maxUsedPasses;                ///< Maximum passes used in the request
    UINT32                        frameNum;                     ///< Current frame number
    UINT32                        mfnrFrameNum;                 ///< MFNR blending frame number
    UINT32                        numOfFrames;                  ///< Number of frames for MFNR use case
    UINT8                         hasTFRefInput;                ///< 1: has reference input, 0: no reference input
    UINT8                         isDigitalZoomEnabled;         ///< 1: digital zoom enable, 0: disable
    FLOAT                         upscalingFactorMFSR;          ///< MFSR upscaling factor
    UINT32                        digitalZoomStartX;            ///< Digital zoom horizontal start offset
    UINT32                        digitalZoomStartY;            ///< Digital zoom vertical start offset
    UINT8                         useCase;                      ///< UseCase: Video, Still
    UINT8                         configMF;                     ///< Multiframe Config: Prefilter or Temporal
    VOID*                         pWarpGeometriesOutput;        ///< Pointer to Warp Geometries Output Structure
    VOID*                         pRefinementParameters;        ///< Pointer to refinement parameters
    VOID*                         pTFParameters;                ///< Pointer to TF parameters
    VOID*                         pInterpolationData;           ///< input memory  for chromatix interpolation data
    VOID*                         pUnpackedfield;               ///< input memory Unpacked filed
    VOID*                         pNCChromatix;                 ///< Chromatix Input pointer
    BOOL                          resetReferenceInput;          ///< Reset reference input during failure
    BOOL                          bypassMode;                   ///< Bypass ANR and TF
    UINT32                        perspectiveConfidence;        ///< Transform confidence
    VOID*                         pLEFInterpolationData;        ///< input memory for chromatix interpolation data of LEF
    VOID*                         pAdaptiveTF;                  ///< Adaptive TF module
};

/// @brief Input Data ICA10 IQ Algorithm
// NOWHINE NC004c: Share code with system team
struct ICA10InputData
{
    ica_1_0_0::chromatix_ica10Type*  pChromatix;                   ///< Input Chromatix tunning data
    FLOAT                            luxIndex;                     ///< Input trigger value:  Lux index
    FLOAT                            digitalGain;                  ///< Input trigger value:  Digital Gain Value
    FLOAT                            lensZoomRatio;                ///< Lens Zoom ratio
    FLOAT                            lensPosition;                 ///< Lens Position
    UINT                             IPEPath;                      ///< IPE path indicating input or reference.
    UINT                             configMode;                   ///< ICA Configuration Mode
    UINT                             ICAIntransformConfidence;     ///< Transform Confidence
    UINT                             ICAReftransformConfidence;    ///< Transform Confidence
    VOID*                            pNCChromatix;                 ///< Chromatix pointer required for NCLib
    VOID*                            pNCRegData;                   ///< ICA Reg Pointer
    struct _ImageDimensions*         pImageDimensions;             ///< Input dimension
    struct _ImageDimensions*         pMarginDimensions;            ///< Margin related dimension
    struct _WindowRegion*            pZoomWindow;                  ///< Zoom Window
    struct _WindowRegion*            pIFEZoomWindow;               ///< IFE Zoom Window
    VOID*                            pCurrICAInData;               ///< ICA input current frame Data
    VOID*                            pCurrICARefData;              ///< ICA reference current frame Data
    VOID*                            pPrevICAInData;               ///< ICA input previous frame Data
    VOID*                            pCurWarpAssistData;           ///< Current Warp assist data
    VOID*                            pPrevWarpAssistData;          ///< Current Warp assist data
    VOID*                            pWarpGeomOut;                 ///< Warp Geometry Output buffer pointer
    VOID*                            pInterpolationParamters;      ///< Interpolation Parameters
    struct FDData*                   pFDData;                      ///< Pointer to FD Data
    BOOL                             mctfEis;                      ///< Enable mctf and eis together
    BOOL                             dumpICAOut;                   ///< Dump for ICA transforms
    UINT64                           frameNum;                     ///< frame number
    VOID*                            pInterpolationData;           ///< input memory for interpolation data
};

/// @brief Input Data to ANR10 IQ Algorithm
// NOWHINE NC004c: Share code with system team
struct ANR10InputData
{
    // Need to modify it as ANR Input data.
    anr_1_0_0::chromatix_anr10Type* pChromatix;                   ///< Chromatix data for ANR10 module
    FLOAT                           lensPosition;                 ///< Input trigger value:  Lens position
    FLOAT                           lensZoom;                     ///< Input trigger value:  Lens Zoom
    FLOAT                           postScaleRatio;               ///< Input trigger value:  Post scale ratio
    FLOAT                           preScaleRatio;                ///< Input trigger value:  Pre scale ratio
    FLOAT                           DRCGain;                      ///< Input trigger value:  drcGain
    FLOAT                           exposureTime;                 ///< Input trigger value:  exposure time
    FLOAT                           AECSensitivity;               ///< Input trigger value:  AEC sensitivity data
    FLOAT                           luxIndex;                     ///< Input trigger value:  Lux index
    FLOAT                           AECGain;                      ///< Input trigger value:  AEC Gain Value
    FLOAT                           CCTTrigger;                   ///< Input trigger value:  AWB colorTemperature
    FLOAT                           anrIntensity;                 ///< Input trigger value:  ANR Strength
    FLOAT                           anrMotionSensitivity;         ///< Input trigger value:  ANR Sensitivity
    VOID*                           pWarpGeometriesOutput;        ///< Pointer to Warp Geometries Output Structure
    UINT32                          numPasses;                    ///< Pass number
    UINT64                          frameNum;                     ///< Current frame number
    UINT32                          bitWidth;                     ///< format bitwidth
    UINT32                          opticalCenterX;               ///< Optical center X
    UINT32                          opticalCenterY;               ///< Optical Center Y
    struct FDData*                  pFDData;                      ///< Pointer to FD Data
    struct _ImageDimensions*        pImageDimensions;             ///< Input dimension
    struct _ImageDimensions*        pMarginDimensions;            ///< Margin related dimension
    VOID*                           pANRParameters;               ///< Pointer to ANR parameters
    VOID*                           pInterpolationData;           ///< input memory  for chromatix interpolation data
    VOID*                           pUnpackedfield;               ///< input memory Unpacked filed
    VOID*                           pNCChromatix;                 ///< Chromatix Input pointer
    VOID*                           pDefogANRdata;                ///< Defog ANR data
    VOID*                           pLEFInterpolationData;        ///< input memory for chromatix interpolation data of LEF
    VOID*                           pAdaptiveANR;                 ///< Adaptive ANR module
};

/// @brief Input Data to LTM13 IQ Algorithm
// NOWHINE NC004c: Share code with system team
struct LTM13InputData
{
    ltm_1_3_0::chromatix_ltm13Type* pChromatix;                    ///< Input Chromatix tunning data
    UINT32                          imageWidth;                    ///< Current Sensor Image Output Width
    UINT32                          imageHeight;                   ///< Current Sensor Image Output Height
    FLOAT                           DRCGain;                       ///< Input trigger value: DRCGain
    FLOAT                           DRCGainDark;                   ///< Input trigger value: DRCGainDark
    FLOAT                           exposureTime;                  ///< Input trigger value: exporsure time
    FLOAT                           AECSensitivity;                ///< Input trigger value: AEC sensitivity data
    FLOAT                           luxIndex;                      ///< Input trigger value: Lux index
    FLOAT                           exposureIndex;                 ///< Exposure Index, Same as Lux index for non ADRC cases
    FLOAT                           prevExposureIndex;             ///< Exposure Index of Previous Frame
    FLOAT                           realGain;                      ///< Input trigger value: Real Gain Value
    FLOAT                           ltmDarkBoostStrength;          ///< Input trigger value: Dark Boost Strength
    FLOAT                           ltmBrightSupressStrength;      ///< Input trigger value: Bright Supress Strength
    FLOAT                           ltmLceStrength;                ///< Input trigger value: Local Contrast Strength
    INT32*                          pGammaPrev;                    ///< Ptr to cached version of InverseGamma() input gamma
    INT32*                          pIGammaPrev;                   ///< Ptr to cached version of InverseGamma() output igamma
    FLOAT                           gammaOutput[65];               ///< Gamma ouput from gamma15 module
    ADRCData*                       pAdrcInputData;                ///< Pointer to the input data for adrc algo calculation.
    VOID*                           pInterpolationData;            ///< input memory for interpolation data
    VOID*                           pDefogInterpolationData;       ///< defog interpolation data
};

/// @brief Input Data to TMC10 IQ Algorithm
// NOWHINE NC004c: Share code with system team
struct TMC10InputData
{
    tmc_1_0_0::chromatix_tmc10Type* pChromatix;      ///< Input Chromatix tunning data
    FLOAT                           drcGain;         ///< Input trigger value:  DRCGain
    FLOAT                           drcGainDark;     ///< Input trigger value:  drcGainDark
    FLOAT                           realGain;        ///< Input trigger value:  Real Gain Value
    FLOAT                           luxIndex;        ///< Input trigger value:  Lux index
    BOOL                            adrcGTMEnable;   ///< GTM is enabled or not for ADRC
    BOOL                            adrcLTMEnable;   ///< LTM is enabled or not for ADRC
    ADRCData*                       pAdrcOutputData; ///< Pointer to the input data for adrc algo calculation.
};

/// @brief Input Data to Upscale20 and Chroma Upsample IQ Algorithm
// NOWHINE NC004c : Shared file with system team so uses non-CamX file naming
struct Upscale20InputData
{
    upscale_2_0_0::chromatix_upscale20Type* pChromatix;         ///< Input Chromatix tunning data
    FLOAT                                   totalScaleRatio;    ///< Input trigger value:  Total ScaleRatio
    FLOAT                                   luxIndex;           ///< Input trigger value:  Lux index
    FLOAT                                   AECGain;            ///< Input trigger value:  Digital Gain Value
    UINT16                                  cosited;            ///< 0: interstitial; 1: cosited
    UINT16                                  evenOdd;            ///< 0: even; 1: odd
    UINT16                                  enableHorizontal;   ///< 1: enable horizontal chroma upsample; 0: disable
    UINT16                                  enableVertical;     ///< 1: enable vertical chroma upsample; 0: disable
    UINT32                                  ch0InputWidth;      ///< Input image width for channel 0
    UINT32                                  ch0InputHeight;     ///< Input image height for channel 0
    UINT32                                  ch1InputWidth;      ///< Input image width for channel 1
    UINT32                                  ch1InputHeight;     ///< Input image height for channel 1
    UINT32                                  ch2InputWidth;      ///< Input image width for channel 2
    UINT32                                  ch2InputHeight;     ///< Input image height for channel 2
    VOID*                                   pInterpolationData; ///< input memory for interpolation data
};

/// @brief Input Data to GRA10 IQ Algorithm
/// NOWHINE NC004c: Share code with system team
struct GRA10IQInput
{
    gra_1_0_0::chromatix_gra10Type* pChromatix;         ///< Input Chromatix tunning data
    FLOAT                           colorTemperature;   ///< Input trigger value:  AWB color Temperature
    UINT32                          frameNum;           ///< Input Frame number for algorithm
    FLOAT                           luxIndex;           ///< Input trigger value:  Lux index
    FLOAT                           linearGain;         ///< Input trigger value:  linear Gain Value
    FLOAT                           preScaleRatio;      ///< Input trigger value:  Prescale ratio Mode
    VOID*                           pInterpolationData; ///< input memory for interpolation data
};

#endif // IQCOMMONDEFS_H
