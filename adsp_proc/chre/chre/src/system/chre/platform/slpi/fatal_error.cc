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

#include "chre/target_platform/fatal_error.h"

#include "qurt_timer.h"

#include "chre/target_platform/host_link_base.h"

extern "C" int HAP_timer_sleep(unsigned long long sleep_duration);

namespace chre {

void preFatalError() {
  HostLinkBase::flushOutboundQueue();

  // The flush above only covers the message leaving our queue, so give a grace
  // period for the last message to actually reach the host.
  constexpr qurt_timer_duration_t kPostFlushDelayUsec = 500000;  // 500 ms
  HAP_timer_sleep(kPostFlushDelayUsec);
}

}  // namespace chre
