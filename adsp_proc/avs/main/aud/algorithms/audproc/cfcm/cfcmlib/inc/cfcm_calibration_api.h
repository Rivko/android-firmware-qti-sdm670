#ifndef CFCM_CALIBRATION_API_H
#define CFCM_CALIBRATION_API_H
/*======================= COPYRIGHT NOTICE ==================================*
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
[*===========================================================================*/
/*===========================================================================*]
[* FILE NAME: cfcm_api.h			                                         *]
[* DESCRIPTION:                                                              *]
[*   Calibration (Public) API for Crossover Filtering and Channel Mixing.    *]
[*  REVISION HISTORY:                                                        *]
[*  when       who     what, where, why                                      *]
[*  ---------- ------- ----------------------------------------------------  *]
[*  2016/09/10 abdula  Initial version.                                      *]
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "AEEStdDef.h"
/*----------------------------------------------------------------------------
Constants
----------------------------------------------------------------------------*/

#define CFCM_IIR_NUM_COEFFS                 (10) 
#define CFCM_MODE_ENABLE                    (1)
#define CFCM_MODE_DISABLE                   (0)                   


/*=============================================================================
Configuration structure for CFCM parameters, values set by user 
=============================================================================*/

/* PARAM ID and associated structure to query library version */
/* access: get only */
#define CFCM_PARAM_LIB_VER                  (0)     
typedef uint64 cfcm_version_t;                       

/* PARAM ID and associated structure to enable/ disable CFCM */
/* access: get & set */
#define CFCM_PARAM_CONFIG_MODE              (1)     
typedef struct cfcm_config_mode_t {            
   uint32 mode;                                             // CFCM_MODE_DISABLE: disable module, 
                                                            // CFCM_MODE_ENABLE:  enable module
} cfcm_config_mode_t;

/*  CFCM run-time configuration is split into two PARAM IDs - CFCM_PARAM_CONFIG_FILTERBANK and      */
/*  CFCM_PARAM_CONFIG_VARS. The reason for splitting into two PARAM IDs is that when                */
/*  SET_PARAM for CFCM_PARAM_CONFIG_FILTERBANK is received, the filterbank                          */
/*  is reset in order to clear its internal state. This is not desirable when non-filterbank        */
/*  related parameters are configured. Such parameters have been placed in the structure            */
/*  CFCM_PARAM_CONFIG_VARS                                                                          */

/* PARAM ID and associated structure to configure crossover filterbank */
/* access: get & set */
#define CFCM_PARAM_CONFIG_FILTERBANK        (2)     
typedef struct cfcm_config_filterbank_t{            
    uint32              crossover_freq;                     // crossover frequency in Hz
    uint32              filterbank_order;                   // supported values: 3, 5
    uint32              num_allpass_stages[2];              // num_allpass_stages[0]: even filter stages; 
	                                                        // num_allpass_stages[1]: odd filter stages
	int32               iir_coeffs[CFCM_IIR_NUM_COEFFS];    // store order: 3-stage even, 2-stage odd.
} cfcm_config_filterbank_t;

/* PARAM ID and associated structure to configure non-filterbaank related variables */
/* access: get & set */
#define CFCM_PARAM_CONFIG_VARS              (3)     
typedef struct cfcm_config_vars_t {                 
    uint32              lf_mixing_switch;                   //    supported values: 0, 1
                                                            //    0: do not add mixed low freq content to receiver path (default)
                                                            //    1: add mixed low freq content to receiver path
    int32               rcvr_path_gain_q27;                 //    receiver path gain (scalar) in Q27 format.
    int32               spkr_path_gain_q27;                 //    speaker path gain (scalar) in Q27 format.  
                                                            //    supported gain values: scalar equivalent of -15dB to +15dB
                                                            //    ie, 23867662 to 754761750
}cfcm_config_vars_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CFCM_CALIBRATION_API_H */
