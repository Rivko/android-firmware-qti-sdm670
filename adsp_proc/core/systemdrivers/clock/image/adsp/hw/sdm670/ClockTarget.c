/*
===========================================================================
*/
/**
  @file ClockTarget.c

*/
/*
  ====================================================================

  Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/adsp/hw/sdm670/ClockTarget.c#7 $
  $DateTime: 2018/04/06 00:39:57 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"
#include "ClockImage.h"
#include "HALclkHWIO.h"
#include "HALhwio.h"
#include "uClock.h"
#include "busywait.h"
#include "HALclkGeneric.h"
#include "ABTimeout.h"

#define LPASS_LPAAUDIO_PLL_CONFIG_CTL_VAL       0x20000954
#define LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_VAL     0x000003D6
#define LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_VAL   0x20485699
#define LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_VAL 0x00002067
#define LPASS_LPAAUDIO_PLL_USER_CTL_VAL         0x0300000F
#define LPASS_LPAAUDIO_DIG_PLL_USER_CTL_VAL     0x00000007
#define LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_VAL   0x00004804
#define LPASS_LPAAUDIO_PLL_TEST_CTL_VAL         0x04000400
#define LPASS_LPAAUDIO_PLL_TEST_CTL_U_VAL       0x00004000
#define LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_VAL     0x40000000
#define LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_VAL   0x00000000

/*=========================================================================
      Functions
==========================================================================*/
ClockPowerDomainIdType AUDIOPwrId = 0;

extern boolean uClock_Init(void);

void HAL_clk_AudioCorePowerDomainEnable  (HAL_clk_PowerDomainDescType *pmPowerDomainDesc);
void HAL_clk_AudioCorePowerDomainDisable  (HAL_clk_PowerDomainDescType *pmPowerDomainDesc);

/*
 * HAL_clk_AudioCorePowerDomainControl
 *
 * Common functions for controlling clock power domains.
 */
HAL_clk_PowerDomainControlType HAL_clk_AudioCorePowerDomainControl =
{
   /* .Enable     = */ HAL_clk_AudioCorePowerDomainEnable,
   /* .Disable    = */ HAL_clk_AudioCorePowerDomainDisable,
   /* .IsEnabled  = */ HAL_clk_GenericPowerDomainIsEnabled,
   /* .IsOn       = */ HAL_clk_GenericPowerDomainIsOn
};

/* ===========================================================================
**  HAL_clk_AudioCorePowerDomainEnable
**
** ======================================================================== */

void HAL_clk_AudioCorePowerDomainEnable
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  uintnt nAddr;
  uint32 nVal;

  nAddr = pmPowerDomainDesc->nGDSCRAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

  nVal  = inpdw(nAddr);

  HWIO_OUTF(SSC_SCC_RO_CBCR,CLK_ENABLE,0x1);
  while(HWIO_IN(SSC_SCC_RO_CBCR) & 0x80000000) busywait(1);
  
  HWIO_OUTF(SSC_SCC_LPASS_AON_GDS_HW_CTRL,SW_OVERRIDE,0x0);
  HWIO_OUTF(SSC_SCC_LPASS_CORE_QHM_QCHANNEL_CTL,QHM_QREQN,0x1);
  HWIO_OUTF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_QREQN,0x1);
  HWIO_OUTF(SSC_SCC_LPASS_CORE_QHM_QCHANNEL_CTL,QHM_OVERRIDE_EN,0x0);
  
  /*
   * Clear the SW PD collapse bit
   */
  nVal &= ~(0x1);
  outpdw(nAddr, nVal);
  
  /*
   * Wait for PD ON
   */
  while( !(inpdw(nAddr) & 0xF0000000) );
  
  //HWIO_OUTF(LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR,CLK_ENABLE,0x1);
  //while(HWIO_IN(LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR) & 0x80000000) busywait(1);

  /*re initialising ABT */
  ABT_InitDeferred(AUDIOPwrId);
  outpdw(HWIO_LPASS_SLEEP_CMD_RCGR_ADDR,0x2);
} /* END HAL_clk_AudioCorePowerDomainEnable */


/* ===========================================================================
**  HAL_clk_AudioCorePowerDomainDisable
**
** ======================================================================== */

void HAL_clk_AudioCorePowerDomainDisable
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  uintnt nAddr,nCounter=0;
  
  nAddr = pmPowerDomainDesc->nGDSCRAddr;

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

   //Begin LPASS Audio Core Power Collapse sequence
  /*
   * (2). Enable the RO clock branch 
   */
  HWIO_OUTF(SSC_SCC_RO_CBCR,CLK_ENABLE,0x1);
  while(HWIO_IN(SSC_SCC_RO_CBCR) & 0x80000000) busywait(1);

  /*
   * (3). Enable retention
   */
  HWIO_OUTF(SSC_SCC_AUDIO_CORE_GDSCR, RETAIN_FF_ENABLE, 0x1);
  /*
   * (4) & (5)	Ensure the MPORT clock is running 
   */
  HWIO_OUTF(LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR,CLK_ENABLE,0x1);
  while(HWIO_IN(LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR) & 0x80000000) busywait(1);

  /*
   * (6) SW should poll on the QACTIVE register until it reads back 0
   */
  while(HWIO_INF(SSC_SCC_LPASS_CORE_QHM_QCHANNEL_CTL,QHM_ACTIVE) != 0x0);
  /*
   * (7) Halt the lpass_noc’s QHM interface by performing a handshake with its Qchannel interface.  
   */
  HWIO_OUTF(SSC_SCC_LPASS_CORE_QHM_QCHANNEL_CTL,QHM_QREQN, 0x0);
  
  busywait(1);
  
  /*
   * (8) & (9) - Handle abnormal case when QHM_ACCEPT is not asserted.
   */
  while(HWIO_INF(SSC_SCC_LPASS_CORE_QHM_QCHANNEL_CTL,QHM_ACCEPTN) != 0x0)
  {
    if(HWIO_INF(SSC_SCC_LPASS_CORE_QHM_QCHANNEL_CTL,QHM_DENY) == 0x1)
    {
      HWIO_OUTF(SSC_SCC_LPASS_CORE_QHM_QCHANNEL_CTL,QHM_QREQN, 0x1); 
    }

    HWIO_OUTF(SSC_SCC_LPASS_CORE_QHM_QCHANNEL_CTL, QHM_QREQN, 0x0);

    busywait(1);
  }
  
  /*
   * (10) SW should poll on the QHS QACTIVE register at least 20 times, and make sure all 20 times read back 0
   */
   
  while(1)
  {
    if(nCounter == 20)
	{
	  break;
	}
	if(HWIO_INF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL, QHS_ACTIVE) == 0x0)
    {  
      nCounter++;
	}
	else
	{
	  nCounter=0;
	}
  }
  
  /*
   * (11) Take the LPASS Core into power collapse
   */
  HWIO_OUTF(SSC_SCC_AUDIO_CORE_GDSCR, SW_COLLAPSE, 0x1);
  
  busywait(1);
  
  /*
   * (12) & (13) In case if PWR_ON is not de-asserted 
   */
  while(HWIO_INF(SSC_SCC_AUDIO_CORE_GDSCR, PWR_ON) != 0x0)
  {
    /*
     * i) Check if the GDS HW CTRL has aborted due to timeout
     */
    if(HWIO_INF(SSC_SCC_LPASS_AON_GDS_HW_CTRL_IRQ_STATUS, STATUS) == 0x1)
    {
      /*
       * ii) See if the lpass_noc QHS interface has returned a deny
       */
      if(HWIO_INF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_DENY) == 0x1)
      {
        /*
         * iii) Set LPASS_CORE_QHS_QCONNECT_CTL.QREQn to 0
         */
        HWIO_OUTF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_QREQN, 0x0); 
        /*
         * iv)Set LPASS_CORE_QHS_QCONNECT_CTL.OVERRIDE_EN to 1
         */
        HWIO_OUTF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_OVERRIDE_EN, 0x1); 
        /*
         *  v)Set LPASS_CORE_QHS_QCONNECT_CTL.QREQn to 1
         */
        HWIO_OUTF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_QREQN, 0x1);
        /*
         *  vi)Wait 10us
         */
        busywait(10);
        /*
         *  vii)Set LPASS_CORE_QHS_QCONNECT_CTL.QREQn to 0
         */
        HWIO_OUTF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_QREQN, 0x0);
        busywait(1);
        /*
         *  viii) and ix)
         */
        while(HWIO_INF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_ACCEPTN) != 0x0)
        {
          HWIO_OUTF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_QREQN, 0x1);
          busywait(10);
          HWIO_OUTF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_QREQN, 0x0);
          busywait(1);
        }
        /*
         *  x)Deassert the software collapse
         */
        HWIO_OUTF(SSC_SCC_AUDIO_CORE_GDSCR, SW_COLLAPSE, 0x0);
        /*
         *  xi)Clear the GDS HW CTRL irq
         */
        HWIO_OUTF(SSC_SCC_LPASS_AON_GDS_HW_CTRL_IRQ_CLEAR,CLEAR,0x1);
        /*
         *  xii)Read back if irq is cleared 
         */
        while(HWIO_INF(SSC_SCC_LPASS_AON_GDS_HW_CTRL_IRQ_STATUS,STATUS) != 0x0);
        /*
         *  xiii)Relaunch the software collapse
         */
        HWIO_OUTF(SSC_SCC_AUDIO_CORE_GDSCR, SW_COLLAPSE, 0x1);
        /*
         *  xiv)Poll on LPASS_AUDIO_CORE_GDSCR.PWR_ON until it reads back 0
         */
        while(HWIO_INF(SSC_SCC_AUDIO_CORE_GDSCR, PWR_ON) != 0x0);
        /*
         *  xv)Set LPASS_CORE_QHS_QCONNECT_CTL.OVERRIDE_EN to 0
         */
        HWIO_OUTF(SSC_SCC_LPASS_CORE_QHS_QCHANNEL_CTL,QHS_OVERRIDE_EN, 0x0); 

      }
    }
  }
  //End of LPASS Audio Core Power Collapse sequence

} /* END HAL_clk_GenericPowerDomainDisable */


/* =========================================================================
**  Function : Clock_ProcessorSleep
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ProcessorSleep
(
  ClockDrvCtxt *pDrvCtxt,
  ClockSleepModeType eMode,
  uint32 nFlags
)
{
  ClockImageCtxtType  *pImageCtxt = pDrvCtxt->pImageCtxt;
  if(strcmp(pImageCtxt->CPUCtxt.pClock->pDomain->pSource->szName,"scc_pll")==0)
  {
	HWIO_OUTF(SSC_QDSP6SS_RSC_EVENT_PLL_OVR, PLL_OVRRD, 1);
  }
  else
  {
	HWIO_OUTF(SSC_QDSP6SS_RSC_EVENT_PLL_OVR, PLL_OVRRD, 0);
  }
  return DAL_SUCCESS;
} /* END Clock_ProcessorSleep */


/* =========================================================================
**  Function : Clock_ProcessorRestore
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ProcessorRestore
(
  ClockDrvCtxt *pDrvCtxt,
  ClockSleepModeType eMode,
  uint32 nFlags
)
{
  return DAL_SUCCESS;

} /* END Clock_ProcessorRestore */



/* =========================================================================
**  Function : Clock_InitTarget
** =========================================================================*/
/*
  See ClockDriver.h.
*/

DALResult Clock_InitTarget (ClockDrvCtxt *pDrvCtxt)
{
  DALResult eResult;
  ClockPowerDomainIdType nPowerDomainId;
  ClockPowerDomainNodeType *pPowerDomain;
  
  eResult = Clock_InitImage(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
  
    /*
   * Get the power domain ID for the audio_core_gdsc for a v1 workaround.
   */
  eResult = Clock_GetPowerDomainId(pDrvCtxt, "audio_core_gdsc", &AUDIOPwrId);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  if (FALSE == uClock_Init())
  {
    return DAL_ERROR;
  }
  
  Clock_GetPowerDomainId(pDrvCtxt, "audio_core_gdsc", &nPowerDomainId);
  pPowerDomain = Clock_GetPowerDomainNode(pDrvCtxt, nPowerDomainId);
  if (pPowerDomain)
  {
    Clock_EnablePowerDomain(Clock_GetInitialClientCtxt(), pPowerDomain);
  }

  //HSR settings for Audio PLLs
  HWIO_OUT(LPASS_LPAAUDIO_PLL_CONFIG_CTL,LPASS_LPAAUDIO_PLL_CONFIG_CTL_VAL);
  HWIO_OUT(LPASS_LPAAUDIO_PLL_CONFIG_CTL_U,LPASS_LPAAUDIO_PLL_CONFIG_CTL_U_VAL);
  HWIO_OUT(LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL,LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_VAL);
  HWIO_OUT(LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U,LPASS_LPAAUDIO_DIG_PLL_CONFIG_CTL_U_VAL);
  
  HWIO_OUT(LPASS_LPAAUDIO_PLL_USER_CTL,LPASS_LPAAUDIO_PLL_USER_CTL_VAL);
  HWIO_OUT(LPASS_LPAAUDIO_DIG_PLL_USER_CTL,LPASS_LPAAUDIO_DIG_PLL_USER_CTL_VAL);
  HWIO_OUT(LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U,LPASS_LPAAUDIO_DIG_PLL_USER_CTL_U_VAL);
  
  HWIO_OUT(LPASS_LPAAUDIO_PLL_TEST_CTL,LPASS_LPAAUDIO_PLL_TEST_CTL_VAL);
  HWIO_OUT(LPASS_LPAAUDIO_PLL_TEST_CTL_U,LPASS_LPAAUDIO_PLL_TEST_CTL_U_VAL);
  HWIO_OUT(LPASS_LPAAUDIO_DIG_PLL_TEST_CTL,LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_VAL);
  HWIO_OUT(LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U,LPASS_LPAAUDIO_DIG_PLL_TEST_CTL_U_VAL);
  
  return DAL_SUCCESS;

} /* END Clock_InitTarget */

/* =========================================================================
**  Function : ClockStub_InitTarget
** =========================================================================*/
/*
  See ClockDriver.h.
*/

DALResult ClockStub_InitTarget (ClockDrvCtxt *pDrvCtxt)
{
  return DAL_SUCCESS;

} /* END ClockStub_InitTarget */

