#ifndef __CLOCK_IMAGE_H__
#define __CLOCK_IMAGE_H__

/*
===========================================================================
*/
/**
  @file ClockImage.h

*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/image/ClockImage.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/

/*
 * Resource scaling disable flags
 *   BY_USER
 *     Set: At init or runtime via debugger.
 *     Cleared: Never.
 *     Usage: Disable feature (at compile or runtime).
 *   BY_EFUSE
 *     Set: When efuse reads feature not supported.
 *     Cleared: Never.
 *     Usage: Disable feature until efuse is read.
 *   BY_CPU_TEST:
 *     Set: When /clk/cpu.test is active.
 *     Cleared: When /clk/cpu.test transitions to inactive.
 *     Usage: Disable feature while /clk/cpu.test is active.
 *   BY_BSP:
 *     Set: When the BSP flag is set to disable the feature.
 *     Usage: Set when init reads the BSP data.
 */
#define CLOCK_FLAG_DISABLED_BY_USER                (0x1 << 0)
//#define CLOCK_FLAG_DISABLED_BY_x                 (0x1 << 1)
#define CLOCK_FLAG_DISABLED_BY_EFUSE               (0x1 << 2)
#define CLOCK_FLAG_DISABLED_BY_CPU_TEST            (0x1 << 3)
#define CLOCK_FLAG_DISABLED_BY_BSP                 (0x1 << 4)


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * Image BSP type.
 */
typedef struct
{
  const char        *szCPUClockName;  /**< CPU clock name. */
  npa_resource_state nInitCPUState;   /**< Initial CPU state (0 to leave unchanged). */
  boolean            bDisableDCS;     /**< Disable dynamic CPU frequency scaling. */
} ClockImageBSPType;


/**
 * CPU context type.
 */
typedef struct
{
  ClockNodeType       *pClock;            /**< CPU clock. */
  ClockMuxConfigType  *pNASConfig;        /**< Pending NAS configuration. */
  uint32               nNASRequestCount;  /**< Number of active NAS requests. */
  uint32               nDisableDCS;       /**< Mask of flags disabling dynamic CPU frequency scaling. */
} ClockCPUCtxtType;


/**
 * Image context type.
 */
typedef struct
{
  ClockImageBSPType   *pBSP;        /**< Image BSP data. */
  void                *pTargetCtxt; /**< Target-specific context. */
  ClockCPUCtxtType     CPUCtxt;     /**< CPU context. */
} ClockImageCtxtType;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : Clock_InitImage
** =========================================================================*/
/**
  Initialize the image specific portion of the clock driver.

  This function initializes the portion of the clock driver that is specific
  to the image we are running on and common between chipsets.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitImage
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitCPU
** =========================================================================*/
/**
  Initialize the CPU clocks.

  This function initializes the CPU clocks.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitCPU
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitQDSS
** =========================================================================*/
/**
  Initialize the remote QDSS NPA resources.

  This function initializes the remote QDSS NPA resources.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitQDSS
(
  ClockDrvCtxt *pDrvCtxt
);


#endif /* __CLOCK_IMAGE_H__ */
