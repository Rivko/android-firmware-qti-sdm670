////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxmulticamcontroller.h
/// @brief CHX Multi Cam Controller related class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXMULTICAMCONTROLLER_H
#define CHXMULTICAMCONTROLLER_H

#include "chi.h"
#include "chxdefs.h"
#include "chxutils.h"
#include "chivendortag.h"

/// Static constants
static const UINT32 DualCamCount = 2;
static const INT32  FocusDistanceCmMax = 1000;

static const FLOAT PercentMarginHysterisis     = 0.05f;
static const FLOAT PercentMarginTransitionZone = 0.20f;

/// Multi-cam controller settings
/// @todo: Move these to CHIOverride settings xml
/* This setting will enable the snapshot fusion.
If set to 1, it will enable capturing snapshots from both the cameras and feeding those
to snapshot fusion algorithm.
If set to 0, a snapshot is captured only from the master camera session at any time. */
#define ENABLE_SNAPSHOT_FUSION 1

/* This setting enables/disables Low Power Mode(LPM). The system is in power optimized state when
in LPM(1) where only one camera- the master camera is streaming. The other camera doesn't stream.
At the time of master role switch, both the cameras stream to switch the master and after
successful role switch, the non-master can be put into LPM.
When disabled(0), both the cameras will stream all the time. */
#define ENABLE_LPM             1

/*This setting indicates the mechanism used to sync the multiple cameras.
 Various sync mechanisms:
 NO_SYNC    : Absence of any sync mechanism between the two cameras.
 HW_SYNC    : Sensor turns on hw-sync.
 SW_SYNC    : Sensor ensures the phase difference is kept close to zero. (This
              mode is currently not supported)
 HYBRID_SYNC: Sensor turns on hw-sync and also injects phase as needed. */
#define SYNC_MECHANSIM         0

 /* This setting indicates low power modes for the two cameras
 Various low power modes:
 LPM_NONE             : No/Invalid mode
 LPM_SENSOR_SUSPEND   : Sensor sleep/suspend.
 LPM_ISPIF_FRAME_DROP : Sensor streams but ISPIF drops the frames.
 LPM_ISPIF_FRAME_SKIP : Sensor streams and ISPIF skips the frame to control frame rate. (This
 mode is currently not supported)
 LPM_STATS_FPS_CONTROL : Sensor and ISPIF stream but stats module controls the frame rate. */
//#define LPM_MAIN               (LPM_ISPIF_FRAME_DROP)
//#define LPM_AUX                (LPM_SENSOR_SUSPEND)

 /* This setting indicates the minimum zoom value for the snapshot fusion.
 Snapshot fusion is only enabled for zoom equal to and higher than this value. */
#define SNAPSHOT_FUSION_ZOOM_MIN           (1.5)

 /* This setting indicates the maximum zoom value for the snapshot fusion.
 Snapshot fusion is only enabled for zoom equal to and lower than this value. */
#define SNAPSHOT_FUSION_ZOOM_MAX           (1.9)

/* These values indicates the thresholds for the max lux index amd min focus distance in cm
 to enable / disable snapshot fusion. If the current lux index is lower than the threshold
 or if the focus distance is lower than the threshold, snapshot fusion will be disabled.
 The LUX index value will be lower for dark lit scene than bright scene. */
#define SNAPSHOT_FUSION_LUX_IDX_THRESHOLD  (1.0)

#define SNAPSHOT_FUSION_FOCUS_DIST_CM_MIN  (15)

/* This value indicates the threshhold view angle to determine 360 deg camera
 from normal camera. Achieving 360 full view each camera's view angle in usecase
 needs to be more than 180 deg */
#define CAMERA_VIEW_ANGLE_THRESHOLD  (180)

/// @brief Enum to indicate controller type
typedef enum multicam_controller_type {
    CONTROLLER_TYPE_NONE,               ///< Controller type none
    CONTROLLER_TYPE_DUALFOV,            ///< Controller type for dual fov
    CONTROLLER_TYPE_RTB,                ///< Controller type for RTB
    CONTROLLER_TYPE_BAYERMONO,          ///< Controller type for bayer-mono
    CONTROLLER_TYPE_VR                  ///< Controller Type for VR
} multicam_controller_type_t;

/// @brief Struct to hold dimensions
typedef struct dimension
{
    UINT32      width;                  ///< Width
    UINT32      height;                 ///< Height
} dimension_t;

/// @brief Struct to hold spatial alignment shifts
typedef struct pixel_shift
{
    INT32       xShift;                 ///< Horizontal shift
    INT32       yShift;                 ///< Vertical shift
} pixel_shift_t;

/// @brief Struct to hold camera info
typedef struct cam_info {
    UINT32                camId;                    ///< Camera Id
    dimension_t           sensorOutDimension;       ///< Sensor out dimension
    CHIRECT               fovRectIFE;               ///< IFE FOV wrt to active sensor array
    const CHICAMERAINFO*  pChiCamInfo;              ///< Pointer to ChiCameraInfo struct
} cam_info_t;

/// @brief Struct to hold stream info
typedef struct stream_info {
    UINT32               streamType;                ///< Stream type
    UINT32               usage;                     ///< Stream gralloc usage flag
    dimension_t          streamDimension;           ///< Stream dimension
} stream_info_t;

/// @brief Struct to hold stream configuration
typedef struct stream_config {
    UINT32                numStreams;               ///< Number of streams in the configuration
    stream_info_t*        pStreamInfo;              ///< Pointer to the stream info
} stream_config_t;

/// @brief Struct to hold the data to create multicamcontroller
typedef struct mcc_create_data {
    UINT32                logicalCameraId;                       ///< Logical camera Id
    UINT32                numBundledCameras;                     ///< Number of bundled cameras
    cam_info_t*           pBundledCamInfo[MaxNumImageSensors];   ///< Array of pointers for bundled camera info
    stream_config_t*      pStreamConfig;                         ///< Pointer to stream config info
    UINT32                desiredControllerMode;                 ///< Desired controller mode
} mcc_create_data_t;

/// @brief Struct for result metadata for multiple cameras
typedef struct multicam_result_metadata {
    UINT32       frameNum;                          ///< Current frame number
    UINT32       numResults;                        ///< Number of results
    VOID**       ppResultMetadata;                  ///< Pointer of pointers for result metadata
} multicam_result_metadata_t;

/// @brief Struct for active camera info
typedef struct active_cameras
{
    UINT32      cameraId;                           ///< Camera Id
    BOOL        isActive;                           ///< Boolean indicating if the camera is active
} active_cameras_t;

/// @brief Struct to hold controller result
typedef struct controller_result
{
    BOOL             isValid;                       ///< Boolean indicating if the result is valid
    BOOL             snapshotFusion;                ///< Boolean indicating if the snapshot fusion is enabled
    UINT32           masterCameraId;                ///< Camera Id for the master camera
    CameraRoleType   masterCameraRole;              ///< Camera role for the master camera
    active_cameras_t activeCameras[DualCamCount];   ///< Camera info indicating the LPM status
} controller_result_t;

/// @brief Struct for request settings for multiple cameras
typedef struct multicam_req_settings {
    UINT32                  frameNum;                           ///< Current frame number
    UINT32                  numSettingsBuffers;                 ///< Number of settings buffers
    controller_result_t     currentRequestMCCResult;            ///< MMC result of current request
    VOID**                  ppSettings;                         ///< Pointer of pointers for settings buffers
    BOOL                    kernelFrameSyncEnable;              ///< Frame sync enable
} multicam_req_settings;


class ZoomTranslator;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief MultiCamController abstract base class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MultiCamController
{
public:

    /// Consolidate the camera info
    static CDKResult ConsolidateCameraInfo(
        UINT32            numBundledCameras,
        CHICAMERAINFO**   ppCamInfo,
        CHICAMERAINFO*    pConsolidatedCamInfo);

    /// Reconfigure the controller
    virtual CDKResult Reconfigure(
        mcc_create_data_t* pMccCreateData);

    /// Translate request settings
    virtual CDKResult TranslateRequestSettings(
        multicam_req_settings* pMultiCamSettings);

    /// Process the result metadata
    virtual VOID ProcessResultMetadata(
        VOID* pResultMetadata);

    /// Destroy the MultiCamController object
    virtual VOID Destroy() = 0;

    /// Get controller result
    virtual controller_result_t GetResult();

    /// Function to get the controller type
    CHX_INLINE multicam_controller_type_t GetType() const
    {
        return m_controllerType;
    }

    /// Function to fill the offline metadata
    virtual CDKResult FillOfflinePipelineInputMetadata(
        multicam_result_metadata_t* pMultiCamResultMetadata,
        VOID*                       pOfflineMetadata,
        BOOL                        isSnapshotPipeline);

    /// Function to extract the subset of camera metadata from the result metadata
    VOID ExtractCameraMetadata(
        VOID*           pResultMetadata,
        CameraMetadata* pExtractedCameraMetadata);

    /// Function to set all the dual camera vendor tags in real time requests
    VOID UpdateVendorTags(
        multicam_req_settings* pMultiCamSettings);

    /// Function to update the scaler crop for snapshot
    virtual VOID UpdateScalerCropForSnapshot(
        VOID* pMetadata);

protected:

    MultiCamController() = default;
    virtual ~MultiCamController();

    multicam_controller_type_t m_controllerType;    ///< Type of the controller
    controller_result_t        m_result;            ///< Controller result
    Mutex*                     m_pLock;             ///< Mutex to protect the result state
    UINT32                     m_frameId;           ///< Current frame id
    VOID*                      m_pRawOTPData;       ///< Pointer to raw OTP data
    UINT32                     m_rawOTPDataSize;    ///< Size of raw OTP data

private:

    /// Do not allow the copy constructor or assignment operator
    MultiCamController(const MultiCamController& rMultiCamController) = delete;
    MultiCamController& operator= (const MultiCamController& rMultiCamController) = delete;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief DualFovController for optical zoom usecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DualFovController final : public MultiCamController
{
public:

    /// Create controller object
    static DualFovController* Create(
        mcc_create_data_t* pMccCreateData);

    /// Consolidate the camera info
    static CDKResult ConsolidateCameraInfo(
        UINT32            numBundledCameras,
        CHICAMERAINFO**   ppCamInfo,
        CHICAMERAINFO*    pConsolidatedCamInfo);

    /// Reconfigure the controller
    virtual CDKResult Reconfigure(
        mcc_create_data_t* pMccCreateData);

    /// Translate request settings
    virtual CDKResult TranslateRequestSettings(
        multicam_req_settings* pMultiCamSettings);

    /// Process the result metadata
    virtual VOID ProcessResultMetadata(
        VOID* pResultMetadata);

    /// Destroy/Cleanup the object
    virtual VOID Destroy();

    /// Get controller result
    controller_result_t GetResult();

    /// Function to fill the offline metadata
    CDKResult FillOfflinePipelineInputMetadata(
        multicam_result_metadata_t* pMultiCamResultMetadata,
        VOID*                       pOfflineMetadata,
        BOOL                        isSnapshotPipeline);

    /// Function to update the scaler crop for snapshot
    VOID UpdateScalerCropForSnapshot(
        VOID* pMetadata);

protected:

private:

    DualFovController() = default;
    virtual ~DualFovController();

    /// Do not allow the copy constructor or assignment operator
    DualFovController(const DualFovController& rDualFovController) = delete;
    DualFovController& operator= (const DualFovController& rDualFovController) = delete;

    CDKResult CalculateTransitionParams();
    VOID SetInitialResultState();
    WeightedRegion TranslateMeteringRegion(WeightedRegion* pRegion, CameraRoleType camRole);
    BOOL isFusionEnabled();

    UINT32              m_camIdWide;                                ///< Wide camera Id
    UINT32              m_camIdTele;                                ///< Tele camera Id
    UINT32              m_camIdLogical;                             ///< Logical camera Id
    FLOAT               m_zoomUser;                                 ///< User zoom value
    FLOAT               m_zoomWide;                                 ///< Total zoom on wide camera
    FLOAT               m_zoomTele;                                 ///< Total zoom on tele camera
    FLOAT               m_adjustedFovRatio;                         ///< Adjusted FOV ratio between cameras
    FLOAT               m_transitionLow;                            ///< Lower transition point to toggle LPM
    FLOAT               m_transitionWideToTele;                     ///< Approx. zoom value for master switch from Wide to Tele
    FLOAT               m_transitionTeleToWide;                     ///< Approx. zoom value for master switch from Tele to Wide
    FLOAT               m_transitionHigh;                           ///< Higher transition point to toggle LPM
    FLOAT               m_focalLengthWide;                          ///< Focal length of wide camera lens
    FLOAT               m_focalLengthTele;                          ///< Focal length of tele camera lens
    FLOAT               m_pixelPitchWide;                           ///< Pixel size of wide camera
    FLOAT               m_pixelPitchTele;                           ///< Pixel size of tele camera
    FLOAT               m_currentLuxIndex;                          ///< Current lux index value
    UINT32              m_currentFocusDistCm;                       ///< Current focus distance in cm
    BOOL                m_isMainCamFovWider;                        ///< Boolean to indicate if main camera has wider fov than aux
    BOOL                m_isVideoStreamSelected;                    ///< Boolean to indicate if the video stream is selected
    dimension_t         m_sensorDimensionWide;                      ///< Wide sensor output dimension
    dimension_t         m_sensorDimensionTele;                      ///< Tele sensor output dimension
    dimension_t         m_previewDimensions;                        ///< Preview dimension
    CHIRECT             m_activeArraySizeWide;                      ///< Wide sensor active pixel array dimension
    CHIRECT             m_activeArraySizeTele;                      ///< Tele sensor active pixel array dimension
    CHIRECT             m_fovRectIFEWide;                           ///< IFE FOV wrt to active sensor array for wide camera
    CHIRECT             m_fovRectIFETele;                           ///< IFE FOV wrt to active sensor array for tele camera
    ZoomTranslator*     m_pZoomTranslator;                          ///< Pointer to zoom translator
    CameraRoleType      m_recommendedMasterCameraRole;              ///< Recommended master camera role
    pixel_shift_t       m_pixelShiftWidePreview;                    ///< Pixel shift applied by SAT on wide camera preview
    pixel_shift_t       m_pixelShiftWideSnapshot;                   ///< Pixel shift applied by SAT on wide camera snapshot
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief RTBController for Real Time Bokeh usecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class RTBController final : public MultiCamController
{
public:

    /// Create controller object
    static RTBController* Create(
        mcc_create_data_t* pMccCreateData);

    /// Translate request settings
    virtual CDKResult TranslateRequestSettings(
        multicam_req_settings* pMultiCamSettings);

    /// Process the result metadata
    virtual VOID ProcessResultMetadata(
        VOID* pResultMetadata);

    /// Destroy/Cleanup the object
    virtual VOID Destroy();

    /// Get controller result
    virtual controller_result_t GetResult();

    /// Function to fill the offline metadata
    CDKResult FillOfflinePipelineInputMetadata(
        multicam_result_metadata_t* pMultiCamResultMetadata,
        VOID*                       pOfflineMetadata,
        BOOL                        isSnapshotPipeline);

    /// Function to update the scaler crop for snapshot
    VOID UpdateScalerCropForSnapshot(
        VOID* pMetadata);

private:

    RTBController() = default;
    virtual ~RTBController();

    /// Do not allow the copy constructor or assignment operator
    RTBController(const RTBController& rRTBController) = delete;
    RTBController& operator= (const RTBController& rRTBController) = delete;

    VOID SetInitialResultState();
    WeightedRegion TranslateMeteringRegion(WeightedRegion* pRegion, CameraRoleType camRole);

    UINT32              m_camIdWide;                                ///< Wide camera Id
    UINT32              m_camIdTele;                                ///< Tele camera Id
    UINT32              m_camIdLogical;                             ///< Logical camera Id
    FLOAT               m_adjustedFovRatio;                         ///< Adjusted FOV ratio between cameras
    BOOL                m_isMainCamFovWider;                        ///< Boolean to indicate if main camera has wider fov than aux
    CHIRECT             m_activeArraySizeWide;                      ///< Wide sensor active pixel array dimension
    CHIRECT             m_activeArraySizeTele;                      ///< Tele sensor active pixel array dimension
    CameraRoleType      m_recommendedMasterCameraRole;              ///< Recommended master camera role
    CHIRECT             m_fovRectIFEWide;                           ///< IFE FOV wrt to active sensor array for wide camera
    CHIRECT             m_fovRectIFETele;                           ///< IFE FOV wrt to active sensor array for tele camera
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief BayerMonoController for Bayer-Mono use case
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BayerMonoController final : public MultiCamController
{
public:

    /// Create controller object
    static BayerMonoController* Create(
        mcc_create_data_t* pMccCreateData);

    /// Consolidate the camera info
    static CDKResult ConsolidateCameraInfo(
        UINT32            numBundledCameras,
        CHICAMERAINFO**   ppCamInfo,
        CHICAMERAINFO*    pConsolidatedCamInfo);

    /// Reconfigure the controller
    virtual CDKResult Reconfigure(
        mcc_create_data_t* pMccCreateData);

    /// Translate request settings
    virtual CDKResult TranslateRequestSettings(
        multicam_req_settings* pMultiCamSettings);

    /// Process the result metadata
    virtual VOID ProcessResultMetadata(
        VOID* pResultMetadata);

    /// Destroy/Cleanup the object
    virtual VOID Destroy();

private:

    BayerMonoController() = default;
    virtual ~BayerMonoController();

    /// Do not allow the copy constructor or assignment operator
    BayerMonoController(const BayerMonoController& rBayerMonoController) = delete;
    BayerMonoController& operator= (const BayerMonoController& rBayerMonoController) = delete;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief VRController for VR use case
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class VRController final : public MultiCamController
{
public:

    /// Create controller object
    static VRController* Create(
        mcc_create_data_t* pMccCreateData);

    /// Consolidate the camera info
    static CDKResult ConsolidateCameraInfo(
        UINT32            numBundledCameras,
        CHICAMERAINFO**   ppCamInfo,
        CHICAMERAINFO*    pConsolidatedCamInfo);

    /// Reconfigure the controller
    virtual CDKResult Reconfigure(
        mcc_create_data_t* pMccCreateData);

    /// Translate request settings
    virtual CDKResult TranslateRequestSettings(
        multicam_req_settings* pMultiCamSettings);

    /// Process the result metadata
    virtual VOID ProcessResultMetadata(
        VOID* pResultMetadata);

    /// Destroy/Cleanup the object
    virtual VOID Destroy();

private:

    VRController() = default;
    virtual ~VRController();

    /// Do not allow the copy constructor or assignment operator
    VRController(const BayerMonoController& rVRController) = delete;
    VRController& operator= (const VRController& rVRController) = delete;

    UINT32              m_camIdVRMaster;                           ///< VR Master camera Id
    UINT32              m_camIdVRSlave;                            ///< VR Slave camera Id
    UINT32              m_camIdLogical;                            ///< Logical camera Id
    CameraRoleType      m_recommendedMasterCameraRole;             ///< Recommended master camera role
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief MultiCamControllerManager is a singleton class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MultiCamControllerManager
{
public:

    /// Get the instance of manager object
    static MultiCamControllerManager* GetInstance()
    {
        static MultiCamControllerManager s_controllerManagerSingleton;

        return &s_controllerManagerSingleton;
    }

    /// Get MultiCamController object
    MultiCamController* GetController(
        mcc_create_data_t* pMccCreateData);

    /// Destroy MultiCamController object
    VOID DestroyController(
        MultiCamController* pMultiCamController);

    /// Destroy MultiCamControllerManager object
    VOID Destroy();

    static ChiVendorTagsOps     s_vendorTagOps;                   ///< CHI vendor tag ops
    static UINT32               m_vendorTagOpticalZoomResultMeta; ///< Vendor tag of OZ result metadata
    static UINT32               m_vendorTagOpticalZoomInputMeta;  ///< Vendor tag of OZ input metadata
    static UINT32               m_vendorTagBokehResultMeta;       ///< Vendor tag of Bokeh result metadata
    static UINT32               m_vendorTagBokehInputMeta;        ///< Vendor tag of Bokeh input metadata
    static UINT32               m_vendorTagMultiCameraRole;       ///< Vendor tag of camera role
    static UINT32               m_vendorTagCropRegions;           ///< Vendor tag of capture request crop regions
    static UINT32               m_vendorTagIFEAppliedCrop;        ///< Vendor tag of IFE applied crop
    static UINT32               m_vendorTagSensorIFEAppliedCrop;  ///< Vendor tag of IFE applied crop
    static UINT32               m_vendorTagIFEResidualCrop;       ///< Vendor tag of IFE residual crop
    static UINT32               m_vendorTagLuxIndex;              ///< Vendor tag of LUX index
    static UINT32               m_vendorTagMasterInfo;            ///< Vendor tag of master camera
    static UINT32               m_vendorTagLPMInfo;               ///< Vendor tag for LPM info
    static UINT32               m_vendorTagSyncInfo;              ///< Vendor tag for SYNC info

protected:
private:

    MultiCamController* m_pController[MaxNumImageSensors];          ///< Array to hold per camera controller

    MultiCamControllerManager();
    virtual ~MultiCamControllerManager();

    /// Do not allow the copy constructor or assignment operator
    MultiCamControllerManager(const MultiCamControllerManager& MultiCamControllerManager) = delete;
    MultiCamControllerManager& operator= (const MultiCamControllerManager& MultiCamControllerManager) = delete;
};

#endif // CHXMULTICAMCONTROLLER_H
