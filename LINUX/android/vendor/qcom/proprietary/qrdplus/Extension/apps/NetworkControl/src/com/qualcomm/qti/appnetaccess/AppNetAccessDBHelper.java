/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.appnetaccess;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.os.Handler;

public class AppNetAccessDBHelper {
    private static final String DB_NAME = "appnetaccess.db";
    private static final long CLOSE_DELAY_TIME = 3000l;

    private static AppNetAccessDBHelper mInstance = null;
    private static Object mDBLock = new Object();

    private static final int DB_VERSION = 1;

    /** DBHelper */
    private SQLiteOpenHelper mDatabase;
    /** Handler */
    private Handler mHandler;

    private final String CREATE_APPNETACCESS_T_CARORDER = "CREATE TABLE IF NOT EXISTS "
            + AppNetAccessDao.TABLE_NAME
            + " ( "
            + AppNetAccessDao.COL_ID
            + " INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            + AppNetAccessDao.COL_APPS_UID
            + " INTEGER, "
            + AppNetAccessDao.COL_NAME
            + " TEXT, "
            + AppNetAccessDao.COL_WIFI
            + " INTEGER, " + AppNetAccessDao.COL_MOBILE + " INTEGER )";
    private Runnable mCloseDatabaseRunnable = new Runnable() {

        @Override
        public void run() {
            synchronized (mDBLock) {
                mDatabase.close();
            }
        }
    };

    @SuppressWarnings("deprecation")
    private AppNetAccessDBHelper(Context context) {
        mHandler = new Handler(context.getMainLooper());

        mDatabase = new SQLiteOpenHelper(context, DB_NAME, null, DB_VERSION) {

            @Override
            public void onCreate(SQLiteDatabase db) {
                AppNetAccessDBHelper.this.onCreate(db);
            }

            @Override
            public void onUpgrade(SQLiteDatabase db, int oldVersion,
                    int newVersion) {
                if (oldVersion <= 1) {
                    db.execSQL(CREATE_APPNETACCESS_T_CARORDER);
                }
            }
        };

        mDatabase.getWritableDatabase().setLockingEnabled(false);
    }

    public static AppNetAccessDBHelper getInstance(Context context) {
        if (mInstance == null) {
            synchronized (mDBLock) {
                if (mInstance == null) {
                    mInstance = new AppNetAccessDBHelper(context);
                }
            }
        }
        return mInstance;
    }

    private void onCreate(SQLiteDatabase db) {
        db.execSQL(CREATE_APPNETACCESS_T_CARORDER);
    }

    public void execSQL(String sql) {
        synchronized (mDBLock) {
            try {
                mDatabase.getWritableDatabase().execSQL(sql);
            } catch (SQLException e) {
            }
        }
    }

    public Cursor query(String queryStr) {
        synchronized (mDBLock) {
            return mDatabase.getReadableDatabase().rawQuery(queryStr, null);
        }
    }

    public Cursor query(String table, String[] columns, String selection,
            String[] selectionArgs, String groupBy, String having,
            String orderBy, String limit) {
        synchronized (mDBLock) {
            SQLiteDatabase database = mDatabase.getWritableDatabase();
            return database.query(table, columns, selection, selectionArgs,
                    groupBy, having, orderBy, limit);
        }
    }

    public void noneQuery(String queryStr) {
        synchronized (mDBLock) {
            SQLiteDatabase database = mDatabase.getWritableDatabase();
            database.execSQL(queryStr);
        }
    }

    public long insert(String table, String nullColumnHack, ContentValues values) {
        synchronized (mDBLock) {
            SQLiteDatabase database = mDatabase.getWritableDatabase();
            return database.insert(table, nullColumnHack, values);
        }
    }

    public long insert(String table, ContentValues values) {
        synchronized (mDBLock) {
            SQLiteDatabase database = mDatabase.getWritableDatabase();
            return database.insert(table, null, values);
        }
    }

    public int delete(String table, String whereClause, String[] whereArgs) {
        synchronized (mDBLock) {
            SQLiteDatabase database = mDatabase.getWritableDatabase();
            return database.delete(table, whereClause, whereArgs);
        }
    }

    public int update(String table, ContentValues values, String whereClause,
            String[] whereArgs) {
        synchronized (mDBLock) {
            SQLiteDatabase database = mDatabase.getWritableDatabase();
            return database.update(table, values, whereClause, whereArgs);
        }
    }

    public int deleteTable(String table) {
        synchronized (mDBLock) {
            SQLiteDatabase database = mDatabase.getWritableDatabase();
            return database.delete(table, null, null);
        }
    }

    public void dropTable(String table, SQLiteDatabase db) {
        String sql = "DROP TABLE IF EXISTS " + table;
        db.execSQL(sql);
    }

    public boolean execTransaction(SimpleCallback<Object> callback) {

        boolean result = false;
        synchronized (mDBLock) {
            SQLiteDatabase database = mDatabase.getWritableDatabase();
            database.beginTransaction();

            try {
                callback.onCallback(this);
                database.setTransactionSuccessful();
                result = true;
            } finally {
                database.endTransaction();
                close();
            }
        }
        return result;
    }

    public synchronized void close() {
        mHandler.removeCallbacks(mCloseDatabaseRunnable);
        mHandler.postDelayed(mCloseDatabaseRunnable, CLOSE_DELAY_TIME);
    }

    public interface SimpleCallback<T> {
        public void onCallback(T data);
    }
}
