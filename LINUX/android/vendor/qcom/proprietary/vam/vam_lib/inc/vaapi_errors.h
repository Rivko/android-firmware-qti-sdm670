/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAMERROR_H_
#define _VAMERROR_H_

enum VAMErrors {
    VAM_OK = 0,
    VAM_FAIL,
    VAM_NOTFOUND,
    VAM_TIMEOUT,

    VAM_NULLPTR,
    VAM_NOT_INITED,

    VAM_IMG_FORMAT_NOT_SUPPORTED,

    VAM_LOCK_FAIL,
    VAM_UNLOCK_FAIL,

    VAM_FRAME_QUEUE_FULL,

    VAM_ENGINE_IS_NOT_RUNNING,
    VAM_ENGINE_HAS_NO_CONFIG,
    VAM_ENGINE_DYLIB_PATH_NOT_SET,
    VAM_ENGINE_DYLIB_NOT_EXIST,
    VAM_ENGINE_SYMBOL_ERROR,

    VAM_SETRULE_FAIL,

    VAM_BUSY,

    VAM_STR_OVERSIZE
};

#endif  // #define _VAMERROR_H_
