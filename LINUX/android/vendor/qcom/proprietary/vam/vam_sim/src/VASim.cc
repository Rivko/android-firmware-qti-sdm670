/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "VASim.h"
#include "VASimError.h"

extern "C" {
#include "configuration_parser_apis.h"
}

#include <VAM/vaapi.h>

#include <assert.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <cstring>
#include <chrono>
#include <thread>

/* polls for possible required screen refresh at least this often,
 * should be less than 1/fps */
#define REFRESH_RATE 0.01

bool isProcessingFrame = false;

VASimParams params;
VASimParams::VASimParams() :
    isStop(false),
    isNewEventFile(true),
    isNewMetadataFile(true),
    isRawVideo(false) {
}

VASimParams::~VASimParams() {
    if (metadataFileWriter.is_open()) {
        metadataFileWriter << std::endl << "]" << std::endl;
        metadataFileWriter.close();
    }

    if (eventFileWriter.is_open()) {
        eventFileWriter << std::endl << "]" << std::endl;
        eventFileWriter.close();
    }
}

int VASimParams::init(int argc, char **argv) {
    int ret = config.parseArgv(argc, argv);
    if (ret != VS_OK) {
        std::cout << "VASim error: config parseArgv failed" << std::endl;
        return ret;
    }

    config.print();

    std::string metadataFileName =
            config.MetadataOutputFolder + "/metadata.txt";
    metadataFileWriter.open(metadataFileName.c_str());
    if (metadataFileWriter.is_open()) {
        metadataFileWriter << "[" << std::endl;
    } else {
        std::cout << "VASimParams init Error: failed to open metadatafile ";
        std::cout << metadataFileName << std::endl;
    }

    std::string eventFileName = config.EventOutputFolder + "/eventLog.txt";
    eventFileWriter.open(eventFileName.c_str());
    if (eventFileWriter.is_open()) {
        eventFileWriter << "[" << std::endl;
    } else {
        std::cout << "VASimParams init Error: failed to open event file ";
        std::cout << eventFileName << std::endl;
    }

    if (config.enableFR) {
        FRConfig.readConfig(config.FREnrollmentFileName);
        FRConfig.print();
    }

    ret = JSONRuleConfig.readConfig(config.VAMRuleConfigFileName);
    JSONRuleConfig.print();

    ret = extRuleConfig.readConfig(config.VAMExtRuleConfigFileName);
    extRuleConfig.print();

    return ret;
}

void sigterm_handler(int sig) {
    std::cout << "Get signal " << sig << std::endl;

    params.isStop = true;
}

int32_t frameProcessedCBFunc(uint64_t time_stamp, void */*usr_data*/) {
    std::cout << "VASim frameProcessedCBFunc: frameTS = " << time_stamp;
    std::cout << std::endl;
    isProcessingFrame = false;
    return VS_OK;
}

int snapshotCBFunc(vaapi_snapshot_info *info, void */*usr_data*/) {
    if (info == nullptr) {
        return VS_NULLPTR;
    }

    std::stringstream targetFile;
    targetFile << params.config.EventOutputFolder << "/" << info->pts << ".yuv";

    std::cout << "VASim snapshotCBFunc: target file:";
    std::cout << targetFile.str() << std::endl;

    // TODO(future): save data

    return 0;
}

int32_t eventCBFunc(struct vaapi_event *pEvent, void */*usr_data*/) {
    if (pEvent == nullptr) {
        return VS_NULLPTR;
    }

    std::cout << "VASim: found event type:" << pEvent->type;
    std::cout << ", id:" << pEvent->id;
    std::cout << ", obj id:" << pEvent->obj.id << std::endl;

    if (params.eventFileWriter.is_open()) {
        if (params.isNewEventFile) {
            params.isNewEventFile = false;
        } else {
            params.eventFileWriter << "," << std::endl;
        }

        char jsonStr[1024];
        int32_t ret = vaapi_convert_event_to_json(pEvent, jsonStr, 1024);
        if (ret != VAM_OK) {
            std::cout << "VASim eventCBFunc error(" << ret << ")." << std::endl;
            return ret;
        }

        std::cout << "VASim eventCBFunc: %s" << jsonStr << std::endl;
        params.eventFileWriter << jsonStr;
    }

    return VS_OK;
}

int metadataFrameCBFunc(vaapi_metadata_frame *pFrame, void */*usrData*/) {
    if (pFrame == nullptr) {
        return VS_NULLPTR;
    }

    if (params.metadataFileWriter.is_open()) {
        if (params.isNewMetadataFile) {
            params.isNewMetadataFile = false;
        } else {
            params.metadataFileWriter << "," << std::endl;
        }

        char jsonStr[1024];
        int32_t ret = vaapi_convert_metadata_to_json(pFrame, jsonStr, 1024);
        if (ret != VAM_OK) {
            std::cout << "VASim metadataFrameCBFunc error(" << ret << ").";
            std::cout << std::endl;
            return ret;
        }

        params.metadataFileWriter << jsonStr;
    }

    return VS_OK;
}


int initVAM(vaapi_source_info *pSourceInfo) {
    int ret = vaapi_init(pSourceInfo, params.config.DynVAEngineFolder.c_str());
    if (ret != VAM_OK) {
        exit(0);
    }

    vaapi_register_metadata_cb(&metadataFrameCBFunc, nullptr);
    vaapi_register_event_cb(&eventCBFunc, nullptr);
    vaapi_register_frame_processed_cb(frameProcessedCBFunc, nullptr);

    if (params.config.enableSnapshot) {
        vaapi_register_snapshot_cb(&snapshotCBFunc, nullptr);
    }

    vaapi_configuration config;
    memset(&config, 0, sizeof(vaapi_configuration) );

    config.rule_size = params.extRuleConfig._configInfoList.size() +
                       params.JSONRuleConfig.vaapiConfig.rule_size;
    config.rule_size = MIN(config.rule_size, VAAPI_RULE_MAX);

    size_t rule_cnt = MIN(params.JSONRuleConfig.vaapiConfig.rule_size,
                          VAAPI_RULE_MAX);
    for (size_t i = 0; i < rule_cnt; i++) {
        memcpy(&config.rules[i],
               &params.JSONRuleConfig.vaapiConfig.rules[i],
               sizeof(vaapi_rule));
    }

    for (size_t i = 0; i < params.extRuleConfig._configInfoList.size(); i++) {
        size_t idx = i + params.JSONRuleConfig.vaapiConfig.rule_size;
        vaapi_rule &r = config.rules[idx];
        VAMExtRuleConfigInfo &rSrc = params.extRuleConfig._configInfoList[i];
        snprintf(r.id, VAAPI_UUID_LEN, "config ID - %s",
                ConfigBase::EventTypeToStr(rSrc.eventType).c_str());
        snprintf(r.name, VAAPI_UUID_LEN, "config name: %s",
                ConfigBase::EventTypeToStr(rSrc.eventType).c_str());
        r.type = rSrc.eventType;

        // TODO(future): remove this hardcode
        r.type = vaapi_event_face_recognized;
        r.sensitivity = rSrc.sensitivity;
    }

    ret = vaapi_set_config(&config);
    if (ret != VAM_OK) {
        exit(0);
    }

    ////////////////////////////////////////////////////////

    ret = vaapi_run();
    if (ret != VAM_OK) {
        exit(0);
    }

    return VS_OK;
}

void putVideoRaw(uint64_t pts, uint8_t *pY, uint8_t *pU, uint8_t *pV) {
    static bool IsVideoInited = false;
    static int imgWidth = 1280, imgHeight = 720;

    if (IsVideoInited == false) {
        vaapi_source_info sourceInfo;
        memset(&sourceInfo, 0, sizeof(vaapi_source_info) );
        sourceInfo.img_format = vaapi_format_nv12;
        sourceInfo.frame_l_enable = 1;
        sourceInfo.frame_l_width[0] = imgWidth;
        sourceInfo.frame_l_width[1] = imgWidth/2;
        sourceInfo.frame_l_width[2] = imgWidth/2;
        sourceInfo.frame_l_pitch[0] = imgWidth;
        sourceInfo.frame_l_pitch[1] = imgWidth/2;
        sourceInfo.frame_l_pitch[2] = imgWidth/2;
        sourceInfo.frame_l_height[0] = imgHeight;
        sourceInfo.frame_l_height[1] = imgHeight/2;
        sourceInfo.frame_l_height[2] = imgHeight/2;
        sourceInfo.frame_s_enable = 0;

        snprintf(sourceInfo.data_folder, VAAPI_PATH_LEN, "%s",
                params.config.DynVAEngineDataFolder.c_str());

        int ret = initVAM(&sourceInfo);
        if (ret != VS_OK) {
            std::cout << "VASim error(" << ret << "): initVAM failed";
            std::cout << std::endl;
            exit(0);
        }
        IsVideoInited = true;
    }

    vaapi_frame_info frameInfo;
    memset(&frameInfo, 0, sizeof(vaapi_frame_info));
    frameInfo.time_stamp = pts;
    frameInfo.frame_l_data[0] = pY;
    frameInfo.frame_l_data[1] = pU;
    frameInfo.frame_l_data[2] = pV;

    if (params.config.filmStripNum > 0) {
        static int frameIDX = -1;
        frameIDX++;

        if ((uint32_t)frameIDX > 60 * (params.config.filmStripNum - 1)) {
            std::cout << "VASim: shoot out stop" << std::endl;
            sigterm_handler(SIGINT);
        }

        std::string fileName = params.config.inputFileNameList[0];
        if (frameIDX % 60 == 0 && fileName.size() > 4) {
            fileName = fileName.substr(0, fileName.size()-4) + "_";
            std::stringstream fileNameSS;
            fileNameSS << fileName;
            fileNameSS << (frameIDX / 60);

            vaapi_snapshot_info ssInfo;
            memset(&ssInfo, 0, sizeof(vaapi_snapshot_info));

            strncpy(ssInfo.file_name, fileNameSS.str().c_str(), VAAPI_PATH_LEN);
            std::cout << "VASim: generating film strip: " << ssInfo.file_name << std::endl;

            ssInfo.img_data[0] = pY;
            ssInfo.img_data[1] = pU;
            ssInfo.img_data[2] = pV;
            ssInfo.source_info.frame_l_width[0] = imgWidth;
            ssInfo.source_info.frame_l_width[1] = imgWidth/2;
            ssInfo.source_info.frame_l_width[2] = imgWidth/2;
            ssInfo.source_info.frame_l_pitch[0] = imgWidth;
            ssInfo.source_info.frame_l_pitch[1] = imgWidth/2;
            ssInfo.source_info.frame_l_pitch[2] = imgWidth/2;
            ssInfo.source_info.frame_l_height[0] = imgHeight;
            ssInfo.source_info.frame_l_height[1] = imgHeight/2;
            ssInfo.source_info.frame_l_height[2] = imgHeight/2;

            // std::cout << "VASim: putVideo frame->format: ";
            // std::cout << vs->frame->format << std::endl;
            ssInfo.source_info.img_format = vaapi_format_YUVJ420P;

//            getSnapshot(&ssInfo);
        }
    }

    {
        static int totalFrameIDX = 0;
        std::cout << "vaapi_process() .. " << totalFrameIDX++ << "th frame: ";
        std::cout << frameInfo.time_stamp << std::endl;


        int res;
        do {
            res = vaapi_process(&frameInfo);
            isProcessingFrame = res == VAM_OK;
        } while (res == VAM_BUSY);


        int retry = 0;
        while (isProcessingFrame && retry < 100) {
            retry++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "vaapi_process() .. stop" << std::endl;
    }
}

int VASimInit(VASimParams *params) {
    if (params == nullptr) {
        return VS_NULLPTR;
    }

    if (params->isRawVideo) {
        return VS_OK;
    }

    for (uint32_t rowIDX = 0;
            rowIDX < params->config.videoRowsPerColumn; rowIDX++) {
        for (uint32_t columnIDX = 0;
                columnIDX < params->config.videosPerRow; columnIDX++) {
            uint32_t idx = rowIDX * params->config.videosPerRow + columnIDX;
            if (idx >= params->config.inputFileNameList.size()) {
                break;
            }

            if (params->config.inputFileNameList[idx].empty()) {
                continue;
            }
        }
    }

    return VS_OK;
}

int VASimDestroy(VASimParams *params) {
    if (params == nullptr) {
        std::cout << "Error: got null pointer in " << __FUNCTION__ << std::endl;
        return VS_NULLPTR;
    }

    vaapi_stop();
    vaapi_deinit();

    return VS_OK;
}

void printHelp() {
    std::cout << "VASim usage:" << std::endl;
    std::cout << "VASim [-r VA_rule_config] [-mo metadata_output_path]";
    std::cout << " [-eo event_output_path]." << std::endl;
    std::cout << "      [-dl dyn_va_engine_folder] [--display=enable/disable]";
    std::cout << std::endl;
}

int main(int argc, char **argv) {

    // TO stop stdout from buffering
    setvbuf(stdout, 0, _IONBF, 0);

    std::string arg = (argc < 2 || argv[1] == nullptr) ? "" : argv[1];
    if (arg == "-h" || arg == "-H" || arg == "help") {
        printHelp();
        return VS_OK;
    }

    std::cout << "VASim start." << std::endl;
    params.init(argc, argv);
    VASimInit(&params);
    params.isRawVideo = params.config.inputFileNameList[0].empty() != true &&
        (params.config.inputFileNameList[0].find(".yuv") != std::string::npos ||
         params.config.inputFileNameList[0].find(".raw") != std::string::npos);

    if (params.isRawVideo) {
        std::ifstream is(params.config.inputFileNameList[0].c_str(),
                         std::ifstream::binary);

        const int imgWidth = 1280, imgHeight = 720;
        char *pY = new char[imgWidth * imgHeight];
        char *pU = new char[imgWidth/2 * imgHeight/2];
        char *pV = new char[imgWidth/2 * imgHeight/2];

        uint64_t pts = 0;

        while (params.isStop == false &&
                // read data as a block:
                is.read(pY, imgWidth * imgHeight) &&
                is.read(pU, imgWidth/2 * imgHeight/2) &&
                is.read(pV, imgWidth/2 * imgHeight/2)) {
            putVideoRaw(pts*1000000LL, (uint8_t*)pY, (uint8_t*)pU, (uint8_t*)pV);
            pts += 33;
        }

        delete [] pY;
        delete [] pU;
        delete [] pV;

    }

    VASimDestroy(&params);

    std::cout << "VASim done" << std::endl;
    return 0;
}
