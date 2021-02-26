/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.accesscache;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.text.TextUtils;
import android.util.Log;

import com.android.internal.os.RegionalizationEnvironment;
import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;

public class CarrierAccessCacheService extends Service {
    private static final String TAG = "CarrierAccessCacheService";

    private static final String CACHE_PARTITION = "/cache";
    private static File CACHE_RECOVERY_DIR = new File(CACHE_PARTITION + "/recovery");
    private static File CACHE_COMMAND_FILE = new File(CACHE_RECOVERY_DIR, "command");
    private static File CACHE_LOG_FILE = new File(CACHE_RECOVERY_DIR, "log");
    private static File CACHE_ACTION_FILE = new File(CACHE_PARTITION + "/action");
    private static File CACHE_SUBMASK_FILE = new File(CACHE_PARTITION + "/submask");
    private static File CACHE_TEMP_DIR = new File(CACHE_PARTITION + "/temp");

    public CarrierAccessCacheService() {
    }

    public boolean writeActionFile(String contents) {
        if (TextUtils.isEmpty(contents)) return false;

        try {
            CACHE_ACTION_FILE.delete();
            FileWriter actionFile = new FileWriter(CACHE_ACTION_FILE);
            try {
                actionFile.write(contents);
                actionFile.write("\n");
            } finally {
                actionFile.close();
            }
        } catch (IOException e) {
            Log.d(TAG, "write action file failed " + e);
            return false;
        }

        return true;
    }

    public boolean writeSubMaskFile(String contents) {
        if (TextUtils.isEmpty(contents)) return false;

        try {
            CACHE_SUBMASK_FILE.delete();
            FileWriter subMaskFile = new FileWriter(CACHE_SUBMASK_FILE);
            try {
                subMaskFile.write(contents);
                subMaskFile.write("\n");
            } finally {
                subMaskFile.close();
            }
        } catch (IOException e) {
            Log.d(TAG, "Write submask file failed. e= " + e);
            return false;
        }
        try {
            Runtime.getRuntime().exec("chmod 644 " + CACHE_SUBMASK_FILE);
        } catch (IOException e){
            Log.e(TAG, "failed chmod file:" + CACHE_SUBMASK_FILE + " e=" + e);
            return false;
        }

        return true;
    }

    public String readSubMaskFile() {
        if (!CACHE_SUBMASK_FILE.exists() || !CACHE_SUBMASK_FILE.canRead()) {
            if (Utils.DEBUG) Log.i(TAG, "CACHE_SUBMASK_FILE does not exist.");
            return null;
        }

        String contents = null;
        try {
            FileReader fr = new FileReader(CACHE_SUBMASK_FILE);
            BufferedReader br = new BufferedReader(fr);
            try {
                contents = br.readLine();
            } finally {
                if (br != null) br.close();
                if (fr != null) fr.close();
                CACHE_SUBMASK_FILE.delete();
            }
        } catch (IOException e) {
            Log.e(TAG, "Failed to read submask file");
            return null;
        }

        return contents;
    }

    public boolean writeCommandFile(String filePath, String locale, boolean wipedata) {

        try {
            CACHE_RECOVERY_DIR.mkdirs();
            CACHE_COMMAND_FILE.delete();
            CACHE_LOG_FILE.delete();

            FileWriter commandFile = new FileWriter(CACHE_COMMAND_FILE);
            try {
                if (!TextUtils.isEmpty(filePath)) {
                    commandFile.write(filePath);
                    commandFile.write("\n");
                }
                if (!TextUtils.isEmpty(locale)) {
                    commandFile.write(locale);
                    commandFile.write("\n");
                }
                if (wipedata) {
                    commandFile.write("--wipe_data");
                    commandFile.write("\n");
                }
            } finally {
                commandFile.close();
            }
        } catch (IOException e) {
            Log.d(TAG, "write CommandFile failed " + e);
            return false;
        }
        return true;
    }

    public String copyFile(String srcPath) {
        File srcFile = new File(srcPath);
        String dstPath = CACHE_PARTITION + "/" +  srcFile.getName();
        File dstFile = new File(dstPath);

        if (dstFile.exists()) return dstPath;

        FileInputStream fins = null;
        FileOutputStream fouts = null;
        try {
            fins = new FileInputStream(srcFile);
            fouts = new FileOutputStream(dstFile);
            int bytesRead = 0;
            byte[] buf = new byte[4 * 1024];
            while ((bytesRead = fins.read(buf)) != -1) {
                fouts.write(buf, 0, bytesRead);
            }
            fouts.flush();
        } catch (IOException ex) {
            ex.printStackTrace();
            return null;
        } finally {
            if (fins != null)
                try {
                    fins.close();
                } catch (IOException e) {}
            if (fouts != null)
                try {
                    fouts.close();
                } catch (IOException e) {}
        }
        return dstPath;
    }

    public HashMap<String, String> getCacheCarrierList() {
        if (Utils.DEBUG) Log.d(TAG, "Try to get Carriers from " + CACHE_PARTITION);

        HashMap<String, String> carrierList = new HashMap<String, String>();
        Utils.getCarrierList(carrierList, CACHE_PARTITION);

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
            String secSdcardPath = Utils.getSecSdcardPath(this);
            if (Utils.DEBUG) Log.d(TAG, "srcFilePath=" + srcFilePath
                    + " secSdcardPath="+ secSdcardPath);
            if ((!TextUtils.isEmpty(secSdcardPath)) && srcFilePath.startsWith(secSdcardPath)) {
                dstFilePath = Utils.SD_PATH_IN_RECOVERY + srcFile.getName();
            } else if (srcFilePath.startsWith(Utils.CACHE_VENDOR_PATH)) {
                dstFilePath = srcFilePath;
            }
        }

        return dstFilePath;
    }

    @SuppressWarnings("unchecked")
    public int getMBNFileDataFromOta(String srcFilePath, String fileName, byte[] fileData,
            int fileMaxSize) {
        int dataSize = 0;

        Log.d(TAG, "srcFilePath=" + srcFilePath + " fileName=" + fileName);
        JarFile jarFile = null;
        try {
            jarFile = new JarFile(srcFilePath);
            Enumeration entries = jarFile.entries();
            JarEntry jarEntry = null;
            while (entries.hasMoreElements()) {
                jarEntry = (JarEntry) entries.nextElement();
                String entryName = jarEntry.getName();
                Log.d(TAG, "entryName="+entryName);
                if (!entryName.endsWith(fileName)) {
                    continue;
                }
                InputStream in = null;
                try {
                    in = jarFile.getInputStream(jarEntry);
                    int byteCount;
                    byte[] by = new byte[1024];

                    while ((byteCount = in.read(by)) != -1) {
                        if (dataSize + byteCount > fileMaxSize) {
                            Log.e(TAG, "MBN file is too large. MaxSize=" + fileMaxSize);
                            dataSize = 0;
                            break;
                        }
                        for (int i=0; i<byteCount; i++) {
                            fileData[dataSize] = by[i];
                            dataSize++;
                        }
                    }
                } catch (IOException ex) {
                    ex.printStackTrace();
                } finally {
                    if (in != null)
                        try {
                            in.close();
                        } catch (IOException e) {}
                }
                break;
            }
        } catch (IOException ex) {
            ex.printStackTrace();
        } finally {
            if (jarFile != null) {
                try {
                    jarFile.close();
                } catch (IOException e) {}
            }
        }
        return dataSize;
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

    private static class ServiceStub extends ICarrierAccessCacheService.Stub {
        WeakReference<CarrierAccessCacheService> mService;

        ServiceStub(CarrierAccessCacheService service) {
            mService = new WeakReference<CarrierAccessCacheService>(service);
        }

        @Override
        public boolean writeActionFile(String contents) {
            return mService.get().writeActionFile(contents);
        }

        @Override
        public boolean writeSubMaskFile(String contents) {
            return mService.get().writeSubMaskFile(contents);
        }

        @Override
        public String readSubMaskFile() {
            return mService.get().readSubMaskFile();
        }

        @Override
        public boolean writeCommandFile(String filepathStr, String localeStr, boolean wipedata) {
            return mService.get().writeCommandFile(filepathStr, localeStr, wipedata);
        }

        @Override
        public String copyFile(String srcPath) {
            return mService.get().copyFile(srcPath);
        }

        @Override
        public HashMap<String, String> getCacheCarrierList() {
            return mService.get().getCacheCarrierList();
        }

        @Override
        public String copyToData(String srcFilePath) {
            return mService.get().copyToData(srcFilePath);
        }

        @Override
        public int getMBNFileDataFromOta(String srcFilePath, String fileName, byte[] fileData,
                int fileMaxSize) {
            return mService.get().getMBNFileDataFromOta(srcFilePath, fileName, fileData,
                    fileMaxSize);
        }
    }

}
