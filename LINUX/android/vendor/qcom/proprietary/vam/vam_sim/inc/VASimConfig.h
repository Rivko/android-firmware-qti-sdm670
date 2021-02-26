/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VASIMCONFIG_H_
#define _VASIMCONFIG_H_

#include <vector>
#include <map>
#include <string>
#include <ConfigBase.h>

class VASimConfig: public ConfigBase {
 public:
    VASimConfig();
    int parseArgv(int argc, char **argv);
    virtual void print();

    bool enableDisplay;
    bool enableSnapshot;
    bool enableFR;
    bool exitOnEOF;

    unsigned int videosPerRow;
    unsigned int filmStripNum;
    unsigned int videoRowsPerColumn;
    unsigned int debugLevel;
    std::string VAMRuleConfigFileName;
    std::string VAMExtRuleConfigFileName;
    std::string MetadataOutputFolder;
    std::string EventOutputFolder;
    std::string DynVAEngineFolder;
    std::string DynVAEngineDataFolder;
    std::string FREnrollmentFileName;
    std::string FREnrollmentFolder;
    std::vector<std::string> inputFileNameList;

 protected:
    virtual int _processItems(std::ifstream *pConfigFile, std::string tagStr);
    virtual int _validateConfig();

 private:
    std::map<std::string, int> _VASimConfigMap;
};

#endif  // _VASIMCONFIG_H_
