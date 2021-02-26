/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.PolicyManager;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v4.content.LocalBroadcastManager;

import qces.qualcomm.qti.com.videosummary.Notification.Notification;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;

public class VideoProcessingCompleteReceiver extends BroadcastReceiver {

    private static boolean registered = false;

    private static final String ACTION_VIDEO_PROCESSING_COMPLETE = "android.intent.action" +
            ".ACTION-VIDEO-PROCESSING-COMPLETED";
    private static final String EXTRA_TIME_STAMP_MILLIS = "qces.qualcomm.qti.com.videosummary.extra.TIME_STAMP_MILLIS";


    private static final String TAG = VideoProcessingCompleteReceiver.class.getSimpleName();
    private static VideoProcessingCompleteReceiver instance = null;


    public static synchronized VideoProcessingCompleteReceiver getInstance() {
        if (instance == null) {
            instance = new VideoProcessingCompleteReceiver();
        }
        return instance;
    }

    public VideoProcessingCompleteReceiver() {
    }

    public static void registerForVideoProcessingComplete(Context context) {

        Utils.sendMessage(context, TAG, "trying to registerForVideoProcessingComplete");
        if (!registered) {
            LocalBroadcastManager.getInstance(context.getApplicationContext()).registerReceiver
                    (getInstance(),
                            new IntentFilter(ACTION_VIDEO_PROCESSING_COMPLETE));
            registered = true;
            Utils.sendMessage(context, TAG, "registerForVideoProcessingComplete successful");
        } else {
            Utils.sendMessage(context, TAG, "Already registered ForVideoProcessingComplete");
        }
    }

    public static void unregisterForVideoProcessingComplete(Context context) {
        Utils.sendMessage(context, TAG, "trying to unregisterForVideoProcessingComplete");
        if (registered) {
            try {
                LocalBroadcastManager.getInstance(context.getApplicationContext()).unregisterReceiver(getInstance());
                Utils.sendMessage(context, TAG, "unregisterForVideoProcessingComplete successful");
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                Utils.sendMessage(context, TAG, e.toString());
            } finally {
                registered = false;
            }
        } else {
            Utils.sendMessage(context, TAG, "Already unregistered ForVideoProcessingComplete");
        }
    }

    public static void sendVideoProcessingCompleteBroadcast(Context context, long timeStampMillis) {
        Intent intent = new Intent(ACTION_VIDEO_PROCESSING_COMPLETE);
        intent.putExtra(EXTRA_TIME_STAMP_MILLIS, timeStampMillis);
        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        // TODO: This method is called when the BroadcastReceiver is receiving
        // an Intent broadcast.
        Utils.sendMessage(context, TAG, intent.getAction());

        if (intent.getAction().equals(ACTION_VIDEO_PROCESSING_COMPLETE)) {
            long timeStampMillis = intent.getLongExtra(EXTRA_TIME_STAMP_MILLIS, 0);
            handleActionVideoProcessingComplete(context, timeStampMillis);
        } else {
            throw new UnsupportedOperationException("Unsupported Intent " + intent.getAction());
        }
    }

    static int count = 0;

    void handleActionVideoProcessingComplete(Context context, long timeStampMillis) {
        FrequencyScheduleAlarmReceiver.cancelTimeoutAlarm(context);
        unregisterForVideoProcessingComplete(context);
        Utils.sendMessage(context, TAG, "all video's summary completed, count = " + count++);
        BatteryPowerEventReceiver.unregisterForPowerConnectedDisconnectedEvents(context);
        //unregisterForVideoProcessingComplete(context);
        //BatteryPowerEventReceiver.unregisterForPowerConnectedDisconnectedEvents(context);
        /*
        you can do notification for this video now
         */

        // BatteryChangeEventReceiver.sendBatteryLocalEvent(context, BatteryChangeEventReceiver
        //         .MSG_SHOW_DIALOG);
        //HighlightsActivity.hideSpinner();
        //Utils.showProcessingCompleteDialog(context);

        int numRowsDeleted = QueryHelper.deleteAssemblyTimeline(context, timeStampMillis);
        Notification notification = new Notification(context, timeStampMillis);
        notification.showNotification(numRowsDeleted > 0);//parameter will be true for automode run


        FrequencyScheduleAlarmReceiver.setupAlarmForSelectedFrequency(context);
    }


}
