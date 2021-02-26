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

#include "chre/core/event_loop.h"
#include "chre/core/event_loop_manager.h"
#include "chre/core/timer_pool.h"
#include "chre/platform/fatal_error.h"
#include "chre/platform/system_time.h"

namespace chre {

TimerPool::TimerPool() {
  if (!mSystemTimer.init()) {
    FATAL_ERROR("Failed to initialize a system timer for the TimerPool");
  }
}

TimerHandle TimerPool::setTimer(const Nanoapp *nanoapp, Nanoseconds duration,
    const void *cookie, bool isOneShot) {
  CHRE_ASSERT(nanoapp);

  TimerRequest timerRequest;
  timerRequest.nanoappInstanceId = nanoapp->getInstanceId();
  timerRequest.timerHandle = generateTimerHandle();
  timerRequest.expirationTime = SystemTime::getMonotonicTime() + duration;
  timerRequest.duration = duration;
  timerRequest.isOneShot = isOneShot;
  timerRequest.cookie = cookie;

  bool newTimerExpiresEarliest =
      (!mTimerRequests.empty() && mTimerRequests.top() > timerRequest);
  bool success = insertTimerRequest(timerRequest);

  if (success) {
    if (newTimerExpiresEarliest) {
      if (mSystemTimer.isActive()) {
        mSystemTimer.cancel();
      }

      mSystemTimer.set(handleSystemTimerCallback, this, duration);
    } else if (mTimerRequests.size() == 1) {
      // If this timer request was the first, schedule it.
      handleExpiredTimersAndScheduleNext();
    }
  }

  return success ? timerRequest.timerHandle : CHRE_TIMER_INVALID;
}

bool TimerPool::cancelTimer(const Nanoapp *nanoapp, TimerHandle timerHandle) {
  CHRE_ASSERT(nanoapp);

  size_t index;
  bool success = false;
  TimerRequest *timerRequest = getTimerRequestByTimerHandle(timerHandle,
      &index);

  if (timerRequest == nullptr) {
    LOGW("Failed to cancel timer ID %" PRIu32 ": not found", timerHandle);
  } else if (timerRequest->nanoappInstanceId != nanoapp->getInstanceId()) {
    LOGW("Failed to cancel timer ID %" PRIu32 ": permission denied",
         timerHandle);
  } else {
    mTimerRequests.remove(index);
    if (index == 0) {
      if (mSystemTimer.isActive()) {
        mSystemTimer.cancel();
      }

      handleExpiredTimersAndScheduleNext();
    }

    success = true;
  }

  return success;
}

TimerPool::TimerRequest *TimerPool::getTimerRequestByTimerHandle(
    TimerHandle timerHandle, size_t *index) {
  for (size_t i = 0; i < mTimerRequests.size(); i++) {
    if (mTimerRequests[i].timerHandle == timerHandle) {
      if (index != nullptr) {
        *index = i;
      }
      return &mTimerRequests[i];
    }
  }

  return nullptr;
}

bool TimerPool::TimerRequest::operator>(const TimerRequest& request) const {
  return (expirationTime > request.expirationTime);
}

TimerHandle TimerPool::generateTimerHandle() {
  TimerHandle timerHandle;
  if (mGenerateTimerHandleMustCheckUniqueness) {
    timerHandle = generateUniqueTimerHandle();
  } else {
    timerHandle = mLastTimerHandle + 1;
    if (timerHandle == CHRE_TIMER_INVALID) {
      // TODO: Consider that uniqueness checking can be reset when the number of
      // timer requests reaches zero.
      mGenerateTimerHandleMustCheckUniqueness = true;
      timerHandle = generateUniqueTimerHandle();
    }
  }

  mLastTimerHandle = timerHandle;
  return timerHandle;
}

TimerHandle TimerPool::generateUniqueTimerHandle() {
  TimerHandle timerHandle = mLastTimerHandle;
  while (1) {
    timerHandle++;
    if (timerHandle != CHRE_TIMER_INVALID) {
      TimerRequest *timerRequest = getTimerRequestByTimerHandle(timerHandle);
      if (timerRequest == nullptr) {
        return timerHandle;
      }
    }
  }
}

bool TimerPool::insertTimerRequest(const TimerRequest& timerRequest) {
  // If the timer request was not inserted, simply append it to the list.
  bool success = (mTimerRequests.size() < kMaxTimerRequests) &&
      mTimerRequests.push(timerRequest);
  if (!success) {
    LOGE("Failed to insert a timer request: out of memory");
  }

  return success;
}

bool TimerPool::handleExpiredTimersAndScheduleNext() {
  bool success = false;
  while (!mTimerRequests.empty()) {
    Nanoseconds currentTime = SystemTime::getMonotonicTime();
    TimerRequest& currentTimerRequest = mTimerRequests.top();
    if (currentTime >= currentTimerRequest.expirationTime) {
      // Post an event for an expired timer.
      success = EventLoopManagerSingleton::get()->getEventLoop().postEvent(
          CHRE_EVENT_TIMER, const_cast<void *>(currentTimerRequest.cookie),
          nullptr, kSystemInstanceId, currentTimerRequest.nanoappInstanceId);

      // Reschedule the timer if needed, and release the current request.
      if (!currentTimerRequest.isOneShot) {
        // Important: we need to make a copy of currentTimerRequest here,
        // because it's a reference to memory that may get moved during the
        // insert operation (thereby invalidating it).
        TimerRequest cyclicTimerRequest = currentTimerRequest;
        cyclicTimerRequest.expirationTime = currentTime
            + currentTimerRequest.duration;
        mTimerRequests.pop();
        CHRE_ASSERT(insertTimerRequest(cyclicTimerRequest));
      } else {
        mTimerRequests.pop();
      }
    } else {
      Nanoseconds duration = currentTimerRequest.expirationTime - currentTime;
      mSystemTimer.set(handleSystemTimerCallback, this, duration);

      // Assign success to true here to handle timers that tick before their
      // expiration time. This should be rarely required, but for systems where
      // a timer may tick earlier than requested the request is rescheduled with
      // the remaining time as computed above.
      success = true;
      break;
    }
  }

  return success;
}

void TimerPool::handleSystemTimerCallback(void *timerPoolPtr) {
  auto callback = [](uint16_t /* eventType */, void *eventData) {
    auto *timerPool = static_cast<TimerPool *>(eventData);
    if (!timerPool->handleExpiredTimersAndScheduleNext()) {
      LOGE("Timer callback invoked with no outstanding timers");
    }
  };

  EventLoopManagerSingleton::get()->deferCallback(
      SystemCallbackType::TimerPoolTick, timerPoolPtr, callback);
}

}  // namespace chre
