/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.tracker;

/**
 * This exception indicates that the tracker failed to register an object
 */
/*package*/ class ObjectRegistrationFailedException extends ObjectTrackerException {
    private static final long serialVersionUID = -8044409747944290075L;

    public ObjectRegistrationFailedException() {
    }

    public ObjectRegistrationFailedException(String detailMessage) {
        super(detailMessage);
    }

    public ObjectRegistrationFailedException(Throwable throwable) {
        super(throwable);
    }

    public ObjectRegistrationFailedException(String detailMessage,
            Throwable throwable) {
        super(detailMessage, throwable);
    }
}
