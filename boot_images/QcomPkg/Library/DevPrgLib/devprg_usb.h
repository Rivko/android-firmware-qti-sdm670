/**************************************************************************
 * FILE: devprg_usb.h
 *
 * USB abstraction layer for TX and RX. Includes VIP.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_usb.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-11-07   wek   Create.

===========================================================================*/

#ifndef __DEVPRG_USB_H__
#define __DEVPRG_USB_H__

#include "comdef.h"

/* Receive data from host, waiting for it to complete.
 *
 * param  data    Pointer to the buffer where to place the data.
 * param  size    Number of bytes in the data buffer.
 *
 * return -1 on error, number of bytes read on success.
 */
int64 devprg_usb_rx_blocking(void *data, size_t size);

/* Queue receive from host of a given size.
 *
 * param  data    Pointer to the buffer where to place the data.
 * param  size       Number of bytes in the data buffer.
 */
dp_res_t devprg_usb_rx_queue(void *data, size_t size);

/* Wait for the queued receive to complete.
 *
 * param  rx_bytes   number of bytes received.
 */
dp_res_t devprg_usb_rx_queue_wait(size_t *rx_bytes);

/* Receive data from host, waiting for it to complete.
 *
 * param  data   Pointer to the buffer with the data to be sent to the host.
 * param  size   Number of bytes in the data buffer.
 */
dp_res_t devprg_usb_tx_blocking(void *data, size_t size);

/* Sets if the host supports zero length packet.
 *
 * param val  1 if it supports it, 0 if it doesn't
 **/
dp_res_t devprg_usb_set_zlp_mode(unsigned int val);

/* Intializes the USB module. */
int devprg_usb_init(void);

#endif /* __DEVPRG_USB_H__ */
