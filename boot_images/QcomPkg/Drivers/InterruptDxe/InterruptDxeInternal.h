#ifndef INTERRUPT_DXE_INTERNAL_H
#define INTERRUPT_DXE_INTERNAL_H

/** @file InterruptDxeInternal.h
 
  Handle generic interrupt controller prototypes. 
  EnableInterruptSource
  RegisterInterruptSource
  InterruptDxeExitBootServicesEvent
  DisableInterruptSource
  SetInterruptSourceConfig
  GetInterruptSourceState
  EndOfInterrupt

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.
  
  Portions Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
  
**/

/*===========================================================================
                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
01/11/10   JZ      Created.

===========================================================================*/

#include <PiDxe.h>
#include <Protocol/HardwareInterrupt.h>


/**
  Interrupt Dxe Source configurations
  
  All possible interrupt configurations.
  INTERRUPT_DXE_SOURCE_CONFIG_TRIGGER_LEVEL : Level trigger
  INTERRUPT_DXE_SOURCE_CONFIG_TRIGGER_LEVEL : Edge trigger
**/
#define INTERRUPT_DXE_SOURCE_CONFIG_TRIGGER_LEVEL 0
#define INTERRUPT_DXE_SOURCE_CONFIG_TRIGGER_EDGE  1


/**
  Shutdown our hardware
  
  DXE Core will disable interrupts and turn off the timer and disable interrupts
  after all the event handlers have run.

  @param[in]  Event   The Event that is being processed
  @param[in]  Context Event Context
**/
VOID
EFIAPI
InterruptDxeExitBootServicesEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );


/**
  Register Handler for the specified interrupt source.

  @param This     Instance pointer for this protocol
  @param Source   Hardware source of the interrupt
  @param Handler  Callback for interrupt. NULL to unregister

  @retval EFI_SUCCESS Source was updated to support Handler.
  @retval EFI_DEVICE_ERROR  Hardware could not be programmed.

**/
EFI_STATUS
EFIAPI
RegisterInterruptSource (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source,
  IN HARDWARE_INTERRUPT_HANDLER         Handler
  );


/**
  Enable interrupt source Source.

  @param This     Instance pointer for this protocol
  @param Source   Hardware source of the interrupt

  @retval EFI_SUCCESS       Source interrupt enabled.
  @retval EFI_DEVICE_ERROR  Hardware could not be programmed.

**/
EFI_STATUS
EFIAPI
EnableInterruptSource (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source
  );


/**
  Disable interrupt source Source.

  @param This     Instance pointer for this protocol
  @param Source   Hardware source of the interrupt

  @retval EFI_SUCCESS       Source interrupt disabled.
  @retval EFI_DEVICE_ERROR  Hardware could not be programmed.

**/
EFI_STATUS
EFIAPI
DisableInterruptSource (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source
  );


/**
  Set interrupt source Source Configuration.

  @param This     Instance pointer for this protocol
  @param Source   Hardware source of the interrupt
  @param Config   Interrupt Configuration

  @retval EFI_SUCCESS       Source interrupt configured.
  @retval EFI_DEVICE_ERROR  Hardware could not be programmed.

**/
EFI_STATUS
EFIAPI
SetInterruptSourceConfig (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source,
  IN uint32                             Config
  );


/**
  Return current state of interrupt source Source.

  @param This     Instance pointer for this protocol
  @param Source   Hardware source of the interrupt
  @param InterruptState  TRUE: source enabled, FALSE: source disabled.

  @retval EFI_SUCCESS       InterruptState is valid
  @retval EFI_DEVICE_ERROR  InterruptState is not valid

**/
EFI_STATUS
EFIAPI
GetInterruptSourceState (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source,
  IN BOOLEAN                            *InterruptState
  );


/**
  Signal to the hardware that the End Of Intrrupt state 
  has been reached.

  @param This     Instance pointer for this protocol
  @param Source   Hardware source of the interrupt

  @retval EFI_SUCCESS       Source interrupt EOI'ed.
  @retval EFI_DEVICE_ERROR  Hardware could not be programmed.

**/
EFI_STATUS
EFIAPI
EndOfInterrupt (
  IN EFI_HARDWARE_INTERRUPT_PROTOCOL    *This,
  IN HARDWARE_INTERRUPT_SOURCE          Source
  );

#endif /* INTERRUPT_DXE_INTERNAL_H */

