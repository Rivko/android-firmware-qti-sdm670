/*
 * Copyright (c) 2011-2012,2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.android.profile;

import static com.android.profile.ProfileConst.*;
import static com.android.profile.ProfileUtils.*;
import android.media.RingtoneManager;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.provider.Settings.System;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.qualcomm.qti.wrapper.TelephonyManager_Wrapper;

public class ProfileRingtoneSetting extends PreferenceActivity {

    private static String TAG = "ProfileRingtoneSetting";
    private int[] ringtoneType = {
            RingtoneManager.TYPE_RINGTONE, ProfileSetting.TYPE_RINGTONE_2
    };

    private String[] key = {
            "ringtone1", "ringtone2"
    };

    private ProfileRingtonePreference mRingtone;
    private int mSubscription;

    public void onCreate(Bundle savedInstanceState) {

        logd("");
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.profile_ringtone_setting);
        mRingtone = (ProfileRingtonePreference) findPreference("dialog_ringtone_select");
        mSubscription = this.getIntent().getIntExtra(KEY_SUBSCRIPTION_ID, 0);
        
        mRingtone.setSlotId(mSubscription);
        mRingtone.setRingtone(getStringValueSaved(this, key[mSubscription], null));
    }

    protected void onResume() {

        logd("");
        super.onResume();
        setState();
    }

    @Override
    protected void onPause() {

        logd("");
        super.onPause();
    }

    @Override
    protected void onDestroy() {

        logd("");
        super.onDestroy();
    }

    private void setState() {
      //APINotAvailable
//        int slotState = TelephonyManager.getSubscriptionState(mSubscription);
//        int cardState = TelephonyManager.getDefault().getSimState(mSubscription);
//        boolean airplane = (System.getInt(getContentResolver(), System.AIRPLANE_MODE_ON, 0) != 0);
//
//        getPreferenceScreen().setEnabled(
//                (slotState == TelephonyManager.SUB_ACTIVATED)
//                        && (cardState == TelephonyManager.SIM_STATE_READY) && !airplane);
      boolean airplane = (System.getInt(getContentResolver(), System.AIRPLANE_MODE_ON, 0) != 0);

      getPreferenceScreen().setEnabled(!airplane && isSubActivated());
    }

    private boolean isSubActivated() {
        return TelephonyManager.SIM_STATE_READY ==
                TelephonyManager_Wrapper.getDefaultWrapperInstance().getSimState(mSubscription);
    }

    @SuppressWarnings("unused")
    private static void loge(Object e) {

        if (!LOG)
            return;
        Thread mThread = Thread.currentThread();
        StackTraceElement[] mStackTrace = mThread.getStackTrace();
        String mMethodName = mStackTrace[3].getMethodName();
        e = "[" + mMethodName + "] " + e;
        Log.e(TAG, e + "");
    }

    private static void logd(Object s) {

        if (!LOG)
            return;
        Thread mThread = Thread.currentThread();
        StackTraceElement[] mStackTrace = mThread.getStackTrace();
        String mMethodName = mStackTrace[3].getMethodName();

        s = "[" + mMethodName + "] " + s;
        Log.d(TAG, s + "");
    }
}
