#include "MotionDetect.h"
#include "CVManagerCommon.h"

#undef LOG_TAG
#define LOG_TAG "CVModuleMotion"
#define CV_START        "$#CV" 
#define CV_END          "%%CV" 

CVMotionDetect mMotion;

CVStatus_t CVInit(void *pCom) {
    DPRINTF("MotionDetectModule CVInit E:\n");
    CVStatus_t status = CV_SUCCESS;
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    CVMotionDetect *pMotion   = (CVMotionDetect *)malloc(sizeof(CVMotionDetect));
    pModule->pComponent       = pMotion;
    pModule->mIssmall         = 1;

    pMotion->CVMotionDetectInit();
    return status;
}

CVStatus_t CVDeinit(void *pCom) {
    DPRINTF("MotionDetectModule CVDeInit E:\n");
    CVStatus_t status = CV_SUCCESS;
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    CVMotionDetect *pMotion   = (CVMotionDetect *)pModule->pComponent;

    if (pMotion) {
        pMotion->CVMotionDetectDeInit();
        free(pMotion);
    }
    return status;
}

Moduleoutput CVProcess(uint8_t *ImginputY,
                       uint8_t *ImginputUV,
                       uint32_t width,
                       uint32_t height,
                       uint32_t srcYStride,
                       uint32_t srcVUStride,
                       void *pCom) {
    DPRINTF("MotionDetectModule CVProcess E:\n");
    CVManager_Module *pModule = (CVManager_Module *)pCom;
    CVMotionDetect *pMotion   = (CVMotionDetect *)pModule->pComponent;

    Moduleoutput output;
    output.status = CV_SUCCESS;
    output.type   = CVModuleOut_Score;
    float32_t score;
    output.mScore = pMotion->CheckROIMotionByDiff(ImginputY,
                                                  width,
                                                  height,
                                                  srcYStride,
                                                  0, 0, width, height);

    output.mScore = util_score_mapping(output.mScore,
                                       0,    //Lower Bound input
                                       255,  //Uppper Bound input
                                       5,    //Lower bound output
                                       23,   //upper bound ouput
                                       18,   //alpha value,
                                       1);   //inversed
    DPRINTF("MotionDetect score %d\n", output.mScore);
    return output;
}

CVStatus_t CVGet(void *pData, void *pCom)
{
    DPRINTF("CVGet E:\n");    
    CVManager_Module *pModule = (CVManager_Module *)pCom;    
    uint32_t offset = 0;
    for (uint32_t i = 0 ; i < 10; ++i) {
        memcpy((uint8_t*)pData+offset,CV_START,4);
        offset += 4;
        memcpy((uint8_t*)pData+offset,"xxxx",4);
        offset += 4;        
    }    
    
    DPRINTF("CVGet X:\n");
    return CV_SUCCESS;
}

extern "C"
CVManager_Module* MotionDetectCreateModule() {
    DPRINTF("MotionDetectCreateModule E:\n");
    CVManager_Module *pModule = CVManagerCreateModule("MotionDetect");

    /***Assign Function Pointers***/
    pModule->mAPI.CV_Init    = CVInit;
    pModule->mAPI.CV_DeInit  = CVDeinit;
    pModule->mAPI.CV_Process = CVProcess;
    pModule->mAPI.CV_Get     = CVGet;

    DPRINTF("MotionDetectCreateModule E:\n");
    return pModule;
}
