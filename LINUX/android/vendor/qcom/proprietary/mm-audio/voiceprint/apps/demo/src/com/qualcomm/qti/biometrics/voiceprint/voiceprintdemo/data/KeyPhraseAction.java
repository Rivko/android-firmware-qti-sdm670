/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data;

import android.content.pm.ResolveInfo;

/*
 * The Class KeyPhraseAction.
 */
public class KeyPhraseAction {

    // These must be unique.
    // Make sure to not change these as the database stores the ids.
    public static final int NO_ACTION = -1;
    public static final int EMAIL = 0;
    public static final int BROWSER = 1;
    public static final int SMS = 2;

    private int mAction = NO_ACTION;
    private String mActionName = null;
    private ResolveInfo mResolveInfo = null;

    public KeyPhraseAction(){
        this(NO_ACTION);
    }

    public KeyPhraseAction(int action) {
        mActionName = action == EMAIL ? "EMAIL" :
                action == BROWSER ? "BROWSER" :
                        action == SMS ? "SMS" :
                                "NO_ACTION";
    }

    public static KeyPhraseAction getDefault() {
        return new KeyPhraseAction(NO_ACTION);
    }

    public KeyPhraseAction(int action, String name, ResolveInfo info) {
        mAction = action;
        mActionName = name;
        mResolveInfo = info;
    }

    public String getActionName() {
        return mActionName == null ? "NO_ACTION" : mActionName;
    }

    public ResolveInfo getResolveInfo() {
        return mResolveInfo;
    }

    @Override
    public String toString() {
        return mActionName;
    }

    public int getId() {
        return mAction;
    }
}
