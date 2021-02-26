/*==================================================================================================

FILE: DalUartDma.c

DESCRIPTION: This module handles DMOV operations when the UART DAL is in DMA mode.  It is
             not used in legacy mode.

                           Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/dal/src/DalUartDma.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   dma_close
   dma_deinit
   dma_init
   dma_open
   dma_receive
   dma_rx_flush
   dma_rx_intr_type
   dma_rx_intr_done
   dma_rx_purge
   dma_rx_wait_for_idle
   dma_transmit
   dma_tx_intr_done
   dma_tx_purge
   dma_tx_wait_for_idle

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#include "DalUartDma.tmh"
#else
#include "comdef.h"
#include "DALDeviceId.h"
#include "DALFramework.h"
#endif

#include "tal.h"
#include "DalUartDma.h"
#include "DalUartFwk.h"

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static boolean create_dma_objects (DMA_CONTEXT *dma_ctxt);
static void    destroy_dma_objects(DMA_CONTEXT *dma_ctxt);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: create_dma_objects

DESCRIPTION:

==================================================================================================*/
static boolean create_dma_objects(DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->config.client_id;
   DALResult result;

   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &dma_ctxt->rx_state_sync, NULL);
   if (result != DAL_SUCCESS) { goto error; }

   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &dma_ctxt->tx_state_sync, NULL);
   if (result != DAL_SUCCESS) { goto error; }

   result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &dma_ctxt->rx_idle_event, NULL);
   if (result != DAL_SUCCESS) { goto error; }

   result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &dma_ctxt->tx_idle_event, NULL);
   if (result != DAL_SUCCESS) { goto error; }

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "create_dma_objects: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: destroy_dma_objects

DESCRIPTION:

==================================================================================================*/
static void destroy_dma_objects(DMA_CONTEXT *dma_ctxt)
{
   if (dma_ctxt->rx_state_sync) { DALSYS_DestroyObject(dma_ctxt->rx_state_sync); }
   if (dma_ctxt->tx_state_sync) { DALSYS_DestroyObject(dma_ctxt->tx_state_sync); }
   if (dma_ctxt->rx_idle_event) { DALSYS_DestroyObject(dma_ctxt->rx_idle_event); }
   if (dma_ctxt->tx_idle_event) { DALSYS_DestroyObject(dma_ctxt->tx_idle_event); }
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: dma_close

DESCRIPTION:

==================================================================================================*/
void dma_close(DMA_CONTEXT *dma_ctxt)
{
   if (dma_ctxt->dma_handle)
   {
      tal_dma_close(dma_ctxt->dma_handle);
      dma_ctxt->dma_handle = NULL;
   }
}

/*==================================================================================================

FUNCTION: dma_deinit

DESCRIPTION:

==================================================================================================*/
void dma_deinit(DMA_CONTEXT *dma_ctxt)
{
   if (!dma_ctxt) { return; }

   destroy_dma_objects(dma_ctxt);
   DALSYS_Free(dma_ctxt);
}

/*==================================================================================================

FUNCTION: dma_init

DESCRIPTION:

==================================================================================================*/
DMA_CONTEXT *dma_init(TAL_DMA_CONFIG *config)
{
   DALDEVICEID id = config->client_id;
   DMA_CONTEXT *dma_ctxt = NULL;
   DALResult result;

   result = DALSYS_Malloc(sizeof(DMA_CONTEXT), (void **)&dma_ctxt);
   if (result != DAL_SUCCESS) { goto error; }

   memset(dma_ctxt, 0, sizeof(DMA_CONTEXT));
   dma_ctxt->config = *config;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_init: rx_dma_state = IDLE");
   dma_ctxt->rx_dma_state = IDLE;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_init: tx_dma_state = IDLE");
   dma_ctxt->tx_dma_state = IDLE;

   if ( !create_dma_objects(dma_ctxt) ) { goto error; }

   return(dma_ctxt);
error:
   dma_deinit(dma_ctxt);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_init: failed");
   return(NULL);
}

/*==================================================================================================

FUNCTION: dma_open

DESCRIPTION:

==================================================================================================*/
boolean dma_open(DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->config.client_id;
   TAL_RESULT result;

   result = tal_dma_open(&dma_ctxt->dma_handle, &dma_ctxt->config);
   if (result != TAL_SUCCESS) { goto error; }

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_open: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: dma_receive

DESCRIPTION:
   Start a DMA RX transfer (UART RXFIFO -> uncached RAM)

==================================================================================================*/
boolean dma_receive(DMA_CONTEXT *dma_ctxt, TAL_DMA_DESCRIPTOR *descriptor)
{
   DALDEVICEID id = dma_ctxt->config.client_id;
   TAL_RESULT result;
   boolean ret = FALSE;

   DALSYS_SyncEnter(dma_ctxt->rx_state_sync);

   if (dma_ctxt->rx_dma_state != IDLE)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_receive: DMA not ready");
   }
   else
   {
      result = tal_dma_rx_transfer(dma_ctxt->dma_handle, descriptor);

      if (result != TAL_SUCCESS)
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_receive: transfer failed");
      }
      else
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_receive: rx_dma_state = TRANSFER");
         dma_ctxt->rx_dma_state = TRANSFER;
         ret = TRUE;
      }
   }

   DALSYS_SyncLeave(dma_ctxt->rx_state_sync);
   return(ret);
}

/*==================================================================================================

FUNCTION: dma_rx_flush

DESCRIPTION:

==================================================================================================*/
void dma_rx_flush(DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->config.client_id;
   TAL_RESULT result;

   DALSYS_SyncEnter(dma_ctxt->rx_state_sync);

   if (dma_ctxt->rx_dma_state == TRANSFER)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_rx_flush: calling tal_dma_rx_cancel()");

      result = tal_dma_rx_cancel(dma_ctxt->dma_handle);

      if (result != TAL_SUCCESS)
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_rx_flush: failed to flush RX DMA channel");
      }

      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_rx_flush: rx_dma_state = FLUSH");
      dma_ctxt->rx_dma_state = FLUSH;
   }

   DALSYS_SyncLeave(dma_ctxt->rx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_rx_intr_done

DESCRIPTION:

==================================================================================================*/
void dma_rx_intr_done(DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->config.client_id;

   DALSYS_SyncEnter(dma_ctxt->rx_state_sync);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_rx_intr_done: rx_dma_state = IDLE");
   dma_ctxt->rx_dma_state = IDLE;
   DALSYS_EventCtrl(dma_ctxt->rx_idle_event, DALSYS_EVENT_CTRL_TRIGGER);
   DALSYS_SyncLeave(dma_ctxt->rx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_rx_intr_type

DESCRIPTION:

==================================================================================================*/
DMA_STATE dma_rx_intr_type(DMA_CONTEXT *dma_ctxt)
{
   DMA_STATE interrupt_type;

   DALSYS_SyncEnter(dma_ctxt->rx_state_sync);
   interrupt_type = dma_ctxt->rx_dma_state;
   DALSYS_SyncLeave(dma_ctxt->rx_state_sync);

   return(interrupt_type);
}

/*==================================================================================================

FUNCTION: dma_rx_purge

DESCRIPTION:

==================================================================================================*/
void dma_rx_purge(DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->config.client_id;
   TAL_RESULT result;

   DALSYS_SyncEnter(dma_ctxt->rx_state_sync);

   if (dma_ctxt->rx_dma_state == TRANSFER)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_rx_purge: calling tal_dma_rx_cancel()");

      result = tal_dma_rx_cancel(dma_ctxt->dma_handle);

      if (result != TAL_SUCCESS)
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_rx_purge: failed to flush RX DMA channel");
      }

      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_rx_purge: rx_dma_state = PURGE");
      dma_ctxt->rx_dma_state = PURGE;
   }
   else if (dma_ctxt->rx_dma_state == FLUSH)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_rx_purge: rx_dma_state = PURGE");
      dma_ctxt->rx_dma_state = PURGE;
   }

   DALSYS_SyncLeave(dma_ctxt->rx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_rx_wait_for_idle

DESCRIPTION:
   This function waits for the RX DMA channel to return to the IDLE state.

==================================================================================================*/
void dma_rx_wait_for_idle(DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->config.client_id;
   boolean rx_idle;
   DALResult result;

   DALSYS_SyncEnter(dma_ctxt->rx_state_sync);
   DALSYS_EventCtrl(dma_ctxt->rx_idle_event, DALSYS_EVENT_CTRL_RESET);
   rx_idle = (dma_ctxt->rx_dma_state == IDLE);
   DALSYS_SyncLeave(dma_ctxt->rx_state_sync);

   if (!rx_idle)
   {
      result = fwk_wait_event(dma_ctxt->rx_idle_event, 500);

      if ( result != DAL_SUCCESS )  // force the RX state to idle and hope for the best
      {
         DALSYS_SyncEnter(dma_ctxt->rx_state_sync);
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_rx_wait_for_idle: wait failed");
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_rx_wait_for_idle: rx_dma_state = IDLE");
         dma_ctxt->rx_dma_state = IDLE;
         DALSYS_SyncLeave(dma_ctxt->rx_state_sync);
      }
   }
}

/*==================================================================================================

FUNCTION: dma_transmit

DESCRIPTION:
   Start a DMA TX transfer (uncached RAM -> UART TXFIFO)

==================================================================================================*/
boolean dma_transmit(DMA_CONTEXT *dma_ctxt, TAL_DMA_DESCRIPTOR *descriptor)
{
   DALDEVICEID id = dma_ctxt->config.client_id;
   TAL_RESULT result;
   boolean ret = FALSE;

   DALSYS_SyncEnter(dma_ctxt->tx_state_sync);

   if (dma_ctxt->tx_dma_state != IDLE)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_transmit: DMA not ready");
   }
   else
   {
      result = tal_dma_tx_transfer(dma_ctxt->dma_handle, descriptor);

      if (result != TAL_SUCCESS)
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_transmit: transfer failed");
      }
      else
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_transmit: tx_dma_state = TRANSFER");
         dma_ctxt->tx_dma_state = TRANSFER;
         ret = TRUE;
      }
   }

   DALSYS_SyncLeave(dma_ctxt->tx_state_sync);
   return(ret);
}

/*==================================================================================================

FUNCTION: dma_tx_intr_done

DESCRIPTION:

==================================================================================================*/
void dma_tx_intr_done(DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->config.client_id;

   DALSYS_SyncEnter(dma_ctxt->tx_state_sync);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_tx_intr_done: tx_dma_state = IDLE");
   dma_ctxt->tx_dma_state = IDLE;
   DALSYS_EventCtrl(dma_ctxt->tx_idle_event, DALSYS_EVENT_CTRL_TRIGGER);
   DALSYS_SyncLeave(dma_ctxt->tx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_tx_purge

DESCRIPTION:

==================================================================================================*/
void dma_tx_purge(DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->config.client_id;
   TAL_RESULT result;

   DALSYS_SyncEnter(dma_ctxt->tx_state_sync);

   if (dma_ctxt->tx_dma_state == TRANSFER)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_tx_purge: calling tal_dma_tx_cancel()");

      result = tal_dma_tx_cancel(dma_ctxt->dma_handle);

      if (result != TAL_SUCCESS)
      {
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_tx_purge: failed to flush TX DMA channel");
      }

      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_tx_purge: tx_dma_state = PURGE");
      dma_ctxt->tx_dma_state = PURGE;
   }

   DALSYS_SyncLeave(dma_ctxt->tx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_tx_wait_for_idle

DESCRIPTION:
   This function waits for the TX DMA channel to return to the IDLE state.

==================================================================================================*/
void dma_tx_wait_for_idle(DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->config.client_id;
   boolean tx_idle;
   DALResult result;

   DALSYS_SyncEnter(dma_ctxt->tx_state_sync);
   DALSYS_EventCtrl(dma_ctxt->tx_idle_event, DALSYS_EVENT_CTRL_RESET);
   tx_idle = (dma_ctxt->tx_dma_state == IDLE);
   DALSYS_SyncLeave(dma_ctxt->tx_state_sync);

   if (!tx_idle)
   {
      result = fwk_wait_event(dma_ctxt->tx_idle_event, 500);

      if ( result != DAL_SUCCESS )  // force the TX state to idle and hope for the best
      {
         DALSYS_SyncEnter(dma_ctxt->tx_state_sync);
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dma_tx_wait_for_idle: wait failed");
         DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "dma_tx_wait_for_idle: tx_dma_state = IDLE");
         dma_ctxt->tx_dma_state = IDLE;
         DALSYS_SyncLeave(dma_ctxt->tx_state_sync);
      }
   }
}
