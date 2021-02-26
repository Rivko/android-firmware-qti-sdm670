/*============================================================================
  @file sns_diag_print.c

  @brief debug message printing utility for hexagon target

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/
#include "msg_diag_service.h"
#include "sns_diag_print.h"
#include "sns_math_util.h"
#include "sns_types.h"

/**
 * Print a message buffer to diag using the hexagon diag api 
 */
void
sns_diag_print_buf(uint32_t ssid, sns_message_priority prio, const char *file,
                   uint32_t line, const char *buf)
{
    msg_const_type msg_const = (msg_const_type) {
      { line, ssid, 1 << prio, }, "%s", file
    };
    msg_sprintf(&msg_const, buf);
}

void
sns_diag_vprintf(uint32_t ssid, sns_message_priority prio,
                     const char *file, uint32_t line, uint32_t nargs,
                     const char *fmt, va_list args)
{
    UNUSED_VAR(file);
    uint32_t values[9]= {0};
    uint32_t max_args = SNS_MIN(ARR_SIZE(values),nargs);
    msg_v2_const_type msg_v2_const = {
      { line, ssid, 1 << prio }, fmt 
    };
    for (int i=0; i < max_args; i++) {
        values[i] = va_arg(args, uint32_t);
    }
    switch (nargs) {
      case 0:
          msg_v2_send(&msg_v2_const);
          break;
      case 1:
          msg_v2_send_1(&msg_v2_const, values[0]);
          break;
      case 2:
          msg_v2_send_2(&msg_v2_const, values[0], values[1]);
          break;
      case 3:
          msg_v2_send_3(&msg_v2_const, values[0], values[1], values[2]);
          break;
      case 4:
          msg_v2_send_var(&msg_v2_const, (uint32_t)4, values[0], values[1], values[2],
                          values[3]);
          break;
      case 5:
          msg_v2_send_var(&msg_v2_const, (uint32_t)5, values[0], values[1], values[2],
                          values[3], values[4]);
          break;
      case 6:
          msg_v2_send_var(&msg_v2_const, (uint32_t)6, values[0], values[1], values[2],
                          values[3], values[4], values[5]);
          break;
      case 7:
          msg_v2_send_var(&msg_v2_const, (uint32_t)7, values[0], values[1], values[2],
                          values[3], values[4], values[5], values[6]);
          break;
      case 8:
          msg_v2_send_var(&msg_v2_const, (uint32_t)8, values[0], values[1], values[2],
                          values[3], values[4], values[5], values[6],
                          values[7]);
          break;
      case 9:
          msg_v2_send_var(&msg_v2_const, (uint32_t)9, values[0], values[1], values[2],
                          values[3], values[4], values[5], values[6],
                          values[7], values[8]);
          break;
    }
}

void sns_diag_vprintf_v2(struct sns_sensor  *sensor,
                         struct sns_sensor_instance  *instance,
                         struct sns_msg_v2_const_type const *msg_v2_struct,
                         uint32_t nargs,
                         va_list args)
{
  UNUSED_VAR(sensor);
  UNUSED_VAR(instance);
  uint32_t values[9]= {0};
  uint32_t max_args = SNS_MIN(ARR_SIZE(values),nargs);
  const msg_v2_const_type *msg_v2_const = (const msg_v2_const_type*)msg_v2_struct;
  for (int i=0; i < max_args; i++) {
    values[i] = va_arg(args, uint32_t);
  }
  switch (nargs) {
    case 0:
      msg_v2_send(msg_v2_const);
      break;
    case 1:
      msg_v2_send_1(msg_v2_const, values[0]);
      break;
    case 2:
      msg_v2_send_2(msg_v2_const, values[0], values[1]);
      break;
    case 3:
      msg_v2_send_3(msg_v2_const, values[0], values[1], values[2]);
      break;
    case 4:
      msg_v2_send_var(msg_v2_const, (uint32_t)4, values[0], values[1], values[2],
                      values[3]);
      break;
    case 5:
      msg_v2_send_var(msg_v2_const, (uint32_t)5, values[0], values[1], values[2],
                      values[3], values[4]);
      break;
    case 6:
      msg_v2_send_var(msg_v2_const, (uint32_t)6, values[0], values[1], values[2],
                      values[3], values[4], values[5]);
      break;
    case 7:
      msg_v2_send_var(msg_v2_const, (uint32_t)7, values[0], values[1], values[2],
                      values[3], values[4], values[5], values[6]);
      break;
    case 8:
      msg_v2_send_var(msg_v2_const, (uint32_t)8, values[0], values[1], values[2],
                      values[3], values[4], values[5], values[6],
                      values[7]);
      break;
    case 9:
      msg_v2_send_var(msg_v2_const, (uint32_t)9, values[0], values[1], values[2],
                      values[3], values[4], values[5], values[6],
                      values[7], values[8]);
      break;
  }
}

void sns_diag_vprintf_v2_err_fatal_only(struct sns_sensor  *sensor,
                                        struct sns_sensor_instance  *instance,
                                        struct sns_msg_v2_const_type const *msg_v2_struct,
                                        uint32_t nargs,
                                        va_list args)
{
  UNUSED_VAR(sensor);
  UNUSED_VAR(instance);
  const msg_v2_const_type *msg_v2_const = (const msg_v2_const_type*)msg_v2_struct;
  if( (msg_v2_const->desc.ss_mask & ( (1 << SNS_ERROR) | (1 << SNS_FATAL) )) != 0 )
  {
    sns_diag_vprintf_v2(sensor, instance, msg_v2_struct, nargs, args);
  }
}
