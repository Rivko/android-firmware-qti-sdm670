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

#include "chre/core/audio_request_manager.h"

#include "chre/core/event_loop_manager.h"
#include "chre/platform/fatal_error.h"
#include "chre/platform/system_time.h"

/*
 * TODO(P1-62e045): Evict pending audio events from the event queue as needed.
 *
 * Under the following conditions, an audio data event may be posted to the
 * CHRE event queue when it should not be.
 *
 * 1. Nanoapp changes request
 * 2. Nanoapp removes request
 *
 * A previously scheduled audio data event may be residing in the event queue
 * and will be dispatched to the nanoapp after it has cancelled the request.
 *
 * The solution is to evict any audio events for a given audio handle that are
 * directed to a nanoapp before scheduling the next request to the platform.
 */

namespace chre {

AudioRequestManager::AudioRequestManager() {
  size_t sourceCount = mPlatformAudio.getSourceCount();
  if (!mAudioRequestLists.reserve(sourceCount)) {
    FATAL_ERROR_OOM();
  }

  for (size_t i = 0; i < sourceCount; i++) {
    mAudioRequestLists.emplace_back();
  }
}

void AudioRequestManager::init() {
  mPlatformAudio.init();
}

bool AudioRequestManager::configureSource(const Nanoapp *nanoapp,
                                          uint32_t handle,
                                          bool enable,
                                          uint64_t bufferDuration,
                                          uint64_t deliveryInterval) {
  uint32_t numSamples;
  bool success = validateConfigureSourceArguments(
      handle, enable, bufferDuration, deliveryInterval, &numSamples);
  if (success) {
    size_t requestIndex;
    auto *audioRequest = findAudioRequest(handle, nanoapp->getInstanceId(),
                                          &requestIndex);
    Nanoseconds nextEventTimestamp = SystemTime::getMonotonicTime()
        + Nanoseconds(deliveryInterval);
    size_t lastNumRequests = mAudioRequestLists[handle].requests.size();
    if (audioRequest == nullptr) {
      // The nanoapp is making a new request for audio data.
      if (enable) {
        mAudioRequestLists[handle].requests.emplace_back(
            nanoapp->getInstanceId(), numSamples,
            Nanoseconds(deliveryInterval), nextEventTimestamp);
        postAudioSamplingChangeEvent(nanoapp->getInstanceId(), handle,
                                     mAudioRequestLists[handle].available);
        scheduleNextAudioDataEvent(handle);
      } else {
        LOGW("Nanoapp disabling nonexistent audio request");
      }
    } else {
      // The nanoapp is modifying an existing request for audio.
      if (enable) {
        audioRequest->numSamples = numSamples;
        audioRequest->deliveryInterval = Nanoseconds(deliveryInterval);
        audioRequest->nextEventTimestamp = nextEventTimestamp;
      } else {
        mAudioRequestLists[handle].requests.erase(requestIndex);
      }

      // Note that if the next request did not change, this call is not strictly
      // necessary. The expectation is that the platform will gracefully handle
      // rescheduling the same request.
      scheduleNextAudioDataEvent(handle);
    }

    size_t numRequests = mAudioRequestLists[handle].requests.size();
    if (lastNumRequests == 0 && numRequests > 0) {
      mPlatformAudio.setHandleEnabled(handle, true);
    } else if (lastNumRequests > 0 && numRequests == 0) {
      mPlatformAudio.setHandleEnabled(handle, false);
    }
  }

  return success;
}

void AudioRequestManager::handleAudioDataEvent(
    const struct chreAudioDataEvent *audioDataEvent) {
  auto callback = [](uint16_t /* eventType */, void *eventData) {
    auto *event = static_cast<struct chreAudioDataEvent *>(eventData);
    EventLoopManagerSingleton::get()->getAudioRequestManager()
        .handleAudioDataEventSync(event);
  };

  // Cast off the event const so that it can be provided to the free callback as
  // non-const. The event is provided to nanoapps as const and the runtime
  // itself will not modify this memory so this is safe.
  EventLoopManagerSingleton::get()->deferCallback(
      SystemCallbackType::AudioHandleDataEvent,
      const_cast<struct chreAudioDataEvent *>(audioDataEvent), callback);
}

void AudioRequestManager::handleAudioAvailability(uint32_t handle, bool available) {
  struct CallbackState {
    uint32_t handle;
    bool available;
  };

  auto *cbState = memoryAlloc<CallbackState>();
  if (cbState == nullptr) {
    LOG_OOM();
  } else {
    cbState->handle = handle;
    cbState->available = available;

    auto callback = [](uint16_t /* eventType */, void *eventData) {
      auto *state = static_cast<CallbackState *>(eventData);
      EventLoopManagerSingleton::get()->getAudioRequestManager()
          .handleAudioAvailabilitySync(state->handle, state->available);
      memoryFree(state);
    };

    EventLoopManagerSingleton::get()->deferCallback(
        SystemCallbackType::AudioAvailabilityChange, cbState, callback);
  }
}

bool AudioRequestManager::validateConfigureSourceArguments(
    uint32_t handle, bool enable, uint64_t bufferDuration,
    uint64_t deliveryInterval, uint32_t *numSamples) {
  bool success = false;
  if (handle >= mAudioRequestLists.size()) {
    LOGE("Provided audio handle out of range");
  } else if (enable) {
    chreAudioSource audioSource;
    if (!mPlatformAudio.getAudioSource(handle, &audioSource)) {
      LOGE("Failed to query for audio source");
    } else if (bufferDuration > deliveryInterval) {
      LOGE("Buffer duration must be less than or equal to delivery interval");
    } else if (bufferDuration < audioSource.minBufferDuration
               || bufferDuration > audioSource.maxBufferDuration) {
      LOGE("Invalid buffer duration %" PRIu64 " not in range [%" PRIu64
           ",%" PRIu64 "]", bufferDuration, audioSource.minBufferDuration,
           audioSource.maxBufferDuration);
    } else {
      *numSamples = getSampleCountFromRateAndDuration(
          audioSource.sampleRate, Nanoseconds(bufferDuration));
      success = true;
    }
  } else {
    // Disabling the request, no need to validate bufferDuration or
    // deliveryInterval.
    success = true;
  }
  return success;
}

AudioRequestManager::AudioRequest *AudioRequestManager::findAudioRequest(
    uint32_t handle, uint32_t instanceId, size_t *index) {
  AudioRequest *foundAudioRequest = nullptr;
  auto& requests = mAudioRequestLists[handle].requests;
  for (size_t i = 0; i < requests.size(); i++) {
    auto& audioRequest = requests[i];
    if (audioRequest.instanceId == instanceId) {
      foundAudioRequest = &audioRequest;
      *index = i;
      break;
    }
  }

  return foundAudioRequest;
}

AudioRequestManager::AudioRequest *AudioRequestManager::findNextAudioRequest(
    uint32_t handle) {
  Nanoseconds earliestNextEventTimestamp = Nanoseconds(UINT64_MAX);
  AudioRequest *nextRequest = nullptr;

  auto& reqList = mAudioRequestLists[handle];
  for (auto& req : reqList.requests) {
    if (req.nextEventTimestamp < earliestNextEventTimestamp) {
      earliestNextEventTimestamp = req.nextEventTimestamp;
      nextRequest = &req;
    }
  }

  return nextRequest;
}

void AudioRequestManager::handleAudioDataEventSync(
    struct chreAudioDataEvent *event) {
  uint32_t handle = event->handle;
  if (handle < mAudioRequestLists.size()) {
    auto& reqList = mAudioRequestLists[handle];
    AudioRequest *nextAudioRequest = reqList.nextAudioRequest;

    if (reqList.nextAudioRequest != nullptr) {
      postAudioDataEventFatal(event, nextAudioRequest->instanceId);
      nextAudioRequest->nextEventTimestamp = SystemTime::getMonotonicTime()
          + nextAudioRequest->deliveryInterval;
      reqList.nextAudioRequest = nullptr;
    } else {
      LOGW("Received audio data event with no pending audio request");
    }

    scheduleNextAudioDataEvent(handle);
  } else {
    LOGE("Audio data event handle out of range: %" PRIu32, handle);
  }
}

void AudioRequestManager::handleAudioAvailabilitySync(uint32_t handle,
                                                      bool available) {
  if (handle < mAudioRequestLists.size()) {
    mAudioRequestLists[handle].available = available;
    postAudioSamplingChangeEvents(handle, available);
    scheduleNextAudioDataEvent(handle);
  } else {
    LOGE("Audio availability handle out of range: %" PRIu32, handle);
  }
}

void AudioRequestManager::scheduleNextAudioDataEvent(uint32_t handle) {
  auto& reqList = mAudioRequestLists[handle];
  AudioRequest *nextRequest = findNextAudioRequest(handle);

  if (reqList.available && nextRequest != nullptr) {
    Nanoseconds curTime = SystemTime::getMonotonicTime();
    Nanoseconds eventDelay = Nanoseconds(0);
    if (nextRequest->nextEventTimestamp > curTime) {
      eventDelay = nextRequest->nextEventTimestamp - curTime;
    }
    reqList.nextAudioRequest = nextRequest;
    mPlatformAudio.requestAudioDataEvent(
        handle, nextRequest->numSamples, eventDelay);
  } else {
    mPlatformAudio.cancelAudioDataEventRequest(handle);
  }
}

void AudioRequestManager::postAudioSamplingChangeEvents(uint32_t handle,
                                                        bool available) {
  for (const auto& request : mAudioRequestLists[handle].requests) {
    postAudioSamplingChangeEvent(request.instanceId, handle, available);
  }
}

void AudioRequestManager::postAudioSamplingChangeEvent(uint32_t instanceId,
                                                       uint32_t handle,
                                                       bool available) {
  auto *event = memoryAlloc<struct chreAudioSourceStatusEvent>();
  event->handle = handle;
  event->status.enabled = true;
  event->status.suspended = !available;

  EventLoopManagerSingleton::get()->getEventLoop()
      .postEvent(CHRE_EVENT_AUDIO_SAMPLING_CHANGE, event,
                 freeEventDataCallback, kSystemInstanceId, instanceId);
}

void AudioRequestManager::postAudioDataEventFatal(
    struct chreAudioDataEvent *event, uint32_t instanceId) {
  EventLoopManagerSingleton::get()->getEventLoop()
      .postEvent(CHRE_EVENT_AUDIO_DATA, event,
                 freeAudioDataEventCallback,
                 kSystemInstanceId, instanceId);
}

void AudioRequestManager::handleFreeAudioDataEvent(
    struct chreAudioDataEvent *audioDataEvent) {
  mPlatformAudio.releaseAudioDataEvent(audioDataEvent);
}

void AudioRequestManager::freeAudioDataEventCallback(uint16_t eventType,
                                                     void *eventData) {
  auto *event = static_cast<struct chreAudioDataEvent *>(eventData);
  EventLoopManagerSingleton::get()->getAudioRequestManager()
      .handleFreeAudioDataEvent(event);
}

}  // namespace chre
