/*
 * Copyright (c) 2014, 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.qti.launcherunreadservice;

import android.app.Service;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.database.ContentObserver;
import android.database.Cursor;
import android.database.sqlite.SQLiteException;
import android.net.Uri;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.RemoteException;
import android.provider.BaseColumns;
import android.provider.CallLog.Calls;
import android.provider.Telephony.MmsSms;
import android.provider.Telephony.Mms;
import android.provider.Telephony.Sms;
import android.util.Log;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.List;
import java.util.ArrayList;

import static com.google.android.mms.pdu.PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND;
import static com.google.android.mms.pdu.PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF;

public class LauncherUnreadService extends Service {
    private static final String TAG = "LauncherUnreadService";
    private static final boolean DEBUG = false;
    private static final int CONTENT_OBSERVER_HANDLER_DELAY_MS = 1000;
    private static final int MISSED_IMS_TYPE = 1002;
    private static final Uri EMAIL_URI = Uri.parse("content://com.android.email.provider/mailbox");

    private Handler mHandler = new Handler();
    private ComponentName mMmsComponentName = new
            ComponentName("com.android.mms", "com.android.mms.ui.ConversationList");
    private ComponentName mDialerComponentName = new
            ComponentName("com.android.dialer", "com.android.dialer.app.DialtactsActivity");
    private ComponentName mEmailComponentName = new
            ComponentName("com.android.email", "com.android.email.activity.Welcome");

    private GetUnreadNumberBinder mBinder = new GetUnreadNumberBinder();

    private static final String[] SMS_PROJECTION = new String[] {
        BaseColumns._ID,
        Sms.THREAD_ID,
    };
    private static final String[] MMS_PROJECTION = new String[] {
        BaseColumns._ID,
        Mms.THREAD_ID,
    };
    private long mMsgId = -1;
    private long mThreadId = -1;
    private String mMsgType = "";
    private Map mCallbacks = new HashMap<IBinder, IUnreadNumberCallback>();

    private final ContentObserver mSMSContentObserver = new ContentObserver(new Handler()) {

        private int lastUnreadCount = -1;
        private long lastMsgId = -1;
        private Runnable changeHandler = new Runnable() {

            @Override
            public void run() {
                int unreadCount = getMissMmsCount();
                if (lastUnreadCount != unreadCount || lastMsgId != mMsgId) {
                    lastUnreadCount = unreadCount;
                    lastMsgId = mMsgId;
                    sendUnreadChanged(mMmsComponentName, unreadCount);
                }
            }
        };

        @Override
        public void onChange(boolean selfChange) {
            Log.i(TAG, "mms content changed");
            mHandler.removeCallbacks(changeHandler);
            mHandler.postDelayed(changeHandler, CONTENT_OBSERVER_HANDLER_DELAY_MS);
        }
    };

    private final ContentObserver mCallContentObserver = new ContentObserver(new Handler()) {
        private int lastMissedCount = -1;
        private Runnable changeHandler = new Runnable() {

            @Override
            public void run() {
                int missedCount = getMissCallCount();
                if (lastMissedCount != missedCount) {
                    lastMissedCount = missedCount;
                    sendUnreadChanged(mDialerComponentName, missedCount);
                }
            }
        };

        @Override
        public void onChange(boolean selfChange) {
            Log.i(TAG, "Call log content changed");
            mHandler.removeCallbacks(changeHandler);
            mHandler.postDelayed(changeHandler, CONTENT_OBSERVER_HANDLER_DELAY_MS);
        }
    };

    private final ContentObserver mEmailContentObserver = new ContentObserver(new Handler()) {

        private int lastUnreadCount = -1;
        private Runnable changeHandler = new Runnable() {

            @Override
            public void run() {
                int unreadCount = getMissEmailCount();
                if (lastUnreadCount != unreadCount) {
                    lastUnreadCount = unreadCount;
                    sendUnreadChanged(mEmailComponentName, unreadCount);
                }
            }
        };

        @Override
        public void onChange(boolean selfChange) {
            Log.i(TAG, "email content changed");
            mHandler.removeCallbacks(changeHandler);
            mHandler.postDelayed(changeHandler, CONTENT_OBSERVER_HANDLER_DELAY_MS);
        }
    };

    @Override
    public void onCreate(){
        super.onCreate();
        ContentResolver resolver = getContentResolver();
        resolver.registerContentObserver(MmsSms.CONTENT_URI, true, mSMSContentObserver);
        resolver.registerContentObserver(Calls.CONTENT_URI, true, mCallContentObserver);
        //resolver.registerContentObserver(EMAIL_URI,true,mEmailContentObserver);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        ContentResolver resolver = getContentResolver();
        resolver.unregisterContentObserver(mCallContentObserver);
        resolver.unregisterContentObserver(mSMSContentObserver);
        //resolver.unregisterContentObserver(mEmailContentObserver);
    }

    private int getMissMmsCount() {
        int missSmsCount = 0;
        int missMmsCount = 0;
        int missMsgCount = 0;
        HashSet<Long> threads = new HashSet<Long>(4);
        Cursor cursorSMS = null;
        Cursor cursorMMS = null;
        try {
            cursorSMS = getContentResolver().query(
                    Uri.parse("content://sms/"), SMS_PROJECTION, "(read=0)",
                    null, null);
            cursorMMS = getContentResolver().query(
                    Uri.parse("content://mms/"), MMS_PROJECTION, "(read=0 AND (" +
                            Mms.MESSAGE_TYPE + "=" + MESSAGE_TYPE_NOTIFICATION_IND + " OR " +
                            Mms.MESSAGE_TYPE + "=" + MESSAGE_TYPE_RETRIEVE_CONF+ "))", null,
                    null);
        } catch (SQLiteException e) {
            return missSmsCount;
        }
        if (cursorSMS != null) {
            missSmsCount = cursorSMS.getCount();
        }
        if (cursorMMS != null) {
            missMmsCount = cursorMMS.getCount();
        }
        missMsgCount = missSmsCount + missMmsCount;
        Log.i(TAG, "getMissMmsCount " + missMsgCount);
        mThreadId = -1;
        mMsgId = -1;
        mMsgType = "";
        try {
            if (missMsgCount == 1) {
                if (missSmsCount == 1 && cursorSMS != null && cursorSMS.moveToFirst()) {
                    mThreadId = cursorSMS.getLong(cursorSMS.getColumnIndex(Sms.THREAD_ID));
                    mMsgId = cursorSMS.getLong(cursorSMS.getColumnIndex(BaseColumns._ID));
                    mMsgType = "sms";
                } else if (cursorMMS != null && cursorMMS.moveToFirst()) {
                    mThreadId = cursorMMS.getLong(cursorMMS.getColumnIndex(Mms.THREAD_ID));
                    mMsgId = cursorMMS.getLong(cursorMMS.getColumnIndex(BaseColumns._ID));
                    mMsgType = "mms";
                }
            } else if (missMsgCount > 1) {
                if (cursorSMS != null) {
                    while (cursorSMS.moveToNext() && threads.size() < 2) {
                        threads.add(cursorSMS.getLong(cursorSMS.getColumnIndex(Sms.THREAD_ID)));
                    }
                }
                if (cursorMMS != null) {
                    while (cursorMMS.moveToNext() && threads.size() < 2) {
                        threads.add(cursorMMS.getLong(cursorMMS.getColumnIndex(Mms.THREAD_ID)));
                    }
                }
                if (threads.size() == 1) {
                    Iterator iterator = threads.iterator();
                    if (iterator.hasNext()) {
                        mThreadId = (Long) iterator.next();
                    }
                }
            }
            if (DEBUG) Log.d(TAG, "mThreadId: " + mThreadId + " mMsgId: " + mMsgId +
                    " mMsgType: " + mMsgType);
        } finally {
            if (cursorSMS != null) {
                cursorSMS.close();
            }
            if (cursorMMS != null) {
                cursorMMS.close();
            }
        }
        return missMsgCount;
    }

    private int getMissCallCount() {
        int missCallCount = 0;
        Uri missingCallUri = Calls.CONTENT_URI;
        String where = "(" + Calls.TYPE + " = '" + Calls.MISSED_TYPE + "' OR "
                + Calls.TYPE + " = '" + MISSED_IMS_TYPE + "')"
                + " AND new=1";
        Cursor cursorCall = null;
        try {
            cursorCall = getContentResolver().query(missingCallUri,
                    null, where, null, null);
        } catch (SQLiteException e) {
            return missCallCount;
        }

        if (cursorCall != null) {
            missCallCount = cursorCall.getCount();
            cursorCall.close();
        }
        Log.i(TAG, "getMissCallCount: " + missCallCount);
        return missCallCount;
    }

    private int getMissEmailCount() {
        int missEmailCount = 0;
        Uri missingEmailUri = EMAIL_URI;

        Cursor cursorEmail = null;
        try {
            cursorEmail = getContentResolver().query(missingEmailUri,
                    new String[] {"sum(unreadCount)"},"type =?", new String[] {"0"}, null);
            if (cursorEmail != null) {
                cursorEmail.moveToFirst();
                if (cursorEmail.getString(0) != null) {
                    missEmailCount = Integer.valueOf(cursorEmail.getString(0));
                }
            }
        } catch (SQLiteException e) {
            return missEmailCount;
        } finally {
            if (cursorEmail != null) {
                cursorEmail.close();
            }
        }

        return missEmailCount;
    }

    private void sendUnreadChanged(ComponentName componentName, int unreadCount) {
        Iterator iter = mCallbacks.entrySet().iterator();
        List<IBinder> deleteItems = new ArrayList<IBinder>();
        while (iter.hasNext()) {
            Map.Entry entry = (Map.Entry) iter.next();
            IBinder token = (IBinder) entry.getKey();
            IUnreadNumberCallback callback = (IUnreadNumberCallback) entry.getValue();
            if (callback != null && callback.asBinder().isBinderAlive()){
                try {
                    callback.onUnreadNumberChanged(componentName, unreadCount);
                }catch (RemoteException ex) {
                    Log.e(TAG,"exception "+ex.toString());
                    deleteItems.add(token);
                }
            }else {
                deleteItems.add(token);
            }
        }
        for (IBinder token: deleteItems){
            Log.d(TAG,"remove token == "+token);
            mCallbacks.remove(token);
        }
    }


    private class GetUnreadNumberBinder extends IGetUnreadNumber.Stub {
        @Override
        public Map GetUnreadNumber() {

            PackageManager packageManager = getPackageManager();
            if (packageManager != null) {
                String[] packageList = packageManager.getPackagesForUid(Binder.getCallingUid());
                if (packageList == null || !isSystemApp(packageList,packageManager)) {
                    return null;
                }
            } else {
                return null;
            }

            Map unreadChangedMap = new HashMap<ComponentName, Integer>();
            unreadChangedMap.put(mDialerComponentName,getMissCallCount());
            unreadChangedMap.put(mMmsComponentName,getMissMmsCount());
//            unreadChangedMap.put(mEmailComponentName,getMissEmailCount());

            return unreadChangedMap;
        }

        @Override
        public void registerUnreadNumberCallback(final IBinder token,
            final IUnreadNumberCallback callback){
            mHandler.post(new Runnable(){
                @Override
                public void run() {
                    mCallbacks.put(token, callback);
                }
            });
        }

        @Override
        public void unRegisterUnreadNumberCallback(final IBinder token){
            mHandler.post(new Runnable(){
                @Override
                public void run() {
                    mCallbacks.remove(token);
                }
            });
        }

        private boolean isSystemApp(String[] packageList, PackageManager packageManager) {
            for (int i = 0; i < packageList.length; i++) {
                String packageName = packageList[i];
                ApplicationInfo applicationInfo;
                try {
                    applicationInfo = packageManager.getApplicationInfo(packageName, 0);
                } catch (PackageManager.NameNotFoundException e) {
                    e.printStackTrace();
                    applicationInfo = null;
                    return false;
                }
                if (((applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0) ||
                        ((applicationInfo.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0)) {
                    return true;
                }
            }
            return false;
        }

    }
}
