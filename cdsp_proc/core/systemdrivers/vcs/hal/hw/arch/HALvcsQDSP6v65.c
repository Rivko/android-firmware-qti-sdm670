
/*
==============================================================================

FILE:         HALvcsQDSP6v65.c

DESCRIPTION:
  This file contains the VCS HAL code for the QDSP6v65 control.

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/hal/hw/arch/HALvcsQDSP6v65.c#1 $

==============================================================================
            Copyright (c) 2017 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALvcsGenericHWIO.h"
#include "HALvcsInternal.h"
#include "HALvcsQDSP6v65.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/

/*
 * Register offsets.
 */
#define HWIO_QDSP6SS_VERSION_OFFS                      0x0
#define HWIO_QDSP6SS_LDO_USER_OFFS                     0x50
#define HWIO_QDSP6SS_LDO_CFG0_OFFS                     0x54
#define HWIO_QDSP6SS_LDO_CFG1_OFFS                     0x58
#define HWIO_QDSP6SS_LDO_CFG2_OFFS                     0x5c
#define HWIO_QDSP6SS_LDO_VREF_SET_OFFS                 0x60
#define HWIO_QDSP6SS_LDO_VREF_CMD_OFFS                 0x64
#define HWIO_QDSP6SS_LDO_LD_CTL_OFFS                   0x68
#define HWIO_QDSP6SS_LDO_TRIM_CTL_OFFS                 0x6c

#define HWIO_QDSP6SS_QMC_SVS_CTL_OFFS                  0x130

#define HWIO_QDSP6SS_CORE_BHS_CFG_OFFS                 0x410
#define HWIO_QDSP6SS_CORE_BHS_CMD_OFFS                 0x414
#define HWIO_QDSP6SS_CORE_BHS_STATUS_OFFS              0x418
#define HWIO_QDSP6SS_LDO_CTL_CFG_OFFS                  0x420
#define HWIO_QDSP6SS_LDO_CTL_CMD_OFFS                  0x424
#define HWIO_QDSP6SS_LDO_CTL_STATUS_OFFS               0x428

#define HWIO_QDSP6SS_CP_PWR_CTL_OFFS                   0x500
#define HWIO_QDSP6SS_CP_RESET_OFFS                     0x504
#define HWIO_QDSP6SS_CP_CLK_CTL_OFFS                   0x508
#define HWIO_QDSP6SS_CP_BHS_CFG_OFFS                   0x510
#define HWIO_QDSP6SS_CP_BHS_CMD_OFFS                   0x514
#define HWIO_QDSP6SS_CP_BHS_STATUS_OFFS                0x518
#define HWIO_QDSP6SS_CPMEM_CFG_OFFS                    0x520
#define HWIO_QDSP6SS_CPMEM_CMD_OFFS                    0x524
#define HWIO_QDSP6SS_CPMEM_STATUS_OFFS                 0x528


/*
 * QDSP6SS_LDO_VREF_SET register definitions.
 */
#define HWIO_QDSP6SS_LDO_VREF_SET_LDO_VREF_SEL_RST_BMSK            0x00010000
#define HWIO_QDSP6SS_LDO_VREF_SET_LDO_VREF_SEL_RST_SHFT                  0x10
#define HWIO_QDSP6SS_LDO_VREF_SET_VREF_LDO_BMSK                    0x0000007f
#define HWIO_QDSP6SS_LDO_VREF_SET_VREF_LDO_SHFT                           0x0

/*
 * QDSP6SS_LDO_VREF_CMD register definitions.
 */
#define HWIO_QDSP6SS_LDO_VREF_CMD_LDO_VREF_SEL_UPDATE_BMSK         0x00000001
#define HWIO_QDSP6SS_LDO_VREF_CMD_LDO_VREF_SEL_UPDATE_SHFT                0x0

/*
 * QDSP6SS_QMC_SVS_CTL register definitions.
 */
#define HWIO_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_BMSK              0x00000002
#define HWIO_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_SHFT                     0x1
#define HWIO_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_BMSK                  0x00000001
#define HWIO_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SHFT                         0x0

/*
 * QDSP6SS_CORE_BHS_CFG register definitions.
 */
#define HWIO_QDSP6SS_CORE_BHS_CFG_BHS_ON_BMSK                      0x00000001
#define HWIO_QDSP6SS_CORE_BHS_CFG_BHS_ON_SHFT                             0x0

/*
 * QDSP6SS_CORE_BHS_CMD register definitions.
 */
#define HWIO_QDSP6SS_CORE_BHS_CMD_UPDATE_BMSK                      0x00000001
#define HWIO_QDSP6SS_CORE_BHS_CMD_UPDATE_SHFT                             0x0

/*
 * QDSP6SS_CORE_BHS_STATUS register definitions.
 */
#define HWIO_QDSP6SS_CORE_BHS_STATUS_BHS_ON_BMSK                   0x00000001
#define HWIO_QDSP6SS_CORE_BHS_STATUS_BHS_ON_SHFT                          0x0

/*
 * QDSP6SS_LDO_CTL_CFG register definitions.
 */
#define HWIO_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_BG_BMSK                0x00000004
#define HWIO_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_BG_SHFT                       0x2
#define HWIO_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_BMSK                   0x00000002
#define HWIO_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_SHFT                          0x1
#define HWIO_QDSP6SS_LDO_CTL_CFG_LDO_BYP_BMSK                      0x00000001
#define HWIO_QDSP6SS_LDO_CTL_CFG_LDO_BYP_SHFT                             0x0

/*
 * QDSP6SS_LDO_CTL_CMD register definitions.
 */
#define HWIO_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_BG_UPDATE_BMSK         0x00000004
#define HWIO_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_BG_UPDATE_SHFT                0x2
#define HWIO_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_UPDATE_BMSK            0x00000002
#define HWIO_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_UPDATE_SHFT                   0x1
#define HWIO_QDSP6SS_LDO_CTL_CMD_LDO_BYP_UPDATE_BMSK               0x00000001
#define HWIO_QDSP6SS_LDO_CTL_CMD_LDO_BYP_UPDATE_SHFT                      0x0

/*
 * QDSP6SSLDO_CTL_STATUS register definitions.
 */
#define HWIO_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_BG_BMSK             0x00000004
#define HWIO_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_BG_SHFT                    0x2
#define HWIO_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_BMSK                0x00000002
#define HWIO_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_SHFT                       0x1
#define HWIO_QDSP6SS_LDO_CTL_STATUS_LDO_BYP_BMSK                   0x00000001
#define HWIO_QDSP6SS_LDO_CTL_STATUS_LDO_BYP_SHFT                          0x0

/*
 * QDSP6SSCP_PWR_CTL register definitions.
 */
#define HWIO_QDSP6SS_CP_PWR_CTL_CLAMP_QMC_MEM_BMSK                 0x00000010
#define HWIO_QDSP6SS_CP_PWR_CTL_CLAMP_QMC_MEM_SHFT                        0x4
#define HWIO_QDSP6SS_CP_PWR_CTL_LDO_2PASS_ON_BMSK                  0x00000008
#define HWIO_QDSP6SS_CP_PWR_CTL_LDO_2PASS_ON_SHFT                         0x3
#define HWIO_QDSP6SS_CP_PWR_CTL_SHUNT_BHS_ON_BMSK                  0x00000004
#define HWIO_QDSP6SS_CP_PWR_CTL_SHUNT_BHS_ON_SHFT                         0x2
#define HWIO_QDSP6SS_CP_PWR_CTL_CLAMP_IO_BMSK                      0x00000001
#define HWIO_QDSP6SS_CP_PWR_CTL_CLAMP_IO_SHFT                             0x0

/*
 * QDSP6SS_CP_RESET register definitions.
 */
#define HWIO_QDSP6SS_CP_RESET_ARES_BMSK                            0x00000001
#define HWIO_QDSP6SS_CP_RESET_ARES_SHFT                                   0x0

/*
 * QDSP6SS_CP_CLK_CTL register definitions.
 */
#define HWIO_QDSP6SS_CP_CLK_CTL_CLK_ENA_BMSK                       0x00000001
#define HWIO_QDSP6SS_CP_CLK_CTL_CLK_ENA_SHFT                              0x0

/*
 * QDSP6SS_CP_BHS_CFG register definitions.
 */
#define HWIO_QDSP6SS_CP_BHS_CFG_BHS_ON_BMSK                        0x00000001
#define HWIO_QDSP6SS_CP_BHS_CFG_BHS_ON_SHFT                               0x0

/*
 * QDSP6SS_CP_BHS_CMD register definitions.
 */
#define HWIO_QDSP6SS_CP_BHS_CMD_UPDATE_BMSK                        0x00000001
#define HWIO_QDSP6SS_CP_BHS_CMD_UPDATE_SHFT                               0x0

/*
 * QDSP6SS_CP_BHS_STATUS register definitions.
 */
#define HWIO_QDSP6SS_CP_BHS_STATUS_BHS_ON_BMSK                     0x00000001
#define HWIO_QDSP6SS_CP_BHS_STATUS_BHS_ON_SHFT                            0x0

/*
 * QDSP6SS_CPMEM_CFG register definitions.
 */
#define HWIO_QDSP6SS_CPMEM_CFG_VTCM_SLP_RET_N_BMSK                 0x00000002
#define HWIO_QDSP6SS_CPMEM_CFG_VTCM_SLP_RET_N_SHFT                        0x1
#define HWIO_QDSP6SS_CPMEM_CFG_VTCM_SLP_NRET_N_BMSK                0x00000001
#define HWIO_QDSP6SS_CPMEM_CFG_VTCM_SLP_NRET_N_SHFT                       0x0

/*
 * QDSP6SS_CPMEM_CMD register definitions.
 */
#define HWIO_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_RET_N_BMSK          0x00000002
#define HWIO_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_RET_N_SHFT                 0x1
#define HWIO_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_NRET_N_BMSK         0x00000001
#define HWIO_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_NRET_N_SHFT                0x0

/*
 * QDSP6SS_CPMEM_STATUS register definitions.
 */
#define HWIO_QDSP6SS_CPMEM_STATUS_VTCM_SLP_RET_N_BMSK              0x00000002
#define HWIO_QDSP6SS_CPMEM_STATUS_VTCM_SLP_RET_N_SHFT                     0x1
#define HWIO_QDSP6SS_CPMEM_STATUS_VTCM_SLP_NRET_N_BMSK             0x00000001
#define HWIO_QDSP6SS_CPMEM_STATUS_VTCM_SLP_NRET_N_SHFT                    0x0

/*
 * Min voltage for the LDO in uV.
 */
#define HAL_LDO_VREF_MIN_UV  331167.7

/* ============================================================================
**    Prototypes
** ==========================================================================*/

static void    HAL_vcs_QDSP6v65ConfigLDO     ( HAL_vcs_LDODescType *pLDO, HAL_vcs_LDOConfigType *pConfig );
static void    HAL_vcs_QDSP6v65EnableLDO     ( HAL_vcs_LDODescType *pLDO );
static void    HAL_vcs_QDSP6v65DisableLDO    ( HAL_vcs_LDODescType *pLDO );
static boolean HAL_vcs_QDSP6v65IsLDOEnabled  ( HAL_vcs_LDODescType *pLDO );
static void    HAL_vcs_QDSP6v65SetLDOVoltage ( HAL_vcs_LDODescType *pLDO, uint32 nVoltageUV );
static void    HAL_vcs_QDSP6v65GetLDOVoltage ( HAL_vcs_LDODescType *pLDO, uint32 *pVoltageUV, uint32 *pVoltageHW );

static void    HAL_vcs_QDSP6v65EnableCoproc    ( HAL_vcs_CoprocDescType *pCoproc );
static void    HAL_vcs_QDSP6v65DisableCoproc   ( HAL_vcs_CoprocDescType *pCoproc );
static boolean HAL_vcs_QDSP6v65IsCoprocEnabled ( HAL_vcs_CoprocDescType *pCoproc );


/* ============================================================================
**    Data Declarations
** ==========================================================================*/

/*
 * HAL_vcs_QDSP6v65LDOControl
 *
 * Control function array for the QDSP6v65 CPU embedded LDO.
 */
HAL_vcs_LDOControlType HAL_vcs_QDSP6v65LDOControl =
{
  .Config     = HAL_vcs_QDSP6v65ConfigLDO,
  .Enable     = HAL_vcs_QDSP6v65EnableLDO,
  .Disable    = HAL_vcs_QDSP6v65DisableLDO,
  .IsEnabled  = HAL_vcs_QDSP6v65IsLDOEnabled,
  .SetVoltage = HAL_vcs_QDSP6v65SetLDOVoltage,
  .GetVoltage = HAL_vcs_QDSP6v65GetLDOVoltage,
};

/*
 * HAL_vcs_QDSP6v65CoprocControl
 * 
 * Control function array for the QDSP6v65 CPU coproc.
 */
HAL_vcs_CoprocControlType HAL_vcs_QDSP6v65CoprocControl =
{
  .Enable    = HAL_vcs_QDSP6v65EnableCoproc,
  .Disable   = HAL_vcs_QDSP6v65DisableCoproc,
  .IsEnabled = HAL_vcs_QDSP6v65IsCoprocEnabled,
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_vcs_QDSP6v65IsLDOEnabled
**
** ======================================================================== */

static boolean _HAL_vcs_QDSP6v65IsLDOEnabled
(
  uint32 nAddr
)
{
  return HAL_VCS_HWIO_INF(QDSP6SS_LDO_CTL_STATUS, nAddr, LDO_PWR_UP);

} /* END of _HAL_vcs_QDSP6v65IsLDOEnabled */


/* ===========================================================================
**  HAL_vcs_QDSP6v65IsLDOEnabled
**
** ======================================================================== */

static boolean HAL_vcs_QDSP6v65IsLDOEnabled
(
  HAL_vcs_LDODescType *pLDO
)
{
  return _HAL_vcs_QDSP6v65IsLDOEnabled(pLDO->nAddr);

} /* END of HAL_vcs_QDSP6v65IsLDOEnabled */


/* ===========================================================================
**  _HAL_vcs_QDSP6v65IsCoprocEnabled
**
** ======================================================================== */

static boolean _HAL_vcs_QDSP6v65IsCoprocEnabled
(
  uint32 nAddr
)
{
  return HAL_VCS_HWIO_INF(QDSP6SS_CP_BHS_STATUS, nAddr, BHS_ON);

} /* END of _HAL_vcs_QDSP6v65IsCoprocEnabled */


/* ===========================================================================
**  HAL_vcs_QDSP6v65IsCoprocEnabled
**
** ======================================================================== */

static boolean HAL_vcs_QDSP6v65IsCoprocEnabled
(
  HAL_vcs_CoprocDescType *pCoproc
)
{
  return _HAL_vcs_QDSP6v65IsCoprocEnabled(pCoproc->nAddr);

} /* END of HAL_vcs_QDSP6v65IsCoprocEnabled */


/* ===========================================================================
**  HAL_vcs_QDSP6v65LDOConfig
**
** ======================================================================== */

static void HAL_vcs_QDSP6v65ConfigLDO
(
  HAL_vcs_LDODescType   *pLDO,
  HAL_vcs_LDOConfigType *pConfig
)
{
  uint32 i;
  HAL_vcs_RegisterSettingType *pSetting;
  
  for (i = 0; i < pConfig->nNumSettings; i++)
  {
    pSetting = &pConfig->pSettings[i];

    if (pSetting->nAddr)
    {
      out_dword(pSetting->nAddr, pSetting->nValue);
    }
  }

} /* END of HAL_vcs_QDSP6v65LDOConfig */


/* ===========================================================================
**  _HAL_vcs_QDSP6v65EnableLDO
**
** ======================================================================== */

static void _HAL_vcs_QDSP6v65EnableLDO
(
  uint32 nAddr
)
{
  boolean bIsCoprocEnabled;
  uint32  nVal;

  // Short-circuit if invalid address provided.
  if (!nAddr)
  {
    return;
  }

  // Short-circuit if already enabled.
  if (_HAL_vcs_QDSP6v65IsLDOEnabled(nAddr))
  {
    return;
  }

  // Determine if coproc is enabled.
  bIsCoprocEnabled = _HAL_vcs_QDSP6v65IsCoprocEnabled(nAddr);

  // Switch compiler memory to low power mode.
  nVal = HAL_VCS_HWIO_IN(QDSP6SS_QMC_SVS_CTL, nAddr);
  nVal = HAL_VCS_HWIO_OUTFV(QDSP6SS_QMC_SVS_CTL, nAddr, QMC_MEM_SVS, 0x1, nVal);
  nVal = HAL_VCS_HWIO_OUTFV(QDSP6SS_QMC_SVS_CTL, nAddr, QMC_MEM_SVS_SEL, 0x1, nVal);
  HAL_VCS_HWIO_OUT(QDSP6SS_QMC_SVS_CTL, nAddr, nVal);

  // Configure LDO bandgap to power on.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CFG, nAddr, LDO_PWR_UP_BG, 0x1);

  // Trigger config update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CMD, nAddr, LDO_PWR_UP_BG_UPDATE, 0x1);

  // Poll until status indicates update complete.
  while(HAL_VCS_HWIO_INF(QDSP6SS_LDO_CTL_STATUS, nAddr, LDO_PWR_UP_BG) != 0x1);

  // Wait 5 us (TBD).
  HAL_vcs_BusyWait(5);

  if (bIsCoprocEnabled)
  {
    // Enable second power switch in the LDO for the coproc.
    HAL_VCS_HWIO_OUTF(QDSP6SS_CP_PWR_CTL, nAddr, LDO_2PASS_ON, 0x1);

    // Enable coproc shunt BHS.
    HAL_VCS_HWIO_OUTF(QDSP6SS_CP_PWR_CTL, nAddr, SHUNT_BHS_ON, 0x1);
  }

  // Configure LDO to power on.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CFG, nAddr, LDO_PWR_UP, 0x1);

  // Trigger config update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CMD, nAddr, LDO_PWR_UP_UPDATE, 0x1);

  // Poll until status incicates update complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_LDO_CTL_STATUS, nAddr, LDO_PWR_UP) != 0x1);

  // Wait 10 us (TBD).
  HAL_vcs_BusyWait(10);

  // Configure core BHS to power off.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CORE_BHS_CFG, nAddr, BHS_ON, 0x0);

  if (bIsCoprocEnabled)
  {
    // Configure coproc BHS to power off.
    HAL_VCS_HWIO_OUTF(QDSP6SS_CP_BHS_CFG, nAddr, BHS_ON, 0x0);

    // Trigger coproc config update.
    HAL_VCS_HWIO_OUTF(QDSP6SS_CP_BHS_CMD, nAddr, UPDATE, 0x1);
  }

  // Trigger core config update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CORE_BHS_CMD, nAddr, UPDATE, 0x1);

  if (bIsCoprocEnabled)
  {
    // Poll until coproc status incicates update complete.
    while (HAL_VCS_HWIO_INF(QDSP6SS_CP_BHS_STATUS, nAddr, BHS_ON) != 0x0);
  }

  // Poll until core status incicates update complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CORE_BHS_STATUS, nAddr, BHS_ON) != 0x0);

  // Config LDO for bypass off.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CFG, nAddr, LDO_BYP, 0x0);

  // Trigger config update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CMD, nAddr, LDO_BYP_UPDATE, 0x1);

  // Poll until status incicates update complete.
  while(HAL_VCS_HWIO_INF(QDSP6SS_LDO_CTL_STATUS, nAddr, LDO_BYP) != 0x0);

} /* END of _HAL_vcs_QDSP6v65EnableLDO */


/* ===========================================================================
**  HAL_vcs_QDSP6v65EnableLDO
**
** ======================================================================== */

static void HAL_vcs_QDSP6v65EnableLDO
(
  HAL_vcs_LDODescType *pLDO
)
{
  return _HAL_vcs_QDSP6v65EnableLDO(pLDO->nAddr);

} /* END of HAL_vcs_QDSP6v65EnableLDO */


/* ===========================================================================
**  _HAL_vcs_QDSP6v65DisableLDO
**
** ======================================================================== */

static void _HAL_vcs_QDSP6v65DisableLDO
(
  uint32 nAddr
)
{
  boolean bIsCoprocEnabled;
  uint32  nVal;

  // Short-circuit if invalid address provided.
  if (!nAddr)
  {
    return;
  }

  // Short-circuit if already disabled.
  if (!_HAL_vcs_QDSP6v65IsLDOEnabled(nAddr))
  {
    return;
  }

  // Determine if coproc is enabled.
  bIsCoprocEnabled = _HAL_vcs_QDSP6v65IsCoprocEnabled(nAddr);

  // Configure core BHS to power on.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CORE_BHS_CFG, nAddr, BHS_ON, 0x1);

  if (bIsCoprocEnabled)
  {
    // Configure coproc BHS to power on.
    HAL_VCS_HWIO_OUTF(QDSP6SS_CP_BHS_CFG, nAddr, BHS_ON, 0x1);
  }

  // Trigger core config update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CORE_BHS_CMD, nAddr, UPDATE, 0x1);

  if (bIsCoprocEnabled)
  {
    // Trigger coproc config update.
    HAL_VCS_HWIO_OUTF(QDSP6SS_CP_BHS_CMD, nAddr, UPDATE, 0x1);
  }

  // Poll until core status incicates update complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CORE_BHS_STATUS, nAddr, BHS_ON) != 0x1);

  if (bIsCoprocEnabled)
  {
    // Poll until coproc status incicates update complete.
    while (HAL_VCS_HWIO_INF(QDSP6SS_CP_BHS_STATUS, nAddr, BHS_ON) != 0x1);
  }

  // Config LDO for bypass on.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CFG, nAddr, LDO_BYP, 0x1);

  // Trigger config update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CMD, nAddr, LDO_BYP_UPDATE, 0x1);

  // Poll until status incicates update complete.
  while(HAL_VCS_HWIO_INF(QDSP6SS_LDO_CTL_STATUS, nAddr, LDO_BYP) != 0x1);

  // Configure LDO to power off.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CFG, nAddr, LDO_PWR_UP, 0x0);

  // Trigger config update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CMD, nAddr, LDO_PWR_UP_UPDATE, 0x1);

  // Poll until status incicates update complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_LDO_CTL_STATUS, nAddr, LDO_PWR_UP) != 0x0);

  // Configure LDO bandgap to power off.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CFG, nAddr, LDO_PWR_UP_BG, 0x0);

  // Trigger config update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_CTL_CMD, nAddr, LDO_PWR_UP_BG_UPDATE, 0x1);

  // Poll until status indicates update complete.
  while(HAL_VCS_HWIO_INF(QDSP6SS_LDO_CTL_STATUS, nAddr, LDO_PWR_UP_BG) != 0x0);

  if (bIsCoprocEnabled)
  {
    // Disable second power switch in the LDO for the coproc.
    HAL_VCS_HWIO_OUTF(QDSP6SS_CP_PWR_CTL, nAddr, LDO_2PASS_ON, 0x0);

    // Disable coproc shunt BHS.
    HAL_VCS_HWIO_OUTF(QDSP6SS_CP_PWR_CTL, nAddr, SHUNT_BHS_ON, 0x0);
  }

  // Switch compiler memory from low power mode.
  nVal = HAL_VCS_HWIO_IN(QDSP6SS_QMC_SVS_CTL, nAddr);
  nVal = HAL_VCS_HWIO_OUTFV(QDSP6SS_QMC_SVS_CTL, nAddr, QMC_MEM_SVS, 0x0, nVal);
  nVal = HAL_VCS_HWIO_OUTFV(QDSP6SS_QMC_SVS_CTL, nAddr, QMC_MEM_SVS_SEL, 0x0, nVal);
  HAL_VCS_HWIO_OUT(QDSP6SS_QMC_SVS_CTL, nAddr, nVal);

} /* END of _HAL_vcs_QDSP6v65DisableLDO */


/* ===========================================================================
**  HAL_vcs_QDSP6v65DisableLDO
**
** ======================================================================== */

static void HAL_vcs_QDSP6v65DisableLDO
(
  HAL_vcs_LDODescType *pLDO
)
{
  return _HAL_vcs_QDSP6v65DisableLDO(pLDO->nAddr);

} /* END of HAL_vcs_QDSP6v65DisableLDO */


/* ===========================================================================
**  HAL_vcs_QDSP6v65SetLDOVoltage
**
** ======================================================================== */

static void HAL_vcs_QDSP6v65SetLDOVoltage
(
  HAL_vcs_LDODescType *pLDO,
  uint32               nVoltageUV
)
{
  uint32 nData;
  uint32 nNumerator, denominator;
  
  // Short-circuit invalid requests.
  if (!pLDO->nAddr || nVoltageUV < HAL_LDO_VREF_MIN_UV)
  {
    return;
  }

  /*
   * [Formula from HW designer]
   * 
   *   V_mv = 331.1677 + 4.5915 * V_hw
   *   
   * [Minimum voltage]
   *   331.1677 + 4.5915 * 0 = 331.1677 mV = 331167.7 uV.
   * 
   * [Maximum voltage]
   *   The VREF_LDO field is 7 bits wide which implies max value = 127.
   *   331.1677 + 4.5915 * 127 = 914.2882 mV = 914288.2 uV.
   *   
   * Execute formula in integer mode by multiplying by factor of 10000, thus:
   * 
   *   V_hw = (10 * V_uv - 3311677) / 45915
   */
  nNumerator = 10 * (nVoltageUV - HAL_LDO_VREF_MIN_UV);
  denominator = 45915;
  nData = nNumerator / denominator;

  /*
   * The above division rounds off any fractional components of the result.
   * For those cases we add one to the V_hw value which has the effect of
   * mapping the requested voltage to the next highest output voltage that is
   * quantized in HW (i.e. stepping).
   */
  if (nNumerator % denominator)
  {
    nData++;
  }

  // Config LDO voltage.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_VREF_SET, pLDO, VREF_LDO, nData);

  // Trigger voltage update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_LDO_VREF_CMD, pLDO, LDO_VREF_SEL_UPDATE, 0x1);

} /* END of HAL_vcs_QDSP6v65SetLDOVoltage */


/* ===========================================================================
**  HAL_vcs_QDSP6v65GetLDOVoltage
**
** ======================================================================== */

static void HAL_vcs_QDSP6v65GetLDOVoltage
(
  HAL_vcs_LDODescType *pLDO,
  uint32              *pVoltageUV,
  uint32              *pVoltageHW
)
{
  uint32 nData;

  // Get LDO voltage in HW format.
  nData = HAL_VCS_HWIO_INF(QDSP6SS_LDO_VREF_SET, pLDO, VREF_LDO);

  // Assign HW format to user provided address is available.
  if (pVoltageHW)
  {
    *pVoltageHW = nData;
  }

  /*
   * [Formula from HW designer]
   * 
   *   V_mv = 331.1677 + 4.5915 * V_hw
   *   
   * Execute formula in integer mode by multiplying by factor of 10000, thus:
   * 
   *   V_uv = (3311677 + (45915 * V_hw)) / 10
   */

  // Convert to uV format if user provided address is available.
  if (pVoltageUV)
  {
    *pVoltageUV = (3311677 + (45915 * nData)) / 10;
  }

} /* END of HAL_vcs_QDSP6v65GetLDOVoltage */


/* ===========================================================================
**  HAL_vcs_QDSP6v65EnableCoproc
**
** ======================================================================== */

static void HAL_vcs_QDSP6v65EnableCoproc
(
  HAL_vcs_CoprocDescType *pCoproc
)
{
  // Short-circuit if invalid address provided.
  if (!pCoproc->nAddr)
  {
    return;
  }

  // Short-circuit if already enabled.
  if (_HAL_vcs_QDSP6v65IsCoprocEnabled(pCoproc->nAddr))
  {
    return;
  }

  // Make sure the LDO is in BHS mode.
  _HAL_vcs_QDSP6v65DisableLDO(pCoproc->nAddr);

  // Disable the coproc clock.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_CLK_CTL, pCoproc->nAddr, CLK_ENA, 0x0);

  // Reset coproc.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_RESET, pCoproc->nAddr, ARES, 0x1);

  // Poll until status complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CP_BHS_STATUS, pCoproc->nAddr, BHS_ON) != 0x0);

  // Configure coproc to power on.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_BHS_CFG, pCoproc->nAddr, BHS_ON, 0x1);

  // Trigger coproc update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_BHS_CMD, pCoproc->nAddr, UPDATE, 0x1);

  // Poll until status complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CP_BHS_STATUS, pCoproc->nAddr, BHS_ON) != 0x1);

  // Remove coproc I/O clamp.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_PWR_CTL, pCoproc->nAddr, CLAMP_IO, 0x0);

  // Remove coproc memory clamp.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_PWR_CTL, pCoproc->nAddr, CLAMP_QMC_MEM, 0x0);

  // Configure coproc memory to power on.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CPMEM_CFG, pCoproc->nAddr, VTCM_SLP_NRET_N, 0x1);

  // Configure coproc memory periphery to power on.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CPMEM_CFG, pCoproc->nAddr, VTCM_SLP_RET_N, 0x1);

  // Trigger coproc memory update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CPMEM_CMD, pCoproc->nAddr, UPDATE_VTCM_SLP_NRET_N, 0x1);

  // Poll until status complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CPMEM_STATUS, pCoproc->nAddr, VTCM_SLP_NRET_N) != 0x1);

  // Trigger coproc memory periphery update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CPMEM_CMD, pCoproc->nAddr, UPDATE_VTCM_SLP_RET_N, 0x1);

  // Poll until status complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CPMEM_STATUS, pCoproc->nAddr, VTCM_SLP_RET_N) != 0x1);

  // Release coproc from reset.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_RESET, pCoproc->nAddr, ARES, 0x0);

  // Enable the coproc clock.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_CLK_CTL, pCoproc->nAddr, CLK_ENA, 0x1);

  // Poll to make sure the previous step is complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CP_CLK_CTL, pCoproc->nAddr, CLK_ENA) != 0x1);

} /* END of HAL_vcs_QDSP6v65EnableCoproc */


/* ===========================================================================
**  HAL_vcs_QDSP6v65DisableCoproc
**
** ======================================================================== */

static void HAL_vcs_QDSP6v65DisableCoproc
(
  HAL_vcs_CoprocDescType *pCoproc
)
{
  // Short-circuit if invalid address provided.
  if (!pCoproc->nAddr)
  {
    return;
  }

  // Short-circuit if already disabled.
  if (!_HAL_vcs_QDSP6v65IsCoprocEnabled(pCoproc->nAddr))
  {
    return;
  }

  // Make sure the LDO is in BHS mode.
  _HAL_vcs_QDSP6v65DisableLDO(pCoproc->nAddr);

  // Disable the coproc clock.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_CLK_CTL, pCoproc->nAddr, CLK_ENA, 0x0);

  // Apply coproc I/O clamp.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_PWR_CTL, pCoproc->nAddr, CLAMP_IO, 0x1);

  // Apply coproc memory clamp.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_PWR_CTL, pCoproc->nAddr, CLAMP_QMC_MEM, 0x1);

  // Configure coproc memory to power off.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CPMEM_CFG, pCoproc->nAddr, VTCM_SLP_NRET_N, 0x0);

  // Configure coproc memory periphery to power off.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CPMEM_CFG, pCoproc->nAddr, VTCM_SLP_RET_N, 0x0);

  // Trigger coproc memory periphery update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CPMEM_CMD, pCoproc->nAddr, UPDATE_VTCM_SLP_RET_N, 0x1);

  // Poll until status complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CPMEM_STATUS, pCoproc->nAddr, VTCM_SLP_RET_N) != 0x0);

  // Trigger coproc memory update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CPMEM_CMD, pCoproc->nAddr, UPDATE_VTCM_SLP_NRET_N, 0x1);

  // Poll until status complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CPMEM_STATUS, pCoproc->nAddr, VTCM_SLP_NRET_N) != 0x0);

  // Configure coproc to power off.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_BHS_CFG, pCoproc->nAddr, BHS_ON, 0x0);

  // Trigger coproc update.
  HAL_VCS_HWIO_OUTF(QDSP6SS_CP_BHS_CMD, pCoproc->nAddr, UPDATE, 0x1);

  // Poll until status complete.
  while (HAL_VCS_HWIO_INF(QDSP6SS_CP_BHS_STATUS, pCoproc->nAddr, BHS_ON) != 0x0);

} /* END of HAL_vcs_QDSP6v65DisableCoproc */

