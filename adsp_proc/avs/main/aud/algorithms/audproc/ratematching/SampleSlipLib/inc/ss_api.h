#ifndef SS_API_H
#define SS_API_H

/*============================================================================
  @file ss_api.h

  Common public header file for the sample slipping solution.

Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/audproc/ratematching/SampleSlipLib/inc/ss_api.h#1 $    */
/*===========================================================================*
 * REVISION HISTORY                                                          *
 *when       who     what, where, why                                        *
 *---------- ------- --------------------------------------------------------*
 *06/18/10   dineshr  First version of the file                              *
 *10/14/11   ernanl   Extend the capability of supporting 32 bit input       *
 *===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AEEStdDef.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

#define DELAY_BUF_SIZE          8    // Size of delay buffer for holding past inputs

typedef enum
{
   SS_SUCCESS   = 0,
   SS_BADPARM   = 1,
} ss_error_resp;

/*----------------------------------------------------------------------------
 * Type Declarations for overall configuration and state data structures
 * -------------------------------------------------------------------------*/
typedef enum
{
   SAMP_SLIP_INTERP_16      =   4,     /* Cubic Spline Interpolation - Frame based sample slipping solution with 16bit operation */
   SAMP_SLIP_INTERP_32      =   5,     /* Cubic Spline Interpolation - Frame based sample slipping solution with 32bit operation */
} SampSlipMode;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

/**
  @brief get the Sample slipping data structure size.

  @param ssMode: [in] Operate bit rate per sample
  @param pCfgSize [out] Pointer to the size of sample slip cfg struct
  @param pSize: [out] Pointer to the size of sample slip data struct
 */
int32 ss_get_size ( const SampSlipMode ssMode,
                    uint32 *pCfgSize,
                    uint32 *pDataSize );

/**
  @brief Initializes the sample slipping configuration structure.

  @param ssCfg: [in] Pointer to sample slipping config structure
  @param frameSize: [in] Nominal frame size of the data
  @param multi_frames: [in] Number of multiple frames 
  @param ssMode: [in] Operate bit rate per sample
 */

int32 ss_cfg_init( int8 *pssCfg,
                   int16 frameSize,
                   int16 multi_frames,
                   SampSlipMode ssMode );

/**
  @brief Initializes the sample slipping data structure.

  @param ssCfg: [in] Pointer to sample slipping config structure
  @param ssData: [in] Pointer to sample slipping data structure  
 */

int32 ss_init( const int8 *pssCfg,
              int8 *pssData );


/**
  @brief Process one frame of sample slipping algorithm.

  @param ssCfg: [in] Pointer to sample slipping config structure
  @param pSSData: [in] Pointer to sample slipping data structure
  @param pInpL16: [in] Pointer to input frame data
  @param pOutL16: [out] Pointer to output frame data
  @param inp_frame: [in] Input frame size
  @param out_frame: [in] Output frame size
 */
int32 samp_slip_process ( const int8 *ssCfg,
                        int8 *pssData,
                        int8 *pInput,
                        int8 *pOutput,
                        int16 inp_frame,
                        int16 out_frame
                        );


#endif /* #ifndef SS_API_H */

