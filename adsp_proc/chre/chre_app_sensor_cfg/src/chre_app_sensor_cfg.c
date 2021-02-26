/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <chre.h>
#include <chre/event.h>
#include <chre/wifi.h>
#include <chre/gnss.h>
#include <chre/qc_ble.h>
#include <chre/qc_geofence.h>
#include <chre/wwan.h>

#include "HAP_farf.h"

#define CHRE_WIFI_TYPE_ACTIVE (CHRE_EVENT_SENSOR_LAST_EVENT + 1)
#define CHRE_WIFI_TYPE_PASSIVE (CHRE_EVENT_SENSOR_LAST_EVENT + 2)
#define CHRE_GNSS_TYPE_LOCATION (CHRE_EVENT_SENSOR_LAST_EVENT + 3)
#define CHRE_GNSS_TYPE_DATA (CHRE_EVENT_SENSOR_LAST_EVENT + 4)
#define CHRE_GNSS_TYPE_GF (CHRE_EVENT_SENSOR_LAST_EVENT + 5)
#define CHRE_WWAN_TYPE (CHRE_EVENT_SENSOR_LAST_EVENT + 6)
#define CHRE_QC_BLE_TYPE_PASSIVE (CHRE_EVENT_SENSOR_LAST_EVENT + 7)

// TODO get from common header
#define CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE 0x00400
#define EVENT_SENSOR_CFG    CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE
#define EVENT_GF_CFG        CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE+1

#define GF_ARRAY_SIZE 5

enum GeofenceCmds {
   GEOFENCE_CMD_INIT = 0,
   GEOFENCE_CMD_ADD = 1,
   GEOFENCE_CMD_REMOVE = 2,
   GEOFENCE_CMD_MODIFY = 3,
   GEOFENCE_CMD_PAUSE = 4,
   GEOFENCE_CMD_RESUME = 5,
   GEOFENCE_CMD_PRINT = 6,
   GEOFENCE_CMD_DEINIT = 7,
};

struct GeofenceCmd {
   uint32_t cmd; // enum GeofenceCmds
   uint32_t gfIdArray[GF_ARRAY_SIZE];
   qcGeofenceDataType gfDataArray[GF_ARRAY_SIZE];
   qcGeofenceOptionsType gfOptionsArray[GF_ARRAY_SIZE];
   uint32_t gfCount;
};

struct ConfigCmd {
   uint64_t latency;
   uint64_t interval;
   uint32_t sensType;
   uint32_t mode;
   uint32_t flags;
};

static void sensor_cfg_heapFree(void* message, size_t messageSize)
{
   //FARF(HIGH, "sensor_cfg_heapFree messageSize 0x%X", messageSize);
   chreHeapFree(message);
}

static void sendGfErrorMessageToHost()
{
   void* pv;
   uint32_t len;
   size_t count;

   count = 9999; /* special value to indicate the cmd failed */
   len = sizeof(size_t);
   pv = chreHeapAlloc(len);
   if (0 == pv) {
      FARF(ERROR, "failed to alloc event to host");
      return;
   }
   memcpy((char*)pv, &count, sizeof(size_t));

   if ((true != chreSendMessageToHost(pv, len, CHRE_EVENT_GNSS_GF_RESPONSE, sensor_cfg_heapFree))) {
      FARF(ERROR, "failed to send sample");
   }
}

void nanoappHandleEvent(uint32_t senderInstanceId, uint16_t evtType, const void* evtData)
{
   FARF(HIGH, "app_sensor_cfg got event 0x%X", (unsigned int)evtType);

   switch (evtType) {
      case CHRE_EVENT_MESSAGE_FROM_HOST:
      {
         struct chreMessageFromHostData* msg = (struct chreMessageFromHostData*)evtData;
         struct ConfigCmd* cmd;
         enum chreSensorConfigureMode mode;
         uint32_t h;

         if (EVENT_SENSOR_CFG == msg->messageType) {
            if (msg->messageSize != sizeof(struct ConfigCmd)) {
               FARF(ERROR, "expected message size %d got %d bytes",
                    sizeof(struct ConfigCmd),
                    msg->messageSize);
               return;
            }
            cmd = (struct ConfigCmd*)msg->message;

            //FARF(HIGH, "got config command type 0x%X mode %d flags 0x%X", cmd->sensType, cmd->mode, cmd->flags);
            switch (cmd->sensType) {
               case CHRE_SENSOR_TYPE_ACCELEROMETER:
               case CHRE_SENSOR_TYPE_UNCALIBRATED_ACCELEROMETER:
               case CHRE_SENSOR_TYPE_ACCELEROMETER_TEMPERATURE:
               case CHRE_SENSOR_TYPE_GYROSCOPE:
               case CHRE_SENSOR_TYPE_UNCALIBRATED_GYROSCOPE:
               case CHRE_SENSOR_TYPE_GYROSCOPE_TEMPERATURE:
               case CHRE_SENSOR_TYPE_GEOMAGNETIC_FIELD:
               case CHRE_SENSOR_TYPE_UNCALIBRATED_GEOMAGNETIC_FIELD:
               case CHRE_SENSOR_TYPE_GEOMAGNETIC_FIELD_TEMPERATURE:
               case CHRE_SENSOR_TYPE_PRESSURE:
               case CHRE_SENSOR_TYPE_LIGHT:
               case CHRE_SENSOR_TYPE_PROXIMITY:
               case CHRE_SENSOR_TYPE_INSTANT_MOTION_DETECT:
               case CHRE_SENSOR_TYPE_STATIONARY_DETECT:
                  if (!chreSensorFindDefault(cmd->sensType, &h)) {
                     FARF(ERROR, "cannot find sensor type 0x%X", cmd->sensType);
                     return;
                  }

                  mode = cmd->mode;

                  FARF(HIGH, "configure sensor type 0x%X mode 0x%X interval 0x%llu latency 0x%llu",
                       cmd->sensType, mode, cmd->interval, cmd->latency);
                  if (!chreSensorConfigure(h, mode, cmd->interval, cmd->latency)) {
                     FARF(ERROR, "failed to configure sensor");
                     return;
                  }
                  break;

               case CHRE_WIFI_TYPE_ACTIVE:
               {
                  if (cmd->mode) {
                     struct chreWifiScanParams params = { };
                     params.scanType = CHRE_WIFI_SCAN_TYPE_ACTIVE;
                     params.maxScanAgeMs = 5000;  // 5 seconds
                     params.frequencyListLen = 0;
                     params.ssidListLen = 0;
					 params.radioChainPref = CHRE_WIFI_RADIO_CHAIN_PREF_DEFAULT;

                     if (!chreWifiRequestScanAsync(&params, 0)) {
                        //if (!chreWifiRequestScanAsyncDefault(0)) { // TODO inline not working
                        FARF(ERROR, "chreWifiRequestScanAsyncDefault failed!");
                     }
                  }
                  return;
               }

               case CHRE_WIFI_TYPE_PASSIVE:
                  if (!chreWifiConfigureScanMonitorAsync(cmd->mode ? 1 : 0, 0)) {
                     FARF(ERROR, "chreWifiConfigureScanMonitorAsync failed!");
                  }
                  return;

               case CHRE_QC_BLE_TYPE_PASSIVE:
                  FARF(HIGH, "QC BLE Capabilities 0x%X", chreQcBleGetCapabilities());
                  if (!chreQcBleConfigureScanMonitorAsync(cmd->mode ? 1 : 0, 0)) {
                     FARF(ERROR, "chreQcBleConfigureScanMonitorAsync failed!");
                  }
                  return;

               case CHRE_GNSS_TYPE_LOCATION:
                  if (cmd->mode) {
                     if (!chreGnssLocationSessionStartAsync((uint32_t)cmd->interval, 0, 0)) {
                        FARF(ERROR, "chreevtessionStartAsync failed!");
                     }
                  } else {
                     if (!chreGnssLocationSessionStopAsync(0)) {
                        FARF(ERROR, "chreevtessionStopAsync failed!");
                     }
                  }
                  return;

               case CHRE_GNSS_TYPE_DATA:
                  // TODO
                  FARF(ERROR, "GNSS data not currently supported!");
                  /*if (cmd->cmd) {
                  if (!chreGnssMeasurementSessionStartAsync((uint32_t)cmd->interval, 0)) {
                  FARF(ERROR, "chrechreGnssMeasurementSessionStartAsync failed!");
                  }
                  } else {
                  if (!chreGnssMeasurementSessionStopAsync(0)) {
                  FARF(ERROR, "chrechreGnssMeasurementSessionStopAsync failed!");
                  }
                  }*/
                  return;

               case CHRE_WWAN_TYPE:
                  if (!chreWwanGetCellInfoAsync(0)) {
                     FARF(ERROR, "chreWwanGetCellInfoAsync failed!");
                  }
                  return;

               default:
                  FARF(ERROR, "Unknown sensor type %d", cmd->sensType);
            }
         } else if (EVENT_GF_CFG == msg->messageType) {
            if (msg->messageSize != sizeof(struct GeofenceCmd)) {
               FARF(ERROR, "expected message size %d got %d bytes",
                    sizeof(struct GeofenceCmd),
                    msg->messageSize);
               return;
            }

            struct GeofenceCmd* gf_cmd;
            uint32_t* ids = NULL;

            gf_cmd = (struct GeofenceCmd*)msg->message;
            switch (gf_cmd->cmd) {
               case GEOFENCE_CMD_INIT:
                  if (chreQcGeofenceInit() != QC_GEOFENCE_ERROR_SUCCESS) {
                     FARF(ERROR, "chreQcGeofenceInit failed!");
                  }
                  break;

               case GEOFENCE_CMD_ADD:
                  if (chreQcGeofenceAdd(gf_cmd->gfCount,
                                        gf_cmd->gfOptionsArray,
                                        gf_cmd->gfDataArray,
                                        &ids) != QC_GEOFENCE_ERROR_SUCCESS) {
                     FARF(ERROR, "chreQcGeofenceAdd failed!");
                     sendGfErrorMessageToHost();
                  }
                  break;

               case GEOFENCE_CMD_REMOVE:
                  if (chreQcGeofenceRemove(gf_cmd->gfCount,
                                           gf_cmd->gfIdArray) != QC_GEOFENCE_ERROR_SUCCESS) {
                     FARF(ERROR, "chreQcGeofenceRemove failed!");
                     sendGfErrorMessageToHost();
                  }
                  break;

               case GEOFENCE_CMD_MODIFY:
                  if (chreQcGeofenceModify(gf_cmd->gfCount,
                                           gf_cmd->gfIdArray,
                                           gf_cmd->gfOptionsArray) != QC_GEOFENCE_ERROR_SUCCESS) {
                     FARF(ERROR, "chreQcGeofenceModify failed!");
                     sendGfErrorMessageToHost();
                  }
                  break;

               case GEOFENCE_CMD_PAUSE:
                  if (chreQcGeofencePause(gf_cmd->gfCount,
                                          gf_cmd->gfIdArray) != QC_GEOFENCE_ERROR_SUCCESS) {
                     FARF(ERROR, "chreQcGeofencePause failed!");
                     sendGfErrorMessageToHost();
                  }
                  break;

               case GEOFENCE_CMD_RESUME:
                  if (chreQcGeofenceResume(gf_cmd->gfCount,
                                           gf_cmd->gfIdArray) != QC_GEOFENCE_ERROR_SUCCESS) {
                     FARF(ERROR, "chreQcGeofenceResume failed!");
                     sendGfErrorMessageToHost();
                  }
                  break;

               case GEOFENCE_CMD_DEINIT:
                  if (chreQcGeofenceDeinit() != QC_GEOFENCE_ERROR_SUCCESS) {
                     FARF(ERROR, "chreQcGeofenceDeinit failed!");
                  }
                  break;

               default:
                  FARF(ERROR, "Unknown Geofence command");
                  break;
            }
            return;
         } else {
            FARF(ERROR, "expected message type 0x%X OR 0x%X got 0x%X",
                 EVENT_SENSOR_CFG,
                 EVENT_GF_CFG,
                 msg->messageType);
            return;
         }
         break;
      }

      case CHRE_EVENT_SENSOR_SAMPLING_CHANGE:
      {
         struct chreSensorSamplingStatusEvent* ss = (struct chreSensorSamplingStatusEvent*)evtData;
         struct chreSensorInfo info;

         chreGetSensorInfo(ss->sensorHandle, &info);

         FARF(HIGH, "sampling change type 0x%X interval 0x%llu latency 0x%llu",
              info.sensorType, ss->status.interval, ss->status.latency);
         break;
      }

      case CHRE_EVENT_WIFI_ASYNC_RESULT:
      {
         #if FARF_HIGH
         struct chreAsyncResult* result = (struct chreAsyncResult*)evtData;

         FARF(HIGH, "wifi async result: requestType %d success %d errorCode %d",
              result->requestType, result->success, result->errorCode);
         #endif
         break;
      }

      case CHRE_EVENT_QC_BLE_ASYNC_RESULT:
      {
         #if FARF_HIGH
         struct chreAsyncResult* result = (struct chreAsyncResult*)evtData;

         FARF(HIGH, "qc ble async result: requestType %d success %d errorCode %d",
              result->requestType, result->success, result->errorCode);
         #endif
         break;
      }

      case CHRE_EVENT_SENSOR_ACCELEROMETER_DATA:
      case CHRE_EVENT_SENSOR_UNCALIBRATED_ACCELEROMETER_DATA:
      case CHRE_EVENT_SENSOR_ACCELEROMETER_TEMPERATURE_DATA:
      case CHRE_EVENT_SENSOR_GYROSCOPE_DATA:
      case CHRE_EVENT_SENSOR_UNCALIBRATED_GYROSCOPE_DATA:
      case CHRE_EVENT_SENSOR_GYROSCOPE_TEMPERATURE_DATA:
      case CHRE_EVENT_SENSOR_GEOMAGNETIC_FIELD_DATA:
      case CHRE_EVENT_SENSOR_UNCALIBRATED_GEOMAGNETIC_FIELD_DATA:
      case CHRE_EVENT_SENSOR_GEOMAGNETIC_FIELD_TEMPERATURE_DATA:
      case CHRE_EVENT_SENSOR_PRESSURE_DATA:
      case CHRE_EVENT_SENSOR_LIGHT_DATA:
      case CHRE_EVENT_SENSOR_PROXIMITY_DATA:
      case CHRE_EVENT_SENSOR_INSTANT_MOTION_DETECT_DATA:
      case CHRE_EVENT_SENSOR_STATIONARY_DETECT_DATA:
      case CHRE_EVENT_WIFI_SCAN_RESULT:
      case CHRE_EVENT_QC_BLE_SCAN_RESULT:
      case CHRE_EVENT_GNSS_LOCATION:
      case CHRE_EVENT_GNSS_DATA:
      case CHRE_EVENT_WWAN_CELL_INFO_RESULT:
      {
         void* pv;
         int len;

         switch (evtType) {
            case CHRE_EVENT_SENSOR_ACCELEROMETER_DATA:
            case CHRE_EVENT_SENSOR_UNCALIBRATED_ACCELEROMETER_DATA:
            case CHRE_EVENT_SENSOR_GYROSCOPE_DATA:
            case CHRE_EVENT_SENSOR_UNCALIBRATED_GYROSCOPE_DATA:
            case CHRE_EVENT_SENSOR_GEOMAGNETIC_FIELD_DATA:
            case CHRE_EVENT_SENSOR_UNCALIBRATED_GEOMAGNETIC_FIELD_DATA:
               len = sizeof(struct chreSensorDataHeader) +
                  (sizeof(((struct chreSensorThreeAxisData*)0)->readings) *
                   ((struct chreSensorDataHeader*)evtData)->readingCount);
               break;
            case CHRE_EVENT_SENSOR_PRESSURE_DATA:
            case CHRE_EVENT_SENSOR_LIGHT_DATA:
            case CHRE_EVENT_SENSOR_ACCELEROMETER_TEMPERATURE_DATA:
            case CHRE_EVENT_SENSOR_GYROSCOPE_TEMPERATURE_DATA:
            case CHRE_EVENT_SENSOR_GEOMAGNETIC_FIELD_TEMPERATURE_DATA:
               len = sizeof(struct chreSensorDataHeader) +
                  (sizeof(((struct chreSensorFloatData*)0)->readings) *
                   ((struct chreSensorDataHeader*)evtData)->readingCount);
               break;
            case CHRE_EVENT_SENSOR_PROXIMITY_DATA:
               len = sizeof(struct chreSensorDataHeader) +
                  (sizeof(((struct chreSensorByteData*)0)->readings) *
                   ((struct chreSensorDataHeader*)evtData)->readingCount);
               break;

            case CHRE_EVENT_SENSOR_INSTANT_MOTION_DETECT_DATA:
            case CHRE_EVENT_SENSOR_STATIONARY_DETECT_DATA:
               len = sizeof(struct chreSensorOccurrenceData);
               break;

            case CHRE_EVENT_WIFI_SCAN_RESULT:
               len = sizeof(struct chreWifiScanEvent) +
                  (sizeof(struct chreWifiScanResult) *
                   ((struct chreWifiScanEvent*)evtData)->resultCount);
               break;
            case CHRE_EVENT_QC_BLE_SCAN_RESULT:
               len = sizeof(struct chreQcBleScanEvent) +
                  (sizeof(unsigned char) *
                   ((struct chreQcBleScanEvent*)evtData)->size);
               break;
            case CHRE_EVENT_GNSS_LOCATION:
               len = sizeof(struct chreGnssLocationEvent);
               break;
            case CHRE_EVENT_GNSS_DATA:
               len = sizeof(struct chreGnssDataEvent) +
                  (sizeof(struct chreGnssMeasurement) *
                   ((struct chreGnssDataEvent*)evtData)->measurement_count);
               break;
            case CHRE_EVENT_WWAN_CELL_INFO_RESULT:
               len = sizeof(struct chreWwanCellInfoResult) +
                  (sizeof(struct chreWwanCellInfo) *
                   ((struct chreWwanCellInfoResult*)evtData)->cellInfoCount);
               break;
            default:
               FARF(HIGH, "missing event handler for %d", evtType);
               return;
         }

         pv = chreHeapAlloc(len);
         if (0 == pv) {
            FARF(ERROR, "failed to alloc event to host");
            return;
         }

         switch (evtType) {
            case CHRE_EVENT_SENSOR_ACCELEROMETER_DATA:
            case CHRE_EVENT_SENSOR_UNCALIBRATED_ACCELEROMETER_DATA:
            case CHRE_EVENT_SENSOR_ACCELEROMETER_TEMPERATURE_DATA:
            case CHRE_EVENT_SENSOR_GYROSCOPE_DATA:
            case CHRE_EVENT_SENSOR_UNCALIBRATED_GYROSCOPE_DATA:
            case CHRE_EVENT_SENSOR_GYROSCOPE_TEMPERATURE_DATA:
            case CHRE_EVENT_SENSOR_GEOMAGNETIC_FIELD_DATA:
            case CHRE_EVENT_SENSOR_UNCALIBRATED_GEOMAGNETIC_FIELD_DATA:
            case CHRE_EVENT_SENSOR_GEOMAGNETIC_FIELD_TEMPERATURE_DATA:
            case CHRE_EVENT_SENSOR_PRESSURE_DATA:
            case CHRE_EVENT_SENSOR_LIGHT_DATA:
            case CHRE_EVENT_SENSOR_PROXIMITY_DATA:
            case CHRE_EVENT_SENSOR_INSTANT_MOTION_DETECT_DATA:
            case CHRE_EVENT_SENSOR_STATIONARY_DETECT_DATA:
            case CHRE_EVENT_GNSS_LOCATION:
               memcpy(pv, evtData, len);
               break;
            case CHRE_EVENT_WIFI_SCAN_RESULT:
               memcpy(pv, evtData, sizeof(struct chreWifiScanEvent));
               memcpy((struct chreWifiScanEvent*)pv + 1,
                      ((struct chreWifiScanEvent*)evtData)->results,
                      sizeof(struct chreWifiScanResult) *
                      ((struct chreWifiScanEvent*)evtData)->resultCount);
               break;
            case CHRE_EVENT_QC_BLE_SCAN_RESULT:
               memcpy(pv, evtData, sizeof(struct chreQcBleScanEvent));
               memcpy((struct chreQcBleScanEvent*)pv + 1,
                      ((struct chreQcBleScanEvent*)evtData)->result,
                      ((struct chreQcBleScanEvent*)evtData)->size);
               break;
            case CHRE_EVENT_GNSS_DATA:
               memcpy(pv, evtData, sizeof(struct chreGnssDataEvent));
               memcpy((struct chreGnssDataEvent*)pv + 1,
                      ((struct chreGnssDataEvent*)evtData)->measurements,
                      sizeof(struct chreGnssMeasurement) *
                      ((struct chreGnssDataEvent*)evtData)->measurement_count);
               break;
            case CHRE_EVENT_WWAN_CELL_INFO_RESULT:
               FARF(HIGH, "wwan event w/ %d cells", ((struct chreWwanCellInfoResult*)evtData)->cellInfoCount);
               memcpy(pv, evtData, sizeof(struct chreWwanCellInfoResult));
               memcpy((struct chreWwanCellInfoResult*)pv + 1,
                      ((struct chreWwanCellInfoResult*)evtData)->cells,
                      sizeof(struct chreWwanCellInfo) *
                      ((struct chreWwanCellInfoResult*)evtData)->cellInfoCount);
               break;
            default:
               FARF(HIGH, "missing event handler for 0x%X", evtType);
               return;
         }
         if ((true != chreSendMessageToHost(pv, len, evtType, sensor_cfg_heapFree))) {
            FARF(ERROR, "failed to send sample");
         }
         break;
      }

      case CHRE_EVENT_GNSS_GF_RESPONSE:
      {
         void* pv;
         uint32_t len;
         size_t count;

         FARF(HIGH, "CHRE_EVENT_GNSS_GF_RESPONSE event received!!!!");
         qcGeofenceResponseEventType* rE;
         rE = (qcGeofenceResponseEventType*)evtData;

         count = rE->count;
         len = sizeof(size_t) + (sizeof(uint8_t) * count) +
            (sizeof(uint32_t) * count);
         pv = chreHeapAlloc(len);
         if (0 == pv) {
            FARF(ERROR, "failed to alloc event to host");
            return;
         }

         memcpy((char*)pv, &(rE->count), sizeof(size_t));
         memcpy((char*)pv + sizeof(size_t),
                rE->err,
                sizeof(uint8_t) * count);
         memcpy((char*)pv + sizeof(size_t) + sizeof(uint8_t) * count,
                (char*)(rE->ids),
                sizeof(uint32_t) * count);

         if ((true != chreSendMessageToHost(pv, len, CHRE_EVENT_GNSS_GF_RESPONSE, sensor_cfg_heapFree))) {
            FARF(ERROR, "failed to send sample");
         }
         break;
      }

      case CHRE_EVENT_GNSS_GF_BREACH:
      {
         void* pv;
         size_t count;
         uint32_t len;

         qcGeofenceBreachEventType* bE;
         bE = (qcGeofenceBreachEventType*)evtData;
         count = bE->count;

         len = sizeof(size_t) +
            (sizeof(uint32_t) * count) +
            sizeof(qcGeofenceLocationType) +
            sizeof(uint8_t) + sizeof(uint64_t);

         pv = chreHeapAlloc(len);
         if (0 == pv) {
            FARF(ERROR, "failed to alloc event to host");
            return;
         }

         memcpy((char*)pv, &(bE->count), sizeof(size_t));
         memcpy((char*)pv + sizeof(size_t), bE->ids, sizeof(uint32_t) * count);
         memcpy((char*)pv + sizeof(size_t) + sizeof(uint32_t) * count,
                &(bE->location), sizeof(qcGeofenceLocationType));
         memcpy((char*)pv + sizeof(size_t) + sizeof(uint32_t) * count +
                sizeof(qcGeofenceLocationType),
                &(bE->breach_type), sizeof(uint8_t));
         memcpy((char*)pv + sizeof(size_t) + sizeof(uint32_t) * count +
                sizeof(qcGeofenceLocationType) + sizeof(uint8_t),
                &(bE->timestamp), sizeof(uint64_t));

         if ((true != chreSendMessageToHost(pv, len, CHRE_EVENT_GNSS_GF_BREACH, sensor_cfg_heapFree))) {
            FARF(ERROR, "failed to send sample");
         }
         break;
      }

      default:
         FARF(HIGH, "unknown event 0x%X from 0x%X", evtType, senderInstanceId);
         break;
   }
}

void nanoappEnd(void)
{
   FARF(HIGH, "sensor_cfg end");
}

bool nanoappStart(void)
{
   FARF(HIGH, "sensor_cfg start");
   return true;
}
