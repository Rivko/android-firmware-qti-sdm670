/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.tracker;

/**
 * This exception indicates that the user attempted a tracker operation on a closed object tracker
 */
/*package*/ class ObjectTrackerNotRunningException extends RuntimeException {
    private static final long serialVersionUID = 5987693746577512389L;

    public ObjectTrackerNotRunningException() {
        super();
    }

    public ObjectTrackerNotRunningException(String detailMessage) {
        super(detailMessage);
    }

    public ObjectTrackerNotRunningException(String detailMessage, Throwable throwable) {
        super(detailMessage, throwable);
    }

    public ObjectTrackerNotRunningException(Throwable throwable) {
        super(throwable);
    }
}
