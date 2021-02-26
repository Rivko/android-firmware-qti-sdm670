/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "ConfigBase.h"


ConfigBase::ConfigBase() :
    _isInitialized(false) {
}

int ConfigBase::readConfig(std::string configFileName, int debugLevel) {
    if (configFileName.size() == 0 || configFileName == "") {
        return VS_OK;
    }

    std::ifstream configFile;
    configFile.open(configFileName.c_str());
    if (configFile.is_open() == false) {
        std::cout << "ConfigBase Error: failed to load the config file ";
        std::cout << configFileName << std::endl;
        return VS_ERROR;
    }

    while (!configFile.eof()) {
        std::string tagStr;
        getline(configFile, tagStr);
        tagStr = trimWhiteSpace(tagStr);

        // skip all comments
        if (isComment(tagStr) || isBlank(tagStr)) {
            continue;
        }

        int ret = _processItems(&configFile, tagStr);
        if (ret != VS_OK) {
            break;
        }
    }

    if (debugLevel > 0) {
        std::cout << "===ConfigBase::readConfig():===" << std::endl;
        print();
        std::cout << "===ConfigBase::readConfig():===End" << std::endl;
    }

    _validateConfig();

    configFile.close();
    _isInitialized = true;

    return VS_OK;
}

std::string ConfigBase::trimWhiteSpace(const std::string& str,
                                       const std::string& whitespace) {
    const size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) {
        return "";  // no content
    }

    const size_t strEnd = str.find_last_not_of(whitespace);
    const size_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string ConfigBase::EventTypeToStr(vaapi_event_type eventType) {
    switch (static_cast<int>(eventType)) {
        case vaapi_event_type_none :
            return "vaapi_event_type_none";
        case vaapi_event_camera_tamper_detected :
            return "vaapi_event_camera_tamper_detected";
        case vaapi_event_motion_detected :
            return "vaapi_event_motion_detected";
        case vaapi_event_line_crossed :
            return "vaapi_event_line_crossed";
        case vaapi_event_intrusion_detected :
            return "vaapi_event_intrusion_detected";
        case vaapi_event_object_counted :
            return "vaapi_event_object_counted";
        case vaapi_event_loitering_detected :
            return "vaapi_event_loitering_detected";
        case vaapi_event_face_detected :
            return "vaapi_event_face_detected";
        case vaapi_event_face_recognized :
            return "vaapi_event_face_recognized";
        case vaapi_event_object_classified :
            return "vaapi_event_object_classified";
        case vaapi_event_object_detected :
            return "vaapi_event_object_detected";
        case vaapi_event_compound_event :
            return "vaapi_event_compound_event";
        case vaapi_event_extension_reserved_1 :
            return "vaapi_event_extension_reserved_1";
        case vaapi_event_extension_reserved_2 :
            return "vaapi_event_extension_reserved_2";
        case vaapi_event_extension_reserved_3 :
            return "vaapi_event_extension_reserved_3";
        case vaapi_event_extension_reserved_4 :
            return "vaapi_event_extension_reserved_4";
        case vaapi_event_extension_reserved_5 :
            return "vaapi_event_extension_reserved_5";
        case vaapi_event_size :
            return "vaapi_event_size";
    }

    return "unsupported event type";
}
