/*
===============================================================================

FILE:         DALDmoviClkRegime.c

DESCRIPTION:  
  This file implements the data mover clock control when clkregime is present.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

===============================================================================

                                Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/21/09   an      Added internal functions to abstract clock management for 
                   ADM and MDM separately since some of clock regime APIs 
				   may not be available for every image of a target
03/27/09   MK      Added clock management 
01/18/08   ah      Created

===============================================================================
                   Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================
*/

/* ----------------------------------------------------------------------------
**                           INCLUDES
** ------------------------------------------------------------------------- */

#include "HALdmov.h"
#include "DALDmov.h"
#include "DALDmoviADM.h"
#include "DALDmoviMDM.h"
#ifndef DMOV_USE_DAL_CLOCK_DRIVER
#include "clkregim.h"
#endif

/*------------------------------------------------------------------------------
Following functions are for DALDriver internal functionality
------------------------------------------------------------------------------*/

/* ============================================================================
**
**  Dmovi_ControlDevEnable
**
**  Description:
**    Enable the Data Mover device clock.
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_ControlDevEnable( DmovDevCtxt * pDevCtxt )
{
  switch (pDevCtxt->dwDeviceInstance) 
  {
    case HAL_DMOV_DEV_ADM:
    case HAL_DMOV_DEV_ADM_1:
        Dmovi_ControlADM_Enable(pDevCtxt);
        break;
    case HAL_DMOV_DEV_MDM:
        Dmovi_ControlMDM_Enable(pDevCtxt);
        break;
    default:
        break;
  }
}

/* ============================================================================
**
**  Dmovi_ControlDevDisable
**
**  Description:
**    Disable the Data Mover device clock.
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_ControlDevDisable( DmovDevCtxt * pDevCtxt )
{
  switch (pDevCtxt->dwDeviceInstance)
  {
    case HAL_DMOV_DEV_ADM:
    case HAL_DMOV_DEV_ADM_1:
        Dmovi_ControlADM_Disable(pDevCtxt);
        break;
    case HAL_DMOV_DEV_MDM:
        Dmovi_ControlMDM_Disable(pDevCtxt);
        break;
    default:
        break;
  }
}

/* ============================================================================
**
**  Dmovi_ControlDevReset
**
**  Description:
**    Assert the Data Mover device reset ADM/MDM (and PBus reset for ADM).
**    NO RESET FOR NOW IN APPS/MODEM. ONLY DONE IN BOOT
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_ControlDevReset( DmovDevCtxt * pDevCtxt )
{
  /* Assert the Data Mover device reset ADM/MDM (and PBus reset for ADM)
   * Disable the Data Mover device clock 
   */

   switch (pDevCtxt->dwDeviceInstance)
   {
     case HAL_DMOV_DEV_ADM:
          Dmovi_ControlDevResetADM(pDevCtxt);
          break;
     case HAL_DMOV_DEV_MDM:
          Dmovi_ControlDevResetMDM(pDevCtxt);
          break;
     default:
          break;
   }
}

/* ============================================================================
**
**  Dmovi_ControlDevInit
**
**  Description:
**    Initialize the Data Mover device.
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    Insure that device clocks are enabled.
**    Not done for ARM11 initialization.
**
** ========================================================================= */

void Dmovi_ControlDevInit( DmovDevCtxt * pDevCtxt )
{
  /* Negate the Data Mover device reset (and PBus reset for ADM) */
  switch (pDevCtxt->dwDeviceInstance) 
  {
    case HAL_DMOV_DEV_ADM:
         Dmovi_ControlDevInitADM(pDevCtxt);
         break;
    case HAL_DMOV_DEV_MDM:
         break;
    default:
         break;
  }

}


/* ============================================================================
**
**  Dmovi_ControlSetPerfLvl
**
**  Description:
**    Sets the desired (max possible) performance level for ADM. It chooses 
**  the best performance level requested among the clients registered with
**  ADM
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    Insure that ADM clock is enabled. Only ADM is supported
**    Not done for ARM11 initialization.
**
** ========================================================================= */

void Dmovi_ControlSetPerfLvl( DmovDevCtxt * pDevCtxt )
{
  DmovPerfLvlType PerfLvl;
  
  switch (pDevCtxt->dwDeviceInstance) 
  {
     case HAL_DMOV_DEV_ADM:
        {
          if(pDevCtxt->dwOpenCount !=0)
          {
             for(PerfLvl=DMOV_PERFLVL_MAX;PerfLvl<DMOV_NUM_PERFLVLS;PerfLvl++)
             {
                if(pDevCtxt->nActiveClientsPerfLvls[PerfLvl]) 
                {
                   if(PerfLvl != pDevCtxt->eCurrentPerfLvl)
                   {
					  Dmovi_ControlSetPerfLvlADM(pDevCtxt, PerfLvl);

                      pDevCtxt->eCurrentPerfLvl = PerfLvl;
                   }  /* if(PerfLvl != pDevCtxt->eCurrentPerfLvl ...*/

                   /* We had an active client with PerfLvl so break the loop */
                   break;
                }  /* if(pDevCtxt->nActiveClientsPerfLvls[PerfLv ...*/
             }  /* for(PerfLvl=DMOV_PERFLVL_MAX;PerfLvl<DMOV_NUM ...*/
          }  /* if(pDevCtxt->dwOpenCount !=0)  */
          else
          {
             /* Default values if no client is active */
			 Dmovi_ControlSetPerfLvlADM(pDevCtxt, DMOV_PERFLVL_MAX);

             pDevCtxt->eCurrentPerfLvl = DMOV_PERFLVL_MAX;
          }
        }
       break;
    default:
        break;
  }
}
