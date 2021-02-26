/* Copyright (c) 2016 Qualcomm Technologies, Inc.
   All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.ims.vt;

import android.content.Context;
import android.media.MediaActionSound;
import org.codeaurora.ims.ImsCallUtils;
import com.qualcomm.ims.utils.Log;
import com.qualcomm.ims.utils.Config;

/**
 * Helper/wrapper class for playing sounds on various camera events, e.g. start/stop recording.
 *
 * @See MediaActionSound
 */
public class CameraActionSoundHelper {
    MediaActionSound mActionSound;
    Context mContext;
    /**
     * Initializes <code> MediaActionSound</code> based on the R.config.config_enable_camera_sound
     * config. If the flag is not set calls to the member functions results in no-op.
     */
    CameraActionSoundHelper(Context context) {
        if (context == null) {
            Log.v(this, "Ctor: Context is null");
            return;
        }
        mContext = context;
    }

    /**
     * Loads media resources.
     *
     * @See MediaActionSound#load
     */
    public void open() {
        if (!Config.isConfigEnabled(mContext,
            org.codeaurora.ims.R.bool.config_enable_camera_sound)) {
            return;
        }

        if (mActionSound == null) {
            mActionSound = new MediaActionSound();
        }

        Log.v(this, "open: Loading media files.");
        mActionSound.load(MediaActionSound.START_VIDEO_RECORDING);
        mActionSound.load(MediaActionSound.STOP_VIDEO_RECORDING);
    }

    /**
     * Releases media resources.
     *
     * @See MediaActionSound#release
     */
    public void close() {
        if (mActionSound == null) {
            return;
        }

        Log.v(this, "close: Releasing resources");
        mActionSound.release();
        mActionSound = null;
    }

    /**
     * Plays video recording started sound.
     *
     * @See MediaActionSound#START_VIDEO_RECORDING
     */
    public void onRecordingStarted() {
        if (mActionSound == null) {
            return;
        }

        Log.v(this, "onRecordingStarted:");
        mActionSound.play(MediaActionSound.START_VIDEO_RECORDING);
    }

    /**
     * Plays video recording stopped sound.
     *
     * @See MediaActionSound#STOP_VIDEO_RECORDING
     */
    public void onRecordingStopped() {
        if (mActionSound == null) {
            return;
        }

        Log.v(this, "onRecordingStopped:");
        mActionSound.play(MediaActionSound.STOP_VIDEO_RECORDING);
    }
}
