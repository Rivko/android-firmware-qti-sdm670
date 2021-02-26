/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.tracker;

/**
 * This exception indicates that the tracker was invoked to track objects when no objects have
 * been registered
 */
/*package*/ class NoObjectsRegisteredException extends ObjectTrackerException {
    private static final long serialVersionUID = -8044409747944290075L;

    public NoObjectsRegisteredException() {
    }

    public NoObjectsRegisteredException(String detailMessage) {
        super(detailMessage);
    }

    public NoObjectsRegisteredException(Throwable throwable) {
        super(throwable);
    }

    public NoObjectsRegisteredException(String detailMessage,
            Throwable throwable) {
        super(detailMessage, throwable);
    }
}
