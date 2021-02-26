/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

import java.util.ArrayList;

import android.util.Log;

public class SoundModel implements Comparable<SoundModel> {
    private final static String TAG = "ListenLog.SoundModel";
    private String name;
    private String prettyName;
    private ArrayList<Keyphrase> keyphrases = new ArrayList<Keyphrase>();
    // Com only.
    private boolean isTrained = false;

    SoundModel(String inName) {
        name = inName;
        prettyName = Global.getInstance().getSmRepo().getSmPrettyName(inName);
    }

    public String getName() {
        return name;
    }

    public String getPrettyName() {
        return prettyName;
    }

    public ArrayList<Keyphrase> getKeyphrases() {
        return keyphrases;
    }

    public ArrayList<User> getUsers() {
        ArrayList<User> users = new ArrayList<User>();
        for (Keyphrase keyphrase : keyphrases) {
            users.addAll(keyphrase.getUsers());
        }
        return users;
    }

    public void addKeyphrase(Keyphrase inKeyphrase) {
        keyphrases.add(inKeyphrase);
    }

    public boolean userExistsForKeyword(String inKeywordName, String inUserName) {
        for (Keyphrase keyword : this.keyphrases) {
            if (keyword.getName().equals(inKeywordName)) {
                for (User user : keyword.getUsers()) {
                    if (user.getName().equals(inUserName)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    public boolean getLaunch(String keywordNameToSet, String userNameToSet) {
        for (Keyphrase keyword : keyphrases) {
            if (keyword.getName().equals(keywordNameToSet)) {
                if (null == userNameToSet) {
                    Log.v(TAG, "Launch for keyword: " + keyword.name + "=" + keyword.getLaunch());
                    return keyword.getLaunch();
                } else {
                    for (User user : keyword.getUsers()) {
                        if (user.getName().equals(userNameToSet)) {
                            Log.v(TAG, "Launch for keyword: " + keyword.name +
                                    " user: " + user.getName() + "=" + user.getLaunch());
                            return user.getLaunch();
                        }
                    }
                }
            }
        }
        Log.e(TAG, "Confidence level could not be set because keyword or user could not be found");
        return false;
    }

    public boolean toggleLaunch(String keywordNameToSet, String userNameToSet) {
        for (Keyphrase keyword : keyphrases) {
            if (keyword.getName().equals(keywordNameToSet)) {
                if (null == userNameToSet) {
                    Log.v(TAG, "Launch set for keyword: " + keyword.name);
                    keyword.toggleLaunch();
                    return true;
                } else {
                    for (User user : keyword.getUsers()) {
                        if (user.getName().equals(userNameToSet)) {
                            Log.v(TAG, "Launch set for keyword: " + keyword.name +
                                    " user: " + user.getName());
                            user.toggleLaunch();
                            return true;
                        }
                    }
                }
            }
        }
        Log.e(TAG, "Confidence level could not be set because keyword or user could not be found");
        return false;
    }

    public int getConfidenceLevel(String inKeywordName, String inUserName) {
        if (null == inKeywordName) {
            Log.e(TAG, "Confidence level could not be set because keyword was null");
            return -1;
        }
        for (Keyphrase keyword : keyphrases) {
            if (keyword.getName().equals(inKeywordName)) {
                if (null == inUserName) {
                    Log.v(TAG, "Confidence level for keyword: " + keyword.name + "=" +
                            keyword.getConfidenceLevel());
                    return keyword.getConfidenceLevel();
                } else {
                    for (User user : keyword.getUsers()) {
                        if (user.getName().equals(inUserName)) {
                            Log.v(TAG, "Confidence level for keyword: " + keyword.name +
                                    " user: " + user.getName() + "=" + user.getConfidenceLevel());
                            return user.getConfidenceLevel();
                        }
                    }
                }
            }
        }
        Log.e(TAG, "Confidence level could not be set because keyword or user could not be found");
        return -1;
    }

    public boolean setConfidenceLevel(String keywordNameToSet, String userNameToSet,
                                      int inConfidenceLevel) {
        for (Keyphrase keyword : keyphrases) {
            if (keyword.getName().equals(keywordNameToSet)) {
                if (null == userNameToSet) {
                    Log.v(TAG, "Launch set for keyword: " + keyword.name);
                    keyword.setConfidenceLevel(inConfidenceLevel);
                    return true;
                } else {
                    for (User user : keyword.getUsers()) {
                        if (user.getName().equals(userNameToSet)) {
                            Log.v(TAG, "Launch set for keyword: " + keyword.name +
                                    " user: " + user.getName());
                            user.setConfidenceLevel(inConfidenceLevel);
                            return true;
                        }
                    }
                }
            }
        }
        Log.e(TAG, "Confidence level could not be set because keyword or user could not be found");
        return false;
    }

    public static enum Setting {
        Session,
        ConfidenceThreshold,
        LaunchPreference
    }

    public boolean getIsTrained() {
        return isTrained;
    }

    public void setIsTrained() {
        Log.v(TAG, "setIsTrained called for sm= " + getName());
        this.isTrained = true;
    }

    @Override
    public boolean equals(Object object) {
        boolean isSameObject = false;
        if (null != object && object instanceof SoundModel) {
            isSameObject = (this.name.equals(((SoundModel) object).getName()));
        }
        return isSameObject;
    }

    @Override
    public int compareTo(SoundModel another) {
        return this.name.compareToIgnoreCase(another.name);
    }
}