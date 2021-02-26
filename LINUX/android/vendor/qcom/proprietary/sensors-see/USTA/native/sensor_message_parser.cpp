/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_message_parser.cpp

  @brief
  sensor_message_parser class implementation.
============================================================================*/

#include"sensor_message_parser.h"
#include"sensor_attributes.h"
#include "logging_util.h"
#include <stddef.h>

#define INVALID_INDEX -1
extern DescriptorPool pool;

MessageParser::MessageParser()
{
  protos_found = false;
  log_instance = usta_logging_util::get_instance();
  is_payload_started = false;
}
MessageParser::~MessageParser()
{

}


void MessageParser::get_protos_from_api(string protoName)
{
  const FileDescriptor *file_desc_proto = pool.FindFileByName(protoName);
  if (NULL == file_desc_proto)
  {
    ERROR_LOG(log_instance , " Error! requested proto file (" + protoName + ") not found in descriptor pool");
    return;
  }
  for(int i =0 ; i < file_desc_proto->dependency_count() ; i++)
  {
    const FileDescriptor *inner_proto_desc = file_desc_proto->dependency(i);
    if(inner_proto_desc == NULL)
      continue;

    string inter_proto_name = inner_proto_desc->name();
    if(inter_proto_name.compare("nanopb.proto") == 0)
    {
      continue;
    }
    if(std::find(std::begin(protos), std::end(protos), inter_proto_name) == std::end(protos))
    {
      protos.push_back(inter_proto_name);
      get_protos_from_api(inter_proto_name);
    }
  }

}
usta_rc MessageParser::set_api_info(vector<string>& api)
{

  usta_rc rc= USTA_RC_SUCCESS;
  request_msgs.clear();
  nested_msgs_pool.clear();
  if(api.size() != 0)
  {
    protos.push_back(api[0]);
    get_protos_from_api(api[0]);
  }
  // check if all the proto files are part of descriptor pool.
  //if none is found return error
  for(auto it=protos.begin();it!=protos.end();it++)
  {
    const FileDescriptor* file_desc = pool.FindFileByName(*it);
    if (NULL == file_desc)
    {
      ERROR_LOG(log_instance , " Error! requested proto file (" + (*it) + ") not found in descriptor pool");
    }
    else
    {
      protos_found |= true;
    }
  }

  if(! protos_found)
  {
    ERROR_LOG(log_instance , " Error None of the proto files found, returning");
    return USTA_RC_NO_PROTO;
  }
  generate_msg_look_up();

  rc = generate_request_msg_list();

  return rc;

}



usta_rc MessageParser::parse_request_message(string messagename, const char* data, int data_size)
{
  usta_rc rc= USTA_RC_SUCCESS;
  output_str.clear();
  rc=parse_message(messagename,data,data_size);
  request_str = output_str;
  return rc;
}

usta_rc MessageParser::parse_event_message(const char* data, int data_size,
    vector<payload_info> &out_sensor_payload_info)
{
  usta_rc rc= USTA_RC_SUCCESS;
  output_str.clear();
  sensor_payload_info.clear();
  is_payload_started = false;
  rc=parse_message("sns_client_event_msg",data,data_size);
  event_str = output_str;
  out_sensor_payload_info = sensor_payload_info;
  return rc;
}

string& MessageParser::get_request_json_string()
{
  return request_str;
}
string& MessageParser::get_event_json_string()
{
  return event_str;
}


usta_rc MessageParser::get_request_msg(vector<msg_info> &final_request_msg_out)
{
  if(!protos_found)
  {
    ERROR_LOG(log_instance , "Error!! no proto file found for get_request_msg");
    return USTA_RC_NO_PROTO;
  }
  if(request_msgs.size()==0)
  {
    ERROR_LOG(log_instance , "Error!! No request message found");
    return USTA_RC_NO_REQUEST_MSG;
  }

  for (auto msg_it=request_msgs.begin();msg_it!=request_msgs.end();msg_it++)
  {
    msg_info req_message;
    req_message.msg_name = (*msg_it).msg_name;
    req_message.msgid = (*msg_it).msgid;
    get_msg_fields(*msg_it, req_message);
    final_request_msg_out.push_back(req_message);
  }

  return USTA_RC_SUCCESS;

}
void MessageParser::get_nested_msg_fields(nested_msg_info &out_nested_msg)
{

  for(unsigned int nested_msg_index = 0 ; nested_msg_index < nested_msgs_pool.size(); nested_msg_index++ )
  {
    if(nested_msgs_pool[nested_msg_index].msg_name.compare(out_nested_msg.msg_name) == 0)
    {

      vector<field_info> fileds_in_nested_msg;

      for(unsigned int field_index = 0 ; field_index < nested_msgs_pool[nested_msg_index].fields.size(); field_index++)
      {
        // for each field in the nested message
        field_info current_field;
        memset(&current_field, 0, sizeof(current_field));
        field_info each_field = nested_msgs_pool[nested_msg_index].fields[field_index];
        if(each_field.is_user_defined_msg == true)
        {
          /*uesr defined message / nested message / dataype*/
          current_field.data_type = each_field.data_type;
          current_field.name = each_field.name;
          current_field.label = each_field.label;
          current_field.is_user_defined_msg = each_field.is_user_defined_msg;

          vector<nested_msg_info> nested_msgs_vector;
          nested_msg_info nested_msg;
          nested_msg.msg_name = each_field.data_type;
          get_nested_msg_fields(nested_msg);
          nested_msgs_vector.push_back(nested_msg);
          current_field.nested_msgs = nested_msgs_vector;
        }
        else
        {
          current_field.data_type = each_field.data_type;
          current_field.name = each_field.name;
          current_field.label = each_field.label;
          current_field.is_user_defined_msg = each_field.is_user_defined_msg;

          current_field.has_default = each_field.has_default;
          if(each_field.has_default)
            current_field.default_val = each_field.default_val;

          current_field.is_enum = each_field.is_enum;
          if(each_field.is_enum)
            current_field.enum_list = each_field.enum_list;
        }
        fileds_in_nested_msg.push_back(current_field);
      }
      out_nested_msg.fields = fileds_in_nested_msg;
      break;
    }
  }
}

void MessageParser::get_msg_fields(msg_info &in_msg_info, msg_info &final_msg_info)
{
  // enlist field information
  for(unsigned int field_index = 0 ; field_index < in_msg_info.fields.size(); field_index++)
  {
    field_info final_field;
    memset(&final_field, 0, sizeof(final_field));
    field_info each_field = in_msg_info.fields[field_index];
    if(each_field.is_user_defined_msg == true)
    {
      /*uesr defined message / nested message / dataype*/
      final_field.data_type = each_field.data_type;
      final_field.name = each_field.name;
      final_field.label = each_field.label;
      final_field.is_user_defined_msg = each_field.is_user_defined_msg;

      vector<nested_msg_info> nested_msgs_vector;
      nested_msg_info nested_msg;
      nested_msg.msg_name = each_field.data_type;
      get_nested_msg_fields(nested_msg);
      nested_msgs_vector.push_back(nested_msg);
      final_field.nested_msgs = nested_msgs_vector;

    }
    else
    {
      final_field = each_field;
    }
    final_msg_info.fields.push_back(final_field);
  }
}

usta_rc MessageParser::formulate_request_msg(send_req_msg_info& req_msg,string& request_string)
{
  usta_rc rc = USTA_RC_SUCCESS;

  const Descriptor* message_descriptor=
      pool.FindMessageTypeByName(req_msg.msg_name);
  if(NULL == message_descriptor)
  {
    ERROR_LOG(log_instance , " Error!! message " + req_msg.msg_name + " not found in descriptor pool,cannot formulate request");
    return USTA_RC_DESC_FAILED;
  }
  DynamicMessageFactory dmf;


  const Message* prototype = dmf.GetPrototype(message_descriptor);
  Message* message = prototype->New();
  if(USTA_RC_SUCCESS !=(rc=InputMessage(message,req_msg.fields)))
  {
    ERROR_LOG(log_instance , " formulation for message " + req_msg.msg_name + " failed");
    return rc;
  }

  message->SerializeToString(&request_string);
  DEBUG_LOG(log_instance , " encoded input message for " + req_msg.msg_name + " is generated");
  return USTA_RC_SUCCESS;

}

usta_rc MessageParser::InputMessage(Message* message,vector<send_req_field_info>& fields)
{
  const Message::Reflection* reflection = message->GetReflection();
  const Descriptor* descriptor = message->GetDescriptor();
  if(NULL == descriptor)
  {
    DEBUG_LOG(log_instance , "descriptor was not found ");
    /* Basically we are continuing for next field or next message if current one was not found in the descriptor pool. Checking Null value to avoid the crash below if any occures  */
    return USTA_RC_SUCCESS;
  }
  for (int i = 0; i < descriptor->field_count();i++)
  {
    if(USTA_RC_SUCCESS != InputFieldValue(message, reflection,
        descriptor->field(i),fields))
    {
      return USTA_RC_FAILED;
    }
  }

  return USTA_RC_SUCCESS;

}

int MessageParser::get_required_field_index(const FieldDescriptor* field,
    vector<send_req_field_info>& user_defined_fields)
{
  if(field == NULL)
    return INVALID_INDEX;
  if(user_defined_fields.empty())
    return INVALID_INDEX;

  for(unsigned int field_index = 0 ; field_index < user_defined_fields.size() ; field_index++)
  {
    if(user_defined_fields[field_index].name.compare(field->name()) == 0)
      return field_index;
  }
  return INVALID_INDEX; // indicates, field not found from user inputs
}

usta_rc MessageParser::InputFieldValue(Message* message,
    const Message::Reflection* reflection,
    const FieldDescriptor* field,
    vector<send_req_field_info>& user_defined_fields)
{

  int user_defined_field_index = get_required_field_index(field, user_defined_fields);
  if(user_defined_field_index == -1)
  {
    return USTA_RC_SUCCESS; // Just continuing with next field, This is to avoid making optional as mandatory.
  }
  send_req_field_info current_user_defined_field = user_defined_fields[user_defined_field_index];

  vector<string> input_str;
  input_str = current_user_defined_field.value;
  if((field->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE)&& input_str.size() ==0)
  {
    return USTA_RC_SUCCESS;
  }

  switch (field->cpp_type())
  {

  case FieldDescriptor::CPPTYPE_INT32:
  {


    if(field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < input_str.size();repeat_index++)
        reflection->AddInt32(message, field,stoi(input_str[repeat_index]));
    }
    else
    {
      reflection->SetInt32(message, field, stoi(input_str[0]));
    }
    break;
  }
  case FieldDescriptor::CPPTYPE_UINT32:
  {

    if(field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < input_str.size();repeat_index++)
        reflection->AddUInt32(message, field,stoul(input_str[repeat_index]));
    }
    else
    {
      reflection->SetUInt32(message, field, stoul(input_str[0]));
    }
    break;
  }
  case FieldDescriptor::CPPTYPE_INT64:
  {

    if(field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < input_str.size();repeat_index++)
        reflection->AddInt64(message, field,stoll(input_str[repeat_index]));
    }
    else
    {
      if(field->containing_oneof())
      {
        int field_index = field->index_in_oneof();
        reflection->ClearOneof(message, field->containing_oneof());
        reflection->SetInt64(message, field, stoll(input_str[0]));
      }
      else
      {
        reflection->AddInt64(message, field, stoll(input_str[0]));
      }
    }

    break;
  }
  case FieldDescriptor::CPPTYPE_UINT64:
  {


    if(field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < input_str.size();repeat_index++)
        reflection->AddUInt64(message, field,stoull(input_str[repeat_index],NULL,0));
    }
    else
    {
      reflection->SetUInt64(message, field, stoull(input_str[0],NULL,0));
    }
    break;

  }
  case FieldDescriptor::CPPTYPE_FLOAT:
  {

    if(field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < input_str.size();repeat_index++)
        reflection->AddFloat(message, field,stof(input_str[repeat_index]));
    }
    else
    {
      if(field->containing_oneof())
      {
        int field_index = field->index_in_oneof();
        reflection->ClearOneof(message, field->containing_oneof());
        reflection->SetFloat(message, field, stof(input_str[0]));
      }
      else
      {
        reflection->SetFloat(message, field, stof(input_str[0]));
      }
    }
    break;

  }
  case FieldDescriptor::CPPTYPE_BOOL:
  {

    bool val;

    if(field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < input_str.size();repeat_index++)
      {
        if (!(input_str[repeat_index].compare("false")) ||
            !(input_str[repeat_index].compare("FALSE")) ||
            !(input_str[repeat_index].compare("0")))
        {
          val = 0;
        }
        else
        {
          val = 1;
        }

        reflection->AddBool(message, field,val);
      }
    }
    else
    {
      if (!(input_str[0].compare("false")) ||
          !(input_str[0].compare("FALSE")) ||
          !(input_str[0].compare("0")))
      {
        val = 0;
      }
      else
      {
        val = 1;
      }
      reflection->SetBool(message, field, val);
    }

    break;
  }
  case FieldDescriptor::CPPTYPE_DOUBLE:
  {
    if(field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < input_str.size();repeat_index++)
        reflection->AddDouble(message, field,stod(input_str[repeat_index]));
    }
    else
    {
      reflection->SetDouble(message, field, stod(input_str[0]));
    }

    break;
  }

  case FieldDescriptor::CPPTYPE_ENUM:
  {

    const EnumDescriptor* ed = field->enum_type();
    if(field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < input_str.size();repeat_index++)
      {
        if (!ed->FindValueByName(input_str[repeat_index]))
        {
          ERROR_LOG(log_instance , "Error!! enum name " + input_str[repeat_index] + " in not member of enum " + ed->name());
          return USTA_RC_FAILED;
          break;
        }

        reflection->AddEnum(message, field,ed->FindValueByName(input_str[repeat_index]));
      }
    }
    else
    {
      if (!ed->FindValueByName(input_str[0]))
      {
        ERROR_LOG(log_instance , "Error!! enum name " + input_str[0] + " in not member of enum " + ed->name());
        return USTA_RC_FAILED;
        break;
      }
      reflection->SetEnum(message, field, ed->FindValueByName(input_str[0]));
    }
    break;
  }
  case FieldDescriptor::CPPTYPE_MESSAGE:
  {
    if (field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < current_user_defined_field.nested_msgs.size();repeat_index++)
      {

        int size = reflection->FieldSize(*message, field);
        reflection->AddMessage(message, field);

        for(int i = 0 ; i < size ; i++)
        {
          reflection->AddMessage(message, field);
        }

        if (USTA_RC_SUCCESS !=
            InputMessage(reflection->MutableRepeatedMessage(message, field, repeat_index),
                current_user_defined_field.nested_msgs[repeat_index].fields))
          return USTA_RC_FAILED;

      }
      break;
    }
    else
    {
      if (USTA_RC_SUCCESS !=
          InputMessage(reflection->MutableMessage(message, field),
              current_user_defined_field.nested_msgs[0].fields))
        return USTA_RC_FAILED;

    }

    break;

  }

  case FieldDescriptor::CPPTYPE_STRING:
  {
    if(field->is_repeated())
    {
      for(unsigned int repeat_index = 0 ; repeat_index < input_str.size();repeat_index++)
      {
        if(field->type() == FieldDescriptor::TYPE_STRING)
          input_str[repeat_index] = input_str[repeat_index] + '\0' ;
        reflection->AddString(message, field,input_str[repeat_index]);
      }
    }
    else
    {
      if(field->type() == FieldDescriptor::TYPE_STRING)
        input_str[0] = input_str[0] + '\0' ;

      if(field->containing_oneof())
      {
        int field_index = field->index_in_oneof();
        reflection->ClearOneof(message, field->containing_oneof());
        reflection->SetString(message, field, input_str[0]);
      }
      else
      {
        reflection->SetString(message, field, input_str[0]);
      }

    }
    break;

  }
  default:
    ERROR_LOG(log_instance , "Error!! type not valid for request message");
    break;
  }

  return USTA_RC_SUCCESS;


}

// defining pvt member functions
usta_rc MessageParser::	generate_request_msg_list()
{
  if (msg_look_up.size() == 0)
  {
    DEBUG_LOG(log_instance , " Error !! msg_look_up size is 0.."
        " generate_request_msg_list failed");
    return USTA_RC_FAILED;
  }
  int index = 0;
  nested_msg_index = 0;

  for (auto it = msg_look_up.begin(); it != msg_look_up.end(); it++)
  {
    if((it->first>=512) && (it->first<=767))
    {
      msg_info new_message;
      new_message.msgid = to_string(it->first);
      DEBUG_LOG(log_instance , "msg ID is " + to_string(it->first));
      new_message.msg_name = msg_look_up[it->first];
      request_msgs.push_back(new_message);
      usta_rc ret = parse_request_msg_fields(request_msgs, msg_look_up[it->first], index);
      if(USTA_RC_SUCCESS != ret)
      {
        if(USTA_RC_DESC_FAILED == ret)
        {
          /*USTA_RC_DESC_FAILED means, message name was not found in the descriptor pool even though message id was present. This is the case with on-change sensors.
          Added this condition to support on-change sensors from USTA*/
          index++;
        }
        else
          return ret;
      }
      else
      {
        index++;
      }
    }
  }
  DEBUG_LOG(log_instance , " number of msgs found is " + to_string(request_msgs.size()));

  if (request_msgs.size()==0) return USTA_RC_NO_REQUEST_MSG;

  DEBUG_LOG(log_instance , " PRINTING THE FIELDS OF EACH MSG");

  for (unsigned int i = 0; i < request_msgs.size(); i++)
  {
    DEBUG_LOG(log_instance , "Details of msg " + to_string(i));
    msg_info each_msg = request_msgs[i];
    DEBUG_LOG(log_instance , " Name of message " + each_msg.msg_name + " msg id enum string is " + each_msg.msgid );
    print_msg_fileds(each_msg);
  }

  return USTA_RC_SUCCESS;

}

void MessageParser::print_msg_fileds(msg_info &each_msg)
{
  DEBUG_LOG(log_instance , " number of fields are " +  to_string(each_msg.fields.size()));
  DEBUG_LOG(log_instance , " details of fields are");
  for (unsigned int j = 0; j < each_msg.fields.size(); j++)
  {
    field_info each_field = each_msg.fields[j];
    if(each_field.is_user_defined_msg == true)
    {
      for(unsigned int nested_msg_index = 0 ; nested_msg_index < nested_msgs_pool.size(); nested_msg_index++ )
      {
        if(nested_msgs_pool[nested_msg_index].msg_name.compare(each_field.data_type) == 0)
        {
          DEBUG_LOG(log_instance , " " + to_string(j) + ") + ,Name of nested message " + each_field.name + " , label " + each_field.label);
          print_msg_fileds(nested_msgs_pool[nested_msg_index]);
          break;
        }
      }
    }
    else
    {
      DEBUG_LOG(log_instance , " " + to_string(j) + ") name " + each_field.name + ", data_type " + each_field.data_type + ",label  " +each_field.label + ", is_enum " + to_string(each_field.is_enum));
      DEBUG_LOG(log_instance , " has_default " + to_string(each_field.has_default) + " default val " + each_field.default_val);
      if (each_field.is_enum)
      {
        DEBUG_LOG(log_instance , " field " + to_string(j)+ " is enum size " + to_string(each_field.enum_list.size()));
        for (unsigned int k = 0; k < each_field.enum_list.size(); k++)
        {
          enum_info each_enum = each_field.enum_list[k];
          DEBUG_LOG(log_instance , each_enum.name + " : " + to_string(each_enum.value));
        }
      }
    }

  }
}

usta_rc MessageParser::parse_request_msg_fields(vector<msg_info> &msg_vector_type,
    const string& message,int index)
{
  const Descriptor* desc = pool.FindMessageTypeByName(message);
  if (NULL == desc)
  {
    DEBUG_LOG(log_instance , " Error ! message " + message + " not found in descriptor pool "
        "parse_request_msg_fields failed");
    return USTA_RC_DESC_FAILED;
  }
  msg_info& this_message = msg_vector_type[index];  // get reference of message

  for (int i = 0; i < desc->field_count(); i++)
  {
    field_info field;
    memset(&field, 0, sizeof(field));
    const FieldDescriptor* fd = desc->field(i);

    if (fd->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE)
    {
      field.name = fd->name();
      field.data_type = fd->message_type()->name();
      field.is_user_defined_msg = true;
      field.label = fd->is_required() ?
          "required" : (fd->is_optional() ? "optional" : "repeated");
      this_message.fields.push_back(field);

      msg_info new_nested_message;
      new_nested_message.msg_name = fd->message_type()->name();
      nested_msgs_pool.push_back(new_nested_message);
      parse_request_msg_fields(nested_msgs_pool, fd->message_type()->name(), nested_msg_index++);
    }
    else
    {
      field.name = fd->name();
      field.data_type = fd->CppTypeName(fd->cpp_type());
      field.is_user_defined_msg = false;
      field.label = fd->is_required() ?
          "required" : (fd->is_optional() ? "optional" : "repeated");
      field.has_default = fd->has_default_value() ? 1 : 0;
      field.is_enum = false;

      if (field.has_default)
      {
        switch (fd->cpp_type())
        {
        case FieldDescriptor::CPPTYPE_INT32:
          field.default_val = to_string(fd->default_value_int32());
          break;

        case FieldDescriptor::CPPTYPE_UINT32:
          field.default_val = to_string(fd->default_value_uint32());
          break;

        case FieldDescriptor::CPPTYPE_INT64:
          field.default_val = to_string(fd->default_value_int64());
          break;

        case FieldDescriptor::CPPTYPE_UINT64:
          field.default_val = to_string(fd->default_value_uint32());
          break;

        case FieldDescriptor::CPPTYPE_FLOAT:
          field.default_val = to_string(fd->default_value_float());
          break;

        case FieldDescriptor::CPPTYPE_BOOL:
          field.default_val = to_string(fd->default_value_bool());
          break;

        case FieldDescriptor::CPPTYPE_DOUBLE:
          field.default_val = to_string(fd->default_value_double());
          break;

        case FieldDescriptor::CPPTYPE_STRING:
          field.default_val = fd->default_value_string();
          break;

        case FieldDescriptor::CPPTYPE_ENUM:
        {
          const EnumValueDescriptor* evd = fd->default_value_enum();
          field.default_val = evd->name();
          break;
        }

        default:
          break;

        }
      }

      if (fd->cpp_type() == FieldDescriptor::CPPTYPE_ENUM)
      {
        field.is_enum = true;
        //enum_info* en = (enum_info*)calloc(sizeof(enum_info),1);
        //field->enum_list.push_back(*en);
        const EnumDescriptor* ed = fd->enum_type();
        for (int j = 0; j < ed->value_count(); j++)
        {
          enum_info en;
          const EnumValueDescriptor* evd = ed->value(j);
          en.name = evd->name();
          en.value = evd->number();
          field.enum_list.push_back(en);
        }
      }
      this_message.fields.push_back(field);
    }
  }
  return USTA_RC_SUCCESS;
}


usta_rc MessageParser::generate_msg_look_up()
{
  msg_look_up.clear();
  for (auto it = protos.begin(); it != protos.end(); it++)
  {
    string enum_name = *it;
    enum_name.replace(enum_name.length() - 6, 6, "_msgid");
    const EnumDescriptor* enumDescriptor = pool.FindEnumTypeByName(enum_name);
    if (enumDescriptor == NULL)
    {
      DEBUG_LOG(log_instance , "Check!! No enum found in " + (*it));
      continue;
    }
    for (int e = 0; e < enumDescriptor->value_count(); e++)
    {
      string message;
      const EnumValueDescriptor* enumValueDescriptor =
          enumDescriptor->value(e);
      int id = enumValueDescriptor->number();

      message = enumValueDescriptor->name();
      transform(message.begin(), message.end(), message.begin(), ::tolower);
      // if enum is SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
      // msg will be sns_std_sensor_config
      size_t pos = message.find(enum_name + "_");
      if (pos != 0)
      {
        DEBUG_LOG(log_instance , " convention not followed for enum");
      }
      else
      {
        message.erase(0, enum_name.length() + 1);
        if (msg_look_up.count(id) == 1)
        {
          DEBUG_LOG(log_instance , " Error!! Duplicate id " + to_string(id) + " found. overwriting");
        }
        msg_look_up.insert(pair<int, string>(id, message));
        DEBUG_LOG(log_instance , " msg found is " + message + " in file " + (*it) + ", id is " + to_string(id));
      }
    }
  }

  if (msg_look_up.size()!=0)
  {
    DEBUG_LOG(log_instance , " printing all the msgs and their msg id");

    for (auto it = msg_look_up.begin(); it != msg_look_up.end(); it++)
    {
      DEBUG_LOG(log_instance , to_string(it->first) + " : " +  it->second.c_str());
    }
  }
  return USTA_RC_SUCCESS;
}


usta_rc MessageParser::parse_message(string message_name,const char* data,
    int data_size)
{
  const Descriptor* message_descriptor =
      pool.FindMessageTypeByName(message_name);
  if (NULL == message_descriptor)
  {
    ERROR_LOG(log_instance , "message " + message_name + " not found in decriptor pool");
    return USTA_RC_DESC_FAILED;
  }
  DynamicMessageFactory dmf;

  const Message* prototype = dmf.GetPrototype(message_descriptor);

  output_str += "\n";
  output_str += "\"" + message_name + "\"";

  Message* message = prototype->New();

  bool ret = message->ParseFromArray(data, data_size);
  if(ret != true)
  {
    DEBUG_LOG(log_instance , "Required fields are missing at encoding level");
    return USTA_RC_REQUIIRED_FILEDS_NOT_PRESENT;
  }

  space = 0;
  OutputNameValueSeparator();
  OutputMessageStart();
  OutputMessage(message);
  OutputMessageEnd();


  return USTA_RC_SUCCESS;
}

// utilities for parsing message
void MessageParser::OutputMessage(Message* message)
{
  if(message == NULL)
    return;

  const Message::Reflection* reflection = message->GetReflection();
  if(NULL == reflection)
  {
    ERROR_LOG(log_instance , "Error!! reflection is Null");
    return;
  }
  vector<const FieldDescriptor*> fields;
  reflection->ListFields(*message, &fields);

  int i = 0;
  for (auto field_it = fields.begin();
      field_it != fields.end(); ++field_it, ++i)
  {

    if (i > 0)
      OutputFieldSeparator();

    OutputField(message, reflection, *field_it);
  }
}


void MessageParser::OutputField(Message* message,
    const Message::Reflection* reflection, const FieldDescriptor* field)
{
  if(NULL == message){
    return;
  }
  if(NULL == reflection){
    return;
  }
  if(NULL == field){
    return ;
  }
  payload_info payload_field;
  if(is_payload_started == true)
  {
    payload_field.name = field->name();
  }
  if (field->is_repeated() &&
      field->cpp_type() != FieldDescriptor::CPPTYPE_STRING &&
      field->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE)
  {
    vector<string> repeated_values;
    OutputRepeatedField(message, reflection, field , repeated_values);

    if(is_payload_started == true)
    {
      payload_field.value = repeated_values;
      sensor_payload_info.push_back(payload_field);
    }
    return;
  }

  int count = 0;
  if (field->is_repeated())
  {
    count = reflection->FieldSize(*message, field);
    if(message->GetTypeName().compare("sns_std_attr_event") == 0)
      DEBUG_LOG(log_instance , "Number of attributes is " + to_string(count));
  }
  else if (reflection->HasField(*message, field))
  {
    count = 1;
  }
  OutputFieldName(field);
  OutputNameValueSeparator();
  if (field->is_repeated()) OutputRepeatedFieldStart();

  for (int j = 0; j < count; ++j)
  {
    if(field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE)
    {
      Message* sub_message = field->is_repeated() ?
          (reflection->MutableRepeatedMessage(message, field, j))
          : (reflection->MutableMessage(message, field));

      OutputMessageStart();
      OutputMessage(sub_message);
      OutputMessageEnd();
    }
    else
    {
      string value;
      OutputFieldValue(message, reflection, field, j, value);
      if(is_payload_started == true)
      {
        vector<string> values ;
        values.push_back(value);
        payload_field.value = values;
        sensor_payload_info.push_back(payload_field);
      }
    }
    if (j != count - 1)
      OutputFieldSeparator();
  }
  if (field->is_repeated())
    OutputRepeatedFieldEnd();
}

/*
MessageParser::OutputFieldValue is executed per field.
If something geoes wrong with google proto buffer encoding/parsing,
Simply we are moving to next field item in the proto file instead of
Force crash the entire application.
*/
void MessageParser::OutputFieldValue(Message* message,
    const Message::Reflection* reflection, const FieldDescriptor* field,
    int index , string &out_value)
{
  if(NULL == message)
  {
    ERROR_LOG(log_instance, "MessageParser::OutputFieldValue message is NULL ");
    return;
  }
  if(NULL == reflection)
  {
    ERROR_LOG(log_instance, "MessageParser::OutputFieldValue reflection is NULL ");
    return;
  }
  if(NULL == field)
  {
    ERROR_LOG(log_instance, "MessageParser::OutputFieldValue field is NULL ");
    return ;
  }

  switch (field->cpp_type())
  {

  case FieldDescriptor::CPPTYPE_INT32:
  {
    int32 msg;
    msg = (field->is_repeated()
        ? reflection->GetRepeatedInt32(*message, field, index)
            : reflection->GetInt32(*message, field));
    if (message->GetTypeName().compare("sns_std_attr") == 0)
    {
      const EnumDescriptor* ed=
          pool.FindEnumTypeByName("sns_std_sensor_attr_id");
      if(ed == NULL)
      {
        ERROR_LOG(log_instance, "MessageParser::OutputFieldValue sns_std_sensor_attr_id not found ");
        return;
      }
      const EnumValueDescriptor* evd = ed->FindValueByNumber(msg);
      if(evd != NULL)
      {
        output_str += "\"";
        output_str += evd->name();
        DEBUG_LOG(log_instance , "Attribute ID is " + evd->name());
        output_str += "\"";
      }
      else
      {
        DEBUG_LOG(log_instance , "Skipping enum ID " + to_string(msg));
      }
    }
    else
    {
      output_str += to_string(msg);
      if(is_payload_started == true)
        out_value = to_string(msg);
    }
    break;
  }
  case FieldDescriptor::CPPTYPE_UINT32:
  {
    uint32 msg;
    msg = field->is_repeated() ?
        reflection->GetRepeatedUInt32(*message, field, index)
        : reflection->GetUInt32(*message, field);

    if (field->name().compare("msg_id") == 0)
    {
      current_msg_id=msg;
    }

    output_str += to_string(msg);
    if(is_payload_started == true)
      out_value = to_string(msg);

    break;
  }
  case FieldDescriptor::CPPTYPE_INT64:
  {
    output_str += to_string(field->is_repeated()
        ? reflection->GetRepeatedInt64(*message, field, index)
            : reflection->GetInt64(*message, field));

    if(is_payload_started == true)
      out_value = to_string(field->is_repeated()
          ? reflection->GetRepeatedInt64(*message, field, index)
              : reflection->GetInt64(*message, field));

    break;
  }
  case FieldDescriptor::CPPTYPE_UINT64:
  {

    uint64 value = field->is_repeated()?
        reflection->GetRepeatedUInt64(*message, field, index)
        : reflection->GetUInt64(*message, field);

    // Handle special formatting for SUIDs

    if ((message->GetTypeName().compare("sns_std_suid") == 0) &&
        ((field->name().compare("suid_low") == 0) ||
            (field->name().compare("suid_high") == 0)))
    {
      stringstream ss;
      ss << std::hex << value;
      output_str += "\"";
      output_str += "0x" + ss.str();
      output_str += "\"";
    }
    else
    {
      output_str += to_string(value);
      if(is_payload_started == true)
        out_value = to_string(value);
    }

    break;
  }
  case FieldDescriptor::CPPTYPE_FLOAT:
  {

    output_str += to_string(field->is_repeated()
        ? reflection->GetRepeatedFloat(*message, field, index)
            : reflection->GetFloat(*message, field));

    if(is_payload_started == true)
      out_value = to_string(field->is_repeated()
          ? reflection->GetRepeatedFloat(*message, field, index)
              : reflection->GetFloat(*message, field));

    break;
  }
  case FieldDescriptor::CPPTYPE_BOOL:
  {
    output_str += to_string(field->is_repeated()
        ? reflection->GetRepeatedBool(*message, field, index)
            : reflection->GetBool(*message, field));

    if(is_payload_started == true)
      out_value = to_string(field->is_repeated()
          ? reflection->GetRepeatedBool(*message, field, index)
              : reflection->GetBool(*message, field));
    break;
  }
  case FieldDescriptor::CPPTYPE_DOUBLE:
  {
    output_str += to_string(field->is_repeated()
        ? reflection->GetRepeatedDouble(*message, field, index)
            : reflection->GetDouble(*message, field));

    if(is_payload_started == true)
      out_value = to_string(field->is_repeated()
          ? reflection->GetRepeatedDouble(*message, field, index)
              : reflection->GetDouble(*message, field));

    break;
  }
  case FieldDescriptor::CPPTYPE_STRING: {

    string scratch;
    const string& value = field->is_repeated() ?
        reflection->GetRepeatedStringReference(*message, field, index,
            &scratch)
            : reflection->GetStringReference(*message, field, &scratch);

    if (field->name().compare("payload") == 0)
    {
      string message_type;

      const EnumValueDescriptor * attrib_req_evd =
          pool.FindEnumValueByName("SNS_STD_MSGID_SNS_STD_ATTR_REQ");

      const EnumValueDescriptor * attrib_event_evd =
          pool.FindEnumValueByName("SNS_STD_MSGID_SNS_STD_ATTR_EVENT");

      if(attrib_req_evd != NULL && current_msg_id== attrib_req_evd->number())
      {
        message_type = "sns_std_attr_req";
      }
      else if (attrib_event_evd != NULL && current_msg_id == attrib_event_evd->number())
      {
        message_type = "sns_std_attr_event";
      }
      else
      {
        // current_msg_id must be present in look up
        if (msg_look_up.count(current_msg_id)!= 0)
          message_type = msg_look_up[current_msg_id];
      }
      is_payload_started = true;
      if (message_type.length() == 0)
      {
        UnknownFieldSet embedded_unknown_fields;
        if (!value.empty() && embedded_unknown_fields.ParseFromString(value))
        {

          OutputMessageStart();
          OutputUnknownFields(embedded_unknown_fields);
          OutputMessageEnd();
        }
      }
      else
        OutputMessageOpaquePayload(message_type, value);

    }
    else
    {
      if (field->type() == FieldDescriptor::TYPE_STRING)
      {
        char* p = (char*)value.c_str();
        output_str += "\"";
        output_str += p;
        output_str += "\"";
        if(is_payload_started == true)
          out_value = p;

      }
      else if (field->type() == FieldDescriptor::TYPE_BYTES)
      {

        if (!message->GetTypeName().compare(
            "sns_std_sensor_config_event.sns_std_sensor_config_payload"))
        {
          // only for physical sensors hadrcoding the message name
          OutputMessageOpaquePayload("sns_std_sensor_physical_config", value);
        }
        else
        {
          UnknownFieldSet embedded_unknown_fields;
          if (!value.empty() &&
              embedded_unknown_fields.ParseFromString(value))
          {

            OutputMessageStart();
            OutputUnknownFields(embedded_unknown_fields);
            OutputMessageEnd();
          }
        }
      }
    }
    break;
  }
  case FieldDescriptor::CPPTYPE_ENUM:
  {
    const EnumValueDescriptor* enum_desc = field->is_repeated()?reflection->GetRepeatedEnum(*message, field, index):reflection->GetEnum(*message, field);
    if(enum_desc == NULL)
    {
      ERROR_LOG(log_instance, "MessageParser::OutputFieldValue enum_desc is NULL ");
      return;
    }

    output_str += "\"";

    if (enum_desc != NULL)
    {
      output_str += enum_desc->name();
      if(is_payload_started == true)
        out_value = enum_desc->name();
    }
    else
    {
      output_str += to_string(enum_desc->number());
      if(is_payload_started == true)
        out_value = to_string(enum_desc->number());
    }
    output_str += "\"";
    // Handle case when enum value is not in range
    break;
  }
  case FieldDescriptor::CPPTYPE_MESSAGE:
  {
    OutputMessage(field->is_repeated()
        ? reflection->MutableRepeatedMessage(message, field, index)
            : reflection->MutableMessage(message, field));
    break;
  }
  default:
    break;
  }
}

/*
MessageParser::OutputRepeatedField is executed per repeated field.
If something geoes wrong with google proto buffer encoding/parsing,
Simply we are moving to next field item in the messge instead of
Force crash the entire application.
*/
void MessageParser::OutputRepeatedField(Message* message,
    const Message::Reflection* reflection, const FieldDescriptor* field,
    vector<string> &repeated_values)
{
  if(message == NULL)
  {
    ERROR_LOG(log_instance, "MessageParser::OutputRepeatedField message is NULL");
    return;
  }
  if(reflection == NULL)
  {
    ERROR_LOG(log_instance, "MessageParser::OutputRepeatedField reflection is NULL");
    return;
  }
  if(field == NULL)
  {
    ERROR_LOG(log_instance, "MessageParser::OutputRepeatedField field is NULL");
    return;
  }

  OutputFieldName(field);

  OutputNameValueSeparator();

  OutputRepeatedFieldStart();

  int size = reflection->FieldSize(*message, field);
  for (int i = 0; i < size; i++)
  {
    if (i > 0)
      OutputFieldSeparator();
    string value;
    OutputFieldValue(message, reflection, field, i, value);
    repeated_values.push_back(value);
  }
  OutputRepeatedFieldEnd();
}

void MessageParser::OutputUnknownFields(const UnknownFieldSet& unknown_fields)
{
  output_str +="\"unknown_fields\"";
  OutputNameValueSeparator();
  output_str += to_string(unknown_fields.field_count());
  //return SetErrorMessage("Unknown fields are NOT supported!");

  for (int i = 0; i < unknown_fields.field_count(); i++)
  {

    const UnknownField& field = unknown_fields.field(i);
    OutputFieldSeparator();
    output_str += "\"field_" + to_string(field.number()) + "\"";
    OutputNameValueSeparator();

    switch (field.type())
    {

    case UnknownField::TYPE_VARINT:
    {

      output_str += to_string(field.varint());
      break;
    }
    case UnknownField::TYPE_FIXED32:
    {

      output_str += "\"" + to_string(field.fixed32()) + "\"";
      break;
    }
    case UnknownField::TYPE_FIXED64:
    {
      uint64 value64 = field.fixed64();
      stringstream ss;
      ss << std::hex << value64;
      output_str += "0x" + ss.str();

      break;
    }
    case UnknownField::TYPE_LENGTH_DELIMITED:
    {

      const string& value = field.length_delimited();
      UnknownFieldSet embedded_unknown_fields;

      // This field is parseable as a message,
      //so it is probably an embedded message.
      if (!value.empty() && embedded_unknown_fields.ParseFromString(value))
      {
        OutputMessageStart();
        OutputUnknownFields(embedded_unknown_fields);
        OutputMessageEnd();
      }
      // This field is not parseable as a message, so assume a byte string.
      else
      {
        char* p = (char*)value.c_str();
        output_str += p;
      }
      break;
    }
    case UnknownField::TYPE_GROUP:
    {

      OutputMessageStart();
      OutputUnknownFields(field.group());
      OutputMessageEnd();
      break;
    }
    }
  }

}

void MessageParser::OutputMessageOpaquePayload(const string& message_name,
    const string& value)
{
  // Get the message descriptor from the descriptor pool
  const Descriptor* message_descriptor =
      pool.FindMessageTypeByName(message_name);

  if (NULL == message_descriptor)
  {
    ERROR_LOG(log_instance , "Error!! message " + message_name + " not found in descriptor"
        " pool cannot parse..");
    // parsing it as unknown field
    UnknownFieldSet embedded_unknown_fields;
    if (!value.empty() && embedded_unknown_fields.ParseFromString(value))
    {
      OutputMessageStart();
      OutputUnknownFields(embedded_unknown_fields);
      OutputMessageEnd();
    }

    return;
  }

  DynamicMessageFactory dmf;

  // Create an empty Message object that will hold the result of
  // deserializing a byte array by the proto definition
  const Message* prototype = dmf.GetPrototype(message_descriptor);
  if(prototype == NULL)
  {
    ERROR_LOG(log_instance, "MessageParser::OutputMessageOpaquePayload prototype is NULL");
    return;
  }

  // Create a mutable message object
  Message* message = prototype->New();
  if(message == NULL)
  {
    ERROR_LOG(log_instance, "MessageParser::OutputMessageOpaquePayload message is NULL");
    return;
  }
  // Deserialize a binary buffer that contains a message that is
  //described by the proto definition
  message->ParsePartialFromString(value);

  // Parse the message by reflection
  OutputMessageStart();
  OutputMessage(message);
  OutputMessageEnd();
}

// utilities for generating output string in json format

void MessageParser::OutputFieldName(const FieldDescriptor* field)
{
  if(field == NULL)
  {
    ERROR_LOG(log_instance, "MessageParser::OutputFieldName field is NULL");
    return;
  }

  if (field->is_extension())
    output_str += "\"[" + field->full_name() + "]\"";
  else if (field->type() == FieldDescriptor::TYPE_GROUP)
    output_str += "\"" + field->message_type()->name() + "\"";
  else
    output_str += "\"" + field->name() + "\"";
}


void MessageParser::OutputNameValueSeparator()
{
  output_str += " : ";
}
void MessageParser::OutputMessageStart()
{
  output_str += "{";
  output_str += "\n";
  space++;
  for (unsigned int i = 0; i < space; i++)
  {
    output_str += " ";
  }
}
void MessageParser::OutputMessageEnd()
{
  output_str += "\n";
  space--;
  for (unsigned int i = 0; i < space; i++)
  {
    output_str += " ";
  }
  output_str += "}";
}
void MessageParser::OutputFieldSeparator()
{
  output_str += ",";
  output_str += "\n";
  for (unsigned int i = 0; i < space; i++)
  {
    output_str += " ";
  }
}
void MessageParser::OutputRepeatedFieldStart()
{
  output_str += "[";
  output_str += "\n";
  space++;
  for (unsigned int i = 0; i < space; i++)
  {
    output_str += " ";
  }
}
void MessageParser::OutputRepeatedFieldEnd()
{
  output_str += "\n";
  space--;
  for (unsigned int i = 0; i < space; i++)
  {
    output_str += " ";
  }
  output_str += "]";
}

void MessageParser::OutputAttributeValueSeparator()
{
  output_str += ", ";
}


