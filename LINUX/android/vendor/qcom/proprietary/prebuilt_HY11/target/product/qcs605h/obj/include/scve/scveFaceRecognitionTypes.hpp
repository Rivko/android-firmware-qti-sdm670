/**=============================================================================

@file
   scveFaceRecognitionTypes.hpp

@brief
   SCVE Types Definition for Face Recognition feature.

Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================**/

//=============================================================================
///@defgroup scveFaceRecognition Face-Recognition
///@brief Defines Types for SCVE-Face-Recognition
///@ingroup scve
//=============================================================================

#ifndef SCVE_FACE_RECOGNITION_TYPES_HPP
#define SCVE_FACE_RECOGNITION_TYPES_HPP

#include "scveTypes.hpp"

namespace SCVE
{

#define SCVE_FACE_RECOGNITION_DEFAULT_MAX_FACES                10
#define SCVE_FACE_RECOGNITION_DEFAULT_MAX_REGISTERED_FACES     100
#define SCVE_FACE_RECOGNITION_DEFAULT_MIN_FACE_SIZE_TO_DETECT  80
#define SCVE_FACE_RECOGNITION_FEATURE_ID_LEN                   20
#define SCVE_FACE_RECOGNITION_DEFAULT_GOOD_SCORE               175
#define SCVE_FACE_RECOGNITION_DEFAULT_UNENROLLED_SCORE         100
#define SCVE_FACE_RECOGNITION_DEFAULT_DETECTION_THRESHOLD      750

//------------------------------------------------------------------------------
/// @brief
///    Error codes specific to FaceRecognition
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
enum StatusCodes_FaceRecognition
{
   SCVE_FACERECOGNITION_ERROR_INVALID_FEATURE_TYPE          = SCVE_FACERECOGNITION_ERROR + 1,
   SCVE_FACERECOGNITION_ERROR_INVALID_DATABASE              = SCVE_FACERECOGNITION_ERROR + 2,
   SCVE_FACERECOGNITION_ERROR_FEATURE_NOT_FOUND             = SCVE_FACERECOGNITION_ERROR + 3,
   SCVE_FACERECOGNITION_ERROR_INVALID_FACEINFO              = SCVE_FACERECOGNITION_ERROR + 4,
   SCVE_FACERECOGNITION_ERROR_SKIP_RECOGNITION              = SCVE_FACERECOGNITION_ERROR + 5
};

//------------------------------------------------------------------------------
/// @brief
///    Face Recognition image mode
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
typedef enum
{
   /// For offline processing of large image collection where real time feedback
   /// is not critical
   SCVE_FACERECOGNITION_STILL_IMAGE = 0,
   /// For camera previe mode where real time feedback is very critical
   SCVE_FACERECOGNITION_VIDEO       = 1
} FR_ImageMode;

//------------------------------------------------------------------------------
/// @brief
///    Enum defining pose (yaw) angle coverage that is used for detecting the face.
///    The FRONT will be faster than the HALF. The HALF will be faster than the FULL.
///
/// @param FR_DETECTION_POSE_FRONT
///    Face detection frontal face only (yaw angle of 0 to 30).
///
/// @param FR_DETECTION_POSE_HALF
///    Face detection from frontal face to half profile face (yaw angle of 0 to 60).
///
/// @param FR_DETECTION_POSE_FULL
///    Face detection from frontal face to full profile face (yaw angle of 0 to 90).
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
typedef enum
{
   FR_DETECTION_POSE_FRONT,
   FR_DETECTION_POSE_HALF,
   FR_DETECTION_POSE_FULL
} FR_DetectionPose;

//------------------------------------------------------------------------------
/// @brief
///    Enum defining face tracking level.
///    The higher the tracking level the slower the speed but more faces will
///    be tracked.
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
typedef enum
{
   FR_DETECTION_TRACKING_LOW,
   FR_DETECTION_TRACKING_MEDIUM,
   FR_DETECTION_TRACKING_HIGH
} FR_DetectionTracking;

//------------------------------------------------------------------------------
/// @brief
///    Face landmark location structure to store location of the key elements of
///    eyes, mouth, and nose that are used to recognize the face
/// @param sLeftEye
///    x and y location of left eye
/// @param sRightEye
///    x and y location of right eye
/// @param sLeftEyeInnerCorner
///    x and y location of inner corner of left eye
/// @param sLeftEyeOuterCorner
///    x and y location of outer corner of left eye
/// @param sRightEyeInnerCorner
///    x and y location of inner corner of right eye
/// @param sRightEyeOuterCorner
///    x and y location of outer corner of right eye
/// @param sMouthLeft
///    x and y location of left side of the mouth
/// @param sMouthRight
///    x and y location of right side of the mouth
/// @param sNoseLeft
///    x and y location of left side of the nose
/// @param sNoseRight
///    x and y location of right side of the nose
/// @param sMouthUp
///    x and y location of upper side of the mouth
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
typedef struct {
   Point sLeftEye;
   Point sRightEye;
   Point sLeftEyeInnerCorner;
   Point sLeftEyeOuterCorner;
   Point sRightEyeInnerCorner;
   Point sRightEyeOuterCorner;
   Point sMouthLeft;
   Point sMouthRight;
   Point sNoseLeft;
   Point sNoseRight;
   Point sMouthUp;
} FR_LandmarkLocation;

//------------------------------------------------------------------------------
/// @brief
///    Face info structure to store face information that will be used to recognize
///    face
/// @param nID
///    ID Number
/// @param sLeftTop
///    Left-Top bounding box face coordinates
/// @param sRightTop
///    Right-Top bounding box face coordinates
/// @param sLeftBottom
///    Left-Bottom bounding box face coordinates
/// @param sRightBottom
///    Right-Bottom bounding box face coordinates
/// @param sLandmarkLocation
///    Face Landmark location coordinates
/// @param nPose
///    Approximate pose (yaw) angle.
///    Possible values are -180, -90, -45, 0, 45, 90.
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
typedef struct {
   int32_t              nID;
   Point                sLeftTop;
   Point                sRightTop;
   Point                sLeftBottom;
   Point                sRightBottom;
   FR_LandmarkLocation  sLandmarkLocation;
   int32_t              nPose;
} FR_FaceInfo;

//------------------------------------------------------------------------------
/// @brief
///    FR_Result structure is used for obtaining results from the face recognition
///    operation
/// @param nMatchScore
///    The matching score.
///    SCVE_FACE_RECOGNITION_DEFAULT_GOOD_SCORE usually give a good matching result.
/// @param sPID
///    The associated person ID.
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
typedef struct
{
   float   nMatchScore;
   uint8_t sPID[SCVE_FACE_RECOGNITION_FEATURE_ID_LEN];
} FR_Result;

//------------------------------------------------------------------------------
/// @brief
///    Face Detection configuration parameters.
/// @param sDetectionDimension
///    Image size of the input image for face detection.
///    At SCVE_FACERECOGNITION_STILL_IMAGE mode, it's the max dimension.
///    At SCVE_FACERECOGNITION_VIDEO mode, the library is expected to get the same
///    dimension for every frames.
/// @param nMaxFaces
///    Maximum number of faces per image that can be detected.
/// @param nMinFaceSizeToDetect
///    Minimum face size to detect in pixels for width and height.
///    This minimum face size is for detection face process. Smaller size will increase
///    the distance between detected face and camera with the expense of more face
///    detection process time.
///    To get a good result in recognizing face, it's recommended to have minimum 80x80
///    face size.
/// @param nFaceDetectThreshold
///    Confidence value to detect face.
/// @param ePoseCoverage
///    Pose (yaw) angle coverage that is used for detecting the face.
/// @param eTrackingLevel
///    Face tracking level. The higher the tracking level the slower the speed
///    but more faces will be tracked.
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
typedef struct _FR_DetectionConfig
{
   ImageDimension       sDetectionDimension;
   uint32_t             nMaxFaces;
   uint32_t             nMinFaceSizeToDetect;
   uint32_t             nFaceDetectThreshold;
   FR_DetectionPose     ePoseCoverage;
   FR_DetectionTracking eTrackingLevel;

   INLINE _FR_DetectionConfig() :
      nMaxFaces(SCVE_FACE_RECOGNITION_DEFAULT_MAX_FACES),
      nMinFaceSizeToDetect(SCVE_FACE_RECOGNITION_DEFAULT_MIN_FACE_SIZE_TO_DETECT),
      nFaceDetectThreshold(SCVE_FACE_RECOGNITION_DEFAULT_DETECTION_THRESHOLD),
      ePoseCoverage(FR_DETECTION_POSE_FRONT),
      eTrackingLevel(FR_DETECTION_TRACKING_MEDIUM) {}
} FR_DetectionConfig;

//------------------------------------------------------------------------------
/// @brief
///    FaceRecognition configuration parameters
/// @param bEnableRecognition
///    Flag to enable face recognition. If this flag set to false, the face
///    recognition will be disable, only face detection can be called, and
///    face detection result (FR_FaceInfo) will be without FR_LandmarkLocation
///    information.
/// @param nMaxRegFaces
///    Maximum number of faces that can be enrolled to database.
/// @param sDBFileName
///    FR database file name. If exist, read the existing data to get the
///    enrolled faces. If NULL, read the data from the latest database file that might be
///    passed by newInstance() or SaveDatabase().
///    If NULL and previously never get any database file from newInstance() or SaveDatabase(),
///    then there will be no enrolled faces to begin.
/// @param sModelFileName
///    FR model file name.
/// @param nTrackingScore
///    The score threshold to track the recognized faces. It is used in video mode only.
/// @param nUnenrolledScore
///    The score threshold to track the unenrolled faces. It is used in video mode only.
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
typedef struct _FR_RecognitionConfig
{
   bool           bEnableRecognition;
   uint32_t       nMaxRegFaces;
   const char    *sDBFileName;
   const char    *sModelFileName;
   float          nTrackingScore;
   float          nUnenrolledScore;

   INLINE _FR_RecognitionConfig() :
      bEnableRecognition(true),
      nMaxRegFaces(SCVE_FACE_RECOGNITION_DEFAULT_MAX_REGISTERED_FACES),
      sDBFileName(NULL),
      sModelFileName(NULL),
      nTrackingScore(SCVE_FACE_RECOGNITION_DEFAULT_GOOD_SCORE),
      nUnenrolledScore(SCVE_FACE_RECOGNITION_DEFAULT_UNENROLLED_SCORE) {}
} FR_RecognitionConfig;

//------------------------------------------------------------------------------
/// @brief
///    Configuration parameters for SCVE FaceRecognition feature.
/// @param sDetectionConfig
///    Face Detection configuration.
/// @param sRecognitionConfig
///    Face Recognition configuration.
///
/// @ingroup scveFaceRecognition
//------------------------------------------------------------------------------
typedef struct _FR_Config
{
   FR_DetectionConfig    sDetectionConfig;
   FR_RecognitionConfig  sRecognitionConfig;
} FR_Config;

} //namespace SCVE

#endif //SCVE_FACE_RECOGNITION_HPP

