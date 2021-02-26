/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager;

import java.util.ArrayList;

public class SummaryOutput {

    private ArrayList<Shot> shots;

    public ArrayList<Shot> getShots() {
        return shots;
    }

    public SummaryOutput() {

        shots = new ArrayList<>();

    }

}


