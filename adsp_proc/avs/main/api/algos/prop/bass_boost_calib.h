#ifndef BASS_BOOST_CALIB_H
#define BASS_BOOST_CALIB_H
/*==============================================================================
  @file bass_boost_calib.h
  @brief This file contains BASS_BOOST API
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

/** @h2xml_title1           {BASS_BOOST}
    @h2xml_title_agile_rev  {BASS_BOOST}
    @h2xml_title_date       {February 1, 2017} */

#include "mmdefs.h"



/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition
   Module library is independent of this number, it defined here for static
   loading purpose only */

#define AUDPROC_MODULE_ID_BASS_BOOST                             0x000108A1
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_BASS_BOOST",
                          AUDPROC_MODULE_ID_BASS_BOOST}
    @h2xmlm_displayName  {"AUDIO BASS BOOST"}
    @h2xmlm_description  {ID of the Bass Boost module.\n

      This module supports the following parameter IDs:\n
      - #AUDPROC_PARAM_ID_BASS_BOOST_ENABLE\n
      - #AUDPROC_PARAM_ID_BASS_BOOST_MODE\n
      - #AUDPROC_PARAM_ID_BASS_BOOST_STRENGTH\n
      - #AUDPROC_PARAM_ID_ENABLE\n

      All parameter IDs are device independent.\n}

     @h2xmlm_toolPolicy              {Calibration; RTC}
 @{                   <-- Start of the Module -->
*/


/* ID of the Bass Boost Enable parameter used by AUDPROC_MODULE_ID_BASS_BOOST. */
#define AUDPROC_PARAM_ID_BASS_BOOST_ENABLE                       0x000108A2

/* Structure for the enable parameter of Bass Boost module. */
typedef struct audproc_bass_boost_enable_t audproc_bass_boost_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_BASS_BOOST_ENABLE", AUDPROC_PARAM_ID_BASS_BOOST_ENABLE}
    @h2xmlp_description {Enables the bass boost module} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_BASS_BOOST_ENABLE parameter used by the Bass Boost module. */
struct audproc_bass_boost_enable_t
{
   uint32_t enable_flag;
   /**< @h2xmle_description {Specifies whether bass boost is enabled.}
        @h2xmle_rangeList        {"Disable"= 0;
                                  "Enable"=1} }*/
}
#include "adsp_end_pack.h"
;


/* ID of the Bass Boost Mode parameter used by AUDPROC_MODULE_ID_BASS_BOOST. */
#define AUDPROC_PARAM_ID_BASS_BOOST_MODE                         0x000108A3

/* Structure for the mode parameter of Bass Boost module. */
typedef struct audproc_bass_boost_mode_t audproc_bass_boost_mode_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_BASS_BOOST_MODE", AUDPROC_PARAM_ID_BASS_BOOST_MODE}
    @h2xmlp_description {Configures the bass boost mode}  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_BASS_BOOST_MODE parameter used by the Bass Boost module. */
struct audproc_bass_boost_mode_t
{
   uint32_t bass_boost_mode;
    /**< @h2xmle_description     {Specifies the bass boost mode. This parameter is device dependent.\n
    Physical boost enhances the low frequency contents.\n
    Virtual boost creates harmonics of the low frequency contents (PBE).\n
    Currently, only Physical Boost mode is supported.\n
    Virtual Boost mode is not supported; it is reserved for future use.\n}

    @h2xmle_rangeList        {"Physical boost (used by the headphone)"= 0;
                                  "Virtual boost (used by small speakers)"=1}
    @h2xmle_default          {0}   */

}
#include "adsp_end_pack.h"
;



/* ID of the Bass Boost Strength parameter used by AUDPROC_MODULE_ID_BASS_BOOST. */
#define AUDPROC_PARAM_ID_BASS_BOOST_STRENGTH                     0x000108A4


/* Structure for the strength parameter of Bass Boost module. */
typedef struct audproc_bass_boost_strength_t audproc_bass_boost_strength_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_BASS_BOOST_STRENGTH", AUDPROC_PARAM_ID_BASS_BOOST_STRENGTH}
    @h2xmlp_description {Specifies the effects of bass boost.} */


#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_BASS_BOOST_STRENGTH parameter used by the Bass Boost module. */
struct audproc_bass_boost_strength_t
{
   uint32_t strength;
   /**< @h2xmle_description {Specifies the effects of bass boost. This parameter affects the audio
        stream and is device independent.\n}

        @h2xmle_range        {0..1000}
	@h2xmle_default      {1000} */

}
#include "adsp_end_pack.h"
;

/** @}                   <-- End of the Module -->*/

#endif
