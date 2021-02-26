#ifndef VOL_CALIB_H
#define VOL_CALIB_H
/*==============================================================================
  @file vol_calib.h
  @brief This file contains VOL
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/28/17   shrani  Added h2xml comments.
 ========================================================================== */
#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"


/** @h2xml_title1           {VOICE_MODULE_VOL}
    @h2xml_title_agile_rev  {VOICE_MODULE_VOL}
    @h2xml_title_date       {March 13, 2017} */



#define VOICE_PARAM_VOL 0x00010E12

#include "adsp_begin_pack.h"

typedef struct voice_param_vol_t voice_param_vol_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_VOL", VOICE_PARAM_VOL}
    @h2xmlp_description {Volume control algorithm on the Tx and Rx paths.\n
	Fixed size for this parameter \n
	Version 0 – 4 bytes\n} */

struct voice_param_vol_t {

    uint16_t Volume;
    /**< @h2xmle_description {Volume gain in Q13}
         @h2xmle_default     {0x2000}
         @h2xmle_range       {0..0xFFFF}

         @h2xmle_policy       {Basic} */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}
		 @h2xmle_readOnly    {true} */

}
#include "adsp_end_pack.h"
;

#define VOICE_PARAM_SOFT_VOL 0x00010E21

#include "adsp_begin_pack.h"

typedef struct voice_param_soft_vol_t voice_param_soft_vol_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_SOFT_VOL", VOICE_PARAM_SOFT_VOL}
    @h2xmlp_description {Soft-stepping volume control algorithm on the Tx and Rx paths.\n This parameter is used to change the volume linearly over ramp duration.\n
	Maximum size for this read-only parameter\n
	Version 0 – 8 bytes\n}
	@h2xmlp_toolPolicy	        {NO_SUPPORT} */

struct voice_param_soft_vol_t {

    uint32_t Volume;
    /**< @h2xmle_description {Value in Q28 format.}
         @h2xmle_default     {0x10000000}
         @h2xmle_range       {0..0xFFFFFFFF}
		 @h2xmle_policy       {Basic} */

    uint16_t GainRampingDuratio;
    /**< @h2xmle_description {Any integer between 0 to 5000 msec.This value is an integer only, which means that fractions of milliseconds are not supported.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..5000}

         @h2xmle_policy       {Basic} */

    uint16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..0xFFFF}
		 @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;



#define VOICE_MODULE_TX_VOL  0x00010EF4
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_VOL",
                          VOICE_MODULE_TX_VOL}
    @h2xmlm_displayName  {"TX VOICE VOL"}
	@h2xmlm_description {Volume control algorithm in the voice preprocessor on the Tx path.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_VOL\n
VOICE_PARAM_SOFT_VOL\n}

    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

@h2xml_Select					{voice_param_vol_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_soft_vol_t}
   @h2xmlm_InsertParameter

   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_RX_VOL  0x00010EF5
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_VOL",
                          VOICE_MODULE_RX_VOL}
    @h2xmlm_displayName  {"RX VOICE VOL"}
	@h2xmlm_description {Volume control algorithm in the voice preprocessor on the preprocessor on the Rx path.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_VOL\n
VOICE_PARAM_SOFT_VOL\n}

    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_vol_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_soft_vol_t}
    @h2xmlm_InsertParameter





   @}                   <-- End of the Module -->*/
#endif
