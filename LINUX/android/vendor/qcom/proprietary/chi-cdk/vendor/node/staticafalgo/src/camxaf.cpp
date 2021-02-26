////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxaf.cpp
/// @brief This class handle creation  of AF algorithm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include "camxcafalgorithmhandler.h"
#include "camxdefs.h"
#include "chistatsalgo.h"
#include "camxdebug.h"
#include "camxdebugprint.h"
#include "camxmem.h"
#include "camxaf.h"
#include "chicommon.h"
#include <system/camera_metadata.h>

#define AWB_DEPENDENT 0
static UINT32         g_vendorTagBase     = 0;                           ///< Chi assigned runtime vendor tag base for the node
static const CHAR     AFAlgoSectionName[] = "com.qtistatic.stats.af";    ///< The section name for node
static const UINT32   AFAlgoBaseTagBase   = 0;                           ///< Tag base
static const UINT32   AFAlgoTag1          = AFAlgoBaseTagBase + 0;       ///< Tag 1
static const UINT32   AFAlgoTag2          = AFAlgoBaseTagBase + 1;      ///< Tag 2
static const FLOAT    AFAlgoTag1Value     = 13.43f;
static const INT32    AFAlgoTag2Value     = 20;
CHIALGORITHMINTERFACE g_ChiAFAlgoInterface;    ///< The instance save the CAMX Chi interface

CamX::AFInternalDataType*  pAFInternalData = NULL;

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagDataAF[] =
{
    { "AFAlgoTag1", TYPE_DOUBLE,    1 },
    { "AFAlgoTag2", TYPE_INT32,     1 },
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagSectionDataAF[] =
{
    {
        AFAlgoSectionName,  0,
        sizeof(g_VendorTagDataAF) / sizeof(g_VendorTagDataAF[0]), g_VendorTagDataAF,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoAF[] =
{
    {
        &g_VendorTagSectionDataAF[0],
        sizeof(g_VendorTagSectionDataAF) / sizeof(g_VendorTagSectionDataAF[0])
    }
};

#if (AWB_DEPENDENT)
// Dependent Vendor Tags Info
static UINT32          g_AWBVendorTagBase = 0;                     ///< Chi assigned runtime vendor tag base for AF
static const CHAR      AWBAlgoSectionName[] = "com.qtistatic.stats.awb";    ///< The section name for AF

static const UINT32   AWBAlgoTag1 = 0;   ///< AF Tag 1
static const UINT32   AWBAlgoTag2 = 1;   ///< AF Tag 2
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PopulateVendorTagOutputs()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void PopulateVendorTagOutputs(StatsVendorTagList* pOutputVendorTags)
{
    CAMX_ASSERT(NULL != pOutputVendorTags);
    UINT32 tagNum = 2;

    CAMX_ASSERT((pOutputVendorTags->vendorTag[0].dataSize >= sizeof(AFAlgoTag1Value)) &&
        (NULL != pOutputVendorTags->vendorTag[0].pData));

    pOutputVendorTags->vendorTag[0].vendorTagId                 = g_vendorTagBase + AFAlgoTag1;
    *static_cast<FLOAT*>(pOutputVendorTags->vendorTag[0].pData) = AFAlgoTag1Value;
    pOutputVendorTags->vendorTag[0].appliedDelay                = 0;
    pOutputVendorTags->vendorTag[0].sizeOfWrittenData           = sizeof(AFAlgoTag1Value);

    CAMX_ASSERT((pOutputVendorTags->vendorTag[1].dataSize >= sizeof(AFAlgoTag2Value)) &&
        (NULL != pOutputVendorTags->vendorTag[1].pData));

    pOutputVendorTags->vendorTag[1].vendorTagId                 = g_vendorTagBase + AFAlgoTag2;
    *static_cast<FLOAT*>(pOutputVendorTags->vendorTag[1].pData) = AFAlgoTag2Value;
    pOutputVendorTags->vendorTag[1].appliedDelay                = 0;
    pOutputVendorTags->vendorTag[1].sizeOfWrittenData           = sizeof(AFAlgoTag2Value);

    for (UINT32 j = 0; j < tagNum; j++)
    {
        CAMX_LOG_VERBOSE(CamxLogGroupStats,
                         "Publish vendor tag using GetMetadata API. TagId(%u) pData(%p)",
                         pOutputVendorTags->vendorTag[0].vendorTagId,
                         pOutputVendorTags->vendorTag[1].pData);
    }
}

CAMX_NAMESPACE_BEGIN



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OverWriteROIConfigOutput
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID OverWriteROIConfigOutput(
    AFBAFFloatingWindowROIConfig* pBAFROIConfig)
{
    pBAFROIConfig->BAFROIType                            = AFBAFFloatingWindowROITypeDefault;
    pBAFROIConfig->isValid                               = 1;
    pBAFROIConfig->numberOfROI                           = 25;

    pBAFROIConfig->pBAFFwROIDimension[0].isValid         = 1;
    pBAFROIConfig->pBAFFwROIDimension[0].regionType      = AFBAFFloatingWindowTypePrimary;
    pBAFROIConfig->pBAFFwROIDimension[5].regionNum       = 0;
    pBAFROIConfig->pBAFFwROIDimension[0].ROI.left        = 192;
    pBAFROIConfig->pBAFFwROIDimension[0].ROI.top         = 256;
    pBAFROIConfig->pBAFFwROIDimension[0].ROI.width       = 18;
    pBAFROIConfig->pBAFFwROIDimension[0].ROI.height      = 24;

    // ROI 5
    pBAFROIConfig->pBAFFwROIDimension[5].isValid         = 1;
    pBAFROIConfig->pBAFFwROIDimension[5].regionType      = AFBAFFloatingWindowTypePrimary;
    pBAFROIConfig->pBAFFwROIDimension[5].regionNum       = 5;
    pBAFROIConfig->pBAFFwROIDimension[5].ROI.left        = 192;
    pBAFROIConfig->pBAFFwROIDimension[5].ROI.top         = 281;
    pBAFROIConfig->pBAFFwROIDimension[5].ROI.width       = 18;
    pBAFROIConfig->pBAFFwROIDimension[5].ROI.height      = 24;

    // ROI 10
    pBAFROIConfig->pBAFFwROIDimension[10].isValid        = 1;
    pBAFROIConfig->pBAFFwROIDimension[10].regionType     = AFBAFFloatingWindowTypePrimary;
    pBAFROIConfig->pBAFFwROIDimension[10].regionNum      = 10;
    pBAFROIConfig->pBAFFwROIDimension[10].ROI.left       = 192;
    pBAFROIConfig->pBAFFwROIDimension[10].ROI.top        = 306;
    pBAFROIConfig->pBAFFwROIDimension[10].ROI.width      = 18;
    pBAFROIConfig->pBAFFwROIDimension[10].ROI.height     = 24;

    // ROI 15
    pBAFROIConfig->pBAFFwROIDimension[15].isValid        = 1;
    pBAFROIConfig->pBAFFwROIDimension[15].regionType     = AFBAFFloatingWindowTypePrimary;
    pBAFROIConfig->pBAFFwROIDimension[15].regionNum      = 15;
    pBAFROIConfig->pBAFFwROIDimension[15].ROI.left       = 192;
    pBAFROIConfig->pBAFFwROIDimension[15].ROI.top        = 331;
    pBAFROIConfig->pBAFFwROIDimension[15].ROI.width      = 18;
    pBAFROIConfig->pBAFFwROIDimension[15].ROI.height     = 24;

    // ROI 20
    pBAFROIConfig->pBAFFwROIDimension[20].isValid        = 1;
    pBAFROIConfig->pBAFFwROIDimension[20].regionType     = AFBAFFloatingWindowTypePrimary;
    pBAFROIConfig->pBAFFwROIDimension[20].regionNum      = 20;
    pBAFROIConfig->pBAFFwROIDimension[20].ROI.left       = 192;
    pBAFROIConfig->pBAFFwROIDimension[20].ROI.top        = 356;
    pBAFROIConfig->pBAFFwROIDimension[20].ROI.width      = 18;
    pBAFROIConfig->pBAFFwROIDimension[20].ROI.height     = 24;

    // ROI 24
    pBAFROIConfig->pBAFFwROIDimension[24].isValid        = 1;
    pBAFROIConfig->pBAFFwROIDimension[24].regionType     = AFBAFFloatingWindowTypePrimary;
    pBAFROIConfig->pBAFFwROIDimension[24].regionNum      = 24;
    pBAFROIConfig->pBAFFwROIDimension[24].ROI.left       = 268;
    pBAFROIConfig->pBAFFwROIDimension[24].ROI.top        = 356;
    pBAFROIConfig->pBAFFwROIDimension[24].ROI.width      = 18;
    pBAFROIConfig->pBAFFwROIDimension[24].ROI.height     = 24;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OverWriteBFConfigOutput()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID OverWriteBFConfigOutput(
    AFBAFFloatingWindowConfig* pBAFConfig)
{
    // 1. H1 config
    pBAFConfig->pBAFFilterConfigH1->horizontalScaleEnable    = 0;
    pBAFConfig->pBAFFilterConfigH1->isValid                  = 1;
    pBAFConfig->pBAFFilterConfigH1->shiftBits                = 3;

    // 1a. H1 Coring Config
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->numberOfCoringFilter = 17;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[0]     = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[1]     = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[2]     = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[3]     = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[4]     = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[5]     = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[6]     = 1;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[7]     = 1;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[8]     = 2;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[9]     = 3;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[10]    = 4;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[11]    = 5;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[12]    = 7;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[13]    = 9;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[14]    = 11;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[15]    = 13;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->pCoringFilter[16]    = 16;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->gain                 = 16;
    pBAFConfig->pBAFFilterConfigH1->pBAFFilterCoringConfig->threshold            = 200;

    // 1b. H1 FIR Config
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->enable                      = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->numOfFIRCoefficients        = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[0]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[1]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[2]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[3]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[4]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[5]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[6]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[7]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[8]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[9]   = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[10]  = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[11]  = 0;
    pBAFConfig->pBAFFilterConfigH1->pBAFFIRFilterConfig->pFIRFilterCoefficients[12]  = 0;

    // 1c. H1 IIR Config
    pBAFConfig->pBAFFilterConfigH1->pBAFIIRFilterConfig->enable = 1;

    // 4. Luma Conversion config
    pBAFConfig->pBAFInputConfig->BAFChannelSel   = BAFChannelSelectTypeG;
    pBAFConfig->pBAFInputConfig->BAFInputGSel    = BAFInputSelectGr;
    pBAFConfig->pBAFInputConfig->isValid         = 1;
    pBAFConfig->pBAFInputConfig->numberOfYConfig = 3;
    pBAFConfig->pBAFInputConfig->pYConfig[0]     = 0.000000f;
    pBAFConfig->pBAFInputConfig->pYConfig[1]     = 0.000000f;
    pBAFConfig->pBAFInputConfig->pYConfig[2]     = 0.000000f;

    // 5. Down scaler config
    pBAFConfig->pBAFScaleConfig->BAFScaleEnable  = 0;
    pBAFConfig->pBAFScaleConfig->scaleM          = 1;
    pBAFConfig->pBAFScaleConfig->scaleN          = 1;

    // 6. Gamma Config
    pBAFConfig->pBAFGammaLUTConfig->numGammaLUT  = 32;
    pBAFConfig->pBAFGammaLUTConfig->pGammaLUT[0] = 0;
    pBAFConfig->pBAFGammaLUTConfig->pGammaLUT[1] = 3390;
    pBAFConfig->pBAFGammaLUTConfig->pGammaLUT[2] = 4646;
    pBAFConfig->pBAFGammaLUTConfig->pGammaLUT[3] = 5586;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// GetupdatedMetaData ()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GetupdatedMetaData(
    UINT64          requestId,
    CHISTATSHANDLE  chiStatsHandle)
{
    CDKResult result = CDKResultSuccess;
#if AWB_DEPENDENT
    // verification of Get/Set Vendor Tag API
    CHIMETADATAI
        NFO AWBVendortagInfo = { 0 };
    UINT32          AWBVendorTagList[2] = { g_AWBVendorTagBase + AWBAlgoTag1 , g_AWBVendorTagBase + AWBAlgoTag2 };

    CHITAGDATA      tagData[2] = { { sizeof(CHITAGDATA), requestId, NULL, 0, 0, FALSE },
                                   { sizeof(CHITAGDATA), requestId, NULL, 0, 0, FALSE } };

    AWBVendortagInfo.pTagList    = &AWBVendorTagList[0];
    AWBVendortagInfo.tagNum      = 2;
    AWBVendortagInfo.size        = sizeof(CHIMETADATAINFO);
    AWBVendortagInfo.pTagData    = &tagData[0];
    AWBVendortagInfo.metadataType = ChiMetadataDynamic;


    g_ChiAFAlgoInterface.pGetMetadata(&AWBVendortagInfo);
    for (UINT32 j = 0; j < AWBVendortagInfo.tagNum; j++)
    {
        CAMX_LOG_VERBOSE(CamxLogGroupStats,
                        "Received vendor tag using GetMetadat API. TagId(%u) pData(%p) size(%u) reqID(%" PRIu64 ")",
                         AWBVendortagInfo.pTagList[j],
                         AWBVendortagInfo.pTagData[j].pData,
                         AWBVendortagInfo.pTagData[j].size,
                         AWBVendortagInfo.pTagData[j].requestId);
    }
#endif

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
    StaticVendortagInfo.chiSession = chiStatsHandle;

    result = g_ChiAFAlgoInterface.pGetMetadata(&StaticVendortagInfo);
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

    // Read input vendor tag
    //UINT32          ControlCaptureIntent    = 65549;
    UINT32          ControlAELock = ANDROID_CONTROL_AE_LOCK | InputMetadataSectionMask;
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
    result = g_ChiAFAlgoInterface.pGetMetadata(&InputVendortagInfo);

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// AFProcess()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult AFProcess(
    CHIAFAlgorithm*         pAFAlgo,
    const AFAlgoInputList*  pInput,
    AFAlgoOutputList*       pOutput)
{
    CamxResult  result = CamxResultSuccess;
    CAMX_UNREFERENCED_PARAM(pAFAlgo);
    CAMX_UNREFERENCED_PARAM(pInput);

    UINT64 requestId = 0L;
    CHISTATSHANDLE  chiStatsHandle = NULL;

    CAMX_ASSERT_MESSAGE(NULL != pOutput, "Output NULL pointer");

    for (UINT32 i = 0; i < pInput->numberOfInputParam; i++)
    {
        switch (pInput->pAFInputs[i].inputType)
        {
            case AFInputTypeVendorTag:
            {
                StatsVendorTagList* pVendorTagList = static_cast<StatsVendorTagList*>(pInput->pAFInputs[i].pAFInput);
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
            case AFInputTypeRequestId:
            {
                requestId = *(static_cast<UINT64*>(pInput->pAFInputs[i].pAFInput));
                CAMX_LOG_VERBOSE(CamxLogGroupStats,
                                "Request ID %llu",
                                requestId);

                break;
            }

            case AFInputTypeStatsChiHandle:
            {
                chiStatsHandle = static_cast<CHISTATSHANDLE>(pInput->pAFInputs[i].pAFInput);
                break;
            }
        default:
            break;
        }
    }

    GetupdatedMetaData(requestId, chiStatsHandle);

    AFAlgoFocusValue* pAFAlgoFocusValue =
        static_cast<AFAlgoFocusValue*>(pOutput->pAFOutputs[AFOutputTypeFocusValue].pAFOutput);
    CAMX_ASSERT_MESSAGE(NULL != pAFAlgoFocusValue, "Focus Value NULL pointer");
    pAFAlgoFocusValue->focusValue       = static_cast<FLOAT>(13369.0371f);

    AFLensPositionInfo* pAFLensPositionInfo =
        static_cast<AFLensPositionInfo*>(pOutput->pAFOutputs[AFOutputTypeMoveLensParam].pAFOutput);
    CAMX_ASSERT_MESSAGE(NULL != pAFLensPositionInfo, "Lens position information NULL pointer");
    pAFLensPositionInfo->moveType                                       = AFLensMoveTypeLogical;
    pAFLensPositionInfo->logicalLensMoveInfo.lensPositionInLogicalUnits = 370;

    AFAlgoStatusParam* pAFAlgoStatusParam =
        static_cast<AFAlgoStatusParam*>(pOutput->pAFOutputs[AFOutputTypeStatusParam].pAFOutput);
    CAMX_ASSERT_MESSAGE(NULL != pAFAlgoStatusParam, "AF status NULL pointer");
    pAFAlgoStatusParam->focusDone   = TRUE;
    pAFAlgoStatusParam->status      = AFAlgoStatusTypeFocused;

    AFBAFFloatingWindowROIConfig* pBAFROIConfig =
        static_cast<AFBAFFloatingWindowROIConfig*>(pOutput->pAFOutputs[AFOutputTypeBAFFloatingWindowROIConfig].pAFOutput);

    OverWriteROIConfigOutput(pBAFROIConfig);

    AFBAFFloatingWindowConfig* pBAFConfig =
        static_cast<AFBAFFloatingWindowConfig*>(pOutput->pAFOutputs[AFOutputTypeBAFFloatingWindowConfig].pAFOutput);

    OverWriteBFConfigOutput(pBAFConfig);

    PopulateVendorTagOutputs(static_cast<StatsVendorTagList*>(pOutput->pAFOutputs[AFOutputPublishingVendorTagsInfo].pAFOutput));
    pOutput->pAFOutputs[AFOutputPublishingVendorTagsInfo].sizeOfWrittenAFOutput = sizeof(StatsVendorTagList);


    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AFSetParam()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult AFSetParam(
    CHIAFAlgorithm*             pAFAlgo,
    const AFAlgoSetParamList*   pSetParams)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pSetParams || NULL == pAFAlgo)
    {
        result = CDKResultEInvalidPointer;
    }

    if (CDKResultSuccess == result)
    {
        for (UINT32 j = 0; j < pSetParams->numberOfSetParam; j++)
        {
            AFAlgoSetParam* pCurrentParam = &pSetParams->pAFSetParams[j];
            switch (pCurrentParam->setParamType)
            {
                case AFSetParamTypeROI:
                {
                    AFROIInfo* pROIInput = (AFROIInfo*) pCurrentParam->pAFSetParam;
                    for (UINT i = 0; i < pROIInput->numberOfROI ; i++)
                    {
                        CAMX_LOG_VERBOSE(CamxLogGroupStats, "roi(%d, %d, %d, %d)",
                                         pROIInput->pWeightedROI[i].ROI.left,
                                         pROIInput->pWeightedROI[i].ROI.top,
                                         pROIInput->pWeightedROI[i].ROI.width,
                                         pROIInput->pWeightedROI[i].ROI.height);
                    }
                   break;
                }

                case AFSetParamTypeFaceROI:
                {
                    AFFaceROIInfo* pROIInput = (AFFaceROIInfo*) pCurrentParam->pAFSetParam;
                    for (UINT i = 0; i < pROIInput->count; i++)
                    {
                        CAMX_LOG_VERBOSE(CamxLogGroupStats, "Face roi(%d, %d, %d, %d)",
                                         pROIInput->weightedROI[i].ROI.left,
                                         pROIInput->weightedROI[i].ROI.top,
                                         pROIInput->weightedROI[i].ROI.width,
                                         pROIInput->weightedROI[i].ROI.height);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AFGetParam()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CamxResult AFGetParam(
    CHIAFAlgorithm* pAFAlgo,
    AFAlgoGetParam* pGetParam)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pGetParam || NULL == pAFAlgo)
    {
        result = CDKResultEInvalidPointer;
    }

    if (CDKResultSuccess == result)
    {
        for (UINT32 j = 0; j < pGetParam->inputInfo.getParamInputCount; j++)
        {
            AFAlgoGetParamInput* pParamInput = &pGetParam->inputInfo.pGetParamInputs[j];
            AFAlgoGetParamOutput* pParamOutput = &pGetParam->outputInfo.pGetParamOutputs[j];

            switch (pParamInput->type)
            {
                case AFGetParamBAFFloatingWindowConfig:
                {
                    AFBAFFloatingWindowConfig* pBAFFwConfig   =
                        static_cast<AFBAFFloatingWindowConfig*>(pParamOutput->pGetParamOutput);
                    OverWriteBFConfigOutput(pBAFFwConfig);
                    pParamOutput->sizeOfWrittenGetParamOutput = sizeof(AFBAFFloatingWindowConfig);
                    break;
                }
                case AFGetParamBAFFloatingWindowROIConfig:
                {
                    AFBAFFloatingWindowROIConfig* pBAFFwROIConfig =
                        static_cast<AFBAFFloatingWindowROIConfig*>(pParamOutput->pGetParamOutput);
                    pParamOutput->sizeOfWrittenGetParamOutput = sizeof(AFBAFFloatingWindowROIConfig);
                    OverWriteROIConfigOutput(pBAFFwROIConfig);
                    break;
                }
                case AFGetParamROIDim:
                {
                    pParamOutput->getParamOutputType = pParamInput->type;
                    break;
                }
                case AFGetParamCurrentLensPosition:
                {
                    pParamOutput->getParamOutputType = pParamInput->type;
                    break;
                }
                case AFGetParamDependentVendorTags:
                {
                    CAMX_LOG_VERBOSE(CamxLogGroupStats, "Update dependent vendor");
                    CAMX_ASSERT(AFGetParamDependentVendorTags == pParamOutput->getParamOutputType);

                    StatsVendorTagInfoList* pVendorTagList =
                        static_cast<StatsVendorTagInfoList*>(pParamOutput->pGetParamOutput);
                    pVendorTagList->vendorTagCount = 0;
#if (AWB_DEPENDENT)
                    pVendorTagList->vendorTag[0].vendorTagId = g_AWBVendorTagBase + AWBAlgoTag1;
                    pVendorTagList->vendorTag[0].appliedDelay = 0;
                    pVendorTagList->vendorTag[1].vendorTagId = g_AWBVendorTagBase + AWBAlgoTag2;
                    pVendorTagList->vendorTag[1].appliedDelay = 0;
                    pVendorTagList->vendorTagCount = 2;
                    pParamOutput->sizeOfWrittenGetParamOutput = sizeof(pVendorTagList);
#endif
                    break;
                }
                case AFGetParamVendorTagList:
                {
                    CAMX_LOG_VERBOSE(CamxLogGroupStats, "Update vendor Tag list");
                    CAMX_ASSERT_MESSAGE(AFGetParamVendorTagList == pParamOutput->getParamOutputType,
                                        "pGetParamOutput->getParamOutputType %d",
                                         pParamOutput->getParamOutputType);

                    StatsVendorTagInfoList* pVendorTagList =
                        static_cast<StatsVendorTagInfoList*>(pParamOutput->pGetParamOutput);

                    pVendorTagList->vendorTag[0]   = { g_vendorTagBase + AFAlgoTag1, 0 };
                    pVendorTagList->vendorTag[1]   = { g_vendorTagBase + AFAlgoTag2, 0 };
                    pVendorTagList->vendorTagCount = g_VendorTagSectionDataAF[0].numTags;
                    pParamOutput->sizeOfWrittenGetParamOutput = sizeof(StatsVendorTagInfoList);
                    break;
                }

                default:
                {
                    CAMX_LOG_ERROR(CamxLogGroupStats, "UnSupported GetParam Type %d", pParamInput->type);
                    break;
                }
            }
        }
    }
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AFDestroy()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID AFDestroy(
    CHIAFAlgorithm*                 pAFAlgorithm,
    const AFAlgoDestroyParamList*   pDestroyParams)
{
    CAMX_UNREFERENCED_PARAM(pAFAlgorithm);
    CAMX_LOG_ERROR(CamxLogGroupStats, "StaticAF enter");

    for (UINT32 j = 0; j < pDestroyParams->paramCount; j++)
    {
        AFAlgoDestroyParam*   pParamList = &pDestroyParams->pParamList[j];

        switch (pParamList->destroyParamType)
        {
            case AFDestroyParamTypeCameraCloseIndicator:
            {
                UINT overrideCameraClose = *static_cast<UINT*>(pParamList->pParam);
                if (1 == overrideCameraClose)
                {
                    CAMX_LOG_ERROR(CamxLogGroupStats, "overrideCameraClose in  AFDestroy %d, pAFInternalData(%p)",
                        overrideCameraClose, pAFInternalData);

                    if (NULL != pAFInternalData)
                    {
                        CAMX_LOG_ERROR(CamxLogGroupStats, "StaticAF pAFInternalData in destroy is %p", pAFInternalData);
                        CAMX_DELETE(pAFInternalData);
                        pAFInternalData = NULL;
                    }
                }
                else
                {
                    CAMX_LOG_ERROR(CamxLogGroupStats, "overrideCameraClose in  AFDestroy %d", overrideCameraClose);
                }
                break;
            }
            default:
                break;
        }
    }
}

CAMX_NAMESPACE_END

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateAFAlgorithm
///
/// @brief  This method creates an instance to the AFAlgorithm.
///
/// @param  pCreateParams   Pointer to create params
/// @param  ppAfAlgorithm   Pointer to the created AfAlgorithm instance
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult CreateAFAlgorithm(
    const AFAlgoCreateParamList*    pCreateParams,
    CHIAFAlgorithm**                ppAfAlgorithm)
{
    CDKResult   result                          = CDKResultSuccess;
    // CamX::AFInternalDataType*  pAFInternalData  = NULL;
    CHISTATSHANDLE  chiSessionHandle = NULL;
    UINT* pOverrideCameraOpen = NULL;

    for (UINT32 j = 0; j < pCreateParams->paramCount; j++)
    {
        AFAlgoCreateParam*   pParamList = &pCreateParams->pParamList[j];

        switch (pParamList->createParamType)
        {
            case AFCreateParamTypeSessionHandle:
            {
                CAMX_LOG_INFO(CamxLogGroupStats, "StaticAF algo sizeOfParam for CHISTATSHANDLE is %d", pParamList->sizeOfParam);
                if ((pParamList->pParam != NULL) && (pParamList->sizeOfParam >= sizeof(CHISTATSHANDLE)))
                {
                    chiSessionHandle = static_cast<CHISTATSHANDLE>(pParamList->pParam);
                    CamX::GetupdatedMetaData(0L, chiSessionHandle);
                    CAMX_LOG_INFO(CamxLogGroupStats, "StaticAF algo create: chiSessionHandle is %p", chiSessionHandle);
                }
                break;
            }

            case AFCreateParamTypeCameraOpenIndicator:
            {
                if ((pParamList->pParam != NULL) && (pParamList->sizeOfParam >= sizeof(UINT)))
                {
                    pOverrideCameraOpen = static_cast<UINT*>(pParamList->pParam);
                    CAMX_LOG_INFO(CamxLogGroupStats, "StaticAF algo create: overrideCameraOpen is %d", *pOverrideCameraOpen);
                }
                break;
            }

            default:
                break;
        }
    }

    if ((NULL != pOverrideCameraOpen) && (1 == (*pOverrideCameraOpen)))
    {
        if (NULL == pAFInternalData)
        {
            CAMX_LOG_INFO(CamxLogGroupStats, "pAFInternalData before %p", pAFInternalData);
            pAFInternalData = CAMX_NEW CamX::AFInternalDataType();

            if (NULL == pAFInternalData)
            {
                result = CDKResultENoMemory;
            }
            else
            {
                pAFInternalData->algorithmOps.AFProcess  = CamX::AFProcess;
                pAFInternalData->algorithmOps.AFSetParam = CamX::AFSetParam;
                pAFInternalData->algorithmOps.AFGetParam = CamX::AFGetParam;
                pAFInternalData->algorithmOps.AFDestroy  = CamX::AFDestroy;
                *ppAfAlgorithm                           = reinterpret_cast<CHIAFAlgorithm*>(pAFInternalData);
                CAMX_LOG_INFO(CamxLogGroupStats, "pAFInternalData first time %p", pAFInternalData);
                result                                   = CDKResultSuccess;
            }
        }
        else
        {
            CAMX_LOG_INFO(CamxLogGroupStats, "pAFInternalData after %p", pAFInternalData);
            *ppAfAlgorithm = reinterpret_cast<CHIAFAlgorithm*>(pAFInternalData);
            result = CDKResultSuccess;
        }
        CAMX_LOG_INFO(CamxLogGroupStats, "CreateAFAlgorithm static result %s ", CamxResultStrings[result]);
    }
    else
    {
        CAMX_LOG_INFO(CamxLogGroupStats, "AF:Not actual open ...ignore:pOverrideCameraOpen %p", pOverrideCameraOpen);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AFGetCapabilities
///
/// @brief  This method creates an instance to the AFAlgorithm.
///
/// @param  pCapsInfo Pointer to Algo capability structure
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AFGetCapabilities(CHIALGORITHMCAPSINFO* pCapsInfo)
{
    CDKResult   result = CDKResultSuccess;

    CAMX_LOG_VERBOSE(CamxLogGroupStats, "Get supported Capabilities size:%d, algomask: %d",
                     pCapsInfo->size,
                     pCapsInfo->algorithmCapsMask);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AFHAFAlgorithm
///
/// @brief  This method creates an instance to the AFAlgorithm.
///
/// @param  pCreateParams Pointer to create params
///
/// @param  algoType      HAF Algo tyep
///
/// @param  ppAlgoHandle  Pointer to the created AfAlgorithm instance
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDKResult AFHAFAlgorithm(
    const AFAlgoCreateParamList* pCreateParams,
    HAFAlgoType                  algoType,
    CHIHAFAlgorithm**            ppAlgoHandle)
{
    CDKResult   result = CDKResultSuccess;
    CAMX_LOG_VERBOSE(CamxLogGroupStats,
                     "pCreateParams: %s, algoType: %d ppAlgoHandle: %s",
                      pCreateParams, algoType, ppAlgoHandle);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AFNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AFNodeQueryVendorTag(
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
            pQueryVendorTag->pVendorTagInfo = g_VendorTagInfoAF;
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
// AFAlgoSetNodeInterface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID AFAlgoSetNodeInterface(
    ChiAlgorithmInterface* pAlgoInterface)
{
    CAMX_ASSERT((NULL != pAlgoInterface)                    &&
                (NULL != pAlgoInterface->pGetVendorTagBase) &&
                (NULL != pAlgoInterface->pGetMetadata)      &&
                (NULL != pAlgoInterface->pSetMetaData));

    UINT32 result;
    g_ChiAFAlgoInterface = *pAlgoInterface;

    // get the vendor tag base
    CHIVENDORTAGBASEINFO vendorTagBase = { 0 };
    vendorTagBase.size                 = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName       = AFAlgoSectionName;

    result = g_ChiAFAlgoInterface.pGetVendorTagBase(&vendorTagBase);
    if (CDKResultSuccess == result)
    {
        g_vendorTagBase = vendorTagBase.vendorTagBase;
    }

    // get the AWB vendor tag base
#if (AWB_DEPENDENT)
    vendorTagBase.pComponentName = AWBAlgoSectionName;

    result = g_ChiAFAlgoInterface.pGetVendorTagBase(&vendorTagBase);
    if (CDKResultSuccess == result)
    {
        g_AWBVendorTagBase = vendorTagBase.vendorTagBase;
    }
#endif
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiAFAlgorithmEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom AF Algorithm.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC void ChiAFAlgorithmEntry(CHIAFALGORITHMCALLBACKS* pCallBacks)
{
    CAMX_LOG_VERBOSE(CamxLogGroupStats, "ChiAFAlgorithmEntry");

    if (NULL == pCallBacks)
    {
        CAMX_LOG_ERROR(CamxLogGroupStats, "Invalid argument: pCallBacks is NULL");
    }
    else if (pCallBacks->size < sizeof(CHIAFALGORITHMCALLBACKS))
    {
        CAMX_LOG_ERROR(CamxLogGroupStats, "pCallBacks is smaller than expected");
    }
    else
    {
        pCallBacks->size                    = sizeof(CHIAFALGORITHMCALLBACKS);
        pCallBacks->pfnGetCapabilities      = AFGetCapabilities;
        pCallBacks->pfnQueryVendorTag       = AFNodeQueryVendorTag;
        pCallBacks->pfnCreateHAFAlgorithm   = AFHAFAlgorithm;
        pCallBacks->pfnSetAlgoInterface     = AFAlgoSetNodeInterface;
        pCallBacks->pfnCreate               = CreateAFAlgorithm;
    }
}

#ifdef __cplusplus
}
#endif // __cplusplus

