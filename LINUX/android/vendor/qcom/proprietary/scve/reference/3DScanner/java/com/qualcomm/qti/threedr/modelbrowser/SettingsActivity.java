/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.modelbrowser;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.SystemClock;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;

import com.qualcomm.qti.threedr.R;
import com.qualcomm.qti.threedr.orbbec.OBNativeHelper;

import java.text.SimpleDateFormat;
import java.util.Date;

public class SettingsActivity extends AppCompatActivity {
    private static final String TAG = SettingsActivity.class.getCanonicalName();
    SettingsFragment settingsFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);


        Toolbar mToolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(mToolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        setTitle(R.string.settings);

        if (savedInstanceState == null) {
            settingsFragment = new SettingsFragment();
            getFragmentManager().beginTransaction()
                    .add(R.id.fragmentContainer, settingsFragment).commit();
        } else {
            //mainFragment = getSupportFragmentManager().getFragments().get(0);
            Log.e("X3", "fragment = " + getSupportFragmentManager().getFragments().get(0));
        }
    }

    public static class SettingsFragment extends PreferenceFragment implements SharedPreferences.OnSharedPreferenceChangeListener {
        public SettingsFragment() {

        }

        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            addPreferencesFromResource(R.xml.prefs);

            Preference button = findPreference(getString(R.string.pref_update_orbbec_calibration_values));
            button.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
                @Override
                public boolean onPreferenceClick(Preference preference) {
                    Thread thread = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            long startTime = System.currentTimeMillis();
                            OBNativeHelper obNativeHelper = OBNativeHelper.getInstance(getContext());
                            obNativeHelper.openOBSensor();
                            obNativeHelper.readOBCmosParams();
                            obNativeHelper.closeOBSensor();
                            long endtime =System.currentTimeMillis();
                            Log.i(TAG,"time take for Orbbec calibration update = "+(endtime-startTime)+"millis");

                        }
                    });
                    thread.start();
                    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                    String currentDateTime = dateFormat.format(new Date()); // Find todays date
                    preference.setSummary("Last updated @ " + currentDateTime);
                    return true;
                }
            });
        }

        @Override
        public void onResume() {
            super.onResume();
            getPreferenceManager().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);

        }

        @Override
        public void onPause() {
            getPreferenceManager().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);
            super.onPause();
        }

        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
            if (key.equals(getResources().getString(R.string.prefs_tutorialScreens))) {
                setPrefDontShowTutuorialCheckbox(getContext(), !getPrefTutuorialScreenSwitch(getContext()));
            }
        }
    }

    /**
     * Tutorial screen switch and dont show tutorial checkbox are similar but its values are inverted.
     * i.e if dont show tutorial is unchecked(value = false), it means sDisplay tutorial screen switch should be ON (value = true)
     * so when either one of them change, then we need to update the value of the other accordingly
     */
    public static void setPrefDontShowTutuorialCheckbox(Context context, boolean val) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putBoolean(context.getResources().getString(R.string.pref_dont_show_tutorial), val);//checkbox
        editor.putBoolean(context.getResources().getString(R.string.prefs_tutorialScreens), !val);//switch
        editor.commit();
    }

    public static boolean getPrefDontShowTutuorialCheckbox(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_dont_show_tutorial), !context
                .getResources().getBoolean(R.bool.perf_show_tutorial_default_val));
    }

    public static boolean getPrefTutuorialScreenSwitch(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.prefs_tutorialScreens), context
                .getResources().getBoolean(R.bool.perf_show_tutorial_default_val));
    }


}
