/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_common.h

  @brief
   which will be accessible to all files.
============================================================================*/

#pragma once
#include <inttypes.h>
#include <string>
#include <cstring>
#include <functional>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <android/log.h>
#include"ssc_test_log.h"
#include "usta_rc.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/compiler/parser.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>

using namespace std;
using namespace google::protobuf::io;
using namespace google::protobuf;
using namespace google::protobuf::compiler;


//#define SENSOR_TEST

typedef struct sensor_uid
{
  uint64_t low;
  uint64_t high;
}sensor_uid;

#define UNUSED_VAR(var) var
#define LOG_FOLDER_PATH "/mnt/sdcard/"
#define PROTO_DELIM '?'
#define USTA_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "USTA_Native_Info", __VA_ARGS__))
#define USTA_LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "USTA_Native_Debug", __VA_ARGS__))
#define USTA_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "USTA_Native_Error", __VA_ARGS__))
