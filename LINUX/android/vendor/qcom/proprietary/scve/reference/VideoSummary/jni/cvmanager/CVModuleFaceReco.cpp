/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "CVManagerCommon.h"
#if 0
#include "scve/scveFaceRecognition.hpp"
#include "scve/scveFaceRecognitionTypes.hpp"
#endif

#undef LOG_TAG
#define LOG_TAG "CVModuleFace"

#define MAX_RESULTS 3
#define FR_SCORE_THRESHOLD      150
#define MAX_UNIQUE_FACE 10
#define FR_DISABLE      "$#MD"
#define FR_START        "$#FR"
#define FR_END          "%%FR"

#ifndef SCVE_FACE_RECOGNITION_FEATURE_ID_LEN
#define SCVE_FACE_RECOGNITION_FEATURE_ID_LEN 20
#endif

extern int32_t getmodulenumber(uint32_t mode);
extern uint8_t checkmode(uint32_t input, uint32_t MODE);

typedef struct {
uint8_t numofuniquefaces;
uint8_t data[MAX_UNIQUE_FACE][SCVE_FACE_RECOGNITION_FEATURE_ID_LEN];
}FRudata;

typedef struct {
    uint32_t frequency;
    uint8_t data[SCVE_FACE_RECOGNITION_FEATURE_ID_LEN];
}FRiddata;

#if 0
typedef struct {
    SCVE::Context *pCtx;
    SCVE::FaceRecognition *pFaceReco;
    SCVE::Image mImage;
    SCVE::FR_FaceInfo *pFaceInfo;
    SCVE::FR_Result   *pFrResult;

    FRudata *pUdata;
    uint32_t nMaxRegisteredUsers;
    uint32_t nDatabaseVersion;
    uint32_t nRegisteredUsers;
    uint32_t nDisableFR;
}FRModule;


CVStatus_t FRInit(void *pCom) {
    DPRINTF("FaceRecognition FDInit E:\n");
    CVStatus_t status = CV_SUCCESS;
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    FRModule *pFR             = (FRModule *)malloc(sizeof(FRModule));
    pModule ->pComponent      = pFR;

    pFR -> pCtx               = SCVE::Context::newInstance();
    pFR -> pFaceReco          = NULL;
    pFR -> mImage.nChannels   = 1;
    pFR -> mImage.sFormat     = SCVE::SCVE_COLORFORMAT_GREY_8BIT;
    pFR -> pFaceInfo          = (SCVE::FR_FaceInfo *) malloc(sizeof(SCVE::FR_FaceInfo)
                                                            * SCVE_FACE_RECOGNITION_DEFAULT_MAX_FACES);
    pFR -> pFrResult          = (SCVE::FR_Result   *) malloc(sizeof(SCVE::FR_Result)
                                                             * MAX_RESULTS);
    pFR -> pUdata             = (FRudata*)malloc(sizeof(FRudata));
    pFR -> pUdata ->numofuniquefaces = 0;
    pFR -> nMaxRegisteredUsers= SCVE_FACE_RECOGNITION_DEFAULT_MAX_REGISTERED_FACES;
    pFR -> nDatabaseVersion   = 0;
    pFR -> nRegisteredUsers   = 0;
    pFR -> nDisableFR         = 0;

    return status;
}

CVStatus_t FRDeinit(void *pCom) {
    DPRINTF("FaceRecognition FDDeInit E:\n");
    CVStatus_t status = CV_SUCCESS;
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    FRModule *pFR             = (FRModule *)pModule ->pComponent;

    if (pFR) {
        if (pFR ->pFaceReco) {
            SCVE:: FaceRecognition::deleteInstance(pFR ->pFaceReco);
        }
        if (pFR ->pCtx) {
            SCVE::Context::deleteInstance(pFR ->pCtx);
        }
        if (pFR ->pFaceInfo) {
            free(pFR ->pFaceInfo);
        }
        if (pFR ->pFrResult) {
            free(pFR ->pFrResult);
        }
        if (pFR ->pUdata) {
            free(pFR ->pUdata);
        }
    }
    return status;
}

Moduleoutput FRProcess(uint8_t *ImginputY,
                       uint8_t *ImginputUV,
                       uint32_t width,
                       uint32_t height,
                       uint32_t srcYStride,
                       uint32_t srcVUStride,
                       void *pCom) {
    DPRINTF("FaceRecognition FRProcess E:\n");
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    FRModule *pFR             = (FRModule *)pModule ->pComponent;
    Moduleoutput output;
    SCVE::ImageDimension dim;
    uint32_t nDetectedFaces      = 0;
    uint32_t numResults          = 0;
    uint32_t maxscore            = 0;
    uint8_t facedetectedresult[3];
    uint8_t tempid[SCVE_FACE_RECOGNITION_FEATURE_ID_LEN];


    dim.nWidth  = width;
    dim.nHeight = height;
    dim.nStride = srcYStride;

    if (pFR ->pFaceReco == NULL) {
        SCVE::FR_ImageMode frMode = SCVE::SCVE_FACERECOGNITION_VIDEO;
        pFR ->pFaceReco =  SCVE::FaceRecognition::newInstance(pFR ->pCtx, "/sdcard/scveData/frdb.dat", frMode);

        pFR ->pFaceReco->GetDatabaseInfo("/sdcard/scveData/frdb.dat", &(pFR ->nDatabaseVersion),
                              &(pFR ->nMaxRegisteredUsers), &(pFR ->nRegisteredUsers));
        DPRINTF("FaceRecognition Database info version %d, maxuser %d, reguser %d", pFR ->nDatabaseVersion,
                                pFR ->nMaxRegisteredUsers, pFR ->nRegisteredUsers);
        //--- Init Face Recognition engine
        pFR ->pFaceReco->Init(dim, SCVE_FACE_RECOGNITION_DEFAULT_MAX_FACES,pFR ->nMaxRegisteredUsers);

        pFR ->mImage.sDim.nWidth  = width;
        pFR ->mImage.sDim.nHeight = height;
        pFR ->mImage.sDim.nStride = width;
    }
    pFR ->mImage.pPtr             = ImginputY;
    pFR ->pFaceReco ->DetectFaces(&(pFR ->mImage), pFR ->pFaceInfo, &nDetectedFaces);
    DPRINTF("FaceRecognition Face detected %d",nDetectedFaces);
    memset(facedetectedresult,0xFF, sizeof(uint8_t)*3);
    if (pFR ->nDisableFR == 0) {  //If FR is not disabled 
    for (uint32_t i = 0; i < nDetectedFaces && i < 3; ++i) {
        pFR ->pFaceReco ->RecognizeFace(&(pFR ->mImage), &(pFR ->pFaceInfo[i]), MAX_RESULTS, pFR ->pFrResult, &numResults);
        maxscore = 0;
        for (uint32_t j = 0; j < numResults; ++ j)
        {
            if (pFR ->pFrResult[j].nMatchScore > FR_SCORE_THRESHOLD && pFR ->pFrResult[j].nMatchScore > maxscore)
            {
                maxscore              = (uint32_t)pFR ->pFrResult[j].nMatchScore;
                memcpy(tempid,pFR ->pFrResult[j].sPID, sizeof(uint8_t)*SCVE_FACE_RECOGNITION_FEATURE_ID_LEN);
                DPRINTF("FaceRecognition Face reco result %c%c%c%c",tempid[0],tempid[1],tempid[2],tempid[3]);
            }
        }
        for (uint8_t x = 0; x < pFR ->pUdata ->numofuniquefaces; ++ x) {
            if ( maxscore != 0 && memcmp(tempid, pFR ->pUdata ->data[x],SCVE_FACE_RECOGNITION_FEATURE_ID_LEN)== 0) {
                facedetectedresult[i] = x;
                DPRINTF("FaceRecognition Face reco found in matching table %d",x);
            }
        }
        if (facedetectedresult[i] == 0xFF && maxscore != 0) {
            memcpy(pFR ->pUdata ->data[pFR ->pUdata ->numofuniquefaces],tempid,SCVE_FACE_RECOGNITION_FEATURE_ID_LEN);
            facedetectedresult[i] = pFR ->pUdata ->numofuniquefaces;
            pFR ->pUdata ->numofuniquefaces++;
            DPRINTF("FaceRecognition Face reco  not found list as %d",facedetectedresult[i]);
        }
    }
    }

    output.mFaceresult = (nDetectedFaces & 0xFF) |
                            (((uint32_t)(facedetectedresult[0]) & 0xFF) << 24) |
                            (((uint32_t)(facedetectedresult[1]) & 0xFF) << 16) |
                            (((uint32_t)(facedetectedresult[2]) & 0xFF) << 8);
    DPRINTF("FaceRecognition Face Result 0x%X",output.mFaceresult);
    output.type        = CVModuleOut_FaceFR;
    output.status      = CV_SUCCESS;

    DPRINTF("FaceRecognition FRProcess X");
    return output;
}

CVStatus_t FRGet(void *pData, void *pCom)
{
    DPRINTF("FRGet E:\n");
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    FRModule *pFR             = (FRModule *)pModule ->pComponent;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < pFR ->pUdata ->numofuniquefaces; ++i) {
        memcpy((uint8_t*)pData+offset,FR_START,4);
        offset += 4;
        memcpy((uint8_t*)pData+offset,pFR ->pUdata ->data[i],SCVE_FACE_RECOGNITION_FEATURE_ID_LEN);
        offset += SCVE_FACE_RECOGNITION_FEATURE_ID_LEN;
    }

    DPRINTF("FRGet X:\n");
    return CV_SUCCESS;
}

CVStatus_t FRSet(void *pData, void *pCom)
{
    DPRINTF("FRSet E:\n");
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    uint8_t *para = (uint8_t *)pData;    
    for (uint32_t i = 0; i < MAX_PARA_LEN;) {
        if (para[i] == '$') {
            if (para[i + 1] == '#') {
                if (para[i + 2] == 'M') {
                    if (para[i + 3] == 'D') {
                        FRModule *pFR = (FRModule *)pModule ->pComponent;
                        pFR ->nDisableFR = para[i + 4];
                    }
                }
            }
        }
    }
    DPRINTF("FRSet X:\n");
    return CV_SUCCESS;
}
#endif
extern "C"
CVManager_Module* FaceRecognitionCreateModule() {
    DPRINTF("FaceRecognitionCreateModule E:\n");
    CVManager_Module *pModule = CVManagerCreateModule("FaceRecognition");

    DPRINTF("FaceRecognitionCreateModule E:\n");
    return pModule;
}


void parseFRdata(CVPara_t *pPara,FRudata *pData)
{
    DPRINTF("E: parseFRdata");
    uint8_t tempda[4];
    memset(pData,0,sizeof(FRudata));
    for (uint32_t i = 0; i < MAX_PARA_LEN;) {
        if (pPara->data[i] == '$') {
            if (pPara->data[i + 1] == '#') {
                if (pPara->data[i + 2] == 'F') {
                    if (pPara->data[i + 3] == 'R') {
                        memcpy(pData->data[pData->numofuniquefaces], &pPara->data[i + 4], SCVE_FACE_RECOGNITION_FEATURE_ID_LEN);
                        pData->numofuniquefaces++;
                        i += SCVE_FACE_RECOGNITION_FEATURE_ID_LEN + 4;
                        continue;
                    }
                }
            }
        }
        i++;
    }
    DPRINTF("parseFRdata Unique faces %d",pData->numofuniquefaces);
    DPRINTF("X: parseFRdata");
}

void addFRdata(int32_t data,FRudata *pData,FRiddata *faceid, uint32_t &faceidcount)
{
    uint8_t found = 0;
    if(data >= pData ->numofuniquefaces)
        return;

    for (uint32_t i = 0; i < faceidcount; ++i) {
        if (memcmp(pData->data[data],faceid[i].data,20) == 0) {
            found = 1;
            faceid[i].frequency++;
            break;
        }
    }
    if (found == 0 && faceidcount < MAX_UNIQUE_FACE * 2) {
        memcpy(faceid[faceidcount].data,pData->data[data],sizeof(uint8_t) * SCVE_FACE_RECOGNITION_FEATURE_ID_LEN);
        faceid[faceidcount].frequency = 1;
        faceidcount++;
    }
}

extern
CVStatus_t Getfacestar(CVSuminput_t input,CVSumouput_t *moutput)
{
    DPRINTF(":E Getfacestar");
    CVStatus_t status = CV_SUCCESS;
    int32_t data      = 0;
    uint32_t input_no = 0;
    uint32_t tMODE    = 0;
    uint32_t datind   = MAX_MODULES;
    uint32_t ufaceid  = 0;
    FRiddata *faceid  = (FRiddata*)malloc(sizeof(FRiddata) * MAX_UNIQUE_FACE * 2);
    FRudata *pData    = (FRudata *)malloc(sizeof(FRudata));
    CVPara_t *pPara   = NULL;
    FRiddata tface;
    memset(faceid, 0, sizeof(FRiddata) * MAX_UNIQUE_FACE * 2);

    for (uint32_t oindx = 0; oindx < moutput->ouput_num; ++oindx)
    {
        if (input.pFileinput[oindx] ->pPara == NULL) {
            DPRINTF("NO face matching table");
            continue;
        }

        for (uint32_t i = 0; i < MAX_MODULES; ++i)
        {
            tMODE       = 1 << i;
            if (checkmode(input.pFileinput[oindx]->DATA_MODE, tMODE)) {
                if (tMODE == MODE_FACE) {
                    datind = input_no;
                }
                input_no++;
            }
        }
        if (datind == MAX_MODULES) {
            //There is no face data
            DPRINTF("No face score data");
            continue;
        }

        pPara = input.pFileinput[oindx] ->pPara;
        parseFRdata(pPara,pData);
        DPRINTF("Unique Faces %d", pData ->numofuniquefaces);
        for (uint32_t sindx = 0; sindx < moutput->output[oindx].output_seg; ++sindx)
        {
            for (uint32_t findx = moutput->output[oindx].start_indx[sindx];
                 findx < moutput->output[oindx].end_indx[sindx];
                 ++findx)
            {
                data        = input.pFileinput[oindx]->pBuffer[findx * input_no + datind];

                addFRdata((data >> 24) & 0xFF, pData, faceid,ufaceid);
                addFRdata((data >> 16) & 0xFF, pData, faceid,ufaceid);
                addFRdata((data >> 8) & 0xFF, pData, faceid,ufaceid);
            }
        }
    }

    if( ufaceid == 0)
        return CV_SUCCESS;

    uint8_t swapped = 1;
    while (swapped) {
        swapped = 0;
        for (uint32_t i = 0; i < ufaceid - 1; ++i) {
            if (faceid[i].frequency < faceid[i + 1].frequency) {
                tface     = faceid[i];
                faceid[i] = faceid[i + 1];
                faceid[i + 1] = tface;
                swapped    = 1;
            }
        }
    }
    /*****************************************/
    uint32_t offset = 0;
    offset = findend(&moutput->data);
    for (uint32_t i = 0; i < MAX_UNIQUE_FACE && i < ufaceid; ++i) {
        if(offset+SCVE_FACE_RECOGNITION_FEATURE_ID_LEN +4 < MAX_PARA_LEN){
        memcpy((uint8_t*)(moutput->data.data) + offset,FR_START,4);
        offset += 4;
            if(offset+SCVE_FACE_RECOGNITION_FEATURE_ID_LEN < MAX_PARA_LEN)
        memcpy((uint8_t*)(moutput->data.data) + offset,faceid[i].data,SCVE_FACE_RECOGNITION_FEATURE_ID_LEN);
        offset += SCVE_FACE_RECOGNITION_FEATURE_ID_LEN;
    }
    }
    if(offset +4 < MAX_PARA_LEN){
        memcpy((uint8_t*)(moutput->data.data) + offset,FRAME_END,4);
    }
    free(pData);
    free(faceid);
    DPRINTF(":X Getfacestar");
    return status;
}
