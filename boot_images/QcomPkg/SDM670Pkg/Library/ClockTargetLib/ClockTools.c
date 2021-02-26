/**
==============================================================================

  @file ClockTools.c

  This file provides clock initialization for starting SDCC clocks at boot.

  Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential.

==============================================================================


==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockTools.c#4 $

  when      who     what, where, why
  --------  ------  ------------------------------------------------------
  02/08/17  vphan   Support QUP V3 clock set frequency
  11/11/16  vphan   Initial SDM845 revision, branched from MSM8998.

==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockHWIO.h"
#include "ClockBSP.h"

/*=========================================================================
      Data
==========================================================================*/
struct Clock_RegType 
{
  uint32 nCMDReg;
  uint32 nCBCRReg;
};

const struct Clock_RegType Clock_QUPV3Regs[CLK_QUPV3_NUM_CLKS] = 
{
  { 0,0 },
  { HWIO_ADDR(GCC_QUPV3_WRAP0_S0_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_S0_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP0_S1_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_S1_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP0_S2_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_S2_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP0_S3_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_S3_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP0_S4_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_S4_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP0_S5_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_S5_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP0_S6_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_S6_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP0_S7_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_S7_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP1_S0_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_S0_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP1_S1_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_S1_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP1_S2_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_S2_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP1_S3_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_S3_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP1_S4_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_S4_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP1_S5_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_S5_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP1_S6_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_S6_CLK_ENA) },
  { HWIO_ADDR(GCC_QUPV3_WRAP1_S7_CMD_RCGR),  HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_S7_CLK_ENA) },
};


struct Clock_SDCCRegAddrType
{
  uint32 nCmd;
  uint32 nApps;
  uint32 nAHB;
};

const struct Clock_SDCCRegAddrType Clock_SDCCRegs[CLK_SDC_NUM_CLKS] = 
{ 
  { 0,0,0 },
  { HWIO_ADDR(GCC_SDCC1_APPS_CMD_RCGR), HWIO_ADDR(GCC_SDCC1_APPS_CBCR), HWIO_ADDR(GCC_SDCC1_AHB_CBCR) },
  { HWIO_ADDR(GCC_SDCC2_APPS_CMD_RCGR), HWIO_ADDR(GCC_SDCC2_APPS_CBCR), HWIO_ADDR(GCC_SDCC2_AHB_CBCR) },
  { 0,0,0 },
  { HWIO_ADDR(GCC_SDCC4_APPS_CMD_RCGR), HWIO_ADDR(GCC_SDCC4_APPS_CBCR), HWIO_ADDR(GCC_SDCC4_AHB_CBCR) },
  { 0,0,0 }
};


void Clock_SetVotableClock( uintnt nVoteRegs, uintnt nField, boolean bEnable )
{
  uintnt nRegVal = inp32( nVoteRegs );

  if ( bEnable )
  {
    nRegVal |= nField;
  }
  else
  {
    nRegVal &= ~(nField);
  }
  outp32 ( nVoteRegs, nRegVal );
}

/* ============================================================================
**  Function : Clock_SetQUPV3Frequency
** ============================================================================
*/
/*!
    Set QUP V3 clock frequency.

    @param 
      eClock   - QUPV3 index 
      nFreqKHz   Clock frequency in KHz

    @return
    TRUE -- Set frequency success
    FALSE -- Set frequency failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetQUPV3Frequency(ClockQUPV3Type eClock, uint32 nFreqKHz)
{
  const Clock_ConfigType *cfg = Clock_Config();
  const ClockFreqMuxCfgType *pCfg = NULL;
  ClockConfigMuxType clkCfg = {0};
  uint32 nIndex;

  /* Check for valid support QUPV3 clocks */
  if ( (eClock >= CLK_QUPV3_NUM_CLKS) || (Clock_QUPV3Regs[eClock].nCMDReg == 0) )
  {
    return FALSE;
  }

  pCfg = cfg->QUPV3_Cfg;

  /* Search for match support frequency */
  for ( nIndex = 0; nIndex < QUPV3_PERF_NUM; nIndex++ )
  {
    /* Match support frequency */
    if ( pCfg[nIndex].nFreqKHz == nFreqKHz )
    {
      break;
    }
  }

  /* Not find matching support frequency */
  if ( nIndex == QUPV3_PERF_NUM )
  {
    return FALSE;
  }

  clkCfg = pCfg[nIndex].Cfg;
  //memcpy( &clkCfg, &(pCfg[nIndex].Cfg), sizeof(ClockConfigMuxType) );
  clkCfg.nCMDRCGRAddr = Clock_QUPV3Regs[eClock].nCMDReg;

  if ( !Clock_EnableSource( clkCfg.eSource )) return FALSE;
  if ( !Clock_ConfigMux(&clkCfg)) return FALSE;

  Clock_SetVotableClock ( HWIO_ADDR(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1), Clock_QUPV3Regs[eClock].nCBCRReg, TRUE );
  return TRUE;
}

/* ============================================================================
**  Function : Clock_DisableQUPV3Clock
** ============================================================================
*/
/*!
    Disable QUP V3 clock.

    @param 
      eClock   - QUPV3 index 

    @return
      TRUE -- Disable success
      FALSE -- Disable failed.

    @dependencies
      None.

    @sa None
*/
boolean Clock_DisableQUPV3Clock(ClockQUPV3Type eClock)
{

  /* Check for valid support QUPV3 clocks */
  if ( (eClock >= CLK_QUPV3_NUM_CLKS) || (Clock_QUPV3Regs[eClock].nCBCRReg == 0) )
  {
    return FALSE;
  }

  Clock_SetVotableClock ( HWIO_ADDR(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1), Clock_QUPV3Regs[eClock].nCBCRReg, FALSE );
  return TRUE;
}

/* ============================================================================
**  Function : Clock_AGGRENOCInit
** ============================================================================
*/
/*!
    Configure AGGRE NOC clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_AGGRENOCInit( void )
{
  static boolean bInit = FALSE;

  /* Init Aggre NOC only one time */
  if ( !bInit )
  {
    Clock_PowerDomainEnable( HWIO_GCC_ANOC_TBU_GDSCR_ADDR );
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_QOSGEN_EXTREF_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_AUDIO_TBU_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_TBU1_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_TBU2_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    
    bInit = TRUE;
  }

  return TRUE;
}


/* ============================================================================
**  Function : Clock_SetSDCClockFrequencyExt
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] Clock frequency level
           eClock            -  [IN] SDC clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_SetSDCClockFrequencyExt
(
  uint32 nFreqKHz,
  ClockSDCType eClock
)
{
  ClockConfigMuxType SDCCCfg = {0};
  const ClockFreqMuxCfgType *pCfg = NULL;
  uint32 n;
  Clock_ConfigType *cfg = Clock_Config();

  /* Check for a valid SDC */
  if ( (eClock >= CLK_SDC_NUM_CLKS) || (Clock_SDCCRegs[eClock].nCmd == 0) )
  {
    return FALSE;
  }

  /* Copy the config to the local so the nCMDRCGRAddr can be modified. */

  switch(eClock)
  {
    case CLK_SDC1:
      pCfg = cfg->SDC1_Ext_Cfg;
    break;
    case CLK_SDC2:
      pCfg = cfg->SDC2_Ext_Cfg;
    break;
    case CLK_SDC4:
      pCfg = cfg->SDC4_Ext_Cfg;
    break;
    default:
    break;
  }

  if(!pCfg)
  {
    return FALSE;
  }
  /* Find the nearest frequency that matches the request */
  for(n = 0; pCfg[n].nFreqKHz != 0; n++)
  {
    if(pCfg[n].nFreqKHz >= nFreqKHz)
    {
      break;
    }
  }
  if ( pCfg[n].nFreqKHz == 0 )
  {
    return FALSE;
  }

  /* A configuration is found.  Set it */
  SDCCCfg = pCfg[n].Cfg; /* copy struct and fix it */
  SDCCCfg.nCMDRCGRAddr = Clock_SDCCRegs[eClock].nCmd;

  /*
   * Enable access to the config NOC.
   */
  Clock_PowerDomainEnable( HWIO_GCC_ANOC_TBU_GDSCR_ADDR );  //SDM670 BU Change
  Clock_ToggleClock(HWIO_GCC_CFG_NOC_SOUTH_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_SOUTH_AHB_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_AGGRE_UFS_PHY_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE); //WA for WL bringup

  Clock_ToggleClock( HWIO_ADDR(GCC_CNOC_PERIPH_EAST_CBCR), CLK_TOGGLE_ENABLE ); //SDM670 BU Change
  Clock_ToggleClock( HWIO_ADDR(GCC_CNOC_PERIPH_SOUTH_CBCR), CLK_TOGGLE_ENABLE ); //SDM670 BU Change
  
  Clock_ToggleClock( HWIO_ADDR(GCC_AGGRE_CNOC_PERIPH_EAST_CBCR), CLK_TOGGLE_ENABLE ); //SDM670 BU Change
  Clock_ToggleClock( HWIO_ADDR(GCC_AGGRE_NOC_TBU1_CBCR), CLK_TOGGLE_ENABLE ); //SDM670 BU Change
  Clock_ToggleClock( HWIO_ADDR(GCC_AGGRE_NOC_TBU2_CBCR), CLK_TOGGLE_ENABLE ); //SDM670 BU Change

  /* For SDCC2 NOM and TURBO Levels GPLL4 is used and the same PLL is used by DFS VCDs,
   * So placing a subsystem specific vote for GPLL4 as its spare vote is removed by AOP once it switches HW control mode 
   */
  if(SDCCCfg.eSource == SRC_GPLL4)
    {
      HWIO_OUTF(GCC_APCS_GPLL_ENA_VOTE,GPLL4,1);
    }
  /*
   * Scaling gcc_aggre_ufs_phy_axi_clk to NOM level, as it is required for SDCC1 in SDM670
   */
  if(eClock == CLK_SDC1)
    {
      Clock_SetClockConfig( &cfg->UFS_Cfg[CLOCK_BOOT_PERF_DEFAULT] );
    }

  if ( (!Clock_EnableSource(SDCCCfg.eSource)) ||
      (!Clock_ConfigMux(&SDCCCfg)) ||
      (!Clock_ToggleClock(Clock_SDCCRegs[eClock].nApps, CLK_TOGGLE_ENABLE)) ||
      (!Clock_ToggleClock(Clock_SDCCRegs[eClock].nAHB, CLK_TOGGLE_ENABLE)) )
  {
    return FALSE;
  }

  return (pCfg[n].nFreqKHz? TRUE : FALSE);
}


/* ============================================================================
**  Function : Clock_SetSDCClockFrequency
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] SDC Clock perf level
           eClock            -  [IN] SDC clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetSDCClockFrequency
(
  ClockBootPerfLevelType ePerfLevel,
  ClockSDCType eClock
)
{
  const Clock_ConfigType *cfg = Clock_Config();

  if(ePerfLevel >= CLOCK_BOOT_PERF_NUM) return FALSE;
 
  return Clock_SetSDCClockFrequencyExt( cfg->SDC_Cfg[ePerfLevel], eClock);

} /* END Clock_SetSDCClockFrequency */


/* ========================================================================
**  Function : Clock_ReplaceUSBBootClockEnable
** ======================================================================*/
/*
    Description: The register GCC_USB_BOOT_CLOCK_CTL turns on all the clocks
    necessary to access USB from the APPS.  This function must be ported
    on every new MSM.  It replaces that single register with all the clock
    enables that it replaced.  This allows for fine grained control of these
    clocks on an individual basis.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
void Clock_ReplaceUSBBootClockEnable()
{
  if ( HWIO_INF(GCC_USB_BOOT_CLOCK_CTL, CLK_ENABLE) != 0 )
  { 
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_CENTER_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_NORTH_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_TBU2_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_USB3_PRIM_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);

    Clock_ToggleClock(HWIO_GCC_CFG_NOC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_CFG_NOC_NORTH_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_CFG_NOC_USB3_PRIM_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    
    Clock_ToggleClock(HWIO_GCC_MSS_MFAB_AXIS_CBCR_ADDR, CLK_TOGGLE_ENABLE);

    Clock_ToggleClock(HWIO_GCC_SYS_NOC_AHB_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_SYS_NOC_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_SYS_NOC_SF_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);

    Clock_ToggleClock(HWIO_GCC_USB30_PRIM_MASTER_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_USB30_PRIM_MOCK_UTMI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_USB30_PRIM_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE);

    /* Don't poll this clock for enable since the clock source from PHY which may not be out of reset yet */
    HWIO_OUTF(GCC_USB_PHY_CFG_AHB2PHY_CBCR, CLK_ENABLE, 1);
    HWIO_OUTF(GCC_USB3_PRIM_PHY_AUX_CBCR, CLK_ENABLE, 1);
    HWIO_OUTF(GCC_USB3_PRIM_PHY_COM_AUX_CBCR, CLK_ENABLE, 1);
    HWIO_OUTF(GCC_USB3_PRIM_PHY_PIPE_CBCR, CLK_ENABLE, 1);

    /* Now disable the master boot control, allowing each of those above to replace */
    HWIO_OUTF(GCC_USB_BOOT_CLOCK_CTL, CLK_ENABLE, 0);
  }

  /* Napali PHYs all use SE REF CLK as POR and DIFF REF CLK used for QUSB2 PHY can be OFF by default.
   * HW default is ON.
   */
  HWIO_OUTF(GCC_RX1_USB2_CLKREF_EN, RX1_USB2_ENABLE, 0);
}


/* ========================================================================
**  Function : Clock_ReplaceUFSBootClockEnable
** ======================================================================*/
/*
    Description: The register GCC_UFS_PHY_BOOT_CLOCK_CTL turns on all the clocks
    necessary to access UFS from the APPS.  This function must be ported
    on every new MSM.  It replaces that single register with all the clock
    enables that it replaced.  This allows for fine grained control of these
    clocks on an individual basis.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
void Clock_ReplaceUFSBootClockEnable()
{
  if ( HWIO_INF(GCC_UFS_PHY_BOOT_CLOCK_CTL, CLK_ENABLE) != 0 )
  { 
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_AUDIO_TBU_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_EAST_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_MONAQ_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_SOUTH_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_TBU1_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_TBU2_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_NOC_WEST_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_AGGRE_UFS_PHY_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_DDRSS_SYS_NOC_SLAVE_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP_0_M_AHB_CLK_ENA, 1);
    HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP_1_M_AHB_CLK_ENA, 1);
    Clock_ToggleClock(HWIO_GCC_UFS_PHY_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_UFS_PHY_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_UFS_PHY_ICE_CORE_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_UFS_PHY_PHY_AUX_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_UFS_PHY_RX_SYMBOL_0_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_UFS_PHY_TX_SYMBOL_0_CBCR_ADDR, CLK_TOGGLE_ENABLE);
    Clock_ToggleClock(HWIO_GCC_UFS_PHY_UNIPRO_CORE_CBCR_ADDR, CLK_TOGGLE_ENABLE);

    /* Now disable the master boot control, allowing each of those above to replace */
    HWIO_OUTF(GCC_UFS_PHY_BOOT_CLOCK_CTL, CLK_ENABLE, 0);
  }
}


/* ========================================================================
**  Function : Clock_InitAHB2PHY
** ======================================================================*/
/*!
    Initialize the clocks necessary to access the PHY and REFGEN.

    @param
      None.

    @return
      None.

    @dependencies
      None.

    @sa None
*/

void Clock_InitAHB2PHY( void )
{
  Clock_ToggleClock(HWIO_ADDR(GCC_CNOC_PERIPH_SOUTH_CBCR), 1);
  Clock_ToggleClock(HWIO_ADDR(GCC_CNOC_PERIPH_NORTH_CBCR), 1);
  Clock_ToggleClock(HWIO_ADDR(GCC_CFG_NOC_AH2PHY_XO_CBCR), 1);
  Clock_ToggleClock(HWIO_ADDR(GCC_USB_PHY_CFG_AHB2PHY_CBCR), 1);
  Clock_ToggleClock(HWIO_ADDR(GCC_AHB2PHY_SOUTH_CBCR), 1);
  Clock_ToggleClock(HWIO_ADDR(GCC_CM_PHY_REFGEN1_CBCR), 1);
  Clock_ToggleClock(HWIO_ADDR(GCC_CM_PHY_REFGEN2_CBCR), 1);
}

/* ============================================================================
**  Function : Clock_InitUSB
** ============================================================================
*/
/*!
    Configure USB clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_InitUSB(void)
{
  ClockConfigMuxType USB30Cfg = {0};
  ClockConfigMuxType USB30_MockCfg = {0};
    
  /* This is redundant with BootROM, except on the debugger tools, 
   * because the JTAG tools do not use BootROM */
  Clock_InitAHB2PHY();

  /* Configured gcc_usb30_prim_master_clk to 200 MHz */
  USB30Cfg.nCMDRCGRAddr = HWIO_ADDR(GCC_USB30_PRIM_MASTER_CMD_RCGR);
  USB30Cfg.eSource = SRC_GPLL0; 
  USB30Cfg.nDiv2x = 6;

  /* Configure gcc_usb30_prim_mock_utmi_clk to 60 MHz */
  USB30_MockCfg.nCMDRCGRAddr = HWIO_ADDR(GCC_USB30_PRIM_MOCK_UTMI_CMD_RCGR);
  USB30_MockCfg.eSource = SRC_GPLL0;
  USB30_MockCfg.nDiv2x = 20;

  Clock_AGGRENOCInit();

  if (!Clock_ConfigMux(&USB30Cfg))
  {
    return FALSE;
  }

  HWIO_OUTF(GCC_AGGRE_NOC_CENTER_AXI_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_AGGRE_NOC_NORTH_AXI_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_AGGRE_USB3_PRIM_AXI_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_USB30_PRIM_MASTER_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_USB30_PRIM_SLEEP_CBCR, CLK_ENABLE, 1);

  // Configure the Mock UTMI clock
  if(!Clock_ConfigMux(&USB30_MockCfg))
  {
    return FALSE; 
  }
  HWIO_OUTF(GCC_USB30_PRIM_MOCK_UTMI_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_USB_PHY_CFG_AHB2PHY_CBCR, CLK_ENABLE, 1);
 
  HWIO_OUTF(GCC_USB3_PRIM_PHY_AUX_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_USB3_PRIM_PHY_COM_AUX_CBCR, CLK_ENABLE, 1);
   
  HWIO_OUTF(GCC_CFG_NOC_USB3_PRIM_AXI_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_USB3_PRIM_PHY_PIPE_CBCR, CLK_ENABLE, 1);

  return TRUE;
} /* END Clock_InitUSB */


/* ============================================================================
**  Function : Clock_USB30_PipeClkSrcSel
** ============================================================================
*/
/*!
    Select USB3 PIPE clock source.

    @param eSrc -  [IN] Clock source choice for the USB3.0 PHY
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.
    @dependencies  None.
    @sa None
*/
boolean Clock_USB30_PipeClkSrcSel(ClockUSB30PipeClkSrcType eSrc)
{
  return TRUE;
}  /* END Clock_USB30_PipeClkSrcSel */


/* ============================================================================
**  Function : Clock_Usb30EnableSWCollapse
** ============================================================================
*/
/*!
    Enable SW Collapse for USB30

    @param Enable/Disable.
    @return
    TRUE always

    @dependencies
    None.

    @sa None
   */
boolean Clock_Usb30EnableSWCollapse(boolean enable)
{
  /* Enable/Disable SW PowerCollapse for USB30 */
  HWIO_OUTF(GCC_USB30_PRIM_GDSCR, SW_COLLAPSE, enable ? 1:0 );
  return TRUE;
}


/* ============================================================================
**  Function : Clock_Usb30GetSWCollapse
** ============================================================================
*/
/*!
    Returns the status of SW Collapse for USB30

    @param None
    @return
    TRUE if enabled
    FALSE if disabled

    @dependencies
    None.

    @sa None
*/
boolean Clock_Usb30GetSWCollapse(void)
{
  uint8 sw_collapse = HWIO_INF(GCC_USB30_PRIM_GDSCR, SW_COLLAPSE);
  return (sw_collapse ? TRUE : FALSE);	
}


/* ============================================================================
**  Function : Clock_DisableUSB
** ============================================================================
*/
/*!
    Disable USB clocks.

    @param None.
    @return
    TRUE -- Disable was successful.
    FALSE -- Disable failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_DisableUSB(void)
{
  /* Don't pool this clock since it sources from PHY which may not be out of reset yet */
  HWIO_OUTF(GCC_USB3_PRIM_PHY_PIPE_CBCR, CLK_ENABLE, 0);

  if( !Clock_ToggleClock(HWIO_GCC_AGGRE_USB3_PRIM_AXI_CBCR_ADDR, CLK_TOGGLE_DISABLE)) return FALSE;
  if( !Clock_ToggleClock(HWIO_GCC_USB30_PRIM_MASTER_CBCR_ADDR, CLK_TOGGLE_DISABLE)) return FALSE;
  if( !Clock_ToggleClock(HWIO_GCC_USB30_PRIM_SLEEP_CBCR_ADDR, CLK_TOGGLE_DISABLE)) return FALSE;
  if( !Clock_ToggleClock(HWIO_GCC_USB30_PRIM_MOCK_UTMI_CBCR_ADDR, CLK_TOGGLE_DISABLE)) return FALSE;
  if( !Clock_ToggleClock(HWIO_GCC_USB_PHY_CFG_AHB2PHY_CBCR_ADDR, CLK_TOGGLE_DISABLE)) return FALSE;
  if( !Clock_ToggleClock(HWIO_GCC_USB3_PRIM_PHY_AUX_CBCR_ADDR, CLK_TOGGLE_DISABLE)) return FALSE;
  if( !Clock_ToggleClock(HWIO_GCC_USB3_PRIM_PHY_COM_AUX_CBCR_ADDR, CLK_TOGGLE_DISABLE)) return FALSE;
  if( !Clock_ToggleClock(HWIO_GCC_CFG_NOC_USB3_PRIM_AXI_CBCR_ADDR, CLK_TOGGLE_DISABLE)) return FALSE;

  return TRUE;
} /* END Clock_DisableUSB */


/* ========================================================================
**  Function : Clock_UFSInit
** ======================================================================*/
/*!
    Get the requested clock frequency in hertz.

    @param
      ePerfLevel [in]  - Performance level

    @return
      True - Success init UFS clocks

    @dependencies
    None.

    @sa None
*/

boolean Clock_UFSInit( ClockBootPerfLevelType ePerfLevel )
{
  static ClockBootPerfLevelType eCurrLevel = CLOCK_BOOT_PERF_NONE;
  Clock_CBCRtoggleType enable;
  Clock_ConfigType *cfg = Clock_Config();

  /* Don't switch performance level if it has been set previously */
  if ( eCurrLevel == ePerfLevel )
  {
    return TRUE;
  }
  eCurrLevel = ePerfLevel;

  if(ePerfLevel >= CLOCK_BOOT_PERF_NUM) return FALSE;

  /* This is redundant with BootROM, except on the debugger tools, 
   * because the JTAG tools do not use BootROM */
  Clock_InitAHB2PHY();

  if ( ePerfLevel == CLOCK_BOOT_PERF_NONE )
  {
    /* Disable UFS clocks before disabling the power domain. */
    enable = CLK_TOGGLE_DISABLE;
  }
  else
  {
    /* Enable UFS clocks after enabling the power domain. */
    enable = CLK_TOGGLE_ENABLE;

    /* Enable Aggre NOC since UFS has depedency. */
    if( ! Clock_AGGRENOCInit()) return FALSE;

    Clock_PowerDomainEnable( HWIO_GCC_UFS_PHY_GDSCR_ADDR );
    Clock_PowerDomainEnable( HWIO_GCC_UFS_PHY_CFG_GDSCR_ADDR );
  }

  Clock_ToggleClock(HWIO_GCC_AGGRE_UFS_PHY_AXI_CBCR_ADDR, enable);

  Clock_ToggleClock(HWIO_GCC_UFS_PHY_AHB_CBCR_ADDR, enable);
  Clock_ToggleClock(HWIO_GCC_UFS_PHY_AXI_CBCR_ADDR, enable);
  Clock_ToggleClock(HWIO_GCC_UFS_PHY_ICE_CORE_CBCR_ADDR, enable);
  Clock_ToggleClock(HWIO_GCC_UFS_PHY_PHY_AUX_CBCR_ADDR, enable);
  Clock_ToggleClock(HWIO_GCC_UFS_PHY_UNIPRO_CORE_CBCR_ADDR, enable);

  HWIO_OUTF(GCC_UFS_PHY_TX_SYMBOL_0_CBCR, CLK_ENABLE, enable);
  HWIO_OUTF(GCC_UFS_PHY_RX_SYMBOL_0_CBCR, CLK_ENABLE, enable) ;

  if ( ePerfLevel == CLOCK_BOOT_PERF_NONE )
  {

    Clock_PowerDomainDisable( HWIO_GCC_UFS_PHY_GDSCR_ADDR );
    Clock_PowerDomainDisable( HWIO_GCC_UFS_PHY_CFG_GDSCR_ADDR );
  }

  /* Configure UFS clock sources */
  Clock_SetClockConfig( &cfg->UFS_Cfg[ePerfLevel] );
  Clock_SetClockConfig( &cfg->UFS_Ice_Cfg[ePerfLevel] );
  Clock_SetClockConfig( &cfg->UFS_Unipro_Cfg[ePerfLevel] );

  return TRUE;
}


/* ============================================================================
**  Function : Clock_InitVSense
** ============================================================================
*/
/*!
    This function enables the Voltage Sensor clocks, and configure them run
    at max frequency level at NOMINAL voltage.

    @return
      TRUE -- Initialization was successful.
      FALSE -- Initialization failed.

   @dependencies
     None.

*/
boolean Clock_InitVSense( void )
{
  return TRUE;
}


/* ============================================================================
**  Function : Clock_ShutdownVSense
** ============================================================================
*/
/*!
    This function disables the voltage sensor clocks.

    @return
      TRUE -- Initialization was successful.
      FALSE -- Initialization failed.

   @dependencies
      None.

*/
boolean Clock_ShutdownVSense( void )
{
  return TRUE;
}
