/**
 * @file UsbMsdSmImpl.h
 *
 * @brief State function declarations
 *
 * This file declares the internal implementation of the state machine
 * state entry and transition functions.
 *
 * Copyright (c) 2014,2016, 2017 by Qualcomm Technologies, Inc. All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/01/17   pm      Port form Boot.xf.1.4, Fix control transfers issue in middle of bulk data transfers
 01/25/16   ck      Separate EDK2 undefined USB data structure to EFIUsbEx.h
 12/08/14   ck      Fix incorrect LunNum after unmount
 10/14/14   ck      Initial Version. Introduce state machine and remove global vars
 =============================================================================*/

#ifndef __USB_MSD_SM_IMPL_H__
#define __USB_MSD_SM_IMPL_H__

#include "UsbMsdSm.h"            // DEV_STATE_MACHINE

/*
 * State function naming convention: Dev_State(s)_Event
 */


/*
 * State entry functions
 */
VOID Dev_Detached_Entry        (IN DEV_STATE_MACHINE *DevSm, IN DEV_STATE PrevState);
VOID Dev_Attached_Entry        (IN DEV_STATE_MACHINE *DevSm, IN DEV_STATE PrevState);
VOID Dev_Suspended_Entry       (IN DEV_STATE_MACHINE *DevSm, IN DEV_STATE PrevState);
VOID Dev_CBW_Entry             (IN DEV_STATE_MACHINE *DevSm, IN DEV_STATE PrevState);

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
DEV_STATE Dev_Attached_EpStatusChangedRx    (IN DEV_STATE_MACHINE          *DevSm,
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
DEV_STATE Dev_Suspended_EpStatusChangedRx   (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_Suspended_EpStatusChangedTx   (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_CBW_EpStatusChangedRx         (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_CSW_EpStatusChangedTx         (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_EpStatusChangedTx             (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
DEV_STATE Dev_EpStatusChangedRx             (IN DEV_STATE_MACHINE          *DevSm,
                                             IN EFI_USBFN_MESSAGE          Message,
                                             IN UINTN                      PayloadSize,
                                             IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload);
#endif //__USB_MSD_SM_IMPL_H__
