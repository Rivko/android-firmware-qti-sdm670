/*
  @file sns_profiler.h

  Provides macros for the QDSS profiler

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/


#if (SSC_TARGET_HEXAGON)
  #include <qurt.h>
  #include <stdbool.h>
  #include "qurt_timer.h"
  #include "tracer.h"
  #include "tracer_event_ids.h"

  extern bool sns_enable_qdss;

  #define SNS_PROFILE(log_id, num_args, ... ) \
    if(sns_enable_qdss)\
    { \
      (num_args == 0 )?  \
      tracer_event_simple((tracer_event_id_t)log_id):  \
      tracer_event_simple_vargs((tracer_event_id_t)log_id, num_args, ##__VA_ARGS__ );\
    }(void)0
#else
  #define SNS_PROFILE(log_id, num_args, ...)
#endif
