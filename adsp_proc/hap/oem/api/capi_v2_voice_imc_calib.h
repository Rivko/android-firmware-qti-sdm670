#ifndef CAPI_V2_VOICE_IMC_CALIB
#define CAPI_V2_VOICE_IMC_CALIB

/* =========================================================================
   Copyright (c) 2015 QUALCOMM Technologies Incorporated.
   All rights reserved. Qualcomm Technologies Proprietary and Confidential.
   ========================================================================= */
#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"

/** @h2xml_title1           {VOICE_IMC}
    @h2xml_title_agile_rev  {VOICE_IMC}
    @h2xml_title_date       {July 13, 2017} */


/* ID of the Decimate module.

    This module supports the following parameter IDs:
    - #VOICE_PARAM_MOD_ENABLE
 */
#define VOICE_IMC_RX   0x10012D17
/**
    @h2xmlm_module       {"VOICE_IMC_RX",
                          VOICE_IMC_RX}

    @h2xmlm_description  {Voice imc module.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}

    @{                   <-- Start of the Module -->
   
    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @}                   <-- End of the Module -->*/

/* ID of the Decimate module.

    This module supports the following parameter IDs:
    - #VOICE_PARAM_MOD_ENABLE
 */
	#define VOICE_IMC_TX   0x10012D18
/**
    @h2xmlm_module       {"VOICE_IMC_TX",
                          VOICE_IMC_TX}

    @h2xmlm_description  {Voice imc module.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}

    @{                   <-- Start of the Module -->
   
    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @}                   <-- End of the Module -->*/
#endif /* CAPI_V2_VOICE_IMC_CALIB */