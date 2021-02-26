#ifndef ENC_CNG_CALIB_H
#define ENC_CNG_CALIB_H
/*==============================================================================
  @file enc_cng_calib.h
  @brief This file contains ENC_CNG
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/11/17   shrani  Added h2xml comments.
 ========================================================================== */
#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"


/** @h2xml_title1           {VOICE_MODULE_ENC_CNG}
    @h2xml_title_agile_rev  {VOICE_MODULE_ENC_CNG}
    @h2xml_title_date       {April 11, 2017} */


#define VOICE_MODULE_ENC_CNG  0x00010F13
/**
    @h2xmlm_module       {"VOICE_MODULE_ENC_CNG",
                          VOICE_MODULE_ENC_CNG}
    @h2xmlm_displayName  {"TX VOICE ENC CNG"}
	@h2xmlm_description {Comfort Noise Generation algorithm in the voice stream on the Tx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

@h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter



   @}                   <-- End of the Module -->*/
#endif
