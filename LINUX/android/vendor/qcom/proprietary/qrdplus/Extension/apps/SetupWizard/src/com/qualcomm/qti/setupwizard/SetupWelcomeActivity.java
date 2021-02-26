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
 * Copyright (C) 2013 The CyanogenMod Project
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

package com.qualcomm.qti.setupwizard;

import android.app.Activity;
import android.app.StatusBarManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemProperties;
import android.provider.Settings;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.NumberPicker;

import com.qualcomm.qti.setupwizard.ui.EmergencyCallBtn;
import com.qualcomm.qti.setupwizard.ui.LocalePicker;

import java.util.List;
import java.util.Locale;

public class SetupWelcomeActivity extends Activity {
    public static final int DISABLE_MASK_CUSTOM = View.STATUS_BAR_DISABLE_EXPAND
            | View.STATUS_BAR_DISABLE_NOTIFICATION_ICONS
            | View.STATUS_BAR_DISABLE_NOTIFICATION_ALERTS
            | View.STATUS_BAR_DISABLE_NOTIFICATION_TICKER
            | View.STATUS_BAR_DISABLE_RECENT
            | View.STATUS_BAR_DISABLE_HOME
            | View.STATUS_BAR_DISABLE_BACK
            | View.STATUS_BAR_DISABLE_CLOCK
            | View.STATUS_BAR_DISABLE_SEARCH;

    private static final String ACTION_EMERGENCY_DIAL = "com.android.phone.EmergencyDialer.DIAL";
    private static final String SETUP_WIZARD_ACTIVE_STATE = "setup_wizard_active_state";
    public static final int REQUEST_CODE = 10;
    private Window mRootView;
    private ArrayAdapter<com.android.internal.app.LocalePicker.LocaleInfo> mLocaleAdapter;
    private Locale mInitialLocale;
    private Locale mCurrentLocale;
    private int[] mAdapterIndices;
    private LocalePicker mLanguagePicker;
    private ImageButton mSelectBtn;
    private EmergencyCallBtn mEmergencyBtn;
    private final Handler mHandler = new Handler();

    private static final String ORIGINAL_LAUNCHER_PACKAGENAME = "com.android.launcher3";
    private static final String ORIGINAL_LAUNCHER_CLASSNAME = "com.android.launcher3.Launcher";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //switch google original provision and qualcomm setupwizard
        if (!SystemProperties.getBoolean("ro.setupwizard.enable", false)) {
            finishSetup();
            return;
        }

        setContentView(R.layout.setup_welcome);
        mRootView = getWindow();
        ((StatusBarManager)getApplication().getSystemService(Context.STATUS_BAR_SERVICE))
                .disable(DISABLE_MASK_CUSTOM);
        mLanguagePicker = (LocalePicker) findViewById(R.id.locale_list);
        mSelectBtn = (ImageButton) findViewById(R.id.select_language);
        mEmergencyBtn = (EmergencyCallBtn) findViewById(R.id.emergency_call);
        loadLanguages();
        mSelectBtn.setOnClickListener(mSelect);
        mEmergencyBtn.setOnClickListener(mEmergencyCall);

        // Add a persistent setting to let other apps know the SetupWizard active state.
        // Value 0: SetupWizard finish
        // Value 1: SetupWizard active
        Settings.Global.putInt(getContentResolver(), SETUP_WIZARD_ACTIVE_STATE, 1);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mRootView != null) {
            int flag = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    | View.SYSTEM_UI_FLAG_IMMERSIVE
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
            mRootView.getDecorView().setSystemUiVisibility(flag);
            mRootView.setStatusBarColor(Color.TRANSPARENT);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_CODE) {
            if (resultCode == SetupPageActivity.RESULT_CODE) {
                finishSetup();
            }
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        return super.dispatchKeyEvent(event);
    }

    private final Runnable mUpdateLocale = new Runnable() {
        public void run() {
            if (mCurrentLocale != null) {
                com.android.internal.app.LocalePicker.updateLocale(mCurrentLocale);
            }
        }
    };

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    private void loadLanguages() {
        mLocaleAdapter = com.android.internal.app.LocalePicker.constructAdapter(this,
                R.layout.locale_picker_item, R.id.locale);
        mInitialLocale = Locale.getDefault();
        mCurrentLocale = mInitialLocale;
        mAdapterIndices = new int[mLocaleAdapter.getCount()];
        int currentLocaleIndex = 0;
        String[] labels = new String[mLocaleAdapter.getCount()];
        for (int i = 0; i < mAdapterIndices.length; i++) {
            com.android.internal.app.LocalePicker.LocaleInfo localLocaleInfo =
                    mLocaleAdapter.getItem(i);
            Locale localLocale = localLocaleInfo.getLocale();
            if (localLocale.equals(mCurrentLocale)) {
                currentLocaleIndex = i;
            }
            mAdapterIndices[i] = i;
            labels[i] = localLocaleInfo.getLabel();
        }
        mLanguagePicker.setDisplayedValues(labels);
        mLanguagePicker.setMaxValue(labels.length - 1);
        mLanguagePicker.setValue(currentLocaleIndex);
        mLanguagePicker.setDescendantFocusability(NumberPicker.FOCUS_BLOCK_DESCENDANTS);
        mLanguagePicker.setOnValueChangedListener(new LocalePicker.OnValueChangeListener() {
            public void onValueChange(LocalePicker picker, int oldVal, int newVal) {
                setLocaleFromPicker();
            }
        });
    }

    private void setLocaleFromPicker() {
        int i = mAdapterIndices[mLanguagePicker.getValue()];
        final com.android.internal.app.LocalePicker.LocaleInfo localLocaleInfo = mLocaleAdapter
                .getItem(i);
        onLocaleChanged(localLocaleInfo.getLocale());
    }

    private void onLocaleChanged(Locale paramLocale) {
        Resources localResources = getResources();
        Configuration localConfiguration1 = localResources.getConfiguration();
        Configuration localConfiguration2 = new Configuration();
        localConfiguration2.locale = paramLocale;
        localResources.updateConfiguration(localConfiguration2, null);
        localResources.updateConfiguration(localConfiguration1, null);
        mHandler.removeCallbacks(mUpdateLocale);
        mCurrentLocale = paramLocale;
        mHandler.postDelayed(mUpdateLocale, 1000);
    }

    public OnClickListener mSelect = new OnClickListener() {
        @Override
        public void onClick(View v) {
            Intent intent = new Intent(SetupWelcomeActivity.this,
                    SetupPageActivity.class);
            startActivityForResult(intent, REQUEST_CODE);
        }
    };

    public OnClickListener mEmergencyCall = new OnClickListener() {
        @Override
        public void onClick(View v) {
            Intent intent = new Intent(ACTION_EMERGENCY_DIAL);
            startActivity(intent);
        }
    };

    protected void finishSetup() {
        // TODO Auto-generated method stub
        // Add a persistent setting to allow other apps to know the device has been provisioned.
        Settings.Global.putInt(getContentResolver(), Settings.Global.DEVICE_PROVISIONED, 1);
        Settings.Secure.putInt(getContentResolver(), Settings.Secure.USER_SETUP_COMPLETE, 1);

        // Add a persistent setting to let other apps know the SetupWizard active state.
        // Value 0: SetupWizard finish
        // Value 1: SetupWizard active
        Settings.Global.putInt(getContentResolver(), SETUP_WIZARD_ACTIVE_STATE, 0);

        // remove this activity from the package manager.
        PackageManager pm = getPackageManager();
        ComponentName name = new ComponentName(this, SetupWelcomeActivity.class);
        pm.setComponentEnabledSetting(name, PackageManager.COMPONENT_ENABLED_STATE_DISABLED,
                PackageManager.DONT_KILL_APP);

        ((StatusBarManager)getApplication().getSystemService(Context.STATUS_BAR_SERVICE))
                .disable(StatusBarManager.DISABLE_NONE);

        //set Launcher3 as the preferred home activity
        setupDefaultLauncher(pm);
        // terminate the activity.
        finish();
    }

    private void setupDefaultLauncher(PackageManager pm){
        Intent queryIntent = new Intent();
        queryIntent.addCategory(Intent.CATEGORY_HOME);
        queryIntent.setAction(Intent.ACTION_MAIN);

        List<ResolveInfo> homeActivities = pm.queryIntentActivities(queryIntent, 0);
        if(homeActivities == null) {
            return;
        }

        ComponentName defaultLauncher = new ComponentName(ORIGINAL_LAUNCHER_PACKAGENAME,
                ORIGINAL_LAUNCHER_CLASSNAME);
        int activityNum = homeActivities.size();
        ComponentName[] set = new ComponentName[activityNum];
        int defaultMatch = -1;
        for(int i = 0; i < activityNum; i++){
            ResolveInfo info = homeActivities.get(i);
            set[i] = new ComponentName(info.activityInfo.packageName, info.activityInfo.name);
            if(ORIGINAL_LAUNCHER_CLASSNAME.equals(info.activityInfo.name)
                    && ORIGINAL_LAUNCHER_PACKAGENAME.equals(info.activityInfo.packageName)){
                defaultMatch = info.match;
            }
        }

        //if Launcher3 is not found, do not set anything
        if(defaultMatch == -1){
            return;
        }
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_MAIN);
        filter.addCategory(Intent.CATEGORY_HOME);
        filter.addCategory(Intent.CATEGORY_DEFAULT);

        pm.addPreferredActivity(filter, defaultMatch, set, defaultLauncher);
    }
}
