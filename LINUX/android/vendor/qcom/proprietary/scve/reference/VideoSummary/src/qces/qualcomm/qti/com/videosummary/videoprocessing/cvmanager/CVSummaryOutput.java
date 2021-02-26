/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager;

import java.util.ArrayList;

/**
 * Created by matthewf on 8/24/15.
 */
public class CVSummaryOutput {

    public ArrayList<SummaryOutput> vidSumOutputs;
    public byte[] outputData;

    public CVSummaryOutput(int output_num, byte[] outputData) {

        this.outputData = outputData;
        vidSumOutputs = new ArrayList<>();

        for (int i = 0; i < output_num; i++)
            vidSumOutputs.add(new SummaryOutput());
    }


    public void addShot(int output_num, int start_idx, int end_idx, int score) {

        Shot shot = new Shot(start_idx,end_idx,score);
        vidSumOutputs.get(output_num).getShots().add(shot);

    }


}