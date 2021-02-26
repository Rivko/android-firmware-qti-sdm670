/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file com_qualcomm_qti_usta_core_SensorContextJNI.cpp
  @brief
  JNI implementation for communicating between App and Native codes.
============================================================================*/

#include "com_qualcomm_qti_usta_core_SensorContextJNI.h"
#include "sensor_cxt.h"
#include "logging_util.h"
#include <mutex>

vector<string> gClientProcessor ;
vector<string> gWakeupDeliveryType ;
vector<string> samplesBufferPool;
static usta_logging_util *log_instance;
std::mutex g_samples_mutex;
bool g_locked_samples_mutex;
vector<string> registrySamplesBufferPool;
std::mutex g_registry_samples_mutex;
bool g_registry_locked_samples_mutex;



/*Below Enum Should be in sync with Java Ewnum */
typedef enum DataType {
  STRING,
  SIGNED_INTEGER32,
  SIGNED_INTEGER64,
  UNSIGNED_INTEGER32,
  UNSIGNED_INTEGER64,
  FLOAT,
  BOOLEAN,
  ENUM,
  USER_DEFINED
}DataType;

typedef enum AccessModifier {
  REQUIRED,
  OPTIONAL,
  REPEATED
}AccessModifier;

typedef enum ModeType {
  USTA_MODE_TYPE_UI,
  USTA_MODE_TYPE_COMMAND_LINE,
  /*Please add all other modes before this line. USTA_MODE_TYPE_COUNT will be the last line to have count */
  USTA_MODE_TYPE_COUNT
}ModeType;

SensorCxt* gSensorContextInstance[USTA_MODE_TYPE_COUNT] = {NULL, NULL};

jobject setFieldInfo(JNIEnv *env, jobject obj, field_info &in_filed_info);
jobject setAccessModifier(JNIEnv *env, jobject obj, string label);
jobject setDataType(JNIEnv *env, jobject obj, string data_type);
jobject setNestedMessageInfo(JNIEnv *env, jobject obj, nested_msg_info &nested_msg);
void getFieldInfo(JNIEnv *env, jobject obj, jobject jfieldobject, send_req_field_info &each_field);

void streamEventsDisplayCallback(string displaySamples , bool is_registry_sensor)
{
  if(is_registry_sensor == false)
  {
    samplesBufferPool.insert(samplesBufferPool.begin(), displaySamples);
    if(g_locked_samples_mutex == true)
      g_samples_mutex.unlock();
  }
  else if(is_registry_sensor == true)
  {
    registrySamplesBufferPool.insert(registrySamplesBufferPool.begin(), displaySamples);
    if(g_registry_locked_samples_mutex == true)
      g_registry_samples_mutex.unlock();
  }
}

string getModeType(JNIEnv *env, jobject obj, jobject modeType)
{
  UNUSED_VAR(obj);
  /*Get the object */
  jclass modeType_class = env->GetObjectClass(modeType);
  if(modeType_class == NULL)
  {
    ERROR_LOG(log_instance , "modeType_class is not found");
    return NULL;
  }

  jmethodID GetEnumvalueMethod = env->GetMethodID(modeType_class, "getEnumValue", "()I");
  jint value = env->CallIntMethod(modeType, GetEnumvalueMethod);

  string modetype;
  if(value == USTA_MODE_TYPE_UI)
  {
    modetype = "UI";
  }
  else if(value == USTA_MODE_TYPE_COMMAND_LINE)
  {
    modetype = "COMMAND_LINE";
  }

  return modetype;

}


jobjectArray getSensorList(JNIEnv *env, jobject obj, jobject inModeType)
{
  UNUSED_VAR(env);
  UNUSED_VAR(obj);
  INFO_LOG(log_instance , " getSensorList Start ");
  log_instance = usta_logging_util::create_instance();
  vector<sensor_info> sensorListInfo;
  string modeType = getModeType(env, obj, inModeType);
  if(modeType.compare("UI") == 0)
  {
    INFO_LOG(log_instance , "getSensorList UI Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_UI] = SensorCxt::getInstance();
    gSensorContextInstance[USTA_MODE_TYPE_UI]->get_sensor_list(sensorListInfo);
    display_samples_cb ptr_display_samples_cb = streamEventsDisplayCallback;
    gSensorContextInstance[USTA_MODE_TYPE_UI]->update_display_samples_cb(ptr_display_samples_cb);
  }
  else if(modeType.compare("COMMAND_LINE") == 0)
  {
    INFO_LOG(log_instance , "getSensorList COMMAND_LINE Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE] = SensorCxt::getInstance();
    gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE]->get_sensor_list(sensorListInfo);
  }

  jobjectArray javaSensorListArray = NULL;
  javaSensorListArray = (jobjectArray)env->NewObjectArray(sensorListInfo.size(),env->FindClass("com/qualcomm/qti/usta/core/SensorListInfo"),NULL);
  if(javaSensorListArray == NULL)
    return NULL;

  for(unsigned int sensor_index = 0; sensor_index < sensorListInfo.size() ; sensor_index++)
  {

    jclass sensor_info_class = env->FindClass("com/qualcomm/qti/usta/core/SensorListInfo");
    if(sensor_info_class == NULL)
    {
      ERROR_LOG(log_instance , "sensor_info_class is not found");
      return NULL;
    }

    jmethodID sensor_info_methodID = env->GetMethodID(sensor_info_class, "<init>", "()V");
    if(sensor_info_methodID == NULL)
    {
      ERROR_LOG(log_instance , "sensor_info_methodID is not created");
      return NULL;
    }

    jobject sensor_instance = env->NewObject(sensor_info_class, sensor_info_methodID);

    /*dataType*/
    jfieldID fid = env->GetFieldID(sensor_info_class, "dataType", "Ljava/lang/String;");
    if(fid == 0)
      return NULL;
    jstring str = env->NewStringUTF(sensorListInfo[sensor_index].data_type.c_str());
    env->SetObjectField(sensor_instance, fid, str);

    /*vendor*/
    fid = env->GetFieldID(sensor_info_class, "vendor", "Ljava/lang/String;");
    if (fid == 0)
      return NULL;
    str = env->NewStringUTF(sensorListInfo[sensor_index].vendor.c_str());
    env->SetObjectField(sensor_instance, fid, str);


    /*suidLow*/
    fid = env->GetFieldID(sensor_info_class, "suidLow", "Ljava/lang/String;");
    if (fid == 0)
      return NULL;
    str = env->NewStringUTF(sensorListInfo[sensor_index].suid_low.c_str());
    env->SetObjectField(sensor_instance, fid, str);


    /*suidHigh*/
    fid = env->GetFieldID(sensor_info_class, "suidHigh", "Ljava/lang/String;");
    if (fid == 0)
      return NULL;
    str = env->NewStringUTF(sensorListInfo[sensor_index].suid_high.c_str());
    env->SetObjectField(sensor_instance, fid, str);


    /*Pushing everything to the vector */
    env->SetObjectArrayElement(javaSensorListArray,sensor_index, sensor_instance);

  }


  INFO_LOG(log_instance , " getSensorList End ");

  return javaSensorListArray;

}

jobjectArray getClientProcessors(JNIEnv *env , jobject jOb, jobject inModeType)
{
  UNUSED_VAR(jOb);
  string modeType = getModeType(env, jOb, inModeType);
  if(modeType.compare("UI") == 0)
  {
    INFO_LOG(log_instance , "getClientProcessors UI Mode it is ");
  }
  else if(modeType.compare("COMMAND_LINE") == 0)
  {
    INFO_LOG(log_instance , "getClientProcessors COMMAND_LINE Mode it is ");
  }
  jobjectArray javaStringArray;
  javaStringArray= (jobjectArray)env->NewObjectArray(gClientProcessor.size(),env->FindClass("java/lang/String"),env->NewStringUTF(""));

  for(unsigned int index = 0; index < gClientProcessor.size(); index++)
  {
    env->SetObjectArrayElement(javaStringArray,index, env->NewStringUTF(gClientProcessor[index].c_str()));
  }

  return javaStringArray;
}

jobjectArray getWakeupDeliveryType(JNIEnv *env , jobject jOb, jobject inModeType)
{
  UNUSED_VAR(jOb);
  string modeType = getModeType(env, jOb, inModeType);
  if(modeType.compare("UI") == 0)
  {
    INFO_LOG(log_instance , "getWakeupDeliveryType UI Mode it is ");
  }
  else if(modeType.compare("COMMAND_LINE") == 0)
  {
    INFO_LOG(log_instance , "getWakeupDeliveryType COMMAND_LINE Mode it is ");
  }
  jobjectArray javaStringArray;
  javaStringArray= (jobjectArray)env->NewObjectArray(gWakeupDeliveryType.size(),env->FindClass("java/lang/String"),env->NewStringUTF(""));
  for(unsigned int index = 0; index < gWakeupDeliveryType.size(); index++)
  {
    env->SetObjectArrayElement(javaStringArray,index, env->NewStringUTF(gWakeupDeliveryType[index].c_str()));
  }

  return javaStringArray;
}

void removeSensors(JNIEnv *env , jobject jOb, jobject inModeType , jintArray inSensorHandleArray)
{
  UNUSED_VAR(jOb);

  INFO_LOG(log_instance , " removeSensors Start ");
  string modeType = getModeType(env, jOb, inModeType);

  int sensorHandleArrayLength = env->GetArrayLength(inSensorHandleArray);

  vector<int> sensorsToBeRemoved;

  for(int index = 0 ; index < sensorHandleArrayLength ; index++)
  {
    int currentSensorHandle = -1;
    env->GetIntArrayRegion(inSensorHandleArray, index, 1, &currentSensorHandle);
    if(currentSensorHandle == -1 )
    {
      ERROR_LOG(log_instance , "Error while getting currentSensorHandle in removeSensors JNI Call");
      continue;
    }

    sensorsToBeRemoved.push_back(currentSensorHandle);
  }
  if(modeType.compare("UI") == 0)
  {
    INFO_LOG(log_instance , "removeSensors UI Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_UI]->remove_sensors(sensorsToBeRemoved);
  }
  else if(modeType.compare("COMMAND_LINE") == 0)
  {
    INFO_LOG(log_instance , "removeSensors COMMAND_LINE Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE]->remove_sensors(sensorsToBeRemoved);
  }

  INFO_LOG(log_instance , " removeSensors End ");

}

jobject setAccessModifier(JNIEnv *env, jobject obj, string label)
{
  UNUSED_VAR(obj);
  jclass accessModifier_class = env->FindClass("com/qualcomm/qti/usta/core/AccessModifier");
  if(accessModifier_class == NULL)
  {
    ERROR_LOG(log_instance , "setAccessModifier is not found");
    return NULL;
  }

  jfieldID fid = 0;
  int accessModifier_value;
  if(label.compare("required") == 0)
  {
    accessModifier_value = REQUIRED;
    fid = env->GetStaticFieldID(accessModifier_class, "REQUIRED" , "Lcom/qualcomm/qti/usta/core/AccessModifier;");
  }
  else if(label.compare("optional") == 0)
  {
    accessModifier_value = OPTIONAL;
    fid = env->GetStaticFieldID(accessModifier_class, "OPTIONAL" , "Lcom/qualcomm/qti/usta/core/AccessModifier;");
  }
  else if(label.compare("repeated") == 0)
  {
    accessModifier_value = REPEATED;
    fid = env->GetStaticFieldID(accessModifier_class, "REPEATED" , "Lcom/qualcomm/qti/usta/core/AccessModifier;");
  }
  if(fid == 0)
    return NULL;

  jobject accessModifier_instance = env->GetStaticObjectField(accessModifier_class, fid);

  return accessModifier_instance;

}

string getAccessModifier(JNIEnv *env, jobject obj, jobject accessModifer)
{
  UNUSED_VAR(obj);
  string label;


  /*Get the object */
  jclass acessModifier_class = env->GetObjectClass(accessModifer);
  if(acessModifier_class == NULL)
  {
    ERROR_LOG(log_instance , "acessModifier_class is not found");
    return NULL;
  }

  jmethodID GetAcessModiferValueMethod = env->GetMethodID(acessModifier_class, "getAccessModifierValue", "()I");
  jint value = env->CallIntMethod(accessModifer, GetAcessModiferValueMethod);

  if(value == REQUIRED)
  {
    label = "required";
  }
  else if(value == OPTIONAL)
  {
    label = "optional";
  }
  else if(value == REPEATED)
  {
    label = "repeated";
  }

  return label;

}


jobject setDataType(JNIEnv *env, jobject obj, string data_type)
{
  UNUSED_VAR(obj);
  jclass datatype_class = env->FindClass("com/qualcomm/qti/usta/core/DataType");
  if(datatype_class == NULL)
  {
    ERROR_LOG(log_instance , "DataType is not found");
    return NULL;
  }

  jfieldID fid ;
  int datatype_value;
  if(data_type.compare("enum") == 0)
  {
    datatype_value = ENUM;
    fid = env->GetStaticFieldID(datatype_class, "ENUM" , "Lcom/qualcomm/qti/usta/core/DataType;");
  }
  else if(data_type.compare("int32") == 0)
  {
    datatype_value = SIGNED_INTEGER32;
    fid = env->GetStaticFieldID(datatype_class, "SIGNED_INTEGER32" , "Lcom/qualcomm/qti/usta/core/DataType;");
  }
  else if(data_type.compare("int64") == 0)
  {
    datatype_value = SIGNED_INTEGER64;
    fid = env->GetStaticFieldID(datatype_class, "SIGNED_INTEGER64" , "Lcom/qualcomm/qti/usta/core/DataType;");
  }
  else if(data_type.compare("uint32") == 0)
  {
    datatype_value = UNSIGNED_INTEGER32;
    fid = env->GetStaticFieldID(datatype_class, "UNSIGNED_INTEGER32" , "Lcom/qualcomm/qti/usta/core/DataType;");
  }
  else if(data_type.compare("uint64") == 0)
  {
    datatype_value = UNSIGNED_INTEGER64;
    fid = env->GetStaticFieldID(datatype_class, "UNSIGNED_INTEGER64" , "Lcom/qualcomm/qti/usta/core/DataType;");
  }
  else if(data_type.compare("float") == 0 || data_type.compare("double") == 0)
  {
    datatype_value = FLOAT;
    fid = env->GetStaticFieldID(datatype_class, "FLOAT" , "Lcom/qualcomm/qti/usta/core/DataType;");
  }
  else if(data_type.compare("bool") == 0)
  {
    datatype_value = BOOLEAN;
    fid = env->GetStaticFieldID(datatype_class, "BOOLEAN" , "Lcom/qualcomm/qti/usta/core/DataType;");
  }
  else if(data_type.compare("string") == 0)
  {
    datatype_value = STRING;
    fid = env->GetStaticFieldID(datatype_class, "STRING" , "Lcom/qualcomm/qti/usta/core/DataType;");
  }
  else
  {
    datatype_value = USER_DEFINED;
    fid = env->GetStaticFieldID(datatype_class, "USER_DEFINED" , "Lcom/qualcomm/qti/usta/core/DataType;");
  }
  if(fid == 0)
    return NULL;

  jobject datatype_instance = env->GetStaticObjectField(datatype_class, fid);


  return datatype_instance;

}
string getDataType(JNIEnv *env, jobject obj, jobject dataType)
{
  UNUSED_VAR(obj);
  /*Get the object */
  jclass dataType_class = env->GetObjectClass(dataType);
  if(dataType_class == NULL)
  {
    ERROR_LOG(log_instance , "dataType_class is not found");
    return NULL;
  }

  jmethodID GetEnumvalueMethod = env->GetMethodID(dataType_class, "getEnumValue", "()I");
  jint value = env->CallIntMethod(dataType, GetEnumvalueMethod);

  string datatype;
  if(value == ENUM)
  {
    datatype = "enum";
  }
  else if(value == SIGNED_INTEGER32)
  {
    datatype = "int32";
  }
  else if(value == SIGNED_INTEGER64)
  {
    datatype = "int64";
  }
  else if(value == UNSIGNED_INTEGER32)
  {
    datatype = "uint32";
  }
  else if(value == UNSIGNED_INTEGER64)
  {
    datatype = "uint64";
  }
  else if(value == FLOAT)
  {
    datatype = "float";
  }
  else if(value == BOOLEAN)
  {
    datatype = "bool";
  }
  else if(value == STRING)
  {
    datatype = "string";
  }

  return datatype;

}


jobject setNestedMessageInfo(JNIEnv *env, jobject obj, nested_msg_info &nested_msg)
{
  UNUSED_VAR(obj);
  jclass nested_msg_class = env->FindClass("com/qualcomm/qti/usta/core/NestedMsgPayload");
  if(nested_msg_class == NULL)
  {
    ERROR_LOG(log_instance , "NestedMsgPayload is not found");
    return NULL;
  }

  jmethodID nested_msg_methodID = env->GetMethodID(nested_msg_class, "<init>", "()V");
  if(nested_msg_methodID == NULL)
  {
    ERROR_LOG(log_instance , "NestedMsgPayload is not created");
    return NULL;
  }

  jobject nested_msg_instance = env->NewObject(nested_msg_class, nested_msg_methodID);

  /*nested msg name*/
  jfieldID fid = env->GetFieldID(nested_msg_class, "nestedMsgName", "Ljava/lang/String;");
  if (fid == 0)
    return NULL;
  jstring str = env->NewStringUTF(nested_msg.msg_name.c_str());
  env->SetObjectField(nested_msg_instance, fid, str);

  /*Vector of fields */
  jobjectArray javaSensorPayLoadFieldsArray;
  javaSensorPayLoadFieldsArray= (jobjectArray)env->NewObjectArray(nested_msg.fields.size(),env->FindClass("com/qualcomm/qti/usta/core/SensorPayloadField"),NULL);

  for(unsigned int field_index = 0 ; field_index < nested_msg.fields.size() ; field_index++)
  {
    jobject field_instance = setFieldInfo(env, obj, nested_msg.fields[field_index]);
    env->SetObjectArrayElement(javaSensorPayLoadFieldsArray,field_index, field_instance);
  }
  fid = env->GetFieldID(nested_msg_class, "fields", "[Lcom/qualcomm/qti/usta/core/SensorPayloadField;");
  if (fid == 0)
    return NULL;
  env->SetObjectField(nested_msg_instance, fid, javaSensorPayLoadFieldsArray);


  return nested_msg_instance;


}


void getNestedMessageInfo(JNIEnv *env, jobject obj, jobject jNestedReqMsg, send_req_nested_msg_info &nested_msg)
{
  UNUSED_VAR(obj);
  /*Get the object */
  jclass req_msg_class = env->GetObjectClass(jNestedReqMsg);
  if(req_msg_class == NULL)
  {
    ERROR_LOG(log_instance , "req_msg_class is not found");
    return;
  }

  /*Message Name*/
  jmethodID GetMessageNameMethod = env->GetMethodID(req_msg_class, "getNestedMsgName", "()Ljava/lang/String;");
  jstring jMsgName = (jstring) env->CallObjectMethod(jNestedReqMsg, GetMessageNameMethod);

  const char *msgNamePtr = env->GetStringUTFChars(jMsgName, NULL);
  string message_name(msgNamePtr);
  nested_msg.msg_name = message_name;

  /*SensorPayloadField[]*/

  jfieldID fid = env->GetFieldID(req_msg_class, "fields", "[Lcom/qualcomm/qti/usta/core/SensorPayloadField;");
  if (fid == 0)
    return;
  jobjectArray javaSensorPayLoadFieldsArray = (jobjectArray) env->GetObjectField(jNestedReqMsg, fid);
  int fieldCount = env->GetArrayLength(javaSensorPayLoadFieldsArray);

  for(int field_index = 0 ; field_index < fieldCount; field_index++)
  {
    jobject jfieldobject = env->GetObjectArrayElement(javaSensorPayLoadFieldsArray, field_index);
    send_req_field_info each_field;
    getFieldInfo(env, obj, jfieldobject, each_field);
    nested_msg.fields.push_back(each_field);
  }


}


jobject setFieldInfo(JNIEnv *env, jobject obj, field_info &in_filed_info)
{
  UNUSED_VAR(obj);
  jclass filed_info_class = env->FindClass("com/qualcomm/qti/usta/core/SensorPayloadField");
  if(filed_info_class == NULL)
  {
    ERROR_LOG(log_instance , "SensorPayloadField is not found");
    return NULL;
  }

  jmethodID field_info_methodID = env->GetMethodID(filed_info_class, "<init>", "()V");
  if(field_info_methodID == NULL)
  {
    ERROR_LOG(log_instance , "SensorPayloadField is not created");
    return NULL;
  }

  jobject field_info_instance = env->NewObject(filed_info_class, field_info_methodID);

  /*Name*/
  jfieldID fid = env->GetFieldID(filed_info_class, "fieldName", "Ljava/lang/String;");
  if (fid == 0)
    return NULL;
  jstring str = env->NewStringUTF(in_filed_info.name.c_str());
  env->SetObjectField(field_info_instance, fid, str);


  /*Data type*/
  fid = env->GetFieldID(filed_info_class, "dataType", "Lcom/qualcomm/qti/usta/core/DataType;");
  if (fid == 0)
    return NULL;
  jobject jdatatype = setDataType(env, obj, in_filed_info.data_type);
  env->SetObjectField(field_info_instance, fid, jdatatype);

  /*Lable*/
  fid = env->GetFieldID(filed_info_class, "accessModifier", "Lcom/qualcomm/qti/usta/core/AccessModifier;");
  if (fid == 0)
    return NULL;
  jobject jAccessModifier = setAccessModifier(env, obj,in_filed_info.label);
  env->SetObjectField(field_info_instance, fid, jAccessModifier);

  /*is_user_defined_msg */
  fid = env->GetFieldID(filed_info_class, "isUserDefined", "Z");
  if (fid == 0)
    return NULL;
  env->SetBooleanField(field_info_instance, fid, in_filed_info.is_user_defined_msg);



  /*Nested Messages*/
  if(in_filed_info.is_user_defined_msg == true)
  {

    /*Vector of nested messages*/
    jobjectArray javaNestedMsgArray;
    javaNestedMsgArray= (jobjectArray)env->NewObjectArray(in_filed_info.nested_msgs.size(),env->FindClass("com/qualcomm/qti/usta/core/NestedMsgPayload"),NULL);

    for(unsigned int nested_msg_index = 0 ; nested_msg_index < in_filed_info.nested_msgs.size() ; nested_msg_index++)
    {
      jobject nested_msg_instance = setNestedMessageInfo(env, obj, in_filed_info.nested_msgs[nested_msg_index]);
      env->SetObjectArrayElement(javaNestedMsgArray,nested_msg_index, nested_msg_instance);
    }
    fid = env->GetFieldID(filed_info_class, "nestedMsgs", "[Lcom/qualcomm/qti/usta/core/NestedMsgPayload;");
    if (fid == 0)
      return NULL;
    env->SetObjectField(field_info_instance, fid, javaNestedMsgArray);

  }
  else
  {
    /*Has default*/
    fid = env->GetFieldID(filed_info_class, "hasDefaultValue", "Z");
    if (fid == 0)
      return NULL;
    env->SetBooleanField(field_info_instance, fid, in_filed_info.has_default);

    /*is enum*/
    fid = env->GetFieldID(filed_info_class, "isEnumerated", "Z");
    if (fid == 0)
      return NULL;
    env->SetBooleanField(field_info_instance, fid, in_filed_info.is_enum);


    if(in_filed_info.has_default == true)
    {
      /*default value*/
      fid = env->GetFieldID(filed_info_class, "defaultValue", "Ljava/lang/String;");
      if (fid == 0)
        return NULL;
      str = env->NewStringUTF(in_filed_info.default_val.c_str());
      env->SetObjectField(field_info_instance, fid, str);
    }

    if(in_filed_info.is_enum == true)
    {
      /*enum_list*/

      jobjectArray javaEnumListArray;
      javaEnumListArray= (jobjectArray)env->NewObjectArray(in_filed_info.enum_list.size(),env->FindClass("java/lang/String"),env->NewStringUTF(""));

      for(unsigned int enum_index = 0 ; enum_index < in_filed_info.enum_list.size(); enum_index ++)
      {
        env->SetObjectArrayElement(javaEnumListArray,enum_index, env->NewStringUTF(in_filed_info.enum_list[enum_index].name.c_str()));
      }
      fid = env->GetFieldID(filed_info_class, "enumValues", "[Ljava/lang/String;");
      if (fid == 0)
        return NULL;
      env->SetObjectField(field_info_instance, fid, javaEnumListArray);
    }

  }

  return field_info_instance;

}
void getFieldInfo(JNIEnv *env, jobject obj, jobject jfieldobject, send_req_field_info &each_field)
{
  UNUSED_VAR(obj);

  /*Get the object */
  jclass field_class = env->GetObjectClass(jfieldobject);
  if(field_class == NULL)
  {
    ERROR_LOG(log_instance , "field_class is not found");
    return;
  }

  /*fieldName*/
  jfieldID fid = env->GetFieldID(field_class, "fieldName", "Ljava/lang/String;");
  if (fid == 0)
    return;
  jstring jFieldName = (jstring) env->GetObjectField(jfieldobject, fid);
  const char *fieldNamePtr = env->GetStringUTFChars(jFieldName, NULL);
  string field_name(fieldNamePtr);
  each_field.name = field_name;

  /*is user defined */
  fid = env->GetFieldID(field_class, "isUserDefined", "Z");
  if (fid == 0)
    return;
  jboolean jisUserDefined = env->GetBooleanField(jfieldobject, fid);
  bool isUserDefined = jisUserDefined;


  if(isUserDefined == true)
  {
    /*NestedMsgPayload[] */
    fid = env->GetFieldID(field_class, "nestedMsgs", "[Lcom/qualcomm/qti/usta/core/NestedMsgPayload;");
    if (fid == 0)
      return;

    jobjectArray javaNestedMessagesArray = (jobjectArray) env->GetObjectField(jfieldobject, fid);
    int nestedMsgCount = env->GetArrayLength(javaNestedMessagesArray);

    for(int index = 0 ; index < nestedMsgCount; index++)
    {
      jobject jnestedMessageobject = env->GetObjectArrayElement(javaNestedMessagesArray, index);
      send_req_nested_msg_info nestedMsgInfo ;
      getNestedMessageInfo(env, obj, jnestedMessageobject, nestedMsgInfo);
      each_field.nested_msgs.push_back(nestedMsgInfo);
    }

  }
  else
  {
    /*Data type*/
    fid = env->GetFieldID(field_class, "dataType", "Lcom/qualcomm/qti/usta/core/DataType;");
    if (fid == 0)
      return;
    jobject jdatatype = env->GetObjectField(jfieldobject, fid);
    string dataType = getDataType(env, obj, jdatatype);
    //    /*Lable*/
    fid = env->GetFieldID(field_class, "accessModifier", "Lcom/qualcomm/qti/usta/core/AccessModifier;");
    if (fid == 0)
      return;
    jobject jAccessModifier = env->GetObjectField(jfieldobject, fid);
    string accessModifier = getAccessModifier(env, obj,jAccessModifier);

    /*value of the field */
    fid = env->GetFieldID(field_class, "valuesForNative", "[Ljava/lang/String;");
    if (fid == 0)
    {
      ERROR_LOG(log_instance , "valuesForNative field not found ");
      return;
    }
    jobjectArray javaValueArray = (jobjectArray) env->GetObjectField(jfieldobject, fid);
    int valueCount = env->GetArrayLength(javaValueArray);
    if(valueCount ==0 )
    {
      if(accessModifier.compare("required") == 0 && dataType.compare("bool") ==0 )
      {
        each_field.value.push_back("false");
      }
    }

    for(int value_index = 0 ; value_index < valueCount; value_index++)
    {
      jstring jValue = (jstring) env->GetObjectArrayElement(javaValueArray, value_index);
      if(jValue == NULL)
      {
        return;
      }
      const char *valuePtr = env->GetStringUTFChars(jValue, NULL);
      string value(valuePtr);
      if(each_field.name.compare("suid_low") == 0 ||
          each_field.name.compare("suid_high") == 0)
      {
        uint64 suid_part = 0;
        std::stringstream stream;
        stream << value;
        stream >> std::hex >> suid_part;
        each_field.value.push_back(to_string(suid_part));
      }
      else
      {
        if(value.empty())
        {
          if(accessModifier.compare("required") == 0 && dataType.compare("bool") ==0 )
          {
            each_field.value.push_back("false");
          }
        }
        else
        {
          each_field.value.push_back(value);
        }
      }
    }
  }
}


jobjectArray getRequestMessages(JNIEnv *env, jobject obj, jobject inModeType, jint jSensorHandle)
{
  UNUSED_VAR(obj);
  vector<msg_info> request_msgs;
  int sensorHandle = jSensorHandle;
  string modeType = getModeType(env, obj, inModeType);
  if(modeType.compare("UI") == 0)
  {
    INFO_LOG(log_instance , "getRequestMessages UI Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_UI]->get_request_list(sensorHandle, request_msgs);
  }
  else if(modeType.compare("COMMAND_LINE") == 0)
  {
    INFO_LOG(log_instance , "getRequestMessages COMMAND_LINE Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE]->get_request_list(sensorHandle, request_msgs);
  }

  jobjectArray javaReqMsgArray;
  javaReqMsgArray= (jobjectArray)env->NewObjectArray(request_msgs.size(),env->FindClass("com/qualcomm/qti/usta/core/ReqMsgPayload"),NULL);

  for(unsigned int req_msg_index = 0; req_msg_index < request_msgs.size() ; req_msg_index++)
  {
    jclass req_msg_class = env->FindClass("com/qualcomm/qti/usta/core/ReqMsgPayload");
    if(req_msg_class == NULL)
    {
      ERROR_LOG(log_instance , "ReqMsgPayload is not found");
      return NULL;
    }

    jmethodID reg_msg_methodID = env->GetMethodID(req_msg_class, "<init>", "()V");
    if(reg_msg_methodID == NULL)
    {
      ERROR_LOG(log_instance , "ReqMsgPayload is not created");
      return NULL;
    }

    jobject reg_msg_instance = env->NewObject(req_msg_class, reg_msg_methodID);

    /*Message Name*/
    jfieldID fid = env->GetFieldID(req_msg_class, "msgName", "Ljava/lang/String;");
    if (fid == 0)
      return NULL;
    jstring str = env->NewStringUTF(request_msgs[req_msg_index].msg_name.c_str());
    env->SetObjectField(reg_msg_instance, fid, str);


    /*Message ID*/
    fid = env->GetFieldID(req_msg_class, "msgID", "Ljava/lang/String;");
    if (fid == 0)
      return NULL;
    str = env->NewStringUTF(request_msgs[req_msg_index].msgid.c_str());
    env->SetObjectField(reg_msg_instance, fid, str);


    /*Vector of fields*/
    jobjectArray javaSensorPayLoadFieldsArray;
    javaSensorPayLoadFieldsArray= (jobjectArray)env->NewObjectArray(request_msgs[req_msg_index].fields.size(),env->FindClass("com/qualcomm/qti/usta/core/SensorPayloadField"),NULL);

    for(unsigned int field_index = 0 ; field_index < request_msgs[req_msg_index].fields.size() ; field_index++)
    {
      jobject field_instance = setFieldInfo(env, obj, request_msgs[req_msg_index].fields[field_index]);
      env->SetObjectArrayElement(javaSensorPayLoadFieldsArray,field_index, field_instance);
    }
    fid = env->GetFieldID(req_msg_class, "fields", "[Lcom/qualcomm/qti/usta/core/SensorPayloadField;");
    if (fid == 0)
      return NULL;
    env->SetObjectField(reg_msg_instance, fid, javaSensorPayLoadFieldsArray);


    /*Pushing everything to the vector */
    env->SetObjectArrayElement(javaReqMsgArray,req_msg_index, reg_msg_instance);

  }
  return javaReqMsgArray;
}

jstring getSensorAttributes(JNIEnv *env, jobject obj, jobject inModeType, jint jSensorHandle)
{
  UNUSED_VAR(obj);
  INFO_LOG(log_instance , " getSensorAttributes Start ");
  string sensorAttributes;
  int sensorHandle = jSensorHandle;
  string modeType = getModeType(env, obj, inModeType);
  if(modeType.compare("UI") == 0)
  {
    INFO_LOG(log_instance , "getSensorAttributes UI Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_UI]->get_attributes(sensorHandle, sensorAttributes);
  }
  else if(modeType.compare("COMMAND_LINE") == 0)
  {
    INFO_LOG(log_instance , "getSensorAttributes COMMAND_LINE Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE]->get_attributes(sensorHandle, sensorAttributes);
  }

  INFO_LOG(log_instance , " getSensorAttributes End ");
  return env->NewStringUTF(sensorAttributes.c_str());
}

void getSendRequestMsgInfo(JNIEnv *env, jobject obj, jobject jReqMsg, send_req_msg_info &out_send_reg)
{
  UNUSED_VAR(obj);

  INFO_LOG(log_instance , " getSendRequestMsgInfo Start ");

  /*Get the object */
  jclass req_msg_class = env->GetObjectClass(jReqMsg);
  if(req_msg_class == NULL)
  {
    ERROR_LOG(log_instance , "ReqMsgPayload is not found");
    return;
  }

  /*Message Name*/
  jfieldID fid = env->GetFieldID(req_msg_class, "msgName", "Ljava/lang/String;");
  if (fid == 0)
    return;
  jstring jMsgName = (jstring) env->GetObjectField(jReqMsg, fid);
  const char *msgNamePtr = env->GetStringUTFChars(jMsgName, NULL);
  string message_name(msgNamePtr);
  out_send_reg.msg_name = message_name;

  /*Message ID */
  fid = env->GetFieldID(req_msg_class, "msgID", "Ljava/lang/String;");
  if (fid == 0)
    return;
  jstring jMsgID = (jstring) env->GetObjectField(jReqMsg, fid);
  const char *msgIDPtr = env->GetStringUTFChars(jMsgID, NULL);
  string message_ID(msgIDPtr);
  out_send_reg.msgid = message_ID;


  /*SensorPayloadField[] */

  fid = env->GetFieldID(req_msg_class, "fields", "[Lcom/qualcomm/qti/usta/core/SensorPayloadField;");
  if (fid == 0) {
    ERROR_LOG(log_instance , "getSendRequestMsgInfo:fields not found");
    return;
  }
  jobjectArray javaSensorPayLoadFieldsArray = reinterpret_cast<jobjectArray>(env->GetObjectField(jReqMsg, fid));	
  int fieldCount = env->GetArrayLength(javaSensorPayLoadFieldsArray);


  for(int field_index = 0 ; field_index < fieldCount; field_index++)
  {

    jobject jfieldobject = reinterpret_cast<jobject>(env->GetObjectArrayElement(javaSensorPayLoadFieldsArray, field_index));
    send_req_field_info each_field;
    getFieldInfo(env, obj, jfieldobject, each_field);
    out_send_reg.fields.push_back(each_field);
  }

}

void getStdFieldsInfo(JNIEnv *env, jobject obj, jobject jSendReqStdFields, client_req_msg_fileds &std_field_info)
{
  UNUSED_VAR(obj);

  /*Get the object */
  jclass send_req_std_fields = env->GetObjectClass(jSendReqStdFields);
  if(send_req_std_fields == NULL)
  {
    ERROR_LOG(log_instance , "send_req_std_fields is not found");
    return;
  }

  jfieldID fid = env->GetFieldID(send_req_std_fields, "batchPeriod", "Ljava/lang/String;");
  if (fid == 0)
    return;
  jstring jbatchPeriod = (jstring) env->GetObjectField(jSendReqStdFields, fid);
  if(jbatchPeriod != NULL)
  {
    const char *batchPeriodPtr = env->GetStringUTFChars(jbatchPeriod, NULL);
    string batch_period(batchPeriodPtr);
    std_field_info.batch_period = batch_period;
  }


  fid = env->GetFieldID(send_req_std_fields, "flushPeriod", "Ljava/lang/String;");
  if (fid == 0)
    return;
  jstring jFlushPeriod = (jstring) env->GetObjectField(jSendReqStdFields, fid);
  if(jFlushPeriod != NULL)
  {
    const char *flushPeriodPtr = env->GetStringUTFChars(jFlushPeriod, NULL);
    string flush_period(flushPeriodPtr);
    std_field_info.flush_period= flush_period;
  }

  fid = env->GetFieldID(send_req_std_fields, "clientType", "Ljava/lang/String;");
  if (fid == 0)
    return;
  jstring jclientType = (jstring) env->GetObjectField(jSendReqStdFields, fid);
  if(jclientType != NULL)
  {
    const char *clientTypePtr = env->GetStringUTFChars(jclientType, NULL);
    string client_Type(clientTypePtr);
    std_field_info.client_type= client_Type;
  }

  fid = env->GetFieldID(send_req_std_fields, "wakeupType", "Ljava/lang/String;");
  if (fid == 0)
    return;
  jstring jwakeupType = (jstring) env->GetObjectField(jSendReqStdFields, fid);
  if(jwakeupType != NULL)
  {
    const char *wakeupTypePtr = env->GetStringUTFChars(jwakeupType, NULL);
    string wakeup_type(wakeupTypePtr);
    std_field_info.wakeup_type= wakeup_type;
  }

  fid = env->GetFieldID(send_req_std_fields, "flushOnly", "Ljava/lang/String;");
  if (fid == 0)
    return;
  jstring jFlushOnly = (jstring) env->GetObjectField(jSendReqStdFields, fid);
  if(jFlushOnly != NULL)
  {
    const char *flushOnlyPtr = env->GetStringUTFChars(jFlushOnly, NULL);
    string flush_only(flushOnlyPtr);
    std_field_info.flush_only= flush_only;
  }

  fid = env->GetFieldID(send_req_std_fields, "maxBatch", "Ljava/lang/String;");
  if (fid == 0)
    return;
  jstring jmaxBatch = (jstring) env->GetObjectField(jSendReqStdFields, fid);
  if(jmaxBatch != NULL)
  {
    const char *maxBatchPtr = env->GetStringUTFChars(jmaxBatch, NULL);
    string max_batch(maxBatchPtr);
    std_field_info.max_batch= max_batch;
  }

}
jobject setErrorCode(JNIEnv *env, jobject obj, usta_rc NativeErrorCode)
{
  UNUSED_VAR(obj);
  jclass errorCode_class = env->FindClass("com/qualcomm/qti/usta/core/NativeErrorCodes");
  if(errorCode_class == NULL)
  {
    ERROR_LOG(log_instance , "errorCode_class is not found");
    return NULL;
  }

  jfieldID fid = 0;
  if(USTA_RC_NO_MEMORY == NativeErrorCode)
  {
    fid = env->GetStaticFieldID(errorCode_class, "USTA_MEMORY_ERROR" , "Lcom/qualcomm/qti/usta/core/NativeErrorCodes;");
  }
  else if(USTA_RC_NO_PROTO == NativeErrorCode)
  {
    fid = env->GetStaticFieldID(errorCode_class, "USTA_PROTOFILES_NOT_FOUND" , "Lcom/qualcomm/qti/usta/core/NativeErrorCodes;");
  }
  else if(USTA_RC_REQUIIRED_FILEDS_NOT_PRESENT == NativeErrorCode)
  {
    fid = env->GetStaticFieldID(errorCode_class, "USTA_REQUIRED_FIELDS_MISSING" , "Lcom/qualcomm/qti/usta/core/NativeErrorCodes;");
  }
  else if(USTA_RC_SUCCESS == NativeErrorCode)
  {
    fid = env->GetStaticFieldID(errorCode_class, "USTA_NO_ERROR" , "Lcom/qualcomm/qti/usta/core/NativeErrorCodes;");
  }
  if(fid == 0)
    return NULL;

  jobject errorCode_instance = env->GetStaticObjectField(errorCode_class, fid);


  return errorCode_instance;

}

jobject sendRequest(JNIEnv *env, jobject obj, jobject inModeType, jint jSensorHandle, jobject jReqMsg , jobject jSendReqStdFields, jstring jlogFileName)
{
  UNUSED_VAR(obj);
  INFO_LOG(log_instance , " sendRequest Start ");
  string modeType = getModeType(env, obj, inModeType);

  int sensorHandle = jSensorHandle;
  string recievedFileName ;
  if(jlogFileName != NULL)
  {
    const char * logFileName = env->GetStringUTFChars(jlogFileName, NULL);
    recievedFileName = logFileName;
  }
  else
  {
    recievedFileName = "";
  }

  client_req_msg_fileds std_field_info;
  getStdFieldsInfo(env, obj, jSendReqStdFields, std_field_info);


  send_req_msg_info reqMsg;
  getSendRequestMsgInfo(env, obj, jReqMsg, reqMsg);
  usta_rc NativeErrorCode = USTA_RC_SUCCESS;
  string usta_mode ;
  if(modeType.compare("UI") == 0)
  {
    INFO_LOG(log_instance , "sendRequest UI Mode it is ");
    usta_mode = "UI";
    NativeErrorCode = gSensorContextInstance[USTA_MODE_TYPE_UI]->send_request(sensorHandle,reqMsg,std_field_info, recievedFileName , usta_mode);
  }
  else if(modeType.compare("COMMAND_LINE") == 0)
  {
    INFO_LOG(log_instance , "sendRequest COMMAND_LINE Mode it is ");
    usta_mode = "command_line";
    NativeErrorCode = gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE]->send_request(sensorHandle,reqMsg,std_field_info, recievedFileName , usta_mode);
  }

  INFO_LOG(log_instance , " sendRequest End ");

  jobject jErrorCode = setErrorCode(env,obj, NativeErrorCode);

  return jErrorCode;
}

void stopRequest(JNIEnv *env, jobject obj, jobject inModeType, jint jSensorHandle, jboolean is_qmi_disable_api_enabled)
{
  UNUSED_VAR(env);
  UNUSED_VAR(obj);
  INFO_LOG(log_instance , " stopRequest Start ");
  int sensorHandle = jSensorHandle;
  string modeType = getModeType(env, obj, inModeType);
  if(modeType.compare("UI") == 0)
  {
    INFO_LOG(log_instance , "stopRequest UI Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_UI]->stop_request(sensorHandle, (bool)is_qmi_disable_api_enabled);
  }
  else if(modeType.compare("COMMAND_LINE") == 0)
  {
    INFO_LOG(log_instance , "stopRequest COMMAND_LINE Mode it is ");
    gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE]->stop_request(sensorHandle , (bool)is_qmi_disable_api_enabled);
  }

  INFO_LOG(log_instance , " stopRequest End ");
}

void destroySensors(JNIEnv *env, jobject obj, jobject inModeType)
{
  UNUSED_VAR(env);
  UNUSED_VAR(obj);

  INFO_LOG(log_instance , " destroySensors Start ");
  string modeType = getModeType(env, obj, inModeType);
  if(modeType.compare("UI") == 0)
  {
    INFO_LOG(log_instance , "destroySensors UI Mode it is ");
    if( NULL != gSensorContextInstance[USTA_MODE_TYPE_UI])
    {
      delete gSensorContextInstance[USTA_MODE_TYPE_UI];
      gSensorContextInstance[USTA_MODE_TYPE_UI] = NULL;
    }
  }
  else if(modeType.compare("COMMAND_LINE") == 0)
  {
    INFO_LOG(log_instance , "destroySensors COMMAND_LINE Mode it is ");
    if( NULL != gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE])
    {
      delete gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE];
      gSensorContextInstance[USTA_MODE_TYPE_COMMAND_LINE] = NULL;
    }
  }


  INFO_LOG(log_instance , " destroySensors End ");
}

void updateLoggingFlag(JNIEnv *env, jobject obj , jboolean is_logging_disabled)
{
  log_instance = usta_logging_util::create_instance();
  log_instance->update_logging_flag((bool)is_logging_disabled);
}

jstring getSamplesFromNative(JNIEnv *env, jobject obj , jboolean jregistry_sensor)
{
  UNUSED_VAR(env);
  UNUSED_VAR(obj);
  bool is_registry_sensor = (bool)jregistry_sensor;
  if(is_registry_sensor == true)
  {
    jstring current_sample;
    if(!registrySamplesBufferPool.empty())
    {
      string currentSample = registrySamplesBufferPool.back();
      registrySamplesBufferPool.pop_back();
      current_sample = env->NewStringUTF(currentSample.c_str());
      return current_sample;
    } else {
      g_registry_samples_mutex.lock();
      g_registry_locked_samples_mutex = true;
    }
    return NULL;
  }
  else if(is_registry_sensor == false)
  {
    jstring current_sample;
    if(!samplesBufferPool.empty())
    {
      string currentSample = samplesBufferPool.back();
      samplesBufferPool.pop_back();
      current_sample = env->NewStringUTF(currentSample.c_str());
      return current_sample;
    } else {
      g_samples_mutex.lock();
      g_locked_samples_mutex = true;
    }
    return NULL;
  }
  return NULL;
}

void enableStreamingStatus(JNIEnv *env, jobject obj , jint sensorHandle)
{
  INFO_LOG(log_instance , " enableStreamingStatus Start ");
  if(sensorHandle == -1)
    return;
  gSensorContextInstance[USTA_MODE_TYPE_UI]->enableStreamingStatus((unsigned int)sensorHandle);
  INFO_LOG(log_instance , " enableStreamingStatus End ");
}

void disableStreamingStatus(JNIEnv *env, jobject obj , jint sensorHandle)
{
  INFO_LOG(log_instance , " disableStreamingStatus Start ");
  if(sensorHandle == -1)
      return;
  gSensorContextInstance[USTA_MODE_TYPE_UI]->disableStreamingStatus((unsigned int)sensorHandle);
  samplesBufferPool.clear();
  INFO_LOG(log_instance , " disableStreamingStatus End ");
}
/*
 * Class and package name
 * */
static const char *classPathName = "com/qualcomm/qti/usta/core/SensorContextJNI";

/*
 * List of native methods
 *
 * */
static JNINativeMethod methods[] = {
    {"getSensorList" , "(Lcom/qualcomm/qti/usta/core/ModeType;)[Lcom/qualcomm/qti/usta/core/SensorListInfo;", (void *)getSensorList},
    {"getClientProcessors" , "(Lcom/qualcomm/qti/usta/core/ModeType;)[Ljava/lang/String;", (void *)getClientProcessors},
    {"getWakeupDeliveryType" , "(Lcom/qualcomm/qti/usta/core/ModeType;)[Ljava/lang/String;", (void *)getWakeupDeliveryType},
    {"removeSensors" , "(Lcom/qualcomm/qti/usta/core/ModeType;[I)V", (void *)removeSensors},
    {"getRequestMessages" , "(Lcom/qualcomm/qti/usta/core/ModeType;I)[Lcom/qualcomm/qti/usta/core/ReqMsgPayload;", (void *)getRequestMessages},
    {"getSensorAttributes" , "(Lcom/qualcomm/qti/usta/core/ModeType;I)Ljava/lang/String;", (void *)getSensorAttributes},
    {"sendRequest" , "(Lcom/qualcomm/qti/usta/core/ModeType;ILcom/qualcomm/qti/usta/core/ReqMsgPayload;Lcom/qualcomm/qti/usta/core/SendReqStdFields;Ljava/lang/String;)Lcom/qualcomm/qti/usta/core/NativeErrorCodes;", (void *)sendRequest},
    {"stopRequest" , "(Lcom/qualcomm/qti/usta/core/ModeType;IZ)V", (void *)stopRequest},
    {"destroySensors" , "(Lcom/qualcomm/qti/usta/core/ModeType;)V", (void *)destroySensors},
    {"updateLoggingFlag" , "(Z)V", (void *)updateLoggingFlag},
    {"enableStreamingStatus" , "(I)V", (void *)enableStreamingStatus},
    {"disableStreamingStatus" , "(I)V", (void *)disableStreamingStatus},
    {"getSamplesFromNative" , "(Z)Ljava/lang/String;", (void *)getSamplesFromNative},
};

/*
 * Register several native methods for one class.
 *
 *
 * */
static int registerUSTANativeMethods(JNIEnv* envVar, const char* inClassName, JNINativeMethod* inMethodsList, int inNumMethods)
{
  jclass javaClazz = envVar->FindClass(inClassName);
  if (javaClazz == NULL)
  {
    return JNI_FALSE;
  }
  if (envVar->RegisterNatives(javaClazz, inMethodsList, inNumMethods) < 0)
  {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 *
 * Returns JNI_TRUE on success
 *
 * */
static int registerNatives(JNIEnv* env)
{
  if (!registerUSTANativeMethods(env, classPathName, methods, sizeof(methods) / sizeof(methods[0])))
  {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

/*
 * This is called by the VM when the shared library is first loaded.
 * */
typedef union
{
  JNIEnv* env;
  void* venv;
}UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
  UNUSED_VAR(reserved);
  UnionJNIEnvToVoid uenv;
  uenv.venv = NULL;
  JNIEnv* env = NULL;
  log_instance = usta_logging_util::create_instance();
  INFO_LOG(log_instance , "JNI_OnLoad Start");

  if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
  {
    ERROR_LOG(log_instance , "ERROR: GetEnv failed");
    return -1;
  }
  env = uenv.env;

  if (registerNatives(env) != JNI_TRUE)
  {
    ERROR_LOG(log_instance , "ERROR: registerNatives failed");
    return -1;
  }
  INFO_LOG(log_instance , "JNI_OnLoad End");

  return JNI_VERSION_1_4;
}
