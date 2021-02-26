/**
@file EFIUsbInitProtocol.h
@brief EFIUsbInitProtocol starts the hardware initialization for USB ports

The USB hardware initialization can take some substantial time in the boot
process and may affect the PMI charger detection. In order to address the
issue, USB hardware initialization can started on demand. The protocol provide
the on demand start/stop of USB controller.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*/
/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Incorporated.
All rights reserved.
Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
EDIT HISTORY

when       who      what, where, why
--------   ---      ----------------------------------------------------------

=============================================================================*/

#ifndef _EFI_USB_INIT_PROTOCOL_H_
#define _EFI_USB_INIT_PROTOCOL_H_

/** @ingroup usb_init_protocol */
#define EFI_USB_INIT_PROTOCOL_GUID \
  { \
    0x2da15ac7, 0x4909, 0x4c4c,{ 0xb8, 0xc3, 0xa7, 0x42, 0x19, 0x5e, 0xa4, 0x6e } \
  }

#define EFI_USB_INIT_PROTOCOL_REVISION 0x00010000

/** @cond */
/* Forward reference for pure ANSI computability */
typedef struct _EFI_USB_INIT_PROTOCOL EFI_USB_INIT_PROTOCOL;
/** @endcond */

/** @ingroup usb_port_config_data_types
  USB mode for the port, the mode type can be ORed */
typedef enum {
  EFI_USB_INVALID_MODE = 0x00,  /**< invalid mode */
  EFI_USB_HOST_MODE = 0x01,  /**< host mode */
  EFI_USB_PERIPHERAL_MODE = 0x02,  /**< peripheral mode */
  EFI_USB_DUAL_ROLE_MODE = 0x04,  /**< dual role mode */
} EFI_USB_MODE_TYPE;

/** @ingroup usb_port_config_data_types
  USB port information */
typedef struct _USB_CORE_INFO {
  UINT8                          CoreNum;
  EFI_USB_MODE_TYPE              ModeType;
} USB_CORE_INFO, *PUSB_CORE_INFO;

/* Protocol definitions */

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_USB_INIT_PROTOCOL_QUERY_CORE_COUNT */
/** @ingroup usb_init_query_max_core
@par Summary
Retrieve how many cores are supported by the system

@param[in]   This             Pointer to the EFI_USB_INIT_PROTOCOL instance.
@param[out]  Count            Pointer to the number of cores supported in the system

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_INVALID_PARAMETER -- NULL parameter was passed. \n
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_INIT_PROTOCOL_QUERY_CORE_COUNT)(
  IN      EFI_USB_INIT_PROTOCOL *This,
  OUT     UINT8                 *Count
  );

/* EFI_USB_INIT_PROTOCOL_QUERY_CORES_INFO */
/** @ingroup usb_init_query_core_info
@par Summary
Retrieves the information of the USB cores in the system.

If the NumOfCore pointer is NULL, the information of the default USB core in the
system will be retrieved. The client can then configure the default USB core
with the CoreNum in the information structure.

The default USB core is system dependent, and the client should not assume the
default USB core number remains the same from one system to another.

@param[in]  This             Pointer to the EFI_USB_INIT_PROTOCOL instance
@param[in]  CoreNum          Pointer to the USB core whose information to be retrieved
                             If the pointer is NULL, the default USB core information
                             is retrieved.
@param[out] CoreInfo         Pointer to the USB_CORE_INFO instance associated with the
                             USB core specified by CoreNum

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_INVALID_PARAMETER -- NULL parameter was passed. \n
EFI_UNSUPPORTED       -- The API is not supported in the system. \n
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_INIT_PROTOCOL_QUERY_CORES_INFO)(
  IN      EFI_USB_INIT_PROTOCOL *This,
  IN      UINT8                 *CoreNum, OPTIONAL
  OUT     PUSB_CORE_INFO        CoreInfo
  );

/* EFI_USB_INIT_PROTOCOL_START_CONTROLLER */
/** @ingroup usb_init_cfg_start_controller
@par Summary
Configure an USB core and specify the default USB mode to be instantiated.

USB_PORT_DEFAULT can be passed in for CoreNum and the very first core in
the system will be configured.

@param[in]  This           Pointer to the EFI_USB_INIT_PROTOCOL instance.
@param[in]  CoreNum        USB core in the system to be configured.
@param[in]  ModeType       The USB mode to be enabled.

@return
EFI_SUCCESS           -- Function completed successfully.
@par
EFI_INVALID_PARAMETER -- Attributes parameter is not valid.
@par
EFI_UNSUPPORTED       -- The USB mode cannot be supported on the core.
@par
EFI_DEVICE_ERROR      -- Physical device reported an error.
*/

typedef
EFI_STATUS
(EFIAPI *EFI_USB_INIT_PROTOCOL_START_CONTROLLER)(
  IN EFI_USB_INIT_PROTOCOL   *This,
  IN UINT8                   CoreNum,
  IN EFI_USB_MODE_TYPE       ModeType
  );

/* EFI_USB_INIT_PROTOCOL_STOP_CONTROLLER */
/** @ingroup usb_init_stop_controller
@par Summary
Stop an USB core and unload the associated USB driver stack.

@param[in]  This           Pointer to the EFI_USB_INIT_PROTOCOL instance.
@param[in]  CoreNum        USB core number in the system to be stopped.
@param[in]  ModeType       Current USB mode running on the core

@return
EFI_SUCCESS           -- Function completed successfully.
@par
EFI_INVALID_PARAMETER -- Attributes parameter is not valid.
@par
EFI_DEVICE_ERROR      -- Physical device reported an error.
*/

typedef
EFI_STATUS
(EFIAPI *EFI_USB_INIT_PROTOCOL_STOP_CONTROLLER)(
  IN EFI_USB_INIT_PROTOCOL   *This,
  IN UINT8                   CoreNum,
  IN EFI_USB_MODE_TYPE       ModeType
  );

/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup usb_init_protocol
@par Summary
This protocol provides the client driver the ability to enable the USB
hardware on demand and loaded the needed USB stack.
*/
struct _EFI_USB_INIT_PROTOCOL {
  UINT64                                       Revision;
  EFI_USB_INIT_PROTOCOL_QUERY_CORE_COUNT       QueryCoreCount;
  EFI_USB_INIT_PROTOCOL_QUERY_CORES_INFO       QueryCoreInfo;
  EFI_USB_INIT_PROTOCOL_START_CONTROLLER       StartController;
  EFI_USB_INIT_PROTOCOL_STOP_CONTROLLER        StopController;
};

/** @cond */
extern EFI_GUID gEfiUsbInitProtocolGuid;
/** @endcond */

#endif
