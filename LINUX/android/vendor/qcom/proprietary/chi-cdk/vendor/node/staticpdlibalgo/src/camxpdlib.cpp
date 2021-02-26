////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxpdlibinterface.cpp
/// @brief Entry to PD library. Implements C style Algorithm interface.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chipdlibinterface.h"
#include "camxtuningdatamanager.h"
// #include "pdlib_api.h"
#include "camxmem.h"
#include "chistatsalgo.h"
#include <system/camera_metadata.h>


using namespace afpdlibelements;

CHIALGORITHMINTERFACE  g_ChiAlgoInterface;                              ///< The instance save the CAMX Chi interface
static UINT32          g_PDVendorTagBase    = 0;                        ///< Chi assigned runtime vendor tag base for PD

static const CHAR      PDLibSectionName[]   = "com.qtistatic.stats.pdlib";    ///< The section name for PD

// static const UINT32    PDLibBaseTagBase     = 0;                        ///< PD Vendor Tag base

/// @todo (CAMX-2931): Add vendor tag support to Pdlib
// static const UINT32    PDLibTag1            = PDLibBaseTagBase + 0;     ///< PD Vendor Tag 1
// static const UINT32    PDLibTag2            = PDLibBaseTagBase + 1;     ///< PD Vendor Tag 2

// static const FLOAT     PDLibTag1Value       = 12.43f;
// static const INT32     PDLibTag2Value       = 19;

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagDataPD[] =
{
    { "PDLibTag1", TYPE_DOUBLE,   1 },
    { "PDLibTag2", TYPE_INT32,    1 },
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagSectionDataPD[] =
{
    {
        PDLibSectionName,  0,
        sizeof(g_VendorTagDataPD) / sizeof(g_VendorTagDataPD[0]), g_VendorTagDataPD,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoPD[] =
{
    {
        &g_VendorTagSectionDataPD[0],
        sizeof(g_VendorTagSectionDataPD) / sizeof(g_VendorTagSectionDataPD[0])
    }
};

/// @brief Internal PD Library data
struct PDLibInternalData
{
    CHIPDLib    algoOps;        ///< PD Library operations that can be performed
    VOID*       pPDCoreLibrary; ///< PD Library handle
};

PDLibInternalData* pPDLibData = NULL;

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PDLibDestroy
///
/// @brief  This method destroys the derived instance of the interface
///
/// @param                  pPDLib Pointer to PD Library instance
/// @param  pDestroyParams  Pointer to destroy Param List
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID PDLibDestroy(
    CHIPDLib*                       pPDLib,
    const PDLibDestroyParamList*    pDestroyParams)
{
    CAMX_UNREFERENCED_PARAM(pPDLib);

    UINT* pOverrideCameraClose = NULL;

    for (UINT32 i = 0; i < pDestroyParams->paramCount; i++)
    {
        if (PDLibDestroyParamTypeCameraCloseIndicator == pDestroyParams->pParamList[i].destroyParamType)
        {
            if ((pDestroyParams->pParamList[i].pParam != NULL) && (pDestroyParams->pParamList[i].sizeOfParam >= sizeof(UINT)))
            {
                pOverrideCameraClose = static_cast<UINT*>(pDestroyParams->pParamList[i].pParam);
                CAMX_LOG_INFO(CamxLogGroupStats, "PDLibDestroy overrideCameraClose is %d pPDLibData %p",
                    *pOverrideCameraClose, pPDLibData);
                break;
            }
        }
    }

    if ((NULL != pOverrideCameraClose) && (1 == *pOverrideCameraClose))
    {
        if (NULL != pPDLibData)
        {
            CAMX_LOG_INFO(CamxLogGroupStats, "PDLib pPDLibData in destroy is %p", pPDLibData);
            // PDLibDeinit(pPDLibData->pPDCoreLibrary);
            CAMX_DELETE(pPDLibData);
            pPDLibData = NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PDLibProcess
///
/// @brief  Runs the pd library on the given inputs. This method must only be called after the library is
///         initialized.
///
/// @param  pPDLib  Pointer to PD library instance
/// @param  pInput  Pointer to stats inputs interface
/// @param  pOutput Pointer to the stats data output by the algorithm
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PDLibProcess(
    CHIPDLib*           pPDLib,
    const PDLibInputs*  pInput,
    PDLibOutputs*       pOutput)
{
    // PDLibInternalData*      pPDLibData      = (PDLibInternalData*) pPDLib;
    PDLibReturnStatus       PDresult        = PDLibSuccess;
    CDKResult               result          = CDKResultSuccess;
    // PDLibParam              processParam    = pInput->processParam;

    if (NULL == pPDLib || NULL == pInput || NULL == pOutput)
    {
        return CDKResultEInvalidPointer;
    }

    // PDresult = PDLibPhaseDetection(pPDLibData->pPDCoreLibrary, &processParam, pInput->numOfWrittenPDlines, &pOutput->libOutput);

    CAMX_LOG_INFO(CamxLogGroupStats, "buffer %p pdaf PDresult %d defocus %d conf %d pd %f processingResult %d",
        pInput->processParam.pPDBuffer, PDresult, pOutput->libOutput.defocus[0].defocus, pOutput->libOutput.defocus[0].confidence,
        pOutput->libOutput.defocus[0].phaseDiff, pOutput->libOutput.processingResult);


    if (PDLibSuccess != PDresult)
    {
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PDLibInitialize
///
/// @brief  This function pointer to method of initializing the CHIPDLibrary.
///
/// @param  pPDLib          Pointer to the pointer of the created CHIPDLib instance
/// @param  pCreateParam    Pointer to create parameters
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PDLibInitialize(
    CHIPDLib*                   pPDLib,
    const PDLibCreateParams*    pCreateParam)
{
    // PDLibInternalData*                  pPDLibData      = (PDLibInternalData*) pPDLib;
    PDLibReturnStatus                   PDresult        = PDLibSuccess;
    CDKResult                           result          = CDKResultSuccess;
    PDLibInitParam                      initParam;

    if (NULL == pPDLib)
    {
      return CDKResultEInvalidPointer;
    }

    if (NULL == pCreateParam )
    {
        return CDKResultEInvalidPointer;
    }

    ///@todo: (CAMX-2930) Hook up pdlib with tuning manager
#if ENABLE_TUNING_MANAGER_FOR_PDLIB

    if (NULL == pCreateParam->pTuningDataManager)
    {
        return CDKResultEInvalidPointer;
    }

    chromatixPDLibType* pPDLibTuningData    = NULL;
    TuningSetManager*   pManager            = NULL;
    TuningMode          mode = { ModeType::Default, { 0 } };

    pManager = static_cast <TuningSetManager*>(pCreateParam->pTuningDataManager);

    if (NULL != pManager)
    {
        pPDLibTuningData = pManager->GetModule_chromatixPDLib(&mode, 1);
    }

#endif

    chromatixPDLibType     tuningParam;
    Utils::Memset(&tuningParam, 0, sizeof(tuningParam));
    initParam = pCreateParam->initParam;
    // PDresult = PDLibInit(&pPDLibData->pPDCoreLibrary, &(initParam), &tuningParam);

    Utils::Memset(&tuningParam, 0, sizeof(tuningParam));
    initParam = pCreateParam->initParam;
    // PDresult = PDLibInit(&pPDLibData->pPDCoreLibrary, &(initParam), &tuningParam);

    if (PDLibSuccess != PDresult)
    {
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PDLibConfigure
///
/// @brief  Sets configuration parameters to CHIPDLib.
///
/// @param  pPDLib      Pointer to CHIPDLib instance
/// @param  pConfig     Configuration parameters to set to the Library.
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PDLibConfigure(
    CHIPDLib*                   pPDLib,
    const PDLibConfigParams*    pConfig)
{
    CAMX_UNREFERENCED_PARAM(pPDLib);
    CAMX_UNREFERENCED_PARAM(pConfig);

    return CDKResultSuccess;
}

CAMX_NAMESPACE_END

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreatePDLib
///
/// @brief  This method creates an instance to the PD Library.
///
/// @param  ppPDLib         Pointer to the created PD Library instance
/// @param  pCreateParams   Pointer to create params
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC CDKResult CreatePDLib(
    CHIPDLib**                  ppPDLib,
    const PDLibCreateParamList* pCreateParams)
{
    CDKResult   result              = CDKResultSuccess;
    UINT*       pOverrideCameraOpen = NULL;

    for (UINT32 i = 0; i < pCreateParams->paramCount; i++)
    {
        if (PDLibCreateParamTypeCameraOpenIndicator == pCreateParams->pParamList[i].createParamType)
        {
            if ((pCreateParams->pParamList[i].pParam != NULL) && (pCreateParams->pParamList[i].sizeOfParam >= sizeof(UINT)))
            {
                pOverrideCameraOpen = static_cast<UINT*>(pCreateParams->pParamList[i].pParam);
                CAMX_LOG_INFO(CamxLogGroupStats, "CreatePDLib static create: overrideCameraOpen is %d", *pOverrideCameraOpen);
                break;
            }
        }
    }

    if ((NULL != pOverrideCameraOpen) && (1 == (*pOverrideCameraOpen)))
    {
        if(NULL == pPDLibData)
        {
            CAMX_LOG_INFO(CamxLogGroupStats, "PDLibData creation");
            pPDLibData = CAMX_NEW PDLibInternalData();

            if (NULL == pPDLibData)
            {
                result = CDKResultENoMemory;
            }
            else
            {
                pPDLibData->algoOps.PDLibProcess    = CamX::PDLibProcess;
                pPDLibData->algoOps.PDLibConfigure  = CamX::PDLibConfigure;
                pPDLibData->algoOps.PDLibDestroy    = CamX::PDLibDestroy;
                pPDLibData->algoOps.PDLibInitialize = CamX::PDLibInitialize;
                *ppPDLib                            = &(pPDLibData->algoOps);
                result                              = CDKResultSuccess;
                CAMX_LOG_INFO(CamxLogGroupStats, "PDLibData first time creation %p", pPDLibData);
            }
        }
        else
        {
            CAMX_LOG_INFO(CamxLogGroupStats, "PDLibData has been created before %p", pPDLibData);
            *ppPDLib = reinterpret_cast<CHIPDLib*>(pPDLibData);
            result = CDKResultSuccess;
        }
    }
    else
    {
        CAMX_LOG_INFO(CamxLogGroupStats, "PDLib:Not actual open ...ignore:pOverrideCameraOpen %p", pOverrideCameraOpen);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PDGetCapabilities
///
/// @brief  This method get the current capabilities of PD Library.
///
/// @param  pCapsInfo Pointer to Algo capability structure
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDKResult PDGetCapabilities(CHIALGORITHMCAPSINFO* pCapsInfo)
{
    CDKResult   result = CDKResultSuccess;

    CAMX_UNREFERENCED_PARAM(pCapsInfo);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PDLibQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult PDLibQueryVendorTag(
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
            pQueryVendorTag->pVendorTagInfo = g_VendorTagInfoPD;
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
// PDLibSetNodeInterface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static VOID PDLibSetNodeInterface(
    ChiAlgorithmInterface* pAlgoInterface)
{
    // Currently we don't have any vendor tags in core algo.
    int result;
    g_ChiAlgoInterface = *pAlgoInterface;

    // get the PD vendor tag base
    CHIVENDORTAGBASEINFO vendorTagBase = { 0 };
    vendorTagBase.size = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName = PDLibSectionName;

    result = g_ChiAlgoInterface.pGetVendorTagBase(&vendorTagBase);
    if (CDKResultSuccess == result)
    {
        g_PDVendorTagBase = vendorTagBase.vendorTagBase;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiPDLibraryEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom PD Library.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC VOID ChiPDLibraryEntry (CHIPDLIBRARYCALLBACKS* pCallBacks)
{
    CAMX_LOG_INFO(CamxLogGroupStats, "ChiPDLibrarEntry");

    if (NULL == pCallBacks)
    {
        CAMX_LOG_ERROR(CamxLogGroupStats, "Invalid argument: pCallBacks is NULL");
    }
    else if (pCallBacks->size < sizeof(CHIPDLIBRARYCALLBACKS))
    {
        CAMX_LOG_ERROR(CamxLogGroupStats, "pCallBacks is smaller than expected");
    }
    else
    {
        pCallBacks->pfnGetCapabilities  = PDGetCapabilities;
        pCallBacks->pfnCreate           = CreatePDLib;
        pCallBacks->pfnQueryVendorTag   = PDLibQueryVendorTag;
        pCallBacks->pfnSetAlgoInterface = PDLibSetNodeInterface;
    }

}
#ifdef __cplusplus
}
#endif // __cplusplus
