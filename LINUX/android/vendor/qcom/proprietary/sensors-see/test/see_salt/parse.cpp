/** ===================================================================
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * FILE: parse.cpp
 * DESC: parse json format sns_client_event_msg for attributes
 * REFERENCE: www.json.org Introducing JSON
 * ================================================================ */
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdexcept>

#include "see_salt.h"
#include "salt_usta.h"
#include "parse.h"

/* por.cpp*/
extern int parse_por(see_salt *psalt);

using namespace std;

// constructor
json_message::json_message(MESSAGE json_msg)
{
   jm.pcursor = json_msg.pcursor;
   jm.len = json_msg.len;
}

string json_message::get_jmtail()
{
   return string ( jm.pcursor, jm.len);
}

// returns character at cursor
char json_message::peek_char()
{
   if (jm.len > 0){
      return *jm.pcursor;
   }
   return '\0';
}

// eats left blanks then returns character at cursor
char json_message::trim_peek_char()
{
   ltrim();
   if (jm.len > 0){
      return *jm.pcursor;
   }
   return '\0';
}

// advance cursor
void json_message::eat_char()
{
   if (jm.len > 0){
      jm.pcursor++;
      jm.len--;
   }
}

// advance cursor by len
void json_message::eat_char(int len)
{
   if (jm.len >= len){
      jm.pcursor += len;
      jm.len -= len;
   }
}

// if cursor points to expected character then advance cursor
// else throw error
void json_message::eat_char(char expected)
{
   if (jm.len > 0){
      char c = *jm.pcursor;
      if ( c == expected) {
         eat_char();
      }
      else {
         char buffer[40];
         sprintf( buffer, "eat_char expected \'%c\', got 0x%2.2x\n",
                  expected, c);
         throw runtime_error(buffer);
      }
   }
}

// remove leading spaces
void json_message::ltrim()
{
   while (jm.len > 0 && isspace( *jm.pcursor)){
      eat_char();
   }
}

// '0' at *jm.pcursor has already been confirmed
bool json_message::is_0x()
{
   if ( jm.len >= 3) {
      char c = *(jm.pcursor + 1);
      if ( c == 'x' || c == 'X') {
         return true;
      }
   }
   return false;
}

// consume digits returning the first non-digit character
char json_message::consume_digits()
{
   while ( jm.len > 0) {
      char c = peek_char();
      if ( !isdigit(c)) {
         return c;
      }
      eat_char();
   }
   return '\0';
}
//
void json_message::get_number(token &t, char c)
{
   if ( c == MINUS) {
      eat_char();
   }
   c = consume_digits();      // consume int portion
   if ( c == PERIOD) {        // consume frac
      eat_char();
      c = consume_digits();
   }
   if ( c == 'e' || c == 'E') { // consume exp
      eat_char();
      c = peek_char();
      if ( c == PLUS || c == MINUS) {  // consume optional + or -
         eat_char();
      }
      c = consume_digits();
   }

   const char *body_start = t.pbegin;
   size_t body_len = ( jm.pcursor - t.pbegin);
   string literally( body_start, body_len);
   t.literally = literally;
}

// jm.pcursor points to the 0x
void json_message::get_hex_number(token &t)
{
   eat_char(2);      // consume the '0x'
   while( char c = peek_char()) {
      if ( isxdigit( c)) {
         eat_char();
      }
      else {
         break;
      }
   }

   const char *body_start = t.pbegin;
   size_t body_len = ( jm.pcursor - t.pbegin);
   string literally( body_start, body_len);
   t.literally = literally;
}

// jm.pcursor points to an unquoted alpha numeric string
void json_message::get_symbol(token &t)
{
   while( char c = peek_char()) {
      if ( c == ',' || c == '\n' || c =='}') {
         break;
      }
      eat_char();
   }

   const char *body_start = t.pbegin;
   size_t body_len = ( jm.pcursor - t.pbegin);
   string literally( body_start, body_len);
   t.literally = literally;
}

// input state: cursor points to leading quote in: "name"
void json_message::get_name(token &t)
{
   eat_char();  // consume leading "

   char c = '\0';
   while ( jm.len > 0) {
      c = peek_char();
      if ( c == QUOTE) {
         break;
      }
      if ( c == ESCAPE) {
         eat_char();      // todo adhere to legal escaped chars spec
      }
      eat_char(); // todo adhere to legal escaped chars spec
   }
   if ( c == QUOTE) {
      eat_char();
      const char *body_start = t.pbegin + 1;
      size_t body_len = ( jm.pcursor - t.pbegin) - 2;
      string literally( body_start, body_len);
      t.literally = literally;
   }
   else {
      throw runtime_error("get_name runaway");
   }
}

// cursor positioned at character after leading BEGIN_OBJECT or BEGIN_ARRAY
void json_message::skip_object(char terminal)
{
    char c = '\0';
    while ( jm.len > 0) {
       c = peek_char();
       if ( c == terminal) {
          break;
       }
       if ( c == BEGIN_OBJECT) {
          eat_char();
          skip_object( END_OBJECT);
       }
       if ( c == BEGIN_ARRAY) {
          eat_char();
          skip_object( END_ARRAY);
       }
       else if ( c == QUOTE) {
          token skip;                    // skip over embedded "name"
          skip.pbegin = jm.pcursor;      //
          skip.for_len = jm.len;         //
          get_name(skip);
       }
       else {
          eat_char();
       }
    }
    if ( c == terminal) {
       eat_char();
    }
    else {
       // todo show t.pbegin t.for_len runaway object, array, name
       throw runtime_error( "skip_object runaway");
    }
}

bool json_message::get_next_token(token &t)
{
   ltrim();

   t.type = T_UNDEFINED;
   t.pbegin = jm.pcursor;      // beginning of token parse
   t.for_len = jm.len;         //
   bool rc = true;

   char c = peek_char();
   if ( c == QUOTE) {
      t.type = T_STRING;
      get_name( t);
   }
   else if ( c == BEGIN_OBJECT) {
      t.type = T_BEGIN_OBJECT;
      eat_char();
   }
   else if ( c == END_OBJECT) {
      t.type = T_END_OBJECT;
      eat_char();
   }
   else if ( c == BEGIN_ARRAY) {
      t.type = T_BEGIN_ARRAY;
      eat_char();
   }
   else if ( c == END_ARRAY) {
      t.type = T_END_ARRAY;
      eat_char();
   }
   else if ( c == COMMA) {
      t.type = T_COMMA;
      eat_char();
   }
   else if ( c == COLON) {
      t.type = T_COLON;
      eat_char();
   }
   else if ( c == '0' && is_0x()) {
      t.type = T_HEX_NUMBER;
      get_hex_number( t);
   }
   else if ( isdigit(c) || c == MINUS) {
      t.type = T_NUMBER;
      get_number( t, c);
   }
   else if ( jm.len >= LEN_LIT_TRUE
             && 0 == strncmp(jm.pcursor, LIT_TRUE, LEN_LIT_TRUE)) {
      t.type = T_TRUE;
      t.literally = "1";
      eat_char( LEN_LIT_TRUE);
   }
   else if ( jm.len >= LEN_LIT_FALSE
             && 0 == strncmp(jm.pcursor, LIT_FALSE, LEN_LIT_FALSE)) {
      t.type = T_FALSE;
      t.literally = "0";
      eat_char( LEN_LIT_FALSE);
   }
   else if ( isalnum(c) || c == '_') {
      t.type = T_SYMBOL;
      get_symbol( t);
   }
   else {
      rc = false;
   }

   ltrim();

   return rc;
}

// expecting json_message cursor at "name" : <value>
bool json_message::get_name_colon(token &t)
{
   get_next_token( t);
   if ( t.type == T_STRING) {
      token c;
      get_next_token( c);
      if ( c.type == T_COLON) {
         return true;
      }
   }
   return false;
}

// expecting json_message cursor at "field_name" : <value>
bool json_message::get_name_colon(token &t, string field_name)
{
   get_next_token( t);
   if ( t.type == T_STRING && t.literally == field_name) {
      token c;
      get_next_token( c);
      if ( c.type == T_COLON) {
         return true;
      }
   }
   return false;
}

/**
 * @brief populate suid from the json message. Cursor positioned at first quote
 *        in "suid" : {
 * @param suid
 * @return bool
 */
bool json_message::parse_suid(sens_uid &suid)
{
   token t;
   if ( get_name_colon(t, lit_suid)) {
      if ( get_next_token(t) && t.type == T_BEGIN_OBJECT) {
         if ( get_name_colon(t, lit_suid_low)) {
            if ( get_next_token( t) && t.type == T_STRING) {
               string low = t.literally;
               suid.low = strtoull( t.literally.c_str(), NULL, 0);
               eat_char(COMMA);

               if ( get_name_colon(t, lit_suid_high)) {
                  if ( get_next_token( t) && t.type == T_STRING) {
                     string high = t.literally;
                     suid.high = strtoull( t.literally.c_str(), NULL, 0);
                     if ( get_next_token(t) && t.type == T_END_OBJECT) {
                        #if 0
                        cout << "suid txt: [" << high.c_str() << ", "
                                              << low.c_str()  << "]" << endl;
                        printf("suid num: [%" PRIx64 " %" PRIx64 "]\n",
                               suid.high, suid.low);
                        #endif
                        return true;
                     }
                  }
               }
            }
         }
      }
   }
   return false;
}

/**
 * @brief populate value with number from "fieldname" : number. Cursor
 *        positioned at first quote in "fieldname" : number
 * @param const string expected "fieldname"
 * @param uint64_t *value gets value
 * @return bool
 */
bool json_message::parse_int(const string &field_name, uint64_t *value)
{
   token t;
   if ( get_name_colon( t, field_name)) {
      get_next_token( t);
      if ( t.type == T_NUMBER) {
         *value = strtol( t.literally.c_str(), NULL, 10);
         #if 0
         cout << "parse_int:" << t.literally << " == " << *value << endl;
         #endif
         return true;
      }
   }
   return false;
}

// cursor at BEGIN_OBJECT in "values" : [ {
bool json_message::parse_attr_values_body(attribute &a)
{
// cout << "parse_attr_values_body" << endl;
   while ( true) {
      char c = trim_peek_char();
      if ( c == BEGIN_OBJECT) {
         eat_char();
         c = trim_peek_char();
         if ( c == END_OBJECT) {
            ; //  todo null / omitted value
         }
         else {
            token t;
            if ( get_name_colon( t)) {
               token v;
               get_next_token( v);
               #if 0
               cout << t.literally << ":  ";
               if ( "" != v.literally) {
                  cout << v.literally;
               }
               cout << endl;
               #endif
               if ( lit_sint == t.literally) {
                  int sint = strtol( v.literally.c_str(), NULL, 10);
                  vtype value( sint);
                  a.append_value( value);
               }
               else if ( lit_flt == t.literally) {
                  float f = atof( v.literally.c_str());
                  vtype value( f);
                  a.append_value( value);
               }
               else if ( lit_str == t.literally) {
                  if ( v.type == T_STRING || v.type == T_SYMBOL) {
                     std::string *s = new string(v.literally);
                     vtype value( s);
                     a.append_value( value);
                  }
               }
               else if ( lit_boolean == t.literally) {
                  int sint = atoi( v.literally.c_str());
                  vtype value( sint);
                  a.append_value( value);
               }
               else if ( lit_subtype == t.literally) {
                  // v.type == T_END_OBJECT
                  parse_attr_values(a); // cursor at "values" : [
                  ltrim();
                  eat_char( END_OBJECT);
               }
               else {
                  cout << "parse_attr_values_body unknown type: ";
                  cout << t.literally << endl;
               }
            }
         }
         c = trim_peek_char();
         if ( c == END_OBJECT) {
            eat_char();
            c = trim_peek_char();
            if ( c == COMMA) {
               eat_char();
               continue;
            }
            return true;
         }
      }
   }
   return false;
}

// cursor at "values" : [
bool json_message::parse_attr_values(attribute &a)
{
// cout << "parse_attr_values" << endl;
   token t;
   if ( get_name_colon( t, lit_values)) {
      char c = peek_char();
      if ( c == BEGIN_ARRAY) {
         eat_char();
         // cursor at BEGIN_OBJECT in "values" : [ {
         if ( parse_attr_values_body(a)) {
            c = trim_peek_char();
            if ( c == END_ARRAY) {
               eat_char();
               return true;
            }
         }
      }
   }
   return false;
}

// cursor at "value" : {
bool json_message::parse_attr_value(attribute &a)
{
// cout << "parse_attr_value" << endl;

   token t;
   if ( get_name_colon( t, lit_value)) {
      char c = peek_char();
      if ( c == BEGIN_OBJECT) {
         eat_char();
         if ( parse_attr_values(a)) { // cursor at "values" : [

            c = trim_peek_char();
            if ( c == END_OBJECT) {
               eat_char();
               return true;;
            }
         }
      }
   }
   return false;
}

// cursor at LEFT BRACE in "attributes" : [ { ...]
bool json_message::parse_single_attribute(sensor &s)
{
// cout << "parse_single_attribute" << endl;
   char c = trim_peek_char();
   if ( c == BEGIN_OBJECT) {
      eat_char();

      token t;
      if ( get_name_colon( t, lit_attr_id)) {
         c = trim_peek_char();
         if ( c == QUOTE && get_next_token(t) && t.type == T_STRING) {
            eat_char( COMMA);

            string attr_name = t.literally;
//          cout << "attr_id: " << t.literally << endl;
            int attr_id = lookup_attr_id_num( attr_name);

            attribute a(attr_id);
//          cout << "======= attribute: " << buffer << " =======" << endl;
            if ( parse_attr_value(a)) { // cursor at "value" : {
               s.append_attribute(a);
            }
         }
         else {
            cout << "parse error: invalid \"attr_id\" : name " << endl;
            skip_object( END_OBJECT);
            return true;      // true to retain sensor
         }
      }

      c = trim_peek_char();
      if ( c == END_OBJECT) {
         eat_char();
         return true;;
      }
   }
   return false;
}

// Cursor positioned at BEGIN_ARRAY in "attributes" : [ ...]
bool json_message::parse_attributes(sensor &s)
{
// cout << "parse_attributes" << endl;
   char c = trim_peek_char();
   if ( c == BEGIN_ARRAY) {
      eat_char();

      while ( true) {
         // cursor at BEGIN_OBJECT in "attributes" : [ {
         parse_single_attribute( s);
         c = trim_peek_char();
         if ( c == COMMA) {
            eat_char();
            continue;
         }
         break;
      }
      c = trim_peek_char();
      if ( c == END_ARRAY) {
         eat_char();
         return true;;
      }
   }
   return false;
}

/**
 * @brief Cursor positioned at BEGIN_OBJECT in "payload" : { ... }
 * @param sens_uid provided the payload
 * @param msg_id identifies payload content
 * @return bool
 */
bool json_message::parse_payload(see_salt *psalt, sens_uid &suid, int msg_id)
{
// cout << "parse_payload" << endl;
   if ( msg_id == SEE_STD_ATTR_EVENT) {
      char c = peek_char();
      if ( c == BEGIN_OBJECT) {   // consume payload's openning {
         eat_char();
         token t;
         if ( get_name_colon( t, lit_attributes)) { // "attributes" : [

            sensor s( suid);
            bool rc = parse_attributes( s);  // cursor at [
            if ( rc) {
               psalt->append_sensor(s);
               return true;
            }
         }
      }
   }
   return false;
}

// cursor positioned at BEGIN_OBJECT in "events : [ { ...}
bool json_message::parse_client_event(see_salt *psalt, sens_uid &suid)
{
// cout << "parse_client_event" << endl;
   uint64_t msg_id = 0;
   uint64_t timestamp = 0;

   token t;
   get_next_token( t);
   if ( t.type == T_BEGIN_OBJECT) {
      if ( parse_int( lit_msg_id, &msg_id)) {
         eat_char( COMMA);
         if ( parse_int( lit_timestamp, &timestamp)) {
            eat_char( COMMA);

            if ( get_name_colon( t, lit_payload)) {       // "payload" : {
               return parse_payload( psalt, suid, msg_id);
            }
         }
      }
   }

   return false;
}

// cursor postioned after "sns_client_event_msg" : { <- at BEGIN_OBJECT
bool json_message::parse_sns_client_event_msg(see_salt *psalt)
{
// cout << "parse_client_event_msg" << endl;
   sens_uid suid;
   token t;
   get_next_token( t);
   if ( t.type == T_BEGIN_OBJECT) {
      parse_suid( suid);
      eat_char(COMMA);
      get_name_colon( t);               // "events" : [
      if ( lit_events == t.literally) {
         get_next_token( t);
         if ( t.type == T_BEGIN_ARRAY) {
            while ( parse_client_event(psalt, suid)) {     // cursor points to '{'
               char c = peek_char();
               if ( c == END_ARRAY) {
                  eat_char();
                  break;
               }
               if ( c == COMMA) {
                  eat_char();
               }
            }
         }
      }
      return true;
   }
   return false;
}

/**
 * @brief populate see_salt::_sensor_list
 */
int see_salt::begin(int argc, char *argv[]) // deprecated
{
   return begin();
}

int see_salt::begin()
{
   init_attr();   // init attribute name <-> num cross reference

   cout << "begin usta_get_sensor_list" << endl;
   vector<sensor_info> sensor_list;
   usta_get_sensor_list( this, sensor_list);
   cout << "end   usta_get_sensor_list" << endl;

   for ( size_t handle = 0; handle < sensor_list.size(); handle++) {
      string data_type = sensor_list[ handle].data_type;
      if ( get_debugging()) {
         cout << "handle " << handle << ": " << data_type << endl;
      }
      usta_get_request_list( this, handle);
   }

   for ( size_t handle = 0; handle < sensor_list.size(); handle++) {
      string attribute_list;
      salt_rc rc = usta_get_sttributes( this, handle, attribute_list);
      #if 0
      cout << "usta_get_attributes rc " << to_string( rc) << endl;
      cout << "usta_get_attributes attribute_list.length() ";
      cout << to_string( attribute_list.length()) << endl;
      #endif

      MESSAGE msg;
      msg.pcursor = attribute_list.c_str();
      msg.len = attribute_list.size();

      json_message parse( msg);

      while (true) {
         token t;
         if ( parse.get_name_colon( t)) {
            if ( lit_sns_client_event_msg == t.literally) {
               parse.parse_sns_client_event_msg(this);
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
   }

   parse_por( this);

   if (get_debugging()) {
      cout << "=== sensor list ===" << endl;
      for ( std::vector<sensor>::iterator it = _sensor_list.begin();
            it != _sensor_list.end();
            it++) {

         sens_uid *suid = it->get_suid();
         cout << "suid = [ high " << hex << suid->high;
         cout << " low " << hex << suid->low << "]" << endl;

         it->show_attributes();
         cout << endl;
      }
   }

   return 0;
}


