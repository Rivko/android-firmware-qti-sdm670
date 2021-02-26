/** 
    @file  spi_iface.h
    @brief spi interface
 */
/*=============================================================================
            Copyright (c) 2019-2020 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_IFACE_H__
#define __SPI_IFACE_H__

typedef struct bus_iface_hw_ctxt
{
    uint16 rx_fifo_depth;
    uint16 tx_fifo_depth;

} bus_iface_hw_ctxt;

spi_status bus_iface_init    (spi_hw_ctxt *h_ctxt);
spi_status bus_iface_deinit (spi_hw_ctxt *h_ctxt);

spi_status bus_iface_queue_transfer (spi_client_ctxt *c_ctxt,boolean is_lasts);
void bus_iface_isr (void *int_handle);


#endif
