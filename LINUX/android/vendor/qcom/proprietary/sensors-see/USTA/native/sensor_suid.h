/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_suid.h

  @brief
  SuidSensor class definition.
============================================================================*/
#pragma once
#include "ssc_connection.h"


#include "sensor_common.h"
#include "sensor_message_parser.h"
#include "logging_util.h"
/**
 * type alias for  event callback from SuidSensor
 * @param suid_low is the lower 64 bit of suid of returned sensor
 * @param suid_high is the higher 64bit of suid of returned sensor
 * @param is_last is boolen value signifying this suid is last suid
 *        for a required data_type
 */
typedef std::function<void( uint64_t& suid_low,
    uint64_t& suid_high, bool is_last)> handle_suid_event_cb;


class SuidSensor
{
private:
  sensor_uid suid;   // stores the uid of suid sensor
  handle_suid_event_cb event_cb;
  ssc_connection* ssc_client_ptr;
  ssc_event_cb ssc_cb;
  MessageParser* sensor_parser;
  // used to log suid event which has all the sensor list
  FILE*        fileStreamPtr;
  string       logfile_name;
  // receiving response from ssc_client
  void sensor_event_cb(const uint8_t *encoded_data_string,
      size_t encoded_string_size) ;
  void get_client_processors_list();
  void get_wakeup_delivery_list();
  usta_logging_util *log_instance;
public:
  /**
   * This creates a connection with SSC via class ssc_connection
   *
   * @param suid_event_cb is call back function of type handle_suid_event_cb,
   *        which will be called as many number of times as the number of suid
   *        returned for a particular data_type
   */
  SuidSensor(handle_suid_event_cb suid_event_cb);
  /**
   * @brief This kills the connection with SSC
   */
  ~SuidSensor();
  /**
   * @brief sends a request to suid sensor for a suid list for a given datatype
   *
   *
   * @param data_type is the data type corresponding to which SUID's are
   *         required for complete sensor list, this should be '\0'
   */
  usta_rc send_request(string& data_type);


};
