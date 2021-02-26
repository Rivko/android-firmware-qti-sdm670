/*
 * Copyright (C) 2016 The Android Open Source Project
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

#include "chre/core/event_loop_manager.h"

#include "chre/platform/fatal_error.h"
#include "chre/platform/memory.h"
#include "chre/util/lock_guard.h"

namespace chre {

void freeEventDataCallback(uint16_t /*eventType*/, void *eventData) {
  memoryFree(eventData);
}

Nanoapp *EventLoopManager::validateChreApiCall(const char *functionName) {
  chre::Nanoapp *currentNanoapp = EventLoopManagerSingleton::get()
      ->getEventLoop().getCurrentNanoapp();
  CHRE_ASSERT_LOG(currentNanoapp, "%s called with no CHRE app context",
                  functionName);
  return currentNanoapp;
}

UniquePtr<char> EventLoopManager::debugDump() {
  constexpr size_t kDebugStringSize = 4096;
  char *debugStr = static_cast<char *>(memoryAlloc(kDebugStringSize));
  if (debugStr != nullptr) {
    size_t debugStrPos = 0;
    if (!mMemoryManager.logStateToBuffer(debugStr, &debugStrPos,
                                         kDebugStringSize)) {
      LOGE("Memory manager debug dump failed.");
    } else if (!mEventLoop.logStateToBuffer(debugStr, &debugStrPos,
                                            kDebugStringSize)) {
      LOGE("Event loop debug dump failed.");
    } else if (!mSensorRequestManager.logStateToBuffer(debugStr, &debugStrPos,
                                                       kDebugStringSize)) {
      LOGE("Sensor request manager debug dump failed.");
    } else if (!mGnssManager.logStateToBuffer(debugStr, &debugStrPos,
                                                     kDebugStringSize)) {
      LOGE("GNSS manager debug dump failed.");
    } else if (!mWifiRequestManager.logStateToBuffer(debugStr, &debugStrPos,
                                                     kDebugStringSize)) {
      LOGE("Wifi request manager debug dump failed.");
    } else if (!mWwanRequestManager.logStateToBuffer(debugStr, &debugStrPos,
                                                     kDebugStringSize)) {
      LOGE("WWAN request manager debug dump failed.");
    }
    LOGD("Debug dump used %zu bytes of log buffer", debugStrPos);
  }

  return UniquePtr<char>(debugStr);
}

uint32_t EventLoopManager::getNextInstanceId() {
  ++mLastInstanceId;

  // ~4 billion instance IDs should be enough for anyone... if we need to
  // support wraparound for stress testing load/unload, then we can set a flag
  // when wraparound occurs and use EventLoop::findNanoappByInstanceId to ensure
  // we avoid conflicts
  if (mLastInstanceId == kBroadcastInstanceId
      || mLastInstanceId == kSystemInstanceId) {
    FATAL_ERROR("Exhausted instance IDs!");
  }

  return mLastInstanceId;
}

void EventLoopManager::lateInit() {
  mGnssManager.init();
  mWifiRequestManager.init();
  mWwanRequestManager.init();
  mQcBleRequestManager.init();

#ifdef CHRE_AUDIO_SUPPORT_ENABLED
  mAudioRequestManager.init();
#endif  // CHRE_AUDIO_SUPPORT_ENABLED
}

// Explicitly instantiate the EventLoopManagerSingleton to reduce codesize.
template class Singleton<EventLoopManager>;

}  // namespace chre
