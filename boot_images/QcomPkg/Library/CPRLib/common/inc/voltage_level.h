/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef VOLTAGE_LEVEL_H
#define VOLTAGE_LEVEL_H
/*
 * Volatge Level defiantion for RPM rails
 *
 *
 * Notes for addign new corners into the Enum::::
 * FIXED ENUM definitions: DO NOT ENUM values for below SVS/NOM/TURBO 
 * RAIL_VOLTAGE_LEVEL_SVS     = 128 (0x080)
 * RAIL_VOLTAGE_LEVEL_NOMINAL = 256 (0x100)
 * RAIL_VOLTAGE_LEVEL_TURBO   = 384 (0x180)
 * 
 * Please don't change current exiting enum values for the defiend corners. 
 * Newly added corners needs to be defined with new enum value without changing the existing values.
 *
 * Please check with Power/ System driver folks before udpating the header file 
 * */

typedef enum
{
  RAIL_VOLTAGE_LEVEL_OFF            = 0,

  //RETENTION Volatge Levels
  RAIL_VOLTAGE_LEVEL_RETENTION      = 0x010,
  RAIL_VOLTAGE_LEVEL_RETENTION_HIGH = 0x020,

  //SVS Volatge Levels
  RAIL_VOLTAGE_LEVEL_SVS_MIN        = 0x030,
  RAIL_VOLTAGE_LEVEL_SVS_LOW        = 0x040,
  RAIL_VOLTAGE_LEVEL_SVS            = 0x080,
  RAIL_VOLTAGE_LEVEL_SVS_L1       	= 0x0C0,

  //NOMINAL Volatge Levels
  RAIL_VOLTAGE_LEVEL_NOM        	= 0x100,
  RAIL_VOLTAGE_LEVEL_NOM_L1 	  	= 0x140,
  RAIL_VOLTAGE_LEVEL_NOM_L2         = 0x150, 


  //TURBO Volatge Levels
  RAIL_VOLTAGE_LEVEL_TUR	        = 0x180,
  RAIL_VOLTAGE_LEVEL_TUR_L1         = 0x1A0, 

  RAIL_VOLTAGE_LEVEL_MAX            = 0x7FFFFFF
} rail_voltage_level;


#endif // VOLTAGE_LEVEL_H

