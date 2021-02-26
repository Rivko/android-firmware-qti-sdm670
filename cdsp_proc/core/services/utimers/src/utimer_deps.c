#include "err.h"
#include "qurt.h"

#include "utimer.h"
#include "utimer_osal.h"

/* registration id obtained for timer signal registered with QURT Sysclock. */
static int utimer_qurt_sysclock_id = -1;

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
void utimer_init_timetick_handle(void)
{
   /* Currently nothing to do */
} /* utimer_init_timetick_handle */

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
)
{
  utimer_qurt_sysclock_id = qurt_sysclock_register (sigs_obj, mask);

  if (utimer_qurt_sysclock_id < 0) 
  {
    ERR_FATAL("sclk registration failed",0,0,0);    
  }
} /* utimer_qurt_sysclock_register */

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
utimer_timetick_type utimer_timetick_get(void)
{
   return uTimetick_Get();
} /* utimer_timetick_get */

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
utimer_timetick_type utimer_timetick_conv_from_ticks(utimer_timetick_type ticks, utimer_unit_type unit)
{
  return (utimer_timetick_type) uTimetick_CvtFromTicks((uTimetick_type)ticks, (uTimetick_unitType)unit);
} /* utimer_timetick_conv_from_ticks */

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
utimer_timetick_type utimer_timetick_conv_to_ticks(utimer_timetick_type time, utimer_unit_type unit)
{
  return (utimer_timetick_type) uTimetick_CvtToTicks((uTimetick_type)time, (uTimetick_unitType)unit);
} /* utimer_timetick_conv_to_ticks */

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
utimer_timetick_type utimer_timetick_set_next_interrupt(utimer_timetick_type match_count, utimer_timetick_type ticks_now)
{
    return qurt_sysclock_alarm_create (utimer_qurt_sysclock_id, ticks_now, match_count);
} /* utimer_timetick_set_next_interrupt */