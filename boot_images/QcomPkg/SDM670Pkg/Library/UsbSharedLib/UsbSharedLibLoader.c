/*=======================================================================*//**
 * @file        UsbSharedLoader.c
 * @author:     pmahajan
 * @date        03-feb-2017
 *
 * @brief       USB common & shared USB core init implementation.
 *
 * @details     This file contains API's the Init sequence for HS and SS core implementation.
 *
 * @note        
 *              Copyright 2017 QUALCOMM Technologies Incorporated.
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
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 02/03/17     pm 	     Initial revision
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "UsbSharedLib.h"
#include "qusb_log.h"
#include "qusb_dci_common.h"


void usb_shared_delay_us(uint32 usecs) {
	qusb_dci_delay_us(usecs);
}

void usb_shared_loop_here(void) {
    static volatile uint32 loop_here = 0x1; 
    while(loop_here);
}

void usb_shared_uart_log_info(char *message) {
	return;
}

void usb_shared_uart_log_error(char *message, uint32 value) {
	qusb_uart_log(message, value);
}

void usb_shared_error_log(uint32 id_0, uint8 offset, uint32 param) {
	qusb_error_log((qusb_log_enum)id_0, offset, param);
}
