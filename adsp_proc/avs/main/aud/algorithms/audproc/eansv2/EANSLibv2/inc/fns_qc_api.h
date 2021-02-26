#ifndef FNS_QC_API_H
#define FNS_QC_API_H
/*============================================================================
  @file FNS_api.h

  Public header file for FNS multi-mic algorithm.

Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/audproc/eansv2/EANSLibv2/inc/fns_qc_api.h#1 $    */
/*===========================================================================*
 * REVISION HISTORY                                                          *
 * when       who     what, where, why                                       *
 *---------- ------- --------------------------------------------------------*
 * 03/18/15   Ian    initial version
 *===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

//#include <stdio.h>
#include "AEEStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define MAX_NUM_CHAN        2

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
   FNS_FEATURE_WNR_ENABLE                            = 1 << 0,       // If 1, Enable WND & WNR feature
   FNS_FEATURE_FREQUENCY_PROCESS_ENABLE              = 1 << 1,       // If 1, Enable Frequency Process
   FNS_FEATURE_VAD_ENABLE                            = 1 << 2,       // If 1, Enable VAD
   FNS_FEATURE_VADNREF_ENABLE                        = 1 << 3,       // If 1, Enable VAD based non-stationary noise reference
   FNS_FEATURE_SMRMT_ENABLE                          = 1 << 4,       // If 1, Enable SMRMT
   FNS_FEATURE_POSTPROC_ENABLE                       = 1 << 5,       // If 1, Enable Posprocessing
   FNS_FEATURE_HISSING_MODE_ENABLE                   = 1 << 6,       // If 1, Enable Hiss mode
   FNS_FEATURE_LINKED_MODE_ENABLE                    = 1 << 7,       // If 1, Enable Linked mode
   FNS_FEATURE_DEBUG_NREF_ENABLE                     = 1 << 31,      // If 1, Enable monitoring noise reference

} FnsV2FeatureModeBits;

typedef enum
{
   FNSV2_STATUS_SUCCESS  = 0,                               /* Success */
   FNSV2_STATUS_CFG_INIT_MEMSIZE_ERROR = -1,                /* Frame size mismatch error for calibration re-init */
   FNSV2_STATUS_DATA_INIT_MEMSIZE_ERROR = -2,               /* Frame size mismatch error for calibration re-init */
   FNSV2_STATUS_SCRATCH_INIT_MEMSIZE_ERROR = -3,            /* Frame size mismatch error for calibration re-init */
   FNSV2_STATUS_NULL_POINTER = -4,
   FNSV2_STATUS_STATIC_CFG_ERROR = -5,
   FNSV2_STATUS_NUMCHAN_ERROR = -6,
   FNSV2_STATUS_INSUFFICIENT_INPUT_SAMPLES = -7,
   FNSV2_STATUS_INSUFFICIENT_MEM_ERROR = -8,
   FNSV2_STATUS_UNKNOWN_PID = -9,

} FNSV2_Retrun_Status_Type;

typedef enum
{
   FSV2_SR_NB = 8000,
   FSV2_SR_WB = 16000,
   FSV2_SR_SWB = 32000,
   FSV2_SR_FB = 48000,

} FNSV2_FS_Type;

typedef enum
{
   FNSV2_INPUT_TYPE_16BIT = 0,                              // 16 bit input data type
   FNSV2_INPUT_TYPE_24BIT = 1,                              // 24 bit input data type       
   FNSV2_INPUT_TYPE_32BIT = 2,                              // 32 bit input data type

} FNSV2_InputType;

typedef enum
{
   FNSV2_FFT_TYPE1 = 0,                                     /* TYPE1, low delay */
   FNSV2_FFT_TYPE2 = 1,                                     /* TYPE2, high delay */

} FNSV2_FTT_Type;

typedef enum
{
   FNSV2_LOW_RES = 0,                                       //Low resolutions processing
   FNSV2_HIGH_RES = 1,                                      //high resolution processing
   FNSV2_MUSIC = 2,                                         //Super high resolution processing

} FNSV2_FREQUENCY_RESOLUTION_Type;

/*----------------------------------------------------------------------------
 * Type Declarations for overall configuration and state data structures
 * -------------------------------------------------------------------------*/

/* Static Config Strcutre */
typedef struct
{
   uint32 featureMode;                          /* Feature Mode */
   uint32 fs;                                   /* Sampling rate */
   uint32 indataType;                           /* input data type */
   uint32 nFFTType;                             /* FFT op mode to determine delay long/low for process */
   uint32 resType;                              /* Resolution Opmode mode to determine to use compress subband grouping resolution */
   uint32 paramID;                              /* Payload interface ID */
   uint16 numChan;                              /* Number of input channels */  
   
} fnsV2StaticStructType; 

/* memory size structure return from algorithm to determinte needed memory size */
typedef struct
{
   uint32 memorySize;
   uint32 scratchSize;
   
} fnsV2SizeStructType;

/* a structure contains lib pointer that points the memory stores cfgs, internal data */
typedef struct
{
   void *libPtr;

} fnsV2LibStructType;

/**
  @brief get algorithm needed memory size.

  To query alogirthm needed memory size for static and scratch memory

  @param pCfg: [in] Pointer to static config
  @param pBuf: [in, out] Pointer to the structure specific needed memory for static and scratch.
  @return error code: 0 success, else errors.
 */
int32 fnsV2_getsize ( fnsV2StaticStructType *pCfg,
                      fnsV2SizeStructType *pBuf );

/**
  @brief get algorithm delay.

  To query alogirthm processing delay for the given configuration

  @param pCfg: [in] Pointer to static config
  @return delay: [out] Algorithmic delay in microseconds.
 */
uint32 fnsV2_get_delay ( fnsV2StaticStructType *pCfg );

/**
  @brief Initialize FNS algorithm

  Performs initialization of cfg, data structures for the
  FNS algorithm. A pointer to static memory is storage
  is passed for configuring the FNS static configuration
  structure.

  @param pLib: [in, out] Pointer to lib memory pointer
  @param pCfg: [in] Pointer to static config
  @param pMem: [in, out] Pointer to static memory
  @param totoalMemSize: [in] memory buffer size
  @return error code: 0 success, else errors.
 */
int32 fnsV2_init ( fnsV2LibStructType *pLib,
                   fnsV2StaticStructType *pCfg,
                   int8 *pMem,
                   uint32 totoalMemSize );

/**
  @brief Re-Initialize FNS algorithm

  Performs re-initialization of data structures for the
  FNS algorithm. 

  @param pLib: [in, out] Pointer to lib memory pointer
  @return error code: 0 success, else errors.
 */
int32 fnsV2_reset ( fnsV2LibStructType *pLib );


/**
  @brief query tuning parameters from lib

  To perform get tuning parameters from lib allow transfering tuning from lib to tools.

  @param pLib: [in] Pointer to lib memory pointer
  @param pParamBuf: [in,out] Pointer to static memory to store queried tuning interface.
  @param paramID: [in] ParamId
  @param memSize: [in] memory buffer size
  @param memSizeWritten: [in, out] memory size are copied.
  @return error code: 0 success, else errors.
 */
int32 fns_get_param ( fnsV2LibStructType *pLib,
                      int8* pParamBuf,
                      uint32 paramID,
                      uint32 memSize,
                      uint32 *memSizeWritten );

/**
  @brief set tuning parameters from lib

  To perform set tuning parameters allow transfering tuning from tools to lib.

  @param pLib: [in] Pointer to lib memory pointer
  @param pParamBuf: [in,out] Pointer to static memory to store queried tuning interface.
  @param paramID: [in] ParamId
  @param memSize: [in] max memory size
  @return error code: 0 success, else errors.
 */
int32 fns_set_param ( fnsV2LibStructType *pLib,
                      int8* pParamBuf,
                      uint32 paramID,
                      uint32 memSize );

/**
  @brief process input and processed output

  To process input data and return denoised output data.

  @param pLib: [in] Pointer to lib memory pointer
  @param pInL32: [in] Pointer to input data.
  @param nSample: [in] number of samples
  @param pOutL32: [in,out] Pointer to output data.
  @r
*/
int32 fns_process ( fnsV2LibStructType *pLib,
                    int8 *pIn[MAX_NUM_CHAN],
                    uint32 nSample,
                    int8 *pOut[MAX_NUM_CHAN] );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef FNS_QC_API_H */
