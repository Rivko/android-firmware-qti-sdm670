#ifndef CLOCKBSP_H
#define CLOCKBSP_H
/*
===========================================================================
*/
/**
  @file ClockBSP.h

  Internal header file for the BSP data structures.
*/
/*
  ====================================================================

  Copyright (c) 2016-2017 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/20/17   vph     Support cap trace resource at fix voltage corner
  10/07/16   vph     Initial code
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/
#include "clock.h"
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

/* QUP V3 performance levels */
#define CLOCK_QUPV3_PERF_NUM  6
#define CLOCK_QDSS_PERF_NUM   7

typedef enum
{
  RAILWAY_NO_REQUEST         = 0,
  RAILWAY_RETENTION          = 1,
  RAILWAY_SVS_MIN            = 2,
  RAILWAY_SVS_LOW            = 3,
  RAILWAY_SVS                = 4,
  RAILWAY_SVS_HIGH           = 5,
  RAILWAY_NOMINAL            = 6,
  RAILWAY_TURBO              = 7,
  RAILWAY_SUPER_TURBO        = 8,
  RAILWAY_SUPER_TURBO_NO_CPR = 9,
  RAILWAY_CORNERS_COUNT,
} RailWayType;

typedef enum
{
  DFS_SHUB_DFS,
  DFS_SYS_NOC
} DfsType;

typedef struct
{
  DfsType eDfs;
  uint8   nRCGIndex;
} DFSSWType;

typedef struct
{
  ClockQDSSLevelType eLevel;
  RailWayType        eCxRailway;
} Clock_QDSSMapType;

/* Data structure for configuration data */
typedef struct
{
  /* Configurations for QUP V3 */
  ClockConfigMuxType QUPV3Cfg[CLOCK_QUPV3_PERF_NUM];

  /* QDSS Configuration */
  Clock_QDSSMapType  QDSS_Mapping[CLOCK_QDSS_PERF_NUM];
  DFSSWType          QDSS_ATBADfs;
  ClockConfigMuxType QDSS_ATBACfg[CLOCK_QDSS_PERF_NUM];
  DFSSWType          QDSS_ATBBDfs;
  ClockConfigMuxType QDSS_ATBBCfg[CLOCK_QDSS_PERF_NUM];
  DFSSWType          QDSS_ATBCDfs;
  ClockConfigMuxType QDSS_ATBCCfg[CLOCK_QDSS_PERF_NUM];
  DFSSWType          QDSS_STMDfs;
  ClockConfigMuxType QDSS_STMCfg[CLOCK_QDSS_PERF_NUM];
  DFSSWType          QDSS_TRACEDfs;
  ClockConfigMuxType QDSS_TRACECfg[CLOCK_QDSS_PERF_NUM];
  DFSSWType          QDSS_TRIGDfs;
  ClockConfigMuxType QDSS_TRIGCfg[CLOCK_QDSS_PERF_NUM];
} Clock_ConfigType;


/* ============================================================================
**  Function : Clock_GetConfigData
** ============================================================================
*/
/*!
    Get pointer of BSP configuration data

    @param 
      None
    @return
      Pointer of BSP configuration data

    @dependencies
    None.

    @sa None
*/
Clock_ConfigType* Clock_GetConfigData( void );

#endif /* !CLOCKBSP_H */


