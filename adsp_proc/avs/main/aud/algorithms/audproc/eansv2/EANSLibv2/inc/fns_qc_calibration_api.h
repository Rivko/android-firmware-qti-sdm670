#ifndef FNS_QC_CALIBRATION_API_H
#define FNS_QC_CALIBRATION_API_H
/*============================================================================
  @file FNS_const.h

  Public header file containing FNS constants declaration.

Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/audproc/eansv2/EANSLibv2/inc/fns_qc_calibration_api.h#1 $

 EDIT HISTORY FOR MODULE

  when       who     what, where, why
  ---------- ------- ---------------------------------------------------------
  2015-05-27 ernanl  Initial revision.
                      
============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "AEEStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#define FNSV2_MAX_SUPPORT_NPAYLOAD_PIDS      FNSV2_GET_FRAMESIZE_PID
#define FNSV2_GET_FRAMESIZE_PID  0x8000
typedef struct
{
   int32 framesizes_perchannel;        //Mark
}fns_Param_QueryFrameSizeStructType;

#define FNSV2_MAX_SUPPORT_PAYLOAD_PIDS      FNSV2_PID_VOICE
#define FNSV1_PID          0x0000   //#FNSV1 Tuning interface
typedef struct
{
   uint16 fns_feature_mode;
   int16 fns_GT_InputGain;          // Q2.13 FNS input gain
   int16 fns_GT_OutputGain;         // Q2.13 FNS output gain
   int16 fns_PP2_TargetNS;          // Q7.8 Target NS limit in dB
   int16 fns_rsv_Salpha;            // Q3.12 stationary noise suppression factor
   int16 fns_PP2_Nalpha;            // Q3.12 Non-stationary noise suppression factor
   int16 fns_PP2_NalphaMax;         // Q3.12 Max Non-stationary noise suppression factor
   int16 fns_rsv_Ealpha;            // Q3.12 Excess noise suppression factor
   int16 fns_PP2_NSNRmax;           // Q7.8 Max SNR limit for non-stationary NS
   int16 fns_rsv_SNblock;           // Q0 Block length for stationary NS estimation
   int16 fns_vadnref_Ni;            // Q0 Block length for initial non-stationary NS estimation
   int16 fns_vadnref_NPscale;       // Q7.8 Speech level scale factor for non-stationary noise computation
   int16 fns_vadnref_NLambda;       // Q15 Smoothing factor for non-stationary noise averaging
   int16 fns_vadnref_NLambdaf;      // Q15 Higher smoothing factor for non-stationary noise averaging
   int16 fns_PP2_GsBias;            // Q7.8 SNR limit for gain calculation
   int16 fns_PP2_GsMax;             // Q7.8 SNR max for gain calculation
   int16 fns_rsv_SalphaHB;          // Q3.12 HB stationary noise suppression factor
   int16 fns_rsv_NalphaMaxHB;       // Q3.12 HB Max Non-stationary noise suppression factor
   int16 fns_rsv_EalphaHB;          // Q3.12 HB Excess noise suppression factor
   int16 fns_vadnref_NLambda0;      // Q15 Highest smoothing factor for non-stationary noise averaging
   int16 fns_avad_thresh;
   int16 fns_avad_pwrScale;
   int16 fns_avad_hangoverMax;
   int16 fns_avad_alphaSNR;
   int16 fns_avad_snrDiffMax;
   int16 fns_avad_snrDiffMin;
   int16 fns_avad_initLength;
   int16 fns_avad_maxVal;
   int16 fns_avad_initBound;
   int16 fns_avad_resetBound;
   int16 fns_avad_avarScale;
   int16 fns_avad_sub_Nc;
   int16 fns_avad_spowMin;
   int16 fns_rsv_GsFast;            // Q0.15, fast gain smooth factor
   int16 fns_rsv_GsMed;             // Q0.15, medium gain smooth factor  
   int16 fns_rsv_GsSlow;            // Q0.15, slow gain smooth factor
   int16 fns_rsv_SWB_Nalpha;
   int16 fns_rsv_SWB_Salpha;
   int16 fns_PP2_TargetNoiseFloor;  // Q0.15 Target noise floor
   int16 fns_PP2_SlopeNoiseFloor;   // Q0.15 Slope noise floor

} fnsV1_Param_StructType;

#define FNSV2_PID          0x0001
#define FNSV2_PID_VOICE    0x0002

typedef struct
{
   uint32 fns_feature_mode;
   uint16 fns_GT_delayMode;         // 0 low delay, 1 high delay
   uint16 fns_GT_resolutionMode;    // 0 low resolution, 1 high resolution, 2 music mode (full fft resolution)
   int16 fns_GT_InputGain;          // Q2.13 FNS input gain
   int16 fns_GT_OutputGain;         // Q2.13 FNS output gain
   int16 fns_vad_Mode;
   int16 fns_vadnref_Ni;            // Q0 Block length for initial non-stationary NS estimation
   int16 fns_vadnref_NPscale;       // Q7.8 Speech level scale factor for non-stationary noise computation
   int16 fns_vadnref_NLambda;       // Q15 Smoothing factor for non-stationary noise averaging
   int16 fns_vadnref_NLambdaf;      // Q15 Higher smoothing factor for non-stationary noise averaging
   int16 fns_vadnref_NLambda0;      // Q15 Highest smoothing factor for non-stationary noise averaging
   int16 fns_avad_thresh;
   int16 fns_avad_pwrScale;
   int16 fns_avad_hangoverMax;
   int16 fns_avad_alphaSNR;
   int16 fns_avad_snrDiffMax;
   int16 fns_avad_snrDiffMin;
   int16 fns_avad_initLength;
   int16 fns_avad_maxVal;
   int16 fns_avad_initBound;
   int16 fns_avad_resetBound;
   int16 fns_avad_avarScale;
   int16 fns_avad_sub_Nc;
   int16 fns_avad_spowMin;
   int16 fns_smrmt_Mode;
   int16 fns_smrmt_wlenInit;
   int16 fns_smrmt_wlenInc;
   int16 fns_smrmt_wlenMax;
   int16 fns_smrmt_band1I;
   int16 fns_smrmt_band2I;
   int16 fns_smrmt_alphaP;
   int16 fns_smrmt_alphaD;
   int16 fns_smrmt_overest_factL16Q12;
   int16 fns_smrmt_nlinitQL16;
   int16 fns_PP_Mode;
   int16 fns_PP1_alphaL16Q13;
   int16 fns_PP1_alpha_smLB_L16Q15;
   int16 fns_PP1_alpha_smNLB_L16Q15;
   int16 fns_PP1_minGainL16Q14;
   int16 fns_PP1_envRestFactL16Q12;
   int16 fns_PP2_TargetNS;          // Q7.8 Target NS limit in dB
   int16 fns_PP2_Nalpha;            // Q3.12 Non-stationary noise suppression factor
   int16 fns_PP2_NalphaMax;         // Q3.12 Max Non-stationary noise suppression factor
   int16 fns_PP2_NSNRmax;           // Q7.8 Max SNR limit for non-stationary NS
   int16 fns_PP2_GsBias;            // Q7.8 SNR limit for gain calculation
   int16 fns_PP2_GsMax;             // Q7.8 SNR max for gain calculation
   int16 fns_PP2_submode;
   int16 fns_PP2_envRestFactL16Q12;
   int16 fns_PP2_TargetNoiseFloor;  // Q0.15 Target noise floor
   int16 fns_PP2_SlopeNoiseFloor;   // Q0.15 Slope noise floor
   int16 reserved1;
   int32 fns_smrmt_thrB1;
   int32 fns_smrmt_thrB2;
   int32 fns_smrmt_thrB3;
   int32 fns_smrmt_nlinitL32;

} fnsV2_Param_StructType;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef FNS_QC_CALIBRATION_API_H */
