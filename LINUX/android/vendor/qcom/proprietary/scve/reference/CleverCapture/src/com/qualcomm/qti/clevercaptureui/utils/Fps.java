/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.utils;

public class Fps {
    private static final int FPS_MAX_SAMPLE_NUM = 10;
    private int fps_index = 0;
    private int fps_sum = 0;
    private int fps_sample_num = 0;
    private int[] fps_ticklist = new int[FPS_MAX_SAMPLE_NUM];

    private long lastTime;
    private long lastUpdateTime;
    private float fps_value;

    public interface Listener {
        public void refreshUI(float fps);
    }

    private Listener listener;

    public Fps(Listener l) {
        listener = l;
    }

    public void updatefps() {
        if(lastTime == 0) {
            reset();
        } else {
            long curTime = System.currentTimeMillis();

            fps_sum -= fps_ticklist[fps_index];
            fps_ticklist[fps_index] = (int) (curTime - lastTime);
            fps_sum += fps_ticklist[fps_index];
            fps_index++;

            if(fps_sample_num < FPS_MAX_SAMPLE_NUM)
                fps_sample_num++;

            if(fps_index == FPS_MAX_SAMPLE_NUM)
                fps_index = 0;

            fps_value = 1000f * fps_sample_num / fps_sum;
            if(curTime - lastUpdateTime > 1000) {
                if(listener != null) {
                    listener.refreshUI(fps_value);
                }
                lastUpdateTime = curTime;
            }
            lastTime = curTime;
        }
    }

    public float get() {
        return fps_value;
    }

    public void reset() {
        lastTime = System.currentTimeMillis();
        lastUpdateTime = lastTime;
        fps_value = 0;

        for(int i = 0; i < FPS_MAX_SAMPLE_NUM; i++)
            fps_ticklist[i] = 0;
        fps_index = 0;
        fps_sum = 0;
        fps_sample_num = 0;

        if(listener != null) {
            listener.refreshUI(fps_value);
        }
    }
}
