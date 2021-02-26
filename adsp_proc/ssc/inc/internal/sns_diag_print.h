/*============================================================================
  @file sns_diag_print.h

  @brief print utility for diag messages

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/
#pragma once

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdarg.h>
#include <stdint.h>
#include "sns_diag_service.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor;
struct sns_fw_sensor_instance;

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

/* define the Max string length */
#define SNS_PRINT_MESSAGE_MAX_LEN  256

/*=============================================================================
  Public functions
  ===========================================================================*/
/**
 * print the buffer to diag
 *
 * Note: please don't use this function directly. Public API is
 * provided by diag service using the sensor_printf() and
 * sensor_inst_printf() functions in sns_diag_service.h
 *
 * @param[i] ssid diag subsystem ID
 * @param[i] prio message priority
 * @param[i] file file name string
 * @param[i] line file line number
 * @param[i] buf buffer to print
 */
void
sns_diag_print_buf(uint32_t ssid, sns_message_priority prio, const char *file,
                   uint32_t line, const char *buf);

/**
 * Depcecated
 */
void
sns_diag_vprintf(uint32_t ssid, sns_message_priority prio,
                        const char *file, uint32_t line, uint32_t nargs,
                        const char *fmt, va_list args);

/**
 * Function prototype for printing a va_list to diag
 *
 * Note: please don't use this function directly. Public API is
 * provided by using the SNS_PRINTF() and
 * SNS_INST_PRINTF() functions in sns_printf.h
 *
 * @param[i] Sensor pointer (either this or instance may be NULL)
 * @param[i] Instance pointer (either this or sensor may be NULL)
 * @param[i] Diag defined debug structure
 * @param[i] Number of arguments
 * @param[i] va_list of arguments
 */
typedef void(*sns_diag_vprintf_fptr)(struct sns_sensor *sensor,
                                     struct sns_sensor_instance *instance,
                                     struct sns_msg_v2_const_type const *msg_v2_struct,
                                     uint32_t nargs,
                                     va_list args);

/**
 * This function sends all messages to diag
 */
void sns_diag_vprintf_v2(struct sns_sensor *sensor,
                         struct sns_sensor_instance *instance,
                         struct sns_msg_v2_const_type const *msg_v2_struct,
                         uint32_t nargs,
                         va_list args);

/**
 * This function sends only error and fatal messages to diag
 */
void sns_diag_vprintf_v2_err_fatal_only(struct sns_sensor *sensor,
                                        struct sns_sensor_instance *instance,
                                        struct sns_msg_v2_const_type const *msg_v2_struct,
                                        uint32_t nargs,
                                        va_list args);

