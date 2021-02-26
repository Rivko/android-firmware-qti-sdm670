/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.Consts;
import com.qualcomm.qti.biometrics.voiceprint.sdk.VoicePrintManager;

/**
 * The class KeyPhrase.
 */
public class KeyPhrase {

    private String mId;
    private String mPhrase;
    private String mName;
    private boolean mIsSelected = false;
    private int mThreshold = VoicePrintManager.THRESHOLD_HIGH;
    private int mMatchSuccess = 0;
    private int mMatchFails = 0;
    private int mFalsePositives = 0;
    private int mActionId = -1;

    public KeyPhrase() {
    }

    public KeyPhrase(String phraseName, String phrase) {

        mName = phraseName;
        mPhrase = phrase;

        // TODO: change the way id will be created
        mId = String.format("%s_%s", mName, mPhrase);
    }

    public void setName(String name) {
        mName = name;

        // TODO: change the way id will be created
        mId = String.format("%s_%s", mName, mPhrase);
    }

    public void setPhrase(String phrase) {
        mPhrase = phrase;
    }


    public String getId() {
        return mId;
    }

    public String getPhrase() {
        return mPhrase;
    }

    public String getName() {
        return mName;
    }

    public void setSelected(boolean selected) {
        mIsSelected = selected;
    }

    public boolean isSelected() {
        return mIsSelected;
    }

    public int getFalsePositives() {
        return mFalsePositives;
    }

    public void setFalsePositives(int falsePositives) {
        mFalsePositives = falsePositives;
    }

    public int getMatchFails() {
        return mMatchFails;
    }

    public void setMatchFails(int matchFails) {
        mMatchFails = matchFails;
    }

    public int getMatchSuccess() {
        return mMatchSuccess;
    }

    public void setMatchSuccess(int matchSuccess) {
        mMatchSuccess = matchSuccess;
    }

    public int getThreshold() {
        return mThreshold;
    }

    public void setThreshold(int threshold) {
        mThreshold = threshold;
    }

    public void setActionId(int action) {
        mActionId = action;
    }

    public int getActionId() {
        return mActionId;
    }

    @Override
    public String toString() {
        return String.format("key=%s, name=%s, phrase='%s', threshold=%d, action=%d, " +
                             "matchSuccess=%d, matchFails=%d, falsePositives=%d",
                             mId, mName, mPhrase, mThreshold, mActionId, mMatchSuccess,
                             mMatchFails, mFalsePositives);
    }
}
