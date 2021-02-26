/*=======================================================================*//**
 * @file        qusb_log_common.c
 * @author:     shreyasr
 * @date        13-Mar-2012
 *
 * @brief       QUSB (Qualcomm High-Speed USB) Logging implementation.
 *
 * @details     This file contains the debug log APIs which can be used for
 *				getting log information
 * @note        
 *
 *              Copyright 2012, 2017 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/


// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when         who        what, where, why
// --------   ---        ----------------------------------------------------------
// 03/02/17   pm     		Add UsbSharedLib
// 06/13/12   shreyasr 		Initial revision
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "qusb_log.h"
#include "boot_logger.h"
#include "CoreString.h"

#ifdef QUSB_UART_LOG

#define QUSB_UART_INT_STR_SIZE      (32)    //MAX int digits can appear on UART log

// UART logging
void qusb_uart_log(char *message, uint32 value)
{
  char uart_int_log_buffer[QUSB_UART_INT_STR_SIZE];
  int len;

  if(message == NULL)
  {
    boot_log_message("NULL_MSG");
    return;
  }
  
  if(value)
  {
    len = snprintf(uart_int_log_buffer, QUSB_UART_INT_STR_SIZE, "usb: %s , 0x%x", message, value);
  }
  else
  {
    len = snprintf(uart_int_log_buffer, QUSB_UART_INT_STR_SIZE, "usb: %s", message);
  }

  if((len < 0) || (len >= QUSB_UART_INT_STR_SIZE))
  {
    boot_log_message("str_overflow");
  }
  else
  {
    boot_log_message(uart_int_log_buffer);
  }
}

// UART logging with optional value parameter
void qusb_uart_w_param_log(char *message, uint32 value)
{ 
  int len;
  char uart_int_log_buffer[QUSB_UART_INT_STR_SIZE];
  len = snprintf(uart_int_log_buffer, QUSB_UART_INT_STR_SIZE, "0x%x", value);
  if((len < 0) || (len >= QUSB_UART_INT_STR_SIZE))
  {
    boot_log_message_optional_data(message, "str_overflow");
  }
  else
  {
    boot_log_message_optional_data(message, uart_int_log_buffer);
  }
}

// UART logging with optional string parameter
void qusb_uart_w_str_log(char *message, char *opt_message)
{
  boot_log_message_optional_data(message, opt_message);
}

#else

#define qusb_uart_log(msg, val)
#define qusb_uart_w_param_log(msg, val)
#define qusb_uart_w_str_log(msg, val)

#endif

