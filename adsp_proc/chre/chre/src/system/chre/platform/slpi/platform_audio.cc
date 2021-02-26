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

#include "chre/platform/platform_audio.h"

#include <cstring>

#include "chre/core/event_loop_manager.h"
#include "chre/platform/host_link.h"
#include "chre/platform/log.h"
#include "chre/platform/slpi/power_control_util.h"
#include "chre/util/memory.h"
#include "wcd_spi.h"

static_assert(
    sizeof(wcd_spi_audio_source_s) == sizeof(struct chreAudioSource),
    "WCD SPI/CHRE audio sources must be equal in size");
static_assert(
    offsetof(wcd_spi_audio_source_s, name)
        == offsetof(struct chreAudioSource, name),
    "WCD SPI/CHRE audio source name must have the same offset");
static_assert(
    offsetof(wcd_spi_audio_source_s, sample_rate_hz)
        == offsetof(struct chreAudioSource, sampleRate),
    "WCD SPI/CHRE audio source sample rate must have the same offset");
static_assert(
    offsetof(wcd_spi_audio_source_s, min_buffer_duration_ns)
        == offsetof(struct chreAudioSource, minBufferDuration),
    "WCD SPI/CHRE audio source min buffer duration must have the same offset");
static_assert(
    offsetof(wcd_spi_audio_source_s, max_buffer_duration_ns)
        == offsetof(struct chreAudioSource, maxBufferDuration),
    "WCD SPI/CHRE audio source max buffer duration must have the same offset");
static_assert(
    offsetof(wcd_spi_audio_source_s, format)
        == offsetof(struct chreAudioSource, format),
    "WCD SPI/CHRE audio source format must have the same offset");

namespace chre {
namespace {

void handleWcdSpiAudioDataEvent(const wcd_spi_audio_data_event_s *event) {
  LOGD("WCD SPI audio data callback");

  auto *dataEvent = memoryAlloc<struct chreAudioDataEvent>();
  if (dataEvent == nullptr) {
    LOGE("Failed to allocate data event");
  } else {
    dataEvent->handle = event->handle;
    dataEvent->timestamp = event->timestamp_ns;
    dataEvent->sampleRate = event->sample_rate_hz;
    dataEvent->sampleCount = event->sample_count;
    dataEvent->format = event->format;

    // The sample pointers are a union, so the value will be correct regardless
    // of the sample format. This is just a shallow copy of the data pointer,
    // not the contents of the buffer itself.
    dataEvent->samplesULaw8 = event->samples_ulaw8;

    EventLoopManagerSingleton::get()->getAudioRequestManager()
        .handleAudioDataEvent(dataEvent);
  }
}

void handleWcdSpiAudioAvailability(uint32_t handle, bool available) {
  LOGD("WCD SPI audio handle %" PRIu32 " available: %d", handle, available);
  EventLoopManagerSingleton::get()->getAudioRequestManager()
      .handleAudioAvailability(handle, available);
}

}  // anonymous namespace

PlatformAudio::PlatformAudio() {}

PlatformAudio::~PlatformAudio() {
  wcd_spi_client_deinit();
}

void PlatformAudio::init() {
  wcd_spi_client_init(handleWcdSpiAudioDataEvent,
                      handleWcdSpiAudioAvailability);
}

void PlatformAudio::setHandleEnabled(uint32_t handle, bool enabled) {
  uint32_t lastNumAudioClients = mNumAudioClients;

  if (enabled) {
    mNumAudioClients++;
  } else if (mNumAudioClients > 0) {
    mNumAudioClients--;
  } else {
    LOGE("Invalid request to change handle enabled state");
  }

  if (lastNumAudioClients == 0 && mNumAudioClients > 0) {
    LOGD("Enabling WCD SLPI");
    sendAudioRequest();
  } else if (lastNumAudioClients > 0 && mNumAudioClients == 0) {
    LOGD("Disabling WCD SLPI");
    sendAudioRelease();
  }
}

bool PlatformAudio::requestAudioDataEvent(uint32_t handle,
                                          uint32_t numSamples,
                                          Nanoseconds eventDelay) {
  slpiForceBigImage();
  return wcd_spi_client_request_audio_data_event(handle, numSamples,
                                                 eventDelay.toRawNanoseconds());
}

void PlatformAudio::cancelAudioDataEventRequest(uint32_t handle) {
  slpiForceBigImage();
  wcd_spi_client_cancel_audio_data_event(handle);
}

void PlatformAudio::releaseAudioDataEvent(struct chreAudioDataEvent *event) {
  wcd_spi_client_release_audio_data_event(event->handle);
  memoryFree(event);
}

size_t PlatformAudio::getSourceCount() {
  slpiForceBigImage();
  return wcd_spi_client_get_source_count();
}

bool PlatformAudio::getAudioSource(uint32_t handle,
                                   chreAudioSource *source) {
  slpiForceBigImage();
  wcd_spi_audio_source_s wcd_spi_audio_source;
  bool result = wcd_spi_client_get_source(handle, &wcd_spi_audio_source);
  if (result) {
    // The WCD SPI and CHRE source definitions are binary compatible so a simple
    // memcpy will suffice.
    memcpy(source, &wcd_spi_audio_source, sizeof(*source));
  }

  return result;
}

}  // namespace chre
