/*==============================================================================
  FILE:         island_trans.c

  OVERVIEW:     This file provides island (hardware based) transition hardware routines

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/transition/ddr/island_trans.c#3 $
$DateTime: 2017/10/12 22:35:09 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "island_trans.h"
#include "uSleep_hwio.h"
#include "rsc.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIClock.h"
#include "ClockDefs.h"

/*==============================================================================
                          INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/* 
 * islandTrans_setupSequenceTransition 
 */
static void islandTrans_setupSequenceTransition(void)
{
  /* Setup configuration register for bus isolation sequence */
  USLEEP_HWIO_OUT(RSCCTL_EN_ISOINTF_CLK,
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLK, AXIM2,  0)   |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLK, AXIM,   1)   |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLK, CORE,   1));

  USLEEP_HWIO_OUT(RSCCTL_EN_ISOINTF_CTL,
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CTL, AXIM2_FIFO_ARESET,  0) |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CTL, AXIM2_HALTREQ,      0) |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CTL, AXIM_FIFO_ARESET,   1) |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CTL, AXIM_HALTREQ,       1));

  USLEEP_HWIO_OUT(RSCCTL_EN_ISOINTF_CLAMP,
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLAMP, AXIM2_CLAMP_E_CLEAR,  0)   |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLAMP, AXIM2_CLAMP_E_SET,    0)   |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLAMP, AXIM2_CLAMP_L_CLEAR,  0)   |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLAMP, AXIM2_CLAMP_L_SET,    0)   |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLAMP, AXIM_CLAMP_E_CLEAR,   1)   |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLAMP, AXIM_CLAMP_E_SET,     1)   |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLAMP, AXIM_CLAMP_L_CLEAR,   1)   |
                  USLEEP_HWIO_FVAL(RSCCTL_EN_ISOINTF_CLAMP, AXIM_CLAMP_L_SET,     1));

  USLEEP_HWIO_OUT(RSC_EVENT_ISOINTF_OVR,
                  USLEEP_HWIO_FVAL(RSC_EVENT_ISOINTF_OVR, AXIM2_HALTACK_OVRRD,  1)   |
                  USLEEP_HWIO_FVAL(RSC_EVENT_ISOINTF_OVR, AXIM_HALTACK_OVRRD,   0)   |
                  USLEEP_HWIO_FVAL(RSC_EVENT_ISOINTF_OVR, ALL_HALTACK_OVRRD,    1));

  return;
}

/*==============================================================================
                          EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * islandTrans_initialize
 */
void islandTrans_initialize(void)
{
  islandTrans_setupSequenceTransition();
  return;
}

/* 
 * islandTrans_enableDebug 
 */ 
void islandTrans_enableDebug(boolean enable)
{
  const char  *pClkName1 = "scc_at_clk";
  const char  *pClkName2 = "scc_pclkdbg_clk";
  const char  *pClkName3 = "scc_sdc_dap_clk";

  static ClockIdType      nClkId1 = 0;
  static ClockIdType      nClkId2 = 0;
  static ClockIdType      nClkId3 = 0;
  static DalDeviceHandle  *clockHandle = NULL;  
  
  if(NULL == clockHandle)
  {
    CORE_DAL_VERIFY( DAL_DeviceAttach(DALDEVICEID_CLOCK, &clockHandle) );
    CORE_DAL_VERIFY( DalClock_GetClockId(clockHandle, pClkName1, &nClkId1) );
    CORE_DAL_VERIFY( DalClock_GetClockId(clockHandle, pClkName2, &nClkId2) );
    CORE_DAL_VERIFY( DalClock_GetClockId(clockHandle, pClkName3, &nClkId3) );
  }
  
  if(TRUE == enable)
  {
    CORE_DAL_VERIFY( DalClock_EnableClock(clockHandle, nClkId1) );
    CORE_DAL_VERIFY( DalClock_EnableClock(clockHandle, nClkId1) );
    CORE_DAL_VERIFY( DalClock_EnableClock(clockHandle, nClkId3) );
  }
  else
  {
    CORE_DAL_VERIFY( DalClock_DisableClock(clockHandle, nClkId1) );
    CORE_DAL_VERIFY( DalClock_DisableClock(clockHandle, nClkId1) );
    CORE_DAL_VERIFY( DalClock_DisableClock(clockHandle, nClkId3) );
  }

  return;
}

