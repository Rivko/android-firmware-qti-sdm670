/** @file UsbMsd.h

  The header file for UsbMsd

  Copyright (c) 2010-2011,2014,2016 QUALCOMM Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/25/16   ck      Separate EDK2 undefined USB data structure to EFIUsbEx.h
 10/14/14   ck      Add state machine and remove global vars
 04/30/14   ar      Cleanup for NDEBUG build  
 06/24/11   ck      Port Usbfn to UEFI
=============================================================================*/

#ifndef __USB_MSD_H__
#define __USB_MSD_H__

#include <Protocol/EFIUsbMsdPeripheral.h>   // usb msd protocol handle
#include <Protocol/EFIUsbfnIo.h>            // USbfnIo protocol handle
#include <Protocol/BlockIo.h>               // BlockIo protocol handle
#include <Protocol/EFIUsbEx.h>              // USB data structure
#include "UsbMsdSm.h"                       // StateMachine

/* default transfer buffer size */
#define USBDEV_CONTROL_BUF_SIZE              (16 * 1024)
#define USBDEV_DEFAULT_BULK_BUF_SIZE         (64 * 1024)
 
/* macros to retrieve pointer to USBMSD_DEV from UsbMsdPrototol and StateMachine*/
#define USBMSD_DEV_SIGNATURE         SIGNATURE_32 ('u', 's', 'b', 'm')
#define USBMSD_FROM_THIS(x)          CR(x, USBMSD_DEV, UsbMsdProtocol, USBMSD_DEV_SIGNATURE)
#define USBMSD_FROM_SM(x)            CR(x, USBMSD_DEV, StateMachine, USBMSD_DEV_SIGNATURE)

/* MAX_SUPPORTED_LUN defines the max number of LUNs supported in the driver. */
#define USB_MSD_MAX_SUPPORTED_LUN    16


/**
  @brief Properties of a LUN
 */
typedef struct _LUN_PROPERTY LUN_PROPERTY;
struct _LUN_PROPERTY {
  UINT32 Enabled        :1;
  UINT32 Removable      :1;
  UINT32 NeedAttention  :1;
  UINT32 ReadOnly       :1;
};


/**
  @brief Client READ ONLY structure to provide current USB MSD configuration
 */
typedef struct _LUN_INFO_TYPE LUN_INFO_TYPE;
struct _LUN_INFO_TYPE {
  /* LUN index */
  UINT8                             Index;

  LUN_PROPERTY                      Property;

  /* For the block device information */
  UINT32                            BlockSize;

  /* Number of blocks supported by the Block IO device */
  UINT32                            NumBlocks;

};


/**
  @brief Bookkeeping structure to track the LUN - BLKIO handle association
 */
typedef struct _USBMSD_LUN_ASSOCIATION USBMSD_LUN_ASSOCIATION;
struct _USBMSD_LUN_ASSOCIATION {
  EFI_BLOCK_IO_PROTOCOL *BlkIoHandle;
  LUN_INFO_TYPE          Lun;
};


/**
  @brief Client READ ONLY structure to provide current USB MSD configuration
 */
typedef struct {

  ///
  /// Number of active LUNs currently supported
  ///
  UINTN                                NumLuns;

  ///
  /// The association between a BlkIo handle and a LUN
  ///
  USBMSD_LUN_ASSOCIATION               BlkIoLunMap[USB_MSD_MAX_SUPPORTED_LUN];
} EFI_USB_MSD_MEDIA;

/* Usb Msd Device data structure */
typedef struct _USBMSD_DEV USBMSD_DEV;
struct _USBMSD_DEV {
  UINTN                                Signature;
  EFI_USB_MSD_PERIPHERAL_PROTOCOL      UsbMsdProtocol;
  EFI_USB_MSD_MEDIA                    UsbMsdMedia;
  EFI_EVENT                            ExitBootServiceEvent;
  EFI_USBFN_IO_PROTOCOL               *UsbfnIoProtocol;
  DEV_STATE_MACHINE                    StateMachine;
  BOOLEAN                              DeviceStarted;
};
#endif /* __USB_MSD_H__ */
