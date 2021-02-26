////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file chifdproperty.h
/// @brief Define Qualcomm Technologies, Inc. FD property definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIFDPROPERTY_H
#define CHIFDPROPERTY_H

#include "chicommontypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Common FD data structures
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const UINT8  FDMaxFaceCount              = 10;       ///< Max number of faces that FD solution can detect
static const UINT8  FDMaxFaceCountIntermediate  = 35;       ///< Max number of intermediate (probable) faces that FD processes
static const INT32  FDInvalidPoint              = -2000;    ///< Co-ordinate value indicating an invalid point

static const CHAR* VendorTagSectionOEMFDConfig  = "org.quic.camera2.oemfdconfig"; ///< OEM FD config vendor tag section
static const CHAR* VendorTagNameOEMFDConfig     = "OEMFDConfig";                  ///< OEM FD config vendor tag name

static const CHAR* VendorTagSectionOEMFDResults    = "org.quic.camera2.oemfdresults";   ///< OEM FD results vendor tag section
static const CHAR* VendorTagNameOEMFDResults       = "OEMFDResults";                    ///< OEM FD rsults vendor tag name

/// @brief This enum indicates the type for face size configuration
enum FDFaceSizeType
{
    FDFaceAdjFixed,     ///< When set, use absolute value set for minimmum
    FDFaceAdjFloating,  ///< When set, min face size is calculated based on ratio
};

/// @brief This enum indicates the value for angle configuration
enum FDSearchAngle
{
    FDAngleNone,    ///< Angle none
    FDAngle45,      ///< 45 degrees each side
    FDAngle90,      ///< 90 degrees each side
    FDAngle135,     ///< 135 degrees each side
    FDAngleAll,     ///< All angles
};

/// @brief This enum indicates the value for search accuracy for already detected faces
enum FDFaceAccuracy
{
    FDAccuracyHigh      = 0,    ///< High accuracy
    FDAccuracyNormal    = 15,   ///< Normal accuracy
};

/// @brief This enum indicates the value for face detection mode
enum FDDetectionMode
{
    FDDetectionModeDefault,     ///< Default detection mode
    FDDetectionModeFull,        ///< Full mode
    FDDetectionModeLite,        ///< Lite mode
    FDDetectionModeContinuous,  ///< Continuous mode
};

/// @brief This enum indicates the values for detection search density
enum FDSearchDensity
{
    FDSearchDensityHighest  = 200,  ///< Highest search density
    FDSearchDensityHigh     = 150,  ///< High search density
    FDSearchDensityNormal   = 100,  ///< Normal search density
    FDSearchDensityLow      = 75,   ///< Low search density
    FDSearchDensityLowest   = 50,   ///< Lowest search density
};

/// @brief This enum indicates the index values for each facial point
enum FDFacialPointIndex
{
    FDFacialPointLeftEye = 0,   ///< Index indicating facial point left eye
    FDFacialPointRightEye,      ///< Index indicating facial point right eye
    FDFacialPointMouth,         ///< Index indicating facial point mouth
    FDFacialPointLeftEyeIn,     ///< Index indicating facial point left eye in
    FDFacialPointLeftEyeOut,    ///< Index indicating facial point left eye out
    FDFacialPointRightEyeIn,    ///< Index indicating facial point right eye in
    FDFacialPointRightEyeOut,   ///< Index indicating facial point right eye out
    FDFacialPointMouthLeft,     ///< Index indicating facial point mout left
    FDFacialPointMouthRight,    ///< Index indicating facial point mouth right
    FDFacialPointNoseLeft,      ///< Index indicating facial point nose left
    FDFacialPointNoseRight,     ///< Index indicating facial point nose right
    FDFacialPointMouthUp,       ///< Index indicating facial point mouth up
    FDFacialPointMax,            ///< Index indicating facial point max
};

/// @brief This enum indicates the index values for each facial contour
enum FDContourIndex
{
    FDContourEyeLeftPupil = 0,  ///< Index indicating contour left pupil
    FDContourEyeLeftIn,         ///< Index indicating contour left in
    FDContourEyeLeftOut,        ///< Index indicating contour left out
    FDContourEyeLeftUp,         ///< Index indicating contour left up
    FDContourEyeLeftDown,       ///< Index indicating contour left down
    FDContourEyeRightPupil,     ///< Index indicating contour right pupil
    FDContourEyeRightIn,        ///< Index indicating contour right in
    FDContourEyeRightOut,       ///< Index indicating contour right out
    FDContourEyeRightUp,        ///< Index indicating contour right up
    FDContourEyeRightDown,      ///< Index indicating contour right down
    FDContourForehead,          ///< Index indicating contour forehead
    FDContourNose,              ///< Index indicating contour nose
    FDContourNoseTip,           ///< Index indicating contour nose tip
    FDContourNoseLeft,          ///< Index indicating contour nose left
    FDContourNoseRight,         ///< Index indicating contour nose right
    FDContourNoseLeft0,         ///< Index indicating contour nose left 0 point
    FDContourNoseRight0,        ///< Index indicating contour nose right 0 point
    FDContourNoseLeft1,         ///< Index indicating contour nose left 1 point
    FDContourNoseRight1,        ///< Index indicating contour nose right 1 point
    FDContourMouthLeft,         ///< Index indicating contour mouth left
    FDContourMouthRight,        ///< Index indicating contour mouth right
    FDContourMouthUp,           ///< Index indicating contour mouth up
    FDContourMouthDown,         ///< Index indicating contour mouth down
    FDContourMouth,             ///< Index indicating contour mouth
    FDContourLipUp,             ///< Index indicating contour lip up
    FDContourLipDown,           ///< Index indicating contour lip down
    FDContourBrowLeftUp,        ///< Index indicating brow left up
    FDContourBrowLeftDown,      ///< Index indicating brow left down
    FDContourBrowLeftIn,        ///< Index indicating brow left in
    FDContourBrowLeftOut,       ///< Index indicating brow left out
    FDContourBrowRightUp,       ///< Index indicating brow right up
    FDContourBrowRightDown,     ///< Index indicating brow rightdown
    FDContourBrowRightIn,       ///< Index indicating brow rightin
    FDContourBrowRightOut,      ///< Index indicating brow rightout
    FDContourChin,              ///< Index indicating chin
    FDContourChinLeft,          ///< Index indicating chin left
    FDContourChinRight,         ///< Index indicating chin right
    FDContourEarLeftDown,       ///< Index indicating ear left down
    FDContourEarRightDown,      ///< Index indicating ear right down
    FDContourEarLeftUp,         ///< Index indicating ear left up
    FDContourEarRightUp,        ///< Index indicating ear right up
    FDContourMax,               ///< Index indicating contour max
};

/// @brief Structure Face ROI information
struct FDFaceROI
{
    UINT32      confidence; ///< Confidence of the face
    CHIPoint    center;     ///< Center of the face
    UINT32      width;      ///< Width of the face
    UINT32      height;     ///< Height of the face
    INT32       rollAngle;  ///< Roll angle of the face
    INT32       pose;       ///< Pose angle of the face
};

/// @brief Structure Accurate face direction info
struct FDFaceDirection
{
    BOOL    valid;      ///< Whether this info is valid
    INT32   upDown;     ///< Up-Down direction of the face
    INT32   leftRight;  ///< Left-Right direction of the face
    INT32   rollAngle;  ///< Roll angle of the face
};

/// @brief Structure Face Point attributes
struct FDFacePoint
{
    CHIPoint    position;   ///< Position of the face point
    UINT32      confidence; ///< Confidence of the face point
};

/// @brief Structure Facial parts points
struct FDFaceParts
{
    BOOL        valid;                          ///< Whether this info is valid
    FDFacePoint facialPoint[FDFacialPointMax];  ///< Face facial points
};

/// @brief Structure Facial contour points
struct FDFaceContour
{
    BOOL        valid;                      ///< Whether this info is valid
    FDFacePoint contourPoint[FDContourMax]; ///< Face contour points
};

/// @brief Structure Facial contour points
struct FDFaceGazeBlink
{
    BOOL    valid;              ///< Whether this info is valid
    INT32   leftRightGaze;      ///< left-Right gaze value
    INT32   upDownGaze;         ///< Up-Down gaze value
    BOOL    blinkDetected;      ///< Whether blink detected
    INT32   leftBlink;          ///< Left eye blink ratio
    INT32   rightBlink;         ///< Righ eye blink ratio
};

/// @brief Structure Face smile attributes
struct FDFaceSmile
{
    BOOL    valid;      ///<  Whether this info is valid
    INT32   degree;     ///< Smile degree
    UINT32  confidence; ///< Confidence of smile degree
};

/// @brief Structure Face attributes
struct FDFaceInfo
{
    INT32           faceID;         ///< Face unique ID
    BOOL            newFace;        ///< Whether this face is a new face
    FDFaceROI       faceROI;        ///< Face ROI information
    FDFaceDirection direction;      ///< Face direction information
    FDFaceParts     facialParts;    ///< Facial parts information
    FDFaceContour   contour;        ///< Face contour information
    FDFaceGazeBlink gazeBlink;      ///< Gaze, Blink information
    FDFaceSmile     smile;          ///< Smile information
};

/// @brief Structure Face results
struct FDResults
{
    UINT8       numFacesDetected;           ///< Number of faces detected
    FDFaceInfo  faceInfo[FDMaxFaceCount];   ///< List of faces detected with face information
};

/// @brief Structure Face intermediate results
struct FDIntermediateResults
{
    UINT8       numFacesDetected;                       ///< Number of faces detected
    FDFaceInfo  faceInfo[FDMaxFaceCountIntermediate];   ///< List of intermediate probable faces detected with face information
};

/// @brief enum to define all the different stabilization modes
enum FDStabilizationMode
{
    Equal,                          ///< Values will be marked as stable when two consecutive values are equal
    Smaller,                        ///< Values will be marked as stable if new values are bigger than old ones
    Bigger,                         ///< Values will be marked as stable if new values are smaller than old ones
    CloserToReference,              ///< Values will be marked as stable when the distance to reference is smaller
    ContinuesEqual,                 ///< The same as Equal, but it works in continues mode
    ContinuesSmaller,               ///< The same as Smaller, but it works in continues mode
    ContinuesBigger,                ///< The same as Bigger, but it works in continues mode
    ContinuesCloserToReference,     ///< The same as CloserToReference, but it works in continues mode
    WithinThreshold                 ///< Values are marked as stable when values are within threshold
};

/// @brief enum to define all the different stabilization filters
enum FDStabilizationFilter
{
    NoFilter,      ///< No stabilization filter
    Temporal,      ///< Temporal filter
    Hysteresis,    ///< Hysteresis
    Average,       ///< Average filter
    Median         ///< Median filter
};

/// @brief Structure to define Temporal Filter
struct FDTemporalFilter
{
    UINT32 numerator;    ///< Strength numerator
    UINT32 denominator;  ///< Strength denominator
};

/// @brief Structure to define Hysteresis Filter
struct FDHysteresisFilter
{
    UINT32 startA;    ///< Start point of Zone A
    UINT32 endA;      ///< End point of Zone A
    UINT32 startB;    ///< Start point of Zone B
    UINT32 endB;      ///< End point of Zone B
};

/// @brief Structure to define Average Filter
struct FDAverageFilter
{
    UINT32 historyLength;  ///< History length of the filter
};

/// @brief Structure to define Median Filter
struct FDMedianFilter
{
    UINT32 historyLength;  ///< History length of the filter
};

/// @brief Structure to hold stabilization tuning parameters
struct FDStabilizationAttributeConfig
{
    BOOL                    enable;             ///< Enable stabilization
    FDStabilizationMode     mode;               ///< Stabilization Mode
    UINT32                  minStableState;     ///< Minimum state count needed to go to a stable state
    UINT32                  stableThreshold;    ///< Stabilization threshold to go into stable state
    UINT32                  threshold;          ///< Within threshold, new values will not be accepted
    UINT32                  stateCount;         ///< Number of consecutive frames to wait until determining stable
    UINT32                  useReference;       ///< Stabilize data by a reference
    FDStabilizationFilter   filterType;         ///< Filter type to be used for stabilization
    union
    {
        FDTemporalFilter   temporalFilter;     ///< Temporal Filter
        FDHysteresisFilter hysteresisFilter;   ///< HysteresisFilter
        FDAverageFilter    averageFilter;      ///< Average Filter
        FDMedianFilter     medianFilter;       ///< Median Filter
    };
};

/// @brief Structure to hold stabilization configuration
struct FDStabilizationConfig
{
    BOOL                            enable;         ///< Enable stabilization
    UINT32                          historyDepth;   ///< Depth of historical data
    FDStabilizationAttributeConfig  position;       ///< Position attibute configurations
    FDStabilizationAttributeConfig  size;           ///< Position attibute configurations
};

/// @brief Structure Face size configuration
struct FDFaceSize
{
    enum FDFaceSizeType type;   ///< Face size type
    union
    {
        UINT32  size;           ///< Face size absolute value
        FLOAT   ratio;          ///< Face size ratio. Size is calculated based on this ratio w.r.t input frame
    };
};

/// @brief Structure Face search angle, threshold configuration for the given profile
struct FDProfileConfig
{
    FDSearchAngle   searchAngle;                ///< Search Angle. Used in below cases :
                                                ///  When upFront is disabled
                                                ///  Even when upFront is enabled, if upfront angle cannot be determined
    UINT32          upFrontSearchAnggle;        ///< Search angle when upFront is enabled and upfront angle can be determined
    INT32           priorityAngleRange;         ///< Priority angle range for threshold configuration
    INT32           priorityThreshold;          ///< Threshold in priority region
    INT32           priorityInnerThreshold;     ///< Inner threshold in priority region
    INT32           nonPriorityThreshold;       ///< Threshold in non-priority region
    INT32           nonPriorityInnerThreshold;  ///< Inner threshold in non-priority region
};

/// @brief Structure Face detection HW configuration
struct FDHwConfig
{
    BOOL            enable;                 ///< Whether to enable HW detection
    FDFaceSize      minFaceSize;            ///< Minimum face size configuration for HW
    FDFaceSize      maxFaceSize;            ///< Maximum face size configuration for HW
    BOOL            enableUpFrontAngles;    ///< Whether to enable upfront only detection when possible.
                                            /// When enabled,
                                            /// If determining upfront is possible, uses 'upFrontAngle' for configuration
                                            /// If determining upfront is not possible, uses 'angle' configuration
    FDSearchAngle   angle;                  ///< Angle configuration for faces in all poses
    UINT32          upFrontAngle;           ///< Angle configuration for faces in all poses for detecting only upfront faces
    UINT32          threshold;              ///< Threshold to determine a face
    UINT32          noFaceFrameSkip;        ///< Frame skip count when no faces detected
    UINT32          newFaceFrameSkip;       ///< Frame skiip count when atleast one face is detected
};

/// @brief Structure Face detection SW configuration
struct FDSwConfig
{
    BOOL            enable;                                 ///< Whether to enable SW detection
    FDFaceSize      minFaceSize;                            ///< Min face size configuration
    FDFaceSize      maxFaceSize;                            ///< Max face size configuration
    FDProfileConfig frontProfileConfig;                     ///< Angle configuration for front profile faces
    FDProfileConfig halfProfileConfig;                      ///< Angle configuration for half profile faces
    FDProfileConfig fullProfileConfig;                      ///< Angle configuration for full profile faces
    BOOL            enableUpFrontAngles;                    ///< Whether to enable upfront only detection when possible.
                                                            /// When enabled , If determining upfront is
                                                            ///     possible, uses 'upFrontAngle' for configuration
                                                            ///     not possible, uses 'angle' configuration
    BOOL            skipIfNoOrientation;                    ///< Skiip Sw detection if orientation information is not available
    FDDetectionMode detectionMode;                          ///< Face detection mode
    FDSearchDensity newFaceSearchDensity;                   ///< Face search density to detect new faces
    FDSearchDensity existingFaceSearchDensity;              ///< Face search density for the faces that are already detected
    BOOL            detectNewFacesInExistingFaceDirection;  ///< Detects new faces that are only in the direction of
                                                            ///  already detected faces
    UINT32          noFaceSearchCycle;                      ///< Search cycle when there are no faces detected already.
                                                            ///  Used if upFront is disabled or
                                                            /// if upFront angle can not be determined
    UINT32          newFaceSearchCycle;                     ///< Search cycle when atleast one face is detected
                                                            ///  Used if upFront is disabled or
                                                            /// if upFront angle can not be determined
    UINT32          newFaceSearchInterval;                  ///< Face search interval
                                                            ///  Used if upFront is disabled or
                                                            /// if upFront angle can not be determined
    UINT32          upFrontNoFaceSearchCycle;               ///< Search cycle when there are no faces detected already. Used
                                                            ///  if upFront is enabled and upFront angle can be determined
    UINT32          upFrontNewFaceSearchCycle;              ///< Search cycle when atleast one face is detected. Used
                                                            ///  if upFront is enabled and upFront angle can be determined
    UINT32          upFrontNewFaceSearchInterval;           ///< Face search interval. Used
                                                            ///  if upFront is enabled and upFront angle can be determined
    UINT32          positionSteadiness;                     ///< Face position steadiness value
    UINT32          sizeSteadiness;                         ///< Face size steadiness value
    BOOL            rollAngleExtension;                     ///< Roll angle extension
    BOOL            yawAngleExtension;                      ///< Yaw angle extension
    UINT32          noFaceFrameSkip;                        ///< Frame skip when no faces detected
    UINT32          newFaceFrameSkip;                       ///< Frame skiip when atleast one face is detected
};

/// @brief Structure Face detection false positive filter configuration
struct FDFPFilterConfig
{
    BOOL            enable;                     ///< Whether to enable false positive filtering
    UINT32          baseThreshold;              ///< Threshold value to configure to FD algo while false positive detection
    UINT32          innerThreshold;             ///< Inner threshold value to configure FD algo while false positive detection
    UINT32          expandFaceSizePercentage;   ///< Value used to determine face size range while false positive detection
    UINT32          expandBoxBorderPercentage;  ///< Value used to determine search window while false positive detection
    FLOAT           faceSpreadTolerance;        ///< Used to determine face spread tolerance while false positive detection
    FDSearchDensity searchDensity;              ///< Value used to determine search density while false positive detection
};

/// @brief Structure Face detection false positive filter configuration
struct FDROIGeneratorConfig
{
    BOOL            enable;                     ///< Whether to enable false positive filtering
    UINT32          baseThreshold;              ///< Threshold value to configure to FD algo while ROI generation
    UINT32          innerThreshold;             ///< Inner threshold value to configure FD algo while false positive detection
    UINT32          expandFaceSizePercentage;   ///< Value used to determine face size range while ROI generation
    UINT32          expandBoxBorderPercentage;  ///< Value used to determine search window while ROI generation
    FLOAT           faceSpreadTolerance;        ///< Value used to determine face spread tolerance while ROI generation
    FDSearchDensity searchDensity;              ///< Value used to determine search density while ROI generation
};

/// @brief Structure Face detection ROI manager configuration
struct FDROIManagerConfig
{
    BOOL    enable;                         ///< Whether to enable Face ROI manager
    UINT32  newGoodFaceConfidence;          ///< Confidence to determine a face as good face while detection
    UINT32  newNormalFaceConfidence;        ///< Confidence to determine a face as face while detection
    UINT32  existingFaceConfidence;         ///< Confidence to determine an existing face
    UINT32  angleDiffForStrictConfidence;   ///< Angle in each direction after which strict confidence values are used
                                            ///  to determine the face
    UINT32  strictNewGoodFaceConfidence;    ///< Strict confidence to determine a face as good face while detection
    UINT32  strictNewNormalFaceConfidence;  ///< Strict confidence to determine a face as face while detection
    UINT32  strictExistingFaceConfidence;   ///< Strict confidence to determine an existing face
    FLOAT   faceLinkMoveDistanceRatio;      ///< Move distance ratio threshold to link a face
    FLOAT   faceLinkMinSizeRatio;           ///< Minimum size ratio threshold to link a face
    FLOAT   faceLinkMaxSizeRatio;           ///< Maximum size ratio threshold to link a face
    FLOAT   faceLinkRollAngleDifference;    ///< Roll angle difference threshold to link a face
};

/// @brief Structure for fd tuning data tool version, SW can ignore this
struct FDConfigVersion
{
    UINT32          id;                                 ///< Tuning id
    INT32           majorRevision;                      ///< Major version of tuning version
    INT32           minorRevision;                      ///< Minor version of tuning version
    INT32           incrRevision;                       ///< Incr version of tuning version
};

/// @brief Structure Face detection tuning configuration
struct FDConfig
{
    UINT32                  id;                           ///< Tuning id
    FDConfigVersion         FDConfigData;                 ///< Tuning config info
    UINT32                  maxNumberOfFaces;             ///< Maximum number of faces to detect in the overall FD solution
    UINT32                  maxFPSWithFaces;              ///< Maximum FPS to run face detection processing when
                                                          ///  faces are detected
    UINT32                  maxFPSWithNoFaces;            ///< Maximum FPS to run face detection processing when no
                                                          ///  faces are detected
    UINT32                  multiCameraMaxFPSWithFaces;   ///< Maximum FPS to run face detection processing when
                                                          ///  faces are detected
    UINT32                  multiCameraMaxFPSWithNoFaces; ///< Maximum FPS to run face detection processing when no
                                                          ///  faces are detected
    BOOL                    lockDetectedFaces;            ///< Lock faces that are already detected
    UINT32                  initialNoFrameSkipCount;      ///< First n number frame which are always processed for face detection
    UINT32                  maxPendingFrames;             ///< Max allowed frames in pending queue
    UINT32                  delayCount;                   ///< Face delay count
    UINT32                  holdCount;                    ///< Face hold count
    FDFaceAccuracy          accuracy;                     ///< Accuracy with which an existing face have to searched
    FDHwConfig              hwConfig;                     ///< HW detection configuration
    FDSwConfig              swConfig;                     ///< SW detection configuration
    FDFPFilterConfig        FPFilter;                     ///< FP filter configuration
    FDROIGeneratorConfig    ROIGenerator;                 ///< ROI generator configuration
    FDROIManagerConfig      managerConfig;                ///< ROI manager configuration
    FDStabilizationConfig   stabilization;                ///< Stabilization configuration
};

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIFDPROPERTY_H
