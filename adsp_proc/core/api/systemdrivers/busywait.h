#ifndef BUSYWAIT_H
#define BUSYWAIT_H
/*===========================================================================

                         Busywait Header File

DESCRIPTION
  This header file contains the public API for the busywait component.

      Copyright (c) 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/busywait.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/10   cmd     Replaced busywait_config() with busywait_config_cpu_freq()
                   and busywait_config_pause_timer_freq().
11/09/09   cmd     Added busywait_test() function.
10/22/09   cmd     Initial version.

===========================================================================*/

#include "com_dtypes.h"     /* Common Definitions and Types */


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*==========================================================================

  FUNCTION      BUSYWAIT

  DESCRIPTION   This function pauses the execution of a thread for a
                specified time.

  PARAMETERS    pause_time_us - Time to pause in microseconds

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void busywait (uint32 pause_time_us);


/*==========================================================================

  FUNCTION      BUSYWAIT_CONFIG_CPU_FREQ

  DESCRIPTION   This function is called to notify the busywait component of
                changes in the CPU frequency.

  PARAMETERS    freq_khz - CPU frequency in kHz

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The global variable for CPU frequency will be updated.

==========================================================================*/

void busywait_config_cpu_freq (uint32 freq_khz);


/*==========================================================================

  FUNCTION      BUSYWAIT_CONFIG_PAUSE_TIMER_FREQ

  DESCRIPTION   This function is called to notify the busywait component of
                changes in the pause timer frequency.

  PARAMETERS    freq_khz - Pause timer frequency in kHz

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The global variable for pause timer frequency will be
                updated.

==========================================================================*/

void busywait_config_pause_timer_freq (uint32 freq_khz);


/*==========================================================================

  FUNCTION      BUSYWAIT_TEST

  DESCRIPTION   This function is intended for unit testing of the busywait
                component only and should not be called from production
                code.  This function makes several calls to the busywait
                function that can then be measured using the rt.view
                facility within Trace32.  Interrupts should be disabled
                during execution of this function.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void busywait_test (void);

#endif /* BUSYWAIT_H */

