#ifndef DALDmoviClkControl_H
#define DALDmoviClkControl_H

/*
===============================================================================

FILE:         DALDmoviClkControl.h

DESCRIPTION:  
  This file exposes the clk management API's for the data mover device driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

===============================================================================

                                Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
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

#include "DALDmov.h"

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

void Dmovi_ControlDevEnable(DmovDevCtxt * pDevCtxt);

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

void Dmovi_ControlDevDisable(DmovDevCtxt * pDevCtxt);

/* ============================================================================
**
**  Dmovi_ControlDevReset
**
**  Description:
**    Assert the Data Mover device reset ADM/MDM (and PBus reset for ADM).
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

void Dmovi_ControlDevReset(DmovDevCtxt * pDevCtxt);

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

void Dmovi_ControlDevInit(DmovDevCtxt * pDevCtxt);

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

void Dmovi_ControlSetPerfLvl( DmovDevCtxt * pDevCtxt);

#endif /*DALDmoviClkControl_H*/
