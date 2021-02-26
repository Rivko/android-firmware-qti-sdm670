/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

import com.qualcomm.qti.sta.R;

/**
 * Enum for the application messages
 */
public enum ApplicationMessage {
    CAMERA_INITIALIZATION_FAILED(R.string.error_camera_failed, MessageType.FATAL),
    CAMERA_SNAPSHOT_FAILED(R.string.error_camera_snapshot_failed, MessageType.INFO),
    GALLERY_IMAGE_LOAD_FAILED(R.string.error_galleryImage_load, MessageType.FATAL),
    STA_API_PARSING_TIMEOUT(R.string.error_api_frame_timeout, MessageType.INFO),
    STA_API_PARSING_RESTART_FAILED(R.string.error_api_restart_failed, MessageType.INFO),
    STA_API_INIT_FAILED(R.string.error_api_init_failed, MessageType.FATAL),
    STA_API_PARSING_FAILED(R.string.error_api_parsing_failed, MessageType.TOAST_LONG),
    STA_API_NO_RECORDS(R.string.error_api_no_records, MessageType.TOAST_SHORT),
    STA_API_NO_NEW_RECORDS(R.string.error_api_no_new_records, MessageType.TOAST_SHORT),
    IC_API_INIT_FAILED(R.string.error_ic_api_init_failed, MessageType.TOAST_SHORT),
    IC_API_NO_CANDIDATE(R.string.error_ic_candidate_failed, MessageType.TOAST_SHORT),
    IC_API_RECTIFY_FAILED(R.string.error_ic_rectify_failed, MessageType.TOAST_SHORT),
    IC_API_GET_RECTIFY_FAILED(R.string.error_ic_get_rectify_failed, MessageType.TOAST_SHORT),
    IC_API_WRONG_IMAGE(R.string.error_ic_wrong_image, MessageType.TOAST_SHORT),
    IC_IMAGE_SAVED(R.string.message_ic_image_saved, MessageType.TOAST_SHORT),
    HOLD_STEADY(R.string.message_hold_steady, MessageType.TOAST_SHORT);

    /**
     * Enum for message types
     */
    public enum MessageType {
        INFO, FATAL, TOAST_SHORT, TOAST_LONG;
    }

    private final MessageType messageType;
    private final int messageId;
    private int errorValue = 0;
    private Runnable finishCallback = null;

    private ApplicationMessage(int messageId, MessageType messageType) {
        this.messageType = messageType;
        this.messageId = messageId;
    }

    /**
     * get the message type
     *
     * @return message type
     */
    public MessageType getType() {
        return messageType;
    }

    /**
     * gets the message id associated with message
     *
     * @return message resource id
     */
    public int getId() {
        return messageId;
    }

    /**
     * Sets the error value associated with error message
     *
     * @param value
     *            error valsue
     */
    public void setErrorValue(int value) {
        errorValue = value;
    }

    /**
     * gets the error id associated with error message
     *
     * @return error id
     */
    public int getErrorId() {
        return errorValue;
    }

    /**
     * Sets a callback which needs to be scheduled
     *
     * @param runnable
     *            callback which needs to be scheduled
     * @param afterMs
     *            time after which the callback needs to be executed
     */
    public void setFinishCallback(Runnable runnable) {
        finishCallback = runnable;
    }

    /**
     * get the finish callback associated with the message
     *
     * @return finish callback
     */
    public Runnable getFinishCallback() {
        return finishCallback;
    }
}
