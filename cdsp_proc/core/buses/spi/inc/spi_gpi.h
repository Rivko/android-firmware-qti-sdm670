/**
    @file  spi_gpi.h
    @brief GSI interface for SPI
 */
/*=============================================================================
            Copyright (c) 2016 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_GPI_H__
#define __SPI_GPI_H__

#include "gpi.h"

#define MAX_TX_TRE_LIST_SIZE_PER_CORE 16
#define MAX_RX_TRE_LIST_SIZE_PER_CORE 8

typedef struct spi_gpi_ctxt
{
    gpi_iface_params params;
    GPI_CHAN_STATE chan_state;
    void *command_wait_signal;
} spi_gpi_ctxt;

typedef struct spi_gpi_io_ctxt
{
    gpi_tre_req tx_tre_req;
    gpi_tre_req rx_tre_req;
    uint8 tx_tre_consumed;
    uint8 rx_tre_consumed;
} spi_gpi_io_ctxt;

spi_status_t spi_gpi_init    (spi_hw_ctxt *h_ctxt);
spi_status_t spi_gpi_deinit  (spi_hw_ctxt *h_ctxt);

spi_status_t spi_gpi_queue_transfer (spi_client_ctxt *c_ctxt);
void spi_gpi_isr (void *int_handle);

#endif /*__SPI_GPI_H__*/
