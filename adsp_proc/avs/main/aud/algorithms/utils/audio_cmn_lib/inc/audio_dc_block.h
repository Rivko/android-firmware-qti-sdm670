/*========================================================================*/
/**
@file audio_dc_block.h

This file contains DC blocking functions declarations.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The elite_audio_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*========================================================================
Copyright (c) 2010, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/audio_dc_block.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/27/09   dineshr  Initial revision                            
10/27/09   dineshr  Upgraded DC blocker coeffs for 64, 96 KHz.  
02/22/10   dineshr  Added block size to the input argument list.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */


#ifndef _DC_BLOCK_H_
#define _DC_BLOCK_H_

#include "AEEStdDef.h"

/*=============================================================================
      Function Declarations 
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup dsp_algorithms
@{ */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
  Number of Infinite Impulse Response (IIR) stages in the High Pass Filter 
  (HPF).
*/
#define MAX_DC_BLOCK_HPF_STAGES      2            

/*----------------------------------------------------------------------------
 * Type Declarations for overall configuration and state data structures
 * -------------------------------------------------------------------------*/

/** @brief Zero Hz blocking HPF configuration.
*/
typedef struct
{
  int16_t hpfDenC;         /**< HPF IIR denominator coefficients. */
} DCbHPFCfgType;

/** @brief Zero Hz blocking HPF data.
*/
typedef struct
{
   int32_t *numMem32;                   /**< Numerator memory. */
   int32_t *denMem32;                   /**< Denominator memory. */

} DCbHPFDataType;

/**
  Initializes the 0 Hz blocking HPF configuration and data structures.
 
  @param[in]     pCfg           Pointer to the 0 Hz blocking HPF configuration 
                                structure.
  @param[in,out] pData          Pointer to the 0 Hz blocking HPF data 
                                structure.
  @param[in]     sRate          Input sampling frequency in Hz.
  @param[in,out] numMemL32Q15   Pointer to the numerator memory in L32Q15.
  @param[in,out] denMemL32Q15   Pointer to the denominator memory in L32Q15.

  @return
  None.
  
  @dependencies
  None.
*/
void dc_block_init ( DCbHPFCfgType *pCfg,
			        DCbHPFDataType *pData,
					int32_t sRate,
        			int32_t *numMemL32Q15,
			        int32_t *denMemL32Q15);

/**
  Implements a 0 Hz blocking HPF.
 
  @param[in]     pInpL32Q15     Pointer to the input samples in L32Q15.
  @param[out]    pOutL32Q15     Pointer to the output samples in L32Q15.
  @param[in]     denPolesQ15    Pointer to the denominator poles in Q15.
  @param[in,out] numMemL32Q15   Pointer to the numerator memory in L32Q15.
  @param[in,out] denMemL32Q15   Pointer to the denominator memory in L32Q15.
  @param[in]     numStages      Number of single order IIR stages.
  @param[in]     frameSize      Frame size.

  @return
  None.
  
  @dependencies
  None.
*/
void dc_block( int32_t *pInpL32Q15,
	  		   int32_t *pOutL32Q15,
			   int16_t denPolesQ15,
			   int32_t *numMemL32Q15,
			   int32_t *denMemL32Q15,			 
			   int16_t numStages,
			   int16_t frameSize );

/** @} */ /* end_addtogroup dsp_algorithms */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _DC_BLOCK_H_ */
