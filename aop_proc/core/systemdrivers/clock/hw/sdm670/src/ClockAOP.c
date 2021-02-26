/*
===========================================================================
*/
/**
  @file clock_aop.c

  Interface implementation file for the clock aop driver.
*/
/*
  ====================================================================

  Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
                              EDIT HISTORY

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/systemdrivers/clock/hw/sdm670/src/ClockAOP.c#1 $ 
  $DateTime: 2018/02/16 04:48:18 $ 
  $Author: pwbldsvc $ 

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/20/17   vph     Support cap trace resource at fix voltage corner
  09/30/16   vph     Initial version

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/
#include <string.h>
#include "clock.h"
#include "aop_hwio.h"
#include "HALhwio.h"
#include "ClockHWIO.h"
#include "CoreVerify.h"
#include "ClockBSP.h"
#include "time_service.h"
#include "qdss_swevent.h"
#include "tracer_event_ids.h"
#include "icb_aop.h"
#include "arc_runtime.h"
#include "aop_interrupt_table.h"
#include "rex.h"
#include "unpa/unpa_resource.h"
#include "pwr_utils_lvl.h"
#include "arc_hal_hwio.h"
#include "qdss_lite.h"

/* SMEM */
#include "msg_ram_layout.h"
#include "page_select.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

#define DDR_COLLAPSE_CP    0x2
#define CLK_ENABLE_MSK 0x00000001
#define CLOCK_CFG_REG_OFFSET            (HWIO_ADDR(GCC_QDSS_ATB_A_CFG_RCGR)-HWIO_ADDR(GCC_QDSS_ATB_A_CMD_RCGR))
#define CLOCK_CFG_HW_CLK_CONTROL_FMSK   HWIO_FMSK(GCC_QDSS_ATB_A_CFG_RCGR, HW_CLK_CONTROL)

#define SMEM_CLOCK_INFO                 474
#define SMEM_BASE_PHY                   0x86000000
#define SMEM_FIRST_FIXED_BUFFER_SIZE    0x40
#define SMEM_VERSION_INFO_SIZE          0x80
#define SMEM_HEAP_INFO_SIZE             0x10
#define SMEM_ALLOCATION_TABLE_SIZE      0x10

#define NUM_MC 12
#define NUM_SHUB 7
typedef struct
{
  uint32 allocated;
  uint32 offset;
  uint32 size;
  uint32 base_ext;
} smem_alloc_table_type;

/*=========================================================================
      Type Definitions
==========================================================================*/
typedef struct
{
  uint16  nVersion;         /* A version number to verify matching XBL and RPM data structures */
  uint8   nNumMCStates;     /* Number of MC states */
  uint8   nNumSHUBStates;   /* Number of SHUB states */
  uint16  nBootCPOffset;    /* Offset to boot CP table address */
  uint16  nMCOffset;        /* Offset to MC table address */
  uint16  nSHUBOffset;      /* Offset to SHUB table address */
  uint16  nReserved;        /* Reserved */
} Clock_SMEMInfoType;


/* Clocks needs to disable/enable during chip sleep/restore */
const int32 AOPClockCBC[] =
{
  HWIO_ADDR(AOSS_CC_AOP_PROC_FCLK_CBCR),
  HWIO_ADDR(AOSS_CC_RPMH_CBCR),
  HWIO_ADDR(AOSS_CC_MESSAGE_RAM_CBCR),
  HWIO_ADDR(AOSS_CC_PWR_MUX_CTRL_CBCR),
  HWIO_ADDR(AOSS_CC_AO_DAP_CBCR),
  HWIO_ADDR(AOSS_CC_AOP_DAP_CBCR),
  HWIO_ADDR(AOSS_CC_WCSS_TS_CBCR),
  HWIO_ADDR(AOSS_CC_RO_CBCR),
  HWIO_ADDR(AOSS_CC_EUD_AT_CBCR),
  HWIO_ADDR(AOSS_CC_GDSC_CBCR),
  HWIO_ADDR(AOSS_CC_SWAO_RPMH_DEBUG_CBCR),
  HWIO_ADDR(AOSS_CC_SWAO_CBCR),
  HWIO_ADDR(AOSS_CC_SWAO_AO_CBCR),
  HWIO_ADDR(AOSS_CC_SWAO_MEM_CBCR),
  HWIO_ADDR(AOSS_CC_SWAO_TS_CBCR),
  HWIO_ADDR(AOSS_CC_SWAO_TS_AO_CBCR),
  HWIO_ADDR(AOSS_CC_SWAO_BUS_CBCR),
  HWIO_ADDR(AOSS_CC_SPMI_AOD_SER_CBCR),
  0
};

const int32 QDSSClockCBCAOn[] =
{
  HWIO_ADDR(GCC_MMNOC_AT_CBCR),  /* cannot be disabled or MMNOC GDSC collapse hangs */
  //HWIO_ADDR(AOSS_CC_SWAO_AO_CBCR), // always on? Enabled in XBL
  //HWIO_ADDR(AOSS_CC_SWAO_BUS_CBCR), // always on? Enabled in XBL
  //HWIO_ADDR(AOSS_CC_SWAO_CBCR), // always on?  Enabled in XBL
  //HWIO_ADDR(AOSS_CC_SWAO_TS_AO_CBCR),  // Enabled in XBL
  HWIO_ADDR(AOSS_CC_SPMI_CFG_AHB_CBCR),
  //HWIO_ADDR(GCC_SP_AT_CBCR), // Needed for SP HW bug, QCTDD03261140
  0
};


const int32 QDSSClockCBC[] =
{
  HWIO_ADDR(AOSS_CC_AO_DAP_CBCR),
  HWIO_ADDR(AOSS_CC_AOP_DAP_CBCR),
  HWIO_ADDR(AOSS_CC_SWAO_RPMH_DEBUG_CBCR),
  HWIO_ADDR(AOSS_CC_WCSS_TS_CBCR),

  HWIO_ADDR(GCC_QDSS_DAP_AHB_CBCR),
  HWIO_ADDR(GCC_QDSS_XO_CBCR),
  HWIO_ADDR(GCC_QDSS_ETR_USB_CBCR),
  HWIO_ADDR(GCC_QDSS_STM_CBCR),
  HWIO_ADDR(GCC_QDSS_TRACECLKIN_CBCR),
  HWIO_ADDR(GCC_QDSS_DAP_CBCR),
  HWIO_ADDR(GCC_SYS_NOC_AT_CBCR),
  HWIO_ADDR(GCC_QDSS_CENTER_AT_CBCR),
  HWIO_ADDR(GCC_SOUTH_AT_CBCR),
  HWIO_ADDR(GCC_EAST_AT_CBCR),
  HWIO_ADDR(GCC_NORTH_AT_CBCR),
  HWIO_ADDR(GCC_AOSS_AT_CBCR),
  HWIO_ADDR(GCC_DDRSS_AT_CBCR),
//  HWIO_ADDR(GCC_LPASS_AT_CBCR),
  HWIO_ADDR(GCC_TURING_AT_CBCR),
  HWIO_ADDR(GCC_CPUSS_AT_CBCR),
  HWIO_ADDR(GCC_MSS_AT_CBCR),
  HWIO_ADDR(GCC_WCSS_AT_CBCR),
  HWIO_ADDR(GCC_GPU_AT_CBCR),
//  HWIO_ADDR(GCC_SP_AT_CBCR),
  HWIO_ADDR(GCC_APSS_QDSS_APB_CBCR),
  HWIO_ADDR(GCC_IPA_APB_CBCR),
  HWIO_ADDR(GCC_WCSS_APB_CBCR),
  HWIO_ADDR(GCC_QDSS_TRIG_CBCR),
  HWIO_ADDR(GCC_TURING_TRIG_CBCR),  
// HWIO_ADDR(GCC_LPASS_TRIG_CBCR),
  HWIO_ADDR(GCC_CPUSS_TRIG_CBCR),
  HWIO_ADDR(GCC_MSS_TRIG_CBCR),
  HWIO_ADDR(GCC_GPU_TRIG_CBCR),
//  HWIO_ADDR(GCC_SP_TRIG_CBCR),
  HWIO_ADDR(GCC_APSS_QDSS_TSCTR_CBCR),
  HWIO_ADDR(GCC_SYS_NOC_QDSS_STM_AXI_CBCR),
  HWIO_ADDR(GCC_MMSS_AT_CBCR),
  HWIO_ADDR(GCC_MMSS_TRIG_CBCR),
  HWIO_ADDR(GCC_QDSS_TSCTR_CBCR),
  0
};

typedef struct
{
  boolean             bQDSSHWMode;
  boolean             bQDSSClocksOn;
  ClockQDSSLevelType  eQDSSLevel;
  RailWayType         CxRailWay;
  ClockConfigMuxType *pATBAActCfg;
  ClockConfigMuxType *pATBBActCfg;
  ClockConfigMuxType *pATBCActCfg;
  ClockConfigMuxType *pSTMActCfg;
  ClockConfigMuxType *pTraceActCfg;
  ClockConfigMuxType *pTRIGActCfg;
  ClockConfigMuxType *pQUPActCfg;
} ClockSettingType;

__attribute__((section("clock_data_sec")))
ClockSettingType ClockSetting =
{
  .bQDSSHWMode = TRUE,
  .bQDSSClocksOn = FALSE,
  .eQDSSLevel = CLOCK_QDSS_LEVEL_OFF,
  .CxRailWay = RAILWAY_NO_REQUEST,
  .pATBAActCfg = NULL,
  .pATBBActCfg = NULL,
  .pATBCActCfg = NULL,
  .pQUPActCfg = NULL,
};

__attribute__((section("clock_data_sec")))
ClockConfigMuxType AOPCfgData[] =
{
  {HWIO_ADDR(AOSS_CC_AOP_CMD_RCGR), SRC_AOPLL1,      1 },  /* Sleep: 19.2MHz */
  {HWIO_ADDR(AOSS_CC_AOP_CMD_RCGR), SRC_AOPLL0_EVEN, 1 },  /* Awake: 200MHz */
};

__attribute__((section("clock_data_sec")))
ClockConfigMuxType EUDATCfgData[] =
{
  {HWIO_ADDR(AOSS_CC_EUD_AT_CMD_RCGR), SRC_AOPLL1,      1  },  /* Sleep: 19.2MHz */
  {HWIO_ADDR(AOSS_CC_EUD_AT_CMD_RCGR), SRC_AOPLL0_EVEN, 10 },  /* Awake: 40MHz  */
};

__attribute__((section("clock_data_sec")))
ClockConfigMuxType SWAOCfgData[] =
{
  {HWIO_ADDR(AOSS_CC_SWAO_CMD_RCGR), SRC_AOPLL1,      1  }, /* Sleep: 19.2MHz */
  {HWIO_ADDR(AOSS_CC_SWAO_CMD_RCGR), SRC_AOPLL0_EVEN, 4 },  /* Awake:  100MHz */
};

__attribute__((section("clock_data_sec")))
ClockConfigMuxType SPMICfgData[] =
{
  {HWIO_ADDR(AOSS_CC_SPMI_AOD_SER_CMD_RCGR), SRC_AOPLL1, 4 },  /*   9.60 MHz */
  {HWIO_ADDR(AOSS_CC_SPMI_AOD_SER_CMD_RCGR), SRC_AOPLL1, 1 },  /*  19.20 MHz */
};


__attribute__((section("clock_data_sec")))
ClockCfgCPType sBootCP =
{
  .nSHUB = 5,        // System HUB
  .nSNOC = 5,        // System NOC
  .nCNOC = 4,        // Config NOC
  .nCE = 2,          // Crypto Engine
  .nIPA = 4,         // IPA
  .nDDR = 2,         // DDR
};

/* Pre-alloc for SMEM data */
__attribute__((section("clock_data_sec"))) static ClockCfgCPType BootCP;

static ClockCfgCPType* pBootCP = &sBootCP;


/*=========================================================================
      Functions
==========================================================================*/
static void Clock_TurnQDSSClocksOn( boolean bTurnOn );
/* =========================================================================
**  Function : Clock_SwitchToHWCtrlMode
** =========================================================================*/
/**
  Transfer clock power domain and PLLs from SW to HW mode.

  @param 
    None

  @return
    None

  @dependencies
    None
*/
void Clock_SwitchToHWCtrlMode
(
  void
)
{
  /* Switch all GDSCRs to HW mode */
  HWIO_OUTF( GCC_ANOC_TBU_GDSCR, HW_CONTROL, 1 );
  HWIO_OUTF( GCC_IPA_GDSCR, HW_CONTROL, 1 );
   HWIO_OUTF( GCC_MMNOC_GDSCR, HW_CONTROL, 1 );

  /* Remove SW votes on PLLs */
  HWIO_OUTF( GCC_SPARE_GPLL_ENA_VOTE, GPLL0, 0 );
  HWIO_OUTF( GCC_SPARE_GPLL_ENA_VOTE, GPLL1, 0 );
  HWIO_OUTF( GCC_SPARE_GPLL_ENA_VOTE, GPLL2, 0 );
  HWIO_OUTF( GCC_SPARE_GPLL_ENA_VOTE, GPLL3, 0 );
  HWIO_OUTF( GCC_SPARE_GPLL_ENA_VOTE, GPLL4, 0 );
  HWIO_OUTF( GCC_SPARE_GPLL_ENA_VOTE, GPLL5, 0 );

  /* Remove SW votes on DDR clock branches. */
  HWIO_OUTF( GCC_DDRMC_CH0_CBCR, CLK_ENABLE, 0 );
  HWIO_OUTF( GCC_DDRMC_CH1_CBCR, CLK_ENABLE, 0 );

  /* Enable fast frequence switching. Required to support DDR low power modes. */
  HWIO_OUTF(MCCC_BAND0_CFG0, FAST_FREQ_SWITCH_ENABLE, 1);
  HWIO_OUT(GCC_RPMH_DDRMC_FAKE_SWITCH_DEBUG, 0);

} /* END Clock_SwitchToHWCtrlMode */


/* =========================================================================
**  Function : Clock_TurnClocksOn
** =========================================================================*/
/**
  Turn clocks on/off

  @param 
    bTurnOn - TRUE -  ON
            - FALSE - OFF
  @return
    None

  @dependencies
    None
*/
static void Clock_TurnClocksOn( const int32* pClockList, boolean bTurnOn )
{
  uint32 nValue, nIndex = 0;

  while ( pClockList[nIndex] != 0 )
  {
    if ( bTurnOn )
      nValue = in_dword(pClockList[nIndex]) | CLK_ENABLE_MSK ;
    else
      nValue = in_dword(pClockList[nIndex]) & ~CLK_ENABLE_MSK ;

    out_dword( pClockList[nIndex], nValue );
    nIndex++;
  }
}


/* =========================================================================
**  Function : Clock_SetDFSRCG
** =========================================================================*/
/**
  This function is used for configure DFS RCG index for setting clock RCG to 
  SW control mode.

  @param 
    nDFSAddr - DFS HWIO register address
    nRCGBMsk - DFS HWIO register bitmask
    nRCGShft - DFS HWIO register bitshift
    nRCGIndex - RCG index

  @return
    None

  @dependencies
    None
*/
static void Clock_SetDFSRCG
(
  uint32 nDFSAddr,
  uint32 nRCGBMsk,
  uint32 nRCGShft,
  uint8  nRCGIndex
)
{
  uint32 nDFSVal = in_dword(nDFSAddr);

  nDFSVal |= 1 << (nRCGShft+nRCGIndex);
  out_dword( nDFSAddr, nDFSVal );
}



/* =========================================================================
**  Function : Clock_TargetInit
** =========================================================================*/
/**
  Perform target specific clock configuration
  @param
    None
  @return
    None

  @dependencies
    None
*/

void Clock_TargetInit( void )
{
  /* For Modem QDSS v1 work-around, enable QDSS for SPMI */
  Clock_TurnClocksOn( QDSSClockCBCAOn, 1);
  
  /*
   * Keep PRNG clock on all the time, since according to the PRNG HW
   * team it's not safe to turn off and doing so causes random
   * hangs accessing PRNG.
   */
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 1);
}

/* =========================================================================
**  Function : Clock_TurnQDSSClocksOn
** =========================================================================*/
/**
  Turn QDSS clocks on/off

  @param 
    bTurnOn - TRUE -  ON
            - FALSE - OFF
  @return
    None

  @dependencies
    None
*/
static void Clock_TurnQDSSClocksOn( boolean bTurnOn )
{
  

  /* Enable QDSS_CFG_AHB clock first if enable */
  if ( bTurnOn )
  {
    HWIO_OUTF( GCC_RPM_CLOCK_BRANCH_ENA_VOTE, QDSS_CFG_AHB_CLK_ENA, 1 );
  }

  if (!bTurnOn) { 
     QDSSTraceDisable();
  }
  HWIO_OUTF(GCC_APB_CBCR, CLK_ENABLE, bTurnOn);
  HWIO_OUTF(AOSS_CC_CDBGPWRUPREQ, REQ, bTurnOn);
  Clock_TurnClocksOn( QDSSClockCBC, bTurnOn );

  if (bTurnOn) {
     QDSSTraceEnable();
  }
  /* Disable QDSS_CFG_AHB clock last if disable */
  if ( !bTurnOn )
  {
    //Needed for MPSS QDSS SPMI work-around
    //HWIO_OUTF( GCC_RPM_CLOCK_BRANCH_ENA_VOTE, QDSS_CFG_AHB_CLK_ENA, 0 );
  }
}


/* =========================================================================
**  Function : Clock_SwitchMuxToSWMode
** =========================================================================*/
/**
  Switch Mux to SW mode

  @param 
    pCfgMux - Configure Mux

  @return
    None

  @dependencies
    None
*/
static void Clock_SwitchMuxToSWMode( DFSSWType* pDFSCfg )
{
  switch ( pDFSCfg->eDfs )
  {
  case DFS_SHUB_DFS:
    Clock_SetDFSRCG ( HWIO_ADDR(GCC_RPMH_SHUB_CMD_DFSR),
                      HWIO_FMSK(GCC_RPMH_SHUB_CMD_DFSR, RCG_SW_CTRL),
                      HWIO_SHFT(GCC_RPMH_SHUB_CMD_DFSR, RCG_SW_CTRL),
                      pDFSCfg->nRCGIndex );
    break;

  case DFS_SYS_NOC:
    Clock_SetDFSRCG ( HWIO_ADDR(GCC_RPMH_SYS_NOC_CMD_DFSR),
                      HWIO_FMSK(GCC_RPMH_SYS_NOC_CMD_DFSR, RCG_SW_CTRL),
                      HWIO_SHFT(GCC_RPMH_SYS_NOC_CMD_DFSR, RCG_SW_CTRL),
                      pDFSCfg->nRCGIndex );
    break;

  default:
    break;
  }
}


/* =========================================================================
**  Function : Clock_QDSSSwitchToSWMode
** =========================================================================*/
/**
  Switch all QDSS Muxes to SW mode

  @param 
    None

  @return
    None

  @dependencies
    None
*/
static void Clock_QDSSSwitchToSWMode( void )
{
  Clock_ConfigType* pBSPCfg = Clock_GetConfigData();

  Clock_SwitchMuxToSWMode( &pBSPCfg->QDSS_ATBADfs );
  Clock_SwitchMuxToSWMode( &pBSPCfg->QDSS_ATBBDfs );
  Clock_SwitchMuxToSWMode( &pBSPCfg->QDSS_ATBCDfs );
  Clock_SwitchMuxToSWMode( &pBSPCfg->QDSS_STMDfs );
  Clock_SwitchMuxToSWMode( &pBSPCfg->QDSS_TRACEDfs );
  Clock_SwitchMuxToSWMode( &pBSPCfg->QDSS_TRIGDfs );
}


/* =========================================================================
**  Function : Clock_SwitchMuxConfig
** =========================================================================*/
/**
  Switch mux configure to new settings.

  @param 
    pNewCfgMux - New Configure Mux
    pOldCfgMux - Old Configure Mux

  @return
    SUCCESS or FAIL

  @dependencies
    None
*/
static int32 Clock_SwitchMuxConfig( ClockConfigMuxType* pNewCfgMux, ClockConfigMuxType* pOldCfgMux )
{
  /* Do nothing if new config level is already at active config */
  if ( pNewCfgMux == pOldCfgMux )
    return CLOCK_SUCCESS;

  /* Enable new Source */
  if ( Clock_EnableSource(pNewCfgMux->eSource) != CLOCK_SUCCESS )
    return CLOCK_ERROR;

  /* Switch clock */
  if ( Clock_ConfigMux(pNewCfgMux) != CLOCK_SUCCESS )
    return CLOCK_ERROR;

  /* Disable old source */
  if ( (pOldCfgMux != NULL) && (Clock_DisableSource(pOldCfgMux->eSource) != CLOCK_SUCCESS) )
    return CLOCK_ERROR;

  /* Update active config to new config */
  pOldCfgMux = pNewCfgMux;
  return CLOCK_SUCCESS;
}


/* =========================================================================
**  Function : Clock_SetQUPPerfLevel
** =========================================================================*/
/**
  Configure QUPv3 performance level.

  @param 
    nLevel - Performance level

  @return
    CLOCK_SUCCESS - Set configuration success
    CLOCK_ERROR   - Set configuration fails

  @dependencies
    None
*/
int32 Clock_SetQUPPerfLevel( uint32 nLevel )
{
  static ClockConfigMuxType* pActiveCfg = NULL;
  Clock_ConfigType* pBSPCfg = Clock_GetConfigData();
  ClockConfigMuxType* pNewCfg;
  boolean bEnable;

  if ( nLevel >= CLOCK_QUPV3_PERF_NUM )
    return CLOCK_ERROR;

  pNewCfg = &pBSPCfg->QUPV3Cfg[nLevel];
  if (pNewCfg == pActiveCfg)
    return CLOCK_SUCCESS;

  if (Clock_SwitchMuxConfig(pNewCfg, pActiveCfg) != CLOCK_SUCCESS)
    return CLOCK_ERROR;

  bEnable = (nLevel > 0);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_CORE_2X_CLK_ENA, bEnable);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP0_CORE_CLK_ENA,    bEnable);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_CORE_2X_CLK_ENA, bEnable);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE_1, QUPV3_WRAP1_CORE_CLK_ENA,    bEnable);

  return CLOCK_SUCCESS;

} /* END Clock_SetQUPPerfLevel */


/* =========================================================================
**  Function : Clock_QDSSSetSpeedLevel
** =========================================================================*/
/**
  Configure QDSS performance level.

  @param 
    nSpeedLevel - Performance level
    bSetVoltage - Used for set voltage only in SW control mode

  @return
    CLOCK_SUCCESS - Set configuration success
    CLOCK_ERROR   - Set configuration fails

  @dependencies
    None
*/
static int32 Clock_QDSSSetSpeedLevel( uint32 nLevel, boolean bSetVoltage )
{
  uint32 nPerfLevel, index;
  Clock_ConfigType* pBSPCfg = Clock_GetConfigData();
  Clock_QDSSMapType *pMapping;

  /* Search perf level index */
  nPerfLevel = 0;
  pMapping = pBSPCfg->QDSS_Mapping;
  for ( index=0; index < CLOCK_QDSS_PERF_NUM; index++ )
  {
    if ( pMapping[index].eLevel == nLevel )
    {
      nPerfLevel = index;
      break;
    }
  }

  /* Increase voltage before switch clocks frequency since frequency scale up */
  if ( bSetVoltage && (pMapping[nPerfLevel].eCxRailway > ClockSetting.CxRailWay) )
  {
    // TO DO - SET VOLTAGE
  }

  Clock_SwitchMuxConfig( &pBSPCfg->QDSS_ATBACfg[nPerfLevel], ClockSetting.pATBAActCfg );
  Clock_SwitchMuxConfig( &pBSPCfg->QDSS_ATBBCfg[nPerfLevel], ClockSetting.pATBBActCfg );
  Clock_SwitchMuxConfig( &pBSPCfg->QDSS_ATBCCfg[nPerfLevel], ClockSetting.pATBCActCfg );
  Clock_SwitchMuxConfig( &pBSPCfg->QDSS_STMCfg[nPerfLevel], ClockSetting.pSTMActCfg );
  Clock_SwitchMuxConfig( &pBSPCfg->QDSS_TRACECfg[nPerfLevel], ClockSetting.pTraceActCfg );
  Clock_SwitchMuxConfig( &pBSPCfg->QDSS_TRIGCfg[nPerfLevel], ClockSetting.pTRIGActCfg );

  /* Decrease voltage after clocks */
  if ( bSetVoltage && (pMapping[nPerfLevel].eCxRailway < ClockSetting.CxRailWay) )
  {
    // TO DO - SET VOLTAGE
  }

  ClockSetting.CxRailWay = pMapping[nPerfLevel].eCxRailway;

  return 0;
}

/* =========================================================================
**  Function : Clock_QDSSSetPerfLevel
** =========================================================================*/
/**
  Set QDSS performance level.
  @param
    eLevel - QDSS performance level
  @return
    None

  @dependencies
    None
*/
void Clock_QDSSSetPerfLevel( ClockQDSSLevelType eLevel )
{
  if ( eLevel > CLOCK_QDSS_LEVEL_MAX_LEVEL )
    eLevel = CLOCK_QDSS_LEVEL_MAX_LEVEL;

  /* Temporary block SW mode due to blocking CX collapse */
  if ( eLevel > CLOCK_QDSS_LEVEL_DYNAMIC )
    eLevel = CLOCK_QDSS_LEVEL_DYNAMIC;

  /* Do nothing if the level is already set */
  if ( eLevel == ClockSetting.eQDSSLevel )
    return;

  /* Save current level */
  ClockSetting.eQDSSLevel = eLevel;

  /* Ignore dynamic switch since it cannot go back to HW mode */
  if ( (eLevel==CLOCK_QDSS_LEVEL_DYNAMIC) && (ClockSetting.bQDSSHWMode!=TRUE) )
    return;

  if ( ClockSetting.bQDSSHWMode )
  {
    if ( CLOCK_QDSS_LEVEL_OFF == eLevel )
    {
      if ( ClockSetting.bQDSSClocksOn )
      {
        Clock_TurnQDSSClocksOn( FALSE );
        ClockSetting.bQDSSClocksOn = FALSE;
        return;
      }
    }
    else if ( CLOCK_QDSS_LEVEL_DYNAMIC == eLevel )
    {
      if ( !ClockSetting.bQDSSClocksOn )
      {
        Clock_TurnQDSSClocksOn( TRUE );
        ClockSetting.bQDSSClocksOn = TRUE;
        return;
      }
    }
  }

  /* Switch to SW control mdoe */
  if ( ClockSetting.bQDSSHWMode && (CLOCK_QDSS_LEVEL_OFF!=eLevel) && (CLOCK_QDSS_LEVEL_DYNAMIC!=eLevel) )
  {
    Clock_QDSSSwitchToSWMode();
    ClockSetting.bQDSSHWMode = FALSE;
  }

  /* Configure QDSS clocks speed */
  Clock_QDSSSetSpeedLevel( eLevel, !ClockSetting.bQDSSHWMode );

  if ( CLOCK_QDSS_LEVEL_OFF == eLevel )
  {
    /* Turn Clocks off if they were on before */
    if ( ClockSetting.bQDSSClocksOn == TRUE )
    {
      Clock_TurnQDSSClocksOn( FALSE );
      ClockSetting.bQDSSClocksOn = FALSE;
    }
  }
  else
  {
    /* Turn Clocks on if they were off before */
    if ( ClockSetting.bQDSSClocksOn == FALSE )
    {
      Clock_TurnQDSSClocksOn( TRUE );
      ClockSetting.bQDSSClocksOn = TRUE;
    }
  }
}


/* =========================================================================
**  Function : Clock_ProcessorSleep
** =========================================================================*/
/**
  This function is used for disable all necessary AOP PLLs and clocks before
  chip sleep.

  @param
    None
  @return
    None

  @dependencies
    None
*/
void Clock_ProcessorSleep( void )
{
  /* Switch clocks to sleep performance level before disabling AO PLL0. */
  Clock_ConfigMux( &(AOPCfgData[0]) );
  Clock_ConfigMux( &(EUDATCfgData[0]) );
  Clock_ConfigMux( &(SWAOCfgData[0]) );
  Clock_ConfigMux( &(SPMICfgData[0]) );

  /* Put AOP PLLs in standby mode */
  HWIO_OUTF( AOSS_CC_PLL0_USER_CTL_U, BIAS_ON_IN_STANDBY, 1);
  HWIO_OUTF( AOSS_CC_PLL0_OPMODE, PLL_OPMODE, 0);
    if( ClockSetting.bQDSSClocksOn == FALSE) {
  /* Allow PLL1 shutdown, it will turn off later in the sequencer */
  HWIO_OUTF( AOSS_CC_PLL1_OPMODE, PLL_OPMODE, 0);
	}
  /* Shutdown AOP clocks */
  Clock_TurnClocksOn( AOPClockCBC, FALSE );
}


/* =========================================================================
**  Function : Clock_ProcessorRestore
** =========================================================================*/
/**
  This function is used for restore all necessary AOP PLLs and clocks that
  were disabled during chip sleep.

  @param
    None
  @return
    None

  @dependencies
    None
*/
void Clock_ProcessorRestore( void )
{
  /* Restore AOP clocks */
  Clock_TurnClocksOn( AOPClockCBC, TRUE );

  /* Allow PLL1 shutdown, it will turn off later in the sequencer */
  HWIO_OUTF( AOSS_CC_PLL1_OPMODE, PLL_OPMODE, 1);

  /* Bring AOP PLLs out of standby mode */
  HWIO_OUTF( AOSS_CC_PLL0_USER_CTL_U, BIAS_ON_IN_STANDBY, 0);
  HWIO_OUTF( AOSS_CC_PLL0_OPMODE, PLL_OPMODE, 1);

  /* Wait until PLL stable before enable output */
  while ( HWIO_INF(AOSS_CC_PLL0_MODE, PLL_LOCK_DET) == 0 )
  {
    busywait(1);
  }

  /* Switch clocks back to awake performance levels */
  Clock_ConfigMux( &(AOPCfgData[1]) );
  Clock_ConfigMux( &(EUDATCfgData[1]) );
  Clock_ConfigMux( &(SWAOCfgData[1]) );
  Clock_ConfigMux( &(SPMICfgData[1]) );
}

/* =========================================================================
**  Function : Clock_ProcessorReduce
** =========================================================================*/
/**
  This function is used for doing clock reductions before CPU halt

  @param
    boolean enable : true reduces the CPU clock, false restores it to full speed
  @return
    None

  @dependencies
    None
*/
void Clock_ProcessorReduce( boolean enable )
{
  static uint32 aoss_cc_swao_cfg, aoss_cc_swao_ts_cfg;
  uint32 div = 0x0;

  if ( enable ) {
    if( ClockSetting.bQDSSClocksOn == FALSE) {
    aoss_cc_swao_cfg = HWIO_IN(AOSS_CC_SWAO_CFG_RCGR);
    HWIO_OUT(AOSS_CC_SWAO_CFG_RCGR, 0x31F);
    aoss_cc_swao_ts_cfg = HWIO_IN(AOSS_CC_SWAO_TS_CFG_RCGR);
    HWIO_OUT(AOSS_CC_SWAO_TS_CFG_RCGR, 0x31F);
	HWIO_OUT(AOSS_CC_SWAO_CMD_RCGR, 0x1);
	HWIO_OUT(AOSS_CC_SWAO_TS_CMD_RCGR, 0x1);
	}
    div = 0xf;
  } else {
    if( ClockSetting.bQDSSClocksOn == FALSE) {
    HWIO_OUT(AOSS_CC_SWAO_CFG_RCGR, aoss_cc_swao_cfg);
    HWIO_OUT(AOSS_CC_SWAO_TS_CFG_RCGR, aoss_cc_swao_ts_cfg);
	HWIO_OUT(AOSS_CC_SWAO_CMD_RCGR, 0x1);
	HWIO_OUT(AOSS_CC_SWAO_TS_CMD_RCGR, 0x1);
	}
  }

  HWIO_OUT(AOSS_CC_AOP_DIV_CDIVR, div);
} 


/* =========================================================================
**  Function : Clock_GetBootConfigCP
** =========================================================================*/
/**
  This function is used for getting clock performance level settings in XBL

  @param
    None
  @return
    Pointer to Clock configure settings structure

  @dependencies
    None
*/
ClockCfgCPType* Clock_GetBootConfigCP( void )
{
  if ( pBootCP == &sBootCP ) 
  {
    /* The Clock Plan is not initialized yet, so go get it */
    Clock_CopyConfigFromShared();
  }
  return pBootCP;
}


/* =========================================================================
**  Function : Clock_GetBootConfigDDR
** =========================================================================*/
/**
  This function is used for getting DDR configure settings that have been
  setting in XBL

  @param
    None
  @return
    Pointer to DDR settings structure

  @dependencies
    None
*/
DDRCfgType* Clock_GetBootConfigDDR( void )
{
  return NULL;
}


uint32 get_smem_addr(uint32 smem_id)
{
  uint32 smem_addr = 0x0;
  uint32 table_addr = 0x0;
  smem_alloc_table_type *alloc_table = NULL;

  table_addr = SMEM_BASE_PHY + SMEM_FIRST_FIXED_BUFFER_SIZE +
               SMEM_VERSION_INFO_SIZE + SMEM_HEAP_INFO_SIZE;

  table_addr += smem_id * sizeof(smem_alloc_table_type);

  alloc_table = (smem_alloc_table_type *)set_ddr_access ((uint64_t)table_addr);

  if (alloc_table->allocated)
  {
    smem_addr = SMEM_BASE_PHY + alloc_table->offset;
  }

  set_page_select (0);

  return smem_addr;
}


/* =========================================================================
**  Function : Clock_CopyConfigFromShared
** =========================================================================*/
/**
  This function is used for getting clock configure settings in XBL.  These
  data has been stored in SMEM.

  @param
    None

  @return
    None

  @dependencies
    None
*/
void Clock_CopyConfigFromShared( void )
{
  Clock_SMEMInfoType* pSMEMInfo;
  freq_state *pMC, *pSHUB;
  uint32 nSize;
  char *pSMEM;
  smem_table_err eError;

  if ( pBootCP != &sBootCP ) 
  {
    /* Prevent double init, if this was already setup */
    return;
  }
  
  eError = msg_ram_get_smem_addr(SMEM_CLOCK_INFO, (uint32 *)&pSMEM);
    
  if ( eError != SMEM_TABLE_SUCCESS )
  {
    pSMEM = (char*)get_smem_addr( SMEM_CLOCK_INFO );
  }
  
  if ( pSMEM == NULL )
  {
    /* No clock data in SMEM.  Used hardcode settings */
    return;
  }

  pSMEM = (char*)set_ddr_access( (uint64_t)pSMEM );
  pSMEMInfo = (Clock_SMEMInfoType*)pSMEM;

  CORE_VERIFY ( pSMEMInfo->nVersion == CLOCK_SMEM_VERSION );

  nSize = sizeof(ClockCfgCPType);
  memcpy( &BootCP, pSMEM + pSMEMInfo->nBootCPOffset, nSize );
  pBootCP = &BootCP;

  set_page_select (0);
}

