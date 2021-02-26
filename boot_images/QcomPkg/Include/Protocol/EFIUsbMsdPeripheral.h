/**
  @file EFIUsbMsdPeripheral.h
  @brief MSD Peripheral Protocol definitions
*/
/*=============================================================================
  Copyright (c) 2011-2012, 2014 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/14/14   ck      Add Start and Stop Device functionality
 08/14/14   martinl Support for USB mass storager app 
 09/21/12    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 11/15/11   cching  Initial support of USB MSD

=============================================================================*/

#ifndef __EFI_USB_MSD_PERIPHERAL_H__
#define __EFI_USB_MSD_PERIPHERAL_H__

#include <Protocol/BlockIo.h>

/** @ingroup efi_usbMsdPeripheral_constants */
/** Protocol version. */
#define EFI_USB_MSD_PERIPHERAL_PROTOCOL_REVISION  0x00010001


/* Global ID for the USB MSD Peripheral Protocol

   {C8591FAF-DBCC-479E-9EF2-FD085BC37BC7}  */
/** @ingroup efi_usbMsdPeripheral_protocol */
#define EFI_USB_MSD_PERIPHERAL_PROTOCOL_GUID \
  { \
    0xc8591faf, 0xdbcc, 0x479e, {0x9e, 0xf2, 0xfd, 0x8, 0x5b, 0xc3,  0x7b, 0xc7} \
  }


/* Related Definitions */

/* Forward reference for pure ANSI compatibility. */
/** @cond */
typedef struct _EFI_USB_MSD_PERIPHERAL_PROTOCOL EFI_USB_MSD_PERIPHERAL_PROTOCOL;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_USB_MSD_ASSIGN_BLKIO_HANDLE */
/** @ingroup efi_usb_msd_assign_blkio_handle
  @par Summary
  Assigns and removes a BLKIO/LUN association.
  
  @param[in]  This         Pointer to the EFI_USB_MSD_PERIPHERAL_PROTOCOL 
                           instance.
  @param[in]  BlkIoHandle  Block IO handle to be associated with the LUN.
  @param[in]  LunOffset    LUN to be associated with BlkIoHandle. The first LUN 
                           is LUN 0.

  @description
  This function associates a block IO handle to an LUN. By default, all 
  LUNs are in the disabled state. Once the association has been established,
  the LUN appears in the host PC as a removable disk. Passing a NULL block IO 
  handle removes this association. Once the association has been removed, the 
  removable disk disappears from the host PC. After removing the association, 
  another block IO handle can be assigned to the LUN.
  @par
  If an LUN is currently associated with a block IO handle, assigning another 
  block IO handle to the same LUN results in an EFI_NOT_READY error.
  @par
  If a block IO handle is currently associated with an LUN, assigning the same 
  block IO handle to a different LUN results in an EFI_UNSUPPORTED error.
  @par
  If the LUN to be associated is greater than the maximum number of LUNs 
  supported, a EFI_INVALID_PARAMETER error is returned. 

  @return
  EFI_SUCCESS           -- Function completed successfully. 
  EFI_NOT_READY         -- LUN is currently in use and cannot be associated 
                           with another block IO handle. 
  EFI_UNSUPPORTED       -- Association of a block IO handle with two 
                           different LUNs is not supported. 
  EFI_INVALID_PARAMETER -- LUN offset is too large. 
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_MSD_ASSIGN_BLKIO_HANDLE)(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL *This,
  IN  EFI_BLOCK_IO_PROTOCOL           *BlkIoHandle,
  IN  UINT32                          LunOffset
);

/* EFI_USB_MSD_QUERY_MAX_LUN */
/** @ingroup efi_usb_msd_query_max_lun
  @par Summary
  Queries the maximum number of LUNs the driver can currently support.
  
  @param[in]   This             Pointer to the EFI_USB_MSD_PERIPHERAL_PROTOCOL 
                                instance.
  @param[out]  NumLunSupported  Number of LUNs supported by the driver.


  @return
  EFI_SUCCESS        -- Function completed successfully. 
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_MSD_QUERY_MAX_LUN)(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL *This,
  OUT UINT8                           *NumLunSupported
);

/* EFI_USB_MSD_EVENT_POLLING */
/** @ingroup efi_usb_msd_event_polling
  @par Summary
  The client application is responsible to call the polling function periodically
  so the driver can handle USB events.
  
  @param[in]   This             Pointer to the EFI_USB_MSD_PERIPHERAL_PROTOCOL 
                                instance.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_MSD_EVENT_HANDLER)(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL *This
);


/* EFI_USB_MSD_START_DEVICE */
/** @ingroup efi_usb_msd_start_devce
@par Summary
Start the mass storage device enumeration process

@param[in]   This             Pointer to the EFI_USB_MSD_PERIPHERAL_PROTOCOL instance.


@return
EFI_SUCCESS        -- Function completed successfully.
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_MSD_START_DEVICE)(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL *This
);

/* EFI_USB_MSD_STOP_DEVICE */
/** @ingroup efi_usb_msd_stop_devce
@par Summary
Stop the mass storage device so it appears to be removed
from host pc.

@param[in]   This             Pointer to the EFI_USB_MSD_PERIPHERAL_PROTOCOL instance.


@return
EFI_SUCCESS        -- Function completed successfully.
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_MSD_STOP_DEVICE)(
  IN  EFI_USB_MSD_PERIPHERAL_PROTOCOL *This
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_usbMsdPeripheral_protocol
  @par Summary
  USB Mass Storage Device (MSD) Peripheral Protocol interface.
  @par
  This protocol enables USB boot mass storage in a pre-boot environment.

  @par Parameters
  @inputprotoparams{usb_msd_peripheral_proto_params.tex} 
*/

struct _EFI_USB_MSD_PERIPHERAL_PROTOCOL {
  ///
  /// The revision to which the EFI_USB_MSD_PERIPHERAL_PROTOCOL adheres. All future
  /// revisions must be backwards compatible. If a future version is not
  /// backwards compatible, a different GUID must be used.
  ///
  UINT32                                    Revision;
  ///
  /// Assign and remove BLKIO / LUN association
  ///
  EFI_USB_MSD_ASSIGN_BLKIO_HANDLE           AssignBlkIoHandle;
  ///
  /// Query how many LUNs could the device supports
  ///
  EFI_USB_MSD_QUERY_MAX_LUN                 QueryMaxLun;
  ///
  /// USB Bus Query  Event
  /// Run this in a tight loop to achieve faster USB Speeds
  ///
  EFI_USB_MSD_EVENT_HANDLER                 EventHandler;
  ///
  /// Start Enumeration 
  ///
  EFI_USB_MSD_START_DEVICE                  StartDevice;
  ///
  /// Stop Enumeration 
  ///
  EFI_USB_MSD_STOP_DEVICE                   StopDevice;
};

/** @cond */
extern EFI_GUID gEfiUsbMsdPeripheralProtocolGuid;
/** @endcond */

#endif //__EFIUSBFNIO__
