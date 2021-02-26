package com.qti.datastoragecleanerservice;

/* Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
import java.io.File;
import java.lang.ref.WeakReference;
import java.util.List;

import android.app.Service;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.IBinder;
import android.os.RemoteException;
import android.text.TextUtils;

import com.qti.storagecleaner.ICleanerCallBack;
import com.qti.storagecleaner.IStorageCleanerService;

public class DataStorageCleanerService extends Service {
    private static final String START_CLEAR_APPLICATION_CACHE = "com.qti.datastoragecleanerservice.CLEAR_APPLICATION_STORAGE";
    private static final String APPLICATION_CACHE_PATTERN = "Application Cache";
    private ICleanerCallBack mCallBack;

    public DataStorageCleanerService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        if (mCallBack != null)
            mCallBack = null;
        return super.onUnbind(intent);
    }

    private final IBinder mBinder = new ServiceStub(this);

    private class ServiceStub extends IStorageCleanerService.Stub {
        private WeakReference<DataStorageCleanerService> mService;

        public ServiceStub(DataStorageCleanerService service) {
            mService = new WeakReference<DataStorageCleanerService>(service);
        }

        @Override
        public void startCleaner(String path, String pattern)
                throws RemoteException {
            String[] patten = pattern.split(",");
            mService.get().startCleaner(path, patten);
        }

        @Override
        public void registCleanerCallBack(ICleanerCallBack callback)
                throws RemoteException {
            mCallBack = callback;
        }
    }

    private void startCleaner(final String path, final String[] patten) {
        if (TextUtils.isEmpty(path)) {
            return;
        }
        final File dir = new File(path);
        if (!dir.isDirectory()) {
            return;
        }

        Runnable r = new Runnable() {

            @Override
            public void run() {
                for (String p : patten) {
                    cleanDirectory(dir, p);
                }
            }
        };
        new CleanAsyncTask(r).execute();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        String action = intent.getAction();
        if (START_CLEAR_APPLICATION_CACHE.equals(action)) {
            Runnable r = new Runnable() {
                @Override
                public void run() {
                    beginClearAppsCache();
                }
            };
            new CleanAsyncTask(r).execute();
        }
        return super.onStartCommand(intent, flags, startId);
    }

    private void beginClearAppsCache() {
        PackageManager pm = getPackageManager();
        List<PackageInfo> list = pm
                .getInstalledPackages(PackageManager.GET_UNINSTALLED_PACKAGES);
        for (PackageInfo info : list) {
            pm.deleteApplicationCacheFiles(info.packageName, null);
        }
    }

    private void cleanDirectory(File dir, String p) {
        if (dir == null || !dir.isDirectory()) {
            return;
        }
        if (!dir.canRead()) {
            return;
        }
        if (APPLICATION_CACHE_PATTERN.equalsIgnoreCase(p)) {
            beginClearAppsCache();
            return;
        }
        String[] mode = p.split("/");

        if (mode.length > 1) {
            for (File f : dir.listFiles()) {
                if (wildcardMatch(mode[0].trim(), f.getName())
                        && f.isDirectory()) {
                    cleanDirectory(f, p.substring(mode[0].length() + 1));
                }
            }
        } else {
            for (File f : dir.listFiles()) {
                if (wildcardMatch(mode[0].trim(), f.getName())) {
                    f.delete();
                } else if (f.isDirectory() && p.contains("*")) {
                    cleanDirectory(f, p);
                }
            }
        }
    }

    private static boolean wildcardMatch(String pattern, String str) {
        int patternLength = pattern.length();
        int strLength = str.length();
        int strIndex = 0;
        char ch;
        for (int patternIndex = 0; patternIndex < patternLength; patternIndex++) {
            ch = pattern.charAt(patternIndex);
            if (ch == '*') {
                while (strIndex < strLength) {
                    if (wildcardMatch(pattern.substring(patternIndex + 1),
                            str.substring(strIndex))) {
                        return true;
                    }
                    strIndex++;
                }
            } else if (ch == '?') {
                strIndex++;
                if (strIndex > strLength) {
                    return false;
                }
            } else {
                if ((strIndex >= strLength) || (ch != str.charAt(strIndex))) {
                    return false;
                }
                strIndex++;
            }
        }
        return (strIndex == strLength);
    }

    private class CleanAsyncTask extends AsyncTask<Void, Void, Void> {

        private final Runnable mTask;

        public CleanAsyncTask(Runnable task) {
            mTask = task;
        }

        @Override
        protected Void doInBackground(Void... params) {
            mTask.run();
            return null;
        }

        @Override
        protected void onPostExecute(Void aVoid) {
            if (mCallBack != null) {
                try {
                    mCallBack.onCleanFinish();
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
