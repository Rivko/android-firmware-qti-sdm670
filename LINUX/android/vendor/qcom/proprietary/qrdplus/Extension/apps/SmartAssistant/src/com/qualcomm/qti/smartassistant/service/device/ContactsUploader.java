/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.service.device;

import android.content.ContentResolver;
import android.content.Context;
import android.database.ContentObserver;
import android.database.Cursor;
import android.os.AsyncTask;
import android.os.Handler;
import android.provider.ContactsContract;
import android.text.TextUtils;

import com.qualcomm.qti.smartassistant.asr.VoiceSdkHelper;
import com.qualcomm.qti.smartassistant.utils.LogUtils;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class ContactsUploader {

    private static final String TAG = "ContactsUploader";
    private final Context mContext;
    private final Handler mHandler = new Handler();
    private final VoiceSdkHelper mVoiceSdk;
    private ContactsObserver mContactsObserver;
    private QueryContactsTask queryContactsTask;

    public ContactsUploader(Context context, VoiceSdkHelper voiceSdk){
        mContext = context;
        mVoiceSdk = voiceSdk;
        registerContactsObserver();
    }

    public void updateContacts(){
        mHandler.removeCallbacks(uploadContactsTask);
        mHandler.post(uploadContactsTask);
    }

    public void release(){
        unRegisterContactsObserver();
    }

    private void registerContactsObserver(){
        if (mContactsObserver == null) {
            mContactsObserver = new ContactsObserver(mHandler);
            mContext.getContentResolver().registerContentObserver(
                    ContactsContract.Contacts.CONTENT_URI,
                    true, mContactsObserver);
        }
    }

    private void unRegisterContactsObserver(){
        if (mContactsObserver != null) {
            mContext.getContentResolver().unregisterContentObserver(mContactsObserver);
            mContactsObserver = null;
        }
    }

    private class ContactsObserver extends ContentObserver{

        public ContactsObserver(Handler handler) {
            super(handler);
        }

        @Override
        public void onChange(boolean selfChange){
            LogUtils.v(TAG,"onChange selfChange = "+selfChange);
            mHandler.removeCallbacks(uploadContactsTask);
            mHandler.postDelayed(uploadContactsTask, 1000);
        }
    }

    private Runnable uploadContactsTask = new Runnable() {
        @Override
        public void run() {
            queryContactsTask = new QueryContactsTask();
            queryContactsTask.execute();
        }
    };

    private class QueryContactsTask extends AsyncTask<String, Void, String> {

        @Override
        protected String doInBackground(String... strings) {
            try {
                return getAllContacts(mContext);
            } catch (JSONException e) {
                e.printStackTrace();
            }
            return "";
        }

        @Override
        protected void onPostExecute(String result) {
            LogUtils.v(TAG, "QueryContactsTask.onPostExecute result = "+result);
            if (!TextUtils.isEmpty(result)){
                if (mVoiceSdk != null) {
                    mVoiceSdk.updateContacts(result);
                }
            }else {
                LogUtils.e(TAG, "don't need update contacts");
            }
        }
    }

    public static String getAllContacts(Context context) throws JSONException {
        JSONArray array = new JSONArray();
        ContentResolver resolver = context.getContentResolver();
        Cursor phoneCursor = null;

        try {
            phoneCursor = resolver.query(ContactsContract.Contacts.CONTENT_URI, new String[]{"_id", "display_name"}, (String)null, (String[])null, (String)null);
            if(phoneCursor != null) {
                int column = phoneCursor.getColumnIndex("display_name");

                while(phoneCursor.moveToNext() && column > -1) {
                    String displayName = phoneCursor.getString(column);
                    if(!TextUtils.isEmpty(displayName)) {
                        JSONObject object = new JSONObject();
                        object.put("name", displayName);
                        array.put(object);
                    }
                }
            }
        } finally {
            if(null != phoneCursor) {
                phoneCursor.close();
            }

        }

        return array.toString();
    }
}
