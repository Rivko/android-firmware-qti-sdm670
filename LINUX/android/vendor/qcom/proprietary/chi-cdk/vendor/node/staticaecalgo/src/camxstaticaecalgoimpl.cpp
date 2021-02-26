////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxstaticaecalgoimpl.cpp
/// @brief The class that implements the static AEC algorithm.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxdefs.h"
#include "camxmem.h"
#include "camxstaticaecalgoimpl.h"
#include "camxutils.h"
#include "chistatsalgo.h"
#include <system/camera_metadata.h>


CAMX_NAMESPACE_BEGIN

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// START Static AEC algorithm implementation
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static UINT32  g_AECVendorTagBase = 0;    ///< Chi assigned runtime vendor tag base for the AEC

static const CHAR      AECAlgoSectionName[] = "com.qtistatic.stats.aec";    ///< The section name for AEC
CHIALGORITHMINTERFACE  g_ChiAlgoInterface;                                  ///< The instance save the CAMX Chi interface

static const UINT32 AECAlgoVendorTagBase = 0;                          ///< Tag base
static const UINT32 AECAlgoTag1          = AECAlgoVendorTagBase + 0;   ///< Tag for VendorTag1
static const UINT32 AECAlgoTag2          = AECAlgoVendorTagBase + 1;   ///< Tag for VendorTag2

static const FLOAT AECAlgoTag1Value = 12.55f;
static const INT32 AECAlgoTag2Value = 18;


AECAlgoOperationModeType currentOperationMode = AECAlgoOperationModeMax;

/* hard coded gains and currents START */

static const UINT64 startupExpTime      = 16666666;
static const FLOAT  startupGain         = 4.0f;

static const UINT64 previewExpTime      = 33333333;
static const FLOAT  previewGain         = 8.0f;

static const UINT64 preFlashExpTime     = 41465865;
static const FLOAT  preFlashGain        = 7.534790f;

static const UINT64 mainFlashExpTime    = 27640562;
// static const FLOAT  mainFlashGain       = 4.137608f;

UINT32  preFlashLEDCurrents[StatisticsMaxNumOfLED];  ///< The LED currents for LED snapshot
UINT32  mainFlashLEDCurrents[StatisticsMaxNumOfLED]; ///< The LED currents for LED snapshot

static const UINT32 numberOfFramesToConverge        = 15;
static UINT32       currentFrameCount               = 0;
static BOOL         bAlgoDecisionForFlashRequired   = TRUE; ///< Indicates if preflash is required for capture or not
static BOOL         bAlgoDecisionForFlashAFRequired = TRUE; ///< Indicates if AF under preflash required for capture or not

/* hard coded gains and currents END */

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagDataAEC[] =
{
    { "VendorTag1", TYPE_DOUBLE,  1 },
    { "VendorTag2", TYPE_INT32,   1 },
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagSectionDataAEC[] =
{
    {
        AECAlgoSectionName,  0,
        sizeof(g_VendorTagDataAEC) / sizeof(g_VendorTagDataAEC[0]), g_VendorTagDataAEC,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoAEC[] =
{
    {
        &g_VendorTagSectionDataAEC[0],
        sizeof(g_VendorTagSectionDataAEC) / sizeof(g_VendorTagSectionDataAEC[0])
    }
};

StaticInterface* pAEC = NULL;

// Dependent Vendor Tags Info
#define AF_DEPENDENT 0
#if AF_DEPENDENT
static UINT32          g_AFVendorTagBase   = 0;                            ///< Chi assigned runtime vendor tag base for AF
static const CHAR      AFAlgoSectionName[] = "com.qtistatic.stats.af";    ///< The section name for AF

static const UINT32   AFAlgoTag1 = 0;   ///< AF Tag 1
static const UINT32   AFAlgoTag2 = 1;   ///< AF Tag 2
#endif //AF_DEPENDENT

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PopulateVendorTagOutputs()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void PopulateVendorTagOutputs(StatsVendorTagList* pOutputVendorTags)
{
    CAMX_ASSERT(NULL != pOutputVendorTags);

    CAMX_ASSERT((pOutputVendorTags->vendorTag[0].dataSize >= sizeof(AECAlgoTag1Value)) &&
        (NULL != pOutputVendorTags->vendorTag[0].pData));
    pOutputVendorTags->vendorTag[0].vendorTagId = g_AECVendorTagBase + AECAlgoTag1;
    *static_cast<FLOAT*>(pOutputVendorTags->vendorTag[0].pData) = AECAlgoTag1Value;
    pOutputVendorTags->vendorTag[0].appliedDelay = 0;
    pOutputVendorTags->vendorTag[0].sizeOfWrittenData = sizeof(AECAlgoTag1Value);

    CAMX_ASSERT((pOutputVendorTags->vendorTag[1].dataSize >= sizeof(AECAlgoTag2Value)) &&
        (NULL != pOutputVendorTags->vendorTag[1].pData));
    pOutputVendorTags->vendorTag[1].vendorTagId = g_AECVendorTagBase + AECAlgoTag2;
    *static_cast<FLOAT*>(pOutputVendorTags->vendorTag[1].pData) = AECAlgoTag2Value;
    pOutputVendorTags->vendorTag[1].appliedDelay = 0;
    pOutputVendorTags->vendorTag[1].sizeOfWrittenData = sizeof(AECAlgoTag2Value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HardcodedAECAlgoExposureSet
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID HardcodedAECAlgoExposureSet(
    AECAlgoExposureSet*     pExposureSet,
    AECAlgoGetParam*        pGetParam)
{
    UINT64  exposureTime = previewExpTime;
    FLOAT   gain         = previewGain;

    if (AECAlgoOperationModePreflash == currentOperationMode)
    {
        exposureTime    = preFlashExpTime;
        gain            = preFlashGain;
    }
    else
    {
        exposureTime    = previewExpTime;
        gain            = previewGain;
    }

    /* if this is called through get param, and snapshot type is Flash */
    if (NULL != pGetParam)
    {
        if (AECAlgoGetParamSnapshotExposure == pGetParam->type)
        {
            AECAlgoSnapshotType snapshotType = AECAlgoSnapshotNormal;
            for (UINT32 i = 0; i < pGetParam->inputList.numberOfAECGetInputParams; i++)
            {
                if (AECAlgoGetParamInputSnapshotType == pGetParam->inputList.pAECGetParamList[i].inputType &&
                    NULL != pGetParam->inputList.pAECGetParamList[i].pInputData)
                {
                    snapshotType = *(reinterpret_cast<AECAlgoSnapshotType*>(pGetParam->inputList.pAECGetParamList[i].pInputData));
                }
            }
            if (AECAlgoSnapshotFlash == snapshotType)
            {
                exposureTime    = mainFlashExpTime;
                gain            = mainFlashExpTime;
            }
            else
            {
                exposureTime    = previewExpTime;
                gain            = previewGain;
            }
        }
        else if (AECAlgoGetParamStartExposure == pGetParam->type)
        {
            /* we can always give different start-up exposure during preflash or mode switch etc */
            exposureTime        = startupExpTime;
            gain                = startupGain;
        }
    }

    for (UINT32 i = 0; i < pExposureSet->exposureInfoCount; i++)
    {
        pExposureSet->pExposureInfo[i].exposureData.exposureTime      = exposureTime;
        pExposureSet->pExposureInfo[i].exposureData.linearGain        = gain;
        pExposureSet->pExposureInfo[i].exposureData.sensitivity       = exposureTime * gain;
        pExposureSet->pExposureInfo[i].exposureData.deltaEVFromTarget = 0.0f;
    }

    pExposureSet->pExposureInfo[0].exposureType = AECAlgoExposureShort;
    pExposureSet->pExposureInfo[1].exposureType = AECAlgoExposureLong;
    pExposureSet->pExposureInfo[2].exposureType = AECAlgoExposureSafe;
}

VOID GetUpdatedMetadata(
    AECAlgoInputStatsInfo* statsInfo,
    CHISTATSHANDLE         chiStatsHandle)
{

    // verification of Get/Set Vendor Tag API
    CDKResult              result             = CDKResultSuccess;

#if AF_DEPENDENT
    CHIMETADATAINFO        AFVendortagInfo    = { 0 };
    UINT32                 AFVendorTagList[2] = { g_AFVendorTagBase + AFAlgoTag1 , g_AFVendorTagBase + AFAlgoTag2 };
    UINT32*                pAFAlgoTag1Value   = NULL;
    FLOAT*                 pAFAlgotag2Value   = NULL;


    CHITAGDATA      tagData[2] = { { sizeof(CHITAGDATA), statsInfo->requestId, pAFAlgoTag1Value, 0, 0, FALSE },
                                   { sizeof(CHITAGDATA), statsInfo->requestId, pAFAlgotag2Value, 0, 0, FALSE } };
    AFVendortagInfo.pTagList   = &AFVendorTagList[0];
    AFVendortagInfo.tagNum     = 2;
    AFVendortagInfo.size       = sizeof(CHIMETADATAINFO);
    AFVendortagInfo.pTagData   = &tagData[0];
    AFVendortagInfo.chiSession = chiStatsHandle;

    g_ChiAlgoInterface.pGetMetadata(&AFVendortagInfo);
    for (UINT32 k = 0; k < AFVendortagInfo.tagNum; k++)
    {
        CAMX_LOG_VERBOSE(CamxLogGroupStats,
            "Received vendor tag using GetMetadat API. TagId(%u) pData(%p) size(%u) reqID(%" PRIu64 ")",
            AFVendortagInfo.pTagList[k],
            AFVendortagInfo.pTagData[k].pData,
            AFVendortagInfo.pTagData[k].size,
            AFVendortagInfo.pTagData[k].requestId);
    }
#endif

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
        CAMX_LOG_ERROR(CamxLogGroupStats,
            "Failed to receive static vendor tag Lens Facing.  id(%u)",
            StaticVendortagInfo.pTagList[0]);
    }
    else
    {
        CAMX_LOG_VERBOSE(CamxLogGroupStats,
            "Received static vendor tag Lens Facing.  id(%u) value(%d) DataSize(%d)",
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
    InputVendortagInfo.pTagData->requestId = statsInfo->requestId;
    InputVendortagInfo.pTagList = &InputVendortagList[0];
    InputVendortagInfo.tagNum = 1;
    InputVendortagInfo.metadataType = ChiMetadataDynamic;
    InputVendortagInfo.size = sizeof(CHIMETADATAINFO);
    InputVendortagInfo.chiSession = chiStatsHandle;

    // OEM_AWB_LOG_VERBOSE("Retrieving input tag for requestId %" PRIu64, requestId);
    result = g_ChiAlgoInterface.pGetMetadata(&InputVendortagInfo);

    if ((CDKResultSuccess != result) || (NULL == InputtagData[0].pData))
    {
        CAMX_LOG_ERROR(CamxLogGroupStats,
            "Failed to receive input vendor tag ControlAELock id(%u) ",
            InputVendortagInfo.pTagList[0]);
    }
    else
    {
        CAMX_LOG_VERBOSE(CamxLogGroupStats,
            "Received input vendor tag ControlAELock.  id(%u) value(%d) DataSize(%d)",
            InputVendortagInfo.pTagList[0],
            *static_cast<BYTE*>(InputtagData[0].pData),
            InputtagData[0].dataSize);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AECProcess
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult AECProcess(
    CHIAECAlgorithm*            pAECAlgorithm,
    const AECAlgoInputList*     pInput,
    AECAlgoOutputList*          pOutput)
{
    CAMX_UNREFERENCED_PARAM(pAECAlgorithm);

    AECAlgoFrameInfo*      pFrameInfo      = NULL;
    AECAlgoFrameControl*   pFrameControl   = NULL;
    AECAlgoAPEXData*       pAPEXData       = NULL;
    UINT32*                pLEDCurrents    = NULL;
    AECAlgoExposureSet*    pExposureSet    = NULL;
    AECAlgoInputStatsInfo* statsInfo       = NULL;
    CHISTATSHANDLE         pStatsHandle    = NULL;

    for (UINT32 i = 0; i < pInput->numberOfAECInputs; i++)
    {
        switch (pInput->pAECInputList[i].inputType)
        {
        case AECAlgoInputStatisticInfo:
        {
            statsInfo =
                static_cast<AECAlgoInputStatsInfo*>(pInput->pAECInputList[AECAlgoInputStatisticInfo].pAECInput);
            CAMX_ASSERT(NULL != statsInfo);
            break;
        }
        case AECAlgoInputStatsChiHandle:
        {
            pStatsHandle = static_cast<CHISTATSHANDLE>(pInput->pAECInputList[AECAlgoInputStatsChiHandle].pAECInput);
            CAMX_ASSERT(NULL != pStatsHandle);
            break;
        }
        case AECAlgoInputVendorTag:
            {
                StatsVendorTagList* pVendorTagList = static_cast<StatsVendorTagList*>(pInput->pAECInputList[i].pAECInput);
                for (UINT32 j = 0; j < pVendorTagList->vendorTagCount; j++)
                {
                    CAMX_LOG_VERBOSE(CamxLogGroupStats,
                                    "Received vendor tag id(%u) pData(%p) size(%u)",
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

    for (UINT32 i = 0; i < pOutput->numberOfAECOutputs; i++)
    {
        AECAlgoOutput* pCurrentParam = &pOutput->pAECOutputList[i];

        switch (pCurrentParam->outputType)
        {
            case AECAlgoOutputFrameInfo:
            {
                pFrameInfo                             = reinterpret_cast<AECAlgoFrameInfo *>(pCurrentParam->pAECOutput);
                pCurrentParam->sizeOfWrittenAECOutput  = pCurrentParam->sizeOfAECOutput;
                break;
            }

            case AECAlgoOutputFrameControl:
            {
                pFrameControl                          = reinterpret_cast<AECAlgoFrameControl *>(pCurrentParam->pAECOutput);
                pCurrentParam->sizeOfWrittenAECOutput  = pCurrentParam->sizeOfAECOutput;
                break;
            }

            case AECAlgoOutputEXIFApex:
            {
                pAPEXData                              = reinterpret_cast<AECAlgoAPEXData *>(pCurrentParam->pAECOutput);
                pCurrentParam->sizeOfWrittenAECOutput  = pCurrentParam->sizeOfAECOutput;
                break;
            }

            case AECAlgoOutputLEDCurrents:
            {
                pLEDCurrents                           = reinterpret_cast<UINT32*>(pCurrentParam->pAECOutput);
                pCurrentParam->sizeOfWrittenAECOutput  = pCurrentParam->sizeOfAECOutput;
                break;
            }

            case AECAlgoOutputExposureSets:
            {
                pExposureSet                           = reinterpret_cast<AECAlgoExposureSet*>(pCurrentParam->pAECOutput);
                pCurrentParam->sizeOfWrittenAECOutput  = pCurrentParam->sizeOfAECOutput;
                break;
            }

            case AECAlgoOutputVendorTag:
            {
                PopulateVendorTagOutputs(static_cast<StatsVendorTagList*>(pCurrentParam->pAECOutput));
                pCurrentParam->sizeOfWrittenAECOutput = sizeof(StatsVendorTagList);
                break;
            }

            default:
                CAMX_LOG_INFO(CamxLogGroupStats,
                               "Input param %d not implemented/not filled-in", pCurrentParam->outputType);
                break;
        }
    }

    if (NULL != pFrameInfo)
    {
        CamX::Utils::Memset(&pFrameInfo->legacyYStats, 0, sizeof(pFrameInfo->legacyYStats));
        pFrameInfo->snapshotIndicator       = AECAlgoSnapshotNormal;
        pFrameInfo->asdExtremeBlueRatio     = 1.00f;
        pFrameInfo->asdExtremeGreenRatio    = 1.00f;
        pFrameInfo->brightnessSettled       = TRUE;
        pFrameInfo->LEDAFRequired           = bAlgoDecisionForFlashAFRequired;
        pFrameInfo->touchEVIndicator        = AECAlgoTouchEVInactive;

        if (currentFrameCount > numberOfFramesToConverge)
        {
            pFrameInfo->aecSettled = TRUE;
            /* Here if algo decides flash is required for this snapshot capture, we need to set this enum to flash or normal */
            if (TRUE == bAlgoDecisionForFlashRequired)
            {
                pFrameInfo->snapshotIndicator = AECAlgoSnapshotFlash;
            }
            else
            {
                pFrameInfo->snapshotIndicator = AECAlgoSnapshotNormal;
            }
        }
        else
        {
            pFrameInfo->aecSettled          = FALSE;
        }

        if (NULL != pFrameControl)
        {
            pFrameControl->flashState        = AECAlgoFlashStateOff;
            pFrameControl->LEDInfluenceRatio = 1.00f;
            pFrameControl->LEDRGRatio        = 1.00f;
            pFrameControl->LEDBGRatio        = 1.00f;
            pFrameControl->luxIndex          = 100.0f;
            pFrameControl->ISOValue          = 100;
        }

        if (NULL != pAPEXData)
        {
            pAPEXData->apertureValue   = 1.0f;
            pAPEXData->brightnessValue = 1.0f;
            pAPEXData->exposureValue   = 100.0f;
            pAPEXData->speedValue      = 1.0f;
            pAPEXData->timeValue       = 10.0f;
        }

        if (NULL != pLEDCurrents)
        {
            /* if we are in preflash mode - give preflash currents & flash state should be of type Pre */
            if (AECAlgoOperationModePreflash == currentOperationMode)
            {
                /* In PreFlash mode, if we converged good, we can produce the main flash currents here */
                if (FALSE == pFrameInfo->aecSettled)
                {
                    pLEDCurrents[0]                 = preFlashLEDCurrents[0];
                    pLEDCurrents[1]                 = preFlashLEDCurrents[1];
                    if (NULL != pFrameControl)
                    {
                        pFrameControl->flashState       = AECAlgoFlashStatePre;
                    }
                    if (NULL != pFrameInfo)
                    {
                        pFrameInfo->snapshotIndicator   = AECAlgoSnapshotFlash;
                    }
                }
                else
                {
                    pLEDCurrents[0]                 = mainFlashLEDCurrents[0];
                    pLEDCurrents[1]                 = mainFlashLEDCurrents[1];
                    if (NULL != pFrameControl)
                    {
                        pFrameControl->flashState       = AECAlgoFlashStateMain;
                    }
                    if (NULL != pFrameInfo)
                    {
                        pFrameInfo->snapshotIndicator   = AECAlgoSnapshotFlash;
                    }
                }
            }
            else /* No Preflash - Just normal aec mode */
            {
                pLEDCurrents[0]                 = 0;
                pLEDCurrents[1]                 = 0;
                if (NULL != pFrameControl)
                {
                    pFrameControl->flashState       = AECAlgoFlashStateOff;
                }
                if (NULL != pFrameInfo)
                {
                    pFrameInfo->snapshotIndicator   = AECAlgoSnapshotNormal;
                }
            }
        }
    }

    if (NULL != pExposureSet)
    {
        HardcodedAECAlgoExposureSet(pExposureSet, NULL);
    }
    currentFrameCount++;

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HandleGetParamForFrameControl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static CDKResult HandleGetParamForFrameControl(
    AECAlgoGetParam* pGetParam)
{
    AECAlgoFrameControl*    pFrameControl      = NULL;
    AECAlgoAPEXData*        pAPEXData          = NULL;
    UINT32*                 pLEDCurrents       = NULL;
    AECAlgoExposureSet*     pExposureSet       = NULL;

    for (UINT32 i = 0; i < pGetParam->outputList.numberOfAECOutputs; i++)
    {
        AECAlgoGetParamOutput* pCurrentParam = &pGetParam->outputList.pAECOutputList[i];
        switch (pCurrentParam->outputType)
        {
            case AECAlgoGetParamOutputFrameControl:
            {
                pFrameControl                           = reinterpret_cast<AECAlgoFrameControl *>(pCurrentParam->pOutputData);
                pCurrentParam->sizeOfWrittenOutputData  = pCurrentParam->sizeOfOutputData;
                break;
            }

            case AECAlgoGetParamOutputEXIFApex:
            {
                pAPEXData                               = reinterpret_cast<AECAlgoAPEXData *>(pCurrentParam->pOutputData);
                pCurrentParam->sizeOfWrittenOutputData  = pCurrentParam->sizeOfOutputData;
                break;
            }

            case AECAlgoGetParamOutputLEDCurrents:
            {
                pLEDCurrents                            = reinterpret_cast<UINT32*>(pCurrentParam->pOutputData);
                pCurrentParam->sizeOfWrittenOutputData  = pCurrentParam->sizeOfOutputData;
                break;
            }
            case AECAlgoGetParamOutputExposureSets:
            {
                pExposureSet                            = reinterpret_cast<AECAlgoExposureSet*>(pCurrentParam->pOutputData);
                pCurrentParam->sizeOfWrittenOutputData  = pCurrentParam->sizeOfOutputData;
                break;
            }
            case AECAlgoGetParamOutputVendorTagInfoList:
            {
                StatsVendorTagInfoList* pVendorTagInfoList =
                    static_cast<StatsVendorTagInfoList*>(pCurrentParam->pOutputData);

                pVendorTagInfoList->vendorTag[0].vendorTagId = g_AECVendorTagBase + AECAlgoTag1;
                pVendorTagInfoList->vendorTag[0].appliedDelay = 0;
                pVendorTagInfoList->vendorTag[1].vendorTagId = g_AECVendorTagBase + AECAlgoTag2;
                pVendorTagInfoList->vendorTag[1].appliedDelay = 0;
                pVendorTagInfoList->vendorTagCount = g_VendorTagSectionDataAEC[0].numTags;
                pCurrentParam->sizeOfWrittenOutputData = sizeof(StatsVendorTagInfoList);
                break;
            }
            case AECGetParamOutputTypeDependentVendorTags:
            {
                StatsVendorTagInfoList* pVendorTagInfoList =
                    static_cast<StatsVendorTagInfoList*>(pCurrentParam->pOutputData);
#if AF_DEPENDENT
                pVendorTagInfoList->vendorTag[0].vendorTagId = g_AFVendorTagBase + AFAlgoTag1;
                pVendorTagInfoList->vendorTag[0].appliedDelay = 0;
                pVendorTagInfoList->vendorTag[1].vendorTagId = g_AFVendorTagBase + AFAlgoTag2;
                pVendorTagInfoList->vendorTag[1].appliedDelay = 0;
                pVendorTagInfoList->vendorTagCount = 2;
#else //AF_DEPENDENT
                pVendorTagInfoList->vendorTagCount = 0;
#endif //AF_DEPENDENT

                pCurrentParam->sizeOfWrittenOutputData = sizeof(StatsVendorTagInfoList);
                break;
            }
            default:
                break;
        }
    }

    if (NULL != pFrameControl)
    {
        /* this indicates stats node wants snapshot exposure details */
        if (AECAlgoGetParamSnapshotExposure == pGetParam->type)
        {
            AECAlgoSnapshotType snapshotType = AECAlgoSnapshotNormal;
            for (UINT32 i = 0; i < pGetParam->inputList.numberOfAECGetInputParams; i++)
            {
                if (AECAlgoGetParamInputSnapshotType == pGetParam->inputList.pAECGetParamList[i].inputType &&
                    NULL != pGetParam->inputList.pAECGetParamList[i].pInputData)
                {
                    snapshotType = *(reinterpret_cast<AECAlgoSnapshotType*>(pGetParam->inputList.pAECGetParamList[i].pInputData));
                }
            }
            if (AECAlgoSnapshotFlash == snapshotType)
            {
                pFrameControl->flashState   = AECAlgoFlashStateMain;
                if (NULL != pLEDCurrents)
                {
                    pLEDCurrents[0]             = mainFlashLEDCurrents[0];
                    pLEDCurrents[1]             = mainFlashLEDCurrents[1];
                }
            }
            else
            {
                pFrameControl->flashState   = AECAlgoFlashStateOff;
                if (NULL != pLEDCurrents)
                {
                    pLEDCurrents[0]             = 0;
                    pLEDCurrents[1]             = 0;
                }
            }
        }
        pFrameControl->LEDInfluenceRatio = 1.00f;
        pFrameControl->LEDRGRatio        = 1.00f;
        pFrameControl->LEDBGRatio        = 1.00f;
        pFrameControl->luxIndex          = 100.0f;
        pFrameControl->ISOValue          = 100;
    }

    if (NULL != pAPEXData)
    {
        pAPEXData->apertureValue   = 1.0f;
        pAPEXData->brightnessValue = 1.0f;
        pAPEXData->exposureValue   = 100.0f;
        pAPEXData->speedValue      = 1.0f;
        pAPEXData->timeValue       = 10.0f;
    }

    if (NULL != pExposureSet && NULL != pLEDCurrents)
    {
        HardcodedAECAlgoExposureSet(pExposureSet, pGetParam);
    }

    return CDKResultSuccess;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AECGetParam
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult AECGetParam(
    CHIAECAlgorithm*   pAECAlgorithm,
    AECAlgoGetParam*   pGetParam)
{
    CDKResult         result  = CDKResultSuccess;

    if (NULL == pGetParam || NULL == pAECAlgorithm)
    {
        CAMX_LOG_ERROR(CamxLogGroupStats, "Invalid parameter");
        result = CDKResultEInvalidPointer;
    }

    if (CDKResultSuccess == result)
    {
        CAMX_LOG_VERBOSE(CamxLogGroupStats, "GetParam Type %d", pGetParam->type);
        switch (pGetParam->type)
        {
            case AECAlgoGetParamStartExposure:
            {
                HandleGetParamForFrameControl(pGetParam);
                break;
            }
            case AECAlgoGetParamSnapshotExposure:
            {
                HandleGetParamForFrameControl(pGetParam);
                break;
            }
            case AECAlgoGetParamPublishingVendorTagsInfo:
            {
                HandleGetParamForFrameControl(pGetParam);
                break;
            }
            case AECAlgoGetParamDependentVendorTags:
            {
                HandleGetParamForFrameControl(pGetParam);
                break;
            }
            case AECAlgoGetParamGainFromISOPreview:
            {
                break;
            }
            default:
                result = CDKResultEUnsupported;
                break;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AECSetParam
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult AECSetParam(
    CHIAECAlgorithm*            pAECAlgorithm,
    const AECAlgoSetParamList*  pSetParam)
{
    CAMX_ENTRYEXIT(CamxLogGroupStats);
    CDKResult  result  = CDKResultSuccess;

    if ((NULL == pSetParam) || (NULL == pAECAlgorithm))
    {
        CAMX_LOG_ERROR(CamxLogGroupStats, "Invalid parameter");
        result = CDKResultEInvalidPointer;
    }

    if (CDKResultSuccess == result)
    {
        for (UINT32 i = 0; i < pSetParam->numberOfAECSetParams; i++)
        {
            AECAlgoSetParam* pCurrentParam = &pSetParam->pAECSetParamList[i];
            switch (pCurrentParam->setParamType)
            {
                case AECAlgoSetParamOperationMode:
                {
                    AECAlgoOperationModeType mode =
                        *reinterpret_cast<AECAlgoOperationModeType *>(pCurrentParam->pAECSetParam);
                    CAMX_LOG_VERBOSE(CamxLogGroupStats, "Set Operation Mode %d", mode);

                    currentOperationMode = mode;
                    currentFrameCount    = 0;
                    break;
                }
                case AECAlgoSetParamChromatixData:
                {
                    CAMX_LOG_VERBOSE(CamxLogGroupStats, "Set Configuration %p", pCurrentParam->pAECSetParam);
                    break;
                }
                case AECAlgoSetParamMeteringMode:
                {
                    CAMX_NOT_IMPLEMENTED();
                    break;
                }
                default:
                    break;
            }
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// StaticAECGetFeatureCapability
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult AECGetFeatureCapability(
    CHIAECAlgorithm*    pAECAlgorithm,
    UINT64*             pFeatures)
{
    CAMX_UNREFERENCED_PARAM(pAECAlgorithm);
    CAMX_UNREFERENCED_PARAM(pFeatures);

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// StaticAECDestroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC VOID AECDestroy(
    CHIAECAlgorithm*                 pAECAlgorithm,
    const AECAlgoDestroyParamList*   pDestroyParams)
{
    CAMX_UNREFERENCED_PARAM(pAECAlgorithm);

    for (UINT32 j = 0; j < pDestroyParams->paramCount; j++)
    {
        AECAlgoDestroyParam*   pParamList = &pDestroyParams->pParamList[j];
        switch (pParamList->destroyParamType)
        {
            case AECDestroyParamTypeCameraCloseIndicator:
            {
                UINT overrideCameraClose = *static_cast<UINT*>(pParamList->pParam);
                if (1 == overrideCameraClose)
                {
                    if (NULL != pAEC)
                    {
                        CAMX_LOG_ERROR(CamxLogGroupStats, "StaticAEC pAEC in destroy is %p", pAEC);
                        CAMX_DELETE(pAEC);
                        pAEC = NULL;
                    }
                }
                else
                {
                    CAMX_LOG_ERROR(CamxLogGroupStats, "overrideCameraClose in  AECDestroy %d", overrideCameraClose);
                }
                break;
            }
            default:
                break;
        }
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateStaticAECInterface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult CreateAEC(
    const AECAlgoCreateParamList*      pCreateParam,
    CHIAECAlgorithm**                  ppAECAlgorithm)
{
    CAMX_LOG_VERBOSE(CamxLogGroupStats, "CreateAEC static");
    CDKResult result = CDKResultSuccess;

    CHISTATSHANDLE  chiSessionHandle = NULL;
    UINT* pOverrideCameraOpen        = NULL;

    for (UINT32 j = 0; j < pCreateParam->numberOfCreateParams; j++)
    {
        AECAlgoCreateParam*   pParamList = &pCreateParam->pCreateParamList[j];

        switch (pParamList->createParamType)
        {
            case AECAlgoCreateParamTypeSessionHandle:
            {
                CAMX_LOG_INFO(CamxLogGroupStats, "StaticAEC algo sizeOfParam for CHISTATSHANDLE is %d", pParamList->sizeOfCreateParam);
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

                    CAMX_LOG_INFO(CamxLogGroupStats, "StaticAEC algo create: chiSessionHandle is %p", chiSessionHandle);
                }
                break;
            }
            case AECAlgoCreateParamTypeCameraOpenIndicator:
            {
                if ((pParamList->pCreateParam != NULL) && (pParamList->sizeOfCreateParam >= sizeof(UINT)))
                {
                    pOverrideCameraOpen = static_cast<UINT*>(pParamList->pCreateParam);
                    CAMX_LOG_INFO(CamxLogGroupStats, "StaticAEC algo create: overrideCameraOpen is %d", *pOverrideCameraOpen);
                }
                break;
            }

            default:
                break;
        }
    }

    if ((NULL != pOverrideCameraOpen) && (1 == (*pOverrideCameraOpen)))
    {
        if (NULL == pAEC)
        {
            pAEC = reinterpret_cast<StaticInterface *>(CAMX_CALLOC(sizeof(StaticInterface)));
            if (NULL == pAEC)
            {
                result = CDKResultENoMemory;
            }
            else
            {
                pAEC->interface.AECDestroy              = AECDestroy;
                pAEC->interface.AECProcess              = AECProcess;
                pAEC->interface.AECGetParam             = AECGetParam;
                pAEC->interface.AECSetParam             = AECSetParam;
                pAEC->interface.AECGetFeatureCapability = AECGetFeatureCapability;
                *ppAECAlgorithm                         = &pAEC->interface;
                CAMX_LOG_ERROR(CamxLogGroupStats, "pAEC first time %p", pAEC);
            }
        }
        else
        {
            *ppAECAlgorithm = &pAEC->interface;
            CAMX_LOG_ERROR(CamxLogGroupStats, "pAEC after %p", pAEC);
        }
    }
    else
    {
        CAMX_LOG_ERROR(CamxLogGroupStats, "AEC:Not actual open ...ignore:pOverrideCameraOpen %p", pOverrideCameraOpen);
    }

    /* Just initalizing some default values -> these currents needs to be computed during preflash */
    preFlashLEDCurrents[0]  = 86;
    preFlashLEDCurrents[1]  = 34;
    mainFlashLEDCurrents[0] = 652;
    mainFlashLEDCurrents[1] = 248;


    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AECGetCapabilities
///
/// @brief  This method creates an instance to the AFAlgorithm.
///
/// @param  pCapsInfo Pointer to Algo capability structure
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDKResult AECGetCapabilities(CHIALGORITHMCAPSINFO* pCapsInfo)
{
    CDKResult   result = CDKResultSuccess;

    CAMX_LOG_VERBOSE(CamxLogGroupStats,
                    "Get supported Capabilities size:%d, algo mask: %d",
                     pCapsInfo->size,
                     pCapsInfo->algorithmCapsMask);
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AECNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AECNodeQueryVendorTag(
    CHIQUERYVENDORTAG* pQueryVendorTag)
{
    CDKResult result = CDKResultSuccess;
    if (NULL == pQueryVendorTag)
    {
        result = CDKResultEInvalidPointer;
        CAMX_LOG_ERROR(CamxLogGroupChi, "Invalid argument: pQueryVendorTag is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (pQueryVendorTag->size >= sizeof(CHIQUERYVENDORTAG))
        {
            pQueryVendorTag->pVendorTagInfo = g_VendorTagInfoAEC;
        }
        else
        {
            CAMX_LOG_ERROR(CamxLogGroupChi, "pQueryVendorTag is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AECAlgoSetNodeInterface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID AECAlgoSetNodeInterface(
    ChiAlgorithmInterface* pAlgoInterface)
{
    CAMX_ASSERT((NULL != pAlgoInterface) &&
        (NULL != pAlgoInterface->pGetVendorTagBase) &&
        (NULL != pAlgoInterface->pGetMetadata) &&
        (NULL != pAlgoInterface->pSetMetaData));

    int result;
    g_ChiAlgoInterface = *pAlgoInterface;

    // get the AEC vendor tag base
    CHIVENDORTAGBASEINFO vendorTagBase = { 0 };
    vendorTagBase.size = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName = AECAlgoSectionName;

    result = g_ChiAlgoInterface.pGetVendorTagBase(&vendorTagBase);
    if (CDKResultSuccess == result)
    {
        g_AECVendorTagBase = vendorTagBase.vendorTagBase;
    }
#if AF_DEPENDENT
    // get the AF vendor tag base
    vendorTagBase.pComponentName = AFAlgoSectionName;

    result = g_ChiAlgoInterface.pGetVendorTagBase(&vendorTagBase);
    if (CDKResultSuccess == result)
    {
        g_AFVendorTagBase = vendorTagBase.vendorTagBase;
    }
#endif //AF_DEPENDENT
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiAECAlgorithmEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom AEC Algorithm.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC void ChiAECAlgorithmEntry(CHIAECALGORITHMCALLBACKS* pCallBacks)
{
    CAMX_LOG_VERBOSE(CamxLogGroupStats, "ChiAECAlgorithmEntry static");

    pCallBacks->size                = sizeof(CHIAECALGORITHMCALLBACKS);
    pCallBacks->pfnGetCapabilities  = AECGetCapabilities;
    pCallBacks->pfnQueryVendorTag   = AECNodeQueryVendorTag;
    pCallBacks->pfnCreate           = CreateAEC;
    pCallBacks->pfnSetAlgoInterface = AECAlgoSetNodeInterface;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// END Static AEC algorithm implementation
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif // __cplusplus

CAMX_NAMESPACE_END
