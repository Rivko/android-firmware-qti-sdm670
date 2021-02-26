/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.VoicePrintDemoApp;

import android.app.Fragment;

public class BaseFragment extends Fragment {

protected static final String TAG = "VoicePrintDemo";

protected VoicePrintDemoApp getApp() {
return (VoicePrintDemoApp) this.getActivity().getApplication();
}
}
