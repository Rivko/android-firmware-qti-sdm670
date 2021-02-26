/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.appnetaccess;

import java.util.List;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;

public class AppNetAccessDao {
    private static AppNetAccessDao mInstance;
    private Context mContext;
    private ContentResolver mResolver;
    public static String TABLE_NAME = "appnetaccess_t";
    public static String COL_ID = "_id";
    public static String COL_APPS_UID = "app_uid";
    public static String COL_NAME = "app_name";
    public static String COL_WIFI = "app_wifi";
    public static String COL_MOBILE = "app_moblie";
    public static String PROVIDER_NAME = "com.qualcomm.qti.appnetaccess.AppNetAccessProvider";
    public static String URI = "content://" + PROVIDER_NAME + "/" + TABLE_NAME;

    public AppNetAccessDao(Context context) {
        this.mContext = context;
        mResolver = mContext.getContentResolver();
    }

    public static AppNetAccessDao getInstance(Context context) {
        if (mInstance == null) {
            mInstance = new AppNetAccessDao(context);
        }
        return mInstance;
    }

    public void insert(ContentValues values) {
        Uri uri = Uri.parse(URI);
        mResolver.insert(uri, values);
    }

    public void delete(int id) {
        Uri uri = Uri.parse(URI);
        String where = COL_APPS_UID + "=?";
        mResolver.delete(uri, where, new String[] { String.valueOf(id) });
    }

    public void update(int id, ContentValues values) {
        Uri uri = Uri.parse(URI);
        String where = COL_APPS_UID + "=?";
        mResolver.update(uri, values, where,
                new String[] {
                    String.valueOf(id)
                });
    }

    public Cursor queryAll() {
        Uri uri = Uri.parse(URI);
        Cursor cursor = mResolver.query(uri, null, null, null, COL_ID + " asc");
        return cursor;
    }

    public Cursor query(int id) {
        Uri uri = Uri.parse(URI);
        String where = COL_APPS_UID + "=?";
        Cursor cursor = mResolver.query(uri, null, where,
                new String[] {
                    String.valueOf(id)
                }, COL_ID + " asc");
        return cursor;
    }

    public ContentValues generateContentValues(AppNetAccessEntity entity) {
        ContentValues values = new ContentValues();
        values.put(COL_APPS_UID, entity.getPackageInfo().applicationInfo.uid);
        values.put(COL_NAME, entity.getAppLable());
        values.put(COL_WIFI, 0);
        values.put(COL_MOBILE, 0);
        return values;
    }

    public void insertOrUpdate(final List<AppNetAccessEntity> entitys) {
        for (AppNetAccessEntity e : entitys) {
            int app_uid = e.getPackageInfo().applicationInfo.uid;
            if (!isExists(app_uid)) {
                insert(generateContentValues(e));
            }
        }
    }

    public boolean isExists(int id) {
        Cursor cursor = query(id);
        if (cursor != null && cursor.moveToFirst()) {
            int num = cursor.getInt(0);
            cursor.close();
            return num > 0;
        }
        return false;
    }
}
