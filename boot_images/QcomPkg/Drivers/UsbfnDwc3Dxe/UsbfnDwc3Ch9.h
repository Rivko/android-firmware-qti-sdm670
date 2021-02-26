/**
 * @file UsbfnDwc3Ch9.h
 *
 * @brief Chapter 9 functionality
 *
 * @copyright Copyright (C) 2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*=============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/20/14   ml      Migrate to official SuperSpeed support
 06/10/13   mliss   Initial rework for Synopsys Usbfn driver
 =============================================================================*/

#ifndef _USBFN_DWC3_CH9_H_
#define _USBFN_DWC3_CH9_H_

#include "UsbfnDwc3.h"


/** transfer direction
 *  Bit 7
 */
#define USB_SETUP_RT_DIR_POS    (7)     /**< Position */
#define USB_SETUP_RT_DIR_SEED   (0x01)  /**< Seed */
#define USB_SETUP_RT_DIR_MASK   (USB_SETUP_RT_DIR_SEED << USB_SETUP_RT_DIR_POS) /**< Mask */
#define USB_SETUP_RT_DIR_H2D    (0)     /**< Host to Device */
#define USB_SETUP_RT_DIR_D2H    (1)     /**< Device to Host */

/** Type
 *  Bits 6:5
 */
#define USB_SETUP_RT_TYPE_POS   (5)    /**< Position */
#define USB_SETUP_RT_TYPE_SEED  (0x03) /**< Seed */
#define USB_SETUP_RT_TYPE_MASK  (USB_SETUP_RT_TYPE_SEED << USB_SETUP_RT_TYPE_POS) /**< Mask */
#define USB_SETUP_RT_TYPE_STD   (0)    /**< Standard */
#define USB_SETUP_RT_TYPE_CLS   (1)    /**< Class */
#define USB_SETUP_RT_TYPE_VND   (2)    /**< Vendor */
#define USB_SETUP_RT_TYPE_RSVD  (3)    /**< Reservrd */

/** Recipient
 *  Bits 4:0
 */
#define USB_SETUP_RT_RCP_POS    (0)     /**< Position */
#define USB_SETUP_RT_RCP_SEED   (0x1f)  /**< Seed */
#define USB_SETUP_RT_RCP_MASK   (USB_SETUP_RT_RCP_SEED << USB_SETUP_RT_RCP_POS) /**< Mask */
#define USB_SETUP_RT_RCP_DEV    (0)     /**< Device */
#define USB_SETUP_RT_RCP_IFC    (1)     /**< Interface */
#define USB_SETUP_RT_RCP_EP     (2)     /**< Endpoint */
#define USB_SETUP_RT_RCP_OTH    (3)     /**< Other */

#define USB_VENDOR_ID_QUALCOMM  (0x05C6)
#define USB_EP_ADDR_OUT         (0)
#define USB_EP_ADDR_IN          (0x80)
#define USB_EP_ATTR_CNTRL       (0)
#define USB_EP_ATTR_ISOCH       (1)
#define USB_EP_ATTR_BULK        (2)
#define USB_EP_ATTR_INTR        (3)


/**
 * @brief Top level initialization routine.
 * 
 * Should be called prior to any other USB activity.
 * 
 * @param [in]  Usbfn           Usbfn device instance pointer
 */
VOID
UsbfnDwcCoreInit (
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Top level shutdown routine.
 * 
 * Performs a software disconnect if still attached and stops all activity.
 * 
 * @param [in]  Usbfn           Usbfn device instance pointer
 */
VOID
UsbfnDwcCoreShutdown (
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Top level polling routine.
 * 
 * Should be periodically invoked by upper layer. Polls the cable status and,
 * if attached, polls the controller for events.
 * 
 * @param [in]  Usbfn           Usbfn device instance pointer
 */
VOID
UsbfnDwcCorePoll (
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Handle cable state change (attach/detach)
 *
 * Callback function invoked when the USB cable is inserted or removed.
 * Performs charger detection and invokes controller specific initialization
 * on attach or detach.
 *
 * @param [in]  Usbfn           Usbfn device instance pointer
 * @param [in]  IsAttached      Cable attach or cable detach event
 */
VOID
UsbfnDwcCoreCableStatus (
  IN USBFN_DEV                  *Usbfn,
  IN BOOLEAN                    IsAttached
  );


/**
 * @brief Handle suspend/resume conditions
 * 
 * Callback function invoked when the device is suspended or resumed.
 * Sets maximum current draw and notifies client of event.
 * 
 * @param [in]  Usbfn           Usbfn device instance pointer
 * @param [in]  IsSuspended     Suspend or resume event
 */
VOID
UsbfnDwcCorePortStatusChanged (
  IN USBFN_DEV                  *Usbfn,
  IN BOOLEAN                    IsSuspended
  );


/**
 * @brief Handle a USB reset event
 * 
 * Resets the configuration and device address to 0 and notifies client
 * of reset event.
 *
 * @param [in]  Usbfn           Usbfn device instance pointer
 */
VOID
UsbfnDwcCoreReset (
  IN USBFN_DEV                  *Usbfn
  );

/**
 * @brief Process a setup packet
 *
 * Perform processing of standard requests and invoke protocol layer callback
 * to notify client of setup data and defer processing to client.
 *
 * @param [in]  Usbfn           Usbfn device instance pointer
 * @param [in]  Buffer          Setup packet buffer (8 bytes long)
 * @param [out] HasDataStage    True for 3-stage transfer, false for 2-stage
 * @param [out] DataStageDir    If 3-stage, direction of data stage
 */
VOID
UsbfnDwcCoreProcessSetupPkt (
  IN  USBFN_DEV                 *Usbfn,
  IN  VOID                      *Buffer,
  OUT BOOLEAN                   *HasDataStage,
  OUT DWC_ENDPOINT_DIR          *DataStageDir
  );


#endif /* _USBFN_DWC3_CH9_H_ */
