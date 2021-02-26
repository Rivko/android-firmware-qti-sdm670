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
#include "chre_app_coverage.h"

#include "HAP_farf.h"

struct coverageTask {
  uint32_t sensHandle; // Current design can only enable one sensor test at a time
  uint32_t sensType;
  uint32_t configMode;
  uint32_t lastSensEvtType;
};
static struct coverageTask mTask;
//static void *scanData = NULL;

void chreMessageFreeCbk(void* message, size_t messageSize)
{
  if (message) {
    chreHeapFree(message);
  }
  FARF(LOW, "chreMessageFreeCb() finished ~~~");
}

void chreEventCompleteCbk(uint16_t eventType, void* eventData)
{
  if (eventData) {
    chreHeapFree(eventData);
  }
  FARF(LOW, "chreEventCompleteCb() callback Event Type= 0x%X ~~~", eventType);
}

void compose_send_msg(uint32_t code)
{
  int sent = 0;
  uint32_t data_len;
  struct sensorConfig* status = NULL;

  switch(code) {
/*
    case SENSOR_WIFI_SCAN_RESULTS:
        if (scanData) {
            data_len = sizeof(struct sensorConfig) + ((struct WifiScanEvent*)scanData)->size;
            status = chreHeapAlloc(data_len);
            memcpy(status+1, scanData, ((struct WifiScanEvent*)scanData)->size);
            chreHeapFree(scanData);
        } else {
            data_len = sizeof(struct sensorConfig) + sizeof(uint32_t);
            status = chreHeapAlloc(data_len);
        }
        break;
*/
    case SENSOR_SAMPLING_STATUS:
      data_len = sizeof(struct sensorConfig);
      status = (struct sensorConfig*)chreHeapAlloc(sizeof(struct sensorConfig));
      chreGetSensorSamplingStatus(mTask.sensHandle, &status->SSStatus);
      break;

    default:
      data_len = sizeof(struct sensorConfig);
      status = (struct sensorConfig*)chreHeapAlloc(sizeof(struct sensorConfig));
      break;
  }
  if (status) {
    status->code = code;
    status->mode = mTask.configMode;
    status->sensType = mTask.sensType;
    status->evtType = mTask.lastSensEvtType;
    sent = chreSendMessageToHost(status, data_len, CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_SENSOR_CONFIG, chreMessageFreeCbk);
    if (!sent && status) {
      FARF(ERROR, "return code 0x%X: message failed to sent to client", code);
      chreMessageFreeCbk(status, data_len);
    }
  } else {
    FARF(ERROR, "Faile to allocate memory to sent to client for return code 0x%X", code);
  }
}

void nanoappHandleEvent(uint32_t senderInstanceId, uint16_t evtType, const void* evtData)
{
  static int waitForTimer = 0;
  static int result = 1;
  int success = 0;

  FARF(HIGH, "app_coverage got event 0x%X", (unsigned int)evtType);

  switch(evtType) {

    case CHRE_EVENT_MESSAGE_FROM_HOST:
    {
      struct chreMessageFromHostData* msg = (struct chreMessageFromHostData*)evtData;

      switch(msg->messageType) {
        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_ECHO:
        {
          FARF(HIGH, "Got echo request len %d", msg->messageSize);

          void* message = 0;

          if (msg->messageSize) {
            FARF(HIGH, "alloc response");

            message = chreHeapAlloc(msg->messageSize);
            if (message) {
              memcpy(message, msg->message, msg->messageSize);
            } else {
              FARF(HIGH, "alloc failure, send zero length rsp");
            }
          }
          FARF(HIGH, "Send echo response");
          if (chreSendMessageToHost(message, msg->messageSize, msg->messageType, chreMessageFreeCbk)) {
            FARF(HIGH, "message echoed");
          } else {
            FARF(HIGH, "failed to echo");
            if (message) {
              chreHeapFree(message);
            }
          }
        }
          break;

        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_TIMER:
        {
          FARF(HIGH, "Got timer request");
          waitForTimer = 1;
          chreTimerSet(5000000000LL, (void*)0xDEADBEEF, TRUE);
          break;
        }

        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_ABORT:
        {
          FARF(HIGH, "Got abort request");
          chreSendMessageToHost(0, 0, msg->messageType, 0);
          chreAbort(0);
          break;
        }

        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_EVENT_TO_SELF:
        {
          FARF(HIGH, "Got event to self request");
          chreSendEvent(CHRE_EVENT_FIRST_USER_VALUE, 0, 0, chreGetInstanceId());
          break;
        }

        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_SENSOR_CONFIG:
        {
          struct sensorConfig* config = (struct sensorConfig*)(msg->message);
          FARF(HIGH, "Got Sensor Type %d Config request, mode= %d", config->sensType, config->mode);
          mTask.configMode = config->mode;
          mTask.sensType = config->sensType;
          if (chreSensorFindDefault(config->sensType, &mTask.sensHandle)) {
            if (!chreSensorConfigure(mTask.sensHandle, config->mode, config->SSStatus.interval, config->SSStatus.latency)) {
              FARF(ERROR, "Failed to Configure Sensor Type 0x%X  interval %llu  latency %llu !!!", config->sensType, config->SSStatus.interval, config->SSStatus.latency);
              compose_send_msg(SENSOR_FAIL_CONFIGURE);
            } else {
              FARF(HIGH, "Sensor Type 0x%X mode %u Configured, waiting event!!!", config->sensType, config->mode);
              compose_send_msg(SENSOR_FOUND_CONFIGURE_WAITING);
            }
          } else {
            FARF(ERROR, "Sensor Type %d NOT found!!!", config->sensType);
            compose_send_msg(SENSOR_NOT_FOUND);
          }
          break;
        }

        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_GET_SENSOR_SAMPLING_STATUS:
        {
          FARF(HIGH, "Got COVERAGE_GET_SENSOR_SAMPLING_STATUS, current Sensor Type 0x%X, mode %u", mTask.sensType, mTask.configMode);
          compose_send_msg(SENSOR_SAMPLING_STATUS);
          break;
        }
/*
        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_GET_WIFI_SCAN_RESULTS:
        {
          FARF(HIGH, "Got COVERAGE_GET_WIFI_SCAN_RESULTS current Sensor Type 0x%X, mode %u", mTask.sensType, mTask.configMode);
          compose_send_msg(SENSOR_WIFI_SCAN_RESULTS);
          break;
        }
*/
        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_CONFIG_MODE_DONE:
        {
          FARF(HIGH, "Got COVERAGE_CONFIG_MODE_DONE, current Sensor Type 0x%X", mTask.sensType);
          mTask.configMode = CHRE_SENSOR_CONFIGURE_MODE_DONE;
          mTask.lastSensEvtType = 0;
          if (!chreSensorConfigureModeOnly(mTask.sensHandle, CHRE_SENSOR_CONFIGURE_MODE_DONE)) {
            FARF(ERROR, "CHRE_SENSOR_CONFIGURE_MODE_DONE Failed !!!");
          }
          compose_send_msg(SENSOR_SAMPLING_STATUS);
          break;
        }
          
        default:
          FARF(HIGH, "Unknown private message 0x%X", (unsigned int)evtType);

      } // switch(msg->reservedMessageType)
    }   // case EVT_APP_FROM_HOST
      break;

    case CHRE_EVENT_FIRST_USER_VALUE:
    {
      FARF(HIGH, "Got event to self");
      chreSendMessageToHost(0, 0, CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_EVENT_TO_SELF, 0);
      break;
    }


    case CHRE_EVENT_TIMER:
      if (!waitForTimer) {
        FARF(ERROR, "timer expire not expected");
        result = 1;
        break;
      }
      waitForTimer = 0;

      result = 0;
      if (!((uint32_t)evtData == 0xDEADBEEF)) {
        FARF(HIGH, "timer failed cookie");
        result = 1;
      }
      FARF(HIGH, "timer success");
      success = chreSendMessageToHost(&result, sizeof(result), CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + COVERAGE_TIMER, 0);
      break;

      // This checks chre_sensor.c functionalities
    case CHRE_EVENT_SENSOR_ACCELEROMETER_DATA:
    {
      mTask.lastSensEvtType = evtType;
      struct chreSensorThreeAxisData* accel = (struct chreSensorThreeAxisData*)evtData;
      FARF(ALWAYS, "Accel sample count [%d]", accel->header.readingCount);
      FARF(ALWAYS, "Accel samples : %f,  %f,  %f", accel->readings[0].x, accel->readings[0].y, accel->readings[0].z);
      FARF(ALWAYS, "EVENT Accel Data success");
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

    case CHRE_EVENT_SENSOR_GYROSCOPE_DATA:
    {
      mTask.lastSensEvtType = evtType;
#if FARF_HIGH == 1
      struct chreSensorThreeAxisData* accel = (struct chreSensorThreeAxisData*)evtData;
      FARF(HIGH, "Gyro sample count [%d]", accel->header.readingCount);
      FARF(HIGH, "Gyro samples : %f,  %f,  %f", accel->readings[0].x, accel->readings[0].y, accel->readings[0].z);
      FARF(HIGH, "EVENT Gyro Data success");
#endif
      break;
    }
      /*
          case 0x121:  // TODO: WiFi data event is not defined yet in new CHRE API
          {
              mTask.lastSensEvtType = evtType;
              FARF(HIGH, "EVENT 0x%X Wifi", evtType);
              struct WifiScanEvent *wifiEvt = (struct WifiScanEvent*)evtData;
              if (scanData) {
                  chreHeapFree(scanData);
              }
              scanData = chreHeapAlloc(wifiEvt->size);
              memcpy(scanData, evtData, wifiEvt->size);
            break;
          }
      */

    default:
      FARF(HIGH, "Unknown Event 0x%X", (unsigned int)evtType);
  }
}

bool nanoappStart(void)
{
  mTask.sensHandle = 0;
  FARF(HIGH, "  app_coverage Start");
  return true;
}

void nanoappEnd(void)
{
  FARF(HIGH, "  app_coverage End");
}


