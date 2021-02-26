/*
 * osal_pack_start.h
 *
 * This file defines attributes to begin packing of structure across
 * different platforms.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#if defined(_ANDROID_)
    /* Nothing for Android */
#elif defined(_QCET)
    /* Nothing for QCET */
#elif defined(_WIN_OS)
    /* Nothing for Windows mobile */
#elif defined(LINUX_ENABLED)
    /* Nothing for LEUM */
#elif defined(_OPEN_WRT_)
    /* Nothing for _OPEN_WRT_ */
#else
  #error Unsupported compiler.
#endif /* _ANDROID_ */
