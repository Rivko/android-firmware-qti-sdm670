#ifndef UTIMETICK_H
#define UTIMETICK_H
/**
* @file uTimetick.h
* @brief Timetick Header File for uImage.
* This file contains the definitions of the constants, data
* structures, and interfaces that comprise the Timetick driver. 
*
*/

/*
===========================================================================
                             Edit History
$Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/uTimetick.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright © 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/
#include "DDITimetick.h"



typedef DalTimetickUnitType  uTimetick_unitType; /**< -- Various units supported by the timetick module*/
typedef DalTimetickTime64Type  uTimetick_type; /**< -- Time in 32bit ticks*/ 

/*=============================================================================

FUNCTION UTIMETICK_GET

DESCRIPTION
  Returns the QTimer Count. The QTimer runs at 19.2MHz 

DEPENDENCIES
  None.

RETURN VALUE
  QTimer Count value

SIDE EFFECTS
  None

=============================================================================*/

uTimetick_type uTimetick_Get( void );

/*=============================================================================

FUNCTION UTIMETICK_CVTFROMTICKS

DESCRIPTION
    Converts the QTimer ticks to specificed unit.

DEPENDENCIES
  None.

RETURN VALUE
  QTimer Count value in the unit specified.

SIDE EFFECTS
  None

=============================================================================*/

uTimetick_type uTimetick_CvtFromTicks
( 
   uTimetick_type, 
   uTimetick_unitType 
);

/*=============================================================================

FUNCTION UTIMETICK_CVTTOTICKS

DESCRIPTION
    Converts the specified time to QTimer equivalent ticks.

DEPENDENCIES
  None.

RETURN VALUE
  QTimer Count ticks equivalent to the time specified.

SIDE EFFECTS
  None

=============================================================================*/

uTimetick_type uTimetick_CvtToTicks
( 
   uTimetick_type, 
   uTimetick_unitType 
);


/*=============================================================================

FUNCTION UTIMETICK_SETWAKEUPTIMERINTERRUPT

DESCRIPTION
  Program the match value for Wakeup Timer 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void uTimetick_SetWakeUpTimerInterrupt
(
	uint64 matchValue 
);


/*=============================================================================

FUNCTION UTIMETICK_ENABLEWAKEUPTIMERINTERRUPT

DESCRIPTION
  Enable/Disable the WakeupTimer interrupt.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void uTimetick_EnableWakeUpTimerInterrupt
(
boolean enable
);
#endif /* UTIMETICK_H */

