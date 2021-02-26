/**
 * @file UsbfnDwc3Util.h
 *
 * @brief Utility macros and functions for debug messages and status checking.
 *
 * @copyright Copyright (C) 2013-2014, 2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/09/17   ck      Work around the issue that LPM Not Rejected During Control Transfer
 02/01/17   pm      Port form Boot.xf.1.4
 11/20/14   ml      Migrate to official SuperSpeed support
 08/23/13   mliss   Initial rework for Synopsys Usbfn driver
 =============================================================================*/
#ifndef USBFNDWC3UTIL_H_
#define USBFNDWC3UTIL_H_


#include "UsbfnDwc3Impl.h"


// Tag all output with the calling function name, terminate with a newline, and
// eliminate the need for double parens with a variadic macro.
#define _DBG(EfiD, Expression, ...)	DEBUG((EfiD, Expression, ##__VA_ARGS__))
#define DBG(EfiD, Str, ...) \
  _DBG (EfiD, "%a: " Str "\n", __FUNCTION__, ##__VA_ARGS__);

// Cannot continue, exit function.
#define DWC_ERR_CHK(ErrStr, ...) \
  do { \
    if (DWC_ERROR (DwcStatus)) { \
      DBG (EFI_D_ERROR, ErrStr, ##__VA_ARGS__); \
      goto ON_EXIT; \
    } \
  } while (0)

// Highly undesirable, but continue anyway.
#define DWC_WRN_CHK(WrnStr, ...) \
  do { \
    if (DWC_ERROR (DwcStatus)) { \
      DBG (EFI_D_WARN, WrnStr, ##__VA_ARGS__); \
    } \
  } while (0)

// Cannot continue, exit function.
#define ERR_CHK(ErrStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_ERROR, ErrStr, ##__VA_ARGS__); \
      goto ON_EXIT; \
    } \
  } while (0)

// Highly undesirable, but continue anyway.
#define WRN_CHK(WrnStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_WARN, WrnStr, ##__VA_ARGS__); \
    } \
  } while (0)

// Combine two 32 bit unsinged integer to UINTN.
// In 64 bit environment, it results in UINT64
// In 32 bit envrionment, it results in UINT32, and the value stored in HI
// will be ignored.
#define FORM_UINTN_FROM_2_UINT32(HI, LOW) ((UINTN) ((((UINT64) (HI)) << (32)) | ((UINT32) (LOW))))


// USB helper macros
#define USB_EP_FROM_ADDR(addr)      ((addr) & 0xf)
#define USB_EPDIR_FROM_ADDR(addr)   ((addr) >> 7)
#define USB_EPDIR_TO_ADDR(ep,dir)   ((dir) << 7 | (ep))
#define USB_EPTYPE_FROM_ATTR(attr)  ((attr) & 3)


/**
 * @brief Print the bus speed in text
 *
 * Print the bus speed in text
 *
 * @param [in]  Spd         Bus speed
 *
 * @return Bus speed in text
 */
CHAR8 *
PrintBusSpeed(
  IN  DWC_DEVICE_CONNECT_SPEED const Spd
  );


/**
 * @brief Print the device event type in text
 *
 * Print the device event type in text
 *
 * @param [in]  EvtType   Device event type
 *
 * @return Device event type in text
 */
CHAR8 *
PrintDevEvtType(
  IN UINT32 EvtType
  );


/**
 * @brief Print endpoint command type
 *
 * Print endpoint command type in text
 *
 * @param [in]  EpCommand endpoint command to be printed in text
 *
 * @return endpoint command in text
 */
CHAR8 *
PrintEpCommandType(
  IN UINT32   EpCommand
  );


/**
 * @brief Print the endpoint event type in text
 *
 * Print the endpoint event type in text
 *
 * @param [in]  EvtType       Endpoint event type
 *
 * @return endpoint event type in text
 */
CHAR8 *
PrintEpEvtType(
  IN UINT32 EvtType
  );


/**
 * @brief Print the LINK state in text
 *
 * Print the LINK state in text
 *
 * @param [in]  LinkState   LINK state
 *
 * @return Link state in text
 */
CHAR8 *
PrintLinkState(
  IN BOOLEAN            IsSuperSpeedEvt,
  IN DWC_USB_LINK_STATE LinkState
  );


#endif /* USBFNDWC3UTIL_H_ */
