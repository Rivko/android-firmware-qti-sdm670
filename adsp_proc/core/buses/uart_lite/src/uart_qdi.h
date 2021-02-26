/*==================================================================================================

FILE: uart_qdi.h

DESCRIPTION: Defines the interface and functionality of a QDI Driver for the needed UART functions 
while in the micro image. 

                     Copyright (c) 2015,2017 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "qurt.h"
#include "uart.h"
#include "qurt_qdi_driver.h"
#include "err.h"

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/
#define UART_OPEN             (0 + QDI_PRIVATE)
#define UART_TRANSMIT         (1 + QDI_PRIVATE)
#define UART_RECEIVE          (2 + QDI_PRIVATE)
#define UART_WAIT_FOR_EVENT   (3 + QDI_PRIVATE)
#define UART_POWER_OFF        (4 + QDI_PRIVATE)
#define UART_POWER_ON         (5 + QDI_PRIVATE)
#define UART_CLOSE            (6 + QDI_PRIVATE)

/*==================================================================================================
                                             TYPEDEFS
==================================================================================================*/
typedef enum
{
   UART_PD_EVENT_TX_COMPLETION,
   UART_PD_EVENT_RX_COMPLETION,
   UART_PD_EVENT_WAKEUP,
   UART_PD_EVENT_MAX
}uart_pd_event;

typedef struct
{
   uart_pd_event           pd_event;
   UART_CALLBACK           client_tfr_cb;
   void                   *cb_data;
   UART_WAKEUP_CALLBACK    client_wakeup_cb;
   uint32                  bytes_transferred;
} uart_pd_event_cb_ctxt;
