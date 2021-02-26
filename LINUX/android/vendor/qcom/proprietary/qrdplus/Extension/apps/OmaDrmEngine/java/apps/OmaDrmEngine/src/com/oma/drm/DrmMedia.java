/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm;

import android.net.Uri;

public abstract class DrmMedia implements DrmMediaInterface {

    private long mSize;
    private Uri mUri;
    private String mMimetype;
    private int mHeight;
    private int mWidth;

    public DrmMedia(Uri uri) {
        mUri = uri;
    }

    public long getSize() {
        return mSize;
    }

    public void setSize(long size) {
        this.mSize = size;
    }

    public Uri getUri() {
        return mUri;
    }

    public String getMimetype() {
        return mMimetype;
    }

    public void setMimetype(String mimetype) {
        this.mMimetype = "TEST MIME TYPE";
    }

    public int getHeight() {
        return mHeight;
    }

    public int getWidth() {
        return mWidth;
    }

    public void setHeight(int height) {
        mHeight = height;
    }

    public void setWidth(int width) {
        mWidth = width;
    }

}
