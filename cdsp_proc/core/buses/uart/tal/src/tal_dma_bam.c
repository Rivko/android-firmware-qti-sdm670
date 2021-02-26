/*==================================================================================================

FILE: tal_dma_bam.c

DESCRIPTION: This module defines the stub implementation of the DMA APIs for
             the Target Abstraction Layer.  Note that the TAL DMA APIs are
             intended to be used with device->memory (RX) or memory->device
             (TX) transfers.  They do not support device->device or
             memory->memory transfers.

                           Copyright (c) 2012-2013 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/tal/src/tal_dma_bam.c#1 $

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
#include "tal_dma_bam.tmh"
#else
#include "bam.h"
#include "comdef.h"
#include "DALDeviceId.h"
#include "DALFramework.h"
#include "DDIUart.h"
#endif

#include "tal.h"

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

//  Descriptor queue size.

#define DMA_BAM_DESCRIPTOR_QUEUE_SIZE_BYTES  128

//  Maximum size buffer that BAM can handle.

#define DMA_BAM_MAX_BUFFER_SIZE_BYTES        (64 * 1024)

#define DMA_BAM_MAX_SUM_THRESHOLD            (32 * 1024)

typedef struct tal_dma_context TAL_DMA_CONTEXT;

typedef struct
{
   bam_dir_type            direction;               // BAM direction, CONSUMER or PRODUCER.
   uint32                  number;                  // BAM pipe number.
   DALSYSMemHandle         descriptor_mem_handle;   // Handle for descriptor memory.
}BAM_PIPE_PROPERTIES;


typedef struct
{
   DALSYSSyncHandle        lock;                    // Protects pipe state.
   BAM_PIPE_PROPERTIES     properties;              // Pointer to BAM pipe data.
   boolean                 is_error;                // Has there been an error?
   boolean                 transfer_complete;       // To check during transfer_cancel
   uint32                  xfer_bytes;              // Bytes on last transfer.
   bam_handle              handle;                  // BAM pipe handle.
   TAL_DMA_DESCRIPTOR      *client_descriptor;

   TAL_DMA_CONTEXT*        device;                  // Pointer back to parent.
   const char*             name;                    // pipe name.

   TAL_DMA_CALLBACK        callback;                // Callback function.
   void*                   callback_ctxt;           // Callback context.
}DMA_BAM_PIPE;


typedef struct
{
   uint32                  physical_addr;
   uint32                  irq;
}DMA_BAM_PROPERTIES;


//  Equivalent to DMA_BAM_DEVICE
struct tal_dma_context
{
   DALDEVICEID             uart_dal_id;
   bam_handle              handle;
   DALSYSMemHandle         mem_handle;
   DMA_BAM_PROPERTIES      properties;
   DMA_BAM_PIPE*           rx;
   DMA_BAM_PIPE*           tx;
};

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static void             bam_device_close( TAL_DMA_CONTEXT* device );

static TAL_DMA_CONTEXT* bam_device_open( TAL_DMA_CONFIG* dma_config );

static TAL_RESULT       bam_transfer( DMA_BAM_PIPE* pipe, TAL_DMA_DESCRIPTOR *descriptor);

static TAL_RESULT       bam_transfer_cancel( DMA_BAM_PIPE* pipe );

static void             bam_transfer_done(DMA_BAM_PIPE* pipe, uint32 xfer_bytes, boolean is_error);

static void             device_event_callback_isr( bam_result_type bam_result );

static boolean          device_get_dword_property( TAL_DMA_CONTEXT* device,
                                                   DALSYSPropertyHandle handle,
                                                   const char* prop_name,
                                                   uint32* prop_val );

static boolean          device_get_properties( TAL_DMA_CONTEXT* device );

static const char*      get_event_str(bam_event_type event);

static void             pipe_close(DMA_BAM_PIPE* pipe);

static void             pipe_event_callback_isr( bam_result_type bam_result );

static boolean          pipe_init( DMA_BAM_PIPE* pipe, boolean is_rx );

static DMA_BAM_PIPE*    pipe_open( TAL_DMA_CONTEXT* device,
                                   TAL_DMA_CONFIG* dma_config,
                                   boolean is_rx );



/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION:     bam_device_close

DESCRIPTION:

==================================================================================================*/

void bam_device_close(TAL_DMA_HANDLE device)
{
   //  This function can also be used to clean up a partially
   //  constructed object.  Make no assumptions about initialized
   //  data.

   if( NULL == device )
   {
      return;
   }

   //  Close subobjects.

   pipe_close( device->rx );

   pipe_close( device->tx );

   //  Free resource allocated in device_open.

   if( device->handle )
   {
      bam_deinit( device->handle, TRUE );
   }

   if( device->mem_handle )
   {
      DALSYS_DestroyObject( device->mem_handle );
   }

   DALSYS_Free( device );
}

/*==================================================================================================

FUNCTION:     bam_device_open

DESCRIPTION:

==================================================================================================*/

TAL_DMA_HANDLE
bam_device_open(TAL_DMA_CONFIG *dma_config)
{

   TAL_DMA_HANDLE          device = NULL;
   DALDEVICEID             id = dma_config->client_id;
   DALResult               dal_result;
   bam_config_type         bam_cfg;
   bam_callback_type       bam_callback;


   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "+device_open");

   //  Allocate, initialize context (BAM device) object.

   dal_result = DALSYS_Malloc( sizeof( *device ), (void **) &device );
   if( dal_result != DAL_SUCCESS )
   {
      goto error;
   }

   memset( device, 0, sizeof( *device ));

   device->uart_dal_id = dma_config->client_id;

   //  Get BAM properties.

   if( !device_get_properties( device ))
   {
      goto error;
   }


   //  Open BAM device.

   bam_cfg.bam_pa         = device->properties.physical_addr;
   bam_cfg.bam_va         = 0;
   bam_cfg.bam_mti_irq_pa = 0;
   bam_cfg.bam_irq        = device->properties.irq;
   bam_cfg.bam_irq_mask   = BAM_IRQ_HRESP_ERR_EN | BAM_IRQ_ERR_EN;
   bam_cfg.sum_thresh     = DMA_BAM_MAX_SUM_THRESHOLD;
   bam_cfg.options        = 0;

   bam_callback.func = device_event_callback_isr;
   bam_callback.data = device;

   device->handle = bam_init( &bam_cfg, &bam_callback );

   if( NULL == device->handle )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "bam_init failed");
      goto error;
   }

   //  Open BAM RX Pipe

   device->rx = pipe_open( device, dma_config, TRUE );
   if( NULL == device->rx )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "RX pipe_open failed");
      goto error;
   }

   //  Open BAM TX pipe.

   device->tx = pipe_open( device, dma_config, FALSE );
   if( NULL == device->tx )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "TX pipe_open failed");
      goto error;
   }

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "-device_open");

   return( device );

error:

   bam_device_close( device );

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "-device_open = NULL");

   return( NULL );

}

/*==================================================================================================

FUNCTION:     bam_transfer

DESCRIPTION:

==================================================================================================*/

TAL_RESULT
bam_transfer( DMA_BAM_PIPE* pipe, TAL_DMA_DESCRIPTOR *descriptor )
{
   DALDEVICEID             id = pipe->device->uart_dal_id;
   const char*             error_msg;
   const char*             dir_string;
   bam_status_type         bam_status;
   uint16                  xfer_opts;



   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "+pipe_transfer()");

   dir_string = ( (pipe->properties.direction == BAM_DIR_PRODUCER) ? "to" : "from");

   //  More error checking.

   if( descriptor->bytes_to_transfer >= DMA_BAM_MAX_BUFFER_SIZE_BYTES )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR,
                       "-pipe_transfer() - failed:  Max buffer size reached." );
      return( TAL_ERROR );
   }

   //  Check pipe state.

   if( pipe->is_error )
   {
      error_msg = "Pipe in error state";
      goto error;
   }


   //  See if we have enough descriptors to transfer everything we need to.

   if( 0 == bam_pipe_getfreecount( pipe->handle ))
   {
      error_msg = "not enough descriptors to transfer";
      goto error;
   }

   //  As long as the buffer is less that DMA_BAM_MAX_BUFFER_SIZE_BYTES,
   //  we can transfer it with a single descriptor.  BAM does the
   //  negotiation with the UART FIFOs.
   //
   //  Set the transfer options.  We only care about when the entire
   //  transfer is finished, not any of the intermediate descriptors
   //  (if any).
   xfer_opts = BAM_IOVEC_FLAG_INT;
   if ( pipe->properties.direction == BAM_DIR_CONSUMER )
   {
      xfer_opts |= BAM_IOVEC_FLAG_EOT ;
   }

   //  Transfer from input physical memory location to the UART device (or
   //  vice-versa).
   pipe->client_descriptor = descriptor;
   pipe->transfer_complete = FALSE;
   bam_status = bam_pipe_transfer( pipe->handle,
                                   (uint32)descriptor->physical_address,
                                   descriptor->bytes_to_transfer,
                                   xfer_opts,
                                   NULL );

   if( bam_status != BAM_SUCCESS )
   {
      error_msg = "bam_pipe_transfer failed";
      goto error;
   }

   DALSYS_LogEvent( id,
                    DALSYS_LOGEVENT_INFO,
                    "pipe_transfer:  %s %d bytes %s 0x%08X",
                    pipe->name,
                    descriptor->bytes_to_transfer,
                    dir_string,
                    descriptor->physical_address );

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "-pipe_transfer()");

   return( TAL_SUCCESS );

error:

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "-pipe_transfer() - failed:  %s", error_msg );

   return( TAL_ERROR );
}

/*==================================================================================================

FUNCTION:     bam_transfer_cancel

DESCRIPTION:

==================================================================================================*/

static TAL_RESULT
bam_transfer_cancel(DMA_BAM_PIPE* pipe)
{
   DALDEVICEID          id;
   bam_status_type      bamstatus;

   id = pipe->device->uart_dal_id;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "+pipe_transfer_cancel(%s)", pipe->name );

   DALSYS_SyncEnter( pipe->lock );

   if (pipe->transfer_complete)
   {
      DALSYS_SyncLeave( pipe->lock );
      DALSYS_LogEvent( id,DALSYS_LOGEVENT_INFO,"-pipe_transfer_cancel(). Transfer Completed");
      return( TAL_SUCCESS );
   }
   pipe->transfer_complete = TRUE;

   DALSYS_SyncLeave( pipe->lock );

   pipe->client_descriptor->transfer_complete   = TRUE;
   pipe->client_descriptor->bytes_transferred   = 0;

   bamstatus = bam_pipe_disable(pipe->handle);

   if (bamstatus != BAM_SUCCESS)
   {
      DALSYS_LogEvent( id,DALSYS_LOGEVENT_ERROR,
                       "bam_pipe_disable failed." );
      goto error;

   }

   bamstatus = bam_pipe_enable(pipe->handle);
   if (bamstatus != BAM_SUCCESS)
   {
      DALSYS_LogEvent( id,DALSYS_LOGEVENT_ERROR,
                       "bam_pipe_enable failed." );
      goto error;
   }

   pipe->is_error = FALSE;
   pipe->client_descriptor->error               = pipe->is_error;
   pipe->client_descriptor                      = NULL;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "Invoking %s DMA callback.", pipe->name );

   pipe->callback( pipe->callback_ctxt );

   DALSYS_LogEvent( id,DALSYS_LOGEVENT_INFO,"-pipe_transfer_cancel()");
   return( TAL_SUCCESS );

error:
   pipe->is_error = TRUE;
   pipe->client_descriptor->error               = pipe->is_error;
   pipe->client_descriptor                      = NULL;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "Invoking %s DMA callback.", pipe->name );
   pipe->callback( pipe->callback_ctxt );

   DALSYS_LogEvent( id,DALSYS_LOGEVENT_INFO,"-pipe_transfer_cancel() - failed");
   return( TAL_ERROR );
}

/*==================================================================================================

FUNCTION:     bam_transfer_done

DESCRIPTION:

==================================================================================================*/

static void
bam_transfer_done( DMA_BAM_PIPE* pipe, uint32 xfer_bytes, boolean is_error )
{
   DALDEVICEID             id = pipe->device->uart_dal_id;

   //  Invoke the callback.
   //  Check if the transfers are cancelled.

   DALSYS_SyncEnter( pipe->lock );
   if ( pipe->transfer_complete )
   {
      DALSYS_SyncLeave( pipe->lock );
      return;
   }

   pipe->transfer_complete = TRUE;
   DALSYS_SyncLeave( pipe->lock );

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "+pipe_transfer_done(%s)", pipe->name );

   if( !pipe->is_error )
   {
      pipe->is_error   = is_error;
      pipe->client_descriptor->bytes_transferred = ( is_error ? 0 : xfer_bytes );
   }
   pipe->client_descriptor->transfer_complete   = TRUE;
   pipe->client_descriptor->error               = pipe->is_error;
   pipe->client_descriptor                      = NULL;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "Invoking %s DMA callback.", pipe->name );
   pipe->callback( pipe->callback_ctxt );
   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "-pipe_transfer_done(%s)", pipe->name );

}

/*==================================================================================================

FUNCTION:     device_event_callback_isr

DESCRIPTION:

==================================================================================================*/

static void
device_event_callback_isr( bam_result_type bam_result )
{
   TAL_DMA_CONTEXT*    device = (TAL_DMA_CONTEXT*) bam_result.cb_data;
   DALDEVICEID         id = device->uart_dal_id;
   const char*         event_str;

   event_str = get_event_str(bam_result.event);

   // Device call back ISR is called only for top level errors.

   DALSYS_LogEvent( id,DALSYS_LOGEVENT_ERROR, "device_event_callback_isr:  event %s",event_str );

   bam_transfer_done( device->rx, 0, TRUE );
   bam_transfer_done( device->tx, 0, TRUE );
}

/*==================================================================================================

FUNCTION:     device_get_dword_property

DESCRIPTION:

==================================================================================================*/

static boolean
device_get_dword_property( TAL_DMA_CONTEXT* device, DALSYSPropertyHandle handle,
                                          const char* prop_name, uint32* prop_val )
{
   DALDEVICEID             id = device->uart_dal_id;
   DALResult               dal_result;
   DALSYSPropertyVar       property;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "property %s", prop_name );
   dal_result = DALSYS_GetPropertyValue( handle, prop_name, 0, &property );
   if( dal_result != DAL_SUCCESS )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "property %s failed!", prop_name );
      return( FALSE );
   }

   *prop_val = property.Val.dwVal;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "property %s = 0x%08X", prop_name, *prop_val );

   return( TRUE );
}

/*==================================================================================================

FUNCTION:     device_get_properties

DESCRIPTION:

==================================================================================================*/

static boolean
device_get_properties( TAL_DMA_CONTEXT* device )
{
   DALResult               dal_result;
   DALSYS_PROPERTY_HANDLE_DECLARE(handle);


   //  Get BAM DMA related properties.

   dal_result = DALSYS_GetDALPropertyHandle( device->uart_dal_id, handle );
   if( dal_result != DAL_SUCCESS )
   {
      return ( FALSE );
   }

   if( !device_get_dword_property(device,handle,"BamBase",&device->properties.physical_addr) )
   {
      return ( FALSE );
   }

   if( !device_get_dword_property(device,handle,"BamIrq" ,&device->properties.irq) )
   {
      return ( FALSE );
   }

   return( TRUE );

}

/*==================================================================================================

FUNCTION:     get_event_str

DESCRIPTION:

==================================================================================================*/

static const char* get_event_str(bam_event_type event)
{
   switch (event)
   {
      case BAM_EVENT_INVALID:     return(  "INVALID"    );
      case BAM_EVENT_EOT:         return(  "EOT"        );
      case BAM_EVENT_DESC_DONE:   return(  "DESC_DONE"  );
      case BAM_EVENT_OUT_OF_DESC: return(  "OUT_OF_DESC");
      case BAM_EVENT_WAKEUP:      return(  "WAKEUP"     );
      case BAM_EVENT_FLOWOFF:     return(  "FLOWOFF"    );
      case BAM_EVENT_INACTIVE:    return(  "INACTIVE"   );
      case BAM_EVENT_ERROR:       return(  "ERROR"      );
      case BAM_EVENT_EMPTY:       return(  "EMPTY"      );
      case BAM_EVENT_IN_PROGRESS: return(  "IN_PROGRESS");
      default:                    return(  "(unknown)"  );
   }
}

/*==================================================================================================

FUNCTION:     pipe_close

DESCRIPTION:

==================================================================================================*/

static void
pipe_close(DMA_BAM_PIPE* pipe)
{
   if ( NULL == pipe )
   {
      return;
   }

   if( pipe->handle )
   {
      bam_pipe_deinit( pipe->handle );
   }

   if ( pipe->properties.descriptor_mem_handle)
   {
      DALSYS_DestroyObject(pipe->properties.descriptor_mem_handle);
   }

   if( pipe->lock )
   {
      DALSYS_DestroyObject( pipe->lock );
   }

   DALSYS_Free( pipe );
}

/*==================================================================================================

FUNCTION:     pipe_event_callback_isr

DESCRIPTION:

==================================================================================================*/

static void
pipe_event_callback_isr( bam_result_type bam_result )
{
   DMA_BAM_PIPE*            pipe;
   DALDEVICEID              id;
   uint32                   xfer_bytes;
   boolean                  is_eot;
   boolean                  is_error;
   const char*              event_str;

   pipe     = (DMA_BAM_PIPE*) bam_result.cb_data;
   id       = pipe->device->uart_dal_id;

   xfer_bytes = 0;
   is_error   = FALSE;
   is_eot     = FALSE;

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "+pipe_event_callback_isr(%s)", pipe->name );

   event_str = get_event_str(bam_result.event);

   //  Do as little processing as possible in ISR context, especially
   //  ISR context from another subsystem (BAM).

   switch( bam_result.event )
   {
      case BAM_EVENT_EOT:
         is_eot = TRUE;
         xfer_bytes = bam_result.data.xfer.accum_size_bytes;
         DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "%s - %s %d bytes.",event_str,
                                                                        pipe->name, xfer_bytes );
         break;

      case BAM_EVENT_DESC_DONE:
         DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "%s - %s .",event_str, pipe->name);
         break;

      default:
         DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "Error: Event %s.",event_str );
         is_error = TRUE;
         break;
   }

   //  Is the transfer done?  If so, signal the DAL layer
   //  so it can get results and start a new transfer.

   if( is_eot || is_error )
   {
      bam_transfer_done( pipe, xfer_bytes, is_error );
   }

   DALSYS_LogEvent( id, DALSYS_LOGEVENT_INFO, "-pipe_event_callback_isr()");
}

/*==================================================================================================

FUNCTION:     pipe_init

DESCRIPTION:

==================================================================================================*/

static boolean
pipe_init(DMA_BAM_PIPE* pipe,  boolean is_rx)
{
   DALDEVICEID              id = pipe->device->uart_dal_id  ;
   DALResult                dal_result                      ;
   bam_pipe_config_type     pipe_config                     ;
   bam_callback_type        pipe_callback                   ;
   bam_handle               pipe_handle                     ;
   DALSYSMemInfo            mem_info                        ;
   const char*              pipe_prop_name                  ;
   DALSYS_PROPERTY_HANDLE_DECLARE(handle)                   ;


   dal_result = DALSYS_GetDALPropertyHandle( pipe->device->uart_dal_id, handle );
   if( dal_result != DAL_SUCCESS )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR,
                       "DALSYS_GetDALPropertyHandle failed %d", dal_result );
      return ( FALSE );
   }

  //  Set the pipe direction.

   pipe->properties.direction = ( is_rx ? BAM_DIR_PRODUCER : BAM_DIR_CONSUMER );


   //  Set the pipe number.
   pipe_prop_name = ( is_rx ? "BamRxPipe" : "BamTxPipe" );
   if( !device_get_dword_property(pipe->device,handle,pipe_prop_name,&pipe->properties.number) )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR,
                       " device_get_dword_property failed - Property : %s",pipe_prop_name );
      return ( FALSE );
   }

   //  Allocate memory for the descriptor FIFO.

   dal_result = DALSYS_MemRegionAlloc( DALSYS_MEM_PROPS_UNCACHED | DALSYS_MEM_PROPS_PHYS_CONT,
                                       DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                       DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                       DMA_BAM_DESCRIPTOR_QUEUE_SIZE_BYTES,
                                       &pipe->properties.descriptor_mem_handle,
                                       NULL);

   if( dal_result != DAL_SUCCESS )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "DALSYS_MemRegionAlloc failed %d", dal_result );
      return( FALSE  ); // Error
   }

   //  Get information about the memory allocated for transfer descriptors.

   dal_result = DALSYS_MemInfo( pipe->properties.descriptor_mem_handle, &mem_info );
   if( dal_result != DAL_SUCCESS )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "DALSYS_MemInfo failed %d", dal_result );
      return( FALSE ); // Error
   }

   //  Set up the config parameters for the bam_pipe_init call.

   pipe_config.options      = BAM_O_ERROR | BAM_O_EOT       ;
   pipe_config.dir          = pipe->properties.direction    ;
   pipe_config.mode         = BAM_MODE_SYSTEM               ;
   pipe_config.desc_base_va = mem_info.VirtualAddr          ;
   pipe_config.desc_base_pa = mem_info.PhysicalAddr         ;
   pipe_config.desc_size    = mem_info.dwLen                ;
   pipe_config.evt_thresh   = 0;

   //  Set up the callback for the bam_pipe_init call.

   pipe_callback.func = pipe_event_callback_isr;
   pipe_callback.data = pipe;

   pipe_handle = bam_pipe_init( pipe->device->handle,
                                pipe->properties.number,
                                &pipe_config,
                                &pipe_callback );

   if( NULL == pipe_handle )
   {
      DALSYS_LogEvent( id, DALSYS_LOGEVENT_ERROR, "bam_pipe_init failed");
      return( FALSE ); // Error
   }

   pipe->handle     = pipe_handle       ;

   return( TRUE );

}


/*==================================================================================================

FUNCTION:     pipe_open

DESCRIPTION:

==================================================================================================*/

static DMA_BAM_PIPE*
pipe_open(TAL_DMA_CONTEXT* device, TAL_DMA_CONFIG* dma_config, boolean is_rx )
{
   DMA_BAM_PIPE*      pipe       = NULL ;
   DALResult          dal_result        ;


   //  Allocate pipe.
   dal_result = DALSYS_Malloc( sizeof( *pipe ), (void **) &pipe );
   if( dal_result != DAL_SUCCESS )
   {
      goto error;
   }

   memset( pipe, 0, sizeof( *pipe ));

   //  Set the pipe parameters.
   pipe->device        = device;
   pipe->name          = ( is_rx ? "RX" : "TX" );
   pipe->callback      = ( is_rx ? dma_config->rx_callback : dma_config->tx_callback );
   pipe->callback_ctxt = dma_config->cb_context;

   dal_result = DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE, &pipe->lock, NULL );
   if( dal_result != DAL_SUCCESS )
   {
      goto error;
   }

   // Initialize the pipe
   if( !pipe_init( pipe, is_rx ) )
   {
      goto error;
   }

   return( pipe );

error:
   // The Bam_device_open will call bam_device close in this case. So all pipes and the memory
   // allocated will be taken care of.
   return( NULL );
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION:     tal_dma_close

DESCRIPTION:

==================================================================================================*/

void tal_dma_close (TAL_DMA_HANDLE handle)
{
   bam_device_close(handle);
}

/*==================================================================================================

FUNCTION:     tal_dma_open

DESCRIPTION:

==================================================================================================*/

TAL_RESULT tal_dma_open(TAL_DMA_HANDLE *phandle, TAL_DMA_CONFIG *config)
{

   *phandle = bam_device_open(config);

   if ( NULL == *phandle )
   {
      return( TAL_ERROR );
   }

   return ( TAL_SUCCESS );
}


/*==================================================================================================

FUNCTION:     tal_dma_rx_cancel

DESCRIPTION:

==================================================================================================*/

TAL_RESULT tal_dma_rx_cancel (TAL_DMA_HANDLE handle)
{
   return (bam_transfer_cancel(handle->rx));
}


/*==================================================================================================

FUNCTION:     tal_dma_rx_transfer

DESCRIPTION:

==================================================================================================*/

TAL_RESULT tal_dma_rx_transfer(TAL_DMA_HANDLE handle, TAL_DMA_DESCRIPTOR *descriptor)
{
   return ( bam_transfer(handle->rx, descriptor) );
}


/*==================================================================================================

FUNCTION:     tal_dma_tx_cancel

DESCRIPTION:

==================================================================================================*/

TAL_RESULT tal_dma_tx_cancel  (TAL_DMA_HANDLE handle)
{
   return ( bam_transfer_cancel(handle->tx) );
}


/*==================================================================================================

FUNCTION:     tal_dma_tx_transfer

DESCRIPTION:

==================================================================================================*/

TAL_RESULT tal_dma_tx_transfer(TAL_DMA_HANDLE handle, TAL_DMA_DESCRIPTOR *descriptor)
{
   return ( bam_transfer(handle->tx, descriptor) );
}

