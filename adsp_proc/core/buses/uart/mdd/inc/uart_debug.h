/*==============================================================================

                                  uart_debug.h

GENERAL DESCRIPTION

  This header defines the public interface for debug code for the UART driver
  to dump the state of the UART registers and the TLMM GPIOs used by the UART
  driver at the time of a crash, so these registers can be recovered from a
  RAM dump to aid in post- mortem debugging.


EXTERNALIZED FUNCTIONS

  siors_mdd_api_dump_uart_status()
    Dumps the hardware state of all open UART ports. This function may be called
    directly by external code at any time.


                    Copyright (c) 2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     QUALCOMM Confidential and Proprietary

==============================================================================*/
/*==============================================================================

$Header: //components/rel/core.qdsp6/2.1/buses/uart/mdd/inc/uart_debug.h#1 $

==============================================================================*/

#ifndef UART_DEBUG_H
#define UART_DEBUG_H

void siors_mdd_api_dump_uart_status( void );

#endif /* UART_DEBUG_H */
