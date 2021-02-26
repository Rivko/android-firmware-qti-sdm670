////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxtrace.h
/// @brief Trace related definitions and prototypes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXTRACE_H
#define CAMXTRACE_H

#if CAMX_TRACES_ENABLED
#define ATRACE_TAG (ATRACE_TAG_CAMERA | ATRACE_TAG_HAL)
#include <cutils/trace.h>
#endif // CAMX_TRACES_ENABLED
#include <stdarg.h>

#include "camxdefs.h"
#include "camxosutils.h"

CAMX_NAMESPACE_BEGIN

/// @brief Debug trace settings
extern CamxTraceInfo g_traceInfo;

// Event UID
const UINT32 SCOPEEventTest = 0; ///< Test
const UINT32 SCOPEEventHAL3Open = 1; ///< HAL3: open
const UINT32 SCOPEEventHAL3GetTagCount = 2; ///< HAL3: get_tag_count
const UINT32 SCOPEEventHAL3GetAllTags = 3; ///< HAL3: get_all_tags
const UINT32 SCOPEEventHAL3GetSectionName = 4; ///< HAL3: get_section_name
const UINT32 SCOPEEventHAL3GetTagName = 5; ///< HAL3: get_tag_name
const UINT32 SCOPEEventHAL3GetTagType = 6; ///< HAL3: get_tag_type
const UINT32 SCOPEEventHAL3GetNumberOfCameras = 7; ///< HAL3: get_number_of_cameras
const UINT32 SCOPEEventHAL3GetCameraInfo = 8; ///< HAL3: get_camera_info
const UINT32 SCOPEEventHAL3SetCallbacks = 9; ///< HAL3: set_callbacks
const UINT32 SCOPEEventHAL3GetVendorTagOps = 10; ///< HAL3: get_vendor_tag_ops
const UINT32 SCOPEEventHAL3OpenLegacy = 11; ///< HAL3: open_legacy
const UINT32 SCOPEEventHAL3SetTorchMode = 12; ///< HAL3: set_torch_mode
const UINT32 SCOPEEventHAL3Init = 13; ///< HAL3: init
const UINT32 SCOPEEventHAL3Close = 14; ///< HAL3: close
const UINT32 SCOPEEventHAL3Initialize = 15; ///< HAL3: initialize
const UINT32 SCOPEEventHAL3ConfigureStreams = 16; ///< HAL3: configure_streams
const UINT32 SCOPEEventHAL3ConstructDefaultRequestSettings = 17; ///< HAL3: construct_default_request_settings
const UINT32 SCOPEEventHAL3ProcessCaptureRequest = 18; ///< HAL3: process_capture_request
const UINT32 SCOPEEventHAL3Dump = 19; ///< HAL3: dump
const UINT32 SCOPEEventHAL3Flush = 20; ///< HAL3: flush
const UINT32 SCOPEEventHAL3CameraDeviceStatusChange = 21; ///< HAL3: camera_device_status_change
const UINT32 SCOPEEventHAL3TorchModeStatusChange = 22; ///< HAL3: torch_mode_status_change
const UINT32 SCOPEEventHAL3ProcessCaptureResult = 23; ///< HAL3: process_capture_result
const UINT32 SCOPEEventHAL3Notify = 24; ///< HAL3: notify
const UINT32 SCOPEEventThreadCoreProcessJobQueue = 25; ///< ThreadCore: ProcessJobQueue
const UINT32 SCOPEEventThreadCoreDispatchJob = 26; ///< ThreadCore: DispatchJob
const UINT32 SCOPEEventThreadCoreOnJobStopped = 27; ///< ThreadCore: OnJobStopped
const UINT32 SCOPEEventThreadCoreWait = 28; ///< ThreadCore: Wait
const UINT32 SCOPEEventCSLInitialize = 29; ///< CSL: Initialize
const UINT32 SCOPEEventCSLUninitialize = 30; ///< CSL: Uninitialize
const UINT32 SCOPEEventCSLOpen = 31; ///< CSL: Open
const UINT32 SCOPEEventCSLClose = 32; ///< CSL: Close
const UINT32 SCOPEEventCSLQueryCameraPlatform = 33; ///< CSL: QueryCameraPlatform
const UINT32 SCOPEEventCSLImageSensorProbe = 34; ///< CSL: ImageSensorProbe
const UINT32 SCOPEEventCSLEnumerateDevices = 35; ///< CSL: EnumerateDevices
const UINT32 SCOPEEventCSLQueryDeviceCapabilities = 36; ///< CSL: QueryDeviceCapabilities
const UINT32 SCOPEEventCSLAcquireDevice = 37; ///< CSL: AcquireDevice
const UINT32 SCOPEEventCSLReleaseDevice = 38; ///< CSL: ReleaseDevice
const UINT32 SCOPEEventCSLStreamOn = 39; ///< CSL: StreamOn
const UINT32 SCOPEEventCSLStreamOff = 40; ///< CSL: StreamOff
const UINT32 SCOPEEventCSLSubmit = 41; ///< CSL: Submit
const UINT32 SCOPEEventCSLFlush = 42; ///< CSL: Flush
const UINT32 SCOPEEventCSLRegisterMessageHandler = 43; ///< CSL: RegisterMessageHandler
const UINT32 SCOPEEventCSLAlloc = 44; ///< CSL: Alloc
const UINT32 SCOPEEventCSLMapBuffer = 45; ///< CSL: MapBuffer
const UINT32 SCOPEEventCSLGetBufferInfo = 46; ///< CSL: GetBufferInfo
const UINT32 SCOPEEventCSLBufferCacheOp = 47; ///< CSL: BufferCacheOp
const UINT32 SCOPEEventCSLReleaseBuffer = 48; ///< CSL: ReleaseBuffer
const UINT32 SCOPEEventCSLCreatePrivateFence = 49; ///< CSL: CreatePrivateFence
const UINT32 SCOPEEventCSLCreateNativeFence = 50; ///< CSL: CreateNativeFence
const UINT32 SCOPEEventCSLMergeFences = 51; ///< CSL: MergeFences
const UINT32 SCOPEEventCSLGetFenceAttrib = 52; ///< CSL: GetFenceAttrib
const UINT32 SCOPEEventCSLFenceWait = 53; ///< CSL: FenceWait
const UINT32 SCOPEEventCSLFenceWaitMultiple = 54; ///< CSL: FenceWaitMultiple
const UINT32 SCOPEEventCSLFenceAsyncWait = 55; ///< CSL: FenceAsyncWait
const UINT32 SCOPEEventCSLFenceAsyncCancel = 56; ///< CSL: FenceAsyncCancel
const UINT32 SCOPEEventCSLFenceSignal = 57; ///< CSL: FenceSignal
const UINT32 SCOPEEventCSLReleaseFence = 58; ///< CSL: ReleaseFence
const UINT32 SCOPEEventHAL3DeviceInitializeHAL3Stream = 59; ///< HAL3Device: InitializeHAL3Stream
const UINT32 SCOPEEventThreadManagerCreate = 60; ///< ThreadManager: Create
const UINT32 SCOPEEventThreadManagerDestroy = 61; ///< ThreadManager: Destroy
const UINT32 SCOPEEventMetadataPoolCreate = 62; ///< MetadataPool: Create
const UINT32 SCOPEEventMetadataPoolDestroy = 63; ///< MetadataPool: Destroy
const UINT32 SCOPEEventTopologyCreate = 64; ///< Topology: Create
const UINT32 SCOPEEventTopologyDestroy = 65; ///< Topology: Destroy
const UINT32 SCOPEEventTopologyProcessRequest = 66; ///< Topology: ProcessRequest
const UINT32 SCOPEEventCSLFenceTrace = 67; ///< CSL: FenceTrace
const UINT32 SCOPEEventHAL3RequestTrace = 68; ///< HAL3: RequestTrace
const UINT32 SCOPEEventNodeProcessRequest = 69; ///< Node: ProcessRequest
const UINT32 SCOPEEventSensorNodeProcessingNodeInitialize = 70; ///< SensorNode: ProcessingNodeInitialize
const UINT32 SCOPEEventSensorNodeOnStreamOn = 71; ///< SensorNode: OnStreamOn
const UINT32 SCOPEEventSensorNodeExecuteProcessRequest = 72; ///< SensorNode: ExecuteProcessRequest
const UINT32 SCOPEEventVFE4NodeProcessingNodeInitialize = 73; ///< VFE4Node: ProcessingNodeInitialize
const UINT32 SCOPEEventVFE4NodeOnStreamOn = 74; ///< VFE4Node: OnStreamOn
const UINT32 SCOPEEventVFE4NodeExecuteProcessRequest = 75; ///< VFE4Node: ExecuteProcessRequest
const UINT32 SCOPEEventCSLMapNativeBuffer = 76; ///< CSL: MapNativeBuffer
const UINT32 SCOPEEventOsUtilsSleepMicroseconds = 77; ///< OsUtils: SleepMicroseconds
const UINT32 SCOPEEventOsUtilsMutexCreate = 78; ///< OsUtils: Mutex: Create
const UINT32 SCOPEEventOsUtilsMutexDestroy = 79; ///< OsUtils: Mutex: Destroy
const UINT32 SCOPEEventOsUtilsMutexWaitOnLock = 80; ///< OsUtils: Mutex: Wait on Lock
const UINT32 SCOPEEventOsUtilsMutexLock = 81; ///< OsUtils: Mutex: Lock
const UINT32 SCOPEEventOsUtilsMutexTryLock = 82; ///< OsUtils: Mutex: TryLock
const UINT32 SCOPEEventOsUtilsReadWriteLockCreate = 83; ///< OsUtils: ReadWriteLock: Create
const UINT32 SCOPEEventOsUtilsReadWriteLockDestroy = 84; ///< OsUtils: ReadWriteLock: Destroy
const UINT32 SCOPEEventOsUtilsReadWriteLockWaitOnReadLock = 85; ///< OsUtils: ReadWriteLock: Wait on Read Lock
const UINT32 SCOPEEventOsUtilsReadWriteLockWaitOnWriteLock = 86; ///< OsUtils: ReadWriteLock: Wait on Write Lock
const UINT32 SCOPEEventOsUtilsReadWriteLockLock = 87; ///< OsUtils: ReadWriteLock: Lock
const UINT32 SCOPEEventOsUtilsSemaphoreCreate = 88; ///< OsUtils: Semaphore: Create
const UINT32 SCOPEEventOsUtilsSemaphoreDestroy = 89; ///< OsUtils: Semaphore: Destroy
const UINT32 SCOPEEventOsUtilsSemaphoreWait = 90; ///< OsUtils: Semaphore: Wait
const UINT32 SCOPEEventOsUtilsSemaphoreSignal = 91; ///< OsUtils: Semaphore: Signal
const UINT32 SCOPEEventOsUtilsConditionCreate = 92; ///< OsUtils: Condition: Create
const UINT32 SCOPEEventOsUtilsConditionDestroy = 93; ///< OsUtils: Condition: Destroy
const UINT32 SCOPEEventOsUtilsConditionWait = 94; ///< OsUtils: Condition: Wait
const UINT32 SCOPEEventOsUtilsConditionSignal = 95; ///< OsUtils: Condition: Signal
const UINT32 SCOPEEventOsUtilsConditionBroadcast = 96; ///< OsUtils: Condition: Broadcast
const UINT32 SCOPEEventAutoFocusNodeProcessingNodeInitialize = 97; ///< AutoFocusNode: ProcessingNodeInitialize
const UINT32 SCOPEEventAutoFocusNodeExecuteProcessRequest = 98; ///< AutoFocusNode: ExecuteProcessRequest
const UINT32 SCOPEEventStatsProcessingNodeProcessingNodeInitialize = 99; ///< StatsProcessingNode: ProcessingNodeInitialize
const UINT32 SCOPEEventStatsProcessingNodeExecuteProcessRequest = 100; ///< StatsProcessingNode: ExecuteProcessRequest
const UINT32 SCOPEEventCAECStatsProcessorExecuteProcessRequest = 101; ///< CAECStatsProcessor: ExecuteProcessRequest
const UINT32 SCOPEEventCAFStatsProcessorExecuteProcessRequest = 102; ///< CAFStatsProcessor: ExecuteProcessRequest
const UINT32 SCOPEEventCAWBStatsProcessorExecuteProcessRequest = 103; ///< CAWBStatsProcessor: ExecuteProcessRequest
const UINT32 SCOPEEventCASDStatsProcessorExecuteProcessRequest = 104; ///< CASDStatsProcessor: ExecuteProcessRequest
const UINT32 SCOPEEventCSLSetDebugBuffer = 105; ///< CSL: SetDebugBuffer
const UINT32 SCOPEEventAECConvergence = 106; ///< AEC: Convergence
const UINT32 SCOPEEventAFInitialize = 107; ///< AutoFocus: Initialize
const UINT32 SCOPEEventAFStatusUpdate = 108; ///< AutoFocus: Status update
const UINT32 SCOPEEventAFLensPositionUpdate = 109; ///< AutoFocus: Final Lens Position
const UINT32 SCOPEEventAFResult = 110; ///< AutoFocus: AF Result
const UINT32 SCOPEEventAFDeInitialize = 111; ///< AutoFocus: DeInitialize
const UINT32 SCOPEEventSensorAcquire = 112; ///< SensorNode: Acquire
const UINT32 SCOPEEventSubModuleAcquire = 113; ///< SensorNode: SubModuleAcquire

static const UINT32 MaxTraceStringLength = 512;     ///< CamX internal trace message length limit

#if CAMX_TRACES_ENABLED

// The local name is just some random name that hopefully wont ever collide with some real local
#define CAMX_TRACE_SYNC_BEGIN_F(group, ...)                                                                                   \
    if (CamX::g_traceInfo.groupsEnable & group)                                                                               \
    {                                                                                                                         \
        CHAR traceNameRandomString[CamX::MaxTraceStringLength];                                                               \
        CamX::OsUtils::SNPrintF(traceNameRandomString, sizeof(traceNameRandomString), ##__VA_ARGS__);                         \
        ATRACE_BEGIN(traceNameRandomString);                                                                                  \
    }

#define CAMX_TRACE_ASYNC_BEGIN_F(group, id, ...)                                                                              \
    if (CamX::g_traceInfo.groupsEnable & group)                                                                               \
    {                                                                                                                         \
        CHAR traceNameRandomString[CamX::MaxTraceStringLength];                                                               \
        CamX::OsUtils::SNPrintF(traceNameRandomString, sizeof(traceNameRandomString), ##__VA_ARGS__);                         \
        ATRACE_ASYNC_BEGIN(traceNameRandomString, static_cast<UINT32>(id));                                                   \
    }

#define CAMX_TRACE_ASYNC_END_F(group, id, ...)                                                                                \
    if (CamX::g_traceInfo.groupsEnable & group)                                                                               \
    {                                                                                                                         \
        CHAR traceNameRandomString[CamX::MaxTraceStringLength];                                                               \
        CamX::OsUtils::SNPrintF(traceNameRandomString, sizeof(traceNameRandomString), ##__VA_ARGS__);                         \
        ATRACE_ASYNC_END(traceNameRandomString, static_cast<UINT32>(id));                                                     \
    }

#define CAMX_TRACE_INT32_F(group, value, ...)                                                                                 \
    if (CamX::g_traceInfo.groupsEnable & group)                                                                               \
    {                                                                                                                         \
        CHAR traceNameRandomString[CamX::MaxTraceStringLength];                                                               \
        CamX::OsUtils::SNPrintF(traceNameRandomString, sizeof(traceNameRandomString), ##__VA_ARGS__);                         \
        ATRACE_INT(traceNameRandomString, value);                                                                             \
    }

#define CAMX_TRACE_MESSAGE_F(group, ...)                                                                                      \
    if (CamX::g_traceInfo.groupsEnable & group)                                                                               \
    {                                                                                                                         \
        CHAR traceNameRandomString[CamX::MaxTraceStringLength];                                                               \
        CamX::OsUtils::SNPrintF(traceNameRandomString, sizeof(traceNameRandomString), ##__VA_ARGS__);                         \
        ATRACE_BEGIN(traceNameRandomString);                                                                                  \
        ATRACE_END();                                                                                                         \
    }

#define CAMX_TRACE_SYNC_BEGIN(group, pName)                                                                                   \
    if (CamX::g_traceInfo.groupsEnable & group)                                                                               \
    {                                                                                                                         \
        ATRACE_BEGIN(pName);                                                                                                  \
    }

#define CAMX_TRACE_SYNC_END(group)                                                                                            \
    if (CamX::g_traceInfo.groupsEnable & group)                                                                               \
    {                                                                                                                         \
        ATRACE_END();                                                                                                         \
    }

#define CAMX_TRACE_MESSAGE(group, pString)                                                                                    \
    if (CamX::g_traceInfo.groupsEnable & group)                                                                               \
    {                                                                                                                         \
        ATRACE_BEGIN(pString);                                                                                                \
        ATRACE_END();                                                                                                         \
    }

#else

#define CAMX_TRACE_SYNC_BEGIN_F(...)
#define CAMX_TRACE_ASYNC_BEGIN_F(...)
#define CAMX_TRACE_ASYNC_END_F(...)
#define CAMX_TRACE_INT32_F(...)
#define CAMX_TRACE_MESSAGE_F(...)
#define CAMX_TRACE_SYNC_BEGIN(...)
#define CAMX_TRACE_SYNC_END(...)
#define CAMX_TRACE_MESSAGE(...)

#endif // CAMX_TRACES_ENABLED

CAMX_NAMESPACE_END

#endif // CAMXTRACE_H
