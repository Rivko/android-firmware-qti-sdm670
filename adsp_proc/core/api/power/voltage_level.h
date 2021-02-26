/*===========================================================================

  Copyright (c) 2015-2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef VOLTAGE_LEVEL_H
#define VOLTAGE_LEVEL_H
/*
 * Volatge Level defiantion for RPM rails
 *
 * Please don't change current exiting enum values for the defiend corners.
 * Newly added corners needs to be defined with new enum value without changing the existing values.
 *
 * Please check with Power/ System driver folks before udpating the header file 
 * */

typedef enum
{
  RAIL_VOLTAGE_LEVEL_OFF            = 0x0,    // 0
  RAIL_VOLTAGE_LEVEL_RET            = 0x10,   // 16
  RAIL_VOLTAGE_LEVEL_MIN_SVS        = 0x30,   // 48
  RAIL_VOLTAGE_LEVEL_LOW_SVS        = 0x40,   // 64
  RAIL_VOLTAGE_LEVEL_SVS            = 0x80,   // 128
  RAIL_VOLTAGE_LEVEL_SVS_L1         = 0xC0,   // 192
  RAIL_VOLTAGE_LEVEL_NOM            = 0x100,  // 256
  RAIL_VOLTAGE_LEVEL_NOM_L1         = 0x140,  // 320
  RAIL_VOLTAGE_LEVEL_NOM_L2         = 0x150,  // 336
  RAIL_VOLTAGE_LEVEL_TUR            = 0x180,  // 384
  RAIL_VOLTAGE_LEVEL_TUR_L1         = 0x1A0,  // 416

  RAIL_VOLTAGE_LEVEL_MAX            = RAIL_VOLTAGE_LEVEL_TUR_L1,
  RAIL_VOLTAGE_LEVEL_NUM_LEVELS     = 11,
  RAIL_VOLTAGE_LEVEL_INVALID        = -1,
  RAIL_VOLTAGE_LEVEL_OVERLIMIT      = -2
} rail_voltage_level;

#endif // VOLTAGE_LEVEL_H

