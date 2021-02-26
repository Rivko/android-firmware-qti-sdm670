/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm;

import android.net.Uri;

public interface DrmMediaInterface {

    int INVALID_DIMENSION = 0;

    public long getSize();

    public void setSize(long size);

    public Uri getUri();

    public String getMimetype();

    public void setMimetype(String mimetype);

    public int getWidth();

    public int getHeight();

}
