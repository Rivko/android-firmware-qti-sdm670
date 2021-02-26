/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

import java.util.ArrayList;

import android.content.Intent;

public class Keyphrase implements Comparable<Keyphrase> {
    String name;
    int id;
    private boolean isUdk = false;
    int confidenceLevel = Global.getInstance().getSettingKeyPhraseConfidenceLevel();
    boolean launchGoogleNow = false;
    private ArrayList<User> users = new ArrayList<User>();
    private String actionName;
    private Intent actionIntent;
    private boolean isUserVerificationEnabled;


    Keyphrase(String inName, int inId) {
        this.name = inName;
        this.id = inId;
    }

    // COM Keyphrase constructor
    Keyphrase(String inName, String inActionName, Intent inActionIntent, boolean inIsUserVerificationEnabled) {
        this.name = inName;
        this.actionName = inActionName;
        this.actionIntent = inActionIntent;
        this.isUserVerificationEnabled = inIsUserVerificationEnabled;
        this.id = 0;
    }

    public Keyphrase(String inName, int inConfidenceLevel, boolean inLaunchGoogleNow) {
        name = inName;
        confidenceLevel = inConfidenceLevel;
        launchGoogleNow = inLaunchGoogleNow;
    }

    public String getName() {
        return this.name;
    }

    public boolean getIsUdk() {
        return this.isUdk;
    }

    public void setIsUdk(boolean inIsUdk) {
        this.isUdk = inIsUdk;
    }

    public void setConfidenceLevel(int inConfidenceLevel) {
        this.confidenceLevel = inConfidenceLevel;
    }

    public void toggleLaunch() {
        this.launchGoogleNow = !this.launchGoogleNow;
    }

    public int getConfidenceLevel() {
        return confidenceLevel;
    }

    public boolean getLaunch() {
        return launchGoogleNow;
    }

    public String getActionName() {
        return actionName;
    }

    public void setActionName(String actionName) {
        this.actionName = actionName;
    }

    public Intent getActionIntent() {
        return actionIntent;
    }

    public void setActionIntent(Intent inActionIntent) {
        this.actionIntent = inActionIntent;
    }

    // COM functions
    public ArrayList<User> getUsers() {
        return users;
    }

    public void addUser(String inName, int inId) {
        users.add(new User(inName, inId));
    }

    public boolean getIsUserVerificationEnabled() {
        return isUserVerificationEnabled;
    }

    public void setIsUserVerificationEnabled(boolean inIsEnabled) {
        this.isUserVerificationEnabled = inIsEnabled;
    }

    @Override
    public int compareTo(Keyphrase another) {
        return this.name.compareToIgnoreCase(another.name);
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((name == null) ? 0 : name.hashCode());
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        Keyphrase other = (Keyphrase) obj;
        if (name == null) {
            if (other.name != null)
                return false;
        } else if (!name.equals(other.name))
            return false;
        return true;
    }
}
