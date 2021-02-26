#ifndef VIRTUALIZER_CALIB_H
#define VIRTUALIZER_CALIB_H
/*==============================================================================
  @file virtualizer_calib.h
  @brief This file contains VIRTUALIZER API
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

/** @h2xml_title1           {Audio Virtualizer}
    @h2xml_title_agile_rev  {Audio Virtualizer}
    @h2xml_title_date       {February 6, 2017} */

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

#define AUDPROC_MODULE_ID_VIRTUALIZER                            0x000108A5
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_VIRTUALIZER",
                          AUDPROC_MODULE_ID_VIRTUALIZER}
    @h2xmlm_displayName  {"AUDIO VIRTUALIZER"}
    @h2xmlm_description  {ID of the Virtualizer module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_VIRTUALIZER_ENABLE\n
    - #AUDPROC_PARAM_ID_VIRTUALIZER_STRENGTH\n
    - #AUDPROC_PARAM_ID_VIRTUALIZER_OUT_TYPE\n
    - #AUDPROC_PARAM_ID_VIRTUALIZER_GAIN_ADJUST
    - #AUDPROC_PARAM_ID_ENABLE\n\n

    All parameter IDs are device independent.\n}
    @h2xmlm_toolPolicy              {Calibration; RTC}


    @{                   <-- Start of the Module -->
*/

/* ID of the Virtualizer Enable parameter used by AUDPROC_MODULE_ID_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_VIRTUALIZER_ENABLE                      0x000108A6

/* Structure for the enable parameter of Virtualizer module. */
typedef struct audproc_virtualizer_enable_t audproc_virtualizer_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VIRTUALIZER_ENABLE", AUDPROC_PARAM_ID_VIRTUALIZER_ENABLE}
    @h2xmlp_description {Enables the virtualizer module} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VIRTUALIZER_ENABLE parameter used by the Virtualizer module. */

struct audproc_virtualizer_enable_t
{
   uint32_t enable_flag;
    /**< @h2xmle_description  { Specifies whether virtualizer is enabled. }
         @h2xmle_rangeList        {"Disable"= 0;
                                   "Enable"=1}  */

}
#include "adsp_end_pack.h"
;


/* ID of the Virtualizer Strength parameter used by AUDPROC_MODULE_ID_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_VIRTUALIZER_STRENGTH                    0x000108A7

/* Structure for the strength parameter of VIRTUALIZER module. */
typedef struct audproc_virtualizer_strength_t audproc_virtualizer_strength_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VIRTUALIZER_STRENGTH", AUDPROC_PARAM_ID_VIRTUALIZER_STRENGTH}
    @h2xmlp_description {Specifies the effects virtualizer} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VIRTUALIZER_STRENGTH parameter used by the Virtualizer module. */
struct audproc_virtualizer_strength_t
{
   uint32_t strength;
   /**< @h2xmle_description  {Specifies the effects of the virtualizer.\n
    This parameter affects the audio stream and is device independent.\n }
         @h2xmle_range       {0..1000}
         @h2xmle_default	   {1000}*/

}
#include "adsp_end_pack.h"
;


/* ID of the Virtualizer Out Type parameter used by AUDPROC_MODULE_ID_VIRTUALIZER.*/
#define AUDPROC_PARAM_ID_VIRTUALIZER_OUT_TYPE                    0x000108A8

/* Structure for the out type parameter of VIRTUALIZER module. */
typedef struct audproc_virtualizer_out_type_t audproc_virtualizer_out_type_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VIRTUALIZER_OUT_TYPE", AUDPROC_PARAM_ID_VIRTUALIZER_OUT_TYPE}
    @h2xmlp_description {Specifies the output device type of the virtualizer.} */

/* Payload of the AUDPROC_PARAM_ID_VIRTUALIZER_OUT_TYPE parameter used by the Virtualizer module. */
#include "adsp_begin_pack.h"

struct audproc_virtualizer_out_type_t
{
   uint32_t out_type;
   /**< @h2xmle_description  { Specifies the output device type of the virtualizer.\n
   This parameter is device dependent.\n}
        @h2xmle_rangeList        {"Headphone"= 0;
                                   "Desktop speakers"=1}
    @h2xmle_default	        {0}    */

}
#include "adsp_end_pack.h"
;


/* ID of the Virtualizer Gain Adjust parameter used by AUDPROC_MODULE_ID_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_VIRTUALIZER_GAIN_ADJUST                 0x000108A9


/* Structure for the strength parameter of VIRTUALIZER module. */
typedef struct audproc_virtualizer_gain_adjust_t audproc_virtualizer_gain_adjust_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VIRTUALIZER_GAIN_ADJUST", AUDPROC_PARAM_ID_VIRTUALIZER_GAIN_ADJUST}
    @h2xmlp_description {Specifies the overall gain adjustment of virtualizer outputs.}  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VIRTUALIZER_GAIN_ADJUST parameter used by the Virtualizer module. */

struct audproc_virtualizer_gain_adjust_t
{
   int32_t gain_adjust;
   /**< @h2xmle_description  { Specifies the overall gain adjustment of virtualizer outputs, in the unit 'millibels'.\n
   This parameter is device dependent.\n}
        @h2xmle_default     {0}
        @h2xmle_range       {-600..600}
        @@h2xmle_default  {0}	*/

}
#include "adsp_end_pack.h"
;

/** @}                   <-- End of the Module -->*/
#endif

