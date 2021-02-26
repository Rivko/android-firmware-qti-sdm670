/*
==============================================================================

FILE:         HALclkGeneric.c

DESCRIPTION:
   This file contains common clock HAL code for the Badger family of chipsets.


==============================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/HALclkSharedLib/HALclkGeneric.c#2 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/12/11   vs      Created

==============================================================================
      Copyright © 2011-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include <string.h>
#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkGeneric.h"



/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Prototypes
** ==========================================================================*/


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk_GenericClockControl
 *
 * Common functions for controlling clocks.
 */
HAL_clk_ClockControlType HAL_clk_GenericClockControl =
{
  /* .Enable           = */ HAL_clk_GenericEnable,
  /* .Disable          = */ HAL_clk_GenericDisable,
  /* .IsEnabled        = */ HAL_clk_GenericIsEnabled,
  /* .IsOn             = */ HAL_clk_GenericIsOn,
  /* .Reset            = */ HAL_clk_GenericReset,
  /* .IsReset          = */ HAL_clk_GenericIsReset,
  /* .Config           = */ NULL,
  /* .DetectConfig     = */ NULL,
  /* .ConfigDivider    = */ HAL_clk_GenericConfigDivider,
  /* .DetectDivider    = */ HAL_clk_GenericDetectDivider,
  /* .ConfigFootswitch = */ HAL_clk_GenericConfigFootswitch,
  /* .Invert           = */ NULL,
  /* .ResetBranch      = */ HAL_clk_GenericResetBranch,
};


/*
 * HAL_clk_GenericDomainControl
 *
 * Common functions for controlling clock domains.
 */
HAL_clk_ClockDomainControlType HAL_clk_GenericDomainControl =
{
  /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
  /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
  /* .IsOn               = */ HAL_clk_GenericDomainIsOn,
  /* .RootEnable         = */ HAL_clk_GenericUpdateRootEnable,
  /* .EnableDFS          = */ HAL_clk_GenericEnableDFS,
  /* .IsDFSEnabled       = */ HAL_clk_GenericDomainIsDFSEnabled,
  /* .ConfigDFSMux       = */ HAL_clk_GenericConfigDFSMux,
  /* .DetectDFSMuxConfig = */ HAL_clk_GenericDetectDFSMuxConfig,
};


/*
 * HAL_clk_GenericPowerDomainControl
 *
 * Common functions for controlling clock power domains.
 */
HAL_clk_PowerDomainControlType HAL_clk_GenericPowerDomainControl =
{
   /* .Enable     = */ HAL_clk_GenericPowerDomainEnable,
   /* .Disable    = */ HAL_clk_GenericPowerDomainDisable,
   /* .IsEnabled  = */ HAL_clk_GenericPowerDomainIsEnabled,
   /* .IsOn       = */ HAL_clk_GenericPowerDomainIsOn,
   /* .EnableHW   = */ HAL_clk_GenericPowerDomainEnableHW,
   /* .DisableHW  = */ HAL_clk_GenericPowerDomainDisableHW
};


/*
 * HAL_clk_GenericDividerControl
 *
 * Common functions for controlling clock dividers.
 */
HAL_clk_DividerControlType HAL_clk_GenericDividerControl =
{
  /* .ConfigDivider = */ HAL_clk_GenericDividerConfigDivider,
  /* .DetectDivider = */ HAL_clk_GenericDividerDetectDivider,
};


/*=============================================================================

                        SS specific clock domain controls

=============================================================================*/


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_GenericEnable
**
** ======================================================================== */

void HAL_clk_GenericEnable
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uintnt nAddr;
  uint32 nVal, nMask;

  /*
   * Check for HW votable registers
   */
  nAddr = pmClockDesc->mRegisters.mHWVotable.nAddr;
  if (nAddr)
  {
    nMask = pmClockDesc->mRegisters.mHWVotable.nMask;
  }
  else
  {
    nAddr = pmClockDesc->mRegisters.nBranchAddr;
    nMask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
  }

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return;
  }

  /*
   * Enable clock
   */
  nVal = inpdw(nAddr) | nMask;
  outpdw(nAddr, nVal);

} /* HAL_clk_GenericEnable */


/* ===========================================================================
**  HAL_clk_GenericDisable
**
** ======================================================================== */

void HAL_clk_GenericDisable
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uintnt nAddr;
  uint32 nVal, nMask;

  /*
   * Check for HW votable registers
   */
  nAddr = pmClockDesc->mRegisters.mHWVotable.nAddr;
  if (nAddr)
  {
    nMask = pmClockDesc->mRegisters.mHWVotable.nMask;
  }
  else
  {
    nAddr = pmClockDesc->mRegisters.nBranchAddr;
    nMask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
  }

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return;
  }

  /*
   * Disable clock
   */
  nVal  = inpdw(nAddr);
  nVal &= ~nMask;
  outpdw(nAddr, nVal);

} /* HAL_clk_GenericDisable */


/* ===========================================================================
**  HAL_clk_GenericIsEnabled
**
** ======================================================================== */

boolean HAL_clk_GenericIsEnabled
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uintnt nAddr;
  uint32 nMask;

  nAddr = pmClockDesc->mRegisters.mHWVotable.nAddr;

  if (nAddr != 0)
  {
    nMask = pmClockDesc->mRegisters.mHWVotable.nMask;
  }
  else
  {
    nAddr = pmClockDesc->mRegisters.nBranchAddr;
    nMask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
  }

  if (nAddr && (inpdw(nAddr) & nMask))
  {
    return TRUE;
  }

  return FALSE;

} /* HAL_clk_GenericIsEnabled */


/* ===========================================================================
**  HAL_clk_GenericIsOn
**
** ======================================================================== */

boolean HAL_clk_GenericIsOn
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uintnt nAddr;
  uint32 nVal, nClkOffVal;

  nAddr = pmClockDesc->mRegisters.nBranchAddr;

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return FALSE;
  }

  nVal = inpdw(nAddr);

  /*
   * Extract the CLK_OFF field.
   */
  nClkOffVal = nVal & HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_FMSK;
  nClkOffVal >>= HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_SHFT;

  /*
   * The clock is on if the CLK_OFF field equals any of the
   * pre-defined ON_VAL_[1|2] macros.  These macros represent
   * the different on states that the FSM can be in.
   */

  if (nClkOffVal == HAL_CLK_BRANCH_CTRL_REG_CLK_ON_VAL_1 ||
      nClkOffVal == HAL_CLK_BRANCH_CTRL_REG_CLK_ON_VAL_2)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* HAL_clk_GenericIsOn */


/* ===========================================================================
**  HAL_clk_GenericReset
**
** ======================================================================== */

void HAL_clk_GenericReset
(
  HAL_clk_ClockDescType *pmClockDesc,
  boolean                bAssert
)
{
  uintnt nAddr;
  uint32 nVal;

  nAddr = pmClockDesc->mRegisters.nResetAddr;

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return;
  }

  /*
   * Read current value
   */
  nVal = inpdw(nAddr);
  if (bAssert)
  {
    nVal |= HAL_CLK_BLOCK_CTRL_REG_BLK_ARES_FMSK;
  }
  else
  {
    nVal &= ~HAL_CLK_BLOCK_CTRL_REG_BLK_ARES_FMSK;
  }

  /*
   * Write updated value
   */
  outpdw(nAddr, nVal);

} /* HAL_clk_GenericReset */


/* ===========================================================================
**  HAL_clk_GenericIsReset
**
** ======================================================================== */

boolean HAL_clk_GenericIsReset
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uintnt nAddr;

  nAddr = pmClockDesc->mRegisters.nResetAddr;

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return FALSE;
  }

  /*
   * Read current value
   */
  if (inpdw(nAddr) & HAL_CLK_BLOCK_CTRL_REG_BLK_ARES_FMSK)
  {
    return TRUE;
  }

  return FALSE;

} /* HAL_clk_GenericIsReset */


/* ===========================================================================
**  HAL_clk_GenericResetBranch
**
** ======================================================================== */

void HAL_clk_GenericResetBranch
(
  HAL_clk_ClockDescType *pmClockDesc,
  boolean                bAssert
)
{
  uintnt nAddr;
  uint32 nVal;

  nAddr = pmClockDesc->mRegisters.nBranchAddr;

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return;
  }

  /*
   * Read current value
   */
  nVal = inpdw(nAddr);
  if (bAssert)
  {
    nVal |= HAL_CLK_BRANCH_CTRL_REG_CLK_ARES_FMSK;
  }
  else
  {
    nVal &= ~HAL_CLK_BRANCH_CTRL_REG_CLK_ARES_FMSK;
  }

  /*
   * Write updated value
   */
  outpdw(nAddr, nVal);

} /* HAL_clk_GenericReset */


/* ===========================================================================
**  HAL_clk_GenericConfigDivider
**
** ======================================================================== */

void HAL_clk_GenericConfigDivider
(
  HAL_clk_ClockDescType *pmClockDesc,
  uint32                nDiv
)
{
  uintnt nAddr;
  uint32 nVal;

  nAddr = pmClockDesc->mRegisters.nBranchAddr;

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return;
  }

  nVal = inpdw(nAddr);
  if(nDiv <= 1)
  {
    nDiv = 0;
  }
  else
  {
    nDiv--;
  }

  /*
   * Clear previous divider value.
   */
  nVal &= ~HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_FMSK;

  /*
   * Set divider value.
   */
  nVal |= ((nDiv << HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_SHFT)
            & HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_FMSK);

  /*
   * Write final value.
   */
  outpdw(nAddr, nVal);

} /* END HAL_clk_GenericConfigDivider */


/* ===========================================================================
**  HAL_clk_GenericDetectDivider
**
** ======================================================================== */

void HAL_clk_GenericDetectDivider
(
  HAL_clk_ClockDescType *pmClockDesc,
  uint32                *pnDiv
)
{
  uintnt nAddr;
  uint32 nVal;

  nAddr = pmClockDesc->mRegisters.nBranchAddr;
  *pnDiv = 1;

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return;
  }

  nVal   = inpdw(nAddr);
  *pnDiv = ((nVal &  HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_FMSK)
                  >> HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_SHFT) + 1;

} /* END HAL_clk_GenericDetectDivider */


/* ===========================================================================
**  HAL_clk_GenericConfigFootswitch
**
** ======================================================================== */

void HAL_clk_GenericConfigFootswitch
(
  HAL_clk_ClockDescType        *pmClockDesc,
  HAL_clk_FootswitchConfigType eConfig
)
{
  uintnt nAddr;
  uint32 nVal;

  nAddr = pmClockDesc->mRegisters.nBranchAddr;

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return;
  }

  nVal = inpdw(nAddr);
  if (eConfig == HAL_CLK_FOOTSWITCH_CONFIG_CORE_FORCE_ON)
  {
    nVal |= HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_CORE_ON_FMSK;
  }
  else if (eConfig == HAL_CLK_FOOTSWITCH_CONFIG_CORE_ALLOW_OFF)
  {
    nVal &= ~HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_CORE_ON_FMSK;
  }
  else if(eConfig == HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_FORCE_ON)
  {
    nVal |= HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_ON_FMSK;
  }
  else if(eConfig == HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_ALLOW_OFF)
  {
    nVal &= ~HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_ON_FMSK;
  }
  else if (eConfig == HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_FORCE_OFF)
  {
    nVal |= HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_OFF_FMSK;
  }
  else if (eConfig == HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_ALLOW_ON)
  {
    nVal &= ~HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_OFF_FMSK;
  }
  else
  {
    return;
  }

  outpdw(nAddr, nVal);

} /* END HAL_clk_GenericConfigFootswitch */


/* ===========================================================================
**  HAL_clk_GenericConfigMuxOffs
**
** ======================================================================== */

static void HAL_clk_GenericConfigMuxOffs
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig,
  uint32                            nCfgCGROffs,
  uint32                            nMOffs,
  uint32                            nNOffs,
  uint32                            nDOffs
)
{
  uintnt nCfgCGRAddr, nMAddr, nNAddr, nDAddr;
  uint32 nCfgCGRVal;

  /*
   * Sanity check
   */
  if((pmConfig                    == NULL) ||
     (pmClockDomainDesc           == NULL) ||
     (pmClockDomainDesc->nCGRAddr == 0   ))
  {
    return;
  }

  /*
   * Get current CMD and CFG register values
   */
  nCfgCGRAddr = pmClockDomainDesc->nCGRAddr + nCfgCGROffs;
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Clear the fields
   */
  nCfgCGRVal &= ~(HAL_CLK_CFG_CGR_SRC_SEL_FMSK |
                  HAL_CLK_CFG_CGR_SRC_DIV_FMSK |
                  HAL_CLK_CFG_CGR_MODE_FMSK);

  /*
   * Clear HW_CLK_CONTROL, since it forces both sources to be on
   * for the mux to switch, even if the RCG is off.
   */
  nCfgCGRVal &= ~HAL_CLK_CFG_CGR_HW_CLK_CONTROL_FMSK;

  /*
   * Program the source and divider values.
   */
  nCfgCGRVal |= ((pmConfig->nMuxSel << HAL_CLK_CFG_CGR_SRC_SEL_SHFT) & HAL_CLK_CFG_CGR_SRC_SEL_FMSK);
  nCfgCGRVal |= ((HALF_DIVIDER(pmConfig) << HAL_CLK_CFG_CGR_SRC_DIV_SHFT)
                  & HAL_CLK_CFG_CGR_SRC_DIV_FMSK);

  /*
   * Set MND counter mode depending on if it is in use
   */
  if (pmConfig->nM != 0 && (pmConfig->nM < pmConfig->nN))
  {
    /*
     * Get M, N and D addresses
     */
    nMAddr = pmClockDomainDesc->nCGRAddr + nMOffs;
    nNAddr = pmClockDomainDesc->nCGRAddr + nNOffs;
    nDAddr = pmClockDomainDesc->nCGRAddr + nDOffs;

    /*
     * Set M value
     */
    outpdw(nMAddr, pmConfig->nM);

    /*
     * Set N value
     */
    outpdw(nNAddr, ENCODE_NOT_N_MINUS_M(pmConfig));

    /*
     * Set D value
     */
    outpdw(nDAddr, ENCODE_NOT_2D(pmConfig));

    /*
     * Dual-edge mode
     */
    nCfgCGRVal |= ((HAL_CLK_CFG_CFG_DUAL_EDGE_MODE_VAL << HAL_CLK_CFG_CGR_MODE_SHFT)
                    & HAL_CLK_CFG_CGR_MODE_FMSK);
  }

  /*
   * Write the final CFG register value
   */
  outpdw(nCfgCGRAddr, nCfgCGRVal);

} /* HAL_clk_GenericConfigMuxOffs */


/* ===========================================================================
**  HAL_clk_GenericConfigMux
**
** ======================================================================== */

void HAL_clk_GenericConfigMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
)
{
  uintnt nCmdCGRAddr;
  uint32 nCmdCGRVal;

  HAL_clk_GenericConfigMuxOffs(pmClockDomainDesc, pmConfig,
    HAL_CLK_CFG_REG_OFFSET,
    HAL_CLK_M_REG_OFFSET,
    HAL_CLK_N_REG_OFFSET,
    HAL_CLK_D_REG_OFFSET);

  /*
   * Trigger the update
   */
  nCmdCGRAddr = pmClockDomainDesc->nCGRAddr;
  nCmdCGRVal  = inpdw(nCmdCGRAddr);
  nCmdCGRVal |= HAL_CLK_CMD_CFG_UPDATE_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  /*
   * Wait until update finishes
   */
  while (inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CFG_UPDATE_FMSK);

} /* HAL_clk_GenericConfigMux */


/* ===========================================================================
**  HAL_clk_GenericDetectMuxConfigOffs
**
** ======================================================================== */

static void HAL_clk_GenericDetectMuxConfigOffs
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig,
  uint32                       nCfgCGROffs,
  uint32                       nMOffs,
  uint32                       nNOffs
)
{
  uintnt nCfgCGRAddr, nMAddr, nNAddr;
  uint32 nCfgCGRVal;
  uint32 nMask;

  if (pmConfig == NULL)
  {
    return;
  }

  pmConfig->nMuxSel = 0;
  pmConfig->nDiv2x  = 2;
  pmConfig->nM      = 0;
  pmConfig->nN      = 0;

  /*
   * Sanity check
   */
  if((pmClockDomainDesc           == NULL) ||
     (pmClockDomainDesc->nCGRAddr == 0   ) )
  {
    return;
  }

  /*
   * Get current CMD and CFG register values
   */
  nCfgCGRAddr = pmClockDomainDesc->nCGRAddr + nCfgCGROffs;
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Get the source
   */
  pmConfig->nMuxSel = (nCfgCGRVal & HAL_CLK_CFG_CGR_SRC_SEL_FMSK) >> HAL_CLK_CFG_CGR_SRC_SEL_SHFT;

  /*
   * Get the divider
   *
   * If the divider value (SRC_DIV) is 0, it indicates BYPASS.
   * Thus we need set nDiv2x = 2 i.e. divider = 1 (No division)
   * For all other values, nDiv2x = SRC_DIV + 1
   */
  pmConfig->nDiv2x =
    ((nCfgCGRVal & HAL_CLK_CFG_CGR_SRC_DIV_FMSK) >> HAL_CLK_CFG_CGR_SRC_DIV_SHFT);

  if(pmConfig->nDiv2x == 0)
  {
    pmConfig->nDiv2x = 2;
  }
  else
  {
    pmConfig->nDiv2x += 1;
  }

  /*
   * Read MND counter mode depending on if it is in use
   */
  if (pmClockDomainDesc->nMNDWidth &&
      (nCfgCGRVal & HAL_CLK_CFG_CGR_MODE_FMSK) != 0)
  {
    /*
     * Get M and N addresses
     */
    nMAddr = pmClockDomainDesc->nCGRAddr + nMOffs;
    nNAddr = pmClockDomainDesc->nCGRAddr + nNOffs;
    nMask = (1 << pmClockDomainDesc->nMNDWidth) - 1;

    /*
     * Return the M and N values.
     */
    pmConfig->nM = (uint16)(inpdw(nMAddr) & nMask);
    pmConfig->nN = (uint16)(((~inpdw(nNAddr)) & nMask) + pmConfig->nM);
  }

} /* END HAL_clk_GenericDetectMuxConfigOffs */


/* ===========================================================================
**  HAL_clk_GenericDetectMuxConfig
**
** ======================================================================== */

void HAL_clk_GenericDetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  HAL_clk_GenericDetectMuxConfigOffs(pmClockDomainDesc, pmConfig,
    HAL_CLK_CFG_REG_OFFSET,
    HAL_CLK_M_REG_OFFSET,
    HAL_CLK_N_REG_OFFSET);

  pmConfig->nDFSIdx = 0xFF;

} /* END HAL_clk_GenericDetectMuxConfig */


/* ===========================================================================
**  HAL_clk_GenericDomainIsOn
**
** ======================================================================== */

boolean HAL_clk_GenericDomainIsOn
(
  HAL_clk_ClockDomainDescType *pmClkDomainDesc
)
{
  uintnt nCmdCGRAddr;

  nCmdCGRAddr = pmClkDomainDesc->nCGRAddr;

  /*
   * Nothing to do if there is no RCG hooked up.
   */
  if (nCmdCGRAddr == 0)
  {
    return FALSE;
  }

  if (inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CGR_ROOT_OFF_FMSK)
  {
    return FALSE;
  }

  return TRUE;

} /* END HAL_clk_GenericDomainIsOn */


/* ===========================================================================
**  HAL_clk_GenericUpdateRootEnable
**
** ======================================================================== */

void HAL_clk_GenericUpdateRootEnable
(
  HAL_clk_ClockDomainDescType  *pmClockDomainDesc,
  boolean                       bAssert
)
{
  uintnt  nCmdCGRAddr;
  uint32  nCmdCGRVal;

  /*
   * Sanity check
   */
  if ((pmClockDomainDesc           == NULL) ||
      (pmClockDomainDesc->nCGRAddr == 0   ) )
  {
    return;
  }

  /*
   * Get current CMD register value
   */
  nCmdCGRAddr = pmClockDomainDesc->nCGRAddr;
  nCmdCGRVal  = inpdw(nCmdCGRAddr);

  if (bAssert)
  {
    nCmdCGRVal |= HAL_CLK_CMD_CGR_ROOT_EN_FMSK;
  }
  else
  {
    nCmdCGRVal &= ~HAL_CLK_CMD_CGR_ROOT_EN_FMSK;
  }

  /*
   * Write the final CMD register value
   */
  outpdw(nCmdCGRAddr, nCmdCGRVal);

} /* HAL_clk_GenericUpdateRootEnable */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainEnable
**
** ======================================================================== */

void HAL_clk_GenericPowerDomainEnable
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
  if (nAddr == 0)
  {
    return;
  }

  nVal  = inpdw(nAddr);

  /*
   * Clear the SW PD collapse bit
   */
  nVal &= ~HAL_CLK_GDSCR_SW_COLLAPSE_FMSK;
  outpdw(nAddr, nVal);

  /*
   * Wait for PD ON
   */
  while (!(inpdw(nAddr) & HAL_CLK_GDSCR_PWR_ON_FMSK));

} /* END HAL_clk_GenericPowerDomainEnable */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainDisable
**
** ======================================================================== */

void HAL_clk_GenericPowerDomainDisable
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
  if (nAddr == 0)
  {
    return;
  }

  nVal  = inpdw(nAddr);

  /*
   * Set the SW PD collapse bit
   */
  nVal |= HAL_CLK_GDSCR_SW_COLLAPSE_FMSK;
  outpdw(nAddr, nVal);

} /* END HAL_clk_GenericPowerDomainDisable */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainIsEnabled
**
** ======================================================================== */

boolean HAL_clk_GenericPowerDomainIsEnabled
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
  if (nAddr == 0)
  {
    return FALSE;
  }

  nVal = inpdw(nAddr);

  if (nVal & HAL_CLK_GDSCR_SW_COLLAPSE_FMSK)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* END HAL_clk_GenericPowerDomainIsEnabled */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainIsOn
**
** ======================================================================== */

boolean HAL_clk_GenericPowerDomainIsOn
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
  if (nAddr == 0)
  {
    return FALSE;
  }

  nVal = inpdw(nAddr);

  if (nVal & HAL_CLK_GDSCR_PWR_ON_FMSK)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* END HAL_clk_GenericPowerDomainIsOn */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainEnableHW
**
** ======================================================================== */

void HAL_clk_GenericPowerDomainEnableHW
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
  if (nAddr == 0)
  {
    return;
  }

  nVal = inpdw(nAddr);

  /*
   * Set the HW Control bit
   */
  nVal |= HAL_CLK_GDSCR_HW_CONTROL_FMSK;
  outpdw(nAddr, nVal);

} /* END HAL_clk_GenericPowerDomainEnableHW */


/* ===========================================================================
**  HAL_clk_GenericPowerDomainDisableHW
**
** ======================================================================== */

void HAL_clk_GenericPowerDomainDisableHW
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
  if (nAddr == 0)
  {
    return;
  }

  nVal = inpdw(nAddr);

  /*
   * Clear the HW Control bit
   */
  nVal &= ~HAL_CLK_GDSCR_HW_CONTROL_FMSK;
  outpdw(nAddr, nVal);

} /* END HAL_clk_GenericPowerDomainDisableHW */


/* ===========================================================================
**  HAL_clk_GenericDividerConfigDivider
**
** ======================================================================== */

void HAL_clk_GenericDividerConfigDivider
(
  HAL_clk_DividerDescType *pmDividerDesc,
  uint32                   nDiv
)
{
  uintnt nAddr;
  uint32 nVal;

  nAddr = pmDividerDesc->nAddr;

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return;
  }

  nVal = inpdw(nAddr);
  if (nDiv <= 1)
  {
    nDiv = 0;
  }
  else
  {
    nDiv--;
  }

  /*
   * Clear previous divider value.
   */
  nVal &= ~HAL_CLK_CDIVR_CLK_DIV_FMSK;

  /*
   * Set divider value.
   */
  nVal |= ((nDiv << HAL_CLK_CDIVR_CLK_DIV_SHFT)
                  & HAL_CLK_CDIVR_CLK_DIV_FMSK);

  /*
   * Write final value.
   */
  outpdw(nAddr, nVal);

} /* END HAL_clk_GenericDividerConfigDivider */


/* ===========================================================================
**  HAL_clk_GenericDividerDetectDivider
**
** ======================================================================== */

void HAL_clk_GenericDividerDetectDivider
(
  HAL_clk_DividerDescType *pmDividerDesc,
  uint32                  *pnDiv
)
{
  uintnt nAddr;
  uint32 nVal;

  nAddr = pmDividerDesc->nAddr;
  *pnDiv = 1;

  /*
   * Sanity check
   */
  if (nAddr == 0)
  {
    return;
  }

  nVal = inpdw(nAddr);
  *pnDiv = ((nVal &  HAL_CLK_CDIVR_CLK_DIV_FMSK)
                  >> HAL_CLK_CDIVR_CLK_DIV_SHFT) + 1;

} /* END HAL_clk_GenericDividerDetectDivider */


/* ===========================================================================
**  HAL_clk_GenericIsPLLConfigEqual
**
** ======================================================================== */

boolean HAL_clk_GenericIsPLLConfigEqual
(
  const HAL_clk_PLLConfigType  *pConfigA,
  const HAL_clk_PLLConfigType  *pConfigB
)
{
  if (pConfigA->nSource   == pConfigB->nSource    &&
      pConfigA->nMode     == pConfigB->nMode      &&
      pConfigA->nPreDiv   == pConfigB->nPreDiv    &&
      pConfigA->nL        == pConfigB->nL         &&
      pConfigA->nFracVal  == pConfigB->nFracVal   &&
      pConfigA->eFracMode == pConfigB->eFracMode)
  {
    return TRUE;
  }

  return FALSE;

} /* END HAL_clk_GenericIsPLLConfigEqual */


/* ===========================================================================
**  HAL_clk_GenericEnableDFS
**
** ======================================================================== */

void HAL_clk_GenericEnableDFS
(
  HAL_clk_ClockDomainDescType *pmClkDomainDesc
)
{
  uintnt nCmdCGRAddr, nDFSRAddr;
  uint32 nDFSRVal;

  nCmdCGRAddr = pmClkDomainDesc->nCGRAddr;
  nDFSRAddr = nCmdCGRAddr + HAL_CLK_CMD_DFSR_REG_OFFSET;

  if (!nCmdCGRAddr || pmClkDomainDesc->nDFSStates == 0)
  {
    return;
  }

  nDFSRVal = 0;
  nDFSRVal |= HAL_CLK_CMD_DFSR_HW_CLK_CONTROL_FMSK;
  nDFSRVal |= HAL_CLK_CMD_DFSR_DFS_EN_FMSK;
  outpdw(nDFSRAddr, nDFSRVal);

} /* END HAL_clk_GenericEnableDFS */


/* ===========================================================================
**  HAL_clk_GenericDomainIsDFSEnabled
**
** ======================================================================== */

boolean HAL_clk_GenericDomainIsDFSEnabled
(
  HAL_clk_ClockDomainDescType *pmClkDomainDesc
)
{
  uintnt nCmdCGRAddr, nDFSRAddr;

  nCmdCGRAddr = pmClkDomainDesc->nCGRAddr;
  nDFSRAddr = nCmdCGRAddr + HAL_CLK_CMD_DFSR_REG_OFFSET;

  /*
   * Nothing to do if there is no RCG hooked up.
   */
  if (nCmdCGRAddr == 0 || pmClkDomainDesc->nDFSStates == 0)
  {
    return FALSE;
  }

  if (inpdw(nDFSRAddr) & HAL_CLK_CMD_DFSR_DFS_EN_FMSK)
  {
    return TRUE;
  }

  return FALSE;

} /* END HAL_clk_GenericDomainIsDFSEnabled */


/* ===========================================================================
**  HAL_clk_GenericConfigDFSMux
**
** ======================================================================== */

void HAL_clk_GenericConfigDFSMux
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  uint32 nPerfOffset;

  /*
   * Sanity check
   */
  if(pmClockDomainDesc             == NULL ||
     pmClockDomainDesc->nDFSStates <= pmConfig->nDFSIdx)
  {
    return;
  }

  nPerfOffset = 0x4 * pmConfig->nDFSIdx;

  HAL_clk_GenericConfigMuxOffs(pmClockDomainDesc, pmConfig,
    HAL_CLK_PERF_DFSR_REG_OFFSET + nPerfOffset,
    HAL_CLK_PERF_M_DFSR_REG_OFFSET + nPerfOffset,
    HAL_CLK_PERF_N_DFSR_REG_OFFSET + nPerfOffset,
    HAL_CLK_PERF_D_DFSR_REG_OFFSET + nPerfOffset);

} /* END HAL_clk_GenericConfigDFSMux */


/* ===========================================================================
**  HAL_clk_GenericDetectDFSMuxConfig
**
** ======================================================================== */

void HAL_clk_GenericDetectDFSMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig,
  uint32                       nPerfLevel
)
{
  uint32 nPerfOffset;

  /*
   * Sanity check
   */
  if(pmClockDomainDesc             == NULL ||
     pmClockDomainDesc->nDFSStates <= nPerfLevel)
  {
    return;
  }

  nPerfOffset = 0x4 * nPerfLevel;

  HAL_clk_GenericDetectMuxConfigOffs(pmClockDomainDesc, pmConfig,
    HAL_CLK_PERF_DFSR_REG_OFFSET + nPerfOffset,
    HAL_CLK_PERF_M_DFSR_REG_OFFSET + nPerfOffset,
    HAL_CLK_PERF_N_DFSR_REG_OFFSET + nPerfOffset);

  pmConfig->nDFSIdx = (uint8)nPerfLevel;

} /* END HAL_clk_GenericDetectDFSMuxConfig */

