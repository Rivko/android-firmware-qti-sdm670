/** @file UsbWrapperLayerLib.c
  
  Translation layer for QUSB APIs.

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who       what, where, why
 --------   ---       ----------------------------------------------------------
 11/24/14   wkimberl  Initial revision

=============================================================================*/

#include "usb_wrapper.h"


void usb_al_bulk_init(void)
{
  qusb_al_bulk_init(QUSB_MAX_SPEED_SUPER);
}

void usb_al_bulk_poll(void)
{
  qusb_al_bulk_poll();
}

void usb_al_bulk_shutdown(void)
{
  qusb_al_bulk_shutdown();
}

uint32 usb_al_bulk_receive(byte *rx_buf, 
			   uint32 len, 
			   sahara_rx_cb_type rx_cb, 
			   uint32 * err_code)
{
  return qusb_al_bulk_receive(rx_buf, len, rx_cb, err_code);
}

uint32 usb_al_bulk_transmit(byte *tx_buf,    
			    uint32 len,       
			    sahara_tx_cb_type tx_cb, 
			    uint32 * err_code)
{
  return qusb_al_bulk_transmit(tx_buf, len, tx_cb, err_code);
}

uint32 usb_al_bulk_get_max_packet_size(void)
{
  return qusb_al_bulk_get_max_packet_size();
}


boolean usb_al_check_for_fedl(uint32 core_id)
{
   return qusb_al_check_for_fedl(core_id);
}


uint32 usb_al_bulk_get_max_raw_data_size(void)
{
  return qusb_al_bulk_get_max_raw_data_size();
}


void usb_al_bulk_set_zlp_mode(boolean host_sends_zlp)
{
  qusb_al_bulk_set_zlp_mode(host_sends_zlp); 
}
