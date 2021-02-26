/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_attributes.cpp
  @brief
   sensor_attributes class implementation.
============================================================================*/

#include "sensor_attributes.h"
#include "logging_util.h"

extern DescriptorPool pool;

sensor_attribute::sensor_attribute()
{
  attribute_count = 0;
  log_instance = usta_logging_util::get_instance();
}

sensor_attribute::~sensor_attribute()
{
  delete_memory();
}

void sensor_attribute::delete_memory()
{
  free(attribute_list.attributes);
  attribute_list.attributes = NULL;
}

usta_rc sensor_attribute::create_memory()
{
  attribute_list.attributes =
      (sensors_attribute *)calloc(sizeof(sensors_attribute) , attribute_count);
  if(NULL == attribute_list.attributes)
  {
    ERROR_LOG(log_instance , "attributes memory creation failed ");
    return USTA_RC_NO_MEMORY;
  }

  attribute_list.num_attrib = attribute_count;

  return USTA_RC_SUCCESS;
}


usta_rc sensor_attribute::store(string& payload)
{

  // now we will parse payload as a new message as we know the attr event  type

  const Descriptor* message_descriptor =
      pool.FindMessageTypeByName("sns_std_attr_event");
  if(message_descriptor == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::store message_descriptor is NULL");
    return USTA_RC_FAILED; // Once message_descriptor is NULL. Always it will be NULL for all attribute Events.
  }

  DynamicMessageFactory dmf;
  const Message* prototype = dmf.GetPrototype(message_descriptor);
  if(prototype == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::store prototype is NULL");
    return USTA_RC_FAILED; // Once prototype is NULL. Always it will be NULL for all attribute Events.
  }

  Message* message = prototype->New();
  if(message == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::store message is NULL");
    return USTA_RC_FAILED; // Once message is NULL. Always it will be NULL for all attribute Events.
  }

  message->ParseFromString(payload);

  const Message::Reflection* reflection = message->GetReflection();
  if(reflection == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::store reflection is NULL");
    return USTA_RC_FAILED; // Once reflection is NULL. Always it will be NULL for all attribute Events.
  }

  const FieldDescriptor* fd =
      message_descriptor->FindFieldByName("attributes");
  if(fd == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::store FindFieldByName is NULL");
    return USTA_RC_FAILED;
  }

  attribute_count = reflection->FieldSize(*message, fd);
  DEBUG_LOG(log_instance , "number of attributes are " + to_string(attribute_count));

  if( USTA_RC_SUCCESS != create_memory())
  {
    ERROR_LOG(log_instance, "sensor_attribute::store create_memory is failed");
    return USTA_RC_FAILED;
  }


  for (int attributeIndex =0 ; attributeIndex< attribute_count;
      attributeIndex++)
  {
    Message* sub_message =
        reflection->MutableRepeatedMessage(message, fd, attributeIndex);
    if(sub_message == NULL)
    {
      ERROR_LOG(log_instance, "sub_message is NULL . So Skipping this attribute");
      // Case occurs when there is a mismatch between HLOS and SLPI proto files w.r.t to attributes.
      continue; // Skip the current attribute and continue with another proper attributes.
    }

    const Descriptor* sub_msg_des = sub_message->GetDescriptor();
    if(sub_msg_des == NULL)
    {
      ERROR_LOG(log_instance, "sub_msg_des is NULL . So Skipping this attribute");
      // Case occurs when there is a mismatch between HLOS and SLPI proto files w.r.t to attributes.
      continue;// Skip the current attribute and continue with another proper attributes.
    }
    const FieldDescriptor* fd1 = sub_msg_des->FindFieldByName("attr_id");

    int32  attrib_id =
        (int32)sub_message->GetReflection()->GetInt32(*sub_message, fd1);
    attribute_list.attributes[attributeIndex].attribute_id = attrib_id;

    fd1 = sub_msg_des->FindFieldByName("value");
    Message* value_message =
        sub_message->GetReflection()->MutableMessage(sub_message, fd1);
    if(value_message == NULL)
    {
      ERROR_LOG(log_instance, "Value_message is NULL");
      // Case occurs when there is a mismatch between HLOS and SLPI proto files w.r.t to attributes.
      continue;// Skip the current attribute and continue with another proper attributes.
    }
    //storing each attribute in encoded form.
    // will be decoded on demand
    value_message->SerializeToString(
        &attribute_list.attributes[attributeIndex].encoded_attribute);

  }
  return USTA_RC_SUCCESS;
}

inline int sensor_attribute::get_count()
{
  return attribute_count;
}



bool sensor_attribute::is_available(string attrib_name)
{

  const EnumDescriptor* ed=
      pool.FindEnumTypeByName("sns_std_sensor_attr_id");
  if(ed == NULL)
  {
    ERROR_LOG(log_instance,"sns_std_sensor_attr_id is not found");
    // case where sns_std_sensor_attr_id is not found in the proto file.
    // Valid for All sensor attributes . Error handling is done at caller level.
    return false;
  }

  const EnumValueDescriptor *evd = ed->FindValueByName(attrib_name);
  if(evd == NULL)
  {
    ERROR_LOG(log_instance,"sns_std_sensor_attr_id is not found");
    // case where attrib_name is not found in the proto file.
    // Valid for All sensor attributes . Error handling is done at caller level.
    return false;
  }

  int32 attrib_id = evd->number();

  for (int attributeIndex =0 ; attributeIndex< attribute_count;
      attributeIndex++)
  {
    if(attrib_id == attribute_list.attributes[attributeIndex].attribute_id)
    {
      return true;
    }
  }
  return false;
}

/*Returns the sensors data type based on its attributes.
If something goes wrong, Simply returning NULL as its data type so that I&T and Dev team
can still use USTA and continue with its usage.*/
string sensor_attribute::get_dataType()
{
  string data_type;
  string empty_string = "";
  const EnumDescriptor* ed=
      pool.FindEnumTypeByName("sns_std_sensor_attr_id");
  if(ed == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::get_dataType FindEnumTypeByName is failed");
    return empty_string;
  }

  const EnumValueDescriptor *evd =ed->FindValueByName(sns_std_sensor_attrid_type);
  if(evd == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::get_dataType FindValueByName is failed");
    return empty_string;
  }

  int32 attrib_id = evd->number();

  for (int attributeIndex =0 ; attributeIndex< attribute_count;
      attributeIndex++)
  {
    if(attrib_id ==
        attribute_list.attributes[attributeIndex].attribute_id)
    {

      // decode the message using reflection
      DynamicMessageFactory dmf;
      const Descriptor* value_message_descriptor =
          pool.FindMessageTypeByName("sns_std_attr_value");
      if(value_message_descriptor == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_dataType value_message_descriptor is NULL");
        return empty_string;
      }

      const Message* prototype= dmf.GetPrototype(value_message_descriptor);
      if(prototype == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_dataType prototype is NULL");
        return empty_string;
      }

      Message* msg= prototype->New();
      if(msg == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_dataType msg is NULL");
        return empty_string;
      }

      msg->ParseFromString(
          attribute_list.attributes[attributeIndex].encoded_attribute);
      const Message::Reflection* reflection = msg->GetReflection();
      if(reflection == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_dataType reflection is NULL");
        return empty_string;
      }

      const Descriptor* message_descriptor = msg->GetDescriptor();
      if(message_descriptor == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_dataType message_descriptor is NULL");
        return empty_string;
      }

      const FieldDescriptor* fd =
          message_descriptor->FindFieldByName("values");
      if(fd == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_dataType fd is NULL");
        return empty_string;
      }

      // since it is a repeated field, will pass index 0
      Message* data_value_message =
          reflection->MutableRepeatedMessage(msg, fd, 0);
      if(data_value_message == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_dataType data_value_message is NULL");
        return empty_string;
      }

      string scratch;  // dummy
      vector<const FieldDescriptor*> fields;
      // populate fields vector with field descriptor of set fields
      data_value_message->GetReflection()->ListFields(*data_value_message,
          &fields);

      // since there is only one field
      string received_data_type = data_value_message->GetReflection()->
          GetStringReference(*data_value_message, fields[0], &scratch);
      // following is done to avoid NULL char from the received string
      data_type = (char*) received_data_type.c_str();

      break;
    }
  }


  return data_type;
}

/*Returns the sensors vendor type based on its attributes.
If something goes wrong, Simply returning NULL as its data type so that I&T and Dev team
can still use USTA and continue with its usage.*/
string sensor_attribute::get_vendor()
{
  string vendor;
  string empty_string = "";
  const EnumDescriptor* ed=
      pool.FindEnumTypeByName("sns_std_sensor_attr_id");
  if(ed == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::get_vendor FindEnumTypeByName is Fail");
    return empty_string;
  }

  const EnumValueDescriptor *evd = ed->FindValueByName(sns_std_sensor_attrid_vendor);
  if(evd == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::get_vendor FindValueByName is Fail");
    return empty_string;
  }

  int32 attrib_id = evd->number();


  for (int attributeIndex =0 ; attributeIndex< attribute_count;
      attributeIndex++)
  {
    if(attrib_id ==
        attribute_list.attributes[attributeIndex].attribute_id)
    {

      // decode the message using reflection
      DynamicMessageFactory dmf;
      const Descriptor* value_message_descriptor =
          pool.FindMessageTypeByName("sns_std_attr_value");
      if(value_message_descriptor == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_vendor value_message_descriptor is NULL");
        return empty_string;
      }

      const Message* prototype= dmf.GetPrototype(value_message_descriptor);
      if(prototype == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_vendor prototype is NULL");
        return empty_string;
      }

      Message* msg= prototype->New();
      if(msg == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_vendor msg is NULL");
        return empty_string;
      }

      msg->ParseFromString(
          attribute_list.attributes[attributeIndex].encoded_attribute);
      const Message::Reflection* reflection = msg->GetReflection();
      if(reflection == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_vendor reflection is NULL");
        return empty_string;
      }

      const Descriptor* message_descriptor = msg->GetDescriptor();
      if(message_descriptor == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_vendor message_descriptor is NULL");
        return empty_string;
      }

      const FieldDescriptor* fd =
          message_descriptor->FindFieldByName("values");
      if(fd == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_vendor FindFieldByName is failed");
        return empty_string;
      }
      // since it is a repeated field, will pass index 0
      Message* data_value_message =
          reflection->MutableRepeatedMessage(msg, fd, 0);
      if(data_value_message == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_vendor data_value_message is NULL");
        return empty_string;
      }

      string scratch;  // dummy
      vector<const FieldDescriptor*> fields;
      // populate fields vector with field descriptor of set fields
      data_value_message->GetReflection()->ListFields(*data_value_message,
          &fields);

      // since there is only one field
      string received_vendor = data_value_message->GetReflection()->
          GetStringReference(*data_value_message, fields[0], &scratch);

      // following is done to avoid NULL char from the received string
      vendor = (char*) received_vendor.c_str();
      break;
    }
  }
  return vendor;
}

vector<string> sensor_attribute::get_API()
{
  vector<string> api;
  const EnumDescriptor* ed=
      pool.FindEnumTypeByName("sns_std_sensor_attr_id");
  if(ed == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::get_API FindEnumTypeByName is fail");
    return api;
  }

  const EnumValueDescriptor *evd = ed->FindValueByName(sns_std_sensor_attrid_api);
  if(evd == NULL)
  {
    ERROR_LOG(log_instance, "sensor_attribute::get_API FindValueByName is fail");
    return api;
  }

  int32 attrib_id = evd->number();

  for (int attributeIndex = 0; attributeIndex< attribute_count;
      attributeIndex++)
  {
    if(attrib_id ==
        attribute_list.attributes[attributeIndex].attribute_id)
    {
      // decode the message using reflection
      DynamicMessageFactory dmf;
      const Descriptor* value_message_descriptor =
          pool.FindMessageTypeByName("sns_std_attr_value");
      if(value_message_descriptor == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_API value_message_descriptor is NULL");
        return api;
      }

      const Message* prototype= dmf.GetPrototype(value_message_descriptor);
      if(prototype == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_API prototype is NULL");
        return api;
      }

      Message* msg= prototype->New();
      if(msg == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_API msg is NULL");
        return api;
      }

      msg->ParseFromString(
          attribute_list.attributes[attributeIndex].encoded_attribute);
      const Message::Reflection* reflection = msg->GetReflection();
      if(reflection == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_API reflection is NULL");
        return api;
      }

      const Descriptor* message_descriptor = msg->GetDescriptor();
      if(message_descriptor == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_API message_descriptor is NULL");
        return api;
      }

      const FieldDescriptor* fd =
          message_descriptor->FindFieldByName("values");
      if(fd == NULL)
      {
        ERROR_LOG(log_instance, "sensor_attribute::get_API FindFieldByName is Fail");
        return api;
      }

      // we know there can be multiple proto files
      int count = reflection->FieldSize(*msg, fd);

      for (int i = 0; i<count; i++)
      {
        Message* data_value_message =
            reflection->MutableRepeatedMessage(msg, fd, i);
        if(data_value_message == NULL)
        {
          ERROR_LOG(log_instance, "sensor_attribute::get_API data_value_message is Fail");
          return api;
        }

        string scratch;
        vector<const FieldDescriptor*> fields;
        data_value_message->GetReflection()->ListFields(*data_value_message,
            &fields);
        string data_type = data_value_message->GetReflection()->
            GetStringReference(*data_value_message, fields[0], &scratch);

        data_type = (char*) data_type.c_str();  // to avoid NULL char
        api.push_back(data_type);
      }
      break;
    }
  }

  return api;
}


