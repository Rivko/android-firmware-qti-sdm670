/* ===================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* FILE: por.cpp
* DESC: parse/apply por_spec.txt file to sensor attributes
* ================================================================ */
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>

#include "see_salt.h"
#include "parse.h"

/*
* "see_salt_por_spec" : {
*   "target" : "identifier",
*   "sensors" : [
*     {
*       "TYPE" : e.g. "accel" | "gyro" | "mag" | "pressure" | "motion_detect" ...,
*       "NAME" : e.g. "bosch_bmp285" | "Sensirion SHTW2 driver" | ...,
*       "DRI" :  true | false,
*       "RIGID_BODY" : "display" | "keyboard" | "external",
*       "OVERRIDE_MAX_RATE" : number
*     },
*     ...
*   ]
* }
*/

using namespace std;

const string lit_see_salt_por_spec = "see_salt_por_spec";
const string lit_target = "target";
const string lit_sensors = "sensors";
const string lit_OVERRIDE = "OVERRIDE_";

/**
 * struct key value
 */
typedef struct kv {
   std::string key;     // attribute name
   std::string value;   // attribute value
} kv;

typedef struct {
   std::vector<kv> list;
} por_sensor;

void show_por_spec(string margin, por_sensor &por_spec)
{
   for (size_t i = 0; i < por_spec.list.size(); i++) {
      kv *entry = &por_spec.list[ i];
      cout << margin << entry->key << " : " << entry->value << endl;
   }
}

/**
 * @brief evaluate whether candidate matches por spec
 * @param psensor - sensor attributes for candidate matching TYPE
 * @param por_spec
 *
 * @return bool
 */
bool does_candidate_match_por_spec(sensor *psensor, por_sensor &por_spec)
{
   bool match = true;

   for (size_t i = 0; i < por_spec.list.size(); i++) {
      kv *entry = &por_spec.list[ i];
      if ( "TYPE" == entry->key) {
         // assert entry->value == psensor->get_type();
         continue;
      }
      else if ( "NAME" == entry->key) {
         string actual_name = psensor->get_name();
         if ( actual_name != entry->value) {
            match = false;
         }
      }
      else if ( "DRI" == entry->key) {
         if ( psensor->has_dri()) {
            bool actual_dri = psensor->get_dri();
            bool target_dri = atoi( entry->value.c_str());
            if (actual_dri == target_dri) {
               continue;
            }
         }
         match = false;
      }
      else if ( "RIGID_BODY" == entry->key) {
         if ( psensor->has_rigid_body()) {
            string actual_value = psensor->get_rigid_body();
            if ( actual_value == entry->value) {
               continue;
            }
         }
         match = false;
      }
      else if ( lit_OVERRIDE == entry->key.substr( 0,lit_OVERRIDE.length())) {
         ; // ignore
      }
      else {
         match = false;
         cout << "por spec not implemented for: " << entry->key << endl;
      }
   }
   return match;
}

/**
 * return the value of "TYPE" from the por_spec
 */
string get_por_type(por_sensor &por_spec)
{
   string target_type = "";
   for (size_t i = 0; i < por_spec.list.size(); i++) {
      kv *entry = &por_spec.list[ i];
      if ( string( "TYPE") == entry->key) {
         target_type = entry->value;
         break;
      }
   }
   return target_type;
}

/*
* apply "OVERRIDE_MAX_RATE" : number
*/
void apply_override(sensor *psensor, por_sensor &por_spec)
{
   for (size_t i = 0; i < por_spec.list.size(); i++) {
      kv *entry = &por_spec.list[ i];
      if ( lit_OVERRIDE == entry->key.substr( 0, lit_OVERRIDE.length())) {
         string shortname = entry->key.substr( lit_OVERRIDE.length());
         if ( shortname == string("MAX_RATE")) {
            /* --------------------
            * set psensor->max_rate
            */
            float max_rate = atof( entry->value.c_str());
            string attr_name = "SNS_STD_SENSOR_ATTRID_MAX_RATE";
            int attr_id = lookup_attr_id_num( attr_name);
            attribute a(attr_id);
            vtype v( max_rate);
            a.append_value(v);
            psensor->append_attribute(a);
         }
         else {
            cout << "POR OVERRIDE not implemented for " << entry->key << endl;
         }
      }
   }
}

/**
 * @brief append POR attribute to candidate matching por spec
 * @param por_spec - one complete por sensor entry.
 * @return bool
 */
bool apply_one_por(see_salt *psalt, por_sensor &por_spec)
{
   bool match = false;

   // get target sensor type
   string target_sensor = get_por_type( por_spec);
   if ( target_sensor != "") {
      // get vector of all suids for this sensor type
      vector<sens_uid *> sens_uids;
      psalt->get_sensors( target_sensor, sens_uids);
      if ( sens_uids.size()) {
         for (size_t i = 0; i < sens_uids.size(); i++) {
            sens_uid *candidate_suid = sens_uids[i];
            // get sensor
            sensor *psensor = psalt->get_sensor( candidate_suid);

            if ( does_candidate_match_por_spec( psensor, por_spec)) {
               if (psalt->get_debugging()) {
                  cout << "+ por match found for:" << endl;
                  cout << "+ suid high " << hex <<  candidate_suid->high;
                  cout << " low " << hex <<  candidate_suid->low << endl;
                  show_por_spec( string("+ "), por_spec);
               }
               match = true;
               /* --------------------------------
               * set psensor->por attribute = true
               */
               string attr_name = "SNS_STD_SENSOR_ATTRID_POR";
               int attr_id = lookup_attr_id_num( attr_name);
               attribute a(attr_id);
               vtype v(1);
               a.append_value(v);
               psensor->append_attribute(a);
               /*------------------------
               * apply supplied OVERIDE_s
               */
               apply_override( psensor, por_spec);
            }
         }
      }
   }
   if ( !match) {
      if ( psalt->get_debugging()) {
         cout << "-   por match NOT found for:" << endl;
         show_por_spec( string("-   "), por_spec);
      }
   }
   return match;
}


/**
 * class por extends json_message
 */
class por : public json_message {
public:
   por(MESSAGE json_msg) : json_message( json_msg) {}
   bool parse_sensors(see_salt *psalt);
   bool parse_single_spec(see_salt *psalt);
   bool parse_see_salt_por_spec(see_salt *psalt);
};

// cursor positioned at BEGIN_OBJECT somewhere within:
// "sensors" : [ {...}, ..., {...} ]
bool por::parse_single_spec(see_salt *psalt)
{
// cout << "parse_single_spec" << endl;
   char c = trim_peek_char();
   if ( c == BEGIN_OBJECT) {
      eat_char();

      token t;
      por_sensor por_spec;

      while ( true) {
         char c = trim_peek_char();
         if ( c == '"' && get_name_colon( t)) {
            token v;
            if ( get_next_token( v)) {

               kv entry;
               entry.key = t.literally;
               entry.value = v.literally;
               por_spec.list.push_back( entry);
            }
            else {
               break;
            }
         }

         c = trim_peek_char();
         if ( c == COMMA) {
            eat_char();
            continue;
         }
         break;
      }

      apply_one_por( psalt, por_spec);

      c = trim_peek_char();
      if ( c == END_OBJECT) {
         eat_char();
         return true;
      }
   }
   string tail = get_jmtail();
   cout << "syntax error near: " << tail.substr( 0, 64) << endl;
   return false;
}


// Cursor positioned at BEGIN_ARRAY in "sensors" : [ ...]
bool por::parse_sensors(see_salt *psalt)
{
// cout << "parse_sensors" << endl;
   char c = trim_peek_char();
   if ( c == BEGIN_ARRAY) {
      eat_char();

      while ( true) {
         // cursor at LEFT BRACE in "sensors": [ {
         if ( parse_single_spec(psalt)) {
            c = trim_peek_char();
            if ( c == COMMA) {
               eat_char();
               continue;
            }
         }
         break;
      }
      token t;
      if ( get_next_token(t) && t.type == T_END_ARRAY) {
         return true;
      }
   }
   return false;
}

// cursor postioned at BEGIN_OBJECT in "see_salt_por_spec" : {
bool por::parse_see_salt_por_spec(see_salt *psalt)
{
   cout << "parse_see_salt_por_spec" << endl;
   token t;

   get_next_token( t);
   if ( t.type == T_BEGIN_OBJECT) {

      // "target" : "target_name",
      if ( get_name_colon( t, lit_target)) {
         if ( get_next_token(t) && t.type == T_STRING) {
            string target_name = t.literally;
            cout << "target_name: " << t.literally << endl;
            eat_char( COMMA);

            // "sensors" : [
            if ( get_name_colon( t, lit_sensors)) {
               // cursor positioned at "sensors" : [ <- BEGIN ARRAY
               if ( parse_sensors(psalt)) {
                  if ( get_next_token(t) && t.type == T_END_OBJECT) {
                     return true;
                  }
               }
            }
         }
      }
   }
   return false;
}

/**
 * @brief - parse the por_spec.txt file located in /data/sensors and set the
 *        SNS_STD_SENSOR_ATTRID_POR in matching sensors
 */
int parse_por( see_salt *psalt)
{
    char *json_fn = (char *)"/data/sensors/por_spec.txt";
    cout << "por file: " << json_fn;

    ifstream in_file;
    in_file.open( json_fn);
    if (in_file.fail()) {
       cout << " not found." << endl;
       return 4;
    }
    cout << " found." << endl;
    stringstream buffer;
    buffer << in_file.rdbuf();
    in_file.close();

    string jmsg = buffer.str();

    MESSAGE msg;
    msg.pcursor = jmsg.c_str();
    msg.len = jmsg.size();

    por parse( msg);

    while (true) {
       token t;
       if ( parse.get_name_colon( t)) {
          if ( lit_see_salt_por_spec == t.literally) {
             parse.parse_see_salt_por_spec(psalt);
          }
       }
       else if ( t.type == T_BEGIN_OBJECT) {
          parse.skip_object( END_OBJECT);
       }
       else if ( t.type == T_BEGIN_ARRAY) {
          parse.skip_object( END_ARRAY);
       }

       // silently consume everything else :-)

       char c = parse.peek_char();
       if ( c == '\0') {
          break; // eof
       }
    }

    cout << endl;
    return 0;
}

