/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#ifndef __cplusplus
#include "qcril_log.h"
#else

#include <mutex>
#include <string>
#include <vector>
#include <assert.h>
#ifdef QMI_RIL_UTF
#include "qcril_log.h"
#else
#include <settingsd/Logger.h>
using namespace QcSettingsD;

#ifndef QCRIL_LOG_ADDITIONAL
#define QCRIL_LOG_ADDITIONAL    QCRIL_HAL_LOG_INFO
#endif

#ifndef QCRIL_LOG_INFO
#define QCRIL_LOG_INFO          QCRIL_HAL_LOG_INFO
#endif

#ifndef QCRIL_LOG_WARN
#define QCRIL_LOG_WARN          QCRIL_HAL_LOG_WARN
#endif

#ifndef QCRIL_LOG_ERROR
#define QCRIL_LOG_ERROR         QCRIL_HAL_LOG_HIGH
#endif 

#ifndef QCRIL_LOG_DEBUG
#define QCRIL_LOG_DEBUG         QCRIL_HAL_LOG_DEBUG
#endif

#ifndef QCRIL_LOG_FUNC_ENTRY 
#define QCRIL_LOG_FUNC_ENTRY    QCRIL_HAL_LOG_ENTER
#endif

#ifndef QCRIL_LOG_FUNC_RETURN
#define QCRIL_LOG_FUNC_RETURN   QCRIL_HAL_LOG_LEAVE
#endif

#ifndef QCRIL_LOG_ESSENTIAL
#define QCRIL_LOG_ESSENTIAL     QCRIL_HAL_LOG_FATAL // LPA REARCH - add a new macro for QXDM logging.
#endif

#ifndef QCRIL_LOG_FATAL
#define QCRIL_LOG_FATAL         QCRIL_HAL_LOG_FATAL
#endif

#ifndef QCRIL_LOG_ESSENTIAL
#define QCRIL_LOG_ESSENTIAL     QCRIL_HAL_LOG_FATAL // LPA REARCH - add a new macro for QXDM logging.
#endif

#ifndef QCRIL_LOG_VERBOSE
#define QCRIL_LOG_VERBOSE       QCRIL_HAL_LOG_DEBUG
#endif

#ifndef QCRIL_LOG_ADDITIONAL
#define QCRIL_LOG_ADDITIONAL    QCRIL_HAL_LOG_DEBUG
#endif

#ifndef QCRIL_LOG_CF_PKT_RIL_FN
#define QCRIL_LOG_CF_PKT_RIL_FN(instance, label)
#endif

#ifndef QCRIL_LOG_FUNC_RETURN_WITH_RET
#define QCRIL_LOG_FUNC_RETURN_WITH_RET( ... ) \
{ QCRIL_HAL_LOG_LEAVE("function exit with ret %d", ##__VA_ARGS__ ); }
#endif

#ifndef QCRIL_LOG_ASSERT
#define QCRIL_LOG_ASSERT( cond )    ({ if(!(cond)) {QCRIL_HAL_LOG_FATAL("assert" #cond " failed"); assert(cond);} })
#endif
#endif
#endif  /* QMI_RIL_UTF */

#ifdef __cplusplus
using std::vector;
using std::string;
using std::mutex;

class Log {
 private:
  bool mEnabled = true;
  mutex mMutex;
  Log() {}

 public:
  Log(Log const &) = delete;             // Copy construct
  Log(Log &&) = delete;                  // Move construct
  Log &operator=(Log const &) = delete;  // Copy assign
  Log &operator=(Log &&) = delete;       // Move assign

  void d(string str);
  void logTime(string str);
  void setEnabled(bool enabled);
  static Log &getInstance();
};

#endif  /* __cplusplus */
