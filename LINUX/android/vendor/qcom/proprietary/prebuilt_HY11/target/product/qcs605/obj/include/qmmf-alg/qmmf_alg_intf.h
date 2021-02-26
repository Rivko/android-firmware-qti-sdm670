/*
* Copyright (c) 2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
*
*/

#pragma once
#include <cstdint>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Max number of image planes */
#define QMMF_ALG_MAX_PLANES 3

/** qmmf_alg_status_t:
 *    @QMMF_ALG_ERR_HW_PLATFORM_NOT_AVAILABLE: HW Platform not available
 *    @QMMF_ALG_ERR_HW_DEVICE_NOT_AVAILABLE: HW device not available
 *    @QMMF_ALG_ERR_GET_DEVICE_INFO: failed to get device information
 *    @QMMF_ALG_FILE_DOESNT_EXIST: designated file doesn't exist
 *    @QMMF_ALG_UNABLE_TO_DETERMINE_FILESIZE: unable to determine the
 *      size of the designated file
 *    @QMMF_ALG_ERR_FAILED_TO_LOAD_IMGPROC_KERNEL: failed to enqueue
 *      image processing kernel
 *    @QMMF_ALG_ERR_SET_IMGPROC_KERNEL_ARG: error at setting image processing
 *      kernel argument
 *    @QMMF_ALG_FAILED_TO_CREATE_IMGPROC_KERNEL: error at creating
 *     image processing kernel from program
 *    @QMMF_ALG_UNABLE_TO_BUILD_IMGPROC_PROGRAM: error at compiling
 *      image processing kernel
 *    @QMMF_ALG_ERR_FAILED_TO_FLUSH_CQ: failed to flush command queue
 *    @QMMF_ALG_UNABLE_TO_READ_FILE: failed to read designated file
 *    @QMMF_ALG_WRONG_STRIDE_OUTBUF: output buffer stride and argument
 *      stride don't match
 *    @QMMF_ALG_WRONG_STRIDE_INBUF: input buffer stride and argument
 *      stride don't match
 *    @QMMF_ALG_WRONG_WIDTH_OUTBUF: error, input buffer value does not match
 *    @QMMF_ALG_WRONG_WIDTH_INBUF: error, output buffer value does not match
 *    @QMMF_ALG_ERR_UNABLE_TO_MAP_OUTBUF: error at mapping output buffer
 *    @QMMF_ALG_ERR_UNABLE_TO_MAP_INBUF: error at mapping input buffer
 *    @QMMF_ALG_ERR_INVALID_INPUT: invalid input
 *    @QMMF_ALG_ERR_INPUT_BUFFER_NOT_MAPPED: input buffer not mapped
 *    @QMMF_ALG_ERR_OUTPUT_BUFFER_NOT_MAPPED: output buffer not mapped
 *    @QMMF_ALG_FAILED_TO_CREATE_PROG_WITH_BIN: error at creating image
 *      processing program from bin file
 *    @QMMF_ALG_FAILED_TO_CREATE_PROG_WITH_SRC: error at creating compiling
 *      image processing src
 *    @QMMF_ALG_FAILED_TO_CREATE_CONTEXT: error at creating image processing
 *      context
 *    @QMMF_ALG_FAILED_TO_CREATE_CQUEUE: error at creating image processing
 *      command queue
 *    @QMMF_ALG_NOT_INITIALIZED: error, qmmf_alg library is not initialized
 *    @QMMF_ALG_ERR_NO_MEMORY: error, not enough heap memory
 *    @QMMF_ALG_FAIL: generic error
 *    @QMMF_ALG_UNABLE_TO_OPEN_FILE: error, failed to open designated file
 *    @QMMF_ALG_UNABLE_TO_WRITE_FILE: error, failed to write designated file
 *    @QMMF_ALG_ERR_SET_PERF_HINT: failed to set performance hint
 *    @QMMF_ALG_FAILED_TO_GET_BLD_LOG_SIZE: failed to get size of build log
 *    @QMMF_ALG_FAILED_TO_GET_BLD_LOG: failed to get pointer to build log
 *    @QMMF_ALG_FAILED_TO_GET_DEV_ID_FROM_PROG_INFO: failed to get image
 *      processing device info from program ID
 *    @QMMF_ALG_PROGRAM_NOT_BUILT_FOR_DEV: program built for different device
 *    @QMMF_ALG_FAILED_TO_GET_PROG_BIN_SIZE: could not get size of image
 *      processing program binary
 *    @QMMF_ALG_FAILED_TO_GET_PROG_BINNARY: could not get image processing
 *      program binary
 *    @QMMF_ALG_SUCCESS: success
 *
 *  This enum defines the return status from qmmf_alg library interface
 */
typedef enum {
  QMMF_ALG_ERR_HW_PLATFORM_NOT_AVAILABLE =      -1024,
  QMMF_ALG_ERR_HW_DEVICE_NOT_AVAILABLE,         //1023
  QMMF_ALG_ERR_GET_DEVICE_INFO,                 //1022
  QMMF_ALG_FILE_DOESNT_EXIST,                   //1021
  QMMF_ALG_UNABLE_TO_DETERMINE_FILESIZE,        //1020
  QMMF_ALG_ERR_FAILED_TO_LOAD_IMGPROC_KERNEL,   //1019
  QMMF_ALG_ERR_SET_IMGPROC_KERNEL_ARG,          //1018
  QMMF_ALG_FAILED_TO_CREATE_IMGPROC_KERNEL,     //1017
  QMMF_ALG_UNABLE_TO_BUILD_IMGPROC_PROGRAM,     //1016
  QMMF_ALG_ERR_FAILED_TO_FLUSH_CQ,              //1015
  QMMF_ALG_UNABLE_TO_READ_FILE,                 //1014
  QMMF_ALG_WRONG_STRIDE_OUTBUF,                 //1013
  QMMF_ALG_WRONG_STRIDE_INBUF,                  //1012
  QMMF_ALG_WRONG_WIDTH_OUTBUF,                  //1011
  QMMF_ALG_WRONG_WIDTH_INBUF,                   //1010
  QMMF_ALG_ERR_UNABLE_TO_MAP_OUTBUF,            //1009
  QMMF_ALG_ERR_UNABLE_TO_MAP_INBUF,             //1008
  QMMF_ALG_ERR_INVALID_INPUT,                   //1007
  QMMF_ALG_ERR_INPUT_BUFFER_NOT_MAPPED,         //1006
  QMMF_ALG_ERR_OUTPUT_BUFFER_NOT_MAPPED,        //1005
  QMMF_ALG_FAILED_TO_CREATE_PROG_WITH_BIN,      //1004
  QMMF_ALG_FAILED_TO_CREATE_PROG_WITH_SRC,      //1003
  QMMF_ALG_FAILED_TO_CREATE_CONTEXT,            //1002
  QMMF_ALG_FAILED_TO_CREATE_CQUEUE,             //1001
  QMMF_ALG_NOT_INITIALIZED,                     //1000
  QMMF_ALG_ERR_NO_MEMORY,                       //999
  QMMF_ALG_FAIL,                                //998
  QMMF_ALG_UNABLE_TO_OPEN_FILE,                 //997
  QMMF_ALG_UNABLE_TO_WRITE_FILE,                //996
  QMMF_ALG_ERR_SET_PERF_HINT,                   //995
  QMMF_ALG_FAILED_TO_GET_BLD_LOG_SIZE,          //994
  QMMF_ALG_FAILED_TO_GET_BLD_LOG,               //993
  QMMF_ALG_FAILED_TO_GET_DEV_ID_FROM_PROG_INFO, //992
  QMMF_ALG_PROGRAM_NOT_BUILT_FOR_DEV,           //991
  QMMF_ALG_FAILED_TO_GET_PROG_BIN_SIZE,         //990
  QMMF_ALG_FAILED_TO_GET_PROG_BINNARY,          //989
  QMMF_ALG_SUCCESS = 0,
} qmmf_alg_status_t;

/** qmmf_alg_pixel_format_t:
 *    @QMMF_ALG_PIXFMT_RAW_BGGR10_MIPI10_PACKED:
 *      Bayer raw "|B|Gb|Gr|R|" MIPI10 packed
 *    @QMMF_ALG_PIXFMT_RAW_GBRG10_MIPI10_PACKED:
 *      Bayer raw "|Gb|B|R|Gr|" MIPI10 packed
 *    @QMMF_ALG_PIXFMT_RAW_GRBG10_MIPI10_PACKED:
 *      Bayer raw "|Gr|R|Gb|B|" MIPI10 packed
 *    @QMMF_ALG_PIXFMT_RAW_RGGB10_MIPI10_PACKED:
 *      Bayer raw "|R|Gr|Gb|B|" MIPI10 packed
 *
 *    @QMMF_ALG_PIXFMT_RAW_BGGR12_MIPI12_PACKED:
 *      Bayer raw "|B|Gb|Gr|R|" MIPI12 packed
 *    @QMMF_ALG_PIXFMT_RAW_GBRG12_MIPI12_PACKED:
 *      Bayer raw "|Gb|B|R|Gr|" MIPI12 packed
 *    @QMMF_ALG_PIXFMT_RAW_GRBG12_MIPI12_PACKED:
 *      Bayer raw "|Gr|R|Gb|B|" MIPI12 packed
 *    @QMMF_ALG_PIXFMT_RAW_RGGB12_MIPI12_PACKED:
 *      Bayer raw "|R|Gr|Gb|B|" MIPI12 packed
 *
 *    @QMMF_ALG_PIXFMT_RAW_BGGR10:
 *      Bayer raw "|B|Gb|Gr|R|" 2 bytes per pixel unpacked
 *    @QMMF_ALG_PIXFMT_RAW_GBRG10:
 *      Bayer raw "|Gb|B|R|Gr|" 2 bytes per pixel unpacked
 *    @QMMF_ALG_PIXFMT_RAW_GRBG10:
 *      Bayer raw "|Gr|R|Gb|B|" 2 bytes per pixel unpacked
 *    @QMMF_ALG_PIXFMT_RAW_RGGB10:
 *      Bayer raw "|R|Gr|Gb|B|" 2 bytes per pixel unpacked
 *
 *    @QMMF_ALG_PIXFMT_RAW_BGGR12:
 *      Bayer raw "|B|Gb|Gr|R|" 2 bytes per pixel unpacked
 *    @QMMF_ALG_PIXFMT_RAW_GBRG12:
 *      Bayer raw "|Gb|B|R|Gr|" 2 bytes per pixel unpacked
 *    @QMMF_ALG_PIXFMT_RAW_GRBG12:
 *      Bayer raw "|Gr|R|Gb|B|" 2 bytes per pixel unpacked
 *    @QMMF_ALG_PIXFMT_RAW_RGGB12:
 *      Bayer raw "|R|Gr|Gb|B|" 2 bytes per pixel unpacked
 *
 *    @QMMF_ALG_PIXFMT_YUYV422I: YUV 4:2:2 "|Y|U|Y|V|" yuv interleaved
 *    @QMMF_ALG_PIXFMT_YVYU422I: YUV 4:2:2 "|Y|V|Y|U|" yuv interleaved
 *    @QMMF_ALG_PIXFMT_UYVY422I: YUV 4:2:2 "|U|Y|V|Y|" yuv interleaved
 *    @QMMF_ALG_PIXFMT_VYUY422I: YUV 4:2:2 "|V|Y|U|V|" yuv interleaved
 *
 *    @QMMF_ALG_PIXFMT_NV12: NV12 semiplanar: Y/CbCr 4:2:0
 *    @QMMF_ALG_PIXFMT_NV21: NV21 semiplanar: Y/CrCb 4:2:0
 *
 *    @QMMF_ALG_PIXFMT_YUV420: YUV 4:2:0
 *    @QMMF_ALG_PIXFMT_YVU420: YVU 4:2:0
 *
 *    @QMMF_ALG_PIXFMT_YUV420P: YUV 4:2:0 Planar
 *    @QMMF_ALG_PIXFMT_YVU420P: YVU 4:2:0 Planar
 *
 *    @QMMF_ALG_PIXFMT_RGB444:   RGB 16bit  xxxxrrrr ggggbbbb
 *    @QMMF_ALG_PIXFMT_ARGB444:  RGB 16bit  aaaarrrr ggggbbbb
 *    @QMMF_ALG_PIXFMT_XRGB444:  RGB 16bit  xxxxrrrr ggggbbbb
 *    @QMMF_ALG_PIXFMT_RGB555:   RGB 16bit  RGB-5-5-5
 *    @QMMF_ALG_PIXFMT_ARGB555:  RGB 16bit  ARGB-1-5-5-5
 *    @QMMF_ALG_PIXFMT_XRGB555:  RGB 16bit  XRGB-1-5-5-5
 *    @QMMF_ALG_PIXFMT_RGB565:   RGB 16bit  RGB-5-6-5
 *    @QMMF_ALG_PIXFMT_RGB555X:  RGB 16bit  RGB-5-5-5 BE
 *    @QMMF_ALG_PIXFMT_ARGB555X: RGB 16bit  ARGB-5-5-5 BE
 *    @QMMF_ALG_PIXFMT_XRGB555X: RGB 16bit  XRGB-5-5-5 BE
 *    @QMMF_ALG_PIXFMT_RGB565X:  RGB 16bit  RGB-5-6-5 BE
 *    @QMMF_ALG_PIXFMT_BGR666:   RGB 18bit  BGR-6-6-6
 *    @QMMF_ALG_PIXFMT_BGR24:    RGB 24bit  BGR-8-8-8
 *    @QMMF_ALG_PIXFMT_RGB24:    RGB 24bit  RGB-8-8-8
 *    @QMMF_ALG_PIXFMT_BGR32:    RGB 32bit  BGR-8-8-8-8
 *    @QMMF_ALG_PIXFMT_ABGR32:   RGB 32bit  BGRA-8-8-8-8
 *    @QMMF_ALG_PIXFMT_XBGR32:   RGB 32bit  BGRX-8-8-8-8
 *    @QMMF_ALG_PIXFMT_RGB32:    RGB 32bit  RGB-8-8-8-8
 *    @QMMF_ALG_PIXFMT_ARGB32:   RGB 32bit  ARGB-8-8-8-8
 *    @QMMF_ALG_PIXFMT_XRGB32:   RGB 32bit  XRGB-8-8-8-8
 *
 *    @QMMF_ALG_PIXFMT_JPEG: JFIF JPEG
 *    @QMMF_ALG_PIXFMT_GREY: Greyscale
 *
 *  This enum defines the supported pixel formats
 */
typedef enum {
  QMMF_ALG_PIXFMT_RAW_BGGR10_MIPI10_PACKED = 0x100,
  QMMF_ALG_PIXFMT_RAW_GBRG10_MIPI10_PACKED = 0x101,
  QMMF_ALG_PIXFMT_RAW_GRBG10_MIPI10_PACKED = 0x102,
  QMMF_ALG_PIXFMT_RAW_RGGB10_MIPI10_PACKED = 0x103,

  QMMF_ALG_PIXFMT_RAW_BGGR12_MIPI12_PACKED = 0x200,
  QMMF_ALG_PIXFMT_RAW_GBRG12_MIPI12_PACKED = 0x201,
  QMMF_ALG_PIXFMT_RAW_GRBG12_MIPI12_PACKED = 0x202,
  QMMF_ALG_PIXFMT_RAW_RGGB12_MIPI12_PACKED = 0x203,

  QMMF_ALG_PIXFMT_RAW_BGGR10 = 0x300,
  QMMF_ALG_PIXFMT_RAW_GBRG10 = 0x301,
  QMMF_ALG_PIXFMT_RAW_GRBG10 = 0x302,
  QMMF_ALG_PIXFMT_RAW_RGGB10 = 0x303,

  QMMF_ALG_PIXFMT_RAW_BGGR12 = 0x400,
  QMMF_ALG_PIXFMT_RAW_GBRG12 = 0x401,
  QMMF_ALG_PIXFMT_RAW_GRBG12 = 0x402,
  QMMF_ALG_PIXFMT_RAW_RGGB12 = 0x403,

  QMMF_ALG_PIXFMT_YUYV422I = 0x500,
  QMMF_ALG_PIXFMT_YVYU422I = 0x501,
  QMMF_ALG_PIXFMT_UYVY422I = 0x502,
  QMMF_ALG_PIXFMT_VYUY422I = 0x503,

  QMMF_ALG_PIXFMT_NV12 = 0x600,
  QMMF_ALG_PIXFMT_NV21 = 0x601,

  QMMF_ALG_PIXFMT_YCbCr_420_SP_VENUS_UBWC = 0x610,

  QMMF_ALG_PIXFMT_YUV420 = 0x700,
  QMMF_ALG_PIXFMT_YVU420 = 0x701,

  QMMF_ALG_PIXFMT_YUV420P = 0x800,
  QMMF_ALG_PIXFMT_YVU420P = 0x801,

  QMMF_ALG_PIXFMT_RGB444   = 0x900,
  QMMF_ALG_PIXFMT_ARGB444  = 0x901,
  QMMF_ALG_PIXFMT_XRGB444  = 0x902,
  QMMF_ALG_PIXFMT_RGB555   = 0x903,
  QMMF_ALG_PIXFMT_ARGB555  = 0x904,
  QMMF_ALG_PIXFMT_XRGB555  = 0x905,
  QMMF_ALG_PIXFMT_RGB565   = 0x906,
  QMMF_ALG_PIXFMT_RGB555X  = 0x907,
  QMMF_ALG_PIXFMT_ARGB555X = 0x908,
  QMMF_ALG_PIXFMT_XRGB555X = 0x909,
  QMMF_ALG_PIXFMT_RGB565X  = 0x90A,
  QMMF_ALG_PIXFMT_BGR666   = 0x90B,
  QMMF_ALG_PIXFMT_BGR24    = 0x90C,
  QMMF_ALG_PIXFMT_RGB24    = 0x90D,
  QMMF_ALG_PIXFMT_BGR32    = 0x90E,
  QMMF_ALG_PIXFMT_ABGR32   = 0x90F,
  QMMF_ALG_PIXFMT_XBGR32   = 0x910,
  QMMF_ALG_PIXFMT_RGB32    = 0x911,
  QMMF_ALG_PIXFMT_ARGB32   = 0x912,
  QMMF_ALG_PIXFMT_XRGB32   = 0x913,

  QMMF_ALG_PIXFMT_JPEG = 0xA00,
  QMMF_ALG_PIXFMT_GREY = 0xA01,

} qmmf_alg_pixel_format_t;

/** qmmf_alg_plane_t:
 *    @stride: Stride in bytes
 *    @offset: Offset to valid plane data in bytes
 *    @length: Plane length in bytes
 *
 *  This structure defines plane representation in memory
 **/
typedef struct {
  uint32_t stride;
  uint32_t offset;
  uint32_t length;
} qmmf_alg_plane_t;

/** qmmf_alg_format_t:
 *    @width: Width in pixels
 *    @height: Height in pixels
 *    @pix_fmt: Pixel format
 *    @num_planes: Number of planes depends on pixel format.
 *      Note: Bayer formats are represented as formats
 *      with one plane.
 *    @plane: Array of planes. Plane information depends on
 *      pixel format.
 *
 *  This structure defines image format
 */
typedef struct {
  uint32_t width;
  uint32_t height;
  qmmf_alg_pixel_format_t pix_fmt;
  uint8_t num_planes;
  qmmf_alg_plane_t plane[QMMF_ALG_MAX_PLANES];
} qmmf_alg_format_t;

/** qmmf_alg_fmt_list_t:
 *    @cnt: Number of format elements in "fmts" array
 *    @fmts: Array of formats with "cnt" number of elements.
 *
 *  This structure defines the format list
 **/
typedef struct {
  uint32_t cnt;
  qmmf_alg_format_t *fmts;
} qmmf_alg_fmt_list_t;

/** qmmf_alg_buffer_t:
 *    @fmt: Buffer format. check qmmf_alg format structure.
 *    @vaddr: Buffer virtual address.
 *    @fd: buffer fd (Platform specific can be ion or dmabuf)
 *    @size: Buffer size in bytes.
 *    @handle: Buffer handle.
 *    @priv: Private data attached to the buffer, it should
 *       be returned as it is in completion callback.
 *
 *  This structure defines the image buffer
 **/
typedef struct {
  qmmf_alg_format_t fmt;
  uint8_t *vaddr;
  int32_t fd;
  uint32_t size;
  void *handle;
  void *priv;
} qmmf_alg_buffer_t;

/** qmmf_alg_crop_t:
 *    @x: Upper-left x-coordinate
 *    @y: Upper-left y-coordinate
 *    @width: Width in pixels
 *    @height: Height in pixels
 *
 *  This structure defines image format
 */
typedef struct {
  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t height;
} qmmf_alg_crop_t;

/** qmmf_alg_buf_list_t:
 *    @cnt: Number of buffer elements in "bufs" array
 *    @bufs: Array of buffers with "cnt" number of elements.
 *
 *  This structure defines buffer list
 **/
typedef struct {
  uint32_t cnt;
  qmmf_alg_buffer_t *bufs;
  uint32_t crop_cnt;
  qmmf_alg_crop_t *crop;
} qmmf_alg_buf_list_t;

/** qmmf_alg_blob_t:
 *    @data: Pointer to blob data
 *    @size: Size of blob data in bytes
 *
 *  This structure defines blob data used for tuning calibration etc.
 **/
typedef struct {
  void *data;
  uint32_t size;
} qmmf_alg_blob_t;

/** qmmf_alg_caps_t:
 *    @lib_version: lib version
 *
 *    @in_fmt_cnt: Number of supported input pixel formats.
 *    @input_pix_fmts: Array of supported input pixel formats with
 *      "in_fmt_cnt" elements.
 *
 *    @out_fmt_cnt: Number of supported output pixel formats.
 *    @output_pix_fmts: Array of supported output pixel formats with
 *      "out_fmt_cnt" elements.
 *
 *    @max_width: Max supported image width
 *    @max_height: Max supported image height
 *
 *    @min_width: Min supported image width
 *    @min_height: Min supported image height
 *
 *  This structure defines the capabilities
 **/
typedef struct {
  float lib_version;

  uint32_t in_fmt_cnt;
  qmmf_alg_pixel_format_t *input_pix_fmts;

  uint32_t out_fmt_cnt;
  qmmf_alg_pixel_format_t *output_pix_fmts;

  uint32_t max_width;
  uint32_t max_height;

  uint32_t min_width;
  uint32_t min_height;
} qmmf_alg_caps_t;

/** qmmf_alg_cb_t:
 *    @buf: Buffer released by this callback.
 *    @user_data: Pointer to user_data passed in "qmmf_alg_process_data_t"
 *    @status: Status to notify if some error occurred during
 *      buffer processing.
 *
 *  This structure defines the qmmf_alg callback data.
 *  Callback can be called (without buffer)
 *  to notify for some error status.
 **/
typedef struct {
  qmmf_alg_buffer_t *buf;
  void *user_data;
  qmmf_alg_status_t status;
} qmmf_alg_cb_t;

/** qmmf_alg_completion_callback_t
 *    @cb_data: callback data
 *
 * function pointer to be called when buffers are released.
 *
 * return: nothing
 **/
typedef void (*qmmf_alg_completion_callback_t)(qmmf_alg_cb_t *cb_data);

/** qmmf_alg_config_t:
 *    @input: List of input buffer formats which will be used for processing.
 *      Library is free to modify the sizes based on library restrictions.
 *    @output: List of output buffer formats which will be used for processing.
 *      Library is free to modify the sizes based on library restrictions.
 *
 *  This structure defines configuration parameters. More precisely
 *  the input and output formats of the buffers that will be procesed.
 *  Output of configuration will be used for size negotiations
 *  with other components.
 **/
typedef struct {
  qmmf_alg_fmt_list_t input;
  qmmf_alg_fmt_list_t output;
} qmmf_alg_config_t;

/** qmmf_alg_process_data_t:
 *    @input: List of input buffers used for processing
 *    @output: List of output buffers need to be filled by the library
 *    @complete: Completion callback. If Callback is set it need to be called
 *      to release the buffers, after this call library should not
 *      use the buffers for processing.
 *      If callback is NULL library should block process function
 *      until all buffers are released (Library should work in blocking mode)
 *    @user_data: Client data to be passed back from
 *      qmmf_alg_completion_callback
 *
 *      Note: NULL is also valid userdata
 *
 *  This structure defines the qmmf_alg process data
 **/
typedef struct {
  qmmf_alg_buf_list_t input;
  qmmf_alg_buf_list_t output;
  qmmf_alg_completion_callback_t complete;
  void *user_data;
} qmmf_alg_process_data_t;

/** qmmf_alg_init
 *    @handle: qmmf_alg handle
 *    @calibration_data: blob containing calibration data
 *
 * Initializes qmmf_alg library
 *
 * return: QMMF_ALG status
 **/
qmmf_alg_status_t qmmf_alg_init(void **handle,
                                qmmf_alg_blob_t *calibration_data);

/** qmmf_alg_deinit
 *    @handle: qmmf_alg handle
 *
 * Deinitializes qmmf_alg library
 *
 * return: nothing
 **/
void qmmf_alg_deinit(void *handle);

/** qmmf_alg_get_caps
 *    @handle: qmmf_alg handle
 *    @caps: query capabilities data
 *
 * qmmf_alg lib fills it's capability and requirements
 *
 * return: QMMF_ALG status
 **/
qmmf_alg_status_t qmmf_alg_get_caps(void *handle, qmmf_alg_caps_t *caps);

/**qmmf_alg_set_tuning
 *    @handle: qmmf_alg handle
 *    @tuning_data: blob containing tuning data
 *
 * Set library tuning
 *
 * return: QMMF_ALG status
 **/
qmmf_alg_status_t qmmf_alg_set_tuning(void *handle,
                                      qmmf_alg_blob_t *tuning_data);

/** qmmf_alg_config
 *    @handle: qmmf_alg handle
 *    @config: qmmf_alg configuration
 *
 * Configures qmmf_alg library. Library is free to modify and align
 * configuration with library internal constraints.
 *
 * return: QMMF_ALG status
 **/
qmmf_alg_status_t qmmf_alg_config(void *handle, qmmf_alg_config_t *config);

/** qmmf_alg_register_bufs
 *    @handle: qmmf_alg handle
 *    @bufs: List of buffers to register
 *
 * Register buffers to qmmf_alg library. All buffers should be registered
 * before passed to the library for processing.
 *
 * return: QMMF_ALG status
 **/
qmmf_alg_status_t qmmf_alg_register_bufs(void *handle,
                                         qmmf_alg_buf_list_t bufs);

/** qmmf_alg_unregister_bufs
 *    @handle: qmmf_alg handle
 *    @bufs: List of buffers to register
 *
 * Unregister buffers from the library. After this call buffers
 * can not be used for processing. Library responsibility is to
 * free all references to this buffers. If buffers are in library
 * processing queue unregister should return an error.
 *
 * return: QMMF_ALG status
 **/
qmmf_alg_status_t qmmf_alg_unregister_bufs(void *handle,
                                           qmmf_alg_buf_list_t bufs);

/** qmmf_alg_flush
 *    @handle: qmmf_alg handle
 *
 * Flush all buffers from library processing queue.
 * All buffers should be returned with corresponding callbacks,
 * error status should be set if output buffers are released
 * and not yet processed.
 * After this call library processing queue should be empty.
 *
 * return: QMMF_ALG status
 **/
qmmf_alg_status_t qmmf_alg_flush(void *handle);

/** qmmf_alg_process
 *    @handle: qmmf_alg handle
 *    @proc_data: Image data to process
 *
 * main qmmf_alg function to process image data
 *
 * return: QMMF_ALG status
 **/
qmmf_alg_status_t qmmf_alg_process(void *handle,
                                   qmmf_alg_process_data_t *proc_data);

/** qmmf_alg_get_debug_info_log
 *    @handle: qmmf_alg handle
 *    @log: pointer to a string to store the log
 *
 * returns debug info log from the library
 *
 * return: pointer to debug info build log
 **/
qmmf_alg_status_t qmmf_alg_get_debug_info_log(void *handle, char **log);

#ifdef __cplusplus
}
#endif

