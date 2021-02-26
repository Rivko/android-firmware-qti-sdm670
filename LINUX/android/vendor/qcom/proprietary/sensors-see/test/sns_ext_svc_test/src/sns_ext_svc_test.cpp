/*=============================================================================
  @file sns_ext_svc_test.cpp

  Test client for ext_svc Sensor.  This Sensor provides a bridge for SEE Sensors
  to access external QMI services.  This particular test client tests access
  to the QMI_LOC service.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <iostream>
#include <cinttypes>
#include "ssc_connection.h"
#include "ssc_utils.h"
#include "sensors_log.h"

#include "sns_ext_svc.pb.h"
#include "location_service_v02.h"

using namespace std;

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

/*=============================================================================
  Static Data
  ===========================================================================*/

static ssc_connection *connection;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

static void
event_cb(const uint8_t *data, size_t size)
{
  sns_client_event_msg pb_event_msg;

  sns_logv("Received QMI indication with length %zu", size);

  pb_event_msg.ParseFromArray(data, size);
  for(int i = 0; i < pb_event_msg.events_size(); i++)
  {
    const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
    sns_logv("event[%d] msg_id=%d, ts=%llu", i, pb_event.msg_id(),
             (unsigned long long) pb_event.timestamp());

    if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == pb_event.msg_id())
    {
      sns_std_error_event error;
      error.ParseFromString(pb_event.payload());

      sns_loge("Received error event %i", error.error());
    }
    else if(SNS_EXT_SVC_MSGID_SNS_EXT_SVC_RESP == pb_event.msg_id())
    {
      sns_ext_svc_resp resp;
      resp.ParseFromString(pb_event.payload());

      sns_logv("Received response msg_id %i, transp_err %i, transaction_id %i",
          resp.msg_id(), resp.transp_err(), resp.transaction_id());
    }
    else if(SNS_EXT_SVC_MSGID_SNS_EXT_SVC_IND == pb_event.msg_id())
    {
      qmiLocEventPositionReportIndMsgT_v02 pos_ind;

      sns_ext_svc_ind ind;

      ind.ParseFromString(pb_event.payload());
      memcpy(&pos_ind, ind.payload().c_str(), ind.payload().size());

      sns_logv("Received ind msg_id %i", ind.msg_id());
      sns_logv("Position: %f, %f", pos_ind.latitude, pos_ind.longitude);
    }
    else
    {
      sns_loge("Received unknown message ID %i", pb_event.msg_id());
    }
  }
}

/* Send a qmiLocRegEventsReqMsgT_v02 to QMI_LOC */
static void
send_event_req(ssc_connection *conn, sensor_uid const *suid)
{
  string pb_req_msg_encoded;
  string ext_svc_req_encoded;
  sns_client_request_msg pb_req_msg;
  sns_ext_svc_req ext_svc_req;
  qmiLocRegEventsReqMsgT_v02 loc_req;
  memset(&loc_req, 0, sizeof(loc_req));

  loc_req.eventRegMask = QMI_LOC_EVENT_MASK_POSITION_REPORT_V02;

  ext_svc_req.set_svc_id(SNS_EXT_SVC_LOCATION);
  ext_svc_req.set_msg_id(QMI_LOC_REG_EVENTS_REQ_V02);
  ext_svc_req.set_transaction_id(5);  // Arbitrary value
  ext_svc_req.set_payload(&loc_req, sizeof(loc_req));
  ext_svc_req.SerializeToString(&ext_svc_req_encoded);

  pb_req_msg.set_msg_id(SNS_EXT_SVC_MSGID_SNS_EXT_SVC_REQ);
  pb_req_msg.mutable_request()->set_payload(ext_svc_req_encoded);
  pb_req_msg.mutable_suid()->set_suid_high(suid->high);
  pb_req_msg.mutable_suid()->set_suid_low(suid->low);
  pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->
      set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

  pb_req_msg.SerializeToString(&pb_req_msg_encoded);

  conn->send_request(pb_req_msg_encoded);
}

/* Send a qmiLocStartReqMsgT_v02 to QMI_LOC */
static void
send_start_req(ssc_connection *conn, sensor_uid const *suid)
{
  string pb_req_msg_encoded;
  string ext_svc_req_encoded;
  sns_client_request_msg pb_req_msg;
  sns_ext_svc_req ext_svc_req;
  qmiLocStartReqMsgT_v02 loc_req;
  memset(&loc_req, 0, sizeof(loc_req));

  loc_req.sessionId = 0;
  loc_req.fixRecurrence_valid = true;
  loc_req.fixRecurrence = eQMI_LOC_RECURRENCE_PERIODIC_V02;
  loc_req.minInterval_valid = false;

  ext_svc_req.set_svc_id(SNS_EXT_SVC_LOCATION);
  ext_svc_req.set_msg_id(QMI_LOC_START_REQ_V02);
  ext_svc_req.set_transaction_id(5);  // Arbitrary value
  ext_svc_req.set_payload(&loc_req, sizeof(loc_req));
  ext_svc_req.SerializeToString(&ext_svc_req_encoded);

  pb_req_msg.set_msg_id(SNS_EXT_SVC_MSGID_SNS_EXT_SVC_REQ);
  pb_req_msg.mutable_request()->set_payload(ext_svc_req_encoded);
  pb_req_msg.mutable_suid()->set_suid_high(suid->high);
  pb_req_msg.mutable_suid()->set_suid_low(suid->low);
  pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->
      set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

  pb_req_msg.SerializeToString(&pb_req_msg_encoded);

  conn->send_request(pb_req_msg_encoded);
}

static void
suid_cb(const std::string& datatype, const std::vector<sensor_uid>& suids)
{
  sns_logv("Received SUID event with length %zu", suids.size());
  if(suids.size() > 0)
  {
    sensor_uid suid = suids.at(0);
    connection = new ssc_connection(event_cb);

    sns_logv("Received SUID %" PRIx64 "%" PRIx64 " for '%s'",
        suid.high, suid.low, datatype.c_str());

    send_event_req(connection, &suid);
    send_start_req(connection, &suid);
  }
}

int
main(int argc, char *argv[])
{
  UNUSED_VAR(argc);
  UNUSED_VAR(argv);

  sensors_log::set_tag("sns_ext_svc_test");
  sensors_log::set_level(sensors_log::VERBOSE);
  sensors_log::set_stderr_logging(true);

  suid_lookup lookup(suid_cb);
  lookup.request_suid("ext_svc");

  cin.get();

  return 0;
}
