/** 
    @file  i2c_iface.h
    @brief i2c interface
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __I2C_IFACE_H__
#define __I2C_IFACE_H__

typedef struct bus_iface_hw_ctxt
{
    uint16 rx_fifo_depth;
    uint16 tx_fifo_depth;

} bus_iface_hw_ctxt;

i2c_status bus_iface_init    (hw_ctxt *h_ctxt);
i2c_status bus_iface_de_init (hw_ctxt *h_ctxt);

i2c_status bus_iface_queue_transfer (client_ctxt *c_ctxt);
void bus_iface_isr (void *int_handle);

boolean bus_iface_has_queueing(hw_ctxt *h_ctxt);
boolean bus_iface_has_interrupt_consumer(hw_ctxt *h_ctxt);

#endif
