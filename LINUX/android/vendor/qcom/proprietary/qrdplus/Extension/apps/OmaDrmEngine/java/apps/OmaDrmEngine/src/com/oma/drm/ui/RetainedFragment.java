/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm.ui;

import android.app.Fragment;
import android.os.Bundle;

import com.oma.drm.DrmImage;

/**
 * An entity class used to store Bitmap.
 */

public class RetainedFragment extends Fragment {

    DrmImage mDrmImage;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRetainInstance(true);
    }

    void setDrmImage(DrmImage drmImage) {
        mDrmImage = drmImage;
    }

    DrmImage getDrmImage() {
        return mDrmImage;
    }
}
