/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 */
package com.qti.csm.backup.mms;

import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.BaseColumns;
import android.provider.Telephony;
import android.provider.Telephony.Mms;
import android.util.Log;

import com.google.android.mms.MmsException;
import com.google.android.mms.pdu.PduComposer;
import com.google.android.mms.pdu.PduPersister;
import com.google.android.mms.pdu.RetrieveConf;
import com.google.android.mms.pdu.SendReq;
import com.google.android.mms.pdu.PduParser;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.io.StreamCorruptedException;
import java.util.ArrayList;


class QMMSBackup implements Serializable{

    public ArrayList<QTIMMSObject> MMSList;
    public QMMSBackup() {
        MMSList = new ArrayList<QTIMMSObject>();
    }
}

public class QTIBackupMMS {
    Cursor cursor;
    String vfile;
    Context mContext;
    private final boolean DEBUG = true;
    private final String TAG = "QTIBackupMMS";
    QMMSBackup mmsBackupSent, mmsBackupInbox, mmsBackupDraft;
    FileOutputStream mFileOutputStream;
    FileInputStream mFileInputStream;

    private enum MsgBox{
        SENT,
        INBOX,
        DRAFT;
    }

    static final String[] MAILBOX_PROJECTION = new String[] {
        BaseColumns._ID,
            Mms.MESSAGE_BOX,
            Mms.MESSAGE_TYPE,
            Mms.READ,
            Mms.DATE,
            Mms.LOCKED,
            Mms.MESSAGE_SIZE,
            "sub_id",
    };

    public QTIBackupMMS(Context context, String _vfile) {
        mContext = context;
        vfile = _vfile;
        mmsBackupSent = new QMMSBackup();
        mmsBackupInbox = new QMMSBackup();
        mmsBackupDraft = new QMMSBackup();
    }

    public void performBackup(){
        int numOfMmsSent = getMmsList(MsgBox.SENT);
        int numOfMmsInbox = getMmsList(MsgBox.INBOX);
        int numOfMmsDraft = getMmsList(MsgBox.DRAFT);
        writeMmsList(numOfMmsSent, numOfMmsInbox, numOfMmsDraft);
    }

    public void performRestore(){
        QMMSBackup mmsbk;

        try {
            mFileInputStream = mContext.openFileInput(vfile);
            ObjectInputStream ois = new ObjectInputStream(mFileInputStream);

            //For sent messages
            int numOfObjects = ois.readInt();
            mmsbk = (QMMSBackup) ois.readObject();
            if(numOfObjects > 0){
                Log.d(TAG, "No messages to restore in Sent");
            }
            writeMMS(mmsbk, Telephony.Mms.Sent.CONTENT_URI);

            //For inbox messages
            numOfObjects = ois.readInt();
            mmsbk = (QMMSBackup) ois.readObject();
            if(numOfObjects > 0){
                Log.d(TAG, "No messages to restore in Inbox");
            }
            writeMMS(mmsbk, Telephony.Mms.Inbox.CONTENT_URI);

            //For draft messages
            numOfObjects = ois.readInt();
            mmsbk = (QMMSBackup) ois.readObject();
            if(numOfObjects > 0){
                Log.d(TAG, "No messages to restore in Draft");
            }
            writeMMS(mmsbk, Telephony.Mms.Draft.CONTENT_URI);

        } catch (FileNotFoundException e) {
            Log.e(TAG, e.getLocalizedMessage());
        } catch (StreamCorruptedException e) {
            Log.e(TAG, e.getLocalizedMessage());
        } catch (IOException e) {
            Log.e(TAG, e.getLocalizedMessage());
        } catch (ClassNotFoundException e) {
            Log.e(TAG, e.getLocalizedMessage());
        }
    }

    /* Read the deserialized object contents and write them to database.
     * MMS data is also restored using PduPersister.
     */
    private void writeMMS(QMMSBackup mmsbk, Uri uri) {
        ContentResolver cr = mContext.getContentResolver();
        ContentValues values = new ContentValues();
        PduPersister pdup = PduPersister.getPduPersister(mContext);

        for(QTIMMSObject mmsObj : mmsbk.MMSList){

            byte[] Data = mmsObj.getMmData();
            Uri msgUri = null;
            try {
                if (uri.equals(Mms.Inbox.CONTENT_URI)) {
                    RetrieveConf retrieveConf = (RetrieveConf) new PduParser(
                            Data, PduParserUtil.shouldParseContentDisposition()).parse();
                    msgUri = pdup.persist(retrieveConf, uri,true ,false ,null);
                } else {
                    SendReq retrieveConf = (SendReq) new PduParser(Data,
                            PduParserUtil.shouldParseContentDisposition()).parse();
                    msgUri = pdup.persist(retrieveConf, uri,true ,false ,null);
                }
            } catch (MmsException e) {
                Log.e(TAG, e.getLocalizedMessage());
            }
            if(DEBUG) Log.d(TAG, msgUri.toString());

            values.put(Telephony.Mms.Sent.DATE, mmsObj.getDate());
            values.put(Telephony.Mms.Sent.READ, mmsObj.getRead());
            values.put(Telephony.Mms.Sent.LOCKED, mmsObj.getLocked());
            values.put(Telephony.Mms.Sent.MESSAGE_SIZE, mmsObj.getMsg_size());

            try{
                int updatecount =
                    mContext.getContentResolver().update(msgUri, values, null, null);
            } catch (Exception e) {
                Log.e(TAG, e.getLocalizedMessage());
                return;
            }
        }
    }

    /* Serializes mmsBackup object. Writes it to mmsBackup file */
    private void writeMmsList(int numSent, int numInbox, int numDraft) {
        try {
            mFileOutputStream =  mContext.openFileOutput(vfile, Context.MODE_PRIVATE);
            ObjectOutputStream oos = new ObjectOutputStream(mFileOutputStream);
            oos.writeInt(numSent);
            oos.writeObject( mmsBackupSent );
            oos.writeInt(numInbox);
            oos.writeObject( mmsBackupInbox );
            oos.writeInt(numDraft);
            oos.writeObject( mmsBackupDraft );
            oos.flush();
            oos.close();
            mFileOutputStream.close();
        } catch (IOException e) {
            Log.e(TAG, e.getLocalizedMessage());
        }
    }

    private int getMmsList(MsgBox Box){
        // The URI will change depending on what we want to backup.
        // Currently only backing up Sent messages
        int numOfEntries = 0;
        Uri uri;
        Cursor cr;

        switch (Box){
            case SENT:
                uri = Telephony.Mms.Sent.CONTENT_URI;
                cr = mContext.getContentResolver().query(uri, null, null, null, null);
                numOfEntries = BackupMsg(cr, mmsBackupSent);
                if (cr != null) cr.close();
                break;

            case INBOX:
                uri = Telephony.Mms.Inbox.CONTENT_URI;
                cr = mContext.getContentResolver().query(uri, null, null, null, null);
                numOfEntries = BackupMsg(cr, mmsBackupInbox);
                if (cr != null) cr.close();
                break;

            case DRAFT:
                uri = Telephony.Mms.Draft.CONTENT_URI;
                cr = mContext.getContentResolver().query(uri, null, null, null, null);
                numOfEntries = BackupMsg(cr, mmsBackupDraft);
                if (cr != null) cr.close();
                break;
        }

        return numOfEntries;
    }

    /* Iterate over all rows in cursor. Create QTIMMSObject for each row
     * and add them to array list in mmsBacup object.
     * */
    private int BackupMsg(Cursor cr, QMMSBackup mmsBackup) {
        if(cr == null){
            if(DEBUG) Log.d(TAG, "cursor is null");
            return -1;
        }

        if(cr.getCount() == 0){
            if(DEBUG) Log.d(TAG, "No messages");
            return 0;
        }

        QTIMMSObject mmsObj;
        int numOfEntries = 0;

        cr.moveToFirst();
        for (int i = 0; ; i++) {
            mmsObj = exportMms(cr);
            mmsBackup.MMSList.add(mmsObj);
            numOfEntries++;

            if(DEBUG) Log.d(TAG, "MMS " + (i + 1) + mmsBackup.MMSList.get(i));
            if(cr.isLast()) {
                break;
            } else {
                cr.moveToNext();
            }
        }
        return numOfEntries;
    }

    /* Read MMS.Sent table contents to object. */
    private QTIMMSObject exportMms(Cursor c) {
        QTIMMSObject mms = new QTIMMSObject();

        mms.setId(c.getString(c.getColumnIndexOrThrow(Telephony.Mms.Sent._ID)));
        mms.setDate(c.getString(c.getColumnIndexOrThrow(Telephony.Mms.Sent.DATE)));
        mms.setLocked(c.getString(c.getColumnIndexOrThrow(Telephony.Mms.Sent.LOCKED)));
        mms.setMsg_box(c.getString(c.getColumnIndexOrThrow(Telephony.Mms.Sent.MESSAGE_BOX)));
        mms.setMsg_size(c.getString(c.getColumnIndexOrThrow(Telephony.Mms.Sent.MESSAGE_SIZE)));
        mms.setMsg_type(c.getString(c.getColumnIndexOrThrow(Telephony.Mms.Sent.MESSAGE_TYPE)));
        mms.setRead(c.getString(c.getColumnIndexOrThrow(Telephony.Mms.Sent.READ)));

        //Getting pdu content
        Uri uri = ContentUris.withAppendedId(Mms.CONTENT_URI, Long.parseLong( mms.getId() ));
        PduPersister persister = PduPersister.getPduPersister(mContext);
        SendReq sendReq = null;
        RetrieveConf retrieveConf = null;
        byte[] mmData = null;
        PduComposer composer = null;

        if(Telephony.Mms.MESSAGE_BOX_INBOX == Long.parseLong ( mms.getMsg_box())) {
            try{
                retrieveConf = (RetrieveConf) persister.load(uri);
                composer = new PduComposer(mContext, retrieveConf);
                mmData = composer.make();
            } catch(MmsException e){
                Log.e(TAG, e.getLocalizedMessage());
            } catch(ClassCastException e){
                Log.e(TAG, e.getLocalizedMessage());
            } catch(Exception e){
                Log.e(TAG, e.getLocalizedMessage());
            }
        } else{
            try {
                sendReq = (SendReq) persister.load(uri);
                mmData = new PduComposer(mContext, sendReq).make();
            } catch(MmsException e){
                Log.e(TAG, e.getLocalizedMessage());
            }
        }

        mms.setMmData(mmData);
        return mms;
    }
}
