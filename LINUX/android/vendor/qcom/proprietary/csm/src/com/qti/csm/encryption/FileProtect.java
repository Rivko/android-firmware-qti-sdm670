/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;

import com.qti.csm.R;
import com.qti.csm.utils.UtilsConvert;
import com.qti.csm.utils.UtilsFileOperation;
import com.qti.csm.utils.UtilsStrings;
import com.qti.csm.utils.UtilsSystem;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.spec.SecretKeySpec;

import static com.qti.csm.encryption.system.ProcessInfo.logd;
import static com.qti.csm.encryption.system.ProcessInfo.loge;
import static com.qti.csm.encryption.system.ProcessInfo.toast;
import static com.qti.csm.encryption.system.SystemInfo.MODE_DECRYPT;
import static com.qti.csm.encryption.system.SystemInfo.MODE_ENCRYPT;

public class FileProtect extends AsyncTask<String, String, Boolean>
        implements DialogInterface.OnCancelListener {

    Activity mAttachedActivity;
    private ProgressDialog mProgressDialog;
    static final int HEADER_SIZE = 1023;
    static final int AES_BLOCK_SIZE = 16;
    static final int BUFFER_SIZE = 4096;
    int mMode = MODE_ENCRYPT;

    final static boolean HEADER = true;

    @Override
    public void onCancel(DialogInterface dialog) {
        logd("");
        cancel(true);
    }

    @Override
    protected void onPostExecute(Boolean result) {
        logd("");
        if (!result) {
            logd("failed");
            toast(mAttachedActivity,
                    mAttachedActivity.getString(R.string.failed));
        }

        ((FileEncrypt) mAttachedActivity).mFileExplorerFragment
                .refreshExplorer();
        if (mProgressDialog != null && mProgressDialog.isShowing()) {
            try {
                mProgressDialog.dismiss();
            } catch (Exception e) {
                loge(e);
            }
            mProgressDialog = null;
        }

        mAttachedActivity = null;
    }

    @Override
    protected void onPreExecute() {

        logd("");
        mProgressDialog = new ProgressDialog(mAttachedActivity);
        mProgressDialog.setIcon(android.R.drawable.ic_dialog_info);
        if (mMode == MODE_DECRYPT) {
            mProgressDialog.setTitle(com.qti.csm.R.string.decrypt);
            mProgressDialog.setMessage(mAttachedActivity
                    .getString(com.qti.csm.R.string.decrypting));
        } else {
            mProgressDialog.setTitle(com.qti.csm.R.string.encrypt);
            mProgressDialog.setMessage(mAttachedActivity
                    .getString(com.qti.csm.R.string.encrypting));
        }
        mProgressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        mProgressDialog.setOnCancelListener(this);
        mProgressDialog.setCanceledOnTouchOutside(false);
        mProgressDialog.setCancelable(false);
        mProgressDialog.show();
    }

    public FileProtect(Activity activity, int mode) {
        mAttachedActivity = activity;
        mMode = mode;
    }

    private boolean decrytFile(String inPath, String outPath) {
        File inFile = new File(inPath);
        File outFile = new File(outPath);
        if (outFile.exists()) {
            outPath = UtilsStrings.getFileNameWithoutPostfix(outPath)
                    + " - Copy" + UtilsStrings.getFileType(outFile.getName());
            outFile = new File(outPath);
        }

        FileInputStream fileInputStream = null;
        FileOutputStream fileOutputStream = null;
        EncInfo fileDescriptor = new EncInfo();

        try {
            fileInputStream = new FileInputStream(inFile);
            fileOutputStream = new FileOutputStream(outFile);

            byte[] header = new byte[roundUpBlockSize(HEADER_SIZE)];
            if (HEADER) {
                fileInputStream.read(header);
                logd("read header size=" + header.length);
                header = encrypt(header, getCipher(Cipher.DECRYPT_MODE));
                fileDescriptor = (EncInfo) UtilsConvert
                        .ByteToObject(header);
                if (fileDescriptor != null) {
                    logd(fileDescriptor);
                    if (!"EncryptOption".equals(fileDescriptor.magic))
                        return false;
                } else {
                    loge("Broken file!");
                    return false;
                }
            }

            int cnt;
            byte[] buf = new byte[roundUpBlockSize(BUFFER_SIZE)];
            while ((cnt = fileInputStream.read(buf)) >= 0) {
                byte[] rawData = buf;
                if (cnt != buf.length)
                    rawData = UtilsConvert.subBytes(buf, 0, cnt);
                byte[] decryptData = encrypt(rawData,
                        getCipher(Cipher.DECRYPT_MODE));
                if (decryptData != null)
                    fileOutputStream.write(decryptData, 0, decryptData.length);
                else
                    return false;
            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return false;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        } finally {

            if (fileInputStream != null) {
                try {
                    fileInputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                    return false;
                }
            }
            if (fileOutputStream != null) {
                try {
                    fileOutputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                    return false;
                }
            }

        }
        return true;
    }

    private SecretKeySpec mSecretKeySpec = null;

    private SecretKeySpec getSecretKeySpec() {

        if (mSecretKeySpec == null) {
            String defaultKey = "default";
            String temp = defaultKey + "1234567890123456";
            temp = temp.substring(0, 16);
            byte[] rawKey = temp.getBytes();
            mSecretKeySpec = new SecretKeySpec(rawKey, "AES");
        }
        return mSecretKeySpec;
    }

    private byte[] makeHeader(Object obj) {

        byte[] header = new byte[HEADER_SIZE];
        byte[] fileDescriptor = UtilsConvert.ObjectToByte(obj);
        logd(fileDescriptor.length);
        System.arraycopy(fileDescriptor, 0, header, 0, fileDescriptor.length);
        return header;
    }

    private boolean encrytFile(String inPath, String outPath) {
        File inFile = new File(inPath);
        if (!inFile.exists())
            return false;

        File outFile = new File(outPath);
        if (outFile.exists()) {
            outPath = UtilsStrings.getFileNameWithoutPostfix(outPath)
                    + " - Copy" + UtilsStrings.getFileType(outFile.getName());
            outFile = new File(outPath);
        }

        FileInputStream fileInputStream = null;
        FileOutputStream fileOutputStream = null;
        EncInfo fileDescriptor = new EncInfo();

        try {
            fileInputStream = new FileInputStream(inFile);
            fileOutputStream = new FileOutputStream(outFile);

            fileDescriptor.fileName = inFile.getName();
            fileDescriptor.size = fileInputStream.available();

            if (HEADER) {
                byte[] tmp;
                tmp = makeHeader(fileDescriptor);
                byte[] header = tmp;
                tmp = encrypt(tmp, getCipher(Cipher.ENCRYPT_MODE));
                header = UtilsConvert.subBytes(tmp, 0, tmp.length);
                fileOutputStream.write(header);
            }

            int cnt;
            byte[] buf = new byte[BUFFER_SIZE];
            while ((cnt = fileInputStream.read(buf)) >= 0) {
                byte[] rawData = buf;
                if (cnt != buf.length)
                    rawData = UtilsConvert.subBytes(buf, 0, cnt);
                byte[] encryptData = encrypt(rawData,
                        getCipher(Cipher.ENCRYPT_MODE));
                fileOutputStream.write(encryptData, 0, encryptData.length);
            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return false;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        } finally {
            if (fileInputStream != null) {
                try {
                    fileInputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                    return false;
                }
            }
            if (fileOutputStream != null) {
                try {
                    fileOutputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                    return false;
                }
            }
        }
        return true;
    }

    private int roundUpBlockSize(int bufferSize) {
        int size = ((bufferSize) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
        return size;
    }

    private Cipher mEncrytCipher = null;
    private Cipher mDecrytCipher = null;

    private Cipher getCipher(int mode) {

        Cipher cipher = null;
        try {
            switch (mode) {
                case Cipher.ENCRYPT_MODE:
                    if (mEncrytCipher == null) {
                        mEncrytCipher = Cipher.getInstance("AES");
                        mEncrytCipher.init(Cipher.ENCRYPT_MODE, getSecretKeySpec());
                    }
                    cipher = mEncrytCipher;
                    break;
                case Cipher.DECRYPT_MODE:
                    if (mDecrytCipher == null) {
                        mDecrytCipher = Cipher.getInstance("AES");
                        mDecrytCipher.init(Cipher.DECRYPT_MODE, getSecretKeySpec());
                    }
                    cipher = mDecrytCipher;
                    break;
                default:
                    return null;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return cipher;
    }

    public byte[] encrypt(byte[] rawValue, Cipher cipher) {

        if (cipher == null) {
            return null;
        }
        byte[] encrypted = null;
        try {
            encrypted = cipher.doFinal(rawValue);
        } catch (IllegalBlockSizeException e) {
            e.printStackTrace();
        } catch (BadPaddingException e) {
            e.printStackTrace();
        }
        return encrypted;
    }

    @Override
    protected Boolean doInBackground(String... params) {
        boolean ret = false;
        String inPath = params[0];
        String outPath = params[1];
        File inFile = new File(inPath);
        File outFile = new File(outPath);
        if (mMode == MODE_DECRYPT) {
            ret = decrytFile(inPath, outPath);
            if (ret && UtilsSystem.isImageFile(outPath)) {
                Uri uri = Uri.fromFile(outFile);
                mAttachedActivity.sendBroadcast(new Intent(
                        Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, uri));
            }
        } else {
            ret = encrytFile(inPath, outPath);
            if (ret && UtilsSystem.isImageFile(inPath)) {
                Uri uri = Uri.fromFile(inFile);
                mAttachedActivity.sendBroadcast(new Intent(
                        Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, uri));
            }
        }
        if (ret) {
            UtilsFileOperation.removeFile(inPath);
        }
        return ret;
    }
}
