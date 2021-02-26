/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.tracker;

/**
 * This exception indicates that the user attempted to track or unregister an object with an
 * unrecognized ID
 */
/*package*/ class InvalidObjectIdentifierException extends ObjectTrackerException {
    private static final long serialVersionUID = -8044409747944290075L;

    public InvalidObjectIdentifierException() {
    }

    public InvalidObjectIdentifierException(String detailMessage) {
        super(detailMessage);
    }

    public InvalidObjectIdentifierException(Throwable throwable) {
        super(throwable);
    }

    public InvalidObjectIdentifierException(String detailMessage,
            Throwable throwable) {
        super(detailMessage, throwable);
    }
}
