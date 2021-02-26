#ifndef SHOEBOX_CALIB_H
#define SHOEBOX_CALIB_H
/*==============================================================================
  @file shoebox_calib.h
  @brief This file contains SHOEBOX API
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

#include "mmdefs.h"
#include "audio_common_param_calib.h"


/** @h2xml_title1           {SHOEBOX}
    @h2xml_title_agile_rev  {SHOEBOX}
    @h2xml_title_date       {January 30, 2017} */

/**
   @h2xmlx_xmlNumberFormat {int}
*/


#define AUDPROC_MODULE_ID_SHOEBOX                          0x00010938
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_SHOEBOX",
                          AUDPROC_MODULE_ID_SHOEBOX}
    @h2xmlm_displayName  {"AUDIO SHOEBOX"}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->  */

#define AUDPROC_PARAM_ID_SHOEBOX_ENABLE                    0x00010939

/* Structure for the enable parameter of Shoebox module. */
typedef struct audproc_shoebox_enable_t audproc_shoebox_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_SHOEBOX_ENABLE", AUDPROC_PARAM_ID_SHOEBOX_ENABLE}
    @h2xmlp_description {Used to configure a device}  */


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_SHOEBOX_ENABLE parameter used by the
 Shoebox module.
 */
struct audproc_shoebox_enable_t
{
   uint32_t enable_flag;
   /**< @h2xmle_description  {Specifies whether shoebox module is enabled.}
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}  */
}
#include "adsp_end_pack.h"
;


#define AUDPROC_PARAM_ID_SHOEBOX_WET_MIX                    0x0001093A

/* Structure for the wet mix parameter of Shoebox module. */
typedef struct audproc_shoebox_wet_mix_t audproc_shoebox_wet_mix_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_SHOEBOX_WET_MIX", AUDPROC_PARAM_ID_SHOEBOX_WET_MIX}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_SHOEBOX_WET_MIX parameter used by the Shoebox module}
    @h2xmlp_version     {0}    */

#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_SHOEBOX_WET_MIX parameter used by the
 Shoebox module.
 */
struct audproc_shoebox_wet_mix_t
{
   uint32_t version;
   /**< @h2xmle_description {Specifies the version information of this parameter.(currently, only 0 is supported)}
		@h2xmle_default     {0}
		@h2xmle_readonly	{true}
         @h2xmle_isVersion    {true}       */

   uint32_t wet_mix;
   /**< @h2xmle_description {Specifies the mix ratio between dry and wet sound}
		@h2xmle_default     {600}
		@h2xmle_range  {0..1000}  */
}
#include "adsp_end_pack.h"
;

#define AUDPROC_PARAM_ID_SHOEBOX_ROOM_SIZE                    0x0001093B

/* Structure for the room size parameter of Shoebox module. */
typedef struct audproc_shoebox_room_size_t audproc_shoebox_room_size_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_SHOEBOX_ROOM_SIZE", AUDPROC_PARAM_ID_SHOEBOX_ROOM_SIZE}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_SHOEBOX_ROOM_SIZE parameter used by the Shoebox module}
     @h2xmlp_version     {0}   */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_SHOEBOX_ROOM_SIZE parameter used by the
 Shoebox module.
 */
struct audproc_shoebox_room_size_t
{
   uint32_t version;
   /**< @h2xmle_description {Specifies the version information of this parameter.}
		@h2xmle_default     {0}
		@h2xmle_readonly	{true}
         @h2xmle_isVersion    {true}        */


   uint32_t length;
   /**< @h2xmle_description {Specifies length of the room, in the units of 'meter'}
		@h2xmle_default     {524288}
		@h2xmle_range  {0..13107200}
		@h2xmle_dataFormat  {Q16}     */

   uint32_t width;
   /**< @h2xmle_description { Specifies width of the room, in the units of 'meter'}
		@h2xmle_default     {327680}
		@h2xmle_range  {0..6553600}
		@h2xmle_dataFormat  {Q16}     */

   uint32_t height;
   /**< @h2xmle_description { Specifies height of the room, in the units of 'meter'}
		@h2xmle_default     {196608}
		@h2xmle_range  {0..655360}
		@h2xmle_dataFormat  {Q16}     */

}
#include "adsp_end_pack.h"
;

#define AUDPROC_PARAM_ID_SHOEBOX_ROOM_MATERIAL                    0x0001093C

/** supported material IDs forAUDPROC_PARAM_ID_SHOEBOX_ROOM_MATERIAL
 */

/** To simulate the case when there is no reflection. */
#define  AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT                   0

/** To simulate the reflection from acoustic ceiling tiles. */
#define  AUDPROC_SHOEBOX_MATERIAL_ACOUSTIC_CEILING_TILES        1

/** To simulate the reflection from bare brick surface. */
#define  AUDPROC_SHOEBOX_MATERIAL_BRICK_BARE                    2

/** To simulate the reflection from painted brick surface. */
#define  AUDPROC_SHOEBOX_MATERIAL_BRICK_PAINTED                 3

/** To simulate the reflection from coarse concrete block. */
#define  AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_COARSE         4

/** To simulate the reflection from painted concrete block. */
#define  AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_PAINTED        5

/** To simulate the reflection from heavy curtain. */
#define  AUDPROC_SHOEBOX_MATERIAL_CURTAIN_HEAVY                 6

/** To simulate the reflection from fiber glass insulation. */
#define  AUDPROC_SHOEBOX_MATERIAL_FIBER_GLASS_INSULATION        7

/** To simulate the reflection from thick glass */
#define  AUDPROC_SHOEBOX_MATERIAL_GLASS_THICK                   8

/** To simulate the reflection from thin glass. */
#define  AUDPROC_SHOEBOX_MATERIAL_GLASS_THIN                    9

/** To simulate the reflection from grass. */
#define  AUDPROC_SHOEBOX_MATERIAL_GRASS                        10

/** To simulate the reflection from linoleum on concrete. */
#define  AUDPROC_SHOEBOX_MATERIAL_LINOLEUM_ON_CONCRETE         11

/** To simulate the reflection from marble. */
#define  AUDPROC_SHOEBOX_MATERIAL_MARBLE                       12

/** To simulate the reflection from metal. */
#define  AUDPROC_SHOEBOX_MATERIAL_METAL                        13

/** To simulate the reflection from parquet on concrete. */
#define  AUDPROC_SHOEBOX_MATERIAL_PARQUET_ON_CONCRETE          14

/** To simulate the reflection from rough plaster. */
#define  AUDPROC_SHOEBOX_MATERIAL_PLASTER_ROUGH                15

/** To simulate the reflection from smooth plaster. */
#define  AUDPROC_SHOEBOX_MATERIAL_PLASTER_SMOOTH               16

/** To simulate the reflection from plywood panel. */
#define  AUDPROC_SHOEBOX_MATERIAL_PLYWOOD_PANEL                17

/** To simulate the reflection from polished concrete or tile. */
#define  AUDPROC_SHOEBOX_MATERIAL_POLISHED_CONCRETE_OR_TILE    18

/** To simulate the reflection from sheet rock. */
#define  AUDPROC_SHOEBOX_MATERIAL_SHEET_ROCK                   19

/** To simulate the reflection from water or ice surface. */
#define  AUDPROC_SHOEBOX_MATERIAL_WATER_OR_ICE_SURFACE         20

/** To simulate the reflection from wood on ceiling. */
#define  AUDPROC_SHOEBOX_MATERIAL_WOOD_ON_CEILING              21

/** To simulate the reflection from wood panel. */
#define  AUDPROC_SHOEBOX_MATERIAL_WOOD_PANEL                   22

/* Structure for the room material parameter of Shoebox module. */
typedef struct audproc_shoebox_room_material_t audproc_shoebox_room_material_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_SHOEBOX_ROOM_MATERIAL", AUDPROC_PARAM_ID_SHOEBOX_ROOM_MATERIAL}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_SHOEBOX_ROOM_MATERIAL parameter used by the
 Shoebox module.\n
To simulate the case when there is no reflection - AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT\n
To simulate the reflection from acoustic ceiling tiles - AUDPROC_SHOEBOX_MATERIAL_ACOUSTIC_CEILING_TILES\n
To simulate the reflection from bare brick surface - AUDPROC_SHOEBOX_MATERIAL_BRICK_BARE\n
To simulate the reflection from painted brick surface -
AUDPROC_SHOEBOX_MATERIAL_BRICK_PAINTED\n
To simulate the reflection from coarse concrete block -
AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_COARSE\n
To simulate the reflection from painted concrete block -
AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_PAINTED\n
To simulate the reflection from heavy curtain -
AUDPROC_SHOEBOX_MATERIAL_CURTAIN_HEAVY\n
To simulate the reflection from fiber glass insulation -
AUDPROC_SHOEBOX_MATERIAL_FIBER_GLASS_INSULATION\n
To simulate the reflection from thick glass -
AUDPROC_SHOEBOX_MATERIAL_GLASS_THICK\n
To simulate the reflection from thin glass -
AUDPROC_SHOEBOX_MATERIAL_GLASS_THIN\n
To simulate the reflection from grass -
AUDPROC_SHOEBOX_MATERIAL_GRASS\n
To simulate the reflection from linoleum on concrete -
AUDPROC_SHOEBOX_MATERIAL_LINOLEUM_ON_CONCRETE\n
To simulate the reflection from marble -
AUDPROC_SHOEBOX_MATERIAL_MARBLE\n
To simulate the reflection from metal -
AUDPROC_SHOEBOX_MATERIAL_METAL\n
To simulate the reflection from parquet on concrete -
AUDPROC_SHOEBOX_MATERIAL_PARQUET_ON_CONCRETE\n
To simulate the reflection from rough plaster -
AUDPROC_SHOEBOX_MATERIAL_PLASTER_ROUGH\n
To simulate the reflection from smooth plaster -
AUDPROC_SHOEBOX_MATERIAL_PLASTER_SMOOTH\n
To simulate the reflection from plywood panel -
AUDPROC_SHOEBOX_MATERIAL_PLYWOOD_PANEL\n
To simulate the reflection from polished concrete or tile -
AUDPROC_SHOEBOX_MATERIAL_POLISHED_CONCRETE_OR_TILE\n
To simulate the reflection from sheet rock -
AUDPROC_SHOEBOX_MATERIAL_SHEET_ROCK\n
To simulate the reflection from water or ice surface -
AUDPROC_SHOEBOX_MATERIAL_WATER_OR_ICE_SURFACE\n
To simulate the reflection from wood on ceiling -
AUDPROC_SHOEBOX_MATERIAL_WOOD_ON_CEILING\n
To simulate the reflection from wood panel -
AUDPROC_SHOEBOX_MATERIAL_WOOD_PANEL }

@h2xmlp_version    {0}*/


#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_SHOEBOX_ROOM_MATERIAL parameter used by the
 Shoebox module.

 This parameter supports the following material IDs:

 - #AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT
 - #AUDPROC_SHOEBOX_MATERIAL_ACOUSTIC_CEILING_TILES
 - #AUDPROC_SHOEBOX_MATERIAL_BRICK_BARE
 - #AUDPROC_SHOEBOX_MATERIAL_BRICK_PAINTED
 - #AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_COARSE
 - #AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_PAINTED
 - #AUDPROC_SHOEBOX_MATERIAL_CURTAIN_HEAVY
 - #AUDPROC_SHOEBOX_MATERIAL_FIBER_GLASS_INSULATION
 - #AUDPROC_SHOEBOX_MATERIAL_GLASS_THICK
 - #AUDPROC_SHOEBOX_MATERIAL_GLASS_THIN
 - #AUDPROC_SHOEBOX_MATERIAL_GRASS
 - #AUDPROC_SHOEBOX_MATERIAL_LINOLEUM_ON_CONCRETE
 - #AUDPROC_SHOEBOX_MATERIAL_MARBLE
 - #AUDPROC_SHOEBOX_MATERIAL_METAL
 - #AUDPROC_SHOEBOX_MATERIAL_PARQUET_ON_CONCRETE
 - #AUDPROC_SHOEBOX_MATERIAL_PLASTER_ROUGH
 - #AUDPROC_SHOEBOX_MATERIAL_PLASTER_SMOOTH
 - #AUDPROC_SHOEBOX_MATERIAL_PLYWOOD_PANEL
 - #AUDPROC_SHOEBOX_MATERIAL_POLISHED_CONCRETE_OR_TILE
 - #AUDPROC_SHOEBOX_MATERIAL_SHEET_ROCK
 - #AUDPROC_SHOEBOX_MATERIAL_WATER_OR_ICE_SURFACE
 - #AUDPROC_SHOEBOX_MATERIAL_WOOD_ON_CEILING
 - #AUDPROC_SHOEBOX_MATERIAL_WOOD_PANEL
 */
struct audproc_shoebox_room_material_t
{
   uint32_t version;
  /**< @h2xmle_description {Specifies the version information of this parameter.}
		@h2xmle_default     {0}
		@h2xmle_readonly	{true}
        @h2xmle_isVersion   {true} */

   uint32_t front_wall;
   /**< @h2xmle_description {Specifies the material of front wall}
		@h2xmle_default     {AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT}
	@h2xmle_rangeList         {"AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT"=0;
                              "AUDPROC_SHOEBOX_MATERIAL_ACOUSTIC_CEILING_TILES"=1;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_BARE"=2;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_PAINTED"=3;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_COARSE"=4;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_PAINTED"=5;
							  "AUDPROC_SHOEBOX_MATERIAL_CURTAIN_HEAVY"=6;
							  "AUDPROC_SHOEBOX_MATERIAL_FIBER_GLASS_INSULATION"=7;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THICK"=8;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THIN"=9;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=10;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=11;
							  "AUDPROC_SHOEBOX_MATERIAL_LINOLEUM_ON_CONCRETE"=12;
							  "AUDPROC_SHOEBOX_MATERIAL_MARBLE"=13;
							  "AUDPROC_SHOEBOX_MATERIAL_METAL"=14;
							  "AUDPROC_SHOEBOX_MATERIAL_PARQUET_ON_CONCRETE"=15;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_ROUGH"=16;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_SMOOTH"=17;
							  "AUDPROC_SHOEBOX_MATERIAL_PLYWOOD_PANEL"=18;
							  "AUDPROC_SHOEBOX_MATERIAL_POLISHED_CONCRETE_OR_TILE"=19;
							  "AUDPROC_SHOEBOX_MATERIAL_SHEET_ROCK"=20;
							  "AUDPROC_SHOEBOX_MATERIAL_WATER_OR_ICE_SURFACE"=21;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_ON_CEILING"=22;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_PANEL"=23 }				*/

   uint32_t back_wall;

	/**< @h2xmle_description {Specifies the material of back wall}
		@h2xmle_default     {AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT}
			@h2xmle_rangeList         {"AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT"=0;
                              "AUDPROC_SHOEBOX_MATERIAL_ACOUSTIC_CEILING_TILES"=1;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_BARE"=2;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_PAINTED"=3;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_COARSE"=4;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_PAINTED"=5;
							  "AUDPROC_SHOEBOX_MATERIAL_CURTAIN_HEAVY"=6;
							  "AUDPROC_SHOEBOX_MATERIAL_FIBER_GLASS_INSULATION"=7;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THICK"=8;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THIN"=9;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=10;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=11;
							  "AUDPROC_SHOEBOX_MATERIAL_LINOLEUM_ON_CONCRETE"=12;
							  "AUDPROC_SHOEBOX_MATERIAL_MARBLE"=13;
							  "AUDPROC_SHOEBOX_MATERIAL_METAL"=14;
							  "AUDPROC_SHOEBOX_MATERIAL_PARQUET_ON_CONCRETE"=15;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_ROUGH"=16;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_SMOOTH"=17;
							  "AUDPROC_SHOEBOX_MATERIAL_PLYWOOD_PANEL"=18;
							  "AUDPROC_SHOEBOX_MATERIAL_POLISHED_CONCRETE_OR_TILE"=19;
							  "AUDPROC_SHOEBOX_MATERIAL_SHEET_ROCK"=20;
							  "AUDPROC_SHOEBOX_MATERIAL_WATER_OR_ICE_SURFACE"=21;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_ON_CEILING"=22;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_PANEL"=23 }*/

   uint32_t left_wall;

	/**< @h2xmle_description {Specifies the material of left wall}
		@h2xmle_default     {AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT}
		@h2xmle_rangeList         {"AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT"=0;
                              "AUDPROC_SHOEBOX_MATERIAL_ACOUSTIC_CEILING_TILES"=1;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_BARE"=2;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_PAINTED"=3;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_COARSE"=4;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_PAINTED"=5;
							  "AUDPROC_SHOEBOX_MATERIAL_CURTAIN_HEAVY"=6;
							  "AUDPROC_SHOEBOX_MATERIAL_FIBER_GLASS_INSULATION"=7;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THICK"=8;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THIN"=9;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=10;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=11;
							  "AUDPROC_SHOEBOX_MATERIAL_LINOLEUM_ON_CONCRETE"=12;
							  "AUDPROC_SHOEBOX_MATERIAL_MARBLE"=13;
							  "AUDPROC_SHOEBOX_MATERIAL_METAL"=14;
							  "AUDPROC_SHOEBOX_MATERIAL_PARQUET_ON_CONCRETE"=15;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_ROUGH"=16;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_SMOOTH"=17;
							  "AUDPROC_SHOEBOX_MATERIAL_PLYWOOD_PANEL"=18;
							  "AUDPROC_SHOEBOX_MATERIAL_POLISHED_CONCRETE_OR_TILE"=19;
							  "AUDPROC_SHOEBOX_MATERIAL_SHEET_ROCK"=20;
							  "AUDPROC_SHOEBOX_MATERIAL_WATER_OR_ICE_SURFACE"=21;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_ON_CEILING"=22;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_PANEL"=23 }*/

   uint32_t right_wall;

    /**< @h2xmle_description {Specifies the material of right wall}
		@h2xmle_default     {AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT}
		@h2xmle_rangeList         {"AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT"=0;
                              "AUDPROC_SHOEBOX_MATERIAL_ACOUSTIC_CEILING_TILES"=1;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_BARE"=2;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_PAINTED"=3;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_COARSE"=4;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_PAINTED"=5;
							  "AUDPROC_SHOEBOX_MATERIAL_CURTAIN_HEAVY"=6;
							  "AUDPROC_SHOEBOX_MATERIAL_FIBER_GLASS_INSULATION"=7;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THICK"=8;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THIN"=9;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=10;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=11;
							  "AUDPROC_SHOEBOX_MATERIAL_LINOLEUM_ON_CONCRETE"=12;
							  "AUDPROC_SHOEBOX_MATERIAL_MARBLE"=13;
							  "AUDPROC_SHOEBOX_MATERIAL_METAL"=14;
							  "AUDPROC_SHOEBOX_MATERIAL_PARQUET_ON_CONCRETE"=15;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_ROUGH"=16;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_SMOOTH"=17;
							  "AUDPROC_SHOEBOX_MATERIAL_PLYWOOD_PANEL"=18;
							  "AUDPROC_SHOEBOX_MATERIAL_POLISHED_CONCRETE_OR_TILE"=19;
							  "AUDPROC_SHOEBOX_MATERIAL_SHEET_ROCK"=20;
							  "AUDPROC_SHOEBOX_MATERIAL_WATER_OR_ICE_SURFACE"=21;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_ON_CEILING"=22;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_PANEL"=23 }*/

   uint32_t ceiling;

	/**< @h2xmle_description {Specifies the material of ceiling}
		@h2xmle_default     {AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT}
		@h2xmle_rangeList         {"AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT"=0;
                              "AUDPROC_SHOEBOX_MATERIAL_ACOUSTIC_CEILING_TILES"=1;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_BARE"=2;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_PAINTED"=3;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_COARSE"=4;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_PAINTED"=5;
							  "AUDPROC_SHOEBOX_MATERIAL_CURTAIN_HEAVY"=6;
							  "AUDPROC_SHOEBOX_MATERIAL_FIBER_GLASS_INSULATION"=7;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THICK"=8;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THIN"=9;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=10;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=11;
							  "AUDPROC_SHOEBOX_MATERIAL_LINOLEUM_ON_CONCRETE"=12;
							  "AUDPROC_SHOEBOX_MATERIAL_MARBLE"=13;
							  "AUDPROC_SHOEBOX_MATERIAL_METAL"=14;
							  "AUDPROC_SHOEBOX_MATERIAL_PARQUET_ON_CONCRETE"=15;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_ROUGH"=16;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_SMOOTH"=17;
							  "AUDPROC_SHOEBOX_MATERIAL_PLYWOOD_PANEL"=18;
							  "AUDPROC_SHOEBOX_MATERIAL_POLISHED_CONCRETE_OR_TILE"=19;
							  "AUDPROC_SHOEBOX_MATERIAL_SHEET_ROCK"=20;
							  "AUDPROC_SHOEBOX_MATERIAL_WATER_OR_ICE_SURFACE"=21;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_ON_CEILING"=22;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_PANEL"=23 }*/

   uint32_t floor;

	/**< @h2xmle_description {Specifies the material of floor}
		@h2xmle_default     {AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT}
		@h2xmle_rangeList         {"AUDPROC_SHOEBOX_MATERIAL_TRANSPARENT"=0;
                              "AUDPROC_SHOEBOX_MATERIAL_ACOUSTIC_CEILING_TILES"=1;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_BARE"=2;
							  "AUDPROC_SHOEBOX_MATERIAL_BRICK_PAINTED"=3;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_COARSE"=4;
							  "AUDPROC_SHOEBOX_MATERIAL_CONCRETE_BLOCK_PAINTED"=5;
							  "AUDPROC_SHOEBOX_MATERIAL_CURTAIN_HEAVY"=6;
							  "AUDPROC_SHOEBOX_MATERIAL_FIBER_GLASS_INSULATION"=7;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THICK"=8;
							  "AUDPROC_SHOEBOX_MATERIAL_GLASS_THIN"=9;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=10;
							  "AUDPROC_SHOEBOX_MATERIAL_GRASS"=11;
							  "AUDPROC_SHOEBOX_MATERIAL_LINOLEUM_ON_CONCRETE"=12;
							  "AUDPROC_SHOEBOX_MATERIAL_MARBLE"=13;
							  "AUDPROC_SHOEBOX_MATERIAL_METAL"=14;
							  "AUDPROC_SHOEBOX_MATERIAL_PARQUET_ON_CONCRETE"=15;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_ROUGH"=16;
							  "AUDPROC_SHOEBOX_MATERIAL_PLASTER_SMOOTH"=17;
							  "AUDPROC_SHOEBOX_MATERIAL_PLYWOOD_PANEL"=18;
							  "AUDPROC_SHOEBOX_MATERIAL_POLISHED_CONCRETE_OR_TILE"=19;
							  "AUDPROC_SHOEBOX_MATERIAL_SHEET_ROCK"=20;
							  "AUDPROC_SHOEBOX_MATERIAL_WATER_OR_ICE_SURFACE"=21;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_ON_CEILING"=22;
							  "AUDPROC_SHOEBOX_MATERIAL_WOOD_PANEL"=23 } */

}
#include "adsp_end_pack.h"
;

#define AUDPROC_PARAM_ID_SHOEBOX_GAIN                    0x0001093D

/* Structure for the gain parameter of Shoebox module. */
typedef struct audproc_shoebox_gain_t audproc_shoebox_gain_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_SHOEBOX_GAIN", AUDPROC_PARAM_ID_SHOEBOX_GAIN}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_SHOEBOX_GAIN parameter used by the
 Shoebox module.}
 @h2xmlp_version     {0} */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_SHOEBOX_GAIN parameter used by the
 Shoebox module.
 */
struct audproc_shoebox_gain_t
{
   uint32_t version;
	/**< @h2xmle_description {Specifies the version information of this parameter.}
		@h2xmle_default     {0}
		@h2xmle_readonly	{true}
        @h2xmle_isVersion    {true}
        */

   uint32_t reverb_gain;
   /**< @h2xmle_description  {Specifies Gain adjustment to the environment reverb effect}
        @h2xmle_default      {65536}
        @h2xmle_range        {0..4294967295}
		@h2xmle_dataFormat   {Q16}  */
}
#include "adsp_end_pack.h"
;

#define AUDPROC_PARAM_ID_SHOEBOX_DECAY_TIME_ADJUST              0x0001093E

/* Structure for the decay time adjust parameter of Shoebox module. */
typedef struct audproc_shoebox_decay_time_adjust_t audproc_shoebox_decay_time_adjust_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_SHOEBOX_DECAY_TIME_ADJUST", AUDPROC_PARAM_ID_SHOEBOX_DECAY_TIME_ADJUST}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_SHOEBOX_DECAY_TIME_ADJUST parameter used by the Shoebox module.}
    @h2xmlp_version     {0}*/
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_SHOEBOX_DECAY_TIME_ADJUST parameter used by the
 Shoebox module.
 */
struct audproc_shoebox_decay_time_adjust_t
{
   uint32_t version;
  /**< @h2xmle_description {Specifies the version information of this parameter.}
		@h2xmle_default     {0}
		@h2xmle_readonly	{true}
        @h2xmle_isVersion    {true}        */

   uint32_t decay_time_adjust;
   /**< @h2xmle_description  {Specifies Adjustment to RT60 time.}
        @h2xmle_default      {65536}
        @h2xmle_range        {0..4294967295}
		@h2xmle_dataFormat   {Q16}  */

}
#include "adsp_end_pack.h"
;

#define AUDPROC_PARAM_ID_SHOEBOX_BRIGHTNESS_ADJUST              0x0001093F

/* Structure for the brightness adjust parameter of Shoebox module. */
typedef struct audproc_shoebox_brightness_adjust_t audproc_shoebox_brightness_adjust_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_SHOEBOX_BRIGHTNESS_ADJUST", AUDPROC_PARAM_ID_SHOEBOX_BRIGHTNESS_ADJUST}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_SHOEBOX_BRIGHTNESS_ADJUST parameter used by the
 Shoebox module.}
  @h2xmlp_version     {0}
 */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_SHOEBOX_BRIGHTNESS_ADJUST parameter used by the
 Shoebox module.
 */
struct audproc_shoebox_brightness_adjust_t
{
   uint32_t version;
   /**< @h2xmle_description {Specifies the version information of this parameter.}
		@h2xmle_default     {0}
		@h2xmle_readonly	{true}
        @h2xmle_isVersion    {true}*/

   uint32_t brightness_adjust;
   /**< @h2xmle_description  {Specifies The balance between the low and high frequency components of the Shoebox reverberation.}
        @h2xmle_default      {65536}
        @h2xmle_range        {0..4294967295}
		@h2xmle_dataFormat   {Q16}  */
}
#include "adsp_end_pack.h"
;

/**
	@h2xml_Select					{audproc_lib_version_t}
    @h2xmlm_InsertParameter
*/

/**  @}                   <-- End of the Module -->*/
#endif
