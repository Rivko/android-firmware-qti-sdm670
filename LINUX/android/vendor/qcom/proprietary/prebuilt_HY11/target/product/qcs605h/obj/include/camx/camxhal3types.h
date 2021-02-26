////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxhal3types.h
/// @brief Declarations of common and wrapped HAL3 types.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXHAL3TYPES_H
#define CAMXHAL3TYPES_H

#include "camxcommontypes.h"
#include "camxdefs.h"
#include "camxtypes.h"

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Camera3CbOps;
struct Camera3Device;
struct Camera3StreamBufferSet;
struct HAL3CbData;
struct HwDevice;
struct HwModuleMethods;
struct VendorTagQueryOps;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// hardware/hardware.h Wrappers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief This structure redefines the hw_module_methods_t structure defined in hardware/hardware.h. Additional information
///        can be found there.
struct HwModuleMethods
{
    /// @brief Defines the prototype for the device open method
    INT (*Open)(const HwModule*, const CHAR*, HwDevice**);
};

/// @brief Callback methods for the HAL to call into the framework. This structure redefines the camera3_callback_ops_t
///        structure defined in hardware/camera3.h. Additional information can be found there.
struct Camera3CbOps
{
    /// @brief Defines the prototype for the callback method to send completed capture results to the framework.
    void (*ProcessCaptureResult)(const Camera3CbOps*, const Camera3CaptureResult*);

    /// @brief Defines the prototype for the asynchronous notification callback method from to the framework.
    void (*Notify)(const Camera3CbOps*, const Camera3NotifyMessage*);
};

/// @brief Install HAL3DeviceCb to Topology
typedef VOID* (*HAL3DeviceCbFunc)(VOID* pData);

/// @brief Overall callback structure used between Topology and HAL
struct HAL3CbData
{
    CbType  type;   ///< One of the different callback types

    union
    {
        CbPayloadError      error;      ///< Error payload
        CbPayloadAsync      async;      ///< Async payload
        CbPayloadMetadata   metadata;   ///< Metadata payload
        CbPayloadBuffer     buffer;     ///< Buffer payload
    } cbPayload;                        ///< One of the Callback payloads
};

CAMX_NAMESPACE_END

#endif // CAMXHAL3TYPES_H
