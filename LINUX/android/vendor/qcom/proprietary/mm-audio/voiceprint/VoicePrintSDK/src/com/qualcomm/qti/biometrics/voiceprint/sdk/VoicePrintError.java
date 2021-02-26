/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.sdk;

import java.util.HashMap;
import java.util.Map;

public class VoicePrintError {

    // These should match VoicePrintService returns.

    public final static int NO_ERROR = 0;

    /** The Constant SUCCESS. */
    public final static int SUCCESS = 0;

    /** The Constant FAILURE. */
    public final static int FAILURE = 1;

    /** The Constant TIMEOUT. */
    public final static int TIMEOUT = 2;

    /** The Constant ERROR. */
    public final static int ERROR = 3;

    /** The Constant RESOURCE_UNAVAILABLE. */
    public final static int RESOURCE_UNAVAILABLE = 4;

    /** The Constant PERMISSION_DENIED. */
    public static final int PERMISSION_DENIED = 5;

    public static final int FEATURE_NOT_AVAILABLE = 6;

    public static final int INVALID_PARAMETERS = 7;


    private static Map<Integer, String> ErrorMap = new HashMap<Integer, String>();

    static {
        ErrorMap.put(RESOURCE_UNAVAILABLE, "RESOURCE_UNAVAILABLE");
        ErrorMap.put(FEATURE_NOT_AVAILABLE, "FEATURE_NOT_AVAILABLE");
        ErrorMap.put(PERMISSION_DENIED, "PERMISSION_DENIED");
        ErrorMap.put(TIMEOUT, "TIMEOUT");
        ErrorMap.put(INVALID_PARAMETERS, "INVALID_PARAMETERS");
    };

    public static String getErrorString(int code) {
        String errorString = ErrorMap.get(code);
        if ( errorString == null ) {
            errorString = "UNKNOWN";
        }

        return errorString;
    }

}