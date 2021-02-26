#ifndef TX_STREAM_LIMITER_CALIB_H
#define TX_STREAM_LIMITER_CALIB_H
/*==============================================================================
  @file tx_stream_limiter_calib.h
  @brief This file contains TX_STREAM_LIMITER
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


/** @h2xml_title1           {VOICE_MODULE_TX_STREAM_LIMITER}
    @h2xml_title_agile_rev  {VOICE_MODULE_TX_STREAM_LIMITER}
    @h2xml_title_date       {March 13, 2017} */


#define VOICE_PARAM_TX_STREAM_LIMITER 0x00010E33

#include "adsp_begin_pack.h"

typedef struct voice_param_tx_stream_limiter_t voice_param_tx_stream_limiter_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_TX_STREAM_LIMITER", VOICE_PARAM_TX_STREAM_LIMITER}
    @h2xmlp_description {Limiter used to control the dynamic range of signals.\n
Maximum array size for this parameter\n
Version 0 – 16 bytes}
@h2xmlp_version     {0} */

struct voice_param_tx_stream_limiter_t {

    uint16_t Version;
    /**< @h2xmle_description {Read only.}
         @h2xmle_default     {0}
         @h2xmle_readOnly      {true}
         @h2xmle_isVersion    {true}         */

    int16_t sLimMode;
    /**< @h2xmle_description {..}
         @h2xmle_default     {01}
         @h2xmle_policy      {Basic} */

    int16_t sLimMakeUpGain;
    /**< @h2xmle_description {..}
         @h2xmle_default     {256}
         @h2xmle_policy      {Basic} */

    int16_t sLimGc;
    /**< @h2xmle_description {..}
         @h2xmle_default     {29491}
         @h2xmle_policy      {Basic} */

    int16_t sLimDelay;
    /**< @h2xmle_description {..}
         @h2xmle_default     {98}
         @h2xmle_policy      {Basic} */

    int16_t sLimMaxWait;
    /**< @h2xmle_description {..}
         @h2xmle_default     {98}
         @h2xmle_policy      {Basic} */

    int32_t nLimThreshold;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1017}
         @h2xmle_policy      {Basic} */

}
#include "adsp_end_pack.h"
;


#define VOICE_MODULE_TX_STREAM_LIMITER  0x00010F15
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_STREAM_LIMITER",
                          VOICE_MODULE_TX_STREAM_LIMITER}
    @h2xmlm_displayName  {"TX VOICE STREAM LIMITER"}
	@h2xmlm_description {Limiter algorithm in the voice stream on the Tx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
 VOICE_PARAM_TX_STREAM_LIMITER\n\n}

    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{ voice_param_tx_stream_limiter_t}
    @h2xmlm_InsertParameter

    @}                   <-- End of the Module -->*/
#endif
