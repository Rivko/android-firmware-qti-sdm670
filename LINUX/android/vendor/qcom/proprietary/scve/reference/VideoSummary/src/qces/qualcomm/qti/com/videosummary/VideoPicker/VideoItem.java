/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.VideoPicker;

import android.graphics.Bitmap;

import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;

/**
 * Created by matthewf on 8/18/15.
 */
public class VideoItem {

    public Bitmap thumbBitmap;

    public boolean selected;

    MediaInfo info;

    public VideoItem() {
        info = new MediaInfo();
    }
}
