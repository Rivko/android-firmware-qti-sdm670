/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.util;

/**
 * This interface wraps constants used to share images between various activities within the
 * clone/remove apps
 * @author slaver
 */
public interface ImageSharingIntentExtras {
    public static final String EXTRA_TARGET_IMAGE_URI = "TARGET_IMAGE_URI";
    public static final String EXTRA_OBJECT_IMAGE_URI = "OBJECT_IMAGE_URI";
    public static final String EXTRA_MASK_IMAGE_URI = "MASK_IMAGE_URI";
    public static final String BACKGROUND_IMAGE_URI = "BACKGROUND_IMAGE_URI";
    public static final String FOREGROUND_IMAGE_URI = "FOREGROUND_IMAGE_URI";
    public static final String CLONE_MATRIX = "CLONE_MATRIX";
    public static final String FULL_MP_MODE = "FULL_MP_MODE";
    public static final String NEXT_ACTIVITY = "NEXT_ACTIVITY";
}
