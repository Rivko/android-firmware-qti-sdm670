/*
===========================================================================
*/
/**
  @file ClockAppsBSP.c

  BSP data for the SDM670 apps clock driver.
*/
/*
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockLib/ClockAppsBSP.c#2 $
  $DateTime: 2017/10/16 00:39:36 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockAppsBSP.h"
#include "ClockApps.h"
#include "HALhwio.h"
#include "../HALclkLib/HALclkHWIO.h"

#ifdef WIN8EA
#ifndef CONSOLE_DEBUGGING
#include "PEP_wpp.h"
#include "ClockAppsBSP.tmh"
#endif
#include "PepVpp.h"
#include "ClockWinTarget.h"
#endif

/*=========================================================================
      Externals
==========================================================================*/

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * OSM definitions
 */
#define CLOCK_OSM_MAX_VC          0x3F
#define CLOCK_OSM_MAX_LVAL        0xFF
#define CLOCK_OSM_MAX_VOLTAGE     0xFFFF


/*=========================================================================
      Macro Definitions
==========================================================================*/

/*=========================================================================
      Data
==========================================================================*/

/*
 * Performance level configuration data for the Silver Cluster clock.
 * This table is a placeholder and is updated at runtime.
 */
static ClockAppsBSPCPUConfigType ClockAppsBSPConfigSilverCluster =
{
  /*
   * Define performance levels.
   */
  CLOCK_APPS_PERF_LEVEL_0,
  CLOCK_APPS_PERF_LEVEL_16,

  /*
   * Define default performance level mappings.
   */
  {
    CLOCK_APPS_PERF_LEVEL_0,
    CLOCK_APPS_PERF_LEVEL_1,
    CLOCK_APPS_PERF_LEVEL_2,
    CLOCK_APPS_PERF_LEVEL_3,
    CLOCK_APPS_PERF_LEVEL_4,
    CLOCK_APPS_PERF_LEVEL_5,
    CLOCK_APPS_PERF_LEVEL_6,
    CLOCK_APPS_PERF_LEVEL_7,
    CLOCK_APPS_PERF_LEVEL_8,
    CLOCK_APPS_PERF_LEVEL_9,
    CLOCK_APPS_PERF_LEVEL_10,
    CLOCK_APPS_PERF_LEVEL_11,
    CLOCK_APPS_PERF_LEVEL_12,
    CLOCK_APPS_PERF_LEVEL_13,
    CLOCK_APPS_PERF_LEVEL_14,
    CLOCK_APPS_PERF_LEVEL_15,
    CLOCK_APPS_PERF_LEVEL_16,
    CLOCK_APPS_PERF_LEVEL_17,
    CLOCK_APPS_PERF_LEVEL_18,
    CLOCK_APPS_PERF_LEVEL_19,
    CLOCK_APPS_PERF_LEVEL_20,
    CLOCK_APPS_PERF_LEVEL_21,
    CLOCK_APPS_PERF_LEVEL_22,
    CLOCK_APPS_PERF_LEVEL_23,
    CLOCK_APPS_PERF_LEVEL_24,
    CLOCK_APPS_PERF_LEVEL_25,
    CLOCK_APPS_PERF_LEVEL_26,
    CLOCK_APPS_PERF_LEVEL_27,
    CLOCK_APPS_PERF_LEVEL_28,
    CLOCK_APPS_PERF_LEVEL_29,
    CLOCK_APPS_PERF_LEVEL_30,
    CLOCK_APPS_PERF_LEVEL_31,
    CLOCK_APPS_PERF_LEVEL_32,
    CLOCK_APPS_PERF_LEVEL_33,
    CLOCK_APPS_PERF_LEVEL_34,
    CLOCK_APPS_PERF_LEVEL_35,
    CLOCK_APPS_PERF_LEVEL_36,
    CLOCK_APPS_PERF_LEVEL_37,
    CLOCK_APPS_PERF_LEVEL_38,
    CLOCK_APPS_PERF_LEVEL_39,
  },

  /*
   * Configuration data.  Each entry should match the corresponding value
   * in the enumeration CLOCK_APPS0_CONFIG_xxx.
   */
  {
    /*  nFreqHz---------| HWVersion--------------------------------------------| nVRegAPCCornerIndex-| */
    {  300000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 0,                  },
    {  422400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 1,                  },
    {  499200 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 2,                  },
    {  576000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 3,                  },
    {  652800 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 4,                  },
    {  748800 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 5,                  },
    {  825600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 6,                  },
    {  902400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 7,                  },
    {  979200 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 8,                  },
    { 1056000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 9,                  },
    { 1132800 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 10,                 },
    { 1209600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 11,                 },
    { 1286400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 12,                 },
    { 1363200 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 13,                 },
    { 1440000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 14,                 },
    { 1516800 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 15,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 16,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 17,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 18,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 19,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 20,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 21,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 22,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 23,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 24,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 25,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 26,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 27,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 28,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 29,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 30,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 31,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 32,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 33,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 34,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 35,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 36,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 37,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 38,                 },
    { 1593600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 39,                 },
  }
};


/*
 * Performance level configuration data for the Gold Cluster clock.
 * This table is a placeholder and is updated at runtime.
 */
static ClockAppsBSPCPUConfigType ClockAppsBSPConfigGoldCluster =
{
  /*
   * Define performance levels.
   */
  CLOCK_APPS_PERF_LEVEL_0,
  CLOCK_APPS_PERF_LEVEL_21,

  /*
   * Define default performance level mappings.
   */
  {
    CLOCK_APPS_PERF_LEVEL_0,
    CLOCK_APPS_PERF_LEVEL_1,
    CLOCK_APPS_PERF_LEVEL_2,
    CLOCK_APPS_PERF_LEVEL_3,
    CLOCK_APPS_PERF_LEVEL_4,
    CLOCK_APPS_PERF_LEVEL_5,
    CLOCK_APPS_PERF_LEVEL_6,
    CLOCK_APPS_PERF_LEVEL_7,
    CLOCK_APPS_PERF_LEVEL_8,
    CLOCK_APPS_PERF_LEVEL_9,
    CLOCK_APPS_PERF_LEVEL_10,
    CLOCK_APPS_PERF_LEVEL_11,
    CLOCK_APPS_PERF_LEVEL_12,
    CLOCK_APPS_PERF_LEVEL_13,
    CLOCK_APPS_PERF_LEVEL_14,
    CLOCK_APPS_PERF_LEVEL_15,
    CLOCK_APPS_PERF_LEVEL_16,
    CLOCK_APPS_PERF_LEVEL_17,
    CLOCK_APPS_PERF_LEVEL_18,
    CLOCK_APPS_PERF_LEVEL_19,
    CLOCK_APPS_PERF_LEVEL_20,
    CLOCK_APPS_PERF_LEVEL_21,
    CLOCK_APPS_PERF_LEVEL_22,
    CLOCK_APPS_PERF_LEVEL_23,
    CLOCK_APPS_PERF_LEVEL_24,
    CLOCK_APPS_PERF_LEVEL_25,
    CLOCK_APPS_PERF_LEVEL_26,
    CLOCK_APPS_PERF_LEVEL_27,
    CLOCK_APPS_PERF_LEVEL_28,
    CLOCK_APPS_PERF_LEVEL_29,
    CLOCK_APPS_PERF_LEVEL_30,
    CLOCK_APPS_PERF_LEVEL_31,
    CLOCK_APPS_PERF_LEVEL_32,
    CLOCK_APPS_PERF_LEVEL_33,
    CLOCK_APPS_PERF_LEVEL_34,
    CLOCK_APPS_PERF_LEVEL_35,
    CLOCK_APPS_PERF_LEVEL_36,
    CLOCK_APPS_PERF_LEVEL_37,
    CLOCK_APPS_PERF_LEVEL_38,
    CLOCK_APPS_PERF_LEVEL_39,
  },

  /*
   * Configuration data.  Each entry should match the corresponding value
   * in the enumeration CLOCK_APPS1_CONFIG_xxx.
   */
  {
    /*  nFreqHz---------| HWVersion--------------------------------------------| nVRegAPCCornerIndex-| */
    {  300000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 0,                  },
    {  422400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 1,                  },
    {  499200 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 2,                  },
    {  576000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 3,                  },
    {  652800 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 4,                  },
    {  729600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 5,                  },
    {  806400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 6,                  },
    {  883200 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 7,                  },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 8,                  },
    { 1036800 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 9,                  },
    { 1113600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 10,                 },
    { 1190400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 11,                 },
    { 1267200 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 12,                 },
    { 1344000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 13,                 },
    { 1420800 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 14,                 },
    { 1497600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 15,                 },
    { 1574400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 16,                 },
    { 1651200 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 17,                 },
    { 1728000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 18,                 },
    { 1804800 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 19,                 },
    { 1881600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 20,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 21,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 22,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 23,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 24,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 25,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 26,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 27,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 28,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 29,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 30,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 31,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 32,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 33,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 34,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 35,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 36,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 37,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 38,                 },
    { 1958400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 39,                 },
  }
};


/*
 * Performance level configuration data for the L3 clock.
 * This table is a placeholder and is updated at runtime.
 */
static ClockAppsBSPCPUConfigType ClockAppsBSPConfigL3 =
{
  /*
   * Define performance levels.
   */
  CLOCK_APPS_PERF_LEVEL_0,
  CLOCK_APPS_PERF_LEVEL_8,

  /*
   * Define default performance level mappings.
   */
  {
    CLOCK_APPS_PERF_LEVEL_0,
    CLOCK_APPS_PERF_LEVEL_1,
    CLOCK_APPS_PERF_LEVEL_2,
    CLOCK_APPS_PERF_LEVEL_3,
    CLOCK_APPS_PERF_LEVEL_4,
    CLOCK_APPS_PERF_LEVEL_5,
    CLOCK_APPS_PERF_LEVEL_6,
    CLOCK_APPS_PERF_LEVEL_7,
    CLOCK_APPS_PERF_LEVEL_8,
    CLOCK_APPS_PERF_LEVEL_9,
    CLOCK_APPS_PERF_LEVEL_10,
    CLOCK_APPS_PERF_LEVEL_11,
    CLOCK_APPS_PERF_LEVEL_12,
    CLOCK_APPS_PERF_LEVEL_13,
    CLOCK_APPS_PERF_LEVEL_14,
    CLOCK_APPS_PERF_LEVEL_15,
    CLOCK_APPS_PERF_LEVEL_16,
    CLOCK_APPS_PERF_LEVEL_17,
    CLOCK_APPS_PERF_LEVEL_18,
    CLOCK_APPS_PERF_LEVEL_19,
    CLOCK_APPS_PERF_LEVEL_20,
    CLOCK_APPS_PERF_LEVEL_21,
    CLOCK_APPS_PERF_LEVEL_22,
    CLOCK_APPS_PERF_LEVEL_23,
    CLOCK_APPS_PERF_LEVEL_24,
    CLOCK_APPS_PERF_LEVEL_25,
    CLOCK_APPS_PERF_LEVEL_26,
    CLOCK_APPS_PERF_LEVEL_27,
    CLOCK_APPS_PERF_LEVEL_28,
    CLOCK_APPS_PERF_LEVEL_29,
    CLOCK_APPS_PERF_LEVEL_30,
    CLOCK_APPS_PERF_LEVEL_31,
    CLOCK_APPS_PERF_LEVEL_32,
    CLOCK_APPS_PERF_LEVEL_33,
    CLOCK_APPS_PERF_LEVEL_34,
    CLOCK_APPS_PERF_LEVEL_35,
    CLOCK_APPS_PERF_LEVEL_36,
    CLOCK_APPS_PERF_LEVEL_37,
    CLOCK_APPS_PERF_LEVEL_38,
    CLOCK_APPS_PERF_LEVEL_39,
  },

  /*
   * Configuration data.  Each entry should match the corresponding value
   * in the enumeration CLOCK_APPS1_CONFIG_xxx.
   */
  {
  /*  nFreqHz---------| HWVersion----------------------------------------------| nVRegAPCCornerIndex-| */
    {  300000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 0,                  },
    {  422400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 1,                  },
    {  499200 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 2,                  },
    {  576000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 3,                  },
    {  652800 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 4,                  },
    {  729600 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 5,                  },
    {  806400 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 6,                  },
    {  883200 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 7,                  },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 8,                  },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 9,                  },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 10,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 11,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 12,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 13,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 14,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 15,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 16,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 17,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 18,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 19,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 20,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 21,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 22,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 23,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 24,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 25,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 26,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 27,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 28,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 29,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 30,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 31,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 32,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 33,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 34,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 35,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 36,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 37,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 38,                 },
    {  960000 * 1000UL, { {0, 0}, {0xFF, 0xFF}, CHIPINFO_FAMILY_UNKNOWN, NULL }, 39,                 },
  }
};


/*
 * Voltage tables -- one per cluster -- populated by vpp
 */
static ClockVRegCornerDataType ClockVRegCornerDataSilverCluster[CLOCK_OSM_LUT_MAX_ROWS];
static ClockVRegCornerDataType ClockVRegCornerDataGoldCluster[CLOCK_OSM_LUT_MAX_ROWS];
static ClockVRegCornerDataType ClockVRegCornerDataL3[CLOCK_OSM_LUT_MAX_ROWS];


/*
 * Apps BSP data.
 */
static ClockAppsBSPType ClockAppsBSPConfig =
{
  /* .aCPUConfig = */
  {
    {
      /* .szName                 = */ "apcs_silver_sysleaf_clk",
      /* .pConfig                = */ &ClockAppsBSPConfigSilverCluster,
      /* .pVRegCornerData        = */ ClockVRegCornerDataSilverCluster
    },
    {
      /* .szName                 = */ "apcs_gold_sysleaf_clk",
      /* .pConfig                = */ &ClockAppsBSPConfigGoldCluster,
      /* .pVRegCornerData        = */ ClockVRegCornerDataGoldCluster
    },
    {
      /* .szName                 = */ "apcs_l3_sysleaf_clk",
      /* .pConfig                = */ &ClockAppsBSPConfigL3,
      /* .pVRegCornerData        = */ ClockVRegCornerDataL3
    },
  },
///* .aClockOsmLut               = */ {0},
///* .aClockOsmCtxt              = */ {0},
};

/*=========================================================================
     Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_ReadOsmLutData
** =========================================================================*/
/*
  Reads the OSM LUT from the OSM hardware.
*/

static DALResult Clock_ReadOsmLutData
(
  ClockDrvCtxt     *pDrvCtxt,
  ClockAppsCPUType  eCPU
)
{
  uint32  nRowNumber;
  uint32  nDummy;

  for (nRowNumber = 0; nRowNumber < CLOCK_OSM_LUT_MAX_ROWS; nRowNumber++)
  {
    if (!HAL_clk_OSM_LUT_READ_FREQUENCY(
          (HAL_clk_APCSCPUType)eCPU,
          nRowNumber,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nIndex,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nClockSource,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nPostDivideRatio,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nDroopCode,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nFinalFrequency))
    {
      return DAL_ERROR;
    }
    if (!HAL_clk_OSM_LUT_READ_VOLTAGE(
          (HAL_clk_APCSCPUType)eCPU,
          nRowNumber,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMemAccLevel,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nVirtualCorner,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nOpenLoopVoltage))
    {
      return DAL_ERROR;
    }
    if (!HAL_clk_OSM_LUT_READ_PLL_OVERRIDES(
          (HAL_clk_APCSCPUType)eCPU,
          nRowNumber,
          &nDummy,  // throw away the 1/LVal value read from LUT -- unnecessary
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nCoreCountSafeFrequency,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nDcvsSafeFrequency))
    {
      return DAL_ERROR;
    }
    if (!HAL_clk_OSM_LUT_READ_SPARE_0(
          (HAL_clk_APCSCPUType)eCPU,
          nRowNumber,
          &ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nSpares))
    {
      return DAL_ERROR;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_ReadOsmLutData */


/* =========================================================================
**  Function : Clock_SaveOsmLutData
** =========================================================================*/
/*
  Saves the OSM LUT data from the hardware and parces it into the frequency
  and voltage tables.
*/

static DALResult Clock_SaveOsmLutData
(
  ClockDrvCtxt     *pDrvCtxt,
  ClockAppsCPUType  eCPU
)
{
  uint32                nRowNumber, nItemNumber, nMaxPublishedPerfLevel = 0;
  ClockAppsClusterType  eCluster;
  boolean               bLessThanFour = FALSE;
  uint32                nPrevFreq = 0, nPrevVoltage = 0;

  switch (eCPU)
  {
    case CLOCK_APPS_CPU_C0:
    case CLOCK_APPS_CPU_C1:
    case CLOCK_APPS_CPU_C2:
    case CLOCK_APPS_CPU_C3:
    case CLOCK_APPS_CPU_C4:
    case CLOCK_APPS_CPU_C5:
      eCluster = CLOCK_APPS_CPU_CLUSTER_C0;
      break;

    case CLOCK_APPS_CPU_C6:
    case CLOCK_APPS_CPU_C7:
      eCluster = CLOCK_APPS_CPU_CLUSTER_C1;
      break;

    case CLOCK_APPS_CPU_L3:
      eCluster = CLOCK_APPS_CPU_CLUSTER_L3;
      break;

    default:
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "DALSYS_LOGEVENT_FATAL_ERROR: Invalid clock domain detected by Clock_SaveOsmLutData.");
      ERR_FATAL("Clock_SaveOsmLutData", 0, 0, 0);
      return DAL_ERROR_INTERNAL;
  }

  /* Count the number of valid performance levels programmed into the OSM LUT. */
  for (nRowNumber = 0; nRowNumber < CLOCK_OSM_LUT_MAX_ROWS; nRowNumber++)
  {
    /* Identify duplicate rows -- frequency and voltage pairs identical to the previous. */
    if (ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nFinalFrequency == nPrevFreq &&
        ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nOpenLoopVoltage == nPrevVoltage)
    {
      break;
    }
    nPrevFreq = ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nFinalFrequency;
    nPrevVoltage = ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nOpenLoopVoltage;
    if (((ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount == 2 && eCluster == CLOCK_APPS_CPU_CLUSTER_C1) || (ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount == 6 && eCluster == CLOCK_APPS_CPU_CLUSTER_C0) || (ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount == 6 && eCluster == CLOCK_APPS_CPU_CLUSTER_L3) )  && !bLessThanFour)
    {
      nMaxPublishedPerfLevel++;
    }
    if ((ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount < 2 && eCluster == CLOCK_APPS_CPU_CLUSTER_C1) ||  (ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount < 6 && eCluster == CLOCK_APPS_CPU_CLUSTER_C0) || (ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount < 6 && eCluster == CLOCK_APPS_CPU_CLUSTER_L3))
    {
      bLessThanFour = TRUE;
    }
  }

  /* Update selected frequency configuration and voltage plan values. */
  for (nRowNumber = nItemNumber = 0; nRowNumber < CLOCK_OSM_LUT_MAX_ROWS; nRowNumber++)
  {
    /* Filter out rows that belong to the Performance Boost feature. */
    if ((ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount == 2 && eCluster == CLOCK_APPS_CPU_CLUSTER_C1) || (ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount == 6 && eCluster == CLOCK_APPS_CPU_CLUSTER_C0) || (ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nMaxCoreCount == 6 && eCluster == CLOCK_APPS_CPU_CLUSTER_L3))
    {
      if (nRowNumber == 0 && ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nFinalFrequency==15) 
      {
        ClockAppsBSPConfig.aCPUConfig[eCluster].pConfig->aPerfConfig[nItemNumber].nFreqHz = 300000000;
      }
      else
      {
        ClockAppsBSPConfig.aCPUConfig[eCluster].pConfig->aPerfConfig[nItemNumber].nFreqHz =
          ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nFinalFrequency * 19200000;
      }
      ClockAppsBSPConfig.aCPUConfig[eCluster].pVRegCornerData[nItemNumber].nFreqKHz =
        ClockAppsBSPConfig.aCPUConfig[eCluster].pConfig->aPerfConfig[nItemNumber].nFreqHz / 1000;
      ClockAppsBSPConfig.aCPUConfig[eCluster].pVRegCornerData[nItemNumber].nInitialVoltageUV =
        ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nOpenLoopVoltage * 1000;
      nItemNumber++;
    }
  }

  /* Make sure that the rest of the table is also updated. */
  for (nRowNumber = nItemNumber; nRowNumber < CLOCK_OSM_LUT_MAX_ROWS; nRowNumber++)
  {
    ClockAppsBSPConfig.aCPUConfig[eCluster].pConfig->aPerfConfig[nRowNumber].nFreqHz =
      ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nFinalFrequency * 19200000;
    ClockAppsBSPConfig.aCPUConfig[eCluster].pVRegCornerData[nRowNumber].nFreqKHz =
      ClockAppsBSPConfig.aCPUConfig[eCluster].pConfig->aPerfConfig[nRowNumber].nFreqHz / 1000;
    ClockAppsBSPConfig.aCPUConfig[eCluster].pVRegCornerData[nRowNumber].nInitialVoltageUV =
      ClockAppsBSPConfig.aClockOsmLut[eCPU][nRowNumber].nOpenLoopVoltage * 1000;
  }

  /* Update the appropriate MAX Perf Level field. */
  if (nMaxPublishedPerfLevel > 0)
  {
    nMaxPublishedPerfLevel--;
  }
  ClockAppsBSPConfig.aCPUConfig[eCluster].pConfig->nMaxPerfLevel = nMaxPublishedPerfLevel;

  return DAL_SUCCESS;

} /* END Clock_SaveOsmLutData */


/* =========================================================================
**  Function : Clock_InitAppsBSP
** =========================================================================*/
/*
  See ClockAppsBSP.h
*/

DALResult Clock_InitAppsBSP
(
  ClockDrvCtxt *pDrvCtxt
)
{
  uint32             nChipVersion, nChipId;
  ClockAppsCPUType   eCPU;

  nChipVersion = HAL_clk_GetChipVersion();
  nChipId = HAL_clk_GetChipId();

  /*-----------------------------------------------------------------------*/
  /* Read data programmed into the Operation State Manager (OSM) by Trust  */
  /* Zone clock driver and recreate the corresponding frequency and        */
  /* voltage plan tables.                                                  */
  /*-----------------------------------------------------------------------*/

  /* Read the OSM hardware to get the values from the OSM LUT. */
  for (eCPU = CLOCK_APPS_CPU_C0; eCPU < CLOCK_APPS_CPU_TOTAL; eCPU++)
  {
    if (Clock_ReadOsmLutData(pDrvCtxt, eCPU) != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
    if (Clock_SaveOsmLutData(pDrvCtxt, eCPU) != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_InitAppsBSP */


/* =========================================================================
**  Function : Clock_GetAppsBSP
** =========================================================================*/
/*
  See ClockAppsBSP.h
*/

ClockAppsBSPType *Clock_GetAppsBSP
(
  void
)
{
  return &ClockAppsBSPConfig;

} /* END Clock_GetAppsBSP */


/* =========================================================================
**  Function : Clock_ReadAppsBSPConfig
** =========================================================================*/
/*
  See ClockApps.h
*/

void Clock_ReadAppsBSPConfig
(
  eConfigParamType eConfigParam,
  uint32           *nValue
)
{
  ClockDrvCtxt  *pDrvCtxt = Clock_GetDrvCtxt();

  switch (eConfigParam)
  {
    case CLOCK_CONFIG_PARAM_GET_VMIN_REDUCTION_STATE_CLUSTER_0:
      *nValue = (uint32)ClockAppsBSPConfig.aClockOsmCtxt[0].bVminReduction;
      break;

    case CLOCK_CONFIG_PARAM_GET_PERFORMANCE_BOOST_STATE_CLUSTER_0:
      *nValue = (uint32)ClockAppsBSPConfig.aClockOsmCtxt[0].bPerformanceBoost;
      break;

    case CLOCK_CONFIG_PARAM_GET_BOOST_DELTA_HZ_CLUSTER_0:
      *nValue = (uint32)ClockAppsBSPConfig.aClockOsmCtxt[0].nBoostDeltaHz;
      break;

    case CLOCK_CONFIG_PARAM_GET_BOOST_PERF_LEVEL_CLUSTER_0:
      *nValue = CLOCK_OSM_LUT_MAX_ROWS - 1;
      break;

    case CLOCK_CONFIG_PARAM_GET_VMIN_REDUCTION_STATE_CLUSTER_1:
      *nValue = (uint32)ClockAppsBSPConfig.aClockOsmCtxt[6].bVminReduction;
      break;

    case CLOCK_CONFIG_PARAM_GET_PERFORMANCE_BOOST_STATE_CLUSTER_1:
      *nValue = (uint32)ClockAppsBSPConfig.aClockOsmCtxt[6].bPerformanceBoost;
      break;

    case CLOCK_CONFIG_PARAM_GET_BOOST_DELTA_HZ_CLUSTER_1:
      *nValue = (uint32)ClockAppsBSPConfig.aClockOsmCtxt[6].nBoostDeltaHz;
      break;

    case CLOCK_CONFIG_PARAM_GET_BOOST_PERF_LEVEL_CLUSTER_1:
      *nValue = CLOCK_OSM_LUT_MAX_ROWS - 1;
      break;

    case CLOCK_CONFIG_PARAM_GET_VMIN_REDUCTION_STATE_L3:
      *nValue = (uint32)ClockAppsBSPConfig.aClockOsmCtxt[8].bVminReduction;
      break;

    case CLOCK_CONFIG_PARAM_GET_PERFORMANCE_BOOST_STATE_L3:
      *nValue = (uint32)ClockAppsBSPConfig.aClockOsmCtxt[8].bPerformanceBoost;
      break;

    case CLOCK_CONFIG_PARAM_GET_BOOST_DELTA_HZ_L3:
      *nValue = (uint32)ClockAppsBSPConfig.aClockOsmCtxt[8].nBoostDeltaHz;
      break;

    case CLOCK_CONFIG_PARAM_GET_BOOST_PERF_LEVEL_L3:
      *nValue = CLOCK_OSM_LUT_MAX_ROWS - 1;
      break;

    case CLOCK_CONFIG_PARAM_INTERPOLATION_MODE:
    case CLOCK_CONFIG_PARAM_DYNAMIC_CEILING_MODE:
    case CLOCK_CONFIG_PARAM_DYNAMIC_FLOOR_MODE:
    case CLOCK_CONFIG_PARAM_PLL_SLEWING_MODE:
    case CLOCK_CONFIG_PARAM_GFX_FULL_RPM_EMULATION_MODE:
    case CLOCK_CONFIG_PARAM_PLL_DROOP_DETECTION_MODE:
    case CLOCK_CONFIG_PARAM_ACD_REDUCTION_MODE:
    case CLOCK_CONFIG_PARAM_APM_CONTROL_MODE:
    case CLOCK_CONFIG_PARAM_LDO_CONTROL_MODE:
    case CLOCK_CONFIG_PARAM_LDO_MAX_VOLTAGE_UV:
    case CLOCK_CONFIG_PARAM_LDO_MIN_VOLTAGE_UV:
    case CLOCK_CONFIG_PARAM_LDO_MIN_HEADROOM_UV:
    case CLOCK_CONFIG_PARAM_LDO_SAFETY_MARGIN_UV:
    case CLOCK_CONFIG_PARAM_LDO_APC_RETENTION_UV:
    case CLOCK_CONFIG_PARAM_GLOBAL_VOLTAGE_LIMIT_UV:
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "DALSYS_LOGEVENT_WARNING: Unused Apps BSP configuration type.");
      break;

    default:
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "DALSYS_LOGEVENT_FATAL_ERROR: Undefined Apps BSP configuration type.");
      ERR_FATAL("Clock_ReadAppsBSPConfig", 0, 0, 0);
      break;
  }

} /* END Clock_ReadAppsBSPConfig */


/* =========================================================================
**  Function : Clock_UpdateAppsBSPConfig
** =========================================================================*/
/*
  See ClockApps.h
*/

void Clock_UpdateAppsBSPConfig
(
  eConfigParamType eConfigParam,
  uint32           nValue
)
{
  ClockDrvCtxt  *pDrvCtxt = Clock_GetDrvCtxt();

  switch (eConfigParam)
  {
    case CLOCK_CONFIG_PARAM_APCS_SHARED_CX_RAIL:
    case CLOCK_CONFIG_PARAM_INTERPOLATION_MODE:
    case CLOCK_CONFIG_PARAM_DYNAMIC_CEILING_MODE:
    case CLOCK_CONFIG_PARAM_DYNAMIC_FLOOR_MODE:
    case CLOCK_CONFIG_PARAM_PLL_SLEWING_MODE:
    case CLOCK_CONFIG_PARAM_GFX_FULL_RPM_EMULATION_MODE:
    case CLOCK_CONFIG_PARAM_PLL_DROOP_DETECTION_MODE:
    case CLOCK_CONFIG_PARAM_ACD_REDUCTION_MODE:
    case CLOCK_CONFIG_PARAM_APM_CONTROL_MODE:
    case CLOCK_CONFIG_PARAM_LDO_CONTROL_MODE:
    case CLOCK_CONFIG_PARAM_LDO_MAX_VOLTAGE_UV:
    case CLOCK_CONFIG_PARAM_LDO_MIN_VOLTAGE_UV:
    case CLOCK_CONFIG_PARAM_LDO_MIN_HEADROOM_UV:
    case CLOCK_CONFIG_PARAM_LDO_SAFETY_MARGIN_UV:
    case CLOCK_CONFIG_PARAM_LDO_APC_RETENTION_UV:
    case CLOCK_CONFIG_PARAM_GLOBAL_VOLTAGE_LIMIT_UV:
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "DALSYS_LOGEVENT_WARNING: Unused Apps BSP configuration type.");
      break;

    default:
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "DALSYS_LOGEVENT_FATAL_ERROR: Undefined Apps BSP configuration type.");
      ERR_FATAL("Clock_UpdateAppsBSPConfig", 0, 0, 0);
      break;
  }

} /* END Clock_UpdateAppsBSPConfig */


/* =========================================================================
**  Function : Clock_MapPerfLevelToFrequency
** =========================================================================*/
/**
  See ClockApps.h
*/

uint32 Clock_MapPerfLevelToFrequency
(
  ClockDrvCtxt     *pDrvCtxt,
  ClockAppsCPUType  eCPU,
  uint32            nPerfLevel
)
{
  ClockAppsCtxtType         *pAppsCtxt = (ClockAppsCtxtType *)pDrvCtxt->pImageCtxt;
  ClockAppsBSPCPUConfigType *pBSPConfig;

  /*-----------------------------------------------------------------------*/
  /* Get the frequency data.                                               */
  /*-----------------------------------------------------------------------*/

  pBSPConfig = pAppsCtxt->aCPU[eCPU].pBSPConfig->pConfig;
  return pBSPConfig->aPerfConfig[pBSPConfig->anPerfLevel[nPerfLevel]].nFreqHz;

} /* END Clock_MapPerfLevelToFrequency */

