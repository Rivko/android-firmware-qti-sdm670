/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data;


import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.provider.BaseColumns;
import android.util.Log;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;

/**
 * The Class VoicePrintDatabase. It maintains the database for the VoicePrintDemo application. There
 * is no content provider as other processes and applications should not be accessing this data.
 * <p>
 * This is a Singleton class and can be invoked by VoicePrintDatabase.getInstance(). This instance
 * will be null if no context has been provided. The application must instantiate the singleton
 * and pass in its context.
 * </p>
 * <p>
 * Each row represents a KeyPhrase object. All rows may be requested or a single row with the
 * given key can be returned.
 * </p>
 */
public class VoicePrintDatabase {

    /**
     * The Constant log TAG.
     */
    private static final String TAG = "VoicePrintDemo";

    /**
     * The Constant database name.
     */
    private static final String DB_NAME = "config.db";

    /**
     * The Constant database version.
     */
    private static final int DB_VERSION = 1;
    private Context context;

    /**
     * The database helper object for wrapping SQL queries.
     */
    private DatabaseHelper dbHelper;

    private static VoicePrintDatabase mInstance = null;

    public static abstract class TableEntry implements BaseColumns {
        public static final String TABLE_NAME = "config";
        public static final String COLUMN_ENTRY_KEY = "key";
        public static final String COLUMN_KEYPHRASE_NAME = "name";
        public static final String COLUMN_KEYPHRASE = "phrase";
        public static final String COLUMN_THRESHOLD = "threshold";
        public static final String COLUMN_ACTION = "action";
        public static final String COLUMN_MATCH_SUCCESS = "matchSuccess";
        public static final String COLUMN_MATCH_FAILED = "matchFailed";
        public static final String COLUMN_MATCH_FALSE_POSITIVE = "falsePositive";
    }

    /**
     * The projection column map for all entries.
     * Only add to this table. Don't insert and change the ids as the ordering will
     * not be backwards compatible.
     */
    private static HashMap<String, Integer> columnMap;

    /*
     * This is the projection map used for column names and column ids.
     * Only add to the end of this map and not insert between old values.
     * Column numbers should not change from versino to version.
     */
    static {
        columnMap = new LinkedHashMap<String, Integer>();
        columnMap.put(TableEntry.COLUMN_ENTRY_KEY, 0);
        columnMap.put(TableEntry.COLUMN_KEYPHRASE_NAME, 1);
        columnMap.put(TableEntry.COLUMN_KEYPHRASE, 2);
        columnMap.put(TableEntry.COLUMN_THRESHOLD, 3);
        columnMap.put(TableEntry.COLUMN_ACTION, 4);
        columnMap.put(TableEntry.COLUMN_MATCH_SUCCESS, 5);
        columnMap.put(TableEntry.COLUMN_MATCH_FAILED, 6);
        columnMap.put(TableEntry.COLUMN_MATCH_FALSE_POSITIVE, 7);
    }


    /**
     * Instantiate a Singleton instance of this database.
     *
     * @param context
     */
    public static void instantiate(Context context) {
        mInstance = new VoicePrintDatabase(context);
    }

    /**
     * Gets the single instance of VoicePrintDatabase.
     *
     * @return single instance of VoicePrintDatabase.
     */
    public static VoicePrintDatabase getInstance() {
        return mInstance;
    }

    /*
     * Private class constructor. This class requires an instantiation call with context.
     */
    private VoicePrintDatabase(Context context) {
        try {
            this.context = context;
            dbHelper = new DatabaseHelper(context);
        } catch (Exception e) {
            Log.d(TAG, "Exception in VoicePrintDatabase() " + e.getMessage());
        }
    }

    /**
     * Insert a new entry into the database. If the row already exists, it will be overwritten.
     * The threshold will be set to 0 and no action will be set.
     *
     * @param key
     * @param name
     * @param phrase
     */
    public void insertEntry(String key, String name, String phrase) {
        insertEntry(key, name, phrase, 0, KeyPhraseAction.NO_ACTION);
    }

    /**
     * Insert a new entry into the database. If the row already exists, it will be overwritten.
     * No action will be set.
     *
     * @param key
     * @param name
     * @param phrase
     * @param threshold
     */
    public void insertEntry(String key, String name, String phrase, int threshold) {
        insertEntry(key, name, phrase, threshold, KeyPhraseAction.NO_ACTION);
    }

    /**
     * Insert a new entry into the database. If the row already exists, it will be overwritten.
     * All counts will be reset to 0.
     *
     * @param key
     * @param name
     * @param phrase
     * @param threshold
     * @param action
     */
    public void insertEntry(String key, String name, String phrase, int threshold, int action) {
        insertEntry(key, name, phrase, threshold, action, 0, 0, 0);
    }

    /**
     * Insert a new entry into the database. If the row already exists, it will be overwritten.
     *
     * @param key
     * @param name
     * @param phrase
     * @param threshold
     * @param action
     * @param matchSuccess
     * @param matchFailed
     * @param falsePositives
     */
    public void insertEntry(String key, String name, String phrase, int threshold, int action,
                            int matchSuccess, int matchFailed, int falsePositives) {
        SQLiteDatabase db = null;
        try {
            db = dbHelper.getReadableDatabase();

            // Create new row entry
            ContentValues values = new ContentValues();
            values.put(TableEntry.COLUMN_ENTRY_KEY, key);
            values.put(TableEntry.COLUMN_KEYPHRASE_NAME, name);
            values.put(TableEntry.COLUMN_KEYPHRASE, phrase);
            values.put(TableEntry.COLUMN_THRESHOLD, threshold);
            values.put(TableEntry.COLUMN_ACTION, action);
            values.put(TableEntry.COLUMN_MATCH_SUCCESS, matchSuccess);
            values.put(TableEntry.COLUMN_MATCH_FAILED, matchFailed);
            values.put(TableEntry.COLUMN_MATCH_FALSE_POSITIVE, falsePositives);
            // Update row if it already exists. Otherwise insert a new row.
            String selection = TableEntry.COLUMN_ENTRY_KEY + " LIKE ?";
            String[] selectionArgs = {String.valueOf(key)};

            String query = "SELECT * FROM " + TableEntry.TABLE_NAME + " WHERE " + TableEntry
                           .COLUMN_ENTRY_KEY + " =?";

            Cursor cursor = db.rawQuery(query, new String[] {key});

            if (cursor.moveToFirst()) {
                Log.d(TAG, "Updating database for " + key);
                Log.d(TAG, " Data: " + values.toString());
                db.update(
                    TableEntry.TABLE_NAME,
                    values,
                    selection,
                    selectionArgs);
            } else {
                Log.d(TAG, "Inserting into database key=" + key);
                Log.d(TAG, " Data: " + values.toString());
                db.insert(
                    TableEntry.TABLE_NAME,
                    TableEntry.COLUMN_KEYPHRASE_NAME,
                    values);
            }

            cursor.close();

        } catch (Exception e) {
            Log.d(TAG, "Exception in insertEntry " + e.getMessage());
            e.printStackTrace();
        } finally {
            if (db != null && db.isOpen()) {
                db.close();
            }
        }
    }

    /**
     * Insert a new entry into the database. If the row already exists, it will be overwritten.
     *
     * @param keyphrase
     * @throws SQLException
     */
    public void insertEntry(KeyPhrase keyphrase) throws SQLException {

        if (keyphrase == null) {
            throw new SQLException("KeyPhrase cannot be null.");
        }

        insertEntry(keyphrase.getId(), keyphrase.getName(), keyphrase.getPhrase(),
                    keyphrase.getThreshold(), keyphrase.getActionId(), keyphrase.getMatchSuccess(),
                    keyphrase.getMatchFails(), keyphrase.getFalsePositives());
    }

    /**
     * Insert a new entry into the database. If the row already exists, it will be overwritten.
     *
     * @param keyphrase
     * @throws SQLException
     */
    public void update(KeyPhrase keyphrase) throws SQLException {
        if (keyphrase == null) {
            throw new SQLException("KeyPhrase cannot be null.");
        }

        insertEntry(keyphrase.getId(), keyphrase.getName(), keyphrase.getPhrase(),
                    keyphrase.getThreshold(), keyphrase.getActionId(), keyphrase.getMatchSuccess(),
                    keyphrase.getMatchFails(), keyphrase.getFalsePositives());
    }

    /**
     * Resets the statistic counts for given row.
     *
     * @param keyphrase
     * @throws SQLException
     */
    public void resetCounts(KeyPhrase keyphrase) throws SQLException {
        if (keyphrase == null) {
            throw new SQLException("KeyPhrase cannot be null.");
        }

        resetCounts(keyphrase.getId());
    }

    /**
     * Resets the statistic counts for given row.
     *
     * @param key
     * @throws SQLException
     */
    public void resetCounts(String key) throws SQLException {
        SQLiteDatabase db = null;

        try {


            db = dbHelper.getReadableDatabase();

            // If row exists, update. If it does not exist, return false.

            ContentValues values = new ContentValues();
            values.put(TableEntry.COLUMN_ENTRY_KEY, key);
            values.put(TableEntry.COLUMN_MATCH_SUCCESS, 0);
            values.put(TableEntry.COLUMN_MATCH_FAILED, 0);
            values.put(TableEntry.COLUMN_MATCH_FALSE_POSITIVE, 0);

            // Update row if it already exists. Otherwise insert a new row.
            String selection = TableEntry.COLUMN_ENTRY_KEY + " LIKE ?";
            String[] selectionArgs = {String.valueOf(key)};


            Log.d(TAG, "Updating database for " + key);
            db.update(
                TableEntry.TABLE_NAME,
                values,
                selection,
                selectionArgs);


        } catch (Exception e) {
            Log.d(TAG, "Exception in resetCounts " + e.getMessage());
            e.printStackTrace();
        } finally {
            if (db != null && db.isOpen()) {
                db.close();
            }
        }
    }

    /**
     * Sets the threshold for the given row.
     *
     * @param key       The key of the row
     * @param threshold The threshold value to set.
     * @throws SQLException
     */
    public void setThreshold(String key, int threshold) throws SQLException {
        SQLiteDatabase db = null;

        try {

            db = dbHelper.getReadableDatabase();

            // If row exists, update. If it does not exist, return false.

            ContentValues values = new ContentValues();
            values.put(TableEntry.COLUMN_ENTRY_KEY, key);
            values.put(TableEntry.COLUMN_THRESHOLD, threshold);

            // Update row if it already exists. Otherwise insert a new row.
            String selection = TableEntry.COLUMN_ENTRY_KEY + " LIKE ?";
            String[] selectionArgs = {String.valueOf(key)};


            Log.d(TAG, "Updating database for " + key);
            db.update(
                TableEntry.TABLE_NAME,
                values,
                selection,
                selectionArgs);


        } catch (Exception e) {
            Log.d(TAG, "Exception in resetCounts " + e.getMessage());
            e.printStackTrace();
        } finally {
            if (db != null && db.isOpen()) {
                db.close();
            }
        }
    }

    public void deleteAll() {
        SQLiteDatabase db = null;

        try {
            db = dbHelper.getReadableDatabase();
            db.delete(TableEntry.TABLE_NAME, null, null);
        } catch (Exception e) {
            Log.d(TAG, "Exception in deleteAll " + e.getMessage());
            e.printStackTrace();
        } finally {
            if (db != null && db.isOpen()) {
                db.close();
            }
        }
    }

    /**
     * Deletes the given row. Throws an exception if row does not exist.
     *
     * @param key
     * @throws SQLException
     */
    public void deleteEntry(String key) throws SQLException {
        SQLiteDatabase db = null;

        try {


            db = dbHelper.getReadableDatabase();

            String selection = TableEntry.COLUMN_ENTRY_KEY + " LIKE ?";
            String[] selectionArgs = {String.valueOf(key)};

            Log.d(TAG, "Deleting entry '" + key + "' from database.");
            db.delete(
                TableEntry.TABLE_NAME,
                selection,
                selectionArgs);


        } catch (Exception e) {
            Log.d(TAG, "Exception in deleteEntry " + e.getMessage());
            e.printStackTrace();
        } finally {
            if (db != null && db.isOpen()) {
                db.close();
            }
        }
    }

    /**
     * Deletes the given row. Throws an exception if row does not exist.
     *
     * @param keyphrase
     * @throws SQLException
     */
    public void deleteEntry(KeyPhrase keyphrase) throws SQLException {
        SQLiteDatabase db = null;

        try {
            if (keyphrase == null) {
                throw new SQLException("KeyPhrase cannot be null.");
            }

            deleteEntry(keyphrase.getId());

        } catch (Exception e) {
            Log.d(TAG, "Exception in deleteEntry " + e.getMessage());
            e.printStackTrace();
        } finally {
            if (db != null && db.isOpen()) {
                db.close();
            }
        }
    }

    /**
     * Find and populate a KeyPhrase object with data from its row in the database.
     * Returns null if not found.
     *
     * @param key
     * @return returns the key phrase row info or null if not found.
     */
    public KeyPhrase getEntry(String key) {
        return getKeyPhrase(key);
    }

    /**
     * Find and populate a KeyPhrase object with data from its row in the database.
     * Returns null if not found.
     *
     * @param key
     * @return returns the key phrase row info or null if not found.
     */
    public KeyPhrase getKeyPhrase(String key) {
        KeyPhrase keyphrase = null;
        try {

            String[] array = new String[columnMap.size()];
            columnMap.keySet().toArray(array);

            Cursor cursor = query((String[]) array, TableEntry
                                  .COLUMN_ENTRY_KEY, new String[] {key});

            if (cursor.moveToFirst()) {
                keyphrase = extractKeyPhrase(cursor);
            }

            cursor.close();


        } catch (Exception e) {
            Log.d(TAG, "Exception in query " + e.getMessage());
            e.printStackTrace();
        }
        return keyphrase;
    }

    private KeyPhrase extractKeyPhrase(Cursor cursor) {
        if (cursor == null) {
            return null;
        }

        KeyPhrase keyphrase = null;
        try {
            keyphrase = new KeyPhrase(cursor.getString(columnMap.get(TableEntry
                                      .COLUMN_KEYPHRASE_NAME)),
                                      cursor.getString(columnMap.get(TableEntry.COLUMN_KEYPHRASE)));
            keyphrase.setThreshold(cursor.getInt(columnMap.get(TableEntry.COLUMN_THRESHOLD)));
            keyphrase.setActionId(cursor.getInt(columnMap.get(TableEntry.COLUMN_ACTION)));
            keyphrase.setMatchSuccess(cursor.getInt(columnMap.get(TableEntry
                                                    .COLUMN_MATCH_SUCCESS)));
            keyphrase.setMatchFails(cursor.getInt(columnMap.get(TableEntry
                                                  .COLUMN_MATCH_FAILED)));
            keyphrase.setFalsePositives(cursor.getInt(columnMap.get(TableEntry
                                        .COLUMN_MATCH_FALSE_POSITIVE)));


        } catch (Exception e) {
            Log.d(TAG, "Exception in query " + e.getMessage());
            e.printStackTrace();
        }
        return keyphrase;
    }

    /**
     * Returns all the keyphrases in the database. Each keyphrase is represented as a row in this
     * database.
     *
     * @return
     */
    public List<KeyPhrase> getKeyPhrases() {
        List<KeyPhrase> list = new ArrayList<KeyPhrase>();

        try {
            // Select All Query
            String selectQuery = "SELECT  * FROM " + TableEntry.TABLE_NAME;

            SQLiteDatabase db = dbHelper.getReadableDatabase();

            String[] array = new String[columnMap.size()];
            columnMap.keySet().toArray(array);
            Cursor cursor = db.query(TableEntry.TABLE_NAME, array, null, null, null, null,
                                     null);

            // looping through all rows and adding to list
            if (cursor.moveToFirst()) {
                do {
                    KeyPhrase keyphrase = extractKeyPhrase(cursor);
                    if (keyphrase != null) {
                        list.add(keyphrase);
                    }
                } while (cursor.moveToNext());
            }

            cursor.close();
            db.close();

        } catch (Exception e) {
            Log.d(TAG, "Exception in getKeyPhrases " + e.getMessage());
            e.printStackTrace();
        }
        return list;
    }

    /**
     * Find the particular row with given key and return Cursor object.
     * It is better to request a KeyPhrase via the getKeyPhrase() method.
     * The caller of this method needs to close the cursor when done with it.
     *
     * @param key
     * @param columns
     * @return
     */
    public Cursor query(String key, String[] columns) {
        try {

            return query(columns, TableEntry.COLUMN_ENTRY_KEY, new String[] {key});

        } catch (Exception e) {
            Log.d(TAG, "Exception in query " + e.getMessage());
            e.printStackTrace();
        }
        return null;
    }

    /**
     * Find the particular row with given key and return Cursor object.
     * It is better to request a KeyPhrase via the getKeyPhrase() method.
     * The caller of this method needs to close the cursor when done with it.
     *
     * @param columns
     * @param selection
     * @param selectionArgs
     * @return
     */
    public Cursor query(String[] columns, String selection,
                        String[] selectionArgs) {
        Cursor c = null;

        try {
            SQLiteDatabase db = dbHelper.getReadableDatabase();
            c = db.query(TableEntry.TABLE_NAME, columns, selection, selectionArgs, null, null,
                         null);
            db.close();
        } catch (Exception e) {
            Log.d(TAG, "Exception in query " + e.getMessage());
            e.printStackTrace();
        }

        return c;
    }


    /**
     * The Class DatabaseHelper. This is a helper class to help manager database
     * creation and version management.
     */
    private static class DatabaseHelper extends SQLiteOpenHelper {

        /**
         * Instantiates a new database helper.
         *
         * @param context the context
         */
        DatabaseHelper(Context context) {
            super(context, DB_NAME, null, DB_VERSION);
        }

        /*
         * (non-Javadoc)
         *
         * @see
         * android.database.sqlite.SQLiteOpenHelper#onCreate(android.database
         * .sqlite.SQLiteDatabase)
         */
        @Override
        public void onCreate(SQLiteDatabase db) {
            Log.d(TAG, "Creating the database table.");

            // Create the table creation query.
            // This query creates a table with name TABLE_NAME, with
            // unique row ids that auto increment and columns that are defined
            // in TableEntry class.
            // Make sure this table matches the map projection order.
            // Insert new entries at the end and not in the middle of the table as the
            // column numbers will no longer be backwards compatible.
            db.execSQL("CREATE TABLE " + TableEntry.TABLE_NAME + " ("
                       + BaseColumns._ID + " INTEGER PRIMARY KEY AUTOINCREMENT ,"
                       + TableEntry.COLUMN_ENTRY_KEY + " TEXT,"
                       + TableEntry.COLUMN_KEYPHRASE_NAME + " TEXT, "
                       + TableEntry.COLUMN_KEYPHRASE + " TEXT,"
                       + TableEntry.COLUMN_THRESHOLD + " INTEGER,"
                       + TableEntry.COLUMN_ACTION + " INTEGER,"
                       + TableEntry.COLUMN_MATCH_SUCCESS + " INTEGER,"
                       + TableEntry.COLUMN_MATCH_FAILED + " INTEGER,"
                       + TableEntry.COLUMN_MATCH_FALSE_POSITIVE + " INTEGER"
                       + ");");
        }

        /*
         * (non-Javadoc)
         *
         * @see
         * android.database.sqlite.SQLiteOpenHelper#onUpgrade(android.database
         * .sqlite.SQLiteDatabase, int, int)
         */
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            Log.w(TAG, "Upgrading table " + TableEntry.TABLE_NAME
                  + " from version " + oldVersion + " to " + newVersion
                  + ". All data will be lost.");

            // Delete the existing table and replace with the new one.
            // Replace this method if want to persist data.
            db.execSQL("DROP TABLE IF EXISTS " + TableEntry.TABLE_NAME);
            onCreate(db);
        }
    }

}
