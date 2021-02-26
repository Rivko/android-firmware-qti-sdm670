/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.server.wigig;

import android.content.ContentResolver;
import android.content.Context;
import android.provider.Settings;
import android.util.Log;

/* Tracks persisted settings for Wigig and airplane mode interaction */
final class WigigSettingsStore {
    public static final String TAG = "WigigSettingsStore";

    /* settings entry for Wigig state */
    private static final String WIGIG_ON = "wigig_on";
    private static final String WIGIG_SAVED_STATE = "wigig_saved_state";

    /* Values stored in WIGIG_ON */
    private static final int WIGIG_DISABLED                      = 0;
    private static final int WIGIG_ENABLED                       = 1;
    /* Wigig enabled while in airplane mode */
    private static final int WIGIG_ENABLED_AIRPLANE_OVERRIDE     = 2;
    /* Wigig disabled due to airplane mode on */
    private static final int WIGIG_DISABLED_AIRPLANE_ON          = 3;

    /* Persisted state that tracks the wigig & airplane interaction from settings */
    private int mPersistWigigState = WIGIG_DISABLED;
    /* Tracks current airplane mode state */
    private boolean mAirplaneModeOn = false;

    private final Context mContext;

    /* Tracks if we have checked the saved wigig state after boot */
    private boolean mCheckSavedStateAtBoot = false;

    WigigSettingsStore(Context context) {
        mContext = context;
        mAirplaneModeOn = getPersistedAirplaneModeOn();
        mPersistWigigState = getPersistedWigigState();
    }

    /**
     * Returns true if wigig is currently on.
     * The decision is based, among other things, on current airplane mode.
     *
     * @return {@code true} if wigig is on.
     */
    synchronized boolean getWigigOn() {
        if (!mCheckSavedStateAtBoot) {
            mCheckSavedStateAtBoot = true;
            if (testAndClearWigigSavedState()) return true;
        }

        if (mAirplaneModeOn) {
            return mPersistWigigState == WIGIG_ENABLED_AIRPLANE_OVERRIDE;
        } else {
            return mPersistWigigState != WIGIG_DISABLED;
        }
    }

    /**
     * Returns true if airplane mode is currently on.
     *
     * @return {@code true} if airplane mode is on.
     */
    synchronized boolean isAirplaneModeOn() {
       return mAirplaneModeOn;
    }

    /**
     * Update the persisted Wigig state upon wigig enable/disable.
     * The new wigig state depends on current airplane mode.
     *
     * Returns false if wigig state didn't change or true if wigig state might have changed
     */
    synchronized boolean handleWigigToggled(boolean wigigEnabled) {
        Log.i(TAG, "handleWigigToggled(" + wigigEnabled + ")");

        // Can Wi-Fi be toggled in airplane mode ?
        if (mAirplaneModeOn && !isAirplaneToggleable()) {
            Log.d(TAG, "wigig state cannot be toggled");
            return false;
        }

        if (wigigEnabled) {
            if (mAirplaneModeOn) {
                persistWigigState(WIGIG_ENABLED_AIRPLANE_OVERRIDE);
            } else {
                persistWigigState(WIGIG_ENABLED);
            }
        } else {
            // When wigig state is disabled, we do not care
            // if airplane mode is on or not. The scenario of
            // wifi being disabled due to airplane mode being turned on
            // is handled in handleAirplaneModeToggled()
            persistWigigState(WIGIG_DISABLED);
        }

        Log.i(TAG, "airplane mode " + mAirplaneModeOn
                + ", new wigig state=" + mPersistWigigState);
        return true;
    }

    /**
     * Update the persisted Wigig state according to current airplane mode.
     * Airplane mode is retrieved from system (Settings.Global.AIRPLANE_MODE_ON).
     *
     * Returns false if wigig state didn't change or true if wigig state might have changed
     */
    synchronized boolean handleAirplaneModeToggled() {
        Log.i(TAG, "handleAirplaneModeToggled");
        // Is Wi-Fi (re-use for wigig) sensitive to airplane mode changes ?
        if (!isAirplaneSensitive()) {
            Log.d(TAG, "wigig is not sensitive for airplane mode change");
            return false;
        }

        mAirplaneModeOn = getPersistedAirplaneModeOn();
        if (mAirplaneModeOn) {
            // Wigig is being disabled due to airplane on
            if (mPersistWigigState == WIGIG_ENABLED) {
                persistWigigState(WIGIG_DISABLED_AIRPLANE_ON);
            }
        } else {
            /* On airplane mode disable, restore wigig state if necessary */
            if (testAndClearWigigSavedState() ||
                    mPersistWigigState == WIGIG_ENABLED_AIRPLANE_OVERRIDE) {
                persistWigigState(WIGIG_ENABLED);
            }
        }

        Log.i(TAG, "new airplane mode " + mAirplaneModeOn
                + ", new wigig state=" + mPersistWigigState);
        return true;
    }

    private void persistWigigState(int state) {
        final ContentResolver cr = mContext.getContentResolver();
        mPersistWigigState = state;
        Settings.Global.putInt(cr, WIGIG_ON, state);
    }

    /* Does Wi-Fi (re-use for wigig) need to be disabled when airplane mode is on ? */
    private boolean isAirplaneSensitive() {
        String airplaneModeRadios = Settings.Global.getString(mContext.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_RADIOS);
        return airplaneModeRadios == null
                || airplaneModeRadios.contains(Settings.Global.RADIO_WIFI);
    }

    /* Is Wi-Fi (re-use for wigig) allowed to be re-enabled while airplane mode is on ? */
    private boolean isAirplaneToggleable() {
        String toggleableRadios = Settings.Global.getString(mContext.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_TOGGLEABLE_RADIOS);
        return toggleableRadios != null
                && toggleableRadios.contains(Settings.Global.RADIO_WIFI);
    }

     /* After a reboot, we restore wigig to be on if it was turned off temporarily for tethering.
      * The settings app tracks the saved state, but the framework has to check it at boot to
      * make sure the wigig is turned on in case it was turned off for the purpose of tethering.
      *
      * Note that this is not part of the regular WIGIG_ON setting because this only needs to
      * be controlled through the settings app and not the Wigig public API.
      */
    private boolean testAndClearWigigSavedState() {
        final ContentResolver cr = mContext.getContentResolver();
        int wigigSavedState = 0;
        try {
            wigigSavedState = Settings.Global.getInt(cr, WIGIG_SAVED_STATE);
            if(wigigSavedState == 1)
                Settings.Global.putInt(cr, WIGIG_SAVED_STATE, 0);
        } catch (Settings.SettingNotFoundException e) {
            ;
        }
        return (wigigSavedState == 1);
    }

    private int getPersistedWigigState() {
        final ContentResolver cr = mContext.getContentResolver();
        try {
            return Settings.Global.getInt(cr, WIGIG_ON);
        } catch (Settings.SettingNotFoundException e) {
            Settings.Global.putInt(cr, WIGIG_ON, WIGIG_DISABLED);
            return WIGIG_DISABLED;
        }
    }

    private boolean getPersistedAirplaneModeOn() {
        return Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0) == 1;
    }
}
