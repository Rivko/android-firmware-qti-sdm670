/*=======================================================================*//**
 * @file        qusb_ldr_log.c
 * @author:     shreyasr
 * @date        13-Mar-2012
 *
 * @brief       QUSB (Qualcomm High-Speed USB) Logging implementation.
 *
 * @details     This file contains the debug log APIs which can be used for
 *				getting log information
 * @note        
 *
 *              Copyright 2012 Qualcomm Technologies Incorporated.
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
// 06/13/12   shreyasr 		Initial revision
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "qusb_log.h"

#ifdef QUSB_ENABLE_LOGGING

#define QUSB_GENERIC_LOG_SIZE       (4)
#define QUSB_BUS_LOG_SIZE           (4)
#define QUSB_ERROR_LOG_SIZE         (8)

typedef struct
{
  uint16 seq_num;
  uint16 generic_buffer_index;
  uint16 bus_buffer_index;
  uint16 error_buffer_index;
  qusb_generic_log_entry generic_log[QUSB_GENERIC_LOG_SIZE];
  qusb_bus_log_entry     bus_log[QUSB_BUS_LOG_SIZE];
  qusb_error_log_entry   error_log[QUSB_ERROR_LOG_SIZE];
} qusb_log_type;

qusb_log_type qusb_log_buffer;
uint32 qusb_generic_log_buf_size    =   QUSB_GENERIC_LOG_SIZE;
uint32 qusb_bus_log_buf_size        =   QUSB_BUS_LOG_SIZE;
uint32 qusb_error_log_buf_size      =   QUSB_ERROR_LOG_SIZE;

#endif

void qusb_log(qusb_log_enum id_0, uint8 offset, uint32 param)
{
#ifdef QUSB_ENABLE_LOGGING
  qusb_log_buffer.generic_log[qusb_log_buffer.generic_buffer_index].idx =  qusb_log_buffer.seq_num;
  qusb_log_buffer.seq_num = (qusb_log_buffer.seq_num + 1);
  qusb_log_buffer.generic_log[qusb_log_buffer.generic_buffer_index].id_0 = (qusb_log_enum)(id_0 + offset);
  qusb_log_buffer.generic_log[qusb_log_buffer.generic_buffer_index].param = param;
  qusb_log_buffer.generic_buffer_index = (qusb_log_buffer.generic_buffer_index + 1) % QUSB_GENERIC_LOG_SIZE;
#endif
  return;
}

void qusb_bus_log(qusb_log_enum id_0, uint8 offset, uint32 param)
{
#ifdef QUSB_ENABLE_LOGGING
  qusb_log_buffer.bus_log[qusb_log_buffer.bus_buffer_index].idx =  qusb_log_buffer.seq_num;
  qusb_log_buffer.seq_num = (qusb_log_buffer.seq_num + 1);
  qusb_log_buffer.bus_log[qusb_log_buffer.bus_buffer_index].id_0 = (qusb_log_enum)(id_0 + offset);
  qusb_log_buffer.bus_log[qusb_log_buffer.bus_buffer_index].param = param;
  qusb_log_buffer.bus_buffer_index = (qusb_log_buffer.bus_buffer_index + 1) % QUSB_BUS_LOG_SIZE;
#endif
  return;
}

void qusb_error_log(qusb_log_enum id_0, uint8 offset, uint32 param)
{
#ifdef QUSB_ENABLE_LOGGING
  qusb_log_buffer.error_log[qusb_log_buffer.error_buffer_index].idx =  qusb_log_buffer.seq_num;
  qusb_log_buffer.seq_num = (qusb_log_buffer.seq_num + 1);
  qusb_log_buffer.error_log[qusb_log_buffer.error_buffer_index].id_0 = (qusb_log_enum)(id_0 + offset);
  qusb_log_buffer.error_log[qusb_log_buffer.error_buffer_index].param = param;
  qusb_log_buffer.error_buffer_index = (qusb_log_buffer.error_buffer_index + 1) % QUSB_ERROR_LOG_SIZE;
#endif
  return;
}

