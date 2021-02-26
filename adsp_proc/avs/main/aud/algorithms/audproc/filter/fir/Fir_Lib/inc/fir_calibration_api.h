#ifndef FIR_CALIBRATION_API_H
#define FIR_CALIBRATION_API_H
/*============================================================================
  @file CFIRCalibApi.h

  Public api for FIR.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/*============================================================================

 $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/audproc/filter/fir/Fir_Lib/inc/fir_calibration_api.h#1 $

  when       who     what, where, why
  ---------- ------- ---------------------------------------------------------
  2016-01-28 hesuh   Initial revision.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


#include "AEEStdDef.h"
#include "audio_dsp.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#define DATA_16BIT	16
#define DATA_32BIT	32
#define COEF_16BIT	16
#define COEF_32BIT	32
#define FIR_DISABLED 0
#define FIR_ENABLED 1
#define MAX_NUM_TAPS_DEFAULT 1024
#define DATA_WIDTH_DEFAULT 16


// param ID and the corresponding payload for lib
#define FIR_PARAM_GET_LIB_VER (0)			// read only
typedef int32	fir_lib_ver_t;				// lib version(major.minor.bug); (8bits.16bits.8bits)

// param ID and the corresponding payload for FIR feature mode
#define FIR_PARAM_FEATURE_MODE (1)			// read/write
typedef int32	fir_feature_mode_t;			// 1 is with FIR processing; 0 is no FIR filtering

// param ID and the corresponding payload for FIR filtering
#define FIR_PARAM_CONFIG (2)				// read/write
typedef struct fir_config_struct_t
{
	uint32	coeffs_ptr;						// stores the address of coeff buffer starting point
	uint32	coef_width;						// 16 :16-bits coef  or 32: 32-bits coef
	int16	coefQFactor;					// Qfactor of the coeffs
	int16	num_taps;						// Filter length

} fir_config_struct_t;


// param ID for reset(to reset internal history buffer)
// no payload needed for this ID
#define FIR_PARAM_RESET (3)		// write only


// param ID and the corresponding payload for group delay(in samples) for Linear-phase FIR only
#define FIR_PARAM_GET_LINEAR_PHASE_DELAY (4)// read only
typedef uint32	fir_delay_t;				// Q0 Delay in samples




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef FIR_CALIB_API_H */
