/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <chre.h>
#include <chre/event.h>
#include <chre/sensor.h>
#include <chre/wifi.h>
#include "chre_app_oem.h"
#include "HAP_farf.h"

uint32_t sensHandle;
struct sensorConfig config[] = {
  { .sensType =  0,
    .mode = CHRE_SENSOR_CONFIGURE_MODE_DONE,
    .interval = CHRE_SENSOR_INTERVAL_DEFAULT,
    .latency = CHRE_SENSOR_INTERVAL_DEFAULT   
  },
  { .sensType = CHRE_SENSOR_TYPE_ACCELEROMETER,
    .mode = CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS,  // 0x3
    .interval = CHRE_SENSOR_INTERVAL_DEFAULT,
    .latency = CHRE_SENSOR_LATENCY_DEFAULT
  },
  { .sensType =  CHRE_SENSOR_TYPE_ACCELEROMETER,
    .mode = CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS,
    .interval = (40000000ull),     // 25 Hz,
    .latency = (640000000ull)   // 0.64 S
  },
};

static void appOemMessageFreeCbk(void *message, size_t messageSize)
{
    chreHeapFree(message);
    FARF(LOW, "appOemMessageFreeCbk() callback finished ~~~");
}

void nanoappHandleEvent(uint32_t senderInstanceId, uint16_t evtType, const void* evtData)
{
    static int waitForTimer = 0;
    static int result = 1;
    int success = 0;  

    FARF(HIGH, "App OEM got event 0x%X", (unsigned int)evtType);

    switch(evtType) {

    case CHRE_EVENT_MESSAGE_FROM_HOST:
    {
        int success = 0;   
        struct chreMessageFromHostData* msgIn = (struct chreMessageFromHostData*)evtData;

        switch (msgIn->reservedMessageType) {
          case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + APP_OEM_ECHO:
          {
              FARF(HIGH, "Got echo request");
              void* msg = 0;

              if (0 == msgIn->messageSize) {
                  success = chreSendMessageToHost(0, 0, msgIn->reservedMessageType, 0);
              } else {
                  msg = chreHeapAlloc(msgIn->messageSize);
                  if (msg) {
                      memcpy(msg, msgIn->message, msgIn->messageSize);
                      success = chreSendMessageToHost(msg, msgIn->messageSize, msgIn->reservedMessageType, appOemMessageFreeCbk);
                  } else {
                      FARF(HIGH, "failed to echo, alloc failure");
                  }
              }
              if (success) {
                  FARF(HIGH, "message echoed");
              } else {
                  FARF(HIGH, "failed to echo");
                  if (msg) {
                      chreHeapFree(msg);
                  }
              }
          }
            break;

          case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + APP_OEM_ABORT:
          {
              FARF(HIGH, "Got abort request");
              chreSendMessageToHost(0, 0, msgIn->reservedMessageType, 0);
              chreAbort(0);
              break;
          }

        case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + APP_OEM_SENSOR_CONFIG:
        {
          int n = *((int*)(msgIn->message));
          FARF(HIGH, "Got Sensor Type %d Config request, mode= %d", config[n].sensType, config[n].mode);
          if (n) {
              if (chreSensorFindDefault(config[n].sensType, &sensHandle)) {
                  if (!chreSensorConfigure(sensHandle, config[n].mode, config[n].interval, config[n].latency)) {
                      FARF(ERROR, "Failed to Configure Sensor Type 0x%X  interval %llu  latency %llu !!!", config[n].sensType, config[n].interval, config[n].latency);
                  }else{
                      FARF(HIGH, "Sensor Type 0x%X mode %u Configured, waiting event!!!", config[n].sensType, config[n].mode);
                  }
              } else {
                  FARF(ERROR, "Sensor Type %d NOT found!!!", config[n].sensType);
              }
          } else {
             if (!chreSensorConfigureModeOnly(sensHandle, CHRE_SENSOR_CONFIGURE_MODE_DONE)) {
                FARF(ERROR, "CHRE_SENSOR_CONFIGURE_MODE_DONE Failed !!!");
            }
          }
          chreSendMessageToHost(0, 0, msgIn->reservedMessageType, 0);
          break;
        }

        break;  // switch (msgIn->reservedMessageType)
        }
    }
    break;

    case APP_OEM_TIMER:
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
      success = chreSendMessageToHost(&result, sizeof(result), CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + APP_OEM_TIMER, 0);
      break;

    // This checks chre_sensor.c functionalities
    case CHRE_EVENT_SENSOR_ACCELEROMETER_DATA:
    case CHRE_EVENT_SENSOR_GYROSCOPE_DATA:
    {
        struct chreSensorThreeAxisData *accel = (struct chreSensorThreeAxisData *)evtData;
        FARF(ALWAYS, "Three Axis sample count [%d]", accel->header.readingCount);
        FARF(ALWAYS, "Three Axis samples : %f,  %f,  %f", accel->readings[0].x, accel->readings[0].y, accel->readings[0].z);
        FARF(ALWAYS, "EVENT Three Axis Data success");
      break;
    }

    case CHRE_EVENT_SENSOR_SAMPLING_CHANGE: {
      FARF(HIGH, "EVENT 0x%X CHRE_EVENT_SENSOR_SAMPLING_CHANGE", CHRE_EVENT_SENSOR_SAMPLING_CHANGE);
      const struct chreSensorSamplingStatusEvent *ev = (const struct chreSensorSamplingStatusEvent *)(evtData);

      FARF(ALWAYS, "Sampling Change: handle 0x%X, status: interval %llu, latency %llu, enabled %d",
           ev->sensorHandle, ev->status.interval, ev->status.latency, ev->status.enabled);
      break;
    }

    default:
      FARF(HIGH, "Unknown Event 0x%X", (unsigned int)evtType);
  }
}

bool nanoappStart(void)
{
    uint32_t tid = chreGetInstanceId();
    FARF(ALWAYS, "    %ld App OEM App Init\n", tid);
    return true;
}

void nanoappEnd(void)
{
    FARF(HIGH, "    App OEM  App End\n");
}

