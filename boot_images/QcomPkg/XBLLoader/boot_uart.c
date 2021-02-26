/*===========================================================================

                    BOOT UART DRIVER 

DESCRIPTION
  Contains wrapper definition for external uart drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013, 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/17   kvs     Check the uart cable connection status and flush the uart logs 
09/17/14   kedara  Update Uart Driver api's.
08/20/13   lm      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_uart.h"
#include "busywait.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
// Define the cable detection loop count
#define UART_CABLE_DETECTION_MAX_LOOP_COUNT 20

// Define the cable detection pause time in us
#define UART_CABLE_DETECTION_PAUSE_TIME 10 

/*check uart connection status*/
static boolean uart_connection_status = FALSE;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/*===========================================================================

**  Function :  boot_uart_init

** ==========================================================================
*/
/*!
* 
* @brief
* This wrapper funcion will call the uart driver function which will
* initiate the uart driver for Tx/Rx operation.
*
*@param[in] 
*  None
*
* @par Dependencies
*  None
*   
* @retval
*  boolean:  TRUE if success.
* 
* @par Side Effects
*  None
* 
*/
boolean boot_uart_init(void)
{
  UINT32 cable_detection_loop_count = 0;
  
  uart_initialize();

   /* Query Serial Cable Status weather its connected or not.
     Update uart_connection_status to TRUE if cable is connected
     Update uart_connection_status to FALSE if cable is not connected */  	 

  do {
	uart_connection_status = uart_is_cable_connected();
	 
    if (uart_connection_status == FALSE) {                                                                  		 
	  break;
	}
	  
	busywait (UART_CABLE_DETECTION_PAUSE_TIME);
	  
	cable_detection_loop_count++;
  } while ((cable_detection_loop_count) < UART_CABLE_DETECTION_MAX_LOOP_COUNT);
  
  /* uart init does not return any status currently */
  return TRUE;
}


/* ========================================================================
**  Function : boot_uart_receive
** ======================================================================*/
/*!
* 
* @brief
* This wrapper funcion will call the uart driver receive function which will
* receive the data comming from the uart port.
*
*@param[in] buf - receive buffer
*
* @param[in] bytes_to_rx - receive data size

* @par Dependencies
*   None
*   
* @retval
*   Received Bytes
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_uart_rx(char* buf, uint32 bytes_to_rx)
{
  return uart_read( (UINT8 *)buf, bytes_to_rx);
}

/* ============================================================================
**  Function : boot_uart_transmit
** ============================================================================
*/
/*!
* 
* @brief
* This wrapper funcion will call the uart driver transmit function which will
* transmit the data from the uart port.
*
*@param[in] buf - transmit buffer
*
* @param[in] bytes_to_tx - transmit data size

* @par Dependencies
*  None
*   
* @retval
*  Transmitted Bytes
* 
* @par Side Effects
*  None
* 
*/
uint32 boot_uart_tx(char* buf, uint32 bytes_to_tx)
{
  UINT8* msg_buff;
  uintnt bytes_remaining, bytes_sent;

  bytes_remaining = bytes_to_tx;
  msg_buff = (UINT8 *) buf;

  /* If uart connection is disconnected then don't flush log through uart*/
  if (uart_connection_status == FALSE) {
    return 0;
  }
  
  while (bytes_remaining)
  {
    bytes_sent = uart_write (msg_buff, bytes_remaining);
    msg_buff += bytes_sent;
    bytes_remaining -= bytes_sent;
  }

  return bytes_to_tx;  
}
