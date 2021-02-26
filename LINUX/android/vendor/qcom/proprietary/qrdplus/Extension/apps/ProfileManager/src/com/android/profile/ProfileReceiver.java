/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.android.profile;

import static com.android.profile.ProfileConst.CURRENT_PROFILE_ID;
import static com.android.profile.ProfileConst.RINGER_MODE_CHANGED;
import static com.android.profile.ProfileConst.RINGER_MODE_GENERAL;
import static com.android.profile.ProfileConst.RINGER_MODE_INVALID;
import static com.android.profile.ProfileConst.RINGER_MODE_SILENT;
import static com.android.profile.ProfileConst.RINGER_MODE_MEETING;
import static com.android.profile.ProfileConst.RINGER_MODE_OUTDOOR;
import static com.android.profile.ProfileConst.VOLUME_CHANGED_ACTION;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class ProfileReceiver extends BroadcastReceiver {
    private static final String TAG = "ProfileReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (!context.getResources().getBoolean(R.bool.config_autoprofile_enable)) {
            return;
        }
        ProfileApplyManager profileApplyManager = new ProfileApplyManager(context);

        if (intent.getAction().equals(VOLUME_CHANGED_ACTION)
            || intent.getAction().equals(RINGER_MODE_CHANGED)) {
            if (ProfileConst.MODIFY_IN_THIS_APP) {
                ProfileConst.MODIFY_IN_THIS_APP = false;
                return;
            }
            profileApplyManager.clearDataBaseAdapter();
            if (profileApplyManager.getSelectedId() != RINGER_MODE_INVALID) {
                profileApplyManager.setSelectedId(RINGER_MODE_INVALID);
            }
            return;
        }

        int index = intent.getIntExtra(CURRENT_PROFILE_ID, RINGER_MODE_INVALID);
        switch (index) {
            case RINGER_MODE_GENERAL:
            case RINGER_MODE_MEETING:
            case RINGER_MODE_OUTDOOR:
            case RINGER_MODE_SILENT:
                profileApplyManager.applyProfile(index);
                profileApplyManager.clearDataBaseAdapter();
                profileApplyManager.setSelectedId(index);
                break;
            default:
                break;
        }
    }

}
