#ifndef __UART_SETTINGS_H__
#define __UART_SETTINGS_H__
/*==================================================================================================

FILE: Uart.h

DESCRIPTION: UEFI driver for UARTDM/UARTBAM

                              Copyright (c) 2013 Qualcomm Technologies, Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
#include "UartApi.h"
#include "UartDefs.h"

uart_result uart_settings_get_fw(uint8** base_ptr, uint32 version);
UART_PROPERTIES* uart_settings_get_properties(uart_port_id port);

#endif  // __UART_TARGET_H__
