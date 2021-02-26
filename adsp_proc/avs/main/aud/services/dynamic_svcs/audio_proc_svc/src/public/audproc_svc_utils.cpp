
/*==========================================================================
ELite Source File

This file implements the buffer utility functions for the Audio Post
Processing Dynamic Service.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
18/07/16    HB      Created file.
=========================================================================== */

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "audproc_comdef.h"

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

/**
 * This function is to suspend hw modules.
 * @param[in] me
 * Pointer to the current instance structure
 *
 * @return int
 * Returns an ADSP error code indicating status.
 */
ADSPResult AudPP_SuspendPrepare(const ThisAudDynaPPSvc_t *me)
{
	ADSPResult result = ADSP_EOK;
	return result;
}

/**
 * This function is to resume hw modules.
 * @param[in] me
 * Pointer to the current instance structure
 *
 * @return int
 * Returns an ADSP error code indicating status.
 */
ADSPResult AudPP_RunPrepare(const ThisAudDynaPPSvc_t *me)
{
	ADSPResult result = ADSP_EOK;
	return result;
}

/**
 * Returns how many output buffers are needed based on the current PP
 * configuration.
 *   - Dependencies: The PP configuration structure must have been initialized.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 *   Pointer to the current instance structure.
 *
 * @return uint32_t
 * Returns the number of output buffers needed.
 */
uint32_t AudPP_NumOutputBuffersRequired(ThisAudDynaPPSvc_t *me)
{
   return 2; // Double buffering to enable concurrent processing, with minimum delay.
}



/**
 * Function to initialize specific modules.
 * @param[in, out] me
 * Pointer to the current instance structure.
 *
 * @return ADSPResult
 * Returns the error code indicating status.
 */
ADSPResult AudPP_InitializePPFeatures(ThisAudDynaPPSvc_t* me)
{
   ADSPResult result = ADSP_EOK;
   return result;
}

uint32_t topo_get_search_module_id(const uint32_t module_id)
{
   return module_id;
}

ADSPResult topo_get_virtualizer_module_id(uint32_t *module_id)
{
	return ADSP_ENOTEXIST;
}
ADSPResult topo_get_virtualizer_delay_param_id(uint32_t *param_id)
{
	return ADSP_ENOTEXIST;
}
ADSPResult topo_get_virtualizer_volume_ramp_param_id(uint32_t *param_id)
{
	return ADSP_ENOTEXIST;
}
