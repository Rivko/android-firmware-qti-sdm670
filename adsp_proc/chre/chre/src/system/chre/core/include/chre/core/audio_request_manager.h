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

#ifndef CHRE_CORE_AUDIO_REQUEST_MANAGER_H_
#define CHRE_CORE_AUDIO_REQUEST_MANAGER_H_


#include <cstdint>

#include "chre_api/chre/audio.h"
#include "chre/core/nanoapp.h"
#include "chre/platform/platform_audio.h"
#include "chre/util/dynamic_vector.h"
#include "chre/util/non_copyable.h"

namespace chre {

/**
 * Manages requests for audio resources from nanoapps and multiplexes these
 * requests into the platform-specific implementation of the audio subsystem.
 */
class AudioRequestManager : public NonCopyable {
 public:
  /**
   * Sets up the initial condition for the audio request manager including
   * initial allocation of requests managers.
   */
  AudioRequestManager();

  /**
   * Initializes the platform-specific audio module. Must be called prior to
   * invoking any other methods in this class.
   */
  void init();

  /**
   * Obtains an audio source from the platform audio implementation.
   *
   * @param handle The audio source to query for.
   * @param audioSource The audio source to populate with the details of this
   *        handle, if found.
   * @return true if the query was successful.
   */
  bool getAudioSource(uint32_t handle, struct chreAudioSource *audioSource) {
    return mPlatformAudio.getAudioSource(handle, audioSource);
  }

  /**
   * Updates the current request for audio from a nanoapp for a given audio
   * source.
   *
   * @param nanoapp A non-null pointer to the nanoapp requesting this change.
   * @param handle The audio source handle for which this request is directed
   *        toward.
   * @param enable true if enabling the source, false if disabling.
   * @param bufferDuration The amount of data to deliver to the nanoapp.
   * @param deliveryInterval How frequently to deliver this data.
   * @return true if the request was successful, false otherwise. When enabling,
   *         the bufferDuration must be less than or equal to deliveryInterval.
   *
   * @see chreAudioConfigureSource()
   */
  bool configureSource(const Nanoapp *nanoapp, uint32_t handle, bool enable,
                       uint64_t bufferDuration, uint64_t deliveryInterval);

  /**
   * Handles a new batch of audio from the PlatformAudio interface.
   *
   * @param event The audio data event provided to the audio request manager.
   *        Ownership of this event is transferred to the CHRE runtime until it
   *        is released via the PlatformAudio::releaseAudioDataEvent platform
   *        API.
   */
  void handleAudioDataEvent(const struct chreAudioDataEvent *event);

  /**
   * Handles events from the platform related to whether or not audio is
   * available.
   *
   * @param handle The handle for which audio data availability has changed.
   * @param available true if requests for audio data will be processed and
   *        data events posted, false otherwise.
   */
  void handleAudioAvailability(uint32_t handle, bool available);

  /**
   * A convenience function to convert sample count and sample rate into a time
   * duration. It is illegal to call this function with a rate of zero.
   *
   * @param sampleCount The number of samples to convert to time at the provided
   *        rate.
   * @param sampleRate The rate to perform the time conversion at.
   * @return The duration of time for these two parameters.
   */
  static constexpr Nanoseconds getDurationFromSampleCountAndRate(
      uint32_t sampleCount, uint32_t sampleRate) {
    // This function will overflow with high sample counts but does work for
    // reasonable expected values.
    //
    // Example: 22050 * 1000000000 / 44100 = 500000000ns
    return Nanoseconds((sampleCount * kOneSecondInNanoseconds) / sampleRate);
  }

  /**
   * A convenience function to convert sample rate and duration into a sample
   * count. This can be used by platform implementations to ensure that the
   * computed buffer sizes match those expected by CHRE.
   *
   * @param sampleRate The sample rate of the audio source.
   * @param duration The duration of the buffer delivered.
   * @return The number of samples given this configuration.
   */
  static constexpr uint32_t getSampleCountFromRateAndDuration(
      uint32_t sampleRate, Nanoseconds duration) {
    // This function will overflow at high sample rates or extremely high
    // durations, but does work for reasonable expected values.
    //
    // Example: 44100 * 60 seconds (in nanoseconds) fits into a uint64_t as an
    // intermediate value before casting to uint32_t.
    return static_cast<uint32_t>((sampleRate * duration.toRawNanoseconds())
        / kOneSecondInNanoseconds);
  }

 private:
  /**
   * One instance of an audio request from a nanoapp.
   */
  struct AudioRequest {
    AudioRequest(uint32_t instanceId, uint32_t numSamples,
                 Nanoseconds deliveryInterval, Nanoseconds nextEventTimestamp)
        : instanceId(instanceId),
          numSamples(numSamples),
          deliveryInterval(deliveryInterval),
          nextEventTimestamp(nextEventTimestamp) {}

    //! The nanoapp instance ID that owns this request.
    uint32_t instanceId;

    //! The number of samples requested for this request.
    uint32_t numSamples;

    //! The delivery interval for this request.
    Nanoseconds deliveryInterval;

    //! The expected timestamp of the next event delivery.
    Nanoseconds nextEventTimestamp;
  };

  /**
   * A list of audio requests for a given source. Note that each nanoapp may
   * have at most one open request for a given source. When the source is
   * disabled it is removed from this list.
   */
  struct AudioRequestList {
    //! Whether or not the source is available. It is unavailable by default.
    bool available = false;

    //! The request to post the next event to.
    AudioRequest *nextAudioRequest = nullptr;

    //! The list of requests for this source that are currently open.
    DynamicVector<AudioRequest> requests;
  };

  //! Maps audio handles to requests from multiple nanoapps for an audio source.
  //! The array index implies the audio handle which is being managed.
  DynamicVector<AudioRequestList> mAudioRequestLists;

  //! The instance of platform audio that is managed by this AudioRequestManager
  //! and used to service requests for audio data.
  PlatformAudio mPlatformAudio;

  /**
   * Validates the arguments provided to configureSource to ensure that the
   * handle is valid and enable, bufferDuration and deliveryInterval are in a
   * valid configuration.
   *
   * @see configureSource for paramater documentation.
   *
   * @param numSamples Assigned the number of samples for this request if the
   *        return value is true.
   * @return true if the arguments are configured in a valid arrangement.
   */
  bool validateConfigureSourceArguments(uint32_t handle, bool enable,
      uint64_t bufferDuration, uint64_t deliveryInterval, uint32_t *numSamples);

  /**
   * Finds an audio request for a given audio handle and nanoapp instance ID. If
   * no existing request is available, nullptr is returned.
   *
   * @param handle The audio handle to query for. This must be guaranteed by the
   *     caller to be less than the size of the mAudioRequestLists member.
   * @param instanceId The nanoapp instance ID that owns the existing request
   *     for this handle.
   * @param index Populated with the index of the request if it was found.
   * @return The AudioRequest for this handle and instanceId, nullptr if not
   *     found.
   */
  AudioRequest *findAudioRequest(uint32_t handle, uint32_t instanceId,
                                 size_t *index);

  /**
   * Finds the next expiring request for audio data for a given handle.
   *
   * @param handle the handle to determine the next request to service for
   * @return The audio request for this handle that expires next. If no requests
   *         are bound to this handle, nullptr is returned.
   */
  AudioRequest *findNextAudioRequest(uint32_t handle);

  /**
   * Handles an audio data event from the platform synchronously. This is
   * invoked on the CHRE thread through a scheduled callback.
   *
   * @param event The event to provide to nanoapps containg audio data.
   */
  void handleAudioDataEventSync(struct chreAudioDataEvent *event);

  /**
   * Handles audio availability from the platform synchronously. This is
   * invoked on the CHRE thread through a deferred callback. Refer to
   * handleAudioAvailability for details on these supplied parameters.
   */
  void handleAudioAvailabilitySync(uint32_t handle, bool available);

  /**
   * Iterates the list of outstanding requests for the provided handle and
   * schedules the next request to the platform.
   *
   * @param handle the audio source for which to schedule a request.
   */
  void scheduleNextAudioDataEvent(uint32_t handle);

  /**
   * Posts CHRE_EVENT_AUDIO_SAMPLING_CHANGE events to all nanoapps subscribed to
   * the supplied handle.
   *
   * @param handle The handle for the audio source that is changing.
   * @param available true if audio is available for the supplied handle, false
   *        otherwise.
   */
  void postAudioSamplingChangeEvents(uint32_t handle, bool available);

  /**
   * Posts a CHRE_EVENT_AUDIO_SAMPLING_CHANGE event to the specified nanoapp.
   *
   * @param instanceId The instance ID of the nanoapp to post to.
   * @param handle The handle for the audio source that is changing.
   * @param available true if audio is available for the supplied handle, false
   *        otherwise.
   */
  void postAudioSamplingChangeEvent(uint32_t instanceId, uint32_t handle,
                                    bool available);

  /**
   * Posts the provided audio data event to a nanoapp with the given instance ID
   * and fails fatally if the event is not posted. Fatal error is an acceptable
   * error handling mechanism here because there is no way to satisfy the
   * requirements of the API without posting an event.
   *
   * @param audioDataEvent The audio data event to send to a nanoapp.
   * @param instanceId The nanoapp instance ID to direct the event to.
   */
  void postAudioDataEventFatal(struct chreAudioDataEvent *event,
                               uint32_t instanceId);

  /**
   * Invoked by the freeAudioDataEventCallback to decrement the reference count
   * of the most recently published event and free it if unreferenced.
   *
   * @param audioDataEvent the audio data event to process.
   */
  void handleFreeAudioDataEvent(struct chreAudioDataEvent *audioDataEvent);

  /**
   * Releases an audio data event after nanoapps have consumed it.
   *
   * @param eventType the type of event being freed.
   * @param eventData a pointer to the scan event to release.
   */
  static void freeAudioDataEventCallback(uint16_t eventType, void *eventData);
};

}  // namespace chre

#endif  // CHRE_CORE_AUDIO_REQUEST_MANAGER_H_
