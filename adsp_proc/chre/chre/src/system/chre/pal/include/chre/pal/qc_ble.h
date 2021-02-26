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

#ifndef CHRE_PAL_QC_BLE_H_
#define CHRE_PAL_QC_BLE_H_

/**
 * @file
 * Defines the interface between the common CHRE core system and the
 * platform-specific QC BLE module.
 */

#include <stdbool.h>
#include <stdint.h>

#include "chre_api/chre/common.h"
#include "chre_api/chre/qc_ble.h"
#include "chre/pal/system.h"
#include "chre/pal/version.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initial version of the CHRE QC BLE PAL, tied to CHRE API 
 * v1.1. 
 */
#define CHRE_PAL_QC_BLE_API_V1_0  CHRE_PAL_CREATE_API_VERSION(1, 0)

/**
 * The version of the QC BLE PAL defined in this header file.
 */
#define CHRE_PAL_QC_BLE_API_CURRENT_VERSION  CHRE_PAL_QC_BLE_API_V1_0

struct chrePalQcBleCallbacks {
    /**
     * Callback invoked to inform the CHRE of the result of changes to the scan
     * monitor registration status requested via configureScanMonitor in struct
     * chrePalQcBleApi.
     *
     * Unsolicited calls to this function must not be made. In other words,
     * this callback should only be invoked as the direct result of an earlier
     * call to configureScanMonitor. If the scan monitor registration is lost,
     * for example due to a reset of the Ble subsystem, then the PAL
     * implementation is required to silently re-register the scan monitor when
     * it recovers, as needed.
     *
     * @param enabled true if the scan monitor is currently active and
     *        scanEventCallback will receive unsolicited scan results, false
     *        otherwise
     * @param errorCode An error code from enum chreError
     *
     * @see #configureScanMonitor
     * @see #chreError
     */
    void (*scanMonitorStatusChangeCallback)(bool enabled, uint8_t errorCode);

    /**
     * Callback used to pass scan results from the Ble module to the core CHRE
     * system, which distributes it to clients (nanoapps).
     *
     * This function call passes ownership of the event memory to the core CHRE
     * system, i.e. the PAL module must not modify the referenced data until the
     * associated API function is called to release the memory.
     *
     * @param event Event data to distribute to clients. The Ble module
     *        must ensure that this memory remains accessible until it is passed
     *        to the releaseScanEvent() function in struct
     *        chrePalQcBleApi.
     *
     * @see #configureScanMonitor
     */
    void (*scanEventCallback)(struct chreQcBleScanEvent *event);
};

struct chrePalQcBleApi {
    /**
     * Version of the module providing this API. This value should be
     * constructed from CHRE_PAL_CREATE_MODULE_VERSION using the supported
     * API version constant (CHRE_PAL_QC_BLE_API_*) and the 
     * module-specific patch version. 
     */
    uint32_t moduleVersion;

    /**
     * Initializes the QC BLE module. Initialization must complete 
     * synchronously. 
     *
     * @param systemApi Structure containing CHRE system function pointers which
     *        the PAL implementation should prefer to use over equivalent
     *        functionality exposed by the underlying platform. The module does
     *        not need to deep-copy this structure; its memory remains
     *        accessible at least until after close() is called.
     * @param callbacks Structure containing entry points to the core CHRE
     *        system. The module does not need to deep-copy this structure; its
     *        memory remains accessible at least until after close() is called.
     *
     * @return true if initialization was successful, false otherwise
     */
    bool (*open)(const struct chrePalSystemApi *systemApi,
                 const struct chrePalQcBleCallbacks *callbacks);

    /**
     * Performs clean shutdown of the QC BLE module, usually done in
     * preparation for stopping the CHRE. The QC BLE module must 
     * ensure that it will not invoke any callbacks past this point, 
     * and complete any relevant teardown activities before 
     * returning from this function. 
     */
    void (*close)(void);

    //! @see chreQcBleGetCapabilities()
    uint32_t (*getCapabilities)(void);

    /**
     * Configures whether the scanEventCallback receives unsolicited scan
     * results.
     *
     * @param enable true to enable listening for all available scan results
     *
     * @return true if the request was accepted for processing, in which case a
     *         subsequent call to scanMonitorStatusChangeCallback will be used
     *         to communicate the result of the operation
     *
     * @see chreQcBleConfigureScanMonitorAsync()
     */
    bool (*configureScanMonitor)(bool enable);

    /**
     * Invoked when the core CHRE system no longer needs a QC BLE 
     * scan event structure that was provided to it via 
     * scanEventCallback() 
     *
     * @param event Event data to release
     */
    void (*releaseScanEvent)(struct chreQcBleScanEvent *event);
};

/**
 * Retrieve a handle for the CHRE QC BLE PAL.
 *
 * @param requestedApiVersion The implementation of this function must return a
 *        pointer to a structure with the same major version as requested.
 *
 * @return Pointer to API handle, or NULL if a compatible API version is not
 *         supported by the module, or the API as a whole is not implemented. If
 *         non-NULL, the returned API handle must be valid as long as this
 *         module is loaded.
 */
const struct chrePalQcBleApi *chrePalQcBleGetApi(uint32_t requestedApiVersion);

#ifdef __cplusplus
}
#endif

#endif  // CHRE_PAL_QC_BLE_H_

