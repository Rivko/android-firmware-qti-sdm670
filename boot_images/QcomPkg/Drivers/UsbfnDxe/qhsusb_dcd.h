#ifndef QHSUSB_DCD_H
#define QHSUSB_DCD_H
//============================================================================
/**
 * @file        qhsusb_dcd.h
 *
 * @brief       DCD API
 *
 * @details     Defines structures and functions required to initialize DCD
 *
 *              In order to function, DCD should be provided with device
 *              definition. This includes definitions for device,
 *              its configurations, interfaces, endpoints, strings etc.
 *
 *              For every element, there are 2 structures representing it:
 *              hardware (USB descriptor) and software one. Software structure
 *              holds all relevant data, including internal states;
 *              it also holds pointer to USB descriptor.
 *
 *              It is responsibility of upper layer to initialize device
 *              definition structures prior to DCD initialization.
 *
 *              USB descriptors should be allocated in such way that
 *              all descriptors related to particular configuration follows
 *              its configuration descriptor, in order specified in
 *              USB 2.0 spec, clause 9.4.3.
 *
 *              For class specific requests, 'setup' callbacks may be defined
 *              at device, interface, endpoint levels. If callback is defined,
 *              it is called with any non-standard SETUP.
 *
 *              Copyright (c) 2008,2014 QUALCOMM Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
//============================================================================

// ===========================================================================
//
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
//
//
// when       who      what, where, why
// --------   ---      -------------------------------------------------------
// 04/30/14   ar       Cleanup for NDEBUG build  
//
// ===========================================================================

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_ch9.h"
#include "UsbUrb.h"
#include <Protocol/EFIUsbfnIo.h>
#include <Library/BaseLib.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*=====features====*/
/**
 * support for charger, both wall charger and USB host
 */
//#define FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
/**
 * Multiple configurations support.
 *
 * In single configuration mode, it is assumed that defice have 1 configuration;
 */
//#define QHSUSB_HAVE_MULTI_CONFIG 1
/**
 * Multiple functions support.
 *
 * In single function mode, it is assumed that defice serve one function;
 * in USB, function == interface.
 * device may have multiple configurations
 * each configuration have 1 interface with 1 alt. setting.
 */
//#define QHSUSB_HAVE_MULTI_FUNCTIONS 1
/* attachment detection */
/* multi speed */
/* shutdown */

/**
 * Charger uses multi-config composition
 */
#if defined(FEATURE_QHSUSB_DEAD_BATTERY_CHARGING)
   #if !defined(QHSUSB_HAVE_MULTI_CONFIG)
      #define QHSUSB_HAVE_MULTI_CONFIG 1
   #endif
#endif

/**
 * CDC/ACM uses multi-functions composition
 */
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
   #if !defined(QHSUSB_HAVE_MULTI_FUNCTIONS)
      #define QHSUSB_HAVE_MULTI_FUNCTIONS 1
   #endif
#endif


#define DCD_MAX_EP                       16
#define DCD_MAX_DIR                      2


#define MAX_URBS                         4
#define DEFAULT_URB_MAX_BUFFER_SIZE      (16*1024*1024) /* For an arbitrarily aligned buffer. */
#define MAX_XFER_CB_MSGS                 MAX_URBS
  /*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct _EFI_USBFN_MESSAGE_NODE {
  LIST_ENTRY Link;
  EFI_USBFN_MESSAGE Message;
  union {
      EFI_USB_DEVICE_REQUEST SetupCbRequest;
      EFI_USBFN_TRANSFER_RESULT XferCbResult;
      EFI_USB_BUS_SPEED  SpeedCbSpeed;
  } Data;
} EFI_USBFN_MESSAGE_NODE;

struct qhsusb_dcd_dsc_endpoint;
struct qhsusb_dcd_dsc_interface;
struct qhsusb_dcd_dsc_configuration;
struct qhsusb_dcd_dsc_string;

struct qhsusb_dcd_dsc_device {
   /*===========supplied by application==============*/
   struct usb_desc_device* descriptor;
   struct qhsusb_dcd_dsc_configuration* configurations;
   uint32 core_id;
   /**
    * Event notification
    *
    * Called when device state changed. Device state is updated
    * with new values before calling this function.
    */
   void (*notify)(struct qhsusb_dcd_dsc_device* dev, uint32 event);
#define QHSUSB_DCD_EVT_ATTACH  (0) /**< is_attached changed */
#define QHSUSB_DCD_EVT_SPEED   (1) /**< speed changed */
#define QHSUSB_DCD_EVT_ADDRESS (2) /**< address changed */
#define QHSUSB_DCD_EVT_CONFIG  (3) /**< config_value changed */
#define QHSUSB_DCD_EVT_CURRENT (4) /**< max_current changed */
#define QHSUSB_DCD_EVT_SUSPEND (5) /**< is_suspended changed (suspend/resume) */
#define QHSUSB_DCD_EVT_OTHER   (6) /**< any other event */
   /**
    * Custom SETUP handler.
    *
    * Called for all device level SETUP not understood by DCD
    *
    * Return number of bytes transmitted;
    * for "set" type commands return 0.
    * In case of error, or if SETUP was not handled, return
    * negative error.
    */
   int (*setup)(struct qhsusb_dcd_dsc_device* dev, struct usb_setup_packet* req);
   int (*setup_ifc)(struct qhsusb_dcd_dsc_interface* ifc, struct usb_setup_packet* req);
   int (*setup_ep)(struct qhsusb_dcd_dsc_endpoint* ep, struct usb_setup_packet* req);

   /*==============private===============*/
   /* current state */
   int is_attached; /**< is Vbus connected? */
   int speed; /**< Low - 1, Full - 12, High - 480, wall charger - -1 */
#define QHSUSB_DCD_SPEED_UNKNOWN     (UsbBusSpeedUnknown)
#define QHSUSB_DCD_SPEED_LOW             (UsbBusSpeedLow)
#define QHSUSB_DCD_SPEED_FULL           (UsbBusSpeedFull)
#define QHSUSB_DCD_SPEED_HIGH           (UsbBusSpeedHigh)

   boolean init;
   int max_current; /**< in ma */
   int is_suspended;
   uint8 address;
   uint8 config_value;
   struct qhsusb_dcd_dsc_configuration* active_configuration;
   /* for setup transfers */
   // TODO: setup_buffer is unaligned but don't see errors.

   struct qhsusb_urb Urb[MAX_URBS];

   LIST_ENTRY UsbfnMessages;

   LIST_ENTRY UsbfnMessagesMemPool;
   EFI_USBFN_MESSAGE_NODE  *UsbfnMessageMemPoolBuffer;

   EFI_PHYSICAL_ADDRESS PhyAddr;
   UINTN Pages;

   void (*xtach_cb)(struct qhsusb_dcd_dsc_device *dev, boolean is_cable_attached);
   void (*reset_cb)(struct qhsusb_dcd_dsc_device *dev);
   void (*suspend_cb)(struct qhsusb_dcd_dsc_device *dev);
   void (*resume_cb)(struct qhsusb_dcd_dsc_device *dev);
   void (*speed_cb)(struct qhsusb_dcd_dsc_device *dev, int speed);

   struct _USBFN_DEV * usbfn;


   boolean zlt[DCD_MAX_EP][DCD_MAX_DIR];

   UINTN max_transfer_size[DCD_MAX_EP][DCD_MAX_DIR];
};

struct qhsusb_dcd_dsc_configuration {
   /**
    * list of all configurations for device
    */
   struct qhsusb_dcd_dsc_configuration* next;
   struct usb_desc_configuration* descriptor;
   struct qhsusb_dcd_dsc_device* device;
   struct qhsusb_dcd_dsc_interface* interfaces;

};

struct qhsusb_dcd_dsc_string {
   struct qhsusb_dcd_dsc_string* next;
   struct usb_desc_header* descriptor;
   uint16 langid;
   uint8  index;
};

struct qhsusb_dcd_dsc_interface {
   /**
    * list of all interfaces/altsettings for config
    */
   struct qhsusb_dcd_dsc_interface* next;
   struct usb_desc_interface* descriptor;
   struct qhsusb_dcd_dsc_configuration* configuration;
   struct qhsusb_dcd_dsc_endpoint* endpoints;
   /**
    * Custom SETUP handler.
    *
    * Called for all interface level SETUP not understood by DCD
    *
    * Return number of bytes transmitted;
    * for "set" type commands return 0.
    * In case of error, or if SETUP was not handled, return
    * negative error.
    */
   int (*setup)(struct qhsusb_dcd_dsc_interface* ifc, struct usb_setup_packet* req);

   /**
    * currently selected alt setting for this interface.
    *
    * set simultaneously for all altsettings for interface
    */
   uint8 current_altsetting;
};

struct qhsusb_dcd_dsc_endpoint {
   /**
    * list of endpoints for interface
    */
   struct qhsusb_dcd_dsc_endpoint* next;
   usb_desc_endpoint_t* descriptor;
   struct qhsusb_dcd_dsc_interface* ifc;
   /**
    * Custom SETUP handler.
    *
    * Called for all endpoint level SETUP not understood by DCD
    *
    * Return number of bytes transmitted;
    * for "set" type commands return 0.
    * In case of error, or if SETUP was not handled, return
    * negative error.
    */
   int (*setup)(struct qhsusb_dcd_dsc_endpoint* ep, struct usb_setup_packet* req);
   /* private data */
   unsigned int is_halt:1;
};

enum qhsusb_log_event {
   evt_zero = 0,
   evt_init,
   evt_reset,
   evt_set_addr,
   evt_set_config,
   evt_get_config,
   evt_set_power,
   evt_raw_setup,
   evt_suspend,
   evt_resume,
   evt_attach,
   evt_speed,
   evt_urb_err,
   evt_notify,
   evt_portsc,
};
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
 * Initialize DCD.
 *
 * Should be called prior to any other USB activity
 *
 * @param device Descriptor for current device composition.
 */
void qhsusb_dcd_init(struct qhsusb_dcd_dsc_device* device);

/**
 * Should be periodically invoked by upper layer
 *
 * Non blocking
 *
 * @param device Device to poll.
 *
 *               May be NULL; in this case, no actions performed
 *               to protect from use of un-initialized device.
 */
void qhsusb_dcd_poll(struct qhsusb_dcd_dsc_device* device);

/**
 * Shutdown DCD
 *
 * Move device to unconfigured state
 * cancell all pending URB's
 *
 * @param device
 */
void qhsusb_dcd_shutdown(struct qhsusb_dcd_dsc_device* device);

/**
 * Initiate control transfer
 *
 * Used in custom SETUP flow.
 *
 * @param dev
 * @param is_tx  1 if Tx, 0 if Rx
 * @param buff   buffer to transmit;
 *               should not be modified till transmit completion.
 * @param len    number of bytes to transmit
 *
 * @return number of bytes transferred; or negative error code
 */
int qhsusb_dcd_control_transfer(struct qhsusb_dcd_dsc_device* dev, int is_tx, void* buff, uint32 len);

/**
 * Set new configuration value,
 * as if SET_CONFIG was received.
 *
 * Use to configure device to run with
 * already defined configuration value.
 * It assumes enumeration has already been done and
 * host issued SET_CONFIG before QHSUSB stack initialized.
 *
 * @param dev
 * @param new_config
 *
 * @return
 */
int qhsusb_dcd_set_config(struct qhsusb_dcd_dsc_device* dev, uint8 new_config);

/**
 * Force re-evaluation of USB port conditions.
 *
 * Conditions include:
 *
 * - attachment state
 * - connection speed
 * - suspended state
 *
 * Suitable for DCI callback in case of "port status changed"
 * interrupt
 *
 * @param dev
 */
void qhsusb_dcd_port_status_changed(struct qhsusb_dcd_dsc_device* dev);

/**
* Check if the USB device is enumerated.
*
*
* @param dev The device handle
*
* @return TRUE if the device is enumerated. FALSE otherwise.
*/
boolean qhsusb_dcd_is_enumerated(struct qhsusb_dcd_dsc_device* dev);

struct qhsusb_dcd_dsc_endpoint* find_ep(
               const struct qhsusb_dcd_dsc_device* dev, uint16 ep_address);

#endif /*QHSUSB_DCD_H*/
