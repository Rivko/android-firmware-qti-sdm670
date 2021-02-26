/** 
    @file  spi_drv.h
    @brief SPI driver interface
 */
/*=============================================================================
            Copyright (c) 2019-2020 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_DRV_H__
#define __SPI_DRV_H__

#include "SpiApi.h"
#include "spi_api.h"



// transfer state
typedef enum spi_xfer_state     spi_xfer_state;

// all about controller
typedef struct spi_hw_ctxt          spi_hw_ctxt;

// all about transfer
typedef struct spi_xfer_ctxt    spi_xfer_ctxt;

// all about client
typedef struct spi_client_ctxt      spi_client_ctxt;

enum spi_xfer_state
{
    TRANSFER_INVALID = 0,
    TRANSFER_IN_PROGRESS,
    TRANSFER_QUEUED,
    TRANSFER_OUTPUT_DONE,
    TRANSFER_INPUT_DONE,
    TRANSFER_DONE,
    TRANSFER_CANCELED,
    TRANSFER_TIMED_OUT,
};

struct spi_hw_ctxt
{
	// handle to device config
	void            *core_config;
   // handle to logs
   void            *core_debug;
   // handle to bus interface
   void            *core_iface;
   // for critical sections
   void            *core_lock;
   // for signaling
   void            *core_signal;
   // island mode
   boolean          polled;

   spi_config_t       saved_config;
   // store head and tail of the client requests in queue for async transfers
   spi_client_ctxt     *client_ctxt_head;
   spi_client_ctxt     *client_ctxt_tail;
   // synchronization for the client request queue
   void                *queue_lock;

   // reference counts
   uint32          core_ref_count;
   uint32          power_ref_count;

   // next controller
   spi_hw_ctxt         *next;
};

struct spi_xfer_ctxt
{
    // client parameters
   spi_config_t       *config;
   spi_descriptor_t   *desc;
   callback_fn         callback;
   void               *ctxt;
   uint32              num_descs;

   // track the transfer
   uint32 			   tx_transferred;
   uint32				rx_transferred;
   uint32              xfer_count;
   spi_xfer_state      xfer_state;
   spi_status        xfer_status;
   uint32              eots_set;
   uint32              eots_rxd;
   uint32              start_bit_timestamp;
   uint32              stop_bit_timestamp;

   //saved dfs and divider for island
   uint32              saved_dfs_index;
   uint32              saved_div;

	boolean             async;
	boolean             get_timestamp;
    boolean             config_0_sent;

    //timestamps - TBD

    // bus iface io context
    void               *io_ctxt;
};

// the handle that is returned to the client in open
struct spi_client_ctxt
{
    spi_hw_ctxt             *h_ctxt;
    spi_xfer_ctxt       t_ctxt;
    spi_client_ctxt         *next;
};

#endif
