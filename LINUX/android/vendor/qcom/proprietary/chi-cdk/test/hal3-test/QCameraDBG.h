////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/

#ifndef __MM_CAMERA_DBG_H__
#define __MM_CAMERA_DBG_H__

// System dependencies
#include <utils/Log.h>

#ifdef QCAMERA_REDEFINE_LOG

// Camera dependencies
#include "cam_types.h"

typedef enum {
    CAM_NO_MODULE,
    CAM_HAL_MODULE,
    CAM_MCI_MODULE,
    CAM_JPEG_MODULE,
    CAM_LAST_MODULE
} cam_modules_t;

/* values that persist.camera.global.debug can be set to */
/* all camera modules need to map their internal debug levels to this range */
typedef enum {
    CAM_GLBL_DBG_NONE  = 0,
    CAM_GLBL_DBG_ERR   = 1,
    CAM_GLBL_DBG_WARN  = 2,
    CAM_GLBL_DBG_HIGH  = 3,
    CAM_GLBL_DBG_DEBUG = 4,
    CAM_GLBL_DBG_LOW   = 5,
    CAM_GLBL_DBG_INFO  = 6
} cam_global_debug_level_t;

extern int g_cam_log[CAM_LAST_MODULE][CAM_GLBL_DBG_INFO + 1];

#define FATAL_IF(cond, ...) LOG_ALWAYS_FATAL_IF(cond, ## __VA_ARGS__)

#undef CLOGx
#define CLOGx(module, level, fmt, args...)                         \
{\
if (g_cam_log[module][level]) {                                  \
  mm_camera_debug_log(module, level, __func__, __LINE__, fmt, ##args); \
}\
}

#undef CLOGI
#define CLOGI(module, fmt, args...)                \
    CLOGx(module, CAM_GLBL_DBG_INFO, fmt, ##args)
#undef CLOGD
#define CLOGD(module, fmt, args...)                \
    CLOGx(module, CAM_GLBL_DBG_DEBUG, fmt, ##args)
#undef CLOGL
#define CLOGL(module, fmt, args...)                \
    CLOGx(module, CAM_GLBL_DBG_LOW, fmt, ##args)
#undef CLOGW
#define CLOGW(module, fmt, args...)                \
    CLOGx(module, CAM_GLBL_DBG_WARN, fmt, ##args)
#undef CLOGH
#define CLOGH(module, fmt, args...)                \
    CLOGx(module, CAM_GLBL_DBG_HIGH, fmt, ##args)
#undef CLOGE
#define CLOGE(module, fmt, args...)                \
    CLOGx(module, CAM_GLBL_DBG_ERR, fmt, ##args)

#ifndef CAM_MODULE
#define CAM_MODULE CAM_MCI_MODULE
#endif

#undef LOGD
#define LOGD(fmt, args...) CLOGD(CAM_MODULE, fmt, ##args)
#undef LOGL
#define LOGL(fmt, args...) CLOGL(CAM_MODULE, fmt, ##args)
#undef LOGW
#define LOGW(fmt, args...) CLOGW(CAM_MODULE, fmt, ##args)
#undef LOGH
#define LOGH(fmt, args...) CLOGH(CAM_MODULE, fmt, ##args)
#undef LOGE
#define LOGE(fmt, args...) CLOGE(CAM_MODULE, fmt, ##args)
#undef LOGI
#define LOGI(fmt, args...) CLOGI(CAM_MODULE, fmt, ##args)

/* reads and updates camera logging properties */
void mm_camera_set_dbg_log_properties(void);

/* generic logger function */
void mm_camera_debug_log(const cam_modules_t module,
                   const cam_global_debug_level_t level,
                   const char *func, const int line, const char *fmt, ...);

void mm_camera_debug_open(void);
void mm_camera_debug_close(void);

#else

#undef LOGD
#define LOGD(fmt, args...) ALOGD(fmt, ##args)
#undef LOGL
#define LOGL(fmt, args...) ALOGD(fmt, ##args)
#undef LOGW
#define LOGW(fmt, args...) ALOGW(fmt, ##args)
#undef LOGH
#define LOGH(fmt, args...) ALOGD(fmt, ##args)
#undef LOGE
#define LOGE(fmt, args...) ALOGE(fmt, ##args)
#undef LOGI
#define LOGI(fmt, args...) ALOGV(fmt, ##args)

#endif

#endif /* __MM_CAMERA_DBG_H__ */
