#ifndef __DALUARTDMA_H__
#define __DALUARTDMA_H__
/*==================================================================================================

FILE: DalUartDma.h

DESCRIPTION: This module handles DMOV operations when the UART DAL is in DMA mode.  It is
             not used in legacy mode.

                              Copyright (c) 2010 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1/buses/uart/dal/src/DalUartDma.h#1 $

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#else
#include "comdef.h"  // to define "boolean"
#include "DALSysTypes.h"
#endif

#include "tal.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   IDLE,      // no outstanding DMOV requests
   TRANSFER,  // waiting on callback for DMTransfer()
   FLUSH,     // waiting on callback for DMChanFlush() - keep data
   PURGE      // waiting on callback for DMChanFlush() - discard data
} DMA_STATE;

typedef struct
{
   TAL_DMA_CONFIG     config;         // client configuration
   TAL_DMA_HANDLE     dma_handle;     // handle to DMA device

   DMA_STATE          rx_dma_state;   // current state of RX DMA requests
   DMA_STATE          tx_dma_state;   // current state of TX DMA requests
   DALSYSSyncHandle   rx_state_sync;  // protects receive state
   DALSYSSyncHandle   tx_state_sync;  // protects transmit state
   DALSYSEventHandle  rx_idle_event;  // used when closing to indicate RX is idle
   DALSYSEventHandle  tx_idle_event;  // used when closing to indicate TX is idle
} DMA_CONTEXT;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

void          dma_close              (DMA_CONTEXT *dma_ctxt);
void          dma_deinit             (DMA_CONTEXT *dma_ctxt);
DMA_CONTEXT   *dma_init              (TAL_DMA_CONFIG *config);
boolean       dma_open               (DMA_CONTEXT *dma_ctxt);
boolean       dma_receive            (DMA_CONTEXT *dma_ctxt, TAL_DMA_DESCRIPTOR *descriptor);
void          dma_rx_flush           (DMA_CONTEXT *dma_ctxt);
void          dma_rx_intr_done       (DMA_CONTEXT *dma_ctxt);
DMA_STATE     dma_rx_intr_type       (DMA_CONTEXT *dma_ctxt);
void          dma_rx_purge           (DMA_CONTEXT *dma_ctxt);
void          dma_rx_wait_for_idle   (DMA_CONTEXT *dma_ctxt);
boolean       dma_transmit           (DMA_CONTEXT *dma_ctxt, TAL_DMA_DESCRIPTOR *descriptor);
void          dma_tx_intr_done       (DMA_CONTEXT *dma_ctxt);
void          dma_tx_purge           (DMA_CONTEXT *dma_ctxt);
void          dma_tx_wait_for_idle   (DMA_CONTEXT *dma_ctxt);

#endif  // __DALUARTDMA_H__
