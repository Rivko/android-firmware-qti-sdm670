/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.permission.bean;

public class AppReceiverInfo {
    public String packageName;
    public String componentName;
    public boolean defaultEnabled;
    public int state;
    public boolean hasBoot;

    public AppReceiverInfo() {
    }

    public AppReceiverInfo(String packageName, String componentName, boolean defaultEnabled, int state,
            boolean hasBoot) {
        this.packageName = packageName;
        this.componentName = componentName;
        this.defaultEnabled = defaultEnabled;
        this.state = state;
        this.hasBoot = hasBoot;
    }

    public void setBoot(boolean hasboot) {
        this.hasBoot = hasboot;
    }
}
