/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __SETTINGSD_H_INCL__
#define __SETTINGSD_H_INCL__

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <map>

using namespace std;

#define DEFAULT_PROP "default"
#define MCC_MNC_MAP_FILE  "mcc_mnc_map.txt"
#define ICCID_MAP_FILE    "iccid_map.txt"
#define MAX_INSTANCE_ID 3

namespace QcSettingsD {

class settingsD {
    map<string, string> mcc_mnc_map;
    map<string, string> iccid_map;
    unordered_map<string, string> property_map;
    unordered_map<string, string> carrier_key_map[MAX_INSTANCE_ID];
    unordered_map<string, string> carrier_delta_key_map[MAX_INSTANCE_ID];
    vector<string> m_carrier_name;
    vector<string> m_mcc_mnc_pair;
    vector<string> m_carrier_file_name;
    vector<string> m_carrier_delta_file_name;
    void init(const std::string &root);

    void load_prop(const string filename, const string carrier, unordered_set<string> &changed, unsigned instance_id);
    void load_delta_prop(const string file_name, unordered_set<string> &changed, unsigned instance_id);
    void clear_carrier_prop(unordered_set<string> &changed, unsigned instance_id);
    void clear_delta_prop(unordered_set<string> &changed, unsigned instance_id);
    void reset_default_prop(const string carrier, unordered_set<string> &changed);
    bool parse_line(const string &line, string &key, string &value);
    void process_key(string &key, unsigned instance_id);

    void load_mcc_mnc_map();
    void load_iccid_map();
public:
    static settingsD &getInstance();
    settingsD();
    settingsD(const string filename);
    ~settingsD();
    void load_default_prop(const string filename);
    void load_carrier_prop(unsigned instance_id, const string &mcc, const string &mnc, const string &iccid);
    void notify_changed(const unordered_set<string> &changed);
    void notify_start();
};

}

#endif // __SETTINGSD_H_INCL__
