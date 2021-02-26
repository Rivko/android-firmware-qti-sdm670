/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva.ux.user;

import android.content.Context;
import android.util.Log;
import android.widget.ImageView;

import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.R;


public class RecordingCounter {
    private static final String TAG = "ListenLog.RecordingCounter";
    private final static int MAX_ITEM_COUNT = 5;
    private ImageView[] views;
    private int recordingCounter = 0;

    public RecordingCounter(Context context, ImageView[] views) throws Exception {
        this.views = views;

        if (MAX_ITEM_COUNT != views.length) {
            throw new Exception("Invalid argument");
        }

        initCounter();
    }

    private void initCounter() {
        recordingCounter = 0;
        views[0].setImageResource(R.drawable.unfinished_1);
        views[1].setImageResource(R.drawable.unfinished_2);
        views[2].setImageResource(R.drawable.unfinished_3);
        views[3].setImageResource(R.drawable.unfinished_4);
        views[4].setImageResource(R.drawable.unfinished_5);

        setImageFocusChange();
    }

    // Sets the current training image to blue when training
    private void setImageFocusChange() {
        if (MAX_ITEM_COUNT <= recordingCounter) {
            return;
        }

        int resourceId = 0;
        switch (recordingCounter) {
            case 0:
                resourceId = R.drawable.progress_1;
                break;
            case 1:
                resourceId = R.drawable.progress_2;
                break;
            case 2:
                resourceId = R.drawable.progress_3;
                break;
            case 3:
                resourceId = R.drawable.progress_4;
                break;
            case 4:
                resourceId = R.drawable.progress_5;
                break;
        }

        views[recordingCounter].setImageResource(resourceId);
    }

    // Sets the current training image to white when done training
    private void setImageDone() {
        if (MAX_ITEM_COUNT <= recordingCounter) {
            return;
        }

        /*int resourceId = 0;
        switch (recordingCounter) {
            case 0: resourceId = R.drawable.recording_1_white; break;
            case 1: resourceId = R.drawable.recording_2_white; break;
            case 2: resourceId = R.drawable.recording_3_white; break;
            case 3: resourceId = R.drawable.recording_4_white; break;
            case 4: resourceId = R.drawable.recording_5_white; break;
        }

        views[recordingCounter].setBackgroundResource(R.drawable.recording_step_done);
        views[recordingCounter].setImageResource(resourceId);*/
        views[recordingCounter].setImageResource(R.drawable.finished);
    }

    // Sets the current training image to white when training fails
    /*private void setImageFailed() {
        if (MAX_ITEM_COUNT <= recordingCounter) {
			return;
		}

        int resourceId = 0;
        switch (recordingCounter) {
            case 0: resourceId = R.drawable.recording_1_white; break;
            case 1: resourceId = R.drawable.recording_2_white; break;
            case 2: resourceId = R.drawable.recording_3_white; break;
            case 3: resourceId = R.drawable.recording_4_white; break;
            case 4: resourceId = R.drawable.recording_5_white; break;
        }

        views[recordingCounter].setBackgroundResource(R.drawable.recording_step_fail);
        views[recordingCounter].setImageResource(resourceId);
    }*/

    // Updates the UI based on training result.
    public void updateRecordingResult(boolean isGoodRecording) {
        if (isGoodRecording) {
            setImageDone();
            recordingCounter++;
            setImageFocusChange();
        } else {
            Global.getInstance().discardLastUserRecording();
            //setImageFailed();
        }
    }

    public boolean isFinished() {
        Log.v(TAG, "isFinished: recordingCounter= " + recordingCounter);
        return MAX_ITEM_COUNT <= recordingCounter;
    }

}
