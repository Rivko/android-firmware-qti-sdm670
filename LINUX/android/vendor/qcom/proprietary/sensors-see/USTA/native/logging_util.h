/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file logging_util.cpp
  @brief
  Logging util implementation for power related requirement.
============================================================================*/

#ifndef NATIVE_LOGGING_UTIL_H_
#define NATIVE_LOGGING_UTIL_H_

#include "sensor_common.h"

#define INFO_LOG(x, msg) (x)->info_log(msg)
#define DEBUG_LOG(x, msg) (x)->debug_log(msg)
#define ERROR_LOG(x, msg) (x)->error_log(msg)

class usta_logging_util
{
public:
  static usta_logging_util* create_instance();
  static usta_logging_util* get_instance();
  void update_logging_flag(bool is_logging_disabled);
  bool is_log_disabled();
  void info_log(string log_message);
  void debug_log(string log_message);
  void error_log(string log_message);
  ~usta_logging_util();
private:
  bool is_logging_disabled;
  static usta_logging_util* self;
  usta_logging_util();
};



#endif /* NATIVE_LOGGING_UTIL_H_ */
