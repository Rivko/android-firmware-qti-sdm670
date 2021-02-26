/* ===================================================================
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: salt_usta
** DESC: salt interface with USTA: libUSTANative.so
** ================================================================ */
#include <string>
#include <sstream>
#include <iostream>

#include "see_salt.h"

using namespace std;
#include "sensor_cxt.h"
#include "logging_util.h"
#include "usta_rc.h"
#include "salt_usta.h"

// forward declaration
static std::string usta_rc_to_string(usta_rc rc);

static usta_logging_util* log_instance = NULL;
static bool usta_logging = true;

static string unknown_axis("MULTISHAKE_AXIS_UNKNOWN_AXIS");
static string type_stationary("SNS_PSMD_TYPE_STATIONARY");

// enable or disable usta logging
void set_usta_logging( bool enable)
{
   usta_logging = enable;  // 1 == enabled, 0 == disabled
   if ( log_instance) {
      bool disabled = !usta_logging;
      log_instance->update_logging_flag( disabled);
   }
}

// vector holds SensorCxt instance per see_salt_instance_num_num;
static std::vector<SensorCxt *> sns_cxts;

static SensorCxt* usta_get_instance(see_salt *psalt)
{
   if ( log_instance == NULL) {
      log_instance = usta_logging_util::create_instance();
      bool disabled = !usta_logging;
      log_instance->update_logging_flag( disabled);
      sns_cxts.clear();
   }
   // note: first salt_instance_num == 1
   int salt_instance_num = psalt->get_instance_num();

   if ( salt_instance_num > (int)sns_cxts.size()) {
      SensorCxt *sns_cxt_instance = SensorCxt::getInstance();
      sns_cxts.push_back( sns_cxt_instance);
      // assert sns_cxts.size() == salt_instance_num
   }
   return sns_cxts[ salt_instance_num - 1];
}

void usta_destroy_instance( see_salt *psalt)
{
   if ( psalt) {
      SensorCxt *sns_cxt_instance = usta_get_instance( psalt);
      int salt_instance_num = psalt->get_instance_num();
      if (salt_instance_num > 0) {
         sns_cxts[ salt_instance_num - 1] = nullptr;
      }
      if ( sns_cxt_instance) {
         delete sns_cxt_instance;
      }
   }
}

salt_rc usta_get_sensor_list(see_salt *psalt,
                             vector<sensor_info>& sensor_list)
{
   usta_rc rc = (usta_rc)SALT_RC_NULL_ARG;
   if ( psalt) {
      SensorCxt *sns_cxt_instance = usta_get_instance( psalt);
      if (sns_cxt_instance) {
         rc = sns_cxt_instance->get_sensor_list( sensor_list);
      }
   }
   return (salt_rc)rc;
}

salt_rc usta_get_request_list(see_salt *psalt,
                              unsigned int handle)
{
   vector<msg_info> req_msgs;
   usta_rc rc = (usta_rc)SALT_RC_NULL_ARG;

   if ( psalt) {
      SensorCxt *sns_cxt_instance = usta_get_instance( psalt);
      if ( sns_cxt_instance) {
         rc = sns_cxt_instance->get_request_list(handle, req_msgs);
      }

      if ( psalt->get_debugging()) {
         cout << "msg_info" << endl;
         for ( size_t i = 0; i < req_msgs.size(); i++) {
            cout << "  ." << i << ". " << "msg_name: " << req_msgs[ i].msg_name;
            cout << ", msgid: " << req_msgs[ i].msgid << endl;
            for ( size_t j = 0; j < req_msgs[i].fields.size(); j++) {
               field_info field = req_msgs[i].fields[j];
               cout << "      field.name: " << field.name << endl;
               if ( field.is_user_defined_msg) {
                  for ( size_t k = 0; k < field.nested_msgs.size(); k++) {
                     nested_msg_info nested = field.nested_msgs[k];
                     cout << "            nested.msg_name: " << nested.msg_name;
                     cout << endl;
                     for ( size_t m = 0; m < nested.fields.size(); m++) {
                        field_info nfield = nested.fields[m];
                        cout << "                  .nfield.name: ";
                        cout << nfield.name << endl;
                     }
                  }
               }
            }
         }
         cout << endl;
      }
   }
   return (salt_rc)rc;
}

salt_rc usta_get_sttributes(see_salt *psalt,
                            unsigned int handle,
                            string& attribute_list)
{
   usta_rc rc = (usta_rc)SALT_RC_NULL_ARG;
   if ( psalt) {
      SensorCxt *sns_cxt_instance = usta_get_instance( psalt);
      if (sns_cxt_instance) {
         rc = sns_cxt_instance->get_attributes( handle, attribute_list);
      }
   }
   return (salt_rc)rc;
}

salt_rc usta_stop_request(see_salt *psalt,
                          unsigned int handle)
{
   usta_rc rc = (usta_rc)SALT_RC_NULL_ARG;
   if ( psalt) {
      SensorCxt *sns_cxt_instance = usta_get_instance( psalt);
      if (sns_cxt_instance) {
         rc = sns_cxt_instance->stop_request( handle, true);
      }
   }
   return (salt_rc)rc;
}

/**
 * @brief call libUSTANative.so SensorCxt::send_request(...)
 * @param handle - index of sensor
 * @param client_req
 * @return salt_rc
 */
salt_rc usta_send_request( see_salt *psalt,
                           unsigned int handle,
                           see_client_request_message &client_req)
{
    if (!psalt) {
       return SALT_RC_NULL_ARG;
    }

    see_std_request *std_req = client_req.get_request();

    /* init client_req_msg_fields*/
    // perhaps change name client_msg to client_msg_fields
    client_req_msg_fileds client_msg;
    see_std_client_processor_e client;
    see_client_delivery_e wakeup;
    client_req.get_suspend_config( client, wakeup);
    client_msg.client_type = client_req.get_client_symbolic( client);
    client_msg.wakeup_type = client_req.get_wakeup_symbolic( wakeup);

    if ( std_req->has_batch_period()) {
       int batch_period = std_req->get_batch_period();
       client_msg.batch_period = to_string( batch_period);
    }
    if ( std_req->has_flush_period()) {
       int flush_period = std_req->get_flush_period();
       client_msg.flush_period = to_string( flush_period);
    }
    if ( std_req->has_flush_only()) {
       int flush_only = std_req->get_flush_only();
       client_msg.flush_only = to_string( flush_only);
    }
    if ( std_req->has_max_batch()) {
       int max_batch = std_req->get_max_batch();
       client_msg.max_batch = to_string( max_batch);
    }

    send_req_msg_info msg_info;
    see_msg_id_e msg_id = client_req.get_msg_id();
    msg_info.msgid = to_string( msg_id);

    if ( std_req->is_payload_std_sensor_config()) {
       see_std_sensor_config *std_sensor_config =
           std_req->get_payload_std_sensor_config();

       send_req_field_info field_info;
       float rate = std_sensor_config->get_sample_rate();
       field_info.name = "sample_rate";
       field_info.value.push_back( to_string(rate));

       msg_info.msg_name = "sns_std_sensor_config";
       msg_info.fields.push_back( field_info);
    }
    else if ( std_req->is_payload_self_test_config()) {
       see_self_test_config *self_test_config =
                             std_req->get_payload_self_test_config();

       send_req_field_info field_info;
       see_self_test_type_e test_type = self_test_config->get_test_type();
       string s = self_test_config->get_test_type_symbolic( test_type);
       field_info.name = "test_type";
       field_info.value.push_back( s);

       msg_info.msg_name = "sns_physical_sensor_test_config";
       msg_info.fields.push_back( field_info);
    }
    else if ( std_req->is_payload_resampler_config()) {

       see_resampler_config *resampler_config =
            std_req->get_payload_resampler_config();

       msg_info.msg_name = "sns_resampler_config";

       /* sensor_uid */
       send_req_field_info suid_field;
       suid_field.name = "sensor_uid";

       send_req_nested_msg_info sensor_uid_msg;
       sensor_uid_msg.msg_name = "sns_std_suid";

       sens_uid *target = resampler_config->get_suid();

       send_req_field_info suid_low_field;
       suid_low_field.name = "suid_low";
       suid_low_field.value.push_back(to_string(target->low));

       send_req_field_info suid_high_field;
       suid_high_field.name = "suid_high";
       suid_high_field.value.push_back(to_string(target->high));

       sensor_uid_msg.fields.push_back(suid_low_field);
       sensor_uid_msg.fields.push_back(suid_high_field);
       suid_field.nested_msgs.push_back(sensor_uid_msg);
       msg_info.fields.push_back(suid_field);

       /* resampled_rate */
       send_req_field_info rate_field;
       rate_field.name = "resampled_rate";
       float rate = resampler_config->get_resampled_rate();
       rate_field.value.push_back( to_string(rate));
       msg_info.fields.push_back( rate_field);

       /* rate_type */
       send_req_field_info type_field;
       type_field.name = "rate_type";
       see_resampler_rate rtype = resampler_config->get_rate_type();
       string lit_rtype = resampler_config->get_rate_type_symbolic( rtype);
       type_field.value.push_back( lit_rtype);
       msg_info.fields.push_back( type_field);

       /* filter */
       send_req_field_info filter_field;
       filter_field.name = "filter";
       bool filter = resampler_config->get_filter();
       filter_field.value.push_back( to_string(filter));
       msg_info.fields.push_back( filter_field);
    }
    else if ( std_req->is_payload_diag_log_trigger_req()) {
       see_diag_log_trigger_req *diag_log_trigger_req =
           std_req->get_payload_diag_log_trigger_req();

       msg_info.msg_name = "sns_diag_log_trigger_req";

       if ( diag_log_trigger_req->has_cookie()) {
          send_req_field_info cookie_field_info;
          cookie_field_info.name = "cookie";
          uint64_t cookie = diag_log_trigger_req->get_cookie();
          cookie_field_info.value.push_back( to_string(cookie));
          msg_info.fields.push_back( cookie_field_info);
       }

       send_req_field_info log_type_field_info;
       log_type_field_info.name = "log_type";
       string s = diag_log_trigger_req->get_log_type_symbolic();
       log_type_field_info.value.push_back( s);
       msg_info.fields.push_back( log_type_field_info);
    }
    else if ( std_req->is_payload_set_distance_bound()) {
       see_set_distance_bound *set_distance_bound =
                             std_req->get_payload_set_distance_bound();

       send_req_field_info field_info;
       float distance_bound = set_distance_bound->get_distance_bound();
       field_info.name = "distance_bound";
       field_info.value.push_back( to_string( distance_bound));

       msg_info.msg_name = "sns_set_distance_bound";
       msg_info.fields.push_back( field_info);
    }
    else if ( std_req->is_payload_basic_gestures_config()) {
       send_req_field_info field_info;
       field_info.name = "";
       msg_info.msg_name = "sns_basic_gestures_config";
       msg_info.fields.push_back( field_info);
    }
    else if ( std_req->is_payload_multishake_config()) {
       send_req_field_info field_info;
       field_info.name = "shake_axis";
       field_info.value.push_back( unknown_axis);
       msg_info.msg_name = "sns_multishake_config";
       msg_info.fields.push_back( field_info);
    }
    else if ( std_req->is_payload_psmd_config()) {
       send_req_field_info field_info;
       field_info.name = "type";
       field_info.value.push_back( type_stationary);
       msg_info.msg_name = "sns_psmd_config";
       msg_info.fields.push_back( field_info);
    }
    else if ( msg_id == MSGID_FLUSH_REQ
              || msg_id == MSGID_DISABLE_REQ
              || msg_id == MSGID_ON_CHANGE_CONFIG) {
       ; // aok
    }
    else {
       int payload_type = std_req->get_payload_type();
       cout << "usta_send_request() unknown payload type: "
            << to_string( payload_type) << endl; // debug
    }

    string logfile_suffix = "see_salt.json";
    string usta_mode( "C" + to_string( psalt->get_instance_num()));

    SensorCxt *sns_cxt_instance = usta_get_instance( psalt);
    usta_rc rc = sns_cxt_instance->send_request( handle,
                                                 msg_info,
                                                 client_msg,
                                                 logfile_suffix,
                                                 usta_mode);
    if ( rc) {
       cout << "usta_send_request rc " << to_string(rc) << " - ";
       cout <<  usta_rc_to_string(rc) << endl << endl;
    }
    return (salt_rc)rc;
}

static std::string usta_rc_to_string(usta_rc rc)
{
   if ( rc == USTA_RC_SUCCESS) {
      return string( "USTA_RC_SUCCESS");
   }
   else if ( rc == USTA_RC_FAILED) {
      /* operation failed ; cannot continue */
      return string( "USTA_RC_FAILED");
   }
   else if ( rc == USTA_RC_DESC_FAILED) {
      /* descriptor unavailable */
      return string( "USTA_RC_DESC_FAILED");
   }
   else if ( rc == USTA_RC_NO_MEMORY) {
      /* Memory Allocation Failed */
      return string( "USTA_RC_NO_MEMORY");
   }
   else if ( rc == USTA_RC_NO_PROTO) {
      /* proto files not found in desccriptor pool */
      return string( "USTA_RC_NO_PROTO");
   }
   else if ( rc == USTA_RC_NO_REQUEST_MSG) {
      /* No request messages found */
      return string( "USTA_RC_NO_REQUEST_MSG");
   }
   else if ( rc == USTA_RC_ATTRIB_FAILED) {
      /* requested attribute not found */
      return string( "USTA_RC_ATTRIB_FAILED");
   }
   else if ( rc == USTA_RC_REQUIIRED_FILEDS_NOT_PRESENT) {
      /*Required fileds are missing in the proto files*/
      return string( "USTA_RC_ATTRIB_FAILED");
   }
   else if ( rc == SALT_RC_NULL_ARG) {
      /*Required argument is null*/
      return string( "SALT_RC_NULL_ARG");
   }
   else {
      return string(" unknown rc " + to_string( rc));
   }
}
