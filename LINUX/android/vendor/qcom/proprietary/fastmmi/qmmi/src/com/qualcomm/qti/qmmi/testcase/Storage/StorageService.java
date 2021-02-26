/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Storage;

import android.content.Intent;
import android.os.Environment;
import android.os.StatFs;
import android.os.storage.StorageManager;
import android.os.storage.StorageVolume;
import android.text.format.Formatter;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.NumericUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import java.io.File;
import java.util.List;

public class StorageService extends BaseService {
    private StorageManager mStorageManager = null;
    private String mSdDirectory;
    private static final int VOLUME_SDCARD_INDEX = 1;


    /**
     * Get the raw sdcard total data.
     * @return
     */
    private long getSdcardTotalData(){
        File path = new File(getSdDirectory());
        LogUtils.logi(path.getAbsolutePath());
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSizeLong();
        long totalBlocks = stat.getBlockCountLong();
        return blockSize * totalBlocks;
    }

    /**
     * Get the formatted sdcard total data.
     *
     * @return
     */
    private String getSdcardTotalSize() {
        long size = getSdcardTotalData();
        return formatData(size);
    }

    /**
     * Get the raw sdcard available data.
     * @return
     */
    private long getSdcardAvailableData(){
        File path = new File(getSdDirectory());
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSizeLong();
        long availableBlocks = stat.getAvailableBlocksLong();
        return blockSize * availableBlocks;
    }

    /**
     * Get the formatted sdcard available data.
     *
     * @return
     */
    private String getSdcardAvailableSize() {
        long size = getSdcardAvailableData();
        return formatData(size);
    }

    /**
     * Get the raw ROM Total data.
     *
     * @return
     */
    private long getRomTotalData(){
        File path = Environment.getDataDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSizeLong();
        long totalBlocks = stat.getBlockCountLong();
        return blockSize * totalBlocks;
    }

    /**
     * Get the formatted ROM Total data.
     *
     * @return
     */
    private String getRomTotalSize() {
        long size = getRomTotalData();
        return formatData(size);
    }

    /**
     * Get the raw ROM available data.
     *
     * @return
     */
    private long getRomAvailableData(){
        File path = Environment.getDataDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSizeLong();
        long availableBlocks = stat.getAvailableBlocksLong();
        return blockSize * availableBlocks;
    }

    /**
     * Get the formatted ROM available data.
     *
     * @return
     */
    private String getRomAvailableSize() {
        long size = getRomAvailableData();
        return formatData(size);
    }

    private String formatData(long size){
        return Formatter.formatFileSize(this.getApplicationContext(), size);
    }

    private String getSdDirectory() {
        if (mSdDirectory == null) {
            try {
                final List<StorageVolume> volumes = mStorageManager.getStorageVolumes();
                if (volumes.size() > VOLUME_SDCARD_INDEX) {
                    StorageVolume volume = volumes.get(VOLUME_SDCARD_INDEX);
                    if (volume.isRemovable()) {
                        mSdDirectory = volume.getPath();
                    }
                }
            } catch (Exception e) {
                LogUtils.loge("couldn't talk to MountService" + e);
            }
        }
        return mSdDirectory;
    }

    private String getSdState() {

        String sdPath = getSdDirectory();

        if (sdPath != null) {
            final StorageVolume vol = mStorageManager.getStorageVolume(new File(sdPath));
            if (vol != null) {
                return vol.getState();
            } else {
                return Environment.MEDIA_UNKNOWN;
            }
        }
        return Environment.MEDIA_UNMOUNTED;
    }

    public boolean isSdMounted() {
        return getSdState().equals(Environment.MEDIA_MOUNTED);
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi("onStartCommand");
        mStorageManager =
                (StorageManager) getSystemService(STORAGE_SERVICE);
        if (mStorageManager == null) {
            LogUtils.loge("No mStorageManager service here");
        }
        return super.onStartCommand(intent, flags, startId);
    }


    @Override
    public void register() {

    }

    @Override
    public int stop(TestCase testCase) {

        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        LogUtils.logi( "StorageService service run:" + isSdMounted());
        StringBuffer sb = new StringBuffer();
        String type = testCase.getParameter().get("type");
        if(type.equals("sdcard")) {
            if (isSdMounted()) {
                LogUtils.logi("sdcard mounted, read the info");

                String sdcardTotalSize = getSdcardTotalSize();
                String sdcardAvailSize = getSdcardAvailableSize();

                sb.append(this.getResources().getString(R.string.sdcard_total))
                    .append(sdcardTotalSize).append("\n")
                    .append(this.getResources().getString(R.string.sdcard_avai))
                    .append(sdcardAvailSize).append("\n");

                testCase.addTestData("Sdcard Total Size", sdcardTotalSize);
                testCase.addTestData("Sdcard Available Size", sdcardAvailSize);

                boolean sdcardTotalState = checkState(testCase, getSdcardTotalData());
                boolean sdcardAvailState = checkState(testCase, getSdcardAvailableData());

                if(sdcardTotalState && sdcardAvailState){
                    updateResultForCase(testCase.getName(), TestCase.STATE_PASS);
                }else{
                    updateResultForCase(testCase.getName(), TestCase.STATE_FAIL);
                }

            } else {
                sb.append(this.getResources().getString(R.string.sdcard_not_mount));
                testCase.addTestData("Sdcard", "not detected");
                updateResultForCase(testCase.getName(), TestCase.STATE_FAIL);
            }
        }else if(type.equals("storage")){

            String romTotalSize = getRomTotalSize();
            String romAvailSize = getRomAvailableSize();

            sb.append(this.getResources().getString(R.string.internal_total))
            .append(romTotalSize).append("\n")
            .append(this.getResources().getString(R.string.internal_avai))
            .append(romAvailSize).append("\n");

            testCase.addTestData("Internal Total Size", romTotalSize);
            testCase.addTestData("Internal Available Size", romAvailSize);

            boolean romTotalState = checkState(testCase, getRomTotalData());
            boolean romAvailState = checkState(testCase, getRomAvailableData());

            if(romTotalState && romAvailState){
                updateResultForCase(testCase.getName(), TestCase.STATE_PASS);
            }else{
                updateResultForCase(testCase.getName(), TestCase.STATE_FAIL);
            }
        }
        updateView(testCase.getName(), sb.toString());
        return 0;
    }

    private boolean checkState(TestCase testCase, long size){
        float data = (float)(size)/Utils.GIGABYTE_BIT;
        String formattedData = NumericUtils.formatDecimals(data, 2);
        float twoDecimalData = Float.valueOf(formattedData);
        LogUtils.logi("Check size: " + twoDecimalData + " GB");
        return testCase.checkLimit(twoDecimalData, "value");
    }
}
