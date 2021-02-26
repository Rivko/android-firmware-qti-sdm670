/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva.ux.dev;

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

		if (MAX_ITEM_COUNT != views.length ) {
			throw new Exception("Invalid argument");
		}

		initCounter();
	}

    private void initCounter() {
        recordingCounter = 0;
        views[0].setBackgroundResource(R.drawable.recording_step_off);
        views[0].setImageResource(R.drawable.recording_1_gray);
        views[1].setBackgroundResource(R.drawable.recording_step_off);
        views[1].setImageResource(R.drawable.recording_2_gray);
        views[2].setBackgroundResource(R.drawable.recording_step_off);
        views[2].setImageResource(R.drawable.recording_3_gray);
        views[3].setBackgroundResource(R.drawable.recording_step_off);
        views[3].setImageResource(R.drawable.recording_4_gray);
        views[4].setBackgroundResource(R.drawable.recording_step_off);
        views[4].setImageResource(R.drawable.recording_5_gray);

		setImageFocusChange();
	}

	// Sets the current training image to blue when training
	private void setImageFocusChange() {
		if (MAX_ITEM_COUNT <= recordingCounter) {
			return;
		}

        int resourceId = 0;
        switch (recordingCounter) {
            case 0: resourceId = R.drawable.recording_1_blue; break;
            case 1: resourceId = R.drawable.recording_2_blue; break;
            case 2: resourceId = R.drawable.recording_3_blue; break;
            case 3: resourceId = R.drawable.recording_4_blue; break;
            case 4: resourceId = R.drawable.recording_5_blue; break;
        }

        views[recordingCounter].setBackgroundResource(R.drawable.recording_step_focused);
        views[recordingCounter].setImageResource(resourceId);
    }

	// Sets the current training image to white when done training
	private void setImageDone() {
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

        views[recordingCounter].setBackgroundResource(R.drawable.recording_step_done);
        views[recordingCounter].setImageResource(resourceId);
    }

	// Sets the current training image to white when training fails
	private void setImageFailed() {
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
    }

	// Updates the UI if training was successful
	public void updateRecordingResult(boolean isGoodRecording) {
		if (isGoodRecording) {
			setImageDone();
			recordingCounter++;
			setImageFocusChange();
		}
		else {
            Global.getInstance().discardLastUserRecording();
			setImageFailed();
		}
	}

	public boolean isFinished() {
	    Log.v(TAG, "isFinished: recordingCounter= " + recordingCounter);
		return MAX_ITEM_COUNT <= recordingCounter;
	}

}
