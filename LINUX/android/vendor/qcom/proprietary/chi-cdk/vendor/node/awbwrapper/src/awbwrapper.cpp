////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxawb.cpp
/// @brief AWB algorithm interface implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "awbwrapper.h"
#include "camxdefs.h"
#include "chicommon.h"
#include "chistatsalgo.h"
#include <system/camera_metadata.h>
#include <cutils/log.h>

// Logging function pointer for the algorithm
StatsLoggingFunction gStatsLogger;
#define AWB_MSG_ERROR(fmt, ...)     gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogError,fmt, ##__VA_ARGS__)
#define AWB_MSG_WARN(fmt, ...)      gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogWarning,fmt, ##__VA_ARGS__)
#define AWB_MSG_HIGH(fmt, ...)      gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogVerbose,fmt, ##__VA_ARGS__)
#define AWB_MSG_LOW(fmt, ...)       gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogVerbose,fmt, ##__VA_ARGS__)
#define AWB_MSG_INFO(fmt, ...)      gStatsLogger(CamX::Log::GetFileName(__FILE__),__LINE__,__FUNCTION__,StatsLogGroupAWB,StatsLogInfo,fmt, ##__VA_ARGS__)

const CHAR*              pDefaultAWBLibName = "com.qti.stats.awb";
static const UINT32      MaxCameras         = 8;                            ///< Define max simultaneous cameras supported
CHIAWBALGORITHMCALLBACKS g_AWBLibCallbacks;
CHIAWBAlgorithm*         g_AWBLibAlgorithmIntf[MaxCameras];
CHIALGORITHMINTERFACE    g_ChiAlgoInterface;                                ///< The instance save the CAMX Chi interface

static UINT32            g_AWBVendorTagBase   = 0;                          ///< Chi assigned runtime vendor tag base for AWB
static const CHAR        AWBAlgoSectionName[] = "com.qti.stats.awbwrapper"; ///< The section name for AWB
static INT32             g_manualCCT          = 0;


// static UINT32 g_CCTVal;
///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagDataAWB[] =
{
    { "AWBGains",   TYPE_FLOAT,    3 },
    { "AWBCCT",     TYPE_INT32,    1 }
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

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


BOOL IsValidAWBGain(AWBAlgoGains* pAWBGains)
{
    if (NULL != pAWBGains &&
        (0.0 != pAWBGains->red && 0.0 != pAWBGains->green && 0.0 != pAWBGains->blue))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// WriteManualConfiguration()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDKResult WriteManualConfiguration(
    UINT64 requestId,
    CHISTATSHANDLE  chiStatsHandle)
{
    UINT32 vendorTagId = 0xFFFFFFFF;

    CDKResult result = CDKResultSuccess;

    result = g_ChiAlgoInterface.pQueryVendorTagLocation(
        "com.qti.stats.awbwrapper",
        "AWBGains",
        &vendorTagId);

    if (result != CDKResultSuccess)
    {
        AWB_MSG_ERROR( "Unable to find tag %s", "com.qti.stats.awbwrapper:AWBGains");
        return result;
    }

    CHIMETADATAINFO writeVendorTagInfo                             = { 0 };
    UINT32          writeVendorTagList[]                           = { vendorTagId };
    CHITAGDATA      writeTagData[sizeof(writeVendorTagList)]       = { { sizeof(CHITAGDATA), 0, NULL, 0, 0, FALSE } };

    FLOAT manualAWBGains[] = { 5.0f, 1.0f, 1.0f};

    writeTagData[0].pData           = &manualAWBGains;
    writeTagData[0].requestId       = requestId;
    writeTagData[0].size            = sizeof(FLOAT);
    writeTagData[0].dataSize        = sizeof(manualAWBGains)/sizeof(FLOAT);

    writeVendorTagInfo.pTagData     = &writeTagData[0];
    writeVendorTagInfo.pTagList     = &writeVendorTagList[0];
    writeVendorTagInfo.tagNum       = sizeof(writeVendorTagList) / sizeof(writeVendorTagList[0]);
    writeVendorTagInfo.metadataType = ChiMetadataDynamic;
    writeVendorTagInfo.size         = sizeof(CHIMETADATAINFO);
    writeVendorTagInfo.chiSession   = chiStatsHandle;

    result = g_ChiAlgoInterface.pSetMetaData(&writeVendorTagInfo);

    if ((CDKResultSuccess != result) || (NULL == writeTagData[0].pData))
    {
        AWB_MSG_ERROR(
            "Failed to write vendor tag %s",
            "com.qti.stats.awbwrapper:AWBGains");
    }
    else
    {
        AWB_MSG_LOW(
            "Successfully wrote svendor tag %s value: %d",
            "com.qti.stats.awbwrapper:AWBGains",
            *(UINT32 *)writeTagData[0].pData);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// WriteManualConfigurationForCCT()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDKResult WriteManualConfigurationForCCT(
    UINT64 requestId,
    CHISTATSHANDLE  chiStatsHandle)
{
    UINT32 vendorTagId = 0xFFFFFFFF;

    CDKResult result = CDKResultSuccess;

    result = g_ChiAlgoInterface.pQueryVendorTagLocation(
        "com.qti.stats.awbwrapper",
        "AWBCCT",
        &vendorTagId);

    if (result != CDKResultSuccess)
    {
        AWB_MSG_ERROR("Unable to find tag %s", "com.qti.stats.awbwrapper:AWBCCT");
        return result;
    }

    if (requestId > 100 && requestId <= 200)
    {
        g_manualCCT = 5000;
    }
    else if (requestId > 200 && requestId <= 300)
    {
        g_manualCCT = 6000;
    }
    else if (requestId > 300 && requestId <= 400)
    {
        g_manualCCT = 7000;
    }
    else if (requestId > 400 && requestId <= 600)
    {
        g_manualCCT = 8000;
    }
    else
    {
        g_manualCCT = 10000;
    }
    CHIMETADATAINFO writeVendorTagInfo = { 0 };
    UINT32          writeVendorTagList[] = { vendorTagId };
    CHITAGDATA      writeTagData[sizeof(writeVendorTagList)] = { { sizeof(CHITAGDATA), 0, NULL, 0, 0, FALSE } };

    writeTagData[0].pData = &g_manualCCT;
    writeTagData[0].requestId = requestId;
    writeTagData[0].size = sizeof(INT32);
    writeTagData[0].dataSize = sizeof(g_manualCCT) / sizeof(INT32);

    writeVendorTagInfo.pTagData = &writeTagData[0];
    writeVendorTagInfo.pTagList = &writeVendorTagList[0];
    writeVendorTagInfo.tagNum = sizeof(writeVendorTagList) / sizeof(writeVendorTagList[0]);
    writeVendorTagInfo.metadataType = ChiMetadataDynamic;
    writeVendorTagInfo.size = sizeof(CHIMETADATAINFO);
    writeVendorTagInfo.chiSession = chiStatsHandle;

    result = g_ChiAlgoInterface.pSetMetaData(&writeVendorTagInfo);

    if ((CDKResultSuccess != result) || (NULL == writeTagData[0].pData))
    {
        AWB_MSG_ERROR(
            "Failed to write vendor tag %s",
            "com.qti.stats.awbwrapper:AWBCCT");
    }
    else
    {
        AWB_MSG_LOW(
            "Successfully wrote svendor tag %s value: %d ReqId=%llu",
            "com.qti.stats.awbwrapper:AWBCCT",
            *(UINT32 *)writeTagData[0].pData, requestId);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetAWBGain()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDKResult GetAWBGain(
    UINT64  requestId,
    CHISTATSHANDLE  chiStatsHandle,
    AWBAlgoGains* pAWBGains)
{
    CHIMETADATAINFO AWBVendortagInfo   = { 0 };
    UINT32          AWBVendorTagList[] = { (g_AWBVendorTagBase + 0) | InputMetadataSectionMask };
    CHITAGDATA  tagData[]              = { { sizeof(CHITAGDATA), requestId, NULL, 0, 0, FALSE } };
    CDKResult result                   = CDKResultSuccess;

    AWBVendortagInfo.pTagList            = &AWBVendorTagList[0];
    AWBVendortagInfo.tagNum              = sizeof(AWBVendorTagList)/sizeof(AWBVendorTagList[0]);
    AWBVendortagInfo.size                = sizeof(CHIMETADATAINFO);
    AWBVendortagInfo.pTagData            = &tagData[0];
    AWBVendortagInfo.metadataType        = ChiMetadataDynamic;
    AWBVendortagInfo.chiSession          = chiStatsHandle;
    AWBVendortagInfo.pTagData->requestId = requestId;

    result = g_ChiAlgoInterface.pGetMetadata(&AWBVendortagInfo);

    if ((CDKResultSuccess != result) || (NULL == AWBVendortagInfo.pTagData[0].pData))
    {
        AWB_MSG_LOW(
            "Input vendor tag AWBGain id(%u) not found",
            AWBVendortagInfo.pTagList[0]);
        result = CDKResultENoSuch;
    }
    else
    {
        AWB_MSG_LOW(
            "Received vendor tag using GetMetadata API. TagId(%u) pData(%p) size(%u) reqID(%llu)",
            AWBVendortagInfo.pTagList[0],
            AWBVendortagInfo.pTagData[0].pData,
            AWBVendortagInfo.pTagData[0].size,
            AWBVendortagInfo.pTagData[0].requestId);

        memcpy(pAWBGains, AWBVendortagInfo.pTagData[0].pData, sizeof(AWBAlgoGains));
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetAWBCCT()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDKResult GetAWBCCT(
    UINT64  requestId,
    CHISTATSHANDLE  chiStatsHandle,
    UINT32* pAWBCCT)
{
    CDKResult result = CDKResultSuccess;

    CHIMETADATAINFO AWBVendortagInfo    = { 0 };
    UINT32          AWBVendorTagList[] = { (g_AWBVendorTagBase + 1) | InputMetadataSectionMask };
    CHITAGDATA      tagData[]           = { { sizeof(CHITAGDATA), requestId, NULL, 0, 0, FALSE } };

    AWBVendortagInfo.pTagList            = &AWBVendorTagList[0];
    AWBVendortagInfo.tagNum              = sizeof(AWBVendorTagList) / sizeof(AWBVendorTagList[0]);
    AWBVendortagInfo.size                = sizeof(CHIMETADATAINFO);
    AWBVendortagInfo.pTagData            = &tagData[0];
    AWBVendortagInfo.metadataType        = ChiMetadataDynamic;
    AWBVendortagInfo.chiSession          = chiStatsHandle;
    AWBVendortagInfo.pTagData->requestId = requestId;

    result = g_ChiAlgoInterface.pGetMetadata(&AWBVendortagInfo);

    if ((CDKResultSuccess != result) || (NULL == AWBVendortagInfo.pTagData[0].pData))
    {
        AWB_MSG_LOW(
            "Input vendor tag AWBCCT id(%u) not found",
            AWBVendortagInfo.pTagList[0]);
    }
    else
    {
        AWB_MSG_LOW(
            "Received vendor tag using GetMetadata API. TagId(%u) pData(%p) size(%u) reqID(%llu)",
            AWBVendortagInfo.pTagList[0],
            AWBVendortagInfo.pTagData[0].pData,
            AWBVendortagInfo.pTagData[0].size,
            AWBVendortagInfo.pTagData[0].requestId);
        memcpy(pAWBCCT, AWBVendortagInfo.pTagData[0].pData, sizeof(UINT32));
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// IsManualAWBMode()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL IsManualAWBMode(
    UINT64          requestId,
    CHISTATSHANDLE  chiStatsHandle)
{
    CDKResult result = CDKResultSuccess;

    // Read input vendor tag
    UINT32          ControlAWBMode        = ANDROID_CONTROL_AWB_MODE | InputMetadataSectionMask;
    CHIMETADATAINFO InputVendortagInfo    = { 0 };
    UINT32          InputVendortagList[1] = { ControlAWBMode };
    CHITAGDATA      InputtagData[1]       = { { sizeof(CHITAGDATA), 0, NULL, 0, 0, FALSE } };

    InputVendortagInfo.pTagData            = &InputtagData[0];
    InputVendortagInfo.pTagData->requestId = requestId;
    InputVendortagInfo.pTagList            = &InputVendortagList[0];
    InputVendortagInfo.tagNum              = sizeof(InputVendortagList)/sizeof(InputVendortagList[0]);
    InputVendortagInfo.metadataType        = ChiMetadataDynamic;
    InputVendortagInfo.size                = sizeof(CHIMETADATAINFO);
    InputVendortagInfo.chiSession          = chiStatsHandle;

    result = g_ChiAlgoInterface.pGetMetadata(&InputVendortagInfo);

    if ((CDKResultSuccess != result) || (NULL == InputtagData[0].pData))
    {
        AWB_MSG_LOW(
            "Input vendor tag ControlAWBMode id(%u) not found",
            InputVendortagInfo.pTagList[0]);
    }
    else
    {
        AWB_MSG_LOW(
            "Received input vendor tag ControlAWBMode.  id(%u) value(%d) DataSize(%d)",
            InputVendortagInfo.pTagList[0],
            *static_cast<BYTE*>(InputtagData[0].pData),
            InputtagData[0].dataSize);

        if (0 == *static_cast<BYTE*>(InputtagData[0].pData)) // Per android 0 is AWBModeOFF (manual)
            return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AWBGetParam
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AWBGetParam(
    CHIAWBAlgorithm*    pAWBAlgorithm,
    AWBAlgoGetParam*    pGetParams)
{
    CDKResult   result          = CDKResultSuccess;
    UINT32      curCameraID     = MaxCameras; // Valid camera ID should range from 0 to MaxCameras - 1
    UINT32      index           = 0;

    if ((NULL == pAWBAlgorithm) || (NULL == pGetParams))
    {
        return CDKResultEInvalidArg;
    }
    for (index = 0; index < pGetParams->inputInfoList.getParamInputCount; index++)
    {
        if (AWBGetParamInputTypeCameraInfo == pGetParams->inputInfoList.pGetParamInputs[index].getParamInputType)
        {
            curCameraID = reinterpret_cast<StatsCameraInfo*>(
                pGetParams->inputInfoList.pGetParamInputs[index].pGetParamInput)->cameraId;
        }
    }

    if (MaxCameras == curCameraID)
    {
        AWB_MSG_ERROR("Incorrect camera ID: %d", curCameraID);
        return CDKResultEInvalidArg;
    }

    /* AWB wrapper calls awb lib awb AWBGetParam */
    if (NULL != g_AWBLibAlgorithmIntf[curCameraID]->AWBGetParam)
    {
        result = g_AWBLibAlgorithmIntf[curCameraID]->AWBGetParam(g_AWBLibAlgorithmIntf[curCameraID], pGetParams);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AWBSetParam
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AWBSetParam(
    CHIAWBAlgorithm*            pAWBAlgorithm,
    const AWBAlgoSetParamList*  pSetParams)
{
    if ((NULL == pAWBAlgorithm) || (NULL == pSetParams))
    {
        return CDKResultEInvalidArg;
    }

    CDKResult   result          = CDKResultSuccess;
    UINT32      curCameraID     = MaxCameras; // Valid camera ID should range from 0 to MaxCameras - 1
    UINT32      index           = 0;

    for (index = 0; index < pSetParams->inputParamsCount; index++)
    {
        if (AWBSetParamTypeCameraInfo == pSetParams->pAWBSetParams[index].setParamType)
        {
            curCameraID = reinterpret_cast<const StatsCameraInfo*>(pSetParams->pAWBSetParams[index].pAWBSetParam)->cameraId;
        }
    }

    if (MaxCameras == curCameraID)
    {
        AWB_MSG_ERROR("Incorrect camera ID: %d", curCameraID);
        return CDKResultEInvalidArg;
    }

    /* AWB wrapper calls awb lib awb AWBSetParam */
    if (NULL != g_AWBLibAlgorithmIntf[curCameraID]->AWBSetParam)
    {
        result = g_AWBLibAlgorithmIntf[curCameraID]->AWBSetParam(g_AWBLibAlgorithmIntf[curCameraID], pSetParams);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetAWBGainsFromCCT()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GetAWBGainsFromCCT(
    CHIAWBAlgorithm* pAWBAlgorithm,
    UINT32 cctValueIn,
    AWBAlgoGains* pGainsOut,
    StatsCameraInfo* pCamInfo)
{
    CDKResult result = CDKResultSuccess;
    if (NULL == pGainsOut || NULL == pCamInfo)
    {
        result = CDKResultEInvalidPointer;
        AWB_MSG_ERROR("Invalid argument: pGainsOut is NULL");
    }

    AWBAlgoGetParam             algoGetParam;
    AWBAlgoGetParamInput        algoGetParamInput[AWBGetParamInputTypeLastIndex];
    AWBAlgoGetParamOutput       algoGetParamOutput;

    memset(&algoGetParam, 0, sizeof(algoGetParam));
    memset(&algoGetParamInput, 0, sizeof(algoGetParamInput));
    memset(&algoGetParamOutput, 0, sizeof(algoGetParamOutput));

    algoGetParamInput[AWBGetParamInputTypeCameraInfo].getParamInputType   = AWBGetParamInputTypeCameraInfo;
    algoGetParamInput[AWBGetParamInputTypeCameraInfo].pGetParamInput      = reinterpret_cast<VOID*>(pCamInfo);
    algoGetParamInput[AWBGetParamInputTypeCameraInfo].sizeOfGetParamInput = sizeof(StatsCameraInfo);
    algoGetParamInput[AWBGetParamInputTypeCCT].getParamInputType = AWBGetParamInputTypeCCT;
    algoGetParamInput[AWBGetParamInputTypeCCT].pGetParamInput = &cctValueIn;
    algoGetParamInput[AWBGetParamInputTypeCCT].sizeOfGetParamInput = sizeof(cctValueIn);

    algoGetParamOutput.getParamOutputType = AWBGetParamOutputTypeGains;
    algoGetParamOutput.pGetParamOutput = pGainsOut;
    algoGetParamOutput.sizeOfGetParamOutput = sizeof(AWBAlgoGains);

    algoGetParam.type = AWBGetParamTypeCctToGain;
    algoGetParam.inputInfoList.getParamInputCount = AWBGetParamInputTypeLastIndex;
    algoGetParam.inputInfoList.pGetParamInputs = &algoGetParamInput[0];

    algoGetParam.outputInfoList.getParamOutputCount = 1;
    algoGetParam.outputInfoList.pGetParamOutputs = &algoGetParamOutput;

    result = AWBGetParam(pAWBAlgorithm, &algoGetParam);
    if (CDKResultSuccess != result)
    {
        AWB_MSG_ERROR("AWBWrapper: Failed to get AWB Gains from CCT");
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetCCTFromAWBGains()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GetCCTFromAWBGains(
    CHIAWBAlgorithm* pAWBAlgorithm,
    AWBAlgoGains* pAwbGainsIn,
    UINT32* pCCTOut,
    StatsCameraInfo* pCamInfo)
{
    CDKResult result = CDKResultSuccess;
    if (NULL == pCCTOut || NULL == pCamInfo)
    {
        result = CDKResultEInvalidPointer;
        AWB_MSG_ERROR("Invalid argument: pGainsOut is NULL");
    }

    AWBAlgoGetParam             algoGetParam;
    AWBAlgoGetParamInput        algoGetParamInput[AWBGetParamInputTypeLastIndex];
    AWBAlgoGetParamOutput       algoGetParamOutput;

    memset(&algoGetParam, 0, sizeof(algoGetParam));
    memset(&algoGetParamInput, 0, sizeof(algoGetParamInput));
    memset(&algoGetParamOutput, 0, sizeof(algoGetParamOutput));

    algoGetParamInput[AWBGetParamInputTypeCameraInfo].getParamInputType   = AWBGetParamInputTypeCameraInfo;
    algoGetParamInput[AWBGetParamInputTypeCameraInfo].pGetParamInput      = reinterpret_cast<VOID*>(pCamInfo);
    algoGetParamInput[AWBGetParamInputTypeCameraInfo].sizeOfGetParamInput = sizeof(StatsCameraInfo);
    algoGetParamInput[AWBGetParamInputTypeGain].getParamInputType = AWBGetParamInputTypeGain;
    algoGetParamInput[AWBGetParamInputTypeGain].pGetParamInput = pAwbGainsIn;
    algoGetParamInput[AWBGetParamInputTypeGain].sizeOfGetParamInput = sizeof(AWBAlgoGains);

    algoGetParamOutput.getParamOutputType = AWBGetParamOutputTypeCCT;
    algoGetParamOutput.pGetParamOutput = pCCTOut;
    algoGetParamOutput.sizeOfGetParamOutput = sizeof(UINT32);

    algoGetParam.type = AWBGetParamTypeGainToCCT;
    algoGetParam.inputInfoList.getParamInputCount = AWBGetParamInputTypeLastIndex;
    algoGetParam.inputInfoList.pGetParamInputs = &algoGetParamInput[0];

    algoGetParam.outputInfoList.getParamOutputCount = 1;
    algoGetParam.outputInfoList.pGetParamOutputs = &algoGetParamOutput;

    result = AWBGetParam(pAWBAlgorithm, &algoGetParam);
    if (CDKResultSuccess != result)
    {
        AWB_MSG_ERROR("AWBWrapper: Failed to get AWB Gains from CCT");
    }

    return result;
}

VOID* GetAWBOutputPointer(AWBAlgoOutputList*  pOutputs, AWBAlgoOutputType outputType)
{
    VOID* pOutPtr = NULL;
    for (UINT32 i = 0; i < pOutputs->outputCount; i++)
    {
        if (outputType == pOutputs->pAWBOutputs[i].outputType)
        {
            pOutPtr = pOutputs->pAWBOutputs[i].pAWBOutput;
            break;
        }
    }
    return pOutPtr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OverrideAlgoOutput()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID OverrideAlgoOutput(
    CHIAWBAlgorithm* pAWBAlgorithm,
    UINT64          requestId,
    CHISTATSHANDLE  chiStatsHandle,
    AWBAlgoOutputList*  pOutputs,
    StatsCameraInfo* pCamInfo)
{
    for (UINT32 i = 0; i < pOutputs->outputCount; i++)
    {
        switch (pOutputs->pAWBOutputs[i].outputType)
        {
            case AWBOutputTypeGains:
                {
                    AWBAlgoGains tmpAWBGains;
                    memset(&tmpAWBGains, 0, sizeof(AWBAlgoGains));
                    CDKResult result = GetAWBGain(requestId, chiStatsHandle, &tmpAWBGains);

                    if ( CDKResultSuccess == result &&
                         TRUE == IsValidAWBGain(&tmpAWBGains) )
                    {
                        result    = CDKResultSuccess;
                        UINT32 cctValueOut  = 0;
                        AWBAlgoGains *pAwbGainFromAlgo = static_cast<AWBAlgoGains*>(pOutputs->pAWBOutputs[i].pAWBOutput);
                        VOID* pAwbCCTFromAlgo          = GetAWBOutputPointer(pOutputs, AWBOutputTypeColorTemperature);
                        memcpy(pAwbGainFromAlgo, &tmpAWBGains, sizeof(AWBAlgoGains));
                        pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput = sizeof(AWBAlgoGains);
                        result = GetCCTFromAWBGains(pAWBAlgorithm, pAwbGainFromAlgo, &cctValueOut, pCamInfo);
                        AWB_MSG_LOW("AWBWrapper: InGains = %f:%f:%f To OutCCT:%d",
                            pAwbGainFromAlgo->red,
                            pAwbGainFromAlgo->green,
                            pAwbGainFromAlgo->blue,
                            cctValueOut);
                        if (NULL != pAwbCCTFromAlgo)
                        {
                            memcpy(pAwbCCTFromAlgo, &cctValueOut, sizeof(cctValueOut));
                            AWB_MSG_LOW("AWBWrapper: Job Done - Updated CCT output");
                        }
                    }
                }
                break;

            case AWBOutputTypeColorTemperature:
                {
                    UINT32 cctValueFromAlgo = 0;
                    UINT32 cctValueFromApp  = 0;
                    VOID* pAwbGainsFromAlgo = GetAWBOutputPointer(pOutputs, AWBOutputTypeGains);
                    cctValueFromAlgo = *(static_cast<UINT32*>(pOutputs->pAWBOutputs[i].pAWBOutput));
                    CDKResult result = GetAWBCCT(requestId, chiStatsHandle, &cctValueFromApp);
                    if (CDKResultSuccess == result &&
                        0 != cctValueFromApp)
                    {
                        *(static_cast<UINT32*>(pOutputs->pAWBOutputs[i].pAWBOutput)) = cctValueFromApp;
                        pOutputs->pAWBOutputs[i].sizeOfWrittenAWBOutput = sizeof(UINT32);

                        if (cctValueFromAlgo != cctValueFromApp)
                        {
                            result = CDKResultSuccess;
                            AWBAlgoGains awbGainsOut;
                            memset(&awbGainsOut, 0, sizeof(awbGainsOut));
                            result = GetAWBGainsFromCCT(pAWBAlgorithm, cctValueFromApp, &awbGainsOut, pCamInfo);
                            AWB_MSG_LOW("AWB_WRAPPER_DEBUG:ReqId=%llu Ret=%d CCT:FromAlgo=%d FromApp=%d to Gains = %f:%f:%f",
                                requestId,
                                result,
                                cctValueFromAlgo,
                                cctValueFromApp,
                                awbGainsOut.red,
                                awbGainsOut.green,
                                awbGainsOut.blue);
                            if (NULL != pAwbGainsFromAlgo)
                            {
                                memcpy(pAwbGainsFromAlgo, &awbGainsOut, sizeof(awbGainsOut));
                                AWB_MSG_LOW("AWBWrapper: Job Done - Updated GAIN output");
                            }
                        }
                    }
                }
                break;

            default:
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AWBDestroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID AWBDestroy(
    CHIAWBAlgorithm*                 pAWBAlgorithm,
    const AWBAlgoDestroyParamList*   pDestroyParams)
{
    UINT32 curCameraID  = MaxCameras; // Valid camera ID should range from 0 to MaxCameras - 1
    UINT32 index        = 0;

    if (NULL == pAWBAlgorithm)
    {
        AWB_MSG_ERROR("pAWBAlgorithm is NULL");
        return;
    }
    else
    {
        for (index = 0; index < pDestroyParams->paramCount; index++)
        {
            if (AWBDestroyParamTypeCameraInfo == pDestroyParams->pParamList[index].destroyParamType)
            {
                curCameraID = reinterpret_cast<StatsCameraInfo*>(pDestroyParams->pParamList[index].pParam)->cameraId;
            }
        }
        /* AWB wrapper calls awb lib destroy */
        if (MaxCameras != curCameraID)
        {
            if (NULL != g_AWBLibAlgorithmIntf[curCameraID]->AWBDestroy)
            {
                g_AWBLibAlgorithmIntf[curCameraID]->AWBDestroy(g_AWBLibAlgorithmIntf[curCameraID], pDestroyParams);
                g_AWBLibAlgorithmIntf[curCameraID] = NULL;
            }
        }
        else
        {
            AWB_MSG_ERROR("Incorrect camera ID: %d", curCameraID);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AWBProcess
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AWBProcess(
    CHIAWBAlgorithm*        pAWBAlgorithm,
    const AWBAlgoInputList* pInputs,
    AWBAlgoOutputList*      pOutputs)
{
    if ((NULL == pAWBAlgorithm) || (NULL == pInputs) || (NULL == pOutputs))
    {
        return CDKResultEInvalidArg;
    }

    CDKResult   result          = CDKResultSuccess;
    UINT32      curCameraID     = MaxCameras; // Valid camera ID should range from 0 to MaxCameras - 1
    UINT32      index           = 0;
    StatsCameraInfo* pCamInfo   = NULL;

    for (index = 0; index < pInputs->inputCount; index++)
    {
        if (AWBInputTypeCameraInfo == pInputs->pAWBInputs[index].inputType)
        {
            pCamInfo = reinterpret_cast<StatsCameraInfo*>(pInputs->pAWBInputs[index].pAWBInput);
            curCameraID = pCamInfo->cameraId;
        }
    }

    if (MaxCameras == curCameraID)
    {
        AWB_MSG_ERROR("Incorrect camera ID: %d", curCameraID);
        return CDKResultEInvalidArg;
    }

    /* AWB wrapper calls awb lib awb process */
    if (NULL != g_AWBLibAlgorithmIntf[curCameraID]->AWBProcess)
    {
        result = g_AWBLibAlgorithmIntf[curCameraID]->AWBProcess(g_AWBLibAlgorithmIntf[curCameraID], pInputs, pOutputs);
    }

    if (CDKResultSuccess == result)
    {
        UINT64 requestId = 0L;
        CHISTATSHANDLE  chiStatsHandle = NULL;

        CAMX_ASSERT_MESSAGE(NULL != pOutputs, "Output NULL pointer");

        for (UINT32 i = 0; i < pInputs->inputCount; i++)
        {
            if (pInputs->pAWBInputs[i].sizeOfAWBInput == 0)
                continue;

            switch (pInputs->pAWBInputs[i].inputType)
            {
                case AWBInputTypeRequestNumber:
                {
                    requestId = *(static_cast<UINT64*>(pInputs->pAWBInputs[i].pAWBInput));
                    break;
                }

                case AWBInputTypeStatsChiHandle:
                {
                    chiStatsHandle = static_cast<CHISTATSHANDLE>(pInputs->pAWBInputs[i].pAWBInput);
                    break;
                }
                default:
                    break;
            }
        }

#if 0 /* Test code to verify Manual AWB call flow */
        g_manualCCT = 0;
        if (requestId > 100 && requestId < 1000)
        {
            WriteManualConfigurationForCCT(requestId, chiStatsHandle);
        }
#endif

        if (TRUE == IsManualAWBMode(requestId, chiStatsHandle))
        {
            AWB_MSG_LOW("AWB_WRAPPER_DEBUG: Manual AWB Mode : ReqId=%llu g_manualCCT=%d", requestId, g_manualCCT);
            OverrideAlgoOutput(pAWBAlgorithm, requestId, chiStatsHandle, pOutputs, pCamInfo);
        }
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
    CDKResult result        = CDKResultSuccess;
    UINT32    curCameraID   = MaxCameras; // Valid camera ID should range from 0 to MaxCameras - 1

    const CHAR* pAWBLibName ="com.qti.stats.awb";
#if defined(_LP64)
    const CHAR* pLibPath = "/vendor/lib64/camera/components/";
#else // _LP64
    const CHAR* pLibPath = "/vendor/lib/camera/components/";
#endif // _LP64
    CHAR        libFilename[FILENAME_MAX];

    if (pCreateParams->pCreateParams != NULL)
    {
        AWBAlgoCreateParam* pCreateLoggerFunction = NULL;
        pCreateLoggerFunction = &pCreateParams->pCreateParams[AWBCreateParamsTypeLoggerFunctionPtr];
        gStatsLogger = reinterpret_cast<StatsLoggingFunction>(pCreateLoggerFunction->pCreateParam);
        result = CDKResultSuccess;

        (void)snprintf(libFilename, FILENAME_MAX, "%s%s.%s", pLibPath, pAWBLibName, "so");

        CamX::OSLIBRARYHANDLE       handle = CamX::OsUtils::LibMap(libFilename);

        if (NULL != handle)
        {
            VOID *pAddr = NULL;
            PFAWBCHIALGORITHMENTRY      pAWBAlgoEntry = NULL;
            const CHAR                  ChiAWBEntry[] = "ChiAWBAlgorithmEntry";

            pAddr = CamX::OsUtils::LibGetAddr(handle, ChiAWBEntry);
            if (NULL != pAddr)
            {
                pAWBAlgoEntry = reinterpret_cast<PFAWBCHIALGORITHMENTRY>(pAddr);

                memset(&g_AWBLibCallbacks, 0, sizeof(CHIAWBALGORITHMCALLBACKS));

                if (NULL != pAWBAlgoEntry)
                {
                    g_AWBLibCallbacks.size = sizeof(CHIAWBALGORITHMCALLBACKS);

                    // Call AWB lib entry function to retrieve callbacks
                    pAWBAlgoEntry(&g_AWBLibCallbacks);

                    // Create AWB lib instance
                    if (NULL != g_AWBLibCallbacks.pfnCreate)
                    {
                        result = g_AWBLibCallbacks.pfnCreate(pCreateParams, ppAWBAlgorithm);
                    }
                    else
                    {
                        AWB_MSG_ERROR( "AWB lib create function is NULL pointer");
                        result = CDKResultEFailed;
                    }
                    if (result == CDKResultSuccess)
                    {
                        UINT32 index = 0;

                        for (index = 0; index < pCreateParams->createParamsCount; index++)
                        {
                            if (AWBCreateParamTypeCameraInfo == pCreateParams->pCreateParams[index].createParamType)
                            {
                                curCameraID = reinterpret_cast<StatsCameraInfo*>(
                                    pCreateParams->pCreateParams[index].pCreateParam)->cameraId;
                            }
                        }

                        if (MaxCameras == curCameraID)
                        {
                            AWB_MSG_ERROR("Incorrect camera ID: %d", curCameraID);
                            result = CDKResultEInvalidArg;
                        }
                    }
                    if (result == CDKResultSuccess)
                    {
                        // Store awb lib API for awb wrapper to wrap the call
                        g_AWBLibAlgorithmIntf[curCameraID] = *ppAWBAlgorithm;

                        /* Instantiate AWB wrapper and hook up API */
                        AWBWrapperInternalDataType* pAWBWrapperInternalData = new AWBWrapperInternalDataType;
                        if (NULL == pAWBWrapperInternalData)
                        {
                            result = CDKResultENoMemory;
                        }

                        if (CDKResultSuccess == result)
                        {
                            pAWBWrapperInternalData->AWBOps.AWBProcess  = AWBProcess;
                            pAWBWrapperInternalData->AWBOps.AWBSetParam = AWBSetParam;
                            pAWBWrapperInternalData->AWBOps.AWBGetParam = AWBGetParam;
                            pAWBWrapperInternalData->AWBOps.AWBDestroy  = AWBDestroy;
                            *ppAWBAlgorithm                             =
                                reinterpret_cast<CHIAWBAlgorithm*>(pAWBWrapperInternalData);
                            AWB_MSG_INFO("AWB Wrapper loaded");
                        }
                    }
                }
                else
                {
                    AWB_MSG_ERROR( "AWB lib entry function is NULL");
                    result = CDKResultEFailed;
                }
            }
        }
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

    AWB_MSG_LOW("Get supported Capabilities size:%d, algo mask: %d", pCapsInfo->size, pCapsInfo->algorithmCapsMask);

    // AWB wrapper direct calling AWB lib GetCapabilities
    if (g_AWBLibCallbacks.pfnGetCapabilities != NULL)
    {
        result = g_AWBLibCallbacks.pfnGetCapabilities(pCapsInfo);
    }
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
// AWBAlgoSetAlgoInterface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID AWBAlgoSetAlgoInterface(
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
            g_AWBVendorTagBase  = vendorTagBase.vendorTagBase;
            g_AWBVendorTagBase |= InputMetadataSectionMask;
        }

    CAMX_UNREFERENCED_PARAM(pAlgoInterface);
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
CAMX_VISIBILITY_PUBLIC VOID ChiAWBAlgorithmEntry(CHIAWBALGORITHMCALLBACKS* pCallBacks)
{
    ALOGE( "ChiAWBAlgorithmEntry");

    if (NULL == pCallBacks)
    {
        ALOGE( "Invalid argument: pCallBacks is NULL");
    }
    else if (pCallBacks->size < sizeof(CHIAWBALGORITHMCALLBACKS))
    {
        ALOGE( "pCallBacks is smaller than expected");
    }
    else
    {
        pCallBacks->pfnGetCapabilities  = AWBGetCapabilities;
        pCallBacks->pfnCreate           = CreateAWBAlgorithm;
        pCallBacks->pfnSetAlgoInterface = AWBAlgoSetAlgoInterface;
        pCallBacks->pfnQueryVendorTag   = AWBNodeQueryVendorTag;
    }
}

#ifdef __cplusplus
}
#endif // __cplusplus

