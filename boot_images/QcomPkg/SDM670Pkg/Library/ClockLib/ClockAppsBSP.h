#ifndef __CLOCKAPPSBSP_H__
#define __CLOCKAPPSBSP_H__
/*
===========================================================================
*/
/**
  @file ClockAppsBSP.h

  Header description for the SDM670 apps clock driver BSP format.
*/
/*
  ====================================================================

  Copyright (c) 2010-2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockLib/ClockAppsBSP.h#2 $
  $DateTime: 2017/09/29 23:30:49 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "Drivers/ClockDxe/ClockBSP.h"
#include "Drivers/ClockDxe/ClockDriver.h"
#include "../HALclkLib/HALclkAPCS.h"


/*=========================================================================
      Macros
==========================================================================*/

#define PEP_CLOCK_PPP_VOTER_ID  0x00000003
#define PEP_INITIAL_GFX_VOLTAGE 905000      // NOMINAL GFX voltage in uV

#define CLOCK_OSM_LUT_MAX_ROWS  OSM_LUT_MAX_ROWS
#define CLOCK_GFX_DATA_MAX_ROWS 16


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * Enumeration of CPU elements.
 */
typedef enum
{
  CLOCK_APPS_CPU_C0  = HAL_CLK_APCS_CPU_CLUSTER0_C0,
  CLOCK_APPS_CPU_C1  = HAL_CLK_APCS_CPU_CLUSTER0_C1,
  CLOCK_APPS_CPU_C2  = HAL_CLK_APCS_CPU_CLUSTER0_C2,
  CLOCK_APPS_CPU_C3  = HAL_CLK_APCS_CPU_CLUSTER0_C3,
  CLOCK_APPS_CPU_C4  = HAL_CLK_APCS_CPU_CLUSTER0_C4,
  CLOCK_APPS_CPU_C5  = HAL_CLK_APCS_CPU_CLUSTER0_C5,
  CLOCK_APPS_CPU_C6  = HAL_CLK_APCS_CPU_CLUSTER1_C0,
  CLOCK_APPS_CPU_C7  = HAL_CLK_APCS_CPU_CLUSTER1_C1,
  CLOCK_APPS_CPU_L3  = HAL_CLK_APCS_CPU_L3,
  CLOCK_APPS_CPU_TOTAL,
  CLOCK_APPS_CPU_CLUSTER_0_TOTAL = CLOCK_APPS_CPU_C6,
  CLOCK_APPS_CPU_CLUSTER_1_TOTAL = CLOCK_APPS_CPU_L3,
  CLOCK_APPS_CPU_L2  = CLOCK_APPS_CPU_L3,

  CLOCK_ENUM_32BITS(APPS_CPU)
} ClockAppsCPUType;

/**
 * Enumeration of CPU Cluster elements.
 */
typedef enum
{
  CLOCK_APPS_CPU_CLUSTER_C0,
  CLOCK_APPS_CPU_CLUSTER_C1,
  CLOCK_APPS_CPU_CLUSTER_L3,
  CLOCK_APPS_CPU_CLUSTER_TOTAL,

  CLOCK_ENUM_32BITS(APPS_CPU_CLUSTER)
} ClockAppsClusterType;

/*
 * Enumeration of performance levels.
 */
enum
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
  CLOCK_APPS_PERF_LEVEL_TOTAL
};

/*
 * Corner voltage data.
 */
typedef struct
{
  uint32              nUVMin;                   /* voltage floor (uV)                     */
  uint32              nUVMax;                   /* voltage ceiling (uV)                   */
  ClockCPR3CornerType eCornerType;              /* CPR Corner Type enum                   */
  uint32              nFreqKHz;                 /* corresponding frequency (KHz)          */
  uint32              nInitialVoltageUV;        /* initial voltage (uV)                   */
  uint32              nUVMinAdj;                /* Max Floor to Ceiling Range (uV)        */
  int                 nOLVAdjUV;                /* CPR Open Loop Adjustment (uV)          */
  int                 nCLVAdjUV;                /* CPR Closed Loop Adjustment (uV)        */
  uint32              nVC;                      /* virtual corner from the voltage plan   */
} ClockVRegCornerDataType;

/*
 * ClockAppsPerfLevelConfigType
 *
 * Configuration parameters for a performance level.
 *
 *  nFreqHz             - Frequency in Hz for this configuration.
 *  HWVersion           - Version of the chip HW this configuration is for.
 *  nVRegAPCCornerIndex - Index to APC rail voltage corner for CPR control.
 */
typedef struct
{
  uint32             nFreqHz;
  ClockHWVersionType HWVersion;
  uint32             nVRegAPCCornerIndex;
} ClockAppsPerfLevelConfigType;

/*
 * Actual format for the data stored in the BSP.
 */
typedef struct
{
  uint32                        nMinPerfLevel;
  uint32                        nMaxPerfLevel;
  uint32                        anPerfLevel[CLOCK_APPS_PERF_LEVEL_TOTAL];
  ClockAppsPerfLevelConfigType  aPerfConfig[CLOCK_APPS_PERF_LEVEL_TOTAL];
} ClockAppsBSPCPUConfigType;

/*
 * CPU BSP type.
 */
typedef struct
{
  const char                    *szName;
  ClockAppsBSPCPUConfigType     *pConfig;
  ClockVRegCornerDataType       *pVRegCornerData;
} ClockAppsBSPCPUType;

/*
 * GPU BSP type.
 */
typedef struct
{
  ClockVRegCornerDataType       *pVRegCornerData;
} ClockAppsBSPGPUType;

/*
 * OSM type.
 */
typedef struct
{
  uint32        nIndex;
  uint32        nVlvl;
  uint32        nVirtualCorner;
  uint32        nOpenLoopVoltage;
  uint32        nFinalFrequency;
  uint32        nPostDivideRatio;
  uint32        nClockSource;
  uint32        nMaxCoreCount;
  uint32        nDroopCode;
  uint32        nDcvsSafeFrequency;
  uint32        nCoreCountSafeFrequency;
  uint32        nMemAccLevel;
  uint32        nSpares;
} ClockOsmLutType;

/**
 * OSM context.
 */
typedef struct
{
  boolean bVminReduction;
  boolean bPerformanceBoost;
  uint32  nBoostDeltaHz;
  uint8   nAPMCrossoverVC;
  uint8   nAPMCrossoverHopVC;
  uint8   nMEMACCCrossoverVC;
  uint8   nMEMACCCrossoverHopVC;
  uint8   nFirstNominalVC;
  uint8   nFirstTurboVC;
  uint8   nFirstTurboLVal;
  uint16  nCrossoverVoltageAPM;
  uint16  nCrossoverVoltageMEMACC;
} ClockOsmCtxtType;

/*
 * Apps BSP type.
 */
typedef struct
{
  ClockAppsBSPCPUType aCPUConfig[CLOCK_APPS_CPU_CLUSTER_TOTAL];
  ClockOsmLutType     aClockOsmLut[CLOCK_APPS_CPU_TOTAL][CLOCK_OSM_LUT_MAX_ROWS];
  ClockOsmCtxtType    aClockOsmCtxt[CLOCK_APPS_CPU_TOTAL];
} ClockAppsBSPType;

/* =========================================================================
**  Function : Clock_GetMaxCprVc
** =========================================================================*/
/*
  Returns the highest CPR Virtual Corner eligible for the Vmin Reduction feature.

  @param eCPU [in] -- OSM instance.

  @return
  Highest CPR Virtual Corner eligible for Vmin Reduction feature.

  @dependencies
  None.
*/

uint32 Clock_GetMaxCprVc
(
  ClockAppsCPUType  eCPU
);

/* =========================================================================
**  Function : Clock_GetMinCprVc
** =========================================================================*/
/*
  Returns the lowest CPR Virtual Corner eligible for the Vmin Reduction feature.

  @param eCPU [in] -- OSM instance.

  @return
  Lowest CPR Virtual Corner eligible for Vmin Reduction feature.

  @dependencies
  None.
*/

uint32 Clock_GetMinCprVc
(
  ClockAppsCPUType  eCPU
);

/* =========================================================================
**  Function : Clock_GetBoostDeltaHz
** =========================================================================*/
/**
  Returns the Performance Boost delta above the guaranteed OSM F-Max.

  @param eCPU [in] -- OSM instance.

  @return
  Performance Boost delta value in Hz.

  @dependencies
  None.
*/

uint32 Clock_GetBoostDeltaHz
(
  ClockAppsCPUType  eCPU
);

/* =========================================================================
**  Function : Clock_RemoveBoostPerfLevels
** =========================================================================*/
/**
  Scans through the OSM LUT table and removes all Boost Perf Levels, if any.

  @param eCPU [in] -- OSM instance.

  @return
  None.

  @dependencies
  None.
*/

DALResult Clock_RemoveBoostPerfLevels
(
  ClockAppsCPUType  eCPU
);

/* =========================================================================
**  Function : Clock_InitAppsBSP
** =========================================================================*/
/**
  Initializes the Apps BSP data.

  Initializes the Apps BSP data by doing any necessary runtime detection based
  on information such as: chip ID, chip version, PVS bins, and speed bins.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- BSP initialized successfully.
  DAL_ERROR -- BSP initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitAppsBSP
(
  ClockDrvCtxt *pDrvCtxt
);

/* =========================================================================
**  Function : Clock_GetAppsBSP
** =========================================================================*/
/**
  Returns a pointer to the Apps BSP data.

  @return
  Pointer to the BSP data.

  @dependencies
  None.
*/

ClockAppsBSPType *Clock_GetAppsBSP
(
  void
);

#endif  /* __CLOCKAPPSBSP_H__ */

