/*==================================================================================================

FILE: tal_dma_dmov.c

DESCRIPTION: This module defines the reference implementation of the DMA APIs for the
             Target Abstraction Layer.  It uses the DMOV DAL.  Note that the TAL DMA APIs
             are intended to be used with device->memory (RX) or memory->device (TX)
             transfers.  They do not support device->device or memory->memory transfers.

                              Copyright (c) 2012 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/tal/src/tal_dma_dmov.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   tal_dma_close
   tal_dma_open
   tal_dma_rx_cancel
   tal_dma_rx_transfer
   tal_dma_tx_cancel
   tal_dma_tx_transfer

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#include "tal_dma_dmov.tmh"
#else
#include "DALDeviceId.h"
#include "DALFramework.h"
#include "DDIDmov.h"
#endif

#include "tal.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef struct
{
   boolean          write_to_device;  // TRUE for TX channel, FALSE for RX channel
   uint32           dmov_device;      // DMOV device instance
   uint32           dmov_channel;     // DMOV channel number
   uint32           dmov_crci;        // DMOV CRCI
   uint32           fifo_address;     // physical address of device FIFO
   uint32           burst_size;       // DMA burst size
   TAL_DMA_CALLBACK cb_routine;       // callback function for completion events
   void *           cb_context;       // context to pass to callbacks
} CHANNEL_PARAMETERS;

typedef struct
{
   TAL_DMA_HANDLE      dma_ctxt;            // pointer back to the DMA context
   CHANNEL_PARAMETERS  parameters;        // channel configuration parameters
   TAL_DMA_DESCRIPTOR  *client_descriptor;  // used to communicate with the client
   DALSYSMemHandle     cmd_handle;        // uncached buffer that contains DMOV command
   DALSYSEventHandle   completion_event;  // event signaled during DMA transfer completion
   DmovTransferRequest dmov_descriptor;     // transfer descriptor
   DmovReqStatus       status;            // transfer request status
} DMA_CHANNEL;

typedef struct tal_dma_context
{
   DALDEVICEID      client_id;
   uint32           dmov_dal_id;   // (from XML props) DMOV DAL device ID
   uint32           dmov_chan_tx;  // (from XML props) DMOV TX channel number
   uint32           dmov_chan_rx;  // (from XML props) DMOV RX channel number
   uint32           dmov_crci_tx;  // (from XML props) DMOV TX CRCI number
   uint32           dmov_crci_rx;  // (from XML props) DMOV RX CRCI number
   DalDeviceHandle *dmov_dal;
   DMA_CHANNEL     *rx_channel;
   DMA_CHANNEL     *tx_channel;
} TAL_DMA_CONTEXT;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static DMA_CHANNEL *allocate_channel(TAL_DMA_CONTEXT *dma_ctxt, CHANNEL_PARAMETERS *parameters);
static void         deallocate_channel(DMA_CHANNEL *channel);
static void         dmov_completion(void *context);
static boolean      is_error(TAL_DMA_CONTEXT *dma_ctxt, DmovReqStatus *status);
static boolean      get_dmov_properties(TAL_DMA_CONTEXT *dma_ctxt);
static void         setup_command(TAL_DMA_CONTEXT *dma_ctxt, DMA_CHANNEL *channel);
static void         setup_descriptor(DMA_CHANNEL *channel);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: allocate_channel

DESCRIPTION:

==================================================================================================*/
static DMA_CHANNEL *allocate_channel(TAL_DMA_CONTEXT *dma_ctxt, CHANNEL_PARAMETERS *parameters)
{
   DALResult result;
   DMA_CHANNEL *channel = NULL;

   // allocate the channel structure

   result = DALSYS_Malloc(sizeof(DMA_CHANNEL), (void **)&channel);
   if (result != DAL_SUCCESS) { goto error; }

   // allocate the uncached command buffer

   result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED | DALSYS_MEM_PROPS_PHYS_CONT,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED, DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  sizeof(DmovBoxCommand), &channel->cmd_handle, NULL);
   if (result != DAL_SUCCESS) { goto error; }

   // create the completion event

   result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &channel->completion_event, NULL);
   if (result != DAL_SUCCESS) { goto error; }

   result = DALSYS_SetupCallbackEvent(channel->completion_event,
                                      (DALSYSCallbackFunc)dmov_completion, channel);
   if (result != DAL_SUCCESS) { goto error; }

   // populate the channel structure, command buffer, and transfer descriptor

   channel->dma_ctxt = dma_ctxt;
   channel->parameters = *parameters;
   channel->client_descriptor = NULL;
   setup_command(dma_ctxt, channel);
   setup_descriptor(channel);

   // set the channel operation mode

   result = DalDmov_SetChanOperationMode(dma_ctxt->dmov_dal, parameters->dmov_channel,
                                         DMOV_CHAN_MODE_INTERRUPT);
   if (result != DAL_SUCCESS) { goto error; }

   return(channel);
error:
   if (channel) { deallocate_channel(channel); }
   return(NULL);
}

/*==================================================================================================

FUNCTION: deallocate_channel

DESCRIPTION:

==================================================================================================*/
static void deallocate_channel(DMA_CHANNEL *channel)
{
   if (channel->completion_event) { DALSYS_DestroyObject(channel->completion_event); }
   if (channel->cmd_handle)       { DALSYS_DestroyObject(channel->cmd_handle);       }
   DALSYS_Free(channel);
}

/*==================================================================================================

FUNCTION: dmov_completion

DESCRIPTION:

==================================================================================================*/
static void dmov_completion(void *context)
{
   DMA_CHANNEL *channel = context;

   channel->client_descriptor->bytes_transferred = 0;  // this field is not supported on DMOV
   channel->client_descriptor->error             = is_error(channel->dma_ctxt, &channel->status);
   channel->client_descriptor->transfer_complete = TRUE;  // set this flag last
   channel->client_descriptor = NULL;  // relinquish ownership of the descriptor
   channel->parameters.cb_routine(channel->parameters.cb_context);
}

/*==================================================================================================

FUNCTION: is_error

DESCRIPTION:

==================================================================================================*/
static boolean is_error(TAL_DMA_CONTEXT *dma_ctxt, DmovReqStatus *status)
{
   DALDEVICEID id = dma_ctxt->client_id;
   DALResult dal_result;
   DmovResult dm_result;
   boolean error_detected = FALSE;

   dal_result = DalDmov_GetTransferResult(dma_ctxt->dmov_dal, status->dwToken, &dm_result);

   if (dal_result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "is_error: failed to get result");
   }
   else if (dm_result.eState == DMOV_TRANSFER_NON_EXISTENT)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "is_error: DMOV_TRANSFER_NON_EXISTENT");
   }
   else if (dm_result.eState == DMOV_TRANSFER_ERROR)
   {
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "is_error: DMOV_TRANSFER_ERROR");
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "dwResultValue: 0x%08X", dm_result.dwResultValue);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "FlushState[0]: 0x%08X", dm_result.FlushState[0]);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "FlushState[1]: 0x%08X", dm_result.FlushState[1]);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "FlushState[2]: 0x%08X", dm_result.FlushState[2]);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "FlushState[3]: 0x%08X", dm_result.FlushState[3]);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "FlushState[4]: 0x%08X", dm_result.FlushState[4]);
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "FlushState[5]: 0x%08X", dm_result.FlushState[5]);
      error_detected = TRUE;
   }

   return(error_detected);
}

/*==================================================================================================

FUNCTION: get_dmov_properties

DESCRIPTION:
   Get statically configurable DAL properties from XML file

==================================================================================================*/
static boolean get_dmov_properties(TAL_DMA_CONTEXT *dma_ctxt)
{
   DALDEVICEID id = dma_ctxt->client_id;
   DALSYS_PROPERTY_HANDLE_DECLARE(hDALProps);
   DALSYSPropertyVar PropVar;
   DALResult result;

   result = DALSYS_GetDALPropertyHandle(dma_ctxt->client_id, hDALProps);
   if (result != DAL_SUCCESS) { goto error; }

   result = DALSYS_GetPropertyValue(hDALProps, "DMOV_DEV", 0, &PropVar);
   if (result != DAL_SUCCESS) { goto error; }
   dma_ctxt->dmov_dal_id = PropVar.Val.dwVal;

   result = DALSYS_GetPropertyValue(hDALProps, "DMOV_CHAN_RX", 0, &PropVar);
   if (result != DAL_SUCCESS) { goto error; }
   dma_ctxt->dmov_chan_rx = PropVar.Val.dwVal;

   result = DALSYS_GetPropertyValue(hDALProps, "DMOV_CHAN_TX", 0, &PropVar);
   if (result != DAL_SUCCESS) { goto error; }
   dma_ctxt->dmov_chan_tx = PropVar.Val.dwVal;

   result = DALSYS_GetPropertyValue(hDALProps, "DMOV_CRCI_RX", 0, &PropVar);
   if (result != DAL_SUCCESS) { goto error; }
   dma_ctxt->dmov_crci_rx = PropVar.Val.dwVal;

   result = DALSYS_GetPropertyValue(hDALProps, "DMOV_CRCI_TX", 0, &PropVar);
   if (result != DAL_SUCCESS) { goto error; }
   dma_ctxt->dmov_crci_tx = PropVar.Val.dwVal;

   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "get_dmov_properties: dmov_dal_id = 0x%08X", dma_ctxt->dmov_dal_id);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "get_dmov_properties: dmov_chan_rx = 0x%08X", dma_ctxt->dmov_chan_rx);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "get_dmov_properties: dmov_chan_tx = 0x%08X", dma_ctxt->dmov_chan_tx);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "get_dmov_properties: dmov_crci_rx = 0x%08X", dma_ctxt->dmov_crci_rx);
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO,
                   "get_dmov_properties: dmov_crci_tx = 0x%08X", dma_ctxt->dmov_crci_tx);

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "get_dmov_properties: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: setup_command

DESCRIPTION:

==================================================================================================*/
static void setup_command(TAL_DMA_CONTEXT *dma_ctxt, DMA_CHANNEL *channel)
{
   DALSYSMemInfo cmd_info;
   DmovBoxCommand *cmd_ptr;
   DmovCommandOptions cmd_options;
   uint32 src_row_addr, src_row_len, src_row_num, src_row_offset;
   uint32 dst_row_addr, dst_row_len, dst_row_num, dst_row_offset;
   CHANNEL_PARAMETERS *parameters = &channel->parameters;

   // src_row_num and dst_row_num will be filled in before each transfer.  src_row_addr
   // will be filled in before TX transfers and dst_row_addr will be filled in before
   // RX transfers.

   src_row_addr   = parameters->write_to_device ? 0 : parameters->fifo_address;
   src_row_len    = parameters->burst_size;
   src_row_num    = 0;
   src_row_offset = parameters->write_to_device ? parameters->burst_size : 0;

   dst_row_addr   = parameters->write_to_device ? parameters->fifo_address : 0;
   dst_row_len    = parameters->burst_size;
   dst_row_num    = 0;
   dst_row_offset = parameters->write_to_device ? 0 : parameters->burst_size;

   DALSYS_MemInfo(channel->cmd_handle, &cmd_info);
   cmd_ptr = (DmovBoxCommand *)cmd_info.VirtualAddr;

   memset(cmd_ptr, 0, sizeof(DmovBoxCommand));
   cmd_ptr->dwFristSrcRowAddr = src_row_addr;
   cmd_ptr->dwFristDstRowAddr = dst_row_addr;
   cmd_ptr->dwRowLength       = (src_row_len    << 16) | dst_row_len;
   cmd_ptr->dwNumRows         = (src_row_num    << 16) | dst_row_num;
   cmd_ptr->dwRowOffset       = (src_row_offset << 16) | dst_row_offset;

   memset(&cmd_options, 0, sizeof(DmovCommandOptions));
   cmd_options.dwAddrMode   = DMOV_CMD_ADDR_MODE_BOX;
   cmd_options.dwSrcCRCI    = parameters->write_to_device ? 0 : parameters->dmov_crci;
   cmd_options.dwDstCRCI    = parameters->write_to_device ? parameters->dmov_crci : 0;
   cmd_options.bLastCommand = 1;

   DalDmov_PrepareCommand(dma_ctxt->dmov_dal, &cmd_options, (uint32 *)cmd_ptr);
}

/*==================================================================================================

FUNCTION: setup_descriptor

DESCRIPTION:

==================================================================================================*/
static void setup_descriptor(DMA_CHANNEL *channel)
{
   channel->dmov_descriptor.dwDevice          = channel->parameters.dmov_device;
   channel->dmov_descriptor.dwChan            = channel->parameters.dmov_channel;
   channel->dmov_descriptor.dwPriority        = 0;  // zero for now
   channel->dmov_descriptor.pUserData         = 0;
   channel->dmov_descriptor.hEvent            = channel->completion_event;
   channel->dmov_descriptor.dwNumCmds         = 1;
   channel->dmov_descriptor.TransferType      = DMOV_TRANSFER_SYNCHRONOUS;
   channel->dmov_descriptor.CommandType       = DAL_DMOV_CMD_LIST;
   channel->dmov_descriptor.cmd_ptr           = 0;  // legacy
   channel->dmov_descriptor.pCallbackFunction = 0;  // legacy
   channel->dmov_descriptor.options           = 0;  // legacy
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: tal_dma_close

DESCRIPTION:

==================================================================================================*/
void tal_dma_close(TAL_DMA_HANDLE handle)
{
   deallocate_channel(handle->rx_channel);
   deallocate_channel(handle->tx_channel);
   DalDevice_Close(handle->dmov_dal);
   DAL_DeviceDetach(handle->dmov_dal);
   DALSYS_Free(handle);
}

/*==================================================================================================

FUNCTION: tal_dma_open

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_dma_open(TAL_DMA_HANDLE *phandle, TAL_DMA_CONFIG *config)
{
   TAL_DMA_CONTEXT *dma_ctxt = NULL;
   DalDeviceHandle *dmov_dal = NULL;
   DMA_CHANNEL *rx_channel = NULL;
   DMA_CHANNEL *tx_channel = NULL;
   CHANNEL_PARAMETERS parameters;
   uint32 dmov_device;
   DALResult result;

   result = DALSYS_Malloc(sizeof(TAL_DMA_CONTEXT), (void **)&dma_ctxt);
   if (result != DAL_SUCCESS) { goto error; }

   dma_ctxt->client_id = config->client_id;

   if ( !get_dmov_properties(dma_ctxt) ) { goto error; }

   result = DAL_DeviceAttach(dma_ctxt->dmov_dal_id, &dmov_dal);
   if (result != DAL_SUCCESS) { goto error; }

   result = DalDevice_Open(dmov_dal, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS) { goto error; }

   dma_ctxt->dmov_dal = dmov_dal;

   dmov_device = (dma_ctxt->dmov_dal_id == DALDEVICEID_DMOV_DEVICE_0) ? (0) :
                 (dma_ctxt->dmov_dal_id == DALDEVICEID_DMOV_DEVICE_1) ? (1) :
                 (dma_ctxt->dmov_dal_id == DALDEVICEID_DMOV_DEVICE_2) ? (2) : (0);

   parameters.write_to_device = FALSE;
   parameters.dmov_device     = dmov_device;
   parameters.dmov_channel    = dma_ctxt->dmov_chan_rx;
   parameters.dmov_crci       = dma_ctxt->dmov_crci_rx;
   parameters.fifo_address    = config->rx_fifo;
   parameters.burst_size      = config->burst_size;
   parameters.cb_routine      = config->rx_callback;
   parameters.cb_context      = config->cb_context;

   rx_channel = allocate_channel(dma_ctxt, &parameters);
   if (!rx_channel) { goto error; }

   parameters.write_to_device = TRUE;
   parameters.dmov_device     = dmov_device;
   parameters.dmov_channel    = dma_ctxt->dmov_chan_tx;
   parameters.dmov_crci       = dma_ctxt->dmov_crci_tx;
   parameters.fifo_address    = config->tx_fifo;
   parameters.burst_size      = config->burst_size;
   parameters.cb_routine      = config->tx_callback;
   parameters.cb_context      = config->cb_context;

   tx_channel = allocate_channel(dma_ctxt, &parameters);
   if (!tx_channel) { goto error; }

   dma_ctxt->rx_channel = rx_channel;
   dma_ctxt->tx_channel = tx_channel;

   *phandle = dma_ctxt;
   return(TAL_SUCCESS);
error:
   if (rx_channel) { deallocate_channel(rx_channel); }
   if (tx_channel) { deallocate_channel(tx_channel); }
   if (dmov_dal)   { DAL_DeviceDetach(dmov_dal);     }
   if (dma_ctxt)   { DALSYS_Free(dma_ctxt);          }
   *phandle = NULL;
   return(TAL_ERROR);
}

/*==================================================================================================

FUNCTION: tal_dma_rx_cancel

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_dma_rx_cancel(TAL_DMA_HANDLE handle)
{
   DALResult result;

   // perform a graceful flush
   result = DalDmov_FlushChannel(handle->dmov_dal,
                                 handle->rx_channel->parameters.dmov_channel, FALSE, FALSE);

   return (result == DAL_SUCCESS) ? (TAL_SUCCESS) : (TAL_ERROR);
}

/*==================================================================================================

FUNCTION: tal_dma_rx_transfer

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_dma_rx_transfer(TAL_DMA_HANDLE handle, TAL_DMA_DESCRIPTOR *descriptor)
{
   uint32 burst_size;
   uint32 dst_row_addr, src_row_num, dst_row_num;
   DALSYSMemInfo cmd_info;
   DmovBoxCommand *cmd_ptr;
   DmovReqStatus *status_ptr;
   DALResult result;

   // queueing multiple descriptors per channel is not presently supported
   if (handle->rx_channel->client_descriptor) { return(TAL_ERROR); }

   burst_size = handle->rx_channel->parameters.burst_size;
   dst_row_addr = descriptor->physical_address;
   src_row_num  = (descriptor->bytes_to_transfer + burst_size - 1) / burst_size;
   dst_row_num  = src_row_num;

   DALSYS_MemInfo(handle->rx_channel->cmd_handle, &cmd_info);
   cmd_ptr = (DmovBoxCommand *)cmd_info.VirtualAddr;
   cmd_ptr->dwFristDstRowAddr = dst_row_addr;
   cmd_ptr->dwNumRows         = (src_row_num << 16) | dst_row_num;

   status_ptr = &handle->rx_channel->status;
   memset(status_ptr, 0, sizeof(DmovReqStatus));

   handle->rx_channel->client_descriptor = descriptor;

   result = DalDmov_Transfer(handle->dmov_dal, &handle->rx_channel->dmov_descriptor,
                             (uint32 *)cmd_info.PhysicalAddr, status_ptr);

   if (result != DAL_SUCCESS)
   {
      handle->rx_channel->client_descriptor = NULL;
      return(TAL_ERROR);
}

   return(TAL_SUCCESS);
}

/*==================================================================================================

FUNCTION: tal_dma_tx_cancel

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_dma_tx_cancel(TAL_DMA_HANDLE handle)
{
   DALResult result;

   // perform a discard flush
   result = DalDmov_FlushChannel(handle->dmov_dal,
                                 handle->tx_channel->parameters.dmov_channel, TRUE, TRUE);

   return (result == DAL_SUCCESS) ? (TAL_SUCCESS) : (TAL_ERROR);
}

/*==================================================================================================

FUNCTION: tal_dma_tx_transfer

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_dma_tx_transfer(TAL_DMA_HANDLE handle, TAL_DMA_DESCRIPTOR *descriptor)
{
   uint32 burst_size;
   uint32 src_row_addr, src_row_num, dst_row_num;
   DALSYSMemInfo cmd_info;
   DmovBoxCommand *cmd_ptr;
   DmovReqStatus *status_ptr;
   DALResult result;

   // queueing multiple descriptors per channel is not presently supported
   if (handle->tx_channel->client_descriptor) { return(TAL_ERROR); }

   burst_size = handle->tx_channel->parameters.burst_size;
   src_row_addr = descriptor->physical_address;
   src_row_num  = (descriptor->bytes_to_transfer + burst_size - 1) / burst_size;
   dst_row_num  = src_row_num;

   DALSYS_MemInfo(handle->tx_channel->cmd_handle, &cmd_info);
   cmd_ptr = (DmovBoxCommand *)cmd_info.VirtualAddr;
   cmd_ptr->dwFristSrcRowAddr = src_row_addr;
   cmd_ptr->dwNumRows         = (src_row_num << 16) | dst_row_num;

   status_ptr = &handle->tx_channel->status;
   memset(status_ptr, 0, sizeof(DmovReqStatus));

   handle->tx_channel->client_descriptor = descriptor;

   result = DalDmov_Transfer(handle->dmov_dal, &handle->tx_channel->dmov_descriptor,
                             (uint32 *)cmd_info.PhysicalAddr, status_ptr);

   if (result != DAL_SUCCESS)
   {
      handle->tx_channel->client_descriptor = NULL;
      return(TAL_ERROR);
   }

   return(TAL_SUCCESS);
}
