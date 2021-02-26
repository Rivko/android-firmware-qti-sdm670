#ifndef VISUALIZER_CALIB_H
#define VISUALIZER_CALIB_H
/*==============================================================================
  @file visualizer_calib.h
  @brief This file contains VISUALIZER API
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

/** @h2xml_title1           {Audio Visualizer}
    @h2xml_title_agile_rev  {Audio Visualizer}
    @h2xml_title_date       {February 8, 2017} */

#include  "mmdefs.h"


/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition
   Module library is independent of this number, it defined here for static
   loading purpose only */
#define AUDPROC_MODULE_ID_VISUALIZER                            0x000108C9
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_VISUALIZER",
                          AUDPROC_MODULE_ID_VISUALIZER}
    @h2xmlm_displayName  {"AUDIO VISUALIZER"}
    @h2xmlm_description  {ID of the Visualizer module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_VISUALIZER_ENABLE\n
    - #AUDPROC_PARAM_ID_VISUALIZER_CONFIG\n
    - #AUDPROC_PARAM_ID_VISUALIZER_CAPTURE_SIZE\n
    - #AUDPROC_PARAM_ID_VISUALIZER_SCALING_MODE\n
    - #AUDPROC_PARAM_ID_VISUALIZER_PCM_DATA\n
    - #AUDPROC_PARAM_ID_VISUALIZER_FFT_DATA\n
    - #AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT_MODE\n
    - #AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT\n
    - #AUDPROC_PARAM_ID_ENABLE\n

    All parameter IDs are device independent.\n}
    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
*/

/* ID of the Visualizer Enable parameter used by AUDPROC_MODULE_ID_VISUALIZER. */
#define   AUDPROC_PARAM_ID_VISUALIZER_ENABLE                 0x000108CA

/* Structure for the visualzier enable parameter */
typedef struct audproc_visualizer_enable_t audproc_visualizer_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VISUALIZER_ENABLE", AUDPROC_PARAM_ID_VISUALIZER_ENABLE}
    @h2xmlp_description {Enables the Visualizer module}  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VISUALIZER_ENABLE parameter used by the Visualizer module. */

struct audproc_visualizer_enable_t
{
   uint32_t enable_flag;
   /**< @h2xmle_description  { Specifies whether visualizer is enabled. }
         @h2xmle_rangeList    {"Disable"= 0;
                              "Enable"=1}  */
}
#include "adsp_end_pack.h"
;

/* ID of the Visualizer Configuration parameter used by AUDPROC_MODULE_ID_VISUALIZER. */
#define   AUDPROC_PARAM_ID_VISUALIZER_CONFIG                 0x000108CB

/* Structure for the visualzier configuration parameter */
typedef struct audproc_visualizer_config_t audproc_visualizer_config_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VISUALIZER_CONFIG", AUDPROC_PARAM_ID_VISUALIZER_CONFIG}
    @h2xmlp_description {Configures the Visualizer module}  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VISUALIZER_CONFIG parameter used by the Visualizer module. */

struct audproc_visualizer_config_t
{
   uint32_t latency_ms;
   /**< @h2xmle_description  { Specifies the latency of the visualizer in milliseconds.\n
   The latency is greater than or equal to 0.\n The maximum allowed value of latency depends
   on the capture size and audio sampling rate:\n
   ((8192 - capture_size) / sampling_rate) x 1000 \n
   The capture size is set by \n
   AUDPROC_PARAM_ID_VISUALIZER_CAPTURE_SIZE.\n } */

}
#include "adsp_end_pack.h"
;

/* ID of the Visualizer Capture Size parameter used by AUDPROC_MODULE_ID_VISUALIZER. */

#define   AUDPROC_PARAM_ID_VISUALIZER_CAPTURE_SIZE           0x000108CC

/* Structure for the visualzier capture size parameter */
typedef struct audproc_visualizer_capture_size_t audproc_visualizer_capture_size_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VISUALIZER_CAPTURE_SIZE", AUDPROC_PARAM_ID_VISUALIZER_CAPTURE_SIZE}
    @h2xmlp_description {Specifies the visualizer input data capture window length for FFT analysis }  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VISUALIZER_CAPTURE_SIZE parameter used by the Visualizer module. */

struct audproc_visualizer_capture_size_t
{
   uint32_t capture_size;
   /**< @h2xmle_description  {Specifies the visualizer input data capture window length for FFT
        analysis.\n The size must be a power of 2.\n
        Set the parameter before audio playback starts.\n
        Get the parameter only after playback starts.\n}
        @h2xmle_rangeList        {"128" = 128;
                                  "256" = 256;
                                  "512" = 512;
                                  "1024" = 1024;
                                  "2048" = 2048;
                                  "4096" = 4096}
		@h2xmle_default {256}*/
}
#include "adsp_end_pack.h"
;

/* ID of the Visualizer Scaling Mode parameter used by AUDPROC_MODULE_ID_VISUALIZER. */
#define   AUDPROC_PARAM_ID_VISUALIZER_SCALING_MODE           0x000108CD

/* Structure for the visualzier scaling mode parameter */
typedef struct audproc_visualizer_scaling_mode_t audproc_visualizer_scaling_mode_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VISUALIZER_SCALING_MODE", AUDPROC_PARAM_ID_VISUALIZER_SCALING_MODE}
    @h2xmlp_description {Specifies Specifies the visualizer Scaling mode. }  */

#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_VISUALIZER_SCALING_MODE parameter used by the
 Visualizer module.
 */
struct audproc_visualizer_scaling_mode_t
{
   uint32_t scaling_mode;
   /**< @h2xmle_description  {Specifies the visualizer Scaling mode.\n
   Input PCM samples are normalized frame-by-frame and saved in 8-bit resolution.\n
   Input PCM samples are simply right-shifted and saved in 8-bit resolution.\n}

        @h2xmle_rangeList     {"NORMALIZED" = 0;
                               "AS_PLAYED" = 1}  */

}
#include "adsp_end_pack.h"
;


/* ID of the read-only Visualizer PCM Data parameter used by AUDPROC_MODULE_ID_VISUALIZER. */
#define   AUDPROC_PARAM_ID_VISUALIZER_PCM_DATA               0x000108CE

/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VISUALIZER_PCM_DATA", AUDPROC_PARAM_ID_VISUALIZER_PCM_DATA}
    @h2xmlp_description {This parameter is used only for Get Parameter calls:
    #ADM_CMD_GET_PP_PARAMS_V5 and #ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n
    These Get parameters work only when the module is enabled
    (#AUDPROC_PARAM_ID_VISUALIZER_ENABLE). \n}  */

/* ID of the read-only Visualizer FFT Data parameter used by AUDPROC_MODULE_ID_VISUALIZER. */
#define   AUDPROC_PARAM_ID_VISUALIZER_FFT_DATA               0x000108CF


/* @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VISUALIZER_FFT_DATA", AUDPROC_PARAM_ID_VISUALIZER_FFT_DATA}
    @h2xmlp_description {This parameter is used only for Get Parameter calls:
    #ADM_CMD_GET_PP_PARAMS_V5 and #ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n
    These Get parameters work only when the module is enabled
    (#AUDPROC_PARAM_ID_VISUALIZER_ENABLE). \n}  */

/* ID of the Visualizer RMS Measurement Mode parameter used by AUDPROC_MODULE_ID_VISUALIZER. */
#define   AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT_MODE       0x000108D0

/* Structure for the visualzier measurement mode parameter */
typedef struct audproc_visualizer_measurement_mode_t audproc_visualizer_measurement_mode_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT_MODE", AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT_MODE}
    @h2xmlp_description {Specifies whether the visualizer RMS Measurement mode is enabled.\n
    This parameter is used only for Get Parameter calls:
    #ADM_CMD_GET_PP_PARAMS_V5 and #ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n
    These Get parameters work only when the module is enabled
    (#AUDPROC_PARAM_ID_VISUALIZER_ENABLE) and Measurement mode is 1.\n}  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT_MODE parameter used by the
 Visualizer module.
 */
struct audproc_visualizer_measurement_mode_t
{
   uint32_t measurement_mode;
   /**< @h2xmle_description  { Specifies whether the visualizer RMS Measurement mode is enabled. }
         @h2xmle_rangeList    {"Disable"= 0;
                              "Enable"=1}  */

}
#include "adsp_end_pack.h"
;



/* ID of the read-only Visualizer Measurement Data parameter used by AUDPROC_MODULE_ID_VISUALIZER. */

#define   AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT            0x000108D1

/* Structure for the visualzier RMS measurement data parameter */
typedef struct audproc_visualizer_measurement_t audproc_visualizer_measurement_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT ", AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT }
    @h2xmlp_description {Specifies the peak absolute amplitude and the average RMS power of input PCM data in decibels. }  */


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VISUALIZER_MEASUREMENT parameter used by the
 Visualizer module.
 */

struct audproc_visualizer_measurement_t
{
   int32_t peak_value_db;
   /**< @h2xmle_description  {Specifies the peak absolute amplitude of input PCM data in decibels. } */

   int32_t rms_power_db;
   /**< @h2xmle_description  { Specifies the average RMS power of input PCM data in decibels. } */

}
#include "adsp_end_pack.h"
;
/** @}                   <-- End of the Module -->      */

#endif
