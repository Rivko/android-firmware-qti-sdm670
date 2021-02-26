#ifndef BEAMR_CALIB_H
#define BEAMR_CALIB_H
/*==============================================================================
  @file beamr_calib.h
  @brief This file contains BEAMR
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


/** @h2xml_title1           {VOICE_MODULE_BEAMR}
    @h2xml_title_agile_rev  {VOICE_MODULE_BEAMR}
    @h2xml_title_date       {March 13, 2017} */


#define VOICE_MODULE_BEAMR  0x00010F19
/**
    @h2xmlm_module       {"VOICE_MODULE_BEAMR",
                          VOICE_MODULE_BEAMR}
    @h2xmlm_displayName  {"VOICE BEAMR"}
	@h2xmlm_description {Blind Bandwidth extension for eAMR and AMR-NB vocoders on the Rx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n}
    @h2xmlm_ToolPolicy 				{NO_SUPPORT}
    @{                   <-- Start of the Module -->

   @h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

   @}                   <-- End of the Module -->*/
#endif
