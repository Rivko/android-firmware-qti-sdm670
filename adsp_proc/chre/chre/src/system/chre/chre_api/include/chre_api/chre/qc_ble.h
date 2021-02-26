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

#ifndef _CHRE_QC_BLE_H_
#define _CHRE_QC_BLE_H_

#include <chre/common.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The set of flags returned by chreBleGetCapabilities().
 * @defgroup CHRE_BLE_CAPABILITIES
 * @{
 */

//! No QC BLE APIs are supported
#define CHRE_QC_BLE_CAPABILITIES_NONE             UINT32_C(0)

//! Listening to scan results is supported, as enabled via
//! chreQcBleConfigureScanMonitorAsync()
#define CHRE_QC_BLE_CAPABILITIES_SCAN_MONITORING  UINT32_C(1 << 0)

/** @} */

/**
 * Produce an event ID in the block of IDs reserved for QC BLE
 * @param offset  Index into QC BLE event ID block; valid range 
 *                [0,15]
 */
#define CHRE_QC_BLE_EVENT_ID(offset)  (CHRE_EVENT_QC_BLE_FIRST_EVENT + (offset))

/**
 * nanoappHandleEvent argument: struct chreAsyncResult
 *
 * Communicates the asynchronous result of a request to the QC 
 * BLE API. The requestType field in chreAsyncResult is set to a
 * value from enum chreQcBleRequestType. 
 */
#define CHRE_EVENT_QC_BLE_ASYNC_RESULT  CHRE_QC_BLE_EVENT_ID(0)

/**
 * nanoappHandleEvent argument: struct chreQcBleScanEvent
 *
 * Provides results of a QC BLE scan.
 */
#define CHRE_EVENT_QC_BLE_SCAN_RESULT  CHRE_QC_BLE_EVENT_ID(1)

// NOTE: Do not add new events with ID > 15; only values 0-15 are reserved
// (see chre/event.h)

/**
 * The current compatibility version of the chreQcBleScanEvent structure,
 * including nested structures.
 */
#define CHRE_QC_BLE_SCAN_EVENT_VERSION  UINT8_C(1)


/**
 * Indicates a type of request made in this API. Used to populate the resultType
 * field of struct chreAsyncResult sent with 
 * CHRE_EVENT_QC_BLE_ASYNC_RESULT. 
 */
enum chreQcBleRequestType {
    CHRE_QC_BLE_REQUEST_TYPE_CONFIGURE_SCAN_MONITOR = 1,
};

/**
 * Data structure sent by QC BLE driver.
 */
struct chreQcBleScanEvent {
    //! Indicates the version of the structure, for compatibility purposes.
    //! Clients do not normally need to worry about this field; the CHRE
    //! implementation guarantees that the client only receives the structure
    //! version it expects.
    uint8_t version;

    // Size of result in bytes,
    // i.e. sizeof(result)
    // the whole size of this structure is size + sizeof(struct chreQcBleScanEvent).
    uint32_t size;

    // array contains the whole packet of one QC BLE advertisemet event.
    unsigned char result[0];
};

/**
 * Retrieves a set of flags indicating the Ble features supported by the
 * current CHRE implementation. The value returned by this function must be
 * consistent for the entire duration of the Nanoapp's execution.
 *
 * The client must allow for more flags to be set in this response than it knows
 * about, for example if the implementation supports a newer version of the API
 * than the client was compiled against.
 *
 * @return A bitmask with zero or more 
 *         CHRE_QC_BLE_CAPABILITIES_* flags set
 *
 * @since v1.1
 */
uint32_t chreQcBleGetCapabilities(void);

/**
 * Manages a client's request to receive the results of Ble scans performed from
 * android side, for example sensor app in android side will set filter and start
 * ble routing scan.
 *
 * These scan results are delivered to the Nanoapp's handle event callback using
 * CHRE_EVENT_QC_BLE_SCAN_RESULT.
 *
 * This result of this request is delivered asynchronously via an event of type
 * CHRE_EVENT_QC_BLE_ASYNC_RESULT. Refer to the note in {@link 
 * #chreAsyncResult} for more details. 
 *
 * @param enable Set to true to enable monitoring scan results, false to
 *        disable
 * @param cookie An opaque value that will be included in the chreAsyncResult
 *        sent in relation to this request.
 *
 * @return true if the request was accepted for processing, false otherwise
 *
 * @since v1.1
 */
bool chreQcBleConfigureScanMonitorAsync(bool enable, const void *cookie);

#ifdef __cplusplus
}
#endif

#endif  /* _CHRE_QC_BLE_H_ */
