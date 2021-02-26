/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <hardware/camera.h>
#include <cam_types.h>
#include <cam_intf.h>

#include "mmi_module.h"
#include "view.h"

extern "C" {
#include "mm_jpeg_interface.h"
#include "mm_qcamera_app.h"
    extern int setFocusMode(mm_camera_test_obj_t * test_obj, cam_focus_mode_type mode);
}

using namespace android;

/**Camera preview dimenstion config*/
#define DIMENSION_SMALL_WIDTH 288
#define DIMENSION_SMALL_HEIGHT 352
#define DIMENSION_NORMAL_WIDTH 480
#define DIMENSION_NORMAL_HEIGHT 640
#define DIMENSION_LARGE_WIDTH  960
#define DIMENSION_LARGE_HEIGHT 1280

/**
* Defined supported command list here.And put into extra_cmd_list,
* so server could get it.
*/
#define SUBCMD_SNAPSHOT "snapshot"

static draw_control_t g_cam_draw;
ANativeWindow_Buffer outBuffer;

static const char *extra_cmd_list[] = {
    SUBCMD_SNAPSHOT
};

#define MAX_CAM_SENSOR_NUM 5
#define MM_CAMERA_DEV_NAME_LEN 32

#define BACK_CAMERA_DEVICE    "/dev/video1"
#define BACK_CAMERA_ID        0
#define FRONT_CAMERA_DEVICE   "/dev/video2"
#define FRONT_CAMERA_ID       1
#define DEFAULT_CAMERA_DEVICE BACK_CAMERA_DEVICE
#define DEFAULT_CAMERA_ID     BACK_CAMERA_ID

#define DEFAULT_PREVIEW_WIDTH   640
#define DEFAULT_PREVIEW_HEIGHT  480
#define DEFAULT_SNAPSHOT_WIDTH  DEFAULT_PREVIEW_WIDTH
#define DEFAULT_SNAPSHOT_HEIGHT DEFAULT_PREVIEW_HEIGHT

#define STR_KEY_CAM_POSITION  "type"

#define STR_EQUAL(str1, str2)  (!strcmp(str1, str2))

/** DUMP_TO_FILE:
 *  @filename: file name
 *  @p_addr: address of the buffer
 *  @len: buffer length
 *
 *  dump the image to the file
 **/
#define DUMP_TO_FILE(filename, p_addr, len) ({ \
  int rc = 0; \
  FILE *fp = fopen(filename, "w+"); \
  if (fp) { \
    rc = fwrite(p_addr, 1, len, fp); \
    fclose(fp); \
  } else { \
    MMI_ALOGE("dump image file(%s) fail", filename); \
  } \
})

typedef enum {
    TEST_FA_PREVIEW = 0,
    TEST_FA_LED_FLASH = 1,
    TEST_FA_MAX
} mmi_cam_test_func_area_t;

typedef struct {
    int width;
    int height;
} mmi_cam_dim_t;

typedef enum {
    CAM_FACING_BACK = 0,
    CAM_FACING_FRONT
} mmi_cam_pos_t;

typedef struct {
    int cam_id;
    char dev_name[MM_CAMERA_DEV_NAME_LEN];

    int32_t facing;
    int32_t idx_by_facing;

    mmi_cam_dim_t snapshot_dim;
    mmi_cam_dim_t preview_dim;

    int preview_buff_size;
    int mmi_surface_pixel_format;

    int sensor_mount_angle;
    int is_af_supported;
    cam_focus_mode_type focus_mode;
    int is_fd_supported;
    int is_aux_camera_supported;

    module_mode_t test_mode;
    mmi_cam_test_func_area_t test_func_area;

    char jpeg_name[256];

    int is_preview_running;

    mm_camera_app_t app_ctx;
    mm_camera_test_obj_t camera_obj;
} mmi_cam_module_info_t;

typedef struct {
    char *filename;
    int width;
    int height;
    char *out_filename;
} jpeg_test_input_t;

typedef struct {
    struct ion_fd_data ion_info_fd;
    struct ion_allocation_data alloc;
    int p_pmem_fd;
    long size;
    int ion_fd;
    uint8_t *addr;
} buffer_test_t;

typedef struct {
    char *filename;
    int width;
    int height;
    char *out_filename;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    buffer_test_t input;
    buffer_test_t output;
    int use_ion;
    uint32_t handle;
    mm_jpeg_ops_t ops;
    uint32_t job_id[5];
    mm_jpeg_encode_params_t params;
    mm_jpeg_job_t job;
    uint32_t session_id;
} mm_jpeg_intf_test_t;

typedef enum {
    TEST_TYPE_IMG_PIPELINE = 1,
    TEST_TYPE_FLASHLIGHT,
    TEST_TYPE_VCM,
    TEST_TYPE_JPG_ENCODE,
    TEST_TYPE_MAX
} mmi_cam_test_type;

typedef enum {
    TEST_METHOD_PREVIEW = 1,
    TEST_METHOD_SNAPSHOT,
    TEST_METHOD_TORCH,
    TEST_METHOD_BLINK,
    TEST_METHOD_MAX
} mmi_cam_test_method;

typedef enum {
    TEST_FLAG_FLASHLIGHT_OFF,
    TEST_FLAG_FLASHLIGHT_ON,
    TEST_FLAG_MAX
} mmi_cam_test_ext_flag;

/*
  align with params in pcba.cfg file
   - type:     back/front/flashlight/vcm, camera sub-component, [M]mandatory
   - method:   preview/snapshot/torch/blink, function of specified component
   - ext_flag: flashlight torch mode on/off
 */
typedef struct {
    int type;
    int method;
    int ext_flag;
} pcba_test_profile_t;


static const mmi_module_t *g_module;
static int32_t g_num_of_cameras = 0;
static mmi_cam_module_info_t g_mmi_cam;
static sem_t g_sem;
static pthread_mutex_t g_mutex_lock = PTHREAD_MUTEX_INITIALIZER;


static uint8_t *g_pPreviewYUV420;
static uint8_t *g_pPreview_Y;
static uint8_t *g_pPreview_UV;
static uint8_t *g_pPreviewRGB8888;
static uint8_t *g_pRotate90_tmp;

static int mp0len = 0;
static int mp1len = 0;
static int g_encode_after_stop = 0;
static char cur_module_name[64];
static int frm_cnt_with_face = 0;
static mutex_locker g_mutex;

static void init_draw(unordered_map < string, string > &params) {
    strlcpy(g_cam_draw.name, "CAM", sizeof(g_cam_draw.name));
    g_cam_draw.layer = 0x7FFFFFFF;

    const char *str_preview_dim = params[KEY_DIMENSION].c_str();

    if(str_preview_dim != NULL) {
        if(!strcmp(str_preview_dim, KEY_SMALL)) {
            g_cam_draw.surface_h = DIMENSION_SMALL_HEIGHT;
            g_cam_draw.surface_w = DIMENSION_SMALL_WIDTH;
        } else if(!strcmp(str_preview_dim, KEY_NORMAL)) {
            g_cam_draw.surface_h = DIMENSION_NORMAL_HEIGHT;
            g_cam_draw.surface_w = DIMENSION_NORMAL_WIDTH;
        } else if(!strcmp(str_preview_dim, KEY_LARGE)) {
            g_cam_draw.surface_h = DIMENSION_LARGE_HEIGHT;
            g_cam_draw.surface_w = DIMENSION_LARGE_WIDTH;
        }
    }
    MMI_ALOGD("camera layer=0x%x, surface high=%d, surface width=%d\n",
              g_cam_draw.layer, g_cam_draw.surface_h, g_cam_draw.surface_w);
}

static int mmi_query_camera_sensors() {
    g_num_of_cameras = get_num_of_cameras();
    MMI_ALOGI("num of physical cameras: %d\n", g_num_of_cameras);

    return 0;
}

static void allocate_buffers(int width, int height) {
    int preview_pixels = height * width;
    int stride, scanline;

    stride = PAD_TO_SIZE(width, CAM_PAD_TO_16);
    scanline = PAD_TO_SIZE(height, CAM_PAD_TO_2);
    mp0len = stride * scanline;
    stride = PAD_TO_SIZE(width, CAM_PAD_TO_16);
    scanline = PAD_TO_SIZE(height / 2, CAM_PAD_TO_2);
    mp1len = stride * scanline;

    g_pPreviewYUV420 = new uint8_t[mp0len + mp1len];
    g_pPreview_Y = new uint8_t[preview_pixels];
    g_pPreview_UV = new uint8_t[preview_pixels / 2];

    g_pRotate90_tmp = new uint8_t[preview_pixels * 4];
    g_pPreviewRGB8888 = new uint8_t[preview_pixels * 4];

    return;
}

void dealloc_buffers() {
    delete[]g_pPreviewYUV420;
    delete[]g_pPreview_Y;
    delete[]g_pPreview_UV;
    delete[]g_pRotate90_tmp;
    delete[]g_pPreviewRGB8888;
}



/** YUV2_Y_UV:
 *  @raw_buf: yuv420sp raw data
 *
 *  split yuv420sp data to Y and CbCr components
 **/
void YUV2_Y_UV(uint8_t * raw_buf) {
    int32_t stride, scanline;
    uint8_t *ptr;
    int width = g_mmi_cam.preview_dim.width;
    int height = g_mmi_cam.preview_dim.height;

    MMI_ALOGI("dimension: width=%d, height=%d\n", width, height);

    stride = PAD_TO_SIZE(width, CAM_PAD_TO_16);
    scanline = PAD_TO_SIZE(height, CAM_PAD_TO_2);

    if(raw_buf == NULL) {
        MMI_ALOGW("input buf is NULL\n");
        return;
    }

    ptr = raw_buf;
    for(int h = 0; h < height; h++) {
        memcpy(&g_pPreview_Y[h * width], ptr, width);
        ptr += stride;
    }

    ptr = raw_buf + stride * scanline;
    stride = PAD_TO_SIZE(width, CAM_PAD_TO_16);
    scanline = PAD_TO_SIZE(height / 2, CAM_PAD_TO_2);

    for(int h = 0; h < height / 2; h++) {
        memcpy(&g_pPreview_UV[h * width], ptr, width);
        ptr += stride;
    }
}

/** yuvtorgb8888:
 *  @width: frame width
 *  @height: frame height
 *  @src_y: pointer to Y componet datas
 *  @src_yuv: pointer to cbcr component datas
 *  @dest_rgb8888: pointer to dest buffer
 *
 *  translate yuv data to rgb8888
 **/
void yuvtorgb8888(int width, int height, unsigned char *src_y, unsigned char *src_uv, unsigned char *dest_rgb8888) {
    uint32_t i, j;
    int r, g, b;
    uint32_t YPOS, UPOS, VPOS;
    uint32_t num = height * width - 1;

    if(src_y == NULL || src_uv == NULL || dest_rgb8888 == NULL) {
        MMI_ALOGE("invalid input ptr\n");
        return;
    }

    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++) {
            YPOS = i * width + j;
            VPOS = (i / 2) * width + (j & 0xFFFE);
            UPOS = (i / 2) * width + (j | 0x0001);
            r = src_y[YPOS] + (1.370705 * (src_uv[VPOS] - 128));
            g = src_y[YPOS] - (0.698001 * (src_uv[VPOS] - 128)) - (0.337633 * (src_uv[UPOS] - 128));
            b = src_y[YPOS] + (1.732446 * (src_uv[UPOS] - 128));

            if(r > 255)
                r = 255;
            if(r < 0)
                r = 0;

            if(g > 255)
                g = 255;
            if(g < 0)
                g = 0;

            if(b > 255)
                b = 255;
            if(b < 0)
                b = 0;

            dest_rgb8888[num * 4] = r;
            dest_rgb8888[num * 4 + 1] = g;
            dest_rgb8888[num * 4 + 2] = b;
            dest_rgb8888[num * 4 + 3] = 0xFF;

            num--;
        }
    }
    num++;
}


/** rotate_screen:
 *  @data: pointer to buffer to be rotated
 *  @w: frame width
 *  @h: frame height
 *  @n: bytes per pixel
 *
 *  rotate the frame by 90or270 degree
 **/
void rotate_screen(unsigned char *data, int w, int h, int n, int degree) {
    int nw = h, nh = w;
    int i, j;
    int ni, nj;

    if(data == NULL) {
        MMI_ALOGE("invalide input ptr\n");
        return;
    }

    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++) {
            if(degree == 270)
                ni = j;
            else
                ni = nh - j - 1;
            nj = i;
            unsigned char *src = data + (i * w + j) * n;
            unsigned char *dst = g_pRotate90_tmp + (ni * nw + nj) * n;

            memcpy(dst, src, n);
        }
    }
    memcpy(data, g_pRotate90_tmp, w * h * n);
}

/** buffer_allocate:
 *
 *     @p_buffer: ION buffer
 *
 *  Return:
 *     buffer address
 *
 *  Description:
 *      allocates ION buffer
 *
 **/
void *buffer_allocate(buffer_test_t * p_buffer) {
    void *l_buffer = NULL;

    int lrc = 0;
    struct ion_handle_data lhandle_data;

    p_buffer->alloc.len = p_buffer->size;
    p_buffer->alloc.align = 4096;
    p_buffer->alloc.flags = 0;
    p_buffer->alloc.heap_id_mask = 0x1 << ION_IOMMU_HEAP_ID;

    p_buffer->ion_fd = open("/dev/ion", O_RDONLY);
    if(p_buffer->ion_fd < 0) {
        MMI_ALOGE("file(%s) open failed, error=%s", "/dev/ion", strerror(errno));
        goto ION_ALLOC_FAILED;
    }

    /* Make it page size aligned */
    p_buffer->alloc.len = (p_buffer->alloc.len + 4095) & (~4095);
    lrc = ioctl(p_buffer->ion_fd, ION_IOC_ALLOC, &p_buffer->alloc);
    if(lrc < 0) {
        MMI_ALOGE("ION allocation failed, error=%s", strerror(errno));
        goto ION_ALLOC_FAILED;
    }

    p_buffer->ion_info_fd.handle = p_buffer->alloc.handle;
    lrc = ioctl(p_buffer->ion_fd, ION_IOC_SHARE, &p_buffer->ion_info_fd);
    if(lrc < 0) {
        MMI_ALOGE("ION share failed, error=%s", strerror(errno));
        goto ION_MAP_FAILED;
    }

    p_buffer->p_pmem_fd = p_buffer->ion_info_fd.fd;

    l_buffer = mmap(NULL, p_buffer->alloc.len, PROT_READ | PROT_WRITE, MAP_SHARED, p_buffer->p_pmem_fd, 0);

    if(l_buffer == MAP_FAILED) {
        MMI_ALOGE("ION mmap failed, error=%s", strerror(errno));
        goto ION_MAP_FAILED;
    }

    return l_buffer;

  ION_MAP_FAILED:
    lhandle_data.handle = p_buffer->ion_info_fd.handle;
    ioctl(p_buffer->ion_fd, ION_IOC_FREE, &lhandle_data);
    return NULL;
  ION_ALLOC_FAILED:
    return NULL;

}

/** buffer_deallocate:
 *
 *     @p_buffer: ION buffer
 *
 *  Return:
 *     buffer address
 *
 *  Description:
 *      deallocates ION buffer
 *
 **/
int buffer_deallocate(buffer_test_t * p_buffer) {
    int lrc = 0;
    int lsize = (p_buffer->size + 4095) & (~4095);

    struct ion_handle_data lhandle_data;

    lrc = munmap(p_buffer->addr, lsize);

    close(p_buffer->ion_info_fd.fd);

    lhandle_data.handle = p_buffer->ion_info_fd.handle;
    ioctl(p_buffer->ion_fd, ION_IOC_FREE, &lhandle_data);

    close(p_buffer->ion_fd);
    return lrc;
}

static void mm_jpeg_encode_callback(jpeg_job_status_t status,
                                    uint32_t client_hdl, uint32_t jobId, mm_jpeg_output_t * p_output, void *userData) {
    mm_jpeg_intf_test_t *p_obj = (mm_jpeg_intf_test_t *) userData;

    if(status == JPEG_JOB_STATUS_ERROR) {
        MMI_ALOGE("Encode jepg file(%s) fail", p_obj->out_filename);
    } else {
        MMI_ALOGI("Encode jepg file success, dump jepg file to '%s'", p_obj->out_filename);
        DUMP_TO_FILE(p_obj->out_filename, p_output->buf_vaddr, p_output->buf_filled_len);
    }

    MMI_ALOGI("Notice jepg file encode finished");
    pthread_cond_signal(&p_obj->cond);

}

int mm_jpeg_test_alloc(buffer_test_t * p_buffer, int use_pmem) {
    int ret = 0;

    /*Allocate buffers */
    if(use_pmem) {
        p_buffer->addr = (uint8_t *) buffer_allocate(p_buffer);
        if(NULL == p_buffer->addr) {
            MMI_ALOGE("ioc memroy allocate fail");
            return -1;
        }
    } else {
        /* Allocate heap memory */
        p_buffer->addr = (uint8_t *) malloc(p_buffer->size);
        if(NULL == p_buffer->addr) {
            MMI_ALOGE("heap memory allocate fail");
            return -1;
        }
    }
    return ret;
}

void mm_jpeg_test_free(buffer_test_t * p_buffer) {
    if(p_buffer == NULL || p_buffer->addr == NULL)
        return;

    if(p_buffer->p_pmem_fd > 0)
        buffer_deallocate(p_buffer);
    else
        free(p_buffer->addr);

    memset(p_buffer, 0x0, sizeof(buffer_test_t));
}

int mm_jpeg_test_read(mm_jpeg_intf_test_t * p_obj, void *data) {
    if(data == NULL) {
        MMI_ALOGE("input ptr is NULL\n");
        return -1;
    }
    memcpy(p_obj->input.addr, data, p_obj->input.size);

    return 0;
}

static int encode_init(jpeg_test_input_t * p_input, mm_jpeg_intf_test_t * p_obj, void *data) {
    int rc = -1;
    int size = p_input->width * p_input->height;
    mm_jpeg_encode_params_t *p_params = &p_obj->params;
    mm_jpeg_encode_job_t *p_job_params = &p_obj->job.encode_job;

    if(data == NULL) {
        MMI_ALOGE("input data ptr is NULL\n");
        return -1;
    }

    p_obj->filename = p_input->filename;
    p_obj->width = p_input->width;
    p_obj->height = p_input->height;
    p_obj->out_filename = p_input->out_filename;
    p_obj->use_ion = 1;

    pthread_mutex_init(&p_obj->lock, NULL);
    pthread_cond_init(&p_obj->cond, NULL);

    /* allocate buffers */
    p_obj->input.size = size * 3 / 2;
    rc = mm_jpeg_test_alloc(&p_obj->input, p_obj->use_ion);
    if(rc) {
        MMI_ALOGE("memory allocate failed");
        return -1;
    }

    p_obj->output.size = size * 3 / 2;
    rc = mm_jpeg_test_alloc(&p_obj->output, 1);
    if(rc) {
        MMI_ALOGE("memory allocate failed");
        return -1;
    }

    rc = mm_jpeg_test_read(p_obj, data);
    if(rc) {
        MMI_ALOGE("jpeg test read fail");
        return -1;
    }

    /* set encode parameters */
    p_params->jpeg_cb = mm_jpeg_encode_callback;
    p_params->userdata = p_obj;
    p_params->color_format = MM_JPEG_COLOR_FORMAT_YCRCBLP_H2V2;

    /* dest buffer config */
    p_params->dest_buf[0].buf_size = p_obj->output.size;
    p_params->dest_buf[0].buf_vaddr = p_obj->output.addr;
    p_params->dest_buf[0].fd = p_obj->output.p_pmem_fd;
    p_params->dest_buf[0].index = 0;
    p_params->num_dst_bufs = 1;

    /* src buffer config */
    p_params->src_main_buf[0].buf_size = p_obj->input.size;
    p_params->src_main_buf[0].buf_vaddr = p_obj->input.addr;
    p_params->src_main_buf[0].fd = p_obj->input.p_pmem_fd;
    p_params->src_main_buf[0].index = 0;
    p_params->src_main_buf[0].format = MM_JPEG_FMT_YUV;
    p_params->src_main_buf[0].offset.mp[0].len = (uint32_t) size;
    p_params->src_main_buf[0].offset.mp[0].stride = PAD_TO_SIZE(p_input->width, CAM_PAD_TO_16);
    p_params->src_main_buf[0].offset.mp[0].offset_x = 0;
    p_params->src_main_buf[0].offset.mp[0].offset_y = 0;
    p_params->src_main_buf[0].offset.mp[0].scanline = PAD_TO_SIZE(p_input->height, CAM_PAD_TO_2);
    p_params->src_main_buf[0].offset.mp[1].len = (uint32_t) (size >> 1);
    p_params->src_main_buf[0].offset.mp[1].offset_x = 0;
    p_params->src_main_buf[0].offset.mp[1].offset_y = 0;
    p_params->src_main_buf[0].offset.mp[1].stride = PAD_TO_SIZE(p_input->width, CAM_PAD_TO_16);
    p_params->src_main_buf[0].offset.mp[1].scanline = PAD_TO_SIZE(p_input->height / 2, CAM_PAD_TO_2);
    p_params->src_main_buf[0].offset.num_planes = 2;
    p_params->num_src_bufs = 1;

    p_params->src_thumb_buf[0].buf_size = p_obj->input.size;
    p_params->src_thumb_buf[0].buf_vaddr = p_obj->input.addr;
    p_params->src_thumb_buf[0].index = 0;
    p_params->src_thumb_buf[0].fd = p_obj->input.p_pmem_fd;
    p_params->src_thumb_buf[0].format = MM_JPEG_FMT_YUV;
    p_params->src_thumb_buf[0].offset = p_params->src_main_buf[0].offset;
    p_params->num_tmb_bufs = 1;

    p_params->encode_thumbnail = 1;
    p_params->quality = 80;

    p_job_params->dst_index = 0;
    p_job_params->src_index = 0;
    p_job_params->thumb_index = 0;
    p_job_params->rotation = 0;

    p_params->main_dim.src_dim.width = p_obj->width;
    p_params->main_dim.src_dim.height = p_obj->height;
    p_params->main_dim.dst_dim.width = p_obj->width;
    p_params->main_dim.dst_dim.height = p_obj->height;
    p_params->main_dim.crop.top = 0;
    p_params->main_dim.crop.left = 0;
    p_params->main_dim.crop.width = p_obj->width;
    p_params->main_dim.crop.height = p_obj->height;

    p_params->thumb_dim = p_params->main_dim;

    /* main dimension */
    p_job_params->main_dim.src_dim.width = p_obj->width;
    p_job_params->main_dim.src_dim.height = p_obj->height;
    p_job_params->main_dim.dst_dim.width = p_obj->width;
    p_job_params->main_dim.dst_dim.height = p_obj->height;
    p_job_params->main_dim.crop.top = 0;
    p_job_params->main_dim.crop.left = 0;
    p_job_params->main_dim.crop.width = p_obj->width;
    p_job_params->main_dim.crop.height = p_obj->height;

    /* thumb dimension */
    p_job_params->thumb_dim.src_dim.width = p_obj->width;
    p_job_params->thumb_dim.src_dim.height = p_obj->height;
    p_job_params->thumb_dim.dst_dim.width = p_obj->width;
    p_job_params->thumb_dim.dst_dim.height = p_obj->width;
    p_job_params->thumb_dim.crop.top = 0;
    p_job_params->thumb_dim.crop.left = 0;
    p_job_params->thumb_dim.crop.width = p_obj->width;
    p_job_params->thumb_dim.crop.height = p_obj->height;

    return 0;
}

#ifndef CAM_API_LEGACY
typedef uint32_t(*jpeg_open_t) (mm_jpeg_ops_t * ops, mm_jpeg_mpo_ops_t * mpo_ops,
                                mm_dimension picture_size, cam_related_system_calibration_data_t * calibration_data);
#else
typedef uint32_t(*jpeg_open_t) (mm_jpeg_ops_t *ops, mm_dimension picture_size);
#endif

static int encode_mmi(jpeg_test_input_t * p_input, void *data) {
    int rc = 0;
    mm_jpeg_intf_test_t jpeg_obj;
    int i = 0;
    mm_dimension picture_size;

    memset(&jpeg_obj, 0x0, sizeof(jpeg_obj));
    rc = encode_init(p_input, &jpeg_obj, data);
    if(rc) {
        MMI_ALOGE("encode init fail");
        return -1;
    }
    /*open mm-jpeg-interface.so */

    jpeg_open_t jpeg_open_func;
    void *libqcamera = dlopen("libmmjpeg_interface.so", RTLD_NOW);

    if(!libqcamera) {
        MMI_ALOGE("FATAL ERROR: lib(%s) open fail, error=%s\n", "libmmjpeg_interface.so", dlerror());
        return -1;
    } else {
        MMI_ALOGI("lib(%s) open success\n", "libmmjpeg_interface.so");
    }
    jpeg_open_func = (jpeg_open_t) dlsym(libqcamera, "jpeg_open");
    if(!jpeg_open_func) {
        MMI_ALOGE("FATAL ERROR: symbol(%s) loading fail, error=%s\n", "jpeg_open", dlerror());
        return -1;
    } else {
        MMI_ALOGI("symbol(%s) loading success\n", "jpeg_open");
    }

    picture_size.w = p_input->width;
    picture_size.h = p_input->height;
#ifndef CAM_API_LEGACY
    jpeg_obj.handle = jpeg_open_func(&jpeg_obj.ops, NULL, picture_size, NULL);
#else
    jpeg_obj.handle = jpeg_open_func(&jpeg_obj.ops, picture_size);
#endif
    if(jpeg_obj.handle == 0) {
        MMI_ALOGE("jpeg file(%s) open fail", p_input[0].out_filename);
        goto end;
    }

    rc = jpeg_obj.ops.create_session(jpeg_obj.handle, &jpeg_obj.params, &jpeg_obj.job.encode_job.session_id);
    if(jpeg_obj.job.encode_job.session_id == 0) {
        MMI_ALOGE("create session fail");
        goto end;
    }


    MMI_ALOGI("start encode jpeg file");
    jpeg_obj.job.job_type = JPEG_JOB_TYPE_ENCODE;
    rc = jpeg_obj.ops.start_job(&jpeg_obj.job, &jpeg_obj.job_id[0]);
    if(rc) {
        MMI_ALOGE("encode jpeg file fail");
        goto end;
    }

    pthread_mutex_lock(&jpeg_obj.lock);
    pthread_cond_wait(&jpeg_obj.cond, &jpeg_obj.lock);
    pthread_mutex_unlock(&jpeg_obj.lock);

    MMI_ALOGI("encode jpeg file(%s) finished", p_input[0].out_filename);

    jpeg_obj.ops.destroy_session(jpeg_obj.job.encode_job.session_id);

    jpeg_obj.ops.close(jpeg_obj.handle);


  end:
    mm_jpeg_test_free(&jpeg_obj.input);
    mm_jpeg_test_free(&jpeg_obj.output);
    return 0;
}

void do_capture() {
    jpeg_test_input_t jpeg_input[1];
    int width = g_mmi_cam.snapshot_dim.width;
    int height = g_mmi_cam.snapshot_dim.height;

    MMI_ALOGI("start snapshot(photo width=%d photo height=%d), photo file path:%s\n",
              width, height, g_mmi_cam.jpeg_name);

    jpeg_input[0].width = width;
    jpeg_input[0].height = height;
    jpeg_input[0].out_filename = g_mmi_cam.jpeg_name;
    encode_mmi(&jpeg_input[0], g_pPreviewYUV420);
}

int get_buffer_transform_mask(int degree, int mirror) {
    int transform = 0;

    if(mirror == 0) {
        if(degree == 90) {
            transform = HAL_TRANSFORM_ROT_90;
        } else if(degree == 270) {
            transform = HAL_TRANSFORM_ROT_270;
        }
    } else if(mirror == 1) {
        if(degree == 90) {
            transform = HAL_TRANSFORM_FLIP_V | HAL_TRANSFORM_ROT_90;
        } else if(degree == 270) {
            transform = HAL_TRANSFORM_FLIP_H | HAL_TRANSFORM_ROT_90;
        }
    }

    MMI_ALOGI("%s, transform: %d\n", __func__, transform);

    return transform;
}

/** mmi_cam_display_buffer:
*  @frame : frame info
*  @frame_idx: frame index
*
*  copy the preview data to share mem, so mmi main process can display it on screen.
**/
void mmi_cam_display_buffer(mm_camera_buf_def_t * frame, int frame_idx) {
    int preview_width = g_mmi_cam.preview_dim.width;
    int preview_height = g_mmi_cam.preview_dim.height;
    int rotation = g_mmi_cam.sensor_mount_angle;
    int32_t facing = g_mmi_cam.facing;
    int buf_size = mp0len + mp1len;

    if(frame == NULL) {
        MMI_ALOGE("Invalid parameter, frame is NULL\n");
        return;
    }
    // TODO: copy preview buffer for jpeg encode use. should remvoe it later...
    memcpy(g_pPreviewYUV420, (uint8_t *) frame->buffer, mp0len + mp1len);

    //split YUV to Y,UV component
    // YUV2_Y_UV(g_pPreviewYUV420);

    //   yuvtorgb8888(preview_width, preview_height, g_pPreview_Y, g_pPreview_UV, g_pPreviewRGB8888);

    //rotate the image
    //rotate_screen(g_pPreviewRGB8888, preview_width, preview_height, 4, rotation);

    sp < ANativeWindow > anw(g_cam_draw.surface);
    native_window_set_buffers_transform(anw.get(), get_buffer_transform_mask(rotation, (facing == CAM_FACING_FRONT)));
    ANativeWindow_setBuffersGeometry(anw.get(), PAD_TO_SIZE(preview_width, CAM_PAD_TO_16),  /* stride    */
                                     PAD_TO_SIZE(preview_height, CAM_PAD_TO_2), /* scanline */
                                     0x11); /*  pixel format  */

    ANativeWindow_lock(anw.get(), &outBuffer, NULL);
    memcpy(outBuffer.bits, (uint8_t *) frame->buffer, buf_size);
    ANativeWindow_unlockAndPost(anw.get());
}

void mmi_cam_user_preview_cb(mm_camera_buf_def_t * frame) {
    mmi_cam_display_buffer(frame, frame->frame_idx);
}

void mmi_cam_send_face_detect_result(int detected) {
    MMI_ALOGI("result for  face detected:%d", detected);
    char buf[128]={0};
    snprintf(buf, sizeof(buf), "Face Detection Result = %s \n",detected ==1?"Detected":"Not Detected" );
    //save data to result file
    g_module->cb_print(cur_module_name, SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
}

void mmi_cam_user_metadata_cb(mm_camera_buf_def_t * meta_frame) {

    int face_detected = 0;

    if(meta_frame == NULL) {
        MMI_ALOGE("Invalid parameter");
        return;
    }

#ifndef CAM_API_LEGACY
    metadata_buffer_t *p_metadata = (metadata_buffer_t *) meta_frame->buffer;
    //cam_metadata_info_t *p_metadata = g_mmi_cam.camera_obj.metadata;
    if(p_metadata == NULL) {
        return;
    }

    IF_META_AVAILABLE(cam_face_detection_data_t, faces_data,
            CAM_INTF_META_FACE_DETECTION, p_metadata) {
        MMI_ALOGI("%s, [FD_DBG] detected face nums: %d, frm_idx: %d\n", __func__,
                 faces_data->num_faces_detected, meta_frame->frame_idx);
        if (faces_data->num_faces_detected > 0) {
            frm_cnt_with_face++;
        }
    }
#else
    cam_metadata_info_t *p_metadata = (cam_metadata_info_t *) meta_frame->buffer;
    //cam_metadata_info_t *p_metadata = g_mmi_cam.camera_obj.metadata;
    if(p_metadata == NULL) {
        MMI_ALOGE("Invalid buffer addr");
        return;
    }

    if(p_metadata->is_faces_valid) {
        MMI_ALOGI("%s, [FD_DBG] detected face nums: %d, frm_idx: %d\n", __func__,
              p_metadata->faces_data.num_faces_detected, meta_frame->frame_idx);
        if(p_metadata->faces_data.num_faces_detected > 0) {
            frm_cnt_with_face++;
        }
    }
#endif

    if (g_mmi_cam.test_mode == TEST_MODE_UI) {
        if (face_detected) {
            frm_cnt_with_face++;
        }

        if (frm_cnt_with_face == 5) {
            mmi_cam_send_face_detect_result(1);
        }

    } else if (g_mmi_cam.test_mode == TEST_MODE_PCBA) {
        if (face_detected) {
            frm_cnt_with_face++;
        }
        if(frm_cnt_with_face >= 30) {
            frm_cnt_with_face = 0;
            MMI_ALOGI("frm cnt reaches to 30, signal pcba runnig thread to return");
            sem_post(&g_sem);
        }
    }
}

int32_t mmi_cam_find_out_camera_id(int32_t facing, int32_t idx) {
    int32_t camera_id = -1;
    uint32_t _idx = -1;

    if(idx < 0 || idx >= MAX_CAM_SENSOR_NUM) {
        MMI_ALOGE("Invalid camera index: %d\n", idx);
        return camera_id;
    }

    for(int i = 0; i < g_num_of_cameras; i++) {
#ifndef CAM_API_LEGACY
        cam_sync_type_t cam_sync_type;
        struct camera_info *info = get_cam_info(i, &cam_sync_type);

        MMI_ALOGI("camera%d info: facing:%s, mount angle:%d, cam sync type:%d\n",
                  i, (CAM_FACING_BACK == info->facing) ? "back" : "front", info->orientation, cam_sync_type);
#else
        struct camera_info *info = get_cam_info(i);
        MMI_ALOGI("camera%d info: facing:%s, mount angle:%d\n",
                  i, (CAM_FACING_BACK == info->facing) ? "back" : "front", info->orientation);
#endif

        if(info->facing == facing) {
            _idx++;
        }

        if(_idx == idx) {
            camera_id = i;
            break;
        }
    }

    MMI_ALOGI("camera id: %d\n", camera_id);
    return camera_id;
}

int mmi_cam_parse_params(unordered_map < string, string > &params) {

    const char *str_cam_positon = params[STR_KEY_CAM_POSITION].c_str();
    const char *str_cam_idx = params["cam-idx"].c_str();
    const char *str_preview_dim = params[KEY_DIMENSION].c_str();
    const char *str_test_func_area = params["type"].c_str();

    MMI_ALOGI("camera params info: facing type=%s, preview dimension=%s, camera index=%s\n",
              MMI_STR(str_cam_positon), MMI_STR(str_preview_dim), MMI_STR(str_cam_idx));

    int32_t facing = CAM_FACING_BACK;

    if(str_cam_positon != NULL) {
        if(!strcmp(str_cam_positon, "back")) {
            facing = CAM_FACING_BACK;
        } else if(!strcmp(str_cam_positon, "front")) {
            facing = CAM_FACING_FRONT;
        }
    }

    int32_t cam_idx = 0;

    if(str_cam_idx != NULL) {
        cam_idx = atoi(str_cam_idx);
        cam_idx = (cam_idx < 0) ? 0 : (cam_idx);
    }

    g_mmi_cam.facing = facing;
    g_mmi_cam.idx_by_facing = cam_idx;
    g_mmi_cam.cam_id = mmi_cam_find_out_camera_id(facing, cam_idx);
    if(g_mmi_cam.cam_id < 0) {
        MMI_ALOGE("error: invalid camera id\n");
        return FAILED;
    }

    if(str_preview_dim != NULL) {
        if(!strcmp(str_preview_dim, KEY_SMALL)) {
            g_mmi_cam.preview_dim.height = DIMENSION_SMALL_WIDTH;
            g_mmi_cam.preview_dim.width = DIMENSION_SMALL_HEIGHT;
        } else if(!strcmp(str_preview_dim, KEY_NORMAL)) {
            g_mmi_cam.preview_dim.height = DIMENSION_NORMAL_WIDTH;
            g_mmi_cam.preview_dim.width = DIMENSION_NORMAL_HEIGHT;
        } else if(!strcmp(str_preview_dim, KEY_LARGE)) {
            g_mmi_cam.preview_dim.height = DIMENSION_LARGE_WIDTH;
            g_mmi_cam.preview_dim.width = DIMENSION_LARGE_HEIGHT;
        }
    }

    if(str_test_func_area != NULL) {
        if(!strcmp(str_test_func_area, "flash")) {
            MMI_ALOGI("test type is flash, will test function of led flash\n");
            g_mmi_cam.test_func_area = TEST_FA_LED_FLASH;
        }
    }

    g_mmi_cam.snapshot_dim = g_mmi_cam.preview_dim;
    g_mmi_cam.preview_buff_size = g_mmi_cam.preview_dim.height * g_mmi_cam.preview_dim.width * 4;

    /* default jpeg file name: img_<mode>_<type>_<cam-idx>.jpg */
    g_mmi_cam.jpeg_name[0] = '\0';
    const char *str_jpg_name = params["filename"].c_str();

    if(str_jpg_name == NULL || str_jpg_name[0] == '\0') {
        snprintf(g_mmi_cam.jpeg_name, sizeof(g_mmi_cam.jpeg_name), "%s/img_%s_%s_%d.jpg",
                 get_value(KEY_FTM_AP_DIR),
                 (g_mmi_cam.test_mode == TEST_MODE_PCBA) ? "pcba" : "manual",
                 (g_mmi_cam.facing == 1) ? "front" : "back", g_mmi_cam.idx_by_facing);
    } else {
        memcpy(g_mmi_cam.jpeg_name, str_jpg_name, strlen(str_jpg_name) + 1);
    }

    MMI_ALOGI("camera info: camera id=%d, facing=%d, preview size=(width:%d x high:%d), "
              "preview buf size=%d, jpeg file=%s\n",
              g_mmi_cam.cam_id,
              g_mmi_cam.facing,
              g_mmi_cam.preview_dim.width,
              g_mmi_cam.preview_dim.height, g_mmi_cam.preview_buff_size, g_mmi_cam.jpeg_name);
    return SUCCESS;
}


int mmi_cam_open_camera() {
    mm_camera_app_t *p_cam_app = &g_mmi_cam.app_ctx;
    mm_camera_test_obj_t *p_camera_obj = &g_mmi_cam.camera_obj;

    /*load hal interface */
    memset(p_cam_app, 0, sizeof(mm_camera_app_t));
    g_mmi_cam.app_ctx.num_cameras = g_num_of_cameras;
    if((mm_app_load_hal(p_cam_app) != MM_CAMERA_OK)) {
        MMI_ALOGE("loading camera hal fail\n");
        goto error;
    }

    /*open camera */
    memset(p_camera_obj, 0, sizeof(mm_camera_test_obj_t));
    MMI_ALOGI("opening camera with id: %d", g_mmi_cam.cam_id);
    if(mm_app_open(p_cam_app, g_mmi_cam.cam_id, p_camera_obj) != MM_CAMERA_OK) {
        MMI_ALOGE("opening camera with id(%d) fail\n", g_mmi_cam.cam_id);
        goto error;
    }
    MMI_ALOGI("opening camera with id(%d) success\n", g_mmi_cam.cam_id);
    return SUCCESS;

  error:
    if(p_cam_app->hal_lib.ptr_jpeg != NULL) {
        dlclose(p_cam_app->hal_lib.ptr_jpeg);
    }

    if(p_cam_app->hal_lib.ptr != NULL) {
        dlclose(p_cam_app->hal_lib.ptr);
    }

    return FAILED;
}

int mmi_cam_get_capabilites() {
    //get the sensor's mount angle
    cam_capability_t *cam_cap = (cam_capability_t *) (g_mmi_cam.camera_obj.cap_buf.buf.buffer);

    g_mmi_cam.sensor_mount_angle = cam_cap->sensor_mount_angle;

    g_mmi_cam.is_af_supported = FALSE;
    g_mmi_cam.focus_mode = CAM_FOCUS_MODE_FIXED;
    if(cam_cap->supported_focus_modes_cnt > 0) {
        for(int i = 0; i < cam_cap->supported_focus_modes_cnt; i++) {
            if(cam_cap->supported_focus_modes[i] == CAM_FOCUS_MODE_AUTO) {
                g_mmi_cam.is_af_supported = TRUE;
                g_mmi_cam.focus_mode = CAM_FOCUS_MODE_AUTO;
                break;
            }
        }
    }

    g_mmi_cam.is_fd_supported = (cam_cap->max_num_roi > 0) ? 1 : 0;

    MMI_ALOGI("camera capabilites info: mount angle=%d, auto focus support=%s, face detection support=%s\n",
              g_mmi_cam.sensor_mount_angle,
              (TRUE == g_mmi_cam.is_af_supported) ? "YES" : "NO", g_mmi_cam.is_fd_supported ? "YES" : "NO");
    return SUCCESS;
}

int mmi_cam_set_init_params() {
    int rc = 0;
    mm_camera_test_obj_t *p_camera_obj = &g_mmi_cam.camera_obj;

    p_camera_obj->buffer_width = g_mmi_cam.preview_dim.width;
    p_camera_obj->buffer_height = g_mmi_cam.preview_dim.height;
    p_camera_obj->preview_resolution.user_input_display_width = p_camera_obj->buffer_width;
    p_camera_obj->preview_resolution.user_input_display_height = p_camera_obj->buffer_height;

    rc = setFocusMode(&g_mmi_cam.camera_obj, g_mmi_cam.focus_mode);
    if(rc != MM_CAMERA_OK) {
        MMI_ALOGE("set focus mode fail\n");
        return FAILED;
    }
    MMI_ALOGI("set focus mode to: %d\n", g_mmi_cam.focus_mode);

    // enable face detection if we're testing preview
    if(g_mmi_cam.test_func_area == TEST_FA_PREVIEW) {
        if(g_mmi_cam.is_fd_supported) {
            cam_fd_set_parm_t fd_set_parm;

            memset(&fd_set_parm, 0, sizeof(cam_fd_set_parm_t));
            fd_set_parm.fd_mode = 1;
            fd_set_parm.num_fd = 1;
            rc = mm_app_set_face_detection(&g_mmi_cam.camera_obj, &fd_set_parm);
            if(rc != MM_CAMERA_OK) {
                MMI_ALOGE("set face detection fail\n");
                return FAILED;
            }
            MMI_ALOGI("enable face detection\n");
        }
    }
#if 0
    /* set Preview FPS Range */
    cam_fps_range_t preview_fps_range;

    memset(&preview_fps_range, 0, sizeof(cam_fps_range_t));
    preview_fps_range.min_fps = 30.0;
    preview_fps_range.max_fps = 30.0;
    if(mm_app_set_preview_fps_range_lib != NULL) {
        mm_app_set_preview_fps_range_lib(&g_mmi_cam.camera_obj, &preview_fps_range);
    }
#endif

    /*set EV */
    //    if (mm_app_set_params != NULL) {
    //        mm_app_set_params(&g_mmi_cam.camera_obj, CAM_INTF_PARM_AEC_ALGO_TYPE, 0);
    //        mm_app_set_params(&g_mmi_cam.camera_obj, CAM_INTF_PARM_ANTIBANDING, CAM_ANTIBANDING_MODE_AUTO);
    //    }

    return SUCCESS;
}

/** mmi_cam_create_buffers:
 *
 *  allocate new buffers for local usage, such as preview, JPEG enc, etc...
 *  import buffers via mmap(), for sharing buffers between processes.
 *
 **/
static int mmi_cam_create_buffers() {
    int width = g_mmi_cam.preview_dim.width;
    int height = g_mmi_cam.preview_dim.height;
    int preview_pixels = height * width;

    allocate_buffers(width, height);

    int preview_buf_size = preview_pixels * 4;

    return SUCCESS;
}

static int mmi_cam_destroy_buffers() {
    dealloc_buffers();
    // unmap_buffers();

    return SUCCESS;
}

int mmi_cam_set_user_preview_cb(cam_stream_user_cb user_preview_cb) {

    if(g_mmi_cam.camera_obj.user_preview_cb != NULL) {
        MMI_ALOGW("already set user custom preview callback");
    }
    g_mmi_cam.camera_obj.user_preview_cb = user_preview_cb;

    return SUCCESS;
}

int mmi_cam_start_preview() {
    MMI_ALOGI("start camera preview\n");

    create_surface(&g_cam_draw);
    return mm_app_start_preview(&g_mmi_cam.camera_obj);
}

int mmi_cam_stop_preview() {
    int rc = 0;

    rc = mm_app_stop_preview(&g_mmi_cam.camera_obj);
    remove_surface(&g_cam_draw);
    MMI_ALOGI("stop camera preview\n");

    return rc;
}

int mmi_cam_close_camera() {
    mm_camera_app_t *p_cam_app = &g_mmi_cam.app_ctx;
    mm_camera_test_obj_t *p_camera_obj = &g_mmi_cam.camera_obj;

    MMI_ALOGI("close camera\n");

    mm_app_close(p_camera_obj);

    // unload hal
    if(p_cam_app->hal_lib.ptr_jpeg != NULL) {
        dlclose(p_cam_app->hal_lib.ptr_jpeg);
    }

    if(p_cam_app->hal_lib.ptr != NULL) {
        dlclose(p_cam_app->hal_lib.ptr);
    }

    return SUCCESS;
}

int mmi_cam_do_auto_focus(int sync_flag) {
    int rc = 0;

    MMI_ALOGI("start auto focus\n");
    g_mmi_cam.camera_obj.cam->ops->do_auto_focus(g_mmi_cam.camera_obj.cam->camera_handle);
    mm_camera_app_wait();
    MMI_ALOGI("auto focus finished\n");

    return SUCCESS;
}

int set_led_flash_mode(int led_on) {
    cam_flash_mode_t led_flash_mode = CAM_FLASH_MODE_OFF;

    if(led_on == 1) {
        led_flash_mode = CAM_FLASH_MODE_TORCH;
    }

    MMI_ALOGI("setting led flash mode: %s\n", (1 == led_on) ? "TORCH" : "OFF");
    return mm_app_set_flash_mode(&g_mmi_cam.camera_obj, led_flash_mode);
}

void *switch_led_onoff_thread_func(void *arg) {
    int led_mode = 1;

    while(g_mmi_cam.is_preview_running) {
        set_led_flash_mode(led_mode);
        led_mode = (led_mode + 1) % 2;
        MMI_ALOGI("change led mode to %s\n", !led_mode ? "off" : "on");
        sleep(1);
    }

    return (void *) NULL;
}

int pcba_get_test_profile(unordered_map < string, string > &params, pcba_test_profile_t * profile) {

    if(profile == NULL) {
        MMI_ALOGE("Invalid parameter");
        return 0;
    }
    memset(profile, 0, sizeof(pcba_test_profile_t));

    const char *str_type = params["type"].c_str();

    MMI_ALOGI("params info: type=%s, method=%s, switch=%s\n",
              MMI_STR(params["type"].c_str()), MMI_STR(params["method"].c_str()), MMI_STR(params["switch"].c_str()));

    if(str_type == NULL) {
        MMI_ALOGE("Invalid param <type>, type=%s\n", MMI_STR(str_type));
        return -1;
    }

    if(!strcmp(str_type, "flash")) {
        profile->type = TEST_TYPE_FLASHLIGHT;
    } else if(!strcmp(str_type, "back") || !strcmp(str_type, "front")) {
        profile->type = TEST_TYPE_IMG_PIPELINE;
    } else if(!strcmp(str_type, "vcm")) {
        profile->type = TEST_TYPE_VCM;
    } else {
        MMI_ALOGE("Invalid param <type>, type=%s\n", MMI_STR(str_type));
    }

    const char *str_method = params["method"].c_str();

    if(str_method != NULL) {
        if(STR_EQUAL(str_method, "preview")) {
            profile->method = TEST_METHOD_PREVIEW;
        } else if(STR_EQUAL(str_method, "snapshot")) {
            profile->method = TEST_METHOD_SNAPSHOT;
        } else if(STR_EQUAL(str_method, "torch")) {
            profile->method = TEST_METHOD_TORCH;
        } else if(STR_EQUAL(str_method, "blink")) {
            profile->method = TEST_METHOD_BLINK;
        } else {
            MMI_ALOGE("Invalid param <method>, method=%s\n", MMI_STR(str_method));
        }
    }

    const char *str_switch = params["switch"].c_str();

    if(str_switch != NULL) {
        if(!strcmp(str_switch, "on")) {
            profile->ext_flag = TEST_FLAG_FLASHLIGHT_ON;
        } else if(!strcmp(str_switch, "off")) {
            profile->ext_flag = TEST_FLAG_FLASHLIGHT_OFF;
        } else {
            MMI_ALOGE("Invalid param <switch>, switch=%s\n", MMI_STR(str_switch));
        }
    }

    MMI_ALOGI("pcba test profile: type=%d, method=%d, ext_flag=%d\n",
              profile->type, profile->method, profile->ext_flag);
    return 0;
}

int mmi_cam_pcba_test_preview(unordered_map < string, string > &params) {
    int rc = 0;

    pthread_mutex_lock(&g_mutex_lock);

    memset(&g_mmi_cam, 0, sizeof(g_mmi_cam));
    g_mmi_cam.test_mode = TEST_MODE_PCBA;
    g_mmi_cam.test_func_area = TEST_FA_PREVIEW;

    g_mmi_cam.cam_id = DEFAULT_CAMERA_ID;
    strlcpy(g_mmi_cam.dev_name, DEFAULT_CAMERA_DEVICE, sizeof(g_mmi_cam.dev_name));

    g_mmi_cam.preview_dim.width = DEFAULT_PREVIEW_WIDTH;
    g_mmi_cam.preview_dim.height = DEFAULT_PREVIEW_HEIGHT;
    g_mmi_cam.preview_buff_size = g_mmi_cam.preview_dim.width * g_mmi_cam.preview_dim.width;
    g_mmi_cam.snapshot_dim = g_mmi_cam.preview_dim;

    if(mmi_cam_parse_params(params) != SUCCESS) {
        MMI_ALOGE("fail to parse parameters\n");
        goto ERROR;
    }

    if(mmi_cam_open_camera() != SUCCESS) {
        MMI_ALOGE("fail to open camera\n");
        goto ERROR;
    }

    if(mmi_cam_get_capabilites() != SUCCESS) {
        MMI_ALOGE("fail to get capabilities\n");
        goto ERROR;
    }

    if(mmi_cam_set_init_params() != SUCCESS) {
        MMI_ALOGE("fail to set focus or enable face detection\n");
        goto ERROR;
    }

    if(mm_app_set_metadata_usercb(&g_mmi_cam.camera_obj, mmi_cam_user_metadata_cb) != SUCCESS) {
        MMI_ALOGE("fail to set user metadata callback function\n");
        goto ERROR;
    }

    if(mmi_cam_create_buffers() != SUCCESS) {
        MMI_ALOGE("fail to create buffers\n");
        goto ERROR;
    }

    if(mmi_cam_set_user_preview_cb(&mmi_cam_user_preview_cb) != SUCCESS) {
        MMI_ALOGE("fail to set user preview callback function!\n");
        goto ERROR;
    }

    /*start preview */
    mmi_cam_start_preview();

    // perform auto focus if supported

    // wait for fd detection result in metadata_stream_cb()
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;
    rc = sem_timedwait(&g_sem, &ts);
    if(rc == -1) {
        if(errno == ETIMEDOUT) {
            MMI_ALOGE("wait 5 seconds fd time out\n");
        } else {
            MMI_ALOGE("wait 5 seconds fd error, errno=%s\n", strerror(errno));
        }
        rc = FAILED;
    } else {
        MMI_ALOGI("sem_timedwait waked up by post\n");
    }

    /*stop preview */
    mmi_cam_stop_preview();

    mmi_cam_destroy_buffers();
    mmi_cam_close_camera();

    pthread_mutex_unlock(&g_mutex_lock);
    if(FAILED != rc) {
        MMI_ALOGI("pcba preview test pass");
    } else {
        MMI_ALOGE("pcba preview test fail");
    }
    return rc;

  ERROR:
    pthread_mutex_unlock(&g_mutex_lock);
    MMI_ALOGE("pcba preview test fail");
    return FAILED;
}

int mmi_cam_pcba_test_snapshot(unordered_map < string, string > &params) {
    int rc = 0;

    pthread_mutex_lock(&g_mutex_lock);

    memset(&g_mmi_cam, 0, sizeof(g_mmi_cam));
    g_mmi_cam.test_mode = TEST_MODE_PCBA;
    g_mmi_cam.test_func_area = TEST_FA_PREVIEW;

    g_mmi_cam.cam_id = DEFAULT_CAMERA_ID;
    strlcpy(g_mmi_cam.dev_name, DEFAULT_CAMERA_DEVICE, sizeof(g_mmi_cam.dev_name));

    g_mmi_cam.preview_dim.width = DEFAULT_PREVIEW_WIDTH;
    g_mmi_cam.preview_dim.height = DEFAULT_PREVIEW_HEIGHT;
    g_mmi_cam.preview_buff_size = g_mmi_cam.preview_dim.width * g_mmi_cam.preview_dim.width;
    g_mmi_cam.snapshot_dim = g_mmi_cam.preview_dim;

    if(mmi_cam_parse_params(params) != SUCCESS) {
        MMI_ALOGE("fail to parse parameters\n");
        goto ERROR;
    }

    if(mmi_cam_open_camera() != SUCCESS) {
        MMI_ALOGE("fail to open camera\n");
        goto ERROR;
    }

    if(mmi_cam_get_capabilites() != SUCCESS) {
        MMI_ALOGE("fail to get capabilities\n");
        goto ERROR;
    }

    if(mmi_cam_set_init_params() != SUCCESS) {
        MMI_ALOGE("fail to set focus or enable face detection\n");
        goto ERROR;
    }

    if(mmi_cam_create_buffers() != SUCCESS) {
        MMI_ALOGE("fail to create buffers\n");
        goto ERROR;
    }

    if(mmi_cam_set_user_preview_cb(&mmi_cam_user_preview_cb) != SUCCESS) {
        MMI_ALOGE("fail to set user preview callback function\n");
        goto ERROR;
    }

    /*start preview */
    mmi_cam_start_preview();
    g_mmi_cam.is_preview_running = 1;

    sleep(1);
    if(g_mmi_cam.is_af_supported == TRUE) {
        mmi_cam_do_auto_focus(1);
    }

    /*stop preview */
    g_mmi_cam.is_preview_running = 0;
    mmi_cam_stop_preview();

    do_capture();

    mmi_cam_destroy_buffers();
    mmi_cam_close_camera();

    pthread_mutex_unlock(&g_mutex_lock);
    MMI_ALOGI("pcba snapshot test pass");
    return rc;

  ERROR:
    pthread_mutex_unlock(&g_mutex_lock);
    MMI_ALOGE("pcba snapshot test fail");
    return FAILED;
}

int mmi_cam_pcba_test_flashlight(unordered_map < string, string > &params) {
    int rc = 0;

    pthread_mutex_lock(&g_mutex_lock);

    memset(&g_mmi_cam, 0, sizeof(g_mmi_cam));
    g_mmi_cam.test_mode = TEST_MODE_PCBA;
    g_mmi_cam.test_func_area = TEST_FA_LED_FLASH;

    g_mmi_cam.cam_id = DEFAULT_CAMERA_ID;
    strlcpy(g_mmi_cam.dev_name, DEFAULT_CAMERA_DEVICE, sizeof(g_mmi_cam.dev_name));

    g_mmi_cam.preview_dim.width = DEFAULT_PREVIEW_WIDTH;
    g_mmi_cam.preview_dim.height = DEFAULT_PREVIEW_HEIGHT;
    g_mmi_cam.preview_buff_size = g_mmi_cam.preview_dim.width * g_mmi_cam.preview_dim.width;
    g_mmi_cam.snapshot_dim = g_mmi_cam.preview_dim;

    // don't need to parse params here..
    // if (mmi_cam_parse_params(params) != SUCCESS) {
    //     MMI_ALOGE("%s, fail to parse parameters!\n", __func__);
    //     goto ERROR;
    // }

    if(mmi_cam_open_camera() != SUCCESS) {
        MMI_ALOGE("fail to open camera\n");
        goto ERROR;
    }

    if(mmi_cam_get_capabilites() != SUCCESS) {
        MMI_ALOGE("fail to get capabilities\n");
        goto ERROR;
    }

    if(mmi_cam_set_init_params() != SUCCESS) {
        MMI_ALOGE("fail to set focus or enable face detection\n");
        goto ERROR;
    }

    set_led_flash_mode(1);

    /*start preview */
    mmi_cam_start_preview();

    /*wait */
    sem_wait(&g_sem);

    g_mmi_cam.is_preview_running = 0;

    /*stop preview */
    mmi_cam_stop_preview();
    mmi_cam_close_camera();

    pthread_mutex_unlock(&g_mutex_lock);
    MMI_ALOGI("pcba flashlight test pass");
    return rc;

  ERROR:
    pthread_mutex_unlock(&g_mutex_lock);
    MMI_ALOGE("pcba flashlight test fail");
    return FAILED;
}

/**
* Defined case run in mmi mode,this mode support UI.
* @return, 0 -success; -1
*/
static int32_t mmi_cam_module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("run mmi start for module:[%s]", module->name);

    g_module = module;
   frm_cnt_with_face = 0;
   char *m = "Face Detection Result = Not Detected";
   g_module->cb_print(cur_module_name, SUBCMD_MMI,m, strlen(m), PRINT_DATA);

    pthread_mutex_lock(&g_mutex_lock);

    memset(&g_mmi_cam, 0, sizeof(g_mmi_cam));
    g_mmi_cam.test_mode = TEST_MODE_UI;
    g_mmi_cam.test_func_area = TEST_FA_PREVIEW;

    g_mmi_cam.cam_id = DEFAULT_CAMERA_ID;
    strlcpy(g_mmi_cam.dev_name, DEFAULT_CAMERA_DEVICE, sizeof(g_mmi_cam.dev_name));

    g_mmi_cam.preview_dim.width = DEFAULT_PREVIEW_WIDTH;
    g_mmi_cam.preview_dim.height = DEFAULT_PREVIEW_HEIGHT;
    g_mmi_cam.preview_buff_size = g_mmi_cam.preview_dim.width * g_mmi_cam.preview_dim.width;
    g_mmi_cam.snapshot_dim = g_mmi_cam.preview_dim;

    if(mmi_cam_parse_params(params) != SUCCESS) {
        MMI_ALOGE("fail to parse parameters\n");
        goto ERROR;
    }

    if(mmi_cam_open_camera() != SUCCESS) {
        MMI_ALOGE("fail to open camera\n");
        goto ERROR;
    }

    if(mmi_cam_get_capabilites() != SUCCESS) {
        MMI_ALOGE("fail to get capabilities\n");
        goto ERROR;
    }

    if(mmi_cam_set_init_params() != SUCCESS) {
        MMI_ALOGE("fail to set focus or enable face detection\n");
        goto ERROR;
    }

    if(mm_app_set_metadata_usercb(&g_mmi_cam.camera_obj, mmi_cam_user_metadata_cb) != SUCCESS) {
        MMI_ALOGE("fail to set user metadata callback function\n");
        goto ERROR;
    }

    if(mmi_cam_create_buffers() != SUCCESS) {
        MMI_ALOGE("fail to create buffers\n");
        goto ERROR;
    }

    if(g_mmi_cam.test_func_area != TEST_FA_LED_FLASH) {
        if(mmi_cam_set_user_preview_cb(&mmi_cam_user_preview_cb) != SUCCESS) {
            MMI_ALOGE("fail to set user preview callback function\n");
            goto ERROR;
        }
    }


  STAR_PREVIEW:

    /*start preview */
    mmi_cam_start_preview();
    g_mmi_cam.is_preview_running = 1;

    pthread_t tid_led_blink;

    if(g_mmi_cam.test_func_area == TEST_FA_LED_FLASH) {
        pthread_create(&tid_led_blink, NULL, switch_led_onoff_thread_func, NULL);
        MMI_ALOGI("create thread(thread id=%lu) for switch led on/off\n", tid_led_blink);
    }

    /*wait */
    sem_wait(&g_sem);

    g_mmi_cam.is_preview_running = 0;
    if(g_mmi_cam.test_func_area == TEST_FA_LED_FLASH) {
        pthread_join(tid_led_blink, (void **) NULL);
        MMI_ALOGI("switch led on/off thread(thread id=%lu) be stoped\n", tid_led_blink);
    }

    /*stop preview */
    mmi_cam_stop_preview();

    if(g_encode_after_stop == 1) {
        do_capture();
        g_encode_after_stop = 0;
        goto STAR_PREVIEW;
    }

    mmi_cam_destroy_buffers();
    mmi_cam_close_camera();

    pthread_mutex_unlock(&g_mutex_lock);
    MMI_ALOGI("module:[%s] test pass", module->name);
    MMI_ALOGI("run mmi finished for module:[%s]", module->name);
    return SUCCESS;

  ERROR:
    pthread_mutex_unlock(&g_mutex_lock);
    MMI_ALOGE("module:[%s] test fail", module->name);
    return FAILED;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t mmi_cam_module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    int rc = 0;

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("run pcba start for module:[%s]", module->name);

    g_module = module;

    pcba_test_profile_t test_profile;

    pcba_get_test_profile(params, &test_profile);

    if(test_profile.type == TEST_TYPE_IMG_PIPELINE) {
        if(test_profile.method == TEST_METHOD_PREVIEW) {
            rc = mmi_cam_pcba_test_preview(params);
        } else if(test_profile.method == TEST_METHOD_SNAPSHOT) {
            rc = mmi_cam_pcba_test_snapshot(params);
        } else {
            MMI_ALOGW("Invalid method:%d, test preview as default\n", test_profile.method);
            rc = mmi_cam_pcba_test_preview(params);
        }

        MMI_ALOGI("module:[%s] test %s", module->name, MMI_TEST_RESULT(rc));
    } else if(test_profile.type == TEST_TYPE_FLASHLIGHT) {
        if(test_profile.ext_flag == TEST_FLAG_FLASHLIGHT_ON) {
            rc = mmi_cam_pcba_test_flashlight(params);

            MMI_ALOGI("module:[%s] test %s", module->name, MMI_TEST_RESULT(rc));
        } else if(test_profile.ext_flag == TEST_FLAG_FLASHLIGHT_OFF) {
            MMI_ALOGI("signal pcba flashlight test thread to stop\n");
            sem_post(&g_sem);
            rc = 0;
        } else {
            MMI_ALOGE("Invalid ext_flag: %d\n", test_profile.ext_flag);
        }
    } else {
        MMI_ALOGE("invalid or un-implemented test type: %d\n", test_profile.type);
    }

    MMI_ALOGI("run pcba finished for module:[%s]", module->name);
    return rc;
}

/**
* Defined OEM specified run cases.
*
*/
static int32_t mmi_cam_module_run_snapshot(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("run snapshot start for module:[%s]", module->name);

    // TODO: need to check preview is started or not!!

    // TODO: not allowed in pcba mode.

    if(g_mmi_cam.is_af_supported == TRUE) {
        mmi_cam_do_auto_focus(1);
    }

    MMI_ALOGI("signal mmi runnig thread to take a picture");
    g_encode_after_stop = 1;
    sem_post(&g_sem);

    MMI_ALOGI("run snapshot finished for module:[%s]", module->name);
    return SUCCESS;
}

static int32_t mmi_cam_module_run_test2(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    return SUCCESS;
}

static int32_t mmi_cam_module_init(const mmi_module_t * module, unordered_map < string, string > &params) {

    char value[PROPERTY_VALUE_MAX] = { 0 };

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    // we should dlsym libmmqcamera.so here to save up time during run_mmi()

    sem_init(&g_sem, 0, 0);

    property_get("persist.camera.expose.aux", value, "0");
    g_mmi_cam.is_aux_camera_supported = atoi(value);

    // workaround : enable aux camera before query camera
    property_set("persist.camera.expose.aux", "1");

    // wait for mm-qcamera-daemon to start successfully.
    sleep(5);

    // enum all camera sensors for the first call to module init
    // back camera, front camera, led flash will invoke module_init() parallelly,
    // should enum camera sensors only once.
    // using pthread_once()??
    mmi_query_camera_sensors();


    /**Init */
    init_draw(params);
    init_surface(&g_cam_draw);

    MMI_ALOGI("module init finished for module:[%s]", module->name);
    return SUCCESS;
}

static int32_t mmi_cam_module_deinit(const mmi_module_t * module) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module deinit start for module:[%s]", module->name);

    sem_close(&g_sem);

    MMI_ALOGI("module deinit finished for module:[%s]", module->name);
    return SUCCESS;
}

static int32_t mmi_cam_module_stop(const mmi_module_t * module) {
    char value[PROPERTY_VALUE_MAX] = { 0 };

    value[0] = g_mmi_cam.is_aux_camera_supported ? '1': '0';
    property_set("persist.camera.expose.aux", value);
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }

    mutex_locker::autolock _L(g_mutex);
    MMI_ALOGI("module stop start for module:[%s]", module->name);

    g_encode_after_stop = 0;
    if(g_mmi_cam.is_preview_running) {
        MMI_ALOGI("signal mmi runnig thread to stop");
        sem_post(&g_sem);
    }

    remove_surface(&g_cam_draw);
    MMI_ALOGI("module stop finished for module:[%s]", module->name);
    return SUCCESS;
}

/**
* Before call Run function, caller should call module_init first to initialize the module.
* the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
*
*/
static int32_t mmi_cam_module_run(const mmi_module_t * module, const char *cmd, unordered_map < string,
                                  string > &params) {
    int ret = -1;

    if(!module || !cmd) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module run start for module:[%s]", module->name);

    //remove_surface(&g_cam_draw);
    init_draw(params);
    strlcpy(cur_module_name, params[KEY_MODULE_NAME].c_str(), sizeof(cur_module_name));
    //create_surface(&g_cam_draw);

    if(!strcmp(cmd, SUBCMD_MMI))
        ret = mmi_cam_module_run_mmi(module, params);
    //ret = mmi_cam_module_run_pcba(module, params);
    else if(!strcmp(cmd, SUBCMD_PCBA))
        ret = mmi_cam_module_run_pcba(module, params);
    else if(!strcmp(cmd, SUBCMD_SNAPSHOT))
        ret = mmi_cam_module_run_snapshot(module, params);
    else {
        MMI_ALOGE("Received invalid command: %s", MMI_STR(cmd));
        ret = FAILED;
    }

    MMI_ALOGI("module run finished for module:[%s]", module->name);
   /** Default RUN mmi*/
    return ret;
}

/**
* Methods must be implemented by module.
*/
static struct mmi_module_methods_t module_methods = {
    .module_init = mmi_cam_module_init,
    .module_deinit = mmi_cam_module_deinit,
    .module_run = mmi_cam_module_run,
    .module_stop = mmi_cam_module_stop,
};

/**
* Every mmi module must have a data structure named MMI_MODULE_INFO_SYM
* and the fields of this data structure must be initialize in strictly sequence as definition,
* please don't change the sequence as g++ not supported in CPP file.
*/
mmi_module_t MMI_MODULE_INFO_SYM = {
    .version_major = 1,
    .version_minor = 0,
    .name = "Camera",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = extra_cmd_list,
    .supported_cmd_list_size = sizeof(extra_cmd_list) / sizeof(char *),
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
