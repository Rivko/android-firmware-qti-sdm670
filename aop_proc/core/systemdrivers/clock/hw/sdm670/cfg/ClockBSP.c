/** @file ClockBSP.c

  Definitions of the support clock perf level

  Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---     ---------------------------------------------------------
 01/20/17    vph     Support cap trace resources at fix voltage corner
 12/07/16    vph     Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/
#include "HALhwio.h"
#include "ClockBSP.h"
#include "ClockHWIO.h"
#include "ClockDriver.h"

/*=========================================================================
      Prototypes
==========================================================================*/

/*=========================================================================
      Data
==========================================================================*/

static Clock_ConfigType Clock_ConfigData =
{
  /* V1: Max QUPV3 clocks run at 100MHz */
  .QUPV3Cfg = 
  {
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), SRC_CXO,         1 },  /*  19.20 MHz */
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), SRC_CXO,         1 },  /*  19.20 MHz */
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), SRC_GPLL0_EVEN, 12 },  /*  50.00 MHz */
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), SRC_GPLL0,      12 },  /* 100.00 MHz */
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), SRC_GPLL0,      12 },  /* 100.00 MHz */
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), SRC_GPLL0,      12 },  /* 100.00 MHz */
  },
  .QDSS_Mapping =
  {
    { CLOCK_QDSS_LEVEL_OFF,     RAILWAY_NO_REQUEST  },
    { CLOCK_QDSS_LEVEL_MIN_SVS, RAILWAY_SVS_MIN     },
    { CLOCK_QDSS_LEVEL_LOW_SVS, RAILWAY_SVS_LOW     },
    { CLOCK_QDSS_LEVEL_SVS,     RAILWAY_SVS         },
    { CLOCK_QDSS_LEVEL_SVS_L1,  RAILWAY_SVS_HIGH    },
    { CLOCK_QDSS_LEVEL_NOMINAL, RAILWAY_NOMINAL     },
    { CLOCK_QDSS_LEVEL_TURBO,   RAILWAY_SUPER_TURBO },

  },
  .QDSS_ATBADfs =  {DFS_SHUB_DFS, 6 },
  .QDSS_ATBACfg =
  {
    {HWIO_ADDR(GCC_QDSS_ATB_A_CMD_RCGR), SRC_CXO,         1 },  /*  19.2MHz - OFF     */
    {HWIO_ADDR(GCC_QDSS_ATB_A_CMD_RCGR), SRC_GPLL0_EVEN, 20 },  /*  30MHz   - MinSVS  */
    {HWIO_ADDR(GCC_QDSS_ATB_A_CMD_RCGR), SRC_GPLL0_EVEN, 10 },  /*  60MHz   - LowSVS  */
    {HWIO_ADDR(GCC_QDSS_ATB_A_CMD_RCGR), SRC_GPLL0,      10 },  /* 120MHz   - SVS     */
    {HWIO_ADDR(GCC_QDSS_ATB_A_CMD_RCGR), SRC_GPLL0,      10 },  /* 120MHz   - SVS_L1  */
    {HWIO_ADDR(GCC_QDSS_ATB_A_CMD_RCGR), SRC_GPLL0,       5 },  /* 240MHz   - NOMINAL */
    {HWIO_ADDR(GCC_QDSS_ATB_A_CMD_RCGR), SRC_GPLL0,       4 },  /* 300MHz   - TURBO   */
  },
  .QDSS_ATBBDfs =  {DFS_SHUB_DFS, 7 },
  .QDSS_ATBBCfg =
  {
    {HWIO_ADDR(GCC_QDSS_ATB_B_CMD_RCGR), SRC_CXO,         1 },  /*  19.2MHz - OFF     */
    {HWIO_ADDR(GCC_QDSS_ATB_B_CMD_RCGR), SRC_GPLL0_EVEN, 20 },  /*  30MHz   - MinSVS  */
    {HWIO_ADDR(GCC_QDSS_ATB_B_CMD_RCGR), SRC_GPLL0_EVEN, 10 },  /*  60MHz   - LowSVS  */
    {HWIO_ADDR(GCC_QDSS_ATB_B_CMD_RCGR), SRC_GPLL0,      10 },  /* 120MHz   - SVS     */
    {HWIO_ADDR(GCC_QDSS_ATB_B_CMD_RCGR), SRC_GPLL0,      10 },  /* 120MHz   - SVS_L1  */
    {HWIO_ADDR(GCC_QDSS_ATB_B_CMD_RCGR), SRC_GPLL0,       5 },  /* 240MHz   - NOMINAL */
    {HWIO_ADDR(GCC_QDSS_ATB_B_CMD_RCGR), SRC_GPLL0,       4 },  /* 300MHz   - TURBO   */
  },
  .QDSS_ATBCDfs =  {DFS_SHUB_DFS, 8 },
  .QDSS_ATBCCfg =
  {
    {HWIO_ADDR(GCC_QDSS_ATB_C_CMD_RCGR), SRC_CXO,         1 },  /*  19.2MHz - OFF     */
    {HWIO_ADDR(GCC_QDSS_ATB_C_CMD_RCGR), SRC_CXO,         1 },  /*  19.2MHz - MinSVS  */
    {HWIO_ADDR(GCC_QDSS_ATB_C_CMD_RCGR), SRC_GPLL0_EVEN, 20 },  /*  30MHz   - LowSVS  */
    {HWIO_ADDR(GCC_QDSS_ATB_C_CMD_RCGR), SRC_GPLL0,      20 },  /*  60MHz   - SVS     */
    {HWIO_ADDR(GCC_QDSS_ATB_C_CMD_RCGR), SRC_GPLL0,      20 },  /*  60MHz   - SVS_L1  */
    {HWIO_ADDR(GCC_QDSS_ATB_C_CMD_RCGR), SRC_GPLL0,      10 },  /* 120MHz   - NOMINAL */
    {HWIO_ADDR(GCC_QDSS_ATB_C_CMD_RCGR), SRC_GPLL0,      10 },  /* 120MHz   - TURBO   */
  },
  .QDSS_STMDfs =  { DFS_SYS_NOC, 5 },
  .QDSS_STMCfg =
  {
    {HWIO_ADDR(GCC_QDSS_STM_CMD_RCGR), SRC_CXO,         1 },  /*  19.2MHz - OFF     */
    {HWIO_ADDR(GCC_QDSS_STM_CMD_RCGR), SRC_GPLL0_EVEN, 24 },  /*  25MHz - MinSVS    */
    {HWIO_ADDR(GCC_QDSS_STM_CMD_RCGR), SRC_GPLL0_EVEN, 12 },  /*  50MHz   - LowSVS  */
    {HWIO_ADDR(GCC_QDSS_STM_CMD_RCGR), SRC_GPLL0,      12 },  /* 100MHz   - SVS     */
    {HWIO_ADDR(GCC_QDSS_STM_CMD_RCGR), SRC_GPLL0,      12 },  /* 100MHz   - SVS_L1  */
    {HWIO_ADDR(GCC_QDSS_STM_CMD_RCGR), SRC_GPLL0,       6 },  /* 200MHz   - NOMINAL */
    {HWIO_ADDR(GCC_QDSS_STM_CMD_RCGR), SRC_GPLL0,       6 },  /* 200MHz   - TURBO   */
  },
  .QDSS_TRACEDfs = { DFS_SHUB_DFS, 9 },
  .QDSS_TRACECfg =
  {
    {HWIO_ADDR(GCC_QDSS_TRACECLKIN_CMD_RCGR), SRC_CXO,         1 },  /*  19.2MHz - OFF     */
    {HWIO_ADDR(GCC_QDSS_TRACECLKIN_CMD_RCGR), SRC_GPLL0_EVEN, 16 },  /*  37.5MHz - MinSVS  */
    {HWIO_ADDR(GCC_QDSS_TRACECLKIN_CMD_RCGR), SRC_GPLL0_EVEN,  8 },  /*  75MHz   - LowSVS  */
    {HWIO_ADDR(GCC_QDSS_TRACECLKIN_CMD_RCGR), SRC_GPLL0,       8 },   /* 150MHz   - SVS     */
    {HWIO_ADDR(GCC_QDSS_TRACECLKIN_CMD_RCGR), SRC_GPLL0,       8 },  /* 150MHz   - SVS_L1  */
    {HWIO_ADDR(GCC_QDSS_TRACECLKIN_CMD_RCGR), SRC_GPLL0,       4 },  /* 300MHz   - NOMINAL */
    {HWIO_ADDR(GCC_QDSS_TRACECLKIN_CMD_RCGR), SRC_GPLL0,       4 },  /* 300MHz   - TURBO   */
  },
  .QDSS_TRIGDfs =  { DFS_SYS_NOC, 6 },
  .QDSS_TRIGCfg =
  {
    {HWIO_ADDR(GCC_QDSS_TRIG_CMD_RCGR), SRC_CXO,         1 },  /*  19.2MHz - OFF     */
    {HWIO_ADDR(GCC_QDSS_TRIG_CMD_RCGR), SRC_CXO,         1 },  /*  19.2MHz - MinSVS  */
    {HWIO_ADDR(GCC_QDSS_TRIG_CMD_RCGR), SRC_GPLL0_EVEN, 16 },  /*  37.5MHz - LowSVS  */
    {HWIO_ADDR(GCC_QDSS_TRIG_CMD_RCGR), SRC_GPLL0,      16 },  /*  75MHz   - SVS     */
    {HWIO_ADDR(GCC_QDSS_TRIG_CMD_RCGR), SRC_GPLL0,      16 },  /*  75MHz   - SVS_L1  */
    {HWIO_ADDR(GCC_QDSS_TRIG_CMD_RCGR), SRC_GPLL0,       8 },  /* 150MHz   - NOMINAL */
    {HWIO_ADDR(GCC_QDSS_TRIG_CMD_RCGR), SRC_GPLL0,       8 },  /* 150MHz   - TURBO   */
  },
};


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
Clock_ConfigType* Clock_GetConfigData( void )
{
  return &Clock_ConfigData;
}

/* ============================================================================
**  Function : Clock_SourceMapToMux
** ============================================================================
*/
/*!
   Map the source enumeration to a physical mux setting for GCC.

   @param  eSource : The source enumeration to map.
   @param  nMuxValue : output parameter.

   @retval a pointer to the configuration data

*/
boolean Clock_SourceMapToMux
(
  ClockConfigMuxType *pConfig,
  uint32 *nMuxValue
)
{
  switch( pConfig->eSource )
  {
    case SRC_CXO:
      *nMuxValue = 0;
      break;
    case SRC_GPLL0:
      *nMuxValue = 1;
      break;          
    case SRC_GPLL0_EVEN:
      *nMuxValue = 6;
      break;
    case SRC_GPLL1:
      *nMuxValue = 4;
      break;
    case SRC_GPLL4:
      *nMuxValue = 5;
      break;
    case SRC_GPLL5:
      *nMuxValue = 3;
      break;
    case SRC_AOPLL0_EVEN:
      *nMuxValue = 1;
      break;
    case SRC_AOPLL1:
      *nMuxValue = 3;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}


/* ============================================================================
**  Function : Clock_SourceMapToMux
** ============================================================================
*/
/*!
   Map the source enumeration to a physical PLL bit index.

   @param  eSource : The source enumeration to map.
   @param  nBitIndex : output parameter.

   @retval a pointer to the configuration data

*/
boolean Clock_SourceMapToBitIndex
(
  uint32 eSource,
  uint32 *nBitIndex
)
{
  switch( eSource )
  {
    case SRC_GPLL0:
      *nBitIndex = 0;
      break;          
    case SRC_GPLL0_EVEN:
      *nBitIndex = 0;
      break;
    case SRC_GPLL1:
      *nBitIndex = 1;
      break;
    case SRC_GPLL4:
      *nBitIndex = 4;
      break;
    case SRC_GPLL5:
      *nBitIndex = 5;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

