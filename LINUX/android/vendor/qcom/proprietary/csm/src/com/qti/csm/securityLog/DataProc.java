/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.securityLog;

import android.app.AppGlobals;
import android.app.AppOpsManager;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.IPackageManager;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;
import android.os.Environment;
import android.os.Process;
import android.os.RemoteException;
import android.os.UserHandle;
import android.os.UserManager;
import android.util.ArrayMap;
import android.util.AtomicFile;
import android.util.Log;
import android.util.SparseArray;
import android.util.Xml;
import android.view.Gravity;

import com.android.internal.util.FastXmlSerializer;
import com.android.internal.util.XmlUtils;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlSerializer;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

class DataProc {
    private final Context mContext;
    private static final String ANDROID_ID = "android";
    private static final long LOG_CHECKING = 1000L * (31 * 24 * 60 * 60);
    private static final int REC_NUM = 20;

    private static final String LOG = "Data";
    private final AtomicFile mFile;
    private static final String RECORD = "req.xml";
    static boolean mWrite = false;
    static boolean mInit = false;
    static int mSet = 0;

    private final static SparseArray<data> mRec = new SparseArray<>();

    private static final int[] CHECK_OPS = new int[]{
            AppOpsManager.OP_CALL_PHONE,
            AppOpsManager.OP_SEND_SMS,
    };

    public DataProc(Context context) {
        mContext = context;
        File dataDir = Environment.getDataDirectory();
        File systemDir = new File(dataDir, "system");
        systemDir.mkdirs();
        mFile = new AtomicFile(new File(systemDir, RECORD));
    }

    void updateRec(Context context) {
        if (!mInit) {
            readRec();
            mInit = true;
        }
        getUpdate();
        if (mWrite) {
            writeRec();
            mWrite = false;
        }
    }

    void writeRec() {
        Log.i(LOG, "write");
        synchronized (mFile) {
            FileOutputStream stream;
            try {
                stream = mFile.startWrite();
            } catch (IOException e) {
                Log.w(LOG, "Write fail : " + e);
                return;
            }
            try {
                XmlSerializer out = new FastXmlSerializer();
                out.setOutput(stream, StandardCharsets.UTF_8.name());
                out.startDocument(null, true);
                out.startTag(null, "qc-req");

                final int recCount = mRec.size();
                for (int i = 0; i < recCount; i++) {
                    data pkg = mRec.valueAt(i);
                    out.startTag(null, "pkg");
                    out.attribute(null, "u", Integer.toString(pkg.mUid));
                    out.attribute(null, "n", pkg.mPackageName);
                    if (pkg.callList != null && pkg.callList.size() > 0) {
                        out.startTag(null, "ca");
                        final int Count = pkg.callList.size();
                        for (int j = 0; j < Count; j++) {
                            long time = pkg.callList.get(j);
                            if (time != 0) {
                                out.startTag(null, "rec");
                                out.attribute(null, "t", Long.toString(time));
                                out.endTag(null, "rec");
                            }
                        }
                        out.endTag(null, "ca");
                    }
                    if (pkg.smsList != null && pkg.smsList.size() > 0) {
                        out.startTag(null, "cs");
                        final int Count = pkg.smsList.size();
                        for (int j = 0; j < Count; j++) {
                            long time = pkg.smsList.get(j);
                            if (time != 0) {
                                out.startTag(null, "rec");
                                out.attribute(null, "t", Long.toString(time));
                                out.endTag(null, "rec");
                            }
                        }
                        out.endTag(null, "cs");
                    }
                    out.endTag(null, "pkg");
                }

                out.endTag(null, "qc-req");
                out.endDocument();
                mFile.finishWrite(stream);
            } catch (IOException e) {
                Log.w(LOG, "Failed to write. ", e);
                mFile.failWrite(stream);
            }
        }
    }

    void readRec() {
        Log.i(LOG, "read");
        synchronized (mFile) {
            synchronized (this) {
                FileInputStream fileStr;
                try {
                    fileStr = mFile.openRead();
                } catch (FileNotFoundException e) {
                    Log.i(LOG, "file ERR");
                    return;
                }
                mRec.clear();
                boolean ready = false;
                try {
                    XmlPullParser xmlread = Xml.newPullParser();
                    xmlread.setInput(fileStr, StandardCharsets.UTF_8.name());
                    int tagInfo;
                    while ((tagInfo = xmlread.next()) != XmlPullParser.START_TAG
                            && tagInfo != XmlPullParser.END_DOCUMENT) {
                    }
                    if (tagInfo != XmlPullParser.START_TAG) {
                        throw new IllegalStateException("start error");
                    }
                    int readLength = xmlread.getDepth();
                    while ((tagInfo = xmlread.next()) != XmlPullParser.END_DOCUMENT
                            && (tagInfo != XmlPullParser.END_TAG || xmlread.getDepth() > readLength)) {
                        if (tagInfo == XmlPullParser.END_TAG || tagInfo == XmlPullParser.TEXT) {
                            continue;
                        }
                        String tagName = xmlread.getName();
                        if (tagName.equals("pkg")) {
                            pkgInfo(xmlread);
                        } else {
                            Log.w(LOG, "pkg wrong: " + xmlread.getName());
                            XmlUtils.skipCurrentTag(xmlread);
                        }
                    }
                    ready = true;
                } catch (IllegalStateException e) {
                    Log.w(LOG, "Parsing error: " + e);
                } catch (NullPointerException e) {
                    Log.w(LOG, "Parsing error: " + e);
                } catch (NumberFormatException e) {
                    Log.w(LOG, "Parsing error: " + e);
                } catch (XmlPullParserException e) {
                    Log.w(LOG, "Parsing error: " + e);
                } catch (IOException e) {
                    Log.w(LOG, "Parsing error: " + e);
                } catch (IndexOutOfBoundsException e) {
                    Log.w(LOG, "Parsing error: " + e);
                } finally {
                    if (!ready) {
                        mRec.clear();
                    }
                    try {
                        fileStr.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    private void pkgInfo(XmlPullParser parser) throws NumberFormatException,
            XmlPullParserException, IOException {
        String pkgName = parser.getAttributeValue(null, "n");
        int uid = Integer.parseInt(parser.getAttributeValue(null, "u"));
        int outerDepth = parser.getDepth();
        int type;
        while ((type = parser.next()) != XmlPullParser.END_DOCUMENT
                && (type != XmlPullParser.END_TAG || parser.getDepth() > outerDepth)) {
            if (type == XmlPullParser.END_TAG || type == XmlPullParser.TEXT) {
                continue;
            }
            String tagName = parser.getName();
            switch (tagName) {
                case "ca":
                    caInfo(parser, pkgName, uid);
                    break;
                case "cs":
                    csInfo(parser, pkgName, uid);
                    break;
                default:
                    Log.w(LOG, "<pkg> err element: "
                            + parser.getName());
                    XmlUtils.skipCurrentTag(parser);
                    break;
            }
        }
    }

    private void caInfo(XmlPullParser parser, String pkgName, int uid) throws NumberFormatException,
            XmlPullParserException, IOException {
        int outerDepth = parser.getDepth();
        int type;
        while ((type = parser.next()) != XmlPullParser.END_DOCUMENT
                && (type != XmlPullParser.END_TAG || parser.getDepth() > outerDepth)) {
            if (type == XmlPullParser.END_TAG || type == XmlPullParser.TEXT) {
                continue;
            }
            String tagName = parser.getName();
            if (tagName.equals("rec")) {
                String time = parser.getAttributeValue(null, "t");
                if (time != null) {
                    long tl = Long.parseLong(time);
                    data pkg = mRec.get(uid);
                    if (pkg == null) {
                        ArrayList<Long> pkgP = new ArrayList<>();
                        ArrayList<Long> pkgS = new ArrayList<>();
                        pkg = new data(pkgName, uid, pkgP, pkgS);
                        mRec.put(uid, pkg);
                    }
                    int len = pkg.callList.size();
                    if (len == 0) {
                        pkg.callList.add(tl);
                    } else if (pkg.callList.get(len - 1) < tl) {
                        pkg.callList.add(tl);
                    }
                }
            } else {
                Log.w(LOG, "Error ca: "
                        + parser.getName());
                XmlUtils.skipCurrentTag(parser);
            }
        }
    }

    private void csInfo(XmlPullParser parser, String pkgName, int uid) throws NumberFormatException,
            XmlPullParserException, IOException {
        int outerDepth = parser.getDepth();
        int type;
        while ((type = parser.next()) != XmlPullParser.END_DOCUMENT
                && (type != XmlPullParser.END_TAG || parser.getDepth() > outerDepth)) {
            if (type == XmlPullParser.END_TAG || type == XmlPullParser.TEXT) {
                continue;
            }
            String tagName = parser.getName();
            if (tagName.equals("rec")) {
                String time = parser.getAttributeValue(null, "t");
                if (time != null) {
                    long tl = Long.parseLong(time);
                    data pkg = mRec.get(uid);
                    if (pkg == null) {
                        ArrayList<Long> pkgP = new ArrayList<>();
                        ArrayList<Long> pkgS = new ArrayList<>();
                        pkg = new data(pkgName, uid, pkgP, pkgS);
                        mRec.put(uid, pkg);
                    }
                    int len = pkg.smsList.size();
                    if (len == 0) {
                        pkg.smsList.add(tl);
                    } else if (pkg.smsList.get(len - 1) < tl) {
                        pkg.smsList.add(tl);
                    }
                }
            } else {
                Log.w(LOG, "Error ca: "
                        + parser.getName());
                XmlUtils.skipCurrentTag(parser);
            }
        }
    }

    public ArrayList<String> getReqPh(Context context, int detail) {
        updateRec(context);
        ArrayList<String> requests = new ArrayList<>();
        synchronized (this) {
            SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy-MM-dd HH-mm-ss");
            data pkg = mRec.get(detail);
            int len = pkg.callList.size();
            if (len > 0) {
                for (int i = len - 1; i >= 0; i--) {
                    requests.add(simpleDateFormat.format(new Date(pkg.callList.get(i))));
                }
            }
        }
        return requests;
    }

    public ArrayList<String> getReqSm(Context context, int detail) {
        updateRec(context);
        ArrayList<String> requests = new ArrayList<>();
        synchronized (this) {
            SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy-MM-dd HH-mm-ss");
            data pkg = mRec.get(detail);
            int len = pkg.smsList.size();
            if (len > 0) {
                for (int i = len - 1; i >= 0; i--) {
                    requests.add(simpleDateFormat.format(new Date(pkg.smsList.get(i))));
                }
            }
        }
        return requests;
    }

    public void getUpdate() {
        Log.i(LOG, "update");
        synchronized (this) {
            AppOpsManager myManager =
                    (AppOpsManager) mContext.getSystemService(Context.APP_OPS_SERVICE);
            List<AppOpsManager.PackageOps> appOps = myManager.getPackagesForOps(CHECK_OPS);
            boolean checking = false;
            final int getCount = appOps != null ? appOps.size() : 0;
            final long now = System.currentTimeMillis();
            final UserManager um = (UserManager) mContext.getSystemService(Context.USER_SERVICE);
            final List<UserHandle> profiles = um.getUserProfiles();
            for (int i = 0; i < getCount; ++i) {
                AppOpsManager.PackageOps ops = appOps.get(i);
                String packageName = ops.getPackageName();
                int uid = ops.getUid();
                int userId = UserHandle.getUserId(uid);
                boolean isSys =
                        (uid == Process.SYSTEM_UID) && ANDROID_ID.equals(packageName);
                if (isSys || !profiles.contains(new UserHandle(userId))) {
                    continue;
                }
                data pkgo = mRec.get(uid);
                if (pkgo == null) {
                    ArrayList<Long> pkgP = new ArrayList<>();
                    ArrayList<Long> pkgS = new ArrayList<>();
                    pkgo = new data(packageName, uid, pkgP, pkgS);
                    mRec.put(uid, pkgo);
                    checking = true;
                }
                List<AppOpsManager.OpEntry> entries = ops.getOps();
                long ent = 1L;
                long recentCutoffTime = now - LOG_CHECKING;
                for (AppOpsManager.OpEntry entry : entries) {
                    if (entry.isRunning() || entry.getTime() >= recentCutoffTime) {
                        if (entry.getTime() > ent) {
                            ent = entry.getTime();
                        }
                        switch (entry.getOp()) {
                            case AppOpsManager.OP_CALL_PHONE:
                                int lenC = pkgo.callList.size();
                                if (lenC == 0) {
                                    pkgo.callList.add(ent);
                                    checking = true;
                                } else if (pkgo.callList.get(lenC - 1) < ent) {
                                    pkgo.callList.add(ent);
                                    if (pkgo.callList.size() > REC_NUM) {
                                        pkgo.callList.remove(0);
                                    }
                                    checking = true;
                                }
                                break;
                            case AppOpsManager.OP_SEND_SMS:
                                int lenS = pkgo.smsList.size();
                                if (lenS == 0) {
                                    pkgo.smsList.add(ent);
                                    checking = true;
                                } else if (pkgo.smsList.get(lenS - 1) < ent) {
                                    pkgo.smsList.add(ent);
                                    if (pkgo.smsList.size() > REC_NUM) {
                                        pkgo.smsList.remove(0);
                                    }
                                    checking = true;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            mWrite = checking;
        }
    }

    private final static class data {
        private final int mUid;
        private final String mPackageName;
        public final ArrayList<Long> callList;
        public final ArrayList<Long> smsList;

        public data(String packageName, int uid,
                    ArrayList<Long> callL, ArrayList<Long> smsL) {
            mPackageName = packageName;
            mUid = uid;
            this.callList = callL;
            this.smsList = smsL;
        }
    }
}

