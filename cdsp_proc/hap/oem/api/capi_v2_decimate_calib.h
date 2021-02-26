#ifndef CAPI_V2_DECIMATE_CALIB
#define CAPI_V2_DECIMATE_CALIB

/* =========================================================================
   Copyright (c) 2015 QUALCOMM Technologies Incorporated.
   All rights reserved. Qualcomm Technologies Proprietary and Confidential.
   ========================================================================= */
#include "mmdefs.h"
/** @h2xml_title1           {DECIMATE}
    @h2xml_title_agile_rev  {DECIMATE}
    @h2xml_title_date       {July 13, 2017} */


/* ID of the Decimate module.

    This module supports the following parameter IDs:
    - #CAPI_V2_PARAM_ID_DECIMATE_ENABLE
    - #CAPI_V2_PARAM_ID_UPDATE_DECIMATION_FACTOR
 */
#define CAPI_V2_DECIMATE   0x10012D20
/**
    @h2xmlm_module       {"CAPI_V2_DECIMATE",
                          CAPI_V2_DECIMATE}

    @h2xmlm_description  {ID of the DECIMATE module.\n

   This module supports the CAPI_V2_PARAM_ID_DECIMATE_ENABLE and CAPI_V2_PARAM_ID_UPDATE_DECIMATION_FACTOR.\n }

    @h2xmlm_toolPolicy              {Calibration; RTC}

    @{                   <-- Start of the Module -->
*/
/* ID of the decimate enable parameter used by CAPI_V2_DECIMATE. */
#define CAPI_V2_PARAM_ID_DECIMATE_ENABLE	0x10012D13

typedef struct decimate_filter_enable_cfg_t decimate_filter_enable_cfg_t;
/** @h2xmlp_parameter   {"CAPI_V2_PARAM_ID_DECIMATE_ENABLE", CAPI_V2_PARAM_ID_DECIMATE_ENABLE}
    @h2xmlp_description {Specifies the enablement of the decimation process.}  */

struct decimate_filter_enable_cfg_t {
    uint32_t enable_flag;
   /**< @h2xmle_description  {  Specifies the enablement of the decimation process.}
        @h2xmle_default      {0}
        @h2xmle_range        {0..0xFFFFFFFF}

        @h2xmle_policy       {Basic}.*/

};

/* ID of the decimation factor used by CAPI_V2_DECIMATE. */
#define CAPI_V2_PARAM_ID_UPDATE_DECIMATION_FACTOR	0x10012D15

typedef struct decimate_factor_cfg_t decimate_factor_t;
/** @h2xmlp_parameter   {"CAPI_V2_PARAM_ID_UPDATE_DECIMATION_FACTOR", CAPI_V2_PARAM_ID_UPDATE_DECIMATION_FACTOR}
    @h2xmlp_description {Specifies the decimation factor.}  */

struct decimate_factor_cfg_t {
   uint32_t decimation_factor;
   /**< @h2xmle_description  {  Specifies the decimation factor.}
        @h2xmle_default      {0}
        @h2xmle_range        {0..0xFFFFFFFF}

        @h2xmle_policy       {Basic}.*/

};
/** @}                   <-- End of the Module -->*/
#endif /* CAPI_V2_DECIMATE_H_ */