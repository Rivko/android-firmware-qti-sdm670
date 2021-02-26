// Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
#include <stdint.h>
#include "stringl.h"
#include "qsee_log.h"
#include <qsee_timer.h>
#include "seccamlib.h"

////////////// SECURE MODE TRANSITION CALLBACKS ///////////////
seccam_error_t transition_to_secure_event(void* user_ctx)
{
    //setup anything which has to be initialized only AFTER secure camera session was activated
    //e.g: turning ON protected LED

    qsee_log(QSEE_LOG_MSG_HIGH, "transition_to_secure_event");

    return SECCAM_E_SUCCESS;
}

seccam_error_t transition_to_non_secure_event(void* user_ctx)
{
    //release everything requiring de-initialization BEFORE secure camera session will be completed.
    //e.g: turning OFF protected LED

    qsee_log(QSEE_LOG_MSG_HIGH, "transition_to_non_secure_event");

    return SECCAM_E_SUCCESS;
}

/////////////////// FRAME/PREVIEW CALLBACK ///////////////////
// DUMMY frame_event. Will not be called
seccam_error_t frame_event(void* user_ctx, const seccam_frame_info_t* frame_info,
        uint8_t* camera_buffer, uint32_t cam_buf_size, const seccam_buffer_info_t* cam_buf_info,
        uint8_t* preview_buffer, uint32_t preview_buf_size, const seccam_buffer_info_t* preview_buf_info,
        const void* custom_req, uint32_t custom_req_size,
        void* custom_rsp, uint32_t custom_rsp_size)
{
    seccam_error_t status = SECCAM_E_SUCCESS;

    qsee_log(QSEE_LOG_MSG_HIGH, "frame_event:");


    return status;
}

//=========================================================================
//  @brief
//    Add any app specific initialization code here
//    QSEE will call this function after secure app is loaded and
//    authenticated
//=========================================================================
void tz_app_init(void) {
    seccam_callbacks_t callbacks = {
        .frame_event_cb =      frame_event,
        .transition_to_non_secure_cb = transition_to_non_secure_event,
        .transition_to_secure_cb = transition_to_secure_event,
    };
    //uncomment below to enable all debug logs for this TA
    qsee_log_set_mask(qsee_log_get_mask() | QSEE_LOG_MSG_HIGH /* | QSEE_LOG_MSG_DEBUG */);

    qsee_log(QSEE_LOG_MSG_HIGH, "tz_app_init");

    //Valid Haven license for secure camera in DER format should be used in production
    const uint8_t haven_license[1024] = {0};

    seccam_error_t retval = secCamInit(NULL, &callbacks, sizeof(callbacks),
                                       haven_license, sizeof(haven_license));
    if (retval != SECCAM_E_SUCCESS) {
        qsee_log(QSEE_LOG_MSG_ERROR, "failed to initialize secure camera library %d", retval);
    }
}

//=========================================================================
//  @brief
//    App specific shutdown
//    App will be given a chance to shutdown gracefully
//=========================================================================
void tz_app_shutdown(void) {
    qsee_log(QSEE_LOG_MSG_HIGH, "tz_app_shutdown");
    seccam_error_t retval = secCamShutdown();
    if (retval != SECCAM_E_SUCCESS) {
        qsee_log(QSEE_LOG_MSG_ERROR, "failed to release secure camera library %d", retval);
    }
}

//=========================================================================
//  @brief
//    Dummy App command handler
//=========================================================================
void tz_app_cmd_handler(void* cmd_ptr, uint32_t cmdlen, void* rsp_ptr, uint32_t rsplen) {

        qsee_log(QSEE_LOG_MSG_DEBUG, "unknown cmd id received");
        return;
}
