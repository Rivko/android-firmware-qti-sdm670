#ifndef COMPRESSED_LATENCY_CALIB_H
#define COMPRESSED_LATENCY_CALIB_H
/*==============================================================================
  @file compressed_latency_calib.h
  @brief This file contains COMPRESSED_LATENCY API
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/21/17   pag     Added h2xml comments.
 ========================================================================== */

/** @h2xml_title1           {Compressed Latency}
    @h2xml_title_agile_rev  {Compressed Latency}
    @h2xml_title_date       {February 2, 2017} */
/**
   @h2xmlx_xmlNumberFormat {int}
*/


#include "mmdefs.h"
#define AUDPROC_MODULE_ID_COMPRESSED_LATENCY            0x0001076E
/** @h2xmlm_module       {"AUDPROC_MODULE_ID_COMPRESSED_LATENCY",
                          AUDPROC_MODULE_ID_COMPRESSED_LATENCY}

    @h2xmlm_displayName  {"AUDIO COMPRESSED LATENCY"}
	@h2xmlm_description	 {ID of the Compressed Latency module.\n

    This module introduces non-negative delays in the compressed data path,
    achieved by introducing IEC61937 pause bursts for the specified delay.\n
    When the delay is decreased, data is dropped.

    Because the minimum pause burst duration that is possible depends on the
    format of the compressed data, the minimum delay possible also varies
    according to the format. For AC3/EAC3, the minimum is three samples
    duration.\n

    This module supports the AUDPROC_PARAM_ID_COMPRESSED_LATENCY parameter ID. \n}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
 */

#define AUDPROC_PARAM_ID_COMPRESSED_LATENCY             0x0001076F

/* Structure for latency parameter in compressed data paths. */
typedef struct audproc_compressed_latency_param_t audproc_compressed_latency_param_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_COMPRESSED_LATENCY", AUDPROC_PARAM_ID_COMPRESSED_LATENCY}
    @h2xmlp_description {Structure for latency parameter in compressed data paths.\n
	Payload of the AUDPROC_PARAM_ID_COMPRESSED_LATENCY parameter used by
 AUDPROC_MODULE_ID_COMPRESSED_LATENCY.}
	@h2xmlp_toolPolicy {NO_SUPPORT}
	*/

#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_COMPRESSED_LATENCY parameter used by
 AUDPROC_MODULE_ID_COMPRESSED_LATENCY.
 */
struct audproc_compressed_latency_param_t
{
   uint32_t delay_us;
   /**< @h2xmle_description  {Delay in microseconds.\n

        It must be greater than 0. If the value is 0, this module is
        disabled.\n

        The actual resolution of the delay value depends on the compressed
        data format.}
        @h2xmle_range    {0..100000}
        @h2xmle_visibility	{hide}                        */
}
#include "adsp_end_pack.h"
;

/**
   @}                   <-- End of the Module --> */

#endif
