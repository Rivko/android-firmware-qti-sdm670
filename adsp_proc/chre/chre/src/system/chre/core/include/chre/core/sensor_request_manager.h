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

#ifndef CHRE_CORE_SENSOR_REQUEST_MANAGER_H_
#define CHRE_CORE_SENSOR_REQUEST_MANAGER_H_

#include "chre/core/request_multiplexer.h"
#include "chre/core/sensor.h"
#include "chre/core/sensor_request.h"
#include "chre/util/fixed_size_vector.h"
#include "chre/util/non_copyable.h"
#include "chre/util/optional.h"

namespace chre {

class SensorRequestManager : public NonCopyable {
 public:
  /**
   * Performs initialization of the SensorRequestManager and populates the
   * sensor list with platform sensors.
   */
  SensorRequestManager();

  /**
   * Destructs the sensor request manager and releases platform sensor resources
   * if requested.
   */
  ~SensorRequestManager();

  /**
   * Determines whether the runtime is aware of a given sensor type. The
   * supplied sensorHandle is only populated if the sensor type is known.
   *
   * @param sensorType The type of the sensor.
   * @param sensorHandle A non-null pointer to a uint32_t to use as a sensor
   *                     handle for nanoapps.
   * @return true if the supplied sensor type is available for use.
   */
  bool getSensorHandle(SensorType sensorType, uint32_t *sensorHandle) const;

  /**
   * Sets a sensor request for the given nanoapp for the provided sensor handle.
   * If the nanoapp has made a previous request, it is replaced by this request.
   * If the request changes the mode to SensorMode::Off the request is removed.
   *
   * @param nanoapp A non-null pointer to the nanoapp requesting this change.
   * @param sensorHandle The sensor handle for which this sensor request is
   *        directed at.
   * @param request The new sensor request for this nanoapp.
   * @return true if the request was set successfully. If the sensorHandle is
   *         out of range or the platform sensor fails to update to the new
   *         request false will be returned.
   */
  bool setSensorRequest(Nanoapp *nanoapp, uint32_t sensorHandle,
                        const SensorRequest& sensorRequest);

  /**
   * Populates the supplied info struct if the sensor handle exists.
   *
   * @param sensorHandle The handle of the sensor.
   * @param nanoapp The nanoapp requesting this change.
   * @param info A non-null pointer to a chreSensorInfo struct.
   * @return true if the supplied sensor handle exists.
   */
  bool getSensorInfo(uint32_t sensorHandle, const Nanoapp& nanoapp,
                     struct chreSensorInfo *info) const;
  /*
   * Removes all requests of a sensorType and unregisters all nanoapps for its
   * events.
   *
   * @param sensorType The sensor type whose requests are to be removed.
   * @return true if all requests of the sensor type have been successfully
   *         removed.
   */
  bool removeAllRequests(SensorType sensorType);

  /**
   * Obtains a pointer to the Sensor of the specified sensorType.
   *
   * @param sensorType The SensorType of the sensor.
   * @return A pointer to the Sensor of sensorType, or nullptr if sensorType is
   *         invalid or the requested SensorType is not supported on the current
   *         platform.
   */
  Sensor *getSensor(SensorType sensorType);

  /**
   * Populates the supplied sampling status struct if the sensor handle exists.
   *
   * @param sensorHandle The handle of the sensor.
   * @param status A non-null pointer to a chreSensorSamplingStatus struct.
   * @return true if the supplied sensor handle exists.
   */
  bool getSensorSamplingStatus(uint32_t sensorHandle,
                               struct chreSensorSamplingStatus *status) const;

  /**
   * Obtains the list of open requests of the specified SensorType.
   *
   * @param sensorType The SensorType of the sensor.
   * @return The list of open requests of this sensor in a DynamicVector.
   */
  const DynamicVector<SensorRequest>& getRequests(SensorType sensorType) const;

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
   * This allows tracking the state of a sensor with the various requests for it
   * and can trigger a change in mode/rate/latency when required.
   */
  struct SensorRequests {
    //! The sensor associated with this request multiplexer. If this Optional
    //! container does not have a value, then the platform does not support this
    //! type of sensor.
    Optional<Sensor> sensor;

    //! The request multiplexer for this sensor.
    RequestMultiplexer<SensorRequest> multiplexer;

    /**
     * Searches through the list of sensor requests for a request owned by the
     * given nanoapp. The provided non-null index pointer is populated with the
     * index of the request if it is found.
     *
     * @param nanoapp The nanoapp whose request is being searched for.
     * @param index A non-null pointer to an index that is populated if a request
     *        for this nanoapp is found.
     * @return A pointer to a SensorRequest that is owned by the provided nanoapp
     *         if one is found otherwise nullptr.
     */
    const SensorRequest *find(const Nanoapp *nanoapp, size_t *index) const;

    /**
     * Adds a new sensor request to the request multiplexer for this sensor.
     *
     * @param request The request to add to the multiplexer.
     * @param requestChanged A non-null pointer to a bool to indicate that the
     *        net request made to the sensor has changed. This boolean is always
     *        assigned to the status of the request changing (true or false).
     * @return true if the add operation was successful.
     */
    bool add(const SensorRequest& request, bool *requestChanged);

    /**
     * Removes a sensor request from the request multiplexer for this sensor.
     * The provided index must fall in the range of the sensor requests managed
     * by the multiplexer.
     *
     * @param removeIndex The index to remove the request from.
     * @param requestChanged A non-null pointer to a bool to indicate that the
     *        net request made to the sensor has changed. This boolean is always
     *        assigned to the status of the request changing (true or false).
     * @return true if the remove operation was successful.
     */
    bool remove(size_t removeIndex, bool *requestChanged);

    /**
     * Updates a sensor request in the request multiplexer for this sensor. The
     * provided index must fall in range of the sensor requests managed by the
     * multiplexer.
     *
     * @param updateIndex The index to update the request at.
     * @param request The new sensor request to replace the existing request
     *        with.
     * @return true if the update operation was successful.
     */
    bool update(size_t updateIndex, const SensorRequest& request,
                bool *requestChanged);

    /**
     * Removes all requests and consolidates all the maximal request changes
     * into one sensor configuration update.
     *
     * @return true if all the requests have been removed and sensor
     *         configuration successfully updated.
     */
    bool removeAll();
  };

  //! The list of sensor requests
  FixedSizeVector<SensorRequests, getSensorTypeCount()> mSensorRequests;
};

}  // namespace chre

#endif  // CHRE_CORE_SENSOR_REQUEST_MANAGER_H_
