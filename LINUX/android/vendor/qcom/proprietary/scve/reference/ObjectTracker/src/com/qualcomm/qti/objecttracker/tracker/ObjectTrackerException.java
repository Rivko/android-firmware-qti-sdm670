/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttracker.tracker;

/**
 * This is the base class for all types of checked object tracker exceptions
 */
/*package*/ abstract class ObjectTrackerException extends Exception {
    private static final long serialVersionUID = 3027208645484561954L;

    public ObjectTrackerException() {
    }

    public ObjectTrackerException(String detailMessage) {
        super(detailMessage);
    }

    public ObjectTrackerException(Throwable throwable) {
        super(throwable);
    }

    public ObjectTrackerException(String detailMessage, Throwable throwable) {
        super(detailMessage, throwable);
    }
}
