#ifndef UART_INIT_H
#define UART_INIT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             U A R T   I N I T

            Initialization of UART device driver

GENERAL DESCRIPTION

  This module contains the code used by the device manager (SIO) to initialize
  the UART driver.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  The only external function, sio_uart_driver_init, is called by the device
  manager (SIO) to perform device driver initialization.  The name of this
  function cannot change since SIO depends on it.

Copyright (c) 1999-2000, 2007-2008 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/mdd/inc/uart_init.h#1 $

when         who       what, where, why
-----------  --------  ------------------------------------------------------
11-Nov-2008  cmihalik  Initial revision.

===========================================================================*/

#include "comdef.h"
#include "assert.h"

#include "sio.h"

/*===========================================================================

FUNCTION       sio_uart_driver_init

DESCRIPTION    This is the UART device driver initialization function.  It's
               called by the device manager (SIO) to initialize the
               UART driver.

DEPENDENCIES   None.

RETURN VALUE   1.

SIDE EFFECTS   None.
===========================================================================*/

uint32         sio_uart_driver_init( sio_register_device_driver_cb_type register_driver_cb_fn );

/*===========================================================================

FUNCTION SIO_RS232_OPEN_STREAM

DESCRIPTION
  This function allocates stream as indicated in open_ptr.

DEPENDENCIES
  None

RETURN VALUE
  SIO_DONE_S: TX was successfully allocated.
  SIO_UNAVAIL_S: TX could not be allocated.

SIDE EFFECTS
  None

===========================================================================*/

sio_status_type
siors_mdd_api_open_stream( sio_open_type* open_ptr );

/*===========================================================================

FUNCTION SIO_RS232_CLOSE_STREAM

DESCRIPTION
  This function deallocates given stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

typedef void       (*sio_close_func)(void);
typedef void       (*sio_flush_func)(void);

void
siors_mdd_api_close_stream( sio_stream_id_type stream_id, sio_close_func close_func_ptr );

/*===========================================================================

FUNCTION SIO_RS232_FLUSH_TX

DESCRIPTION
  This function flushes the TX.

DEPENDENCIES
 User should not queue more data until after the registered callback
 is executed.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void
siors_mdd_api_flush_tx( sio_stream_id_type stream_id, sio_port_id_type port_id, sio_flush_func flush_func_ptr );

/*===========================================================================

FUNCTION SIO_RS232_DEV_INIT

DESCRIPTION
  Initialize the MSM UART serial interface for operation and initialize
  data structures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
siors_mdd_api_dev_init( void );

/*===========================================================================

FUNCTION SIO_RS232_HW_TRANSMIT

DESCRIPTION
  This function transmits over MSMs RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
siors_mdd_api_dev_transmit( sio_stream_id_type stream_id, sio_port_id_type port_id, dsm_item_type *tx_ptr );

/*===========================================================================

FUNCTION SIO_RS232_IOCTL                                EXTERNALIZED FUNCTION

DESCRIPTION
  Allows control of open stream, given that the device is an RS232 serial
  device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void
siors_mdd_api_ioctl ( sio_stream_id_type stream_id, sio_port_id_type port_id, sio_ioctl_cmd_type cmd, sio_ioctl_param_type *param );

/*===========================================================================

FUNCTION SIO_RS232_DISABLE_DEVICE                       EXTERNALIZED FUNCTION

DESCRIPTION
  This routine disables given device, including turning off clock to save
  power.  This is different from UART powerdown feature which attempts to
  save power with minimal loss of data.  Instead, this function is meant
  for customers who may not want to use both UARTs.

  Effects of this function may be reversed by calling sio_rs232_dev_init.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
siors_mdd_api_disable_device( sio_port_id_type port_id );

/*===========================================================================

FUNCTION SIO_RS232_PURGE_TX

DESCRIPTION
  This function cancels the active transfer and frees up the queued DSM items.

DEPENDENCIES
  None

RETURN VALUE
  Return the status of type sio_status_type

SIDE EFFECTS
  None
===========================================================================*/

sio_status_type
siors_mdd_api_purge_tx( sio_stream_id_type stream_id, sio_port_id_type port_id );
#endif
