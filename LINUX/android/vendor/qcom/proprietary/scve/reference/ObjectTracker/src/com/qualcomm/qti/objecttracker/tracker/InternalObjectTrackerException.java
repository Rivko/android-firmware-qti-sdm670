/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.tracker;

/**
 * This exception is used for all unclassified internal object tracker errors
 */
/*package*/ class InternalObjectTrackerException extends ObjectTrackerException {
    private static final long serialVersionUID = -8044409747944290075L;

    public InternalObjectTrackerException() {
    }

    public InternalObjectTrackerException(String detailMessage) {
        super(detailMessage);
    }

    public InternalObjectTrackerException(Throwable throwable) {
        super(throwable);
    }

    public InternalObjectTrackerException(String detailMessage,
            Throwable throwable) {
        super(detailMessage, throwable);
    }
}
