/*=======================================================================*//**
  @file         qhsusb_dcd_ch9.c

  @brief        Chapter 9 functionality

  @details

                Copyright (c) 2008 - 2014, QUALCOMM Technologies Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.


  when        who       what, where, why
  ----------  ---       ------------------------------------------------------
  2014-10-04  ck        Fix mass storage app issue in 64 bit UEFI
  2014-09-22  ar        Add NULL pointer checks for security warnings
  2009-04-17  dhaval    Ported from VU_CORE_WIREDCONNECTIVITY_QHSUSB.00.00.08
                        Disable ZLT for Rx endpoints. This way, long transfer
                        will be split into several dTD's by hardware.
  2009-03-17  dhaval    Removed compiler warnings.
  2007-08-14  vkondrat  check device state prior to transfer
  2007-08-10  vkondrat  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_dcd.h"
#include <Library/UsbfnLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static void dcd_callback_reset(struct qhsusb_dcd_dsc_device* dev);
static void dcd_callback_setup(struct qhsusb_dcd_dsc_device* dev, void* buffer_ptr);
static void dcd_callback_cable_status(struct qhsusb_dcd_dsc_device* dev, boolean is_cable_attached);

static qhsusb_dci_callback_table_t dci_callbacks = {
   dcd_callback_reset,
   dcd_callback_setup,
   qhsusb_dcd_port_status_changed,
   dcd_callback_cable_status,
};

/**
 * Find config by configuration value
 *
 * @param dev
 * @param value
 *
 * @return
 */
static struct qhsusb_dcd_dsc_configuration* find_config_by_value(
               const struct qhsusb_dcd_dsc_device* dev, uint8 value)
{
#ifdef QHSUSB_HAVE_MULTI_CONFIG
   struct qhsusb_dcd_dsc_configuration* conf;
   for ( conf = dev->configurations; conf != NULL; conf = conf->next ) {
      if ( conf->descriptor->bConfigurationValue == value ) {
         return conf;
      }
   }
   return conf;
#else
   return dev->configurations;
#endif /* #ifdef QHSUSB_HAVE_MULTI_CONFIG */
}

static struct qhsusb_dcd_dsc_interface* find_ifc(
               const struct qhsusb_dcd_dsc_device* dev, uint16 index)
{
#ifdef QHSUSB_HAVE_MULTI_FUNCTIONS
   struct qhsusb_dcd_dsc_configuration* conf = dev->active_configuration;
   if ( conf ) {
      struct qhsusb_dcd_dsc_interface* ifc;
      for ( ifc = conf->interfaces; ifc != NULL; ifc = ifc->next ) {
         if ( ( ifc->descriptor->bInterfaceNumber == index ) &&
              ( ifc->descriptor->bAlternateSetting == ifc->current_altsetting ) ) {
            return ifc;
         }
      }
   }
#else
   if ( dev && dev->active_configuration ) { 
     return dev->active_configuration->interfaces;
   }
#endif /* #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS */
   return NULL;
}

/**
 * Find EP specified
 *
 * search strategy:
 * - go to current configuration
 * - iterate over interfaces
 *   - look fo current altsetting
 *   - iterate over endpoints
 *
 * @param dev
 * @param ep_address
 *
 * @return EP software descriptor,
 *         NULL if not found
 */
struct qhsusb_dcd_dsc_endpoint* find_ep(
               const struct qhsusb_dcd_dsc_device* dev, uint16 ep_address)
{
 struct qhsusb_dcd_dsc_configuration* conf = dev->active_configuration;
 if ( conf ) {
 #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS
      struct qhsusb_dcd_dsc_interface* ifc;
      for ( ifc=conf->interfaces; ifc != NULL; ifc = ifc->next ) {
         if ( ifc->descriptor->bAlternateSetting == ifc->current_altsetting ) {
            struct qhsusb_dcd_dsc_endpoint* ep;
            for ( ep=ifc->endpoints; ep != NULL; ep = ep->next ) {
               if ( ep->descriptor->bEndpointAddress == ep_address ) {
                  return ep;
               }
            }
         }
      }
#else
   struct qhsusb_dcd_dsc_endpoint* ep;
   if ( dev && dev->active_configuration && dev->active_configuration->interfaces ) { 
     for ( ep=dev->active_configuration->interfaces->endpoints;
	   ep != NULL; ep = ep->next ) {
       if ( ep->descriptor->bEndpointAddress == ep_address ) {
         return ep;
       }
     }
   }
#endif /* #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS */
 }
   return NULL;
}

static void dcd_notify(struct qhsusb_dcd_dsc_device* dev, uint32 event)
{
   if ( NULL != dev->notify ) {
      dev->notify(dev,event);
   }
}

#ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
/**
 * Set maximum power to be drawn drom USB host.
 *
 * Called every time when power allowed to be drawn changes.
 *
 * Call charger driver with updated value
 *
 * @param dev
 * @param max_current
 *               maximum current to draw from Vbus, ma
 */
static void dcd_set_max_current( struct qhsusb_dcd_dsc_device* dev, int max_current )
{
   dev->max_current = max_current;
   DEBUG(( EFI_D_INFO, "dcd_set_max_current: max current = %d\r\n", max_current));
   dcd_notify(dev,QHSUSB_DCD_EVT_CURRENT);
}
#else
static inline void dcd_set_max_current( struct qhsusb_dcd_dsc_device* dev, int max_current )
{}
#endif /* #ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING */

/**
 * Get maximum power to be drawn from USB host.
 *
 * Called to get maximum current allowed
 *
 * Call charger driver with updated value
 *
 * @param dev
 * @return max_current maximum current can be drawn from Vbus
 */
uint32 qhsusb_dcd_get_max_current(struct qhsusb_dcd_dsc_device* dev)
{
    return dev->max_current;
}

/**
 * Set alternate setting for given interface
 *
 * 2 path:
 * - 1-st to make sure correct ifc/alt exist;
 * - 2-nd to set correct alt for all ifc's with given index
 *
 * @param conf
 * @param ifc_num
 * @param alt
 *
 * @return error code:
 *         - 0 if OK
 *         - negative if error
 */
static int dcd_set_interface(
            struct qhsusb_dcd_dsc_configuration* conf, uint8 ifc_num, uint8 alt)
{
#ifdef QHSUSB_HAVE_MULTI_FUNCTIONS
   struct qhsusb_dcd_dsc_device* dev = conf->device;
   struct qhsusb_dcd_dsc_interface* ifc;
   struct qhsusb_dcd_dsc_interface* ifc1;
   for ( ifc=conf->interfaces; ifc != NULL; ifc = ifc->next ) {
      if ( ( ifc->descriptor->bInterfaceNumber == ifc_num ) &&
           ( ifc->descriptor->bAlternateSetting == alt ) ) {
         /* found, 2-nd pass to write current alt */
         for ( ifc1 = conf->interfaces; ifc1 != NULL; ifc1 = ifc1->next ) {
            if ( ifc1->descriptor->bInterfaceNumber == ifc_num ) {
               ifc1->current_altsetting = alt;
            }
         }
         /* configure DCI */
         {
            struct qhsusb_dcd_dsc_endpoint* ep;
            for ( ep = ifc->endpoints; ep != NULL; ep = ep->next ) {
               usb_desc_endpoint_t* dsc = ep->descriptor;
               uint8 is_ep_in = (uint8)(!!(dsc->bEndpointAddress & 0x80));

               /* Ignore wMaxPacketSize as specified by client and use the
                * maximum for the connected bus speed. */
               UINT16 wMaxPacketSize = 8;
               switch ( dsc->bmAttributes )
               {
                 case USB_EP_ATTR_CNTRL:
                   wMaxPacketSize = dev->speed == QHSUSB_DCD_SPEED_LOW
                                  ? 8
                                  : 64;
                 break;
                 case USB_EP_ATTR_ISOCH:
                   wMaxPacketSize = dev->speed == QHSUSB_DCD_SPEED_FULL
                                  ? 1023
                                  : 1024;
                 break;
                 case USB_EP_ATTR_BULK:
                   wMaxPacketSize = dev->speed == QHSUSB_DCD_SPEED_FULL
                                  ? 64
                                  : 512;
                 break;
                 case USB_EP_ATTR_INTR:
                   wMaxPacketSize = dev->speed == QHSUSB_DCD_SPEED_LOW
                                  ? 8
                                  : dev->speed == QHSUSB_DCD_SPEED_FULL
                                  ? 64
                                  : 1024;
                 break;
                 default:
                    DEBUG((EFI_D_ERROR, "Unknown Ep Type"));
               }

               qhsusb_dci_config_endpoint(conf->device->core_id,
                                          (uint8)(dsc->bEndpointAddress & 0xf),
                                          is_ep_in,
                                          (uint8)(dsc->bmAttributes & 3),
                                          wMaxPacketSize,
                                          ifc->configuration->device->zlt[dsc->bEndpointAddress & 0xF][is_ep_in]);//is_ep_in);
            }
         }
         return 0;
      }
   }
   return -1;
#else
   struct qhsusb_dcd_dsc_interface* ifc = conf->interfaces;
   if ( ( ifc->descriptor->bInterfaceNumber == ifc_num ) &&
        ( ifc->descriptor->bAlternateSetting == alt ) ) {
      struct qhsusb_dcd_dsc_endpoint* ep;
      for ( ep = ifc->endpoints; ep != NULL; ep = ep->next ) {
         usb_desc_endpoint_t* dsc = ep->descriptor;
         uint8 is_ep_in = (uint8)(!!(dsc->bEndpointAddress & 0x80));
         qhsusb_dci_config_endpoint(conf->device->core_id,
                                    (uint8)(dsc->bEndpointAddress & 0xf),
                                    is_ep_in,
                                    (uint8)(dsc->bmAttributes & 3),
                                    dsc->wMaxPacketSize,
                                    ifc->configuration->device->zlt[dsc->bEndpointAddress & 0xF][is_ep_in]);
      }
      return 0;
   }
   return -1;
#endif /* #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS */
}

int qhsusb_dcd_set_config(struct qhsusb_dcd_dsc_device* dev, uint8 new_config)
{
   /**
    * It is illegal to change configuration from one non-0 to
    * another
    */
   if ( ( 0 != dev->config_value ) &&
        ( 0 != new_config ) &&
        ( new_config != dev->config_value )  ) {
      return -1;
   }
   if ( new_config != dev->config_value ) {
      if ( new_config != 0 ) {
         struct qhsusb_dcd_dsc_configuration* conf =
            find_config_by_value(dev, new_config);
         if ( NULL == conf ) { /* requested configuration do not exist */
            return -1;
         }
         {
            /* all ifc's assigned altsetting 0 */
   #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS
            struct qhsusb_dcd_dsc_interface* ifc;
            for ( ifc = conf->interfaces; ifc != NULL; ifc = ifc->next ) {
               if ( dcd_set_interface(conf, ifc->descriptor->bInterfaceNumber, 0) < 0 ) {
                  return -1;
               }
            }
   #else
            if ( dcd_set_interface(conf, conf->interfaces->descriptor->bInterfaceNumber, 0) < 0 ) {
               return -1;
            }
   #endif /* #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS */
         }
         /* apply changes */
         dev->config_value = new_config;
         dev->active_configuration = conf;
#ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
         dev->is_suspended = 0;
         /**
          * Accordingly to USB standard (#9.6.3), bMaxPower is in units
          * of 2ma. To get value in ma, need to multiply it by 2
          */
         dcd_set_max_current(dev, dev->active_configuration->descriptor->bMaxPower * 2);
#endif /* #ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING */
      } else {
         dev->config_value = 0;
         dev->active_configuration = NULL;
         dcd_set_max_current(dev, 0);
         /**
          *  unconfigure all EP's
          *
          *  Accordingly to USB standard, device may contain up to 16
          *  EP's in each direction. EP0 is control EP and is handled
          *  separately (by DCI) , thus not mentioned here
          */
         {
            uint8 ep;
            for(ep=1;ep<16;ep++) {
               qhsusb_dci_cancel_transfer(dev->core_id,ep,QHSUSB_EP_DIR_RX);
               qhsusb_dci_unconfig_endpoint(dev->core_id,ep,QHSUSB_EP_DIR_RX);
               qhsusb_dci_cancel_transfer(dev->core_id,ep,QHSUSB_EP_DIR_TX);
               qhsusb_dci_unconfig_endpoint(dev->core_id,ep,QHSUSB_EP_DIR_TX);
            }
         }
      }
      dcd_notify(dev,QHSUSB_DCD_EVT_CONFIG);
   }
   return 0;
}

static void handle_setup_dev(struct qhsusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
   switch ( req->bRequest ) {
   case USB_SETUP_REQ_SET_ADDRESS:
      if ( 0 == dev->config_value ) {
         uint8 addr = (uint8)(req->wValue & 0xff);
         dev->address = addr;
         qhsusb_dci_set_address(dev->core_id, dev->address);
      }
      break;
   case USB_SETUP_REQ_SET_CONFIGURATION:
      qhsusb_dcd_set_config(dev, (uint8)(req->wValue & 0xff));
      break;
   }
   if ( NULL != dev->setup ) {
      dev->setup(dev, req);
   }
}

static void handle_setup_ifc(struct qhsusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
   struct qhsusb_dcd_dsc_interface* ifc = find_ifc(dev, req->wIndex);
   switch ( req->bRequest ) {
   case USB_SETUP_REQ_SET_INTERFACE:
      /* ifc as found above, is irrelevant */
      {
         struct qhsusb_dcd_dsc_configuration* conf = dev->active_configuration;
         if ( conf != NULL ) {
            dcd_set_interface(conf, (uint8)(req->wIndex & 0xff),
                                     (uint8)(req->wValue & 0xff));
         }
      }
      break;
   }
   if ( ( NULL != ifc ) && ( NULL != ifc->setup ) ) {
      ifc->setup(ifc, req);
   }
}


/********callbacks for DCI*************/
/**
 * Handle reset notification from DCI
 *
 * Reset moves USB device to 'default' state
 *
 * It means not configured and address == 0
 *
 * @param dev
 */
static void dcd_callback_reset(struct qhsusb_dcd_dsc_device* dev)
{
   /**
    * Prior to reset per se, perform port state analysis
    */
   qhsusb_dcd_port_status_changed(dev);
   qhsusb_dcd_set_config(dev, 0);
   dev->address = 0;
   qhsusb_dci_set_address(dev->core_id, dev->address);
   /**
    *  reset EP0 software state
    *
    *  Hardware has reset EP0 transfers, reflect this in software.
    */

   dev->reset_cb(dev);
}

static void dcd_callback_setup(struct qhsusb_dcd_dsc_device* dev, void* buffer_ptr)
{

   // CI 8.4.3.1.2
   // Note: After receiving a new setup packet the status and/or handshake phases may still
   // be pending from a previous control sequence. These should be flushed & deallocated before linking
   // a new status and/or handshake dTD for the most recent setup packet.

    qhsusb_dci_cancel_transfer(dev->core_id,0,QHSUSB_EP_DIR_TX);
    qhsusb_dci_cancel_transfer(dev->core_id,0,QHSUSB_EP_DIR_RX);

   struct usb_setup_packet* req = (struct usb_setup_packet*)buffer_ptr;
  DEBUG((
      EFI_D_INFO,
      "req->bmRequestType 0x%x\n"
      "req->bRequest 0x%x\n"
      "req->wIndex 0x%x\n"
      "req->wLength 0x%x\n"
      "req->wValue 0x%x\n",
      req->bmRequestType,
      req->bRequest,
      req->wIndex,
      req->wLength,
      req->wValue
  ));

  // Handle only the standard request. For other type of request,
  // just pass the setup pkt to the upper layer.

  if( ((req->bmRequestType >> USB_SETUP_RT_TYPE_POS) & USB_SETUP_RT_TYPE_SEED) != USB_SETUP_RT_TYPE_STD){
    DEBUG((EFI_D_INFO, "Non-Standard Request Received. Pass to Client to Handle it. \n"));
    dev->setup(dev, req);
  } else {
   /* data phase */
   switch ( (req->bmRequestType >> USB_SETUP_RT_RCP_POS) & USB_SETUP_RT_RCP_SEED ) {
   case USB_SETUP_RT_RCP_DEV:
      handle_setup_dev(dev, req);
      break;
   case USB_SETUP_RT_RCP_IFC:
      handle_setup_ifc(dev, req);
      break;
   default:
     if ( NULL != dev->setup ) {
        dev->setup(dev, req);
     }
     break;
   }
  }
}


/**
  Callback function invoked when the USB cable is inserted or removed

  @param  dev                 usb device
  @param  is_cable_attached   cable attach or cable detach event

**/
static void dcd_callback_cable_status(struct qhsusb_dcd_dsc_device* dev, boolean is_cable_attached)
{
    if(TRUE == is_cable_attached)
    {
        // 1. Run charger detection upon cable attach.
        //    RS bit will also be set if the port type is either CDC or SDP
        DEBUG(( EFI_D_INFO, "dcd_callback_cable_status: cable attached. \r\n"));
        qhsusb_chg_port_type chg_port_type = qhsusb_dci_detect_charger_type(dev->core_id);

        // If the port is DCP or invalid, set the charger current now.
        if(QHSUSB_CHG_PORT_DCP == chg_port_type)
        {
            dcd_set_max_current(dev, 1500);
        }
        if(QHSUSB_CHG_PORT_INVALID == chg_port_type)
        {
            dcd_set_max_current(dev, 500);
        }
    }
    else
    {
        // upon cable detach, clear RS bit and set max current to zero.
        DEBUG(( EFI_D_INFO, "dcd_callback_cable_status: cable detached. \r\n"));
        dcd_set_max_current(dev, 0);
        qhsusb_dcd_set_config(dev, 0);
        dev->address = 0;
        qhsusb_dci_disconnect(dev->core_id);
    }

    dev->speed = QHSUSB_DCD_SPEED_UNKNOWN;
    dev->is_attached = is_cable_attached;
    dev->xtach_cb(dev, is_cable_attached);
}


/**
  Callback function invoked when the device controller detects suspend/resume or
  the port controller enters the full or high-speed operational state.

  @param  dev  usb device

**/
void qhsusb_dcd_port_status_changed(struct qhsusb_dcd_dsc_device* dev)
{
  int speed = QHSUSB_DCD_SPEED_UNKNOWN;

  int is_suspended = qhsusb_dci_is_suspended(dev->core_id);

  // Act only when suspend/resume status changes
  if (dev->is_suspended == is_suspended) {
    return;
  }

  if (is_suspended) { //suspend
    // draw no current in suspended state
    dcd_set_max_current(dev, 0);

    // speed remains QHSUSB_DCD_SPEED_UNKNOWN in suspended state

    // notify suspend
    dev->suspend_cb(dev);

  } else { //resume
    // suspend can happen when the device is not configured
    // if we resume back a configured state, set max current to bMaxPower * 2
    // otherwise, set it to zero
    if (dev->active_configuration) {
      /**
       * bMaxPower is in units of 2ma - see USB 2.0 #9.6.3
       */
      dcd_set_max_current(dev,
          dev->active_configuration->descriptor->bMaxPower * 2);
    } else {
      dcd_set_max_current(dev, 0);
    }

    // upon resume, speed detection needs to be performed again.
    switch (qhsusb_dci_get_speed(dev->core_id)) {
    case QHSUSB_DEVICE_SPEED_FULL:
      speed = QHSUSB_DCD_SPEED_FULL;
      break;
    case QHSUSB_DEVICE_SPEED_LOW:
      speed = QHSUSB_DCD_SPEED_LOW;
      break;
    case QHSUSB_DEVICE_SPEED_HIGH:
      speed = QHSUSB_DCD_SPEED_HIGH;
      break;
    default:
      speed = QHSUSB_DCD_SPEED_UNKNOWN;
    }

    dev->resume_cb(dev);
  }

  if (dev->speed != speed) { /* speed changes */
    dev->speed_cb(dev, speed);
  }

  dev->speed = speed;
  dev->is_suspended = is_suspended;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

void qhsusb_dcd_init(struct qhsusb_dcd_dsc_device* dev)
{
   /* device is in "default" state - not configured, no address assigned */
   dev->speed = QHSUSB_DCD_SPEED_UNKNOWN;
   dev->address = 0;
   dev->active_configuration = NULL;
   dev->config_value = 0;
   dcd_set_max_current(dev, 0);

   /* init DCI */
   qhsusb_dci_init(dev->core_id, dev, &dci_callbacks);
   qhsusb_dci_start_timer(dev->core_id,QHSUSB_TIMER_0_FREE_RUN,1);
}

void qhsusb_dcd_shutdown(struct qhsusb_dcd_dsc_device* dev)
{

   qhsusb_dcd_set_config(dev, 0);
   qhsusb_dci_shutdown(dev->core_id);
   dev->is_attached = 0;
   dev->speed = QHSUSB_DCD_SPEED_UNKNOWN;
   dev->address = 0;
   //clear run/stop bit
   qhsusb_dci_disconnect(dev->core_id);
}

void qhsusb_dcd_poll(struct qhsusb_dcd_dsc_device* device)
{
   if (NULL != device) {
      qhsusb_dci_poll_events(device->core_id);
   }
}

int qhsusb_submit_urb(struct qhsusb_urb* urb)
{
   urb->signature = QHSUSB_URB_SIGNATURE;
   urb->transfer_status = QHSUSB_URB_STATUS_REUSABLE;
   urb->actual_length = 0;
   urb->current_dTD_index = 0;
   urb->max_dTD_index     = 0;
   if ( urb->usb_device != NULL ) {
      urb->core_id = urb->usb_device->core_id;
      /**
       * When device is not in configured state, valid transfer
       * may be only for EP0
       *
       * urb->endpoint_address have EP number in bits 3:0
       */
      if ( ( urb->usb_device->config_value != 0 ) ||
           ( ( urb->endpoint_address & 0xf ) == 0 ) ) {
         int rc = (int)qhsusb_dci_transfer_request(urb);
         return rc;
      }
   }
   urb->transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
   return -1;
}

/**
* Check if the USB device is enumerated.
*
*
* @param dev The device handle
*
* @return TRUE if the device is enumerated. FALSE otherwise.
*/
boolean qhsusb_dcd_is_enumerated(struct qhsusb_dcd_dsc_device* dev)
{
  qhsusb_dcd_poll(dev);

  return (dev->config_value != 0);
}
