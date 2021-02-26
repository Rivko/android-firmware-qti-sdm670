#ifndef BOOT_UART_H
#define BOOT_UART_H
/*===========================================================================

                    BOOT UART DRIVER DEFINITIONS

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
09/17/14   kedara  Update Uart Driver api's.
                   remove FEATURE_BOOT_EXTERN_UART_COMPLETED
08/20/13   lm      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "Uart.h"
#include "boot_comdef.h"

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
*  boolean: TRUE if successful.
* 
* @par Side Effects
*  None
* 
*/
boolean boot_uart_init(void);

/* ========================================================================
**  Function : boot_uart_rx
** ======================================================================*/
/*!
* 
* @brief
* This wrapper funcion will call the uart driver receive function which will
* receive the data comming from the uart port.
*
*@param[in] handle - handler
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
uint32 boot_uart_rx(char* buf, uint32 bytes_to_rx);

/* ============================================================================
**  Function : boot_uart_tx
** ============================================================================
*/
/*!
* 
* @brief
* This wrapper funcion will call the uart driver transmit function which will
* transmit the data from the uart port.
*
*@param[in] handle - handler
*
*@param[in] buf - transmit buffer
*
* @param[in] bytes_to_tx - transmit data size

* @par Dependencies
*   None
*   
* @retval
*   Transmitted Bytes
* 
* @par Side Effects
*   None
* 
*/

uint32 boot_uart_tx(char* buf, uint32 bytes_to_tx);

#endif /* BOOT_UART_H */

