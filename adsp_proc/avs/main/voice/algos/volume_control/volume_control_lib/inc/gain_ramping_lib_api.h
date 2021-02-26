#ifndef Gain_Ramping_H
#define Gain_Ramping_H

/*============================================================================
  FILE:          CGainRampingLib_api.h

  OVERVIEW:      Entry point for CSim

  DEPENDENCIES:  None

Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/avs.adsp/2.8.5/voice/algos/volume_control/volume_control_lib/inc/gain_ramping_lib_api.h#1 $

  when       who     what, where, why
  ---------- ------- ---------------------------------------------------------
  2011-04-07 hesuh   Initial revision.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "AEEStdDef.h"
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define GAIN_0DB_Q28				268435456
#define ONE_Q31						2147483647
#define UNIT_Q28				    268435456
#define HALF_Q28                    134217728


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  @brief Algorithm features, corresponding to bits in features word in GainRampingCfgType
 */
typedef enum
{
    GAIN_RAMPING_DISABLED   =	0,     /* Enable the DC blocking HPF on the input */
    GAIN_RAMPING_ENABLED    =   1,     /* Enable the limiter processing */

} GainRampingFeaturesType;

typedef enum
{
    VOLUME_NOCHANGE     = 0,
    VOLUME_CHANGED      = 1,

} GainRampingVolumeChangedFlagType;


/*----------------------------------------------------------------------------
 * Default Calibration Constants Declaration
 * -------------------------------------------------------------------------*/
/**
  @brief Default values of tuning parameters and hardcoded parameters used in the algorithm
 */
typedef enum
{
	// Default parameters
    GAIN_RAMPING_FRAME_SIZE_DEFAULT	= 160,
    GAIN_RAMPING_MODE_DEFAULT		= 1,
    GAIN_RAMPING_VOLUME_CHANGE_FLAG	= VOLUME_NOCHANGE,
    GAIN_RAMPING_DURATION_DEFAULT	= 0, //4000,             //500ms for 8k sampling rate
    GAIN_RAMPING_TARGET_G_DEFAULT	= GAIN_0DB_Q28,

} GainRampingParamsDefault;


/*----------------------------------------------------------------------------
 * Type Declarations for overall configuration and state data structures
 * -------------------------------------------------------------------------*/

/**
  @brief configuration parameter structure containing tuning parameters
 */
typedef struct
{
    uint32 gainRampingTargetGUL32Q28;           // The new linear gain got from UI
    uint32 gainRampingLenInSamples;             // The duration of the gain ramping in samples
} GainRampingCfgType;


typedef struct
{
    // Q0 Sampling rate - Internally set value
//    int32 sampleRate;
	// UL32Q31 format step constant used to compute step size: stepConst = 1/LenSamples
    uint32 gainRampingStepConstUL32Q31;
    // UL32Q28 format step size internally computed each time the gain is changed:
    // Stepsize = step_size_norm*gainChange = StepConstUL32Q31*(newTargetGain - currGain)
    int32 gainRampingStepSizeL32Q28;
    // UL32Q28 format actual gain applied to current sample
    uint32 gainRampingCurrGUL32Q28;
    // Q0 index used to indicate how many samples the gain ramping has done
    int32 gainRampingIndex;
    // UL32Q28 format target gain for the gain ramping just finished, used to restore the gain to target gain after gain ramping is done
    uint32 gainRampingLastTargetGUL32Q28;
    uint32 gainRampingLenInSamples;             // The duration of the gain ramping in samples

} GainRampingDataType;


/**
  @brief Inintializing the default values constants used
  in Gain Ramping algorithm

  @param    paramValues: [in, out] Pointer to configuration structure storing calibration data
*/


void gain_ramping_init_default ( GainRampingCfgType *paramValues );


/**
  @brief Initialize Gain Ramping algorithm

  Performs initialization of data structures for the
  gain ramping. A pointer to configuration data strucure
  is passed for configuring the tuning parameters
  structure.

  @param samplingRate: [in] Input sampling rate
  @param paramVaues: [in] Pointer to tuning parameter default values
  @param pCfg: [in,out] Pointer to configuration data structure
  @param pData: [in,out] Pointer to internal data structure
 */
void gain_ramping_init ( GainRampingCfgType *paramValues,
						GainRampingCfgType *pCfg,
						GainRampingDataType *pData,
                        int32 samplingRate );

void gain_ramping_data_struct_init (    GainRampingCfgType  *pCfg,
                                        GainRampingDataType *pData,
                                        uint32 initGain);

void apply_gain_with_ramp (	GainRampingDataType *pData,
                            int16 *pInPtrL16,
                            int16 *pOutPtrL16,
                            int16 frameSize,
                            uint16 samplingRate);

void apply_gain_with_no_ramp (int16 *pInPtrL16,
                              int16 *pOutPtrL16,
                              int16 frameSize,
                              uint32 gain);




/**
  @brief Process input audio data with gain ramping algorithm

  @param pCfg: [in] Pointer to configuration data structure
  @param pData: [in,out] Pointer to internal data struture
  @param pInPtrL16 [in] Pointer to 16-bit Q15 input channel data
  @param pOutPtrL16: [out] Pointer to 16-bit Q15 output channel data

 */




#endif /* #ifndef AIG_API_H */
