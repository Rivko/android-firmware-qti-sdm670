/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.clevercapture.data;

import android.content.Context;

import com.qualcomm.qti.clevercaptureui.R;

// TODO: clean up errors, some must no longer be required?

/**
 * This enum defines the application Errors
 */
public enum AppError {
    NONE(false, 0),
    LIBRARY_LOADING_FAILED(true, R.string.err_library_loading),
    CAMERA_FAILED(true, R.string.err_camera_init),
    SENSORS_FAILED(true, R.string.err_sensors_init),
    CLEVERCAPTURE_API_FAILED(true, R.string.err_cc_init),

    SCVE_CLEVERCAPTURE_ERROR_SESSION_NOT_INITED(true, R.string.err_session_init),
    SCVE_CLEVERCAPTURE_ERROR_QUEUE_FULL(true, R.string.err_queue_full);

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
            return AppError.SCVE_CLEVERCAPTURE_ERROR_SESSION_NOT_INITED.isFatal;

        case 0xF0010002:
            return AppError.SCVE_CLEVERCAPTURE_ERROR_QUEUE_FULL.isFatal;
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

            default:
                return null;
        }
    }
}
