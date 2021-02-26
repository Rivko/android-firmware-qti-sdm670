#ifndef __UART_H__
#define __UART_H__
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

RETURN_STATUS EFIAPI uart_initialize(void);
BOOLEAN       EFIAPI uart_poll(void);
UINTN         EFIAPI uart_read(OUT UINT8 *user_buffer, IN UINTN bytes_requested);
UINTN         EFIAPI uart_write(IN UINT8 *user_buffer, IN UINTN bytes_to_send);
BOOLEAN       EFIAPI uart_is_cable_connected(void);
#endif  // __UART_H__
