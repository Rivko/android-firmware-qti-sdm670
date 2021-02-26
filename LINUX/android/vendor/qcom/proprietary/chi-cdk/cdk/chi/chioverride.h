////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
// Not a Contribution.
// Apache license notifications and license are retained
// for attribution purposes only.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chioverride.h
/// @brief CamX Hardware Interface (CHI) Override definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIOVERRIDE_H
#define CHIOVERRIDE_H

#include <hardware/camera3.h>
#include "camxcdktypes.h"
#include "chi.h"
#include "chicommon.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#pragma pack(push, 8)

/// @brief Callback functions implemented in the driver called by the CHI override module
typedef struct chi_hal_ops
{
    /// @brief Defines the prototype for the callback method to send completed capture results to the framework.
    void (*process_capture_result)(
        const camera3_device_t*,
        const camera3_capture_result_t*);

    /// @brief Defines the prototype for the asynchronous notification callback method from to the framework.
    void (*notify_result)(
        const camera3_device_t*,
        const camera3_notify_msg_t*);

} chi_hal_ops_t;

/// @brief Interface for callback functions to call from driver into the override module.
typedef struct chi_hal_callback_ops {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_get_num_cameras
    ///
    /// @brief  Get the number of cameras
    ///
    /// @param  [out]    numFwCameras        number of cameras to return to FW
    /// @param  [out]    numLogicalCameras   number of logical cameras
    ///
    /// @return Number of cameras
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void (*chi_get_num_cameras)(
        uint32_t* numFwCameras,
        uint32_t* numLogicalCameras);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_get_camera_info
    ///
    /// @brief  Get camera info for a camera id
    ///
    /// @return 0 if success
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int (*chi_get_camera_info)(
        int                 cameraId,
        struct camera_info* pCameraInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_initialize_override_session
    ///
    /// @brief  Callback for initializing an override session.
    ///
    /// The driver calls this function when the HAL receives configure_streams() from the framework. The override module
    /// should inspect the stream configuration and determine if it will use custom live stream pipeline, as specified by
    /// the module, or if it will use default pipeline as chosen by the driver. Default live stream pipelines are not
    /// supported with custom offline pipelines. Any memory required to manage per-session state can be allocated in this
    /// function. A private pointer to the state received as output from this function, is passed into the override module
    /// on every call to allow for multiple concurrent camera devices.
    ///
    /// @param  [in]    cameraId        Camera Id
    /// @param  [in]    camera3_device  Pointer to the camera3_device instance of Chi driver.
    /// @param  [in]    stream_config   Pointer to the details about the streams being processed.
    /// @param  [out]   override_config This is returned as TRUE only if the override
    ///                                 module wants to use custom pipeline(s) for the specified configuration.
    ///                                 If the override module wants to use the default functionality provided by the driver,
    ///                                 it must return FALSE.
    /// @param  [out]   priv            Double pointer to any internal state which is needed by override module to manage
    ///                                 per-session. This pointer is passed back into every chi_hal_callback_ops_t function,
    ///                                 and must be cast to the appropriate internal structure by override module. It is valid
    ///                                 to return NULL. This value must be NULL if the pipeline parameter is not set to a
    ///                                 valid pipeline handle.
    ///
    /// @return CDKResultSuccess, if success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*chi_initialize_override_session)(
        uint32_t                        cameraId,
        const camera3_device_t*         camera3_device,
        const chi_hal_ops_t*            hal_ops,
        camera3_stream_configuration_t* stream_config,
        bool*                           override_config,
        void**                          priv);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_finalize_override_session
    ///
    /// @brief  Callback for finalizing an override session.
    ///
    /// The driver calls this function after it finishes initializing itself at the end of config_streams(), only if the
    /// override module returned a valid pipeline handle in chi_initialize_override_session(). This callback allows the
    /// override module to create any offline pipelines that it might need to use for the use case. This function is
    /// optional, and the function pointer need not be specified in the chi_hal_callback_ops_t during the
    /// chi_hal_override_entry() call.
    ///
    /// @param  [in] camera3_device Pointer to the camera3_device instance of Chi driver.
    /// @param  [in] priv           Pointer to the private data for the module.
    ///
    /// @return CDKResultSuccess, if success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*chi_finalize_override_session)(
        const camera3_device_t* camera3_device,
        void*                   priv);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_teardown_override_session
    ///
    /// @brief  Tear down a session.
    ///
    /// The driver calls this function when the HAL receives configure_streams() from the framework with a different stream
    /// configuration. The old session is torn down before beginning a new session to avoid wasting system resources. Any
    /// memory that was allocated for the override session can be destroyed.
    ///
    /// @param  [in] camera3_device Pointer to the camera3_device instance of Chi driver.
    /// @param  [in] session        Handle of the session.
    /// @param  [in] priv           Pointer to the private data for the module.
    ///
    /// @return None.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void (*chi_teardown_override_session)(
        const camera3_device_t* camera3_device,
        uint64_t                session,
        void*                   priv);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_override_process_request
    ///
    /// @brief  Processes an override request.
    ///
    /// This function is called only when the Chi override module specifies a custom pipeline in chi_initialize_hal_override().
    /// When a custom pipeline is active, this function is called for every framework-request. The override module plugin
    /// assumes complete responsibility for processing this request and calling the notification functions for the filled
    /// buffers and metadata. When a framework-request is received by the Chi override module, it can modify the request,
    /// create additional requests for multiframe features, and forward these requests to the Chi layer in the driver. The
    /// request is submitted to the appropriate pipeline via Chi for processing by the driver.
    ///
    /// @param  [in] camera3_device     Pointer to the camera3_device instance of Chi driver.
    /// @param  [in] capture_request    Pointer to the details about the request.
    /// @param  [in] priv               Pointer to the private data for the module.
    ///
    /// @return ChiResuCDKResultSuccess, if success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*chi_override_process_request)(
        const camera3_device_t*     camera3_device,
        camera3_capture_request_t*  capture_request,
        void*                       priv);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_extend_open
    ///
    /// @brief  Allows override processing during HAL open call
    ///
    /// @param  [in]     cameraId           logical camera ID.
    /// @param  [in]     priv               Pointer to the private data for the module.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void (*chi_extend_open)(
        uint32_t    cameraId,
        void*       priv);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_extend_close
    ///
    /// @brief  Allows override processing during HAL close call
    ///
    /// @param  [in]     cameraId           logical camera ID.
    /// @param  [in]     priv               Pointer to the private data for the module.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void(*chi_extend_close)(
        uint32_t    cameraId,
        void*       priv);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_remap_camera_id
    ///
    /// @brief  Allows override to detect special camera IDs
    ///
    /// @param  [in]     frameworkCameraId  camera ID from the framework
    /// @param  [in]     mode               Type of remapping requested.
    ///
    /// @return logical camera ID.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t (*chi_remap_camera_id)(
        uint32_t            frameworkCameraId,
        CameraIdRemapMode   mode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_modify_settings
    ///
    /// @brief  Interface to allow various override-specific settings to be toggled.
    ///
    /// @param  [in]     priv         Pointer to the private data being consumed by the override.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void (*chi_modify_settings)(
        void*       priv);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_get_default_request_settings
    ///
    /// @brief  Get default request settings
    ///
    /// The override can provide any vendor tag specific request settings they want to get added to the default request
    /// settings
    ///
    /// @param  [in] cameraId           Camera Id
    /// @param  [in] requestTemplate    Request template
    /// @param  [in] settings           Pointer to the request settings filled in by the override
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID (*chi_get_default_request_settings)(
        uint32_t                  cameraId,
        int                       requestTemplate,
        const camera_metadata_t** settings);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// chi_override_flush
    ///
    /// @brief  Allows override processing during HAL flush call
    ///
    /// @param  [in] camera3_device     Pointer to the camera3_device instance of Chi driver.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void (*chi_override_flush)(
        const camera3_device_t*     camera3_device);

} chi_hal_callback_ops_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chi_hal_override_entry
///
/// @brief  Entry point called by the driver to initialize the HAL override module.
///
/// This function must be exported by every com.<vendor>.chi.override.so in order for driver to initialize the override
/// interface. This function is called during the camera server initialization, which occurs during HAL process start.
/// In addition to communicating the necessary function pointers between the driver and the override module, this allows the
/// override module the opportunity to do any initialization work that it would prefer to do at boot time. This could include
/// loading different .so files, which provide additional functionality. Anything done here must not be specific to a session,
/// and any global variables stored in the override module must be protected against multiple sessions accessing it at
/// the same time.
///
/// @param [in]     ops         Pointer to a structure that defines the CHI functions. The override module must save these
///                             function pointers in order to make calls into the driver.
/// @param [in,out] callbacks   Pointer to a structure that defines various callbacks that the driver needs to make into the
///                             override module. The override module must fill in these function pointers.
///
/// @return None.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDK_VISIBILITY_PUBLIC void chi_hal_override_entry(
    chi_hal_callback_ops_t* callbacks);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// chi_hal_query_vendertag
///
/// @brief  Entry point called by the driver to query vendor tags defined in CHI extension
///
/// This function is optional for every com.<vendor>.chi.override.so in order for driver to query the vendor tags from override.
/// This function is called during the camera server initialization, which occurs during HAL process start. This function should
/// only return the vendor tag information, no other operation is allowed at this point.
///
///
/// @param [in,out] p_query_vendor_tags   Pointer to a structure that defines the vendor tag information.
///
/// @return True if successful otherwise False.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDK_VISIBILITY_PUBLIC CDKResult chi_hal_query_vendertag(
    CHIQUERYVENDORTAG* p_query_vendor_tags);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIOVERRIDE_H
