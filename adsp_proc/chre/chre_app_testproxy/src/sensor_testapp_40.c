/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <inttypes.h>

#include <chre.h>
#include <chre/gnss.h>
#include <chre/wifi.h>
#include <chre/wwan.h>

#include "HAP_farf.h"
#include "qc_chre_proxy_v01.h"

#define APP_ID_VENDOR_QUALCOMM UINT64_C(0x2051434f4d) // " QCOM"

#define TEST_NAPP_VERSION 1
#define NANOAPP_1         0x40
#define TEST_NAPP_ID  APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, NANOAPP_1)

// TODO remove these, what does chre provide?
#define SENSOR_RATE_ONDEMAND    0xFFFFFF00UL
#define SENSOR_RATE_ONCHANGE    0xFFFFFF01UL
#define SENSOR_RATE_ONESHOT     0xFFFFFF02UL
#define SENSOR_HZ(_hz)          ((uint32_t)((_hz) * 1024.0f))
#define NANOSECONDS_IN_1_SEC    (1000000000ULL)
#define RATE_TO_NANO_SEC(_rate) ((uint64_t)((NANOSECONDS_IN_1_SEC*1024.0f)/(_rate)))
#define APP_ID_MAKE(vendor, app)   ((((uint64_t)(vendor)) << 24) | ((app) & APP_SEQ_ID_ANY))
#define APP_ID_VENDOR_GOOGLE       UINT64_C(0x476F6F676C) // "Googl"
#define APP_VENDOR_ANY             UINT64_C(0xFFFFFFFFFF)
#define APP_SEQ_ID_ANY             UINT64_C(0xFFFFFF)
#define APP_ID_ANY                 UINT64_C(0xFFFFFFFFFFFFFFFF)
#define EVT_NO_FIRST_USER_EVENT          0x00000100    //all events lower than this are reserved for the OS. all of them are nondiscardable necessarily!
#define EVT_NO_FIRST_SENSOR_EVENT        EVT_NO_FIRST_USER_EVENT    //sensor type SENSOR_TYPE_x produces events of type EVT_NO_FIRST_SENSOR_EVENT + SENSOR_TYPE_x for all Google-defined sensors
#define sensorGetMyEventType(_sensorType) (EVT_NO_FIRST_SENSOR_EVENT + (_sensorType))

#ifndef CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE
#define CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE 0x00400
#endif

static bool evt_to_proxy = true; // Default is false;

void myfree(void *message, size_t messageSize)
{
  if (0 == message) {
    FARF(ERROR, "null passed to free callback");
  }

  chreHeapFree(message);
}

static void handle_napp_cmds(qcp_napp_cmd_msg_s_v01 *ncmd, uint32_t msg_type)
{
  struct HostHubPacket *msg = 0;
  uint32_t evt_type = msg_type;
  uint32_t msgLen = 0;
  uint32_t cur_rate = 0;
  qcp_napp_evt_msg_s_v01 evt_data = { 0, 0, 0, 0, 0, 0, 0, 0};

  switch(msg_type)
  {
   case NAPP_CMD_RESET_TESTAPP_V01:
   {
     FARF(HIGH,"NCMD: RESET_TESTAPP");
     evt_data.rc = QCP_RC_SUCCESS_V01;
     break;
   }

   case NAPP_CMD_GET_TESTAPP_INFO_V01:
   {
     FARF(HIGH,"NCMD: GET_TESTAPP_INFO");
     evt_data.napp_version = TEST_NAPP_VERSION;
     evt_data.rc = QCP_RC_SUCCESS_V01;
     break;
   }
   
   case NAPP_CMD_EVENT_ENABLE_V01:
     FARF(HIGH,"NCMD: EVENT_ENABLE");
     evt_to_proxy = true;
     evt_data.rc = QCP_RC_SUCCESS_V01;
     FARF(HIGH,"NCMD: Event to proxy enabled");
     break;

   case NAPP_CMD_EVENT_DISABLE_V01:
     FARF(HIGH,"NCMD: EVENT_DISABLE");
     evt_to_proxy = false; 
     evt_data.rc = QCP_RC_SUCCESS_V01;
     FARF(HIGH,"NCMD: Event to proxy disabled");
     break;

   case NAPP_CMD_SENS_FIND_V01:
   {
     int slen = 0;
     uint32_t sensor_handle;
     struct chreSensorInfo chreSI={NULL, 0, 0};

     FARF(HIGH,"NCMD: SENS_FIND type %u handle %u msg_type 0x%X", ncmd->sensorType, ncmd->sensorHandle, msg_type);

     if (chreSensorFindDefault(ncmd->sensorType, &sensor_handle)){
        if (!chreGetSensorInfo(sensor_handle, &chreSI)){
            FARF(ERROR, "NCMD: Failed to find Sensor Info for Type %u ", ncmd->sensorType);
        }
     } else {
         FARF(ERROR, "NCMD: Failed to find Sensor Type %u ", ncmd->sensorType);
     }
     if (NULL != chreSI.sensorType) {
       evt_data.rc = QCP_RC_SUCCESS_V01;
       evt_data.sensorHandle = sensor_handle;

       slen = strlen(chreSI.sensorName);
       //slen = (slen > QCP_SENSOR_NAME_MAX_LEN_V01)? QCP_SENSOR_NAME_MAX_LEN_V01: slen;
       strlcpy(evt_data.si.sensorName, chreSI.sensorName, slen);
       evt_data.si.sensorName_len = slen;

       FARF(HIGH,"NCMD:slen %d, chreSensorFindDefault() found %s present %s", slen, evt_data.si.sensorName, chreSI.sensorName);
       evt_data.si.sensorType = chreSI.sensorType;

     } else {
       FARF(HIGH,"NCMD: chreSensorFindDefault() failed for senstype %u", ncmd->sensorType);
     }
     break;
   }
   
   case NAPP_CMD_SENS_REQUEST_V01:
   {
     if (chreSensorConfigure(ncmd->sensorHandle, ncmd->mode, RATE_TO_NANO_SEC(ncmd->rate), ncmd->latency)){
       evt_data.rc = QCP_RC_SUCCESS_V01;
     } else {
       evt_data.rc = QCP_RC_FAILED_V01;
     }
     FARF(HIGH,"NCMD: SENS_REQUEST mode %d rate %u latency %llx rc %d", ncmd->mode, ncmd->rate, ncmd->latency, evt_data.rc);
   }
   break;

   case NAPP_CMD_SENS_EVENT_V01:
      FARF(HIGH,"NCMD: SENS_EVENT");
      break;

   case NAPP_CMD_SENS_REQ_RATE_CHANGE_V01:
     if (chreSensorConfigure(ncmd->sensorHandle, ncmd->mode, RATE_TO_NANO_SEC(ncmd->rate), ncmd->latency)){
         evt_data.rc = QCP_RC_SUCCESS_V01;
     } else {
         evt_data.rc = QCP_RC_FAILED_V01;
         FARF(ERROR,"NCMD: RATE CHANGE failed !!!");
     }
     FARF(HIGH,"NCMD: SENS_REQ_RATE_CHANGE mode %d rc %d", ncmd->mode, evt_data.rc);
     break;

   case NAPP_CMD_SENS_GET_CUR_RATE_V01:
   {
     struct chreSensorSamplingStatus curStatus;
     if (chreGetSensorSamplingStatus(ncmd->sensorHandle, &curStatus)){
         cur_rate = SENSOR_HZ((float)NANOSECONDS_IN_1_SEC/curStatus.interval);
         FARF(HIGH,"NCMD: sensor current rate %u", cur_rate);
         evt_data.cur_rate = cur_rate;
         evt_data.enabled = curStatus.enabled;
         evt_data.interval = curStatus.interval;
         evt_data.latency = curStatus.latency;
         evt_data.rc = QCP_RC_SUCCESS_V01;
     } else {
         FARF(ERROR,"NCMD: chreGetSensorSamplingStatus failed to get sensor sampling status");
         evt_data.rc = QCP_RC_FAILED_V01;
     }
     FARF(HIGH,"NCMD: SENS_GET_CUR_RATE rc %d", evt_data.rc);
     break;
   }
   
   case NAPP_CMD_SENS_RELEASE_V01:
     if (chreSensorConfigureModeOnly(ncmd->sensorHandle, CHRE_SENSOR_CONFIGURE_MODE_DONE)){
         evt_data.rc = QCP_RC_SUCCESS_V01;
     } else {
         evt_data.rc = QCP_RC_FAILED_V01;
         FARF(ERROR,"NCMD: MODE DONE failed !!!");
     }
     FARF(HIGH,"NCMD: SENS_RELEASE rc %d", evt_data.rc);
     break;

   case NAPP_CMD_SENS_TRIG_ON_DEMAND_V01:
     evt_data.rc = QCP_RC_FAILED_V01;
     FARF(HIGH,"NCMD: SENS_TRIG_ON_DEMAND not available");
     break;

   default:
     FARF(HIGH, "Unknown napp msg type %u", msg_type);
     break;

  }

  msgLen = sizeof(qcp_napp_evt_msg_s_v01);
  msg = chreHeapAlloc(msgLen);
  if (NULL == msg) {
    FARF(ERROR, "Failed to alloc sample msg");
    return;
  }
  memcpy(msg, &evt_data, msgLen);
  if (!chreSendMessageToHost(msg, msgLen, evt_type, myfree)) {
    FARF(ERROR, "Failed to send sample");
  }
}

static void handle_sensor_events(uint32_t sensType, const void* evtData)
{
  struct HostHubPacket *msgout = 0;
  qcp_sensor_event_msg_v01 sensData;
  uint32_t snsDataLen = 0;

  sensData.hub_id = 0; //Populate hub ID from HAL
  sensData.app_id = TEST_NAPP_ID;
  sensData.sensorType = sensType;
  int i = 0;

  bool send_more_msg = true;
  uint64_t sum_delta_timestamp = 0;
  int itr = 0;

  while(send_more_msg) {
    sensData.numAxis = 0;
    sensData.samplesInfo.numFlushes = 0;
    sensData.samplesInfo.interrupt = 0;
    sensData.samplesInfo.biasBits = 0;
    sensData.samplesInfo.numSamples = 0;
    sensData.firstSampleTs = 0;
    sensData.samples_len = 0;
    
    switch(sensType)
    {
      case CHRE_SENSOR_TYPE_ACCELEROMETER:
      case CHRE_SENSOR_TYPE_UNCALIBRATED_ACCELEROMETER:
      case CHRE_SENSOR_TYPE_GYROSCOPE:
      case CHRE_SENSOR_TYPE_UNCALIBRATED_GYROSCOPE:
      case CHRE_SENSOR_TYPE_GEOMAGNETIC_FIELD:
      case CHRE_SENSOR_TYPE_UNCALIBRATED_GEOMAGNETIC_FIELD:
      {
        struct chreSensorThreeAxisData *evt = (struct chreSensorThreeAxisData *)evtData;
        sensData.numAxis       = NUM_AXIS_THREE_V01;
        sensData.samplesInfo.numFlushes = 0; //evt->samples[0].firstSample.numFlushes;
        sensData.samplesInfo.interrupt  = 0; //evt->samples[0].firstSample.interrupt;

        if(evt->header.readingCount - QCP_MAX_SAMPLES_PER_EVENT_V01 * itr > QCP_MAX_SAMPLES_PER_EVENT_V01) {
          sensData.samplesInfo.numSamples = QCP_MAX_SAMPLES_PER_EVENT_V01;
        } else {
          sensData.samplesInfo.numSamples = evt->header.readingCount - QCP_MAX_SAMPLES_PER_EVENT_V01 * itr;
          send_more_msg = false;
        }
        sensData.firstSampleTs = evt->header.baseTimestamp + sum_delta_timestamp +
                                 evt->readings[QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].timestampDelta;
        for(i = 0; i < sensData.samplesInfo.numSamples; i++) {
          sum_delta_timestamp += evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].timestampDelta;
          sensData.samples[i].deltaTime = evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].timestampDelta;
          sensData.samples[i].x = evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].x;
          sensData.samples[i].y = evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].y;
          sensData.samples[i].z = evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].z;
        }
        sensData.samples_len = i;
        sensData.samples[0].deltaTime = 0;

        snsDataLen = sizeof(qcp_sensor_event_msg_v01);
      }
      break;

      case CHRE_SENSOR_TYPE_PRESSURE:
      {
        struct chreSensorFloatData *evt = (struct chreSensorFloatData *)evtData;
        sensData.numAxis       = NUM_AXIS_ONE_V01;

        if(evt->header.readingCount - QCP_MAX_SAMPLES_PER_EVENT_V01 * itr > QCP_MAX_SAMPLES_PER_EVENT_V01) {
          sensData.samplesInfo.numSamples = QCP_MAX_SAMPLES_PER_EVENT_V01;
        } else {
          sensData.samplesInfo.numSamples = evt->header.readingCount - QCP_MAX_SAMPLES_PER_EVENT_V01 * itr;
          send_more_msg = false;
        }
        sensData.firstSampleTs = evt->header.baseTimestamp + sum_delta_timestamp +
                                 evt->readings[QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].timestampDelta;
        for(i = 0; i < sensData.samplesInfo.numSamples; i++) {
          sum_delta_timestamp += evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].timestampDelta;
          sensData.samples[i].deltaTime = evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].timestampDelta;
          sensData.samples[i].x = evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].value;
          sensData.samples[i].y = 0;
          sensData.samples[i].z = 0;
        }
        sensData.samples_len = i;
        sensData.samples[0].deltaTime = 0;

        snsDataLen = sizeof(qcp_sensor_event_msg_v01);
      }
      break;

      case CHRE_SENSOR_TYPE_PROXIMITY:
      {
        struct chreSensorByteData *evt = (struct chreSensorByteData *)evtData;
        sensData.numAxis       = NUM_AXIS_ONE_V01;

        if (evt->header.readingCount - QCP_MAX_SAMPLES_PER_EVENT_V01 * itr > QCP_MAX_SAMPLES_PER_EVENT_V01) {
          sensData.samplesInfo.numSamples = QCP_MAX_SAMPLES_PER_EVENT_V01;
        } else {
          sensData.samplesInfo.numSamples = evt->header.readingCount - QCP_MAX_SAMPLES_PER_EVENT_V01 * itr;
          send_more_msg = false;
        }
        sensData.firstSampleTs = evt->header.baseTimestamp + sum_delta_timestamp +
                                 evt->readings[QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].timestampDelta;
        for(i = 0; i < sensData.samplesInfo.numSamples; i++) {
          sum_delta_timestamp += evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].timestampDelta;
          sensData.samples[i].deltaTime = evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].timestampDelta;
          sensData.samples[i].x = evt->readings[i + QCP_MAX_SAMPLES_PER_EVENT_V01 * itr].isNear;
          sensData.samples[i].y = 0;
          sensData.samples[i].z = 0;
        }
        sensData.samples_len = i;
        sensData.samples[0].deltaTime = 0;

        snsDataLen = sizeof(qcp_sensor_event_msg_v01);
      }
      break;

      case CHRE_SENSOR_TYPE_INSTANT_MOTION_DETECT:
      case CHRE_SENSOR_TYPE_STATIONARY_DETECT:
      {
        struct chreSensorOccurrenceData *evt = (struct chreSensorOccurrenceData*)evtData;
        sensData.numAxis = 0;
        sensData.samplesInfo.numSamples = 0;
        sensData.firstSampleTs = evt->header.baseTimestamp;
        sensData.samples_len = 0;
        sensData.samples[0].deltaTime = 0;

        snsDataLen = sizeof(qcp_sensor_event_msg_v01);

        send_more_msg = false;
      }
      break;

      default:
        FARF(ERROR, "Unknown Sensor Event Type %u", sensType);
        return;
    }

    FARF(HIGH, "Sensor Event type %d samples %d FirstTime %" PRIu64, sensType, sensData.samplesInfo.numSamples, sensData.firstSampleTs);

    msgout = chreHeapAlloc(snsDataLen);
    if (NULL == msgout) {
      FARF(ERROR, "Failed to alloc sample msg");
      return;
    }
    
    memcpy(msgout, &sensData, snsDataLen);
    if (!chreSendMessageToHost(msgout, snsDataLen, (CHRE_EVENT_SENSOR_FIRST_EVENT+sensType), myfree)) {
      FARF(ERROR, "Failed to send sample");
    }

    itr++;
  }
}

void nanoappHandleEvent(uint32_t senderInstanceId, uint16_t evtType, const void* evtData)
{
  switch(evtType)
  {
    case CHRE_EVENT_MESSAGE_FROM_HOST:
    {
      struct chreMessageFromHostData* msgIn = (struct chreMessageFromHostData*)evtData;

      FARF(HIGH, "Got message from host: type 0x%X, len %u",
           msgIn->reservedMessageType, msgIn->messageSize);

      if (msgIn->reservedMessageType >= (NAPP_CMD_RESET_TESTAPP_V01 + CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE)){
        msgIn->reservedMessageType = msgIn->reservedMessageType - CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE;
        FARF(HIGH, "hub added CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE to type, new type 0x%X", msgIn->reservedMessageType);
      }
      if (msgIn->reservedMessageType < NAPP_CMD_RESET_TESTAPP_V01 || msgIn->reservedMessageType > NAPP_CMD_SENS_TRIG_ON_DEMAND_V01) {
        FARF(ERROR, "invalid cmd type %u", msgIn->reservedMessageType);
        return;
      }
      
      handle_napp_cmds((qcp_napp_cmd_msg_s_v01*)(msgIn->message), msgIn->reservedMessageType);
    }
    break;

    default:
      if (evtType > CHRE_EVENT_SENSOR_FIRST_EVENT && evtType < CHRE_EVENT_SENSOR_LAST_EVENT) {

       // TODO looks like enable/disable is broken, once disabled no way to reenable
       if (evt_to_proxy) {
         handle_sensor_events(evtType & 0xFF, evtData);
       } else {
         FARF(HIGH, "Sensor events disabled");
       }
     } else {
       FARF(HIGH, "Unknown Event 0x%X", (unsigned int)evtType);
     }
  }
  return;
}

void nanoappEnd(void)
{
  FARF(HIGH, "sensor_testapp End");
}

bool nanoappStart(void)
{
  FARF(HIGH, "sensor_testapp Start\n");
  return true;
}
