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

#include <cinttypes>

#include "chre/core/event_loop_manager.h"
#include "chre/core/qc_ble_request_manager.h"
#include "chre/platform/fatal_error.h"
#include "chre/platform/log.h"
#include "chre/platform/system_time.h"
#include "chre/util/system/debug_dump.h"

namespace chre {

QcBleRequestManager::QcBleRequestManager() {
  // Reserve space for at least one scan monitoring nanoapp. This ensures that
  // the first asynchronous push_back will succeed. Future push_backs will be
  // synchronous and failures will be returned to the client.
  if (!mScanMonitorNanoapps.reserve(1)) {
    FATAL_ERROR("Failed to allocate scan monitoring nanoapps list at startup");
  }
}

void QcBleRequestManager::init() {
  mPlatformQcBle.init();
}

uint32_t QcBleRequestManager::getCapabilities() {
  return mPlatformQcBle.getCapabilities();
}

bool QcBleRequestManager::configureScanMonitor(Nanoapp *nanoapp, bool enable,
                                              const void *cookie) {
  CHRE_ASSERT(nanoapp);

  bool success = false;
  uint32_t instanceId = nanoapp->getInstanceId();
  bool hasScanMonitorRequest = nanoappHasScanMonitorRequest(instanceId);
  if (!mScanMonitorStateTransitions.empty()) {
    success = addScanMonitorRequestToQueue(nanoapp, enable, cookie);
  } else if (scanMonitorIsInRequestedState(enable, hasScanMonitorRequest)) {
    // The scan monitor is already in the requested state. A success event can
    // be posted immediately.
    success = postScanMonitorAsyncResultEvent(instanceId, true /* success */,
                                              enable, CHRE_ERROR_NONE, cookie);
  } else if (scanMonitorStateTransitionIsRequired(enable,
                                                  hasScanMonitorRequest)) {
    success = addScanMonitorRequestToQueue(nanoapp, enable, cookie);
    if (success) {
      success = mPlatformQcBle.configureScanMonitor(enable);
      if (!success) {
        // TODO: Add a pop_back method.
        mScanMonitorStateTransitions.remove(
            mScanMonitorStateTransitions.size() - 1);
        LOGE("Failed to enable the scan monitor for nanoapp instance %" PRIu32,
             instanceId);
      }
    }
  } else {
    CHRE_ASSERT_LOG(false, "Invalid scan monitor configuration");
  }

  return success;
}

void QcBleRequestManager::handleScanMonitorStateChange(bool enabled,
                                                      uint8_t errorCode) {
  struct CallbackState {
    bool enabled;
    uint8_t errorCode;
  };

  auto *cbState = memoryAlloc<CallbackState>();
  if (cbState == nullptr) {
    LOGE("Failed to allocate callback state for scan monitor state change");
  } else {
    cbState->enabled = enabled;
    cbState->errorCode = errorCode;

    auto callback = [](uint16_t /* eventType */, void *eventData) {
      auto *state = static_cast<CallbackState *>(eventData);
      EventLoopManagerSingleton::get()->getQcBleRequestManager()
          .handleScanMonitorStateChangeSync(state->enabled, state->errorCode);
      memoryFree(state);
    };

    EventLoopManagerSingleton::get()->deferCallback(
        SystemCallbackType::QcBleScanMonitorStateChange, cbState, callback);
  }
}

void QcBleRequestManager::handleScanEvent(chreQcBleScanEvent *event) {
  auto callback = [](uint16_t eventType, void *eventData) {
    chreQcBleScanEvent *scanEvent = static_cast<chreQcBleScanEvent *>(eventData);
    EventLoopManagerSingleton::get()->getQcBleRequestManager()
        .handleScanEventSync(scanEvent);
  };

  EventLoopManagerSingleton::get()->deferCallback(
      SystemCallbackType::QcBleHandleScanEvent, event, callback);
}

bool QcBleRequestManager::logStateToBuffer(char *buffer, size_t *bufferPos,
                                          size_t bufferSize) const {
  bool success = debugDumpPrint(buffer, bufferPos, bufferSize, "\nQcBle: "
                                "scan monitor %s\n", scanMonitorIsEnabled() ?
                                "enabled" : "disabled");

  success &= debugDumpPrint(buffer, bufferPos, bufferSize,
                            " QcBle scan monitor enabled nanoapps:\n");
  for (const auto& instanceId : mScanMonitorNanoapps) {
    success &= debugDumpPrint(buffer, bufferPos, bufferSize,
                              "  nanoappId=%" PRIu32 "\n", instanceId);
  }

  if (mScanRequestingNanoappInstanceId.has_value()) {
    success &= debugDumpPrint(buffer, bufferPos, bufferSize,
                              " QcBle request pending nanoappId=%" PRIu32 "\n",
                              mScanRequestingNanoappInstanceId.value());
  }

  success &= debugDumpPrint(buffer, bufferPos, bufferSize,
                            " QcBle transition queue:\n");
  for (const auto& transition : mScanMonitorStateTransitions) {
    success &= debugDumpPrint(buffer, bufferPos, bufferSize,
                              "  enable=%s nanoappId=%" PRIu32 "\n",
                              transition.enable ? "true" : "false",
                              transition.nanoappInstanceId);
  }

  return success;
}

bool QcBleRequestManager::scanMonitorIsEnabled() const {
  return !mScanMonitorNanoapps.empty();
}

bool QcBleRequestManager::nanoappHasScanMonitorRequest(
    uint32_t instanceId, size_t *nanoappIndex) const {
  size_t index = mScanMonitorNanoapps.find(instanceId);
  bool hasScanMonitorRequest = (index != mScanMonitorNanoapps.size());
  if (hasScanMonitorRequest && nanoappIndex != nullptr) {
    *nanoappIndex = index;
  }

  return hasScanMonitorRequest;
}

bool QcBleRequestManager::scanMonitorIsInRequestedState(
    bool requestedState, bool nanoappHasRequest) const {
  return (requestedState == scanMonitorIsEnabled() || (!requestedState
      && (!nanoappHasRequest || mScanMonitorNanoapps.size() > 1)));
}

bool QcBleRequestManager::scanMonitorStateTransitionIsRequired(
    bool requestedState, bool nanoappHasRequest) const {
  return ((requestedState && mScanMonitorNanoapps.empty())
      || (!requestedState && nanoappHasRequest
              && mScanMonitorNanoapps.size() == 1));
}

bool QcBleRequestManager::addScanMonitorRequestToQueue(Nanoapp *nanoapp,
                                                      bool enable,
                                                      const void *cookie) {
  ScanMonitorStateTransition scanMonitorStateTransition;
  scanMonitorStateTransition.nanoappInstanceId = nanoapp->getInstanceId();
  scanMonitorStateTransition.cookie = cookie;
  scanMonitorStateTransition.enable = enable;

  bool success = mScanMonitorStateTransitions.push(scanMonitorStateTransition);
  if (!success) {
    LOGW("Too many scan monitor state transitions");
  }

  return success;
}

bool QcBleRequestManager::updateNanoappScanMonitoringList(bool enable,
                                                         uint32_t instanceId) {
  bool success = true;
  Nanoapp *nanoapp = EventLoopManagerSingleton::get()->getEventLoop()
      .findNanoappByInstanceId(instanceId);
  if (nanoapp == nullptr) {
    CHRE_ASSERT_LOG(false, "Failed to update scan monitoring list for "
                    "non-existent nanoapp");
  } else {
    size_t nanoappIndex;
    bool hasExistingRequest = nanoappHasScanMonitorRequest(instanceId,
                                                           &nanoappIndex);
    if (enable) {
      if (!hasExistingRequest) {
        success = nanoapp->registerForBroadcastEvent(
            CHRE_EVENT_QC_BLE_SCAN_RESULT);
        if (!success) {
          LOGE("Failed to register nanoapp for QcBle scan events");
        } else {
          // The scan monitor was successfully enabled for this nanoapp and
          // there is no existing request. Add it to the list of scan monitoring
          // nanoapps.
          success = mScanMonitorNanoapps.push_back(instanceId);
          if (!success) {
            nanoapp->unregisterForBroadcastEvent(CHRE_EVENT_QC_BLE_SCAN_RESULT);
            LOGE("Failed to add nanoapp to the list of scan monitoring "
                 "nanoapps");
          }
        }
      }
    } else {
      if (!hasExistingRequest) {
        success = false;
        LOGE("Received a scan monitor state change for a non-existent nanoapp");
      } else {
        // The scan monitor was successfully disabled for a previously enabled
        // nanoapp. Remove it from the list of scan monitoring nanoapps.
        mScanMonitorNanoapps.erase(nanoappIndex);
        nanoapp->unregisterForBroadcastEvent(CHRE_EVENT_QC_BLE_SCAN_RESULT);
      }
    }
  }

  return success;
}

bool QcBleRequestManager::postScanMonitorAsyncResultEvent(
    uint32_t nanoappInstanceId, bool success, bool enable, uint8_t errorCode,
    const void *cookie) {
  // Allocate and post an event to the nanoapp requesting QcBle.
  bool eventPosted = false;
  if (!success || updateNanoappScanMonitoringList(enable, nanoappInstanceId)) {
    chreAsyncResult *event = memoryAlloc<chreAsyncResult>();
    if (event == nullptr) {
      LOGE("Failed to allocate QcBle scan monitor async result event");
    } else {
      event->requestType = CHRE_QC_BLE_REQUEST_TYPE_CONFIGURE_SCAN_MONITOR;
      event->success = success;
      event->errorCode = errorCode;
      event->reserved = 0;
      event->cookie = cookie;

      // Post the event.
      eventPosted = EventLoopManagerSingleton::get()->getEventLoop()
          .postEvent(CHRE_EVENT_QC_BLE_ASYNC_RESULT, event, freeEventDataCallback,
                     kSystemInstanceId, nanoappInstanceId);
      if (!eventPosted) {
        memoryFree(event);
      }
    }
  }

  return eventPosted;
}

void QcBleRequestManager::postScanMonitorAsyncResultEventFatal(
    uint32_t nanoappInstanceId, bool success, bool enable, uint8_t errorCode,
    const void *cookie) {
  if (!postScanMonitorAsyncResultEvent(nanoappInstanceId, success, enable,
                                       errorCode, cookie)) {
    FATAL_ERROR("Failed to send QcBle scan monitor async result event");
  }
}

void QcBleRequestManager::postScanEventFatal(chreQcBleScanEvent *event) {
  bool eventPosted = EventLoopManagerSingleton::get()->getEventLoop()
      .postEvent(CHRE_EVENT_QC_BLE_SCAN_RESULT, event, freeQcBleScanEventCallback,
                 kSystemInstanceId, kBroadcastInstanceId);
  if (!eventPosted) {
    FATAL_ERROR("Failed to send QcBle scan event");
  }
}

void QcBleRequestManager::handleScanMonitorStateChangeSync(bool enabled,
                                                          uint8_t errorCode) {
  // Success is defined as having no errors ... in life ༼ つ ◕_◕ ༽つ
  bool success = (errorCode == CHRE_ERROR_NONE);

  // TODO(b/62904616): re-enable this assertion
  //CHRE_ASSERT_LOG(!mScanMonitorStateTransitions.empty(),
  //                "handleScanMonitorStateChangeSync called with no transitions");
  if (mScanMonitorStateTransitions.empty()) {
    LOGE("QcBle PAL error: handleScanMonitorStateChangeSync called with no "
         "transitions (enabled %d errorCode %" PRIu8 ")", enabled, errorCode);
  }

  // Always check the front of the queue.
  if (!mScanMonitorStateTransitions.empty()) {
    const auto& stateTransition = mScanMonitorStateTransitions.front();
    success &= (stateTransition.enable == enabled);
    postScanMonitorAsyncResultEventFatal(stateTransition.nanoappInstanceId,
                                         success, stateTransition.enable,
                                         errorCode, stateTransition.cookie);
    mScanMonitorStateTransitions.pop();
  }

  while (!mScanMonitorStateTransitions.empty()) {
    const auto& stateTransition = mScanMonitorStateTransitions.front();
    bool hasScanMonitorRequest = nanoappHasScanMonitorRequest(
        stateTransition.nanoappInstanceId);
    if (scanMonitorIsInRequestedState(
        stateTransition.enable, hasScanMonitorRequest)) {
      // We are already in the target state so just post an event indicating
      // success
      postScanMonitorAsyncResultEventFatal(stateTransition.nanoappInstanceId,
                                           success, stateTransition.enable,
                                           errorCode, stateTransition.cookie);
    } else if (scanMonitorStateTransitionIsRequired(
        stateTransition.enable, hasScanMonitorRequest)) {
      if (mPlatformQcBle.configureScanMonitor(stateTransition.enable)) {
        break;
      } else {
        postScanMonitorAsyncResultEventFatal(stateTransition.nanoappInstanceId,
                                             false /* success */,
                                             stateTransition.enable, CHRE_ERROR,
                                             stateTransition.cookie);
      }
    } else {
      CHRE_ASSERT_LOG(false, "Invalid scan monitor state");
      break;
    }

    mScanMonitorStateTransitions.pop();
  }
}

void QcBleRequestManager::handleScanEventSync(chreQcBleScanEvent *event) {
  postScanEventFatal(event);
}

void QcBleRequestManager::handleFreeQcBleScanEvent(chreQcBleScanEvent *scanEvent) {
  if (mScanRequestingNanoappInstanceId.has_value()) {
    Nanoapp *nanoapp = EventLoopManagerSingleton::get()->getEventLoop()
    .findNanoappByInstanceId(*mScanRequestingNanoappInstanceId);
    if (nanoapp == nullptr) {
      CHRE_ASSERT_LOG(false, "Attempted to unsubscribe unknown nanoapp from "
                      "QcBle scan events");
    } else if (!nanoappHasScanMonitorRequest(
       *mScanRequestingNanoappInstanceId)) {
      nanoapp->unregisterForBroadcastEvent(CHRE_EVENT_QC_BLE_SCAN_RESULT);
    }

    mScanRequestingNanoappInstanceId.reset();
  }

  mPlatformQcBle.releaseScanEvent(scanEvent);
}

void QcBleRequestManager::freeQcBleScanEventCallback(uint16_t eventType,
                                                   void *eventData) {
  chreQcBleScanEvent *scanEvent = static_cast<chreQcBleScanEvent *>(eventData);
  EventLoopManagerSingleton::get()->getQcBleRequestManager()
      .handleFreeQcBleScanEvent(scanEvent);
}

}  // namespace chre
