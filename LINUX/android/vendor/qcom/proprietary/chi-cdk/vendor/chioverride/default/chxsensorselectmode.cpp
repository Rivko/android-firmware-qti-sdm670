////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxsensorselectmode.cpp
/// @brief SensorSelectMode class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxextensionmodule.h"
#include "chxsensorselectmode.h"
#include "chxutils.h"
#include <cutils/log.h>

static const UINT MinHFRFps = 90;

#undef  LOG_TAG
#define LOG_TAG "CHIUSECASE"

/// @todo This is a reference default implementation that can be replaced with customized selection
static const BOOL ModeSelectTable[MaxModeSelectCondition][MaxModeSelectUsecase] =
{
    // FAST AEC  QUAD  HFR   IHDR  RHDR VHDR Snapshot Video/P
    {    0,      0,    1,    1,    1,   0,   0,       1    },    // Condition FPS
    {    0,      0,    0,    0,    0,   0,   0,       0    },    // Condition Bounded FPS
    {    0,      0,    0,    1,    1,   0,   0,       1    },    // Condition Aspect ratio
    {    0,      0,    0,    1,    1,   0,   0,       1    },    // Condition Resolution w
    {    0,      0,    0,    1,    1,   0,   0,       1    },    // Condition Resolution h
    {    0,      0,    0,    0,    0,   0,   0,       0    },    // Condition Pix Clk
    {    0,      0,    0,    0,    0,   0,   0,       0    },    // Condition QUADRA
    {    0,      0,    1,    0,    0,   0,   0,       0    },    // Condition HFR
    {    0,      0,    0,    0,    0,   0,   0,       1    },    // Condition NORMAL
    {    0,      0,    0,    1,    0,   0,   0,       0    },    // Condition IHDR
    {    0,      0,    0,    0,    1,   0,   0,       0    },    // Condition ZZHDR / RHDR
    {    0,      0,    1,    0,    0,   0,   0,       0    },    // Condition Max Res / MPIX
    {    0,      0,    0,    1,    1,   0,   0,       1    },    // Condition BestRes
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingWidth
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingWidth(
    const ChiSensorModeInfo* pModeInfo,
    const DesiredSensorMode* pDesiredSensorMode)
{
    CHX_ASSERT(StreamImage == pModeInfo->streamtype);

    CHX_LOG("FrameDimension.width %d  optimal width %d maxWidth %d minWidth %d",
             pModeInfo->frameDimension.width, pDesiredSensorMode->optimalWidth,
             pDesiredSensorMode->maxWidth, pDesiredSensorMode->minWidth);

    // Width selected should be greater than or equal to the optimal requested width but it shouldn't exceed the maximum width
    // than the requesting port can handle
    if (pDesiredSensorMode->maxWidth == pDesiredSensorMode->optimalWidth)
    {
        return (pModeInfo->frameDimension.width >= pDesiredSensorMode->optimalWidth);

    }
    else
    {
        return ((pModeInfo->frameDimension.width >= pDesiredSensorMode->optimalWidth) &&
            (pModeInfo->frameDimension.width <= pDesiredSensorMode->maxWidth));

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingHeight
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingHeight(
    const ChiSensorModeInfo* pModeInfo,
    const DesiredSensorMode* pDesiredSensorMode)
{
    CHX_ASSERT(StreamImage == pModeInfo->streamtype);

    CHX_LOG("FrameDimension.height %d  optimal height %d maxHeight %d minHeight %d",
            pModeInfo->frameDimension.height, pDesiredSensorMode->optimalHeight,
            pDesiredSensorMode->maxHeight, pDesiredSensorMode->minHeight);


    /// Height selected should be greater than or equal to the optimal requested height
    /// but it shouldn't exceed the maximum height that the requesting port can handle.
    if (pDesiredSensorMode->maxHeight == pDesiredSensorMode->optimalHeight)
    {
        return (pModeInfo->frameDimension.height >= pDesiredSensorMode->optimalHeight);

    }
    else
    {
        return ((pModeInfo->frameDimension.height >= pDesiredSensorMode->optimalHeight) &&
            (pModeInfo->frameDimension.height <= pDesiredSensorMode->maxHeight));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingAspectRatio
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingAspectRatio(
    const ChiSensorModeInfo*     pModeInfo,
    const DesiredSensorMode*     pDesiredSensorMode,
    SensorSelectBestAspectRatio* pSelectedAspectRatio)
{
    FLOAT modeAspectRatio      = 0;
    FLOAT requestedAspectRatio = 0;
    FLOAT aspectRatioDiff      = 0;

    CHX_ASSERT(StreamImage == pModeInfo->streamtype);

    modeAspectRatio = (static_cast<FLOAT>(pModeInfo->frameDimension.width) /
                       static_cast<FLOAT>(pModeInfo->frameDimension.height));

    requestedAspectRatio = (static_cast<FLOAT>(pDesiredSensorMode->optimalWidth) /
                            static_cast<FLOAT>(pDesiredSensorMode->optimalHeight));

    aspectRatioDiff = ChxUtils::AbsoluteFLOAT(modeAspectRatio - requestedAspectRatio);

    pSelectedAspectRatio->tempAspectRatioDiff = aspectRatioDiff;

    CHX_LOG(" aspectRatioDiff %f returnValue %d", aspectRatioDiff, (aspectRatioDiff <= pSelectedAspectRatio->lastAspectRatioDiff));

    return ( aspectRatioDiff <= pSelectedAspectRatio->lastAspectRatioDiff );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingFrameRate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingFrameRate(
    const ChiSensorModeInfo*  pModeInfo,
    const DesiredSensorMode* pDesiredSensorMode)
{
    FLOAT modeFrameRate      = 0;
    FLOAT requestedFrameRate = 0;

    modeFrameRate      = static_cast<FLOAT>(pModeInfo->frameRate);
    requestedFrameRate = static_cast<FLOAT>(pDesiredSensorMode->frameRate);
    if (1 == pModeInfo->sensorModeCaps.u.SHDR)
    {
        return TRUE;
    }
    return ChxUtils::AbsoluteFLOAT(modeFrameRate - requestedFrameRate) < 0.5;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingCapability
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingCapability(
    const ChiSensorModeInfo* pModeInfo,
    const DesiredSensorMode* pDesiredSensorMode)
{
    BOOL   isFound = FALSE;

    if (pDesiredSensorMode->sensorModeCaps.value & pModeInfo->sensorModeCaps.value)
    {
        isFound = TRUE;
    }

    return isFound;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingBestResolution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingBestResolution(
    const ChiSensorModeInfo*    pModeInfo,
    const DesiredSensorMode*   pDesiredSensorMode,
    SensorSelectBestResolution* pSelectedResolution)
{
    CHX_ASSERT(StreamImage == pModeInfo->streamtype);

    UINT32 modeResolution = pModeInfo->frameDimension.width * pModeInfo->frameDimension.height;

    UINT32 expectedResolution = pDesiredSensorMode->optimalWidth * pDesiredSensorMode->optimalHeight;

    pSelectedResolution->temporaryResolution = modeResolution;

    if (0 == pSelectedResolution->lastResolution)
    {
        pSelectedResolution->lastResolution = modeResolution;
    }

    BOOL returnValue = (modeResolution <= pSelectedResolution->lastResolution) && (modeResolution >= expectedResolution);

    CHX_LOG(" SelectResolution %d expectedRes %d returnValue %d", modeResolution, expectedResolution, returnValue);

    return returnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingMaxResolution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingMaxResolution(
    const ChiSensorModeInfo*    pModeInfo,
    SensorSelectBestResolution* pSelectedResolution)
{
    CHX_ASSERT(StreamImage == pModeInfo->streamtype);

    UINT32 selectResolution = pModeInfo->frameDimension.width * pModeInfo->frameDimension.height;

    pSelectedResolution->temporaryResolution = selectResolution;

    CHX_LOG(" selectResolution %d returnValue %d", selectResolution, (selectResolution >= pSelectedResolution->lastResolution));

    return ( selectResolution >= pSelectedResolution->lastResolution );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingHFR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingHFR(
    const ChiSensorModeInfo*  pModeInfo,
    const DesiredSensorMode* pDesiredSensorMode)
{
    FLOAT modeFrameRate    = 0;
    FLOAT desiredFrameRate = 0;

    CHX_ASSERT(StreamImage == pModeInfo->streamtype);

    modeFrameRate    = static_cast<FLOAT>(pModeInfo->frameRate);
    desiredFrameRate = static_cast<FLOAT>(pDesiredSensorMode->frameRate);

    return ((modeFrameRate >= MinHFRFps) && (desiredFrameRate >= MinHFRFps) ? TRUE : FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingDefaultMode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingDefaultMode(
    const ChiSensorModeInfo* pModeInfo)
{
    BOOL   isFound = FALSE;

    CHX_ASSERT(StreamImage == pModeInfo->streamtype);

    if (1 == pModeInfo->sensorModeCaps.u.Normal)
    {
        isFound = TRUE;
    }

    CHX_LOG(" IsMatching Default %d", isFound);

    return isFound;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingRHDRMode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingRHDRMode(
    const ChiSensorModeInfo* pModeInfo)
{
    BOOL   isFound = FALSE;

    CHX_ASSERT(StreamImage == pModeInfo->streamtype);

    if ((1 == pModeInfo->sensorModeCaps.u.ZZHDR) || (1 == pModeInfo->sensorModeCaps.u.SHDR))
    {
        isFound = TRUE;
    }

    CHX_LOG(" IsMatching RHDR %d", isFound);

    return isFound;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::IsMatchingIHDRMode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChxSensorModeSelect::IsMatchingIHDRMode(
    const ChiSensorModeInfo* pModeInfo)
{
    BOOL   isFound                   = FALSE;

    CHX_ASSERT(StreamImage == pModeInfo->streamtype);

    if (1 == pModeInfo->sensorModeCaps.u.IHDR)
    {
        isFound = TRUE;
    }

    CHX_LOG(" IsMatching IHDR %d", isFound);

    return isFound;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::SelectUseCase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SensorModeSelectUseCase ChxSensorModeSelect::SelectUseCase(
    UINT32                   cameraId,
    const DesiredSensorMode* pDesiredSensorMode)
{
    SensorModeSelectUseCase usecase;
    BOOL                    selectIHDRUsecase   = ExtensionModule::GetInstance()->SelectIHDRUsecase();
    UINT32                  modeCount           = 0;
    ChiSensorModeInfo*      pAllModes           = NULL;
    BOOL                    isSensorSupportIHDR = FALSE;
    CDKResult               result              =
        ExtensionModule::GetInstance()->GetAllSensorModes(cameraId, &modeCount, &pAllModes);

    if (CDKResultSuccess == result)
    {
        for (UINT32 i = modeCount; i > 0; --i)
        {
            if (1 == pAllModes[i - 1].sensorModeCaps.u.IHDR)
            {
                isSensorSupportIHDR = TRUE;
                break;
            }
        }
    }

    if ((pDesiredSensorMode->frameRate >= MinHFRFps) || (1 == pDesiredSensorMode->sensorModeCaps.u.HFR))
    {
        usecase = UsecaseHFR;
    }
    else if (1 == pDesiredSensorMode->sensorModeCaps.u.ZZHDR)
    {
        usecase = UsecaseRHDR;
    }
    else if (TRUE == selectIHDRUsecase && TRUE == isSensorSupportIHDR)
    {
        usecase = UsecaseIHDR;
    }
    else
    {
        /// @todo Reference implementation to be filled with customized usecase selection
        usecase = UsecaseVideoPreview;
    }

    CHX_LOG("Selected usecase = %d", usecase);

    return usecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChxSensorModeSelect::GetModeIndex
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiSensorModeInfo* ChxSensorModeSelect::FindBestSensorMode(
    UINT32                   cameraId,
    const DesiredSensorMode* pDesiredSensorMode)
{
    BOOL                        isFound             = FALSE;
    CDKResult                   result              = CDKResultSuccess;
    ChiSensorModeInfo*          pSelectedMode       = NULL;
    UINT32                      modeCount           = 0;
    ChiSensorModeInfo*          pAllModes           = NULL;
    SensorSelectBestResolution  selectedResolution  = {0};
    SensorSelectBestAspectRatio selectedAspectRatio = {10};

    result        = ExtensionModule::GetInstance()->GetAllSensorModes(cameraId, &modeCount, &pAllModes);
    pSelectedMode = &pAllModes[0]; // Initialize with default mode

    if ((CDKResultSuccess == result) &&
        (NULL != pDesiredSensorMode) &&
        (pDesiredSensorMode->forceMode >= modeCount))
    {
        CHX_LOG("Desired sensor mode:");
        CHX_LOG("    optimalWidth: %d", pDesiredSensorMode->optimalWidth);
        CHX_LOG("    optimalHeight: %d", pDesiredSensorMode->optimalHeight);
        CHX_LOG("    maxWidth: %d", pDesiredSensorMode->maxWidth);
        CHX_LOG("    maxHeight: %d", pDesiredSensorMode->maxHeight);
        CHX_LOG("    minWidth: %d", pDesiredSensorMode->minWidth);
        CHX_LOG("    minHeight: %d", pDesiredSensorMode->minHeight);
        CHX_LOG("    frameRate: %f", pDesiredSensorMode->frameRate);
        CHX_LOG("    sensorModeCaps: %08x", pDesiredSensorMode->sensorModeCaps.value);
        CHX_LOG("Available sensor modes:");
        for (UINT32 i = 0; i < modeCount; i++)
        {
            CHX_LOG("    ModeInfo[%d]:", i);
            CHX_LOG("        modeIndex: %d", pAllModes[i].modeIndex);
            CHX_LOG("        arraySizeInMPix: %d", pAllModes[i].arraySizeInMPix);
            CHX_LOG("        frameDimension: TL:(%d, %d)  width: %d height: %d",
                    pAllModes[i].frameDimension.left, pAllModes[i].frameDimension.top,
                    pAllModes[i].frameDimension.width, pAllModes[i].frameDimension.height);
            CHX_LOG("        cropInfo: TL:(%d, %d)  width: %d height: %d",
                    pAllModes[i].cropInfo.left, pAllModes[i].cropInfo.top,
                    pAllModes[i].cropInfo.width, pAllModes[i].cropInfo.height);
            CHX_LOG("        aspectRatio: %d / %d",
                    pAllModes[i].aspectRatio.numerator, pAllModes[i].aspectRatio.denominator);
            CHX_LOG("        bpp: %d", pAllModes[i].bpp);
            CHX_LOG("        framerate: %d", pAllModes[i].frameRate);
            CHX_LOG("        batchedFrames: %d", pAllModes[i].batchedFrames);
            CHX_LOG("        caps: %08x", pAllModes[i].sensorModeCaps.value);
            CHX_LOG("        streamType: %08x", pAllModes[i].streamtype);
        }


        SensorModeSelectUseCase useCase = SelectUseCase(cameraId, pDesiredSensorMode);

        for (UINT32 i = modeCount; i > 0; --i)
        {
            UINT32 modeIndex = (i - 1);

            /// Go through all the usecases in the select table, if any of the selected usecase fails then use default res
            for (UINT32 condition = 0; condition < MaxModeSelectCondition; condition++)
            {
                if (TRUE == ModeSelectTable[condition][useCase])
                {
                    SensorModeSelectCondition selectCondition = static_cast<SensorModeSelectCondition>(condition);
                    CHX_LOG("Select sensor mode condition = %d", selectCondition);

                    switch (selectCondition)
                    {
                        case ConditionFrameRate:
                            isFound = ChxSensorModeSelect::IsMatchingFrameRate(&pAllModes[modeIndex], pDesiredSensorMode);
                            break;

                        case ConditionAspectRatio:
                            isFound = ChxSensorModeSelect::IsMatchingAspectRatio(&pAllModes[modeIndex],
                                                                                 pDesiredSensorMode,
                                                                                 &selectedAspectRatio);
                            break;

                        case ConditionWidth:
                            isFound = ChxSensorModeSelect::IsMatchingWidth(&pAllModes[modeIndex], pDesiredSensorMode);
                            break;

                        case ConditionHeight:
                            isFound = ChxSensorModeSelect::IsMatchingHeight(&pAllModes[modeIndex], pDesiredSensorMode);
                            break;

                        case ConditionMPIX:
                            isFound = ChxSensorModeSelect::IsMatchingMaxResolution(&pAllModes[modeIndex], &selectedResolution);
                            break;

                        case ConditionHFR:
                            isFound = ChxSensorModeSelect::IsMatchingHFR(&pAllModes[modeIndex], pDesiredSensorMode);
                            break;

                        case ConditionDEF:
                            isFound = ChxSensorModeSelect::IsMatchingDefaultMode(&pAllModes[modeIndex]);
                            break;

                        case ConditionRHDR:
                            isFound = ChxSensorModeSelect::IsMatchingRHDRMode(&pAllModes[modeIndex]);
                            break;

                        case ConditionIHDR:
                            isFound = ChxSensorModeSelect::IsMatchingIHDRMode(&pAllModes[modeIndex]);
                            break;

                        case ConditionBestResolution:
                            if (FALSE == ModeSelectTable[ConditionMPIX][useCase])
                            {
                                isFound = ChxSensorModeSelect::IsMatchingBestResolution(&pAllModes[modeIndex],
                                                                                        pDesiredSensorMode,
                                                                                        &selectedResolution);
                            }
                            break;

                        default:
                            CHX_LOG("Condition %d No implemented", selectCondition);
                            break;
                    }

                    if (FALSE == isFound)
                    {
                        CHX_LOG("Condition %d doesn't match", selectCondition);
                        break;
                    }
                }
            }

            /// If isFound is still TRUE at the end , then we select this mode
            if (TRUE == isFound)
            {
                pSelectedMode                           = &pAllModes[modeIndex];
                selectedResolution.lastResolution       = selectedResolution.temporaryResolution;
                selectedAspectRatio.lastAspectRatioDiff = selectedAspectRatio.tempAspectRatioDiff;

                CHX_LOG("Selecting mode %d", modeIndex);
            }
        }
    }
    else
    {
        if ((NULL != pDesiredSensorMode) && (pDesiredSensorMode->forceMode < modeCount))
        {
            pSelectedMode = &pAllModes[pDesiredSensorMode->forceMode];
            CHX_LOG_ERROR("***FORCING SENSOR MODE %d  -  for debug only", pDesiredSensorMode->forceMode);
        }
        else
        {
            ALOGE("Desired Sensor Mode is NULL pointer");
            result = CDKResultEInvalidPointer;
        }
    }

    CHX_LOG("SelectedMode W=%d, H=%d, FPS:%d, NumBatchedFrames:%d, modeIndex:%d",
            pSelectedMode->frameDimension.width, pSelectedMode->frameDimension.height, pSelectedMode->frameRate,
            pSelectedMode->batchedFrames, pSelectedMode->modeIndex);

    return pSelectedMode;
}
