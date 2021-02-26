////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxawb.cpp
/// @brief AWB algorithm interface implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxawb.h"
#include "camxdefs.h"
#include "chicommon.h"
#include "chistatsalgo.h"
#include <system/camera_metadata.h>
//#include "q3a_platform.h"

// Logging function pointer for the algorithm
StatsLoggingFunction gStatsLogger;

// Below macros map the core algorithm's logging macros to CamX logging macros
#define STATS_LOG_ERROR(fmt, ...) CAMX_LOG_ERROR(CamxLogGroupStats,     fmt,    ##__VA_ARGS__)
#define STATS_LOG_WARN(fmt, ...)  CAMX_LOG_WARN(CamxLogGroupStats,      fmt,    ##__VA_ARGS__)
#define STATS_LOG_HIGH(fmt, ...)  CAMX_LOG_VERBOSE(CamxLogGroupStats,   fmt,    ##__VA_ARGS__)
#define STATS_LOG_LOW(fmt, ...)   CAMX_LOG_VERBOSE(CamxLogGroupStats,   fmt,    ##__VA_ARGS__)
#define STATS_LOG_INFO(fmt, ...)  CAMX_LOG_INFO(CamxLogGroupStats,      fmt,    ##__VA_ARGS__)
#define AWB_MSG_ERROR(fmt, ...)     gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogError,fmt, ##__VA_ARGS__)
#define AWB_MSG_WARN(fmt, ...)      gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogWarning,fmt, ##__VA_ARGS__)
#define AWB_MSG_HIGH(fmt, ...)      gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogVerbose,fmt, ##__VA_ARGS__)
#define AWB_MSG_LOW(fmt, ...)       gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogVerbose,fmt, ##__VA_ARGS__)
#define AWB_MSG_INFO(fmt, ...)      gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogInfo,fmt, ##__VA_ARGS__)

// NOWHINE CF003,DC011: We're defining the macro
CAMX_NAMESPACE_BEGIN

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static const UINT32 TL84CCT       = 4100;         ///< CCT for TL84 illuminant
static const FLOAT  TL84RGain     = 1.700643f;    ///< Red gain for TL84 illuminant
static const FLOAT  TL84GGain     = 1.000000f;    ///< Red gain for TL84 illuminant
static const FLOAT  TL84BGain     = 2.054097f;    ///< Red gain for TL84 illuminant

// CCM data for TL84 illuminant
static const StatsAWBCCM TL84CCM = { TRUE,                                      ///< Specifies if CCM override is enabled
                                     {{1.810600f,    -0.998800f, 0.188100f},    ///< CCM data 3x3 matrix
                                      {-0.063200f,   1.091400f,  -0.028200f},
                                      {0.123500f,    -0.949200f, 1.825700f}},
                                     {0.0f, 0.0f, 0.0f }};                      ///< RGB offset k0=k1=k2=0


CHIALGORITHMINTERFACE  g_ChiAlgoInterface;                          ///< The instance save the CAMX Chi interface
static UINT32          g_AWBVendorTagBase = 0;                      ///< Chi assigned runtime vendor tag base for AWB

static const CHAR      AWBAlgoSectionName[] = "com.qtistatic.stats.awb";  ///< The section name for AWB

static const UINT32 AWBAlgoBaseTagBase  = 0;                         ///< AWB Vendor Tag base
static const UINT32 AWBAlgoTag1         = AWBAlgoBaseTagBase + 0;           ///< AWB Vendor Tag 1
static const UINT32 AWBAlgoTag2         = AWBAlgoBaseTagBase + 1;           ///< AWB Vendor Tag 2

static const DOUBLE AWBAlgoTag1Value = 12.43f;
static const INT32  AWBAlgoTag2Value = 19;

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagDataAWB[] =
{
    { "AWBAlgoTag1", TYPE_DOUBLE,   1 },
    { "AWBAlgoTag2", TYPE_INT32,    1 },
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagSectionDataAWB[] =
{
    {
        AWBAlgoSectionName,  0,
        sizeof(g_VendorTagDataAWB) / sizeof(g_VendorTagDataAWB[0]), g_VendorTagDataAWB,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoAWB[] =
{
    {
        &g_VendorTagSectionDataAWB[0],
        sizeof(g_VendorTagSectionDataAWB) / sizeof(g_VendorTagSectionDataAWB[0])
    }
};

// Dependent Vendor Tags Info
#define AF_DEPENDENT 0
static UINT32          g_AFVendorTagBase   = 0;                     ///< Chi assigned runtime vendor tag base for AF
static const CHAR      AFAlgoSectionName[] = "com.qtistatic.stats.af";    ///< The section name for AF

static const UINT32   AFAlgoTag1 = 0;   ///< AF Tag 1
static const UINT32   AFAlgoTag2 = 1;   ///< AF Tag 2

//Flash Data
static AWBAlgoFlashEstimationProgress  gFlashEstimationState    = AWBAlgoFlashEstimationInactive;
static AWBAlgoFlashState               gFlashState              = AWBAlgoFlashInactive;

const AWBAlgoGains mainFlashGain = { 3.9f, 1.0f, 2.0f };
const AWBAlgoGains preFlashGain  = { 1.0f, 1.0f, 1.0f };

AWBInternalDataType* pAWBInternalData = NULL;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PopulateVendorTagOutputs()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void PopulateVendorTagOutputs(StatsVendorTagList* pOutputVendorTags)
{
    CAMX_ASSERT(NULL != pOutputVendorTags);

    CAMX_ASSERT((pOutputVendorTags->vendorTag[0].dataSize >= sizeof(AWBAlgoTag1Value)) &&
        (NULL != pOutputVendorTags->vendorTag[0].pData));
    pOutputVendorTags->vendorTag[0].vendorTagId = g_AWBVendorTagBase + AWBAlgoTag1;
    *static_cast<DOUBLE*>(pOutputVendorTags->vendorTag[0].pData) = AWBAlgoTag1Value;
    pOutputVendorTags->vendorTag[0].appliedDelay = 0;
    pOutputVendorTags->vendorTag[0].sizeOfWrittenData = sizeof(AWBAlgoTag1Value);

    CAMX_ASSERT((pOutputVendorTags->vendorTag[1].dataSize >= sizeof(AWBAlgoTag2Value)) &&
        (NULL != pOutputVendorTags->vendorTag[1].pData));
    pOutputVendorTags->vendorTag[1].vendorTagId = g_AWBVendorTagBase + AWBAlgoTag2;
    *static_cast<FLOAT*>(pOutputVendorTags->vendorTag[1].pData) = AWBAlgoTag2Value;
    pOutputVendorTags->vendorTag[1].appliedDelay = 0;
    pOutputVendorTags->vendorTag[1].sizeOfWrittenData = sizeof(AWBAlgoTag2Value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetHardCodedOutput()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GetHardCodedOutput(
    AWBAlgoOutputList*      pOutputs)
{

    for (UINT32 i = 0; i < pOutputs->outputCount; i++)
    {
        switch (pOutputs->pAWBOutputs[i].outputType)
        {
            case AWBOutputTypeGains:
                if (AWBAlgoFlashEstimationRunning == gFlashEstimationState)
                {
                    *static_cast<AWBAlgoGains*>(pOutputs->pAWBOutputs[i].pAWBOutput) = preFlashGain;
                    AWB_MSG_HIGH("Populating Preflash Running Gain(R:%f, G:%f, B:%f)",
                                 preFlashGain.red, preFlashGain.green, preFlashGain.blue);
                }
                else if (AWBAlgoPreFlashCompleteLED == gFlashState)
                {
                    *static_cast<AWBAlgoGains*>(pOutputs->pAWBOutputs[i].pAWBOutput) = { TL84RGain, TL84GGain, TL84BGain };
                    AWB_MSG_HIGH("Populating restore Gains(R:%f, G:%f, B:%f)",
                                 TL84RGain, TL84GGain, TL84BGain);
                }
                else
                {
                    *static_cast<AWBAlgoGains*>(pOutputs->pAWBOutputs[i].pAWBOutput) = { TL84RGain, TL84GGain, TL84BGain };
                }
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput = sizeof(AWBAlgoGains);
                break;
            case AWBOutputTypeColorTemperature:
                *static_cast<UINT32*>(pOutputs->pAWBOutputs[i].pAWBOutput)  = TL84CCT;
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput             = sizeof(UINT32);
                break;
            case AWBOutputTypeIlluminantType:
                *static_cast<StatsIlluminantType*>(pOutputs->pAWBOutputs[i].pAWBOutput)   = StatsIlluminantTL84;
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput                           = sizeof(StatsIlluminantType);
                break;
            case AWBOutputTypeSampleDecision:
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput = AWBAlgoDecisionMapSize * sizeof(StatsIlluminantType);
                break;
            case AWBOutputTypeBGConfig:
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput = 0;
                break;
            case AWBOutputTypeState:
                *static_cast<AWBAlgoState*>(pOutputs->pAWBOutputs[i].pAWBOutput)    = AWBAlgoStateConverged;
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput                     = sizeof(AWBAlgoState);
                break;
            case AWBOutputTypeMode:
                *static_cast<AWBAlgoMode*>(pOutputs->pAWBOutputs[i].pAWBOutput) = AWBAlgoModeAuto;
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput                 = sizeof(AWBAlgoMode);
                break;
            case AWBOutputTypeLock:
                *static_cast<BOOL*>(pOutputs->pAWBOutputs[i].pAWBOutput)    = FALSE;
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput             = sizeof(BOOL);
                break;
            case AWBOutputTypeVendorTag:
                PopulateVendorTagOutputs(static_cast<StatsVendorTagList*>(pOutputs->pAWBOutputs[i].pAWBOutput));
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput = sizeof(StatsVendorTagList);
                break;
            case AWBOutputTypeDebugData:
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput = sizeof(StatsDataPointer);
                break;
            case AWBOutputTypeCCM:
                StatsAWBCCMList AWBCCM;
                Utils::Memset(&AWBCCM, 0, sizeof(StatsAWBCCMList));
                AWBCCM.numValidCCMs = 1;
                AWBCCM.CCM[0] = TL84CCM;

                *static_cast<StatsAWBCCMList*>(pOutputs->pAWBOutputs[i].pAWBOutput) = AWBCCM;
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput                 = sizeof(StatsAWBCCM);
                break;
            default:
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetFlashOutput()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GetFlashOutput(
    AWBAlgoOutputList*      pOutputs)
{

    for (UINT32 i = 0; i < pOutputs->outputCount; i++)
    {
        switch (pOutputs->pAWBOutputs[i].outputType)
        {
            case AWBOutputTypeGains:
                *static_cast<AWBAlgoGains*>(pOutputs->pAWBOutputs[i].pAWBOutput) = mainFlashGain;
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput = sizeof(AWBAlgoGains);
                AWB_MSG_HIGH("Populating Main Flash AWB Gain (R:%f, G:%f, B:%f)",
                             mainFlashGain.red, mainFlashGain.green, mainFlashGain.blue);
                break;
            case AWBOutputTypeColorTemperature:
                *static_cast<UINT32*>(pOutputs->pAWBOutputs[i].pAWBOutput) = TL84CCT;
                pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput = sizeof(UINT32);
                break;
            default:
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AWBDestroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC VOID AWBDestroy(
    CHIAWBAlgorithm*                pAWBAlgorithm,
    const AWBAlgoDestroyParamList*  pDestroyParams)
{
    CAMX_UNREFERENCED_PARAM(pAWBAlgorithm);
    CAMX_LOG_ERROR(CamxLogGroupStats, "StaticAWB enter");
    for (UINT32 j = 0; j < pDestroyParams->paramCount; j++)
    {
        AWBAlgoDestroyParam*   pParamList = &pDestroyParams->pParamList[j];
        switch (pParamList->destroyParamType)
        {
            case AWBDestroyParamTypeCameraCloseIndicator:
            {
                UINT overrideCameraClose = *static_cast<UINT*>(pParamList->pParam);
                if (1 == overrideCameraClose)
                {
                    if (NULL != pAWBInternalData)
                    {
                        CAMX_LOG_ERROR(CamxLogGroupStats, "StaticAWB pAWBInternalData in destroy is %p", pAWBInternalData);
                        CAMX_DELETE(pAWBInternalData);
                        pAWBInternalData = NULL;
                    }
                }
                else
                {
                    CAMX_LOG_ERROR(CamxLogGroupStats, "overrideCameraClose in  AWBDestroy %d", overrideCameraClose);
                }
                break;
            }
            default:
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AWBProcess
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult AWBProcess(
    CHIAWBAlgorithm*        pAWBAlgorithm,
    const AWBAlgoInputList* pInputs,
    AWBAlgoOutputList*      pOutputs)
{
    CDKResult result    = CDKResultSuccess;
    UINT64    requestId  = 0;
    CHISTATSHANDLE  chiStatsHandle = NULL;

    if ((NULL == pAWBAlgorithm) || (NULL == pInputs) || (NULL == pOutputs))
    {
        CAMX_LOG_ERROR(CamxLogGroupStats, "Invalid args pAWBAlgorithm:%p pInputs:%p pOutputs:%p");
        result = CDKResultEInvalidArg;
        return result;
    }

    if (CDKResultSuccess == result)
    {
        for (UINT32 i = 0; i < pInputs->inputCount; i++)
        {
            if (pInputs->pAWBInputs[i].sizeOfAWBInput == 0)
                continue;

            switch (pInputs->pAWBInputs[i].inputType)
            {
            case AWBInputTypeRequestNumber:
                {
                    requestId = *static_cast<UINT64*>(pInputs->pAWBInputs[i].pAWBInput);
                    break;
                }
            case AWBInputTypeStatsChiHandle:
                {
                    chiStatsHandle = static_cast<CHISTATSHANDLE>(pInputs->pAWBInputs[i].pAWBInput);
                    break;
                }

                case AWBInputTypeVendorTag:
                {
                    StatsVendorTagList* pVendorTagList = static_cast<StatsVendorTagList*>(pInputs->pAWBInputs[i].pAWBInput);
                    for (UINT32 j = 0; j < pVendorTagList->vendorTagCount; j++)
                    {
                        AWB_MSG_HIGH("Received vendor tag id(%u) pData(%p) size(%u)",
                                     pVendorTagList->vendorTag[j].vendorTagId,
                                     pVendorTagList->vendorTag[j].pData,
                                     pVendorTagList->vendorTag[j].dataSize);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    if (NULL != g_ChiAlgoInterface.pGetMetadata && NULL != g_ChiAlgoInterface.pSetMetaData)
    {
        //Read Dynamic VendorTags
        CHIMETADATAINFO AFVendortagInfo = { 0 };
        UINT32          AFVendorTagList[2] = { g_AFVendorTagBase + AFAlgoTag1 , g_AFVendorTagBase + AFAlgoTag2 };
        CHITAGDATA      tagData[2] = { { sizeof(CHITAGDATA), requestId, NULL, 0, 0, FALSE },
                                       { sizeof(CHITAGDATA), requestId, NULL, 0, 0, FALSE } };

        AFVendortagInfo.pTagList = &AFVendorTagList[0];
        AFVendortagInfo.tagNum = 2;
        AFVendortagInfo.size = sizeof(CHIMETADATAINFO);
        AFVendortagInfo.pTagData = &tagData[0];
        AFVendortagInfo.metadataType = ChiMetadataDynamic;
        AFVendortagInfo.chiSession = chiStatsHandle;

        g_ChiAlgoInterface.pGetMetadata(&AFVendortagInfo);
        for (UINT32 j = 0; j < AFVendortagInfo.tagNum; j++)
        {
            AWB_MSG_HIGH("Received vendor tag using GetMetadat API. TagId(%u) pData(%p) DataSize(%u) reqID(%llu)",
                AFVendortagInfo.pTagList[j],
                AFVendortagInfo.pTagData[j].pData,
                AFVendortagInfo.pTagData[j].dataSize,
                AFVendortagInfo.pTagData[j].requestId);
        }

        // Read static vendor tags
        UINT32          LensFacingId = 524293;
        CHIMETADATAINFO StaticVendortagInfo = { 0 };
        UINT32          StaticVendortagList[1] = { LensFacingId };
        CHITAGDATA      staticTagData[1] = { { sizeof(CHITAGDATA), 0, NULL, 0, 0, FALSE } };

        StaticVendortagInfo.pTagData = &staticTagData[0];
        StaticVendortagInfo.pTagList = &StaticVendortagList[0];
        StaticVendortagInfo.tagNum = 1;
        StaticVendortagInfo.metadataType = ChiMetadataStatic;
        StaticVendortagInfo.size = sizeof(CHIMETADATAINFO);
        StaticVendortagInfo.chiSession = chiStatsHandle;

        result = g_ChiAlgoInterface.pGetMetadata(&StaticVendortagInfo);
        if ((CDKResultSuccess != result) || (NULL == staticTagData[0].pData))
        {
            AWB_MSG_ERROR("Failed to receive static vendor tag Lens Facing.  id(%u)", StaticVendortagInfo.pTagList[0]);
        }
        else
        {
            AWB_MSG_HIGH("Received static vendor tag Lens Facing.  id(%u) value(%d) DataSize(%d)",
                StaticVendortagInfo.pTagList[0],
                *static_cast<BYTE*>(staticTagData[0].pData),
                staticTagData[0].dataSize);
        }

        // Read input vendor tag
        //UINT32          ControlCaptureIntent    = 65549;
        UINT32          ControlAELock = 65538;
        CHIMETADATAINFO InputVendortagInfo = { 0 };
        UINT32          InputVendortagList[1] = { ControlAELock };
        CHITAGDATA      InputtagData[1] = { { sizeof(CHITAGDATA), 0, NULL, 0, 0, FALSE } };

        InputVendortagInfo.pTagData = &InputtagData[0];
        InputVendortagInfo.pTagData->requestId = requestId;
        InputVendortagInfo.pTagList = &InputVendortagList[0];
        InputVendortagInfo.tagNum = 1;
        InputVendortagInfo.metadataType = ChiMetadataDynamic;
        InputVendortagInfo.size = sizeof(CHIMETADATAINFO);
        InputVendortagInfo.chiSession = chiStatsHandle;

        // OEM_AWB_LOG_VERBOSE("Retrieving input tag for requestId %" PRIu64, requestId);
        result = g_ChiAlgoInterface.pGetMetadata(&InputVendortagInfo);

        if ((CDKResultSuccess != result) || (NULL == InputtagData[0].pData))
        {
            AWB_MSG_ERROR("Failed to receive input vendor tag ControlAELock id(%u) ", InputVendortagInfo.pTagList[0]);
        }
        else
        {
            AWB_MSG_HIGH("Received input vendor tag ControlAELock.  id(%u) value(%d) DataSize(%d)",
                InputVendortagInfo.pTagList[0],
                *static_cast<BYTE*>(InputtagData[0].pData),
                InputtagData[0].dataSize);
        }
    }
    // update flash state
    if (AWBAlgoPreFlashTriggerAWB == gFlashState)
    {
        static UINT preflashFrameCount = 3;
        preflashFrameCount -= 1;

        if (0 == preflashFrameCount)
        {
            gFlashEstimationState = AWBAlgoFlashEstimationDone;
            preflashFrameCount = 3;
        }
        else
        {
            gFlashEstimationState = AWBAlgoFlashEstimationRunning;
        }

    }
    else if ((AWBAlgoPreFlashCompleteLED == gFlashState) || (AWBAlgoPreFlashCompleteNoLED == gFlashState))
    {
        AWB_MSG_INFO("Received AWBAlgoPreFlashCompleteLED. Restoring previous gains");
    }
    else if (AWBAlgoMainFlash == gFlashState)
    {
        AWB_MSG_INFO("Received AWBAlgoMainFlash. Skipping frame and providing previous gains");
    }

    if (CDKResultSuccess == result)
    {
        /// @todo (CAMX-1012): Integrate AWB algorithm
        // Hardcode process output for now
        GetHardCodedOutput(pOutputs);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AWBGetParam
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult AWBGetParam(
    CHIAWBAlgorithm*    pAWBAlgorithm,
    AWBAlgoGetParam*    pGetParams)
{
    CDKResult result = CDKResultSuccess;

    if ((NULL == pAWBAlgorithm) || (NULL == pGetParams))
    {
        result = CDKResultEInvalidArg;
    }
    else
    {
        if (AWBGetParamTypeLastOutput == pGetParams->type)
        {
            /// @todo (CAMX-1012): Integrate AWB algorithm
            // Hardcode initial outputs
            AWBAlgoGetParamOutput*  pGetParamLastOutput =
                &pGetParams->outputInfoList.pGetParamOutputs[AWBGetParamTypeLastOutput];
            AWBAlgoOutputList*      pLastOutputList     = static_cast<AWBAlgoOutputList*>(pGetParamLastOutput->pGetParamOutput);

            CAMX_ASSERT(NULL != pLastOutputList);

            GetHardCodedOutput(pLastOutputList);
        }
        else if (AWBGetParamTypeFlashEstimationState == pGetParams->type)
        {
            AWBAlgoGetParamOutput*  pGetParamLastOutput                                         =
                &pGetParams->outputInfoList.pGetParamOutputs[AWBGetParamTypeFlashEstimationState];

            CAMX_ASSERT(NULL != pGetParamLastOutput->pGetParamOutput);
            *static_cast<AWBAlgoFlashEstimationProgress*>(pGetParamLastOutput->pGetParamOutput) = gFlashEstimationState;
            pGetParamLastOutput->sizeOfWrittenGetParamOutput = sizeof(AWBAlgoFlashEstimationProgress);
        }
        else if (AWBGetParamTypeFlashOutput == pGetParams->type)
        {
            AWBAlgoGetParamOutput*  pGetParamLastOutput =
                &pGetParams->outputInfoList.pGetParamOutputs[AWBGetParamTypeLastOutput];
            AWBAlgoOutputList*      pFlashOutputList    = static_cast<AWBAlgoOutputList*>(pGetParamLastOutput->pGetParamOutput);

            CAMX_ASSERT(NULL != pFlashOutputList);

            if (AWBAlgoFlashEstimationDone == gFlashEstimationState)
            {
                GetFlashOutput(pFlashOutputList);
                pGetParamLastOutput->sizeOfWrittenGetParamOutput = sizeof(AWBAlgoFlashEstimationProgress);
            }
            else
            {
                pGetParamLastOutput->sizeOfWrittenGetParamOutput = 0;
            }

        }
        else if (AWBGetParamTypePublishingVendorTagsInfo == pGetParams->type)
        {

            AWBAlgoGetParamOutput*  pGetParamOutput =
                &pGetParams->outputInfoList.pGetParamOutputs[AWBGetParamTypePublishingVendorTagsInfo];

            CAMX_ASSERT(AWBGetParamOutputTypePublishingVendorTagsInfo == pGetParamOutput->getParamOutputType);


            StatsVendorTagInfoList* pVendorTagList = static_cast<StatsVendorTagInfoList*>(pGetParamOutput->pGetParamOutput);

            pVendorTagList->vendorTag[0]   = { g_AWBVendorTagBase+AWBAlgoTag1, 0 };
            pVendorTagList->vendorTag[1]   = { g_AWBVendorTagBase+AWBAlgoTag2, 0 };
            pVendorTagList->vendorTagCount = g_VendorTagSectionDataAWB[0].numTags;

            pGetParamOutput->sizeOfWrittenGetParamOutput = sizeof(StatsVendorTagInfoList);
        }
        else if (AWBGetParamTypeDependentVendorTags == pGetParams->type)
        {
            AWBAlgoGetParamOutput*  pGetParamOutput =
                &pGetParams->outputInfoList.pGetParamOutputs[AWBGetParamTypeDependentVendorTags];

            CAMX_ASSERT(AWBGetParamOutputTypeDependentVendorTags == pGetParamOutput->getParamOutputType);

            StatsVendorTagInfoList* pVendorTagList = static_cast<StatsVendorTagInfoList*>(pGetParamOutput->pGetParamOutput);

#if AF_DEPENDENT
            pVendorTagList->vendorTag[0].vendorTagId  = g_AFVendorTagBase + AFAlgoTag1;
            pVendorTagList->vendorTag[0].appliedDelay = 0;
            pVendorTagList->vendorTag[1].vendorTagId = g_AFVendorTagBase + AFAlgoTag2;
            pVendorTagList->vendorTag[1].appliedDelay = 0;
            pVendorTagList->vendorTagCount = 2;
#else //AF_DEPENDENT
            pVendorTagList->vendorTagCount = 0;
#endif //AF_DEPENDENT

            pGetParamOutput->sizeOfWrittenGetParamOutput = sizeof(pVendorTagList);
        }
        else
        {
            /// @todo (CAMX-1012): Integrate AWB algorithm
            result = CDKResultENotImplemented;
            AWB_MSG_ERROR("Get Param(%d) Not yet implemented!", pGetParams->type);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AWBSetParam
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult AWBSetParam(
    CHIAWBAlgorithm*            pAWBAlgorithm,
    const AWBAlgoSetParamList*  pSetParams)
{
    CDKResult result = CDKResultSuccess;

    if ((NULL != pAWBAlgorithm) && (NULL != pSetParams))
    {
        for (SIZE_T i = 0; i < pSetParams->inputParamsCount && result == CDKResultSuccess; i++)
        {
            AWBAlgoSetParam* pAWBSetParamInfo = &(pSetParams->pAWBSetParams[i]);

            if (pAWBSetParamInfo->pAWBSetParam == NULL)
            {
                continue;
            }

            switch (pAWBSetParamInfo->setParamType)
            {
                case AWBSetParamTypeFlashData:
                {
                    gFlashState = reinterpret_cast<const AWBAlgoFlashInformation*>(pAWBSetParamInfo->pAWBSetParam)->flashState;
                    AWB_MSG_HIGH("Received Flash state: %d", gFlashState);

                    if ((AWBAlgoPreFlashCompleteLED == gFlashState) || (AWBAlgoPreFlashCompleteNoLED == gFlashState))
                    {
                        gFlashEstimationState = AWBAlgoFlashEstimationInactive;
                    }

                    break;
                }

                default:
                    break;
            }
        }
    }
    else
    {
        result = CDKResultEInvalidArg;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CreateAWBAlgorithm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CreateAWBAlgorithm(
    const AWBAlgoCreateParamList*   pCreateParams,
    CHIAWBAlgorithm**               ppAWBAlgorithm)
{
    CDKResult result = CDKResultSuccess;
    CHISTATSHANDLE  chiSessionHandle = NULL;
    UINT* pOverrideCameraOpen = NULL;

    if ((NULL == pCreateParams) || (NULL == ppAWBAlgorithm))
    {
        AWB_MSG_ERROR("Invalid Args pCreateParams:%p ppAWBAlgorithm:%p", pCreateParams, ppAWBAlgorithm);
        result = CDKResultEInvalidArg;
    }

    if (result == CDKResultSuccess)
    {
        for (UINT32 j = 0; j < pCreateParams->createParamsCount; j++)
        {
            AWBAlgoCreateParam*   pParamList = &pCreateParams->pCreateParams[j];

            switch (pParamList->createParamType)
            {
            case AWBCreateParamTypeSessionHandle:
            {
                CAMX_LOG_INFO(CamxLogGroupStats, "StaticAWB algo sizeOfParam for CHISTATSHANDLE is %d", pParamList->sizeOfCreateParam);
                if ((pParamList->pCreateParam != NULL) && (pParamList->sizeOfCreateParam >= sizeof(CHISTATSHANDLE)))
                {
                    chiSessionHandle = static_cast<CHISTATSHANDLE>(pParamList->pCreateParam);

                    // Read static vendor tags
                    UINT32          LensFacingId = ANDROID_LENS_FACING;
                    CHIMETADATAINFO StaticVendortagInfo = { 0 };
                    UINT32          StaticVendortagList[1] = { LensFacingId };
                    CHITAGDATA      staticTagData[1] = { { sizeof(CHITAGDATA), 0, NULL, 0, 0, FALSE } };

                    StaticVendortagInfo.pTagData = &staticTagData[0];
                    StaticVendortagInfo.pTagList = &StaticVendortagList[0];
                    StaticVendortagInfo.tagNum = 1;
                    StaticVendortagInfo.metadataType = ChiMetadataStatic;
                    StaticVendortagInfo.size = sizeof(CHIMETADATAINFO);
                    StaticVendortagInfo.chiSession = chiSessionHandle;

                    result = g_ChiAlgoInterface.pGetMetadata(&StaticVendortagInfo);
                    if ((CDKResultSuccess != result) || (NULL == staticTagData[0].pData))
                    {
                        CAMX_LOG_ERROR(CamxLogGroupStats,
                            "Failed to receive static vendor tag Lens Facing.  id(%u)",
                            StaticVendortagInfo.pTagList[0]);
                    }
                    else
                    {
                        CAMX_LOG_INFO(CamxLogGroupStats,
                            "Received static vendor tag Lens Facing.  id(%u) value(%d) DataSize(%d)",
                            StaticVendortagInfo.pTagList[0],
                            *static_cast<BYTE*>(staticTagData[0].pData),
                            staticTagData[0].dataSize);
                    }
                }
                break;
            }

            case AWBCreateParamTypeCameraOpenIndicator:
            {
                if ((pParamList->pCreateParam != NULL) && (pParamList->sizeOfCreateParam >= sizeof(UINT)))
                {
                    pOverrideCameraOpen = static_cast<UINT*>(pParamList->pCreateParam);
                    CAMX_LOG_INFO(CamxLogGroupStats, "StaticAWB algo create: overrideCameraOpen is %d", *pOverrideCameraOpen);
                }
                break;
            }

            default:
                break;
            }
        }
    }

    if (CDKResultSuccess == result)
    {
        if (pCreateParams != NULL && pCreateParams->pCreateParams != NULL)
        {
            AWBAlgoCreateParam* pCreateLoggerFunction = NULL;
            pCreateLoggerFunction = &pCreateParams->pCreateParams[AWBCreateParamsTypeLoggerFunctionPtr];
            gStatsLogger = reinterpret_cast<StatsLoggingFunction>(pCreateLoggerFunction->pCreateParam);
            result = CDKResultSuccess;
        }
    }

    if ((NULL != pOverrideCameraOpen) && (1 == (*pOverrideCameraOpen)))
    {
        if (NULL == pAWBInternalData)
        {
            CAMX_LOG_INFO(CamxLogGroupStats, "pAWBInternalData before %p", pAWBInternalData);
            pAWBInternalData = CAMX_NEW AWBInternalDataType;
            if (NULL == pAWBInternalData)
            {
                result = CDKResultENoMemory;
            }
            else
            {
                if (CDKResultSuccess == result)
                {
                    pAWBInternalData->hAWBAlgorithm = NULL; /// @todo (CAMX-1012): Integrate AWB algorithm
                    if (CDKResultSuccess == result)
                    {
                        pAWBInternalData->AWBFuctions.AWBProcess = AWBProcess;
                        pAWBInternalData->AWBFuctions.AWBSetParam = AWBSetParam;
                        pAWBInternalData->AWBFuctions.AWBGetParam = AWBGetParam;
                        pAWBInternalData->AWBFuctions.AWBDestroy = AWBDestroy;
                        *ppAWBAlgorithm = reinterpret_cast<CHIAWBAlgorithm*>(pAWBInternalData);
                        CAMX_LOG_INFO(CamxLogGroupStats, "pAWBInternalData first time %p", pAWBInternalData);
                    }
                    else
                    {
                        CAMX_DELETE pAWBInternalData;
                        pAWBInternalData = NULL;
                    }
                }
            }
        }
        else
        {
            CAMX_LOG_INFO(CamxLogGroupStats, "pAWBInternalData after %p", pAWBInternalData);
            *ppAWBAlgorithm = reinterpret_cast<CHIAWBAlgorithm*>(pAWBInternalData);
            result = CDKResultSuccess;
        }
        // AWB_MSG_HIGH("CreateAWBAlgorithm static result %d", CamxResultStrings[result]);
    }
    else
    {
        CAMX_LOG_INFO(CamxLogGroupStats, "AWB:Not actual open ...ignore:pOverrideCameraOpen %p", pOverrideCameraOpen);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AWBGetCapabilities
///
/// @brief  This method creates an instance to the AFAlgorithm.
///
/// @param  pCapsInfo Pointer to Algo capability structure
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AWBGetCapabilities(CHIALGORITHMCAPSINFO* pCapsInfo)
{
    CDKResult   result = CDKResultSuccess;

    AWB_MSG_HIGH("Get supported Capabilities size:%d, algo mask: %d", pCapsInfo->size, pCapsInfo->algorithmCapsMask);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AWBNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AWBNodeQueryVendorTag(
    CHIQUERYVENDORTAG* pQueryVendorTag)
{
    CDKResult result = CDKResultSuccess;
    if (NULL == pQueryVendorTag)
    {
        result = CDKResultEInvalidPointer;
        AWB_MSG_ERROR("Invalid argument: pQueryVendorTag is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (pQueryVendorTag->size >= sizeof(CHIQUERYVENDORTAG))
        {
            pQueryVendorTag->pVendorTagInfo = g_VendorTagInfoAWB;
        }
        else
        {
            AWB_MSG_ERROR("pQueryVendorTag is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AWBAlgoSetNodeInterface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID AWBAlgoSetNodeInterface(
    ChiAlgorithmInterface* pAlgoInterface)
{
    CAMX_ASSERT((NULL != pAlgoInterface) &&
                (NULL != pAlgoInterface->pGetVendorTagBase) &&
                (NULL != pAlgoInterface->pGetMetadata) &&
                (NULL != pAlgoInterface->pSetMetaData));

    int result;
    g_ChiAlgoInterface = *pAlgoInterface;

    // get the AWB vendor tag base
    CHIVENDORTAGBASEINFO vendorTagBase = { 0 };
    vendorTagBase.size = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName = AWBAlgoSectionName;

    result = g_ChiAlgoInterface.pGetVendorTagBase(&vendorTagBase);
    if (CDKResultSuccess == result)
    {
        g_AWBVendorTagBase = vendorTagBase.vendorTagBase;
    }

    // get the AF vendor tag base
    vendorTagBase.pComponentName = AFAlgoSectionName;

    result = g_ChiAlgoInterface.pGetVendorTagBase(&vendorTagBase);
    if (CDKResultSuccess == result)
    {
        g_AFVendorTagBase = vendorTagBase.vendorTagBase;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiAWBAlgorithmEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom AWB Algorithm.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC void ChiAWBAlgorithmEntry(CHIAWBALGORITHMCALLBACKS* pCallBacks)
{
    // AWB_MSG_HIGH("ChiAWBAlgorithmEntry static");

    if (NULL == pCallBacks)
    {
        STATS_LOG_ERROR("Invalid argument: pCallBacks is NULL");
    }
    else if (pCallBacks->size < sizeof(CHIAWBALGORITHMCALLBACKS))
    {
        STATS_LOG_ERROR("pCallBacks is smaller than expected");
    }
    else
    {
        pCallBacks->pfnGetCapabilities  = AWBGetCapabilities;
        pCallBacks->pfnCreate           = CreateAWBAlgorithm;
        pCallBacks->pfnQueryVendorTag   = AWBNodeQueryVendorTag;
        pCallBacks->pfnSetAlgoInterface = AWBAlgoSetNodeInterface;
    }
}

#ifdef __cplusplus
}
#endif // __cplusplus

CAMX_NAMESPACE_END
