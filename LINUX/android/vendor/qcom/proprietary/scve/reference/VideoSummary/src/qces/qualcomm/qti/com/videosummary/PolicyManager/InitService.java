/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.PolicyManager;

import android.app.IntentService;
import android.content.Context;
import android.content.Intent;
import android.preference.PreferenceManager;
import android.util.Log;

import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.CalendarHelper;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;

/**
 * An {@link IntentService} subclass for handling asynchronous task requests in
 * a service on a separate handler thread.
 * <p/>
 * TODO: Customize class - update intent actions, extra parameters and static
 * helper methods.
 */
public class InitService extends IntentService {
    // TODO: Rename actions, choose action names that describe tasks that this
    // IntentService can perform, e.g. ACTION_FETCH_NEW_ITEMS
    private static final String ACTION_INIT = "qces.qualcomm.qti.com.videosummary.PolicyManager.action.INIT";

    // TODO: Rename parameters
    private static final String EXTRA_PARAM1 = "qces.qualcomm.qti.com.videosummary.PolicyManager.extra.PARAM1";
    private static final String EXTRA_PARAM2 = "qces.qualcomm.qti.com.videosummary.PolicyManager.extra.PARAM2";

    private static final String TAG = InitService.class.getSimpleName();

    /**
     * Starts this service to perform action Foo with the given parameters. If
     * the service is already performing a task this action will be queued.
     *
     * @see IntentService
     */
    // TODO: Customize helper method
    public static void startActionInit(Context context) {
        Intent intent = new Intent(context, InitService.class);
        intent.setAction(ACTION_INIT);
        context.startService(intent);
    }

    public InitService() {
        super("InitService");
    }

    @Override
    protected void onHandleIntent(Intent intent) {
        Log.i(TAG, "onHandleIntent InitService");
        if (intent != null) {
            final String action = intent.getAction();
            if (ACTION_INIT.equals(action)) {
                handleActionInit();
            }
        }
    }

    /**
     * Handle action Init in the provided background thread with the provided
     * parameters.
     */
    private void handleActionInit() {

        Utils.copyConfigFileAssets(this);
        /*
         *Do NOT change the order for the below 2 function calls.
         * 1st settings need to be populated and then schedule alarms based on the values in
         * settings
         */
        populateDefaultSettings();
        initializeAssemblyTableUpdatedTimestamp();
        scheduleAlarms();
    }

    void populateDefaultSettings() {
        //To load the default settings values from xml for first time
        PreferenceManager.setDefaultValues(this, R.xml.pref_settings, false);

    }

    void scheduleAlarms() {
        Log.i(TAG, "updating/scheduling alarms");
        FrequencyScheduleAlarmReceiver.setupAlarmForSelectedFrequency(this);
    }

    void initializeAssemblyTableUpdatedTimestamp() {
        if (SettingsActivity.getAssemblyTimelineTableUpdateTimestamp(this) == 0) {
            SettingsActivity.setAssemblyTimelineTableUpdateTimestamp(this, CalendarHelper.getInitialValForAssemblyTableUpdateTimestamp());
        }
    }

}
