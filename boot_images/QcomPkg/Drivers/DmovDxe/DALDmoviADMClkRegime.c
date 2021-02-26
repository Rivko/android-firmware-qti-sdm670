/*
===============================================================================

FILE:         DALDmoviADMClkRegime.c

DESCRIPTION:  
  This file implements the ADM clock control when clkregime is present.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

===============================================================================

                                Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/13/09   an      Created

===============================================================================
                   Copyright (c) 2009-2009 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================
*/

/* ----------------------------------------------------------------------------
**                           INCLUDES
** ------------------------------------------------------------------------- */

#ifndef DMOV_USE_DAL_CLOCK_DRIVER
#include "clkregim.h"
#endif
#include "DALDmoviADM.h"

/*------------------------------------------------------------------------------
Following functions are for DALDriver internal functionality
------------------------------------------------------------------------------*/

/* ============================================================================
**
**  Dmovi_ControlADM_Enable
**
**  Description:
**    Enables ADM device clock.
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_ControlADM_Enable(DmovDevCtxt *pDevCtxt)
{
  switch(pDevCtxt->dwDeviceInstance)
  {
      case HAL_DMOV_DEV_ADM:
#ifdef DMOV_USE_DAL_CLOCK_DRIVER
        /* TODO: we are ignoring the return value */
        DalClock_EnableClock(pDevCtxt->hDALClkRegim,pDevCtxt->hClockID[DMOV_CLK_ADM0]);
        DalClock_EnableClock(pDevCtxt->hDALClkRegim,pDevCtxt->hClockID[DMOV_CLK_ADM0_PBUS]);
#else
        clk_regime_resource_enable(CLKRGM_CLIENT_DMOV, CLKRGM_RESOURCE_ADM);
#endif
        break;
#ifdef DMOV_MANAGE_ADM1_CLK
      case HAL_DMOV_DEV_ADM_1:
#ifdef DMOV_USE_DAL_CLOCK_DRIVER
        DalClock_EnableClock(pDevCtxt->hDALClkRegim,pDevCtxt->hClockID[DMOV_CLK_ADM1]);
        DalClock_EnableClock(pDevCtxt->hDALClkRegim,pDevCtxt->hClockID[DMOV_CLK_ADM1_PBUS]);
#else
        clk_regime_resource_enable(CLKRGM_CLIENT_DMOV, CLKRGM_RESOURCE_ADM_1);
#endif
        break;
#endif
      default:
        break;
  }
}

/* ============================================================================
**
**  Dmovi_ControlADM_Disable
**
**  Description:
**    Disable ADM device clock.
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_ControlADM_Disable(DmovDevCtxt *pDevCtxt)
{
  switch(pDevCtxt->dwDeviceInstance)
  {
      case HAL_DMOV_DEV_ADM:
#ifdef DMOV_USE_DAL_CLOCK_DRIVER
        DalClock_DisableClock(pDevCtxt->hDALClkRegim,pDevCtxt->hClockID[DMOV_CLK_ADM0]);
        DalClock_DisableClock(pDevCtxt->hDALClkRegim,pDevCtxt->hClockID[DMOV_CLK_ADM0_PBUS]);   
#else
        clk_regime_resource_disable(CLKRGM_CLIENT_DMOV, CLKRGM_RESOURCE_ADM);
#endif
        break;
#ifdef DMOV_MANAGE_ADM1_CLK
      case HAL_DMOV_DEV_ADM_1:
#ifdef DMOV_USE_DAL_CLOCK_DRIVER
        DalClock_DisableClock(pDevCtxt->hDALClkRegim,pDevCtxt->hClockID[DMOV_CLK_ADM1]);
        DalClock_DisableClock(pDevCtxt->hDALClkRegim,pDevCtxt->hClockID[DMOV_CLK_ADM1_PBUS]);           
#else
        clk_regime_resource_disable(CLKRGM_CLIENT_DMOV, CLKRGM_RESOURCE_ADM_1);
#endif
        break;
#endif
      default:
        break;
  }
}

/* ============================================================================
**
**  Dmovi_ControlDevResetADM
**
**  Description:
**    Assert the ADM device and PBus reset.
**    NO RESET FOR NOW IN APPS/MODEM. ONLY DONE IN BOOT
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_ControlDevResetADM(DmovDevCtxt *pDevCtxt)
{
#ifndef DMOV_USE_DAL_CLOCK_DRIVER
  clk_regime_msm_reset_assert(CLKRGM_APPS_RESET_ADM);
  clk_regime_msm_reset_assert(CLKRGM_APPS_RESET_ADMCPB_PBDG);
#endif
}

/* ============================================================================
**
**  Dmovi_ControlDevInitADM
**
**  Description:
**    Initialize the ADM device.
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    Insure that device clocks are enabled.
**
** ========================================================================= */

void Dmovi_ControlDevInitADM(DmovDevCtxt *pDevCtxt)
{
#ifndef DMOV_USE_DAL_CLOCK_DRIVER
  clk_regime_msm_reset_deassert(CLKRGM_APPS_RESET_ADM);
  clk_regime_msm_reset_deassert(CLKRGM_APPS_RESET_ADMCPB_PBDG);
#endif
}

/* ============================================================================
**
**  Dmovi_ControlSetPerfLvlADM
**
**  Description:
**    Sets the desired (max possible) performance level for ADM. It chooses 
**  the best performance level requested among the clients registered with
**  ADM
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**    PerfLvl - desired performance level
**
**  Return:
**    None
**
**  Dependencies:
**    Insure that ADM clock is enabled. Only ADM is supported
**
** ========================================================================= */

void Dmovi_ControlSetPerfLvlADM(DmovDevCtxt *pDevCtxt, DmovPerfLvlType PerfLvl)
{
#ifndef DMOV_USE_DAL_CLOCK_DRIVER
  switch(PerfLvl)
  {
     case DMOV_PERFLVL_MAX: 
        clk_regime_sel_adm_clk(CLKRGM_ADM_PERF_LEVEL_MAX);
        break;
     case DMOV_PERFLVL_HIGH:
        clk_regime_sel_adm_clk(CLKRGM_ADM_PERF_LEVEL_HIGH);
        break;
     case DMOV_PERFLVL_MID:
        clk_regime_sel_adm_clk(CLKRGM_ADM_PERF_LEVEL_MID);
        break;
     case DMOV_PERFLVL_LOW:
        clk_regime_sel_adm_clk(CLKRGM_ADM_PERF_LEVEL_LOW);
        break;
  }
#endif
}
