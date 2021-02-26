#ifndef DALDmoviADM_H
#define DALDmoviADM_H

/*
===============================================================================

FILE:         DALDmoviADM.h

DESCRIPTION:  
  This file exposes the ADM clk management API's for the data mover device driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

===============================================================================

                                Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/13/09   an      Created

===============================================================================
                   Copyright (c) 2009-2009 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================
*/

/* ----------------------------------------------------------------------------
**                           INCLUDES
** ------------------------------------------------------------------------- */
#include "DDIDmov.h"
#include "DALDmov.h"

/*------------------------------------------------------------------------------
Following functions are for DALDriver internal functionality
------------------------------------------------------------------------------*/

/* ============================================================================
**
**  Dmovi_ControlADM_Enable
**
**  Description:
**    Enables ADM device clock.
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

void Dmovi_ControlADM_Enable(DmovDevCtxt *pDevCtxt);

/* ============================================================================
**
**  Dmovi_ControlADM_Disable
**
**  Description:
**    Disable ADM device clock.
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

void Dmovi_ControlADM_Disable(DmovDevCtxt *pDevCtxt);

/* ============================================================================
**
**  Dmovi_ControlDevResetADM
**
**  Description:
**    Assert the ADM device and PBus reset.
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

void Dmovi_ControlDevResetADM(DmovDevCtxt * pDevCtxt);

/* ============================================================================
**
**  Dmovi_ControlDevInitADM
**
**  Description:
**    Initialize the ADM device.
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    Insure that device clocks are enabled.
**
** ========================================================================= */

void Dmovi_ControlDevInitADM(DmovDevCtxt * pDevCtxt);

/* ============================================================================
**
**  Dmovi_ControlSetPerfLvlADM
**
**  Description:
**    Sets the desired (max possible) performance level for ADM. It chooses 
**  the best performance level requested among the clients registered with
**  ADM
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**    PerfLvl - desired performance level
**
**  Return:
**    None
**
**  Dependencies:
**    Insure that ADM clock is enabled. Only ADM is supported
**
** ========================================================================= */

void Dmovi_ControlSetPerfLvlADM(DmovDevCtxt * pDevCtxt, DmovPerfLvlType PerfLvl);

#endif /*DALDmoviADM_H*/
