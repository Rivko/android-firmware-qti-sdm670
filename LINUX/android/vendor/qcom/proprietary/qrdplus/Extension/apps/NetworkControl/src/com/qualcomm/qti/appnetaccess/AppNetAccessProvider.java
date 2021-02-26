/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.appnetaccess;

import android.content.ContentProvider;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.UriMatcher;
import android.database.Cursor;
import android.net.Uri;

public class AppNetAccessProvider extends ContentProvider {
    private AppNetAccessDBHelper dbOpenHelper;
    private UriMatcher URI_MATCHER;
    private static final int MUTI_APPS = 0;
    private static final int SINGLE_APP = 1;
    private ContentResolver mResolver;

    @Override
    public boolean onCreate() {
        initUriMatcher();
        dbOpenHelper = AppNetAccessDBHelper.getInstance(getContext());
        mResolver = getContext().getContentResolver();
        return true;
    }

    private void initUriMatcher() {
        URI_MATCHER = new UriMatcher(UriMatcher.NO_MATCH);
        URI_MATCHER.addURI(AppNetAccessDao.PROVIDER_NAME, AppNetAccessDao.TABLE_NAME,
                MUTI_APPS);
        URI_MATCHER.addURI(AppNetAccessDao.PROVIDER_NAME, AppNetAccessDao.TABLE_NAME
                + "/#", SINGLE_APP);

    }

    @Override
    public String getType(Uri uri) {
        switch (URI_MATCHER.match(uri)) {
        case MUTI_APPS:
            return "vnd.android.cursor.dir/apps";
        case SINGLE_APP:
            return "vnd.android.cursor.item/app";
        default:
            throw new IllegalArgumentException("unknown uri" + uri.toString());
        }
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        switch (URI_MATCHER.match(uri)) {
        case MUTI_APPS:
            long rowid = dbOpenHelper.insert(AppNetAccessDao.TABLE_NAME,
                    AppNetAccessDao.COL_ID, values);
            Uri noteUri = null;
            if (rowid != -1) {
                noteUri = ContentUris.withAppendedId(uri, rowid);
                mResolver.notifyChange(noteUri, null);
            }
            return noteUri;
        default:
            throw new IllegalArgumentException("unknown uri" + uri.toString());
        }
    }

    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) {
        int deletedNum = 0;
        switch (URI_MATCHER.match(uri)) {
        case MUTI_APPS:
            deletedNum = dbOpenHelper.delete(AppNetAccessDao.TABLE_NAME,
                    selection, selectionArgs);
            break;
        case SINGLE_APP:
            long id = ContentUris.parseId(uri);
            String where = AppNetAccessDao.COL_APPS_UID + "=" + id;
            if (selection != null && !"".equals(selection.trim())) {
                where = selection + " and " + where;
            }
            deletedNum = dbOpenHelper.delete(AppNetAccessDao.TABLE_NAME, where,
                    selectionArgs);
            break;
        default:
            throw new IllegalArgumentException("unknown uri" + uri.toString());
        }
        mResolver.notifyChange(uri, null);
        return deletedNum;
    }

    @Override
    public int update(Uri uri, ContentValues values, String selection,
            String[] selectionArgs) {
        int updataNum = 0;
        switch (URI_MATCHER.match(uri)) {
        case MUTI_APPS:
            updataNum = dbOpenHelper.update(AppNetAccessDao.TABLE_NAME, values,
                    selection, selectionArgs);
            mResolver.notifyChange(uri, null);
            break;
        case SINGLE_APP:
            long id = ContentUris.parseId(uri);
            String where = AppNetAccessDao.COL_APPS_UID + "=" + id;
            if (selection != null && !"".equals(selection.trim())) {
                where = selection + " and " + where;
            }
            updataNum = dbOpenHelper.update(AppNetAccessDao.TABLE_NAME, values,
                    where, selectionArgs);
            mResolver.notifyChange(uri, null);
            break;
        default:
            throw new IllegalArgumentException("unknown uri" + uri.toString());
        }
        return updataNum;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection,
            String[] selectionArgs, String sortOrder) {
        Cursor cursor;
        switch (URI_MATCHER.match(uri)) {
        case MUTI_APPS:
            cursor = dbOpenHelper.query(AppNetAccessDao.TABLE_NAME, projection,
                    selection, selectionArgs, null, null, sortOrder, null);
            break;
        case SINGLE_APP:
            long id = ContentUris.parseId(uri);
            String where = AppNetAccessDao.COL_APPS_UID + "=" + id;
            if (selection != null && !"".equals(selection.trim())) {
                where = selection + " and " + where;
            }
            cursor = dbOpenHelper.query(AppNetAccessDao.TABLE_NAME, projection,
                    where, selectionArgs, null, null, sortOrder, null);
            break;
        default:
            throw new IllegalArgumentException("unknown uri" + uri.toString());
        }
        cursor.setNotificationUri(mResolver, uri);
        return cursor;
    }
}
