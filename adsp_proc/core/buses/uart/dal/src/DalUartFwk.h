#ifndef __DALUARTFWK_H__
#define __DALUARTFWK_H__
/*==================================================================================================

FILE: DalUartFwk.h

DESCRIPTION: Interface between UART driver and DAL framework

                              Copyright (c) 2009 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.qdsp6/2.1/buses/uart/dal/src/DalUartFwk.h#1 $

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#else
#include "DALFramework.h"
#endif

#include "DalUart.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef struct
{
   // base members (MUST match DalDeviceHandle)
   uint32               dwDalHandleId;
   const UartInterface  *pVtbl;
   DALClientCtxt        *pClientCtxt;

   // driver-specific extensions
   DALClientCtxt        client_ctxt;
   DALDevCtxt           device_ctxt;
   UART_CONTEXT         *uart_ctxt;
} DalUartHandle;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

void      fwk_sleep_ms(uint32 duration);
DALResult fwk_wait_event(DALSYSEventHandle caller_event, uint32 timeout_ms);

#endif  // __DALUARTFWK_H__
