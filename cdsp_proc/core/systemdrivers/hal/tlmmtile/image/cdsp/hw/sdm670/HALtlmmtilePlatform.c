/*
===========================================================================

FILE:         HALtlmmtilePlatform.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for the
  tlmm csr block.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/hal/tlmmtile/image/cdsp/hw/sdm670/HALtlmmtilePlatform.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/01/15   cpaulo  Created.

===========================================================================
       Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <HALtlmmtile.h>
#include "HALtlmmtileInternal.h"
#include <HALhwio.h>
#include "HALtlmmtileHWIO.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */



/*
 * Gpio Structure
 */
#define HALTLMMTILEMAXGPIO 150
uint32 pHalTlmmGpioArray[HALTLMMTILEMAXGPIO];

#define HALTLMMTILENUMTILES 3
uint32 pHalTlmmTileArray[HALTLMMTILENUMTILES] =
{
  TLMM_WEST_REG_BASE,
  TLMM_NORTH_REG_BASE,
  TLMM_SOUTH_REG_BASE
};

uint32 pHalTlmmTileDirConnArray[HALTLMMTILENUMTILES] =
{
  HWIO_TLMM_WEST_DIR_CONN_INTRn_CFG_TURING_ADDR(0),
  HWIO_TLMM_NORTH_DIR_CONN_INTRn_CFG_TURING_ADDR(0),
  HWIO_TLMM_SOUTH_DIR_CONN_INTRn_CFG_TURING_ADDR(0)
};

#define HALTLMMTILEGPIOOFFSET 0x1000

/* ===========================================================================
**  HAL_tlmmtile_PlatformInit
**
** ======================================================================== */

void HAL_tlmmtile_PlatformInit (HalTlmmGpioDescriptionType *pDesc)
{
  pDesc->pGpio                                    = &pHalTlmmGpioArray[0];
  pDesc->nMaxGpio                                 = HALTLMMTILEMAXGPIO;
  pDesc->nNumTiles                                = HALTLMMTILENUMTILES;
  pDesc->GpioConfig.nOffset                       = (uint32)( HWIO_TLMM_GPIO_CFG0_ADDR - HWIO_TLMM_GPIO_CFG0_ADDR ); 
  pDesc->GpioConfig.nGpioPullShift                = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_PULL_SHFT ); 
  pDesc->GpioConfig.nGpioPullBmsk                 = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_PULL_BMSK ); 
  pDesc->GpioConfig.nGpioFuncSelShift             = (uint32)( HWIO_TLMM_GPIO_CFG0_FUNC_SEL_SHFT ); 
  pDesc->GpioConfig.nGpioFuncSelBmsk              = (uint32)( HWIO_TLMM_GPIO_CFG0_FUNC_SEL_BMSK ); 
  pDesc->GpioConfig.nGpioDrvStrShift              = (uint32)( HWIO_TLMM_GPIO_CFG0_DRV_STRENGTH_SHFT ); 
  pDesc->GpioConfig.nGpioDrvStrBmsk               = (uint32)( HWIO_TLMM_GPIO_CFG0_DRV_STRENGTH_BMSK ); 
  pDesc->GpioConfig.nGpioOEShift                  = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_OE_SHFT ); 
  pDesc->GpioConfig.nGpioOEBmsk                   = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_OE_BMSK ); 
  pDesc->GpioConfig.nGpioHiHysShift               = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_HIHYS_EN_SHFT ); 
  pDesc->GpioConfig.nGpioHiHysBmsk                = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_HIHYS_EN_BMSK ); 
  pDesc->GpioInOut.nOffset                        = (uint32)( HWIO_TLMM_GPIO_IN_OUT0_ADDR - HWIO_TLMM_GPIO_CFG0_ADDR ); 
  pDesc->GpioInOut.nGpioInShift                   = (uint32)( HWIO_TLMM_GPIO_IN_OUT0_GPIO_IN_SHFT ); 
  pDesc->GpioInOut.nGpioInBmsk                    = (uint32)( HWIO_TLMM_GPIO_IN_OUT0_GPIO_IN_BMSK ); 
  pDesc->GpioInOut.nGpioOutShift                  = (uint32)( HWIO_TLMM_GPIO_IN_OUT0_GPIO_OUT_SHFT ); 
  pDesc->GpioInOut.nGpioOutBmsk                   = (uint32)( HWIO_TLMM_GPIO_IN_OUT0_GPIO_OUT_BMSK ); 
  pDesc->GpioIntrConfig.nOffset                   = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_ADDR - HWIO_TLMM_GPIO_CFG0_ADDR ); 
  pDesc->GpioIntrConfig.nGpioIntrEnShift          = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_INTR_ENABLE_SHFT ); 
  pDesc->GpioIntrConfig.nGpioIntrEnBmsk           = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_INTR_ENABLE_BMSK ); 
  pDesc->GpioIntrConfig.nGpioIntrPolShift         = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_INTR_POL_CTL_SHFT ); 
  pDesc->GpioIntrConfig.nGpioIntrPolBmsk          = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_INTR_POL_CTL_BMSK ); 
  pDesc->GpioIntrConfig.nGpioIntrDetShift         = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_INTR_DECT_CTL_SHFT ); 
  pDesc->GpioIntrConfig.nGpioIntrDetBmsk          = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_INTR_DECT_CTL_BMSK ); 
  pDesc->GpioIntrConfig.nGpioIntrRawStatusShift   = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_INTR_RAW_STATUS_EN_SHFT ); 
  pDesc->GpioIntrConfig.nGpioIntrRawStatusBmsk    = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_INTR_RAW_STATUS_EN_BMSK ); 
  pDesc->GpioIntrConfig.nGpioIntrTargetProcShift  = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_TARGET_PROC_SHFT ); 
  pDesc->GpioIntrConfig.nGpioIntrTargetProcBmsk   = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_TARGET_PROC_BMSK ); 
  pDesc->GpioIntrConfig.nGpioIntrDirConnEnShift   = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_DIR_CONN_EN_SHFT ); 
  pDesc->GpioIntrConfig.nGpioIntrDirConnEnBmsk    = (uint32)( HWIO_TLMM_GPIO_INTR_CFG0_DIR_CONN_EN_BMSK ); 
  pDesc->GpioIntrStatus.nOffset                   = (uint32)( HWIO_TLMM_GPIO_INTR_STATUS0_ADDR - HWIO_TLMM_GPIO_CFG0_ADDR ); 
  pDesc->GpioIntrStatus.nGpioIntrStatusShift      = (uint32)( HWIO_TLMM_GPIO_INTR_STATUS0_INTR_STATUS_SHFT ); 
  pDesc->GpioIntrStatus.nGpioIntrStatusBmsk       = (uint32)( HWIO_TLMM_GPIO_INTR_STATUS0_INTR_STATUS_BMSK ); 
  pDesc->DirConnIntrCfg.nBase                     = &pHalTlmmTileDirConnArray[0]; 
  pDesc->DirConnIntrCfg.nOffset                   = (uint32)( HWIO_TLMM_NORTH_DIR_CONN_INTRn_CFG_TURING_ADDR(1) - HWIO_TLMM_NORTH_DIR_CONN_INTRn_CFG_TURING_ADDR(0) ); 
  pDesc->DirConnIntrCfg.nMax                      = (uint32)( HWIO_TLMM_NORTH_DIR_CONN_INTRn_CFG_TURING_MAXn ); 
  pDesc->DirConnIntrCfg.nDirConnIntrCfgSelShift   = (uint32)( HWIO_TLMM_NORTH_DIR_CONN_INTRn_CFG_TURING_GPIO_SEL_SHFT ); 
  pDesc->DirConnIntrCfg.nDirConnIntrCfgSelBmsk    = (uint32)( HWIO_TLMM_NORTH_DIR_CONN_INTRn_CFG_TURING_GPIO_SEL_BMSK ); 
  pDesc->DirConnIntrCfg.nDirConnIntrCfgPolShift   = (uint32)( HWIO_TLMM_NORTH_DIR_CONN_INTRn_CFG_TURING_POLARITY_SHFT ); 
  pDesc->DirConnIntrCfg.nDirConnIntrCfgPolBmsk    = (uint32)( HWIO_TLMM_NORTH_DIR_CONN_INTRn_CFG_TURING_POLARITY_BMSK ); 
  pDesc->GpioIdStatus.nOffset                     = (uint32)( HWIO_TLMM_GPIO_ID_STATUS0_ADDR - HWIO_TLMM_GPIO_CFG0_ADDR );
  pDesc->GpioIdStatus.nGPIOIdStatusShift          = (uint32)( HWIO_TLMM_GPIO_ID_STATUS0_GPIO_ID_STATUS_SHFT );
  pDesc->GpioIdStatus.nGPIOIdStatusBmsk           = (uint32)( HWIO_TLMM_GPIO_ID_STATUS0_GPIO_ID_STATUS_BMSK );
  pDesc->GpioLpConfig.nOffset                     = (uint32)( HWIO_TLMM_GPIO_LP_CFG0_ADDR - HWIO_TLMM_GPIO_CFG0_ADDR ); 
  pDesc->GpioLpConfig.nGpioPullShift              = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_PULL_SHFT ); 
  pDesc->GpioLpConfig.nGpioPullBmsk               = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_PULL_BMSK ); 
  pDesc->GpioLpConfig.nGpioFuncSelShift           = (uint32)( HWIO_TLMM_GPIO_CFG0_FUNC_SEL_SHFT ); 
  pDesc->GpioLpConfig.nGpioFuncSelBmsk            = (uint32)( HWIO_TLMM_GPIO_CFG0_FUNC_SEL_BMSK ); 
  pDesc->GpioLpConfig.nGpioDrvStrShift            = (uint32)( HWIO_TLMM_GPIO_CFG0_DRV_STRENGTH_SHFT ); 
  pDesc->GpioLpConfig.nGpioDrvStrBmsk             = (uint32)( HWIO_TLMM_GPIO_CFG0_DRV_STRENGTH_BMSK ); 
  pDesc->GpioLpConfig.nGpioOEShift                = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_OE_SHFT ); 
  pDesc->GpioLpConfig.nGpioOEBmsk                 = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_OE_BMSK ); 
  pDesc->GpioLpConfig.nGpioOutputValShft          = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_HIHYS_EN_SHFT ); 
  pDesc->GpioLpConfig.nGpioOutputValBmsk          = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_HIHYS_EN_BMSK );
  pDesc->GpioLpConfig.nGpioValidatedShft          = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_HIHYS_EN_BMSK +1 ); 
  pDesc->GpioLpConfig.nGpioValidatedBmsk          = (uint32)( HWIO_TLMM_GPIO_CFG0_GPIO_HIHYS_EN_BMSK << 1 ); 

} /* END HAL_tlmmtile_PlatformInit */


/* ===========================================================================
**  HAL_tlmmtile_PlatformGetGpioBase
**
** ======================================================================== */

void HAL_tlmmtile_PlatformGetGpioBase ( HalTlmmGpioDescriptionType *pDesc, uint32 nGpio )
{
  uint32 nTileAddr, nTileIter;

  /*
   * Initialize the tile address to the first tile for the case where there is only 1 tile
   */
  pHalTlmmGpioArray[nGpio] = pHalTlmmTileArray[0] + ( nGpio * HALTLMMTILEGPIOOFFSET );
  for ( nTileIter = 0; nTileIter < HALTLMMTILENUMTILES; nTileIter++ )
  {
    /*
     * Read potential base address for this GPIO
     */
    nTileAddr = pHalTlmmTileArray[nTileIter] + ( nGpio * HALTLMMTILEGPIOOFFSET );
    if ( in_dword(nTileAddr + pDesc->GpioIdStatus.nOffset) & pDesc->GpioIdStatus.nGPIOIdStatusBmsk )
    {
      /*
       * Found the tile, break out
       */
      pHalTlmmGpioArray[nGpio] = nTileAddr;
      break;
    }
  }
} /* END HAL_tlmmtile_PlatformGetGpioBase */
