/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _CVMANAGER_TYPE_H_
#define _CVMANAGER_TYPE_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fastcv/fastcv.h>
#include "CVManagerNative.h"
#include "CVManagerModule.h"
#ifdef _ANDROID_
#include <android/log.h>
#endif

#ifdef _WIN32
#include <cstring>
#include <time.h>
#include <ctime>
#endif

enum CV_PARA{
    PARA_PEAKVALTHRESH = 0,          //PEAK value threshold
    PARA_LOCALPEAKSPACE,             //space between adjacent peaks
    PARA_VIDEOSEGSPACE,              //SPACE between adjacent segments
    PARA_MINSHOTLEN,                 //in 30FPS scene this is 1 second
    PARA_MAXSHOTLEN,                 //in 30FPS scene this is 6 second 
    PARA_TIMELAPSE_THRESH,
    PARA_LOWCOMPRE_LEN1,
    PARA_LOWCOMPRE_LEN2,
    PARA_LOWCOMPRE_LEN3,
    PARA_LOWCOMPRE_LEN4,
    PARA_HIGHCOMPRE_LEN1,
    PARA_HIGHCOMPRE_LEN2,
    PARA_HIGHCOMPRE_LEN3,
    PARA_HIGHCOMPRE_LEN4,
    PARA_ASDCV_AQUARIUM,
    PARA_ASDCV_BEDROOM,
    PARA_ASDCV_CLASSROOM,
    PARA_ASDCV_CONFERENCE_ROOM,
    PARA_ASDCV_DINING_ROOM,
    PARA_ASDCV_HOME,
    PARA_ASDCV_INDOOR,
    PARA_ASDCV_KITCHEN,
    PARA_ASDCV_LIVING_ROOM,
    PARA_ASDCV_OFFICE,
    PARA_ASDCV_RESTAURANT,
    PARA_ASDCV_STORE,
    PARA_ASDCV_SUPERMARKET,
    PARA_ASDCV_WORK_PLACE,
    PARA_ASDCV_PARKING_LOT_STRUCTURE,
    PARA_ASDCV_INDUSTRIAL,
    PARA_ASDCV_SPORTS,
    PARA_ASDCV_AMUSEMENT_PARK,
    PARA_ASDCV_ARCHITECTURE_BUILDING,
    PARA_ASDCV_BRIDGE,
    PARA_ASDCV_CHURCH,
    PARA_ASDCV_PLAYGROUND,
    PARA_ASDCV_SKYCRAPER,
    PARA_ASDCV_STREET,
    PARA_ASDCV_CANYON,
    PARA_ASDCV_COAST,
    PARA_ASDCV_DESERT,
    PARA_ASDCV_FOREST,
    PARA_ASDCV_GARDEN,
    PARA_ASDCV_ICE,
    PARA_ASDCV_LANDSCAPE,
    PARA_ASDCV_MOUNTAIN,
    PARA_ASDCV_OCEAN,
    PARA_ASDCV_PLANTS,
    PARA_ASDCV_POND,
    PARA_ASDCV_RIVER,
    PARA_ASDCV_SKY,
    PARA_ASDCV_SWIMMING_POOL,
    PARA_ASDCV_WATER,
    PARA_ASDCV_PASTURE,
    PARA_MAX
};

extern "C"
{
    typedef struct {
        int32_t data[PARA_MAX];        
    }CVpara_t;

    typedef struct {
        uint8_t* mImginputY;
        uint8_t* mImginputUV;
        uint32_t mwidth;
        uint32_t mheight;
        uint32_t msrcYStride;
        uint32_t msrcVUStride;
        uint8_t* mImgoutputY;
        uint8_t* mImgoutputUV;
        uint32_t mdstYStride;
        uint32_t mdstVUStride;

        CVpara_t mPara;
        //Output data
        CVoutput_t mOutput;
        /******/
        uint32_t max_facevalue;

        pthread_mutex_t threadmutex;
        pthread_cond_t threadcond;
        uint32_t threadcount;
        //Module List
        CVManager_Module *mModuleList[MAX_MODULE_NO];
        uint32_t totalmodule;
        uint32_t initmode;
    }CVisionInfo_t;
}

#endif