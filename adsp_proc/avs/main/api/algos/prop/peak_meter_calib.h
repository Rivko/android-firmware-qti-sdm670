#ifndef PEAK_METER_CALIB_H
#define PEAK_METER_CALIB_H
/*==============================================================================
  @file peak_meter_calib.h
  @brief This file contains PEAK_METER API
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

#include "mmdefs.h"
/** @h2xml_title1           {PEAK METER}
    @h2xml_title_agile_rev  {PEAK METER}
    @h2xml_title_date       {February 2, 2017} */

/**
   @h2xmlx_xmlNumberFormat {int}
*/


#define AUDPROC_MODULE_ID_PEAK_METER            0x00010712
/** @h2xmlm_module       {"AUDPROC_MODULE_ID_PEAK_METER",
                          AUDPROC_MODULE_ID_PEAK_METER}

    @h2xmlm_displayName  {"AUDIO PEAKMETER"}
	@h2xmlm_description	 {ID of the Peak Meter module.

    This module supports the following parameter IDs:
     - #AUDPROC_PARAM_ID_PEAK_INTERVAL
     - #AUDPROC_PARAM_ID_PEAK_VALUES
     - #AUDPROC_PARAM_ID_ENABLE   }

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->  */

#define AUDPROC_PARAM_ID_PEAK_INTERVAL                  0x00010711




#define PEAKMETER_MAX_NUM_CHANNELS                  8

typedef struct audproc_peakmeter_interval_t audproc_peakmeter_interval_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_PEAK_INTERVAL",AUDPROC_PARAM_ID_PEAK_INTERVAL}
@h2xmlp_description	 {ID of the Peak Meter Configuration parameter used by AUDPROC_MODULE_ID_PEAK_METER. This parameter indicates the update interval of the peak meter.}
@h2xmlp_toolPolicy {NO_SUPPORT}
*/
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_PEAK_INTERVAL parameter used by
 the Peak Meter module.
 */
struct audproc_peakmeter_interval_t
{
   uint32_t sample_interval;
   /**< @h2xmle_description  {Update time interval in milliseconds of the peak meter.

        A value of zero disables the peak meter. In this case, the aDSP
        does not report valid peak meter values.

        A nonzero value indicates the peak meter log/reset frequency. For
        example, a value of 20 indicates an interval of 20 ms. This means
        a peak value is updated every 20 ms, and the newly calculated peak
        value replaces the existing value once every 20 ms.

        @note1hang The recommended sample interval is a multiple of 10 ms.}
        @h2xmle_range    {0..4294967295}
	    @h2xmle_visibility	{hide}  */
}
#include "adsp_end_pack.h"
;

#define AUDPROC_PARAM_ID_PEAK_VALUES                    0x00010710


typedef struct audproc_peakmeter_values_t audproc_peakmeter_values_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_PEAK_VALUES",AUDPROC_PARAM_ID_PEAK_VALUES}
@h2xmlp_description	 {ID of the Peak Meter Values parameters used by
    #AUDPROC_MODULE_ID_PEAK_METER. The aDSP returns the peak meter values.}
	@h2xmlp_toolPolicy {NO_SUPPORT} */
#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_PEAK_VALUES parameters used by the
 Peak Meter module.
 */
struct audproc_peakmeter_values_t
{
   uint32_t num_supported_channels;
   /**< @h2xmle_description {Number of channels for which the peak value was calculated. A value of zero indicates that the peak meter is disabled.}
	@h2xmle_range  {0..PEAKMETER_MAX_NUM_CHANNELS}
	@h2xmle_visibility	{hide}   */


   int16_t peak_values[PEAKMETER_MAX_NUM_CHANNELS];
   /**< @h2xmle_description {Array of peak values for each channel in the most recent window.}
	@h2xmle_range  {0..32767}
	@h2xmle_visibility	{hide}  */

}
#include "adsp_end_pack.h"
;

/**
   @}                   <-- End of the Module -->*/


#endif
