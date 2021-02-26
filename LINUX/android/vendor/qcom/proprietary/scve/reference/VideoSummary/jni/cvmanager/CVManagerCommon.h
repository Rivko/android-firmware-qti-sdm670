/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 #ifndef _CVMANAGER_COMMON_H_
#define _CVMANAGER_COMMON_H_

#include "CVManagerType.h"
#include "CVManagerModule.h"

#define CVMANAGER_VERSION "CVMANAGER 2.0"
#define CVMANAGER_DESCRIPTION "Second Version of CVManager with multivideo, feature:video summary, face detect, motion"
#define PEAK_VAL_THRESH  (22)    //PEAK value threshold
#define LOCAL_PEAK_RANGE (5)   //space between adjacent peaks
#define VIDEO_SEG_SPACE  (10)   //SPACE between adjacent segments
#define MIN_SHOT_LENGTH  (45)   //in 30FPS scene this is 1 second
#define MAX_SHOT_LENGTH  (30*6)  //in 30FPS scene this is 9 second 
#define MAX_SEG  (1024*32)

extern "C"
{    
    //CVManager_Module *CVManagerCreateModule(char modulename[]);
    CVStatus_t CVManagerAddModule(CVisionInfo_t *pList, CVManager_Module *pModule);    
    CVStatus_t CVManagerCleanModule(CVisionInfo_t *pList);

    //Init all the modules one by one
    CVStatus_t CVManagerModuleInit(CVisionInfo_t *pList);
    //Process all the modules on the list
    CVStatus_t CVManagerModuleProcess(CVisionInfo_t *pList);
    //Deinit All the modules on the list
    CVStatus_t CVManagerModuleDeinit(CVisionInfo_t *pList);
    //Get Para all the modules 
    CVStatus_t CVManagerModuleGet(CVisionInfo_t *pList, CVPara_t *para, uint32_t mode);
    //Set Para all the modules 
    CVStatus_t CVManagerModuleSet(CVisionInfo_t *pList, CVPara_t para, uint32_t mode);
}

//extern "C"
//int32_t util_score_mapping(int32_t value, int32_t lbound,int32_t hbound,int32_t lout, int32_t hout,uint32_t alpha, uint8_t inverse);

extern "C"
void gaussian1d5(int32_t *input, uint32_t length,float32_t *output);
extern "C"
void log1d9(uint8_t *input, uint32_t length, int16_t *output);
extern "C"
void Sort(uint32_t *indx, uint32_t size, uint32_t *data1, uint32_t *data2, uint32_t *dataindx, uint32_t *revindx);
extern "C"
void readconfig(CVpara_t &para);
extern "C"
CVpara_t initconfig();

extern "C"
CVsum_t MergeSegment(uint32_t *segmax,   //Sorted 
                    uint32_t *segstart,
                    uint32_t *segstop,
                    uint32_t *segindx,
                    uint32_t *sortindx,
                    uint32_t segcount,
                    uint32_t datalength,
                    int32_t minshotlength,
                    int32_t desireshotlength);


#define LOG_CV
#define KPI_DEBUG
//#define RELEASE

#ifdef LOG_CV
#ifdef __ANDROID__
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define DPRINTF(format,...)  printf(format,__VA_ARGS__)
#endif
#else
#define DPRINTF(fmt,...) do {} while(0)
#endif

#endif