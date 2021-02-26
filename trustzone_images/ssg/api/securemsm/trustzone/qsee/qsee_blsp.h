#ifndef QSEE_BLSP_H
#define QSEE_BLSP_H

/**
@file qsee_blsp.h
@brief Provide SPI API wrappers
*/

/*===========================================================================
  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      -----------------------------------
08/17/16   dpk       Removed modify ownership API from being accessed from TA
05/14/15   dpk       Initial version

===========================================================================*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


typedef enum {
   QSEE_NONE_ID = 0,   
   QSEE_TZ_ID = 1,
   QSEE_APSS_ID = 3,
   QSEE_ADSP_ID = 6,
   QSEE_MAX_ID = 0xFFFF
} qsee_subsystem_id_t;

typedef enum {
   QSEE_QUP_DEVICE_ID_1 = 1,
   QSEE_QUP_DEVICE_ID_2,
   QSEE_QUP_DEVICE_ID_3,
   QSEE_QUP_DEVICE_ID_4,
   QSEE_QUP_DEVICE_ID_5,
   QSEE_QUP_DEVICE_ID_6,
   QSEE_QUP_DEVICE_ID_7,
   QSEE_QUP_DEVICE_ID_8,
   QSEE_QUP_DEVICE_ID_9,
   QSEE_QUP_DEVICE_ID_10,
   QSEE_QUP_DEVICE_ID_11,
   QSEE_QUP_DEVICE_ID_12,  
   QSEE_UART_DEVICE_START,
   QSEE_UART_DEVICE_ID_1 = QSEE_UART_DEVICE_START,
   QSEE_UART_DEVICE_ID_2,
   QSEE_UART_DEVICE_ID_3,
   QSEE_UART_DEVICE_ID_4,
   QSEE_UART_DEVICE_ID_5,
   QSEE_UART_DEVICE_ID_6,
   QSEE_UART_DEVICE_ID_7,
   QSEE_UART_DEVICE_ID_8,
   QSEE_UART_DEVICE_ID_9,
   QSEE_UART_DEVICE_ID_10,
   QSEE_UART_DEVICE_ID_11,
   QSEE_UART_DEVICE_ID_12,
   QSEE_INVALID_DEVICE_ID = 0xFFFFFFFF, 
}qsee_qup_device_id_t;


#endif
