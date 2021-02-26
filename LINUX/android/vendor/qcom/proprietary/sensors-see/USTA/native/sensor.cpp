/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor.cpp
  @brief
  sensor class implementation.
============================================================================*/

#include "sensor.h"
#include "logging_util.h"


extern DescriptorPool pool;
#define MAX_ADB_LOGCAT_LENGTH 1000
Sensor::Sensor(handle_attribute_event_func attrib_event_f,
    const uint64_t& suid_low,const uint64_t& suid_high,
    handle_stream_event_func stream_event_f,
    handle_stream_error_func stream_error_f ,
    handle_stream_error_func attrib_error_f)
{
  log_instance = usta_logging_util::get_instance();
  attrib_event_cb = attrib_event_f;
  suid.low=suid_low;
  suid.high=suid_high;

  ssc_stream_cb=[this](const uint8_t* msg , int msgLength)
          { this->sensor_stream_event_cb(msg, msgLength); };

  // stream event obj will be created later
  ssc_stream_client_obj = NULL;
  ssc_attrib_client_obj = NULL;

  stream_error_cb  = stream_error_f;
  attrib_error_cb  = attrib_error_f;

  ssc_attrib_cb=[this](const uint8_t* msg , int msgLength)
          { this->sensor_attrib_event_cb(msg, msgLength); };

  ssc_stream_error_cb=[this](ssc_error_type error)
          { this->sensor_stream_error_cb(error); };

  ssc_stream_resp_cb=[this](uint32_t resp_value)
          { this->sensor_stream_resp_cb(resp_value); };

  ssc_attrib_error_cb=[this](ssc_error_type error)
          { this->sensor_attrib_error_cb(error); };


#if 0
  if (NULL == (ssc_attrib_client_obj = new ssc_connection(ssc_attrib_cb)))
  {
    ERROR_LOG(log_instance , "ssc_attrib_client_obj creation failed");
  }
#endif
  sensor_attribute_obj = NULL;
  if(log_instance->is_log_disabled() == false)
    sensors_diag_log_obj = new sensors_diag_log((uint64_t)this);
  else
    sensors_diag_log_obj = NULL;
  stream_event_cb =stream_event_f;
  if (NULL == (sensor_parser = new MessageParser()))
  {
    ERROR_LOG(log_instance , " message parser creation failed ");
  }
  file_stream_ptr = NULL;
  event_count = 0;
  // initialize file mutex
  pthread_mutex_init(&file_mutex, NULL);
  pthread_mutex_init(&proto_buf_mutex, NULL);
  pthread_mutex_init(&sample_event_mutex, NULL);

  streaming_status_flag = false;
  is_registry_sensor = false;
}
Sensor::~Sensor()
{
  pthread_mutex_lock(&file_mutex);
  if(NULL !=  file_stream_ptr )
  {
    fflush(file_stream_ptr);
    fclose(file_stream_ptr);
    file_stream_ptr = NULL;
  }
  pthread_mutex_unlock(&file_mutex);

  if (sensor_attribute_obj) delete sensor_attribute_obj;
  sensor_attribute_obj = NULL;

  if (ssc_attrib_client_obj) delete ssc_attrib_client_obj;
  ssc_attrib_client_obj=NULL;

  if (ssc_stream_client_obj) delete ssc_stream_client_obj;
  ssc_stream_client_obj=NULL;

  if (sensor_parser) delete sensor_parser;
  sensor_parser=NULL;

  if(sensors_diag_log_obj) delete sensors_diag_log_obj;
  sensors_diag_log_obj=NULL;

  pthread_mutex_destroy(&file_mutex);
  pthread_mutex_destroy(&proto_buf_mutex);
  pthread_mutex_destroy(&sample_event_mutex);
}

void Sensor::remove_attribute_connection()
{
  DEBUG_LOG(log_instance , "ssc_attrib_client_obj removed");
  if (ssc_attrib_client_obj) delete ssc_attrib_client_obj;
  ssc_attrib_client_obj=NULL;

}
usta_rc Sensor::send_attrib_request()
{

  usta_rc rc= USTA_RC_SUCCESS;

  string sns_attribute_request_encoded;
  if(ssc_attrib_client_obj == NULL)
  {
    try {
      ssc_attrib_client_obj = new ssc_connection(ssc_attrib_cb);
      DEBUG_LOG(log_instance, "Attribute connection is created ");
    } catch (const exception& e) {
      string error_msg(e.what());
      ERROR_LOG(log_instance, "could not create ssc_connection:" + error_msg);
      return USTA_RC_NO_MEMORY;
    }
    ssc_attrib_client_obj->register_error_cb(ssc_attrib_error_cb);
  }

  send_req_msg_info sensor_req_msg;
  sensor_req_msg.msg_name = "sns_std_attr_req";
  send_req_field_info register_updates_field;
  {
    register_updates_field.name = "register_updates";
    register_updates_field.value.push_back("false");
  }
  sensor_req_msg.fields.push_back(register_updates_field);


  if(USTA_RC_SUCCESS !=
      (rc=sensor_parser->formulate_request_msg(sensor_req_msg,
          sns_attribute_request_encoded)))
  {
    ERROR_LOG(log_instance , "encoding attribute payload request failed");
    return rc;
  }

  const EnumValueDescriptor *sns_attrib_req_evd =
      pool.FindEnumValueByName("SNS_STD_MSGID_SNS_STD_ATTR_REQ");

  if (NULL == sns_attrib_req_evd)
  {
    ERROR_LOG(log_instance , " SNS_STD_MSGID_SNS_STD_ATTR_REQ not found in descriptor pool");
    return USTA_RC_DESC_FAILED;
  }
  // formulating main message
  send_req_msg_info client_req_msg;
  client_req_msg.msg_name = "sns_client_request_msg";
  client_req_msg.msgid = (to_string(sns_attrib_req_evd->number()));

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
    msg_id_field.value.push_back(to_string(sns_attrib_req_evd->number()));
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
        payload_field.value.push_back(sns_attribute_request_encoded);
      }
      sns_std_request_msg.fields.push_back(payload_field);

    }
    request_field.nested_msgs.push_back(sns_std_request_msg);
  }
  client_req_msg.fields.push_back(request_field);

  //encoding the final request
  string sns_request_encoded;

  if(USTA_RC_SUCCESS !=
      (rc=sensor_parser->formulate_request_msg(client_req_msg,
          sns_request_encoded)))
  {
    ERROR_LOG(log_instance , "Encoding main request for attribute failed");
    return rc;
  }

  rc = sensor_parser->parse_request_message(client_req_msg.msg_name,sns_request_encoded.c_str(),
      sns_request_encoded.size());
  if(USTA_RC_SUCCESS != rc)
  {
    ERROR_LOG(log_instance , "Error while parse request message ");
    return rc;
  }

  USTA_LOGD("attribute request message is   %s" , sensor_parser->get_request_json_string().c_str());

  ssc_attrib_client_obj->send_request(sns_request_encoded);

  return USTA_RC_SUCCESS;

}

void Sensor::sensor_stream_event_cb(const uint8_t *encoded_data_string,
    size_t encoded_string_size)
{
  //streaming events
  string encoded_client_event_msg((char *)encoded_data_string,encoded_string_size);
  if(log_instance->is_log_disabled() == false)
  {
    char logging_module_name[] = "USTA";
    if(sensors_diag_log_obj != NULL)
      sensors_diag_log_obj->log_event_msg(
          encoded_client_event_msg,data_type,logging_module_name);
  }
  pthread_mutex_lock(&proto_buf_mutex);
  vector<payload_info> sensor_payload_info;
  sensor_parser->parse_event_message((const char *)encoded_data_string,
      encoded_string_size , sensor_payload_info);
  string currentSample = "";

  string log_message = sensor_parser->get_event_json_string();
  DEBUG_LOG(log_instance , "log_message_size " + to_string(log_message.size()));
  if(log_message.size() < MAX_ADB_LOGCAT_LENGTH)
  {
    DEBUG_LOG(log_instance , log_message);
  }
  else
  {
    int maxLoopCount = log_message.size() / MAX_ADB_LOGCAT_LENGTH + 1;
    string remainingLogBuffer = log_message;
    int loopCount = 0;
    while(loopCount < maxLoopCount) {
      int startIndex = 0;
      int endIndex = (remainingLogBuffer.size() > MAX_ADB_LOGCAT_LENGTH )?MAX_ADB_LOGCAT_LENGTH:remainingLogBuffer.size();
      string subString = remainingLogBuffer.substr(startIndex, endIndex);
      DEBUG_LOG(log_instance , subString);
      loopCount = loopCount + 1 ;
      remainingLogBuffer = remainingLogBuffer.substr(endIndex );
    }
  }

  for(unsigned int i =0; i < sensor_payload_info.size() ; i++)
  {
    //DEBUG_LOG(log_instance , "name " + sensor_payload_info[i].name);
    if(sensor_payload_info[i].name.empty())
      continue;
    currentSample += sensor_payload_info[i].name;
    currentSample += "::";
    for(unsigned int j=0; j < sensor_payload_info[i].value.size() ; j++)
    {
      //DEBUG_LOG(log_instance, sensor_payload_info[i].value[j]);
      currentSample += sensor_payload_info[i].value[j];
      if(j ==  sensor_payload_info[i].value.size() - 1)
        currentSample += ";";
      else
        currentSample += ",";
    }
    currentSample += "\n";
  }

  // acquiring file mutex
  pthread_mutex_lock(&file_mutex);
  if(log_instance->is_log_disabled() == false)
  {
    if(NULL != file_stream_ptr)
    {
      fprintf(file_stream_ptr, "%s" ,
          sensor_parser->get_event_json_string().c_str());
      fflush(file_stream_ptr);
    }
  }
  pthread_mutex_unlock(&file_mutex);
  pthread_mutex_unlock(&proto_buf_mutex);

  event_count++;
  DEBUG_LOG(log_instance , name_vendor + " event message count (image (05_24)) is " + to_string(event_count));


  // call to JNI funciton only if this flag is true
  if(streaming_status_flag == true)
  {
    if(stream_event_cb)
    {
      pthread_mutex_lock(&sample_event_mutex);
      if(data_type.compare("registry") == 0)
        is_registry_sensor = true;
      else
        is_registry_sensor = false;
      stream_event_cb(sensor_parser->get_event_json_string() , is_registry_sensor);
      pthread_mutex_unlock(&sample_event_mutex);
    }
  }


}

void Sensor::sensor_stream_error_cb(ssc_error_type error_code)
{
  stream_error_cb(error_code);
}
string Sensor::get_enum_string_from_value(uint32_t resp_value)
{
  string enum_name = "";
  switch(resp_value)
  {
    case 0:
      enum_name += "SNS_STD_ERROR_NO_ERROR";
      break;
    case 1:
      enum_name += "SNS_STD_ERROR_FAILED";
      break;
    case 2:
      enum_name += "SNS_STD_ERROR_NOT_SUPPORTED";
      break;
    case 3:
      enum_name += "SNS_STD_ERROR_INVALID_TYPE";
      break;
    case 4:
      enum_name += "SNS_STD_ERROR_INVALID_STATE";
      break;
    case 5:
      enum_name += "SNS_STD_ERROR_INVALID_VALUE";
      break;
    case 6:
      enum_name += "SNS_STD_ERROR_NOT_AVAILABLE";
      break;
    case 7:
      enum_name +="SNS_STD_ERROR_POLICY";
      break;
    default:
      enum_name +="InValid Error Type";
      break;
  }
  return enum_name;
}
void Sensor::sensor_stream_resp_cb(uint32_t resp_value)
{
  if(log_instance->is_log_disabled() == false)
  {
    char logging_module_name[] = "USTA";
    if(sensors_diag_log_obj != NULL)
      sensors_diag_log_obj->log_response_msg(
          resp_value,data_type,logging_module_name);
  }
  string resp_msg_name = "sns_client_response_msg";
  string suid_name = "suid";
  string suid_low_name = "suid_low";
  string suid_high_name = "suid_high";
  string sns_std_error_name = "sns_std_error";
  string value_name = "value";

  stringstream ss_low;
  ss_low << std::hex << suid.low;

  stringstream ss_high;
  ss_high << std::hex << suid.high;

  string resp_msg = "";
  resp_msg += "\"" + resp_msg_name + "\"";
  resp_msg += " : {";
  resp_msg += "\n";
  resp_msg += " ";
  resp_msg += "\"" + suid_name + "\"";
  resp_msg += " : {";
  resp_msg += "\n";
  resp_msg += "  ";
  resp_msg += "\"" + suid_low_name + "\"";
  resp_msg += " : ";
  resp_msg += "\"" ;
  resp_msg += "0x" + ss_low.str() + "\"";
  resp_msg += ",";
  resp_msg += "\n";
  resp_msg += "  ";
  resp_msg += "\"" + suid_high_name + "\"";
  resp_msg += " : ";
  resp_msg += "\"" ;
  resp_msg += "0x" + ss_high.str() + "\"";
  resp_msg += ",";
  resp_msg += "\n";
  resp_msg += "},";
  resp_msg += "\n";
  resp_msg += "\""+ sns_std_error_name + "\"";
  resp_msg += " : {";
  resp_msg += "\n";
  resp_msg += " ";
  resp_msg += "\"" + value_name + "\"";
  resp_msg += " : ";
  resp_msg += "\"" + get_enum_string_from_value(resp_value) + "\"";
  resp_msg += "\n";
  resp_msg += " }";
  resp_msg += "\n";
  resp_msg += "}";
  USTA_LOGD("\n%s" , resp_msg.c_str());

  pthread_mutex_lock(&file_mutex);
  if(NULL != file_stream_ptr)
  {
    fprintf(file_stream_ptr, "%s" ,resp_msg.c_str());
    fflush(file_stream_ptr);
  }
  pthread_mutex_unlock(&file_mutex);

}

void Sensor::sensor_attrib_error_cb(ssc_error_type error_code)
{
  //sending attribute request for this sensor again
  ERROR_LOG(log_instance, "Error cb was initiated ");
  send_attrib_request();
}

void Sensor::sensor_attrib_event_cb(const uint8_t *encoded_data_string,
    size_t encoded_string_size)
{
  string encoded_client_event_msg((char *)encoded_data_string,encoded_string_size);

  const Descriptor* message_descriptor =
      pool.FindMessageTypeByName("sns_client_event_msg");
  if(message_descriptor == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: message_descriptor is NULL");
    //message_descriptor is returned by google proto buffer.
    // Possible only if sns_client_event_msg is missing in proto file.
    return;
  }

  DynamicMessageFactory dmf;
  const Message* prototype = dmf.GetPrototype(message_descriptor);
  if(prototype == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: prototype is NULL");

    // It is common to all the sensors
    //prototype is returned by google proto buffer.
    // Which is highly impossible.
    return;
  }

  Message* message = prototype->New();
  if(message == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: message is NULL");

    // It is common to all the sensors
    //message is returned by google proto buffer.
    // Which is highly impossible.
   return;

  }

  message->ParseFromArray((void *)encoded_data_string,encoded_string_size);

  const Message::Reflection* reflection = message->GetReflection();
  if(reflection == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: reflection is NULL");
    //reflection is returned by google proto buffer.
    // Which is highly impossible.
    return;

  }
  //decoding each element of sns_client_event_msg using their field descriptors
  /* first element is suid of type sns_std_suid */
  const FieldDescriptor* suid_field_desc =
      message_descriptor->FindFieldByName("suid");
  if(suid_field_desc == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: suid_field_desc is NULL");
    return; //It is w.r.t only this particular event. Shout not crash the app. Continue with next events
  }
  // since suid is of message type we need to get the sub_message
  Message* sub_message = reflection->MutableMessage(message, suid_field_desc);
  if(sub_message == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: sub_message is NULL");
    return; //It is w.r.t only this particular event. Shout not crash the app. Continue with next events
  }
  const Descriptor* suid_msg_desc = suid_field_desc->message_type();
  if(suid_msg_desc == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: suid_msg_desc is NULL");
    return;//It is w.r.t only this particular event. Shout not crash the app. Continue with next events
  }
  // way to get msg descriptor from field descriptor is field is a message type
  uint64 suid_low, suid_high;

  suid_low = sub_message->GetReflection()->GetUInt64(*sub_message,
      suid_msg_desc->FindFieldByName("suid_low"));
  suid_high = sub_message->GetReflection()->GetUInt64(*sub_message,
      suid_msg_desc->FindFieldByName("suid_high"));
  //2nd arg gives field descriptor


  /* second element is repeated event of type sns_client_event*/

  const FieldDescriptor* events_field_desc =
      message_descriptor->FindFieldByName("events");
  if(events_field_desc == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: events_field_desc is NULL");
    return;//It is w.r.t only this particular event. Shout not crash the app. Continue with next events
  }
  int num_events = reflection->FieldSize(*message, events_field_desc);

  if(num_events != 1)
  {
//    DEBUG_LOG(log_instance , " unexpected number " + to_string(num_events) + " of events for attributes");
    return ;
  }

  // parse for printing in json format
  vector<payload_info> sensor_payload_info;
  sensor_parser->parse_event_message((const char *)encoded_data_string,
      encoded_string_size, sensor_payload_info);
  DEBUG_LOG(log_instance , " attribute event message is %s" + sensor_parser->get_event_json_string());
  attrib_json_string =  sensor_parser->get_event_json_string();


  // assuming only one event, which is valid assumption for attribute
  sub_message =
      reflection->MutableRepeatedMessage(message, events_field_desc, 0);
  const Descriptor* events_msg_desc = events_field_desc->message_type();
  if(events_msg_desc == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: events_msg_desc is NULL");
    return;//It is w.r.t only this particular event. Shout not crash the app. Continue with next events
  }
  //way to get msg descriptor from field descriptor is field is a message type

  uint32 msg_id = sub_message->GetReflection()->GetUInt32(*sub_message,events_msg_desc->FindFieldByName("msg_id"));
  const EnumValueDescriptor *sns_flush_event_evd =
      pool.FindEnumValueByName("SNS_STD_MSGID_SNS_STD_FLUSH_EVENT");
  if(sns_flush_event_evd == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attrib_event_cb: sns_flush_event_evd is NULL");
    return;//sns_flush_event_evd is NULL if SNS_STD_MSGID_SNS_STD_FLUSH_EVENT is not present in proto. In this case, skip this event.
  }
  uint32 flush_event_msgid = sns_flush_event_evd->number();
  if(msg_id == flush_event_msgid)
  {
    DEBUG_LOG(log_instance , "Avoiding Flush Event");
    return;//flush_event_msgid is NULL if SNS_STD_MSGID_SNS_STD_FLUSH_EVENT is not present in proto. In this case, skip this event.
  }

  string payload = sub_message->GetReflection()->GetString(*sub_message,
      events_msg_desc->FindFieldByName("payload"));


  if (NULL == (sensor_attribute_obj = new sensor_attribute()))
  {
    ERROR_LOG(log_instance , " failed to created SensorAttributeObj");
    return; // Continue with next attribute event. Should not force crash the application
  }
  else
  {
    if(USTA_RC_SUCCESS != sensor_attribute_obj->store(payload))
      return; // Continue with next attribute event. Should not force crash the application

  }

  if(log_instance->is_log_disabled() == false)
  {
    char logging_module_name[] = "USTA";
    if(sensors_diag_log_obj != NULL)
      sensors_diag_log_obj->log_event_msg(
          encoded_client_event_msg,sensor_attribute_obj->get_dataType(),logging_module_name);
  }

  if (attrib_event_cb) attrib_event_cb();


}



bool Sensor::is_platform_sensor()
{
  if(!sensor_attribute_obj)
  {
    ERROR_LOG(log_instance , " No attribute class created");
    return true; // better to delete this sensor from the sensor list
  }

  if(sensor_attribute_obj->is_available(sns_std_sensor_attrid_type))
  {
    string data_type = sensor_attribute_obj->get_dataType();
    if((!data_type.compare("async_com_port")) ||
        (!data_type.compare("interrupt")) ||
        (!data_type.compare("internal_system_signal")) ||
        (!data_type.compare("test")) ||
        (!data_type.compare("timer"))||
        (!data_type.compare("suid")))
    {
      DEBUG_LOG(log_instance , " Filtered sensor is " + data_type);
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    ERROR_LOG(log_instance , "unknown DATA TYPE");
    return true;
  }

}



usta_rc Sensor::get_sensor_info(sensor_info& sensor_info_var)
{
  string dataType = sensor_attribute_obj->get_dataType();
  string vendorName = sensor_attribute_obj->get_vendor();


  string log_file("");
  name_vendor = log_file + dataType + "-"+ vendorName;

  data_type = dataType;

  stringstream suidLow;
  suidLow << std::hex << suid.low;

  stringstream suidHigh;
  suidHigh << std::hex << suid.high;

  sensor_info_var.data_type = dataType;
  sensor_info_var.vendor = vendorName;
  sensor_info_var.suid_high = suidHigh.str();
  sensor_info_var.suid_low = suidLow.str();

  return USTA_RC_SUCCESS;
}

usta_rc Sensor::get_request_list(vector<msg_info> &request_msgs)
{

  vector<string> api;
  usta_rc rc=USTA_RC_SUCCESS;
  if (sensor_parser!=NULL)
  {
    if (sensor_attribute_obj->is_available(sns_std_sensor_attrid_api))
      api = sensor_attribute_obj->get_API();
    else
    {
      ERROR_LOG(log_instance , "API attribute not found");
      return USTA_RC_ATTRIB_FAILED;
    }

    if(USTA_RC_SUCCESS != (rc=sensor_parser->set_api_info(api)))
    {
      ERROR_LOG(log_instance , " get request failed");
      return rc;
    }
    if(USTA_RC_SUCCESS!= (rc=sensor_parser->get_request_msg(request_msgs)))
    {
      ERROR_LOG(log_instance , " get request failed");
      return rc;
    }
    return rc;
  }

  return USTA_RC_FAILED;  // if sensor_parser is NULL

}

usta_rc  Sensor::get_attributes(string& attribute_json_format)
{
  if (attrib_json_string.size() != 0)
    attribute_json_format.assign(attrib_json_string);
  else
  {
    ERROR_LOG(log_instance , " Attribute list is Empty");
  }

  return USTA_RC_SUCCESS;
}

void Sensor::register_error_cb(usta_error_cb_type connection_type)
{
  if(connection_type == USTA_QMI_ERRRO_CB_FOR_ATTRIBUTES)
  {
    if(ssc_attrib_client_obj == NULL)
    {
      if (NULL == (ssc_attrib_client_obj = new ssc_connection(ssc_attrib_cb)))
      {
        ERROR_LOG(log_instance , "ssc_attrib_client_obj creation failed");
      }
    }
    ssc_attrib_client_obj->register_error_cb(ssc_attrib_error_cb);
  }
  else if(connection_type == USTA_QMI_ERRRO_CB_FOR_STREAM)
  {
    if (ssc_stream_client_obj==NULL)
    {
      if (NULL == (ssc_stream_client_obj = new ssc_connection(ssc_stream_cb)))
      {
        ERROR_LOG(log_instance , " creation of ssc connection failed for streaming");
      }
    }
    ssc_stream_client_obj->register_error_cb(ssc_stream_error_cb);
    ssc_stream_client_obj->register_resp_cb(ssc_stream_resp_cb);
  }
}
usta_rc Sensor::send_request(send_req_msg_info& formated_request,client_req_msg_fileds &std_fields_data,
    string& in_logfile_name , string &usta_mode)
{
  usta_rc rc= USTA_RC_SUCCESS;

  if (ssc_stream_client_obj==NULL)
  {
    if (NULL == (ssc_stream_client_obj = new ssc_connection(ssc_stream_cb)))
    {
      ERROR_LOG(log_instance , " creation of ssc connection failed for streaming");
      return USTA_RC_NO_MEMORY;
    }
    ssc_stream_client_obj->register_resp_cb(ssc_stream_resp_cb);
  }

  string logfile_completepath ;

  if(in_logfile_name.length() == 0)
  {
    logfile_completepath = LOG_FOLDER_PATH +
        name_vendor + "_"+ usta_mode +".txt";
  }
  else
  {
    logfile_completepath = LOG_FOLDER_PATH +
        name_vendor + "_" + usta_mode + "_" + in_logfile_name ;
  }

  DEBUG_LOG(log_instance , "logFile Name for " + name_vendor + " : is " + logfile_completepath);
  if(log_instance->is_log_disabled() == false)
  {
    if(NULL == file_stream_ptr)
    {
      file_stream_ptr = fopen(logfile_completepath.c_str() , "w");
      /*Adding sensor attribute info @ the beg of the file*/
      if(NULL != file_stream_ptr)
      {
        fprintf(file_stream_ptr, "%s" , attrib_json_string.c_str());
        fflush(file_stream_ptr);
      }
      else
      {
        ERROR_LOG(log_instance , "logFile creation failed for " + name_vendor);
      }
    }
  }

  string encoded_sns_request;
  if(USTA_RC_SUCCESS !=
      (rc=sensor_parser->formulate_request_msg(formated_request,
          encoded_sns_request)))
  {
    if(rc != USTA_RC_DESC_FAILED)
    {
      /*USTA_RC_DESC_FAILED means, message name was not found in the descriptor pool even though message id was present. This is the case with on-change sensors.
	  Added this condition to support on-change sensors from USTA*/
      return rc;
    }
  }
  pthread_mutex_lock(&proto_buf_mutex);
  rc = sensor_parser->parse_request_message(formated_request.msg_name, encoded_sns_request.c_str(),
      encoded_sns_request.size());
  if(USTA_RC_SUCCESS != rc)
  {
    if(encoded_sns_request.empty()) {
      DEBUG_LOG(log_instance , "encoded string is empty");
    }
    else{
      DEBUG_LOG(log_instance , "Encoded string is not empty");
    }
  }
  pthread_mutex_unlock(&proto_buf_mutex);

  send_req_msg_info client_req_msg;
  client_req_msg.msg_name = "sns_client_request_msg";
  client_req_msg.msgid = formated_request.msgid;

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
    msg_id_field.value.push_back(formated_request.msgid);
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
        if(!std_fields_data.client_type.empty())
          client_field.value.push_back(std_fields_data.client_type);
      }
      send_req_field_info wakeup_field;
      {
        wakeup_field.name = "delivery_type";
        if(!std_fields_data.wakeup_type.empty())
          wakeup_field.value.push_back(std_fields_data.wakeup_type);
      }
      if(client_field.value.size() != 0)
        susp_config_msg.fields.push_back(client_field);
      if(wakeup_field.value.size() != 0)
        susp_config_msg.fields.push_back(wakeup_field);
    }
    if(susp_config_msg.fields.size() != 0)
      susp_config_field.nested_msgs.push_back(susp_config_msg);
  }
  if(susp_config_field.nested_msgs.size() != 0)
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
            if(!std_fields_data.batch_period.empty())
            {
              batch_period_field.value.push_back(std_fields_data.batch_period);
            }
          }
          send_req_field_info flush_period_field;
          {
            flush_period_field.name = "flush_period";
            if(!std_fields_data.flush_period.empty())
            {
              flush_period_field.value.push_back(std_fields_data.flush_period);
            }

          }
          send_req_field_info flush_only_field;
          {
            flush_only_field.name = "flush_only";
            if(!std_fields_data.flush_only.empty())
            {
              flush_only_field.value.push_back(std_fields_data.flush_only);
            }
          }
          send_req_field_info max_batch_field;
          {
            max_batch_field.name = "max_batch";
            if(!std_fields_data.max_batch.empty())
            {
              max_batch_field.value.push_back(std_fields_data.max_batch);
            }
          }

          if(batch_period_field.value.size() != 0)
            batch_spec_msg.fields.push_back(batch_period_field);
          if(flush_period_field.value.size() != 0)
            batch_spec_msg.fields.push_back(flush_period_field);
          if(flush_only_field.value.size() != 0)
            batch_spec_msg.fields.push_back(flush_only_field);
          if(max_batch_field.value.size() != 0)
            batch_spec_msg.fields.push_back(max_batch_field);
        }
        if(batch_spec_msg.fields.size() != 0) {
          batching_field.nested_msgs.push_back(batch_spec_msg);
        }
      }
      if(batching_field.nested_msgs.size() != 0 )
        sns_std_request_msg.fields.push_back(batching_field);

      send_req_field_info payload_field;
      {
        payload_field.name = "payload";
        if(!encoded_sns_request.empty()) {
          DEBUG_LOG(log_instance , "payload is not empty");
          payload_field.value.push_back(encoded_sns_request);
        }
      }
      if(payload_field.value.size() != 0)
        sns_std_request_msg.fields.push_back(payload_field);

    }
    request_field.nested_msgs.push_back(sns_std_request_msg);
  }
  client_req_msg.fields.push_back(request_field);


  string main_request_message("sns_client_request_msg");

  string main_request_encoded;
  if(USTA_RC_SUCCESS !=
      (rc=sensor_parser->formulate_request_msg(client_req_msg,
          main_request_encoded)))
  {
    ERROR_LOG(log_instance , "encoding main request failed");
    return rc;
  }
  pthread_mutex_lock(&proto_buf_mutex);
  rc = sensor_parser->parse_request_message(client_req_msg.msg_name, main_request_encoded.c_str(),
      main_request_encoded.size());
  if(USTA_RC_SUCCESS != rc)
  {
    ERROR_LOG(log_instance , "Error while Parse request message");
    pthread_mutex_unlock(&proto_buf_mutex);
    return rc;
  }

  USTA_LOGD("%s" , sensor_parser->get_request_json_string().c_str());

  pthread_mutex_lock(&file_mutex);
  if(log_instance->is_log_disabled() == false)
  {
    if(NULL != file_stream_ptr)
    {
      fprintf(file_stream_ptr, "%s" ,
          sensor_parser->get_request_json_string().c_str());
      fflush(file_stream_ptr);
    }
  }
  pthread_mutex_unlock(&file_mutex);
  pthread_mutex_unlock(&proto_buf_mutex);

  if(log_instance->is_log_disabled() == false)
  {
    char logging_module_name[] = "USTA";
    if(sensors_diag_log_obj != NULL)
      sensors_diag_log_obj->log_request_msg(
          main_request_encoded,data_type, logging_module_name);
  }
  ssc_stream_client_obj->send_request(main_request_encoded);

  DEBUG_LOG(log_instance , main_request_encoded);

  return rc;

}

usta_rc Sensor::stop_request(bool is_qmi_disable_api_enabled)
{
  usta_rc rc= USTA_RC_SUCCESS;

  if(is_qmi_disable_api_enabled == false)
  {
    DEBUG_LOG(log_instance , "is_qmi_disable_api_enabled is false");
    if(ssc_stream_client_obj != NULL)
    {
      delete ssc_stream_client_obj;
      ssc_stream_client_obj=NULL;
    }
    else
    {
      ERROR_LOG(log_instance , "Check!! sensor already in stop");

      return rc;
    }
  }
  else
  {
    send_disable_requst();
  }

  pthread_mutex_lock(&file_mutex);
  if(NULL !=  file_stream_ptr )
  {
    fclose(file_stream_ptr);
    file_stream_ptr = NULL;
  }
  pthread_mutex_unlock(&file_mutex);

  //used in testing x number of events
  event_count = 0;
  return rc;
}

void Sensor::send_disable_requst()
{
  DEBUG_LOG(log_instance , "is_qmi_disable_api_enabled is true");
  usta_rc rc= USTA_RC_SUCCESS;

  if(ssc_stream_client_obj == NULL)
    return;

  const EnumValueDescriptor *sns_client_disable_req_des =
      pool.FindEnumValueByName("SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ");

  if (NULL == sns_client_disable_req_des)
  {
    ERROR_LOG(log_instance , " SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ not found in descriptor pool");
    return;
  }

  send_req_msg_info client_req_msg;
  client_req_msg.msg_name = "sns_client_request_msg";
  client_req_msg.msgid = (to_string(sns_client_disable_req_des->number()));

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
    msg_id_field.value.push_back(to_string(sns_client_disable_req_des->number()));
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

    }
    request_field.nested_msgs.push_back(sns_std_request_msg);
  }
  client_req_msg.fields.push_back(request_field);

  //encoding the final request
  string sns_request_encoded;

  if(USTA_RC_SUCCESS !=
      (rc=sensor_parser->formulate_request_msg(client_req_msg,
          sns_request_encoded)))
  {
    ERROR_LOG(log_instance , "Encoding main request for attribute failed");
    return;
  }

  pthread_mutex_lock(&proto_buf_mutex);
  rc = sensor_parser->parse_request_message(client_req_msg.msg_name,sns_request_encoded.c_str(),
      sns_request_encoded.size());
  if(USTA_RC_SUCCESS != rc)
  {
    ERROR_LOG(log_instance , "Error while parse request message ");
    pthread_mutex_unlock(&proto_buf_mutex);
    return;
  }

  USTA_LOGD("Disable request message is  %s" , sensor_parser->get_request_json_string().c_str());

  pthread_mutex_lock(&file_mutex);
  if(log_instance->is_log_disabled() == false)
  {
    if(NULL != file_stream_ptr)
    {
      fprintf(file_stream_ptr, "%s" ,
          sensor_parser->get_request_json_string().c_str());
      fflush(file_stream_ptr);
    }
  }
  pthread_mutex_unlock(&file_mutex);

  if(log_instance->is_log_disabled() == false)
  {
    char logging_module_name[] = "USTA";
    if(sensors_diag_log_obj != NULL)
      sensors_diag_log_obj->log_request_msg(
          sns_request_encoded,data_type, logging_module_name);
  }
  pthread_mutex_unlock(&proto_buf_mutex);

  ssc_stream_client_obj->send_request(sns_request_encoded);

}

void Sensor::enableStreamingStatus()
{
  streaming_status_flag = true;
}

void Sensor::disableStreamingStatus()
{
  streaming_status_flag = false;
}
