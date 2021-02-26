/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <VASimFRConfig.h>
#include <VASimError.h>


void VASimFRConfig_enrollmentInfo::print() {
    std::cout << "    uuid: " << uuid.c_str() << std::endl;
    std::cout << "    displayName: " << displayName.c_str() << std::endl;
    std::cout << "    imageName: " << imageName.c_str() << std::endl;
}

VASimFRConfig::VASimFRConfig() {
    std::cout << "VASimFRConfig::~VASimFRConfig()" << std::endl;
}

void VASimFRConfig::print() {
    std::cout << "===VASimFRConfig=============================begin";
    std::cout << std::endl;
    for (size_t idx = 0; idx < enrollmentList.size(); idx++) {
        std::cout << "  enrollment " << idx << std::endl;
        enrollmentList[idx].print();
    }

    std::cout << "===VASimFRConfig=============================end";
    std::cout << std::endl << std::endl;
}

int VASimFRConfig::_processItems(std::ifstream */*pConfigFile*/,
                                 std::string str) {
    VASimFRConfig_enrollmentInfo info;
    std::string *s[3] = {
        &info.uuid,
        &info.displayName,
        &info.imageName};
    size_t idx = 0;

    str = trimWhiteSpace(str);
    std::size_t found = str.find(',');
    while (found != std::string::npos && idx < 3) {
        std::string field = str.substr(0, found);
        field = trimWhiteSpace(field);

        *s[idx++] = field;

        str = str.substr(found+1);
        found = str.find(',');
    }

    if (idx < 3) {
        str = trimWhiteSpace(str);
        std::cout << "%s" << str.c_str() << std::endl;
        *s[idx] = str;
    }

    enrollmentList.push_back(info);

    return VS_OK;
}

