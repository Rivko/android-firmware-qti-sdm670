#ifndef __TRAMP_GPIO_H__
#define __TRAMP_GPIO_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  T R A M P O L I N E    S E R V I C E S
                      G P I O    I N T E R R U P T S

GENERAL DESCRIPTION
  This file contains the interface for the trampoline services used to
  manage the GPIO interrupt controller.

EXTERNALIZED FUNCTIONS
  tramp_gpio_init
  tramp_gpio_register_isr
  tramp_gpio_deregister_isr
  tramp_gpio_set_trigger
  tramp_gpio_save
  tramp_gpio_restore
  tramp_gpio_switch_to_apps
  tramp_gpio_switch_to_modem
  tramp_gpio_is_interrupt_enabled
  tramp_gpio_is_interrupt_pending

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Function tramp_gpio_init() should be called before any of the
  Tramp GPIO services can be used.

Copyright (c) 2007 by Qualcomm Technologies Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is
regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/tramp_gpio.h#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/11/08   gfr     Remove tramp_gpio completely from WM.
06/23/07   jz      Implemented status triggering gpio interrupt during
                   power collapse instead of shared memory.
09/19/07   gfr     Support for WinCE.
07/28/07   gfr     Initial version.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "tramp.h"



/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

/*
 * See tramp.h
 */


/*=========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/


/*==========================================================================

  FUNCTION      TRAMP_GPIO_SET_TRIGGER

  DESCRIPTION   Configures the trigger type for a GPIO interrupt.

  PARAMETERS    gpio   - the gpio interrupt to configure
                trigger - the trigger type to set

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_gpio_set_trigger
(
  uint32               gpio,
  tramp_trigger_type   trigger
);


/*==========================================================================

  FUNCTION      TRAMP_GPIO_REGISTER_ISR

  DESCRIPTION   Registers an ISR for a GPIO interrupt.

  PARAMETERS    gpio    - the gpio interrupt to configure
                trigger - the trigger type to set
                isr     - the service routine
                param   - parameter to pass to service routine

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  Enables the GPIO interrupt.

==========================================================================*/

void tramp_gpio_register_isr
(
  uint32              gpio,
  tramp_trigger_type  trigger,
  tramp_handler_type  isr,
  uint32              param
);


/*==========================================================================

  FUNCTION      TRAMP_GPIO_DEREGISTER_ISR

  DESCRIPTION   Deregisters an ISR for a GPIO interrupt.

  PARAMETERS    gpio    - the gpio interrupt to configure
                isr     - the service routine which was installed

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  Disables the GPIO interrupt.

==========================================================================*/

void tramp_gpio_deregister_isr
(
  uint32              gpio,
  tramp_handler_type  isr           
);


/*==========================================================================

  FUNCTION      TRAMP_GPIO_IS_INTERRUPT_ENABLED

  DESCRIPTION   Returns if the GPIO interrupt is enabled.

  PARAMETERS    gpio - Which GPIO to check.

  DEPENDENCIES  None.

  RETURN VALUE  boolean - Whether or not the GPIO interrupt is enabled.

  SIDE EFFECTS  None.

==========================================================================*/

boolean tramp_gpio_is_interrupt_enabled
(
  uint32 gpio
);


/*==========================================================================

  FUNCTION      TRAMP_GPIO_IS_INTERRUPT_PENDING

  DESCRIPTION   Returns if the GPIO interrupt is pending.

  PARAMETERS    gpio - Which GPIO to check.

  DEPENDENCIES  None.

  RETURN VALUE  boolean - Whether or not the GPIO interrupt is pending.

  SIDE EFFECTS  None.

==========================================================================*/

boolean tramp_gpio_is_interrupt_pending
(
  uint32 gpio
);


/*==========================================================================

  FUNCTION      TRAMP_GPIO_SWITCH_TO_MODEM

  DESCRIPTION   Switches GPIO interrupt processing from the apps processor
                to the modem processor.
                On the modem processor loads the apps power collapse GPIO
                settings from shared memory and begin monitoring apps GPIOs.
                On the apps processor this function currently does nothing.

  PARAMETERS    isr - ISR to invoke when one of the monitored GPIOs
                      triggers.  The GPIO number will be passed as the
                      ISR parameter.
  DEPENDENCIES  Interrupts should be locked.  

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_gpio_switch_to_modem
(
  tramp_handler_type isr
);


/*==========================================================================

  FUNCTION      TRAMP_GPIO_SWITCH_TO_APPS

  DESCRIPTION   Switches apps GPIO interrupts back to the apps processor.
                On the modem processor this function stops monitoring
                apps GPIOs.
                On the apps processor, nothing need to be done because
                all gpio interrups were perserved by modem processor.

  PARAMETERS    None.

  DEPENDENCIES  Interrupts must be locked if the return value is used.

  RETURN VALUE  Not in use.

  SIDE EFFECTS  On the apps processor will trigger an interrupt if the
                modem indicated one was received during power collapse.

==========================================================================*/

uint32 tramp_gpio_switch_to_apps (void);


/*==========================================================================

  FUNCTION      TRAMP_GPIO_SAVE

  DESCRIPTION   Save GPIO interrupt settings before power collapse.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_gpio_save (void);


/*==========================================================================

  FUNCTION      TRAMP_GPIO_RESTORE

  DESCRIPTION   Restore GPIO interrupt settings before power collapse.

  PARAMETERS    None.

  DEPENDENCIES  tramp_gpio_save must have been called at some point.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_gpio_restore (void);

/*==========================================================================

  FUNCTION      TRAMP_GPIO_MAP_MPM_INTERRUPT

  DESCRIPTION   This function maps the given interrupt to the MPM interrupt
                type if it is a wakeup interrupt.

  PARAMETERS    uint32     gpio : The GPIO interrupt pin number.
                uint32     mpm_irq : The MPM id.
 
  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_gpio_map_mpm_interrupt
(
  uint32         gpio,
  uint32         mpm_irq
);

/*==========================================================================

  FUNCTION      TRAMP_GPIO_TRIGGER_INTERRUPT

  DESCRIPTION   This function can be used to manually trigger a gpio interrupt.

  PARAMETERS    uint32     gpio : The GPIO interrupt pin number to be triggered.
 
  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_gpio_trigger_interrupt
(
  uint32         gpio
);

#endif /* __TRAMP_GPIO_H__ */

