/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (c) 2010, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.server.wigigapp;


import android.content.Context;
import android.preference.Preference;
import android.view.View;
import android.widget.TextView;

class AccessPoint extends Preference {

    private static int[] wigig_signal_icon = {
            R.drawable.ic_wigig4,
            R.drawable.ic_wigig3,
            R.drawable.ic_wigig2,
            R.drawable.ic_wigig1
    };

    private static int[] wigig_secured_signal_icon = {
            R.drawable.ic_wigig_lock4,
            R.drawable.ic_wigig_lock3,
            R.drawable.ic_wigig_lock2,
            R.drawable.ic_wigig_lock1
    };


    /**
     * AP info will be received from a system Wigig object (e.g. WigigSettings, WigigScanResults etc.)
     * Currently we use and keep the info inside the class for testing purposes till the system
     * objects are ready.
     */
    static final int SECURITY_NONE = 0;
    static final int SECURITY_PSK = 1;

    static final int AP_SAVED_AWAY = 0;
    static final int AP_NOT_CONNECTED = 1;
    static final int AP_SAVED = 2;
    static final int AP_AUTHENTICATION_PROBLEM = 3;
    static final int AP_CONNECTING = 4;
    static final int AP_AUTHENTICATING = 5;
    static final int AP_OBTAINING_IP = 6;
    static final int AP_CONNECTED = 7;

    String ssid;
    int security;
    private int mRssi = Integer.MAX_VALUE;

    AccessPoint(Context context, String _ssid, int _rssi, int _security) {
        super(context);
        ssid = _ssid;
        mRssi = _rssi;
        security = _security;
        refresh();
    }

    @Override
    protected void onBindView(View view) {
        super.onBindView(view);
        updateIcon(getLevel());

        notifyChanged();
    }

    protected void updateIcon(int level) {
        if (level == -1 || level > 3) {
            if (security == SECURITY_NONE)
                setIcon(R.drawable.ic_wigig0);
            else
                setIcon(R.drawable.ic_wigig_lock0);
        } else {
            if(security == SECURITY_NONE)
                setIcon(wigig_signal_icon[level]);
            else
                setIcon(wigig_secured_signal_icon[level]);
        }
    }

    public int GetApStatus() {
        int ret;
        String summary;

        if (getSummary() == null) {
            return AP_NOT_CONNECTED;
        }
        summary = getSummary().toString();
        if (summary.matches(getContext().getString(R.string.wifi_saved))) {
            ret = (mRssi != 0) ? AP_SAVED : AP_SAVED_AWAY;
        }
        else if (summary.matches(getContext().getString(R.string.wifi_connecting))) {
            ret = AP_CONNECTING;
        }
        else if (summary.matches(getContext().getString(R.string.wifi_obtaining_ipaddr))) {
            ret = AP_OBTAINING_IP;
        }
        else if (summary.matches(getContext().getString(R.string.wifi_connected))) {
            ret = AP_CONNECTED;
        }
        else if (summary.matches(getContext().getString(R.string.wifi_authenticating))) {
            ret = AP_AUTHENTICATING;
        }
        else if (summary.matches(getContext().getString(R.string.wifi_authentication_problem))) {
            ret = AP_AUTHENTICATION_PROBLEM;
        }
        else {
            ret = AP_NOT_CONNECTED;
        }

        return ret;
    }

    @Override
    public int compareTo(Preference preference) {
        if (!(preference instanceof AccessPoint))
            return 1;

        AccessPoint other = (AccessPoint) preference;

        if (GetApStatus() != other.GetApStatus())
            return (other.GetApStatus() - GetApStatus());

        if (mRssi != other.mRssi)
            return (other.mRssi - mRssi);

        if (security != other.security)
            return (other.security - security);

        // Sort by ssid.
        return ssid.compareToIgnoreCase(other.ssid);
    }

    @Override
    public boolean equals(Object other) {
        if (!(other instanceof AccessPoint)) return false;
        return (this.compareTo((AccessPoint) other) == 0);
    }

    @Override
    public int hashCode() {
        int result = 0;
        result += 19 * mRssi;
        result += 23 * security;
        result += 29 * ssid.hashCode();
        return result;
    }


    /*
     Returns the signal level (0-3 or -1)
     */
    int getLevel() {
        if (mRssi == Integer.MAX_VALUE) {
            return -1;
        }
        if (mRssi == 0)
            // saved AP (not seen in scan results)
            return -1;

        // positive values => sqi 0..100
        if (mRssi >= 75)
            return 0;
        if (mRssi >= 50)
            return 1;
        if (mRssi >= 25)
            return 2;
        if (mRssi > 0)
            return 3;

        // negative values => dBm -75..-30
        if (mRssi >= -45)
            return 0;
        if (mRssi >= -55)
            return 1;
        if (mRssi >= -65)
            return 2;
        if (mRssi >= -75)
            return 3;

        return -1;
    }

    /*
     Updates the title and summary; may indirectly call notifyChanged().
     */
    private void refresh() {
        setTitle(ssid);

        final Context context = getContext();
        updateIcon(getLevel());

        // Force new summary
        setSummary(null);
    }
}
