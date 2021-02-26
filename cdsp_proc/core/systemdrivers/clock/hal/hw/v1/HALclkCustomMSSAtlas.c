/*
==============================================================================

FILE:         HALclkCustomMSSAtlas.c

DESCRIPTION:
  This file contains custom code for the MSS clock controller.

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/hal/hw/v1/HALclkCustomMSSAtlas.c#1 $

==============================================================================
            Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkGeneric.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*
 * MSS_UIMx_MND_CFG_RCGR Root clock config fields/masks (*_CFG_RCGR)
 */
#define HAL_CLK_MSS_UIM_CFG_CGR_SRC_SEL_SHFT    0x00000000
#define HAL_CLK_MSS_UIM_CFG_CGR_SRC_SEL_FMSK    0x00000001


/* ============================================================================
**    Prototypes
** ==========================================================================*/

static void HAL_clk_MSSUIMConfigMux (HAL_clk_ClockDomainDescType *pmClockDomainDesc,
                                     const HAL_clk_ClockMuxConfigType *pmConfig);

static void HAL_clk_MSSUIMDetectMuxConfig (HAL_clk_ClockDomainDescType *pmClockDomainDesc,
                                           HAL_clk_ClockMuxConfigType  *pmConfig);

static void HAL_clk_MSSQ6ConfigMux (HAL_clk_ClockDomainDescType *pmClockDomainDesc,
                                    const HAL_clk_ClockMuxConfigType *pmConfig);


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk_mMSSUIMClockDomainControl
 *
 */
HAL_clk_ClockDomainControlType HAL_clk_mMSSUIMClockDomainControl =
{
  /* .ConfigMux          = */ HAL_clk_MSSUIMConfigMux,
  /* .DetectMuxConfig    = */ HAL_clk_MSSUIMDetectMuxConfig,
  /* .IsOn               = */ HAL_clk_GenericDomainIsOn,
};

/*
 * HAL_clk_mMSSQ6ClockDomainControl
 *
 */
HAL_clk_ClockDomainControlType HAL_clk_mMSSQ6ClockDomainControl =
{
  /* .ConfigMux          = */ HAL_clk_MSSQ6ConfigMux,
  /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
  /* .IsOn               = */ HAL_clk_GenericDomainIsOn,
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/

/* ===========================================================================
**  HAL_clk_MSSUIMConfigMux
**
** ======================================================================== */

static void HAL_clk_MSSUIMConfigMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
)
{
  uintnt nCmdCGRAddr, nCfgCGRAddr, nMAddr, nNAddr, nDAddr;
  uint32 nCmdCGRVal, nCfgCGRVal;

  /*
   * Sanity check
   */
  if((pmConfig                     == NULL) ||
     (pmClockDomainDesc            == NULL) ||
     (pmClockDomainDesc->nCGRAddr  == 0   ) ||
     (pmClockDomainDesc->pmControl == NULL))
  {
    return;
  }

  /*
   * Get current CMD and CFG register values
   */
  nCmdCGRAddr = pmClockDomainDesc->nCGRAddr;
  nCmdCGRVal  = inpdw(nCmdCGRAddr);
  nCfgCGRAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_CFG_REG_OFFSET;
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Clear the fields
   */
  nCfgCGRVal &= ~(HAL_CLK_MSS_UIM_CFG_CGR_SRC_SEL_FMSK | HAL_CLK_CFG_CGR_MODE_FMSK);

  /*
   * Program the source. There is no divider in MSS_UIMx_MND_CFG_RCGR
   */
  nCfgCGRVal |= ((pmConfig->nMuxSel << HAL_CLK_MSS_UIM_CFG_CGR_SRC_SEL_SHFT) & HAL_CLK_MSS_UIM_CFG_CGR_SRC_SEL_FMSK);

  /*
   * Set MND counter mode depending on if it is in use
   */
  if (pmConfig->nM != 0 && (pmConfig->nM < pmConfig->nN))
  {
    /*
     * Get M, N and D addresses
     */
    nMAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_M_REG_OFFSET;
    nNAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_N_REG_OFFSET;
    nDAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_D_REG_OFFSET;

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

  /*
   * Trigger the update
   */
  nCmdCGRVal |= HAL_CLK_CMD_CFG_UPDATE_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  /*
   * Wait until update finishes
   */
  while(inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CFG_UPDATE_FMSK);

} /* HAL_clk_MSSUIMConfigMux */


/* ===========================================================================
**  HAL_clk_MSSUIMDetectMuxConfig
**
** ======================================================================== */

static void HAL_clk_MSSUIMDetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  uintnt nCfgCGRAddr, nMAddr, nNAddr;
  uint32 nCfgCGRVal;

  if (pmConfig == NULL)
  {
    return;
  }

  pmConfig->nMuxSel = 0;
  pmConfig->nDiv2x  = 0;
  pmConfig->nM      = 0;
  pmConfig->nN      = 0;

  /*
   * Sanity check
   */
  if((pmClockDomainDesc            == NULL) ||
     (pmClockDomainDesc->nCGRAddr  == 0   ) ||
     (pmClockDomainDesc->pmControl == NULL))
  {
    return;
  }

  /*
   * Get current CMD and CFG register values
   */
  nCfgCGRAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_CFG_REG_OFFSET;
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Get the source
   */
  pmConfig->nMuxSel = (nCfgCGRVal & HAL_CLK_MSS_UIM_CFG_CGR_SRC_SEL_FMSK) >> HAL_CLK_MSS_UIM_CFG_CGR_SRC_SEL_SHFT;

  /*
   * These is no divider.
   */
  pmConfig->nDiv2x = 2;

  /*
   * Read MND counter mode depending on if it is in use
   */
  if ((nCfgCGRVal & HAL_CLK_CFG_CGR_MODE_FMSK) != 0)
  {
    /*
     * Get M and N addresses
     */
    nMAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_M_REG_OFFSET;
    nNAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_N_REG_OFFSET;

    /*
     * Return the M and N values.
     */
    pmConfig->nM = inpdw(nMAddr);
    pmConfig->nN = DECODE_NOT_N_MINUS_M(inpdw(nNAddr), inpdw(nMAddr));
  }

} /* END HAL_clk_MSSUIMDetectMuxConfig */


/* ===========================================================================
**  HAL_clk_Q6ConfigMux
**
** ======================================================================== */

static void HAL_clk_MSSQ6ConfigMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
)
{
  uintnt nAddr;

  /*
   * The Q6 clock can be temporarily disabled during the AXIM2 bus isolation
   * sequence, which will feed back and disable the RCG. If this happens
   * at the same time the UPDATE bit is set, then the RCG will lock up and
   * the UPDATE bit will never clear. To prevent this, manually force the
   * RCG root on during configuration.
   */
  nAddr = pmClockDomainDesc->nCGRAddr;
  outpdw(nAddr, inpdw(nAddr) | HAL_CLK_CMD_CGR_ROOT_EN_FMSK);
  HAL_clk_GenericConfigMux(pmClockDomainDesc, pmConfig);
  outpdw(nAddr, inpdw(nAddr) & ~HAL_CLK_CMD_CGR_ROOT_EN_FMSK);

} /* END HAL_clk_MSSQ6ConfigMux */

