#ifndef DALDmoviMDM_H
#define DALDmoviMDM_H

/*
===============================================================================

FILE:         DALDmoviMDM.h

DESCRIPTION:  
  This file exposes the MDM clk management API's for the data mover device driver.

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
**  Dmovi_ControlMDM_Enable
**
**  Description:
**    Enables MDM device clock.
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

void Dmovi_ControlMDM_Enable(DmovDevCtxt *pDevCtxt);

/* ============================================================================
**
**  Dmovi_ControlMDM_Disable
**
**  Description:
**    Disable MDM device clock.
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

void Dmovi_ControlMDM_Disable(DmovDevCtxt *pDevCtxt);


/* ============================================================================
**
**  Dmovi_ControlDevResetMDM
**
**  Description:
**    Assert the MDM device
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

void Dmovi_ControlDevResetMDM(DmovDevCtxt *pDevCtxt);

#endif /*DALDmoviMDM_H*/
