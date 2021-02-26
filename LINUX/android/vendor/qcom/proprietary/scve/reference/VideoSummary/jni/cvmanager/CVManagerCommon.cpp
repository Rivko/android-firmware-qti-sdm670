/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "CVManagerCommon.h"

#define ASYNC 1
#undef LOG_TAG
#define LOG_TAG "CVManagerCommon"

void* CVCommon_thread_func(void* handle)
{
    DPRINTF(":E ");
    CVManager_Module* pTemp = (CVManager_Module*)handle;
    CVisionInfo_t *pVision  = (CVisionInfo_t*)pTemp->pVision;
    pthread_mutex_lock(&pTemp->cv_mutex);
    DPRINTF("Thread No %d\n", pTemp ->moduleno);
    while (true) {
        DPRINTF("thread Wait\n");
        pthread_cond_wait(&pTemp->cv_cond, &pTemp->cv_mutex);
        DPRINTF("thread Signal Received\n");
        if (pTemp->threadstop) {
            DPRINTF("thread Exit\n");
            break;
        }
#ifdef KPI_DEBUG
        static clock_t start, end;
        start = clock();
#endif
        /**************************************/
        if (pTemp ->mProcesscount%pTemp ->mStepsize == 0) {
            pTemp->mOutput = pTemp->mAPI.CV_Process(pTemp->mImginputY,
                                                    pTemp->mImginputUV,
                                                    pTemp->mwidth,
                                                    pTemp->mheight,
                                                    pTemp->mwidth,
                                                    pTemp->mwidth,
                                                    pTemp);
        pTemp->mLastoutput = pTemp->mOutput;
        }        
        else
            pTemp->mOutput = pTemp->mLastoutput;

        pTemp ->mProcesscount ++;
        if(pTemp->mOutput.status == CV_SUCCESS && pTemp->mOutputCount < OUTPUT_HISTORY_NO)
        {
            pTemp->mOutputCount++;
        }
        /**************************************/
#ifdef KPI_DEBUG
        end = clock();
        DPRINTF("Module %d, Frame Processing Time %f",pTemp->moduleno, ((double)(end - start)) / CLOCKS_PER_SEC);
#endif

        pthread_mutex_lock(&pVision->threadmutex);
        pVision ->threadcount ++;
        pthread_mutex_unlock(&pVision->threadmutex);

        if(pVision ->threadcount == pVision ->totalmodule)
        {
            pthread_mutex_lock(&pVision->threadmutex);
            pthread_cond_signal(&pVision->threadcond);
            pthread_mutex_unlock(&pVision->threadmutex);
        }
    }
    pthread_mutex_unlock(&pTemp->cv_mutex);
    DPRINTF(":X");
    return NULL;
}
void* CVCommon_msg_thread_func(void* handle)
{
    return NULL;
}

/*****Parent Functions*****/
CVStatus_t CVCommonInit(CVManager_Module *pModule)
{
    CVStatus_t status                = CV_SUCCESS;
    pModule ->threadstop             = false;
    //Para initialization
    pModule->mImginputY              = NULL;
    pModule->mImginputUV             = NULL;
    pModule->mwidth                  = 0;
    pModule->mheight                 = 0;
    pModule->msrcYStride             = 0;
    pModule->msrcVUStride            = 0;
    pModule->mImgoutputY             = NULL;
    pModule->mImgoutputUV            = NULL;
    pModule->mdstYStride             = 0;
    pModule->mdstVUStride            = 0;
    pModule->mOutputIndx             = 0;
    pModule->mOutputCount            = 0;
    pModule->mIssmall                = 0;
    pModule->mStepsize               = 1;
    pModule->mProcesscount           = 0;
    pModule->pComponent              = NULL;
    pModule->pData                   = NULL;

    //process thread initialization
    pthread_mutex_init(&pModule->cv_mutex, NULL);
    pthread_cond_init(&pModule->cv_cond, NULL);
    pthread_create(&pModule->cv_aync_thread, NULL,\
                           CVCommon_thread_func, pModule);
    //msg thread initialization
    pthread_mutex_init(&pModule->cv_msg_mutex, NULL);
    pthread_cond_init(&pModule->cv_msg_cond, NULL);
    pthread_create(&pModule->cv_msg_thread, NULL,\
                           CVCommon_msg_thread_func, pModule);
    return status;
}
CVStatus_t CVCommonDeinit(CVManager_Module *pModule)
{
    CVStatus_t status = CV_SUCCESS;
    int32_t ret = 1;
    pModule ->threadstop = true;
    pthread_mutex_lock(&pModule->cv_mutex);
    pthread_cond_signal(&pModule->cv_cond);
    pthread_mutex_unlock(&pModule->cv_mutex);
    ret = pthread_join(pModule->cv_aync_thread, NULL);
    if (ret) {
      DPRINTF("Thread Exit Failure, Failure code %d", ret);
    }
    pthread_mutex_destroy(&pModule->cv_mutex);
    pthread_cond_destroy(&pModule->cv_cond);

    pthread_mutex_lock(&pModule->cv_msg_mutex);
    pthread_cond_signal(&pModule->cv_msg_cond);
    pthread_mutex_unlock(&pModule->cv_msg_mutex);
    ret = pthread_join(pModule->cv_msg_thread, NULL);
    if (ret) {
      DPRINTF("Thread Exit Failure, Failure code %d", ret);
    }
    pthread_mutex_destroy(&pModule->cv_msg_mutex);
    pthread_cond_destroy(&pModule->cv_msg_cond);

    if (pModule->mImginputY != NULL) {
      fcvMemFree(pModule->mImginputY);
      pModule->mImginputY = NULL;
    }

    DPRINTF(":E CVCommonDeinit 2");
    if (pModule->mImgoutputY != NULL) {
      fcvMemFree(pModule->mImgoutputY);
      pModule->mImgoutputY = NULL;
    }

    DPRINTF(":E CVCommonDeinit 7");
    return status;
}
CVStatus_t CVCommonProcess(CVManager_Module *pModule,CVisionInfo_t *pList)
{
    CVStatus_t status = CV_SUCCESS;
#if ASYNC
    if(1 == pModule ->mIssmall)
    {
        pModule->mwidth               = pList ->mwidth >> 2;
        pModule->mheight              = pList ->mheight >> 2;
        pModule->msrcYStride          = pList ->msrcYStride >> 2;
        pModule->msrcVUStride         = pList ->msrcVUStride >> 2;
        pModule->mdstYStride          = pList ->mdstYStride >> 2;
        pModule->mdstVUStride         = pList ->mdstVUStride >> 2;
    }
    else
    {
        pModule->mwidth               = pList->mwidth;
        pModule->mheight              = pList->mheight;
        pModule->msrcYStride          = pList->msrcYStride;
        pModule->msrcVUStride         = pList->msrcVUStride;
        pModule->mdstYStride          = pList->mdstYStride;
        pModule->mdstVUStride         = pList->mdstVUStride;
    }
    if(pModule->mImginputY == NULL)
    {
        pModule->mImginputY           = (uint8_t*)fcvMemAlloc(pModule->mwidth * pModule->mheight * 3 / 2, 16);
        pModule->mImginputUV          = pModule->mImginputY + pModule->mwidth * pModule->mheight;
        pModule->mImgoutputY          = (uint8_t*)fcvMemAlloc(pModule->mwidth * pModule->mheight * 3 / 2, 16);
        pModule->mImgoutputUV         = pModule->mImgoutputY + pModule->mwidth * pModule->mheight;
    }

    pthread_mutex_lock(&pModule->cv_mutex);

    if(1 == pModule ->mIssmall)
    {
        if (pModule->mImginputY && pList->mImginputY) fcvScaleDownBy4u8_v2(pList->mImginputY, pList->mwidth, pList->mheight, pList->msrcYStride, pModule->mImginputY, pModule->msrcYStride);
        if (pModule->mImginputUV && pList->mImginputUV) fcvScaleDownMNInterleaveu8(pList->mImginputUV,
                                                                                   pList->mwidth / 2,
                                                                                   pList->mheight / 2,
                                                                                   pList->msrcVUStride,
                                                                                   pModule->mImginputUV,
                                                                                   pModule->mwidth / 2,
                                                                                   pModule->mheight / 2,
                                                                                   pModule->msrcVUStride);
    }
    else
    {
        if(pModule ->mImginputY && pList->mImginputY)
            memcpy(pModule->mImginputY, pList->mImginputY, pModule->mwidth *pModule->mheight);
        if(pModule ->mImginputUV && pList ->mImginputUV)
            memcpy(pModule->mImginputUV, pList ->mImginputUV, pModule->mwidth * pModule->mheight / 2);
    }
    /////////////////////////////////////////
    pthread_cond_signal(&pModule->cv_cond);
    pthread_mutex_unlock(&pModule->cv_mutex);

    pthread_mutex_lock(&pModule->cv_msg_mutex);
    DPRINTF("Signal Msg \n");
    pthread_cond_signal(&pModule->cv_msg_cond);
    pthread_mutex_unlock(&pModule->cv_msg_mutex);
#else
    Moduleoutput output = pModule->mAPI.CV_Process(
        pList ->mImginputY,
        pList ->mImginputUV,
        pList ->mwidth,
        pList ->mheight,
        pList ->mdstYStride,
        pList ->mdstVUStride,
        pModule);
    status = output.status;
#endif
    return status;
}
/*****************************/

CVManager_Module *CVManagerCreateModule(char modulename[] )
{
    CVManager_Module *pModule = (CVManager_Module *)malloc(sizeof(CVManager_Module));
    strcpy(pModule->modulename, modulename);
    return pModule;
}

CVStatus_t CVManagerAddModule(CVisionInfo_t *pList, CVManager_Module *pModule)
{
    CVStatus_t status = CV_SUCCESS;
    if(pList == NULL || pModule == NULL)
        return CV_ERROR;

    if(pList->totalmodule == MAX_MODULE_NO)
    {
        DPRINTF("Max Module Reached!!");
        return CV_ERROR;
    }

    CVManager_Module **ppmodule   = pList->mModuleList;
    ppmodule[pList->totalmodule]  = pModule;

    pModule ->moduleno            = pList->totalmodule;
    pModule ->pVision             = (void*)pList;

    pList->totalmodule ++;
    return status;
}
CVStatus_t CVManagerCleanModule(CVisionInfo_t *pList)
{
    CVStatus_t status = CV_SUCCESS;
    if(pList == NULL)
        return CV_ERROR;

    CVManagerModuleDeinit(pList);
    for(uint32_t indx = 0 ; indx < pList->totalmodule; ++indx)
    {
        free(pList->mModuleList[indx]);
    }
    return status;
}

CVStatus_t CVManagerModuleInit(CVisionInfo_t *pList)
{
    CVStatus_t status = CV_SUCCESS;
    if(pList == NULL)
        return CV_ERROR;
    pthread_mutex_init(&pList->threadmutex, NULL);
    pthread_cond_init(&pList->threadcond, NULL);

    for(uint32_t indx = 0 ; indx < pList->totalmodule; ++indx)
    {
        CVCommonInit(pList->mModuleList[indx]);
        pList->mModuleList[indx]->mAPI.CV_Init(pList->mModuleList[indx]);
    }
    return status;
}
CVStatus_t CVManagerModuleProcess(CVisionInfo_t *pList)
{
    CVStatus_t status = CV_SUCCESS;
    DPRINTF("CVManagerModuleProcess E:");
    if(pList == NULL)
        return CV_ERROR;
    CVManager_Module *pModule = NULL;
    pList ->threadcount  = 0;

#ifdef KPI_DEBUG
    static clock_t start, end;
    start = clock();
#endif

#if ASYNC
    pthread_mutex_lock(&pList->threadmutex);
#endif
    for(uint32_t indx = 0 ; indx < pList->totalmodule; ++indx)
    {
        pModule = pList->mModuleList[indx];
        status = CVCommonProcess(pModule,pList);
    }
#if ASYNC
    pthread_cond_wait(&pList->threadcond, &pList->threadmutex);
    pthread_mutex_unlock(&pList->threadmutex);
#endif
#ifdef KPI_DEBUG
    end = clock();
    DPRINTF("Overall Frame Processing Time %f", ((double)(end - start)) / CLOCKS_PER_SEC);
#endif

    DPRINTF("CVManagerModuleProcess X:");
    return status;
}

CVStatus_t CVManagerModuleDeinit(CVisionInfo_t *pList)
{
    CVStatus_t status = CV_SUCCESS;
    DPRINTF("CVManagerModuleDeinit E:");
    if(pList == NULL)
        return CV_ERROR;

    for(uint32_t indx = 0 ; indx < pList->totalmodule; ++indx)
    {
        CVCommonDeinit(pList->mModuleList[indx]);
        pList->mModuleList[indx]->mAPI.CV_DeInit(pList->mModuleList[indx]);
    }

    pthread_mutex_destroy(&pList->threadmutex);
    pthread_cond_destroy(&pList->threadcond);
    DPRINTF("CVManagerModuleDeinit X:");
    return status;
}

CVStatus_t CVManagerModuleGet(CVisionInfo_t *pList, CVPara_t *para, uint32_t mode)
{
    CVStatus_t status = CV_SUCCESS;
    DPRINTF("CVManagerModuleGet E:");
    if(pList == NULL)
        return CV_ERROR;

    for(uint32_t indx = 0; indx < pList->totalmodule; ++indx)
    {        
        if (pList->mModuleList[indx] ->identity == mode) {
            pList->mModuleList[indx]->mAPI.CV_Get(para ->data,pList->mModuleList[indx]);            
            DPRINTF("CVManagerModuleGet X:");
            return CV_SUCCESS;
        }
    }

    status = CV_ERROR;
    DPRINTF("CVManagerModuleGet X:");
    return status;
}

CVStatus_t CVManagerModuleSet(CVisionInfo_t *pList, CVPara_t para, uint32_t mode)
{
    CVStatus_t status = CV_SUCCESS;
    DPRINTF("CVManagerModuleSet E:");
    if(pList == NULL)
        return CV_ERROR;

    for(uint32_t indx = 0; indx < pList->totalmodule; ++indx)
    {        
        if (pList->mModuleList[indx] ->identity == mode) {
            pList->mModuleList[indx]->mAPI.CV_Set(para.data,pList->mModuleList[indx]);
            DPRINTF("CVManagerModuleSet X:");
            return CV_SUCCESS;
        }
    }

    status = CV_ERROR;
    DPRINTF("CVManagerModuleSet X:");
    return status;
}
