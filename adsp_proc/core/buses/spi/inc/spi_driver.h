/**
  @file spi_driver.h
  @brief SPI internal definitions.
*/
/*
  ===========================================================================

  Copyright (c) 2016-2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/buses/spi/inc/spi_driver.h#4 $
  $DateTime: 2018/03/21 05:20:34 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#ifndef __SPI_DRIVER_H__
#define __SPI_DRIVER_H__

#include "spi_api.h"

#define SPI_RX_TX_PACKING_DISABLE 0  //See SPI Config 0
#define SPI_RX_TX_PACKING_ENABLE  3

typedef enum   spi_xfer_state       spi_xfer_state;
typedef struct spi_hw_ctxt          spi_hw_ctxt;
typedef struct spi_xfer_ctxt        spi_xfer_ctxt;
typedef struct spi_client_ctxt      spi_client_ctxt;

enum spi_xfer_state
{
    TRANSFER_INVALID = 0,
    TRANSFER_IN_PROGRESS,
    TRANSFER_QUEUED,
    TRANSFER_OUTPUT_OPERATIONS_DONE,
    TRANSFER_INPUT_OPERATIONS_DONE,
    TRANSFER_DONE,
    TRANSFER_CANCELED,
    TRANSFER_TIMED_OUT,
};

// SPI Master controller context
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

// Transfer related data
struct spi_xfer_ctxt
{
    // client parameters
    spi_config_t       *config;
    spi_descriptor_t   *desc;
    callback_fn         callback;
    void               *ctxt;
    uint32              num_descs;

    // track the transfer
    uint32              xfer_count;
    spi_xfer_state      xfer_state;
    spi_status_t        xfer_status;
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

    // SPI GPI io context
    void               *io_ctxt;
};

// the handle that is returned to the client in open
struct spi_client_ctxt
{
    spi_hw_ctxt      *h_ctxt;
    spi_xfer_ctxt    t_ctxt;
    spi_client_ctxt   *next;
};

#endif /*__SPI_DRIVER_H__*/
