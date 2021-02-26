#include <string.h>
#include "cortex-m3.h"
#include "comdef.h"
//#include "rpm_hwio.h"
//#include "HALhwio.h"
//#include "coredump.h"
//#include "image_layout.h"
#include "CoreVerify.h"
//#include "swevent.h"
//#include "busywait.h"
//#include "Chipinfo.h"
//#include "dog.h"



// unsigned intlock_nest_level = 0;

void lock_ints(void)
{
#ifdef VIRTIO_WORKAROUND
  // Disable interrupts globally.
  __disable_irq();
#endif

  // unsigned _was_masked = get_basepri();
  set_basepri(0x10);
  // assert(!intlock_nest_level || _was_masked);
  // ++intlock_nest_level;

#ifdef VIRTIO_WORKAROUND
  __enable_irq();
#endif
}

void unlock_ints(void)
{
#ifdef VIRTIO_WORKAROUND
  // Disable interrupts globally.
  __disable_irq();
#endif

  // unsigned _is_masked = get_basepri();
  // assert(intlock_nest_level && _is_masked);
  // if(!--intlock_nest_level)
  // {
    set_basepri(0);
    // }

#ifdef VIRTIO_WORKAROUND
  __enable_irq();
#endif
}

#if 0
/*===========================================================================
FUNCTION: is_any_interrupt_pending

DESCRIPTION: 
ARM TRM says that ISRPENDING indicates a pending interrupt irrespective
of whether it is enabled or not. So This cannot be used for checking.
VECTPENDING gives the highest priority pending exception, but it also includes the 
effect of the BASEPRI and FAULTMASK registers, but not any effect of the PRIMASK register. 
We’re in an intlocked context, and we use BASEPRI as our intlock mechanism. 
BASEPRI will be 16, and thus VECTPENDING will effectively mask out all of the other interrupts.
Instead we check NVIC Enabled and Set registers to understand whether interrupts
are pending.

RETURN VALUE: True if any interrupt is pending
===========================================================================*/
bool is_any_interrupt_pending(void)
{
  int num_ints, i;
  bool result = false;
  num_ints = ((SCS.IntCtrlType & 0x1F) + 1) << 5;
  for(i = 0; ((i < num_ints / 32) && !result); i++)
  {
    if (SCS.NVIC.Enable[i] & SCS.NVIC.Set[i])
      result = true;
  }
  return result;
}
#endif
