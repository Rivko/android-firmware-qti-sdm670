#ifndef __DDIUART_H__
#define __DDIUART_H__
/*==================================================================================================

FILE: DDIUart.h

DESCRIPTION: External interface to the UART DAL

                           Copyright (c) 2009-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/dal/src/DDIUart.h#1 $

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#else
#include "DalDevice.h"
#endif

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

#define DALUART_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

// must agree in value with HAL_UART_SIZE_TO_32_BITS
#define DAL_UART_SIZE_TO_32_BITS 0x12345678

#define DALDEVICEID_UARTBAM_DEVICE_1   0x02001000
#define DALDEVICEID_UARTBAM_DEVICE_2   0x02001001
#define DALDEVICEID_UARTBAM_DEVICE_3   0x02001002
#define DALDEVICEID_UARTBAM_DEVICE_4   0x02001003
#define DALDEVICEID_UARTBAM_DEVICE_5   0x02001004
#define DALDEVICEID_UARTBAM_DEVICE_6   0x02001005
#define DALDEVICEID_UARTBAM_DEVICE_7   0x02001006
#define DALDEVICEID_UARTBAM_DEVICE_8   0x02001007
#define DALDEVICEID_UARTBAM_DEVICE_9   0x02001008
#define DALDEVICEID_UARTBAM_DEVICE_10  0x02001009
#define DALDEVICEID_UARTBAM_DEVICE_11  0x0200100A
#define DALDEVICEID_UARTBAM_DEVICE_12  0x0200100B
#define DALDEVICEID_UARTCXM            0x0200100C

/*==================================================================================================
                                               MACROS
==================================================================================================*/

#define DAL_UartDeviceAttach(DevId, hDalDevice) \
        DAL_DeviceAttachEx(NULL, DevId, DALUART_INTERFACE_VERSION, hDalDevice)

/*==================================================================================================
                                            ENUMERATIONS
==================================================================================================*/

typedef enum
{
    POWER_INVALID   = 0,
    POWER_FULL      = (1<<0),
    POWER_075       = (1<<1),
    POWER_050       = (1<<2),
    POWER_025       = (1<<3),
    POWER_OFF       = (1<<4),
    _QIDL_PLACEHOLDER_UartPowerType = DAL_UART_SIZE_TO_32_BITS
} UartPowerType;

// line events a client may wait upon to be notified by
typedef enum
{
    EVENT_NONE          = 0,
    EVENT_BREAK         = (1<<0),       // continuous Space value w/o framing detected on input
    EVENT_CTS           = (1<<1),       // CTS changed state
    EVENT_DSR           = (1<<3),       // DSR changed state
    EVENT_ERROR         = (1<<4),       // Line-status error (PARITY, FRAMING, OVERRUN) occurred
    EVENT_RX_WAKEUP     = (1<<5),       // a falling edge (wakeup byte) was detected while sleeping
    EVENT_RING          = (1<<7),
    EVENT_RLSD          = (1<<8),       // RLSD changed state
    EVENT_RX_CHAR       = (1<<9),       // one or more RX chars have been received
    EVENT_RX_HUNTCHAR   = (1<<10),      // the hunt char has been received
    EVENT_TXEMPTY       = (1<<11),      // last char was moved from TXFIFO to unpacking buffer
    EVENT_BREAK_END     = (1<<12),      // End of break condition
    _QIDL_PLACEHOLDER_UartLineEventType = DAL_UART_SIZE_TO_32_BITS
} UartLineEventType;

// current status of device, reset on read by Uart_GetStatus
typedef enum
{
    STATUS_RX_PARITY_ERROR          = (1<<0),
    STATUS_RX_FRAME_ERROR           = (1<<1),
    STATUS_RX_OVERRUN_ERROR         = (1<<2),
    STATUS_RX_BREAK                 = (1<<3),
    STATUS_RX_HUNT_CHAR_DETECTED    = (1<<4),
    STATUS_RX_FULL                  = (1<<5),
    STATUS_RX_READY                 = (1<<6),
    STATUS_TX_EMPTY                 = (1<<7),
    STATUS_TX_READY                 = (1<<8),
    STATUS_RING_ASSERTED            = (1<<16),
    STATUS_RLSD_ASSERTED            = (1<<17),
    STATUS_DSR_ASSERTED             = (1<<18),
    STATUS_CTS_ASSERTED             = (1<<19),
    _QIDL_PLACEHOLDER_UartStatusType = DAL_UART_SIZE_TO_32_BITS
} UartStatusType;

typedef enum
{
    RTS_DEASSERT,  // manually deassert RTS
    RTS_ASSERT,    // manually assert RTS
    RTS_AUTO,      // RTS automatically controlled by HW (based on level of RX FIFO)
    _QIDL_PLACEHOLDER_UartRtsControlType = DAL_UART_SIZE_TO_32_BITS
} UartRtsControlType;

typedef enum
{
    CTS_DISABLE,  // UART transmitter will ignore CTS
    CTS_ENABLE,   // UART transmitter will automatically halt when CTS is deasserted
    _QIDL_PLACEHOLDER_UartCtsControlType = DAL_UART_SIZE_TO_32_BITS
} UartCtsControlType;

// =============== redefine HAL types for use in UART DAL DDI =================
// must be identical to enums in HALuart.h but including HALuart.h here will
// require it for clients which want to use UART's DAL

typedef enum
{
  UART_5_BITS_PER_CHAR  = (1<<0),
  UART_6_BITS_PER_CHAR  = (1<<1),
  UART_7_BITS_PER_CHAR  = (1<<2),
  UART_8_BITS_PER_CHAR  = (1<<3),

  UART_INVALID_BITS_PER_CHAR = 0x7FFFFFFF
} UartBitsPerCharType;

typedef enum
{
  UART_0_5_STOP_BITS    = (1<<0),
  UART_1_0_STOP_BITS    = (1<<1),
  UART_1_5_STOP_BITS    = (1<<2),
  UART_2_0_STOP_BITS    = (1<<3),

  UART_INVALID_STOP_BITS = 0x7FFFFFFF
} UartNumStopBitsType;

typedef enum
{
  UART_NO_PARITY        = (1<<0),
  UART_ODD_PARITY       = (1<<1),
  UART_EVEN_PARITY      = (1<<2),
  UART_SPACE_PARITY     = (1<<3),

  UART_INVALID_PARITY = 0x7FFFFFFF
} UartParityModeType;

/*==================================================================================================
                                             STRUCTURES
==================================================================================================*/

typedef struct
{
   UartParityModeType  ParityMode;
   UartBitsPerCharType BitsPerChar;
   UartNumStopBitsType NumStopBits;
} UartCharFormat;

// The IRQs are opaque data types that describe interrupt sources.  Each IRQ is cast to
// (void *) while passing through the UART DAL layer and recast to the appropriate data type
// inside the OS-specific interrupt controller.  The OS-specific data types are as follows:
//
// WM6 - SYSINTR mapped from a HAL IRQ
// WM7 - HAL IRQ (HAL_PIC_xxx_IRQ)
// AMSS - the irq enumerated in the tramp table
// Symbian - same as the value used by Symbian kernel class Interrupt,
//           as defined in the enum TMSMHwInterruptId
// WOA - pointer to struct that encapsulates vector, IRQL, CPU affinity, etc.

typedef struct
{
   void *uart_irq;                                         // IRQ for UART generated events
   void *rx_irq;                                           // IRQ for the raw RX signal
   uint32 enable_dma;                                      // used to request DMA mode if available
   void *Ctxt;                                             // context to pass to interrupt handlers
   void (*LineEventNotif)(void *Ctxt, uint32 LineEvents);  // interrupt handler for line events
   void (*TransmitReadyNotif)(void *Ctxt);                 // interrupt handler for tx events
   void (*ReceiveReadyNotif)(void *Ctxt);                  // interrupt handler for rx events
} UartInitConfig;

typedef struct
{
   uint32             baud_rate;    // baud rate to use when opening the port
   UartCharFormat     char_format;  // character format to use when opening the port
   UartRtsControlType rts_control;  // initial RTS state
   UartCtsControlType cts_control;  // whether to obey CTS during transmit operations
} UartOpenConfig;

typedef struct
{
   DalDevice DalDevice;
   DALResult (*PostInit)           (uint32 ClientHandle, DalDeviceHandle *h, UartInitConfig *init_cfg);
   DALResult (*OpenEx)             (uint32 ClientHandle, DalDeviceHandle *h, UartOpenConfig *open_cfg);
   DALResult (*Read)               (uint32 ClientHandle, DalDeviceHandle *h, uint8 *read_buffer, uint32 buffer_len,
                                                        uint32 *bytes_read, uint32 *overrun_events);
   DALResult (*Write)              (uint32 ClientHandle, DalDeviceHandle *h, const uint8 *write_buffer,
                                                        uint32 buffer_len, uint32 *bytes_written);
   DALResult (*SetRate)            (uint32 ClientHandle, DalDeviceHandle *h, uint32 baud_rate);
   DALResult (*SetCharFormat)      (uint32 ClientHandle, DalDeviceHandle *h, UartCharFormat *char_format);
   DALResult (*PurgeTx)            (uint32 ClientHandle, DalDeviceHandle *h);
   DALResult (*PurgeRx)            (uint32 ClientHandle, DalDeviceHandle *h);
   DALResult (*SetBreak)           (uint32 ClientHandle, DalDeviceHandle *h);
   DALResult (*ClearBreak)         (uint32 ClientHandle, DalDeviceHandle *h);
   DALResult (*TxSingleChar)       (uint32 ClientHandle, DalDeviceHandle *h, uint8 character);
   DALResult (*GetStatus)          (uint32 ClientHandle, DalDeviceHandle *h, uint32 *status);
   DALResult (*TxComplete)         (uint32 ClientHandle, DalDeviceHandle *h, uint32 *tx_complete);
   DALResult (*SetRtsControl)      (uint32 ClientHandle, DalDeviceHandle *h, UartRtsControlType rts_control);
   DALResult (*SetCtsControl)      (uint32 ClientHandle, DalDeviceHandle *h, UartCtsControlType cts_control);
   DALResult (*GetCTS)             (uint32 ClientHandle, DalDeviceHandle *h, uint32 *cts_state);
   DALResult (*PowerCapabilities)  (uint32 ClientHandle, DalDeviceHandle *h, uint32 *power_mask);
   DALResult (*PowerGet)           (uint32 ClientHandle, DalDeviceHandle *h, UartPowerType *power_state);
   DALResult (*PowerSet)           (uint32 ClientHandle, DalDeviceHandle *h, UartPowerType power_state);
   DALResult (*LoopbackSet)        (uint32 ClientHandle, DalDeviceHandle *h, uint32 enabled);
   DALResult (*SetCxm)             (uint32 ClientHandle, DalDeviceHandle *h, uint32 enabled, uint32 sam);
   DALResult (*DumpRegs)           (uint32 ClientHandle, DalDeviceHandle *h);
   DALResult (*GetCxmTxSticky)     (uint32 ClientHandle, DalDeviceHandle *h, uint32 *tx_sticky, uint32 clear);
   DALResult (*CxmTxDirectChar)    (uint32 ClientHandle, DalDeviceHandle *h, uint8 character);
   DALResult (*RxActive)           (uint32 ClientHandle, DalDeviceHandle *h, uint32 *rx_active); 
} UartInterface;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*!
 * \brief Finalizes UART device initialization.
 *
 * This is an extended initialization function that allows the passage of an input parameter.
 * It performs one-time system initializations such as allocating system resources and
 * registering for interrupts.
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in init_cfg Pointer to configuration settings
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_PostInit(DalDeviceHandle *h, UartInitConfig *init_cfg)
{
   return ((UartInterface *)h->pVtbl)->PostInit(DALDEVICE_LOCAL_CLIENTID, h, init_cfg);
}

/*!
 * \brief Opens the UART
 *
 * This is an extended open function that allows the passage of an input parameter.
 * It will turn on the HW using the input settings.
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in open_cfg Pointer to configuration settings
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note all fields must be specified in the input settings
 */
static __inline DALResult
DalUart_OpenEx(DalDeviceHandle *h, UartOpenConfig *open_cfg)
{
   return ((UartInterface *)h->pVtbl)->OpenEx(DALDEVICE_LOCAL_CLIENTID, h, open_cfg);
}

/*!
 * \brief Retrieves data available in receive buffer.
 *
 * Should be called from client in response to call of client's registered
 * receive-ready notification callback.
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in overrun_events Buffer to be filled in with number of receive overrun events
 * \param in read_buffer Buffer to be filled with received data
 * \param in buffer_len Length of buffer pointed to by pReadBuffer
 * \param in bytes_read Buffer to be filled with length of data actually read
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_Read(DalDeviceHandle *h, uint8 *read_buffer, uint32 buffer_len,
             uint32 *bytes_read, uint32 *overrun_events)
{
   return ((UartInterface *)h->pVtbl)->Read(DALDEVICE_LOCAL_CLIENTID, h, read_buffer, buffer_len, bytes_read, overrun_events);
}

/*!
 * \brief Sends data from supplied buffer.
 *
 * Should be called from client for initial transmission of a quantity of data
 * and in response to call of client's registered transmit-ready notification
 * callback for any follow-on transmissions
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in write_buffer Buffer containing data to be sent
 * \param in buffer_len Quantity of data pointed to by write_buffer
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_Write(DalDeviceHandle *h, const uint8 *write_buffer, uint32 buffer_len,
              uint32 *bytes_written)
{
   return ((UartInterface *)h->pVtbl)->Write(DALDEVICE_LOCAL_CLIENTID, h, write_buffer, buffer_len, bytes_written);
}

/*!
 * \brief Sets the current baud rate for device
 *
 * Sets the current baud rate, changing the device's clock source and divisor setting as needed
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in BaudRate one of supported UartBaudRateType
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_SetRate(DalDeviceHandle *h, uint32 baud_rate)
{
   return ((UartInterface *)h->pVtbl)->SetRate(DALDEVICE_LOCAL_CLIENTID, h, baud_rate);
}

/*!
 * \brief Sets the character format parameters for device
 *
 * Sets the character format parameters (UartBitsPerCharType, UartNumStopBitsType,
 * UartParityModeType) to one of supported values
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in char_format pointer to a UartCharFormat type to be filled in
 * \param in dwCharFormatLen size of UartCharFormat
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_SetCharFormat(DalDeviceHandle *h, UartCharFormat *char_format)
{
   return ((UartInterface *)h->pVtbl)->SetCharFormat(DALDEVICE_LOCAL_CLIENTID, h, char_format);
}

/*!
 * \brief Purges device's transmitter
 *
 * Purges the device's transmit FIFO and packing buffer
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_PurgeTx(DalDeviceHandle *h)
{
   return ((UartInterface *)h->pVtbl)->PurgeTx(DALDEVICE_LOCAL_CLIENTID, h);
}

/*!
 * \brief Purges device's receiver
 *
 * Purges the device's receive FIFO and unpacking buffer
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_PurgeRx(DalDeviceHandle *h)
{
   return ((UartInterface *)h->pVtbl)->PurgeRx(DALDEVICE_LOCAL_CLIENTID, h);
}

/*!
 * \brief Forces the transmitter signal low
 *
 * The transmitter must be enabled. If the transmitter is busy, the break is
 * started when all characters in the transmit FIFO and the transmit shift
 * register have been completely sent
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_SetBreak(DalDeviceHandle *h)
{
   return ((UartInterface *)h->pVtbl)->SetBreak(DALDEVICE_LOCAL_CLIENTID, h);
}

/*!
 * \brief Causes the transmitter signal to go high
 *
 * If executed while channel is breaking, this command causes the transmitter
 * signal to go high. The signal remains high for at least one bit time before
 * sending out a new character.
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_ClearBreak(DalDeviceHandle *h)
{
   return ((UartInterface *)h->pVtbl)->ClearBreak(DALDEVICE_LOCAL_CLIENTID, h);
}

/*!
 * \brief Sends a single character out the transmit port
 *
 * Usually used for XON/XOFF software flow control characters
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in dwChar the character to send
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning This function will not send the character immediately but only after
 *   all data currently enqueued
 */
static __inline DALResult
DalUart_TxSingleChar(DalDeviceHandle *h, uint8 character)
{
   return ((UartInterface *)h->pVtbl)->TxSingleChar(DALDEVICE_LOCAL_CLIENTID, h, character);
}

/*!
 * \brief Gets the status of the UART device
 *
 * Gets the status of the UART as an ORed combination of applicable UartStatusType values
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in status pointer to a uint32 to be filled with the status value
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_GetStatus(DalDeviceHandle *h, uint32 *status)
{
   return ((UartInterface *)h->pVtbl)->GetStatus(DALDEVICE_LOCAL_CLIENTID, h, status);
}

/*!
 * \brief Gets a boolean indicating if transmission is complete
 *
 * A value of true indicates the stop bit at the end of the last character from
 * the transmit FIFO has been sent. It's reset to false when a character is written
 * to the TX fifo
 *
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in tx_complete pointer to a uint32 to be filled with the boolean value
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_TxComplete(DalDeviceHandle *h, uint32 *tx_complete)
{
   return ((UartInterface *)h->pVtbl)->TxComplete(DALDEVICE_LOCAL_CLIENTID, h, tx_complete);
}

/*!
 * \brief Sets the current type of RTS control (rx flow control)
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in rts_control the type of RTS control
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_SetRtsControl(DalDeviceHandle *h, UartRtsControlType rts_control)
{
   return ((UartInterface *)h->pVtbl)->SetRtsControl(DALDEVICE_LOCAL_CLIENTID, h, rts_control);
}

/*!
 * \brief Sets the current type of CTS control (tx flow control)
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in cts_control the type of CTS control
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_SetCtsControl(DalDeviceHandle *h, UartCtsControlType cts_control)
{
   return ((UartInterface *)h->pVtbl)->SetCtsControl(DALDEVICE_LOCAL_CLIENTID, h, cts_control);
}

/*!
 * \brief Gets current state of CTS
 * \param in DalDeviceHandle Handle to UART returned by attach
 * \param in cts_state is pointer to current CTS state
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
static __inline DALResult
DalUart_GetCTS(DalDeviceHandle *h, uint32 *cts_state)
{
   return ((UartInterface *)h->pVtbl)->GetCTS(DALDEVICE_LOCAL_CLIENTID, h, cts_state);
}

/*!
 * \brief Gets a list of supported power states
 * \param in  DalDeviceHandle handle to UART returned by attach
 * \param out power_mask      pointer to a mask of supported power states
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_PowerCapabilities(DalDeviceHandle *h, uint32 *power_mask)
{
   return ((UartInterface *)h->pVtbl)->PowerCapabilities(DALDEVICE_LOCAL_CLIENTID, h, power_mask);
}

/*!
 * \brief Gets the current power state
 * \param in  DalDeviceHandle handle to UART returned by attach
 * \param out power_state     pointer to location to write the current power state
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_PowerGet(DalDeviceHandle *h, UartPowerType *power_state)
{
   return ((UartInterface *)h->pVtbl)->PowerGet(DALDEVICE_LOCAL_CLIENTID, h, power_state);
}

/*!
 * \brief Requests a change in the power state
 * \param in  DalDeviceHandle handle to UART returned by attach
 * \param in  power_state     requested power state
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_PowerSet(DalDeviceHandle *h, UartPowerType power_state)
{
   return ((UartInterface *)h->pVtbl)->PowerSet(DALDEVICE_LOCAL_CLIENTID, h, power_state);
}

/*!
 * \brief Requests a change in the loopback state
 * \param in  DalDeviceHandle handle to UART returned by attach
 * \param in  enabled         TRUE to enable internal loopback, FALSE to disable
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_LoopbackSet(DalDeviceHandle *h, uint32 enabled)
{
   return ((UartInterface *)h->pVtbl)->LoopbackSet(DALDEVICE_LOCAL_CLIENTID, h, enabled);
}

/*!
 * \brief Enables or disables two-wire coexistence
 * \param in  DalDeviceHandle handle to UART returned by attach
 * \param in  enabled         TRUE to enable two-wire coexistence, FALSE to disable
 * \param in  sam             The station address included in certain CXM messages
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_SetCxm(DalDeviceHandle *h, uint32 enabled, uint32 sam)
{
   return ((UartInterface *)h->pVtbl)->SetCxm(DALDEVICE_LOCAL_CLIENTID, h, enabled, sam);
}

/*!
 * \brief Prints the contents of the UART hardware registers to the UART log
 * for debugging
 * \param in  DalDeviceHandle handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_DumpRegs(DalDeviceHandle *h)
{
   return ((UartInterface *)h->pVtbl)->DumpRegs(DALDEVICE_LOCAL_CLIENTID, h);
}

/*!
 * \brief Reads and optionally clears the 802.11 TX sticky flag
 * \param in  DalDeviceHandle handle to UART returned by attach
 * \param out tx_sticky       Returns the value of the TX sticky flag
 * \param in  clear           TRUE to clear the TX sticky flag
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_GetCxmTxSticky(DalDeviceHandle *h, uint32 *tx_sticky, uint32 clear)
{
   return ((UartInterface *)h->pVtbl)->GetCxmTxSticky(DALDEVICE_LOCAL_CLIENTID, h, tx_sticky, clear);
}

/*!
 * \brief Writes a byte to direct TX register of CXM UART for immediate transfer
 * \param in  DalDeviceHandle handle to UART returned by attach
 * \param in  character       Data byte to be sent immediately
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_CxmTxDirectChar(DalDeviceHandle *h, uint8 character)
{
   return ((UartInterface *)h->pVtbl)->CxmTxDirectChar(DALDEVICE_LOCAL_CLIENTID, h, character);
}

/*!
 * \brief Gets a boolean indicating if there is an active RX transfer
 * \param in DalDeviceHandle handle to UART returned by attach
 * \param in rx_active       pointer to a uint32 to be filled with the boolean value
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
static __inline DALResult
DalUart_RxActive(DalDeviceHandle *h, uint32 *rx_active)
{
   return ((UartInterface *)h->pVtbl)->RxActive(DALDEVICE_LOCAL_CLIENTID, h, rx_active);
}
#endif  // __DDIUART_H__
