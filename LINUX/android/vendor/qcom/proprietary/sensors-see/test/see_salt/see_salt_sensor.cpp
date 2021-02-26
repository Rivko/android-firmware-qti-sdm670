/** ===================================================================
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * FILE: see_salt_sensor.cpp
 * DESC: sensor/attribute name<to>id mapping/getters/setters
 * ================================================================ */
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

#include "see_salt.h"

using namespace std;

const string lit_undef( "() on undefined attribute");

/*
* containers for mapping between attribute names and attribute id numbers
*/
static int done_once = false;
static std::vector<std::string> attr_num_to_name;
static std::map<std::string,int> attr_name_to_num;

//constructor
attribute::attribute(int _attr_id)
{
   attr_id = _attr_id;
}

void attribute::append_value(vtype &v)
{
   values.push_back( v);
}

std::string attribute::get_name()
{
   std::string attr_name;
   try {
      return ( attr_num_to_name.at( attr_id));
   }
   catch ( std::out_of_range) {
      return "";
   }
}

//constructor
sensor::sensor(sens_uid &suid)
{
   _suid = suid;
   data_type = "";      // <-- need to set this too
}

void sensor::set_suid(sens_uid &suid)
{
   _suid = suid;
}

void sensor::append_attribute(attribute &attr)
{
   int attr_id = attr.get_attr_id();
   _attribute_list.insert(std::make_pair( attr_id, attr));
}

/**
 * @brief constants from sns_std_sensor.proto
 */
const int SNS_STD_SENSOR_ATTRID_NAME = 0;
const int SNS_STD_SENSOR_ATTRID_VENDOR = 1;
const int SNS_STD_SENSOR_ATTRID_TYPE = 2;
const int SNS_STD_SENSOR_ATTRID_AVAILABLE = 3;
const int SNS_STD_SENSOR_ATTRID_VERSION = 4;
const int SNS_STD_SENSOR_ATTRID_API = 5;
const int SNS_STD_SENSOR_ATTRID_RATES = 6;
const int SNS_STD_SENSOR_ATTRID_RESOLUTIONS = 7;
const int SNS_STD_SENSOR_ATTRID_FIFO_SIZE = 8;
const int SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT = 9;
const int SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT = 10;
const int SNS_STD_SENSOR_ATTRID_RANGES = 11;
const int SNS_STD_SENSOR_ATTRID_OP_MODES = 12;
const int SNS_STD_SENSOR_ATTRID_DRI = 13;
const int SNS_STD_SENSOR_ATTRID_STREAM_SYNC = 14;
const int SNS_STD_SENSOR_ATTRID_EVENT_SIZE = 15;
const int SNS_STD_SENSOR_ATTRID_STREAM_TYPE = 16;
const int SNS_STD_SENSOR_ATTRID_DYNAMIC = 17;
const int SNS_STD_SENSOR_ATTRID_HW_ID = 18;
const int SNS_STD_SENSOR_ATTRID_RIGID_BODY = 19;
const int SNS_STD_SENSOR_ATTRID_PLACEMENT = 20;
const int SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR = 21;
const int SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS = 22;
const int SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION = 23;
const int SNS_STD_SENSOR_ATTRID_SELECTED_RANGE = 24;

const int SNS_STD_SENSOR_ATTRID_POR = 25;
const int SNS_STD_SENSOR_ATTRID_MAX_RATE = 26;

// Stream types
const int SNS_STD_SENSOR_STREAM_TYPE_STREAMING = 0;
const int SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE = 1;
const int SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT = 2;

// Rigid body types
const int SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY = 0;
const int SNS_STD_SENSOR_RIGID_BODY_TYPE_KEYBOARD = 1;
const int SNS_STD_SENSOR_RIGID_BODY_TYPE_EXTERNAL = 2;

/**
 * @brief display user friendly description for some int attributes
 * @param attr_id
 * @param v - value
 * @return string
 */
std::string sensor::explain_attribute(int attr_id, int v)
{
   std::string s;

   if ( attr_id == SNS_STD_SENSOR_ATTRID_AVAILABLE
        || attr_id == SNS_STD_SENSOR_ATTRID_DRI
        || attr_id == SNS_STD_SENSOR_ATTRID_DYNAMIC
        || attr_id == SNS_STD_SENSOR_ATTRID_STREAM_SYNC) {
      s = (v) ? "true" : "false";
   }
   else if ( attr_id == SNS_STD_SENSOR_ATTRID_VERSION) {
       uint16_t minor = v & 0xff;
       uint16_t major = ( v & 0xff00) >> 8;
       s = std::to_string( major) + "." + std::to_string( minor);
   }
   else if ( attr_id == SNS_STD_SENSOR_ATTRID_STREAM_TYPE) {
       if ( v == SNS_STD_SENSOR_STREAM_TYPE_STREAMING) {
           s = "streaming";
       }
       else if ( v == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE) {
           s = "on_change";
       }
       else if ( v == SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT) {
           s = "single_output";
       }
       else {
           s = "unknown";
       }
   }
   else if ( attr_id == SNS_STD_SENSOR_ATTRID_RIGID_BODY) {
       if ( v == SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY) {
           s = "display";
       }
       else if ( v == SNS_STD_SENSOR_RIGID_BODY_TYPE_KEYBOARD) {
           s = "keyboard";
       }
       else if ( v == SNS_STD_SENSOR_RIGID_BODY_TYPE_EXTERNAL) {
           s = "external";
       }
       else {
           s = "unknown";
       }
   }
   else {
       s = std::to_string( v);
   }
   return s;
}

void sensor::show_attributes()
{
   // _attribute_list map ordered by attr_id
   for (std::map<int,attribute>::iterator alit = _attribute_list.begin();
         alit != _attribute_list.end();
         alit++) {

      int attr_id = alit->first;
      attribute *pat = &alit->second;
      std::string attr_name = pat->get_name();

      std::cout << "attr_id " << attr_name;
      std::cout << "( " << std::dec << attr_id << ")" << " = ";
      size_t count = pat->values_size();

      if (count > 1) {
         std::cout << "[";
      }
      for ( size_t j = 0; j < count; j++) {
         if ( j) {
            std::cout << ", ";
         }
         vtype vt = pat->get_value( j);
         if (vt.has_sint()) {
            int i = vt.sint();
            std::cout << sensor::explain_attribute( attr_id, i);
         }
         else if (vt.has_flt()) {
            float f = vt.flt();
            std::cout << std::to_string( f);
         }
         else if (vt.has_str()) {
            std::string s = vt.str();
            std::cout << s;
         }
         else {
            std::cout << "unknown vtype";
         }
      }
      if (count > 1) {
         std::cout << "]";
      }
      std::cout << std::endl;
   }
}
/**
 * @brief lookup attribute by attr_id
 * @return NULL or pointer to attribute
 */

attribute *sensor::get_attribute(int attr_id)
{
   attribute *pat = NULL;
   std::map<int,attribute>::iterator alit;
   alit = _attribute_list.find( attr_id);
   if ( alit != _attribute_list.end()) {
       pat = &alit->second;
   }
   return pat;
}

void init_attr()
{
   if (done_once == false) {
      done_once = true;
      // predefined names copied from sns_std_sensor.proto
      attr_num_to_name.reserve(24);
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_NAME");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_VENDOR");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_TYPE");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_AVAILABLE");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_VERSION");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_API");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_RATES");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_RESOLUTIONS");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_FIFO_SIZE");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_RANGES");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_OP_MODES");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_DRI");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_STREAM_SYNC");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_EVENT_SIZE");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_STREAM_TYPE");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_DYNAMIC");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_HW_ID");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_RIGID_BODY");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_PLACEMENT");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_SELECTED_RANGE");

      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_POR");
      attr_num_to_name.push_back("SNS_STD_SENSOR_ATTRID_MAX_RATE");

      for ( size_t attr_id = 0; attr_id < attr_num_to_name.size(); attr_id++) {
         std::string attr_name = attr_num_to_name[attr_id];
         attr_name_to_num.insert(std::make_pair( attr_name, attr_id));
      }
   }
}

/**
 * @brief lookup attribute id by attribute name
 * @param attr_name
 * @return int attr_id
 */
int lookup_attr_id_num(std::string &attr_name)
{
   int attr_id = -1;
   std::map<std::string,int>::iterator it = attr_name_to_num.find(attr_name);
   if ( it != attr_name_to_num.end()) {
      attr_id = it->second;
   }
   else {
      // dynamically assign attr_id for new names
      attr_id = attr_name_to_num.size();
      attr_id++;
      attr_name_to_num.insert(std::make_pair( attr_name, attr_id));
      attr_num_to_name[attr_id] = attr_name;
   }
   return attr_id;
}

/**
 * @brief get all public sensor data_types
 * @param [io] vector<string> &sensor_types gets populated with all available
 *       sensor data_types
 */
void see_salt::get_sensors(std::vector<std::string> &sensor_types)
{
   sensor_types.clear();
   for ( std::vector<sensor>::iterator slit = _sensor_list.begin();
         slit != _sensor_list.end();
         slit++) {

      attribute *pat = slit->get_attribute(SNS_STD_SENSOR_ATTRID_TYPE);
      if ( pat != NULL) {
         vtype vt = pat->get_value(0);
         if ( vt.has_str()) {
            std::string data_type = vt.str();
            sensor_types.push_back( data_type);
         }
      }
   }
}

/**
 * @brief populate vector with pointers to public suids
 * @param [io] vector<sens_uid *> populated with pointers to sensor uids
 */
void see_salt::get_sensors(std::vector<sens_uid *> &sens_uids)
{
   sens_uids.clear();
   for ( std::vector<sensor>::iterator slit = _sensor_list.begin();
         slit != _sensor_list.end();
         slit++) {

      sens_uid *suid = slit->get_suid();
      sens_uids.push_back( suid);
   }
}


/**
 * @brief populate vector with pointers to suids for the input sensor_type
 * @param [i] string sensor_type
 * @param [io] vector<sens_uid *> populated with pointers to sensor uids
 */
void see_salt::get_sensors(std::string sensor_type,
                           std::vector<sens_uid *> &sens_uids)
{
   sens_uids.clear();
   for ( std::vector<sensor>::iterator slit = _sensor_list.begin();
         slit != _sensor_list.end();
         slit++) {

      attribute *pat = slit->get_attribute(SNS_STD_SENSOR_ATTRID_TYPE);
      if ( pat != NULL) {
         vtype vt = pat->get_value(0);
         if ( vt.has_str()) {
            if ( sensor_type == vt.str()) {
               sens_uid *suid = slit->get_suid();
               sens_uids.push_back( suid);
            }
         }
      }
   }
}

/**
 * @brief locate sensor for input suid
 * @param [i] pointer to sens_uid
 * @return NULL or pointer to sensor for the input suid
 */
sensor *see_salt::get_sensor(sens_uid *suid)
{
   for ( size_t i = 0; i < _sensor_list.size(); i++) {
      sensor *psens = &(_sensor_list[i]);

      sens_uid *target = psens->get_suid();
      if ( suid->low == target->low &&
           suid->high == target->high){
         return psens;
      }
   }
   return NULL;
}
// attribute helpers
static std::string ret_str_attr(attribute *pat)
{
   if ( pat != NULL) {
      vtype vt = pat->get_value(0);
      if ( vt.has_str()) {
         return vt.str();
      }
   }
   return "";
}

static int ret_sint_attr(attribute *pat)
{
   if ( pat != NULL) {
      vtype vt = pat->get_value(0);
      if ( vt.has_sint()) {
         return vt.sint();
      }
   }
   return 0;
}

static float ret_flt_attr(attribute *pat)
{
   if ( pat != NULL) {
      vtype vt = pat->get_value(0);
      if ( vt.has_flt()) {
         return vt.flt();
      }
   }
   return 0.0;
}

// populate vector<float>
static void ret_vflt_attr(attribute *pat, std::vector<float> &vectr)
{
   vectr.clear();
   if ( pat != NULL) {
      for ( size_t i = 0; i < pat->values_size(); i++) {
         vtype vt = pat->get_value(i);
         if ( vt.has_flt()) {
            vectr.push_back( vt.flt());
         }
      }
   }
}

// populate vector<string>
static void ret_vstr_attr(attribute *pat, std::vector<std::string> &vectr)
{
   vectr.clear();
   if ( pat != NULL) {
      for ( size_t i = 0; i < pat->values_size(); i++) {
         vtype vt = pat->get_value(i);
         if ( vt.has_str()) {
            vectr.push_back( vt.str());
         }
      }
   }
}

/**
 * @brief attribute getters
 */
bool sensor::has_name()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_NAME));
}
std::string sensor::get_name()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_NAME);
   return ret_str_attr( pat);
}

bool sensor::has_vendor()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_VENDOR));
}
std::string sensor::get_vendor()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_VENDOR);
   return ret_str_attr( pat);
}

bool sensor::has_type()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_TYPE));
}
std::string sensor::get_type()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_TYPE);
   return ret_str_attr( pat);
}

bool sensor::has_available()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_AVAILABLE));
}
bool sensor::get_available()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_AVAILABLE);
   return ret_sint_attr( pat);
}

bool sensor::has_apis()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_API));
}
void sensor::get_apis(std::vector<std::string> &apis)
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_API);
   return ret_vstr_attr( pat, apis);
}
/* ------------------------
* begin optional attributes
*/
bool sensor::has_dri()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_DRI));
}
bool sensor::get_dri()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_DRI);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ret_sint_attr( pat);
}

bool sensor::has_fifo_size()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_FIFO_SIZE));
}
int sensor::get_fifo_size()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_FIFO_SIZE);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ret_sint_attr( pat);
}

bool sensor::has_stream_sync()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_STREAM_SYNC));
}
bool sensor::get_stream_sync()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_STREAM_SYNC);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ret_sint_attr( pat);
}

bool sensor::has_dynamic()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_DYNAMIC));
}
bool sensor::get_dynamic()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_DYNAMIC);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ret_sint_attr( pat);
}
bool sensor::has_event_size()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_EVENT_SIZE));
}
int  sensor::get_event_size()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_EVENT_SIZE);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ret_sint_attr( pat);
}

bool sensor::has_sleep_current()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT));
}
int  sensor::get_sleep_current()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ret_sint_attr( pat);
}

bool sensor::has_hw_id()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_HW_ID));
}
int  sensor::get_hw_id()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_HW_ID);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ret_sint_attr( pat);
}

bool sensor::has_rates()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_RATES));
}
void sensor::get_rates(std::vector<float> &rates)
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_RATES);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   ret_vflt_attr( pat, rates);
}

bool sensor::has_placement()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_PLACEMENT));
}
void sensor::get_placement(std::vector<float> &matrix)
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_PLACEMENT);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   ret_vflt_attr( pat, matrix);
}

bool sensor::has_rigid_body()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_RIGID_BODY));
}

/* <display, keyboard, external> */
std::string sensor::get_rigid_body()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_RIGID_BODY);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   int v = ret_sint_attr( pat);
   return explain_attribute( SNS_STD_SENSOR_ATTRID_RIGID_BODY, v);
}

bool sensor::has_physical_sensor()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR));
}
bool sensor::get_physical_sensor()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ret_sint_attr( pat);
}

bool sensor::has_stream_type()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_STREAM_TYPE));
}

bool sensor::is_stream_type_streaming()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_STREAM_TYPE);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ( ret_sint_attr( pat) == SNS_STD_SENSOR_STREAM_TYPE_STREAMING);
}

bool sensor::is_stream_type_on_change()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_STREAM_TYPE);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ( ret_sint_attr( pat) == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE);
}

bool sensor::is_stream_type_single_output()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_STREAM_TYPE);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ( ret_sint_attr( pat) == SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT);
}

bool sensor::has_por()
{
   return ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_POR));
}
bool sensor::get_por()
{
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_POR);
   if ( pat == NULL) {
      string func(__func__);
      throw std::runtime_error( func + lit_undef);
   }
   return ret_sint_attr( pat);
}

bool sensor::has_max_rate()
{
   if ( NULL != get_attribute(SNS_STD_SENSOR_ATTRID_MAX_RATE)) {
      return true;
   }
   return has_rates();
}
float sensor::get_max_rate()
{
   // if max_rate set, return it
   attribute *pat = get_attribute(SNS_STD_SENSOR_ATTRID_MAX_RATE);
   if ( NULL != pat) {
      return ret_flt_attr( pat);
   }
   // otherwise return max ODR from rates
   std::vector<float> rates;
   get_rates( rates);
   if ( rates.size()) {
      return rates[ rates.size() - 1];
   }
   return 0.0;
}
