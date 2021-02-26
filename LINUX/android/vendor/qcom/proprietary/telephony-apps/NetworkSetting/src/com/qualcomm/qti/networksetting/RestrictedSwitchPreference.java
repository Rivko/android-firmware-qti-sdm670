/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2016 The Android Open Source Project
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

package com.qualcomm.qti.networksetting;

import android.content.Context;
import android.os.UserHandle;
import android.os.UserManager;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.util.AttributeSet;
import android.view.View;
import android.widget.TextView;
import com.qualcomm.qti.networksetting.RestrictedLockUtils.EnforcedAdmin;

public class RestrictedSwitchPreference extends SwitchPreference {
    private final Context mContext;
    private boolean mDisabledByAdmin;
    private final int mSwitchWidgetResId;

    public RestrictedSwitchPreference(Context context, AttributeSet attrs, int defStyleAttr,
            int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mSwitchWidgetResId = getWidgetLayoutResource();
        mContext = context;
    }

    public RestrictedSwitchPreference(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public RestrictedSwitchPreference(Context context, AttributeSet attrs) {
        this(context, attrs, com.android.internal.R.attr.switchPreferenceStyle);
    }

    public RestrictedSwitchPreference(Context context) {
        this(context, null);
    }

    @Override
    public void onBindView(View view) {
        super.onBindView(view);
        if (mDisabledByAdmin) {
            view.setEnabled(true);
        }
        final TextView summaryView = (TextView) view.findViewById(android.R.id.summary);
        if (summaryView != null && mDisabledByAdmin) {
            summaryView.setText(
                    isChecked() ? R.string.enabled_by_admin : R.string.disabled_by_admin);
            summaryView.setVisibility(View.VISIBLE);
        }
    }

    public void checkRestrictionAndSetDisabled(String userRestriction) {
        UserManager um = UserManager.get(mContext);
        UserHandle user = UserHandle.of(um.getUserHandle());
        boolean disabledByAdmin = um.hasUserRestriction(userRestriction, user)
                && !um.hasBaseUserRestriction(userRestriction, user);
        setDisabledByAdmin(disabledByAdmin);
    }

    @Override
    public void setEnabled(boolean enabled) {
        if (enabled && mDisabledByAdmin) {
            setDisabledByAdmin(false);
        } else {
            super.setEnabled(enabled);
        }
    }

    public void setDisabledByAdmin(boolean disabled) {
        if (mDisabledByAdmin != disabled) {
            mDisabledByAdmin = disabled;
            setWidgetLayoutResource(disabled ? R.layout.restricted_icon : mSwitchWidgetResId);
            setEnabled(!disabled);
        }
    }

    @Override
    public void performClick(PreferenceScreen preferenceScreen) {
        if (mDisabledByAdmin) {
            RestrictedLockUtils.sendShowAdminSupportDetailsIntent(mContext,
                    new EnforcedAdmin());
        } else {
            super.performClick(preferenceScreen);
        }
    }
}
