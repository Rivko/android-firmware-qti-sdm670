#ifndef Cross_Fade_API_H
#define Cross_Fade_API_H

/*============================================================================
  FILE:          CCrossFadeLib_api.h

  OVERVIEW:      This file has the configaration and data structures, API for cross fade algorithm.

  DEPENDENCIES:  None

                 Copyright (c) 2011 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
                 Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/avs.adsp/2.8.5/voice/algos/cross_fade/inc/voice_cross_fade_api.h#1 $

  when       who     what, where, why
  ---------- ------- ---------------------------------------------------------
  2011-05-05 sivab   Initial revision.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


#include "AEEStdDef.h"
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  @brief Algorithm features, corresponding to bits in features word in GainRampingCfgType
 */


/*----------------------------------------------------------------------------
 * Default Calibration Constants Declaration
 * -------------------------------------------------------------------------*/
/**
  @brief Default values of tuning parameters and hardcoded parameters used in the algorithm
 */

typedef enum
{
	// Default parameters
    STEADY_STATE	= 0,
	CONVERGENCE_STATE = 1,
    CROSSFADE_STATE		= 2,
} CrossFadeState;


/*----------------------------------------------------------------------------
 * Type Declarations for overall configuration and state data structures
 * -------------------------------------------------------------------------*/

/**
  @brief configuration parameter structure containing tuning parameters
 */
typedef struct CrossFadeCfgType
{
	int16  iConvergeNumSamples;     /// < number of samples for the new IIR filter to converge: 20 or 40 samples
	int16  iTotalPeriodMsec;        /// < T_convergence + T_crossfade: = 20 ms
} CrossFadeCfgType;

typedef struct CrossFadeDataType
{
	int16 crossFadeStepSize;
	int16 rampUpScaleFactor;
	int16 rampDownScaleFactor;
	int16 sampleCount;
	int16 state;
	int16 convergeNumOfSamples;
	int32 iTotalPeriodSamples;
} CrossFadeDataType;

/**
  @brief Inintializing the default values constants used
  in cross fade algorithm

  @param    pCfg: [out] Pointer to configuration structure storing calibration data
*/


void cross_fade_init_default ( CrossFadeCfgType *pCfg );


/**
  @brief Initialize cross fade algorithm

  Performs initialization of data structures for the
  cross fade. A pointer to configuration data strucure
  is passed for configuring the tuning parameters
  structure.

  @param pCfg: [in,out] Pointer to configuration data structure
  @param pData: [in,out] Pointer to internal data structure
  @param paramVaues: [in] Pointer to tuning parameter default values
  @param samplingRate: [in] Input sampling rate
 */
void cross_fade_init ( CrossFadeCfgType *pCfg,
						CrossFadeDataType *pData,
                        uint32 samplingRate );

/**
  @brief Process input audio data with cross fade algorithm

  @param pCfg: [in] Pointer to configuration data structure
  @param pData: [in,out] Pointer to internal data struture
  @param pInPtrL16 [in] Pointer to 16-bit Q15 input channel data
  @param pOutPtrL16: [out] Pointer to 16-bit Q15 output channel data

 */
int16 cross_fade_process (	CrossFadeCfgType *pCfg,
							CrossFadeDataType *pData,
                            int16 *pInPtr1L16,
							int16 *pInPtr2L16,
							int16 *pOutPtrL16,
                            int16 frameSize );


/**
	@brief getting size of the configatation structure of cross fade

 */
int16 get_size_cross_fade_cfg(void);

/**
	@brief getting size of the data structure of cross fade

 */
int16 get_size_cross_fade_data(void);


#endif /* #ifndef Cross_Fade_API_H */

