/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.fr;

/**
 * A base {@link Exception} to be thrown by {@link FaceRecognizer}
 */
public class FaceRecognizerException extends RuntimeException {
    /**
     * Construct a new {@link FaceRecognizerException}
     */
    public FaceRecognizerException() {
        super();
    }

    /**
     * Construct a new {@link FaceRecognizerException}
     */
    public FaceRecognizerException(String detailMessage) {
        super(detailMessage);
    }

    /**
     * Construct a new {@link FaceRecognizerException}
     */
    public FaceRecognizerException(String detailMessage, Throwable throwable) {
        super(detailMessage, throwable);
    }

    /**
     * Construct a new {@link FaceRecognizerException}
     */
    public FaceRecognizerException(Throwable throwable) {
        super(throwable);
    }
}
