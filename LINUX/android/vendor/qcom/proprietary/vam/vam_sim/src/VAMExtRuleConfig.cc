/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "VAMExtRuleConfig.h"
#include "VASimError.h"

enum VAMExtRuleConfigTypeEnum {
    ruleConfigField_isEnabled,
    ruleConfigField_sensitivity
};

VAMExtRuleConfigInfo::VAMExtRuleConfigInfo() {
    reset();
}

void VAMExtRuleConfigInfo::reset() {
    isEnable = false;
    eventType = vaapi_event_type_none;
    sensitivity = 50;
}

void VAMExtRuleConfigInfo::print() {
    std::cout << "  Rule Config for ";
    std::cout << ConfigBase::EventTypeToStr(eventType) << std::endl;
    std::cout << "    isEnable:" << isEnable << std::endl;
    std::cout << "    eventType:" << eventType << std::endl;
    std::cout << "    sensitivity:" << sensitivity << std::endl;
}

VAMExtRuleConfig::VAMExtRuleConfig() {
    _RuleConfigField["<Is Enable>"] = ruleConfigField_isEnabled,
    _RuleConfigField["<Sensitivity>"] = ruleConfigField_sensitivity;

    _EventTagToEventType["<Face Detection>"] = vaapi_event_face_detected;
    _EventTagToEventType["<Object Tracking>"] = vaapi_event_object_detected;
}

void VAMExtRuleConfig::print() {
    std::cout << "===VAMExtRuleConfig============================begin";
    std::cout << std::endl;
    for (size_t i = 0; i < _configInfoList.size(); i++) {
        _configInfoList[i].print();
    }
    std::cout << "===VAMExtRuleConfig============================end";
    std::cout << std::endl << std::endl;
}

int VAMExtRuleConfig::_processItems(std::ifstream *pConfigFile,
                                    std::string tagStr) {
    if (pConfigFile == nullptr) {
        return VS_NULLPTR;
    }

    std::ifstream &configFile = *pConfigFile;

    // whether is start tag
    if (_EventTagToEventType.find(tagStr) != _EventTagToEventType.end()) {
        currentConfig.reset();
        currentConfig.eventType =
                (vaapi_event_type)_EventTagToEventType.find(tagStr)->second;
        return VS_OK;
    }

    // whether is end tag
    if (tagStr.size() > 3) {
        std::string tagStrCpy(tagStr);
        tagStrCpy.erase(tagStrCpy.begin() + 1);
        if (currentConfig.isEnable &&
            _EventTagToEventType.find(tagStrCpy) !=
                    _EventTagToEventType.end()) {
            _configInfoList.push_back(currentConfig);
            currentConfig.reset();
            return VS_OK;
        }
    }

    // whether is rule field
    if (_RuleConfigField.find(tagStr) == _RuleConfigField.end()) {
        return VS_OK;
    }

    // skip all comments
    std::string valueStr;
    do {
        getline(configFile, valueStr);
        valueStr = trimWhiteSpace(valueStr);
    } while (!configFile.eof() && isComment(valueStr));

    if (isBlank(valueStr)) {
        return VS_OK;
    }

    std::stringstream ss(valueStr);
    if (ss.eof()) {
        return VS_OK;
    }

    int number;
    switch (_RuleConfigField.find(tagStr)->second) {
        case ruleConfigField_isEnabled:
            currentConfig.isEnable = valueStr == "enable";
            break;
        case ruleConfigField_sensitivity:
            ss >> number;
            currentConfig.sensitivity = std::max(0, std::min(100, number));
            break;
        default:
            break;
    }

    return VS_OK;
}

int VAMExtRuleConfig::_validateConfig() {
    if (_configInfoList.size() == 0) {
        std::cout << "VAMExtRuleConfig Warning: none of VA rule is enabled";
        std::cout << std::endl;
        return VS_ERROR;
    }

    return VS_OK;
}
