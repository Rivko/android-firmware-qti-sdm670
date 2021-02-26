////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxextensionmodule.h
/// @brief CHX Extension Module
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXEXTENSIONMODULE_H
#define CHXEXTENSIONMODULE_H

#include <assert.h>

#include "camxdefs.h"
#include "chi.h"
#include "chioverride.h"
#include "chxdefs.h"
#include "camxcdktypes.h"
#include "chxutils.h"
#include <cutils/log.h>
#include <stdio.h>
#include "chxperf.h"

#undef LOG_TAG
#define LOG_TAG "CHIUSECASE"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Forward Declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Mutex;
class Session;
class UsecaseFactory;
class UsecaseSelector;
class Usecase;
class PerfLockManager;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Constant Declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const UINT MaxSessions            = 8;       ///< Max sessions
static const UINT MaxSensorModes         = 50;      ///< Max number of sensor modes
static const UINT MaxHFRConfigs          = 64;      ///< Max number of HFR configurations
static const UINT MaxCameras             = 8;       ///< Max number of cameras supported
static const UINT32 MaxNumCameraMap      = 2;       ///< Maximum number of related cameras

/// @brief Pipeline create data
struct PipelineCreateData
{
    CHIPIPELINECREATEDESCRIPTOR*  pPipelineCreateDescriptor;         ///< Pipeline create descriptor
    UINT32                        numOutputs;                        ///< Number of output buffers of this pipeline
    CHIPORTBUFFERDESCRIPTOR*      pOutputDescriptors;                ///< Output buffer descriptors
    UINT32                        numInputs;                         ///< Number of inputs
    CHIPIPELINEINPUTOPTIONS*      pInputOptions;                     ///< Input buffer requirements for this pipeline
};

/// @brief Create session data
struct SessionCreateData
{
    UINT32                          numPipelines;                   ///< Number of pipelines in this session
    CHIPIPELINEINFO*                pPipelineInfo;                  ///< Pipeline info
    CHICALLBACKS*                   pCallbacks;                     ///< Callbacks
    VOID*                           pPrivateCallbackData;           ///< Private callbacks
    CHISESSIONFLAGS                 flags;                          ///< Flags
};

/// @brief Submit request data
struct SubmitRequestData
{
    UINT64                hSession;         ///< Session handle
    UINT64                hPipeline;        ///< Pipeline handle
    UINT32                numPipelines;     ///< Number of pipelines
    CHIPIPELINEREQUEST*   pChiRequest;      ///< Capture request descriptor
};

/// @bried Camera info
struct DeviceInfo
{
    UINT32                       cameraId;                    ///< Physical device cameraID
    const CHICAMERAINFO          *m_pDeviceCaps;              ///< device capability.
    const CHISENSORMODEINFO      *pSensorModeInfo;            ///< ensor mode table array.
};

/// @brief Logical Camera info
struct LogicalCameraInfo
{
    UINT32                   cameraId;                        ///< Logical cameraID
    camera_info              m_cameraInfo;                    ///< Application data
    CHICAMERAINFO            m_cameraCaps;                    ///< camera sub device capability
    CHISENSORMODEINFO*       pSensorModeInfo;                 ///< sensor mode table array.
    UINT32                   numPhysicalCameras;              ///< Number of physical device attached to this logical
                                                              /// camera
    DeviceInfo**             ppDeviceInfo;                    /// Array of physical device info related to this logical camera
    const camera3_device_t*  m_pCamera3Device;                ///< Camera3 device pointer from application.
};

/// @brief Vendor Tags
struct VendorTagInfo
{
    const CHAR*  pSectionName;                ///< Vendor Tag Section Name
    const CHAR*  pTagName;                    ///< Vendor Tag Name
    UINT32       tagId;                       ///< Vendor Tag Id used to query
};

/// @brief HFR configuration
struct HFRConfigurationParams
{
    INT32 width;                ///< Width
    INT32 height;               ///< Height
    INT32 minFPS;               ///< minimum preview FPS
    INT32 maxFPS;               ///< maximum video FPS
    INT32 batchSizeMax;         ///< maximum batch size
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief The ExtensionModule class provides entry/landing implementation of CHX
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ExtensionModule
{
public:

    /// Return the singleton instance of ExtensionModule
    static ExtensionModule* GetInstance();

    const LogicalCameraInfo* GetCameraInfo(
        uint32_t logicalCameraId
        ) const
    {
        return &m_logicalCameraInfo[logicalCameraId];
    }

    UINT32 GetVendorTagId(
        VendorTag tag
        ) const
    {
        return m_pvtVendorTags[tag].tagId;
    }

    /// Get sensor mode info
    CDKResult GetAllSensorModes(
        UINT32              cameraId,               ///< Camera Id
        UINT32*             pNumSensorModes,        ///< Number of sensor modes
        CHISENSORMODEINFO** ppAllSensorModes);      ///< Sensor mode list

    /// Create a pipeline
    CHIPIPELINEDESCRIPTOR CreatePipelineDescriptor(
        PipelineCreateData* pPipelineCreateData);   ///< Pipeline create data

    /// Destroy a pipeline
    VOID DestroyPipelineDescriptor(
        CHIPIPELINEDESCRIPTOR pipelineHandle);       ///< Pipeline handle

    /// Create a session
    CHIHANDLE CreateSession(
        SessionCreateData* pSessionCreateData);     ///< Session create data

    /// Destroy a session
    VOID DestroySession(
        CHIHANDLE sessionHandle,
        BOOL isForced);                             ///< Pipeline handle

    /// Flush a session
    VOID Flush(
        CHIHANDLE sessionHandle);                   ///< Pipeline handle

    /// Forced flush during recovery
    VOID ForcedFlush(
            CHIHANDLE sessionHandle);              ///< Pipeline handle

    /// Submit request
    CDKResult SubmitRequest(
        CHIPIPELINEREQUEST* pSubmitRequestData);     ///< Submit request data

    /// Activate pipeline in a session
    CDKResult ActivatePipeline(
        CHIHANDLE             sessionHandle,        ///< Session handle
        CHIPIPELINEDESCRIPTOR pipelineHandle);      ///< Pipeline handle

    /// Activate pipeline in a session
    CDKResult DeactivatePipeline(
        CHIHANDLE                 sessionHandle,        ///< Session handle
        CHIPIPELINEDESCRIPTOR     pipelineHandle,       ///< Pipeline handle
        CHIDEACTIVATEPIPELINEMODE mode);                ///< Deactivate pipeline mode

    CDKResult EnumerateSensorModes(
        UINT32             physCameraId,
        UINT32             numSensorModes,
        CHISENSORMODEINFO* pSensorModeInfo);         ///< Enumerate sensor modes

    /// Interface function invoked by the CHI driver to give an opportunity to the override module to take control of a
    /// usecase on configure streams

    /// @brief Called by the driver to allow for override to detect special camera IDs for additional processing
    UINT32 RemapCameraId(
        UINT32              frameworkCameraId,
        CameraIdRemapMode   mode);

    /// @brief Called by the driver to allow for additional override processing during open()
    VOID ExtendOpen(
        uint32_t    logicalCameraId,        ///< Logical camera ID.
        VOID*       pPriv);                 ///< private data for the client to make use of during HAL open

                                            /// @brief Called by the driver to allow for additional override processing during open()
    VOID ExtendClose(
        uint32_t    logicalCameraId,        ///< Logical camera ID.
        VOID*       pPriv);                 ///< private data for the client to make use of during HAL open

    VOID GetNumCameras(
        UINT32* pNumFwCameras,              ///< return the number of cameras to expose to the framework
        UINT32* pNumLogicalCameras);        ///< return the number of logical cameras created by the override

    CDKResult GetCameraInfo(
        uint32_t     logicalCcameraId,  ///< Camera Id
        camera_info* cameraInfo);       ///< Camera Info

    CDKResult InitializeOverrideSession(
        uint32_t                        logicalCameraId,    ///< Camera Id
        const camera3_device_t*         camera3_device,     ///< Camera device
        const chi_hal_ops_t*            chiHalOps,          ///< Callbacks into the HAL driver
        camera3_stream_configuration_t* pStreamConfig,      ///< Stream config
        bool*                           pIsOverride,        ///< TRUE to take control of the usecase
        VOID**                          pPriv);             ///< Private data

    /// Interface function invoked by the CHI driver to allow the override module to finalize session creation
    CDKResult FinalizeOverrideSession(
        const camera3_device_t* camera3_device, ///< Camera device
        VOID*                   pPriv);         ///< Private data

    /// Interface function invoked by the CHI driver to destroy/teardown a session
    VOID TeardownOverrideSession(
        const camera3_device_t* camera3_device, ///< Camera device
        UINT64                  session,        ///< Session Handle
        VOID*                   pPriv);         ///< Private data

    /// Interface function invoked by the CHI driver to pass along the process capture request to the override module
    CDKResult OverrideProcessRequest(
        const camera3_device_t*     camera3_device,     ///< Camera device
        camera3_capture_request_t*  pCaptureRequest,    ///< Capture request
        VOID*                       pPriv);             ///< Private data


    VOID HandleProcessRequestErrorAllPCRs(
        camera3_capture_request_t* pRequest,    ///< Framework request
        UINT32 logicalCameraId);                ///< Logical camera id


    VOID TeardownOverrideUsecase(
        const camera3_device_t* camera3_device);
    /// Interface function invoked by the CHI driver to flush
    CDKResult OverrideFlush(
        const camera3_device_t*     camera3_device);     ///< Camera device

    /// @brief Allows implementation-specific settings to be toggled in the override at runtime
    VOID ModifySettings(
        VOID*       pPriv);                 ///< private data for the client to make use of at runtime in the override

    /// @brief Add any vendor tag specific request template settings
    VOID DefaultRequestSettings(
        uint32_t                  cameraId,
        int                       requestTemplate,
        const camera_metadata_t** settings);

    /// Sets the HAL callbacks
    VOID SetHALOps(
        const chi_hal_ops_t* pHalOps,UINT32 logicalCameraId);                  ///< Callback functions

    /// Sets the CHI context ops from the CHI driver
    VOID SetCHIContextOps();

    const chi_hal_ops_t* GetHalOps(UINT32 logicalCameraId) const
    {
        return m_pHALOps[logicalCameraId];
    }

    /// Setting to force-disable ZSL...otherwise on by default
    BOOL DisableZSL() const
    {
        return *m_pDisableZSL;
    }

    /// Setting to select IHDR usecase supporting in-sensor HDR...otherwise on by default
    BOOL SelectIHDRUsecase() const
    {
        return *m_pSelectIHDRUsecase;
    }

    /// Setting to override the use case by user
    UINT OverrideUseCase() const
    {
        return *m_pForceUsecaseId;
    }

    /// Setting to force-use GPU Rotation Node
    BOOL UseGPURotationUsecase() const
    {
        return *m_pGPUNodeRotateUsecase;
    }

    /// Setting to force-use GPU Downscale Node
    BOOL UseGPUDownscaleUsecase() const
    {
        return *m_pGPUNodeDownscaleUsecase;
    }

    /// Setting to Enable MFNR Usecase
    BOOL EnableMFNRUsecase() const
    {
        return *m_pEnableMFNRUsecase;
    }

    /// Setting to Enable MFNR Anchor Selection Algorithm
    UINT32 EnableMFNRAnchorSelectionAlgorithm() const
    {
        return *m_pAnchorSelectionAlgoForMFNR;
    }

    /// Setting to Enable MFNR Usecase
    BOOL EnableHFRNo3AUsecas() const
    {
        return *m_pHFRNo3AUsecase;
    }

    UINT32 GetForceSensorMode() const
    {
        return *m_pForceSensorMode;
    }

    BOOL GetDCVRMode() const
    {
         return *m_pEnableMultiVRCamera;
    }
    BOOL GetIdealRawEnable() const
    {
        return *m_pIdealRawEnable;
    }

    UINT32 GetShdrModeType() const
    {
        return *m_pSHDRModeType;
    }

    BOOL GetLDCEnable() const
    {
        return *m_pLDCEnable;
    }

    BOOL GetEISDGEnable() const
    {
        return *m_pEISDGEnable;
    }

    UINT32 GetStatsSkipPattern() const
    {
        return *m_pStatsSkipPattern;
    }

    UINT32 EnableDumpDebugData() const
    {
        return *m_pEnableDumpDebugData;
    }

    /// Setting to Enable EIS V2 Usecase
    BOOL EnableEISV2Usecase() const
    {
        return *m_pEISV2Enable;
    }

    /// Setting to Enable EIS V3 Usecase
    BOOL EnableEISV3Usecase() const
    {
        return *m_pEISV3Enable;
    }

    UINT32 GetAdvanceFeatureMask() const
    {
        return *m_pAdvanceFeataureMask;
    }

    /// Setting to Disable ASD Processing
    UINT32 DisableASDProcessing() const
    {
        return *m_pDisableASDProcessing;
    }

    BOOL GetDCSATMode() const
    {
        return *m_pEnableMultiCameraSAT;
    }

    UINT32 GetDCFrameSyncMode() const
    {
        return *m_pEnableMultiCameraFrameSync;
    }

    UINT32 GetOutputFormat() const
    {
        return *m_pOutputFormat;
    }

    UINT32 IsCameraClose() const
    {
        return *m_pOverrideCameraClose;
    }

    UINT32 IsCameraOpen() const
    {
        return *m_pOverrideCameraOpen;
    }

    // Get number of PCRs before stream on
    UINT32 GetNumPCRsBeforeStreamOn(VOID* pMetaData) const;


    /// Setting to override the FOVC Usecase settings
    UINT EnableFOVCUseCase() const
    {
        return *m_pEnableFOVC;
    }

    VOID GetVendorTagOps(
        CHITAGSOPS* pVendorTagOps);

    VOID SearchNumBatchedFrames(
        uint32_t                        cameraId,
        camera3_stream_configuration_t* pStreamConfigs,
        UINT*                           pBatchSize,
        UINT*                           pFPSValue);

    struct CameraDeviceInfo
    {
        const camera3_device_t *m_pCamera3Device; ///< Camera3 device
    };


    Usecase* GetSelectedUsecase(const camera3_device*);
    UINT GetUsecaseMaxFPS()
    {
        return m_usecaseMaxFPS;
    }

    UINT GetVideoHDRMode()
    {
        return m_VideoHDRMode;
    }

    UINT GetVideoHDRModeType()
    {
        return m_VideoHDRModeType;
    }

    UINT GetNumBatchedFrames()
    {
        return m_usecaseNumBatchedFrames;
    }

    VOID ReturnFrameworkResult(
        const camera3_capture_result_t* pResult, UINT32 cameraID);

    VOID ReturnFrameworkMessage(
        const camera3_notify_msg_t *msg, UINT32 cameraID);

    UINT32 GetCameraIdfromDevice(const camera3_device_t * pCamera3Device);

    UINT IsTorchWidgetUsecase()
    {
        return m_torchWidgetUsecase;
    }

    UINT32 GetNumMetadataResults()
    {
        return m_numMetadataResults;
    }

    OSLIBRARYHANDLE GetPerfLibHandle()
    {
        return m_perfLibHandle;
    }

private:

    VOID SortCameras();
    CDKResult EnumerateCameras();

    /// Constructor
    ExtensionModule();
    /// Destructor
    ~ExtensionModule();

    /// Do not support the copy constructor or assignment operator
    ExtensionModule(const ExtensionModule& rExtensionModule) = delete;
    ExtensionModule& operator= (const ExtensionModule& rExtensionModule) = delete;
    VOID FreeLastKnownRequestSetting();


    CHIHANDLE               m_hCHIContext;                  ///< CHI context handle
    const chi_hal_ops_t*    m_pHALOps[MaxNumImageSensors];                      ///< HAL ops
    UsecaseSelector*        m_pUsecaseSelector;             ///< Usecase selector
    UsecaseFactory*         m_pUsecaseFactory;              ///< Usecase factory
    Usecase*                m_pSelectedUsecase[MaxNumImageSensors]; ///< Selected usecase
    UINT32                  m_numPhysicalCameras;           ///< Number of physical camera sensors in the system
    UINT32                  m_numLogicalCameras;            ///< Number of cameras exposed to an application
    LogicalCameraInfo       m_logicalCameraInfo[MaxNumImageSensors];
    UINT32                  m_cameraMap[MaxNumImageSensors];
    UINT32*                 m_pConfigSettings;              ///< Configuration settings

    UINT32*                 m_pDisableZSL;
    UINT32*                 m_pForceUsecaseId;
    UINT32*                 m_pEnableFOVC;
    UINT32*                 m_pGPUNodeRotateUsecase;            ///< Select GPU Node Rotate usecase
    UINT32*                 m_pGPUNodeDownscaleUsecase;         ///< Select GPU Node Downscale usecase
    UINT32*                 m_pEnableMFNRUsecase;               ///< Select MFNR usecase
    UINT32*                 m_pAnchorSelectionAlgoForMFNR;      ///< Select MFNR Anchor Selection Algorithm
    UINT32*                 m_pHFRNo3AUsecase;                  ///< Select HFR without 3A usecase
    UINT32*                 m_pForceSensorMode;                 ///< Select a specific sensor mode
    UINT32*                 m_pEISV2Enable;                     ///< Enable EIS V2
    UINT32*                 m_pEISV3Enable;                     ///< Enable EIS V3
    UINT32*                 m_pDisableASDProcessing;            ///< Disable ASD processing
    UINT32*                 m_pEnableMultiVRCamera;             ///< Enbale VR DC
    UINT32*                 m_pIdealRawEnable;                  ///< Enable Ideal Raw
    UINT32*                 m_pLDCEnable;                       ///< Enable LDC (Lens Distortion Correction)
    UINT32*                 m_pEISDGEnable;                     ///< Enable EIS DG
    UINT32*                 m_pSHDRModeType;                    ///< VideoMode SHDR Algo Type
    UINT32*                 m_pOverrideCameraClose;             ///< Camera close indicator
    UINT32*                 m_pOverrideCameraOpen;              ///< Camera open indicator
    UINT32*                 m_pNumPCRsBeforeStreamOn;           ///< Number of PCRs before stream on
    UINT32*                 m_pStatsSkipPattern;                ///< Stats skip pattern
    UINT32*                 m_pEnableMultiCameraSAT;            ///< DC SAT enabled or not
    UINT32*                 m_pEnableMultiCameraFrameSync;      ///< DC Frame sync enabled or not
    UINT32*                 m_pEnableDumpDebugData;             ///< Dump debug-data into a file, enable or not
    UINT32*                 m_pOutputFormat;                    ///< Output format for IMPL_Defined
    UINT32*                 m_pSelectIHDRUsecase;               ///< Select IHDR usecase
    VendorTagInfo           m_pvtVendorTags[NumVendorTags];     ///< List of private vendor tags
    static const UINT       DefaultSettingsNumEntries = 32;     ///< Num of entries
    static const UINT       DefaultSettingsDataSize   = 512;    ///< Data size bytes
    camera_metadata_t*      m_pDefaultSettings;                 ///< Default settings
    UINT*                   m_pDefaultMaxFPS;                   ///< Default MaxFPS
    UINT*                   m_pAdvanceFeataureMask;             ///< Advance Features Mask
    UINT                    m_usecaseMaxFPS;                    ///< Max FPS required for high speed mode
    UINT                    m_VideoHDRMode;                     ///< video HDR mode
    UINT                    m_VideoHDRModeType;                 ///< video HDR mode
    UINT                    m_usecaseNumBatchedFrames;          ///< Number of framework frames batched together if
                                                                ///  batching is enabled
    BOOL                    m_torchWidgetUsecase;               ///< Indicate torch widget usecase.

    PerfLockManager*        m_pPerfLockManager[MaxNumImageSensors];                 ///< PerfLock Manager
    PerfLockType            m_CurrentpowerHint;                 ///< Current power Hint
    PerfLockType            m_previousPowerHint;                ///< Previous power Hint
    UINT                    m_numMetadataResults;               ///< number of metadata results expected from the driver

    UINT32                  m_originalFrameWorkNumber;          ///< Original framework number
    Mutex*                  m_pPCRLock[MaxNumImageSensors];     ///< Lock for process capture request
    BOOL                    m_isFlushLocked;                    ///< Is Flush executing with PCR lock

    camera_metadata_t*      m_pLastKnownRequestSettings;        ///< Save last known metadata to send aftet recovery
    UINT32                  m_firstFrameAfterRecovery;          ///< First frame after recovery to send settings
    UINT32                  m_longExposureFrame;                ///< Long exposure snapshot frameNumber
    volatile UINT32         m_aFlushInProgress[MaxNumImageSensors];                 ///< Is flush in progress
    volatile UINT32         m_aLongExposureInProgress;          ///< Is long exposure in progress
    const camera3_device_t* m_pCamera3Device;                   ///< Camera3 device pointer from application.

    camera3_stream_configuration_t* m_pStreamConfig[MaxNumImageSensors];        ///< Stream configuration array to use in recovery
    UINT32                          m_SelectedUsecaseId[MaxNumImageSensors];    ///< Selected usecase id to use in recoverys
    CameraDeviceInfo                m_pCameraDeviceInfo[MaxNumImageSensors];    ///< Device info for all logical camera Id
    BOOL                    m_firstResult;                      ///< First result after configure streams
    OSLIBRARYHANDLE         m_perfLibHandle;                    ///< PerfLock Library handle
    BOOL                    m_isUsecaseInBadState;              ///< Flag to indicate if recovery is required
    UINT32                  m_logicalCameraId;                  ///< Logical camera id for this session

};

#endif // CHXEXTENSIONMODULE_H
