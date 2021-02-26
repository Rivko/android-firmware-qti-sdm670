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

#ifndef CHRE_CORE_QC_BLE_REQUEST_MANAGER_H_
#define CHRE_CORE_QC_BLE_REQUEST_MANAGER_H_

#include "chre/core/nanoapp.h"
#include "chre/platform/platform_qc_ble.h"
#include "chre/util/non_copyable.h"
#include "chre/util/time.h"

namespace chre {

/**
 * The QcBleRequestManager handles requests from nanoapps for QcBle information.
 * This includes multiplexing multiple requests into one for the platform to
 * handle.
 *
 * This class is effectively a singleton as there can only be one instance of
 * the PlatformQcBle instance.
 */
class QcBleRequestManager : public NonCopyable {
 public:
   /**
    * Initializes the QcBleRequestManager with a default state and memory for any
    * requests.
    */
  QcBleRequestManager();

  /**
   * Initializes the underlying platform-specific module. Must be 
   * called prior to invoking any other methods in this class. 
   */
  void init();

  /**
   * @return the capabilities exposed by this platform.
   */
  uint32_t getCapabilities();

  /**
   * Handles a request from a nanoapp to configure the scan monitor. This
   * includes merging multiple requests for scan monitoring to the PAL (ie: if
   * multiple apps enable the scan monitor the PAL is only enabled once).
   *
   * @param nanoapp The nanoapp that has requested that the scan monitor be
   *        configured.
   * @param enable true to enable scan monitoring, false to disable scan
   *        monitoring.
   * @param cookie A cookie that is round-tripped back to the nanoapp to
   *        provide a context when making the request.
   * @return true if the request was accepted. The result is delivered
   *         asynchronously through a CHRE event.
   */
  bool configureScanMonitor(Nanoapp *nanoapp, bool enable, const void *cookie);

  /**
   * Handles the result of a request to PlatformQcBle to change the state of the
   * scan monitor.
   *
   * @param enabled true if the result of the operation was an enabled scan
   *        monitor.
   * @param errorCode an error code that is provided to indicate success or what
   *        type of error has occurred. See the chreError enum in the CHRE API
   *        for additional details.
   */
  void handleScanMonitorStateChange(bool enabled, uint8_t errorCode);

  /**
   * Handles a CHRE QC BLE scan event.
   *
   * @param event The  scan event provided to the request manager. 
   *        This memory is guaranteed not to be modified until it
   *        has been explicitly released through the PlatformQcBle
   *        instance.
   */
  void handleScanEvent(chreQcBleScanEvent *event);

  /**
   * Prints state in a string buffer. Must only be called from the context of
   * the main CHRE thread.
   *
   * @param buffer Pointer to the start of the buffer.
   * @param bufferPos Pointer to buffer position to start the print (in-out).
   * @param size Size of the buffer in bytes.
   *
   * @return true if entire log printed, false if overflow or error.
   */
  bool logStateToBuffer(char *buffer, size_t *bufferPos,
                        size_t bufferSize) const;

 private:
  /**
   * Tracks the state of the scan monitor.
   */
  struct ScanMonitorStateTransition {
    //! The nanoapp instance ID that prompted the change.
    uint32_t nanoappInstanceId;

    //! The cookie provided to the CHRE API when the nanoapp requested a change
    //! of state to the scan monitoring.
    const void *cookie;

    //! The target state of the PAL scan monitor.
    bool enable;
  };

  //! The maximum number of scan monitor state transitions that can be queued.
  static constexpr size_t kMaxScanMonitorStateTransitions = 8;

  //! The instance of the platform interface.
  PlatformQcBle mPlatformQcBle;

  //! The queue of state transition requests for the scan monitor. Only one
  //! asynchronous scan monitor state transition can be in flight at one time.
  //! Any further requests are queued here.
  ArrayQueue<ScanMonitorStateTransition,
             kMaxScanMonitorStateTransitions> mScanMonitorStateTransitions;

  //! The list of nanoapps who have enabled scan monitoring. This list is
  //! maintained to ensure that nanoapps are always subscribed to scan
  //! results as requested. Note that a request for scan monitoring can
  //! exceed the duration of a single active scan request. This makes it
  //! insuitable only subscribe to scan events when an active request is
  //! made and the scan monitor must remain enabled when an active request has
  //! completed.
  DynamicVector<uint32_t> mScanMonitorNanoapps;

  // TODO: Support multiple requests for active scans.
  //! The instance ID of the nanoapp that has a pending active scan request. At
  //! this time, only one nanoapp can have a pending request for an active scan.
  Optional<uint32_t> mScanRequestingNanoappInstanceId;

  //! The cookie passed in by a nanoapp making an active request for scans.
  //! Note that this will only be valid if the mScanRequestingNanoappInstanceId
  //! is set.
  const void *mScanRequestingNanoappCookie;

  /**
   * @return true if the scan monitor is enabled by any nanoapps.
   */
  bool scanMonitorIsEnabled() const;

  /**
   * @param instanceId the instance ID of the nanoapp.
   * @param index an optional pointer to a size_t to populate with the index of
   *        the nanoapp in the list of nanoapps.
   * @return true if the nanoapp has an active request for scan monitoring.
   */
  bool nanoappHasScanMonitorRequest(uint32_t instanceId,
                                    size_t *index = nullptr) const;

  /**
   * @param requestedState The requested state to compare against.
   * @param nanoappHasRequest The requesting nanoapp has an existing request.
   * @return true if the scan monitor is in the requested state.
   */
  bool scanMonitorIsInRequestedState(bool requestedState,
                                     bool nanoappHasRequest) const;

  /**
   * @param requestedState The requested state to compare against.
   * @param nanoappHasRequest The requesting nanoapp has an existing request.
   * @return true if a state transition is required to reach the requested
   * state.
   */
  bool scanMonitorStateTransitionIsRequired(bool requestedState,
                                            bool nanoappHasRequest) const;

  /**
   * Builds a scan monitor state transition and adds it to the queue of incoming
   * requests.
   * @param nanoapp A non-null pointer to a nanoapp that is requesting the
   *        change.
   * @param enable The target requested scan monitoring state.
   * @param cookie The pointer cookie passed in by the calling nanoapp to return
   *        to the nanoapp when the request completes.
   * @return true if the request is enqueued or false if the queue is full.
   */
  bool addScanMonitorRequestToQueue(Nanoapp *nanoapp, bool enable,
                                    const void *cookie);

  /**
   * Adds a nanoapp to the list of nanoapps that are monitoring 
   * for scans. 
   * @param enable true if enabling scan monitoring.
   * @param instanceId The instance ID of the scan monitoring nanoapp.
   * @return true if the nanoapp was added to the list.
   */
  bool updateNanoappScanMonitoringList(bool enable, uint32_t instanceId);

  /**
   * Posts an event to a nanoapp indicating the result of a QC BLE
   * scan monitoring configuration change. 
   *
   * @param nanoappInstanceId The nanoapp instance ID to direct the event to.
   * @param success If the request for a resource was successful.
   * @param enable The target state of the request. If enable is set to false
   *        and the request was successful, the nanoapp is removed from the
   *        list of nanoapps requesting scan monitoring.
   * @param errorCode The error code when success is set to false.
   * @param cookie The cookie to be provided to the nanoapp. This is
   *        round-tripped from the nanoapp to provide context.
   * @return true if the event was successfully posted to the event loop.
   */
  bool postScanMonitorAsyncResultEvent(
      uint32_t nanoappInstanceId, bool success, bool enable, uint8_t errorCode,
      const void *cookie);

  /**
   * Calls through to postScanMonitorAsyncResultEvent but invokes the
   * FATAL_ERROR macro if the event is not posted successfully. This is used in
   * asynchronous contexts where a nanoapp could be stuck waiting for a response
   * but CHRE failed to enqueue one. For parameter details,
   * @see postScanMonitorAsyncResultEvent
   */
  void postScanMonitorAsyncResultEventFatal(
      uint32_t nanoappInstanceId, bool success, bool enable, uint8_t errorCode,
      const void *cookie);

  /**
   * Posts a broadcast event containing the results of a scan. 
   * Failure to post this event is a FATAL_ERROR. This is 
   * unrecoverable as the nanoapp will be stuck waiting for scan 
   * results but there may be a gap. 
   *
   * @param event the scan event.
   */
  void postScanEventFatal(chreQcBleScanEvent *event);

  /**
   * Handles the result of a request to PlatformQcBle to change the state of the
   * scan monitor. See the handleScanMonitorStateChange method which may be
   * called from any thread. This method is intended to be invoked on the CHRE
   * event loop thread.
   *
   * @param enabled true if the result of the operation was an enabled scan
   *        monitor.
   * @param errorCode an error code that is provided to indicate success or what
   *        type of error has occurred. See the chreError enum in the CHRE API
   *        for additional details.
   */
  void handleScanMonitorStateChangeSync(bool enabled, uint8_t errorCode);

  /**
   * Handles the result of a request to PlatformQcBle to perform 
   * an active scan. See the handleScanResponse method which may 
   * be called from any thread. This method is intended to be 
   * invoked on the CHRE event loop thread. 
   *
   * @param enabled true if the result of the operation was an enabled scan
   *        monitor.
   * @param errorCode an error code that is provided to indicate success or what
   *        type of error has occurred. See the chreError enum in the CHRE API
   *        for additional details.
   */
  void handleScanResponseSync(bool pending, uint8_t errorCode);

  /**
   * Handles a scan event. See the handleScanEvent method with may 
   * be called from any thread. This method is intended to be 
   * invoked on the CHRE event loop thread. 
   *
   * @param event The event to distribute to nanoapps.
   */
  void handleScanEventSync(chreQcBleScanEvent *event);

  /**
   * Handles the releasing of a scan event and unsubscribes a 
   * nanoapp who has made an active request for a scan from scan 
   * events in the future (if it has not subscribed to passive 
   * events). 
   *
   * @param scanEvent The scan event to release.
   */
  void handleFreeQcBleScanEvent(chreQcBleScanEvent *scanEvent);

  /**
   * Releases a scan event after nanoapps have consumed it.
   *
   * @param eventType the type of event being freed.
   * @param eventData a pointer to the scan event to release.
   */
  static void freeQcBleScanEventCallback(uint16_t eventType, void *eventData);

  //! System time when last scan request was made.
  Nanoseconds mLastScanRequestTime;
};

}  // namespace chre

#endif  // CHRE_CORE_QC_BLE_REQUEST_MANAGER_H_
