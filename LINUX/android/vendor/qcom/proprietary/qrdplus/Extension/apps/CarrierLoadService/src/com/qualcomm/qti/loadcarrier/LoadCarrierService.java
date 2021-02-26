/**
 * Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.loadcarrier;

import java.io.File;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.HashMap;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;
import android.text.TextUtils;
import android.util.Log;

import com.android.internal.os.RegionalizationEnvironment;

public class LoadCarrierService extends Service {
    private static final String TAG = "LoadCarrierService";

    private static File CACHE_TEMP_DIR = new File("/cache/temp");

    public LoadCarrierService() {
    }

    public HashMap<String, String> getSdcardCarrierList() {
        // Get the sdcard path.
        String sdcardPath = Utils.getPath(this, Utils.FLAG_STORAGE_EXTERNAL);
        String secSdcardPath = Utils.getPath(this, Utils.FLAG_STORAGE_SECONDARY);

        if (Utils.DEBUG) Log.d(TAG, "The external storage directory: " + sdcardPath);
        if (Utils.DEBUG) Log.d(TAG, "The secondary storage directory: " + secSdcardPath);

        HashMap<String, String> carrierList = new HashMap<String, String>();
        if (!TextUtils.isEmpty(sdcardPath))
            Utils.getCarrierList(carrierList, sdcardPath);
        if (!TextUtils.isEmpty(secSdcardPath))
            Utils.getCarrierList(carrierList, secSdcardPath);

        return carrierList;
    }

    public String copyToData(String srcFilePath) {
        if (TextUtils.isEmpty(srcFilePath)) return null;

        // Check if the original file exist.
        File srcFile = new File(srcFilePath);
        if (srcFile == null || !srcFile.exists()) return null;

        String dstFilePath = null;

        if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
            try {
                Log.i(TAG,"Start unzip ota.zip file :" + srcFilePath);
                Utils.unzip(srcFilePath, CACHE_TEMP_DIR.getAbsolutePath() + "/");
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        } else {
            String secSdcardPath = Utils.getPath(this, Utils.FLAG_STORAGE_SECONDARY);
            Log.d(TAG, "srcFilePath="+srcFilePath+" secSdcardPath="+secSdcardPath);
            if (srcFilePath.startsWith(secSdcardPath)) {
                dstFilePath = "/sdcard/" + srcFile.getName();
            }
        }

        return dstFilePath;
    }

    public String downloadToData(String url) {
        String srcFilePath = Utils.downloadFile(this, url);
        return copyToData(srcFilePath);
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    private final IBinder mBinder = new ServiceStub(this);

    private static class ServiceStub extends ILoadCarrierService.Stub {
        WeakReference<LoadCarrierService> mService;

        ServiceStub(LoadCarrierService service) {
            mService = new WeakReference<LoadCarrierService>(service);
        }

        @Override
        public HashMap<String, String> getSdcardCarrierList() {
            return mService.get().getSdcardCarrierList();
        }

        @Override
        public String copyToData(String srcFilePath) {
            return mService.get().copyToData(srcFilePath);
        }

        @Override
        public String downloadToData(String url) {
            return mService.get().downloadToData(url);
        }
    }

}
