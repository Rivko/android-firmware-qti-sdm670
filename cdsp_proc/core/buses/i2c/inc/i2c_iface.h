/** 
    @file  i2c_iface.h
    @brief i2c interface
 */
/*=============================================================================
            Copyright (c) 2016-2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __I2C_IFACE_H__
#define __I2C_IFACE_H__

#include "i2c_api.h"
#include "i2c_drv.h"
#include "gpi.h"

#define MAX_TX_TRE_LIST_SIZE_PER_CORE 16
#define MAX_RX_TRE_LIST_SIZE_PER_CORE 8

#define CLEAN_TX_CHANNEL 0x1
#define CLEAN_RX_CHANNEL 0x2

typedef struct bus_iface_hw_ctxt
{
    gpi_iface_params params;
    GPI_CHAN_STATE chan_state;
    void *command_wait_signal;

} bus_iface_hw_ctxt;

typedef struct bus_iface_io_ctxt
{
    gpi_tre_req tx_tre_req;
    gpi_tre_req rx_tre_req;
    uint8 tx_tre_remaining;
    uint8 rx_tre_remaining;
    uint8 pending_evts;
    uint8 unlock_pending;
    uint8 clean_cmd;
    uint8 allocated;

} bus_iface_io_ctxt;

i2c_status bus_iface_init    (hw_ctxt *h_ctxt);
i2c_status bus_iface_de_init (hw_ctxt *h_ctxt);

void *bus_iface_get_io_ctxt (uint8 core_index);
void bus_iface_release_io_ctxt (void *ptr);

i2c_status bus_iface_cancel (hw_ctxt *h_ctxt, client_ctxt *c_ctxt);

i2c_status bus_iface_queue_transfer (client_ctxt *c_ctxt);
void bus_iface_isr (void *int_handle);

boolean bus_iface_has_queueing(hw_ctxt *h_ctxt);
boolean bus_iface_has_interrupt_consumer(hw_ctxt *h_ctxt);

#endif
