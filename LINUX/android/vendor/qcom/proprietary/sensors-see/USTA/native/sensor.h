/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor.h
  @brief
  Sensor class definition.
============================================================================*/
#pragma once

#include "ssc_connection.h"
#include "sensor_common.h"
#include "sensor_attributes.h"
#include "sensor_message_parser.h"
#include "logging_util.h"
#include "sensor_diag_log.h"

typedef enum usta_error_cb_type
{
  USTA_QMI_ERRRO_CB_FOR_ATTRIBUTES,
  USTA_QMI_ERRRO_CB_FOR_STREAM
}usta_error_cb_type;

typedef struct client_req_msg_fileds
{
  string batch_period;
  string flush_period;
  string client_type;
  string wakeup_type;
  string flush_only;
  string max_batch;
}client_req_msg_fileds;


typedef struct sensor_info
{
  string data_type;
  string vendor;
  string suid_low;
  string suid_high;
}sensor_info;
/**
 * type alias for  event callback from Sensor for attribute request from client
 * This can be used by client as an acknowledgement that attribute has been
 * received
 */
typedef std::function<void(void)>handle_attribute_event_func;

/**
 * type alias for  event callback from Sensor for stream request from client
 * This can be used by client as an acknowledgement that an event of a stream
 * is received.
 * The client can count the number of streaming events received by keeping a
 * count of this  function call
 *
 */
typedef std::function<void(string in_string , bool is_registry_sensor)>handle_stream_event_func;

typedef std::function<void(ssc_error_type error)>handle_stream_error_func;

/**
 * @brief all sensors must be instantiated with this class. The communication
 *        to SSC is via this class
 *
 */
class Sensor
{

public:
  /**
   * @brief This creates requried connection with SSC via class ssc_connection
   *        for attribute
   *
   *
   * @param attrib_event_f is call back function of type
   *        handle_attribute_event_func, which will be called when attribute is
   *        received, NULL is this event is not required
   *
   * @param suid_low is the lower 64 bit of suid of this sensor
   *
   * @param suid_high is the higher 64 bit of suid of this sensor
   *
   * @param stream_event_f is call back function of
   *        type handle_stream_event_func, which will be called when a
   *        streaming event is received. NULL if no streaming event is required
   */
  Sensor(handle_attribute_event_func attrib_event_f,
      const uint64_t& suid_low,
      const uint64_t& suid_high,
      handle_stream_event_func stream_event_f,
      handle_stream_error_func stream_error_f ,
      handle_stream_error_func attrib_error_f
  );
  ~Sensor();
  /**
   * @brief sends attribute request for this sensor.
   *
   */
  usta_rc send_attrib_request();
  /**
   * @brief sends a request to a sensor in predefined request message format
   *
   * @param batch_period corresponds to batch period for streaming request
   *
   *@param flush_period corresponds to the flush period of streaming request
   *
   * @param client_type is the client triggering this request. one of :
   *        SNS_STD_PROCESSOR_SSC
   *        SNS_STD_PROCESSOR_APSS
   *        SNS_STD_PROCESSOR_ADSP
   *        SNS_STD_PROCESSOR_MDSP
   *
   * @param wakeup_type is a string triggering by the client. one of :
   *        SNS_STD_DELIVERY_WAKEUP
   *        SNS_STD_DELIVERY_NO_WAKEUP
   *        SNS_STD_DELIVERY_FLUSH
   *
   * @param logfile_name is the client supplied file name. The name of file
   *        will be:
   *        <logfile_name><datatype>-<vendor>.txt
   *        if logfile_name is empty, the file will be <datatype>-<vendor>.txt
   *
   * @param formated_request is the request message sent by the client in
   *        predefined format. The format is as follows
   *        "<msgid>:<field_value 1>:<field_value 2> ..."
   *        Note: no fields should be missing,
   *              optional fields must have some default values
   *              if a field is repeated, the number of times its repeated
   *              should preceed its values
   *              for eg.. if field_value 2 is repeated 3 times,
   *              the request msg will be:
   *               "<msgid>
   *               :<field_value 1>
   *               :3
   *               :<1st fv 2>
   *               :<2nd fv 2>
   *               :<3rd fv 2>
   *               :<field_value 3>"

   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc send_request(send_req_msg_info& formated_request,client_req_msg_fileds &std_fields_data,
      string& logfile_name, string& usta_mode);

  /**
   * @brief removes ssc_connection to save CM memory on USTA
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  void remove_attribute_connection();

  /**
   * @brief stops the streaming of this sensor
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */

  usta_rc stop_request(bool is_qmi_disable_api_enabled);
  /**
   * @brief checks if this sensor is a platform sensor which needs to be
   *        ignored/removed
   *
   * @return bool: true if this sensor is one of platform sensors,
   *         false otherwise
   */
  bool is_platform_sensor();
  /**
   * @brief returns sensor info comprising of suid,datatype and
   *        vendor of this sensor
   *
   * @param sensor_info is the string reference provided by the client
   *        which will be populated
   *        by this sensor in following format
   *        "<datatype>-<vendor>:<suid_low>:<suid_high>"
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc get_sensor_info(sensor_info& sensor_info_var);
  /**
   * @brief gives the request messages in a predefined format for all the
   *        request messages supported   by a sensor
   *
   *
   * @param request_vector is an empty vector reference of string provided
   *        by the client. This empty vector will be populated with all the
   *        request messages supported by a sensor corresponding to provided
   *        handle. The request_vector format will be:
   *
   *        “<msgName>:<msgID>{Name:<nameValue>;DataType:<datatypeValue>;
   *                           Label:<labelValue>;Default:<defaultValue>;
   *                           EnumValue:<enumValue1>,..;}
   *                          { <details of field 1> }
   *                          { <details of field 2> }”
   *        " <details of next message>"
   *        " <details of next message>"
   *        ...
   *
   *        msgName       : Message Name as defined in proto file
   *        msgID         : Message Id corresponding to this message as defined
   *                        in proto file
   *        nameValue     : Name of field as present in the message
   *        datatypeValue : Data type of field : int32,uint32,uint64,double,
   *                        float,bool,enum,string,message
   *        labelValue    : label of the field : required/optional/repeated
   *        EnumValue<i>  : list of comma separate possible enum values,
   *                        only if datatypeValue is enum
   *
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc get_request_list(vector<msg_info> &request_msgs);
  /**
   * @brief gets the attribute list of this sensor , It should be called after
   *        send_attrib_request()
   *
   * @param attribute_list is the string reference provided by the client which
   *        will be populated by this sensor with the attribute of this sensor
   *        in json format
   *
   * @return success or failure. Based on defined error code in enum: usta_rc
   */
  usta_rc get_attributes(string& attribute_list);

  void enableStreamingStatus();

  void disableStreamingStatus();

  void register_error_cb(usta_error_cb_type connection_type);

  void send_disable_requst();

private:
  /*
   * @brief suid is structure which contains suid information
   * */
  sensor_uid suid;

  /*
   * @brief pointer to ssc_connection, used for sending sensor attributes requests.
   *
   **/
  ssc_connection* ssc_attrib_client_obj;

  /*
   * @brief pointer to ssc_connection, used for sending sensor streaming requests.
   *
   **/
  ssc_connection* ssc_stream_client_obj;

  /*
   * @brief attrib_event_cb, used for sending data from sensor to sensor_context
   *
   **/
  handle_attribute_event_func attrib_event_cb;

  /*
   * @brief ssc_stream_cb, used for get sensors events from ssc_connection.
   *        Registered as call back
   *
   **/
  ssc_event_cb ssc_stream_cb;

  /*
   * @brief ssc_attrib_cb, used for get sensors attributes events from ssc_connection.
   *        Registered as call back
   *
   **/
  ssc_event_cb ssc_attrib_cb;

  ssc_error_cb ssc_stream_error_cb;
  ssc_error_cb ssc_attrib_error_cb;

  ssc_resp_cb ssc_stream_resp_cb;

  handle_stream_error_func stream_error_cb;
  handle_stream_error_func attrib_error_cb;

  handle_stream_event_func stream_event_cb;
  void sensor_stream_event_cb(const uint8_t *encoded_data_string,
      size_t encoded_string_size);
  // receiving response from ssc_client
  // callback can be provided as a lambda
  void sensor_attrib_event_cb(const uint8_t *encoded_data_string,
      size_t encoded_string_size);

  void sensor_stream_error_cb(ssc_error_type e);

  void sensor_attrib_error_cb(ssc_error_type e);

  void sensor_stream_resp_cb(uint32_t resp_value);
  string get_enum_string_from_value(uint32_t resp_value);

  void save_config_details(send_req_msg_info& formated_request,client_req_msg_fileds &std_fields_data,
      string& in_logfile_name , string &usta_mode);

  void save_payload_config(send_req_msg_info& formated_request);
  void save_standard_config(client_req_msg_fileds &std_fields_data);
  /*
   * @brief sensors_diag_log_obj, Instance for diag logging library
   *        used for logging request, event and response messages on to Diag/QXDM
   *
   **/
  sensors_diag_log* sensors_diag_log_obj;

  /*
   * @brief sensor_attribute instance which will be used to get attribute information
   *        stored inside sensor_attribute class
   *
   **/
  sensor_attribute* sensor_attribute_obj;

  /*
   * Instance to praser class, where all encoding and decoding is taken care
   * */
  MessageParser* sensor_parser;

  /*
   * Count for number of events received
   * */
  uint32 event_count;

  /*
   * file pointer for dumping sensor level information
   * */
  FILE *file_stream_ptr;

  /*
   * file_mutex is used to avoid event truncation into the log file
   * */
  pthread_mutex_t     file_mutex;
  pthread_mutex_t     proto_buf_mutex;
  pthread_mutex_t     sample_event_mutex;
  /*
   * concatenation of name and vendor of the sensor. It is sent to application.
   * */
  string name_vendor;

  /*
   * Sensor data type, It is required by diag loggin library
   **/
  string data_type;

  /*
   * sensor attribute information in json format.
   **/
  string attrib_json_string;

  usta_logging_util *log_instance;

  bool streaming_status_flag;



  send_req_msg_info payload_fields;
  client_req_msg_fileds std_fields;
  string logfile_name;
  string mode_type;
  bool is_registry_sensor;


};
