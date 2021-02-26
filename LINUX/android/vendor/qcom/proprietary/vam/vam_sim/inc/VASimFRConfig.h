/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VASIMFRCONFIG_H_
#define _VASIMFRCONFIG_H_

#include <vector>
#include <string>
#include <ConfigBase.h>

struct VASimFRConfig_enrollmentInfo {
    std::string uuid;
    std::string displayName;
    std::string imageName;

    void print();
};

class VASimFRConfig: public ConfigBase {
 public:
    VASimFRConfig();
    virtual void print();

    std::vector<VASimFRConfig_enrollmentInfo> enrollmentList;

 protected:
    virtual int _processItems(std::ifstream *pConfigFile, std::string str);
};

#endif  // _VASIMFRCONFIG_H_
