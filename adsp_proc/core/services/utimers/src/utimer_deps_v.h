#ifndef UTIMER_DEPS_V_H
#define UTIMER_DEPS_V_H

#include "utimer.h"

/*=============================================================================

FUNCTION UTIMER_INIT_TIMETICK_HANDLE

DESCRIPTION
 This function is called from utimer_init to get the timetick handle.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void utimer_init_timetick_handle(void);

/*=============================================================================

FUNCTION UTIMER_QURT_SYSCLOCK_REGISTER

DESCRIPTION
  This function registers with QURT sysclock for programming match value and getting
  back signal when the match value == timetick value. 

  This function also initializes the mutex that will be used in crit-section
  portions of timer code.

DEPENDENCIES
  This function is applicable only for BLAST.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void utimer_qurt_sysclock_register
(
  qurt_anysignal_t *sigs_obj,
  unsigned int      mask
);

/*=============================================================================

FUNCTION UTIMER_TIMETICK_GET

DESCRIPTION
 This function provides current timetick

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
utimer_timetick_type utimer_timetick_get(void);

/*=============================================================================

FUNCTION UTIMER_TIMETICK_CONV_FROM_TICKS

DESCRIPTION
 This function converts from ticks to given unit

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
utimer_timetick_type utimer_timetick_conv_from_ticks(utimer_timetick_type ticks, utimer_unit_type unit);

/*=============================================================================

FUNCTION UTIMER_TIMETICK_CONV_TO_TICKS

DESCRIPTION
 This function converts to number of target ticks for time in given unit

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
utimer_timetick_type utimer_timetick_conv_to_ticks(utimer_timetick_type time, utimer_unit_type unit);

/*=============================================================================

FUNCTION UTIMER_TIMETICK_SET_NEXT_INTERRUPT

DESCRIPTION
 This function programs next match value to hw

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
utimer_timetick_type utimer_timetick_set_next_interrupt(utimer_timetick_type match_count, utimer_timetick_type ticks_now);

#endif /* UTIMER_DEPS_V_H */