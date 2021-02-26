/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.wrapper;

public class Settings_Wrapper {

    /**
     * Whether silent mode should allow vibration feedback. This is used
     * internally in AudioService and the Sound settings activity to
     * coordinate decoupling of vibrate and silent modes. This setting
     * will likely be removed in a future release with support for
     * audio/vibe feedback profiles.
     *
     * Not used anymore. On devices with vibrator, the user explicitly selects
     * silent or vibrate mode.
     * Kept for use by legacy database upgrade code in DatabaseHelper.
     *
     * Should Sync with Settings.System.VIBRATE_IN_SILENT
     */
    public static final String VIBRATE_IN_SILENT = "vibrate_in_silent";
}
