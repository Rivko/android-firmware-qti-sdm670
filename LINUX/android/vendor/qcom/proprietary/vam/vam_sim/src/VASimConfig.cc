/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "VASimConfig.h"
#include "VASimError.h"

enum VASimConfigTypeEnum {
    configType_fileList,
    configType_dynVAEngineFolder,
    configType_dynVAEngineDataFolder,
    configType_enableDisplay,
    configType_enableSnapshot,
    configType_enableFR,
    configType_videosPerRows,
    configType_filmStripNum,
    configType_videoRowsPerColumn,
    configType_exitOnEOF,
    configType_debugLevel,
    configType_VARuleConfigFile,
    configType_VAMExtRuleConfigFile,
    configType_MetadataOutputFile,
    configType_EventOutputFolder
};

VASimConfig::VASimConfig() :
    enableDisplay(false),
    enableSnapshot(true),
    enableFR(false),
    exitOnEOF(true),
    videosPerRow(1),
    filmStripNum(0),
    videoRowsPerColumn(1),
    debugLevel(0) {
    std::cout << "VASimConfig:VASimConfig()" << std::endl;

    _VASimConfigMap["<Video File List>"] = configType_fileList;
    _VASimConfigMap["<Dynamic VA library folder>"] =
            configType_dynVAEngineFolder;
    _VASimConfigMap["<Dynamic VA library data folder>"] =
            configType_dynVAEngineDataFolder;
    _VASimConfigMap["<Enable Display>"] = configType_enableDisplay;
    _VASimConfigMap["<Enable Snapshot>"] = configType_enableSnapshot;
    _VASimConfigMap["<Enable FR>"] = configType_enableFR;
    _VASimConfigMap["<Videos Per Row>"] = configType_videosPerRows;
    _VASimConfigMap["<FilmStrip number>"] = configType_filmStripNum;
    _VASimConfigMap["<Video Rows Per Column>"] = configType_videoRowsPerColumn;
    _VASimConfigMap["<Exit on EOF>"] = configType_exitOnEOF,
    _VASimConfigMap["<Debug Level>"] = configType_debugLevel;
    _VASimConfigMap["<VA Rule Config File>"] = configType_VARuleConfigFile;
    _VASimConfigMap["<VAM Ext Rule Config File>"] =
            configType_VAMExtRuleConfigFile;
    _VASimConfigMap["<Metadata Output File>"] = configType_MetadataOutputFile;
    _VASimConfigMap["<Event Output Folder>"] = configType_EventOutputFolder;
}

int VASimConfig::parseArgv(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (argv[i] == nullptr) {
            continue;
        }

        std::string arg = argv[i];
        if ((arg == "-i" || arg == "-I") && argv[i+1] != nullptr) {
            inputFileNameList.clear();
            inputFileNameList.push_back(argv[i+1]);
        }

        if ((arg == "-mo" || arg == "-MO") &&
                argc > (i+1) && argv[i+1] != nullptr) {
            MetadataOutputFolder = argv[i+1];
        }

        if ((arg == "-eo" || arg == "-EO") &&
                argc > (i+1) && argv[i+1] != nullptr) {
            EventOutputFolder = argv[i+1];
        }

        if ((arg == "-r" || arg == "-R") &&
                argc > (i+1) && argv[i+1] != nullptr) {
            VAMRuleConfigFileName = argv[i+1];
        }

        if ((arg == "-rext" || arg == "-REXT") &&
                argc > (i+1) && argv[i+1] != nullptr) {
            VAMExtRuleConfigFileName = argv[i+1];
        }

        if ((arg == "-dl" || arg == "-DL") &&
                argc > (i+1) && argv[i+1] != nullptr) {
            DynVAEngineFolder = argv[i+1];
        }

        if ((arg == "-dd" || arg == "-DD") &&
                argc > (i+1) && argv[i+1] != nullptr) {
            DynVAEngineDataFolder = argv[i+1];
        }

        if ((arg == "--FREnrollmentFileName") &&
                argc > (i+1) && argv[i+1] != nullptr) {
            FREnrollmentFileName = argv[i+1];
        }

        if ((arg == "--FREnrollmentFolder") &&
                argc > (i+1) && argv[i+1] != nullptr) {
            FREnrollmentFolder = argv[i+1];
        }

        if (arg == "--display=enable") {
            enableDisplay = true;
        }
        if (arg == "--display=disable") {
            enableDisplay = false;
        }

        if (arg == "--snapshot=enable") {
            enableSnapshot = true;
        }
        if (arg == "--snapshot=disable") {
            enableSnapshot = false;
        }

        if (arg == "--enableFR=enable") {
            enableFR = true;
        }
        if (arg == "--enableFR=disable") {
            enableFR = false;
        }

        if (arg == "--exitOnEOF=enable") {
            exitOnEOF = true;
        }
        if (arg == "--exitOnEOF=disable") {
            exitOnEOF = false;
        }

        if (arg == "--filmStripNum" && argc > (i+1) && argv[i+1] != nullptr) {
            std::stringstream ss;
            ss << argv[i+1];
            ss >> filmStripNum;
        }

        std::cout << "VASimConfig::parseArgv: \"" << arg.c_str() << "\"";
        std::cout << std::endl;
    }

    return VS_OK;
}

void VASimConfig::print() {
    std::cout << "===VASimConfig=============================begin";
    std::cout << std::endl;
    std::cout << "  enableDisplay: " << enableDisplay << std::endl;
    std::cout << "  enableSnapshot: " << enableSnapshot << std::endl;
    std::cout << "  enableFR: " << enableFR << std::endl;
    std::cout << "  videosPerRow: " << videosPerRow << std::endl;
    std::cout << "  filmStripNum: " << filmStripNum << std::endl;
    std::cout << "  videoRowsPerColumn: " << videoRowsPerColumn  << std::endl;
    std::cout << "  exitOnEOF: " << exitOnEOF << std::endl;
    std::cout << "  debugLevel:" << debugLevel << std::endl;
    std::cout << "  VAMRuleConfigFileName:" << VAMRuleConfigFileName;
    std::cout << std::endl;
    std::cout << "  VAMExtRuleConfigFileName:" << VAMExtRuleConfigFileName;
    std::cout << std::endl;
    std::cout << "  MetadataOutputFolder:" << MetadataOutputFolder << std::endl;
    std::cout << "  EventOutputFolder:" << EventOutputFolder << std::endl;
    std::cout << "  DynVAEngineFolder:" << DynVAEngineFolder << std::endl;
    std::cout << "  DynVAEngineDataFolder:" << DynVAEngineDataFolder;
    std::cout << std::endl;
    std::cout << "  inputFileNameList size:" << inputFileNameList.size();
    std::cout << std::endl;
    for (size_t idx = 0; idx < inputFileNameList.size(); idx++) {
        std::cout << "    file " << idx << ": " << inputFileNameList[idx];
        std::cout << std::endl;
    }
    std::cout << "===VASimConfig=============================end" << std::endl;
    std::cout << std::endl;
}

int VASimConfig::_processItems(std::ifstream *pConfigFile, std::string tagStr) {
    if (pConfigFile == nullptr) {
        return VS_NULLPTR;
    }

    std::ifstream &configFile = *pConfigFile;

    // whether it's keyword
    if (_VASimConfigMap.find(tagStr) == _VASimConfigMap.end()) {
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

    int number = 0;
    // get structure based on its types
    switch (_VASimConfigMap.find(tagStr)->second) {
        case configType_enableDisplay:
            enableDisplay = ss.str() == "enable";
            break;
        case configType_enableSnapshot:
            enableSnapshot = ss.str() == "enable";
            break;
        case configType_enableFR:
            enableFR = ss.str() == "enable";
            break;
        case configType_videosPerRows:
            ss >> number;
            videosPerRow = number;
            break;
        case configType_filmStripNum:
            ss >> number;
            filmStripNum = number;
            break;
        case configType_videoRowsPerColumn:
            ss >> number;
            videoRowsPerColumn = number;
            break;
        case configType_exitOnEOF:
            exitOnEOF = ss.str() == "true";
            break;
        case configType_debugLevel:
            ss >> number;
            debugLevel = number;
            break;
        case configType_VARuleConfigFile:
            VAMRuleConfigFileName = valueStr;
            break;
        case configType_VAMExtRuleConfigFile:
            VAMExtRuleConfigFileName = valueStr;
            break;
        case configType_MetadataOutputFile:
            MetadataOutputFolder = valueStr;
            break;
        case configType_EventOutputFolder:
            EventOutputFolder = valueStr;
            break;
        case configType_dynVAEngineFolder:
            DynVAEngineFolder = valueStr;
            break;
        case configType_dynVAEngineDataFolder:
            DynVAEngineDataFolder = valueStr;
            break;
        case configType_fileList:
            while (!ss.eof()) {
                std::string file;
                ss >> file;
                inputFileNameList.push_back(file);
            }
            break;
        default:
            break;
    }

    return VS_OK;
}

int VASimConfig::_validateConfig() {
    if (inputFileNameList.size() == 0) {
        std::cout << "VASimConfig Error: Video file list is empty" << std::endl;
        return VS_ERROR;
    }

    if (inputFileNameList.size() != videosPerRow * videoRowsPerColumn) {
        std::cout << "VASimConfig Warning: input file size(";
        std::cout << (int)inputFileNameList.size();
        std::cout << ") is not equal to video view" << videosPerRow;
        std::cout << videoRowsPerColumn << std::endl;
    }

    return VS_OK;
}
