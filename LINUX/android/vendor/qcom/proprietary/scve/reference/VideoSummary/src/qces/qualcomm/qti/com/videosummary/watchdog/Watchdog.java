/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.watchdog;

import android.content.Context;
import android.util.Log;

import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;

/**
 * Created by mithun on 10/8/15.
 */
public class Watchdog {
    private boolean petted = false;
    private int barkCount = 0;
    private static boolean sleeping = true;
    private static Watchdog watchdogRef = null;
    private static boolean enabled = false;

    private static String TAG = Watchdog.class.getSimpleName();

    public static Watchdog getWatchdogInstance() {
        if (watchdogRef == null) {
            Log.i(TAG, "instantiating a new watch dog");
            watchdogRef = new Watchdog();
        }
        return watchdogRef;
    }

    private Watchdog() {

    }

    Thread dogBarkThread = null;

    private void startWatchdog() {
        while (!sleeping) {
            try {
                Log.i(TAG, "watch dog is awake.");
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (petted) {
                petted = false;
                barkCount = 0;
            } else {
                watchdogBark();
            }
        }
    }


    /***
     * Kick the dog to wake it up, then you need to keep petting it, other wise it will bark few times and will bite you (crash the app)
     */
    public void watchdogKick(Context context) {
        enabled = SettingsActivity.getPrefWatchDogEnabled(context);
        if (!enabled) {
            return;
        }
        if (!sleeping) {
            //watch dog already kicked and started, dont do anything now.
            return;
        }
        sleeping = false;//dog was kicked and woken up
        barkCount = 0;
        if (dogBarkThread == null) {
            dogBarkThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    startWatchdog();
                }
            });
        }
        dogBarkThread.start();
        watchdogPet("first time petting");//for the first time lets pet the dog, so that it wont bark immediately
    }

    private void watchdogBark() {
        barkCount++;
        Log.i(TAG, "watch dog bark (count = " + barkCount + ")");
        if (barkCount >= 5) {
            Log.i(TAG, "watch dog barkCount >=3, so dog will bite");
            watchdogBite();
        }
    }

    private void watchdogBite() {
        throw new AssertionError("Watch Dig Bite!!!");
    }

    public void watchdogPet(String msg) {
        if (!enabled) {
            return;
        }
        if (sleeping) {
            Log.i(TAG, "trying to pet a sleeping dog, please kick the dog first to wake it up and then pet");
            watchdogBark();
        }
        Log.i(TAG, "petting the dog. =>" + msg);
        petted = true;
    }

    public void watchdogSleep() {
        Log.i(TAG, "watch dog is put back to sleep");
        sleeping = true;
        dogBarkThread = null;
    }

    public static void enableDog() {
        enabled = true;
    }

    public static void disableDog() {
        enabled = false;
        if (!getWatchdogInstance().sleeping) {
            getWatchdogInstance().watchdogSleep();
        }
    }

    public static boolean isWatchDogActive() {
        return !sleeping;
    }

    public static boolean isWatchDogEnabled() {
        return enabled;
    }
}
