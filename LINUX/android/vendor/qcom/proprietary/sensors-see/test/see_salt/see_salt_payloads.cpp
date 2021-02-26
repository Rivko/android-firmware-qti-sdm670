/** ===================================================================
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * FILE: see_salt_payloads.cpp
 * ================================================================ */
#include "see_salt.h"

std::string
see_self_test_config::get_test_type_symbolic(see_self_test_type_e test_type)
{
   if ( test_type == SELF_TEST_TYPE_SW) {
      return  "SNS_PHYSICAL_SENSOR_TEST_TYPE_SW";
   }
   if ( test_type == SELF_TEST_TYPE_HW) {
      return "SNS_PHYSICAL_SENSOR_TEST_TYPE_HW";
   }
   if ( test_type == SELF_TEST_TYPE_FACTORY) {
      return "SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY";
   }
   if ( test_type == SELF_TEST_TYPE_COM) {
      return "SNS_PHYSICAL_SENSOR_TEST_TYPE_COM";
   }
   return "";
}

std::string
see_resampler_config::get_rate_type_symbolic(see_resampler_rate rate_type)
{
   if (rate_type == SEE_RESAMPLER_RATE_FIXED) {
      return "SNS_RESAMPLER_RATE_FIXED";
   }
   else if ( rate_type == SEE_RESAMPLER_RATE_MINIMUM) {
      return "SNS_RESAMPLER_RATE_MINIMUM";
   }
   return "";
}

std::string
see_diag_log_trigger_req::get_log_type_symbolic()
{
   if ( _log_type == SEE_LOG_TYPE_ISLAND_LOG) {
      return "SNS_DIAG_TRIGGERED_LOG_TYPE_ISLAND_LOG";
   }
   else if ( _log_type == SEE_LOG_TYPE_MEMORY_USAGE_LOG) {
      return "SNS_DIAG_TRIGGERED_LOG_TYPE_MEMORY_USAGE_LOG";
   }
   return "";
}

