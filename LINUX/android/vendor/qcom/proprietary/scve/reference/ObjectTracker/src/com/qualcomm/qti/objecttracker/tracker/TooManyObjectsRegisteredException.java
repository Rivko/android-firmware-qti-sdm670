/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.tracker;

/**
 * This exception indicates that the tracker library indicated that a new object could not be
 * registered for tracking, as the max number of tracked objects has been reached.
 */
/*package*/ class TooManyObjectsRegisteredException extends ObjectTrackerException {

    private static final long serialVersionUID = -8044409747944290075L;

    public TooManyObjectsRegisteredException() {
    }

    public TooManyObjectsRegisteredException(String detailMessage) {
        super(detailMessage);
    }

    public TooManyObjectsRegisteredException(Throwable throwable) {
        super(throwable);
    }

    public TooManyObjectsRegisteredException(String detailMessage,
            Throwable throwable) {
        super(detailMessage, throwable);
    }
}
