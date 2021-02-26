/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.permission.utils;

import android.annotation.SuppressLint;
import android.content.ComponentName;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.content.res.XmlResourceParser;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.INetworkManagementService;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;

import com.qti.csm.permission.bean.AppPermissionInfo;
import com.qti.csm.permission.bean.AppReceiverInfo;
import com.qti.csm.utils.L;

import org.xmlpull.v1.XmlPullParser;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.StringTokenizer;
import java.util.concurrent.atomic.AtomicInteger;

@SuppressLint("UseSparseArrays")
public class PermissionUtils {
    private static final String DATA_PREFS_NAME = "DataEnablePrefs";
    private static final String MMS_PREFS_NAME = "MmsDisablePrefs";
    private static final String PREF_CHECK_UIDS = "NotAllowedUids";
    private final static String SDK_RESOURCES = "http://schemas.android.com/apk/res/android";
    private final static Object mConn = new Object();

    private static enum ParserState {
        Unknown, InManifest, InApplication, InReceiver, InAction
    }

    private static PackageManager mPackageManager = null;
    public static List<PackageInfo> mAutoApps = new ArrayList<PackageInfo>(0);
    public static List<PackageInfo> mDataApps = new ArrayList<PackageInfo>(0);

    public static HashMap<Integer, AppPermissionInfo> mAppDataInfos = new HashMap<Integer, AppPermissionInfo>(0);
    public static HashMap<Integer, AppPermissionInfo> mAppMmsInfos = new HashMap<Integer, AppPermissionInfo>(0);
    public static HashMap<String, List<AppReceiverInfo>> mAutoReceivers = new HashMap<String, List<AppReceiverInfo>>();
    private static String mPackageName = null;
    private static ParserState mState = ParserState.Unknown;
    private static XmlResourceParser mXML;
    private static Resources mResources;
    private static AppReceiverInfo mReceiverInfo;
    private static LocalSocket mSocket;
    private static OutputStream mOutStream;
    private static InputStream mInStream;
    private static boolean mConnected = false;
    private static final int BUF_SIZE = 4096;

    /********************************
     * Data / Mms Control
     ***********************************/
    public static HashMap<Integer, AppPermissionInfo> getAppMaps(Context ctx,
                                                                 List<PackageInfo> packageInfos,
                                                                 String name) {
        String prefName = null;
        HashMap<Integer, AppPermissionInfo> mAppInfos = null;
        if (name.equals("data")) {
            prefName = DATA_PREFS_NAME;
            mAppInfos = mAppDataInfos;
        } else if (name.equals("mms")) {
            prefName = MMS_PREFS_NAME;
            mAppInfos = mAppMmsInfos;
        }
        final SharedPreferences prefs = ctx.getSharedPreferences(prefName, 0);
        final String checkUids = prefs.getString(PREF_CHECK_UIDS, "");
        int notAllow[] = new int[0];
        if (checkUids.length() > 0) {
            final StringTokenizer tok = new StringTokenizer(checkUids, "|");
            notAllow = new int[tok.countTokens()];
            for (int i = 0; i < notAllow.length; i++) {
                final String uid = tok.nextToken();
                if (!uid.equals("")) {
                    try {
                        notAllow[i] = Integer.parseInt(uid);
                    } catch (Exception ex) {
                        notAllow[i] = -1;
                    }
                }
            }
            Arrays.sort(notAllow);
        }
        try {
            AppPermissionInfo app = null;
            for (final PackageInfo packageInfo : packageInfos) {
                app = mAppInfos.get(packageInfo.applicationInfo.uid);
                if (app == null) {
                    app = new AppPermissionInfo();
                    app.uid = packageInfo.applicationInfo.uid;
                    app.appinfo = packageInfo.applicationInfo;
                    app.names = new String[] { packageInfo.packageName };
                    app.isAllow = true;
                    mAppInfos.put(packageInfo.applicationInfo.uid, app);
                } else {
                    final String newnames[] = new String[app.names.length + 1];
                    System.arraycopy(app.names, 0, newnames, 0, app.names.length);
                    newnames[app.names.length] = packageInfo.packageName;
                    app.names = newnames;
                }

                if (app.isAllow && Arrays.binarySearch(notAllow, app.uid) >= 0) {
                    app.isAllow = false;
                }
            }
            return mAppInfos;
        } catch (Exception e) {
            Log.i(L.TAG, "getApps:", e);
        }
        return null;
    }

    public static void getNewApkUid(Context ctx, String packageName) {
        final SharedPreferences prefs = ctx.getSharedPreferences(MMS_PREFS_NAME, 0);
        final String checkUids = prefs.getString(PREF_CHECK_UIDS, "");
        final StringBuilder saveUids = new StringBuilder();
        List<PackageInfo> mAllInstalledApps = new ArrayList<PackageInfo>(0);
        int newUid = 0;
        boolean isExist = false;
        mAllInstalledApps = ctx.getPackageManager().getInstalledPackages(0);
        for (PackageInfo packageInfo : mAllInstalledApps) {
            if (packageInfo.packageName.equals(packageName)) {
                Log.i(L.TAG, "newuid:" + packageInfo.applicationInfo.uid);
                newUid = packageInfo.applicationInfo.uid;
                break;
            }
        }

        if (checkUids.length() > 0) {
            final StringTokenizer tok = new StringTokenizer(checkUids, "|");
            while (tok.hasMoreTokens()) {
                final String uid = tok.nextToken();
                if (!uid.equals("")) {
                    saveUids.append(Integer.parseInt(uid));
                    saveUids.append('|');
                    if (Integer.parseInt(uid) == newUid) {
                        isExist = true;
                    }
                }
            }
        }
        if (!isExist) {
            saveUids.append(newUid);
            saveUids.append('|');
        }
        final Editor edit = prefs.edit();
        edit.putString(PREF_CHECK_UIDS, saveUids.toString());
        edit.commit();
        enableApps("enableMms", saveUids.toString());
    }

    public static void removeUid(Context ctx) {
        final SharedPreferences sharedPrefs = ctx.getSharedPreferences(MMS_PREFS_NAME, 0);
        final String curUids = sharedPrefs.getString(PREF_CHECK_UIDS, "");
        if (curUids.length() > 0) {
            List<PackageInfo> allApps = new ArrayList<PackageInfo>(0);
            allApps = ctx.getPackageManager().getInstalledPackages(0);
            int doUid = 0;
            boolean needUpdate = false;
            final StringTokenizer tok = new StringTokenizer(curUids, "|");
            final StringBuilder updateUids = new StringBuilder();
            while (tok.hasMoreTokens()) {
                boolean hasApp = false;
                final String uid = tok.nextToken();
                if (!uid.equals("")) {
                    doUid = Integer.parseInt(uid);
                    for (PackageInfo packageInfo : allApps) {
                        if (doUid == packageInfo.applicationInfo.uid) {
                            hasApp = true;
                            break;
                        }
                    }
                    if (hasApp) {
                        updateUids.append(doUid);
                        updateUids.append('|');
                    } else {
                        needUpdate = true;
                    }
                }
            }
            if (needUpdate) {
                final Editor edit = sharedPrefs.edit();
                edit.putString(PREF_CHECK_UIDS, updateUids.toString());
                edit.commit();
                enableApps("enableMms", updateUids.toString());
            }
        }
    }

    public static void initStat(Context ctx) {
        final SharedPreferences prefs = ctx.getSharedPreferences(MMS_PREFS_NAME, 0);
        final String uids = prefs.getString(PREF_CHECK_UIDS, "");
        if (uids.length() > 0) {
            enableApps("enableMms", uids);
        }
    }

    public static void saveNewApps(Context ctx, String name) {
        String uids = saveRules(ctx, name);
        if (name.equals("data")) {
            enableApps("enableData", uids);
        } else if (name.equals("mms")) {
            enableApps("enableMms", uids);
        }
    }

    private static String saveRules(Context ctx, String name) {
        String prefName = null;
        StringBuilder newuids = new StringBuilder();
        AppPermissionInfo[] apps = new AppPermissionInfo[0];
        if (name.equals("data")) {
            prefName = DATA_PREFS_NAME;
            apps = mAppDataInfos.values().toArray(new AppPermissionInfo[mAppDataInfos.size()]);
        } else if (name.equals("mms")) {
            prefName = MMS_PREFS_NAME;
            apps = mAppMmsInfos.values().toArray(new AppPermissionInfo[mAppMmsInfos.size()]);
        }
        final SharedPreferences prefs = ctx.getSharedPreferences(prefName, 0);
        final Editor edit = prefs.edit();
        // clear all old uids
        edit.putString(PREF_CHECK_UIDS, newuids.toString());
        edit.commit();
        for (int i = 0; i < apps.length; i++) {
            if (!apps[i].isAllow) {
                if (newuids.length() != 0)
                    newuids.append('|');
                newuids.append(apps[i].uid);
            }
        }
        newuids.append('|');
        edit.putString(PREF_CHECK_UIDS, newuids.toString());
        edit.commit();
        return newuids.toString();
    }

    private static void enableApps(String funName, String uids) {
        Log.i(L.DATA_TAG, "uid:" + uids);
        synchronized (mConn) {
            if (!mConnected) {
                connectToSocket();
            }
            String cmdString = makeCommand("bandwidth", funName, uids);
            byte[] buf = new byte[BUF_SIZE];
            try {
                mOutStream.write(cmdString.getBytes());
                mOutStream.flush();
                int length = mInStream.read(buf, 0, BUF_SIZE);
                if (length > 0) {
                    final String inMsg = new String(buf, 0, length, StandardCharsets.UTF_8);
                    Log.i(L.DATA_TAG, "input:" + inMsg);
                } else {
                    Log.e(L.DATA_TAG, "input length :" + length);
                }
            } catch (IOException ex) {
                Log.e(L.DATA_TAG, "exception when write to socket");
            } finally {
                if (mOutStream != null) {
                    try {
                        mOutStream.close();
                    } catch (IOException e) {
                        Log.e(L.DATA_TAG, "closing OutStream error: " + e);
                    }
                    mOutStream = null;
                    if (mInStream != null) {
                        try {
                            mInStream.close();
                        } catch (IOException e) {
                            Log.e(L.DATA_TAG, "closing InStream error: " + e);
                        }
                        mInStream = null;
                    }
                }
                closeSocket();
            }
        }
    }

    private static String makeCommand(String cmd, Object... args) {
        StringBuilder cmdBuilder = new StringBuilder();
        int sequenceNumber = new AtomicInteger(0).incrementAndGet();
        cmdBuilder.append(sequenceNumber).append(' ').append(cmd);
        for (Object arg : args) {
            final String argString = String.valueOf(arg);
            cmdBuilder.append(' ');
            cmdBuilder.append(argString);
        }
        cmdBuilder.append('\0');
        String cmdString = cmdBuilder.toString();
        Log.i(L.DATA_TAG, "cmd:" + cmdString);
        return cmdString;
    }

    private static boolean deamonEnabled() {
        boolean enabled = false;
        INetworkManagementService networkService = INetworkManagementService.Stub
                .asInterface(ServiceManager.getService(Context.NETWORKMANAGEMENT_SERVICE));
        try {
            if (networkService.isBandwidthControlEnabled()) {
                enabled = true;
            }
        } catch (RemoteException e) {
            Log.w(L.DATA_TAG, "No bandwidth control; leaving");
            enabled = false;
        }
        return enabled;
    }

    public static boolean connectToSocket() {
        if (mConnected)
            return true;
        boolean connected = false;

        try {
            mSocket = new LocalSocket();
            LocalSocketAddress address = new LocalSocketAddress("netd", LocalSocketAddress.Namespace.RESERVED);
            mSocket.connect(address);
            mInStream = mSocket.getInputStream();
            mOutStream = mSocket.getOutputStream();
            if (mOutStream != null) {
                mConnected = true;
                connected = true;
                if (!deamonEnabled()) {
                    String enable = makeCommand("bandwidth", "enable");
                    mOutStream.write(enable.getBytes());
                    mOutStream.flush();
                } else {
                    Log.d(L.DATA_TAG, "Native deamon enabled");
                }
            }
        } catch (IOException ex) {
            Log.d(L.DATA_TAG, "exception = " + ex.toString());
        }
        return connected;
    }

    public static void closeSocket() {
        if (mSocket == null) {
            return;
        }
        try {
            mSocket.close();
        } catch (IOException ex) {
            Log.e(L.DATA_TAG, "exception when close socket");
        }
        mConnected = false;
    }

    /********************************
     * Auto start
     ***********************************/

    public static HashMap<String, List<AppReceiverInfo>> getAutoAppsMaps() {
        return mAutoReceivers;
    }

    public static void loadReceivers(Context ctx, List<PackageInfo> packageInfos) {
        if (mAutoApps != null) {
            mAutoApps.clear();
        }
        mPackageManager = ctx.getPackageManager();
        for (final PackageInfo packageInfo : packageInfos) {

            List<AppReceiverInfo> autoInfos = new ArrayList<AppReceiverInfo>();

            parsePackage(packageInfo.packageName, ctx, autoInfos);
            if (autoInfos.size() != 0) {
                mAutoReceivers.put(packageInfo.packageName, autoInfos);
                mAutoApps.add(packageInfo);
            }
        }
    }

    private static void parsePackage(String packageName, Context ctx, List<AppReceiverInfo> autoInfos) {
        XmlResourceParser xml = null;
        Resources res = null;
        try {
            Context appContext = ctx.createPackageContext(packageName, 0);
            AssetManager assets = appContext.getAssets();
            xml = openManifest(appContext, assets);
            res = new Resources(assets, ctx.getResources().getDisplayMetrics(), null);
        } catch (Exception e) {
            Log.e(L.RECEIVER_TAG, "Error processing " + packageName, e);
        }

        if (xml == null)
            return;

        if (packageName.equals("android")) {
            return;
        }
        mPackageName = packageName;
        mXML = xml;
        mResources = res;

        try {
            String tagName = null;
            mState = ParserState.Unknown;
            int type = xml.getEventType();
            while (type != XmlPullParser.END_DOCUMENT) {
                switch (type) {
                    case XmlPullParser.START_TAG:
                        tagName = xml.getName();
                        if (tagName.equals("manifest")) {
                            startManifest();
                        } else if (tagName.equals("application")) {
                            startApplication();
                        } else if (tagName.equals("receiver"))
                            startReceiver(autoInfos);
                        else if (tagName.equals("action"))
                            startAction();
                        break;

                    case XmlPullParser.END_TAG:
                        tagName = xml.getName();
                        if (tagName.equals("manifest")) {
                            endManifest();
                        } else if (tagName.equals("application"))
                            endApplication();
                        else if (tagName.equals("receiver")) {
                            endReceiver();
                        } else if (tagName.equals("action"))
                            endAction();
                        break;
                }
                type = xml.nextToken();
            }
        } catch (Exception e) {
            Log.e(L.RECEIVER_TAG, "Unable to process manifest for " + packageName, e);
        } finally {
            mXML = null;
            mResources = null;
            mPackageName = null;
        }
        return;
    }

    private static XmlResourceParser openManifest(Context appContext, AssetManager assets) throws IOException {
        try {
            String path = (String) Context.class.getMethod("getPackageResourcePath").invoke(appContext);
            Method method = AssetManager.class.getMethod("getCookieName", int.class);
            if (path == null && appContext.getPackageName().equals("android")) {
                path = "/system/framework/framework-res.apk";
            }
            for (int i = 1; i < 20; i++) {
                if (path.equals(method.invoke(assets, i))) {
                    return assets.openXmlResourceParser(i, "AndroidManifest.xml");
                }
            }
        } catch (Exception e) {
        }
        return assets.openXmlResourceParser("AndroidManifest.xml");
    }

    private static void startManifest() {
        if (mState == ParserState.Unknown)
            mState = ParserState.InManifest;
    }

    private static void endManifest() {
        if (mState == ParserState.InManifest)
            mState = ParserState.Unknown;
    }

    private static void startApplication() {
        if (mState != ParserState.InManifest)
            return;
        mState = ParserState.InApplication;
    }

    private static void endApplication() {
        if (mState == ParserState.InApplication) {
            mState = ParserState.InManifest;
        }
    }

    private static void startReceiver(List<AppReceiverInfo> autoInfos) {
        if (mState != ParserState.InApplication)
            return;
        mState = ParserState.InReceiver;
        String componentName = getAttr("name");
        if (componentName == null) {
            Log.e(L.RECEIVER_TAG, "A receiver in " + mPackageName + " has no name.");
            componentName = "(no-name)";
        } else if (componentName.startsWith("."))
            componentName = mPackageName + componentName;
        else if (!componentName.contains("."))
            componentName = mPackageName + "." + componentName;

        mReceiverInfo = new AppReceiverInfo();
        mReceiverInfo.packageName = mPackageName;
        mReceiverInfo.componentName = componentName;
        mReceiverInfo.defaultEnabled = !(getAttr("enabled") == "false");
        mReceiverInfo.hasBoot = false;
        mReceiverInfo.state = mPackageManager
                .getComponentEnabledSetting(new ComponentName(mPackageName, componentName));
        autoInfos.add(mReceiverInfo);
    }

    private static void endReceiver() {
        if (mState == ParserState.InReceiver) {
            mReceiverInfo = null;
            mState = ParserState.InApplication;
        }
    }

    private static void startAction() {
        if (mState != ParserState.InReceiver)
            return;
        mState = ParserState.InAction;
        if (mReceiverInfo == null)
            return;

        String action = getAttr("name");
        if (action == null) {
            return;
        }
        if (action.indexOf("BOOT_COMPLETED") > 0) {
            mReceiverInfo.setBoot(true);
        }
    }

    private static void endAction() {
        if (mState == ParserState.InAction)
            mState = ParserState.InReceiver;
    }

    private static String getAttr(String name) {
        String value = mXML.getAttributeValue(SDK_RESOURCES, name);
        if (value == null) {
            for (int i = 0; i < mXML.getAttributeCount(); i++) {
                if (!mXML.getAttributeName(i).equals(""))
                    continue;

                int res = mXML.getAttributeNameResource(i);
                if (res != 0) {
                    String sName = mResources.getResourceEntryName(res);
                    if (sName.equals(name)) {
                        value = mXML.getAttributeValue(i);
                        break;
                    }
                }
            }
        }
        return resolveValue(value, mResources);
    }

    private static String resolveValue(String value, Resources resources) {
        if (value == null || !value.startsWith("@") || resources == null)
            return value;
        try {
            int num = Integer.parseInt(value.substring(1));
            return resources.getString(num);
        } catch (Exception e) {
            Log.w(L.RECEIVER_TAG, "Unable to resolve resource " + value, e);
            return value;
        }
    }
}
