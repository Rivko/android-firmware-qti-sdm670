/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.csm.backup;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.net.Uri;
import android.os.Environment;
import android.provider.ContactsContract;
import android.util.Log;

import java.io.File;
import java.io.IOException;

import com.qti.csm.backup.vcard.ExportProcessor;
import com.qti.csm.backup.vcard.ExportRequest;
import com.qti.csm.backup.vcard.ExportVCard;
import com.qti.csm.backup.vcard.ImportProcessor;
import com.qti.csm.backup.vcard.ImportRequest;
import com.qti.csm.backup.vcard.ImportVCard;
import com.android.vcard.exception.VCardException;

public class QtiBackupContacts {
    String mfile;
    Context mContext;
    private final boolean DEBUG = false;
    private final String TAG = "QtiBackupContacts";

    public QtiBackupContacts(Context context, String vfile) {
        mContext = context;
        mfile = vfile;
    }

    public void backupContacts() {
        exportRequest(mContext.getFilesDir()+ "/" + mfile);
    }

    private void exportRequest(String path) {
        final ExportRequest request;
        request = ExportVCard.constructExportRequest(path);
        ExportProcessor ep = new ExportProcessor(mContext, request);
        ep.run();
    }

    public void restoreContacts(File file) {
        importRequest(Uri.fromFile(file));
    }

    private void importRequest(Uri uri) {
        final ImportRequest request;
        try{
            request = ImportVCard.constructImportRequest(mContext, null, uri, null);
            ImportProcessor ip= new ImportProcessor(mContext, request);
            ip.run();
        }
        catch (VCardException e) {
            if (DEBUG) Log.w(TAG, e.getLocalizedMessage());
        } catch (IOException e) {
            if (DEBUG) Log.w(TAG, e.getLocalizedMessage());
        }
    }
}
