/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CHRE_PLATFORM_PLATFORM_LOG_BASE_H_
#define CHRE_PLATFORM_PLATFORM_LOG_BASE_H_

#include <cstddef>
#include <cstdint>

#include "chre/platform/mutex.h"

namespace chre {

/**
 * Storage for the SLPI implementation of PlatformLog.
 */
class PlatformLogBase {
 public:
//  typedef void (FlushLogBufferCallback)(const char *buffer, size_t bufferSize,
//                                        void *context);

  /**
   * Flushes the log buffer by invoking a provided callback, passing in the log
   * buffer and size, then resetting the log buffer. The callback is invoked
   * within the context of the logging lock which guarantees exclusive access to
   * the buffer. The callback should not attempt to use the PlatformLog::log()
   * method or else a deadlock will occur.
   *
   * @param callback the callback to invoke within the context of a lock on the
   *        log buffer.
   * @param context a context pointer to provide to the callback.
   */
//  void flushLogBuffer(FlushLogBufferCallback callback, void *context);

 protected:
  //! The maximum size of a formatted log message.
  static constexpr size_t kMaxLogMessageSize = 160;

  //! The size of the logging buffer, in bytes. Assuming an average log message
  //! has a length of 60, this allows for ~70 log messages. Some messages may
  //! be longer, some may be shorter. YMMV but this should be a good balance
  //! between AP wakeups and memory usage.
  static constexpr size_t kLogBufferSize = (1 * 1024);

  //! The watermark point at which log buffers should be sent to the host. This
  //! cannot be zero or else an infinite loop may be caused. It must be large
  //! enough to hold a few messages.
  static constexpr size_t kLogBufferWatermarkSize = (kLogBufferSize * 20) / 100;

  //! A mutex for acquiring exclusive control over the log buffer and other
  //! internal state.
  Mutex mMutex;

  //! The buffer to log messages into.
  char mLogBuffer[kLogBufferSize];

  //! The current size of messages logged in the buffer.
  size_t mLogBufferSize = 0;

  //! Set to true when a log flush is pending. Do not request another log flush
  //! if this is set to true.
  bool mLogFlushPending = false;
};

}  // namespace chre

#endif  // CHRE_PLATFORM_PLATFORM_LOG_BASE_H_
