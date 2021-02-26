#ifndef CLOCKBSP_H
#define CLOCKBSP_H
/**
==============================================================================

  @file ClockBSP.h

  Internal header file for the BSP data structures.

  Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential.

==============================================================================


==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockBSP.h#3 $

  when      who     what, where, why
  --------  ------  ------------------------------------------------------
  06/30/17     vg   Support QUP V3 clock set frequency for 19.2Mhz
  02/08/17  vphan   Support QUP V3 clock set frequency
  11/11/16  vphan   Initial SDM845 revision, branched from MSM8998.

==============================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBoot.h"
#include "ClockDriver.h"

/*=========================================================================
      References
==========================================================================*/

/*=========================================================================
      Definitions
==========================================================================*/

/*=========================================================================
      Type Definitions
==========================================================================*/
#define QUPV3_PERF_NUM 7


/* Data structure for configuration data */
typedef struct
{
  /* PLL configurations */
  ClockConfigPLLType PLL0_Cfg;
  ClockConfigPLLType PLL1_Cfg;
  ClockConfigPLLType PLL2_Cfg;
  ClockConfigPLLType PLL3_Cfg;
  ClockConfigPLLType PLL4_Cfg;
  ClockConfigPLLType PLL5_Cfg;
  ClockConfigPLLType PLL6_Cfg;
  ClockConfigPLLType AOSS_PLL0_Cfg;

  /* CPU & L3 config data */
  ClockAPCSCfgType CPU_Cfg[CLOCK_BOOT_PERF_NUM];
  ClockAPCSCfgType L3_Cfg[CLOCK_BOOT_PERF_NUM];

  /* System NOC config data */
  ClockConfigMuxType SNOC_Cfg[CLOCK_BOOT_PERF_NUM];
  ClockConfigMuxType SNOC_GC_Cfg[CLOCK_BOOT_PERF_NUM];
  ClockConfigMuxType SNOC_SF_Cfg[CLOCK_BOOT_PERF_NUM];
  
  /* Aggre NOCs */
  ClockConfigMuxType AGNOC_Cfg[CLOCK_BOOT_PERF_NUM];
  ClockConfigMuxType AGNOC_EAST_Cfg[CLOCK_BOOT_PERF_NUM];
  ClockConfigMuxType AGNOC_NORTH_Cfg[CLOCK_BOOT_PERF_NUM];

  /* Config NOC config data */
  ClockConfigMuxType CNOC_Cfg[CLOCK_BOOT_PERF_NUM];

  /* Config-peripheral NOC config data */
  ClockConfigMuxType CNOC_Periph_Cfg[CLOCK_BOOT_PERF_NUM];

  /* CPUSS AXI config data */
  ClockConfigMuxType CPU_AXI_Cfg[CLOCK_BOOT_PERF_NUM];

  /* Turing AXI config data */
  ClockConfigMuxType TURING_AXI_Cfg[CLOCK_BOOT_PERF_NUM];

  /* MemNOC config data */
  ClockConfigMuxType MEMNOC_Cfg[CLOCK_BOOT_PERF_NUM];

  /* GPU MemNOC config data */
  ClockConfigMuxType GPU_MEMNOC_Cfg[CLOCK_BOOT_PERF_NUM];

  /* Q6 MemNOC config data */
  ClockConfigMuxType Q6_MEMNOC_Cfg[CLOCK_BOOT_PERF_NUM];

  /* IPA config data */
  ClockConfigMuxType IPA_Cfg[CLOCK_BOOT_PERF_NUM];

  /* Crypto Engine (CE) config data */
  ClockConfigMuxType CE_Cfg[CLOCK_BOOT_PERF_NUM];

  /* UFS */
  ClockConfigMuxType UFS_Cfg[CLOCK_BOOT_PERF_NUM];
  ClockConfigMuxType UFS_Ice_Cfg[CLOCK_BOOT_PERF_NUM];
  ClockConfigMuxType UFS_Unipro_Cfg[CLOCK_BOOT_PERF_NUM];

  /* Core2x QUP V3 config data */
  ClockConfigMuxType QUPV3Core2X_Cfg[CLOCK_BOOT_PERF_NUM];

  /* QUP V3 configurations */
  ClockFreqMuxCfgType QUPV3_Cfg[QUPV3_PERF_NUM];

  /* SDC table (for backwards compatibility) */
  uint32 SDC_Cfg[CLOCK_BOOT_PERF_NUM];

  /* SDC extended configurations */
  ClockFreqMuxCfgType SDC1_Ext_Cfg[7];
  ClockFreqMuxCfgType SDC2_Ext_Cfg[6];
  ClockFreqMuxCfgType SDC4_Ext_Cfg[5];


  /* SPI configurations */
  ClockFreqMuxCfgType SPI_Cfg[3];

  /* aClockMapCorner Used for Mapping the clock Driver Voltage corner ENUMs to Railway Driver Voltage Corner ENUMs */
  railway_corner aClockMapCorner[CLOCK_BOOT_PERF_NUM];
} Clock_ConfigType;

/* Data structure for Railway data */
typedef struct
{
  char*             CxRail;
  int               nCxRailId;
  railway_voter_t   CxVoter;
  char*             EBIRail;
  int               nEBIRailId;
  railway_voter_t   EBIVoter;
}Clock_RailwayType;

typedef enum
{
  CLOCK_RAIL_CX,
  CLOCK_RAIL_MX,
  CLOCK_RAIL_APC,
  CLOCK_RAIL_NUM,
}ClockRailType;

typedef enum
{
  CLOCK_RAIL_CLIENT_BOOT,
  CLOCK_RAIL_CLIENT_DDR,
  CLOCK_RAIL_CLIENT_NUM,
}ClockRailClinet;

extern Clock_ConfigType *Clock_Config( void );
extern Clock_RailwayType *Clock_RailwayConfig( void );
extern boolean Clock_EnableSource( ClockSourceType eSource );
extern boolean Clock_DisableSource( ClockSourceType eSource );
extern boolean Clock_ConfigureSource( ClockSourceType eSource );
extern boolean Clock_SwitchRail(ClockRailType eRail, railway_corner corner_vote, ClockRailClinet eClient);
boolean Clock_SourceMapToMux
(
  const ClockConfigMuxType *pConfig,
  uint32 *nMuxValue
);

#endif /* !CLOCKBSP_H */

