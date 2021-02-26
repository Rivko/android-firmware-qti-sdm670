/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.utils;

import android.content.res.AssetFileDescriptor;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class FileUtils {

    public static final int BUFFER_SIZE = 1024 * 10;//10K
    public static final int TRACE_FILE_BUFFER_SIZE = 1024 * 1024;//1M

    public static boolean writeFile(String filePath, String content) {
        boolean res = true;
        File file = new File(filePath);
        File dir = new File(file.getParent());
        if (!dir.exists())
            dir.mkdirs();
        try {
            if (!file.exists()) {
                file.createNewFile();
            } else {
                file.delete();
                file.createNewFile();
            }
            FileWriter mFileWriter = new FileWriter(file, false);
            mFileWriter.write(content);
            mFileWriter.close();
        } catch (IOException e) {
            res = false;
        }
        return res;
    }

    public static String readFile(String filePath) {
        String res = "";
        File file = new File(filePath);
        if (!file.exists())
            return res;

        try {
            char[] buf = new char[1024];
            int count = 0;
            FileReader fileReader = new FileReader(file);
            while ((count = fileReader.read(buf)) > 0) {
                res += new String(buf, 0, count);
            }
            fileReader.close();

        } catch (IOException e) {
            res = "";
        }
        return res;
    }

    public static boolean isFileExist(String filePath) {
        try {
            File f = new File(filePath);
            if (!f.exists()) {
                return false;
            }
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    //copy the raw file to dest
    public static void copy(AssetFileDescriptor fd, String destFile) {

        if (isFileExist(destFile)) {
            LogUtils.logi("already exist, skiped:" + destFile);
            return;
        }

        FileInputStream in = null;
        if (fd != null) {
            try {
                in = fd.createInputStream();
                copy(in, destFile);
            } catch (Exception e) {
                e.printStackTrace();
                LogUtils.loge("copy sound file failed");
            } finally {
                try {
                    if (in != null)
                        in.close();

                    if (fd != null)
                        fd.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        } else {
            LogUtils.loge("can't find the file");
        }
    }

    public static long copy(FileInputStream in, String destFilePath) throws Exception {
        if (in == null || destFilePath == null)
            return -1;

        File destFile = new File(destFilePath);
        if (!destFile.exists()) {
            destFile.createNewFile();
            System.out.print("create new file");
        }

        FileOutputStream fout = new FileOutputStream(destFile);
        long size = copy(in, fout);
        fout.close();

        return size;
    }

    private static long copy(FileInputStream fin, FileOutputStream fout) throws Exception {

        int totalLen = 0;
        try {
            BufferedInputStream bin = null;
            BufferedOutputStream bout = null;
            try {
                bin = new BufferedInputStream(fin,
                        BUFFER_SIZE);
                bout = new BufferedOutputStream(fout,
                        BUFFER_SIZE);
                byte[] buffer = new byte[BUFFER_SIZE];
                int len = 0;
                while ((len = bin.read(buffer)) > 0) {
                    bout.write(buffer, 0, len);
                    totalLen += len;
                }
            } finally {
                if (null != bin) {
                    bin.close();
                }
                if (null != bout) {
                    bout.close();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return totalLen;
    }

}
