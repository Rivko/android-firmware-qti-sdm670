#ifndef QUSB_DCD_H
#define QUSB_DCD_H
/*=======================================================================*//**
  @file         qusb_dcd.h
 
  @brief        DCD API
 
  @details      Defines structures and functions required to initialize DCD

                In order to function, DCD should be provided with device
                definition. This includes definitions for device,
                its configurations, interfaces, endpoints, strings etc.

                For every element, there are 2 structures representing it:
                hardware (USB descriptor) and software one. Software structure 
                holds all relevant data, including internal states;
                it also holds pointer to USB descriptor.

                It is responsibility of upper layer to initialize device
                definition structures prior to DCD initialization.

                USB descriptors should be allocated in such way that
                all descriptors related to particular configuration follows
                its configuration descriptor, in order specified in 
                USB 2.0 spec, clause 9.4.3.

                For class specific requests, 'setup' callbacks may be defined
                at device, interface, endpoint levels. If callback is defined,
                it is called with any non-standard SETUP.

                Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qusb_ch9.h"
#include "qusb_urb.h"
#include "qusb_common.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QUSB_SINGLE_ENUM_COOKIE                 (0xABCDEF)
#define QUSB_WAKEUP_COOKIE_VAL                  (0xABCDDCBA)
#define QUSB_COOKIE_SINGLE_ENUM_UNINIT_VALUE    (0xFFFFFFFF)
#define QUSB_COOKIE_NULL_VAL                    (0x0)
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

struct qusb_dcd_dsc_endpoint;
struct qusb_dcd_dsc_interface;
struct qusb_dcd_dsc_configuration;
struct qusb_dcd_dsc_string;
struct qusb_dcd_dsc_bos;

typedef enum qusb_notify_events {
  QUSB_DCD_EVT_ATTACH = 0x0,    // is_attached changed 
  QUSB_DCD_EVT_SPEED,           // speed changed 
  QUSB_DCD_EVT_ADDRESS,         // address changed 
  QUSB_DCD_EVT_CONFIG,          // config_value changed 
  QUSB_DCD_EVT_CURRENT,         // max_current changed 
  QUSB_DCD_EVT_SUSPEND,         // is_suspended changed (suspend/resume)
  //
  // Custom Event Notification
  // This is called to inform about a disconnect event or a Reset
  // Event to the Application layer.
  //
  QUSB_DCD_EVT_DISCONNECT,      //device disconnected
  QUSB_DCD_EVT_UNCONFIG,        // config_value changed
  QUSB_DCD_EVT_OTHER            // any other event 
}qusb_notify_events_t;


typedef enum
{
  DIR_RX, // OUT Token
  DIR_TX, // IN Token
  DIR_NONE = 0xFF
} qusb_ep_dir_t;

struct qusb_dcd_dsc_device {
  /*===========supplied by application==============*/
  struct usb_desc_device* descriptor;
  struct qusb_dcd_dsc_configuration* configurations;
  struct qusb_dcd_dsc_string* strings;
  struct qusb_dcd_dsc_bos* bos;
  struct qusb_dcd_dsc_configuration* other_speed_configuration;
  struct qusb_dcd_dsc_qual* dev_qual;
  uint32 core_id;
  /**
  * Event notification 
  *  
  * Called when device state changed. Device state is updated 
  * with new values before calling this function. 
  */
  void (*notify)(struct qusb_dcd_dsc_device* dev, qusb_notify_events_t event);
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
  int (*setup)(struct qusb_dcd_dsc_device* dev, struct usb_setup_packet* req);

  /*==============private===============*/
  /* current state */
  int is_attached;                    //is Vbus connected? 
  qusb_device_connect_speed_t speed;  //High=0,Full=1,Low=2,FullSpeed_48=3,SuperSpeed=4,Unknown = 5
  int max_current;                    // in ma
  int is_suspended;
  uint8 address;
  uint8 config_value;
  struct qusb_dcd_dsc_configuration* active_configuration;
  //for setup transfers
  uint8 *setup_status;
  uint8 *setup_data;
  struct qusb_urb setup_tx_urb;
  struct qusb_urb setup_rx_urb;
  qusb_ep_dir_t ctrl_data_dir;
  qusb_ctrl_xfer_state_t ctrl_state;
  qusb_link_state_t  link_state;
  uint32      max_packet_size;
  boolean remote_wakeup_enabled;
  boolean u1_enabled;
  boolean u2_enabled;
  boolean qusb_is_disconnected;
  boolean qusb_is_configured;
  boolean qusb_test_mode_enabled;
  qusb_shutdown_mode_t shutdown_mode;
  uint32  test_mode;
  qusb_max_speed_required_t speed_required; 
};

struct qusb_dcd_dsc_qual {
  struct qusb_dcd_dsc_qual* next;
  struct usb_desc_dev_qual* descriptor;
};

struct qusb_dcd_dsc_bos {
  struct qusb_dcd_dsc_bos* next;
  struct usb_bos_desc* descriptor; 
  struct qusb_dcd_dsc_ss_cap* ss_capability;
};

struct qusb_dcd_dsc_ss_cap {
  struct qusb_dcd_dsc_ss_cap* next; 
  struct usb_ss_cap_desc* descriptor;
};

struct qusb_dcd_dsc_configuration {
  /**
  * list of all configurations for device
  */
  struct qusb_dcd_dsc_configuration* next;
  struct usb_desc_configuration* descriptor;
  struct qusb_dcd_dsc_device* device;
  struct qusb_dcd_dsc_interface* interfaces;
};

struct qusb_dcd_dsc_string {
  struct qusb_dcd_dsc_string* next;
  struct usb_desc_header* descriptor;
  uint16 langid;
  uint8  index;
};

struct qusb_dcd_dsc_interface {
  /**
  * list of all interfaces/altsettings for config
  */
  struct qusb_dcd_dsc_interface* next;
  struct usb_desc_interface* descriptor;
  struct qusb_dcd_dsc_configuration* configuration;
  struct qusb_dcd_dsc_endpoint* endpoints;
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
  int (*setup)(struct qusb_dcd_dsc_interface* ifc, struct usb_setup_packet* req);

  /**
  * currently selected alt setting for this interface.
  * 
  * set simultaneously for all altsettings for interface
  */
  uint8 current_altsetting;
};

struct qusb_dcd_dsc_endpoint {
  /**
  * list of endpoints for interface
  */
  struct qusb_dcd_dsc_endpoint* next;
  usb_desc_endpoint_t* descriptor;
  struct qusb_dcd_dsc_interface* ifc;
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
  int (*setup)(struct qusb_dcd_dsc_endpoint* ep, struct usb_setup_packet* req);
  /* private data */
  uint32 is_halt:1;
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
void qusb_dcd_init(struct qusb_dcd_dsc_device* device);

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
void qusb_dcd_poll(struct qusb_dcd_dsc_device* device);

/**
 * Shutdown DCD
 * 
 * Move device to unconfigured state
 * cancell all pending URB's
 * 
 * @param device
 */
void qusb_dcd_shutdown(struct qusb_dcd_dsc_device* device);

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
int qusb_dcd_control_transfer(struct qusb_dcd_dsc_device* dev, int is_tx, void* buff, uint32 len);

/**
 * Set new configuration value, 
 * as if SET_CONFIG was received.
 * 
 * Use to configure device to run with 
 * already defined configuration value.
 * It assumes enumeration has already been done and
 * host issued SET_CONFIG before QUSB stack initialized.
 * 
 * @param dev
 * @param new_config
 * 
 * @return 
 */
int qusb_dcd_set_config(struct qusb_dcd_dsc_device* dev, uint8 new_config);

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
void qusb_dcd_port_status_changed(struct qusb_dcd_dsc_device* dev);


/**
* Check if the USB device is enumerated.
* 
* 
* @param dev The device handle
*
* @return TRUE if the device is enumerated. FALSE otherwise.
*/
boolean qusb_dcd_is_enumerated(struct qusb_dcd_dsc_device* dev);

#endif /*QUSB_DCD_H*/

