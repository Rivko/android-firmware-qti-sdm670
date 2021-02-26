/*
==============================================================================

FILE:         HALclk.c

DESCRIPTION:
  Common code for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/HALclkSharedLib/HALclk.c#2 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/31/16   sr      Updated to match the refactored clock driver.
07/18/11   vs      Ported from the latest clock driver.

==============================================================================
      Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALclk.h>
#include "HALclkInternal.h"

#include <HALhwio.h>



/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/* ============================================================================
**    Typedefs
** ==========================================================================*/


/*
 * HAL_clk_Type
 *
 * Consolidated set of internal static data for the clock HAL.
 *
 * mHALClkCtxt       - HAL clock driver context data pointer
 */
typedef struct
{
  HAL_clk_ContextType mHALClkCtxt;
} HAL_clk_Type;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk
 *
 * Consolidated static data for the clock HAL.
 *
 * We fill in some non-zero data to ensure that this ends up in the RW
 * segment instead of ZI since the clock HAL is used in the bootloader
 * before the ZI is cleared.
 */
static HAL_clk_Type HAL_clk;


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_IsClockSupported
**
** ======================================================================== */

boolean HAL_clk_IsClockSupported
(
  HAL_clk_ClockDescType *pmClockDesc,
  HAL_clk_ClockAPIType   eAPI
)
{
  /*
   * Basic check.
   */
  if (pmClockDesc == NULL)
  {
    return FALSE;
  }

  if (eAPI == HAL_CLK_CLOCK_API_CALCFREQ)
  {
    return (pmClockDesc->nDebugMuxSel != HAL_CLK_NO_DEBUG_MUX_SEL);
  }

  if (pmClockDesc->pmControl == NULL)
  {
    return FALSE;
  }

  /*
   * Check the API.
   */
  switch (eAPI)
  {
    case HAL_CLK_CLOCK_API_ENABLE:
      return (pmClockDesc->pmControl->Enable != NULL);

    case HAL_CLK_CLOCK_API_DISABLE:
      return (pmClockDesc->pmControl->Disable != NULL);

    case HAL_CLK_CLOCK_API_RESET:
      return (pmClockDesc->pmControl->Reset != NULL);

    case HAL_CLK_CLOCK_API_INVERT:
      return (pmClockDesc->pmControl->Invert != NULL);

    case HAL_CLK_CLOCK_API_ISON:
      return (pmClockDesc->pmControl->IsOn != NULL);

    case HAL_CLK_CLOCK_API_ISENABLED:
      return (pmClockDesc->pmControl->IsEnabled != NULL);

    case HAL_CLK_CLOCK_API_CONFIG:
      return (pmClockDesc->pmControl->Config != NULL);

    case HAL_CLK_CLOCK_API_CONFIGDIVIDER:
      return (pmClockDesc->pmControl->ConfigDivider != NULL);

    default:
      /*
       * Do nothing for unsupported APIs.
       */
      break;
  }

  /*
   * Unknown API.
   */
  return FALSE;

} /* END HAL_clk_IsClockSupported */


/* ===========================================================================
**  HAL_clk_EnableClock
**
** ======================================================================== */

void HAL_clk_EnableClock
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  if (pmClockDesc                    != NULL &&
      pmClockDesc->pmControl         != NULL &&
      pmClockDesc->pmControl->Enable != NULL )
  {
    pmClockDesc->pmControl->Enable(pmClockDesc);
  }

} /* END HAL_clk_EnableClock */


/* ===========================================================================
**  HAL_clk_InvertClock
**
** ======================================================================== */

void HAL_clk_InvertClock
(
  HAL_clk_ClockDescType *pmClockDesc,
  boolean                bInvert
)
{
  if (pmClockDesc                    != NULL &&
      pmClockDesc->pmControl         != NULL &&
      pmClockDesc->pmControl->Invert != NULL )
  {
    pmClockDesc->pmControl->Invert(pmClockDesc, bInvert);
  }

} /* HAL_clk_InvertClock */


/* ===========================================================================
**  HAL_clk_IsClockDomainSupported
**
** ======================================================================== */

boolean HAL_clk_IsClockDomainSupported
(
  HAL_clk_ClockDomainDescType  *pmClockDomainDesc,
  HAL_clk_ClockDomainAPIType    eAPI
)
{
  if (pmClockDomainDesc            == NULL ||
      pmClockDomainDesc->pmControl == NULL)
  {
    return FALSE;
  }

  switch (eAPI)
  {
    case HAL_CLK_CLOCK_DOMAIN_API_ISON:
      return (pmClockDomainDesc->pmControl->IsOn != NULL);

    default:
      return FALSE;
  }

} /* END HAL_clk_IsClockDomainSupported */


/* ===========================================================================
**  HAL_clk_DisableClock
**
** ======================================================================== */

void HAL_clk_DisableClock
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  if (pmClockDesc                     != NULL &&
      pmClockDesc->pmControl          != NULL &&
      pmClockDesc->pmControl->Disable != NULL )
  {
    pmClockDesc->pmControl->Disable(pmClockDesc);
  }

} /* END HAL_clk_DisableClock */


/* ===========================================================================
**  HAL_clk_IsClockDomainOn
**
** ======================================================================== */

boolean HAL_clk_IsClockDomainOn
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc
)
{
  if (pmClockDomainDesc                  != NULL &&
      pmClockDomainDesc->pmControl       != NULL &&
      pmClockDomainDesc->pmControl->IsOn != NULL)
  {
    return pmClockDomainDesc->pmControl->IsOn(pmClockDomainDesc);
  }

  return FALSE;

} /* END HAL_clk_IsClockDomainOn */


/* ===========================================================================
**  HAL_clk_IsClockDomainDFSEnabled
**
** ======================================================================== */

void HAL_clk_EnableDFS
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc
)
{
  if (pmClockDomainDesc                              != NULL &&
      pmClockDomainDesc->pmControl                   != NULL &&
      pmClockDomainDesc->pmControl->EnableDFS        != NULL)
  {
    pmClockDomainDesc->pmControl->EnableDFS(pmClockDomainDesc);
  }

} /* END HAL_clk_IsClockDomainDFSEnabled */


/* ===========================================================================
**  HAL_clk_IsClockDomainDFSEnabled
**
** ======================================================================== */

boolean HAL_clk_IsClockDomainDFSEnabled
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc
)
{
  if (pmClockDomainDesc                              != NULL &&
      pmClockDomainDesc->pmControl                   != NULL &&
      pmClockDomainDesc->pmControl->IsDFSEnabled     != NULL)
  {
    return pmClockDomainDesc->pmControl->IsDFSEnabled(pmClockDomainDesc);
  }

  return FALSE;

} /* END HAL_clk_IsClockDomainDFSEnabled */


/* ===========================================================================
**  HAL_clk_IsClockEnabled
**
** ======================================================================== */

boolean HAL_clk_IsClockEnabled
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  if (pmClockDesc                       != NULL &&
      pmClockDesc->pmControl            != NULL &&
      pmClockDesc->pmControl->IsEnabled != NULL )
  {
    return pmClockDesc->pmControl->IsEnabled(pmClockDesc);
  }

  return FALSE;

} /* END HAL_clk_IsClockEnabled */


/* ===========================================================================
**  HAL_clk_IsClockOn
**
** ======================================================================== */

boolean HAL_clk_IsClockOn
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  if (pmClockDesc                  != NULL &&
      pmClockDesc->pmControl       != NULL &&
      pmClockDesc->pmControl->IsOn != NULL )
  {
    return pmClockDesc->pmControl->IsOn(pmClockDesc);
  }

  return FALSE;

} /* END HAL_clk_IsClockOn */


/* ===========================================================================
**  HAL_clk_ResetClock
**
** ======================================================================== */

void HAL_clk_ResetClock
(
  HAL_clk_ClockDescType *pmClockDesc,
  boolean                bReset
)
{
  if (pmClockDesc                   != NULL &&
      pmClockDesc->pmControl        != NULL &&
      pmClockDesc->pmControl->Reset != NULL )
  {
    pmClockDesc->pmControl->Reset(pmClockDesc, bReset);
  }

} /* END HAL_clk_ResetClock */


/* ===========================================================================
**  HAL_clk_IsClockReset
**
** ======================================================================== */

boolean HAL_clk_IsClockReset
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  if (pmClockDesc                     != NULL &&
      pmClockDesc->pmControl          != NULL &&
      pmClockDesc->pmControl->IsReset != NULL )
  {
    return pmClockDesc->pmControl->IsReset(pmClockDesc);
  }

  return FALSE;

} /* END HAL_clk_IsClockReset */


/* ===========================================================================
**  HAL_clk_ResetClockBranch
**
** ======================================================================== */

void HAL_clk_ResetClockBranch
(
  HAL_clk_ClockDescType *pmClockDesc,
  boolean                bReset
)
{
  if (pmClockDesc                         != NULL &&
      pmClockDesc->pmControl              != NULL &&
      pmClockDesc->pmControl->ResetBranch != NULL )
  {
    pmClockDesc->pmControl->ResetBranch(pmClockDesc, bReset);
  }

} /* END HAL_clk_ResetClock */


/* ===========================================================================
**  HAL_clk_ConfigClockDivider
**
** ======================================================================== */

void HAL_clk_ConfigClockDivider
(
  HAL_clk_ClockDescType *pmClockDesc,
  uint32                 nDiv
)
{
  if (pmClockDesc                           != NULL &&
      pmClockDesc->pmControl                != NULL &&
      pmClockDesc->pmControl->ConfigDivider != NULL )
  {
    pmClockDesc->pmControl->ConfigDivider(pmClockDesc, nDiv);
  }

} /* END HAL_clk_ConfigClockDivider */


/* ===========================================================================
**  HAL_clk_ConfigClock
**
** ======================================================================== */

void HAL_clk_ConfigClock
(
  HAL_clk_ClockDescType  *pmClockDesc,
  HAL_clk_ClockConfigType eConfig
)
{
  if (pmClockDesc                    != NULL &&
      pmClockDesc->pmControl         != NULL &&
      pmClockDesc->pmControl->Config != NULL )
  {
    pmClockDesc->pmControl->Config(pmClockDesc, eConfig);
  }

} /* END HAL_clk_ConfigClock */


/* ===========================================================================
**  HAL_clk_ConfigClockMux
**
** ======================================================================== */

void HAL_clk_ConfigClockMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
)
{
  if (pmClockDomainDesc                       != NULL &&
      pmClockDomainDesc->pmControl            != NULL &&
      pmClockDomainDesc->pmControl->ConfigMux != NULL )
  {
    pmClockDomainDesc->pmControl->ConfigMux(pmClockDomainDesc, pmConfig);
  }

} /* END HAL_clk_ConfigClockMux */


/* ===========================================================================
**  HAL_clk_DetectClockConfig
**
** ======================================================================== */

void HAL_clk_DetectClockConfig
(
  HAL_clk_ClockDescType *pmClockDesc,
  uint32                *pnConfig,
  boolean               *pbValid
)
{
  *pbValid = FALSE;

  if (pmClockDesc                          != NULL &&
      pmClockDesc->pmControl               != NULL &&
      pmClockDesc->pmControl->DetectConfig != NULL )
  {
    pmClockDesc->pmControl->DetectConfig(pmClockDesc, pnConfig, pbValid);
  }

} /* END HAL_clk_DetectClockConfig */


/* ===========================================================================
**  HAL_clk_DetectClockDivider
**
** ======================================================================== */

void HAL_clk_DetectClockDivider
(
  HAL_clk_ClockDescType *pmClockDesc,
  uint32                *pnDiv
)
{
  *pnDiv = 0;

  if (pmClockDesc                           != NULL &&
      pmClockDesc->pmControl                != NULL &&
      pmClockDesc->pmControl->DetectDivider != NULL )
  {
    pmClockDesc->pmControl->DetectDivider(pmClockDesc, pnDiv);
  }

} /* END HAL_clk_DetectClockDivider */


/* ===========================================================================
**  HAL_clk_DetectClockMuxConfig
**
** ======================================================================== */

void HAL_clk_DetectClockMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  if (pmConfig == NULL)
  {
    return;
  }

  if (pmClockDomainDesc                             != NULL &&
      pmClockDomainDesc->pmControl                  != NULL &&
      pmClockDomainDesc->pmControl->DetectMuxConfig != NULL )
  {
    pmClockDomainDesc->pmControl->DetectMuxConfig(pmClockDomainDesc, pmConfig);
  }
  else
  {
    /* TODO: return error code */
  }

} /* END HAL_clk_DetectClockMuxConfig */


/* ===========================================================================
**  HAL_clk_ConfigDFSMux
**
** ======================================================================== */

void HAL_clk_ConfigDFSMux
(
  HAL_clk_ClockDomainDescType   *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType    *pmConfig
)
{
  if(pmConfig == NULL)
  {
    return;
  }

  if(pmClockDomainDesc                          != NULL &&
     pmClockDomainDesc->pmControl               != NULL &&
     pmClockDomainDesc->pmControl->ConfigDFSMux != NULL)
  {
    pmClockDomainDesc->pmControl->ConfigDFSMux(pmClockDomainDesc, pmConfig);
  }

} /* END HAL_clk_ConfigDFSMux */


/* ===========================================================================
**  HAL_clk_DetectDFSMuxConfig
**
** ======================================================================== */

void HAL_clk_DetectDFSMuxConfig
(
  HAL_clk_ClockDomainDescType   *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType    *pmConfig,
  uint32                         nPerfLevel
)
{
  if(pmConfig == NULL)
  {
    return;
  }

  if(pmClockDomainDesc                                != NULL &&
     pmClockDomainDesc->pmControl                     != NULL &&
     pmClockDomainDesc->pmControl->DetectDFSMuxConfig != NULL)
  {
    pmClockDomainDesc->pmControl->DetectDFSMuxConfig(pmClockDomainDesc, pmConfig, nPerfLevel);
  }

} /* END HAL_clk_DetectDFSMuxConfig */


/* ===========================================================================
**  HAL_clk_UpdateRootEnable
**
** ======================================================================== */

void HAL_clk_UpdateRootEnable
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  boolean                      bAssert
)
{

  if(pmClockDomainDesc                        != NULL &&
     pmClockDomainDesc->pmControl             != NULL &&
     pmClockDomainDesc->pmControl->RootEnable != NULL )
  {
    pmClockDomainDesc->pmControl->RootEnable(pmClockDomainDesc, bAssert);
  }

} /* END HAL_clk_UpdateRootEnable */


/* ===========================================================================
**  HAL_clk_ConfigFootswitch
**
** ======================================================================== */

void HAL_clk_ConfigFootswitch
(
  HAL_clk_ClockDescType       *pmClockDesc,
  HAL_clk_FootswitchConfigType eConfig
)
{
  if (pmClockDesc                              != NULL &&
      pmClockDesc->pmControl                   != NULL &&
      pmClockDesc->pmControl->ConfigFootswitch != NULL )
  {
    pmClockDesc->pmControl->ConfigFootswitch(pmClockDesc, eConfig);
  }

} /* END HAL_clk_ConfigFootswitch */


/* ===========================================================================
**  HAL_clk_EnablePowerDomain
**
** ======================================================================== */

void HAL_clk_EnablePowerDomain
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  if (pmPowerDomainDesc                    != NULL &&
      pmPowerDomainDesc->pmControl         != NULL &&
      pmPowerDomainDesc->pmControl->Enable != NULL)
  {
    pmPowerDomainDesc->pmControl->Enable(pmPowerDomainDesc);
  }

} /* END HAL_clk_EnablePowerDomain */


/* ===========================================================================
**  HAL_clk_DisablePowerDomain
**
** ======================================================================== */

void HAL_clk_DisablePowerDomain
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  if (pmPowerDomainDesc                     != NULL &&
      pmPowerDomainDesc->pmControl          != NULL &&
      pmPowerDomainDesc->pmControl->Disable != NULL)
  {
    pmPowerDomainDesc->pmControl->Disable(pmPowerDomainDesc);
  }

} /* END HAL_clk_DisablePowerDomain */


/* ===========================================================================
**  HAL_clk_IsPowerDomainSupported
**
** ======================================================================== */

boolean HAL_clk_IsPowerDomainSupported
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc,
  HAL_clk_PowerDomainAPIType   eAPI
)
{
  /*
   * Basic check.
   */
  if (pmPowerDomainDesc == NULL)
  {
    return FALSE;
  }

  if (pmPowerDomainDesc->pmControl == NULL)
  {
    return FALSE;
  }

  /*
   * Check the API.
   */
  switch (eAPI)
  {
    case HAL_CLK_POWER_DOMAIN_API_ENABLE:
      return (pmPowerDomainDesc->pmControl->Enable != NULL);

    case HAL_CLK_POWER_DOMAIN_API_DISABLE:
      return (pmPowerDomainDesc->pmControl->Disable != NULL);

    case HAL_CLK_POWER_DOMAIN_API_ISENABLED:
      return (pmPowerDomainDesc->pmControl->IsEnabled != NULL);

    case HAL_CLK_POWER_DOMAIN_API_ISON:
      return (pmPowerDomainDesc->pmControl->IsOn != NULL);

    case HAL_CLK_POWER_DOMAIN_API_ENABLE_HW:
      return (pmPowerDomainDesc->pmControl->EnableHW != NULL);

    case HAL_CLK_POWER_DOMAIN_API_DISABLE_HW:
      return (pmPowerDomainDesc->pmControl->DisableHW != NULL);

    default:
      /*
       * Do nothing for unsupported APIs.
       */
      break;
  }

  /*
   * Unknown API.
   */
  return FALSE;

} /* END HAL_clk_IsPowerDomainSupported */


/* ===========================================================================
**  HAL_clk_IsPowerDomainEnabled
**
** ======================================================================== */

boolean HAL_clk_IsPowerDomainEnabled
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  if (pmPowerDomainDesc                       != NULL &&
      pmPowerDomainDesc->pmControl            != NULL &&
      pmPowerDomainDesc->pmControl->IsEnabled != NULL)
  {
    return pmPowerDomainDesc->pmControl->IsEnabled(pmPowerDomainDesc);
  }

  return FALSE;

} /* END HAL_clk_IsPowerDomainEnabled */


/* ===========================================================================
**  HAL_clk_IsPowerDomainOn
**
** ======================================================================== */

boolean HAL_clk_IsPowerDomainOn
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  if (pmPowerDomainDesc                  != NULL &&
      pmPowerDomainDesc->pmControl       != NULL &&
      pmPowerDomainDesc->pmControl->IsOn != NULL)
  {
    return pmPowerDomainDesc->pmControl->IsOn(pmPowerDomainDesc);
  }

  return FALSE;

} /* END HAL_clk_IsPowerDomainOn */


/* ===========================================================================
**  HAL_clk_EnablePowerDomainHW
**
** ======================================================================== */

void HAL_clk_EnablePowerDomainHW
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  if (pmPowerDomainDesc                      != NULL &&
      pmPowerDomainDesc->pmControl           != NULL &&
      pmPowerDomainDesc->pmControl->EnableHW != NULL)
  {
    pmPowerDomainDesc->pmControl->EnableHW(pmPowerDomainDesc);
  }

} /* END HAL_clk_EnablePowerDomainHW */


/* ===========================================================================
**  HAL_clk_DisablePowerDomainHW
**
** ======================================================================== */

void HAL_clk_DisablePowerDomainHW
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  if (pmPowerDomainDesc                       != NULL &&
      pmPowerDomainDesc->pmControl            != NULL &&
      pmPowerDomainDesc->pmControl->DisableHW != NULL)
  {
    pmPowerDomainDesc->pmControl->DisableHW(pmPowerDomainDesc);
  }

} /* END HAL_clk_DisablePowerDomainHW */


/* ===========================================================================
**  HAL_clk_EnableSource
**
** ======================================================================== */

boolean HAL_clk_EnableSource
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  if (pSource                    != NULL &&
      pSource->pmControl         != NULL &&
      pSource->pmControl->Enable != NULL)
  {
    return pSource->pmControl->Enable(pSource, eMode, pData);
  }

  return FALSE;

} /* END HAL_clk_EnableSource */


/* ===========================================================================
**  HAL_clk_DisableSource
**
** ======================================================================== */

void HAL_clk_DisableSource
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  if (pSource                     != NULL &&
      pSource->pmControl          != NULL &&
      pSource->pmControl->Disable != NULL)
  {
    pSource->pmControl->Disable(pSource, eMode, pData);
  }

} /* END HAL_clk_DisableSource */


/* ===========================================================================
**  HAL_clk_EnableSourceVote
**
** ======================================================================== */

boolean HAL_clk_EnableSourceVote
(
  HAL_clk_SourceDescType *pSource
)
{
  if (pSource                        != NULL &&
      pSource->pmControl             != NULL &&
      pSource->pmControl->EnableVote != NULL)
  {
    return pSource->pmControl->EnableVote(pSource);
  }

  return FALSE;

} /* END HAL_clk_EnableSourceVote */


/* ===========================================================================
**  HAL_clk_DisableSourceVote
**
** ======================================================================== */

void HAL_clk_DisableSourceVote
(
  HAL_clk_SourceDescType *pSource
)
{
  if (pSource                         != NULL &&
      pSource->pmControl              != NULL &&
      pSource->pmControl->DisableVote != NULL)
  {
    pSource->pmControl->DisableVote(pSource);
  }

} /* END HAL_clk_DisableSourceVote */


/* ===========================================================================
**  HAL_clk_ConfigSource
**
** ======================================================================== */

void HAL_clk_ConfigSource
(
  HAL_clk_SourceDescType  *pSource,
  HAL_clk_SourceConfigType eConfig
)
{
  /*
   * Configure the source if possible.
   */
  if (pSource                    != NULL &&
      pSource->pmControl         != NULL &&
      pSource->pmControl->Config != NULL)
  {
    pSource->pmControl->Config(pSource, eConfig);
  }

} /* END HAL_clk_ConfigSource */


/* ===========================================================================
**  HAL_clk_ConfigPLL
**
** ======================================================================== */

boolean HAL_clk_ConfigPLL
(
  HAL_clk_SourceDescType      *pSource,
  const HAL_clk_PLLConfigType *pConfig,
  HAL_clk_SourceConfigMode     eMode
)
{
  /*
   * Configure the source if possible.
   */
  if (pSource                       != NULL &&
      pSource->pmControl            != NULL &&
      pSource->pmControl->ConfigPLL != NULL)
  {
    return pSource->pmControl->ConfigPLL(pSource, pConfig, eMode);
  }

  return FALSE;

} /* END HAL_clk_ConfigPLL */


/* ===========================================================================
**  HAL_clk_DetectPLLConfig
**
** ======================================================================== */

void HAL_clk_DetectPLLConfig
(
  HAL_clk_SourceDescType *pSource,
  HAL_clk_PLLConfigType  *pConfig
)
{
  /*
   * Initialize to NULL.
   */
  pConfig->nSource = 0; /* TODO: why? */

  /*
   * Request the frequency for this config.
   */
  if (pSource                             != NULL &&
      pSource->pmControl                  != NULL &&
      pSource->pmControl->DetectPLLConfig != NULL)
  {
    pSource->pmControl->DetectPLLConfig(pSource, pConfig);
  }

} /* END HAL_clk_DetectPLLConfig */


/* ===========================================================================
**  HAL_clk_IsSourceVoteEnabled
**
** ======================================================================== */

boolean HAL_clk_IsSourceVoteEnabled
(
  HAL_clk_SourceDescType *pSource
)
{
  /*
   * Invoke the callback.
   */
  if (pSource                           != NULL &&
      pSource->pmControl                != NULL &&
      pSource->pmControl->IsVoteEnabled != NULL)
  {
    return pSource->pmControl->IsVoteEnabled(pSource);
  }

  return FALSE;

} /* END HAL_clk_IsSourceVoteEnabled */


/* ===========================================================================
**  HAL_clk_WaitForClockOff
**
** ======================================================================== */

boolean HAL_clk_WaitForClockOff
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uint32 nTimeout = 0;
  boolean bIsOn;

  if (pmClockDesc                  == NULL ||
      pmClockDesc->pmControl       == NULL ||
      pmClockDesc->pmControl->IsOn == NULL )
  {
    return FALSE;
  }

  bIsOn = pmClockDesc->pmControl->IsOn(pmClockDesc);

  while (bIsOn && nTimeout++ < 100)
  {
    HAL_clk_BusyWait(1);
    bIsOn = pmClockDesc->pmControl->IsOn(pmClockDesc);
  }

  return !bIsOn;

} /* END HAL_clk_WaitForClockOff */


/* ===========================================================================
**  HAL_clk_WaitForClockOn
**
** ======================================================================== */

boolean HAL_clk_WaitForClockOn
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uint32  nTimeout = 0;
  boolean bIsOn;

  if (pmClockDesc                  == NULL ||
      pmClockDesc->pmControl       == NULL ||
      pmClockDesc->pmControl->IsOn == NULL )
  {
    return FALSE;
  }

  bIsOn = pmClockDesc->pmControl->IsOn(pmClockDesc);

  while (!bIsOn && nTimeout++ < 100)
  {
    HAL_clk_BusyWait(1);
    bIsOn = pmClockDesc->pmControl->IsOn(pmClockDesc);
  }

  return bIsOn;

} /* END HAL_clk_WaitForClockOn */


/* ===========================================================================
**  HAL_clk_IsSourceEnabled
**
** ======================================================================== */

boolean HAL_clk_IsSourceEnabled
(
  HAL_clk_SourceDescType *pSource
)
{
  /*
   * Invoke the callback.
   */
  if (pSource                       != NULL &&
      pSource->pmControl            != NULL &&
      pSource->pmControl->IsEnabled != NULL)
  {
    return pSource->pmControl->IsEnabled(pSource);
  }

  return FALSE;

} /* END HAL_clk_IsSourceEnabled */


/* ===========================================================================
**  HAL_clk_BusyWait
**
** ======================================================================== */

void HAL_clk_BusyWait
(
  uint32 uSec
)
{
  if (HAL_clk.mHALClkCtxt.BusyWait != NULL)
  {
     HAL_clk.mHALClkCtxt.BusyWait(uSec);
  }
  else
  {
    /* We can have our own nop loop here but it won't be precise since
     * we don't know how many time to iterate to meet the required uSec
     * pause - need to know cpu frequency.
     */
  }

} /* END HAL_clk_BusyWait */


/* ===========================================================================
**  HAL_clk_Init
**
** ======================================================================== */

void HAL_clk_Init
(
  HAL_clk_ContextType *pHALClkCtxt
)
{
  /*
   * Sanity check
   */
  if (pHALClkCtxt != NULL)
  {
     /*
      * Save the busy wait function pointer
      */
     HAL_clk.mHALClkCtxt.BusyWait = pHALClkCtxt->BusyWait;

     /*
      * Save the chip id, version, and family.
      */
     HAL_clk.mHALClkCtxt.nChipVersion = pHALClkCtxt->nChipVersion;
     HAL_clk.mHALClkCtxt.nChipId = pHALClkCtxt->nChipId;
     HAL_clk.mHALClkCtxt.nChipFamily = pHALClkCtxt->nChipFamily;
  }

} /* END HAL_clk_Init */


/* ===========================================================================
**  HAL_clk_GetChipVersion
**
** ======================================================================== */

uint32 HAL_clk_GetChipVersion ( void )
{
  return HAL_clk.mHALClkCtxt.nChipVersion;

} /* END HAL_clk_GetChipVersion */


/* ===========================================================================
**  HAL_clk_GetChipId
**
** ======================================================================== */

uint32 HAL_clk_GetChipId ( void )
{
  return HAL_clk.mHALClkCtxt.nChipId;

} /* END HAL_clk_GetChipId */


/* ===========================================================================
**  HAL_clk_GetChipFamily
**
** ======================================================================== */

uint32 HAL_clk_GetChipFamily ( void )
{
  return HAL_clk.mHALClkCtxt.nChipFamily;

} /* END HAL_clk_GetChipFamily */


/* ===========================================================================
**  HAL_clk_ValidateRegSetTimeout
**
** ======================================================================== */

boolean HAL_clk_ValidateRegSetTimeout
(
  uintnt nAddr,
  uint32 nMask,
  uint32 nTimeoutUS
)
{
  /*
   * Wait for the register field to set during the timeout period.
   */
  do
  {
    if (in_dword_masked(nAddr, nMask))
    {
      return TRUE;
    }
    /*
     * Wait 1 us between each register read.
     */
    else
    {
      HAL_clk_BusyWait(1);
    }
  }

  /*
   * nTimeoutUS is post-decremented to guarantee that we waited up to
   * the original nTimeoutUS value.
   */
  while (nTimeoutUS--);

  return FALSE;

} /* END HAL_clk_ValidateRegSetTimeout */


/* ===========================================================================
**  HAL_clk_ValidateRegClearTimeout
**
** ======================================================================== */

boolean HAL_clk_ValidateRegClearTimeout
(
  uintnt nAddr,
  uint32 nMask,
  uint32 nTimeoutUS
)
{
  /*
   * Wait for the register field to set during the timeout period.
   */
  do
  {
    /*
     * Read the register once for each iteration.
     */
    if (!in_dword_masked(nAddr, nMask))
    {
      return TRUE;
    }
    /*
     * Wait 1 us between each register read.
     */
    else
    {
      HAL_clk_BusyWait(1);
    }

  /*
   * nTimeoutUS is post-decremented to guarantee that we waited up to
   * the original nTimeoutUS value.
   */
  } while (nTimeoutUS--);

  return FALSE;

} /* END HAL_clk_ValidateRegClearTimeout */


/* ===========================================================================
**  HAL_clk_ConfigClockDivider
**
** ======================================================================== */

void HAL_clk_ConfigDivider
(
  HAL_clk_DividerDescType *pmDividerDesc,
  uint32                   nDiv
)
{
  if (pmDividerDesc                           != NULL &&
      pmDividerDesc->pmControl                != NULL &&
      pmDividerDesc->pmControl->ConfigDivider != NULL )
  {
    pmDividerDesc->pmControl->ConfigDivider(pmDividerDesc, nDiv);
  }

} /* END HAL_clk_ConfigClockDivider */


/* ===========================================================================
**  HAL_clk_IsClockMuxConfigEqual
**
** ======================================================================== */

boolean HAL_clk_IsClockMuxConfigEqual
(
  const HAL_clk_ClockMuxConfigType *pConfigA,
  const HAL_clk_ClockMuxConfigType *pConfigB
)
{
  if (pConfigA->nMuxSel == pConfigB->nMuxSel  &&
      pConfigA->nDiv2x  == pConfigB->nDiv2x   &&
      pConfigA->nM      == pConfigB->nM       &&
      pConfigA->nN      == pConfigB->nN)
  {
    return TRUE;
  }

  return FALSE;

} /* END HAL_clk_IsClockMuxConfigEqual */


/* ===========================================================================
**  HAL_clk_IsPLLConfigEqual
**
** ======================================================================== */

boolean HAL_clk_IsPLLConfigEqual
(
  const HAL_clk_SourceDescType *pSource,
  const HAL_clk_PLLConfigType  *pConfigA,
  const HAL_clk_PLLConfigType  *pConfigB
)
{
  /*
   * Request the frequency for this config.
   */
  if (pSource != NULL &&
      pSource->pmControl != NULL &&
      pSource->pmControl->IsPLLConfigEqual != NULL)
  {
    return pSource->pmControl->IsPLLConfigEqual(pConfigA, pConfigB);
  }

  return FALSE;

} /* END HAL_clk_IsPLLConfigEqual */


/* ===========================================================================
**  HAL_clk_ConfigDebugMux
**
** ======================================================================== */

void HAL_clk_ConfigDebugMux
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32                     nMuxSel
)
{
  if(pmDebugMux                       != NULL &&
     pmDebugMux->pmControl            != NULL &&
     pmDebugMux->pmControl->ConfigMux != NULL )
  {
    pmDebugMux->pmControl->ConfigMux(pmDebugMux, nMuxSel);
  }

} /* END HAL_clk_ConfigDebugMux */


/* ===========================================================================
**  HAL_clk_CalcDebugMuxFreq
**
** ======================================================================== */

void HAL_clk_CalcDebugMuxFreq
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32                    *pnFreqHz
)
{
  if(pmDebugMux                       != NULL &&
     pmDebugMux->pmControl            != NULL &&
     pmDebugMux->pmControl->CalcFreq  != NULL )
  {
    pmDebugMux->pmControl->CalcFreq(pmDebugMux, pnFreqHz);
  }
  else if (pnFreqHz)
  {
    *pnFreqHz = 0;
  }

} /* END HAL_clk_CalcDebugMuxFreq */

