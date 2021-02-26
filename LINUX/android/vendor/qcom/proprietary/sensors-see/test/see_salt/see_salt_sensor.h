/** ===================================================================
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * FILE: see_salt_sensor.h
 * DESC: class definitions for the SEE sensor testing
 * REFERENCE: sns_std_sensor.proto
 * ================================================================ */
#pragma once
#include <string>

/**
 * public functions
 */
extern void init_attr();
extern int lookup_attr_id_num( std::string &attr_name);

/**
 * @brief Struct to represent sensor's unique ID (128-bit)
 */
struct sens_uid
{
    sens_uid() : low(0), high(0) {}
    sens_uid(uint64_t low, uint64_t high): low(low), high(high) {}
    uint64_t low, high;
};

/**
 * @brief class vtype holds a single sensor attribute value
 */
class vtype {
public:
   vtype() { _type = UNDEF;}
   vtype( int i) { _type = SINT; _v.sint = i;}
   vtype( float f) { _type = FLT; _v.flt = f;}
   vtype( std::string *s) { _type = STR; _v.str = s;}
   ~vtype() {}
   bool has_sint() { return _type == SINT; }
   bool has_flt() { return _type == FLT; }
   bool has_str() { return _type == STR; }
   int sint() {return _v.sint;}
   std::string str() {return *_v.str;}
   float flt() {return _v.flt;}

private:
   enum vtype_e {
      UNDEF,
      SINT,
      FLT,
      STR,
   };

   union value {
      int   sint;
      float   flt;
      std::string   *str;
   };

   vtype_e  _type;
   value _v;
};

/**
 * @brief class attribute holds all of the values for a single attribute
 */
class attribute {
public:
   attribute(int attr_id);
   void append_value(vtype &);
   size_t values_size() { return values.size();}
   int get_attr_id() { return attr_id;}
   std::string get_name();
   vtype get_value(size_t i) { return values[i];}
private:
   int      attr_id;
   std::vector<vtype> values;
};

/**
 * @brief class sensor holds all of the attributes for a single sensor
 */
class sensor {
public:
   sensor(sens_uid &);
   void set_suid(sens_uid &);
   sens_uid *get_suid() { return &_suid;}
   attribute *get_attribute(int attr_id);
   void append_attribute(attribute &);
   std::string explain_attribute(int attr_id, int v);
   void show_attributes();
   size_t attribute_count() { return _attribute_list.size(); }

   // required attribute getters
   bool        has_name();
   std::string get_name(); // chip name
   bool        has_vendor();
   std::string get_vendor();
   bool        has_type();
   std::string get_type(); // e.g. "accel", "gyro", "mag", ...
   bool        has_available();
   bool        get_available();
   bool        has_apis();
   void        get_apis(std::vector<std::string> &); // proto file name(s)

   // optional attribute getters
   bool        has_dri();
   bool        get_dri();
   bool        has_stream_sync();
   bool        get_stream_sync();
   bool        has_dynamic();
   bool        get_dynamic();
   bool        has_event_size();
   int         get_event_size();
   bool        has_fifo_size();
   int         get_fifo_size();
   bool        has_sleep_current();
   int         get_sleep_current();
   bool        has_hw_id();
   int         get_hw_id();

   bool        has_rates();
   void        get_rates(std::vector<float> &); // hz
   bool        has_placement();
   void        get_placement(std::vector<float> &);

   bool        has_stream_type();
   bool        is_stream_type_streaming();
   bool        is_stream_type_on_change();
   bool        is_stream_type_single_output();

   bool        has_rigid_body();
   std::string get_rigid_body(); // display, keyboard, external

   bool        has_physical_sensor();
   bool        get_physical_sensor();

   bool        has_por();
   bool        get_por();

   bool        has_max_rate();
   float       get_max_rate();

//    bool        has_resolutions();
//    int         get_resolutions(std::vector<float> &);
//    bool        has_ranges();
//    int         get_ranges(std::vector<float> &);
//    bool        has_op_modes();
//    int         get_op_modes(std::vector<std::string> &);
//    bool        has_active_current();
//    int         get_active_current(std::vector<int> &);

private:
   sens_uid _suid;
   std::string data_type;
   int   _handle; // sensor handle for USTA
   std::map<int,attribute> _attribute_list; // int attrib_id
};

