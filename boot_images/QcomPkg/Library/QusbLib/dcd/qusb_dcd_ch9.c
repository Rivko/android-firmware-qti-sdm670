/*=======================================================================*//**
  @file         qusb_dcd_ch9.c

  @brief        Chapter 9 functionality

  @details      
 
                Copyright (c) 2008-2015 Qualcomm Technologies Incorporated.
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
  2015-01-01  bhsriniv  Include string.h
  2009-04-17  dhaval    Ported from VU_CORE_WIREDCONNECTIVITY_QUSB.00.00.08 
                        Disable ZLT for Rx endpoints. This way, long transfer 
                        will be split into several dTD's by hardware.	
  2009-03-17  dhaval    Removed compiler warnings.
  2007-08-14  vkondrat  check device state prior to transfer
  2007-08-10  vkondrat  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qusb_dcd.h"
#include "qusb_dci_api.h"
#include "LoaderUtils.h"
#include "qusb_log.h"
#include <string.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QUSB_MAX_CONFIG 1
#define QUSB_BOS_BUFF_SIZE  32
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/*Buffer being passed to DMA*/
__QUSB_DMA_ZI__ static uint8 qusb_setup_data[QUSB_MAX_CTRL_PKT_SIZE];
__QUSB_DMA_ZI__ static uint8 qusb_setup_status[QUSB_MAX_CTRL_PKT_SIZE];

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
 static uint32 QUSB_SHUTDOWN_LOOP_ITERATION = 500;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static void dcd_callback_reset(struct qusb_dcd_dsc_device* dev);
static void dcd_callback_setup(struct qusb_dcd_dsc_device* dev, void* buffer_ptr);
static void dcd_callback_disconnect(struct qusb_dcd_dsc_device* dev);
static void dcd_callback_suspend(struct qusb_dcd_dsc_device* dev);

static qusb_dci_callback_table_t dci_callbacks = {
  dcd_callback_reset,
  dcd_callback_setup,
  qusb_dcd_port_status_changed,
  dcd_callback_disconnect,
  dcd_callback_suspend,
};


/**
 * Find config by index
 * 
 * @param dev
 * @param index
 * 
 * @return 
 */
static struct qusb_dcd_dsc_configuration* find_config(const struct qusb_dcd_dsc_device* dev)
{
  return dev->configurations;
}

/**
 * Find config by configuration value
 * 
 * @param dev
 * @param value
 * 
 * @return 
 */
static struct qusb_dcd_dsc_configuration* find_config_by_value(
               const struct qusb_dcd_dsc_device* dev, uint8 value)
{
  return ((value <= QUSB_MAX_CONFIG) ? dev->configurations : NULL);
}

static struct qusb_dcd_dsc_interface* find_ifc(const struct qusb_dcd_dsc_device* dev)
{
  if (dev->active_configuration != NULL)
  {
    return dev->active_configuration->interfaces;
  }
  else
  {
    qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return NULL;
  }
}
/**
 * Find the speed from the device speed configured and update
 * the maximum packet size to be used 
 * 
 * @param void
 * 
 * @void 
 */
static void qusb_dcd_update_packet_size(void)
{
  uint32 speed; 
  uint32 max_packet_size = 0x0; 

  speed = qusb_dci_get_speed(); 
  switch(speed)
  {
    case QUSB_SUPER_SPEED:
      max_packet_size = 1024;
    break; 
    case QUSB_HIGH_SPEED: 
      max_packet_size = 512; 
    break; 
    case QUSB_FULL_SPEED:
    case QUSB_FULL_SPEED_48:
      max_packet_size = 64;
    break;
    case QUSB_LOW_SPEED: 
      max_packet_size = 8; 
    break;
    default:
    break; 
  }
  qusb_dci_set_max_packet_size(max_packet_size);
  qusb_log(DCD_UPDATE_PACKET_SIZE_LOG, 0, max_packet_size);
  return;
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
static struct qusb_dcd_dsc_endpoint* find_ep(
               const struct qusb_dcd_dsc_device* dev, uint16 ep_address)
{
  struct qusb_dcd_dsc_endpoint* ep;

  if(dev->active_configuration == NULL)
  {
    qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return NULL;
  }

  if(dev->active_configuration->interfaces == NULL)
  {
    qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return NULL;
  }

  for ( ep=dev->active_configuration->interfaces->endpoints;
  ep != NULL; ep = ep->next ) 
  {
    if(ep->descriptor != NULL)
    {
      if ( ep->descriptor->bEndpointAddress == ep_address ) 
      {
        return ep;
      }
    }
  }

  return NULL;
}

static struct qusb_dcd_dsc_string* find_string(
               const struct qusb_dcd_dsc_device* dev, uint16 lang, uint8 index)
{
  struct qusb_dcd_dsc_string* s;
  for ( s = dev->strings; s != NULL; s = s->next )
  {
    if ( ( s->langid == lang ) && ( s->index == index ) )
    {
      return s;
    }
  }
  return NULL;
}

static void dcd_notify(struct qusb_dcd_dsc_device* dev, qusb_notify_events_t event)
{
  qusb_log(DCD_NOTIFY_LOG, 0, event);
  if ( NULL != dev->notify ) 
  {
    dev->notify(dev,event);
  }
  else
  {
    qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
  }    
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
static int dcd_set_interface(struct qusb_dcd_dsc_configuration* conf, uint8 ifc_num, uint8 alt)
{
    
  struct qusb_dcd_dsc_interface* ifc = conf->interfaces;

  if ((ifc == NULL) || (ifc->descriptor == NULL))
  {
    qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return -1;
  }

  qusb_bus_log(DCD_SET_INTERFACE_LOG, 0, ifc_num);
  if ( ( ifc->descriptor->bInterfaceNumber == ifc_num ) && 
      ( ifc->descriptor->bAlternateSetting == alt ) ) 
  {
    struct qusb_dcd_dsc_endpoint* ep;
    for ( ep = ifc->endpoints; ep != NULL; ep = ep->next ) 
    {
      usb_desc_endpoint_t* dsc = ep->descriptor;

      if(dsc != NULL)
      {
        uint8 is_ep_in = (uint8)(!!(dsc->bEndpointAddress & 0x80));

        qusb_dci_config_endpoint((uint8)(dsc->bEndpointAddress & 0xf),
                                  is_ep_in,
                                  (uint8)(dsc->bmAttributes & 3),
                                  dsc->wMaxPacketSize,
                                  0); 
      }
      // Ifc has EP's but no desc, we should STALL 
      else
      {
        qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
        return -1;
      }
    }
      return 0;
  }
  return -1;
}

int qusb_dcd_set_config(struct qusb_dcd_dsc_device* dev, uint8 new_config)
{

  //It is illegal to change configuration from one non-0 to
  //another
  qusb_bus_log(DCD_SET_CONFIG_LOG, 0, new_config);
  if ( ( 0 != dev->config_value ) && 
        ( 0 != new_config ) && 
        ( new_config != dev->config_value )  ) 
  {
    return -1;
  }
  if ( new_config != dev->config_value ) 
  {
    if ( new_config != 0 ) 
    {
      struct qusb_dcd_dsc_configuration* conf = find_config_by_value(dev, new_config);
      if ( NULL == conf ) 
      { 
        /* requested configuration do not exist */
        return -1;
      }
      // Do not perform set interface operation for single enumeration use case.
      if( FALSE == qusb_dci_skip_enumeration() )
      {
        if ( dcd_set_interface(conf, conf->interfaces->descriptor->bInterfaceNumber, 0) < 0 ) 
        {
          return -1;
        }
      }
      /* apply changes */
      dev->config_value = new_config;
      dev->active_configuration = conf;
      dev->qusb_is_configured = TRUE;
      dev->notify(dev, QUSB_DCD_EVT_CONFIG);

    }
    else 
    {
      dev->config_value = 0;
      dev->active_configuration = NULL;
      //  unconfigure all EP's
      //  
      //  Accordingly to USB standard, device may contain up to 16
      //  EP's in each direction. EP0 is control EP and is handled
      //  separately (by DCI) , thus not mentioned here
      {
        uint8 ep;

        for(ep=1;ep<DCI_MAX_ENDPOINTS;ep++) 
        {
          qusb_dci_cancel_transfer(ep,QUSB_EP_DIR_RX);
          qusb_dci_unconfig_endpoint(ep,QUSB_EP_DIR_RX);
          qusb_dci_cancel_transfer(ep,QUSB_EP_DIR_TX);
          qusb_dci_unconfig_endpoint(ep,QUSB_EP_DIR_TX);
        }
      }
      dev->qusb_is_configured = FALSE;
    }
  }
  //Update the packet size after determining the speed    
  qusb_dcd_update_packet_size();
  return 0;
}

int qusb_dcd_control_transfer(struct qusb_dcd_dsc_device* dev, int is_tx, void* buff, uint32 len)
{
  int rc = -1;
  struct qusb_urb* urb = is_tx ? &dev->setup_tx_urb : &dev->setup_rx_urb;

  (is_tx == 1) ? qusb_bus_log(DCD_CONTROL_TRANSFER_TX_LOG, 0, len) : qusb_bus_log(DCD_CONTROL_TRANSFER_RX_LOG, 0, len);

  if ( urb->transfer_status != QUSB_URB_STATUS_ACTIVE )
  {
    urb->transfer_buffer_ptr = buff;
    urb->transfer_length = len;
    rc=qusb_submit_urb(urb);
  }
  return rc < 0 ? rc : (int)len;
}

/**
 * Initiate Tx transfer using dev->setup_status
 * 
 * @param dev
 * @param len    Transfer length
 * 
 * @return 
 */
static int dcd_control_reply(struct qusb_dcd_dsc_device* dev, uint32 len)
{
  return qusb_dcd_control_transfer(dev,DIR_TX,dev->setup_status,len);
}


/**
 * Find the BOS descriptor
 * 
 * @param dev
 * 
 * @return len  - Total length
 * @ref USB3.0 IF specification - Sec 9.6.2 
 */

static struct qusb_dcd_dsc_bos* find_bos(const struct qusb_dcd_dsc_device* dev)
{
  return (dev->bos);
}

int handle_get_descriptor(struct qusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
  PACKED struct dsc_struct
  {
    uint8 desc_index;
    uint8 desc_type;
  } PACKED_POST;
  struct dsc_struct* dsc = (struct dsc_struct*)(void*)(&req->wValue);
  void* buff = NULL;
  int len = -1;

  switch ( dsc->desc_type ) 
  {
    case USB_DESCTYPE_DEVICE:
      buff = (void*)dev->descriptor;

      if(buff == NULL)
      {
        qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
      }
      else
      {
        len = dev->descriptor->bLength;
      }
      qusb_bus_log(HANDLE_GET_DESCRIPTOR_DEVICE_LOG, 0, len);
    break;

    case USB_DESCTYPE_CONFIGURATION:
    {
      struct qusb_dcd_dsc_configuration* cfg = find_config(dev);
      if ( cfg != NULL )
      {

        buff = (void*)cfg->descriptor;

        if(buff == NULL)
        {
          qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
        }
        else
        {
          len = cfg->descriptor->wTotalLength;
        }
      }
      qusb_bus_log(HANDLE_GET_DESCRIPTOR_CONFIG_LOG, 0, len);
    }
    break;
    
    case USB_DESCTYPE_BOS:
    {
      if(dev->descriptor->bcdUSB > QUSB_BCD_USB2)
      {
        struct qusb_dcd_dsc_bos* bos_desc = find_bos(dev);
        if(bos_desc != NULL)
        {
          buff = (void*)bos_desc->descriptor;
          if(buff == NULL)
          {
            qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
          }
          else
          {
            len = bos_desc->descriptor->wTotalLength;
          }
        }
      }
      qusb_bus_log(HANDLE_GET_DESCRIPTOR_BOS_LOG, 0, len);
    }
    break;

    case USB_DESCTYPE_STRING:
    {
      struct qusb_dcd_dsc_string* s = 
            find_string(dev, req->wIndex, dsc->desc_index);
      if ( s != NULL )
      {
        buff = (void*)s->descriptor;

        if(buff == NULL)
        {
          qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
        }
        else
        {
          len = s->descriptor->bLength;
        }
      }
      qusb_bus_log(HANDLE_GET_DESCRIPTOR_STRING_LOG, 0, req->wIndex);
    }
    break;
    
    case USB_DESCTYPE_DEV_QUAL:
    {
      if((dev->speed < QUSB_SUPER_SPEED)
        && (NULL != dev->dev_qual))
      {
        buff = (void *)dev->dev_qual->descriptor;
        if(buff == NULL)
        {
          qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
        }
        else
        {
          len = dev->dev_qual->descriptor->bLength;
        }
        qusb_bus_log(HANDLE_GET_DESCRIPTOR_DEVQUAL_LOG, 0, len);
      }
    }
    break;

    case USB_DESCTYPE_OTH_SPEED:
    {
      if((dev->speed != QUSB_SUPER_SPEED)
         && (NULL != dev->other_speed_configuration))
      {
        buff = (void *)dev->other_speed_configuration->descriptor;
        if(buff == NULL)
        {
          qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
        }
        else
        {
          len = dev->other_speed_configuration->descriptor->wTotalLength;
        }
      }
    }
    break;

    default:
      qusb_bus_log(UNKNOWN_GET_DESCRIPTOR_RECEIVED_LOG, 0, dsc->desc_type);

  }
  if ( (len >= 0) && (buff != NULL))
  {
    if ( len > req->wLength ) 
    {
      len = req->wLength;
    }
    /* send it */
    return qusb_dcd_control_transfer(dev, dev->ctrl_data_dir, buff, (uint32)len);
  }
  return len;
}

static int handle_setup_dev(struct qusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
  int error =0x0;

  switch ( req->bRequest )
  {
    case USB_SETUP_REQ_GET_STATUS:
      if ( req->wLength >= 2 ) 
      { /* USB standard expects 2 bytes here */
        memset(dev->setup_data, 0, sizeof(qusb_setup_data));

        if(dev->speed == QUSB_SUPER_SPEED)
        {
          dev->setup_data[0] = DEV_BUS_PWRD;
          if (dev->u1_enabled) dev->setup_data[0] |= U1_ENABLE;
          if (dev->u2_enabled) dev->setup_data[0] |= U2_ENABLE;
        }
        else
        {
          dev->setup_data[0]= (dev->remote_wakeup_enabled << 1);
          /* bit 0 - is_self_powered */
          /* bit 1 - is_remote_wakeup */
        }
        qusb_bus_log(HANDLE_SETUP_DEV_GET_STATUS_LOG, 0, dev->setup_data[0]);
        return qusb_dcd_control_transfer(dev, dev->ctrl_data_dir, dev->setup_data, 2);
      }
    break;
    
    case USB_SETUP_REQ_CLEAR_FEATURE:
      if (req->wValue == USB_FSEL_DEV_REMOTE_WAKEUP)
      {
        dev->remote_wakeup_enabled = FALSE; 
      }
      else if (req->wValue == USB_FSEL_DEV_TEST_MODE)
      {
        // Disable test mode in HW after STATUS stage (ACK) is sent to host successfully.
        dev->test_mode = USB_PORT_TEST_DISABLE;
      }
      else if ( (req->wValue == USB_FSEL_U1_ENABLE) && (dev->qusb_is_configured == TRUE))
      {
        qusb_dci_handle_u1_u2_enable(dev, QUSB_DCI_U1_INIT_ENABLE, FALSE);
      }
      else if ((req->wValue == USB_FSEL_U2_ENABLE) && (dev->qusb_is_configured == TRUE))
      {
        qusb_dci_handle_u1_u2_enable(dev, QUSB_DCI_U2_INIT_ENABLE, FALSE);
      }
      else
      {
        error = -1;
      }
      qusb_bus_log(HANDLE_SETUP_DEV_CLEAR_FEATURE_LOG, 0, req->wValue);
      return error;

    case USB_SETUP_REQ_SET_FEATURE:
      if (req->wValue == USB_FSEL_DEV_TEST_MODE)
      {
        // Enable test mode in HW after STATUS stage (ACK) is sent to host successfully.
        dev->qusb_test_mode_enabled = TRUE;
        dev->test_mode = ((req->wIndex & 0x0f00) >> 8);
      }
      else if (req->wValue == USB_FSEL_DEV_REMOTE_WAKEUP)
      {
        dev->remote_wakeup_enabled = TRUE; 
      }
      else if ((req->wValue == USB_FSEL_U1_ENABLE) && (dev->qusb_is_configured == TRUE))
      {  
        qusb_dci_handle_u1_u2_enable(dev, QUSB_DCI_U1_INIT_ENABLE, TRUE);
      }
      else if ((req->wValue == USB_FSEL_U2_ENABLE) && (dev->qusb_is_configured == TRUE))
      {  
        qusb_dci_handle_u1_u2_enable(dev, QUSB_DCI_U2_INIT_ENABLE, TRUE);
      }
      else
      {
        error = -1;
      }
      qusb_bus_log(HANDLE_SETUP_DEV_SET_FEATURE_LOG, 0, req->wValue);
      return error;

    case USB_SETUP_REQ_SET_ADDRESS:
      if ( 0 == dev->config_value )
      {
        uint8 addr = (uint8)(req->wValue & 0xff);
        dev->address = addr;
        qusb_dci_set_address(dev->address);
        qusb_bus_log(HANDLE_SETUP_DEV_SET_ADDRESS_LOG, 0, dev->address);
        return 0;
      }
    break;
    
    case USB_SETUP_REQ_SET_SEL:
      if ( req->wLength == 6 )
      {
        qusb_dcd_control_transfer(dev, dev->ctrl_data_dir, dev->setup_data, req->wLength);
        qusb_bus_log(HANDLE_SETUP_DEV_SET_SEL_LOG, 0, req->wLength);
        qusb_dci_handle_set_select();
        return 0;
      }	 
    break;
    
    case USB_SETUP_REQ_SET_ISOCH_DELAY:
      /* We are not updating any data here, we Just return Success ( For CV )*/
      qusb_bus_log(HANDLE_SETUP_DEV_SET_ISOCH_DELAY_LOG, 0, 0);
      return 0;

    case USB_SETUP_REQ_GET_DESCRIPTOR:
      qusb_bus_log(HANDLE_SETUP_DEV_GET_DESCRIPTOR_LOG, 0, 0);
      return handle_get_descriptor(dev, req);
    
    case USB_SETUP_REQ_SET_DESCRIPTOR:
      qusb_bus_log(HANDLE_SETUP_DEV_SET_DESCRIPTOR_LOG, 0, 0);
      /* not supported (yet?) */
    break;
  
    case USB_SETUP_REQ_GET_CONFIGURATION:
      if ( req->wLength >= 1 ) 
      { /* USB standard expects 1 byte here */
        dev->setup_data[0] = dev->config_value;
        qusb_bus_log(HANDLE_SETUP_DEV_GET_CONFIGURATION_LOG, 0, dev->setup_data[0]);
        error = qusb_dcd_control_transfer(dev, dev->ctrl_data_dir, dev->setup_data, 1);
        return error;
      }
    break;
    
    case USB_SETUP_REQ_SET_CONFIGURATION:
      qusb_bus_log(HANDLE_SETUP_DEV_SET_CONFIGURATION_LOG, 0, (uint8)(req->wValue & 0xff));
      error = qusb_dcd_set_config(dev, (uint8)(req->wValue & 0xff));
      /* Allow device to accept U1 and U2 at this point (as per SNPS spec. Table6-57:DCTL) */
      qusb_dci_handle_u1_u2_enable(dev, QUSB_DCI_U1_U2_ACCPET_ENABLE, TRUE);
      return error;
    
    default:
    if ( NULL != dev->setup )
    {
      return dev->setup(dev, req);
    }
    break;
  }
  return -1;
}

static int handle_setup_ifc(struct qusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
  int error =0x0;
  struct qusb_dcd_dsc_interface* ifc = find_ifc(dev);

  switch ( req->bRequest )
  {
    case USB_SETUP_REQ_GET_STATUS:
      if ( ifc && req->wLength >= 2 ) 
      { /* USB standard expects 2 bytes here */
        memset(dev->setup_data, 0, sizeof(qusb_setup_data));
        if(dev->speed == QUSB_SUPER_SPEED)
        {
          if(dev->remote_wakeup_enabled == TRUE)
          {
            dev->setup_data[0] |= FUNC_REMOTE_WAKEUP_CAP |FUNC_REMOTE_WAKEUP;
          }
          else
          {
            dev->setup_data[0] |= FUNC_REMOTE_WAKEUP_CAP;
          }
        }
        qusb_bus_log(HANDLE_SETUP_IFC_GET_STATUS_LOG, 0, dev->setup_data[0]);
        error = qusb_dcd_control_transfer(dev, dev->ctrl_data_dir, dev->setup_data, 2);
        return error;
      }
    break;
    
    case USB_SETUP_REQ_CLEAR_FEATURE:
      qusb_bus_log(HANDLE_SETUP_IFC_CLEAR_FEATURE_LOG, 0, 0);
    break;
  
    case USB_SETUP_REQ_SET_FEATURE:
      if (req->wValue == USB_FSEL_IFACE_FUNC_SUSP)
      {
        dev->remote_wakeup_enabled = ( req->wIndex & 0x200 ) ? TRUE :FALSE;  
        qusb_bus_log(HANDLE_SETUP_IFC_SET_FEATURE_LOG, 0, dev->remote_wakeup_enabled);  
        return 0;
      }
      else
      {
        return -1;
      }

    case USB_SETUP_REQ_GET_INTERFACE:
      if ( ( ifc != NULL ) && ( req->wLength >= 1 ) ) 
      { 
        /* USB standard expects 1 byte here */
        /* qusb_setup_data is already assigned to dev->setup_data */
        memset((void*)dev->setup_data, 0, sizeof(qusb_setup_data));
        dev->setup_data[0] = ifc->current_altsetting;
        qusb_bus_log(HANDLE_SETUP_IFC_GET_INTERFACE_LOG, 0, dev->setup_data[0]);
        error = qusb_dcd_control_transfer(dev, dev->ctrl_data_dir, dev->setup_data, 1);
        return error;
      }
    break;
  
    case USB_SETUP_REQ_SET_INTERFACE:
    /* ifc as found above, is irrelevant */
    {
      struct qusb_dcd_dsc_configuration* conf = dev->active_configuration;
      if ( conf != NULL )
      {
        qusb_bus_log(HANDLE_SETUP_IFC_SET_INTERFACE_LOG, 0, 0);
        // Do not handle set_interface since stack use case only supports one
        // interface and does not have logic or use case to abort EP
        // return dcd_set_interface(conf, (uint8)(req->wIndex & 0xff),(uint8)(req->wValue & 0xff));
        return 0;
      }
    }
    break;
  
    default:
      if ( ( NULL != ifc ) && ( NULL != ifc->setup ) ) 
      {
        qusb_bus_log(CALL_SETUP_HANDLER_FOR_IFC_LOG, 0, 0);
        return ifc->setup(ifc, req);
      }
      break;
    }
  return -1;
}

static int handle_ep_halt(struct qusb_dcd_dsc_device* dev, 
                          struct qusb_dcd_dsc_endpoint* ep, int halt)
{
  uint8 ep_num;
  boolean is_tx;

  if(ep->descriptor == NULL)
  {
    qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return -1;
  }

  ep_num = ep->descriptor->bEndpointAddress & 0x0f;
  is_tx = (ep->descriptor->bEndpointAddress & 0x80)? TRUE:FALSE;
  ep->is_halt = halt;

  qusb_bus_log(HANDLE_EP_HALT_LOG, 0, ep_num);

  if ( halt != 0 ) 
  {
    qusb_dci_stall_endpoint(ep_num, is_tx);
  } 
  else 
  {
    qusb_dci_unstall_endpoint(ep_num, is_tx);
  }
  return 0;
}

static int handle_setup_ep(struct qusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
  int error = -1;
  struct qusb_dcd_dsc_endpoint* ep = find_ep(dev, req->wIndex);

  // Request is not valid for  ep=0=NULL and req->wIndex != ep when the device is  configured 
  if ( (NULL == ep) && (dev->config_value != 0) && (req->wIndex != 0))
  {
    return error;
  }

  switch ( req->bRequest )
  {
    case USB_SETUP_REQ_GET_STATUS:
      if ( req->wLength >= 2 ) 
      {
        if(dev->setup_data)
        {
          memset(dev->setup_data, 0, sizeof(qusb_setup_data));
          /*For Ep=0, we send zero for GET_STATUS*/

          if((req->wIndex != 0) && (ep != NULL) )
          {
            dev->setup_data[0] = ep->is_halt;
          }

          qusb_bus_log(HANDLE_SETUP_EP_GET_STATUS_LOG, 0, dev->setup_data[0]); 
          error = qusb_dcd_control_transfer(dev, dev->ctrl_data_dir, dev->setup_data, 2);
        }
        return error;
      }
    break;
    
    case USB_SETUP_REQ_CLEAR_FEATURE:
      switch ( req->wValue )
      { /* feature selector */
        case USB_FSEL_EP_HALT:
          if(NULL != ep)
          {
            qusb_bus_log(HANDLE_SETUP_EP_CLEAR_FEATURE_LOG, 0, 0);
            return handle_ep_halt(dev, ep, 0);
          }
          else
          {
          qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
          }
        default:
          return error;
      }
  
    case USB_SETUP_REQ_SET_FEATURE:
      switch ( req->wValue )
      { /* feature selector */
        case USB_FSEL_EP_HALT:
        if(NULL != ep)
        {
          qusb_bus_log(HANDLE_SETUP_EP_SET_FEATURE_LOG, 0, 0);
          return handle_ep_halt(dev, ep, 1);
        }
        else
        {
          qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
        }    
        default:
          return error;
      }
  
    case USB_SETUP_REQ_SYNCH_FRAME:
      qusb_bus_log(HANDLE_SETUP_EP_REQ_SYNCH_FRAME_LOG, 0, 0);
    break;

    default:
      if ( (NULL != ep) && (NULL != ep->setup) )
      {
        qusb_bus_log(CALL_SETUP_HANDLER_FOR_EP_LOG, 0, 0);
        return ep->setup(ep, req);
      }
      else
      {
        qusb_error_log(DCD_CH9_NULL_POINTER_ERROR_LOG, 0, __LINE__);
      }
    break;
  }
   return error;
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
static void dcd_callback_reset(struct qusb_dcd_dsc_device* dev)
{

  qusb_bus_log(DCD_CALLBACK_RESET_LOG, 0, 0x0);
  qusb_dcd_set_config(dev, 0);
  dev->address = 0;
  qusb_dci_set_address(dev->address);

  /** 
  *  reset EP0 software state
  *  
  *  Hardware has reset EP0 transfers, reflect this in software.
  */
  dev->setup_rx_urb.transfer_status = QUSB_URB_STATUS_COMPLETE_ERROR;
  dev->setup_tx_urb.transfer_status = QUSB_URB_STATUS_COMPLETE_ERROR;
  dev->qusb_is_configured = FALSE;
}


static void dcd_callback_disconnect(struct qusb_dcd_dsc_device* dev)
{
  qusb_bus_log(DCD_CALLBACK_DISCONNECT_LOG, 0, 0x0);
  dcd_notify(dev,QUSB_DCD_EVT_DISCONNECT);
}

static void dcd_callback_suspend(struct qusb_dcd_dsc_device* dev)
{
  qusb_bus_log(DCD_CALLBACK_SUSPEND_LOG, 0, 0x0);
  dcd_notify(dev,QUSB_DCD_EVT_SUSPEND);
}

static void dcd_callback_setup(struct qusb_dcd_dsc_device* dev, void* buffer_ptr)
{
  int data_out_len = -1; /**< Data phase length, or negative error code */
  struct usb_setup_packet* req = (struct usb_setup_packet*)buffer_ptr;

  if (req->wLength == 0)
  {
    dev->ctrl_data_dir = DIR_NONE;
  }
  else
  {
    dev->ctrl_data_dir =(req->bmRequestType & 0x80) ? DIR_TX : DIR_RX;
  }

  /* data phase */
  switch ( (req->bmRequestType >> USB_SETUP_RT_RCP_POS) & USB_SETUP_RT_RCP_SEED )
  {
    case USB_SETUP_RT_RCP_DEV:
      data_out_len = handle_setup_dev(dev, req);
    break;
    case USB_SETUP_RT_RCP_IFC:
      data_out_len = handle_setup_ifc(dev, req);
    break;
    case USB_SETUP_RT_RCP_EP:
      data_out_len = handle_setup_ep(dev, req);
    break;
    default:
    ;
  }
  /* handshake phase */
  if ( data_out_len > 0 )
  { 
    /* have data phase, data was sent, use Rx for handshake */
    qusb_dcd_control_transfer(dev, DIR_RX, dev->setup_status, 0);
  } 
  else if ( data_out_len == 0 )
  { 
    /* no data phase, use Tx for handshake */
    dcd_control_reply(dev, 0);
  } 
  else if ( data_out_len < 0 )
  { 
    /* error, stall EP0 */
    qusb_error_log(DCD_UNSUPPORTED_SETUP_REQUEST_LOG, 0, ((req->bmRequestType >> USB_SETUP_RT_RCP_POS) & USB_SETUP_RT_RCP_SEED ));
    qusb_dci_stall_endpoint(0,0);
  }
}

void qusb_dcd_port_status_changed(struct qusb_dcd_dsc_device* dev)
{
  dev->speed = (qusb_device_connect_speed_t) qusb_dci_get_speed();
  qusb_log(DCD_PORT_STATUS_CHANGED_LOG, 0, dev->speed);
  dcd_notify(dev,QUSB_DCD_EVT_SPEED);
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

void qusb_dcd_init(struct qusb_dcd_dsc_device* dev)
{
  qusb_log(DCD_INIT_LOG, 0, 0x0);

  /* device is in "default" state - not configured, no address assigned */
  dev->is_attached = 0;
  dev->speed = QUSB_UNKNOWN_SPEED;
  dev->address = 0;
  dev->active_configuration = NULL;
  dev->config_value = 0;

  /* prepare URB */
  dev->setup_tx_urb.usb_device = dev;
  dev->setup_tx_urb.endpoint_address = 0x80;
  dev->setup_tx_urb.complete_callback = NULL;
  dev->setup_tx_urb.transfer_status = QUSB_URB_STATUS_COMPLETE_ERROR;
  dev->setup_tx_urb.transfer_index = QUSB_RSC_IDX_NOT_SET;
  
  dev->setup_rx_urb.usb_device = dev;
  dev->setup_rx_urb.endpoint_address = 0x0;
  dev->setup_rx_urb.complete_callback = NULL;
  dev->setup_rx_urb.transfer_status = QUSB_URB_STATUS_COMPLETE_ERROR;
  dev->setup_rx_urb.transfer_index = QUSB_RSC_IDX_NOT_SET;

  dev->ctrl_data_dir = DIR_NONE;
  dev->ctrl_state = EP0_STATE_MIN ;
  dev->link_state = LS_SS_DIS ;
  dev->remote_wakeup_enabled = FALSE;
  dev->qusb_test_mode_enabled = FALSE;
  dev->qusb_is_configured = FALSE;
  dev->shutdown_mode = QUSB_COMPLETE_SHUTDOWN_MODE;

  dev->u1_enabled = FALSE;
  dev->u2_enabled = FALSE;

  dev->setup_data = qusb_setup_data;
  dev->setup_status = qusb_setup_status;

    qusb_dci_init(dev->core_id, dev, &dci_callbacks);
}

void qusb_dcd_shutdown(struct qusb_dcd_dsc_device* dev)
{
  uint32 i = 0; 
  qusb_log(DCD_SHUTDOWN_LOG, 0, dev->shutdown_mode);
  for (i = 0; (QUSB_URB_STATUS_ACTIVE == dev->setup_tx_urb.transfer_status) && (i < QUSB_SHUTDOWN_LOOP_ITERATION); i++)
  {
    qusb_dcd_poll(dev);
  }

  if(dev->shutdown_mode == QUSB_COMPLETE_SHUTDOWN_MODE)
  {
    qusb_dcd_set_config(dev, 0);
  }

  qusb_dci_shutdown();
  dev->is_attached = 0;
  dev->qusb_is_configured = FALSE;
  dev->speed = QUSB_UNKNOWN_SPEED;
  dev->address = 0;
}

void qusb_dcd_poll(struct qusb_dcd_dsc_device* device)
{
  if (NULL != device) 
  {
    qusb_dci_poll_events();
  }
}

int qusb_submit_urb(struct qusb_urb* urb)
{
  urb->signature = QUSB_URB_SIGNATURE;
  urb->transfer_status = 0;
  urb->actual_length = 0;
  if ( urb->usb_device != NULL ) 
  {   
    /**
    * When device is not in configured state, valid transfer 
    * may be only for EP0
    *  
    * urb->endpoint_address have EP number in bits 3:0
    */    
    if ( ( urb->usb_device->config_value != 0 ) || 
        ( ( urb->endpoint_address & 0xf ) == 0 ) ) 
    {
      int rc = 0x0;
      rc = (int)qusb_dci_transfer_request(urb);

      return rc;
    }
  }
  urb->transfer_status = QUSB_URB_STATUS_COMPLETE_ERROR;
  qusb_error_log(DCD_SUBMIT_URB_ERROR_LOG, 0, urb->transfer_status);
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
boolean qusb_dcd_is_enumerated(struct qusb_dcd_dsc_device* dev)
{
  qusb_dcd_poll(dev);
  return (dev->config_value != 0);
}





