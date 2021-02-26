/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.sdk;

/**
 * The listener interface for receiving ${e.g(1).fl()} events.
 * The class that is interested in processing a ${e.g(1).fl()}
 * event implements this interface, and the object created
 * with that class is registered with a component using the
 * component's <code>version<code> method. When
 * the OnVersion event occurs, that object's appropriate
 * method is invoked.
 *
 */
public interface OnVersionListener {


    public static final String VERSION_MAJOR = "VERSION_MAJOR";
    public static final String VERSION_MINOR = "VERSION_MINOR";

    public void onVersionInfo(int major, int minor);
}
