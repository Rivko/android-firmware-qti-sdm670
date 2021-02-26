#ifndef __DALUART_H__
#define __DALUART_H__
/*==================================================================================================

FILE: DalUart.h

DESCRIPTION: UART DAL (OS-independent layer)

                           Copyright (c) 2009-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/dal/src/DalUart.h#1 $

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#else
#include "comdef.h"
#include "DALFramework.h"
#include "DALSysTypes.h"
#endif

#include "tal.h"
#include "DDIUart.h"
#include "HALuart.h"

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

#define STALE_DEFAULT  0x04  // characters (10 x bit time) until stale timeout

#define TX_BUF_SIZE    4096  // size of the TX buffer
                             // must be less than 65535 due to DMOV limitations

#define RX_BUF_SIZE    2048  // size of each RX buffer
                             // must be at least as large as RXFIFO due to UART HW bug that
                             // disallows writing a value to DMRX that is less than the number
                             // of bytes presently in RXFIFO

#define RX_BUF_CNT        4  // number of RX buffers - must be a power of 2

#define RXLEV_MARGIN     16  // number of empty bytes remaining in RXFIFO when an RXLEV interrupt
                             // fires (must be a multiple of 4)

#define TXLEV_MARGIN     16  // number of full bytes remaining in TXFIFO when a TXLEV interrupt
                             // fires (must be a multiple of 4)

#define AUTORFR_MARGIN    4  // number of empty bytes remaining in RXFIFO when we automatically
                             // deassert RTS (must be a multiple of 4 and less than RXLEV_MARGIN)

#define DMA_BURST_SIZE   16  // do not change; this is hard-wired to 16 in DMOV UARTs but
                             // configurable in BAM UARTs; we will use the same value for both

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   NO_TRIGGER,
   FALLING_EDGE,
   RISING_EDGE,
} RX_TRIGGER_TYPE;

typedef enum
{
   IRQ_RX_FALLING = (1 << 0),
   IRQ_RX_RISING  = (1 << 1),
} RX_EVENTS;

typedef enum
{
   IRQ_TXLEV         = (1 << 0),
   IRQ_RXSTALE       = (1 << 1),
   IRQ_RXLEV         = (1 << 2),
   IRQ_TX_READY      = (1 << 3),
   IRQ_RXBREAK_START = (1 << 4),
   DMA_RX_COMPLETE   = (1 << 5),
   DMA_TX_COMPLETE   = (1 << 6),
} UART_EVENTS;

typedef struct
{
   DALSYSMemHandle  data_handle;   // buffer (uncached in DMA mode) that contains RX data
   TAL_DMA_DESCRIPTOR  descriptor;    // buffer descriptor used for DMA operations
   uint32           read_offset;   // offset of next byte to read
   uint32           write_offset;  // offset of next byte to write
} MSM_UART_RX_BUF;

typedef struct
{
   TAL_INTERRUPT_HANDLE interrupt_handle;    // handle used for TAL interrupt operations
   TAL_TLMM_HANDLE      tlmm_handle;         // handle to TLMM device
   TAL_CLOCK_HANDLE     clock_handle;        // handle used for clock operations
   DALSYSSyncHandle     register_sync;       // protects UART Registers (enforces atomic access)
   DALSYSEventHandle    line_event;          // used for callback to client line event handler
   uint32               prop_uart_physical;  // (from XML props) UART physical base address
   uint32               prop_gsbi_physical;  // (from XML props) GSBI physical base address
   uint32               prop_gsbi_control;   // (from XML props) GSBI control register
   uint32               prop_detect_break;   // (from XML props) whether to detect RX break
   uint32               prop_is_cxm;         // (from XML props) whether UART supports coexistence
   uint32               prop_uart_irq;       // (from XML props) IRQ number assigned to the UART
   DALSYSMemHandle      uart_mem_handle;     // contains virtual address mapping for UART block
   DALSYSMemHandle      gsbi_mem_handle;     // contains virtual address mapping for GSBI block
   uint32               baud_rate;           // current HW baud rate
   UartRtsControlType   rts_control;         // client-specified inbound HW flow control setting
   boolean              clock_on;            // indicates whether the input clock is enabled
   UartPowerType        power_state;         // power state of the driver
   DALDEVICEID          dal_id;              // UART DAL device identifier
   HALUartObject        uart_hal;            // UART HAL object
   boolean              break_notified;      // last break notification was a break start
   boolean              break_halt;          // flag used to indicate break detection is halted
   HALUartType          uart_type;           // specifies the UART type (legacy/DM/BAM)
   boolean              use_dma;             // whether to use DMA transfers to/from FIFOs
   void                 *dma_ctxt;           // DMA context (opaque pointer)
   void                 *init_uart_irq;      // UART interrupt passed during initialization
   void                 *init_rx_irq;        // RX interrupt passed during initialization

   TAL_INTERRUPT_ID     uart_interrupt;      // TAL interrupt for UART generated events
   boolean              uart_isr_registered; // flag to track ISR registration
   uint32               uart_events;         // bitfield of pending UART DPC events

   TAL_INTERRUPT_ID     rx_interrupt;        // TAL interrupt for the raw RX signal
   boolean              rx_isr_registered;   // flag to track ISR registration
   RX_TRIGGER_TYPE      rx_trigger;          // trigger type of currently registered RX ISR
   uint32               rx_events;           // bitfield of pending RX DPC events

   DALSYSSyncHandle     tx_sync;             // protects transmit operations
   DALSYSEventHandle    tx_event;            // used for callback to client tx event handler
   uint32               tx_fifo_size;        // size of TXFIFO (in bytes)
   DALSYSMemHandle      tx_handle;           // buffer (uncached in DMA mode) that contains TX data
   TAL_DMA_DESCRIPTOR   tx_descriptor;       // buffer descriptor used for DMA operations
   const uint8          *tx_ptr;             // pointer to unsent data in TX buffer
   uint32               tx_bytes_left;       // unsent bytes remaining in TX buffer
   boolean              tx_in_progress;      // flag to indicate TX transaction is in progress
   boolean              tx_halt;             // flag used to halt a TX transfer in progress
   boolean              tx_wait;             // flag used to wait for a TX transfer to complete

   DALSYSSyncHandle     rx_sync;             // protects receive operations
   DALSYSEventHandle    rx_event;            // used for callback to client rx event handler
   uint32               rx_fifo_size;        // size of RXFIFO (in bytes)
   MSM_UART_RX_BUF      rx_buf[RX_BUF_CNT];  // array of buffers to hold incoming data until read
   uint32               rx_head;             // index of next RX buffer to be written
   uint32               rx_tail;             // index of next RX buffer to be read
   uint32               rx_out_of_buffers;   // no RX buffer is available for writing
   uint32               rx_watermark;        // current RX watermark (expressed in bytes)
   uint32               rx_xfer_size;        // number of bytes in current RX transfer
   uint32               rx_overruns;         // count of RXFIFO overrun events detected
   uint32               rx_cached_status;    // cached RX status (overrun/parity/framing/break)
   boolean              rx_activated;        // the first RX transfer has been activated
   boolean              rx_halt;             // flag used to halt an RX transfer in progress
} UART_CONTEXT;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

DALResult Uart_DeviceInit        (uint32 ClientHandle, DalDeviceHandle *);
DALResult Uart_DeviceDeInit      (uint32 ClientHandle, DalDeviceHandle *);
DALResult Uart_Open              (uint32 ClientHandle, DalDeviceHandle *, uint32);
DALResult Uart_Close             (uint32 ClientHandle, DalDeviceHandle *);
DALResult Uart_Info              (uint32 ClientHandle, DalDeviceHandle *, DalDeviceInfo *, uint32);
DALResult Uart_PowerEvent        (uint32 ClientHandle, DalDeviceHandle *, DalPowerCmd, DalPowerDomain);

DALResult Uart_PostInit          (uint32 ClientHandle, DalDeviceHandle *, UartInitConfig *);
DALResult Uart_OpenEx            (uint32 ClientHandle,DalDeviceHandle *, UartOpenConfig *);
DALResult Uart_Read              (uint32 ClientHandle,DalDeviceHandle *, uint8 *, uint32, uint32 *, uint32 *);
DALResult Uart_Write             (uint32 ClientHandle,DalDeviceHandle *, const uint8 *, uint32, uint32 *);
DALResult Uart_SetRate           (uint32 ClientHandle,DalDeviceHandle *, uint32);
DALResult Uart_SetCharFormat     (uint32 ClientHandle,DalDeviceHandle *, UartCharFormat *);
DALResult Uart_PurgeTx           (uint32 ClientHandle,DalDeviceHandle *);
DALResult Uart_PurgeRx           (uint32 ClientHandle,DalDeviceHandle *);
DALResult Uart_SetBreak          (uint32 ClientHandle,DalDeviceHandle *);
DALResult Uart_ClearBreak        (uint32 ClientHandle,DalDeviceHandle *);
DALResult Uart_TxSingleChar      (uint32 ClientHandle,DalDeviceHandle *, uint8);
DALResult Uart_GetStatus         (uint32 ClientHandle,DalDeviceHandle *, uint32 *);
DALResult Uart_TxComplete        (uint32 ClientHandle,DalDeviceHandle *, uint32 *);
DALResult Uart_SetRtsControl     (uint32 ClientHandle,DalDeviceHandle *, UartRtsControlType);
DALResult Uart_SetCtsControl     (uint32 ClientHandle,DalDeviceHandle *, UartCtsControlType);
DALResult Uart_GetCTS            (uint32 ClientHandle,DalDeviceHandle *, uint32 *);
DALResult Uart_PowerCapabilities (uint32 ClientHandle,DalDeviceHandle *, uint32 *);
DALResult Uart_PowerGet          (uint32 ClientHandle,DalDeviceHandle *, UartPowerType *);
DALResult Uart_PowerSet          (uint32 ClientHandle,DalDeviceHandle *, UartPowerType);
DALResult Uart_LoopbackSet       (uint32 ClientHandle,DalDeviceHandle *, uint32);
DALResult Uart_SetCxm            (uint32 ClientHandle,DalDeviceHandle *, uint32, uint32);
DALResult Uart_DumpRegs          (uint32 ClientHandle,DalDeviceHandle *);
DALResult Uart_GetCxmTxSticky    (uint32 ClientHandle,DalDeviceHandle *, uint32 *, uint32);
DALResult Uart_CxmTxDirectChar   (uint32 ClientHandle,DalDeviceHandle *, uint8);
DALResult Uart_RxActive          (uint32 ClientHandle,DalDeviceHandle *, uint32 *);

#endif  // __DALUART_H__
