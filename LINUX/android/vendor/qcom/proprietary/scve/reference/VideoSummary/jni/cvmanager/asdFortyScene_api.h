/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/**=============================================================================
@file
asdSceneDetection_api.h
@brief
API for detecting scene categories
=============================================================================**/
#ifndef qcv_dfasdfortyscene_api_h_
#define qcv_dfasdfortyscene_api_h_

#include <cstddef>
#include "fastcv/fastcv.h"

#define SCENE_40

#if 0
namespace qcv
{
namespace AutoSceneDetection
{
#endif

//------------------------------------------------------------------------------
/// @brief
///    Handle refering to the particular instance of ASD. This handle
///    is passed to each function that deals with the ASD.
//------------------------------------------------------------------------------
typedef void *ASDHandle;

enum ASDColorFormat 
{   //always assume for pseudo plannar input 
    YCBCR420 = 0, //Cb Cr down sample on both x and y
    YCBCR422 = 1, 
    YCRCB420 = 2, //Cb Cr down sample only on x, not y
    YCRCB422 = 3,
};

enum ASDSceneID
{
    ASDCV_AQUARIUM = 0,
    ASDCV_BEDROOM = 1,
    ASDCV_CLASSROOM = 2,
    ASDCV_CONFERENCE_ROOM = 3,
    ASDCV_DINING_ROOM = 4,
    ASDCV_HOME = 5,
    ASDCV_INDOOR = 6,
    ASDCV_KITCHEN = 7,
    ASDCV_LIVING_ROOM = 8,
    ASDCV_OFFICE = 9,
    ASDCV_RESTAURANT = 10,
    ASDCV_STORE = 11,
    ASDCV_SUPERMARKET = 12,
    ASDCV_WORK_PLACE = 13,
    ASDCV_PARKING_LOT_STRUCTURE = 14,
    ASDCV_INDUSTRIAL = 15,
    ASDCV_SPORTS = 16,
    ASDCV_AMUSEMENT_PARK = 17,
    ASDCV_ARCHITECTURE_BUILDING = 18,
    ASDCV_BRIDGE = 19,
    ASDCV_CHURCH = 20,
    ASDCV_PLAYGROUND = 21,
    ASDCV_SKYCRAPER = 22,
    ASDCV_STREET = 23,
    ASDCV_CANYON = 24,
    ASDCV_COAST = 25,
    ASDCV_DESERT = 26,
    ASDCV_FOREST = 27,
    ASDCV_GARDEN = 28,
    ASDCV_ICE = 29,
    ASDCV_LANDSCAPE = 30,
    ASDCV_MOUNTAIN = 31,
    ASDCV_OCEAN = 32,
    ASDCV_PLANTS = 33,
    ASDCV_POND = 34,
    ASDCV_RIVER = 35,
    ASDCV_SKY = 36,
    ASDCV_SWIMMING_POOL = 37,
    ASDCV_WATER = 38,
    ASDCV_PASTURE = 39,
#ifdef SCENE_40
    ASDCV_MAX_SCENE_NUMBER = 40, //Always the last entry in the enum
#else
    ASDCV_MAX_SCENE_NUMBER = 1000, //Always the last entry in the enum
#endif
};

#define ASDCV_MAX_IMAGE_WIDTH 4096
struct ASDResult
{
    float32_t score[ASDCV_MAX_SCENE_NUMBER];            //Raw score for each scene
    float32_t probability[ASDCV_MAX_SCENE_NUMBER];        //Normalized score
    uint8_t confidence[ASDCV_MAX_SCENE_NUMBER];            //Decision btw 0 and 1 for each scene
};

//assuming the YUV - NV21 input 
//frameOrientation - four values - 0 (landscape, camera on the left, no rotation), 90 (portrait, camera on top), 180 (landscale, camera on right),  270 (portrait, camera on bottom)
extern "C" void asdInit(uint32_t imgWidth, uint32_t imgHeight, uint32_t imgYStride, uint32_t imgUVStride, ASDColorFormat imgColorFormat, uint32_t frameOrientation,
                        const char *confFileName, const char *modelFileName, ASDHandle *asdHandle, bool isVideo = true);

//detection object from an image return the detection results.?? what should be the data structure of the output, multiple detectors, with scores and probability. 
//return false if the detection was not run 
extern "C" bool asdDetectScene(ASDHandle asdHandle, uint8_t *imgY, uint8_t *imgUV, uint8_t *sceneSelection, ASDResult *asdResult);


extern "C" void asdRelease(ASDHandle asdHandle);

#if 0 
} //namespace asd

} //namespace qcv
#endif

#endif  //qcv_dfasdfortyscene_api_h_
