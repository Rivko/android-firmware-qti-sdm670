// Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
#ifndef SECCAMLIB_H
#define SECCAMLIB_H

#include <stddef.h> // size_t
#include "object.h"

/** @ingroup seccamlib_data_types
 * Available error codes.*/
typedef enum {
    SECCAM_E_SUCCESS            = 0,    /**< Success. */
    SECCAM_E_FAIL               = 1,    /**< General error. */
    SECCAM_E_INVALID_PARAM      = 2,    /**< Invalid parameter given to an API function, e.g., NULL pointer. */
    SECCAM_E_SENSOR_ERROR       = 3,    /* Sensor-related error occurred; reserved for future use. */
    SECCAM_E_NOT_AVAILABLE      = 4,    /**< Camera Security API is not available for usage;
                                             possible reasons can be the API is already in use by another TA or is not available on the platform. */
    SECCAM_E_UNKNOWN_QSEE_COMMAND = 5,  /**< Received command ID is not handled by the camera security library. */
    SECCAM_E_LICENSE_ERROR        = 6,  /**< Haven license error occurred. */ /** @cond */
    SECCAM_E_ERR_SIZE = 0x7FFFFFFF,     /* Internal - not in customer documentation */
    /** @endcond */
} seccam_error_t;

/** Frame information. */
typedef struct {
    uint32_t    cam_id;       /**< Camera ID. */
    uint64_t    frame_number; /**< Frame number. */
    int64_t     time_stamp;   /**< Frame timestamp. */
} seccam_frame_info_t;

/* Image plane information.*/
typedef struct {
    uint32_t    offset;       /**< Plane offset from the beginning of the buffer in bytes*/
    uint32_t    row_stride;   /**< Plane row stride in bytes*/
    uint32_t    pixel_stride; /**< Plane pixel stride in bytes*/
} seccam_plane_info_t;

/* Maximum number of supported planes per image.*/
#define SECCAM_MAX_NUM_OF_PLANES (4)

/* Image buffer information as provided by HLOS.*/
typedef struct {
    uint32_t 	  width;         /**< Image width in pixels.*/
    uint32_t 	  height;        /**< Image height in pixels.*/
    uint32_t 	  stride;        /**< Buffer row stride in bytes.*/
    uint32_t 	  format;        /**< Image format (HAL format).*/
    uint32_t 	  num_of_planes; /**< Number of available planes in the image.*/
    seccam_plane_info_t planes[SECCAM_MAX_NUM_OF_PLANES]; /**< Image planes description.*/
} seccam_buffer_info_t;

/*---------------------------------------------------------------------------------*/
/*                        Callbacks definition                                     */
/*---------------------------------------------------------------------------------*/
/** @addtogroup seccamlib_functions 
@{ */
/**
 * Frame ready notification.
 *
 * This callback is called by the camera security library when the camera security frame buffer is
 * ready for processing by the TA. If secure preview was also configured for the camera by
 * the HLOS application, the preview frame buffer is provided, otherwise it is NULL.
 *
 * @param[in]   user_ctx        User-defined context as provided during initialization
 *                              (secCamInit()). Usually contains user application state
 *                              variables to be used by the callback implementation.
 * @param[in]   cam_id          Camera ID as provided by the HLOS framework. Note that there
 *                              is no guarantee on this parameter's validity because it is
 *                              forwarded directly from HLOS. The parameter is provided
 *                              for implementation.
 * @param[in]   frame_info      Camera frame information reported by the camera HLOS framework,
 *                              e.g., camera ID, frame number, and frame timestamp.
 * @param[in]   camera_buffer   Pointer to the protected camera buffer. The callback
 *                              implementation is expected to read the camera
 *                              frame data from this buffer.
 * @param[in]   camera_buf_size Size of the protected camera buffer. It is guaranteed that
 *                              the buffer access within the size boundaries is protected.
 * @param[in]   cam_buf_info    Camera frame buffer information provided by the HLOS framework.
 *                              The information includes resolution, stride and buffer format.
 * @param[in]   preview_buffer  Pointer to the protected display buffer. The callback
 *                              implementation is expected to write secure preview image
 *                              data to this buffer. If no secure preview was requested
 *                              for this camera, the buffer pointer is NULL.
 * @param[in]   preview_buf_size Size of the protected preview buffer. It is guaranteed
 *                              that the buffer access within the size boundaries is
 *                              protected.
 * @param[in]   preview_buf_inf Preview buffer information provided by the HLOS framework.
 *                              The information includes resolution, stride, and buffer format.
 * @param[in]   custom_req      Custom request data passed from HLOS, the data is not
 *                              processed by the library and is forwarded to the callback.
 *                              Can be NULL if no custom request data as provided.
 * @param[in]   custom_req_size Custom request buffer size.
 * @param[out]  custom_rsp      Custom response data to be returned to HLOS. Callback implementation
 *                              might fill a custom response to be processed by HLOS implementation.
 *                              Value can be NULL if no custom response was requested by HLOS.
 * @param[in]   custom_rsp_size Custom response buffer size.
 *
 * @detdesc
 * From a security perspective, this callback guarantees that the camera buffer is protected,
 * contains data originating from one of the device camera sensors, and was generated during
 * a current camera security session.
 * The preview buffer is guaranteed to contain display protected memory and any data written
 * to this buffer is accessible only by secure preview display hardware.
 * @par
 * There is no guarantee that buffers data content is valid and originates
 * from the specified camera sensor. It is up to implementation to check the
 * buffer content for validity.
 *
 * @return
 * SECCAM_E_SUCCESS                   -- Success. \n
 * Other seccam_error_t error value   -- Error.
 *
 * @dependencies
 * This callback must have been initialized by secCamInit() before it can be invoked
 * by the camera security library.
 *
 * @newpage 
 */
typedef seccam_error_t (*seccam_frame_event)(void* user_ctx, const seccam_frame_info_t* frame_info,
        uint8_t* camera_buffer, uint32_t cam_buf_size, const seccam_buffer_info_t* cam_buf_info,
        uint8_t* preview_buffer, uint32_t preview_buf_size, const seccam_buffer_info_t* preview_buf_info,
        const void* custom_req, uint32_t custom_req_size,
        void* custom_rsp, uint32_t custom_rsp_size);


/**
 * Notification of transition from non-secure to camera security state.
 *
 * This callback is invoked by the library after the camera security
 * transitions to secure state.
 *
 * @param[in]       user_ctx    User-defined context as provided during initialization
 *                              (secCamInit()). Usually contains user application state
 *                              variables to be used by the callback implementation.
 *
 * @detdesc
 * It is guaranteed that this callback is called only when camera security is
 * already in the secure state. TA can use this callback for custom protected hardware
 * initialization, e.g., turning ON an LED indicating that camera is in secure mode.
 *
 * @return
 * SECCAM_E_SUCCESS                   -- Success. \n
 * Other seccam_error_t error value   -- Error.
 *
 * @dependencies
 * This callback must have been initialized by secCamInit() before it can be invoked
 * by the camera security library.
 *
 * @newpage 
 */
typedef seccam_error_t (*seccam_transition_to_secure_event)(void* user_ctx);

/**
 * Notification of transition from secure to non-camera security state.
 *
 * This callback is invoked by the library before the camera security
 * transitions to nonsecure state.
 *
 * @param[in]       user_ctx    User-defined context as provided during initialization
 *                              (secCamInit()). Usually contains user application state
 *                              variables to be used by the callback implementation.
 *
 * @detdesc
 * It is guaranteed that this callback is called when camera security is still in the secure state.
 * TA can use this callback for custom protected hardware de-initialization, e.g.,
 * turning OFF an LED.
 *
 * @return
 * SECCAM_E_SUCCESS                   -- Success. \n
 * Other seccam_error_t error value   -- Error.
 *
 * @dependencies
 * This callback must have been initialized by secCamInit() before it can be invoked
 * by the camera security library.
 *
 * @newpage 
 */
typedef seccam_error_t (*seccam_transition_to_non_secure_event)(void* user_ctx);
/** @} */ /* end_addtogroup seccamlib_functions */
/** @cond */
//reserved for future use
typedef seccam_error_t (*seccam_phy_activation_event)(void* user_ctx, uint32_t phy_id);
//reserved for future use
typedef seccam_error_t (*seccam_phy_deactivation_event)(void* user_ctx, uint32_t phy_id);
/** @endcond */


/** @ingroup seccamlib_data_types 
 * Callback pointers to be initialized with user-defined functions. */
typedef struct {
    seccam_frame_event                          frame_event_cb;              /**< Callback for frame ready event notification. */
    seccam_transition_to_secure_event           transition_to_secure_cb;     /**< Notification callback on transition to secure state. */
    seccam_transition_to_non_secure_event       transition_to_non_secure_cb; /**< Notification callback on transition to non-secure state. *//** @cond */
    seccam_phy_activation_event                 phy_activation_cb;           //reserved for future use
    seccam_phy_deactivation_event               phy_deactivation_cb;         //reserved for future use /** @endcond */
} seccam_callbacks_t;


/*---------------------------------------------------------------------------------*/
/*                     API functions definition                                    */
/*---------------------------------------------------------------------------------*/

/** @addtogroup seccamlib_functions 
@{ */
/**
 * Camera Security API initialization.
 *
 * @param[in]   user_ctx         User-defined context to be forwarded to user
 *                               callback implementation. Can be NULL if not needed.
 * @param[in]   callbacks        Structure containing pointers to user implementation
 *                               of the callbacks.
 * @param[in]   callbacks_size   Size of the callback structure passed to the function.
 * @param[in]   licenseCert      Haven license certificate for camera security in DER format.
 * @param[in]   licenseCert_size Certificate size in bytes.
 *
 * @detdesc
 * The user can provide a custom context pointer to be forwarded to callbacks when
 * invoked by the library. The callback structure must be initialized with pointers to user
 * callback implementations. Currently, only frame_event_cb callback is required.
 * Other callbacks pointers can be initialized to NULL if not needed.
 * @par
 * After calling this function from the TA, it is guaranteed that no other TA can
 * initialize or use the camera security library functions.
 *
 * @return
 * SECCAM_E_SUCCESS                   -- Success. \n
 * Other seccam_error_t error value   -- Error.
 *
 * @dependencies
 * This function must be called before any other camera security library function is used by the TA.
 *
 * @newpage 
 */
seccam_error_t secCamInit(void* user_ctx, const seccam_callbacks_t* callbacks, uint32_t callbacks_size,
                          const uint8_t* licenseCert, size_t licenseCert_size);


/**
 * Determines whether a command is intended for the camera security library, and if so, processes it.
 *
 * If this function returns SECCAM_E_SUCCESS, the command was successfully processed by
 * the library and must not be processed further by the TA.
 *
 * @param[in]   req      Request from the HLOS client application.
 * @param[in]   cmdlen   Length (in bytes) of the request buffer.
 * @param[out]  rsp      Response buffer.
 * @param[in]   rsplen   Length (in bytes) of the response buffer.
 *
 * @return
 * SECCAM_E_SUCCESS                 -- Command was handled successfully.
 * @par
 * SECCAM_E_UNKNOWN_QSEE_COMMAND    -- Command is not handled by the camera security library.
 * @par
 * SECCAM_E_INVALID_PARAM           -- Invalid parameters passed.
 *
 * @dependencies
 * This function must always be called by the TA command handler (tz_app_cmd_handler).
 *
 * @newpage 
 */
seccam_error_t secCamProcessCommand(void* req, uint32_t cmdlen, void* rsp, uint32_t rsplen);


/**
 * Gets camera security session identifier.
 *
 * @param[out]   sessionId      Unique camera security session identifier, or zero.
 *
 * @detdesc
 * If there is no camera security session in progress, the sessionId is always set to zero (0).
 * If there is an active camera security session during this call, the sessionId is filled
 * with a unique non-zero session identifier.
 * @par
 * The secure session ID is changed each time the camera subsystem transitions to the secure state
 * and keeps the same value until the transition back to the non-secure state occurs.
 * @par
 * TA can use this function to detect changes in camera security session.
 *
 * @return
 * SECCAM_E_SUCCESS                   -- Success in getting the session ID. The sessionId parameter
 *                                       contains a valid value.
 * @par
 * Other seccam_error_t error value   -- Error while getting the session ID.
 *                                       The sessionId value is undefined.
 *
 * @newpage 
 */
seccam_error_t secCamGetSecureSesionID(uint32_t* sessionId);


/**
 * Gets camera security session status.
 *
 * @param[out]  is_secured   Set to  a non-zero value (TRUE) if camera security session
 *                           is active; otherwise 0 (FALSE).
 *
 * @detdesc
 * This function sets the is_secured parameter to a non-zero value (TRUE) if a camera
 * security session is active at the time of the call.
 * @par
 * When the camera subsystem is in the secure state, it is a guaranteed that any of the camera
 * sensors can only stream their data to protected memory.
 * @par
 * TA can use this function to query the session state before using features that require
 * the camera subsystem to be in the secure state.
 *
 * @return
 * SECCAM_E_SUCCESS                   -- Success in getting the camera state.
 *                                       The is_secured parameter contains a valid value.
 * @par
 * Other seccam_error_t error value   -- Error while getting the camera state.
 *                                       The is_secured value is undefined.
 *
 * @dependencies
 * secCamInit() must have been called before calling this function.
 *
 */
seccam_error_t secCamGetSecureStatus(uint32_t* is_secured);


/**
 * Releases the camera security library resources.
 *
 * After calling this function, the camera security library functions are not accessible and can be used
 * again only after secCamInit() is called.
 *
 * If a camera security session is in progress, this function takes no action and returns
 * with an error.
 *
 * @return
 * SECCAM_E_SUCCESS                   -- Successfully released the camera security library. \n
 * Other seccam_error_t error value   -- Error occurred during the library shutdown attempt.
 *
 * @dependencies
 * secCamInit() must have been called before calling this function.
 */
seccam_error_t secCamShutdown(void);


/**
 * Copy frame data from a camera buffer to a preview buffer.
 *
 * @param[in]   camera_buffer    Pointer to the protected camera buffer.
 * @param[in]   cam_buf_size     Size of the protected camera buffer.
 * @param[in]   cam_buf_info     Camera frame buffer information provided by the HLOS framework.
 *                               The information includes resolution, stride, and buffer format. 
 * @param[in]   preview_buffer   Pointer to the protected display buffer.
 * @param[in]   preview_buf_size Size of the protected preview buffer.
 * @param[in]   preview_buf_info Preview buffer information provided by the HLOS framework.
 *                               The information includes resolution, stride, and buffer format.
 * @detdesc
 * This function checks source and destination buffers composition and performs copy in
 * the following cases:
 *      - Both camera and preview buffers have the same size, format, width, and height.
 *      - Camera buffer format is SECCAM_YUV_420_888, preview format is SECCAM_FORMAT_NV12,
 *        and buffers have same width and height. In this case, the camera buffer UV plane is realigned
 *        to NV12 format during copy.
 *
 * @return
 * SECCAM_E_SUCCESS                   -- The frame is successfully copied to the preview buffer. \n
 * Other seccam_error_t error value   -- Error occurred during copy.
 */
seccam_error_t secCamCopyPreviewFrame(
        const uint8_t* camera_buffer, uint32_t cam_buf_size, const seccam_buffer_info_t* cam_buf_info,
        uint8_t* preview_buffer, uint32_t preview_buf_size, const seccam_buffer_info_t* preview_buf_info);


/** @} */ /* end_addtogroup seccamlib_functions */

#endif
