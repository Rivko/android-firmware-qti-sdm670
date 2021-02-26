/** ===================================================================
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * FILE: parse.h
 * DESC: base class for parsing json messages
 * ================================================================ */
#pragma once

#include "see_salt.h"

const int T_UNDEFINED      = 1;
const int T_NAME           = 2;       // "name"
const int T_COLON          = 3;       // ':'
const int T_VALUE          = 4;
const int T_COMMA          = 6;       // ','
const int T_BEGIN_OBJECT   = 7;       // '{'
const int T_END_OBJECT     = 8;       // '{'
const int T_BEGIN_ARRAY    = 9;       // '['
const int T_END_ARRAY      = 10;      // '['
const int T_STRING         = 11;      // "text"
const int T_NUMBER         = 12;      // number
const int T_HEX_NUMBER     = 13;      // hex number
const int T_SYMBOL         = 14;      // alphanumeric
const int T_TRUE           = 15;      // 'true'
const int T_FALSE          = 16;      // 'false'
const int T_NULL           = 17;

const char COLON = ':';       // aka name_separator
const char COMMA = ',';       // aka value_separator
const char ESCAPE = '\\';
const char MINUS = '-';
const char PLUS  = '+';
const char PERIOD = '.';
const char QUOTE = '"';
const char BEGIN_OBJECT = '{';
const char END_OBJECT = '}';
const char BEGIN_ARRAY = '[';
const char END_ARRAY = ']';

const char LIT_TRUE[] = "true";
const int  LEN_LIT_TRUE = 4;
const char LIT_FALSE[] = "false";
const int  LEN_LIT_FALSE = 5;

const std::string lit_sns_client_event_msg = "sns_client_event_msg";
const std::string lit_suid = "suid";
const std::string lit_suid_low = "suid_low";
const std::string lit_suid_high = "suid_high";
const std::string lit_events = "events";
const std::string lit_msg_id = "msg_id";
const std::string lit_timestamp = "timestamp";
const std::string lit_payload = "payload";
const std::string lit_attributes = "attributes";
const std::string lit_attr_id = "attr_id";
const std::string lit_value = "value";
const std::string lit_values = "values";

const std::string lit_boolean = "boolean";
const std::string lit_flt = "flt";
const std::string lit_sint = "sint";
const std::string lit_str = "str";
const std::string lit_subtype = "subtype";

const int SEE_STD_ATTR_EVENT = 128;

class token {
public:
   int   type;                // T_TYPEs
   const char  *pbegin;       // beginning of name, object, or array token
   int   for_len;             // trailing length of json message
   std::string literally;     // body of name, object, or array
                              // + with leading/trailing
                              // + quotes/braces/brackets stripped
   token() {
      type = 0;
      pbegin = NULL;
      for_len = 0;
      literally = "";
   }
};

typedef struct {
   const char *pcursor;
   int   len;
} MESSAGE;

class json_message {
public:
    json_message(MESSAGE json_msg);

    char peek_char();
    char trim_peek_char();
    void eat_char();
    void eat_char(int len);
    void eat_char(char expected);
    void ltrim();
    char consume_digits();
    void get_number(token &t, char c);
    void get_hex_number(token &t);
    void get_symbol( token &t);
    void get_name(token &t);  // parse quoted name
    void skip_object(char terminal);
    bool is_0x();
    bool get_next_token(token &t);
    bool get_name_colon(token &t);
    bool get_name_colon( token &t, std::string field_name);
    bool parse_suid(sens_uid &suid);
    bool parse_int(const std::string &field_name, uint64_t *value);
    bool parse_attr_values_body(attribute &a);
    bool parse_attr_values(attribute &a);
    bool parse_attr_value(attribute &a);
    bool parse_single_attribute(sensor &s);
    bool parse_attributes(sensor &s);
    bool parse_payload(see_salt *psalt,sens_uid &suid, int msg_id);
    bool parse_client_event(see_salt *psalt,sens_uid &suid);
    bool parse_sns_client_event_msg(see_salt *psalt);
    std::string get_jmtail();
private:
    MESSAGE jm;
};


