/*
===========================================================================

FILE:         HALgpioint.c

DESCRIPTION:  
  This is the generic hardware abstraction layer implementation for the
  GPIO interrupt controller.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/hal/gpioint/src/HALgpioint.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/08/15   cpaulo  Use HALtlmmtile to handle new tile hardware design.
11/19/08   gfr     Use outpdw instead of outpw.
11/16/07   gfr     Include HALhwIO.h to get HW output functions.
09/12/06   gfr     Created.

===========================================================================
      Copyright (c) 2007 - 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */


#include <HALhwio.h>
#include <HALgpioint.h>
#include "HALgpiointInternal.h"
#include "HALtlmmtile.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/*
 * Identifier string for this HAL.
 */
#define HAL_GPIOINT_VERSION_STRING "HAL_GPIOINT_V2"


/*
 * HAL_gpioint_IntType
 *
 * The description of a gpio interrupt.
 *
 *  eTrigger:      trigger type (edge, level, positive, negative)
 *  nController:   index of the first level controller for this interrupt
 *  nMask:         mask to use for this interrupt in the controller registers
 */
typedef struct
{
  HAL_gpioint_TriggerType   eTrigger;
  HAL_gpioint_InterruptType eInterrupt;
} HAL_gpioint_IntType;

/*
 * HAL_gpioint_Type
 *
 * Consolidate of all gpio intctl related data structures
 *
 */
typedef struct
{
  HAL_gpioint_PlatformType mPlatform;
  HAL_gpioint_IntType  amInt[HAL_GPIOINT_NUM];
} HAL_gpioint_Type;

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */

#define HAL_GPIOINT_IS_DIRCONN(interrupt) \
(interrupt < HAL_GPIOINT_SUMMARY)


/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */


/*
 * Consolidated data.
 */
HAL_gpioint_Type HAL_gpioint;



/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* ===========================================================================
**  HAL_gpioint_GetPending
**
** ======================================================================== */

void HAL_gpioint_GetPending
(
  HAL_gpioint_GroupType eGroup,
  uint32                *pnGPIO
)
{
  uint32 nGPIO,nIdx,nStatus;
  nGPIO = HAL_GPIOINT_NONE;
  for (nIdx = 0; nIdx < HAL_gpioint.mPlatform.nTotalNumber; nIdx++)
  {
    /*
     * Determine if any interrupts are pending in this register by reading
     * the status register and masking with the enabled register
     */
    
    nStatus = HAL_tlmmtile_Get( nIdx, 
                                HAL_TLMMTILE_REG_INTRSTATUS, 
                                HAL_TLMMTILE_FIELD_INTRSTATUS_STATUS );
    
    /*
     * Check if any interrupts are pending from this register
     */
    if (nStatus != 0)
    {
      if(HAL_gpioint_IsEnabled(nIdx))
      {
        nGPIO = nIdx;
        break;
      }
    }
  }

  /*
   * Save the GPIO for the caller
   */
  *pnGPIO = nGPIO;

} /* END HAL_gpioint_GetPending */



/* ===========================================================================
**  HAL_gpioint_GetNumber
**
** ======================================================================== */

void HAL_gpioint_GetNumber
(
  uint32 *pnNumber
)
{
  /*
   * Get the last GPIO number in the last controller and add one since
   * we include GPIO 0.
   */
  *pnNumber = HAL_gpioint.mPlatform.nTotalNumber;
} /* END HAL_gpioint_GetNumber */



/* ===========================================================================
**  HAL_gpioint_Enable
**
** ======================================================================== */

void HAL_gpioint_Enable
(
  uint32 nGPIO
)
{

  HAL_tlmmtile_Set( nGPIO, HAL_TLMMTILE_REG_INTRCFG, HAL_TLMMTILE_FIELD_INTRCFG_EN, 1 );

} /* END HAL_gpioint_Enable */



/* ===========================================================================
**  HAL_gpioint_Disable
**
** ======================================================================== */

void HAL_gpioint_Disable
(
  uint32 nGPIO
)
{

  HAL_tlmmtile_Set( nGPIO, HAL_TLMMTILE_REG_INTRCFG, HAL_TLMMTILE_FIELD_INTRCFG_EN, 0 );

} /* END HAL_gpioint_Disable */



/* ===========================================================================
**  HAL_gpioint_Clear
**
** ======================================================================== */
void HAL_gpioint_Clear
(
  uint32 nGPIO
)
{

  HAL_tlmmtile_Set( nGPIO, HAL_TLMMTILE_REG_INTRSTATUS, HAL_TLMMTILE_FIELD_INTRSTATUS_STATUS, 0 );

} /* END HAL_gpioint_Clear */



/* ===========================================================================
**  HAL_gpioint_IsPending
**
** ======================================================================== */

boolean HAL_gpioint_IsPending
(
  uint32 nGPIO
)
{
  uint32 nStatus;
  boolean bPending = FALSE;

  nStatus  = HAL_tlmmtile_Get( nGPIO, 
                               HAL_TLMMTILE_REG_INTRSTATUS, 
                               HAL_TLMMTILE_FIELD_INTRSTATUS_STATUS );
 
  if( nStatus != 0 )
  {
   if(HAL_gpioint_IsEnabled(nGPIO))
   {    
     bPending = TRUE;
   }
  }

  /*
   * Return if we are set
   */
  return bPending;

} /* END HAL_gpioint_IsPending */


/* ===========================================================================
**  HAL_gpioint_IsSet
**
** ======================================================================== */

boolean HAL_gpioint_IsSet
(
  uint32 nGPIO
)
{
  uint32 nStatus;
  boolean bSet = FALSE;

  nStatus  = HAL_tlmmtile_Get( nGPIO, 
                               HAL_TLMMTILE_REG_INTRSTATUS, 
                               HAL_TLMMTILE_FIELD_INTRSTATUS_STATUS );
 
  if( nStatus != 0 )
  {  
     bSet = TRUE;
  }

  /*
   * Return if we are set.
   */
  return bSet;

} /* END HAL_gpioint_IsSet */


/* ===========================================================================
**  HAL_gpioint_IsEnabled
**
** ======================================================================== */

boolean HAL_gpioint_IsEnabled
(
  uint32 nGPIO
)
{
  HAL_gpioint_ProcessorType eTarget;
  boolean bEnabled;

  uint32 nEnabled = HAL_tlmmtile_Get( nGPIO, 
                                      HAL_TLMMTILE_REG_INTRCFG, 
                                      HAL_TLMMTILE_FIELD_INTRCFG_EN );;

  bEnabled = FALSE;
  HAL_gpioint_GetTargetProcessor(nGPIO, &eTarget);
  if((eTarget  == HAL_gpioint.mPlatform.eProcessor) && (nEnabled == 1))
  {
    bEnabled = TRUE;
  }
  /*
   * Return if we are set
   */
  return bEnabled;

} /* END HAL_gpioint_IsEnabled */



/* ===========================================================================
**  HAL_gpioint_IsSupported
**
** ======================================================================== */

boolean HAL_gpioint_IsSupported
(
  uint32 nGPIO
)
{
  /*
   * Return if this interrupt is supported or not
   */
  return (nGPIO < HAL_gpioint.mPlatform.nTotalNumber);

} /* END HAL_gpioint_IsSupported */



/* ===========================================================================
**  HAL_gpioint_SetTrigger
**
** ======================================================================== */
void HAL_gpioint_SetTrigger
(
  uint32                   nGPIO,
  HAL_gpioint_TriggerType  eTrigger
)
{
  uint32 nPolarity = 1;
  uint32 nDetect;
 if (HAL_gpioint.amInt[nGPIO].eInterrupt == HAL_GPIOINT_SUMMARY)
 {
    switch(eTrigger)
    {
      case HAL_GPIOINT_TRIGGER_HIGH :
        nDetect = HAL_gpioint.mPlatform.pmCtl.nLevelDetectVal;
        break;
      case HAL_GPIOINT_TRIGGER_LOW :
        nPolarity = 0;
        nDetect = HAL_gpioint.mPlatform.pmCtl.nLevelDetectVal;
        break;
      case HAL_GPIOINT_TRIGGER_RISING :
        nDetect = HAL_gpioint.mPlatform.pmCtl.nPositiveEdgeDetectVal;
        break;
      case HAL_GPIOINT_TRIGGER_FALLING :
        nDetect = HAL_gpioint.mPlatform.pmCtl.nNegativeEdgeDetectVal;
        break;
      case HAL_GPIOINT_TRIGGER_DUAL_EDGE :
        nDetect = HAL_gpioint.mPlatform.pmCtl.nDualEdgeDetectVal;
        break;
      default:
        return;
    }

    /*
     * Set both detect and polarity based on trigger type
     */
    HAL_tlmmtile_Set( nGPIO, HAL_TLMMTILE_REG_INTRCFG, HAL_TLMMTILE_FIELD_INTRCFG_DET, nDetect );
    HAL_tlmmtile_Set( nGPIO, HAL_TLMMTILE_REG_INTRCFG, HAL_TLMMTILE_FIELD_INTRCFG_POL, nPolarity );

    /*
     * Save the new value
     */
    HAL_gpioint.amInt[nGPIO].eTrigger = eTrigger;

  }

} /* END HAL_gpioint_SetTrigger */

/* ===========================================================================
**  HAL_gpioint_GetTrigger
**
** ======================================================================== */

void HAL_gpioint_GetTrigger
(
  uint32                   nGPIO,
  HAL_gpioint_TriggerType *peTrigger
)
{
  uint32 nDetectVal, nPolarityVal;

  nDetectVal  = HAL_tlmmtile_Get( nGPIO, 
                                  HAL_TLMMTILE_REG_INTRCFG, 
                                  HAL_TLMMTILE_FIELD_INTRCFG_DET );

  nPolarityVal  = HAL_tlmmtile_Get( nGPIO, 
                                    HAL_TLMMTILE_REG_INTRCFG, 
                                    HAL_TLMMTILE_FIELD_INTRCFG_POL );


  HAL_gpioint_GetTriggerValue( nDetectVal, nPolarityVal, peTrigger );

  /*
   * Update the shadow in case it was incorrect
   */
  HAL_gpioint.amInt[nGPIO].eTrigger = *peTrigger;

} /* HAL_gpioint_GetTrigger */


/* ===========================================================================
**  HAL_gpioint_GetGroup
**
** ======================================================================== */

void HAL_gpioint_GetGroup
(
  uint32                 nGPIO,
  HAL_gpioint_GroupType *peGroup
)
{
  *peGroup = HAL_GPIOINT_GROUP1;
} /* HAL_gpioint_GetGroup */



/* ===========================================================================
**  HAL_gpioint_Save
**
** ======================================================================== */

void HAL_gpioint_Save (void)
{
  /*
   * Invoke platform save if required
   */
  if (HAL_gpioint.mPlatform.Save)
  {
    HAL_gpioint.mPlatform.Save();
  }

} /* END HAL_gpioint_Save */



/* ===========================================================================
**  HAL_gpioint_Restore
**
** ======================================================================== */

void HAL_gpioint_Restore (void)
{
  /*
   * Invoke platform restore if required
   */
  if (HAL_gpioint.mPlatform.Restore)
  {
    HAL_gpioint.mPlatform.Restore();
  }

} /* END HAL_gpioint_Restore */



/* ===========================================================================
**  HAL_gpioint_Init
**
** ======================================================================== */

void HAL_gpioint_Init
(
  HAL_gpioint_ContextType * pTarget 
)
{
  HAL_gpioint_TargetDataType * pTargetData;
  uint32 nIdx;
  /*
   * Platform early init, which fills in the mPlatform structure.
   */
  pTargetData = (HAL_gpioint_TargetDataType *)pTarget->pTargetData;
  HAL_gpioint.mPlatform.eProcessor = pTargetData->eProcessor;
  HAL_gpioint_PlatformInitEarly(&HAL_gpioint.mPlatform);
  
  for(nIdx = 0; nIdx < HAL_gpioint.mPlatform.nTotalNumber; nIdx++)
  { 
    HAL_gpioint.amInt[nIdx].eTrigger   = HAL_GPIOINT_TRIGGER_HIGH;
    HAL_gpioint.amInt[nIdx].eInterrupt = HAL_GPIOINT_SUMMARY;
  }

  HAL_tlmmtile_Init();

  /*
   * Platform late init if it exists
   */
  if (HAL_gpioint.mPlatform.InitLate != NULL)
  {
    HAL_gpioint.mPlatform.InitLate();
  }
} /* END HAL_gpioint_Init */



/* ===========================================================================
**  HAL_gpioint_Reset
**
** ======================================================================== */

void HAL_gpioint_Reset (void)
{
  /* TODO */

} /* END HAL_gpioint_Reset */

/* ===========================================================================
**  HAL_gpioint_SetTargetProcessor
**
** ======================================================================== */

void HAL_gpioint_SetTargetProcessor (HAL_gpioint_ProcessorType eProcId, 
                                     uint32                    nGpio, 
                                     HAL_gpioint_InterruptType eInterrupt)
{  
  /*
   * 0x7 is default value for None
   */
  uint32 nTarget = 0x7;
  if(eInterrupt == HAL_GPIOINT_SUMMARY)
  {
    HAL_gpioint_GetProcessorMask(eProcId, &nTarget);
    HAL_tlmmtile_Set( nGpio, HAL_TLMMTILE_REG_INTRCFG, HAL_TLMMTILE_FIELD_INTRCFG_TARGET, nTarget );
  }
} /* END HAL_gpioint_SetTargetProcessor */

/* ===========================================================================
**  HAL_gpioint_SetDirConnIntrPolarity
**
** ======================================================================== */
void HAL_gpioint_SetDirConnIntrPolarity
(
  HAL_gpioint_TriggerType  eTrigger,
  uint32 nGPIO
)
{
  uint32 nVal,eDirConnIntr;
  eDirConnIntr = HAL_gpioint.amInt[nGPIO].eInterrupt;
  if(HAL_GPIOINT_IS_DIRCONN(eDirConnIntr))
  {
    /*
     * Configure the polarity
     */
    if ( eTrigger == HAL_GPIOINT_TRIGGER_HIGH ||
         eTrigger == HAL_GPIOINT_TRIGGER_RISING )
    {
      nVal = 1;
    }
    else
    {
      nVal = 0;
    }

    HAL_tlmmtile_SetDirConn ( eDirConnIntr, HAL_TLMMTILE_FIELD_DIRCONNINTRCFG_POL, nVal );

    /*
     * Save the new value
     */
    HAL_gpioint.amInt[nGPIO].eTrigger = eTrigger;
  }

} /* END HAL_gpioint_SetDirConnPolarity */

/* ===========================================================================
**  HAL_gpioint_SetDirConnInterrupt
**
** ======================================================================== */
void HAL_gpioint_SetDirConnIntr
(
  uint32                   nGPIO,
  HAL_gpioint_InterruptType eDirConnIntr
)
{
  /* 
   * We need to Set the Direct connect CFG register Gpio select bits
   */
  
  HAL_tlmmtile_SetDirConn ( eDirConnIntr, HAL_TLMMTILE_FIELD_DIRCONNINTRCFG_SEL, nGPIO );
  
  /* 
   * We need to enable the direct connect 
   * interrupt from the Summary CFG register 
   */ 
  
  HAL_tlmmtile_Set( nGPIO, HAL_TLMMTILE_REG_INTRCFG, HAL_TLMMTILE_FIELD_INTRCFG_DIRCONNEN, 1 );

  /*
   * Save the new value
   */
  HAL_gpioint.amInt[nGPIO].eInterrupt = eDirConnIntr;
}

/* ===========================================================================
**  HAL_gpioint_DisableDirConnIntr
**
** ======================================================================== */
void HAL_gpioint_DisableDirConnIntr(uint32 nGPIO)
{
  HAL_gpioint_InterruptType eDirConnIntr;

  eDirConnIntr = HAL_gpioint.amInt[nGPIO].eInterrupt;

  if(HAL_GPIOINT_IS_DIRCONN(eDirConnIntr))
  {
    /* 
     * We need to clear the Direct connect register Gpio select bits
     */
    HAL_tlmmtile_SetDirConn ( eDirConnIntr, HAL_TLMMTILE_FIELD_DIRCONNINTRCFG_SEL, 0 );
  }

  /* 
   * We need to clear the direct connect 
   * interrupt from the Summary register 
   */ 
  HAL_tlmmtile_Set( nGPIO, HAL_TLMMTILE_REG_INTRCFG, HAL_TLMMTILE_FIELD_INTRCFG_DIRCONNEN, 0 );

  HAL_gpioint.amInt[nGPIO].eInterrupt = HAL_GPIOINT_SUMMARY;  

  HAL_gpioint_Disable(nGPIO);
}


/* ===========================================================================
**  HAL_gpioint_GetTargetProcessor
**
** ======================================================================== */

void  HAL_gpioint_GetTargetProcessor (uint32 nGpio, HAL_gpioint_ProcessorType * peProcId)
{  
  uint32 nVal;
  HAL_gpioint_InterruptType eInterrupt;
  eInterrupt = HAL_gpioint.amInt[nGpio].eInterrupt;
  if(eInterrupt == HAL_GPIOINT_SUMMARY)
  {
    nVal  = HAL_tlmmtile_Get( nGpio, 
                              HAL_TLMMTILE_REG_INTRCFG, 
                              HAL_TLMMTILE_FIELD_INTRCFG_TARGET );

    HAL_gpioint_GetProcessorId(nVal,peProcId);
  }
  else
  {
    *peProcId = HAL_gpioint.mPlatform.pmCtl.eProcessorID;
  }

  return;
 
} /* END HAL_gpioint_GetTargetProcessor */

/* ===========================================================================
**  HAL_gpioint_SetTrigger
**
** ======================================================================== */
void HAL_gpioint_TriggerInterrupt
(
  uint32                   nGPIO
)
{

  HAL_tlmmtile_Set( nGPIO, HAL_TLMMTILE_REG_INTRSTATUS, HAL_TLMMTILE_FIELD_INTRSTATUS_STATUS, 1 );

} /* END HAL_gpioint_TriggerInterrupt */

/* ===========================================================================
**  HAL_gpioint_EnableRawStatus
**
** ======================================================================== */
void HAL_gpioint_EnableRawStatus
(
  uint32 nGPIO
)
{

  HAL_tlmmtile_Set( nGPIO, HAL_TLMMTILE_REG_INTRCFG, HAL_TLMMTILE_FIELD_INTRCFG_RAW, 1 );

} /* END HAL_gpioint_EnableRawStatus */
