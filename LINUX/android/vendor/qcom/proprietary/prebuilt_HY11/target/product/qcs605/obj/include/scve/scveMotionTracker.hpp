/**=============================================================================

@file
scveMotionTracker.hpp

@brief
SCVE API Definition for Motion Tracker feature.

Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================**/

#ifndef SCVE_MOTION_TRACKER_HPP
#define SCVE_MOTION_TRACKER_HPP

#include "scveTypes.hpp"
#include "scveContext.hpp"

namespace SCVE
{
enum StatusCodes_MotionTracker {
    SCVE_MotionTracker_ERROR_UNSUPPORTED_SEARCHSIZE         = SCVE_MOTION_TRACKER_ERROR + 1,
    SCVE_MotionTracker_ERROR_UNSUPPORTED_MAXForeground            = SCVE_MOTION_TRACKER_ERROR + 2,
    SCVE_MotionTracker_ERROR_UNSUPPORTED_STEPSIZE           = SCVE_MOTION_TRACKER_ERROR + 3,
    SCVE_MotionTracker_ERROR_UNSUPPORTED_MODE               = SCVE_MOTION_TRACKER_ERROR + 4,
    SCVE_MotionTracker_ERROR_UNSUPPORTED_IMAGE_FORMAT       = SCVE_MOTION_TRACKER_ERROR + 5,
    SCVE_MotionTracker_ERROR_UNSUPPORTED_IMAGE_SIZE         = SCVE_MOTION_TRACKER_ERROR + 6,
    SCVE_MotionTracker_ERROR_TRACKING                              = SCVE_MOTION_TRACKER_ERROR + 7,
    SCVE_MotionTracker_ERROR_TRACKING_UNSUPPORTED_INTERVALNUMBER   = SCVE_MOTION_TRACKER_ERROR + 8,
};

//------------------------------------------------------------------------------
/// @brief
///    ConfigParam is the structure to store configuration parameters for
/// motion-based blob or object tracking.
///
/// @param vaMode
/// Mandatory configuration parameter for object tracking and camera tamper.
/// The parameter also sets default configurations for other parameters which
/// can also be set explicitly through the setConfig API
///     0: home security indoor
///     1: home security outdoor
///     2: professional security close scene
///     3: professional security far scene
///
/// @param iWidth
///    Frame width of Camera stream fed to SCVE API.
///
/// @param iHeight
///    Frame height of Camera stream fed to SCVE API.
///
/// @param iFrameRate
///    Frame rate of Camera stream fed to SCVE API.
/// This parameter is also used to indicate the minimum
/// number of frames for an object to be detected.
/// when it is equal to -1, the default frame rate is 30
///
/// @param maxsize
///    maxsize of the Foreground (pixel area) that will be searched.
/// suggested value: 90000
///
/// @param backgroundLearningFrames
///    The least number of frames required to make the
/// background subtraction model ready
/// Suggested value: 100
///
/// @param trackingConsistencyFrames
///    Number of frames to be tracked for consistency.
/// Suggested value: 30. Parameter will be removed in a
/// future release.
///
/// @param bMergeSplitEnable
///    Flag (0: disable/1: enable) for enabling enhanced merge-split
/// aapproach in object tracking
/// Set to -1 or >1 for auto-config based on vaMode selection.
///
/// @param bSleepingTrackerEnable
///    Flag (0: disable/1: enable) for enabling sleeping object tracker.
/// Feature is enabled by default if it is not set to 0 or 1.
///
/// @param iSleepingDuration
///    Range: [1 1<<30]Number of frames to wait before a sleeping object is
/// un-tracked. Please set to negative value for default settings.
///
/// @param iDelaySensitivityRange
///    Range: [0-100]. Parameter to control trade-off between delay in
/// tracking versus the false positive rate. A larger number implies shorter
/// delay. A value in the range 10-20 enables stronger false positive removal
/// and >20 suggested for shorter dealys in tracking. Default value is 10;
///
/// @param bsetAlwaysLightingCheck
///    Flag (0: disable/1: enable) for lighting condition compensation.
/// Set to -1 or >1 for auto-config based on vaMode selection.
///
/// @param tamperConfirmDuration
///    Range: [0-30] Number of frames needed to confirm a tamper event.
/// Set to -1 for default value of 30.
///
/// @param iTamperRetestDuration
///    Range: [0-2700]. Number of frames to wait before
/// re-enablement of camera tamper after a tamper event generation.
/// Set to -1 for default value of 900.
///
/// @param bBSSetSensitivity
///    Flag (0: disable/1: enable) in Background Suctraction that
/// controls trade-off between detection sensitivity/delay and false positive rate.
/// Suggested to enable flag for vaMode = 3 for professional security-far scene or if
/// the scene is relatively dark and the objects are small.  Not recommended for use
/// under frequent lighting condition changes.
/// Set to -1 or >1 for deafult settings.
///
/// @param iBSSensitivityLevel
///    Range: [0-100]. Level to specify trade-off between detection
/// and false positive rate. Larger values enable stronger background
/// subtraction with more objects (with smaller sizes) being detected
/// sooner, potentially with more false positives. Out of range and
/// negative values for the parameter fall-back to default values.
///
/// @param bBSForceEqualDecLearn
///    Flag (0: disable/1: enable) to set equal detection and learning
/// sensitivities in Background subtraction. Recommended to always
/// set this value to 0. Out of range and negative value defaults to 0.
///
/// @param bsetHomeMoreSensitive
///    Flag (0: disable/1: enable) to enable sensitive tracking for home security.
/// When enabled, more false positive is expected. Feature is disabled by default
/// if it is not set to 0 or 1.
///
/// @param bsetShakinessResilience
///    Flag (0: disable/1: enable) to enable camera shakiness resilience
/// for non-home security cases. When enabled, more false positive is expected.
/// Feature is disabled by default if it is not set to 0 or 1.
///
//------------------------------------------------------------------------------

typedef struct _ConfigParam
{
   uint8_t vaMode;
   int32_t iWidth;
   int32_t iHeight;
   int32_t iFrameRate;
   uint32_t maxsize;
   uint32_t backgroundLearningFrames;
   uint32_t trackingConsistencyFrames;
   int8_t bMergeSplitEnable;
   int8_t bSleepingTrackerEnable;
   int32_t iSleepingDuration;
   int8_t iDelaySensitivityRange;
   int8_t bsetAlwaysLightingCheck;
   int8_t tamperConfirmDuration;
   uint16_t iTamperRetestDuration;
   int8_t bBSSetSensitivity;
   int8_t iBSSensitivityLevel;
   int8_t bBSForceEqualDecLearn;
   int8_t bsetHomeMoreSensitive;
   int8_t bsetShakinessResilience;
}ConfigParam;

//------------------------------------------------------------------------------
/// @brief
///    ForegroundRect is the structure to store one tracked object result.
///
/// @param rect
///    The rectangle that encloses the foreground object.
///
/// @param ID
///    The ID that is assigned to the foreground object, for foreground tracking.
///
//------------------------------------------------------------------------------
typedef struct _ForegroundRect
{
   SCVE::Rect rect;
   int32_t ID;
}ForegroundRect;

//------------------------------------------------------------------------------
/// @brief
///    BackgroundSubResult is the structure to store the result of Background
/// Subtraction.
///
/// @param muiNumForegrounds
///    Number of foreground objects detected.
///
/// @param maxForegrounds
///    Maximum number of foreground objects that can be detected.
///
/// @param resultRects
///    Pointer to an array of foreground objects detected. User should allocate
/// memory sufficient to hold maxForegrounds objects.
///
/// @param bkgImage
///    Pointer to the background image. User should allocate memory equal to input
/// image dimensions. Use image dimensions returned by the doBackgroundSubtraction_Sync()
/// API.
///
/// @param foregndImage
///    Pointer to the foreground image. User should allocate memory equal to input
/// image dimensions. Use image dimensions returned by the doBackgroundSubtraction_Sync()
/// API.
///
//------------------------------------------------------------------------------
typedef struct _BackgroundSubResult
{
   uint32_t muiNumForegrounds;
   uint32_t maxForegrounds;
   ForegroundRect *resultRects;
   SCVE::Image *bkgImage;
   SCVE::Image *foregndImage;
}BackgroundSubResult;

//------------------------------------------------------------------------------
/// @brief
///    TamperResult is the structure to store Camera Tamper Detection Result.
///
/// @param isTamper
///    Presence/absence of tamper.
///
/// @param tamperType
///   Integer refering to type of tamper.
///   NO_TAMPER                = 0x00000000,
///   OCCLUSION                = 0x00000001,
///   CAM_MOVE                 = 0x00000002,
///   DEFOCUS                  = 0x00000003,
///   OTHER_TYPE               = 0x00000004,
///   OVER_DARK                = 0x00000005,
///   NOISY                    = 0x00000006,
///   OVER_BRIGHT              = 0x00000007,
///
/// @param startFrame
///   Frame number since start of stream when tamper was detected.
//------------------------------------------------------------------------------
typedef struct _tamperResult
{
   bool isTamper;
   uint32_t tamperType;
   uint32_t startFrame;
}TamperResult;

//------------------------------------------------------------------------------
/// @brief
///    Definition of the Motion Tracker's Callback corresponding to the
///    SCVE::MotionTracker::Run_Async function. Not supported yet
//------------------------------------------------------------------------------

typedef void (*MotionTracker_Callback) ( SCVE::Status status,
                                         uint32_t* muiNumForegrounds,
                                         SCVE::ForegroundRect* pForegroundResults,
                                         SCVE::TamperResult* pTamperResult,
                                         SCVE::BackgroundSubResult* pBackgroundSubResult,
                                         void* pUserData);

//------------------------------------------------------------------------------
/// @brief
///    The abstract class for Motion Tracker.
///
/// @detail
///    MotionTracker is the abstract class that contains the definition of functions
/// related to motion-based object tracking. The user needs to call newInstance to intialize
/// a MotionTracker instance and call the corresponding virtual functions to
/// return the result, set consiguration parameters, etc.
//------------------------------------------------------------------------------
class SCVE_API MotionTracker
{
public:
    //------------------------------------------------------------------------------
    /// @brief
    ///    Constructor
    //------------------------------------------------------------------------------
    MotionTracker();

    //------------------------------------------------------------------------------
    /// @brief
    ///     the function creates an instance for Motion tracker and return it for
    /// the user to use.
    ///
    /// @param pScveContext
    ///     the context that is passed in which indicate the context the Motion
    /// Tracker will be working under.
    ///
    /// @param cbMotionTracker
    ///     a callback function defined by the user, the call back function will
    /// be called when an async Motion Tracker is called.
    ///
    /// @return
    ///     returns the pointer to the actual instance of Motion Tracker which
    ///  can be used by the user.
    //------------------------------------------------------------------------------
    static MotionTracker *newInstance(Context* pScveContext, MotionTracker_Callback    cbMotionTracker);

    static Status deleteInstance(MotionTracker *pInstance);

    //------------------------------------------------------------------------------
    /// @brief
    ///    Destructor
    //------------------------------------------------------------------------------
    virtual ~MotionTracker();

    //------------------------------------------------------------------------------
    /// @brief
    ///     perform Background subtraction syncronously.
    ///
    /// @param pImgIn
    ///     The input image.
    ///
    /// @param BackgroundSubResult
    ///     The Background Subtraction result. This is
    /// an optional parameter and can be used to get the
    /// background and foreground image. Please pass NULL
    /// if the background subtraction result is not required.
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status doBackgroundSubtraction_Sync(const SCVE::Image *pImgIn, SCVE::BackgroundSubResult *pBackgroundSubResult ) = 0;

    //------------------------------------------------------------------------------
    /// @brief
    ///     perform the Foreground detection/tracking syncronously.
    ///
    /// @param pImgIn
    ///     The input image.
    ///
    /// @param pForegroundResults
    ///     The output Foreground detect result.
    ///
    /// @param muiNumForegrounds
    ///     Number of objects detected in the foreground.
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status doForegroundTracking_Sync(const SCVE::Image *pImgIn, SCVE::ForegroundRect *pForegroundResults, uint32_t * muiNumForegrounds) = 0;

    //------------------------------------------------------------------------------
    /// @brief
    ///     perform the tamper detection syncronously.
    ///
    /// @param pImgIn
    ///     The input image.
    ///
    /// @param pTamperResult
    ///     The output tamper detect result
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status doTamperDetect_Sync(const SCVE::Image *pImgIn, SCVE::TamperResult *pTamperResult) = 0;

    //------------------------------------------------------------------------------
    /// @brief
    ///     reset the tamper detector status. clear previous results
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status resetTamperDetect() = 0;

    //------------------------------------------------------------------------------
    /// @brief
    ///     Set important configurations for tracker.
    ///
    /// @param _configParam
    ///     The configuration parameter structure.
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status setConfig(ConfigParam _configParam)=0;

    //------------------------------------------------------------------------------
    /// @brief
    ///     Get important configurations for tracker.
    ///
    /// @param _configParam
    ///     The configuration parameter structure.
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status getConfig(ConfigParam &_configParam)=0;

    //------------------------------------------------------------------------------
    /// @brief
    ///     Set unique sensitivity control for each vaMode. This function
    /// to be called immediately after setConfig()
    ///
    /// @param i
    ///     i equal to 0 through 100. Default is set to 10
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status setGlobalSensitivity(int i)=0;

    //------------------------------------------------------------------------------
    /// @brief
    ///     Set unique sensitivity control for camera tamper. This function
    /// to be called immediately after setConfig()
    ///
    /// @param i
    ///     i equal to 0 through 100. Default is set to 10
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status setTamperSensitivity(int i)=0;

    //------------------------------------------------------------------------------
    /// @brief
    ///     This is an optional API to set one exclusive zone according to pionts.
    /// Currently, we only create a bounding box covering all points. More precise
    /// function may be provided in a later version.
    ///
    /// @param Points
    ///     a pointer to points
    ///
    /// @param numPoints
    ///     number of points
    ///
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status setOneExclusiveZone(SCVE::Point * Points, int numPoints) = 0;

    //------------------------------------------------------------------------------
    /// @brief
    ///     Async version of Motion Tracker.
    ///
    /// @param muiNumForegrounds
    ///     Number of detected foreground objects.
    ///
    /// @param pForegroundResults
    ///     Pointer to memory allocated by user for storing foreground tracked results.
    ///
    /// @param pTamperResult
    ///     Pointer to memory allocated by user for storing tamper detection result.
    ///
    /// @param pBackgroundSubResult
    ///     Pointer to memory allocated by user for storing background subtraction result.
    ///
    /// @param pUserData
    ///     Memory allocated to store some userdata.
    ///
    /// @return
    ///     The error status.
    //------------------------------------------------------------------------------
    virtual Status Run_Async(const Image *pImgIn, uint32_t* muiNumForegrounds, ForegroundRect* pForegroundResults, TamperResult* pTamperResult, BackgroundSubResult* pBackgroundSubResult, void* pUserData)=0;
};

} //namespace SCVE
#endif //SCVE_MOTION_TRACKER_HPP
