/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
=============================================================

                          EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ------------------------------------------
08/10/17   ak      Initial version
=============================================================*/
#ifndef SECCAM_IF_H
#define SECCAM_IF_H

#include <jni.h>
#include <stdlib.h>
#include <stdbool.h>
#include <utils/Errors.h>
#include <utils/Log.h>

using namespace android;

struct ANativeWindow;

#ifdef __cplusplus
    extern "C" {
#endif

#define SECCAM_MAX_NUM_OF_PLANES (4)
#define MLAPP_SERVER_SERVICE_ID 5000

typedef enum ml_app_if_cmd_id_t {
    ML_APP_IF_CMD_NONE = 56000,
    ML_APP_IF_CMD_GET_IF_VERSION, /* 1 */
    ML_APP_IF_CMD_FRAME_NOTIFICATION, /* 2 */
    ML_APP_IF_CMD_SECCAM_APP_INIT, /* 3 */
    ML_APP_IF_CMD_SECCAM_APP_DEINIT, /* 4 */
    ML_APP_IF_CMD_SECCAM_START, /* 5 */
    ML_APP_IF_CMD_SECCAM_STOP, /* 6 */
    ML_APP_IF_CMD_VENDOR_EXCHANGE_TIMESTAMP, /* 7 */
    ML_APP_IF_CMD_VENDOR_SIZE = 0x7FFFFFFF
} ml_app_if_cmd_id_t;

typedef enum ml_app_if_status_t {
    ML_APP_IF_STATUS_SUCCESS = 0,
    ML_APP_IF_STATUS_GENERAL_FAILURE = -1,
    ML_APP_IF_STATUS_INVALID_INPUT_PARAMS = -2,
    ML_APP_IF_STATUS_INVALID_SENSOR_ID = -3,
    ML_APP_IF_STATUS_BYPASS = -4,
    ML_APP_IF_STATUS_TIMEOUT = -5,

    ML_APP_IF_STATUS_RESET_DONE = 1,
    ML_APP_IF_STATUS_ERR_SIZE = 0x7FFFFFFF
} ml_app_if_status_t;

typedef enum tz_app_if_status_t {
    TZ_APP_IF_STATUS_SUCCESS = 0,
    TZ_APP_IF_STATUS_GENERAL_FAILURE = -1,
    TZ_APP_IF_STATUS_INVALID_INPUT_PARAMS = -2,
    TZ_APP_IF_STATUS_INVALID_SENSOR_ID = -3,
    TZ_APP_IF_STATUS_BYPASS = -4,
    TZ_APP_IF_STATUS_TIMEOUT = -5,

    TZ_APP_IF_STATUS_RESET_DONE = 1,
    TZ_APP_IF_STATUS_ERR_SIZE = 0x7FFFFFFF
} tz_app_if_status_t;

typedef enum seccam_if_source_type_t {
    SECCAM_IF_SOURCE_PREVIEW,
    SECCAM_IF_SOURCE_CAPTURE
} seccam_if_source_type_t;

typedef enum seccam_if_notif_action_t {
    SECCAM_IF_NOTIF_ACTION_CONTINUE,
    SECCAM_IF_NOTIF_ACTION_SKIP
} seccam_if_notif_action_t;

#pragma pack(push, seccam_if, 1)

typedef struct seccam_if_plane_info_t {
    uint32_t    offset;
    uint32_t    row_stride;
    uint32_t    pixel_stride;
} seccam_if_plane_info_t;

typedef struct seccam_if_buffer_t {
    uint64_t                buffer;
    uint32_t                size;
    uint32_t                width;
    uint32_t                height;
    uint32_t                stride;
    uint32_t                format;
    uint32_t                num_of_planes;
    seccam_if_plane_info_t  planes[SECCAM_MAX_NUM_OF_PLANES];
} seccam_if_buffer_t;

typedef struct seccam_if_frame_req_data_t {
    seccam_if_source_type_t source_id;
    int32_t                 cam_id;  //android camera id
    uint64_t                frame_number;
    int64_t                 time_stamp;
    seccam_if_buffer_t      in_buffer;
    seccam_if_buffer_t      out_buffer;
} seccam_if_frame_req_data_t;

typedef struct seccam_if_frame_rsp_data_t {
    uint32_t    duration;  // execution time in milliseconds
} seccam_if_frame_rsp_data_t;

#pragma pack(pop, seccam_if)

/*---------------------------------------------------------------------------------*/
/*                        Callbacks definition                                     */
/*---------------------------------------------------------------------------------*/
/**
 * Callback to notify that frame/s ready and a notification is about to be sent to TA.
 *
 * This callback is called by the secure camera library when the frame buffer/s from one or
 * more secure cameras is/are ready for processing by the TA. If secure preview was also
 * configured for the camera, the preview frame buffer is provided, otherwise it is NULL.
 *
 * @param[in]   surface_id      Surface ID associated with this callback.
 * @param[in]   frame_data      Array of the frame data structures.
 * @param[in]   len             Number of the frame data structures.
 * @param[out]  context         User-defined context to be provided as a parameter to
 *                              the Done notification.
 * @param[out]  custom_cmd      Custom command buffer to be sent to the TA along with
 *                              the frame notification.
 * @param[out]  custom_cmd_size Size of the custom command buffer.
 * @param[out]  custom_rsp_size Size of the custom response buffer to be attached to the TA
 *                              response.
 *
 * @par
 * Note that there is no guarantee that buffers data content is valid and that it originates
 * from the particular camera sensor. Therefore, it is up to implementation to check the
 * buffer content for validity.
 *
 * @return
 * SECCAM_IF_NOTIF_ACTION_CONTINUE    -- continue and send the frame/s to the TA
 * SECCAM_IF_NOTIF_ACTION_SKIP        -- skip TA access and release the allocated resources
 *
 * @dependencies
 * This callback must have been initialized by secCamEnableFrameCallback() before it can be
 * invoked by the secure camera library.
 */
typedef seccam_if_notif_action_t (*secCamWillSendFrameNotification_t)(
        uint64_t surface_id,
        seccam_if_frame_req_data_t* frame_data, uint32_t len,
        void** context,
        void** custom_cmd, uint32_t* custom_cmd_size,
        uint32_t* custom_rsp_size);

/**
 * Callback to notify that response to the frame ready notification is received.
 *
 * This callback is called by the secure camera library when the response to the frame
 * ready is received.
 *
 * @param[in]   surface_id      Surface ID associated with this callback.
 * @param[in]   ret             TA/ML return code(tz_app_if_status_t or ml_app_if_status_t).
 * @param[in]   rsp             Secure camera response structure.
 * @param[in]   context         User-defined context from the frame notification
 *                              callback.
 * @param[in]   custom_rsp      Custom response buffer. The buffer will be deallocated
 *                              right after exiting from the callback.
 *                              the frame notification.
 * @param[in]   custom_rsp_size Size of the custom response buffer.
 *
 * @return
 * SECCAM_IF_NOTIF_ACTION_CONTINUE    -- continue and release the allocated resources
 *
 * @dependencies
 * This callback must have been initialized by secCamEnableFrameCallback() before it can be
 * invoked by the secure camera library.
 */
typedef seccam_if_notif_action_t (*secCamDoneSendFrameNotification_t)(
        uint64_t surface_id,
        int32_t ret, seccam_if_frame_rsp_data_t* rsp,
        void* context,
        const void* custom_rsp, uint32_t custom_rsp_size);

/*---------------------------------------------------------------------------------*/
/*                     API functions definition                                    */
/*---------------------------------------------------------------------------------*/
/**
 * Start TA and get secure camera API version.
 *
 * @param[in]   ta_name         TA name.
 * @param[in]   buf_size        QSEE/QTEE buffer size.
 * @param[out]  if_version_maj  API version major.
 * @param[out]  if_version_min  API version minor.
 * @param[in]   is_2dfa            true only for 2dfa usecase.
 *
 * @return
 * OK(0)                  -- Success.
 * Other status_t value   -- Error.
 *
 * @dependencies
 * Only one TA can be opened at any given time.
 */
status_t secCamStartTzApp(const char* ta_name, int32_t buf_size,
        uint32_t* if_version_maj, uint32_t* if_version_min, bool is_2dfa);
/**
 * Close the TA opened in secCamStartTzApp()
 *
 * @return
 * OK(0)                  -- Success.
 * Other status_t value   -- Error.
 *
 * @dependencies
 * Only one TA can be opened at any given time.
 */
status_t secCamShutdownTzApp();

/**
 * Send command to the TA opened in secCamStartTzApp()
 *
 * @param[in]   cmd_req         Command buffer to be sent.
 * @param[in]   cmd_req_size    Command buffer size.
 * @param[in]   cmd_rsp         Response buffer.
 * @param[in]   cmd_rsp_size    Response buffer size.
 *
 * @return
 * OK(0)                  -- Success.
 * Other status_t value   -- Error.
 *
 * @dependencies
 * Only one TA can be opened at any given time.
 */
status_t secCamSendCmd(
        void* cmd_req, uint32_t cmd_req_size,
        void* cmd_rsp, uint32_t cmd_rsp_size);

/**
 * secCamStart() and ssecCamStop() are for triggering start and stop for 2dfa
 */
status_t secCamStart();

status_t secCamStop();

/**
 * Send modified command to the TA opened in secCamStartTzApp()
 *
 * @param[in]   cmd_req         Command buffer to be sent.
 * @param[in]   cmd_req_size    Command buffer size.
 * @param[in]   cmd_rsp         Response buffer.
 * @param[in]   cmd_rsp_size    Response buffer size.
 * @param[in]   ion_fd_info     List of the ION buffers associated
 *                              with the command.
 *
 * @return
 * OK(0)                  -- Success.
 * Other status_t value   -- Error.
 *
 * @dependencies
 * Only one TA can be opened at any given time.
 */
status_t secCamSendModifiedCmd(
        void* cmd_req, uint32_t cmd_req_size,
        void* cmd_rsp, uint32_t cmd_rsp_size,
        struct QSEECom_ion_fd_info* ion_fd_info);

/**
 * Create Surface Java object associated with designated secure camera.
 *
 * The surface is used as a capture surface.
 *
 * @param[in]   env                     JNI mandatory.
 * @param[in]   thiz                    JNI mandatory.
 * @param[in]   j_camera_id             Camera ID.
 * @param[in]   j_width                 Width of the capture buffer.
 * @param[in]   j_height                Height of the capture buffer.
 * @param[in]   j_format                Format of the capture buffer.
 * @param[in]   j_max_images            Max number of the images to be captured
 *                                      concurrently.
 * @param[in]   j_surface_info_object   Surface info structure. Includes Surface ID.
 *
 * @return
 * Java Surface object -- Success.
 * null                -- Error.
 *
 * @dependencies
 * The capture surface will be associated with the  TA opened in secCamStartTzApp.
 * Only one TA can be opened at any given time.
 *
 */
jobject secCamGetSecureCameraSurface(
        JNIEnv* env, jobject thiz,
        jint j_camera_id, jint j_width, jint j_height, jint j_format, jint j_max_images,
        jobject j_surface_info_object);

/**
 * Release all the resources assigned to the capture surface
 *
 * @param[in]   surface_id        Surface ID of the object to be released.
 *
 * @return
 * true -- Success.
 */
bool secCamReleaseSecureCameraSurface(uint64_t surface_id);

/**
 * Assign secure preview surface to the secure capture surface
 *
 * @param[in]   preview_native_vindow   Handler of the preview surface.
 * @param[in]   capture_surface_id      Surface ID of the capture surface.
 * @param[in]   width                   Width of the preview buffer and window.
 * @param[in]   height                  Height of the preview buffer and window.
 * @param[in]   format                  Format of the preview buffer and window.
 * @param[in]   rotation                Rotation of the preview window.
 * @param[in]   max_images              Max number of the images to be dequeued for preview
 *                                      concurrently.
 * @return
 * true -- Success.
 */
bool secCamSetSecurePreviewSurface(
        ANativeWindow* preview_native_vindow, uint64_t capture_surface_id,
        int32_t width, int32_t height, int32_t format, int32_t rotation, int32_t max_images);

/**
 * Disconnect preview surface from the capture surface
 *
 * @param[in]   native_vindow        Handler of the preview surface.
 * @param[in]   capture_surface_id   Surface ID of the capture surface.
 *
 * @return
 * true -- Success.
 */
bool secCamReleaseSecurePreviewSurface(
        ANativeWindow* native_vindow, uint64_t capture_surface_id);

/**
 * Set frame callbacks
 *
 * @param[in]   will_send_frame_notification  Frame ready and about to be sent callback.
 * @param[in]   done_send_frame_notification  The response to the frame notification
 *                                            is received callback.
 *
 * @return
 * true -- Success.
 */
bool secCamEnableFrameCallback(uint64_t surface_id,
        secCamWillSendFrameNotification_t will_send_frame_notification,
        secCamDoneSendFrameNotification_t done_send_frame_notification);

#ifdef __cplusplus
}
#endif

#endif //SECCAM_IF_H
