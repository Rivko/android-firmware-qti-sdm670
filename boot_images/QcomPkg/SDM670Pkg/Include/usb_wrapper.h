#ifndef _USB_WRAPPER_H_
#define _USB_WRAPPER_H_
/*===========================================================================

  USB wrapper or Abstraction layer header file.

GENERAL DESCRIPTION
  USB APIs are different depending if the controller is CI USB 2.0 or
  SNPS USB 3.0. This header file provides a consistent API regardless of the
  underlying controller. Most of the API are very similar, for example
  qusb_al_bulk_poll or qhsusb_al_bulk_poll gets converted to usb_al_bulk_poll.
  There are APIs thare are not the same e.i. *usb_al_bulk_init(), to maintain
  the same function signature across the two APIs (qusb_ and qhsusb_) no
  arguments are taken and default values are used instead when calling the
  specific init API.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Include/usb_wrapper.h#1 $
  $DateTime: 2017/09/18 09:26:13 $ 
  $Author: pwbldsvc $

when       who        what, where, why
--------   --------   ----------------------------------------------------------
12/02/14   wkimberl   Initial version
============================================================================*/

#include "qusb_al_bulk.h"

/* For ducumentation of each of these APIs see qusb_* equivalent. */
void usb_al_bulk_init(void);
void usb_al_bulk_poll(void);
void usb_al_bulk_shutdown(void);
uint32 usb_al_bulk_receive(byte *rx_buf, 
			   uint32 len, 
			   sahara_rx_cb_type rx_cb, 
			   uint32 * err_code);
uint32 usb_al_bulk_transmit(byte *tx_buf,    
			    uint32 len,       
			    sahara_tx_cb_type tx_cb, 
			    uint32 * err_code);
uint32 usb_al_bulk_get_max_packet_size(void);
boolean usb_al_check_for_fedl(uint32 core_id);
uint32 usb_al_bulk_get_max_raw_data_size(void);
void usb_al_bulk_set_zlp_mode(boolean host_sends_zlp);

#endif
