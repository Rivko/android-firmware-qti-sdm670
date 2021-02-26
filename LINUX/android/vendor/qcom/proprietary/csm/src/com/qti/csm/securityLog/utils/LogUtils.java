/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.securityLog.utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

import com.qti.csm.securityLog.bean.AppLogInfo;
import com.qti.csm.securityLog.bean.AppLogInfo.ActionInfo;
import com.qti.csm.securityLog.bean.AppLogInfo.PackageAction;
import com.qti.csm.utils.L;
import com.qti.csm.utils.Utils;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.os.Environment;
import android.util.Log;

@SuppressLint("UseSparseArrays")
public class LogUtils {
    private static final String DEFAULT_PATH = Environment.getExternalStorageDirectory().toString();
    private static final String LOG_TAG = "CTA_LOG";
    private static final String LOG_ACTION_TAG = "action:";

    public static List<PackageInfo> mLogApps = new ArrayList<PackageInfo>(0);
    public static String mLogs = null;

    /********************************
     * Parse Log
     ***********************************/
    public static List<PackageInfo> getLogApps(Context ctx) {
        if (mLogApps != null) {
            mLogApps.clear();
        }
        mLogs = parseLogFile();
        for (PackageInfo packageInfo : Utils.getAllApps(ctx)) {
            if (mLogs.indexOf(packageInfo.applicationInfo.loadLabel(ctx.getPackageManager()).toString()) > 0) {
                mLogApps.add(packageInfo);
            }
        }
        return mLogApps;
    }

    public static String getLogs() {
        return mLogs;
    }

    public static String parseLogFile() {
        String res = "";
        Log.i(L.LOG_TAG, "filepath:" + DEFAULT_PATH);
        File file = new File(DEFAULT_PATH, "log.txt");
        try {
            InputStream instream = new FileInputStream(file);
            if (instream != null) {
                InputStreamReader inputreader = new InputStreamReader(instream);
                BufferedReader buffreader = new BufferedReader(inputreader);
                String line;
                // 分行读取
                while ((line = buffreader.readLine()) != null) {
                    if ((line.indexOf(LOG_TAG) > 0)) {
                        res += line + "\n";
                    }
                }
                instream.close();
            }
            Log.i(L.LOG_TAG, "log res: " + res);
        } catch (Exception e) {
            Log.i(L.LOG_TAG, "parseLogFile:", e);
        }
        return res;
    }

    public static List<ActionInfo> parseCtaLog(String packageName, String logs) {
        List<ActionInfo> listItem = new ArrayList<ActionInfo>(0);

        String[] lines = logs.split("\n");
        for (String line : lines) {
            if (line.indexOf(packageName) > 0) {
                String action = line.substring(line.lastIndexOf(LOG_ACTION_TAG) + 7);
                String time = line.substring(0, 11);
                ActionInfo item = new ActionInfo(action, time);
                listItem.add(item);
            }
        }
        PackageAction packageAction = new PackageAction();
        packageAction.setName(packageName);
        packageAction.setItems(listItem);
        AppLogInfo.addGroup(packageAction);
        return listItem;
    }
}
