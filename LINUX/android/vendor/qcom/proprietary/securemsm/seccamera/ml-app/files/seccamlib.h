// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
#ifndef SECCAMLIB_H
#define SECCAMLIB_H

#include <stddef.h> // size_t
#include <stdint.h>
#include <sys/types.h>
//#include "cog-scbuf.h" //enable this on receiving cog-scbuf.h form cog
#include "sock_comm.h"

//start of camera internal command offsets
#define MLVM_APP_CMD_OFFSET 56000

/* Plane info not supported as of now.
* Maximum number of supported planes per image.*/
#define MLVM_APP_MAX_NUM_OF_PLANES (0)
/** @ingroup seccamlib_data_types
 * Available error codes.*/
typedef enum {
    MLVM_APP_E_SUCCESS            = 0,    /**< Success. */
    MLVM_APP_E_FAIL               = 1,    /**< General error. */
    MLVM_APP_E_INVALID_PARAM      = 2,    /**< Invalid parameter given to an API function, e.g., NULL pointer. */
    MLVM_APP_E_SENSOR_ERROR       = 3,    /* Sensor-related error occurred; reserved for future use. */
    MLVM_APP_E_NOT_AVAILABLE      = 4,    /**< Camera Security API is not available for usage;
                                             possible use could be when the lib is seperate bin. */
    MLVM_APP_E_UNKNOWN_COMMAND    = 5,    /**< Received command ID is not handled by the camera security library. */
    MLVM_APP_E_DEINIT_COMMAND     = 6,    /**< Received command id for STOPPING use case */
    MLVM_APP_E_SOCK               = 7,    /**< Socket error. */
    MLVM_APP_E_ERR_SIZE = 0x7FFFFFFF,     /* Internal - not in customer documentation */
    /** @endcond */
} mlvmapp_error_t;

//source camera definition as provided by HLOS side. Informational only.
typedef enum {
    SOURCE_CAPTURE = 1,
    SOURCE_PREVIEW = 0,
    SOURCE_ERR_SIZE = 0x7FFFFFFF,
} mlvmapp_source_type_t;

/*-------------------------------------------------------------------------*/
/* Messages IDs to use with the ML app */
typedef enum {
    MLVM_APP_CMD_NONE = MLVM_APP_CMD_OFFSET, /* 0  */
    MLVM_APP_CMD_GET_IF_VERSION, /* 1  */
    MLVM_APP_CMD_FRAME_NOTIFICATION, /* 2 */
    MLVM_APP_CMD_HLOS_APP_INIT, /* 3 */
    MLVM_APP_CMD_HLOS_APP_DEINIT, /* 4 */
    MLVM_APP_CMD_START, /* 5 */
    MLVM_APP_CMD_STOP, /* 6 */
    MLVM_APP_CMD_EXCHANGE_TIMESTAMP, /* 7 */
    MLVM_APP_CMD_LAST,
    MLVM_APP_CMD_ERR_SIZE = 0x7FFFFFFF,
} mlvmapp_cmd_id_t;

#pragma pack(push, seccam_if, 1)
//seccam command id. Expected to be a first int32 of any
//command request structure passed to the seccamlib.
typedef struct {
    uint32_t cmd_id;
} mlvmapp_generic_req_t;

//Response error code. Will be first int32 of any response structure returned to HLOS caller.
typedef struct {
    mlvmapp_error_t ret;
} mlvmapp_generic_rsp_t;

/*-----------------Structures used with MLVM_APP_CMD_EXCHANGE_TIMESTAMP ------------------*/
typedef struct mlvmapp_if_timestamp_send_cmd_t {
    uint32_t  cmd_id;
    uint64_t  cmd_data;
} mlvmapp_if_timestamp_send_cmd_t;

typedef struct mlvmapp_if_timestamp_send_cmd_rsp_t {
    uint32_t  ret;
    uint64_t  ret_data;
} mlvmapp_if_timestamp_send_cmd_rsp_t;

/*-----------------Structures used with MLVM_APP_CMD_GET_IF_VERSION ------------------*/

//version request command
typedef mlvmapp_generic_req_t mlvmapp_get_version_req_t;

/*-----------------Structures used with MLVM_APP_CMD_FRAME_NOTIFICATION ------------------*/

#define SECCAM_MAX_CAMERAS (1)

typedef struct {
    uint32_t    offset;       //plane offset from the beginning of the buffer in bytes
    uint32_t    row_stride;   //plane row stride in bytes
    uint32_t    pixel_stride; //plane pixel stride in bytes
} mlvmapp_plane_cmd_info_t;

//buffer address and frame parameters
typedef struct {
    uint32_t buffer_handle;    //physical address of the buffer
    uint32_t size;      //buffer size
    uint32_t width;     //frame width in pixels
    uint32_t height;    //frame height in pixels
    uint32_t stride;    //frame stride in pixels
    uint32_t format;    //frame format
} mlvmapp_ion_buffer_t;

//camera stream frame parameters
typedef struct {
    mlvmapp_source_type_t source_id;     //preview or capture
    int32_t cam_id;                     //android camera id
    uint64_t frame_number;              //frame number
    int64_t time_stamp;                 //timestamp
    mlvmapp_ion_buffer_t in_buffer;      //camera stream buffer
    mlvmapp_ion_buffer_t out_buffer;     //secure preview display buffer
} mlvmapp_frame_cmd_data_t;

typedef struct mlvmapp_plane_cmd_data_t {
    uint32_t                num_of_in_planes;   //number of planes in the input buffer
    mlvmapp_plane_cmd_info_t in_planes[MLVM_APP_MAX_NUM_OF_PLANES];
    uint32_t                num_of_out_planes;  //number of planes in the output buffer
    mlvmapp_plane_cmd_info_t out_planes[MLVM_APP_MAX_NUM_OF_PLANES];
} mlvmapp_plane_cmd_data_t;

//command structure for on-frame-available event
//- custom request data is passed as is by the library to the callback
//- custom response data is expected to be filled by the callback
//  and return back to HLOS caller
//  USAGE: This request struct should be passed to QSEECom_send_modified_cmd_64
//         along with offsets of ION buffers. For example:
//              ion_fd_info.data[0].cmd_buf_offset = offsetof(mlvmapp_frame_cmd_req_t, frame_data[0].in_buffer.buffer);
typedef struct  {
    uint32_t cmd_id;                //command id
    uint32_t custom_req_size;       //size of custom request data
    uint32_t custom_req_offset;     //offset of custom request data in the request buffer
    uint32_t custom_rsp_size;       //size of custom response buffer data
    uint32_t custom_rsp_offset;     //offset of custom response data in the response buffer
    mlvmapp_frame_cmd_data_t frame_data[SECCAM_MAX_CAMERAS]; //frame data which contains NULL buffer will be skipped

    //API - 2.1
    mlvmapp_plane_cmd_data_t  plane_data[SECCAM_MAX_CAMERAS];  //plane descriptions for the buffers. Mostly useful for YUV images.
} mlvmapp_frame_cmd_req_t;

typedef struct {
    uint32_t duration;  // execution time in milliseconds (for debug only)
} mlvmapp_frame_cmd_rsp_data_t;

//response structure for on-frame-available event
//USAGE: This response struct should be passed to send_response()
typedef struct {
    mlvmapp_error_t ret;                    //returned error
    mlvmapp_frame_cmd_rsp_data_t rsp_data;
} mlvmapp_frame_cmd_rsp_t;

/** Frame information. */
typedef struct {
    uint32_t    cam_id;       /**< Camera ID. */
    uint64_t    frame_number; /**< Frame number. */
    int64_t     time_stamp;   /**< Frame timestamp. */
} mlvmapp_frame_info_t;

/* Image plane information.*/
typedef struct {
    uint32_t    offset;       /**< Plane offset from the beginning of the buffer in bytes*/
    uint32_t    row_stride;   /**< Plane row stride in bytes*/
    uint32_t    pixel_stride; /**< Plane pixel stride in bytes*/
} mlvmapp_plane_info_t;


/* Image buffer information as provided by HLOS.*/
typedef struct {
    uint32_t 	  width;         /**< Image width in pixels.*/
    uint32_t 	  height;        /**< Image height in pixels.*/
    uint32_t 	  stride;        /**< Buffer row stride in bytes.*/
    uint32_t 	  format;        /**< Image format (HAL format).*/
    uint32_t 	  num_of_planes; /**< Number of available planes in the image.*/
    mlvmapp_plane_info_t planes[MLVM_APP_MAX_NUM_OF_PLANES]; /**< Image planes description.*/
} mlvmapp_buffer_info_t;
#pragma pack(pop, seccam_if)

/*---------------------------------------------------------------------------------*/
/*                     API functions definition                                    */
/*---------------------------------------------------------------------------------*/

/** @addtogroup seccamlib_functions
@{ */
/**
 * Frame ready notification.
 *
 * This function is called by the camera security library when the camera security frame buffer is
 * ready for processing by the ML App.
 *
 * @param[in]   frame_info      Camera frame information reported by the camera HLOS framework,
 *                              e.g., camera ID, frame number, and frame timestamp.
 * @param[in]   camera_buffer   Pointer to the protected camera buffer. The function
 *                              implementation is expected to read the camera
 *                              frame data from this buffer.
 * @param[in]   camera_buf_size Size of the protected camera buffer. It is guaranteed that
 *                              the buffer access within the size boundaries is protected.
 * @param[in]   cam_buf_info    Camera frame buffer information provided by the HLOS framework.
 *                              The information includes resolution, stride and buffer format.
 * @param[in]   custom_req      Custom request data passed from HLOS, the data is not
 *                              processed by the library and is forwarded as it is.
 *                              Can be NULL if no custom request data as provided.
 * @param[in]   custom_req_size Custom request buffer size.
 * @param[out]  custom_rsp      Custom response data to be returned to HLOS. Callback implementation
 *                              might fill a custom response to be processed by HLOS implementation.
 *                              Value can be NULL if no custom response was requested by HLOS.
 * @param[in]   custom_rsp_size Custom response buffer size.
 *
 * @detdesc
 * From a security perspective, this call guarantees that the camera buffer is protected,
 * contains data originating from one of the device camera sensors, and was generated during
 * a current camera security session.
 * @par
 * There is no guarantee that buffers data content is valid and originates
 * from the specified camera sensor. It is up to implementation to check the
 * buffer content for validity.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Success. \n
 * Other mlvmapp_error_t error value   -- Error.
 *
 * @newpage
 */
mlvmapp_error_t process_frame(const mlvmapp_frame_info_t* frame_info,
        uint8_t* camera_buffer, uint32_t cam_buf_size, const mlvmapp_buffer_info_t* cam_buf_info,
        const void* custom_req, uint32_t custom_req_size,
        void* custom_rsp, uint32_t custom_rsp_size);

/**
 * Camera Security API initialization.
 *
 *
 * @detdesc
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Success. \n
 * Other mlvmapp_error_t error value   -- Error.
 *
 * @dependencies
 * This function must be called before any other camera security library function is used by the App.
 *
 * @newpage
 */
mlvmapp_error_t secCamInit(void);


/**
 * If this function returns MLVM_APP_E_SUCCESS, the command was successfully processed by
 * the library.
 *
 * @param[in]   req      Request from the HLOS client application.
 * @param[in]   cmdlen   Length (in bytes) of the request buffer.
 * @param[out]  rsp      Response buffer.
 * @param[in]   rsplen   Length (in bytes) of the response buffer.
 *
 * @return
 * MLVM_APP_E_SUCCESS                 -- Command was handled successfully.
 * @par
 * MLVM_APP_E_INVALID_PARAM           -- Invalid parameters passed.
 *
 * @newpage
 */
mlvmapp_error_t processCommand(void* req, uint32_t cmdlen, void* rsp, uint32_t *rsplen);


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
 * MLVM_APP_E_SUCCESS                   -- Successfully released the camera security library. \n
 * Other mlvmapp_error_t error value   -- Error occurred during the library shutdown attempt.
 *
 * @dependencies
 * secCamInit() must have been called before calling this function.
 */
mlvmapp_error_t secCamShutdown(void);
/**
 * Notifies the hypervisor about the usecase start and get ack. Internally calls wait_for_hyp_notification().
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully sent the notification. \n
 * Other mlvmapp_error_t error value   -- Error occurred while sending notification.
 *
 * @dependencies
 * init_cmd_handler is needed for esablishing the connection with
 * hyp again
 */
mlvmapp_error_t start_cmd_handler(void *rcv_buf, uint32_t rcv_size, void *rsp_buf, uint32_t *rsp_size);

/**
 * Notifies the hypervisor about the usecase campletion. Internally calls notify_hyp_done().
 * After this call the secure buffers should no longer be used as it is unmapped by hypervisor.
 * Also the file discriptors are closed.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully sent the notification. \n
 * Other mlvmapp_error_t error value   -- Error occurred while sending notification.
 *
 * @dependencies
 * hyp_dev_open() or wait_for_hyp_notification() is needed for esablishing the connection with
 * hyp again
 */
mlvmapp_error_t stop_cmd_handler(void *rcv_buf, uint32_t rcv_size, void *rsp_buf, uint32_t *rsp_size);

/**
 * waits for Init command from HLOS. Can be used for app init before the use case starts.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully received the cmd. \n
 * Other mlvmapp_error_t error value   -- Error occurred.
 *
 */
mlvmapp_error_t wait_for_hlos_app_init_cmd(void);


/**
 * opens a device node for both hyp communication and mmap call.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully . \n
 * Other mlvmapp_error_t error value   -- Error occurred .
 *
 *
 */
mlvmapp_error_t hyp_dev_open(void);

/**
 * close a device node for both hyp communication and mmap call.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully . \n
 * Other mlvmapp_error_t error value   -- Error occurred .
 *
 *
 */
mlvmapp_error_t hyp_dev_close(void);

/**
 *
 * Notifies the hypervisor about the usecase campletion.
 * After this call the secure buffers should no longer be used as it is unmapped by hypervisor.
 * Also the file discriptors are closed.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully . \n
 * Other mlvmapp_error_t error value   -- Error occurred .
 *
 * @dependencies
 *
 */
mlvmapp_error_t notify_hyp_done(void);

/**
 * Blocks on map success notification from hypervisor. Receiving the notification
 * confirms the buffers are secured.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully . \n
 * Other mlvmapp_error_t error value   -- Error occurred .
 *
 * @dependencies
 *
 */
mlvmapp_error_t  wait_for_hyp_notification(void);

/**
 * pthread waiting on commands from HLOS. Exits on receiving MLVM_APP_CMD_SECCAM_STOP
 * from the HLOS seccamservice
 *
 */
void *hlos_rcv_cmds(void *arg);

/**
 * Initializes the socket communication for receiving commands from
 * seccamservice in HLOS. Here ML app behaves as a server and HLOS
 * seccamservice as client.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully . \n
 * Other mlvmapp_error_t error value   -- Error occurred .
 *
 * @dependencies
 *
 */
mlvmapp_error_t sock_init(void);

/**
 * Invoked on receiving MLVM_APP_CMD_HLOS_APP_INIT. This is before the hypervisor
 * sends a notification for map success. And there is no guarantee that the buffers
 * are secured.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully . \n
 * Other mlvmapp_error_t error value   -- Error occurred .
 *
 * @dependencies
 *
 */
mlvmapp_error_t init_cmd_handler(void *rcv_buf, uint32_t rcv_size, void *rsp_buf, uint32_t *rsp_size);

/**
 * Invoked on receiving MLVM_APP_CMD_HLOS_APP_DEINIT. This is after the buffers are
 * mapped back HLOS and the use case has completed. It is called along with TA unload.
 *
 * @return
 * MLVM_APP_E_SUCCESS                   -- Successfully . \n
 * Other mlvmapp_error_t error value   -- Error occurred .
 *
 * @dependencies
 *
 */
mlvmapp_error_t deinit_cmd_handler(void *rcv_buf, uint32_t rcv_size, void *rsp_buf, uint32_t *rsp_size);

/** @} */ /* end_addtogroup seccamlib_functions */

#endif
