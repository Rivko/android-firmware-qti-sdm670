#ifndef HAL_TLMM_INTERNAL_H
#define HAL_TLMM_INTERNAL_H
/*
===========================================================================

FILE:         HALtlmmInternal.h

DESCRIPTION:  
  This is the hardware abstraction layer internal interface for the tlmm
  csr block.

===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/TLMMTargetLib/HALtlmmtileInternal.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/01/15   cpaulo  Created.

===========================================================================
       Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * Internal Args structure to pass between internal functions
 */

typedef struct
{
  uint32 nOffset;
  uint32 nShift;
  uint32 nBmsk;
}
HalTlmmOutArgs;

/*
 * Register info for TLMM_GPIO_CFG register
 */

typedef struct
{
  uint32 nOffset;
  uint32 nGpioPullShift;
  uint32 nGpioPullBmsk;
  uint32 nGpioFuncSelShift;
  uint32 nGpioFuncSelBmsk;
  uint32 nGpioDrvStrShift;
  uint32 nGpioDrvStrBmsk;
  uint32 nGpioOEShift;
  uint32 nGpioOEBmsk;
  uint32 nGpioHiHysShift;
  uint32 nGpioHiHysBmsk;
}
HalTlmmGpioConfigType;

/*
 * Register info for TLMM_GPIO_IN_OUT register
 */

typedef struct
{
  uint32 nOffset;
  uint32 nGpioInShift;
  uint32 nGpioInBmsk;
  uint32 nGpioOutShift;
  uint32 nGpioOutBmsk;
}
HalTlmmGpioInOutType;

/*
 * Register info for TLMM_GPIO_INTR_CFG register
 */

typedef struct
{
  uint32 nOffset;
  uint32 nGpioIntrEnShift;
  uint32 nGpioIntrEnBmsk;
  uint32 nGpioIntrPolShift;
  uint32 nGpioIntrPolBmsk;
  uint32 nGpioIntrDetShift;
  uint32 nGpioIntrDetBmsk;
  uint32 nGpioIntrRawStatusShift;
  uint32 nGpioIntrRawStatusBmsk;
  uint32 nGpioIntrTargetProcShift;
  uint32 nGpioIntrTargetProcBmsk;
  uint32 nGpioIntrDirConnEnShift;
  uint32 nGpioIntrDirConnEnBmsk;
}
HalTlmmGpioIntrConfigType;

/*
 * Register info for TLMM_GPIO_INTR_STATUS register
 */

typedef struct
{
  uint32 nOffset;
  uint32 nGpioIntrStatusShift;
  uint32 nGpioIntrStatusBmsk;
}
HalTlmmGpioIntrStatusType;

/*
 * Register info for TLMM_GPIO_CFG register
 */

typedef struct
{
  uint32 nOffset;
  uint32 nGpioPullShift;
  uint32 nGpioPullBmsk;
  uint32 nGpioFuncSelShift;
  uint32 nGpioFuncSelBmsk;
  uint32 nGpioDrvStrShift;
  uint32 nGpioDrvStrBmsk;
  uint32 nGpioOEShift;
  uint32 nGpioOEBmsk;
  uint32 nGpioOutputValShft;
  uint32 nGpioOutputValBmsk;
  uint32 nGpioValidatedShft;
  uint32 nGpioValidatedBmsk;
}
HalTlmmGpioLpConfigType;

/*
 * Register info for DIR_CONN_CFG register
 */

typedef struct
{
  uint32 nBase;
  uint32 nOffset;
  uint32 nMax;
  uint32 nDirConnIntrCfgSelShift;
  uint32 nDirConnIntrCfgSelBmsk;
  uint32 nDirConnIntrCfgPolShift;
  uint32 nDirConnIntrCfgPolBmsk;
}
HalTlmmGpioDirConnIntrCfgType;

/*
 * Register info for DIR_CONN_CFG register
 */

typedef struct
{
  uint32 nOffset;
  uint32 nGPIOIdStatusShift;
  uint32 nGPIOIdStatusBmsk;
}
HalTlmmGpioIdStatusType;

/*
 * Main config structure that maintains all the target specific contents
 */

typedef struct
{
  uint32*                       pGpio;
  uint32                        nMaxGpio;
  HalTlmmGpioConfigType         GpioConfig;
  HalTlmmGpioInOutType          GpioInOut;
  HalTlmmGpioIntrConfigType     GpioIntrConfig;
  HalTlmmGpioIntrStatusType     GpioIntrStatus;
  HalTlmmGpioLpConfigType       GpioLpConfig;
  HalTlmmGpioDirConnIntrCfgType DirConnIntrCfg;
  HalTlmmGpioIdStatusType       GpioIdStatus;
}
HalTlmmGpioDescriptionType;


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */
void HAL_tlmmtile_PlatformInit ( HalTlmmGpioDescriptionType *pDesc );

void HAL_tlmmtile_PlatformGetGpioBase ( HalTlmmGpioDescriptionType *pDesc, uint32 nGpio );

/*
===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================

HAL_tlmmtile_PlatformInit( )

Description:
   This function is invoked at the start of HAL_tlmm_Init to fill in the
   platform specific details in the given core interface structure.  This
   function must be implemented by the platform core.

Parameters:
   pDesc - pointer to the platform interface structure to fill in.

===============================================================================
*/


#ifdef __cplusplus
}
#endif

#endif /* HAL_TLMM_INTERNAL_H */
