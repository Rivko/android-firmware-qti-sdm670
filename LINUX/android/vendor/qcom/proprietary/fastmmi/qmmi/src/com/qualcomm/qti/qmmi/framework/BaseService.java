/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.framework;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.model.CaseManager;
import com.qualcomm.qti.qmmi.model.CaseServiceManager;
import com.qualcomm.qti.qmmi.model.FunctionCallback;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import java.util.HashMap;
import java.util.Map;

public abstract class BaseService extends Service {

    private final Map<String, FunctionCallback> mFunctionMap = new HashMap<String, FunctionCallback>();

    public final class ServiceHandler extends Handler{
        public ServiceHandler(Looper looper){
            super(looper);
        }

        @Override
        public void handleMessage(Message msg){

            switch(msg.what){
                case CaseServiceManager.MSG_REGISTER:

                    // register
                    LogUtils.logi("handleMessage:register");
                    register();

                    break;
                case CaseServiceManager.MSG_RUN:

                    // run
                    Intent intentRun = (Intent)msg.obj;
                    String caseNameRun = intentRun.getStringExtra(Utils.BUNDLE_KEY_CASE_NAME);
                    LogUtils.logi("handleMessage:run Case name is:" + caseNameRun);
                    TestCase testCaseRun = CaseManager.getInstance(getApplicationContext()).getTestCaseByName(caseNameRun);
                    run(testCaseRun);
                    testCaseRun.setRunning(true);

                    break;
                case CaseServiceManager.MSG_STOP:

                    // stop
                    Intent intentStop = (Intent)msg.obj;
                    String caseNameStop = intentStop.getStringExtra(Utils.BUNDLE_KEY_CASE_NAME);
                    LogUtils.logi("handleMessage:stop Case name is:" + caseNameStop);
                    TestCase testCaseStop = CaseManager.getInstance(getApplicationContext()).getTestCaseByName(caseNameStop);
                    stop(testCaseStop);

                    /* Just set running as false for BG launch mode,
                     * For FG launch mode, should change in BaseActivity.onDestory().*/
                    if (testCaseStop.getLaunchMode() == TestCase.LAUNCH_MODE_BACKGROUND){
                        testCaseStop.setRunning(false);
                    }

                    break;
                case CaseServiceManager.MSG_RUN_FUNCTION:

                    // run function
                    Intent intentFun = (Intent)msg.obj;
                    String caseNameFun = intentFun.getStringExtra(Utils.BUNDLE_KEY_CASE_NAME);
                    String functionName = intentFun.getStringExtra(Utils.BUNDLE_KEY_RUN_FUNCTIOIN);
                    LogUtils.logi("handleMessage:runFunction Case name is:" + caseNameFun);
                    TestCase testCaseFun = CaseManager.getInstance(getApplicationContext()).getTestCaseByName(caseNameFun);
                    runFunction(testCaseFun,functionName);

                    break;
            }

        }
    };

    public void updateView(String caseName, String message) {
        Bundle bundle = new Bundle();
        bundle.putString(Utils.BUNDLE_KEY_MESSAGE, message);
        bundle.putString(Utils.BUNDLE_KEY_CASE_NAME, caseName);
        this.updateView(bundle);
    }

    public void updateView(Bundle bundle) {
        Intent intent = new Intent();
        intent.putExtras(bundle);
        intent.setAction(Utils.ACTION_UPDATE_MESSAGE);
        sendOrderedBroadcast(intent,null);
    }

    /**
     * Case should call this method to set the result of each case:
     *
     */
    public void updateResultForCase(Bundle bundle) {
        Intent intent = new Intent();
        intent.putExtras(bundle);
        TestCase testCase = CaseManager.getInstance(
                getApplicationContext()).getTestCaseByName(
                        bundle.getString(Utils.BUNDLE_KEY_CASE_NAME));

        /** For different run mode case, will send different broadcast.
         *  In MainActivity, will handle auto case.
         *  In BaseActivity, will handle PCDA and UI cases.
         */
        LogUtils.logi("launchMode:" + testCase.getLaunchMode());
        switch (testCase.getLaunchMode()){
            case TestCase.LAUNCH_MODE_BACKGROUND:
                intent.setAction(Utils.ACTION_UPDATE_BACKGROUND_CASE_RESULT);
                break;
            case TestCase.LAUNCH_MODE_FOREGROUND:
                intent.setAction(Utils.ACTION_UPDATE_FOREGROUND_CASE_RESULT);
                break;
        }

        sendOrderedBroadcast(intent,null);
    }

    /**
     * Case should call this method to set the result of each case:
     * result should be: STATE_NONE,STATE_PASS,STATE_FAIL
     */
    public void updateResultForCase(String caseName, int state) {
        Bundle bundle = new Bundle();
        bundle.putInt(Utils.BUNDLE_KEY_RESULT, state);
        bundle.putString(Utils.BUNDLE_KEY_CASE_NAME, caseName);
        this.updateResultForCase(bundle);
    }

    public void registerFunction(String functionName, FunctionCallback callback) {
        LogUtils.logi( "add method:" + functionName + ",for " + getClass().getCanonicalName());
        mFunctionMap.put(functionName, callback);
    }

    public void unregisterFunction(String functionName) {
        mFunctionMap.remove(functionName);
    }

    public Map<String, FunctionCallback> getFunctionMap() {
        return mFunctionMap;
    }

    private void prepareThread(Intent intent){

        String caseName = intent.getExtras().getString(Utils.BUNDLE_KEY_CASE_NAME);
        LogUtils.logi("prepareThread:" + caseName);

        CaseServiceManager mServiceManager = CaseServiceManager.getInstance();

        if (mServiceManager != null) {
            if (mServiceManager.getServiceHandler(caseName) == null){
                LogUtils.logi("create new thread for:" + caseName);
                HandlerThread thread = new HandlerThread(caseName);
                thread.start();
                ServiceHandler serviceHandler = new ServiceHandler(thread.getLooper());
                mServiceManager.addServiceHandler(caseName, serviceHandler);

                TestCase testCase = CaseManager.getInstance(getApplicationContext()).getTestCaseByName(caseName);
                mServiceManager.goRegister(serviceHandler, testCase);
            }
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        LogUtils.logi( "BaseService - onDestroy " + getClass().getCanonicalName());
    }


    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        prepareThread(intent);

        return START_NOT_STICKY;
    }

    public int runFunction(TestCase testCase, String functionName) {

        FunctionCallback callback = mFunctionMap.get(functionName);
        if (callback == null) {
            LogUtils.loge("no such method, please check");
            return -1;
        }

        return callback.runFunction(testCase);
    }

    public abstract void register();

    public abstract int run(TestCase testCase);

    public abstract int stop(TestCase testCase);
}
