/*
===========================================================================

FILE:         HALtlmmtile.c

DESCRIPTION:  
  This is the hardware abstraction layer interface for the tlmm csr block.

===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/TLMMTargetLib/HALtlmmtile.c#2 $

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


#include <HALhwio.h>
#include <HALtlmmtile.h>
#include "HALtlmmtileInternal.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */


/*
 * Consolidated data.
 */
HalTlmmGpioDescriptionType gHalTlmm;
static uint32 nHalTlmmIsInitialized = 0;


/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */
#define HALTLMMTILE_VALIDGPIO( x ) ( x < gHalTlmm.nMaxGpio )

#define HALTLMMTILE_VALIDDIRCONN( x ) ( x <= gHalTlmm.DirConnIntrCfg.nMax )

#define HALTLMMTILE_IN(base,offset,shift,mask) \
          in_dword_masked( base + offset, mask ) >> shift

#define HALTLMMTILE_OUT(base,offset,shift,mask,val)                       \
          out_dword_masked_ns( base + offset, mask, val << shift,     \
                               in_dword( base + offset ) )


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* ===========================================================================
**  HAL_tlmmtile_Init
** ======================================================================== */

void HAL_tlmmtile_Init            ( void )
{
  if( nHalTlmmIsInitialized == 0 )
  {
    HAL_tlmmtile_PlatformInit( &gHalTlmm ); 
    nHalTlmmIsInitialized++;
  }
}


/* ===========================================================================
**  HAL_tlmmtile_GetDirConnValues
** ======================================================================== */

static uint32 HAL_tlmmtile_GetDirConnValues ( uint32 nDirConn, HAL_tlmmtile_FieldType nField, HalTlmmOutArgs* pArgs )
{
  uint32 nOffset, nShift, nBmsk;

  if( pArgs == NULL )
  {
    return HALTLMMTILE_ERR;
  }

  nOffset = gHalTlmm.DirConnIntrCfg.nOffset * nDirConn;

  switch( nField )
  {
    case HAL_TLMMTILE_FIELD_DIRCONNINTRCFG_SEL:
      nShift = gHalTlmm.DirConnIntrCfg.nDirConnIntrCfgSelShift;
      nBmsk = gHalTlmm.DirConnIntrCfg.nDirConnIntrCfgSelBmsk;
      break;
    case HAL_TLMMTILE_FIELD_DIRCONNINTRCFG_POL:
      nShift = gHalTlmm.DirConnIntrCfg.nDirConnIntrCfgPolShift;
      nBmsk = gHalTlmm.DirConnIntrCfg.nDirConnIntrCfgPolBmsk;
      break;
    case HAL_TLMMTILE_FIELD_ALL:
      nShift = 0;
      nBmsk = ~0;
      break;
    default:
      return HALTLMMTILE_ERR;
  }

  pArgs->nOffset = nOffset;
  pArgs->nShift = nShift;
  pArgs->nBmsk = nBmsk;

  return HALTLMMTILE_SUCCESS;
}


/* ===========================================================================
**  HAL_tlmmtile_GetValues
** ======================================================================== */

static uint32 HAL_tlmmtile_GetValues ( HAL_tlmmtile_RegType nReg, HAL_tlmmtile_FieldType nField, HalTlmmOutArgs* pArgs )
{
  uint32 nOffset, nShift, nBmsk;

  if( pArgs == NULL )
  {
    return HALTLMMTILE_ERR;
  }

  switch( nReg )
  {
    case HAL_TLMMTILE_REG_CFG:
      nOffset = gHalTlmm.GpioConfig.nOffset;
      switch( nField )
      {
        case HAL_TLMMTILE_FIELD_CFG_PULL:
          nShift = gHalTlmm.GpioConfig.nGpioPullShift;
          nBmsk = gHalTlmm.GpioConfig.nGpioPullBmsk;
          break;
        case HAL_TLMMTILE_FIELD_CFG_FUNC_SEL:
          nShift = gHalTlmm.GpioConfig.nGpioFuncSelShift;
          nBmsk = gHalTlmm.GpioConfig.nGpioFuncSelBmsk;
          break;
        case HAL_TLMMTILE_FIELD_CFG_DRV_STR:
          nShift = gHalTlmm.GpioConfig.nGpioDrvStrShift;
          nBmsk = gHalTlmm.GpioConfig.nGpioDrvStrBmsk;
          break;
        case HAL_TLMMTILE_FIELD_CFG_OUTPUT_ENABLE:
          nShift = gHalTlmm.GpioConfig.nGpioOEShift;
          nBmsk = gHalTlmm.GpioConfig.nGpioOEBmsk;
          break;
        case HAL_TLMMTILE_FIELD_CFG_HIHYS:
          nShift = gHalTlmm.GpioConfig.nGpioHiHysShift;
          nBmsk = gHalTlmm.GpioConfig.nGpioHiHysBmsk;
          break;
        case HAL_TLMMTILE_FIELD_ALL:
          nShift = 0;
          nBmsk = ~0;
          break;
        default:
          return HALTLMMTILE_ERR;
      }
      break;
    case HAL_TLMMTILE_REG_INOUT:
      nOffset = gHalTlmm.GpioInOut.nOffset;
      switch( nField )
      {
        case HAL_TLMMTILE_FIELD_INOUT_IN:
          nShift = gHalTlmm.GpioInOut.nGpioInShift;
          nBmsk = gHalTlmm.GpioInOut.nGpioInBmsk;
          break;
        case HAL_TLMMTILE_FIELD_INOUT_OUT:
          nShift = gHalTlmm.GpioInOut.nGpioOutShift;
          nBmsk = gHalTlmm.GpioInOut.nGpioOutBmsk;
          break;
        case HAL_TLMMTILE_FIELD_ALL:
          nShift = 0;
          nBmsk = ~0;
          break;
        default:
          return HALTLMMTILE_ERR;
      }
      break;
    case HAL_TLMMTILE_REG_INTRCFG:
      nOffset = gHalTlmm.GpioIntrConfig.nOffset;
      switch( nField )
      {
        case HAL_TLMMTILE_FIELD_INTRCFG_EN:
          nShift = gHalTlmm.GpioIntrConfig.nGpioIntrEnShift;
          nBmsk = gHalTlmm.GpioIntrConfig.nGpioIntrEnBmsk;
          break;
        case HAL_TLMMTILE_FIELD_INTRCFG_POL:
          nShift = gHalTlmm.GpioIntrConfig.nGpioIntrPolShift;
          nBmsk = gHalTlmm.GpioIntrConfig.nGpioIntrPolBmsk;
          break;
        case HAL_TLMMTILE_FIELD_INTRCFG_DET:
          nShift = gHalTlmm.GpioIntrConfig.nGpioIntrDetShift;
          nBmsk = gHalTlmm.GpioIntrConfig.nGpioIntrDetBmsk;
          break;
        case HAL_TLMMTILE_FIELD_INTRCFG_RAW:
          nShift = gHalTlmm.GpioIntrConfig.nGpioIntrRawStatusShift;
          nBmsk = gHalTlmm.GpioIntrConfig.nGpioIntrRawStatusBmsk;
          break;
        case HAL_TLMMTILE_FIELD_INTRCFG_TARGET:
          nShift = gHalTlmm.GpioIntrConfig.nGpioIntrTargetProcShift;
          nBmsk = gHalTlmm.GpioIntrConfig.nGpioIntrTargetProcBmsk;
          break;
        case HAL_TLMMTILE_FIELD_INTRCFG_DIRCONNEN:
          nShift = gHalTlmm.GpioIntrConfig.nGpioIntrDirConnEnShift;
          nBmsk = gHalTlmm.GpioIntrConfig.nGpioIntrDirConnEnBmsk;
          break;
        case HAL_TLMMTILE_FIELD_ALL:
          nShift = 0;
          nBmsk = ~0;
          break;
        default:
          return HALTLMMTILE_ERR;
      }
      break;
    case HAL_TLMMTILE_REG_INTRSTATUS:
      nOffset = gHalTlmm.GpioIntrStatus.nOffset;
      switch( nField )
      {
        case HAL_TLMMTILE_FIELD_INTRSTATUS_STATUS:
          nShift = gHalTlmm.GpioIntrStatus.nGpioIntrStatusShift;
          nBmsk = gHalTlmm.GpioIntrStatus.nGpioIntrStatusBmsk;
          break;
        case HAL_TLMMTILE_FIELD_ALL:
          nShift = 0;
          nBmsk = ~0;
          break;
        default:
          return HALTLMMTILE_ERR;
      }
      break;
	case HAL_TLMMTILE_REG_LP_CFG:
	  nOffset = gHalTlmm.GpioLpConfig.nOffset;
      switch( nField )
      {
        case HAL_TLMMTILE_FIELD_CFG_PULL:
          nShift = gHalTlmm.GpioLpConfig.nGpioPullShift;
          nBmsk = gHalTlmm.GpioLpConfig.nGpioPullBmsk;
          break;
        case HAL_TLMMTILE_FIELD_CFG_FUNC_SEL:
          nShift = gHalTlmm.GpioLpConfig.nGpioFuncSelShift;
          nBmsk = gHalTlmm.GpioLpConfig.nGpioFuncSelBmsk;
          break;
        case HAL_TLMMTILE_FIELD_CFG_DRV_STR:
          nShift = gHalTlmm.GpioLpConfig.nGpioDrvStrShift;
          nBmsk = gHalTlmm.GpioLpConfig.nGpioDrvStrBmsk;
          break;
        case HAL_TLMMTILE_FIELD_CFG_OUTPUT_ENABLE:
          nShift = gHalTlmm.GpioLpConfig.nGpioOEShift;
          nBmsk = gHalTlmm.GpioLpConfig.nGpioOEBmsk;
          break;
        case HAL_TLMMTILE_FIELD_CFG_LP_OUTVAL:
          nShift = gHalTlmm.GpioLpConfig.nGpioOutputValShft;
          nBmsk = gHalTlmm.GpioLpConfig.nGpioOutputValBmsk;
          break;
		case HAL_TLMMTILE_FIELD_CFG_LP_VALIDATED:
          nShift = gHalTlmm.GpioLpConfig.nGpioValidatedShft;
          nBmsk = gHalTlmm.GpioLpConfig.nGpioValidatedBmsk;
          break;
        case HAL_TLMMTILE_FIELD_ALL:
          nShift = 0;
          nBmsk = ~0;
          break;
        default:
          return HALTLMMTILE_ERR;
      }	  
      break;
    default:
      return HALTLMMTILE_ERR;
  }

  pArgs->nOffset = nOffset;
  pArgs->nShift = nShift;
  pArgs->nBmsk = nBmsk;

  return HALTLMMTILE_SUCCESS;
}

/* ===========================================================================
**  HAL_tlmmtile_Set
** ======================================================================== */

uint32 HAL_tlmmtile_Set ( uint32 nGPIO, HAL_tlmmtile_RegType nReg, HAL_tlmmtile_FieldType nField, uint32 nVal )
{
  HalTlmmOutArgs Args;

  if( !HALTLMMTILE_VALIDGPIO( nGPIO ) )
  {
    return HALTLMMTILE_ERR;
  }

  if ( HAL_tlmmtile_GetValues ( nReg, nField, &Args ) == HALTLMMTILE_ERR )
  {
    return HALTLMMTILE_ERR;
  }

  if ( gHalTlmm.pGpio[nGPIO] == 0 )
  {
    /*
     * Initialize GPIO base address
     */
    HAL_tlmmtile_PlatformGetGpioBase ( &gHalTlmm, nGPIO );
  }

  HALTLMMTILE_OUT( gHalTlmm.pGpio[nGPIO],
                   Args.nOffset,
                   Args.nShift,
                   Args.nBmsk,
                   nVal );

  return HALTLMMTILE_SUCCESS;
}


/* ===========================================================================
**  HAL_tlmmtile_Get
** ======================================================================== */

uint32 HAL_tlmmtile_Get ( uint32 nGPIO, HAL_tlmmtile_RegType nReg, HAL_tlmmtile_FieldType nField )
{
  HalTlmmOutArgs Args;

  if( !HALTLMMTILE_VALIDGPIO( nGPIO ) )
  {
    return HALTLMMTILE_ERR;
  }

  if ( HAL_tlmmtile_GetValues ( nReg, nField, &Args ) == HALTLMMTILE_ERR )
  {
    return HALTLMMTILE_ERR;
  }

  if ( gHalTlmm.pGpio[nGPIO] == 0 )
  {
    /*
     * Initialize GPIO base address
     */
    HAL_tlmmtile_PlatformGetGpioBase ( &gHalTlmm, nGPIO );
  }

  return HALTLMMTILE_IN( gHalTlmm.pGpio[nGPIO],
                         Args.nOffset,
                         Args.nShift,
                         Args.nBmsk );

}


/* ===========================================================================
**  HAL_tlmmtile_SetDirConn
** ======================================================================== */

uint32 HAL_tlmmtile_SetDirConn ( uint32 nDirConn, HAL_tlmmtile_FieldType nField, uint32 nVal )
{
  HalTlmmOutArgs Args;

  if( !HALTLMMTILE_VALIDDIRCONN( nDirConn ) )
  {
    return HALTLMMTILE_ERR;
  }

  if ( HAL_tlmmtile_GetDirConnValues ( nDirConn, nField, &Args ) == HALTLMMTILE_ERR )
  {
    return HALTLMMTILE_ERR;
  }


  HALTLMMTILE_OUT( gHalTlmm.DirConnIntrCfg.nBase,
                   Args.nOffset,
                   Args.nShift,
                   Args.nBmsk,
                   nVal );

  return HALTLMMTILE_SUCCESS;
}


/* ===========================================================================
**  HAL_tlmmtile_GetDirConn
** ======================================================================== */

uint32 HAL_tlmmtile_GetDirConn ( uint32 nDirConn, HAL_tlmmtile_FieldType nField )
{
  HalTlmmOutArgs Args;

  if( !HALTLMMTILE_VALIDDIRCONN( nDirConn ) )
  {
    return HALTLMMTILE_ERR;
  }

  if ( HAL_tlmmtile_GetDirConnValues ( nDirConn, nField, &Args ) == HALTLMMTILE_ERR )
  {
    return HALTLMMTILE_ERR;
  }

  return HALTLMMTILE_IN( gHalTlmm.DirConnIntrCfg.nBase,
                         Args.nOffset,
                         Args.nShift,
                         Args.nBmsk );

}

