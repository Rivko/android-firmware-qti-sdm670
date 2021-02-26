/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.fr;

import android.content.ContentResolver;
import android.content.Context;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.provider.ContactsContract;
import android.util.ArrayMap;
import android.util.Base64;
import android.util.Base64InputStream;
import android.util.Base64OutputStream;
import android.util.Log;
import android.util.LruCache;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Map;
import java.util.Objects;

/**
 * A simple, {@link SharedPreferences}-backed implementation of a user database. It implements
 * caching for high-performance access to the users in the database.
 */
public class UserDatabase {
    /** The supported sharing methods which can be set for users in this database */
    public enum ShareMethod {
        NONE,
        EMAIL,
        SMS
    }

    private static final String TAG = "SocialCamera";
    private static final String DATABASE_NAME = "fr_user_database";
    private static final String USER_PREFIX = "user_";
    private static final String CONTACT_LOOKUP_PREFIX = "contactlookup_";

    private static UserDatabase instance;

    private final Context context;
    private final SharedPreferences prefs;
    private final LruCache<String, UserRecord> cache = new LruCache<>(50);

    /**
     * Get the singleton instance of the {@link UserDatabase}.
     * <br/><i>NOTE: the singleton instance should only be used on the main application thread</i>
     * @param context the active {@link Context}
     * @return the singleton {@link UserDatabase} instance
     */
    public static UserDatabase getInstance(Context context) {
        Objects.requireNonNull(context, "context may not be null");
        if (instance == null) {
            instance = new UserDatabase(context);
        }
        return instance;
    }

    /**
     * Construct a new {@link UserDatabase}
     * @param context the active {@link Context}
     * @throws NullPointerException if context is null
     */
    private UserDatabase(Context context) {
        Objects.requireNonNull(context, "context may not be null");

        this.context = context;
        prefs = context.getSharedPreferences(DATABASE_NAME, 0);
    }

    /**
     * Gets the {@link UserRecord} for the specified user
     * @param id the ID of the specified user
     * @return a {@link UserRecord} describing the specified user, or null if the user was not found
     *         in the database
     * @throws NullPointerException if id is null
     * @throws IllegalArgumentException if id is an empty String
     */
    public UserRecord getUserRecord(String id) {
        Objects.requireNonNull(id, "id may not be null");
        if (id.isEmpty()) {
            throw new IllegalArgumentException("id may not be empty");
        }

        UserRecord record = cache.get(id);
        if (record == null) {
            record = loadEntry(id);
        }

        return record;
    }

    /**
     * Get all {@link UserRecord}s stored in the {@link UserDatabase}
     * @return a {@link Map} of {@link UserRecord}s, with no defined ordering
     */
    public Map<String, UserRecord> getAllUserRecords() {
        ArrayMap<String, UserRecord> users = new ArrayMap<>();

        Map<String, ?> prefEntries = prefs.getAll();
        for (String prefixedKey : prefEntries.keySet()) {
            if (prefixedKey.startsWith(USER_PREFIX)) {
                String key = prefixedKey.substring(USER_PREFIX.length());
                users.put(key, loadEntry(key));
            }
        }

        return users;
    }

    /**
     * Create a new user and insert them into the {@link UserDatabase}. If a user with this ID
     * already exists, they will be replaced.
     * @param id the ID of the new user
     * @param contactLookupKey the {@link ContactsContract.Contacts#LOOKUP_KEY} value for the user
     * @return true if the user was created successfully, else false
     * @throws NullPointerException if id or contactLookupKey are null
     * @throws IllegalArgumentException if id or contactLookupKey are empty Strings
     */
    public boolean createUser(String id, String contactLookupKey) {
        Objects.requireNonNull(id, "id may not be null");
        Objects.requireNonNull(contactLookupKey, "contactLookupKey may not be null");
        if (id.isEmpty()) {
            throw new IllegalArgumentException("id may not be empty");
        } else if (contactLookupKey.isEmpty()) {
            throw new IllegalArgumentException("contactLookupKey may not be empty");
        }

        final Uri contactUri = ContactsContract.Contacts.lookupContact(context.getContentResolver(),
                Uri.withAppendedPath(ContactsContract.Contacts.CONTENT_LOOKUP_URI,
                        contactLookupKey));
        if (contactUri == null) {
            Log.e(TAG, "Contact with lookup key [" + contactLookupKey + "] not found");
            return false;
        }

        UserRecord record = new UserRecord(context, contactLookupKey);
        cache.put(id, record);
        updateEntry(id, record);

        return true;
    }

    /**
     * Update the specified user in the {@link UserDatabase}
     * @param id the ID of the specified user
     * @param shareMethod the new {@link ShareMethod} for the specified user
     * @return true if the user was successfully updated, else false
     * @throws NullPointerException if id or shareMethod are null
     * @throws IllegalArgumentException if id is an empty String
     */
    public boolean updateUser(String id, ShareMethod shareMethod) {
        Objects.requireNonNull(id, "id may not be null");
        Objects.requireNonNull(shareMethod, "shareMethod may not be null");
        if (id.isEmpty()) {
            throw new IllegalArgumentException("id may not be empty");
        }

        UserRecord record = cache.get(id);
        if (record == null) {
            record = loadEntry(id);
        }
        if (record == null) {
            Log.w(TAG, "No user found for ID [" + id + "]; not updating user");
            return false;
        }

        updateEntry(id, new UserRecord(record, shareMethod));

        return true;
    }

    /**
     * Test if the {@link UserDatabase} contains the specified user
     * @param id the ID of the specified user
     * @return true if the database contains the specified user, else false
     * @throws NullPointerException if id is null
     * @throws IllegalArgumentException if id is an empty String
     */
    public boolean hasUser(String id) {
        Objects.requireNonNull(id, "id may not be null");
        if (id.isEmpty()) {
            throw new IllegalArgumentException("id may not be empty");
        }

        UserRecord record = cache.get(id);
        if (record == null) {
            record = loadEntry(id);
        }

        return (record != null);
    }

    /**
     * Search the {@link UserDatabase} for a user with the specified contact lookup key, matching
     * the contact lookup key provided when the user was created (via
     * {@link #createUser(String, String)})
     * @param contactLookupKey the contact lookup key to search the {@link UserDatabase} for
     * @return the id of the matching user, or null if there is no user in the {@link UserDatabase}
     *         with this contact lookup key
     * @throws IllegalArgumentException if contactLookupKey is null or empty
     */
    public String findUserByContactLookupKey(String contactLookupKey) {
        Objects.requireNonNull(contactLookupKey, "contactLookupKey may not be null");

        if (contactLookupKey.isEmpty()) {
            throw new IllegalArgumentException("contactLookupKey may not be empty");
        }

        return prefs.getString(CONTACT_LOOKUP_PREFIX + contactLookupKey, null);
    }

    /**
     * Clear all users from the {@link UserDatabase}
     */
    public void removeAllUsers() {
        prefs.edit().clear().apply();
        cache.evictAll();
    }

    /**
     * Removes the specified user from the {@link UserDatabase}
     * @param id the ID of the specified user
     * @throws NullPointerException if id is null
     * @throws IllegalArgumentException if id is an empty String
     */
    public void removeUser(String id) {
        Objects.requireNonNull(id, "id may not be null");
        if (id.isEmpty()) {
            throw new IllegalArgumentException("id may not be empty");
        }

        prefs.edit().remove(USER_PREFIX + id).apply();
        cache.remove(id);
    }

    // N.B. record is already in cache, no need to re-add it
    private void updateEntry(String id, UserRecord record) {
        try (
            ByteArrayOutputStream os0 = new ByteArrayOutputStream();
            Base64OutputStream os1 = new Base64OutputStream(os0, Base64.NO_PADDING);
            ObjectOutputStream os = new ObjectOutputStream(os1)
        ) {
            os.writeUTF(record.lookupKey);
            os.writeObject(record.shareMethod);
            os.close();
            prefs.edit()
                    .putString(USER_PREFIX + id, os0.toString())
                    .putString(CONTACT_LOOKUP_PREFIX + record.lookupKey, id)
                    .apply();
        } catch (IOException e) {
            Log.e(TAG, "Failed serializing entry to database", e);
            //noinspection UnnecessaryReturnStatement
            return;
        }
    }

    private UserRecord loadEntry(String id) {
        String entryStr = prefs.getString(USER_PREFIX + id, null);
        if (entryStr == null) {
            int idLength = id.length();
            idLength = idLength > 20 ? 20 : idLength;
            Log.w(TAG, "Entry for ID [" + id + "] not found in database. idLength " + idLength);
            entryStr = "~" + id;

            UserRecord record = new UserRecord(context, entryStr);
            cache.put(id, record);
            return record;

        }
        else
        {
            try (
                ByteArrayInputStream is0 = new ByteArrayInputStream(entryStr.getBytes());
                Base64InputStream is1 = new Base64InputStream(is0, Base64.DEFAULT);
                ObjectInputStream is = new ObjectInputStream(is1)
            ) {
                UserRecord record = new UserRecord(context, is.readUTF(), (ShareMethod)is.readObject());
                cache.put(id, record);
                return record;
            } catch (IOException | ClassNotFoundException e) {
                Log.e(TAG, "Failed deserializing entry from database", e);
                return null;
            } catch (IllegalArgumentException e) {
                Log.e(TAG, "Contact not found", e);
                return null;
            }
        }
    }

    public static class UserRecord {
        /** The set of {@link ShareMethod}s which are valid for this user */
        public final ShareMethod[] supportedShareMethods;

        /** The name of this user */
        public final CharSequence name;

        /** The SMS number for this user */
        public final String sms;

        /** The email address for this user */
        public final String email;

        /** The default {@link ShareMethod} for this user */
        public final ShareMethod shareMethod;

        private final String lookupKey;

        private UserRecord(UserRecord copy, ShareMethod shareMethod) {
            lookupKey = copy.lookupKey;
            supportedShareMethods = copy.supportedShareMethods;
            name = copy.name;
            sms = copy.sms;
            email = copy.email;
            this.shareMethod = shareMethod;
        }

        private UserRecord(Context context, String lookupKey) {
            this(context, lookupKey, null);
        }

        private UserRecord(Context context, String lookupKey, ShareMethod shareMethod) {
            this.lookupKey = lookupKey;

            if(lookupKey.startsWith("~")) {
                name = lookupKey.subSequence(0,21);
                supportedShareMethods = new ShareMethod[] { ShareMethod.NONE };
                sms = null;
                email = null;
                this.shareMethod = shareMethod;
            }
            else {
                final ContentResolver cr = context.getContentResolver();

                final Uri lookupUri = Uri.withAppendedPath(ContactsContract.Contacts.CONTENT_LOOKUP_URI,
                    lookupKey);
                final Uri contactUri = ContactsContract.Contacts.lookupContact(cr, lookupUri);
                if (contactUri == null) {
                    throw new IllegalArgumentException("No contact found for lookup key [" +
                            lookupKey + "]");
                }

                Cursor contactData = cr.query(contactUri,
                        new String[] { ContactsContract.Contacts._ID,
                                ContactsContract.Contacts.DISPLAY_NAME },
                        null, null, null);
                assert contactData != null;
                contactData.moveToFirst();
                final long id = contactData.getLong(0);
                name = contactData.getString(1);
                contactData.close();

                Cursor detailsData = cr.query(ContactsContract.RawContactsEntity.CONTENT_URI,
                        new String[] { ContactsContract.RawContactsEntity.MIMETYPE,
                                ContactsContract.RawContactsEntity.DATA1,
                                ContactsContract.RawContactsEntity.DATA2 },
                        ContactsContract.RawContactsEntity.CONTACT_ID + " = ? AND (" +
                                ContactsContract.RawContactsEntity.MIMETYPE + " = '" +
                                ContactsContract.CommonDataKinds.Email.CONTENT_ITEM_TYPE + "' OR " +
                                ContactsContract.RawContactsEntity.MIMETYPE + " = '" +
                                ContactsContract.CommonDataKinds.Phone.CONTENT_ITEM_TYPE + "')",
                        new String[] { Long.toString(id) },
                        ContactsContract.RawContactsEntity.IS_SUPER_PRIMARY + " DESC, " +
                                ContactsContract.RawContactsEntity.IS_PRIMARY + " DESC," +
                                ContactsContract.RawContactsEntity._ID + " ASC");
                assert detailsData != null;

                String email = null;
                for (int dataType : new int[] {
                        ContactsContract.CommonDataKinds.Email.TYPE_MOBILE,
                        ContactsContract.CommonDataKinds.Email.TYPE_HOME,
                        -1}) {
                    email = selectFromDetails(detailsData, 0,
                            ContactsContract.CommonDataKinds.Email.CONTENT_ITEM_TYPE, 2, dataType, 1);
                }
                this.email = email;

                String sms = null;
                for (int dataType : new int[] {
                        ContactsContract.CommonDataKinds.Phone.TYPE_MMS,
                        ContactsContract.CommonDataKinds.Phone.TYPE_MOBILE,
                        ContactsContract.CommonDataKinds.Phone.TYPE_HOME,
                        -1}) {
                    sms = selectFromDetails(detailsData, 0,
                            ContactsContract.CommonDataKinds.Phone.CONTENT_ITEM_TYPE, 2, dataType, 1);
                }
                this.sms = sms;

                detailsData.close();

                if (email != null || sms != null) {
                    supportedShareMethods = new ShareMethod[(email != null ? 1 : 0) +
                            (sms != null ? 1 : 0)];
                    int i = 0;
                    if (email != null) {
                        supportedShareMethods[i++] = ShareMethod.EMAIL;
                    }
                    if (sms != null) {
                        supportedShareMethods[i] = ShareMethod.SMS;
                    }
                } else {
                    supportedShareMethods = new ShareMethod[] { ShareMethod.NONE };
                }

                // If a share method was specified, make sure it is valid for this user
                if (shareMethod != null) {
                    boolean found = false;
                    for (ShareMethod s : supportedShareMethods) {
                        if (s == shareMethod) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        shareMethod = null;
                    }
                }
                this.shareMethod = (shareMethod != null ? shareMethod : supportedShareMethods[0]);
            }
        }

        private static String selectFromDetails(Cursor c, int mimeTypeCol, String mimeType,
                                              int dataTypeCol, int dataType, int dataCol) {
            c.moveToPosition(-1);
            while (c.moveToNext()) {
                if (mimeType.equals(c.getString(mimeTypeCol)) &&
                        (dataType == -1 || dataType == c.getInt(dataTypeCol))) {
                    return c.getString(dataCol);
                }
            }

            return null;
        }
    }
}
