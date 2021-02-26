/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "CVManagerCommon.h"

#ifdef __ANDROID__
#include "asdFortyScene_api.h"
#endif

#undef LOG_TAG
#define LOG_TAG "CVModuleASD"

extern int32_t getmodulenumber(uint32_t mode);
extern int32_t Scenetoscore[];

#define ASD_START "$#SD"

#if 0
CVStatus_t ASDInit(void *pCom) {
    DPRINTF("CVModuleASD ASDInit E:\n");
    CVStatus_t status = CV_SUCCESS;
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    pModule ->mStepsize = 10;
    DPRINTF("CVModuleASD ASDInit X:\n");
    return status;
}

CVStatus_t ASDDeinit(void *pCom) {
    DPRINTF("CVModuleASD ASDDeinit E:\n");
    CVStatus_t status = CV_SUCCESS;
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    if(pModule ->pComponent)
    {
        ASDHandle *phandle1        = (ASDHandle *)pModule ->pComponent ;
        asdRelease(*phandle1);
    }
    DPRINTF("CVModuleASD ASDDeinit E:\n");
    return status;
}

Moduleoutput ASDProcess(uint8_t *ImginputY,
                       uint8_t *ImginputUV,
                       uint32_t width,
                       uint32_t height,
                       uint32_t srcYStride,
                       uint32_t srcVUStride,
                       void *pCom) {
    DPRINTF("CVModuleASD ASDProcess E:\n");
    Moduleoutput output;
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    if(pModule ->pComponent == NULL)
    {
        pModule ->pComponent       = (ASDHandle *)malloc(sizeof(ASDHandle));
        ASDHandle *phandle1        = (ASDHandle *)pModule ->pComponent ;
        ASDColorFormat colorFormat = YCRCB420;
        uint32_t orientation       = 0;
        bool isVideo = true; //false; //
        asdInit(width,
                height,
                width,
                width,
                colorFormat,
                orientation,
                "/sdcard/scveData/dcn_asd_9layer_svd_configBGR_graph.txt",
                "/sdcard/scveData/dcn_asd_9layer_svd_places_modelBGR_v2",
                phandle1,
                isVideo);
    }
    ASDResult asdResult;
    uint8_t sceneSelection[ASDCV_MAX_SCENE_NUMBER];
    memset(sceneSelection, 0, ASDCV_MAX_SCENE_NUMBER*sizeof(uint8_t));
    for (int32_t i = 1; i < (int32_t)ASDCV_MAX_SCENE_NUMBER; i++) {
        sceneSelection[i] = 1;
    }

    ASDHandle *pHandle  = (ASDHandle*)pModule->pComponent;
    bool ret = asdDetectScene(*pHandle,
                              ImginputY,
                              ImginputUV,
                              sceneSelection,
                              &asdResult);
    float32_t maxconfidence  = 0;
    float32_t maxconfidence2 = 0;
    uint32_t maxcate         = 0;
    uint32_t maxcate2        = 0;

    for(uint32_t i = 0; i < ASDCV_MAX_SCENE_NUMBER; ++i)
    {
        if(asdResult.probability[i] > maxconfidence)
        {
            maxconfidence2 = maxconfidence;
            maxcate2       = maxcate;
            maxconfidence  = asdResult.probability[i];
            maxcate        = i;
        }
    }
    uint32_t maxconfidence_int  = (uint32_t)(maxconfidence * 255);
    uint32_t maxconfidence2_int = (uint32_t)(maxconfidence2 * 255);

    output.status = CV_SUCCESS;
    output.type   = CVModuleOut_ASD;
    output.mScore = ((maxconfidence_int & 0xFF) << 24 )|
                    ((maxcate & 0xFF) << 16 )|
                    ((maxconfidence2_int & 0xFF) << 8 )|
                    (maxcate2 & 0xFF);

    //DPRINTF("ASDProcess output 0x%x", output.mScore );
    DPRINTF("CVModuleASD ASDProcess X:\n");
    return output;
}
#endif
extern "C"
CVManager_Module* ASDCreateModule() {
    DPRINTF("ASDCreateModule E:\n");
    CVManager_Module *pModule = CVManagerCreateModule("ASD");

    DPRINTF("ASDCreateModule X:\n");
    return pModule;
}


//For Auto scene detection
extern
int32_t Getscenetype(CVSuminput_t input,CVSumouput_t *moutput)
{
    int32_t cattype = 0;
    int32_t cattotalscore[40];
    int32_t totalmodule = 0;
    int32_t data   = 0;
    int32_t score1 = 0;
    int32_t cat1   = 0;
    int32_t score2 = 0;
    int32_t cat2   = 0;
    int32_t tscore = 0;

    memset(cattotalscore,0,40*sizeof(int32_t));
    for (uint32_t oindx = 0; oindx < moutput->ouput_num; ++oindx)
    {
        totalmodule = getmodulenumber(input.pFileinput[oindx]->DATA_MODE);
        for (uint32_t sindx = 0; sindx < moutput->output[oindx].output_seg; ++sindx)
        {
            for (uint32_t findx = moutput->output[oindx].start_indx[sindx];
                 findx < moutput->output[oindx].end_indx[sindx];
                 ++findx)
            {
                data        = input.pFileinput[oindx]->pBuffer[(findx + 1) * totalmodule - 1];
                score1      = (data >> 24) & 0xFF;
                cat1        = (data >> 16) & 0xFF;
                score2      = (data >> 8) & 0xFF;
                cat2        = data & 0xFF;
                if (cat1 >= 40 || cat1 < 0 || cat2 >= 40 || cat2 < 0) {
                    continue;
                }
                cattotalscore[cat1] += score1;
                cattotalscore[cat2] += score2;
                //DPRINTF("Getscenetype oindx %d, sindx %d, findx %d\n",oindx,sindx,findx);
                //DPRINTF("Getscenetype, input 0x%x output cat1 %d, cat2 %d\n", data, cat1, cat2);
            }
        }
    }

    for (uint32_t catindx = 0; catindx < 40; ++catindx) {
        if (cattotalscore[catindx] > tscore) {
            tscore  = cattotalscore[catindx];
            cattype = catindx;
        }
    }

    /*****************************************/
    uint32_t offset = 0;
    offset = findend(&moutput->data);        
    if(offset + 4 + 4 +1  < MAX_PARA_LEN){
        memcpy((uint8_t*)(moutput->data.data) + offset,ASD_START,4);
        offset += 4;
        moutput->data.data[offset] = (uint8_t)cattype;
        offset += 1;
        memcpy((uint8_t*)(moutput->data.data) + offset,FRAME_END,4);
        offset += 4;
    }
    //moutput ->outputScene = cattype;
    return cattype;
}

int32_t ASDScoreTranslate(int32_t score)
{
    //DPRINTF("ASDScoreTranslate score 0x%x E:\n",score);
    int32_t outputscore = 0;

    outputscore = int32_t(0.5f * (float32_t)(((score >> 24) & 0xFF) * Scenetoscore[((score >> 16) & 0xFF)]) / 255
            + 0.5f * (float32_t)(((score >> 8) & 0xFF) * Scenetoscore[(score & 0xFF)]) / 255);

    //DPRINTF("ASDScoreTranslate score %d X:\n", outputscore);
    return outputscore;
}
