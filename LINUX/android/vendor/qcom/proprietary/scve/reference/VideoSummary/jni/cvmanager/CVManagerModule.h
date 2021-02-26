/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _CVMANAGER_MODULE_H_
#define _CVMANAGER_MODULE_H_

#include "CVManagerNative.h"

#define MAX_MODULE_NO 6
#define OUTPUT_HISTORY_NO 7
#define FRAME_END         "%%ED" 

extern "C"
{
    typedef enum {                         //Module Output Type
        CVModuleOut_Score = 0,        
        CVModuleOut_FaceFR,
        CVModuleOut_ASD,
        CVModuleOut_Max
    }Output_Type;

    typedef struct                    //Module Output Type
    {
        CVStatus_t status;
        Output_Type type;
        union{
            int32_t mScore;
            int32_t mFaceDetectedNum;
            int32_t mFaceresult;
        };
    }Moduleoutput;    

    typedef struct
    {
        CVStatus_t(*CV_Init)(void *pComp);
        Moduleoutput(*CV_Process)(uint8_t *ImginputY,
                                  uint8_t *ImginputUV,
                                  uint32_t width,
                                  uint32_t height,
                                  uint32_t srcYStride,
                                  uint32_t srcVUStride,
                                  void *pComp);
        CVStatus_t(*CV_DeInit)(void *pComp);
        CVStatus_t(*CV_Get)(void *pData,void *pCom);
        CVStatus_t(*CV_Set)(void *pData,void *pCom);
    }moduleapi;

    typedef struct
    {
        moduleapi mAPI;
        uint32_t moduleno;
        char modulename[256];
        uint32_t identity;

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

        Moduleoutput mLastoutput;
        Moduleoutput mOutput;
        Moduleoutput mPreOutput[OUTPUT_HISTORY_NO];
        uint32_t mOutputIndx;
        uint32_t mOutputCount;
        //Thread info
        bool threadstop;
        pthread_mutex_t cv_mutex;
        pthread_cond_t cv_cond;
        pthread_t cv_aync_thread;

        pthread_mutex_t cv_msg_mutex;
        pthread_cond_t cv_msg_cond;
        pthread_t cv_msg_thread;

        void *pVision;
        void *pData;
        void *pComponent;
        uint8_t mIssmall;
        uint8_t mStepsize;
        uint32_t mProcesscount;
    }CVManager_Module;
}


extern "C"
CVManager_Module *CVManagerCreateModule(char modulename[]);

extern "C"
int32_t util_score_mapping(int32_t value, int32_t lbound,int32_t hbound,int32_t lout, int32_t hout,uint32_t alpha, uint8_t inverse);

extern "C"
uint32_t findend(CVPara_t *para);

extern "C"
{
    CVManager_Module *MotionDetectCreateModule();
    /**Add New Module HERE****/
    CVManager_Module *VideoSummaryCreateModule();

    CVManager_Module *FaceDetectCreateModule();

    CVManager_Module *ASDCreateModule();

    int32_t ASDScoreTranslate(int32_t score);

    CVManager_Module* FaceRecognitionCreateModule();
}

#endif