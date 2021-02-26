////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxutils.h
/// @brief CHX utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXUTILS_H
#define CHXUTILS_H

#include "chxdefs.h"
#include <math.h>
#include "chituningmodeparam.h"

/// @brief Bitmask of logs to enable: Bit 0 - Error, Bit 1 - Warn, Bit 2 - Info, Bit 3 - Debug
extern UINT32 g_enableChxLogs;

#if defined (_WIN32)    // This file for Win32 build only
#include <malloc.h>
#include <windows.h>
#define CHX_LOG
#define CHX_LOG_ERROR
#define CHX_LOG_INFO
#define CHX_LOG_WARN
#else
#include <stdlib.h>
#include <cutils/log.h>
#undef LOG_TAG
#define LOG_TAG "CHIUSECASE"

#define CHX_LOG_ERROR(fmt, args...)                                                         \
    if (1 == (g_enableChxLogs & 1))                                                         \
    {                                                                                       \
        ALOGE("%s:%d %s() " fmt "\n", CHX_FILENAME(__FILE__), __LINE__, __func__, ##args);  \
    }

#define CHX_LOG_WARN(fmt, args...)                                                          \
    if (2 == (g_enableChxLogs & 2))                                                         \
    {                                                                                       \
        ALOGW("%s:%d %s() " fmt "\n", CHX_FILENAME(__FILE__), __LINE__, __func__, ##args);  \
    }

#define CHX_LOG_INFO(fmt, args...)                                                          \
    if (4 == (g_enableChxLogs & 4))                                                         \
    {                                                                                       \
        ALOGI("%s:%d %s() " fmt "\n", CHX_FILENAME(__FILE__), __LINE__, __func__, ##args);  \
    }

#define CHX_LOG(fmt, args...)                                                               \
    if (8 == (g_enableChxLogs & 8))                                                         \
    {                                                                                       \
        ALOGD("%s:%d %s() " fmt "\n", CHX_FILENAME(__FILE__), __LINE__, __func__, ##args);  \
    }

#endif // WIN32

/// The directory from which to read configuration files
#if defined (CAMX_ANDROID_API) && (CAMX_ANDROID_API >= 28) // NOWHINE PR002 <- Win32 definition
static const CHAR ConfigFileDirectory[] = "/data/vendor/camera";
static const CHAR FileDumpPath[]        = "/data/vendor/camera";
#else
static const CHAR ConfigFileDirectory[] = "/data/misc/camera";
static const CHAR FileDumpPath[]        = "/data/misc/camera";
#endif // Android-P or later

/// Thread entry function type
typedef VOID* (*OSThreadFunc)(VOID* pArg);
/// @brief Function pointer type for a job function
typedef VOID* (*JobFunc)(VOID* pArg);
/// Dynamic library handle
typedef VOID* OSLIBRARYHANDLE;

#if defined (_LINUX)
#include <dlfcn.h>                  // dynamic linking
#include <errno.h>                  // errno
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>                 // posix_memalign, free
#include <string.h>                 // strlcat
#include <sys/mman.h>               // memory management
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>                 // library functions
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sync/sync.h>

/// Thread handle type
typedef pthread_t OSThreadHandle;
/// Mutex handle type
typedef pthread_mutex_t OSMutexHandle;
/// Native fence type
typedef INT NativeFence;
#elif defined (_WIN32)
/// Thread handle type
typedef HANDLE OSThreadHandle;
/// Mutex handle type
typedef CRITICAL_SECTION OSMutexHandle;
/// Native fence type
typedef VOID* NativeFence;

/// @brief This structure is passed to the OS thread create function. This gets passed back to the thread by the OS when it
///        calls the thread main function entry point registered during thread create.
struct OSThreadParams
{
    OSThreadFunc    threadEntryFunction;    ///< Function to execute in new thread
    VOID*           pThreadData;            ///< Data passed to threadEntryFunction()
};

#else
#error Unsupported target defined
#endif // defined(_LINUX) || defined(_WIN32)

/// @brief Information about one thread
struct PerThreadData
{
    UINT32          threadId;               ///< Logical thread number
    OSThreadHandle  hThreadHandle;          ///< Thread handle
    JobFunc         mainThreadFunc;         ///< Thread main entry function
    VOID*           pPrivateData;           ///< Private data to be interpreted by the thread
};

#define CHX_ASSERT  (VOID)
#define CHX_INLINE  __inline
#define INVALID_THREAD_HANDLE (-1)

static const INT InvalidNativeFence = -1;   ///< Invalid native hFence

/// @brief Information about android to native scene.
typedef struct _AndroidToNativeScene {
    camera_metadata_enum_android_control_scene_mode_t from;
    ChiModeSceneSubModeType to;
} AndroidToCamxScene;

/// @brief Information about android to native effect.
typedef struct _AndroidToNativeEffect {
    camera_metadata_enum_android_control_effect_mode_t from;
    ChiModeEffectSubModeType to;
} AndroidToCamxEffect;

static AndroidToCamxScene SceneMap[] =
{
    { ANDROID_CONTROL_SCENE_MODE_DISABLED,ChiModeSceneSubModeType::None },
    { ANDROID_CONTROL_SCENE_MODE_FACE_PRIORITY,ChiModeSceneSubModeType::FacePriority },
    { ANDROID_CONTROL_SCENE_MODE_ACTION,ChiModeSceneSubModeType::Action },
    { ANDROID_CONTROL_SCENE_MODE_PORTRAIT,ChiModeSceneSubModeType::Portrait },
    { ANDROID_CONTROL_SCENE_MODE_LANDSCAPE,ChiModeSceneSubModeType::Landscape },
    { ANDROID_CONTROL_SCENE_MODE_NIGHT,ChiModeSceneSubModeType::Night },
    { ANDROID_CONTROL_SCENE_MODE_NIGHT_PORTRAIT,ChiModeSceneSubModeType::NightPortrait },
    { ANDROID_CONTROL_SCENE_MODE_THEATRE,ChiModeSceneSubModeType::Theater },
    { ANDROID_CONTROL_SCENE_MODE_BEACH,ChiModeSceneSubModeType::Beach },
    { ANDROID_CONTROL_SCENE_MODE_SNOW,ChiModeSceneSubModeType::Snow },
    { ANDROID_CONTROL_SCENE_MODE_SUNSET,ChiModeSceneSubModeType::Sunset },
    // No STEADYPHOTO in ChiModeSceneSubModeType !!!
    { ANDROID_CONTROL_SCENE_MODE_STEADYPHOTO,ChiModeSceneSubModeType::None },
    { ANDROID_CONTROL_SCENE_MODE_FIREWORKS,ChiModeSceneSubModeType::Fireworks },
    { ANDROID_CONTROL_SCENE_MODE_SPORTS,ChiModeSceneSubModeType::Sports },
    { ANDROID_CONTROL_SCENE_MODE_PARTY,ChiModeSceneSubModeType::Party },
    { ANDROID_CONTROL_SCENE_MODE_CANDLELIGHT,ChiModeSceneSubModeType::CandleLight },
    { ANDROID_CONTROL_SCENE_MODE_BARCODE,ChiModeSceneSubModeType::Barcode },
    // No SPEED_VIDEO in ChiModeSceneSubModeType !!!
    { ANDROID_CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO,ChiModeSceneSubModeType::None },
    // No HDR in ChiModeSceneSubModeType !!!
    { ANDROID_CONTROL_SCENE_MODE_HDR, ChiModeSceneSubModeType::None },
    // No FACE_PRIORITY_LOW_LIGHT in ChiModeSceneSubModeType !!!
    { ANDROID_CONTROL_SCENE_MODE_FACE_PRIORITY_LOW_LIGHT,ChiModeSceneSubModeType::None },
};

static AndroidToCamxEffect EffectMap[] =
{
    { ANDROID_CONTROL_EFFECT_MODE_OFF, ChiModeEffectSubModeType::None },
    { ANDROID_CONTROL_EFFECT_MODE_MONO, ChiModeEffectSubModeType::Mono },
    { ANDROID_CONTROL_EFFECT_MODE_NEGATIVE, ChiModeEffectSubModeType::Negative },
    { ANDROID_CONTROL_EFFECT_MODE_SOLARIZE, ChiModeEffectSubModeType::Solarize },
    { ANDROID_CONTROL_EFFECT_MODE_SEPIA, ChiModeEffectSubModeType::Sepia },
    { ANDROID_CONTROL_EFFECT_MODE_POSTERIZE, ChiModeEffectSubModeType::Posterize },
    { ANDROID_CONTROL_EFFECT_MODE_WHITEBOARD, ChiModeEffectSubModeType::Whiteboard },
    { ANDROID_CONTROL_EFFECT_MODE_BLACKBOARD, ChiModeEffectSubModeType::Blackboard },
    { ANDROID_CONTROL_EFFECT_MODE_AQUA, ChiModeEffectSubModeType::Aqua },
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General portable mutex class implementation
///
/// Basic wrapping of OS mutex to provide abstraction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Mutex final
{
public:
    /// Static method to create an instance of Mutex
    static Mutex* Create();

    /// Method to delete an instance of Mutex
    VOID Destroy();

    /// Lock/acquire an Mutex
    VOID Lock();

    ///Try to acquire a Mutex lock
    CDKResult TryLock();

    /// Unlock/release an Mutex
    VOID Unlock();

    /// Get the native mutex handle, needed for other system calls which takes native handle
    OSMutexHandle* GetNativeHandle();

private:
    /// Default constructor for Mutex object
    Mutex() = default;

    /// Default destructor for Mutex object.
    ~Mutex() = default;

    /// Initialize a newly created Mutex object
    CDKResult Initialize();

    Mutex(const Mutex&) = delete;                ///< Disallow the copy constructor
    Mutex& operator=(const Mutex&) = delete;     ///< Disallow assignment operator

#if defined(_LINUX)
    pthread_mutex_t     m_mutex;                ///< (Linux) Mutex
    BOOL                m_validMutex;           ///< (Linux) Indicates if underlying mutex is valid
#else
    CRITICAL_SECTION    m_criticalSection;      ///< (Win32) Critical Section implementing Mutex
#endif // defined(_LINUX)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief General portable condition class implementation
///
/// Basic wrapping of OS condition variable to provide abstraction
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Condition final
{
public:
    /// Static method to create an instance of Condition
    static Condition* Create();

    /// Method to delete an instance of Semaphore
    VOID Destroy();

    /// Wait on the condition
    CDKResult Wait(OSMutexHandle* phMutex);

    /// Wait with timeout on the Condition
    CDKResult TimedWait(OSMutexHandle*  phMutex,
                        UINT            timeoutMilliseconds);

    /// Signal the condition
    VOID Signal();

    /// Broadcast the condition
    VOID Broadcast();

private:
    Condition()  = default;
    ~Condition() = default;

    /// Initialize a newly created Condition object
    CDKResult Initialize();

    Condition(const Condition&) = delete;                ///< Disallow the copy constructor
    Condition& operator=(const Condition&) = delete;     ///< Disallow assignment operator

#if defined(_LINUX)
    pthread_cond_t      m_conditionVar;                  ///< (Linux) Underlying conditional variable
    BOOL                m_validConditionVar;             ///< (Linux) Indicates if condVar is valid
#else
    CONDITION_VARIABLE  m_conditionVar;                  ///< (Windows) Underlying conditional variable
#endif // defined(_LINUX)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Utils
///
/// @brief General utility class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChxUtils
{
public:
    static UINT32 AlignGeneric32(UINT32 operand,
                                 UINT   alignment);

    static UINT32 EvenCeilingUINT32(UINT32 input);


    static BOOL IsBitSet(UINT32  number,
                         UINT32  bit);

    static UINT32 BitReset(UINT32  number,
                           UINT32  bit);

    static const CHAR* GetFileName(
        const CHAR* pFilePath);

    static VOID* Memcpy(VOID*       pDst,
                        const VOID* pSrc,
                        SIZE_T      numBytes);

    static VOID* Memset(VOID*  pDst,
                        INT    value,
                        SIZE_T numBytes);

    static VOID* Calloc(SIZE_T numBytes);

    static VOID Free(VOID* pMem);

    static  CDKResult ThreadCreate(OSThreadFunc    threadEntryFunction,
                                   VOID*           pThreadData,
                                   OSThreadHandle* phThread);

    static VOID ThreadTerminate(OSThreadHandle hThread);

    static VOID AtomicStoreU64(volatile UINT64* pVar,
                               UINT64           val);

    static UINT64 AtomicLoadU64(volatile UINT64* pVar);

    static VOID AtomicStoreU32(volatile UINT32* pVar,
                               UINT32           val);

    static UINT32 AtomicLoadU32(volatile UINT32* pVar);

    static VOID SleepMicroseconds(UINT microseconds);

    static VOID* LibGetAddr(OSLIBRARYHANDLE hLibrary,
                            const CHAR*     pProcName);

    static OSLIBRARYHANDLE LibMap(const CHAR* pLibraryName);

    static BOOL IsVendorTagPresent(const VOID* pMetadata, VendorTag tag);
    static BOOL IsLongExposureCapture(const VOID* pMetadata);
    static VOID GetVendorTagValue(const VOID* pMetadata,  VendorTag tag, VOID** ppData);
    static CDKResult SetVendorTagValue(VOID* pMetadata,  VendorTag tag, UINT dataCount, VOID* ppData);

    static CHX_INLINE DOUBLE AbsoluteFLOAT(
        FLOAT input)
    {
        return fabs(input);
    }

    static CHX_INLINE camera_metadata* GetMetadataType(VOID* pMetadata)
    {
        return static_cast<camera_metadata*>(pMetadata);
    }

    static INT MergeMetadata(VOID* pMetadata1, VOID* pMetadata2);

    static VOID* AllocateAppendMetaData(const VOID* pMetadata,UINT32  entry_count, UINT32  data_count);

    static VOID* AllocateCopyMetaData(const VOID* pSrcMetadata);

    /// Free Metadata.
    static VOID FreeMetaData(
        VOID* pMetadata);

    static camera_metadata_t* ResetMetadata(
        camera_metadata* pMetadata);

    /// Deep copy camera3_capture_request_t data
    static VOID DeepCopyCamera3CaptureRequest(
        const camera3_capture_request_t* pSrcReq,
        camera3_capture_request_t*       pDestReq);

    /// Get usecase from request
    static ChiModeUsecaseSubModeType GetUsecaseMode(
        camera3_capture_request_t* pRequest);

    /// Fill Tuning mode selection data.
    static VOID FillTuningModeData(
        VOID* pMetaData,
        camera3_capture_request_t* pRequest,
        UINT32              sensorModeIndex,
        UINT32*             pEffectModeValue,
        UINT32*             pSceneModeValue,
        UINT32*             pFeature1Value,
        UINT32*             pFeature2Value);

    /// Fill cameraid.
    static VOID FillCameraId(
        VOID* pMetaData,
        UINT32 cameraId);


    static VOID  WaitOnAcquireFence(
        const CHISTREAMBUFFER* pBuffer);

    /// SkipPreviewFrame
    static VOID SkipPreviewFrame(
        camera3_stream_buffer_t* pBuffer);

    /// Get scene mode from app
    static ChiModeSceneSubModeType GetSceneMode(
        VOID *pMetaData,
        UINT32* pSceneModeValue);
    /// Get ZSL mode from app
    static INT GetZSLMode(
        VOID *pMetaData);

    /// Set effect mode from app
    static VOID SetEffectMode(
        camera_metadata_t* pMetaData,
        VOID*              pData);


    /// Get effect mode from app
    static ChiModeEffectSubModeType GetEffectMode(
        VOID *pMetaData,
        UINT32* pEffectModeValue);

    /// Get feture1 for tuning mode.
    static ChiModeFeature1SubModeType GetFeature1Mode(
        VOID *pMetaData,
        UINT32* pFeature1Value);

    /// Get feature2 for tuning mode.
    static ChiModeFeature2SubModeType GetFeature2Mode(
        VOID *pMetaData,
        UINT32* pFeature2Value);
};

#define CHX_CALLOC   ChxUtils::Calloc
#define CHX_FREE     ChxUtils::Free
#define CHX_FILENAME ChxUtils::GetFileName

CHX_INLINE VOID* operator new(
    size_t numBytes,
    UINT   lineNum)
{
     CDK_UNUSED_PARAM(lineNum);
     return calloc(1, numBytes);
}

CHX_INLINE VOID* operator new[](
    size_t numBytes,
    UINT   lineNum)
{
    CDK_UNUSED_PARAM(lineNum);
    return calloc(1, numBytes);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Operator delete
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int del_count = 0;

CHX_INLINE VOID operator delete(
    VOID* pMem)    ///< Memory pointer
{
    free(pMem);
}

#define CHX_NEW     new(__LINE__)
#define CHX_DELETE  ChxUtils::Free

#endif // CHXUTILS_H
