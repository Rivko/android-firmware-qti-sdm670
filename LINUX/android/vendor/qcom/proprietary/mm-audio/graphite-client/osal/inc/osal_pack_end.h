/*
 * osal_pack_end.h
 *
 * This file defines attributes to end packing of structure across
 * different platforms.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#if defined(_ANDROID_)
  __attribute__((packed))
#elif defined(_QCET)
    /* Nothing for QCET */
#elif defined(_WIN_OS)
    /* Nothing for Windows mobile */
#elif defined(LINUX_ENABLED)
    /* Nothing for LEUM */
#elif defined(_OPEN_WRT_)
  __attribute__((packed))
#else
  #error Unsupported compiler.
#endif /* _ANDROID_ */
