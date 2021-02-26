/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercapture.hardware;

public class CleverCaptureScore {
    boolean isInteresting;
    int frameId;
    int reserved;
    @Override
    public String toString() {
        // TODO Auto-generated method stub
        return (super.toString() +
                "_[" +
                "isInteresting=" + isInteresting +
                " frameId=" + frameId +
                " reserved=" + reserved +
                "]");
    }
}
