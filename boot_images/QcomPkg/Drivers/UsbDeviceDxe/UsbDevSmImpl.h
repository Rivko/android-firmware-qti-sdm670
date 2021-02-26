/**
 * @file UsbDevSmImpl.h
 *
 * @brief State function declarations
 *
 * This file declares the internal implementation of the state machine
 * state entry and transition functions.
 *
 * Copyright (c) 2014 by Qualcomm Technologies, Inc. All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/18/14   ck      Add comments and remove unused code
 08/02/14   ck      Initial port from loopback app
 =============================================================================*/

#ifndef _USB_DEV_SM_IMPL_H_
#define _USB_DEV_SM_IMPL_H_

/*
 * State function naming convention: Dev_State(s)_Event
 */


/*
 * State entry functions
 */
VOID Dev_Detached_Entry     (IN DEV_STATE_MACHINE *DevSm, IN DEV_STATE PrevState);
VOID Dev_Attached_Entry     (IN DEV_STATE_MACHINE *DevSm, IN DEV_STATE PrevState);
VOID Dev_Suspended_Entry    (IN DEV_STATE_MACHINE *DevSm, IN DEV_STATE PrevState);


/*
 * State transition functions
 */
DEV_STATE Dev_EventNotHandled               (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_EventError                    (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Common_Reset                  (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Common_Suspend                (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Common_Detach                 (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Common_SetupPkt               (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Common_EpStatusChangedRx      (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Common_EpStatusChangedTx      (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Common_Speed                  (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Detached_Attach               (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Configured_SetupPkt           (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Suspended_Resume              (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
#endif //_USB_DEV_SM_IMPL_H_
