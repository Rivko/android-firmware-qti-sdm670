/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption;

import android.app.Activity;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceScreen;

import com.qti.csm.R;
import com.qti.csm.encryption.system.SystemInfo;

import static com.qti.csm.encryption.system.ProcessInfo.logd;

public class EncryptProcess extends PreferenceFragment {

    OnOperationClickListener mOnOperationClickListener;

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        mOnOperationClickListener = (OnOperationClickListener) activity;
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
                                         Preference preference) {
        String key = preference.getKey();
        logd(key);
        if ("encrypt".equals(key))
            mOnOperationClickListener.onOperationClick(SystemInfo.MODE_ENCRYPT);
        else if ("decrypt".equals(key))
            mOnOperationClickListener.onOperationClick(SystemInfo.MODE_DECRYPT);
        return super.onPreferenceTreeClick(preferenceScreen, preference);
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {
        logd("");
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.function);
    }

    public interface OnOperationClickListener {

        public void onOperationClick(int mode);
    }
}
