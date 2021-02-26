/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_cxt.cpp

  @brief
  SensorContext class implementation.
============================================================================*/
#include "sensor_suid.h"
#include "logging_util.h"

extern bool support_default_sensor();
extern DescriptorPool pool;
extern vector<string> gClientProcessor ;
extern vector<string> gWakeupDeliveryType ;

SuidSensor::SuidSensor(handle_suid_event_cb suid_event_cb):
    event_cb(suid_event_cb)
{
  log_instance = usta_logging_util::get_instance();
  // first we will get the default suid of suid sensor

  const Descriptor* message_descriptor =
      pool.FindMessageTypeByName("sns_suid_sensor");
  if(message_descriptor == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::SuidSensor sns_suid_sensor is Not found");
    return; // For crash the application if sns_suid_sensor is present in the proto.
    // basic message required for USTA to run
  }

  const FieldDescriptor* suid_low_desc =
      message_descriptor->FindFieldByName("suid_low");

  const FieldDescriptor* suid_high_desc =
      message_descriptor->FindFieldByName("suid_high");

  if ((!suid_low_desc)||(!suid_high_desc))
  {
    ERROR_LOG(log_instance , "\n SUID sensor details not present in descriptor pool");
    return; // For crash the application if sns_suid_sensor low and High values are not found in proto.
    // basic message required for USTA to run.
  }

  suid.low = suid_low_desc->default_value_uint64();
  suid.high = suid_high_desc->default_value_uint64();

  // SUID nows opens a connections to SSC client
  ssc_cb = [this](const uint8_t* msg , int msgLength)
			    { this->sensor_event_cb(msg, msgLength); };

  DEBUG_LOG(log_instance , " connection to SSC client from suid initiated ");
  if (NULL == (ssc_client_ptr = new ssc_connection(ssc_cb)))
  {
    ERROR_LOG(log_instance , "SSC connection creation failed from SUID sensor class");
    return;
  }

  // create MessagParser obj
  // proto file separated by delimiter needs to be passed in constructor
  vector<string> api(1,"sns_suid.proto");
  if (NULL== (sensor_parser = new MessageParser()))
  {
    ERROR_LOG(log_instance , "Message parser instance creation failed "
        "from SUID sensor class");
    return ;
  }
  if (USTA_RC_SUCCESS != sensor_parser->set_api_info(api))
  {
    ERROR_LOG(log_instance , "\n Error!! Set API failed for SUID sensor class");
  }

  // setting log file path and opening file
  logfile_name = "suid_list";

  fileStreamPtr=NULL;
  get_client_processors_list();
  get_wakeup_delivery_list();

}
SuidSensor::~SuidSensor()
{
  if (ssc_client_ptr) delete ssc_client_ptr;
  ssc_client_ptr=NULL;
  if (sensor_parser) delete sensor_parser;
  sensor_parser = NULL;

  if(NULL !=  fileStreamPtr )
  {
    fclose(fileStreamPtr);
    fileStreamPtr = NULL;
  }
}
void SuidSensor::get_wakeup_delivery_list()
{
  const EnumDescriptor* enumDescriptor = pool.FindEnumTypeByName("sns_client_delivery");
  if(enumDescriptor == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::get_wakeup_delivery_list sns_client_delivery is Not found");
    return;
  }

  for (int enum_index = 0; enum_index < enumDescriptor->value_count(); enum_index++)
  {
    const EnumValueDescriptor* enum_value_des = enumDescriptor->value(enum_index);
    if(enum_value_des == NULL)
      continue;// This can happen if any mismatch between the SLPI and HLOS proto files.
    //So skipping the current enum value and proceeding further.
    string enum_name = enum_value_des->name();
    gWakeupDeliveryType.push_back(enum_name);
  }
}
void SuidSensor::get_client_processors_list()
{
  const EnumDescriptor* enumDescriptor = pool.FindEnumTypeByName("sns_std_client_processor");
  if(enumDescriptor == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::get_client_processors_list sns_std_client_processor is Not found");
    return;
  }
  for (int enum_index = 0; enum_index < enumDescriptor->value_count(); enum_index++)
  {
    const EnumValueDescriptor* enum_value_des = enumDescriptor->value(enum_index);
    if(enum_value_des == NULL)
      continue; //This can happen if any mismatch between the SLPI and HLOS proto files.
    //So skipping the current enum value and proceeding further.
    string enum_name = enum_value_des->name();
    gClientProcessor.push_back(enum_name);
  }
}
void SuidSensor::sensor_event_cb(const uint8_t *encoded_data_string,
    size_t encoded_string_size) // receiving response from ssc_client
{


  // now we will decodeendcoded_events via reflection to create sensor obj
  // for each suid

  const Descriptor* message_descriptor =
      pool.FindMessageTypeByName("sns_client_event_msg");
  if(message_descriptor == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb sns_client_event_msg is Not found");
    return; // sns_client_event_msg is the basic message to decode the event.
    // Force crash the app if it is not found in any of the proto files.
  }

  DynamicMessageFactory dmf;

  const Message* prototype = dmf.GetPrototype(message_descriptor);
  if(prototype == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb prototype is Null");
    return; //prototype pointer return by google protobuffer. Returning NULL value is highly impossible.
  }

  Message* message = prototype->New();
  if(message == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb message is Null");
    return; //message pointer return by google protobuffer. Returning NULL value is highly impossible.
  }

  message->ParseFromArray((void *)encoded_data_string,encoded_string_size);
  const Message::Reflection* reflection = message->GetReflection();
  if(reflection == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb reflection is Null");
    return; //reflection pointer return by google protobuffer. Returning NULL value is highly impossible.
  }

  const FieldDescriptor* suid_field_desc =
      message_descriptor->FindFieldByName("suid");
  if(suid_field_desc == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb suid_field_desc is Null");
    return; // suid_field_desc is only NULL if there is an mismatch between HLOS and SLPI proto files.
    // Don't force close the app as it is in event context. Avoiding blocking I&T & dev team.
  }

  // since suid is of message type we need to get the sub_message
  Message* sub_message = reflection->MutableMessage(message, suid_field_desc);
  // way to get msg descriptor from field descriptor is field is a message type
  const Descriptor* suid_msg_desc = suid_field_desc->message_type();

  if((!sub_message) || (!suid_msg_desc) )
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb sub_message / suid_msg_desc is Null");
    return; // sub_message is only NULL if there is an mismatch between HLOS and SLPI proto files.
    // Don't force close the app as it is in event context. Avoiding blocking I&T & dev team.
  }

  uint64 suid_low, suid_high;

  suid_low = sub_message->GetReflection()->GetUInt64(*sub_message,
      suid_msg_desc->FindFieldByName("suid_low"));
  suid_high = sub_message->GetReflection()->GetUInt64(*sub_message,
      suid_msg_desc->FindFieldByName("suid_high"));

  //  parse events assuming only one event, which is valid in this case

  const FieldDescriptor* events_field_desc =
      message_descriptor->FindFieldByName("events");
  if(events_field_desc == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb events_field_desc is Null");
    return;
  }

  int num_suid_events = reflection->FieldSize(*message, events_field_desc);

  DEBUG_LOG(log_instance , " number of events are "+ to_string(num_suid_events));

  if(num_suid_events != 1)
  {
    DEBUG_LOG(log_instance, "Number of events are not equal to 1");
    return ;
  }
  // parse for printing in json format
  vector<payload_info> sensor_payload_info;
  sensor_parser->parse_event_message(
      (const char *)encoded_data_string,encoded_string_size, sensor_payload_info);
  DEBUG_LOG(log_instance , " suid event message is " + sensor_parser->get_event_json_string());
  if(log_instance->is_log_disabled() == false)
  {
    if(NULL != fileStreamPtr)
    {
      flockfile(fileStreamPtr);
      fprintf(fileStreamPtr, "%s" ,
          sensor_parser->get_event_json_string().c_str());
      funlockfile(fileStreamPtr);
    }
  }
  // assuming only one event
  sub_message =
      reflection->MutableRepeatedMessage(message, events_field_desc, 0);
  // way to get msg descriptor from field descriptor is field is a message type
  const Descriptor* events_msg_desc = events_field_desc->message_type();
  if(events_msg_desc == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb events_msg_desc is Null");
    return;//events_msg_desc pointer return by google protobuffer. Returning NULL value is highly impossible.
  }

  //2nd arg gives field descriptor
  string payload = sub_message->GetReflection()->GetString(*sub_message,
      events_msg_desc->FindFieldByName("payload"));

  // now we will parse payload as a new message

  message_descriptor = pool.FindMessageTypeByName("sns_suid_event");
  if (NULL == message_descriptor)
  {
    ERROR_LOG(log_instance , "\n sns_suid_event not found in descriptor pool");
    return;
  }
  prototype = dmf.GetPrototype(message_descriptor);
  if(prototype == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb prototype is Null");
    return; // prototype pointer return by google protobuffer. Returning NULL value is highly impossible.
  }

  message = prototype->New();
  if(message == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb message is Null");
    return; // message pointer return by google protobuffer. Returning NULL value is highly impossible.
  }

  message->ParseFromString(payload);
  reflection = message->GetReflection();
  if(reflection == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb reflection is Null");
    return;//reflection pointer return by google protobuffer. Returning NULL value is highly impossible.
  }
  const FieldDescriptor* fd_data_type =
      message_descriptor->FindFieldByName("data_type");
  if(fd_data_type == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb data_type is not found");
    return; // Occurs when there is a mismatch between SLPI and HLOS protos.
    // Avoid force crash to un-block dev and I&T team
  }

  DEBUG_LOG(log_instance , " suid event received for data type " + (reflection->GetString(*message, fd_data_type)));

  const FieldDescriptor* fd_suid = message_descriptor->FindFieldByName("suid");
  if(fd_suid == NULL)
  {
    ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb suid is not found");
    return; // Occurs when there is a mismatch between SLPI and HLOS protos.
    // Avoid force crash to un-block dev and I&T team
  }
  int num_suids = reflection->FieldSize(*message, fd_suid);

  DEBUG_LOG(log_instance , "\nNumber of suids recevied is " + to_string(num_suids));

  for (int j=0;j<num_suids;j++)
  {
    sensor_uid suid_received;
    sub_message = reflection->MutableRepeatedMessage(message, fd_suid, j);
    if(sub_message == NULL)
    {
      ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb suid is not found");
      continue;// sub_message may NULL due to encoding issues at SLPI.
      // Avoid force crash to contiue with remaining repeated messages
    }

    const Descriptor* sub_des = sub_message->GetDescriptor();
    if(sub_des == NULL)
    {
      ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb sub_des is NULL");
      return;//sub_des pointer return by google proto buffer. Returning NULL value is highly impossible.
    }
    const FieldDescriptor* fd1 = sub_des->FindFieldByName("suid_low");
    if(fd1 == NULL)
    {
      ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb suid_low is not found");
      continue;//fd1 can be NULL if proto mismatch between SLPI and HLOS
    }

    suid_received.low =
        sub_message->GetReflection()->GetUInt64(*sub_message, fd1);
    fd1 = sub_des->FindFieldByName("suid_high");
    if(fd1 == NULL)
    {
      ERROR_LOG(log_instance, "SuidSensor::sensor_event_cb suid_high is not found");
      continue;//fd1 can be NULL if proto mismatch between SLPI and HLOS
    }
    suid_received.high =
        sub_message->GetReflection()->GetUInt64(*sub_message, fd1);


    event_cb(suid_received.low,suid_received.high,
        (j==(num_suids-1)?true:false));
  }

}

usta_rc SuidSensor::send_request(string& data_type)
{
  usta_rc rc= USTA_RC_SUCCESS;

  if(NULL == fileStreamPtr)
  {
    string logfile_completepath = LOG_FOLDER_PATH + logfile_name + ".txt";
    DEBUG_LOG(log_instance , "logFile Name for sensor list: is " + logfile_completepath);
    fileStreamPtr = fopen(logfile_completepath.c_str() , "w");

    if(NULL == fileStreamPtr)
    {
      ERROR_LOG(log_instance , "logFile creation failed for " + logfile_name + " error is " + to_string(errno));
      // Should not crash or return Error messages only because of fopen failed. It is low priority.
    }
  }

  // get msg id from descrptor pool
  const EnumValueDescriptor* suid_req_evd =
      pool.FindEnumValueByName("SNS_SUID_MSGID_SNS_SUID_REQ");

  if (NULL == suid_req_evd)
  {
    ERROR_LOG(log_instance , "\n No enum found with name SNS_SUID_MSGID_SNS_SUID_REQ");
    return USTA_RC_DESC_FAILED;
  }

  string encoded_sns_request;

  send_req_msg_info sensor_req_msg;
  sensor_req_msg.msg_name = "sns_suid_req";
  sensor_req_msg.msgid = (to_string(suid_req_evd->number()));
  send_req_field_info dataType_field;
  {
    dataType_field.name = "data_type";
    dataType_field.value.push_back(data_type);
  }
  send_req_field_info register_updates_field;
  {
    register_updates_field.name = "register_updates";
    register_updates_field.value.push_back("false");
  }
  if(true == support_default_sensor()) {
    send_req_field_info default_only_field;
    {
      default_only_field.name = "default_only";
      default_only_field.value.push_back("false");
    }
    sensor_req_msg.fields.push_back(default_only_field);
  }
  sensor_req_msg.fields.push_back(dataType_field);
  sensor_req_msg.fields.push_back(register_updates_field);

  if(USTA_RC_SUCCESS !=
      (rc=sensor_parser->formulate_request_msg(sensor_req_msg,
          encoded_sns_request)))
  {
    ERROR_LOG(log_instance , "\nError!! encoding payload request failed for"
        " SUID class sensor");
    return rc;
  }
  rc = sensor_parser->parse_request_message(sensor_req_msg.msg_name, encoded_sns_request.c_str(),
      encoded_sns_request.size());
  if(USTA_RC_SUCCESS != rc)
  {
    ERROR_LOG(log_instance , "parse request message is failed for SUID Class");
    return rc;
  }


  // formulating main message
  send_req_msg_info client_req_msg;
  client_req_msg.msg_name = "sns_client_request_msg";
  client_req_msg.msgid = (to_string(suid_req_evd->number()));

  send_req_field_info suid_field;
  {
    suid_field.name = "suid";

    send_req_nested_msg_info suid_msg;
    {
      suid_msg.msg_name = "sns_std_suid";

      send_req_field_info suid_low_field;
      suid_low_field.name = "suid_low";
      suid_low_field.value.push_back(to_string(suid.low));

      send_req_field_info suid_high_field;
      suid_high_field.name = "suid_high";
      suid_high_field.value.push_back(to_string(suid.high));

      suid_msg.fields.push_back(suid_low_field);
      suid_msg.fields.push_back(suid_high_field);
    }
    suid_field.nested_msgs.push_back(suid_msg);
  }
  client_req_msg.fields.push_back(suid_field);

  send_req_field_info msg_id_field;
  {
    msg_id_field.name = "msg_id";
    msg_id_field.value.push_back(to_string(suid_req_evd->number()));
  }
  client_req_msg.fields.push_back(msg_id_field);

  send_req_field_info susp_config_field;
  {
    susp_config_field.name = "susp_config";

    send_req_nested_msg_info susp_config_msg;
    {
      susp_config_msg.msg_name = "suspend_config";
      send_req_field_info client_field;
      {
        client_field.name = "client_proc_type";
        client_field.value.push_back("SNS_STD_CLIENT_PROCESSOR_APSS");
      }
      send_req_field_info wakeup_field;
      {
        wakeup_field.name = "delivery_type";
        wakeup_field.value.push_back("SNS_CLIENT_DELIVERY_WAKEUP");
      }
      susp_config_msg.fields.push_back(client_field);
      susp_config_msg.fields.push_back(wakeup_field);
    }
    susp_config_field.nested_msgs.push_back(susp_config_msg);
  }
  client_req_msg.fields.push_back(susp_config_field);


  send_req_field_info request_field;
  {
    request_field.name = "request";

    send_req_nested_msg_info sns_std_request_msg;
    {
      sns_std_request_msg.msg_name = "sns_std_request";

      send_req_field_info batching_field;
      {
        batching_field.name = "batching";

        send_req_nested_msg_info batch_spec_msg;
        {
          batch_spec_msg.msg_name = "batch_spec";
          send_req_field_info batch_period_field;
          {
            batch_period_field.name = "batch_period";
            batch_period_field.value.push_back("0");
          }
          send_req_field_info flush_period_field;
          {
            flush_period_field.name = "flush_period";
            flush_period_field.value.push_back("0");
          }
          send_req_field_info flush_only_field;
          {
            flush_only_field.name = "flush_only";
            flush_only_field.value.push_back(to_string(false));
          }
          send_req_field_info max_batch_field;
          {
            max_batch_field.name = "max_batch";
            max_batch_field.value.push_back(to_string(false));
          }
          batch_spec_msg.fields.push_back(batch_period_field);
          batch_spec_msg.fields.push_back(flush_period_field);
          batch_spec_msg.fields.push_back(flush_only_field);
          batch_spec_msg.fields.push_back(max_batch_field);
        }
        batching_field.nested_msgs.push_back(batch_spec_msg);
      }
      sns_std_request_msg.fields.push_back(batching_field);

      send_req_field_info payload_field;
      {
        payload_field.name = "payload";
        payload_field.value.push_back(encoded_sns_request);
      }
      sns_std_request_msg.fields.push_back(payload_field);

    }
    request_field.nested_msgs.push_back(sns_std_request_msg);
  }
  client_req_msg.fields.push_back(request_field);



  string main_request_encoded;
  if(USTA_RC_SUCCESS !=
      (rc=sensor_parser->formulate_request_msg(client_req_msg,main_request_encoded)))
  {
    ERROR_LOG(log_instance , "Error!! encoding main request failed for SUID class sensor");
    return rc;
  }

  rc = sensor_parser->parse_request_message(client_req_msg.msg_name,main_request_encoded.c_str(),
      main_request_encoded.size());
  if(USTA_RC_SUCCESS != rc)
  {
    ERROR_LOG(log_instance , "parse request message is failed for SUID Class");
    return rc;
  }

  USTA_LOGD("request message is  %s" , sensor_parser->get_request_json_string().c_str());
  if(log_instance->is_log_disabled() == false)
  {
    if(NULL != fileStreamPtr)
    {
      flockfile(fileStreamPtr);
      fprintf(fileStreamPtr, "%s" ,
          sensor_parser->get_request_json_string().c_str());
      funlockfile(fileStreamPtr);
    }
  }

  DEBUG_LOG(log_instance , "\nSend suid request to SSC client");
  ssc_client_ptr->send_request(main_request_encoded);
  return rc;

}
