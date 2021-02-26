/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_cxt.cpp

  @brief
  SensorContext class implementation.
============================================================================*/
#include "sensor_cxt.h"
#include <algorithm>  //for using sort of vector


bool waitForResponse(pthread_mutex_t *cb_mutex_ptr,pthread_cond_t *cond_ptr,
    bool *cond_var)
{
  bool ret = false;            /* the return value of this function */
  int rc = 0;                  /* return code from pthread calls */


  /* special case where callback is issued before the main function
       can wait on cond */
  pthread_mutex_lock(cb_mutex_ptr);
  if (*cond_var == true)
  {
    ret = true;
  }
  else
  {
    while (*cond_var != true)
    {
      rc = pthread_cond_wait(cond_ptr, cb_mutex_ptr);
    }
    ret = ( rc == 0 || *cond_var ) ? true : false;
  }
  *cond_var = false;
  pthread_mutex_unlock(cb_mutex_ptr);
  return ret;
}

SensorCxt* SensorCxt::getInstance()
{
  SensorCxt* self = new SensorCxt;
  return self;
}

/*============================================================================
  SensorCxt Constructor
============================================================================*/
SensorCxt::SensorCxt()
{
  log_instance = usta_logging_util::get_instance();
  is_all_attributes_received = false;
  INFO_LOG(log_instance , " Unified Sensor Test Application triggered ");
  //intializing condition variable and mutex
  pthread_mutex_init(&cb_mutex, NULL);
  pthread_mutex_init(&attr_count_mutex, NULL);
  pthread_cond_init(&cb_cond, NULL);



  num_sensors =0;
  num_attr_received =0;

  is_resp_arrived=false;
  //initializing sensor event callback
  suid_event_cb = [this](uint64_t& suid_low,uint64_t& suid_high, bool flag)
    { this->handle_suid_event(suid_low,suid_high,flag);};

  attribute_event_cb =[this]()
    { this->handle_attribute_event();};

  stream_event_cb=[this](string inString , bool is_registry_sensor)
    { this->handle_stream_event(inString, is_registry_sensor);};

  stream_error_cb=[this](ssc_error_type error_code)
    { this->handle_stream_error_event(error_code);};

  attrib_error_cb=[this](ssc_error_type error_code)
    { this->handle_attrib_error_event(error_code);};

  // building descriptor pool
  if (USTA_RC_SUCCESS != build_descriptor_pool())
  {
    ERROR_LOG(log_instance , "Descriptor pool generation failed.. exiting the application ");
    assert(false);
  }

  // first SUID class object is created and request is sent for listing
  //all the sensors

  if (NULL == (suid_instance = new SuidSensor(suid_event_cb)))
  {
    ERROR_LOG(log_instance , "suid instance creation failed");
    return;
  }

  DEBUG_LOG(log_instance , " Intiating sensor_list request from SSC via SUID  ");

  string list_req_special_char = "";

  if (USTA_RC_SUCCESS !=(suid_instance->send_request(list_req_special_char)))
  {
    ERROR_LOG(log_instance , "\n SUID listing request failed ");
    return;
  }


  if (waitForResponse(&cb_mutex,&cb_cond,&is_resp_arrived) == false)
  {
    ERROR_LOG(log_instance , "\nError  ");
    pthread_mutex_unlock(&cb_mutex);
  }
  else
  {
    DEBUG_LOG(log_instance , "\nSensor list Response Received");
  }

  // removing suid_instance
  delete suid_instance;
  suid_instance = NULL;


  // sending attrib request for each sensor
  int i = 0;
  for(list<Sensor *>::iterator sensorHandle = mSensors.begin();
      sensorHandle != mSensors.end(); sensorHandle++)
  {
    is_resp_arrived=false;
    DEBUG_LOG(log_instance, "Sending Attribute request for handle " +  to_string(i));
    (*sensorHandle)->send_attrib_request();
    // wait for respective event to come
    if (waitForResponse(&cb_mutex,&cb_cond,&is_resp_arrived) == false)
    {
      ERROR_LOG(log_instance , "\nError  ");
      pthread_mutex_unlock(&cb_mutex);
    }
    else
    {
      DEBUG_LOG(log_instance , "Attribute  Response Received for handle " + to_string(i));
    }
    i++;
    // close the attribute connection as soon as its event arrives
    (*sensorHandle)->remove_attribute_connection();
  }


  DEBUG_LOG(log_instance , "Number of sensors from SSC : " + to_string(mSensors.size()) + "filter out platform sensors");
  for(list<Sensor *>::iterator sensorHandle = mSensors.begin();
      sensorHandle != mSensors.end(); )
  {
    if((*sensorHandle)->is_platform_sensor() == true)
    {
      delete *sensorHandle;
      *sensorHandle = NULL;
      sensorHandle = mSensors.erase(sensorHandle);
    }
    else
    {
      sensorHandle++;
    }
  }

  is_all_attributes_received = true;
  DEBUG_LOG(log_instance , "Number of sensors after filtering : " + to_string(mSensors.size()));


  DEBUG_LOG(log_instance , "Sensor Context instantiated");
}

SensorCxt::~SensorCxt()
{

  if (suid_instance)  delete suid_instance;
  suid_instance =NULL;


  /* Deleting sensor memory */
  for(list<Sensor *>::iterator sensorHandle = mSensors.begin();
      sensorHandle != mSensors.end(); )
  {
    delete *sensorHandle;
    *sensorHandle = NULL;
    sensorHandle = mSensors.erase(sensorHandle);
  }

  pthread_mutex_destroy(&cb_mutex);
  pthread_mutex_destroy(&attr_count_mutex);
  pthread_cond_destroy(&cb_cond);

}

usta_rc SensorCxt::get_request_list(unsigned int handle,vector<msg_info> &request_msgs)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  DEBUG_LOG(log_instance , " get_request_list for handle " + to_string(handle));
  for(sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return USTA_RC_FAILED;
  }
  return (*sensorHandle)->get_request_list(request_msgs);
}

usta_rc SensorCxt::get_attributes(unsigned int handle,string& attribute_list)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  for( sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return USTA_RC_FAILED;
  }

  return (*sensorHandle)->get_attributes(attribute_list);
}

usta_rc SensorCxt::get_sensor_list(vector<sensor_info>& sensor_list)
{

  DEBUG_LOG(log_instance , " Client requests for sensor lists");
  int i=0;
  for(auto sensorHandle = mSensors.begin();
      sensorHandle != mSensors.end(); sensorHandle++,i++)
  {
    sensor_info sensor_info_var;
    if(USTA_RC_SUCCESS!=(*sensorHandle)->get_sensor_info(sensor_info_var))
    {
      ERROR_LOG(log_instance , "Error handle "+ to_string(i) + " not part of sensor list");
    }
    else
    {
      sensor_list.push_back(sensor_info_var);
    }
  }

  return USTA_RC_SUCCESS;

}

usta_rc SensorCxt::remove_sensors(vector<int>& remove_sensor_list)
{
  // sorting the handles in acending order
  sort(remove_sensor_list.begin(), remove_sensor_list.end());
  list<Sensor *>::iterator sensorHandle;
  for(int i=remove_sensor_list.size()-1; i>=0; i--)
  {
    sensorHandle = next(mSensors.begin(), remove_sensor_list[i]);
    delete (*sensorHandle);
    mSensors.erase(sensorHandle);
  }

  return USTA_RC_SUCCESS;


}

usta_rc  SensorCxt::send_request(unsigned int handle, send_req_msg_info& formated_request,
    client_req_msg_fileds &std_fields_data,
    string& logfile_name ,  string usta_mode)
{
  unsigned int i=0;
  usta_rc rc=USTA_RC_SUCCESS;
  for(auto sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i)
    {
      rc=(*sensorHandle)->send_request(formated_request,std_fields_data, logfile_name, usta_mode);
      break;
    }
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return USTA_RC_FAILED;
  }

  if (rc!=USTA_RC_SUCCESS)
  {
    ERROR_LOG(log_instance , "sending request for handle "+ to_string(handle) + " failed");
    return rc;
  }
  // for testing only
#ifdef SENSOR_TEST
  if (waitForResponse(&cb_mutex,&cb_cond,&is_resp_arrived) == false)
  {
    ERROR_LOG(log_instance , "\nError  ");
    pthread_mutex_unlock(&cb_mutex);

  }
  else
  {
    DEBUG_LOG(log_instance , "\nSensor list Response Received");
  }
#endif
  return rc;

}

usta_rc  SensorCxt::stop_request(unsigned int handle , bool is_qmi_disable_api_enabled)
{
  unsigned int i=0;
  usta_rc rc=USTA_RC_SUCCESS;
  for(auto sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i)
    {
      rc=(*sensorHandle)->stop_request(is_qmi_disable_api_enabled);
      break;
    }
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return USTA_RC_FAILED;
  }

  if (rc!=USTA_RC_SUCCESS)
  {
    ERROR_LOG(log_instance , "sending request for handle "+ to_string(handle) + " failed");
    return rc;
  }

  return rc;

}


void SensorCxt::handle_suid_event(uint64_t& suid_low,uint64_t& suid_high,
    bool is_last_suid)
{

  // creating sensor to query attributes in constructor itself
  Sensor* sensorHandle = new Sensor(attribute_event_cb,
      suid_low,
      suid_high,
      stream_event_cb,
      stream_error_cb,
      attrib_error_cb);
  mSensors.push_back(sensorHandle);

  num_sensors++;

  if (is_last_suid)
  {
    pthread_mutex_lock(&cb_mutex);
    is_resp_arrived=true;
    pthread_cond_signal(&cb_cond);
    pthread_mutex_unlock(&cb_mutex);
  }
}

void SensorCxt::handle_attribute_event()
{

  pthread_mutex_lock(&attr_count_mutex);

  pthread_mutex_lock(&cb_mutex);
  is_resp_arrived=true;
  pthread_cond_signal(&cb_cond);
  pthread_mutex_unlock(&cb_mutex);

  pthread_mutex_unlock(&attr_count_mutex);

}

void SensorCxt::handle_stream_event(string inString , bool is_registry_sensor)
{

  if(ptr_display_samples_cb != NULL )
    (*ptr_display_samples_cb)(inString , is_registry_sensor);
}

void SensorCxt::register_qmi_error_cb(unsigned int handle, handle_stream_error_func client_error_func , usta_error_cb_type connection_type)
{
  unsigned int i=0;
  for(auto sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i)
    {

      if(connection_type == USTA_QMI_ERRRO_CB_FOR_ATTRIBUTES)
      {
        if(is_all_attributes_received == false)
        {
          client_attrib_error_cb = client_error_func;
          (*sensorHandle)->register_error_cb(USTA_QMI_ERRRO_CB_FOR_ATTRIBUTES);
        }
      }
      else if(connection_type == USTA_QMI_ERRRO_CB_FOR_STREAM)
      {
        client_stream_error_cb = client_error_func;
        (*sensorHandle)->register_error_cb(USTA_QMI_ERRRO_CB_FOR_STREAM);
      }
      break;
    }
  }

}

void SensorCxt::handle_stream_error_event(ssc_error_type error_code)
{
  if(client_stream_error_cb != NULL)
    client_stream_error_cb(error_code);
}

void SensorCxt::handle_attrib_error_event(ssc_error_type error_code)
{
  if(client_attrib_error_cb != NULL)
    client_attrib_error_cb(error_code);
}

void SensorCxt::enableStreamingStatus(unsigned int handle)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  for(sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return;
  }
  (*sensorHandle)->enableStreamingStatus();
}

void SensorCxt::disableStreamingStatus(unsigned int handle)
{
  unsigned int i=0;
  list<Sensor *>::iterator sensorHandle;
  for(sensorHandle = mSensors.begin(); sensorHandle != mSensors.end();
      sensorHandle++,i++ )
  {
    if (handle==i) break;
  }
  if(i>=mSensors.size())
  {
    ERROR_LOG(log_instance , "Error handle " + to_string(handle) + "exceding the sensor list size " + to_string((unsigned int)mSensors.size()) + "get_request_list failed" );
    return;
  }
  (*sensorHandle)->disableStreamingStatus();
}

void SensorCxt::update_display_samples_cb(display_samples_cb ptr_reg_callback)
{
  ptr_display_samples_cb = ptr_reg_callback;
}
