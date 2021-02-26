/*
 * Copyright (c) 2013,2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2008 The Android Open Source Project
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

package com.android.firewall;

import android.app.TabActivity;
import android.content.Intent;
import android.content.ComponentName;
import android.content.pm.PackageManager;
import android.Manifest;
import android.os.Bundle;
import android.provider.CallLog.Calls;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TabWidget;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;

import com.android.firewall.R;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class FirewallActivity extends TabActivity {

    private static final int REQUEST_CODE_ASK_MULTIPLE_PERMISSIONS = 1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!checkAllNeedPermissions()) {
            return;
        }
        createContinue();
    }

    private void createContinue() {
        TabHost tabHost = getTabHost();

        Intent settingIntent = new Intent(this, FirewallSettingPage.class);
        Intent blacklistIntent = new Intent(this, FirewallListPage.class);
        blacklistIntent.putExtra("mode", "blacklist");
        Intent whitelistIntent = new Intent(this, FirewallListPage.class);
        whitelistIntent.putExtra("mode", "whitelist");

        Intent intent = getIntent();
        String action = intent.getAction();
        Bundle bundle = intent.getExtras();
        if (action != null && action.equals(Intent.ACTION_INSERT)) {
            if (bundle.getString("mode").equals("blacklist")) {
                blacklistIntent.setAction(Intent.ACTION_INSERT);
            } else {
                whitelistIntent.setAction(Intent.ACTION_INSERT);
            }
        }

        tabHost.addTab(tabHost.newTabSpec("blacklist")
                .setIndicator(getText(R.string.firewall_blacklist_title))
                .setContent(blacklistIntent));
        tabHost.addTab(tabHost.newTabSpec("whitelist")
                .setIndicator(getText(R.string.firewall_whitelist_title))
                .setContent(whitelistIntent));
        tabHost.addTab(tabHost.newTabSpec("setting")
                .setIndicator(getText(R.string.firewall_setting))
                .setContent(settingIntent));
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);

        int screenWidth = dm.widthPixels;

        for (int i = 0; i < 3; i++) {
            tabHost.getTabWidget().getChildAt(i).getLayoutParams().width = (screenWidth / 3);

            TextView tv = (TextView) tabHost.getTabWidget().getChildAt(i)
                    .findViewById(android.R.id.title);

            tv.setTextSize(13);
            tv.setSingleLine(true);
        }

        if (action != null && action.equals(Intent.ACTION_INSERT)) {
            if (bundle.getString("mode").equals("blacklist")) {
                tabHost.setCurrentTab(1);
                tabHost.getTabWidget().getChildAt(0).setEnabled(false);
                tabHost.getTabWidget().getChildAt(2).setEnabled(false);
            } else {
                tabHost.setCurrentTab(2);
                tabHost.getTabWidget().getChildAt(0).setEnabled(false);
                tabHost.getTabWidget().getChildAt(1).setEnabled(false);
            }
        }
    }

    private boolean checkAllNeedPermissions() {
        List<String> permissionsList = new ArrayList<String>();
        int smsPermission = checkSelfPermission(Manifest.permission.READ_SMS);
        int dialPermission = checkSelfPermission(Manifest.permission.CALL_PHONE);
        int contactPermission = checkSelfPermission(Manifest.permission.READ_CONTACTS);

        if (smsPermission != PackageManager.PERMISSION_GRANTED) {
            permissionsList.add(Manifest.permission.READ_SMS);
        }
        if (dialPermission != PackageManager.PERMISSION_GRANTED) {
            permissionsList.add(Manifest.permission.CALL_PHONE);
        }
        if (contactPermission != PackageManager.PERMISSION_GRANTED) {
            permissionsList.add(Manifest.permission.READ_CONTACTS);
        }
        if (permissionsList.size() > 0) {
            requestPermissions(permissionsList.toArray(
                    new String[permissionsList.size()]), REQUEST_CODE_ASK_MULTIPLE_PERMISSIONS);
            return false;
        }
        return true;
    }

    @Override
    public void onRequestPermissionsResult (
            int requestCode, String[] permissions, int[] grantResults) {

        if (requestCode == REQUEST_CODE_ASK_MULTIPLE_PERMISSIONS) {
            Map<String, Integer> perms = new HashMap<String, Integer>();
            // Initial
            perms.put(Manifest.permission.READ_SMS,
                    PackageManager.PERMISSION_GRANTED);
            perms.put(Manifest.permission.CALL_PHONE,
                    PackageManager.PERMISSION_GRANTED);
            perms.put(Manifest.permission.READ_CONTACTS,
                    PackageManager.PERMISSION_GRANTED);
            // Fill with results
            for (int i = 0; i < permissions.length; i++) {
                perms.put(permissions[i], grantResults[i]);
            }
            if (perms.get(Manifest.permission.READ_SMS)
                    == PackageManager.PERMISSION_GRANTED
                    && perms.get(Manifest.permission.CALL_PHONE)
                    == PackageManager.PERMISSION_GRANTED
                    && perms.get(Manifest.permission.READ_CONTACTS)
                    == PackageManager.PERMISSION_GRANTED) {
                createContinue();
            } else {
                // Permission Denied
                Toast.makeText(FirewallActivity.this,
                        R.string.have_not_all_permissions, Toast.LENGTH_SHORT)
                        .show();
                createContinue();
            }
        }
    }

}
