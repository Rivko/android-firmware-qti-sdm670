/* Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.storagecleaner;

import android.app.Service;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.IBinder;
import android.os.RemoteException;
import android.text.TextUtils;
import android.util.Log;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;

public class StorageCleanerService extends Service {
    private static final boolean DEBUG = true;
    private static final String TAG = "StorageCleanerService";
    private ICleanerCallBack mCallBack;
    static private ArrayList<String> ignoreDirectories = new ArrayList<String>();

    private static final String INVALID_APK_PATTERN = "Invalid APK";

    static {
        ignoreDirectories.add(".android_secure");
    }

    public StorageCleanerService() {
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
        private WeakReference<StorageCleanerService> mService;

        public ServiceStub(StorageCleanerService service) {
            mService = new WeakReference<StorageCleanerService>(service);
        }

        @Override
        public void startCleaner(String path, String pattern) throws RemoteException {
            String[] patten = pattern.split(",");
            mService.get().startCleaner(path, patten);
        }

        @Override
        public void registCleanerCallBack(ICleanerCallBack callback) throws RemoteException {
            mCallBack = callback;
        }
    }

    private void startCleaner(String path, String[] patten) {
        if (TextUtils.isEmpty(path)) {
            return;
        }
        File dir = new File(path);
        if (!dir.isDirectory()) {
            return;
        }
        new CleanAsyncTask(dir, patten).execute();
    }


    private void cleanDirectory(File dir, String p) {
        if (dir == null || !dir.isDirectory()) {
            return;
        }
        if (!dir.canRead()) {
            return;
        }
        if (ignoreDirectories.contains(dir.getName())) {
            return;
        }
        if (INVALID_APK_PATTERN.equalsIgnoreCase(p)) {
            cleanInvalidApk(dir);
            return;
        }
        String[] mode = p.split("/");

        if (mode.length > 1) {
            for (File f : dir.listFiles()) {
                if (wildcardMatch(mode[0].trim(), f.getName()) && f.isDirectory()) {
                    cleanDirectory(f, p.substring(mode[0].length() + 1));
                }
            }
        } else {
            for (File f : dir.listFiles()) {
                if (wildcardMatch(mode[0].trim(), f.getName())) {
                    log("matched, delete: " + f);
                    f.delete();
                } else if (f.isDirectory() && p.contains("*")) {
                    cleanDirectory(f, p);
                }
            }
        }
    }

    private void cleanInvalidApk(File dir) {
        if (dir == null || !dir.isDirectory()) {
            return;
        }
        if (!dir.canRead()) {
            return;
        }
        if (ignoreDirectories.contains(dir.getName())) {
            return;
        }
        for (File f : dir.listFiles()) {
            if (f.getName().toLowerCase().endsWith(".apk")) {
                PackageManager pm = getPackageManager();
                PackageInfo info = pm.getPackageArchiveInfo(f.getAbsolutePath(),
                        PackageManager.GET_ACTIVITIES);
                if (info == null) {
                    log("delete invalid apk: " + f);
                    f.delete();
                }
            } else if (f.isDirectory()) {
                cleanInvalidApk(f);
            }
        }
    }

    private void log(String s) {
        if (DEBUG) {
            Log.d(TAG, s);
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

        private final File mPath;
        private final String[] mPattern;

        public CleanAsyncTask(File path, String[] pattern) {
            mPath = path;
            mPattern = pattern;
        }

        @Override
        protected Void doInBackground(Void... params) {
            for (String p : mPattern) {
                cleanDirectory(mPath, p);
            }
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
