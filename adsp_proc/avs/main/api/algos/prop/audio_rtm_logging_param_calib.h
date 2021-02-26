#ifndef AUDIO_RTM_LOGGING_PARAM_CALIB_H
#define AUDIO_RTM_LOGGING_PARAM_CALIB_H

/*==============================================================================
  @file audio_rtm_logging_param_calib.h
  @brief This file contains audio rtm logging parameters
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 08/20/17   rishabht  Added h2xml comments.
 ========================================================================== */

#include "mmdefs.h"

/*==============================================================================
   Constants
==============================================================================*/

/**
    Param ID to enable logging for RTM, this PID is common for all the modules supporting RTM

    payload size -- 4 bytes
*/
#define RTM_LOGGING_PARAM_ENABLE                         0x00010EB1

/* Structure for setting/Querying  RTM logging. */
typedef struct audproc_rtm_logging_enable_payload_t audproc_rtm_logging_enable_payload_t;
/** @h2xmlp_parameter   {"RTM_LOGGING_PARAM_ENABLE", RTM_LOGGING_PARAM_ENABLE}
    @h2xmlp_description {To enable/disable RTM logging.}
    @h2xmlp_toolPolicy  {Calibration; RTC}
    @h2xmlp_readOnly    {false}*/

#include "adsp_begin_pack.h"

/* Payload of the RTM_LOGGING_PARAM_ENABLE parameter used by
 any Audio Processing module
 */
struct audproc_rtm_logging_enable_payload_t
{
   uint16_t enable_rtm_logging;
   /**< @h2xmle_description  {Param to enable RTM logging for any Audio module}
        @h2xmle_rangeList    {"Disable" = 0;
                              "Enable"  = 1}
        @h2xmle_default      {0} */

   uint16_t unique_id;
   /**< @h2xmle_description  {Param to query unique identifier of the module accross the sessions}
        @h2xmle_range       {0..65535}
        @h2xmle_default      {0} */
}
#include "adsp_end_pack.h"
;

/**
    Param ID to log PCM data, each module will log RTM data using this PID so that QACT can distinguish PCM bitstream data with other state bit stream data

    payload size -- (48 +  pcm_size) bytes
*/
#define RTM_PCM_LOGGING_PARAM                         0x00010EB2

/* Structure for Querying RTM PCM logging. */
typedef struct audproc_rtm_pcm_logging_payload_t audproc_rtm_pcm_logging_payload_t;
/** @h2xmlp_parameter   {"RTM_PCM_LOGGING_PARAM", RTM_PCM_LOGGING_PARAM}
    @h2xmlp_description {To query RTM PCM logging.\n
    Payload size of this parameter is dynamic and depends on pcm_size. The header size for this param is\n
Version 0 - 48 bytes \n\n}

    @h2xmlp_version     {0}
    @h2xmlp_toolPolicy  {NO_SUPPORT}

    @h2xmlp_readOnly    {true}*/

#include "adsp_begin_pack.h"

/* Payload of the RTM_PCM_LOGGING_PARAM parameter used by
 any Audio Processing module.
 */
struct audproc_rtm_pcm_logging_payload_t
{

    uint32_t                      pcm_size;
	/**< @h2xmle_description {total size of PCM data in Bytes}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0..4294967295}  */

    uint32_t                      log_tap_id;
	/**< @h2xmle_description {tap ID for PCM data}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0..4294967295}  */

    uint32_t                      sampling_rate;
	/**< @h2xmle_description {sampling rate of the PCM data}
         @h2xmle_default     {0x1f40}
         @h2xmle_range       {0..384000}  */

    uint16_t                      num_channels;
	/**< @h2xmle_description {Number of channels in the PCM stream getting logged}
         @h2xmle_default     {0x1}
         @h2xmle_range       {1..32}  */

    uint8_t                       pcm_width;
	/**< @h2xmle_description {pcm width of the PCM data}
         @h2xmle_rangeList    {"bitwidth 16bit"=16;
                               "bitwidth 24bit"=24;
                              "bitwidth 24bit"=32}
         @h2xmle_default     {0x10}  */

    uint8_t                       interleaved;
	/**< @h2xmle_description {specifies whether PCM data packing is interleaved or deinterleaved}
         @h2xmle_rangeList    {"INTERLEAVED"=0;
                               "DEINTERLEAVED_PACKED"=1;
                              "DEINTERLEAVED_UNPACKED"=2}
         @h2xmle_default     {0x0}  */

    uint8_t                       channel_map[32];
	/**< @h2xmle_description {Channel map of each channel in the PCM stream getting logged}
         @h2xmle_default     {0x1}
         @h2xmle_range       {1..63}  */

    int8_t                         payload[0];
	/**< @h2xmle_description {Array of PCM data}

		 @h2xmle_variableArraySize  {"pcm_size"}
		 @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;
#endif  // AUDIO_RTM_LOGGING_PARAM_CALIB_H


