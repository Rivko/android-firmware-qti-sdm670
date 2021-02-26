#ifndef REVERB_CALIB_H
#define REVERB_CALIB_H
/*==============================================================================
  @file reverb_calib.h
  @brief This file contains REVERB API
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


/** @h2xml_title1           {Audio Reverberation}
    @h2xml_title_agile_rev  {Audio Reverberation}
    @h2xml_title_date       {January 31, 2017} */

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

#define AUDPROC_MODULE_ID_REVERB                          0x000108AA
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_REVERB",
                          AUDPROC_MODULE_ID_REVERB}
    @h2xmlm_displayName  {"AUDIO REVERB"}
    @h2xmlm_description  {ID of the Reverberation module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_REVERB_ENABLE\n
    - #AUDPROC_PARAM_ID_REVERB_MODE\n
    - #AUDPROC_PARAM_ID_REVERB_PRESET\n
    - #AUDPROC_PARAM_ID_REVERB_WET_MIX\n
    - #AUDPROC_PARAM_ID_REVERB_GAIN_ADJUST\n
    - #AUDPROC_PARAM_ID_REVERB_ROOM_LEVEL\n
    - #AUDPROC_PARAM_ID_REVERB_ROOM_HF_LEVEL\n
    - #AUDPROC_PARAM_ID_REVERB_DECAY_TIME\n
    - #AUDPROC_PARAM_ID_REVERB_DECAY_HF_RATIO\n
    - #AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL\n
    - #AUDPROC_PARAM_ID_REVERB_REFLECTIONS_DELAY\n
    - #AUDPROC_PARAM_ID_REVERB_LEVEL\n
    - #AUDPROC_PARAM_ID_REVERB_DELAY\n
    - #AUDPROC_PARAM_ID_REVERB_DIFFUSION\n
    - #AUDPROC_PARAM_ID_REVERB_DENSITY\n
    - #AUDPROC_PARAM_ID_ENABLE\a
    All parameter IDs are device independent.\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
*/

/* ID of the Reverberation Enable parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_ENABLE                    0x000108AB

/* Structure for the enable parameter of Reverb module. */
typedef struct audproc_reverb_enable_t audproc_reverb_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_ENABLE", AUDPROC_PARAM_ID_REVERB_ENABLE}
    @h2xmlp_description {Enables the Reverb module} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_ENABLE parameter used by the Reverb module. */
struct audproc_reverb_enable_t
{
   uint32_t enable_flag;
    /**< @h2xmle_description  { Specifies whether reverberation is enabled. }
         @h2xmle_rangeList        {"Disable"= 0;
                              "Enable"=1}   */

}
#include "adsp_end_pack.h"
;


/* ID of the Reverberation Mode parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_MODE                      0x000108AC

/* Structure for the mode parameter of Reverb module. */
typedef struct audproc_reverb_mode_t audproc_reverb_mode_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_MODE", AUDPROC_PARAM_ID_REVERB_MODE}
    @h2xmlp_description { Used only for Get Parameter calls.\n
    This parameter must be initialized once when the library is created.\n
    The mode cannot be changed after initialization. Hence, this parameter
    ID is used only for Get Parameter calls.\n
	Insert effects\n
	One source is in and one source is out. The input/output channels match so
    the reverberation output is a mixture of original (dry) sound and
    reverberation (wet) sound. For example, applying reverberation to a music
    stream.\n
	Auxiliary effects\n
	Multiple input sources and one global reverberation engine. Each input source makes a copy of itself
	with its own (Q15) reverberation send gain applied. The copies are mixed internally inside the
	reverberation library.\n
	The sound sources can have different numbers of channels, and they do not need to match the
	reverberation output numbers of channels.\n
	After mixing all inputs, reverberation generates dense echoes (wet sound). The reverberation (wet)
	output must be mixed somewhere outside the reverberation library with the direct (dry) sound. For
	example, applying one global reverberation for gaming or multi-track MIDI.\n } */


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_MODE parameter used by the
 Reverb module.
 */
struct audproc_reverb_mode_t
{
   uint32_t reverb_mode;
   /**< @h2xmle_description  { Specifies the reverberation topology mode.\n
   The AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_HEADPHONE and
        AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_SPEAKER topologies support only
        Insert Effects mode.\n
        For Auxiliary Effect mode, a custom topology must be defined.\n }

        @h2xmle_rangeList    {"Insert Effects mode"=0;
                              "Auxiliary Effects mode"=1}
        @h2xmle_default      {0}   */

}
#include "adsp_end_pack.h"
;


/* ID of the Reverberation Preset parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_PRESET                    0x000108AD

/** User-customized preset (with environmental reverberation parameters specified individually). */
#define  AUDPROC_REVERB_PRESET_CUSTOM            0

/** Simulates an environment in a room. */
#define  AUDPROC_REVERB_PRESET_ROOM              1

/** Simulates an environment in a bathroom. */
#define  AUDPROC_REVERB_PRESET_BATHROOM          2

/** Simulates an environment in a concert hall. */
#define  AUDPROC_REVERB_PRESET_CONCERTHALL       3

/** Simulates an environment in a cave. */
#define  AUDPROC_REVERB_PRESET_CAVE              4

/** Simulates an environment in an arena. */
#define  AUDPROC_REVERB_PRESET_ARENA             5

/** Simulates an environment in a forest. */
#define  AUDPROC_REVERB_PRESET_FOREST            6

/** Simulates an environment in a city. */
#define  AUDPROC_REVERB_PRESET_CITY              7

/** Simulates an environment in the mountains (open air). */
#define  AUDPROC_REVERB_PRESET_MOUNTAINS         8

/** Simulates an environment under the water. */
#define  AUDPROC_REVERB_PRESET_UNDERWATER        9

/** Simulates an environment in an auditorium. */
#define  AUDPROC_REVERB_PRESET_AUDITORIUM        10

/** Simulates an environment in an alley. */
#define  AUDPROC_REVERB_PRESET_ALLEY             11

/** Simulates an environment in a hallway. */
#define  AUDPROC_REVERB_PRESET_HALLWAY           12

/** Simulates an environment in a hangar. */
#define  AUDPROC_REVERB_PRESET_HANGAR            13

/** Simulates an environment in a living room. */
#define  AUDPROC_REVERB_PRESET_LIVINGROOM        14

/** Simulates an environment in a small room. */
#define  AUDPROC_REVERB_PRESET_SMALLROOM         15

/** Simulates an environment in a medium-sized room. */
#define  AUDPROC_REVERB_PRESET_MEDIUMROOM        16

/** Simulates an environment in a large room. */
#define  AUDPROC_REVERB_PRESET_LARGEROOM         17

/** Simulates an environment in a medium-sized hall. */
#define  AUDPROC_REVERB_PRESET_MEDIUMHALL        18

/** Simulates an environment in a large hall. */
#define  AUDPROC_REVERB_PRESET_LARGEHALL         19

/** Simulates sound being sent to a metal plate, which vibrates back and forth.
    These vibrations are transformed into an audio signal.
 */
 #define  AUDPROC_REVERB_PRESET_PLATE             20

/** Simulates a generic reverberation effect. */
#define  AUDPROC_REVERB_PRESET_GENERIC           21

/** Simulates an environment in a padded cell. */
#define  AUDPROC_REVERB_PRESET_PADDEDCELL        22

/** Simulates an environment in a stone room. */
#define  AUDPROC_REVERB_PRESET_STONEROOM         23

/** Simulates an environment in a carpeted hallway. */
#define  AUDPROC_REVERB_PRESET_CARPETEDHALLWAY   24

/** Simulates an environment in a stone corridor. */
#define  AUDPROC_REVERB_PRESET_STONECORRIDOR     25

/** Simulates an environment in a quarry. */
#define  AUDPROC_REVERB_PRESET_QUARRY            26

/** Simulates an environment on an open plain. */
#define  AUDPROC_REVERB_PRESET_PLAIN             27

/** Simulates an environment in a parking lot. */
#define  AUDPROC_REVERB_PRESET_PARKINGLOT        28

/** Simulates an environment in a sewer pipe. */
#define  AUDPROC_REVERB_PRESET_SEWERPIPE         29

/** Synthetic environment preset: drugged. */
#define  AUDPROC_REVERB_PRESET_DRUGGED           30

/** Synthetic environment preset: dizzy. */
#define  AUDPROC_REVERB_PRESET_DIZZY             31

/** Synthetic environment preset: psychotic. */
#define  AUDPROC_REVERB_PRESET_PSYCHOTIC         32


/* Structure for the preset parameter of Reverb module. */
typedef struct audproc_reverb_preset_t audproc_reverb_preset_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_PRESET", AUDPROC_PARAM_ID_REVERB_PRESET}
    @h2xmlp_description { Used to preset parameters for Reverb module.\n\n
User-customized preset (with environmental reverberation parameters specified individually) - AUDPROC_REVERB_PRESET_CUSTOM\n\n
To simulates an environment in a room - AUDPROC_REVERB_PRESET_ROOM\n\n
To simulates an environment in a bathroom - AUDPROC_REVERB_PRESET_BATHROOM\n \n
To Simulates an environment in a concert hall -AUDPROC_REVERB_PRESET_CONCERTHALL\n  \n
To Simulates an environment in a cave - AUDPROC_REVERB_PRESET_CAVE\n \n
To Simulates an environment in an arena - AUDPROC_REVERB_PRESET_ARENA\n \n
To Simulates an environment in a forest - AUDPROC_REVERB_PRESET_FOREST\n \n
To  Simulates an environment in a city - AUDPROC_REVERB_PRESET_CITY\n \n
To Simulates an environment in the mountains (open air) - AUDPROC_REVERB_PRESET_MOUNTAINS\n \n
To Simulates an environment under the water - AUDPROC_REVERB_PRESET_UNDERWATER\n \n
To Simulates an environment in an auditorium - AUDPROC_REVERB_PRESET_AUDITORIUM\n\n
To Simulates an environment in an alley - AUDPROC_REVERB_PRESET_ALLEY\n\n
To Simulates an environment in a hallway - AUDPROC_REVERB_PRESET_HALLWAY\n\n
To Simulates an environment in a hangar - AUDPROC_REVERB_PRESET_HANGAR\n\n
To Simulates an environment in a living room - AUDPROC_REVERB_PRESET_LIVINGROOM\n \n
To Simulates an environment in a small room - AUDPROC_REVERB_PRESET_SMALLROOM\n\n
To Simulates an environment in a medium-sized room - AUDPROC_REVERB_PRESET_MEDIUMROOM\n\n
To Simulates an environment in a large room - AUDPROC_REVERB_PRESET_LARGEROOM\n\n
To Simulates an environment in a medium-sized hall - AUDPROC_REVERB_PRESET_MEDIUMHALL\n\n
To Simulates an environment in a large hall - AUDPROC_REVERB_PRESET_LARGEHALL\n\n
To Simulates sound being sent to a metal plate, which vibrates back and forth(These vibrations are transformed into an audio signal) - AUDPROC_REVERB_PRESET_PLATE\n\n
To Simulates a generic reverberation effect - AUDPROC_REVERB_PRESET_GENERIC\n\n
To Simulates an environment in a padded cell - AUDPROC_REVERB_PRESET_PADDEDCELL\n\n
ToSimulates an environment in a stone room - AUDPROC_REVERB_PRESET_STONEROOM\n\n
To Simulates an environment in a carpeted hallway - AUDPROC_REVERB_PRESET_CARPETEDHALLWAY\n\n
To Simulates an environment in a stone corridor - AUDPROC_REVERB_PRESET_STONECORRIDOR\n\n
To Simulates an environment in a quarry - AUDPROC_REVERB_PRESET_QUARRY\n\n
To Simulates an environment on an open plain - AUDPROC_REVERB_PRESET_PLAIN\n\n
To Simulates an environment in a parking lot - AUDPROC_REVERB_PRESET_PARKINGLOT\n\n
To Simulates an environment in a sewer pipe - AUDPROC_REVERB_PRESET_SEWERPIPE \n\n
To Synthetic environment preset: drugged - AUDPROC_REVERB_PRESET_DRUGGED \n\n
To Synthetic environment preset: dizzy - AUDPROC_REVERB_PRESET_DIZZY\n\n
To Synthetic environment preset: psychotic - AUDPROC_REVERB_PRESET_PSYCHOTIC\n } */


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_PRESET parameter used by the
 Reverb module.
 */
struct audproc_reverb_preset_t
{
   uint32_t reverb_preset;
   /**< @h2xmle_description  {Specifies one of the reverberation presets that create special
							  environmental audio effects.}
        @h2xmle_range        {0..32}
        @h2xmle_default     {AUDPROC_REVERB_PRESET_CUSTOM}

        @h2xmle_rangeList    {"AUDPROC_REVERB_PRESET_CUSTOM"=0;
                              " AUDPROC_REVERB_PRESET_ROOM"=1;
                "AUDPROC_REVERB_PRESET_BATHROOM"=2;
                "AUDPROC_REVERB_PRESET_CONCERTHALL"=3;
                "AUDPROC_REVERB_PRESET_CAVE"=4;
                "AUDPROC_REVERB_PRESET_ARENA"=5;
                "AUDPROC_REVERB_PRESET_FOREST"=6;
                "AUDPROC_REVERB_PRESET_CITY"=7;
                "AUDPROC_REVERB_PRESET_MOUNTAINS "=8;
                "AUDPROC_REVERB_PRESET_UNDERWATER"=9;
                "AUDPROC_REVERB_PRESET_AUDITORIUM"=10;
                "AUDPROC_REVERB_PRESET_ALLEY"=11;
                "AUDPROC_REVERB_PRESET_HALLWAY"=12;
                " AUDPROC_REVERB_PRESET_HANGAR"=13;
                "AUDPROC_REVERB_PRESET_LIVINGROOM"=14;
                "AUDPROC_REVERB_PRESET_SMALLROOM "=15;
                "AUDPROC_REVERB_PRESET_MEDIUMROOM"=16;
                "AUDPROC_REVERB_PRESET_LARGEROOM "=17;
                "AUDPROC_REVERB_PRESET_MEDIUMHALL"=18;
                " AUDPROC_REVERB_PRESET_LARGEHALL "=19;
                "AUDPROC_REVERB_PRESET_PLATE"=20;
                " AUDPROC_REVERB_PRESET_GENERIC"=21;
                "AUDPROC_REVERB_PRESET_PADDEDCELL "=22;
                "AUDPROC_REVERB_PRESET_STONEROOM"=23;
                "AUDPROC_REVERB_PRESET_CARPETEDHALLWAY"=24;
                "AUDPROC_REVERB_PRESET_STONECORRIDOR"=25;
                "AUDPROC_REVERB_PRESET_QUARRY"=26;
                "AUDPROC_REVERB_PRESET_PLAIN"=27;
                "AUDPROC_REVERB_PRESET_PARKINGLOT"=28;
                "AUDPROC_REVERB_PRESET_SEWERPIPE"=29;
                "AUDPROC_REVERB_PRESET_DRUGGED"=30;
                "AUDPROC_REVERB_PRESET_DIZZY"=31;
                "AUDPROC_REVERB_PRESET_PSYCHOTIC"=32 }	*/

}
#include "adsp_end_pack.h"
;


/* ID of the Reverberation Wet Mix parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_WET_MIX                   0x000108AE

/* Structure for the wet mix parameter of Reverb module. */
typedef struct audproc_reverb_wet_mix_t audproc_reverb_wet_mix_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_WET_MIX", AUDPROC_PARAM_ID_REVERB_WET_MIX}
    @h2xmlp_description { Specifies the reverberation wet/dry mix ratio for Insert Effects mode} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_WET_MIX parameter used by the
 Reverb module.
 */
struct audproc_reverb_wet_mix_t
{
   uint32_t reverb_wet_mix;
   /**< @h2xmle_description  {Specifies the reverberation wet/dry mix ratio for Insert Effects mode.}
        @h2xmle_range        {0..1000}
	@h2xmle_default	{618} */

}
#include "adsp_end_pack.h"
;

/* ID of the Reverberation Gain Adjust parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_GAIN_ADJUST               0x000108AF

/* Structure for the gain adjust parameter of Reverb module. */
typedef struct audproc_reverb_gain_adjust_t audproc_reverb_gain_adjust_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_GAIN_ADJUST", AUDPROC_PARAM_ID_REVERB_GAIN_ADJUST}
    @h2xmlp_description { Specifies the overall gain adjustment of reverberation outputs}  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_GAIN_ADJUST parameter used by the
 Reverb module.
 */
struct audproc_reverb_gain_adjust_t
{
   int32_t gain_adjust;
   /**< @h2xmle_description  {Specifies the overall gain adjustment of
                              reverberation outputs,in the units of 'millibels'.}
        @h2xmle_range        {-600..600}
        @h2xmle_default      {0}	*/

}
#include "adsp_end_pack.h"
;

/* ID of the Reverberation Room Level parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_ROOM_LEVEL                0x000108B0

/* Structure for the room level parameter of Reverb module. */
typedef struct audproc_reverb_room_level_t audproc_reverb_room_level_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_ROOM_LEVEL", AUDPROC_PARAM_ID_REVERB_ROOM_LEVEL}
    @h2xmlp_description { Specifies the master volume level of the environment reverberation effect} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_ROOM_LEVEL parameter used by the
 Reverb module.
 */
struct audproc_reverb_room_level_t
{
   int32_t room_level;
   /**< @h2xmle_description  {Specifies the master volume level of the environment reverberation
                              effect,in the units of 'millibels'.}
        @h2xmle_range        {-9600..0}
	@h2xmle_default       {-9600}       	*/

}
#include "adsp_end_pack.h"
;

/* ID of the Reverberation Room High Frequency Level parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_ROOM_HF_LEVEL             0x000108B1

/* Structure for the room hf level parameter of Reverb module. */
typedef struct audproc_reverb_room_hf_level_t audproc_reverb_room_hf_level_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_ROOM_HF_LEVEL", AUDPROC_PARAM_ID_REVERB_ROOM_HF_LEVEL}
    @h2xmlp_description { Specifies the relative volume level} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_ROOM_HF_LEVEL parameter used by the
 Reverb module.
 */
struct audproc_reverb_room_hf_level_t
{
   int32_t room_hf_level;
   /**< @h2xmle_description  { Specifies the volume level at 5 kHz relative to the volume level at
                               low frequencies of the overall reverberation effect,
							   in the units of 'millibels'.}
        @h2xmle_range        {-9600..0}
        @h2xmle_default       {0}		*/

}
#include "adsp_end_pack.h"
;

/* ID of the Reverberation Decay Time parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_DECAY_TIME                0x000108B2

/* Structure for the decay time parameter of Reverb module. */
typedef struct audproc_reverb_decay_time_t audproc_reverb_decay_time_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_DECAY_TIME", AUDPROC_PARAM_ID_REVERB_DECAY_TIME}
    @h2xmlp_description { Specifies the Specifies the time for the level of reverberation} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_DECAY_TIME parameter used by the
 Reverb module.
 */
struct audproc_reverb_decay_time_t
{
   uint32_t decay_time;
   /**< @h2xmle_description  {Specifies the time for the level of reverberation to decay
                              by 60 dB,in the units of 'milliseconds'.}
		@h2xmle_default      {1000}
        @h2xmle_range        {100..20000}  */

}
#include "adsp_end_pack.h"
;

/* ID of the Reverberation Decay High Frequency Ratio parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_DECAY_HF_RATIO            0x000108B3

/* Structure for the decay hf ratio parameter of Reverb module. */
typedef struct audproc_reverb_decay_hf_ratio_t audproc_reverb_decay_hf_ratio_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_DECAY_HF_RATIO", AUDPROC_PARAM_ID_REVERB_DECAY_HF_RATIO}
    @h2xmlp_description { Specifies the ratio of high frequency decay time} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_DECAY_HF_RATIO parameter used by the
 Reverb module.
 */
struct audproc_reverb_decay_hf_ratio_t
{
   uint32_t decay_hf_ratio;
   /**< @h2xmle_description  {Specifies the ratio of high frequency decay time (at 5 kHz) relative
                              to the decay time at low frequencies,in the units of 'milliseconds'.}
		@h2xmle_default      {500}
        @h2xmle_range        {150..2000}  */

}
#include "adsp_end_pack.h"
;

/*ID of the Reverberation Reflections Level parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL         0x000108B4

/* Structure for the reverb reflections level parameter of Reverb module. */
typedef struct audproc_reverb_reflections_level_t audproc_reverb_reflections_level_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL",AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL}
    @h2xmlp_description { Specifies the volume level of the early reflections} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL parameter used by the
 Reverb module.
 */
struct audproc_reverb_reflections_level_t
{
   int32_t reflections_level;
   /**< @h2xmle_description  {Specifies the volume level of the early reflections,in the units of 'millibels'.}
        @h2xmle_range        {-9600..1000}
	@h2xmle_default       {-9600}  */

}
#include "adsp_end_pack.h"
;


/*ID of the Reverberation Reflections Delay parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_REFLECTIONS_DELAY         0x000108B5

/* Structure for the reverb reflections delay parameter of Reverb module. */
typedef struct audproc_reverb_reflections_delay_t audproc_reverb_reflections_delay_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL",AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL}
    @h2xmlp_description { Specifies the time between the first reflection and the late reverberation} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_REFLECTIONS_DELAY parameter used by the
 Reverb module.
 */
struct audproc_reverb_reflections_delay_t
{
   uint32_t reflections_delay;
   /**< @h2xmle_description  { Specifies the time between the first reflection and
                               the late reverberation,in the units of 'milliseconds'.}
        @h2xmle_range        {0..300}
	@h2xmle_default       {20} */

}
#include "adsp_end_pack.h"
;


/* ID of the Reverberation Level parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_LEVEL                      0x000108B6

/* Structure for the reverb level parameter of Reverb module. */
typedef struct audproc_reverb_level_t audproc_reverb_level_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_LEVEL",AUDPROC_PARAM_ID_REVERB_LEVEL}
    @h2xmlp_description { Specifies the volume level of the late reverberation} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_LEVEL parameter used by the
 Reverb module.
 */
struct audproc_reverb_level_t
{
   int32_t reverb_level;
   /**< @h2xmle_description  {Specifies the volume level of the late reverberation.}
        @h2xmle_range        {-9600..2000}
        @h2xmle_default       {-9600} */

}
#include "adsp_end_pack.h"
;


/*ID of the Reverberation Delay parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_DELAY                      0x000108B7

/* Structure for the reverb delay parameter of Reverb module. */
typedef struct audproc_reverb_delay_t audproc_reverb_delay_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_DELAY",AUDPROC_PARAM_ID_REVERB_DELAY}
    @h2xmlp_description { Specifies the time between the first reflection and the late reverberation} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_DELAY parameter used by the
 Reverb module.
 */
struct audproc_reverb_delay_t
{
   uint32_t reverb_delay;
   /**< @h2xmle_description  {Specifies the time between the first reflection and
                              the late reverberation,in the units of ' milliseconds'.}
        @h2xmle_range        {0..100}
        @h2xmle_default       {40}
		@h2xmle_visibility	{hide} */

}
#include "adsp_end_pack.h"
;


/* ID of the Reverberation Diffusion parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_DIFFUSION                  0x000108B8

/* Structure for the reverb diffusion parameter of Reverb module. */
typedef struct audproc_reverb_diffusion_t audproc_reverb_diffusion_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_DIFFUSION",AUDPROC_PARAM_ID_REVERB_DIFFUSION}
    @h2xmlp_description { Specifies the echo density in the late reverberation decay} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_DIFFUSION parameter used by the
 Reverb module.
 */
struct audproc_reverb_diffusion_t
{
   uint32_t diffusion;
   /**< @h2xmle_description  {Specifies the echo density in the late reverberation decay.}
        @h2xmle_range        {0..1000}
       @h2xmle_default       {1000}	 */

}
#include "adsp_end_pack.h"
;

/** ID of the Reverberation Density parameter used by AUDPROC_MODULE_ID_REVERB. */
#define AUDPROC_PARAM_ID_REVERB_DENSITY                    0x000108B9

/* Structure for the reverb density parameter of Reverb module. */
typedef struct audproc_reverb_density_t audproc_reverb_density_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_REVERB_DENSITY",AUDPROC_PARAM_ID_REVERB_DENSITY}
    @h2xmlp_description { Specifies the modal density of the late reverberation decay} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_REVERB_DENSITY parameter used by the
 Reverb module.
 */
struct audproc_reverb_density_t
{
   uint32_t density;
   /**< @h2xmle_description  {Specifies the modal density of the late reverberation decay}
        @h2xmle_range    {0..1000}

        @h2xmle_default	{1000} 		*/

}
#include "adsp_end_pack.h"
;
/** @}                   <-- End of the Module -->*/

#endif



















