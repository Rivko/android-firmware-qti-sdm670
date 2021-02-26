/** @file InterruptGICDxe.c
   
  Qualcomm implmementation of Generic Interrupt Controller
  for UEFI interrupt protocol.

  Copyright (c) 2010-2011,2013 - 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  Portions Copyright (c) 2008-2009 Apple Inc. All rights reserved.<BR>
  All rights reserved. This program and the accompanying materials 
  are licensed and made available under the terms and conditions of the BSD License 
  which accompanies this distribution. The full text of the license may be found at 
  http://opensource.org/licenses/bsd-license.php 
  
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

**/


/*===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
02/05/14   amitg    Async USB Host Stack
11/22/13   vk      Update registration macro - supports AARCH32 and AARCH64
09/18/13   an      Fix kw warning
01/17/13   vk      Fix warning
04/23/12   an      Interrupt handler fix
01/11/10   JZ      Created.

===========================================================================*/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/ArmLib.h>

#include <Protocol/Cpu.h>
#include <Library/HALqgic.h>

#include "InterruptDxeInternal.h"

/* -----------------------------------------------------------------------
**                           Definition
** ----------------------------------------------------------------------- */

/*
*  Obtains interrupt service routines.
*/
HARDWARE_INTERRUPT_HANDLER  gRegisteredInterruptHandlers[HAL_QGIC_IRQ_MAX];

/*
* Making this global saves a few bytes in image size
*/
EFI_HANDLE  gHardwareInterruptHandle = NULL;

/*
* The protocol instance produced by this driver
*/
EFI_HARDWARE_INTERRUPT_PROTOCOL gHardwareInterruptProtocol = {
  RegisterInterruptSource,
  EnableInterruptSource,
  DisableInterruptSource,
  GetInterruptSourceState,
  EndOfInterrupt
};


/*
 The Qcom protocol instance produced by this driver
 Note:  Since there aren't many interrupts to be initialized
        in UEFI, we can do SetInterruptSourceConfig during
        interrupt init.
        Eventually QCT UEFI team will request UEFI committee
        to add a config API as part of HARDWARE_INTERRUPT_PROTOCOL.
*/
/*
EFI_QCOM_HARDWARE_INTERRUPT_PROTOCOL gQcomHardwareInterruptProtocol = {
  QCOM_HARDWARE_INTERRUPT_PROTOCOL_REVISION,
  RegisterInterruptSource,
  EnableInterruptSource,
  DisableInterruptSource,
  GetInterruptSourceState,
  EndOfInterrupt,
  SetInterruptSourceConfig
};
*/


/*
* Notifications
*/
VOID      *CpuProtocolNotificationToken = NULL;
EFI_EVENT CpuProtocolNotificationEvent  = (EFI_EVENT)NULL;
EFI_EVENT EfiExitBootServicesEvent      = (EFI_EVENT)NULL;


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


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
  )
{
  HAL_qgic_Reset();
}


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
  )
{
  if (Source >= HAL_QGIC_IRQ_MAX) {
    ASSERT(FALSE);
    return EFI_UNSUPPORTED;
  } 

  if ((Handler == NULL) && (gRegisteredInterruptHandlers[Source] == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Handler != NULL) && (gRegisteredInterruptHandlers[Source] != NULL)) {
    return EFI_ALREADY_STARTED;
  }

  gRegisteredInterruptHandlers[Source] = Handler;
  HAL_qgic_Enable(Source);
  HAL_qgic_SetTargets(Source,1);
  return EFI_SUCCESS;
}


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
  )
{
  /*
   * Valid QGIC IRQ
   */
  if (Source >= HAL_QGIC_IRQ_MAX) {
    ASSERT(FALSE);
    return EFI_UNSUPPORTED;
  } 

  /*
   * Enable QGIC IRQ
   */
  HAL_qgic_Enable(Source);
  
  return EFI_SUCCESS;
}


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
  )
{
  /*
   * Valid QGIC IRQ
   */
  if (Source >= HAL_QGIC_IRQ_MAX) {
    ASSERT(FALSE);
    return EFI_UNSUPPORTED;
  } 

  /*
   * Disable QGIC IRQ
   */
  HAL_qgic_Disable(Source);
  
  return EFI_SUCCESS;
}


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
  )
{
  HAL_qgic_TriggerType trigger;

  /*
   * Valid QGIC IRQ
  */
  if (Source >= HAL_QGIC_IRQ_MAX) {
    ASSERT(FALSE);
    return EFI_UNSUPPORTED;
  } 

  if (Config == INTERRUPT_DXE_SOURCE_CONFIG_TRIGGER_LEVEL)
  {
    trigger = HAL_QGIC_TRIGGER_LEVEL;
  }
  else
  {
    trigger = HAL_QGIC_TRIGGER_EDGE;
  }

  HAL_qgic_SetTrigger(Source, trigger);

  return EFI_SUCCESS;
}


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
  )
{

  /*
   * Valid QGIC IRQ
   */
  if (Source >= HAL_QGIC_IRQ_MAX) {
    ASSERT(FALSE);
    return EFI_UNSUPPORTED;
  } 

  *InterruptState = HAL_qgic_IsPending(Source); 
  return EFI_SUCCESS;
}


/**
  Signal to the hardware that the End Of Interrupt state 
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
  )
{

  /*
   * Valid QGIC IRQ
   */
  if (Source >= HAL_QGIC_IRQ_MAX) {
    ASSERT(FALSE);
    return EFI_UNSUPPORTED;
  } 

  HAL_qgic_EndOfInterrupt(Source, 0); 
  
  return EFI_SUCCESS;
}


/**
  EFI_CPU_INTERRUPT_HANDLER that is called when a processor interrupt occurs.

  @param  InterruptType    Defines the type of interrupt or exception that
                           occurred on the processor.This parameter 
                           is processor architecture specific.
  @param  SystemContext    A pointer to the processor context when
                           the interrupt occurred on the processor.

  @return None

**/
VOID
EFIAPI
IrqInterruptHandler (
  IN EFI_EXCEPTION_TYPE           InterruptType,
  IN EFI_SYSTEM_CONTEXT           SystemContext
  )
{
  HAL_qgic_InterruptType intId;
  HAL_qgic_CPUIdType     CPUId;
  HARDWARE_INTERRUPT_HANDLER InterruptHandler;
  EFI_TPL OriginalTPL;

  OriginalTPL = gBS->RaiseTPL (TPL_HIGH_LEVEL);

  HAL_qgic_AckInterrupt(&intId, &CPUId);

  /*
  * we're only supporting CPU0
  */
  if (CPUId != 0)
  {
    ASSERT(FALSE);
  }
  
  if(intId >= HAL_QGIC_IRQ_MAX) // Bad Interrupt will need to be removed once we root cause the problem with spurious interrupts
  {
    return ;
  }
  

  InterruptHandler = gRegisteredInterruptHandlers[intId];
  if (InterruptHandler != NULL) {
    /*
    * Call the registered interrupt handler.
    */
    InterruptHandler (intId, SystemContext);
  } else {
    DEBUG ((EFI_D_ERROR, "Spurious interrupt: %d\n", intId));
  }
  /*
  *  Clear after running the handler
  */
  HAL_qgic_EndOfInterrupt (intId, CPUId);
  
  gBS->RestoreTPL (OriginalTPL);
}


/**
  Notification routines - Setup for Exeption vector to service
                          Interrupt Vector
  @param  Event     Not used 
  @param  Context   Not used 
   
  @return None

**/
VOID
CpuProtocolInstalledNotification (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS              Status;
  EFI_CPU_ARCH_PROTOCOL   *Cpu;
  
  /*
  * Get the cpu protocol that this driver requires.
  */
  Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **)&Cpu);
  ASSERT_EFI_ERROR(Status);

  /*
  * Unregister the default exception handler.
  */
  Status = Cpu->RegisterInterruptHandler (Cpu, ARM_ARCH_EXCEPTION_IRQ, NULL);
  ASSERT_EFI_ERROR(Status);

  /*
  * Register to receive interrupts
  */
  Status = Cpu->RegisterInterruptHandler (Cpu, ARM_ARCH_EXCEPTION_IRQ, IrqInterruptHandler);
  ASSERT_EFI_ERROR(Status);
}


/**
  Initialize the state information for the Hardware interrupt Protocol
  and also setup for GIC specific hardware.

  @param  ImageHandle   of the loaded driver
  @param  SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS           Protocol registered
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      Hardware problems

**/
EFI_STATUS
InterruptDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  HAL_qgic_BaseAddressType qgicBaseAddrs;
  HARDWARE_INTERRUPT_SOURCE interruptNum;
  UINT32 Config = INTERRUPT_DXE_SOURCE_CONFIG_TRIGGER_EDGE;
  
  /*
  * Make sure the Interrupt Controller Protocol is not already installed in the system.
  */
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gHardwareInterruptProtocolGuid);

  /*
   * Setup HAL QGIC
   */
  qgicBaseAddrs.nGICDBaseAddress = PcdGet32(PcdInterruptBaseAddress);
  qgicBaseAddrs.nGICCBaseAddress = PcdGet32(PcdQGICCBaseAddress);
  
  HAL_qgic_Init(&qgicBaseAddrs);

  /*
  * Reset QGIC Interrupt HW
  */
  HAL_qgic_Reset();
  
  /*
  * No Preemption
  */
  HAL_qgic_SetBinaryPoint(HAL_QGIC_BINARYPOINT_NO_PREEMPTION);

  /*
  * Set lowest priority level to 254
  */
  HAL_qgic_SetPriorityMask(0xFE);

  /*
  * Enable Distributor
  */
  HAL_qgic_EnableDistributor();

  /*
  * Enable Interrupts
  */
  HAL_qgic_EnableInterrupts();

  /*
  * UEFI specific init 
  */
  Status = gBS->InstallMultipleProtocolInterfaces(&gHardwareInterruptHandle,
                                                  &gHardwareInterruptProtocolGuid, &gHardwareInterruptProtocol,
                                                  NULL);
  ASSERT_EFI_ERROR(Status);
  
  /*
  * Set up to be notified when the Cpu protocol is installed.
  */
  Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, CpuProtocolInstalledNotification, NULL, &CpuProtocolNotificationEvent);    
  ASSERT_EFI_ERROR(Status);

  Status = gBS->RegisterProtocolNotify(&gEfiCpuArchProtocolGuid, CpuProtocolNotificationEvent, (VOID *)&CpuProtocolNotificationToken);
  ASSERT_EFI_ERROR(Status);

  /*
  * Register for an ExitBootServicesEvent
  */
  Status = gBS->CreateEvent(EVT_SIGNAL_EXIT_BOOT_SERVICES, TPL_CALLBACK, InterruptDxeExitBootServicesEvent, NULL, &EfiExitBootServicesEvent);
  ASSERT_EFI_ERROR(Status);

  /*
  * Configure interrupts
  */

  /*
  * Timer 1
  */
  interruptNum = FixedPcdGet32(PcdTimer1IRQ);
  if(PcdGet32(PcdQTimerEnabled))
  {
    Config = INTERRUPT_DXE_SOURCE_CONFIG_TRIGGER_LEVEL;
  }
  Status = SetInterruptSourceConfig (
             &gHardwareInterruptProtocol, 
              interruptNum, 
              Config);
  ASSERT_EFI_ERROR (Status);

  return Status;
} /* InterruptDxeInitialize */
