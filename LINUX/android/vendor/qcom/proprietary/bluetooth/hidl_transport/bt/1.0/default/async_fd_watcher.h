/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
//
// Copyright 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <map>
#include <mutex>
#include <thread>

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

using ReadCallback = std::function<void(int)>;
using TimeoutCallback = std::function<void(void)>;

class AsyncFdWatcher {
 public:
  AsyncFdWatcher();
  ~AsyncFdWatcher();

  int WatchFdForNonBlockingReads(int file_descriptor,
                                 const ReadCallback& on_read_fd_ready_callback);
  int ConfigureTimeout(const std::chrono::milliseconds timeout,
                       const TimeoutCallback& on_timeout_callback);
  void StopWatchingFileDescriptors();
  int StopThreadRoutine();
 private:
  AsyncFdWatcher(const AsyncFdWatcher&) = delete;
  AsyncFdWatcher& operator=(const AsyncFdWatcher&) = delete;

  int TryStartThread();
  int StopThread();
  int NotifyThread();
  void ThreadRoutine();

  std::atomic_bool running_{false};
  std::thread thread_;
  std::mutex internal_mutex_;
  std::mutex timeout_mutex_;

  std::map<int, ReadCallback> watched_fds_;
  int notification_listen_fd_;
  int notification_write_fd_;
  TimeoutCallback timeout_cb_;
  std::chrono::milliseconds timeout_ms_;
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
