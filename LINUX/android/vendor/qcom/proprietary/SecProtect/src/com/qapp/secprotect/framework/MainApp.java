/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
package com.qapp.secprotect.framework;

import android.app.Application;
import android.content.Context;
import android.os.storage.StorageManager;

import java.lang.reflect.Method;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;

import static com.qapp.secprotect.utils.UtilsLog.logd;

public class MainApp extends Application {

    public ScheduledExecutorService scheduledExecutorService = Executors
            .newSingleThreadScheduledExecutor();
    private static MainApp mMainApp;
    private static Context mContext;
    private BlockingQueue<String> mBlockingQueue = new ArrayBlockingQueue<String>(
            1);
    public static String sPath;
    public static String sPathSd;
    public String mInternalKey = null;
    public String mSdcardKey = null;
    public String mInternalPassword = null;
    public String mSdcardPassword = null;

    private void init() {
        logd("");
        mContext = getApplicationContext();
        StorageManager storageManager = (StorageManager) mContext.getSystemService(Context.STORAGE_SERVICE);
        try {
            Class<?>[] paramList = {};
            Method pathsMethod = StorageManager.class.getMethod("getVolumePaths", paramList);
            pathsMethod.setAccessible(true);
            Object[] paramsObj = {};
            Object pathList = pathsMethod.invoke(storageManager, paramsObj);
            for (int i = 0; i < ((String[])pathList).length; i++) {
                if(((String[])pathList)[i].indexOf("emulated") > 0){
                    sPath = ((String[])pathList)[i];
                }else{
                    sPathSd = ((String[])pathList)[i];
                }
            }
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    public void clearPasswordCache() {
        mInternalKey = null;
        mSdcardKey = null;
        mInternalPassword = null;
        mSdcardPassword = null;
    }

    @Override
    public void onCreate() {
        logd("");
        init();
        super.onCreate();
    }

    @Override
    public void onLowMemory() {
        logd("");
        super.onLowMemory();
    }

    @Override
    public void onTerminate() {
        logd("");
        super.onTerminate();
    }

    public static MainApp getInstance() {
        if (mMainApp == null)
            mMainApp = new MainApp();
        return mMainApp;
    }

    public MainApp() {
    }

    public ScheduledExecutorService getScheduledExecutorService() {
        return scheduledExecutorService;
    }

    public Context getContext() {
        return mContext;
    }

    public static String getPath() {
        return sPath;
    }

    public static String getPathSd() {
        return sPathSd;
    }
}
