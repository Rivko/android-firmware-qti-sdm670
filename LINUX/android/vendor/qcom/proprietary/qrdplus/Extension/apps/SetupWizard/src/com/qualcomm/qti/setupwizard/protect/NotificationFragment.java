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
 * Copyright (C) 2014 The Android Open Source Project
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
 * limitations under the License
 */

package com.qualcomm.qti.setupwizard.protect;

import android.app.admin.DevicePolicyManager;
import android.content.Context;
import android.os.Bundle;
import android.provider.Settings;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import com.qualcomm.qti.setupwizard.R;

public class NotificationFragment extends Fragment implements RadioGroup.OnCheckedChangeListener {

    private RadioGroup mRadioGroup;
    private RadioButton mShowAllButton;
    private RadioButton mRedactSensitiveButton;
    private View mView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        mView = inflater.inflate(R.layout.fragment_notification, container, false);
        initViews(mView);
        return mView;
    }

    private void initViews(View view) {
        mRadioGroup = (RadioGroup) view.findViewById(R.id.lock_screen_notifications_group);
        mShowAllButton = (RadioButton) view.findViewById(R.id.lock_screen_notifications_show);
        mRedactSensitiveButton =
                (RadioButton) view.findViewById(R.id.lock_screen_notifications_hide);
        mRadioGroup.setOnCheckedChangeListener(this);
        // Disable buttons according to policy.
        if (isSecureNotificationsDisabled(getActivity())) {
            mShowAllButton.setEnabled(false);
            mRedactSensitiveButton.setEnabled(false);
        } else if (isUnredactedNotificationsDisabled(getActivity())) {
            mShowAllButton.setEnabled(false);
        }
    }

    private static boolean isSecureNotificationsDisabled(Context context) {
        final DevicePolicyManager dpm =
                (DevicePolicyManager) context.getSystemService(Context.DEVICE_POLICY_SERVICE);
        return dpm != null && (dpm.getKeyguardDisabledFeatures(null)
                & DevicePolicyManager.KEYGUARD_DISABLE_SECURE_NOTIFICATIONS) != 0;
    }

    private static boolean isUnredactedNotificationsDisabled(Context context) {
        final DevicePolicyManager dpm =
                (DevicePolicyManager) context.getSystemService(Context.DEVICE_POLICY_SERVICE);
        return dpm != null && (dpm.getKeyguardDisabledFeatures(null)
                & DevicePolicyManager.KEYGUARD_DISABLE_UNREDACTED_NOTIFICATIONS) != 0;
    }

    @Override
    public void onResume() {
        super.onResume();
        loadFromSettings();
    }

    private void loadFromSettings() {
        final boolean enabled = Settings.Secure.getInt(getActivity().getContentResolver(),
                Settings.Secure.LOCK_SCREEN_SHOW_NOTIFICATIONS, 0) != 0;
        final boolean show = Settings.Secure.getInt(getActivity().getContentResolver(),
                Settings.Secure.LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, 1) != 0;

        int checkedButtonId = R.id.lock_screen_notifications_disable;
        if (enabled) {
            if (show && mShowAllButton.isEnabled()) {
                checkedButtonId = R.id.lock_screen_notifications_show;
            } else if (mRedactSensitiveButton.isEnabled()) {
                checkedButtonId = R.id.lock_screen_notifications_hide;
            }
        }

        mRadioGroup.check(checkedButtonId);
    }

    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {
        final boolean show = (checkedId == R.id.lock_screen_notifications_show);
        final boolean enabled = (checkedId != R.id.lock_screen_notifications_disable);

        Settings.Secure.putInt(getActivity().getContentResolver(),
                Settings.Secure.LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, show ? 1 : 0);
        Settings.Secure.putInt(getActivity().getContentResolver(),
                Settings.Secure.LOCK_SCREEN_SHOW_NOTIFICATIONS, enabled ? 1 : 0);
    }
}
