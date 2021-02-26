////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file chimulticamera.h
/// @brief Define Qualcomm Technologies, Inc. Multicamera input / output Meta data information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIMULTICAMERA_H
#define CHIMULTICAMERA_H

#include "camxcdktypes.h"
#include "chicommontypes.h"
#include "chi.h"

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// @brief Defines maximum number of nodes for FOVs
#define MAX_NODES_NUMBER              4

/// @brief Defines maximum number of linked session for Multicamera
#define MAX_LINKED_SESSIONS           2

/// @brief Defines maximum number of streams for a camera
#define MAX_NUM_STREAMS               11

/// @brief Max number of faces detected
static const UINT8 FDMaxFaces = 10;

///@brief Enumeration describing sensor mode
typedef enum
{
    NoSync = 0,      ///< No Hardware Sync
    MasterMode,      ///< Master Mode
    SlaveMode,       ///< Slave Mode
} SensorSyncMode;

///@brief Enumeration describing sensor status
typedef enum
{
    SensorNoPerf = 0,///< No Perf Ctrl
    SensorActive,    ///< Sensor In Active Status
    SensorStandby,   ///< Sensor In Sleeping Status
} SensorPerfCtrl;

///@brief Structure describing sensor hardware sync mode configure
///Vendor tag section name:com.qti.chi.multicamerasensorconfig
///Vendor tag tag name:sensorsyncmodeconfig
typedef struct
{
    BOOL           isValid;        ///< flag to indicate if sync mode meta is valid
    SensorSyncMode sensorSyncMode; ///< sensor sync mode
} SensorSyncModeMetadata;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing the image size
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    INT                   width;                        ///< Image size width value
    INT                   height;                       ///< Image size height value
} ImageSizeData;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief structure for refernce crop window size
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    INT32 width;       ///< width for reference crop window
    INT32 height;      ///< height for reference crop window
} RefCropWindowSize;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing shift offset information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    INT32                horizonalShift;               ///< x cordinate of the pixel
    INT32                verticalShift;                ///< y cordinate of the pixel
} ImageShiftData;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Struct for a weighted region used for focusing/metering areas
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    INT32                xMin;                           ///< Top-left x-coordinate of the region
    INT32                yMin;                           ///< Top-left y-coordinate of the region
    INT32                xMax;                           ///< Width of the region
    INT32                yMax;                           ///< Height of the region
    INT32                weight;                         ///< Weight of the region
} WeightedRegion;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Struct for a weighted region used for focusing/metering areas
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    UINT8                numFaces;                       ///< Number of faces detected
    UINT8                faceScore[FDMaxFaces];          ///< List of face confidence scores for detected faces
    CHIRECT              faceRect[FDMaxFaces];           ///< List of face rect information for detected faces
} FDMetadataResults;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Enumeration describing Camera Role
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    CameraRoleTypeDefault                = 0,            ///< Camera Type Default
    CameraRoleTypeWide,                                  ///< Camera type wide
    CameraRoleTypeTele,                                  ///< Camera type tele
    CameraRoleTypeVR,                                    ///< Camera type VR 360
    CameraRoleTypeInvalid,                               ///< Invalid role to indicate error
} CameraRoleType;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing Low Power mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    CameraRoleType        cameraRole;                   ///< Camera type of the chosen frame
    BOOL                  isEnabled;                    ///< Flag to check if Low power mode is enabled
} LowPowerModeData;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing camera metadata needed by SAT node
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    BOOL                  isValid;                      ///< If this metadata is valid
    CameraRoleType        cameraRole;                   ///< Camera role this metadata belongs to
    CameraRoleType        masterCameraRole;             ///< Master camera role
    CHIRECT               userCropRegion;               ///< Overall user crop region
    CHIRECT               pipelineCropRegion;           ///< Pipeline specific crop region
    CHIRECT               ifeLimitCropRegion;           ///< Max Crop that IFE can apply
    CHIRECT               fovRectIFE;                   ///< IFE FOV wrt to active sensor array
    CHIRECT               fovRectIPE;                   ///< IPE FOV wrt to active sensor array
    CHIRECT               ifeAppliedCrop;               ///< Crop applied by IFE
    CHIRECT               sensorIFEAppliedCrop;         ///< Crop applied by Sensor+IFE
    CHIRECT               ifeResidualCrop;              ///< Crop remaining after IFE processing
    WeightedRegion        afFocusROI;                   ///< AF focus ROI
    CHIRECT               activeArraySize;              ///< Wide sensor active array size
    FLOAT                 lux;                          ///< LUX value
    INT32                 focusDistCm;                  ///< Focus distance in cm
    UINT32                afState;                      ///< AF state
    INT32                 isoSensitivity;               ///< ISO value
    INT64                 exposureTime;                 ///< Exposure time
    UINT64                sensorTimestamp;              ///< Sensor timestamp
    FDMetadataResults     fdMetadata;                   ///< Face detection metadata
} CameraMetadata;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing Multi camera SAT input meta data structure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    UINT32                frameId;                      ///< Frame ID
    UINT8                 numInputs;                    ///< Number of input frames
    ImageShiftData        outputShiftSnapshot;          ///< Snapshot frame shift due to spatial alignment
    CameraMetadata        cameraMetadata[MAX_LINKED_SESSIONS]; ///< Camera metdata for SAT
} InputMetadataOpticalZoom;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing Multi camera SAT output meta data structure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    CameraRoleType        masterCamera;                 ///< Current master camera
    CameraRoleType        recommendedMasterCamera;      ///< Recommended master camera
    ImageShiftData        outputShiftPreview;           ///< Preview frame shift due to spatial alignment
    ImageShiftData        outputShiftSnapshot;          ///< Snapshot frame shift due to spatial alignment
    ImageSizeData         refResForOutputShift;         ///< Reference resolution for the output shift
    CHIRECT               outputCrop;                   ///< Output Crop information
    LowPowerModeData      lowPowerMode[MAX_LINKED_SESSIONS]; ///< output lower power mode information
} OutputMetadataOpticalZoom;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing Multi camera RTB input meta data structure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    UINT32                frameId;                      ///< Frame ID
    UINT32                blurLevel;                    ///< Blur intensity value to be applied to the Bokeh
    UINT32                blurMinValue;                 ///< Blur minimum value
    UINT32                blurMaxValue;                 ///< Blur maximum value
    CameraMetadata        cameraMetadata[MAX_LINKED_SESSIONS]; ///< Camera metdata for RTB
} InputMetadataBokeh;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing Multi camera RTB output meta data structure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    CameraRoleType        masterCamera;                 ///< Current master camera
    CameraRoleType        recommendedMasterCamera;      ///< Recommended master camera
    UINT32                depthEffectInfo;              ///< Depth Effect status information
} OutputMetadataBokeh;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure for Dual camera role, Id
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    CameraRoleType        currentCameraRole;             ///< Current camera role - wide / tele
    UINT32                currentCameraId;               ///< Current camera id
    UINT32                logicalCameraId;               ///< Logical camera id
    CameraRoleType        masterCameraRole;              ///< Master camera role - wide / tele
} MultiCameraIdRole;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure for Dual camera low power mode information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    UINT32                isLPMEnabled;                         ///< Low power mode status
    UINT32                isSlaveOperational;                   ///< Is Slave operational when LPM is enabled
    LowPowerModeData      lowPowerMode[MAX_LINKED_SESSIONS];    ///< LPM info both the real time pipelines
} LowPowerModeInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure for Dual camera sync mode information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    BOOL                  isSyncModeEnabled;                    ///< Sync mode info to sync both the frames
} SyncModeInfo;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure for crop regions for capture request
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    CHIRECT               userCropRegion;                ///< Overall user crop region
    CHIRECT               pipelineCropRegion;            ///< Pipeline specific crop region
    CHIRECT               ifeLimitCropRegion;            ///< Max Crop that IFE can apply
} CaptureRequestCropRegions;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure for Gamma information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    BOOL    isGammaValid;              ///< Is Valid Gamma Entries
    UINT32  gammaG[65];                ///< Gamma Green Entries
} GAMMAINFO;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing the OEM JPEG EXIF App header Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    SIZE_T  size;   ///< Size in bytes of the EXIF App header data buffer
    VOID*   pData;  ///< Pointer to the EXIF App header data buffer
} OEMJPEGEXIFAppData;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIMULTICAMERA_H
