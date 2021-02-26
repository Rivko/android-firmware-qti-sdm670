/**
 * @file UsbfnLbSmImpl.h
 *
 * @brief State function declarations
 *
 * This file declares the internal implementation of the loopback state machine
 * state entry and transition functions.
 *
 * Copyright (c) 2014 by QUALCOMM Technologies, Inc. All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/06/14   ml      Initial implementation
 =============================================================================*/

#ifndef _USBFN_LB_SM_IMPL_H_
#define _USBFN_LB_SM_IMPL_H_


/*
 * State function naming convention: Ulb_State(s)_Event
 */


/*
 * State entry functions
 */
VOID Ulb_Detached_Entry     (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState);
VOID Ulb_Attached_Entry     (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState);
VOID Ulb_Configured_Entry   (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState);
VOID Ulb_Loopback_Entry     (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState);
VOID Ulb_Suspended_Entry    (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState);


/*
 * State transition functions
 */
ULB_STATE Ulb_EventNotHandled               (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_EventError                    (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Common_Reset                  (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Common_Suspend                (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Common_Detach                 (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Common_SetupPkt               (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Common_EpStatusChangedRx      (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Common_EpStatusChangedTx      (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Common_Speed                  (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Detached_Attach               (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Configured_SetupPkt           (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Loopback_EpStatusChangedRx    (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Loopback_EpStatusChangedTx    (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
ULB_STATE Ulb_Suspended_Resume              (IN ULB_STATE_MACHINE          *Ulb,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);

#endif /* _USBFN_LB_SM_IMPL_H_ */
