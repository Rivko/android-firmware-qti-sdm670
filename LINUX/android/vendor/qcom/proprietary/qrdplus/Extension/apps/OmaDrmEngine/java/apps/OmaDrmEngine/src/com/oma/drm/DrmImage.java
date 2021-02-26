/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm;

import android.graphics.Bitmap;
import android.net.Uri;

public class DrmImage extends DrmMedia {

    private Bitmap mBitmap;
    private OmaDrmInfo mOmaDrmInfo;

    public DrmImage(Bitmap bitmap, Uri uri, OmaDrmInfo omaDrmInfo) {
        super(uri);
        mBitmap = bitmap;
        mOmaDrmInfo = omaDrmInfo;
    }

    public Bitmap getBitmap() {
        return mBitmap;
    }

    public OmaDrmInfo getDrmInfo() {
        return mOmaDrmInfo;
    }
}
