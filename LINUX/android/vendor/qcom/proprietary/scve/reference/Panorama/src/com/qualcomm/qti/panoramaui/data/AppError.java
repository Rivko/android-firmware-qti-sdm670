/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.data;

import android.content.Context;

import com.qualcomm.qti.panoramaui.R;

// TODO: clean up errors, some must no longer be required?

/**
 * This enum defines the application Errors
 */
public enum AppError {
    NONE(false, 0),
    LIBRARY_LOADING_FAILED(true, R.string.err_library_loading),
    CAMERA_FAILED(true, R.string.err_camera_init),
    SENSORS_FAILED(true, R.string.err_sensors_init),
    PANORAMA_API_FAILED(true, R.string.err_pano_init),
    ZSL_FAILED(true, R.string.err_zsl_failed),

    PANORAMA_NO_PREV(false, R.string.err_no_prv),
    PANORAMA_CAPTURING_FAILED(false, R.string.err_api_capturing),
    PANORAMA_LOADING_FAILED(false, R.string.err_panorama_loading),
    PANORAMA_SAVE_FAILED(false, R.string.err_panorama_saving),
    PANORAMA_CAPTURING_HALTED(false, true, R.string.err_api_halted),
    PANORAMA_CAPTURING_MAX_LIMIT(false, true, R.string.err_max_limit),
    PANORAMA_DIRECTION_CHANGED(false, true, R.string.err_direction_changed),

    /*
     * The following describe how the error/info case should be handled internally
     * However, only 2 out of 7 status codes for panorama need break:
     * SCVE_PANORAMA_ERROR_SESSION_NOT_INITED
     * SCVE_PANORAMA_ERROR_QUEUE_FULL
     * And all other status codes are just assistant information for users and panorama result will be generated:
     * SCVE_PANORAMA_ERROR_MAXIMUM_FRAMES_REACHED
     * SCVE_PANORAMA_ERROR_FRAME_NO_OVERLAP
     * SCVE_PANORAMA_ERROR_DIRECTION_NOT_CONSITENT
     * SCVE_PANORAMA_ERROR_RESULT_NOT_READY
     * SCVE_PANORAMA_ERROR_FRAME_NO_FEATURE
     */
    SCVE_PANORAMA_ERROR_SESSION_NOT_INITED(true, R.string.err_session_init),
    SCVE_PANORAMA_ERROR_QUEUE_FULL(true, R.string.err_queue_full),
    SCVE_PANORAMA_ERROR_MAXIMUM_FRAMES_REACHED(false, R.string.err_max_key_frames),
    SCVE_PANORAMA_ERROR_FRAME_NO_OVERLAP(false, R.string.err_no_frame_overlap),
    SCVE_PANORAMA_ERROR_DIRECTION_NOT_CONSITENT(false, R.string.err_inconsistent_direction),
    SCVE_PANORAMA_ERROR_RESULT_NOT_READY(false, R.string.err_result_not_ready),
    SCVE_PANORAMA_ERROR_FRAME_NO_FEATURE(false,R.string.err_frame_no_feature);

    private final int messageId;
    private final boolean isFatal;
    private final boolean isImageRecoverable;
    private int errorCode;

    /**
     * sets error message
     *
     * @param isFatal
     *            true if error is fatal and app should quit, false otherwise
     * @param isImageRecoverable
     *            true if image can still be recovered, false otherwise
     * @param messageId
     *            message resource id to be set
     *
     */
    private AppError(boolean isFatal, boolean isImageRecoverable, int messageId) {
        this.isFatal = isFatal;
        this.isImageRecoverable = isImageRecoverable;
        this.messageId = messageId;
    }

    /**
     * sets error message
     *
     * @param isFatal
     *            true if error is fatal and app should quit, false otherwise
     * @param messageId
     *            message resource id to be set
     *
     */
    private AppError(boolean isFatal, int messageId) {
        this.isFatal = isFatal;
        this.isImageRecoverable = false;
        this.messageId = messageId;
    }

    /**
     * method to get the error string from the string resource id
     *
     * @param context
     *            application context
     * @return resource string
     */
    public String getResourceString(Context context) {
        if (messageId == 0)
            return null;

        return context.getString(messageId);
    }

    /**
     * Sets the error code
     *
     * @param errorCode
     *            error code if any
     */
    public void setErrorCode(int errorCode) {
        this.errorCode = errorCode;
    }

    /**
     * returns the error code
     *
     * @return erro code
     */
    public int getErrorCode() {
        return errorCode;
    }

    /**
     * Checks if the error is fatal or not
     *
     * @return true if error is fatal, false otherwise
     */
    public boolean isFatal() {
        return isFatal;
    }

    /**
     * Checks if the image is still recoverable
     *
     * @return true if image is recoverable, false otherwise
     */
    public boolean isImageRecoverable() {
        return isImageRecoverable;
    }

    /**
     * This function return true if capture action should be terminated
     * false otherwise
     * @return boolean
     */
    public static boolean shouldTerminateCapture(int returnedErrorCode) {
        switch (returnedErrorCode) {
        case 0xF0010001:
            return AppError.SCVE_PANORAMA_ERROR_SESSION_NOT_INITED.isFatal;

        case 0xF0010002:
            return AppError.SCVE_PANORAMA_ERROR_QUEUE_FULL.isFatal;
        }
        return false;
    }

    /**
     * gets the error information for the error code if available
     * @param context the application {@link Context}
     * @return error information
     */
    public String getErrorInfo(Context context) {
        switch (errorCode) {
            case 0xF0010001:
                return context.getString(R.string.err_session_init);

            case 0xF0010002:
                return context.getString(R.string.err_queue_full);

            case 0xF0010003:
                return context.getString(R.string.err_max_key_frames);

            case 0xF0010004:
                return context.getString(R.string.err_no_frame_overlap);

            case 0xF0010005:
                return context.getString(R.string.err_inconsistent_direction);

            case 0xF0010006:
                return context.getString(R.string.err_result_not_ready);

            case 0xF0010007:
                return context.getString(R.string.err_frame_no_feature);

            default:
                return null;
        }
    }
}
