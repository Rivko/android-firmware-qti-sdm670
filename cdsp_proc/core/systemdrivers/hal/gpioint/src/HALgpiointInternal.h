#ifndef HAL_GPIOINT_INTERNAL_H
#define HAL_GPIOINT_INTERNAL_H
/*
===========================================================================

FILE:         HALgpiointInternal.h

DESCRIPTION:  
  This is the hardware abstraction layer internal interface for the GPIO
  interrupt controller block.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/hal/gpioint/src/HALgpiointInternal.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/12/06   gfr     Created.

===========================================================================
             Copyright (c) 2007 Qualcomm Technologies Incorporated.
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
 * HAL_gpioint_CtlType
 * The description of a gpio interrupt controller.
 *
 * nMask:          Mask of bits used in the control registers.
 * nClearReg:      Address of interrupt clear reg.
 * nEnableReg:     Address of interrupt enable reg.
 * nPolarityReg:   Address of interrupt polarity reg.
 * nDetectReg:     Address of interrupt latch control reg.
 * nStatusReg:     Address of status register.
 */
typedef struct
{
  uint32                                    nLevelDetectVal;
  uint32                                    nPositiveEdgeDetectVal;
  uint32                                    nNegativeEdgeDetectVal;
  uint32                                    nDualEdgeDetectVal;
  HAL_gpioint_ProcessorType                 eProcessorID;
} HAL_gpioint_CtlType;

/*
 * HAL_gpioint_PlatformType
 *
 * Platform (asic/processor) specific information filled in by the platform
 * code in HAL_pic_PlatformEarlyInit.
 *
 *  nNumCtl:  Number of gpio interrupt controllers.
 *  pmCtl:    Pointer to array of gpio interrupt controller
 *            descriptions.
 *            NOTE - the array should be in order of GPIOs, in particular
 *            the last GPIO in the last controller must be the last
 *            supported interrupt.
 *
 *  InitLate:      Optional function executed at the end of HAL_pic_Init.
 *  Save:          Optional function executed at the end of HAL_pic_Save.
 *  Restore:       Optional function executed at the start of
 *                 HAL_pic_Restore.
 */
typedef struct
{
  uint32               nTotalNumber;
  HAL_gpioint_CtlType  pmCtl;
  void   (*InitLate)(void);
  void   (*Save)(void);
  void   (*Restore)(void);
  HAL_gpioint_ProcessorType eProcessor;
} HAL_gpioint_PlatformType;




/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */
void HAL_gpioint_PlatformInitEarly ( HAL_gpioint_PlatformType *pmPlatform );
void HAL_gpioint_GetProcessorId(uint32 nMask, HAL_gpioint_ProcessorType* peProcId);
void HAL_gpioint_GetProcessorMask(HAL_gpioint_ProcessorType eProcId, uint32* pMask);
void HAL_gpioint_GetTriggerValue(uint32  nDetectVal,uint32 nPolarityVal,HAL_gpioint_TriggerType* peTrigger);

/*
===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================

HAL_gpioint_PlatformInitEarly( )

Description:
   This function is invoked at the start of HAL_gpioint_Init to fill in the
   platform specific details in the given core interface structure.  This
   function must be implemented by the platform core.

Parameters:
   pmPlatform - pointer to the platform interface structure to fill in.

===============================================================================


HAL_gpioint_GetProcessorId(uint32 nMask, HAL_gpioint_ProcessorType* peProcId)

Description:
   This function is used to get the Processor id for the mask value passed in nMask parameter.

Parameters:
   nMask -  Mask value of the Processor code
  peProcId - pointer of the processor id to be populated per target.
===============================================================================

HAL_gpioint_GetProcessorMask(HAL_gpioint_ProcessorType eProcId, uint32* pMask)

Description:
   This function is used to get the Processor mask per target  for the processor id passed in eProcId parameter.

Parameters:
   eProcId - Processor Id enum passed in.
  pMask - Pointer to the Mask value to be populated.

===============================================================================
*/


#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIOINT_INTERNAL_H */
