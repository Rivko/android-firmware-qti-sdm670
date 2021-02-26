/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

import android.content.Context;
import android.content.Intent;
import android.os.HandlerThread;
import android.os.Message;
import android.util.ArrayMap;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService.ServiceHandler;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

public class CaseServiceManager {

    private final ArrayMap<String, ServiceHandler> mServiceHandlerMap = new ArrayMap<String, ServiceHandler>(); //key:serviceName value:Handler of service,in sub thread

    private volatile static CaseServiceManager mCaseManagerService = null;
    public final static int MSG_RUN = 0;
    public final static int MSG_STOP = 1;
    public final static int MSG_REGISTER = 2;
    public final static int MSG_RUN_FUNCTION = 3;

    private CaseServiceManager() {
        super();
    }

    public static CaseServiceManager getInstance() {
        if (mCaseManagerService == null) {
            synchronized (CaseServiceManager.class) {
                if (mCaseManagerService == null) {
                    mCaseManagerService = new CaseServiceManager();
                }
            }
        }
        return mCaseManagerService;
    }

    public void addServiceHandler(String key, ServiceHandler handler) {
        mServiceHandlerMap.put(key, handler);
        LogUtils.logi("ServiceHandler added for service of :" + key);
    }

    public void removeServiceHandler(String key) {
        ServiceHandler serviceHandler = mServiceHandlerMap.remove(key);
        if (serviceHandler != null) {
            LogUtils.logi("serviceHandler removed: " + serviceHandler.getClass().getCanonicalName());
        }
    }

    public ServiceHandler getServiceHandler(String key) {
        return mServiceHandlerMap.get(key);
    }

    public void startCaseService(Context context, TestCase testCase) {
        Class<?> serviceClass  = testCase.getServiceClass();
        if (serviceClass == null) {
            LogUtils.loge("startCaseService: class is null!!");
            return;
        }

        String caseName = testCase.getName();
        LogUtils.logi("startCaseService:" + caseName);
        Intent intent = new Intent(context, serviceClass);
        intent.setPackage(context.getPackageName());
        intent.putExtra(Utils.BUNDLE_KEY_CASE_NAME, caseName);
        context.startService(intent);
    }

    public int runService(Context context, TestCase testCase) {

        String caseName = testCase.getName();
        LogUtils.logi("need run service of :" + caseName);
        ServiceHandler serviceHandler = getServiceHandler(caseName);

        if (serviceHandler == null) {
            startCaseService(context, testCase);
            serviceHandler = getServiceHandler(caseName);
        }

        if (serviceHandler != null) {
            LogUtils.logi(" service of :" + caseName + " start to run");
            goRun(serviceHandler, testCase);
            return 0;
        } else {
            LogUtils.loge(" service of :" + caseName + " not exist");
            return -1;
        }
    }

    public int runServiceFunction(TestCase testCase, String functionName) {
        String caseName = testCase.getName();
        LogUtils.logi("need run functioin service of :" + caseName);
        ServiceHandler serviceHandler = getServiceHandler(caseName);

        if (serviceHandler != null) {
            LogUtils.logi(" service of : :" + caseName + " start to run. functionName: " + functionName);
            goRunFunction(serviceHandler, testCase, functionName);
            return 0 ;
        } else {
            LogUtils.loge(" service of : :" + caseName + " not exist");
            return -1;
        }
    }

    public int stopService(TestCase testCase) {
        String caseName = testCase.getName();
        LogUtils.logi("need stop service of : :" + caseName);
        ServiceHandler serviceHandler = getServiceHandler(caseName);

        if (serviceHandler != null) {
            LogUtils.logi(" service of : :" + caseName + " start to stop");
            goStop(serviceHandler, testCase);
            return 0 ;
        } else {
            LogUtils.loge( " service of : :" + caseName + " not exist");
            return -1;
        }
    }

    public void stopAllService(Context context) {
        for (String caseName : mServiceHandlerMap.keySet()) {
            Class<?> clazz = Utils.getServiceClass(caseName);
            if (clazz == null) {
                LogUtils.loge("can't found class:" + caseName);
                return;
            }
            stopServiceThread(caseName);
            Intent intent = new Intent(context, clazz);
            LogUtils.logi("Stop service of : : " + clazz.getCanonicalName());
            context.stopService(intent);
        }
        mServiceHandlerMap.clear();
    }

    public void stopServiceThread(String caseName) {
        ServiceHandler handler = getServiceHandler(caseName);
        if (handler == null) {
            LogUtils.loge("can't found class:" + handler.getClass().getCanonicalName());
            return;
        }

        HandlerThread thread = (HandlerThread)handler.getLooper().getThread();
        thread.quit();
        thread = null;
        handler = null;
    }

    public void goRegister(ServiceHandler handler, TestCase testCase){
        LogUtils.logi( "goRegister");
        sendMsg(handler, MSG_REGISTER);
    }

    private void goRun(ServiceHandler handler, TestCase testCase){
        LogUtils.logi( "goRun");
        Intent intent = new Intent();
        intent.putExtra(Utils.BUNDLE_KEY_CASE_NAME, testCase.getName());
        sendMsg(handler, MSG_RUN, intent);
    }

    private void goStop(ServiceHandler handler, TestCase testCase){
        LogUtils.logi( "goStop");
        Intent intent = new Intent();
        intent.putExtra(Utils.BUNDLE_KEY_CASE_NAME, testCase.getName());
        sendMsg(handler, MSG_STOP, intent);
    }

    private void goRunFunction(ServiceHandler handler, TestCase testCase, String functionName){
        LogUtils.logi( "goRunFunction");
        Intent intent = new Intent();
        intent.putExtra(Utils.BUNDLE_KEY_CASE_NAME, testCase.getName());
        intent.putExtra(Utils.BUNDLE_KEY_RUN_FUNCTIOIN, functionName);
        sendMsg(handler, MSG_RUN_FUNCTION, intent);
    }

    private void sendMsg(ServiceHandler handler, int what, Object obj){
        if ( handler != null){
            Message msg = handler.obtainMessage();
            msg.what = what;
            msg.obj = obj;
            sendMsg(handler,msg);
        }
    }

    private void sendMsg(ServiceHandler handler, int what){
        sendMsg(handler, what, null);
    }

    private void sendMsg(ServiceHandler handler, Message msg){
        handler.sendMessage(msg);
    }
}
