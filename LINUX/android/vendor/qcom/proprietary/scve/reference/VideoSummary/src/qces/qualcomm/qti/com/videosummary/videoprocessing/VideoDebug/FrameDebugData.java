/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoDebug;

import android.graphics.Bitmap;

/**
 * Created by matthewf on 8/3/15.
 */
public class FrameDebugData {

    public Bitmap bmp;
    public int[] scores;
    public int frame_number;

    public FrameDebugData(Bitmap bmp, int[] scores, int frame_number) {

        this.bmp = bmp;
        this.scores = scores;
        this.frame_number = frame_number;
    }


}
