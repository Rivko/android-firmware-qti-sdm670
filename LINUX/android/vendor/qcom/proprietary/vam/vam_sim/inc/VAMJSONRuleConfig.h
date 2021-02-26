/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VARULECONFIG_H_
#define _VARULECONFIG_H_
#pragma once

#include <VAM/vaapi.h>
#include <ConfigBase.h>
#include <string>

class VAMJSONRuleConfig: public ConfigBase {
 public:
    VAMJSONRuleConfig();
    virtual ~VAMJSONRuleConfig();

    virtual int readConfig(std::string configFileName, int debugLevel = 0);
    virtual void print();

    vaapi_configuration vaapiConfig;
};

#endif  // _VASRULEONFIG_H_
