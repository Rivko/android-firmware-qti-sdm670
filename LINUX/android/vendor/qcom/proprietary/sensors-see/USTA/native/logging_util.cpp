/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file logging_util.cpp
  @brief
  Logging util implementation for power related requirement.
============================================================================*/

#include "logging_util.h"

usta_logging_util* usta_logging_util::self = NULL;

usta_logging_util* usta_logging_util::create_instance()
{
  if(self == NULL)
  {
    self = new usta_logging_util();
    return self;
  }
  else
  {
    return self;
  }
}
usta_logging_util::usta_logging_util()
{
  is_logging_disabled = false; // Default value
}

usta_logging_util* usta_logging_util::get_instance()
{
  return self;
}

void usta_logging_util::update_logging_flag(bool disable_flag)
{
  this->is_logging_disabled = disable_flag;
}

bool usta_logging_util::is_log_disabled()
{
  return is_logging_disabled;
}

void usta_logging_util::debug_log(string log_message)
{
  if(is_logging_disabled == false)
  {
    USTA_LOGD("%s" , log_message.c_str());
  }
}

void usta_logging_util::error_log(string log_message)
{
  if(is_logging_disabled == false)
  {
    USTA_LOGE("%s", log_message.c_str());
  }
}

void usta_logging_util::info_log(string log_message)
{
  if(is_logging_disabled == false)
  {
    USTA_LOGI("%s", log_message.c_str());
  }
}
