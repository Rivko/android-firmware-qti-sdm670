#include "CVManagerCommon.h"
#include "scve/scveTypes.hpp"
#include "scve/scveContext.hpp"
#include "scve/scveVideoSummary.hpp"

#undef LOG_TAG
#define LOG_TAG "CVModuleVideoSummary"

SCVE::Context *ctx;
SCVE::VideoSummary *pVideoSummary;
SCVE::Image mImage;

CVStatus_t VSInit(void *pCom) {
    DPRINTF("VideoSummary CVInit E:\n");
    CVStatus_t status = CV_SUCCESS;
    ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_DEFAULT);
    pVideoSummary = NULL;
    return status;
}

CVStatus_t VSDeinit(void *pCom) {
    DPRINTF("VideoSummary CVDeInit E:\n");
    CVStatus_t status = CV_SUCCESS;
    if (pVideoSummary) {
    SCVE::VideoSummary::deleteInstance(pVideoSummary);
    }
    if (ctx) {
    SCVE::Context::deleteInstance(ctx);
    }
    return status;
}

Moduleoutput VSProcess(uint8_t *ImginputY,
                       uint8_t *ImginputUV,
                       uint32_t width,
                       uint32_t height,
                       uint32_t srcYStride,
                       uint32_t srcVUStride,
                       void *pCom) {
    DPRINTF("VideoSummary CVProcess E:\n");
    uint32_t featureMask = uint32_t(SCVE::SCVEVideoSummaryMask_Analytic);
    Moduleoutput output;

    SCVE::InputLocal_t scveinput;
    SCVE::OutputLocal_t scveoutput;
    SCVE::ColorFormat cColorformat;

    cColorformat  = SCVE::SCVE_COLORFORMAT_YUV_NV21;

    output.status = CV_SUCCESS;
    output.type   = CVModuleOut_Score;


    mImage.sDim.nWidth  = width;
    mImage.sDim.nHeight = height;
    mImage.sDim.nStride = srcYStride;
    mImage.nChannels = 3;

    DPRINTF("VideoSummary CVProcess width %d, height %d, stride %d\n",
            width,
            height,
            srcYStride);

    if (pVideoSummary == NULL) {
        DPRINTF("VideoSummary CVProcess Create Instance\n");
        pVideoSummary = SCVE::VideoSummary::newInstance(ctx,
                                                        mImage.sDim,
                                                        cColorformat,
                                                        featureMask);
    }
    mImage.pPtr      = ImginputY; //UV plane is directly after Y
    scveinput.mImage = mImage;
    pVideoSummary->RunSync_Local(&scveinput, &scveoutput);
    DPRINTF("VideoSummary SCVE score %d\n", scveoutput.mFusedOutput);
    output.mScore = static_cast<int32_t>(scveoutput.mFusedOutput);    
    output.mScore = util_score_mapping(output.mScore,
                                       0,    //Lower Bound input
                                       255,  //Uppper Bound input
                                       20,    //Lower bound output
                                       85,   //upper bound ouput
                                       5,   //alpha value,
                                       0);   //inversed
    DPRINTF("VideoSummary SCVE Mapped score %d\n", output.mScore);
    DPRINTF("VideoSummary CVProcess X:\n");
    return output;
}

CVStatus_t VideoSummaryGet(void *pData, void *pCom)
{
    DPRINTF("VideoSummaryGet E:\n");    
    //pData                     = (CVManager_Module *)pCom ->pData;
    DPRINTF("VideoSummaryGet X:\n");
    return CV_SUCCESS;
}

CVStatus_t VideoSummarySet(void *pData, void *pCom)
{
    DPRINTF("VideoSummarySet E:\n");
    uint8_t *para = (uint8_t *)pData;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < MAX_PARA_LEN;) {
        if (para[i] == '$') {
            if (para[i + 1] == '#') {
                if (para[i + 2] == 'M') {
                    if (para[i + 3] == 'D') {
                        VSDeinit(NULL);
                        switch (para[i+4]) {
                        case '0':
                            ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_DEFAULT);
                            pVideoSummary = NULL;
                            DPRINTF("VideoSummarySet Change SCVE Mode default");
                            break;
                        case '1':
                            ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_PERFORMANCE);
                            pVideoSummary = NULL;
                            DPRINTF("VideoSummarySet Change SCVE Mode performance");
                            break;
                        case '2':
                            ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_CPU_OFFLOAD);
                            pVideoSummary = NULL;
                            DPRINTF("VideoSummarySet Change SCVE Mode CPU off load");
                            break;
                        case '3':
                            ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_LOW_POWER);
                            pVideoSummary = NULL;
                            DPRINTF("VideoSummarySet Change SCVE Mode low power");
                            break;
                        default:
                            ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_DEFAULT);
                            pVideoSummary = NULL;
                            DPRINTF("VideoSummarySet Change SCVE Mode default");
                            break;
                        }                       
                        i += 4;
                        break;
                    }
                }
            }
        }
        i++;
    }

    DPRINTF("VideoSummarySet X:\n");
    return CV_SUCCESS;
}

extern "C"
CVManager_Module* VideoSummaryCreateModule() {
    DPRINTF("VideoSummaryCreateModule E:\n");
    CVManager_Module *pModule = CVManagerCreateModule("VideoSummary");

    /***Assign Function Pointers***/
    pModule->mAPI.CV_Init    = VSInit;
    pModule->mAPI.CV_DeInit  = VSDeinit;
    pModule->mAPI.CV_Process = VSProcess;
    pModule->mAPI.CV_Set     = VideoSummarySet;
    pModule->mAPI.CV_Get     = VideoSummaryGet;

    DPRINTF("VideoSummaryCreateModule E:\n");
    return pModule;
}
