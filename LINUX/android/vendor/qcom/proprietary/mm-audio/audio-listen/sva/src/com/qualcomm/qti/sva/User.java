/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

public class User implements Comparable<User> {
    String name;
    int id;
    int confidenceLevel = Global.getInstance().getSettingUserConfidenceLevel();
    boolean launchGoogleNow = false;

    User(String inName, int inId) {
        this.name = inName;
        this.id = inId;
    }

    User(String inName, int inConfidenceLevel, boolean inLaunchGoogleNow) {
        name = inName;
        confidenceLevel = inConfidenceLevel;
        launchGoogleNow = inLaunchGoogleNow;
    }

    public String getName() {
        return name;
    }

    public void setConfidenceLevel (int inConfidenceLevel) {
        this.confidenceLevel = inConfidenceLevel;
    }

    public int getConfidenceLevel () {
        return confidenceLevel;
    }

    public void toggleLaunch() {
        this.launchGoogleNow = !this.launchGoogleNow;
    }

    public boolean getLaunch() {
        return launchGoogleNow;
    }

    @Override
    public int compareTo(User another) {
        return this.name.compareToIgnoreCase(another.name);
    }

    @Override
    public boolean equals(Object object) {
        boolean isSameObject = false;
        if (null != object && object instanceof SoundModel) {
            isSameObject = (this.name.equals(((SoundModel) object).getName()));
        }
        return isSameObject;
    }
}
