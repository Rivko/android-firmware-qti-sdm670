/*
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.csm.backup;

import android.app.backup.BackupAgentHelper;
import android.app.backup.FullBackupDataOutput;
import android.content.SharedPreferences;
import android.os.ParcelFileDescriptor;
import android.util.Log;

import java.io.File;
import java.io.IOException;

import com.qti.csm.backup.mms.QTIBackupMMS;
import com.qti.csm.backup.mms.QTIBackupSMS;

public class QtiBackupAgent extends BackupAgentHelper {
    private final boolean DEBUG = true;
    static final String SMS = "smsBackup";
    static final String MMS = "mmsBackup";
    static final private String TAG = "QtiBackupAgent";
    SharedPreferences settings;
    SharedPreferences.Editor editor;

    private void doBackup() {

        //backup contacts
        if (DEBUG) Log.d(TAG, "Starting contacts backup.");
        QtiBackupContacts contactsBackup = new QtiBackupContacts(this, QtiBackupActivity.CONTACTS_FILE);
        contactsBackup.backupContacts();
        QTIBackupSMS smsBackup = new QTIBackupSMS(this, SMS);
        smsBackup.performBackup();
        QTIBackupMMS mmsBackup = new QTIBackupMMS(this, MMS);
        mmsBackup.performBackup();
    }

    private void doRestore(File file) {
        //Restore contacts
        if (DEBUG) Log.d(TAG, "Starting contacts restore.");
        String contactsFile = QtiBackupActivity.CONTACTS_FILE;
        if(contactsFile.equals(file.getName())) {
            QtiBackupContacts contactsBackup = new QtiBackupContacts(this, file.getName());
            contactsBackup.restoreContacts(file);
        }
    }

    @Override
        public void onFullBackup(FullBackupDataOutput data) throws IOException {
            doBackup();
            super.onFullBackup(data);
        }

    @Override
        public void onRestoreFile(ParcelFileDescriptor data, long size,
                File destination, int type, long mode, long mtime)
        throws IOException {
            //Skip restoring user settings for backup app.
            String settingsFile = QtiBackupActivity.PREFS_NAME+".xml";
            if(settingsFile.equals(destination.getName()))
                return;

            super.onRestoreFile(data, size, destination, type, mode, mtime);
            doRestore(destination);
            if(destination.getName().equalsIgnoreCase(MMS)){
                QTIBackupMMS mmsBackup = new QTIBackupMMS(this, MMS);
                mmsBackup.performRestore();
            }
            if(destination.getName().equalsIgnoreCase(SMS)){
                QTIBackupSMS smsBackup = new QTIBackupSMS(this, SMS);
                smsBackup.performRestore();
            }
        }
}
