/*
 * Copyright (c) 2011-2012,2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.android.profile;

import android.text.format.Time;

public class ProfileConst {

    static boolean LOG = false;
    static final int NEW_PROFILE = 0;
    static final int EDIT_PROFILE = 1;
    static Time time = new Time();

    static final boolean ENABLE_CONNECTIVITY = true;
    static final boolean DISABLE_CONNECTIVITY = false;
    static final float DEFAULT_VOLUME_RATIO = 0.7f;
    static final int DEFAULT_BRIGHTNESS = (int) (0.2 * 255);
    static final int DEFAULT_VOLUME = 5;

    static final String KEY_PACKAGE = "intent_package";
    static final String KEY_CLASS = "intent_target_class";
    static final String TARGET_PACKAGE = "com.android.profile";
    static final String TARGET_CLASS = "com.android.profile.ProfileRingtoneSetting";
    static final String KEY_SUBSCRIPTION_ID = "intent_subscription_id";

    static final String PROFILE_GENERAL = "general";
    static final String PROFILE_MEETING = "meeting";
    static final String PROFILE_OUTING = "outing";
    static final String PROFILE_SILENT = "silent";
    static final String PROFILE_POWERSAVE = "powersave";
    // Constant for new profile mode
    static final String PROFILE_NEW = "newprofile";

    static final boolean ENABLE_TAB_RINGTONE_SETTING = false;
    static final String KEY_PROFILE_ID = "ProfileId";

    static final String AUDIO_PROLILE_PROFILEMANAGER_ACTION =
        "com.android.audioprofile.profilemanager";
    static final String CURRENT_PROFILE_ID = "qti_current_profile_id";
    static final String AUDIO_PROLILE_MODE = "audioprofile_mode";
    static final String VOLUME_CHANGED_ACTION = "android.media.VOLUME_CHANGED_ACTION";
    static final String RINGER_MODE_CHANGED = "android.media.RINGER_MODE_CHANGED";
    static boolean MODIFY_IN_THIS_APP = false;

    static final int RINGER_MODE_INVALID = -1;
    static final int RINGER_MODE_GENERAL = 1;
    static final int RINGER_MODE_MEETING = 2;
    static final int RINGER_MODE_OUTDOOR = 3;
    static final int RINGER_MODE_SILENT = 4;
}
