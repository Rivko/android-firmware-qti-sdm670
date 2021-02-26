/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAEXTRULECONFIG_H_
#define _VAEXTRULECONFIG_H_
#pragma once

#include <VAM/vaapi.h>
#include <ConfigBase.h>
#include <string>
#include <vector>
#include <map>

struct VAMExtRuleConfigInfo {
    bool isEnable;
    vaapi_event_type eventType;
    int sensitivity;

    VAMExtRuleConfigInfo();
    void reset();
    void print();
};

class VAMExtRuleConfig: public ConfigBase {
 public:
    VAMExtRuleConfig();
    virtual void print();

    std::vector<VAMExtRuleConfigInfo> _configInfoList;

 protected:
    virtual int _processItems(std::ifstream *pConfigFile, std::string tagStr);
    virtual int _validateConfig();

 private:
    std::map<std::string, int> _RuleConfigField;
    std::map<std::string, int> _EventTagToEventType;

    VAMExtRuleConfigInfo currentConfig;
};

#endif  // _VAEXTRULEONFIG_H_
