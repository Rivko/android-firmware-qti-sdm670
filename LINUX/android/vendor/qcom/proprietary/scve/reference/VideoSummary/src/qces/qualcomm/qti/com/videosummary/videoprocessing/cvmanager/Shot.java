/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager;

/**
 * Created by matthewf on 8/26/15.
 */
public class Shot {
    public int seg_start_frame;
    public int seg_end_frame;
    public int seg_score;

    public Shot(int seg_start_frame, int seg_end_frame, int seg_score) {
        this.seg_start_frame = seg_start_frame;
        this.seg_end_frame = seg_end_frame;
        this.seg_score = seg_score;
    }
}