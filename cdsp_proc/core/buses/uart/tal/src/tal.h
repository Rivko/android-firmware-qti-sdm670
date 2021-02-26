#ifndef __TAL_H__
#define __TAL_H__
/*==================================================================================================

FILE: tal.h

DESCRIPTION: This header file defines the API for the Target Abstraction Layer.  This is a
             shim layer between the UART DAL layer and the external services required by
             the UART DAL.  Strictly speaking, this layer is redundant since the various
             DAL implementations were intended to provide a consistent interface on all
             targets and all OSes.  However, in practice this goal has failed.  DAL has not
             been implemented on all OSes and there have been differences in the DAL APIs
             between OSes.  This layer is intended to insulate the UART DAL layer from these
             issues.  A "stock" implementation of this layer will be defined that assumes
             all DAL APIs are present and consistent.  On targets and OSes where this
             assumption is false this layer will be customized as necessary.

                     Copyright (c) 2012-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/tal/src/tal.h#1 $

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#if ( !defined(_WIN32) )
#include "comdef.h"
#endif
/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

#define TAL_SUCCESS  0  // no error
#define TAL_ERROR   -1  // general failure

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef int TAL_RESULT;
typedef struct tal_clock_context     *TAL_CLOCK_HANDLE;
typedef struct tal_dma_context       *TAL_DMA_HANDLE;
typedef struct tal_interrupt_context *TAL_INTERRUPT_HANDLE;
typedef struct tal_tlmm_context      *TAL_TLMM_HANDLE;

typedef void (*TAL_DMA_CALLBACK)(void *);

typedef struct
{
   uint32            client_id;    // DAL ID of the DMA client
   void             *rx_fifo;      // RX FIFO physical address
   void             *tx_fifo;      // TX FIFO physical address
   uint32            burst_size;   // number of bytes in DMA burst
   TAL_DMA_CALLBACK  rx_callback;  // callback function for RX DMA events
   TAL_DMA_CALLBACK  tx_callback;  // callback function for TX DMA events
   void *            cb_context;   // context to pass to callbacks
} TAL_DMA_CONFIG;

typedef struct
{
   void    *physical_address;
   uint32   bytes_to_transfer;
   uint32   bytes_transferred;
   boolean  error;
   boolean  transfer_complete;
} TAL_DMA_DESCRIPTOR;

typedef struct
{
   void   *irq;   // opaque data type representing a global system interrupt
   uint32  gpio;  // GPIO line used as an interrupt source
} TAL_INTERRUPT_ID;

typedef enum
{
   TAL_INTERRUPT_TRIGGER_HIGH,
   TAL_INTERRUPT_TRIGGER_LOW,
   TAL_INTERRUPT_TRIGGER_RISING,
   TAL_INTERRUPT_TRIGGER_FALLING,
} TAL_INTERRUPT_TRIGGER;

typedef void * (*TAL_INTERRUPT_ISR)(void *);
typedef void * (*TAL_INTERRUPT_DPC)(void *);

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

void       tal_clock_close        (TAL_CLOCK_HANDLE handle);
TAL_RESULT tal_clock_disable      (TAL_CLOCK_HANDLE handle);
TAL_RESULT tal_clock_enable       (TAL_CLOCK_HANDLE handle);
TAL_RESULT tal_clock_open         (TAL_CLOCK_HANDLE *phandle, uint32 client_id);
TAL_RESULT tal_clock_set_frequency(TAL_CLOCK_HANDLE handle, uint32 input_freq);

void       tal_dma_close      (TAL_DMA_HANDLE handle);
TAL_RESULT tal_dma_open       (TAL_DMA_HANDLE *phandle, TAL_DMA_CONFIG *config);
TAL_RESULT tal_dma_rx_cancel  (TAL_DMA_HANDLE handle);
TAL_RESULT tal_dma_rx_transfer(TAL_DMA_HANDLE handle, TAL_DMA_DESCRIPTOR *descriptor);
TAL_RESULT tal_dma_tx_cancel  (TAL_DMA_HANDLE handle);
TAL_RESULT tal_dma_tx_transfer(TAL_DMA_HANDLE handle, TAL_DMA_DESCRIPTOR *descriptor);

void       tal_interrupt_close       (TAL_INTERRUPT_HANDLE handle);
void       tal_interrupt_deregister  (TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt);

void       tal_interrupt_dpc_disable (TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt);
void       tal_interrupt_dpc_enable  (TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt);
void       tal_interrupt_dpc_schedule(TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt);
TAL_RESULT tal_interrupt_open        (TAL_INTERRUPT_HANDLE *phandle, uint32 client_id);
TAL_RESULT tal_interrupt_register    (TAL_INTERRUPT_HANDLE handle,
                                      TAL_INTERRUPT_ID *interrupt, TAL_INTERRUPT_TRIGGER trigger,
                                      TAL_INTERRUPT_ISR isr, TAL_INTERRUPT_DPC dpc, void *ctxt);
void       tal_interrupt_sync_enter  (TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt);
void       tal_interrupt_sync_exit   (TAL_INTERRUPT_HANDLE handle, TAL_INTERRUPT_ID *interrupt);

void       tal_tlmm_close          (TAL_TLMM_HANDLE handle);
TAL_RESULT tal_tlmm_get_rx_gpio_num(TAL_TLMM_HANDLE handle, uint32 *gpio_num);
TAL_RESULT tal_tlmm_gpio_disable   (TAL_TLMM_HANDLE handle);
TAL_RESULT tal_tlmm_gpio_enable    (TAL_TLMM_HANDLE handle);
TAL_RESULT tal_tlmm_open           (TAL_TLMM_HANDLE *phandle, uint32 client_id);

#endif  // __TAL_H__
