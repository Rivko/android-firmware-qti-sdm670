#ifndef CAPI_V2_ADSP_ERROR_CODE_CONVERTER_H
#define CAPI_V2_ADSP_ERROR_CODE_CONVERTER_H

/* ========================================================================*/
/**
 @file capi_v2_adsp_error_code_converter.h
 Utility header for converting between the CAPIv2 error codes and
 the aDSP error codes.
 */

/*======================================================================
 Copyright (c) 2014 QUALCOMM Technologies Incorporated.
 All rights reserved. Qualcomm Proprietary and Confidential.
 ====================================================================== */

/* =========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     ------------------------------------------------------
 05/15/14   dg      Created file.
 ========================================================================= */

#include "Elite_CAPI_V2_types.h"
#include "adsp_error_codes.h"

static const struct adsp_to_capi_v2_err_t {
	ADSPResult adsp_err_code;
	capi_v2_err_t capi_v2_err_code;
} adsp_to_capi_v2_err_lut[] = {
		{ ADSP_EOK, CAPI_V2_EOK },
		{ ADSP_EFAILED, CAPI_V2_EFAILED },
		{ ADSP_EBADPARAM, CAPI_V2_EBADPARAM },
		{ ADSP_EUNSUPPORTED, CAPI_V2_EUNSUPPORTED },
		{ ADSP_ENOMEMORY, CAPI_V2_ENOMEMORY },
		{ ADSP_ENEEDMORE, CAPI_V2_ENEEDMORE },
};

inline ADSPResult capi_v2_err_to_adsp_result(capi_v2_err_t capi_v2_err) {
	uint32_t i;

	if (CAPI_V2_SUCCEEDED(capi_v2_err))
	{
	   return ADSP_EOK;
	}

	for (i = 1; i < sizeof(adsp_to_capi_v2_err_lut)/sizeof(adsp_to_capi_v2_err_lut[0]); ++i)
	{
		if (CAPI_V2_IS_ERROR_CODE_SET(capi_v2_err, adsp_to_capi_v2_err_lut[i].capi_v2_err_code))
		{
			return adsp_to_capi_v2_err_lut[i].adsp_err_code;
		}
	}

	return ADSP_EFAILED;
}

inline capi_v2_err_t adsp_result_to_capi_v2_err(ADSPResult adsp_err) {
	uint32_t i;
	for (i = 0; i < sizeof(adsp_to_capi_v2_err_lut)/sizeof(adsp_to_capi_v2_err_lut[0]); ++i) {
		if (adsp_to_capi_v2_err_lut[i].adsp_err_code == adsp_err)
		{
			return adsp_to_capi_v2_err_lut[i].capi_v2_err_code;
		}
	}

	return CAPI_V2_EFAILED;
}

#endif /* #ifndef CAPI_V2_ADSP_ERROR_CODE_CONVERTER_H */
