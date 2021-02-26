/*
===========================================================================

FILE:         HALgpiointPlatformModem.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for the
  GPIO interrupt controller block.
  This platform is for the Target processor on the MSM8960.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/hal/gpioint/image/adsp/hw/sdm670/HALgpiointPlatform.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/20/11   an     Created to unify platform files.

===========================================================================
             Copyright (c) 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <HALgpioint.h>
#include "HALgpiointInternal.h"
#include <HALhwio.h>
#include "HALgpiointHWIO.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* ===========================================================================
**  HAL_gpioint_PlatformInitEarly
**
** ======================================================================== */

void HAL_gpioint_PlatformInitEarly (HAL_gpioint_PlatformType *pmPlatform)
{
  pmPlatform->nTotalNumber                         = 150;
  pmPlatform->pmCtl.nLevelDetectVal                = HWIO_TLMM_GPIO_INTR_CFG0_INTR_DECT_CTL_LEVEL_FVAL;
  pmPlatform->pmCtl.nPositiveEdgeDetectVal         = HWIO_TLMM_GPIO_INTR_CFG0_INTR_DECT_CTL_POS_EDGE_FVAL;
  pmPlatform->pmCtl.nNegativeEdgeDetectVal         = HWIO_TLMM_GPIO_INTR_CFG0_INTR_DECT_CTL_NEG_EDGE_FVAL;
  pmPlatform->pmCtl.nDualEdgeDetectVal             = HWIO_TLMM_GPIO_INTR_CFG0_INTR_DECT_CTL_DUAL_EDGE_FVAL;
  pmPlatform->pmCtl.eProcessorID                   = HAL_GPIOINT_LPA_DSP_PROC;
} /* END HAL_gpioint_PlatformInitEarly */


/* ===========================================================================
**  HAL_gpioint_GetProcessorId
**
** ======================================================================== */

void HAL_gpioint_GetProcessorId(uint32 nMask, HAL_gpioint_ProcessorType* peProcId)
{
  switch (nMask) 
  {
    case 0x0 : 
      *peProcId = HAL_GPIOINT_SPS_PROC;
      break;
    case 0x1 : 
      *peProcId = HAL_GPIOINT_LPA_DSP_PROC; 
      break;
    case 0x2 : 
      *peProcId = HAL_GPIOINT_RPM_PROC;
      break;
    case 0x4 : 
      *peProcId = HAL_GPIOINT_MSS_PROC;
      break;
    case 0x5 : 
      *peProcId = HAL_GPIOINT_SC_PROC;
      break;
    case 0x6 : 
      *peProcId = HAL_GPIOINT_TURING_PROC;
      break;
    default :
      *peProcId = HAL_GPIOINT_NONE_PROC; 
      break;
  }
  
  return;

} /* HAL_gpioint_GetProcessorId */


/* ===========================================================================
**  HAL_gpioint_GetProcessorMask
**
** ======================================================================== */

void HAL_gpioint_GetProcessorMask(HAL_gpioint_ProcessorType eProcId, uint32* pMask)
{
  switch (eProcId) 
  {
    case HAL_GPIOINT_SPS_PROC :
      *pMask = 0x0;
      break;
    case HAL_GPIOINT_LPA_DSP_PROC: 
      *pMask = (0x1);
      break;
    case HAL_GPIOINT_RPM_PROC: 
      *pMask = (0x2);
      break;
    case HAL_GPIOINT_MSS_PROC: 
      *pMask = (0x4);
      break;
    case HAL_GPIOINT_SC_PROC: 
      *pMask = (0x5);
      break;
    case HAL_GPIOINT_TURING_PROC: 
      *pMask = (0x6);
      break;
    default:
     *pMask = (0x7); 
    break;
  }
  return;

} /* HAL_gpioint_GetProcessorMask */


/* ===========================================================================
**  HAL_gpioint_GetTriggerValue
**
** ======================================================================== */

void HAL_gpioint_GetTriggerValue(uint32  nDetectVal,uint32 nPolarityVal,HAL_gpioint_TriggerType* peTrigger)
{
  switch(nDetectVal)
  {
    case 0 :
       *peTrigger =
       (nPolarityVal ? HAL_GPIOINT_TRIGGER_HIGH : HAL_GPIOINT_TRIGGER_LOW);
      break;
    case 1 :
      *peTrigger = HAL_GPIOINT_TRIGGER_RISING; 
      break;
    case 2 :
      *peTrigger = HAL_GPIOINT_TRIGGER_FALLING;    
      break;
    case 3 :
      *peTrigger = HAL_GPIOINT_TRIGGER_DUAL_EDGE;         
      break;
    default:
      return;
  }

} /* HAL_gpioint_GetTriggerValue */
