/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Key;

import android.app.ActivityManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.PowerManager;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.ShellUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.android.internal.widget.LockPatternUtils;

public class KeyActivity extends BaseActivity {
    private static final String SYSTEM_DIALOG_REASON_KEY = "reason";
    private static final String SYSTEM_DIALOG_REASON_HOME_KEY = "homekey";
    private static final String SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS = "globalactions";
    private static final String SYSTEM_DIALOG_REASON_RECENT_APPS = "recentapps";

    List<String> mKeys = new ArrayList<>(0);
    public static final int FLAG_HOMEKEY_DISPATCHED = 0x80000000;

    private HashMap<Integer, String> mKeyMap = new HashMap<Integer, String>();

    {
        mKeyMap.put(KeyEvent.KEYCODE_HOME, "home");
        mKeyMap.put(KeyEvent.KEYCODE_POWER, "power");
        mKeyMap.put(KeyEvent.KEYCODE_APP_SWITCH, "app_switch");
        mKeyMap.put(KeyEvent.KEYCODE_BACK, "back");
        mKeyMap.put(KeyEvent.KEYCODE_VOLUME_DOWN, "volumedown");
        mKeyMap.put(KeyEvent.KEYCODE_VOLUME_UP, "volumeup");
    }

    /**
     * Can't directly add into TestCase.TestData.
     * When test power/home key, will call BaseActivity.onResume() and will clear TestCase.TestData.
     */
    private Map<String, String> mKeysData = new HashMap<String, String>();

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context arg0, Intent arg1) {
            // TODO Auto-generated method stub
            String action = arg1.getAction();
            LogUtils.logi( "onReceive: action: " + action);

            /* Action: Close system dialogs
             * When click/longclick HOME/RecentApps/... Key, will close system dialog.
             */
            if (Intent.ACTION_CLOSE_SYSTEM_DIALOGS.equals(action)) {
                //Reason for closing system dialogs
                String reason = arg1.getStringExtra(SYSTEM_DIALOG_REASON_KEY);
                 if (SYSTEM_DIALOG_REASON_HOME_KEY.equals(reason)) {
                     //Press HOME key
                     LogUtils.logi("reason: homekey");
                     StartSelfAfterHomeKey();
                     mKeysData.put(mKeyMap.get(KeyEvent.KEYCODE_HOME), "detected");
                     TextView keyText = (TextView) findViewById(R.id.home);
                     setKeyPass(keyText);
                 } else if (SYSTEM_DIALOG_REASON_GLOBAL_ACTIONS.equals(reason)) {
                     //Long press power key
                     LogUtils.logi("reason: globalactions");
                     mKeysData.put(mKeyMap.get(KeyEvent.KEYCODE_POWER), "detected");
                     TextView keyText = (TextView) findViewById(R.id.power);
                     setKeyPass(keyText);
                 }  else if (SYSTEM_DIALOG_REASON_RECENT_APPS.equals(reason)) {
                     //Press Recent apps key
                     LogUtils.logi("reason: recentapps");
                     startSelf();
                     mKeysData.put(mKeyMap.get(KeyEvent.KEYCODE_APP_SWITCH), "detected");
                     TextView keyText = (TextView) findViewById(R.id.recentapps);
                     setKeyPass(keyText);
                 }
            } else if (Intent.ACTION_SCREEN_OFF.equals(action)) {
                //Action: screen off when press power key
                 LogUtils.logi("press powerkey and screen off");
                 //wake up and keep screen on.
                 Utils.wakeUpAndKeepScreenOn(KeyActivity.this);
                 mKeysData.put(mKeyMap.get(KeyEvent.KEYCODE_POWER), "detected");
                 TextView keyText = (TextView) findViewById(R.id.power);
                 setKeyPass(keyText);
            }
        }
    };

    private void StartSelfAfterHomeKey(){

        int waitTimes = 10;
        int counter = 0;
        String pkgName = "com.android.launcher3";

        while (counter < waitTimes){
            if (checkIfPkgName(pkgName)){
                startSelf();
                break;
            }

            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            counter++;
        }
    }

    private boolean checkIfPkgName(String pkgName){
        boolean ret = false;

        ActivityManager manager = (ActivityManager) this.getSystemService(ACTIVITY_SERVICE);
        ActivityManager.RunningAppProcessInfo info = manager.getRunningAppProcesses().get(0);
        LogUtils.logi( "top pkg name:" + info.pkgList[0]);
        return info.pkgList[0].equalsIgnoreCase(pkgName);
    }

    private void startSelf(){
         Intent intent = new Intent(this, KeyActivity.class);
         intent.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
         PendingIntent pendingIntent =
                PendingIntent.getActivity(this, 0, intent, 0);
         try {
             pendingIntent.send();
         } catch (PendingIntent.CanceledException e) {
             e.printStackTrace();
         }
    }

    /**
     * Set screen lock as None to test POWER key easily.
     */
    private void clearScreenLock(){

        // get QMMI userId
        android.os.UserHandle handle = android.os.Process.myUserHandle();
        int userId = handle.getIdentifier();
        LogUtils.logi( "userId:" + userId);

        // get screen lock state: locked or unlocked
        LockPatternUtils utils = new LockPatternUtils(this);

        /* Different type of screen lock has different state:
         * the value of isLockScreenDisable:
         * None: true
         * Swipe, Pattern, PIN, Password: false
         */
        boolean isDisable = utils.isLockScreenDisabled(userId);
        LogUtils.logi( "isLockScreenDisabled:" + isDisable);

        /* get screen secure state: true or false
         * the value of isSecure:
         * Swipe: false
         * None, Pattern, PIN, Password: true
         */
        boolean isSecure = false;

        try {
            isSecure = utils.getLockSettings().getBoolean(LockPatternUtils.DISABLE_LOCKSCREEN_KEY,false,userId);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        LogUtils.logi( "isSecure:" + isSecure);
        if ( !isDisable ){
            utils.setLockScreenDisabled(true,userId);
            if (isSecure ){
                //If screen is locked with Pattern, PIN or password, show the warn info
                Toast.makeText(getApplicationContext(), R.string.keypad_need_unlock_screen, Toast.LENGTH_LONG).show();
            }
        }//If screen don't locked, pass it.
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        clearScreenLock();

        for (Integer in : mKeyMap.keySet()) {
            mKeysData.put(mKeyMap.get(in), "not detected");
        }

        Utils.parseCases(mTestCase.getParameter().get("key"), mKeys);

    }

    @Override
    protected int getLayoutId() {
        return R.layout.key_act;
    }

    @Override
    protected void onResume() {

        registerListener();
        super.onResume();
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        // TODO Auto-generated method stub
        LogUtils.logi( "dispatchKeyEvent -- keyCode:" + event.getKeyCode());
        TextView keyText = null;
        mKeysData.put(mKeyMap.get(event.getKeyCode()), "detected");
        switch (event.getKeyCode()) {
            case KeyEvent.KEYCODE_BACK:
                keyText = (TextView) findViewById(R.id.back);
                break;
            case KeyEvent.KEYCODE_VOLUME_DOWN:
                keyText = (TextView) findViewById(R.id.volume_down);
                break;
            case KeyEvent.KEYCODE_VOLUME_UP:
                keyText = (TextView) findViewById(R.id.volume_up);
                break;
            case KeyEvent.KEYCODE_MENU:
                keyText = (TextView) findViewById(R.id.recentapps);
                break;
        }

        if ( null != keyText) {
            setKeyPass(keyText);
            return true;
        }

        return super.dispatchKeyEvent(event);
    }

    private void setKeyPass(TextView keyText) {
        if (null != keyText) {
            keyText.setBackgroundResource(R.drawable.pass);
        }
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub

        //Write TestCase.TestData
        for (String key : mKeysData.keySet()) {
            mTestCase.addTestData(key, mKeysData.get(key));
            LogUtils.logi( "TestData: " + key + " : " + mKeysData.get(key));
        }

        super.onPause();
    }

    @Override
    protected void onStop() {
        // TODO Auto-generated method stub
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub

        unRegisterListener();
        super.onDestroy();
    }

    private void registerListener() {
        IntentFilter filter = new IntentFilter(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        registerReceiver(mReceiver, filter);
    }

    private void unRegisterListener() {
        if (null != mReceiver) {
            unregisterReceiver(mReceiver);
        }
    }
}
