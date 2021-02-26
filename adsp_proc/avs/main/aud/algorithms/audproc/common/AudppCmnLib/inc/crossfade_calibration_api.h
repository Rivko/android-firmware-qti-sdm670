#ifndef CROSSFADE_CALIBRATION_API_H
#define CROSSFADE_CALIBRATION_API_H

/*============================================================================
  FILE:          crossfade_calibration_api.h

  OVERVIEW:      This file has the configaration and data structures, API for cross fade algorithm.

  DEPENDENCIES:  None

Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/audproc/common/AudppCmnLib/inc/crossfade_calibration_api.h#1 $

  when       who     what, where, why
  ---------- ------- ---------------------------------------------------------
  2013-03-17 juihuaj 1.2.0 Initial revision.  
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


#include "AEEStdDef.h"



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


// this param id is for cross_fade_lib_ver_t
#define CROSS_FADE_PARAM_LIB_VER (0) // read only
typedef int32 cross_fade_lib_ver_t;  


// this param id is for cross_fade_mode_t
#define CROSS_FADE_PARAM_MODE (1) // read/write
typedef uint32 cross_fade_mode_t;	// 0 is no cross fade, 1 is cross fade


// this param id is for cross_fade_config_t
#define CROSS_FADE_PARAM_CONFIG (2) // read/write
typedef struct cross_fade_config_t
{
	uint32  converge_num_samples;      //  number of samples from old output
	uint32  total_period_msec;         //  T_convergence + T_crossfade
	
} cross_fade_config_t;




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef CROSSFADE_CALIBRATION_API_H */
