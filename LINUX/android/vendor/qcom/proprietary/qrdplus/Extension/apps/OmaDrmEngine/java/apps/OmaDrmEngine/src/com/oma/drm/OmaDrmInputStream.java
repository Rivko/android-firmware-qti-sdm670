/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.ContentResolver;
import android.content.Context;
import android.media.MediaDrmException;
import android.net.Uri;
import android.util.Log;

import com.oma.drm.util.OmaDrmUtils;

public class OmaDrmInputStream extends InputStream {
    private static final String TAG = "OmaDrmInputStream";
    private static final int UNIQUE_ID_NOT_INITIALIZED = -1;
    private static final int DRM_DUMP_BYTE_SIZE = 4096;
    private static final boolean IS_DRM_USES_REDIRECTED_INPUT_STREAM = true;

    private static int sSeqUniqueId = 0;

    private int mUniqueId = UNIQUE_ID_NOT_INITIALIZED;
    private Context mContext;
    private Uri mUri;
    private boolean mConsumeRights;
    private FileDescriptor mFD;
    private long mFileSize;
    private String mDumpFilePath;
    FileInputStream mRedirectedInputStream;
    private File tmpFile;
    OmaDrmInfo mOmaDrmInfo;

    /**
     *
     * This constructor call may blocks UI thread, use it carefully.
     *
     * @param context
     * @param uri
     * @param consumeRights
     * @throws FileNotFoundException
     */
    public OmaDrmInputStream(Context context, Uri uri, boolean consumeRights)
            throws FileNotFoundException, MediaDrmException {
        mContext = context;
        mUri = uri;
        mConsumeRights = consumeRights;

        ContentResolver resolver = context.getContentResolver();
        try {
            mFD = resolver.openAssetFileDescriptor(mUri, "r")
                    .getFileDescriptor();
        } catch (FileNotFoundException e) {
            Log.e(TAG, "Error while FD from uri = " + mUri + ", exception ! "
                    + e);
            throw e;
        }
        if (mFD != null) {
            mUniqueId = generateUniqueId();
            int result = OmaDrmUtils.initializeInputStream(mUniqueId,
                    mFD, mConsumeRights);
            if (result < 0) {
                try {
                    close();
                } catch(IOException e) {
                    Log.e(TAG, "Eception while closing OmaDrmInputStream. " + e);
                }
                throw new MediaDrmException("MediaDrmException! May Rights invalid!");
            }
        }

        mFileSize = OmaDrmUtils.getFileSizeFromOmaDrmInputStream(mUniqueId);

        mOmaDrmInfo = (OmaDrmInfo) OmaDrmUtils
                .getOmaDrmInfoOmaDrmInputStream(mUniqueId);

        if (IS_DRM_USES_REDIRECTED_INPUT_STREAM) {
            prepareRedirectedInputStream();
        }

    }

    private void prepareRedirectedInputStream() {
        // TODO : Fixit - workaround
        // dump drm decrypted file into application cache folder
        try {
            mRedirectedInputStream = new FileInputStream(new File(
                    creadeDrmDumpFile()));
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    private void deleteDrmDumpFile() {
        if (mDumpFilePath == null) {
            return;
        }
        File file = new File(mDumpFilePath);
        if (file.exists()) {
            file.delete();
        }
    }

    public File getTmpFile() {
        return tmpFile;
    }

    private String creadeDrmDumpFile() {
        // TODO : Dump drm file into application cache
        // It will delete immediately once input stream closed.
        byte bytes[] = new byte[DRM_DUMP_BYTE_SIZE];
        int offset = 0;
        FileOutputStream fos = null;
        int length;
        File cacheFolder = mContext.getCacheDir();

        try {
            File file = new File(cacheFolder, System.currentTimeMillis()
                    + ".file");
            if (file.exists()) {
                file.delete();
            }
            file.createNewFile();
            tmpFile = file;
            mDumpFilePath = file.getAbsolutePath();
            fos = new FileOutputStream(file);

            while ((length = OmaDrmUtils.readFromOmaDrmInputStream(mUniqueId,
                    bytes, DRM_DUMP_BYTE_SIZE, offset)) > 0) {
                fos.write(bytes, 0, length);
                offset += length;
            }
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } finally {
            try {
                if (fos != null) {
                    fos.close();
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

        }
        return mDumpFilePath;
    }

    private synchronized int generateUniqueId() {
        int id = sSeqUniqueId++;
        if (sSeqUniqueId >= Integer.MAX_VALUE) {
            sSeqUniqueId = 0;
        }
        return id;
    }

    @Override
    public int read() throws IOException {
        if (mUniqueId == UNIQUE_ID_NOT_INITIALIZED) {
            throw new IOException(
                    "DrmInputStream has not been initialized! mUniqueId = "
                            + mUniqueId);
        }
        int result = -1;
        byte[] buffer = new byte[1];
        if (IS_DRM_USES_REDIRECTED_INPUT_STREAM) {
            result = mRedirectedInputStream.read();
        } else {
            OmaDrmUtils.readFromOmaDrmInputStream(mUniqueId, buffer, 1, 0);
            result = buffer[0];
        }
        return result;
    }

    @Override
    public int read(byte[] buffer, int offset, int numBytes) throws IOException {
        if (mUniqueId == UNIQUE_ID_NOT_INITIALIZED) {
            throw new IOException(
                    "DrmInputStream has not been initialized! mUniqueId = "
                            + mUniqueId);
        }
        int length = -1;

        if (IS_DRM_USES_REDIRECTED_INPUT_STREAM) {
            length = mRedirectedInputStream.read(buffer, offset, numBytes);
        } else {
            length = OmaDrmUtils.readFromOmaDrmInputStream(mUniqueId, buffer,
                    numBytes, offset);
        }
        return length;
    }

    @Override
    public void close() throws IOException {
        OmaDrmUtils.releaseInputStream(mUniqueId);
        deleteDrmDumpFile();
    }

    public long size() {
        return this.mFileSize;
    }

    public OmaDrmInfo getOmaDrmInfo() {

        return mOmaDrmInfo;
    }
}
