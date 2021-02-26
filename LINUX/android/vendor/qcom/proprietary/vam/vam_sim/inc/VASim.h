/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VASIM_H_
#define _VASIM_H_

#include <string>
#include <vector>
#include <fstream>
#include "VASimConfig.h"
#include "VASimFRConfig.h"
#include "VAMJSONRuleConfig.h"
#include "VAMExtRuleConfig.h"

struct VASimParams {
    bool isStop;
    VASimConfig config;
    VASimFRConfig FRConfig;
    VAMJSONRuleConfig JSONRuleConfig;
    VAMExtRuleConfig extRuleConfig;
    std::ofstream metadataFileWriter;
    std::ofstream eventFileWriter;
    bool isNewEventFile;
    bool isNewMetadataFile;
    bool isRawVideo;

    VASimParams();
    ~VASimParams();
    int init(int argc, char **argv);
};

#endif  // _VASIM_H_
