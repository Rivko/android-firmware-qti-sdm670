// Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
#ifndef SECCAMLIB_EXPORTED_DEF_H
#define SECCAMLIB_EXPORTED_DEF_H
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// This file contains definition for HLOS/lib communication via QSEE
// The communication is handled by secure camera HLOS framework
// This file is provided as a reference only and is not intended for TA use
///////////////////////////////////////////////////////////////////////////////

//start of camera internal command offsets
#define SECCAM_CMD_OFFSET 56000

//current version of the library API
#define SECCAM_LIB_VERSION_MAJOR (0x2)
#define SECCAM_LIB_VERSION_MINOR (0x1)

// QSEE command IDs to use with the secure camera TA library
#define SECCAM_API_CMD_GET_IF_VERSION       (SECCAM_CMD_OFFSET + 1)
#define SECCAM_API_CMD_FRAME_NOTIFICATION   (SECCAM_CMD_OFFSET + 19)

//status returned as part of the response from secure camera library commands.
//on success the library uses SECCAM_STATUS_SUCCESS in the response struct
typedef enum {
    SECCAM_STATUS_SUCCESS = 0,
    SECCAM_STATUS_GENERAL_FAILURE = -1,
    SECCAM_STATUS_INVALID_INPUT_PARAMS = -2,
    SECCAM_STATUS_INVALID_SENSOR_ID = -3,
    SECCAM_STATUS_BYPASS = -4,
    SECCAM_STATUS_ERR_SIZE = 0x7FFFFFFF,
} seccam_status_t;

//source camera definition as provided by HLOS side. Informational only.
typedef enum {
    SECCAM_SOURCE_CAPTURE = 1,
    SECCAM_SOURCE_PREVIEW = 0,
    SECCAM_SOURCE_ERR_SIZE = 0x7FFFFFFF,
} seccam_source_type_t;

//structures need to be packed to be aligned between TZ/HLOS
#pragma pack(push, seccam, 1)

//QSEE command id. Expected to be a first int32 of any QSEE
//command request structure passed to the seccamlib.
typedef struct {
    uint32_t cmd_id;
} seccam_generic_req_t;

//Response status code. Will be first int32 of any response structure returned to HLOS caller.
typedef struct {
    seccam_status_t ret;
} seccam_generic_rsp_t;

/*-----------------Structures used with SECCAM_CMD_GET_IF_VERSION ------------------*/

//version request command
typedef seccam_generic_req_t seccam_get_version_req_t;

//version command response
typedef struct seccam_get_version_rsp_s {
    seccam_status_t ret;
    int32_t version_maj;
    int32_t version_min;
} seccam_get_version_rsp_t;


/*-----------------Structures used with SECCAM_CMD_FRAME_NOTIFICATION ------------------*/

#define SECCAM_MAX_CAMERAS (2)

typedef struct {
    uint32_t    offset;       //plane offset from the beginning of the buffer in bytes
    uint32_t    row_stride;   //plane row stride in bytes
    uint32_t    pixel_stride; //plane pixel stride in bytes
} seccam_plane_cmd_info_t;

#define SECCAM_MAX_NUM_OF_PLANES (4)

//buffer address and frame parameters
typedef struct {
    uint64_t buffer;    //physical address of the buffer
    uint32_t size;      //buffer size
    uint32_t width;     //frame width in pixels
    uint32_t height;    //frame height in pixels
    uint32_t stride;    //frame stride in pixels
    uint32_t format;    //frame format
} seccam_ion_buffer_t;

//camera stream frame parameters
typedef struct {
    seccam_source_type_t source_id;     //preview or capture
    int32_t cam_id;                     //android camera id
    uint64_t frame_number;              //frame number
    int64_t time_stamp;                 //timestamp
    seccam_ion_buffer_t in_buffer;      //camera stream buffer
    seccam_ion_buffer_t out_buffer;     //secure preview display buffer
} seccam_frame_cmd_data_t;

typedef struct seccam_plane_cmd_data_t {
    uint32_t                num_of_in_planes;   //number of planes in the input buffer
    seccam_plane_cmd_info_t in_planes[SECCAM_MAX_NUM_OF_PLANES];
    uint32_t                num_of_out_planes;  //number of planes in the output buffer
    seccam_plane_cmd_info_t out_planes[SECCAM_MAX_NUM_OF_PLANES];
} seccam_plane_cmd_data_t;

//command structure for on-frame-available event
//- custom request data is passed as is by the library to the callback
//- custom response data is expected to be filled by the callback
//  and return back to HLOS caller
//  USAGE: This request struct should be passed to QSEECom_send_modified_cmd_64
//         along with offsets of ION buffers. For example:
//              ion_fd_info.data[0].cmd_buf_offset = offsetof(seccam_frame_cmd_req_t, frame_data[0].in_buffer.buffer);
typedef struct  {
    uint32_t cmd_id;                //command id
    uint32_t custom_req_size;       //size of custom request data
    uint32_t custom_req_offset;     //offset of custom request data in the request buffer
    uint32_t custom_rsp_size;       //size of custom response buffer data
    uint32_t custom_rsp_offset;     //offset of custom response data in the response buffer
    seccam_frame_cmd_data_t frame_data[SECCAM_MAX_CAMERAS]; //frame data which contains NULL buffer will be skipped

    //API - 2.1
    seccam_plane_cmd_data_t  plane_data[SECCAM_MAX_CAMERAS];  //plane descriptions for the buffers. Mostly useful for YUV images.
} seccam_frame_cmd_req_t;

typedef struct {
    uint32_t duration;  // execution time in milliseconds (for debug only)
} seccam_frame_cmd_rsp_data_t;

//response structure for on-frame-available event
//USAGE: This response struct should be passed to QSEECom_send_modified_cmd_64
typedef struct {
    seccam_status_t ret;                    //returned status
    seccam_frame_cmd_rsp_data_t rsp_data;
} seccam_frame_cmd_rsp_t;

#pragma pack(pop, seccam)

#endif //SECCAMLIB_EXPORTED_DEF_H
