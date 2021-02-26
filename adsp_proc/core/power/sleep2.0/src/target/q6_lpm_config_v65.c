/*==============================================================================
  FILE:         q6_lpm_config_v65.c

  OVERVIEW:     This file implements the APIs from q6_lpm_config header file
                on subsystems for specific Q6 versions.

  NOTE:         This file depends on how Q6 core is integrated within a 
                subystem.
                
                Based on HPG_QDSP6SS_V65_1.0 (rev 0.99 - Aug 4, 2016)

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/target/q6_lpm_config_v65.c#4 $
$DateTime: 2017/11/16 08:24:49 $
==============================================================================*/
#include "comdef.h"
#include "rsc.h"
#include "q6_lpm_config.h"
#include "sleep_hwio.h"
#include "CoreVerify.h"
#include "sleep_target.h"
#include "sleep_statsi.h"

/*==============================================================================
                           EXTERNAL VARIABLES
 =============================================================================*/
/* g_sleepTargetConfig stores target specific config */
extern sleep_target_config g_sleepTargetConfig;

/*==============================================================================
 *                             GLOBAL FUNCTIONS
 *============================================================================*/
/*
 * q6LPMConfig_setupBase
 */
void q6LPMConfig_setupBase(void)
{
  static sleepStats_timestamps dummyWakeupTimes;

  /* Configure Q6SS registers that are invariant across all cpu_vdd low-power modes */
  SLEEP_HWIO_OUT(SLPC_CFG, 
                 SLEEP_HWIO_FVAL(SLPC_CFG, STAGGER_LKAD_DIS,    0x1) | /* Not used */
                 SLEEP_HWIO_FVAL(SLPC_CFG, MEM_FAST_STAGGER,    0x1) | /* Rise & fall xo edge */
                 SLEEP_HWIO_FVAL(SLPC_CFG, IDLE_CORE_CLK_EN,    0x0) |
                 SLEEP_HWIO_FVAL(SLPC_CFG, EXTHW_WAKE_EN,       0x0) | /* Wake from ext hw trig */
                 SLEEP_HWIO_FVAL(SLPC_CFG, MEM_PU_PERI_STAGGER, 0x1) |
                 SLEEP_HWIO_FVAL(SLPC_CFG, MEM_PD_PERI_STAGGER, 0x0) |
                 SLEEP_HWIO_FVAL(SLPC_CFG, MEM_PU_ARRY_STAGGER, 0x1) |
                 SLEEP_HWIO_FVAL(SLPC_CFG, MEM_PD_ARRY_STAGGER, 0x0) |
                 SLEEP_HWIO_FVAL(SLPC_CFG, WAKEUP_IN_EN,        0x0) | /* Default - mode based */
                 SLEEP_HWIO_FVAL(SLPC_CFG, CLK_GATING_MODE,     0x0)); /* Default - mode based */

  SLEEP_HWIO_OUTF(WAKEUP_CTL, L2VIC_WAKE_EN, 0x01);

  SLEEP_HWIO_OUT(RSCCTL_EN_CLAMP, 0x3);
  SLEEP_HWIO_OUT(RSCCTL_EN_CLK,   0x3FF);

  SLEEP_HWIO_OUT(RSCCTL_EN_MEM_CTL,
                 SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_CTL, CLAMP_QMC_MEM,  0x1) |
                 SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_CTL, CLAMP_WL,       0x1) | /* Not used */
                 SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_CTL, MEM_PERIPH,     0x1));

  SLEEP_HWIO_OUTF(RSCCTL_EN_BHS, BHS, 0x01);

  SLEEP_HWIO_OUT(RSCCTL_EN_LDO,
                 SLEEP_HWIO_FVAL(RSCCTL_EN_LDO, BG_OFF,           0x1) |
                 SLEEP_HWIO_FVAL(RSCCTL_EN_LDO, FORCEOFF,         0x1) |
                 SLEEP_HWIO_FVAL(RSCCTL_EN_LDO, VREF_SEL_UPDATE,  0x1) |
                 SLEEP_HWIO_FVAL(RSCCTL_EN_LDO, BYPASS_OPEN,      0x1));

  SLEEP_HWIO_OUTF(RSCCTL_EN_EXT, XO_ENABLE, 0x1); /* Not used in LPASS/SSC/CSS */
          
  SLEEP_HWIO_OUT(RSCCTL_EN_PLL,
                 SLEEP_HWIO_FVAL(RSCCTL_EN_PLL, PLL_STBY,     0x1) |
                 SLEEP_HWIO_FVAL(RSCCTL_EN_PLL, PLL_OUTDIS,   0x1) |
                 SLEEP_HWIO_FVAL(RSCCTL_EN_PLL, PLL_OPMODE1,  0x0) | /* Not used */
                 SLEEP_HWIO_FVAL(RSCCTL_EN_PLL, PLL_RESET,    0x0));

  SLEEP_HWIO_OUT(RSC_EVENT_PLL_OVR,
                 SLEEP_HWIO_FVAL(RSC_EVENT_PLL_OVR, PLL_FINE_OVRRD,  0x1) |
                 SLEEP_HWIO_FVAL(RSC_EVENT_PLL_OVR, PLL_OVRRD,       0x0));

  SLEEP_HWIO_OUT(RSC_EVENT_MEM_OVR, 0);

#ifdef SLEEP_IMAGE_SLPI
  SLEEP_HWIO_OUTF(RSCCTL_EN_PRSC, TCS_HW_TRIGGER,  0xff);
#else
  SLEEP_HWIO_OUTF(RSCCTL_EN_PRSC, TCS_HW_TRIGGER,  0x00);
#endif

  /* Default to no handshaking with parent / hwsolver */
  sleepTarget_setRpmHandshake(FALSE);

  /* Always defer non-wakeup qTimer frames (0 and 1) because the local wakeup timer is
   * always programmed */
  SLEEP_HWIO_OUT(L2VIC_WAKEUP_EN, WAKEUP_EN_QTMR_FRAME_2);

  /* Clear out any previous wakeup timestamps */
  sleepStats_readWakeupTimes(&dummyWakeupTimes);

  return;
}

/*
 * q6LPMConfig_setupModeConfig
 */
void q6LPMConfig_setupModeConfig(rsc_low_power_mode_type coreMode, 
                                 boolean                 flag)
{
  uint32 ares;
  uint32 q6WakeHandshake  = 0;
  uint32 wakeupInterrupt  = 0;
  uint32 saveRestore      = 0;
  uint32 configRsc        = 0;

  switch(coreMode)
  {
    case RSC_MODE_APCR_PLL_LPM:
    {
      /* RSC Driver does not support efficient storage of sequences when
       * multiple low-power mode employ the same rsc sequence commands.
       * RSC Long sequence supports both FULL PC and APCR_PLL_LPM modes.  */
      coreMode = RSC_MODE_PWRC_BHS;
    }

    case RSC_MODE_APCR_PLL_ON:
#ifdef SLEEP_IMAGE_SLPI
    case RSC_MODE_APCR_PLL_LPM_NO_SS:
#endif
    {
      q6WakeHandshake   = 0x1;
      saveRestore       = 0x1;
      ares              = 0x1efe;
      configRsc         = 0x1; /* Configure RSC low-power mode */
      break;
    }
    
    case RSC_MODE_L2_NRET_BHS:
    {
      coreMode = RSC_MODE_PWRC_BHS;
    }

    case RSC_MODE_PWRC_BHS:
    {
      ares              = 0x1fff;
      configRsc         = 0x1; /* Configure RSC low-power mode */
      break;
    }

    case RSC_MODE_CLK_GATE:
    {
      /* Clock gating bit overrides all other settings, so just set this one bit.
       * This setting will be in effect until explicitly unset */
      SLEEP_HWIO_OUTF(SLPC_CFG, CLK_GATING_MODE, (flag == TRUE) ? 0x1 : 0x0);
      return;
    }

    default:
    {
      CORE_VERIFY(0);
      break;
    }
  }

  wakeupInterrupt = (flag == TRUE) ? 0x1 : 0x0;

  SLEEP_HWIO_OUTF(SLPC_CFG, WAKEUP_IN_EN, q6WakeHandshake);
  SLEEP_HWIO_OUT(RSCCTL_EN_ARES, ares);
  SLEEP_HWIO_OUT(RSCCTL_EN_STATERET,
                 SLEEP_HWIO_FVAL(RSCCTL_EN_STATERET, WAKE_IRQ, wakeupInterrupt)  |
                 SLEEP_HWIO_FVAL(RSCCTL_EN_STATERET, RESTORE,  saveRestore)      |
                 SLEEP_HWIO_FVAL(RSCCTL_EN_STATERET, SAVE,     saveRestore));

  /* Configure RSC if required by lprm */
  if(0 != configRsc)
  { 
    sleepTarget_setRscLowPowerMode(RSC_CHILD, coreMode);
  } 

  return;
}

/*
 * q6LPMConfig_setupL2Config
 */
void q6LPMConfig_setupL2Config(mem_state state)
{
  uint32 enMemRet;
  uint32 l2data = SLEEP_HWIO_INF(RSCCTL_EN_L2MEM_RET, L2DATA);
  
  if(MEM_RET == state)
  {
    /* Setup for cache related logic to be retained */ 
    l2data &= ~g_sleepTargetConfig.l2_mask;

    enMemRet = SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2CLADE,   0x00) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2TAG,     0x00) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1DU,      0x00) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU,      0x00) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1DU_TAG,  0x00) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, JU,        0x00) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2PLRU,    0x00) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, ETB,       0x00) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2STBUF,   0x01) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_BTB,  0x01) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_TAG,  0x00) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_PDEC, 0x00);
  }
  else
  {
    /* Setup for cache related logic to be turned off/reset */
    l2data |= g_sleepTargetConfig.l2_mask;

    enMemRet = SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2CLADE,   0x03) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2TAG,     0x0f) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1DU,      0x03) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU,      0x03) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1DU_TAG,  0x01) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, JU,        0x01) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2PLRU,    0x01) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, ETB,       0x03) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L2STBUF,   0x01) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_BTB,  0x01) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_TAG,  0x03) |
               SLEEP_HWIO_FVAL(RSCCTL_EN_MEM_RET, L1IU_PDEC, 0x03);
  }
 
  SLEEP_HWIO_OUTF(RSCCTL_EN_L2MEM_RET, L2DATA, l2data);
  SLEEP_HWIO_OUT(RSCCTL_EN_MEM_RET, enMemRet);
  
  return;
}

/*
 * q6LPMConfig_setupTCMConfig
 */
void q6LPMConfig_setupTCMConfig(mem_state state)
{
  uint32 l2data = SLEEP_HWIO_INF(RSCCTL_EN_L2MEM_RET, L2DATA);
  
  if(MEM_RET == state)
  {
    /* Setup for cache related logic to be retained */ 
    l2data &= ~g_sleepTargetConfig.tcm_mask;
  }
  else
  {
    /* Setup for cache related logic to be turned off/reset */
    l2data |= g_sleepTargetConfig.tcm_mask;
  }
 
  SLEEP_HWIO_OUTF(RSCCTL_EN_L2MEM_RET, L2DATA, l2data);

  return;
}

/*
 * q6LPMConfig_setRpmhHandshake
 */
void q6LPMConfig_setRpmhHandshake(boolean performHandshake)
{
  uint16  request   = (TRUE == performHandshake) ? 1 : 0;
  uint16  nRequest  = (TRUE == performHandshake) ? 0 : 1;

  /* Override the child -> parent handshake singnals */
  SLEEP_HWIO_OUTF(RSCCTL_EN_PRSC, BRINGUP_REQ,  request);
  SLEEP_HWIO_OUTF(RSCCTL_EN_PRSC, SHUTDOWN_REQ, request);

  /* If not performing the handshake, we also need to override the ACK signal
   * in the sequence since it is generic */
  SLEEP_HWIO_OUTF(RSC_EVENT_PRSC_OVR, BRINGUP_ACK_OVRRD,  nRequest);
  SLEEP_HWIO_OUTF(RSC_EVENT_PRSC_OVR, SHUTDOWN_ACK_OVRRD, nRequest);

  return;
}

