/**
 * @file DeviceInfo.h
 *
 * @brief USB 3.0 descriptor upgrade
 *
 * This file declares functions which can be used to upgrade a 2.0 only set of
 * USB descriptors to support SuperSpeed enumeration. They can be used to allow
 * support for USB 3.0 enumeration without explicit support by a client driver.
 *
 * Copyright (c) 2013-2014, Qualcomm Technologies, Inc. All rights reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/20/14   ml      Migrate to official SuperSpeed support
 08/23/13   mliss   Initial version for Synopsys Usbfn driver
 =============================================================================*/
#ifndef _DEVICE_INFO_H_
#define _DEVICE_INFO_H_

#include "UsbfnDwc3.h"


/**
 * @brief Adds USB 3.0 support to a USB 2.0 descriptor set.
 *
 * Using the 2.0 descriptor set as a base, fabricates a 3.0 compatible
 * descriptor set to allow enumeration as a SuperSpeed device without
 * explicit support from the client driver.
 *
 * Note: The descriptors pointed to by the info structures are laid out
 * consecutively in memory, guaranteeing the ability to return the 
 * configuration descriptor (followed by interface and endpoint descriptors)
 * directly to the host, i.e. the interface and endpoint descriptors follow
 * the config descriptor such that config.TotalLength bytes can be accessed
 * starting at the config descriptor.
 *
 * Upgrading the descriptor set, as done by this function, is the first step in
 * allowing the device to be enumerated as SS. Under normal operation of the
 * Usbfn protocol, it is the sole responsibility of the client driver to return
 * the appropriate descriptors to the host. To enumerate a client that knows
 * only of HS as SS, these descriptor requests must be "hijacked" by the
 * function driver and replaced with compatible SS descriptors. The compatible
 * SS descriptors are provided by this function with appropriate SS features
 * enabled (e.x. bursting on bulk endpoints). The hijacking logic is split
 * between the functions that would normally send requests and status to and
 * receive data from the client.
 *
 * There are two types of descriptor hijacking that must occur to successfully
 * enumerate as a SS device.
 *  1) Override the data stage of a request that the client expects, but would
 *     return inapropriate data for (device and config descriptors). This is
 *     accomplished by noting the fact that the data stage reuested by the
 *     client is invalid, and substituting it with our own descriptor in the
 *     transfer function. The status stage proceeds as usual, with the client
 *     thinking its supplied descriptor was accepted.
 *  2) Respond in the client's place for SS only requests (BOS descriptor,
 *     SET_SEL request, etc.). This is accomplished by failing to notify the
 *     client of the request's arrival and submitting the data and status stages
 *     neccessary to complete the control transfer immediately. In this case,
 *     all transfer related events that would normally be generated for the
 *     client must not be sent since the client never knew of the request in
 *     the first place.
 *
 * @param [in]  Usbfn       Usbfn device instance pointer
 * @param [in]  DevInfo2    USB 2.0 descriptor set
 * @param [out] DevInfo3    USB 3.0 descriptor set
 *
 * @retval EFI_SUCCESS      New descriptor set is valid
 * @retval others           Failed to update descriptor set
 */
EFI_STATUS
UpdateDeviceInfo(
  IN  USBFN_DEV                       *Usbfn,
  IN  EFI_USB_DEVICE_INFO             *DevInfo2,
  OUT EFI_USB_SUPERSPEED_DEVICE_INFO  **DevInfo3
  );


/**
 * @brief Frees a USB 3.0 descriptor set returned by UpdateDeviceInfo
 *
 * @param [in]  DevInfo3    USB 3.0 descriptor set to free
 */
VOID
FreeDeviceInfo(
  IN  EFI_USB_SUPERSPEED_DEVICE_INFO  *DevInfo3
  );


/**
 * @brief Validate device info structures
 *
 * Validates the structure tree(s) in a DeviceInfo and, if a SS version is
 * supplied, verifies assumptions about the common data between the SuperSpeed
 * and non-SuperSpeed descriptors.
 *
 * @param [in]     DevInfo    USB 2.0 descriptor set
 * @param [in,opt] SSDevInfo  USB 3.0 descriptor set
 *
 * @retval EFI_SUCCESS        Device Info structure(s) valid
 * @retval others             Invalid data found
 */
EFI_STATUS
ValidateDeviceInfo(
  IN  EFI_USB_DEVICE_INFO             *DevInfo,
  IN  EFI_USB_SUPERSPEED_DEVICE_INFO  *SSDevInfo OPTIONAL
  );


#endif /* _DEVICE_INFO_H_ */
