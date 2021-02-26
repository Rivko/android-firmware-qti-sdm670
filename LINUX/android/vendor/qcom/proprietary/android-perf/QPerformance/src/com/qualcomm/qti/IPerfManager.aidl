/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti;

interface IPerfManager {
    int perfLockRelease();
    int perfLockReleaseHandler(int handle);
    int perfHint(int hint, String userDataStr, int userData1, int userData2);
    int perfLockAcquire(int duration, in int[] list);
}