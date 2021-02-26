/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.highlights;

/**
 * Created by mithun on 9/29/15.
 */
public class CreatingStatus {

    public int currentUriIndex;
    public int maxUris;
    public int progress;

    public CreatingStatus() {

        currentUriIndex = -1;
        maxUris = -1;
        progress = -1;
    }

    public CreatingStatus(int currentUriIndex, int maxUris, int progress) {
        this.currentUriIndex = currentUriIndex;
        this.maxUris = maxUris;
        this.progress = progress;
    }
}
