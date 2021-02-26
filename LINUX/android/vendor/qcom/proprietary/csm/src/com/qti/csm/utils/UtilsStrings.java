/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.utils;

import static com.qti.csm.encryption.system.SystemInfo.INTERNAL_STORAGE;
import static com.qti.csm.encryption.system.SystemInfo.INTERNAL_STORAGE_REAL;
import static com.qti.csm.encryption.system.SystemInfo.SDCARD_REAL;
import static com.qti.csm.encryption.system.SystemInfo.SDCARD_ROOT;
import static com.qti.csm.encryption.system.ProcessInfo.logd;
public class UtilsStrings {

    public static String getFileType(String fileName) {
        if (fileName == null)
            return "";
        int index = fileName.lastIndexOf(".");
        if (index < 0)
            return "";
        String ext = fileName.substring(index, fileName.length()).toLowerCase();
        return ext;
    }

    public static String getFileNameFromPath(String filePath) {
        String ret = "";
        int index = filePath.lastIndexOf("/");
        if (index < 0 || index >= filePath.length() - 1)
            return ret;
        ret = filePath.substring(index + 1);
        return ret;
    }

    public static String getFileNameWithoutPostfix(String filePath) {

        if (filePath == null)
            return "";
        int index = filePath.lastIndexOf(".");
        logd(index);
        if (index < 0 || index >= filePath.length() - 1)
            return filePath;
        return filePath.substring(0, index);
    }

    public static String getProtectedFilePath(String srcFilePath) {

        if (srcFilePath == null)
            return null;
        String dstFilePath = null;
        if (srcFilePath.startsWith(INTERNAL_STORAGE_REAL)) {
            dstFilePath = INTERNAL_STORAGE
                    + srcFilePath.substring(INTERNAL_STORAGE_REAL.length());
        } else if (srcFilePath.startsWith(SDCARD_REAL)) {
            dstFilePath = SDCARD_ROOT
                    + srcFilePath.substring(SDCARD_REAL.length());
        } else {
            return srcFilePath;
        }
        return dstFilePath;
    }
}
