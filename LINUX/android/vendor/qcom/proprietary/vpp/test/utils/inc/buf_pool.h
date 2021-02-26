/*!
 * @file buf_pool.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */
#ifndef _BUF_POOL_H_
#define _BUF_POOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include <media/msm_vidc.h>
#include <media/msm_media_info.h>
#include "vpp_buf.h"
#include "vpp_ion.h"

#define INPUT_FILEPATH_TEMPLATE "/data/test/input/"
#define OUTPUT_FILEPATH_TEMPLATE "/data/test/output/"

// Use "_%03d" in file name if using one file per frame
#define INPUT_FILENAME_TEMPLATE "vdp_%03d.yuv"
#define OUTPUT_FILENAME_TEMPLATE "vdp_%03d.yuv"

enum buf_owner {
    BUF_OWNER_CLIENT = 1,
    BUF_OWNER_LIBRARY,
};

struct bufnode {
    struct buf_pool *pPool;         // pointer back to pool that owns this buffer
    struct bufnode *pNext;          // next node in list
    enum buf_owner owner;
    enum vpp_port port_owner;       // which port is this buffer queued to
    t_StVppBuf *pIntBuf;            // internal vpp buffer buffer
    struct vpp_buffer *pExtBuf;     // external vpp buffer
};

#define MAX_FILE_LEN 256
#define MAX_FILE_SEG_LEN (MAX_FILE_LEN / 2)
enum buffer_format {
    /*! NV12, no padding */
    FILE_FORMAT_NV12 = 1,
    /*! P010, no padding */
    FILE_FORMAT_P010,
    /*! NV12 compressed, UBWC format */
    FILE_FORMAT_NV12_UBWC,
    /*! TP10 compressed, UBWC format */
    FILE_FORMAT_TP10_UBWC,
};

enum file_type {
    FILE_TYPE_SINGLE_FRAMES,
    FILE_TYPE_MULTI_FRAMES,
    FILE_TYPE_NONE,
};

enum buf_pool_protection {
    PROTECTION_ZONE_NONSECURE,
    PROTECTION_ZONE_SECURE,
    PROTECTION_ZONE_MAX,
};

struct buf_pool_params {
    uint32_t u32Width;
    uint32_t u32Height;

    char cInputPath[MAX_FILE_SEG_LEN];
    char cInputName[MAX_FILE_SEG_LEN];
    char cInputNameMbi[MAX_FILE_SEG_LEN];
    char cOutputPath[MAX_FILE_SEG_LEN];
    char cOutputName[MAX_FILE_SEG_LEN];

    enum buf_pool_protection eProtection;

    enum buffer_format eInputFileFormat;
    enum buffer_format eOutputFileFormat;
    enum file_type eInputFileType;
    enum file_type eOutputFileType;

    uint32_t u32MaxInputFrames;

    enum vpp_color_format eInputBufFmt;
    enum vpp_color_format eOutputBufFmt;

    t_EVppBufType eBufferType;

};

struct buf_pool {
    t_StVppCtx *pstCtx;         // Needed to use vpp_library ion APIs

    // Storages, dynamically allocated memory
    uint32_t u32Cnt;            // size of all dynamically allocated arrays
    t_StVppBuf *pstBuf;         // array of VPP internal buf structures
    struct vpp_buffer *vpp_buffers; // array of VPP external buf structures
    struct bufnode *nodes;      // array of nodes used in buf pool

    // Configuration
    struct vpp_port_param stInputPort;  // copy of input port params for convenience
    struct vpp_port_param stOutputPort; // copy of output port params for convenience
    struct buf_pool_params params;      // params used to initialize the buffer pool
    uint32_t u32RdIdx;          // current read index (for fill_buf)
    uint32_t u32WrIdx;          // current write index (for dump_buf)

    int ion_dev_fd;             // fd to /dev/ion
    t_StVppIonBuf *pstIonMemPx; // ptr to array of ion buffer structures (pixel data)
    t_StVppIonBuf *pstIonMemEx; // ptr to array of ion buffer structures (extra data)

    // Actual buffer pool structure
    uint32_t u32ListSz;         // Number of buffers held in the pool
    struct bufnode *pHead;      // Head of the list of free nodes
    pthread_mutex_t mutex;      // Guards this struct
};

#define NUM_FD_MAX 512

enum clip_reg {
    CLIP_NULL = 0,  // Have null clip at index 0
#if 0
    CLIP_BROOK_720x480,
#endif
    CLIP_CANYON_720x480,
#if 0
    CLIP_FLOWER_720x480,
    CLIP_HAIR_720x480,
    CLIP_STONE_BRIDGE_720x480,
    CLIP_WOOD_720x480,
    CLIP_BROOK_1920x1080,
    CLIP_CANYON_1920x1080,
#endif
    CLIP_FLOWER_1920x1080,
    CLIP_NEW_YORK_3840x2160,
    CLIP_HAIR_1920x1080,
    CLIP_BEACH_1280x720,
#if 0
    CLIP_WOOD_1920x1080,
#endif
    CLIP_FRC_1920x1080,
    CLIP_P010_SD,
    CLIP_P010_HD,
    CLIP_P010_FHD,
    CLIP_P010_UHD,
    CLIP_UBWC_NV12_SD,
    CLIP_SLOMO_PITCH_UBWC_NV12_1280x720,
    CLIP_UBWC_NV12_FHD,
    CLIP_UBWC_NV12_UHD,
    CLIP_UBWC_TP10_SD,
    CLIP_UBWC_TP10_HD,
    CLIP_UBWC_TP10_FHD,
    CLIP_UBWC_TP10_UHD,
    CLIP_REG_MAX,
};

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/
void dump_buf_stats();
uint32_t buf_params_init_default(struct buf_pool_params *params,
                                 struct vpp_port_param *port_param);

uint32_t populate_pool_params(enum clip_reg clip, struct buf_pool_params *params);
uint32_t populate_port_params(enum clip_reg clip, enum vpp_port port, struct vpp_port_param *port_param);

uint32_t get_res_for_clip(enum clip_reg clip, uint32_t *pu32Width,
                          uint32_t *pu32Height, enum vpp_color_format *peFmt);
uint32_t init_buf_pool(t_StVppCtx *pstCtx,
                       struct buf_pool **ppool,
                       struct buf_pool_params *params,
                       uint32_t u32Cnt,
                       uint32_t bInitIon);
void free_buf_pool(struct buf_pool *pool, uint32_t bTermIon);
struct bufnode *get_buf(struct buf_pool *pool);
void put_buf(struct buf_pool *pool, struct bufnode *node);

uint32_t fill_buf(struct bufnode *node);
uint32_t fill_mbi_buf(struct bufnode *node);
uint32_t dump_buf(struct bufnode *node);

void validate_extradata_integrity_int_buf(t_StVppBuf *pstIntBuf);
void validate_extradata_integrity(struct bufnode *pNode);
uint32_t fill_extra_data_int_buf(t_StVppBuf *pstIntBuf, uint32_t u32Ft,
                                 uint32_t u32Marker);
uint32_t fill_extra_data(struct bufnode *node, uint32_t u32Ft, uint32_t u32Marker);
uint32_t get_extra_data_marker_int_buf(t_StVppBuf *pstIntBuf, uint32_t* pu32Marker);

uint32_t write_extradata_buf(void *pv, uint32_t u32Ft, uint32_t u32Marker,
                             uint32_t u32Sz);

uint32_t get_missing_node_count(struct buf_pool *pool);

void vVppBuf_Clean(t_StVppBuf *pstBuf, char fill_value);
void vVppBuf_CleanExtradata(t_StVppBuf *pstBuf, char fill_value);
uint32_t restore_bufnode_internal_buf(struct bufnode *pNode);

#ifdef __cplusplus
}
#endif

#endif /* BUF_POOL_H_ */
