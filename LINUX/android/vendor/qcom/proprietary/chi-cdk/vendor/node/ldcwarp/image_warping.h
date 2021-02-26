/****************************************************************************
* Copyright (c) 2018-2019 Qualcomm Technologies, Inc.                       *
* All Rights Reserved.                                                      *
* Confidential and Proprietary - Qualcomm Technologies, Inc.                *
****************************************************************************/
#ifndef __IMAGE_WARPING_H__
#define __IMAGE_WARPING_H__

#include <stdbool.h>
#include <stddef.h>

//#ifdef __cplusplus

//#define LINUX_EMBEDDED

#ifdef DEFINE_CPLUSPLUS
extern "C" {
#endif

typedef struct{
  int ion_device_fd;
  int ion_fd;
  unsigned int bufsize;
  int cached;
  void* virtual_addr;
  void* user_handle;
} mmap_info_ion;

typedef enum {IWARP_IMG_TYPE_NON_IMG, IWARP_IMG_TYPE_NV12, IWARP_IMG_TYPE_P010, IWARP_IMG_TYPE_TP10, IWARP_IMG_TYPE_UBWC_NV12, IWARP_IMG_TYPE_UBWC_P010, IWARP_IMG_TYPE_UBWC_TP10, IWARP_IMG_TYPE_MAX} ImgType ;

typedef struct {
  ImgType type;
  int width;
  int height;
  int stride;
  int stride_in_bytes;
  int scanline;
  unsigned int buf_size;
  void *buf;
  int fd;
  int cached_ion;
} image_warp_buffer_t;

typedef struct {
  int input_buf_count;
  int mesh_buf_count;
  int output_buf_count;
  image_warp_buffer_t *input_buf;
  image_warp_buffer_t *mesh_buf;
  image_warp_buffer_t *output_buf;
} image_warping_bundle_t;

typedef struct {
  int perf_flag;
  int dbg_flag;
  int num_ver_x;
  int num_ver_y;
  void (*completion_callback)(void *user_data, int err);
  void (*malloc_ion) (mmap_info_ion * p_mmap_info_ion);
  void (*free_ion) (mmap_info_ion * p_mmap_info_ion);
  int ion_device_fd;
  int enable_dithering;
} image_warping_config_t;

// perf_flag: 0 GL normal, 1 CL normal, 2 GL performance, 3 CL performance (turbo clk)
 int image_warping_init(void **handle, image_warping_config_t* p_image_warping_config);
 int image_warping_deinit(void *handle);
 int image_warping_process(	void *handle, image_warping_bundle_t *buff_bundle,	void *user_data);

//#ifdef __cplusplus
#ifdef DEFINE_CPLUSPLUS
}
#endif

//extern "C" {int testFunction();}

#endif
