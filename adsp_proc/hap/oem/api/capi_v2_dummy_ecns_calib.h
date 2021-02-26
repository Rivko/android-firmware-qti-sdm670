#ifndef CAPI_V2_CUSTOM_DUMMY_ECNS_CALIB
#define CAPI_V2_CUSTOM_DUMMY_ECNS_CALIB

/* =========================================================================
   Copyright (c) 2015 QUALCOMM Technologies Incorporated.
   All rights reserved. Qualcomm Technologies Proprietary and Confidential.
   ========================================================================= */
#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"

/** @h2xml_title1           {DUMMY_ECNS}
    @h2xml_title_agile_rev  {DUMMY_ECNS}
    @h2xml_title_date       {July 13, 2017} */


/* ID of the Decimate module.

    This module supports the following parameter IDs:
    - #VOICE_PARAM_MOD_ENABLE
 */
#define CAPI_V2_DUMMY_ECNS   0x10027060
/**
    @h2xmlm_module       {"CAPI_V2_DUMMY_ECNS",
                          CAPI_V2_DUMMY_ECNS}

    @h2xmlm_description  {Voice dummy ecns module.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}

    @{                   <-- Start of the Module -->
   
    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @}                   <-- End of the Module -->*/

#endif /* CAPI_V2_DECIMATE_H_ */