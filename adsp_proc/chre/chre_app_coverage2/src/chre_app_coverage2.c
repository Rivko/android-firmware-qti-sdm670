/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <chre.h>
#include <chre/event.h>
#include "chre_app_coverage.h"

#include "HAP_farf.h"

struct coverageTask {
  uint32_t sensHandle; // Current design can only enable one sensor test at a time
  uint32_t sensType;
  uint32_t configMode;
  uint32_t lastSensEvtType;
};
static struct coverageTask mTask;
uint64_t preConfigTime;
uint64_t postConfigTime;
uint64_t lastts;

void coverageMsgFreeCbk(void* message, size_t messageSize)
{
  if (message) {
    chreHeapFree(message);
  }
}

void compose_send_msg2(uint32_t code)
{
  int sent = 0;
  struct sensorConfig* status = chreHeapAlloc(sizeof(struct sensorConfig));

  if (status) {
    status->code = code;
    status->mode = mTask.configMode;
    status->sensType = mTask.sensType;
    status->evtType = mTask.lastSensEvtType;
    if (SENSOR_SAMPLING_STATUS == code) {
      chreGetSensorSamplingStatus(mTask.sensHandle, &status->SSStatus);
    }
  } else {
    FARF(ERROR, "Failed to alloc memory for return status !");
    return;
  }

  sent = chreSendMessageToHost(status, sizeof(struct sensorConfig),
                               CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_SENSOR_CONFIG, coverageMsgFreeCbk);
  if (!sent && status) {
    FARF(ERROR, "return code 0x%X: message failed to sent to client", code);
    coverageMsgFreeCbk(status, sizeof(struct sensorConfig));
  }
}

const char* GetEventName(uint16_t type)
{
  switch(type) {
    case CHRE_EVENT_SENSOR_ACCELEROMETER_DATA:
      return "Accel";
    case CHRE_EVENT_SENSOR_GYROSCOPE_DATA:
      return "Gyro";
    default:
      return "Unknown";
  }
}

void nanoappHandleEvent(uint32_t senderInstanceId, uint16_t evtType, const void* evtData)
{

  FARF(HIGH, "app_coverage2 got event 0x%X at %llu ms", (unsigned int)evtType, chreGetTime());
  switch(evtType) {

    case CHRE_EVENT_MESSAGE_FROM_HOST:
    {
      struct chreMessageFromHostData* msg = (struct chreMessageFromHostData*)evtData;

      switch(msg->reservedMessageType) {
        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_ECHO:
        {
          FARF(HIGH, "Got echo request len %d", msg->messageSize);
          void* message = 0;

          if (msg->messageSize) {
            message = chreHeapAlloc(msg->messageSize);
            if (message) {
              memcpy(message, msg->message, msg->messageSize);
            } else {
              FARF(HIGH, "alloc failure, send zero length rsp");
            }
          }
          if (chreSendMessageToHost(message, msg->messageSize, msg->reservedMessageType, coverageMsgFreeCbk)) {
            FARF(HIGH, "message echoed");
          } else {
            FARF(HIGH, "failed to echo");
            if (message) {
              chreHeapFree(message);
            }
          }
        }
          break;


        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_SENSOR_CONFIG:
        {
          preConfigTime = chreGetTime();
          struct sensorConfig* config = (struct sensorConfig*)(msg->message);
          FARF(HIGH, "Got Sensor Type %d Config request, mode= %d, time= %llu", config->sensType, config->mode, preConfigTime);
          mTask.configMode = config->mode; mTask.sensType = config->sensType;
          if (chreSensorFindDefault(config->sensType, &mTask.sensHandle)) {
            if (!chreSensorConfigure(mTask.sensHandle, config->mode, config->SSStatus.interval, config->SSStatus.latency)) {
              FARF(ERROR, "Failed to Configure Sensor Type 0x%X !!!", config->sensType);
              compose_send_msg2(SENSOR_FAIL_CONFIGURE);
            } else {
              postConfigTime = chreGetTime();
              lastts = preConfigTime;
              FARF(HIGH, "Sensor Type 0x%X mode %u Configured, waiting event!!! %llu (%lld)", config->sensType, config->mode, postConfigTime, (postConfigTime - preConfigTime));
              compose_send_msg2(SENSOR_FOUND_CONFIGURE_WAITING);
            }
          } else {
            FARF(ERROR, "Sensor Type %d NOT found!!!", config->sensType);
            compose_send_msg2(SENSOR_NOT_FOUND);
          }
          break;
        }

        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_CONFIG_MODE_DONE:
        {
          FARF(HIGH, "Got COVERAGE_CONFIG_MODE_DONE, current Sensor Type 0x%X", mTask.sensType);
          mTask.configMode = CHRE_SENSOR_CONFIGURE_MODE_DONE;
          mTask.lastSensEvtType = 0;
          if (!chreSensorConfigureModeOnly(mTask.sensHandle, CHRE_SENSOR_CONFIGURE_MODE_DONE)) {
            FARF(ERROR, "CHRE_SENSOR_CONFIGURE_MODE_DONE Failed !!!");
          }
          compose_send_msg2(SENSOR_SAMPLING_STATUS);
          break;
        }

        default:
          FARF(HIGH, "Unknown private message 0x%X", (unsigned int)evtType);

      }
    }
      break;

      // This checks chre_sensor.c functionalities
    case CHRE_EVENT_SENSOR_ACCELEROMETER_DATA:
    case CHRE_EVENT_SENSOR_GYROSCOPE_DATA:
    {

      int i = 0;
      mTask.lastSensEvtType = evtType;
      struct chreSensorThreeAxisData* accel = (struct chreSensorThreeAxisData*)evtData;
      uint64_t ts = (accel->header.baseTimestamp + accel->readings[0].timestampDelta);
      FARF(HIGH, "%s 0 ts %llu (%lld)", GetEventName(evtType), ts, ts - lastts);
      if (ts < lastts) {
        FARF(ERROR, "%s base timestamp is %f msec earlier than powering on", GetEventName(evtType), (float)(lastts - ts));
      }
      for (i = 1; i < accel->header.readingCount; i++) {
        FARF(HIGH, "%s %d ts %llu (%lld)", GetEventName(evtType),  i, ts + accel->readings[i].timestampDelta, accel->readings[i].timestampDelta);
        ts = ts + accel->readings[i].timestampDelta;
      }
      lastts = ts;
      //FARF(HIGH, "Accel sample count [%d]", accel->header.readingCount);
      //FARF(HIGH, "Accel samples : %f,  %f,  %f", accel->readings[0].x, accel->readings[0].y, accel->readings[0].z);
      //FARF(HIGH, "EVENT Accel Data success");
      break;
    }

    case CHRE_EVENT_SENSOR_INSTANT_MOTION_DETECT_DATA:
    {
      mTask.lastSensEvtType = evtType;
      struct chreSensorOccurrenceData* motion = (struct chreSensorOccurrenceData*)evtData;
      FARF(ALWAYS, "Motion timestampDelta [%d]", motion->readings[0].timestampDelta);
      FARF(ALWAYS, "EVENT 0x%X Motion Data success", evtType);
      break;
    }

    case CHRE_EVENT_SENSOR_STATIONARY_DETECT_DATA:
    {
      mTask.lastSensEvtType = evtType;
      struct chreSensorOccurrenceData* stationary = (struct chreSensorOccurrenceData*)evtData;
      FARF(ALWAYS, "Stationary timestampDelta [%d]", stationary->readings[0].timestampDelta);
      FARF(ALWAYS, "EVENT 0x%X Stationary Data success", evtType);
      break;
    }

    case 0x121:  // TODO: WiFi data event is not defined yet in new CHRE API
    {
      mTask.lastSensEvtType = evtType;
      FARF(HIGH, "EVENT 0x%X Wifi", evtType);
      // TODO: Print out WiFi scan
      break;
    }

    default:
      FARF(HIGH, "Unknown Event 0x%X", (unsigned int)evtType);
  }
}

bool nanoappStart(void)
{
  mTask.sensHandle = 0;
  FARF(HIGH, "  app_coverage2 Start");
  return true;
}

void nanoappEnd(void)
{
  FARF(HIGH, "  app_coverage2 End");
}


