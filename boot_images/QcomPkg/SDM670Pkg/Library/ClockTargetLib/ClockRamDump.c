/**
==============================================================================
  @file ClockRamDump.c

  Clock functions for the XBL RAM Dump image, which primarily supports a 
  video splash screen.

==============================================================================

                             Edit History


when      who           what, where, why
--------  ------------  ------------------------------------------------------
06/07/16  bc            Initial MSM8998 revision, new file

==============================================================================
            Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "ClockHWIO.h"
#include "ClockBoot.h"
#include "ClockDriver.h"
#include "ClockBSP.h"
#include <string.h>
#include "busywait.h"

/*=========================================================================
                       MACRO DEFINITIONS
==========================================================================*/
#define DIV(div) ((int)2*div)
#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0]))

/* To keep ClockDomainID's apart from ClockID's, I am adding this offset */
#define DOMAIN_OFFSET 0x1000

/*=========================================================================
      Data
==========================================================================*/
struct 
{
  ClockConfigPLLType Disp_PLL0_Cfg;
  ClockConfigPLLType GPLL1_Cfg;
  ClockConfigPLLType GPLL4_Cfg;
  
} 
Clock_RD_Plan = 
{
  /* MMPLL0 @ 860 MHz */
  .Disp_PLL0_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(DISP_CC_PLL0_MODE),
    .nVoteAddr     =  0,
    .nVoteMask     =  0,
    .nCalibCtrl    =  2,
    .nPreDiv       =  1,
    .nL            =  44,
    .nFracVal      =  0xCAAA,
    .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
    .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
  },
    /* GPLL1 @ 1066 MHz. */
    .GPLL1_Cfg =
  {
    .bConfig       = FALSE,
    .nRefCount     = 0,
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL1_MODE),
    .nVoteAddr     =  HWIO_ADDR(GCC_SPARE_GPLL_ENA_VOTE),
    .nVoteMask     =  HWIO_FMSK(GCC_SPARE_GPLL_ENA_VOTE, GPLL1),
    .nCalibCtrl    =  2, /* 2 = Automatic */
    .nPreDiv       =  1,
    .nPostDivOdd   =  0,
    .nPostDivEven  =  0,
    .nL            =  55,
    .nCalibrationL =  0,
    .nFracVal      =  0x8555,
    .nFracFormat   =  0, /* 0 = Alpha */
    .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
    .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
  },
    /* GPLL4 @ 806 MHz */
  .GPLL4_Cfg =
  {
    .bConfig       = FALSE,
    .nRefCount     = 0,
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL4_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_SPARE_GPLL_ENA_VOTE),
    .nVoteMask     =  HWIO_FMSK(GCC_SPARE_GPLL_ENA_VOTE, GPLL4),
    .nCalibCtrl    =  2, /* 2 = Automatic */
    .nPreDiv       =  1,
    .nPostDivOdd   =  0,
    .nPostDivEven  =  0,
    .nL            =  41,
    .nCalibrationL =  0,
    .nFracVal      =  0xFAAA,
    .nFracFormat   =  0, /* 0 = Alpha */
    .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
    .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
  },

};


const struct 
{
  const char         *sDomainName;
  uintnt              nGDSCRAddr;
} Clock_RD_MMSS_Domains[] =
{
  {"mdss_core_gdsc", HWIO_ADDR(DISP_CC_MDSS_CORE_GDSCR)},
  {"mmnoc_gdsc", HWIO_ADDR(GCC_MMNOC_GDSCR)},
};

const struct 
{
  const char         *sClockName;
  uintnt              nCBCRAddr;
  uint32              nFreqKHz;
  ClockConfigMuxType  sMuxCfg;
} Clock_RD_MMSS_ClockCfg[] =
{

  {"disp_cc_mdss_esc0_clk", HWIO_ADDR(DISP_CC_MDSS_ESC0_CBCR), 
    19200, {HWIO_ADDR(DISP_CC_MDSS_ESC0_CMD_RCGR), 0, MUX_DISP, SRC_CXO, 0, 0, 0, 0}},
  {"disp_cc_mdss_pclk0_clk", HWIO_ADDR(DISP_CC_MDSS_PCLK0_CBCR), 
    358000, {HWIO_ADDR(DISP_CC_MDSS_PCLK0_CMD_RCGR), 0, MUX_DISP, SRC_EXT1, 0, 0, 0, 0}},
  {"disp_cc_mdss_byte0_clk", HWIO_ADDR(DISP_CC_MDSS_BYTE0_CBCR), 
    358000, {HWIO_ADDR(DISP_CC_MDSS_BYTE0_CMD_RCGR), 0, MUX_DISP, SRC_EXT1, 0, 0, 0, 0}},
  {"disp_cc_mdss_byte0_intf_clk", HWIO_ADDR(DISP_CC_MDSS_BYTE0_INTF_CBCR), 
    358000, {HWIO_ADDR(DISP_CC_MDSS_BYTE0_CMD_RCGR), 0, MUX_DISP, SRC_EXT1, 0, 0, 0, 0}}, /*turbo itself 179 (in Napali config is 262.50 Mhz)*/

  {"disp_cc_mdss_esc1_clk", HWIO_ADDR(DISP_CC_MDSS_ESC1_CBCR), 
    19200, {HWIO_ADDR(DISP_CC_MDSS_ESC1_CMD_RCGR), 0, MUX_DISP, SRC_CXO, 0, 0, 0, 0}},
  {"disp_cc_mdss_pclk1_clk", HWIO_ADDR(DISP_CC_MDSS_PCLK1_CBCR), 
    358000, {HWIO_ADDR(DISP_CC_MDSS_PCLK1_CMD_RCGR), 0, MUX_DISP, SRC_EXT1, 0, 0, 0, 0}},
  {"disp_cc_mdss_byte1_clk", HWIO_ADDR(DISP_CC_MDSS_BYTE1_CBCR), 
    358000, {HWIO_ADDR(DISP_CC_MDSS_BYTE1_CMD_RCGR), 0, MUX_DISP, SRC_EXT1, 0, 0, 0, 0}},
  {"disp_cc_mdss_byte1_intf_clk", HWIO_ADDR(DISP_CC_MDSS_BYTE1_INTF_CBCR), 
    358000, {HWIO_ADDR(DISP_CC_MDSS_BYTE1_CMD_RCGR), 0, MUX_DISP, SRC_EXT1, 0, 0, 0, 0}},

  {"disp_cc_mdss_vsync_clk", HWIO_ADDR(DISP_CC_MDSS_VSYNC_CBCR), 
    19200, {HWIO_ADDR(DISP_CC_MDSS_VSYNC_CMD_RCGR), 0, MUX_DISP, SRC_CXO, 0, 0, 0, 0}},

  /* These CBCs are chained, GCC on first, then DISP */
  {"gcc_disp_axi_clk", HWIO_ADDR(GCC_DISP_AXI_CBCR), 
    266500, {HWIO_ADDR(GCC_MMNOC_HF_QX_CMD_RCGR), 0, MUX_GCC, SRC_GPLL1, DIV(4), 0, 0, 0}},
  {"disp_cc_mdss_axi_clk", HWIO_ADDR(DISP_CC_MDSS_AXI_CBCR), 
    266500, {HWIO_ADDR(GCC_MMNOC_HF_QX_CMD_RCGR), 0, MUX_GCC, SRC_GPLL1, DIV(4), 0, 0, 0}}, /* How this RCG sources to this particular clk*/

  /* These CBCs are chained, GCC on first, then DISP */
  {"gcc_disp_ahb_clk", HWIO_ADDR(GCC_DISP_AHB_CBCR), 
    75000, {HWIO_ADDR(GCC_CONFIG_NOC_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, DIV(8), 0, 0, 0}},
  {"disp_cc_mdss_ahb_clk", HWIO_ADDR(DISP_CC_MDSS_AHB_CBCR), 
    75000, {HWIO_ADDR(GCC_CONFIG_NOC_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, DIV(8), 0, 0, 0}},

  {"disp_cc_mdss_mdp_clk", HWIO_ADDR(DISP_CC_MDSS_MDP_CBCR),
    430000, {HWIO_ADDR(DISP_CC_MDSS_MDP_CMD_RCGR), 0, MUX_DISP, SRC_DISP_PLL0, DIV(2), 0, 0, 0}}, 
  {"gcc_disp_xo_clk", HWIO_ADDR(GCC_DISP_XO_CBCR), 
    19200, {HWIO_ADDR(GCC_XO_CMD_RCGR), 0, MUX_GCC, SRC_CXO, DIV(1), 0, 0, 0}},

  /* gcc_memnoc_clk is already enabled and sped up */
  {"gcc_mmnoc_hf_qx_clk", HWIO_ADDR(GCC_MMNOC_HF_QX_CBCR), 
    266500, {HWIO_ADDR(GCC_MMNOC_HF_QX_CMD_RCGR), 0, MUX_GCC, SRC_GPLL1, DIV(4), 0, 0, 0}},
  {"gcc_ddrss_mmnoc_hf_qx_clk", HWIO_ADDR(GCC_DDRSS_MMNOC_HF_QX_CBCR), 
    266500, {HWIO_ADDR(GCC_MMNOC_HF_QX_CMD_RCGR), 0, MUX_GCC, SRC_GPLL1, DIV(4), 0, 0, 0}},
  {"gcc_mmnoc_tbu_hf0_clk", HWIO_ADDR(GCC_MMNOC_TBU_HF0_CBCR), 
    266500, {HWIO_ADDR(GCC_MMNOC_HF_QX_CMD_RCGR), 0, MUX_GCC, SRC_GPLL1, DIV(4), 0, 0, 0}},
  {"gcc_mmnoc_tbu_hf1_clk", HWIO_ADDR(GCC_MMNOC_TBU_HF1_CBCR), 
    266500, {HWIO_ADDR(GCC_MMNOC_HF_QX_CMD_RCGR), 0, MUX_GCC, SRC_GPLL1, DIV(4), 0, 0, 0}},

  {"gcc_mmnoc_sf_qx_clk", HWIO_ADDR(GCC_MMNOC_SF_QX_CBCR), 
    403000, {HWIO_ADDR(GCC_MMNOC_SF_QX_CMD_RCGR), 0, MUX_GCC, SRC_GPLL4, DIV(2), 0, 0, 0}},
  
};


#define MAX_EXT_SRC 3
static const ClockSourceType Clock_RD_ReverseSourceMap[MAX_EXT_SRC] = 
{
  SRC_CXO,
  SRC_EXT1,
  SRC_EXT2,
};

/*=========================================================================
      Function Prototypes
==========================================================================*/


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*!

  This function initializes the MMSS clock sources.

  @dependencies
  None.

*/
static int Clock_RD_InitSources( void )
{
  /* Be sure that DISP_CC is available */
  HWIO_OUTF(GCC_DISP_AHB_CBCR, CLK_ENABLE, 1 );
  HWIO_OUTF(GCC_MMNOC_AT_CBCR, CLK_ENABLE, 1 );

  if( HWIO_INF(DISP_CC_PLL0_MODE, PLL_LOCK_DET) == 0)
  {
    if( Clock_EnablePLL(&Clock_RD_Plan.Disp_PLL0_Cfg) != TRUE) return FALSE;
  }
  if( HWIO_INF(GCC_GPLL1_MODE, PLL_ACTIVE_FLAG) == 0)
  {
    if( Clock_EnablePLL(&Clock_RD_Plan.GPLL1_Cfg) != TRUE) return FALSE;
  }
  if( HWIO_INF(GCC_GPLL4_MODE, PLL_ACTIVE_FLAG) == 0)
  {
    if( Clock_EnablePLL(&Clock_RD_Plan.GPLL4_Cfg) != TRUE) return FALSE;
  }

  return TRUE;
}

/* FUNCTION: Clock_RD_GetClockID
 *   Input Parameter: char *sClockName -- clock name to return the ID
 *   Return Value : 
 *     Success : A clock ID in the range 0 .. number_of_clocks
 *     Failure : -1
 */
int     Clock_RD_GetClockID( const char *sClockName)
{
  int i;

  if( sClockName == NULL) return -1;

  for(i = 0; i< ARRAY_LEN(Clock_RD_MMSS_ClockCfg); i++)
  {
    if(! strcmp(sClockName, Clock_RD_MMSS_ClockCfg[i].sClockName))
    {
      return i;
    }
  }
  return -1;
};


/* FUNCTION: Clock_RD_EnableClock
 *   Input Parameter: int nClockID -- Use GetClockID() to return this for the clock in question 
 *   Return Value : TRUE=Success, FALSE=Failure or bad parameter
 * 
 * This assumes a standard CBCR (non-votable) with CLK_EN in bit 0 */
boolean Clock_RD_EnableClock( int nClockID )
{
  if( nClockID < 0 || nClockID >= ARRAY_LEN(Clock_RD_MMSS_ClockCfg)) return FALSE;

  return Clock_ToggleClock(Clock_RD_MMSS_ClockCfg[nClockID].nCBCRAddr, CLK_TOGGLE_ENABLE);
}


/* FUNCTION: Clock_RD_DisableClock
 *   Input Parameter: int nClockID -- Use GetClockID() to return this for the clock in question 
 *   Return Value : TRUE=Success, FALSE=Failure or bad parameter
 * 
 * This assumes a standard CBCR (non-votable) with CLK_EN in bit 0 */
boolean Clock_RD_DisableClock( int nClockID )
{
  if( nClockID < 0 || nClockID >= ARRAY_LEN(Clock_RD_MMSS_ClockCfg)) return FALSE;

  return Clock_ToggleClock(Clock_RD_MMSS_ClockCfg[nClockID].nCBCRAddr, CLK_TOGGLE_DISABLE);
}


/* FUNCTION: Clock_RD_IsClockEnabled
 *   Input Parameter: int nClockID -- Use GetClockID() to return this for the clock in question 
 *   Return Value : TRUE=clock is enabled, FALSE=clock is disable
 *
 * Note: This only tells you the state of the control.  After you enable a clock, it takes
 * a few cycles for it to turn on which will be indicated by IsClockOn().  If something
 * goes wrong with a clock enable, then it could be Enabled but not On, or Disabled but not Off.
 *
 * This assumes a standard CBCR (non-votable) with CLK_EN in bit 0 */
boolean Clock_RD_IsClockEnabled( int nClockID )
{
  if( nClockID < 0 || nClockID >= ARRAY_LEN(Clock_RD_MMSS_ClockCfg)) return FALSE;

  return (inp32(Clock_RD_MMSS_ClockCfg[nClockID].nCBCRAddr) & 0x1);
}


/* FUNCTION: Clock_RD_IsClockOn
 *   Input Parameter: int nClockID -- Use GetClockID() to return this for the clock in question 
 *   Return Value : TRUE=clock is on, FALSE=clock is off
 * 
 * This assumes a standard CBCR (non-votable) with CLK_OFF in bit 31 */
boolean Clock_RD_IsClockOn( int nClockID )
{
  if( nClockID < 0 || nClockID >= ARRAY_LEN(Clock_RD_MMSS_ClockCfg)) return FALSE;

  return (inp32(Clock_RD_MMSS_ClockCfg[nClockID].nCBCRAddr)>>31) == 0;
}

/* FUNCTION: Clock_RD_SetClockFreqKHz
 *   Input Parameter: nClockID -- Use GetClockID() to return this for the clock in question 
 *   Input Parameter: nFreqKHz -- The frequency to use.  If this frequency is either
 *     0 or 19200, CXO will be used.  Otherwise, the one frequency in the table is used.
 *   Return Value : TRUE=Success, FALSE=Failed
 *
 * This assumes a standard CBCR (non-votable) with CLK_OFF in bit 31 */
boolean Clock_RD_SetClockFreqKHz( int nClockID, uint32 nFreqKHz )
{
  ClockConfigMuxType muxConfig;

  /* One time initialize the MMSS PLLs */
  Clock_RD_InitSources();

  if( nClockID < 0 || nClockID >= ARRAY_LEN(Clock_RD_MMSS_ClockCfg)) return FALSE;
  muxConfig = Clock_RD_MMSS_ClockCfg[nClockID].sMuxCfg;

  /* This function has exactly two choices, 19200, or whatever is in the plan */
  if( (nFreqKHz == 19200) || (nFreqKHz == 0) ) 
  {
    muxConfig.eSource = 0;
    muxConfig.nDiv2x = 0;

    return Clock_ConfigMux(&muxConfig);
  }

  return Clock_ConfigMux(&Clock_RD_MMSS_ClockCfg[nClockID].sMuxCfg);
}


/* FUNCTION: Clock_RD_SelectExternalSource
 *   Input Parameter: nClockID -- Use GetClockID() to return this for the clock in question 
 *   Input Parameter: nSource, nDivider, nM, nN, nD : This gives the client the ability 
 *    to choose their external source with a divider, similar to the DDIClock function.
 *
 *   Return Value : TRUE=Success, FALSE=Failed
 */
boolean Clock_RD_SelectExternalSource( int nClockID, uint32 nFreqHz, uint32 nSource, uint32  nDivider,
                                       uint32  nM,  uint32  nN,  uint32  n2D)
{
  ClockConfigMuxType muxConfig;
  ClockSourceType eSource = SRC_CXO;

  if( nClockID < 0 || nClockID >= ARRAY_LEN(Clock_RD_MMSS_ClockCfg)) return FALSE;

  if (nSource<MAX_EXT_SRC) eSource = Clock_RD_ReverseSourceMap[nSource];

  muxConfig = Clock_RD_MMSS_ClockCfg[nClockID].sMuxCfg;
  muxConfig.eSource = eSource;
  muxConfig.nDiv2x  = nDivider;
  muxConfig.nM      = nM;
  muxConfig.nN      = nN;
  muxConfig.n2D     = n2D;

  return Clock_ConfigMux(&muxConfig);
}


/* FUNCTION: Clock_RD_GetClockPowerDomainID
 *   Input Parameter: char *sClockName -- clock domain to return the ID
 *   Return Value : 
 *     Success : A clock ID in the range 0x1000 .. 0x1000+number_of_domains
 *     Failure : -1
 *   The Domain IDs are offset by DOMAIN_OFFSET (0x1000) to keep these distinct
 *   from the clock IDs which start at 0.
 */
int     Clock_RD_GetClockPowerDomainID( const char *sClockDomain )
{
  int i;

  if( sClockDomain == NULL) return -1;

  for(i = 0; i< ARRAY_LEN(Clock_RD_MMSS_Domains); i++)
  {
    if(! strcmp(sClockDomain, Clock_RD_MMSS_Domains[i].sDomainName))
    {
      return i + DOMAIN_OFFSET;
    }
  }
  return -1;
};


/* FUNCTION: Clock_RD_EnableClockPowerDomain
 *   Input Parameter: int nClockDomainID -- Use GetClockPowerDomainID() to get a valid ID
 *   Return Value : TRUE=Success, FALSE=Failed
 */
boolean Clock_RD_EnableClockPowerDomain( int nClockDomainID )
{
  int loops = 200;
  int i = nClockDomainID - DOMAIN_OFFSET;
  if( i < 0|| i >= ARRAY_LEN(Clock_RD_MMSS_Domains)) return FALSE;

  outp32(Clock_RD_MMSS_Domains[i].nGDSCRAddr, inp32(Clock_RD_MMSS_Domains[i].nGDSCRAddr) & ~0x1);

  while( (loops > 0) &&  ((inp32(Clock_RD_MMSS_Domains[i].nGDSCRAddr) >> 31) == 0)) {
    loops--;
    busywait(1);
  }
  return (loops != 0);
}


/* FUNCTION: Clock_RD_DisableClockPowerDomain
 *   Input Parameter: int nClockDomainID -- Use GetClockPowerDomainID() to get a valid ID
 *   Return Value : TRUE=Success, FALSE=Failed
 */
boolean Clock_RD_DisableClockPowerDomain( int nClockDomainID )
{
  int i = nClockDomainID - DOMAIN_OFFSET;
  if( i < 0 || i >= ARRAY_LEN(Clock_RD_MMSS_Domains)) return FALSE;

  outp32(Clock_RD_MMSS_Domains[i].nGDSCRAddr, inp32(Clock_RD_MMSS_Domains[i].nGDSCRAddr) | 0x1);

  return TRUE;
}

/* I expect some fails here.  External sources will not be configured
 * during this test */
volatile int Clock_RD_successes = 0;
volatile int Clock_RD_fails = 0;

#if 0
void Clock_RD_Test(void)
{
  int i, id;
  boolean result;

  /* gcc_disp_xo_clk is necessary for MDP GDSCR to enable */
  result = Clock_RD_EnableClock( Clock_RD_GetClockID("gcc_disp_xo_clk") );
  /* Enable the Power Domains */
  for(i = 0; i< ARRAY_LEN(Clock_RD_MMSS_Domains); i++)
  {
    id = Clock_RD_GetClockPowerDomainID( Clock_RD_MMSS_Domains[i].sDomainName );
    result = Clock_RD_EnableClockPowerDomain( id );
    if(result == TRUE) { Clock_RD_successes++; } else { Clock_RD_fails++; };
    //boolean Clock_RD_DisableClockPowerDomain( int nClockDomainID )
  }

  /* Test the Clock APIs */
  for(i = 0; i< ARRAY_LEN(Clock_RD_MMSS_ClockCfg); i++)
  {
    id = Clock_RD_GetClockID(Clock_RD_MMSS_ClockCfg[i].sClockName);
    if(id >= 0) { Clock_RD_successes++; } else { Clock_RD_fails++; };

    result = Clock_RD_IsClockEnabled( id );
    if(result == FALSE) { Clock_RD_successes++; } else { Clock_RD_fails++; };
    result = Clock_RD_IsClockOn( id );
    if(result == FALSE) { Clock_RD_successes++; } else { Clock_RD_fails++; };
    result = Clock_RD_EnableClock( id );
    if(result == TRUE) { Clock_RD_successes++; } else { Clock_RD_fails++; };
    result = Clock_RD_IsClockEnabled( id );
    if(result == TRUE) { Clock_RD_successes++; } else { Clock_RD_fails++; };
    result = Clock_RD_IsClockOn( id );
    if(result == TRUE) { Clock_RD_successes++; } else { Clock_RD_fails++; };

    result = Clock_RD_DisableClock( id );
    if(result == TRUE) { Clock_RD_successes++; } else { Clock_RD_fails++; };
    result = Clock_RD_IsClockEnabled( id );
    if(result == FALSE) { Clock_RD_successes++; } else { Clock_RD_fails++; };
    result = Clock_RD_IsClockOn( id );
    if(result == FALSE) { Clock_RD_successes++; } else { Clock_RD_fails++; };

    result = Clock_RD_EnableClock( id );
    if(result == TRUE) { Clock_RD_successes++; } else { Clock_RD_fails++; };

    /* There is only one config, so you can give three things here.
     * 19200 (CXO) or 0 (also CXO) and anything else, which will select whatever 
     * it has in the table.  If the table lists CXO, you get CXO anyway. */
    if(Clock_RD_MMSS_ClockCfg[id].sMuxCfg.eSource != SRC_EXT1)
    {
      result = Clock_RD_SetClockFreqKHz(id, 100000); 
      if(result == TRUE) { Clock_RD_successes++; } else { Clock_RD_fails++; };
    }
   
  }

  // Unit testing ext clock sources is problematic because the DSI PLLs and such
  // are the property of the DSI driver and PHY and I cannot turn them on.
  {
    const char *ext_clocks[] =
    {
       "disp_cc_mdss_pclk0_clk", 
       "disp_cc_mdss_byte0_clk", 
       "disp_cc_mdss_byte0_intf_clk", 
       "disp_cc_mdss_pclk1_clk", 
       "disp_cc_mdss_byte1_clk", 
       "disp_cc_mdss_byte1_intf_clk", 
    };

    for(i = 0; i< ARRAY_LEN(ext_clocks); i++)
    {
      /* This will set 19.2 MHz */
      id = Clock_RD_GetClockID(ext_clocks[i]);
      result = Clock_RD_SelectExternalSource( id, 0, 0, 2, 0, 0, 0 );
      if(result == TRUE) { Clock_RD_successes++; } else { Clock_RD_fails++; };
    }
  }

  // Try source 1.  This will fail.
  id = Clock_RD_GetClockID( "disp_cc_mdss_byte0_intf_clk" );
  result = Clock_RD_SelectExternalSource( id, 0, 1, 2, 0, 0, 0 );
  if(result == TRUE) { Clock_RD_successes++; } else { Clock_RD_fails++; };

}
#endif

