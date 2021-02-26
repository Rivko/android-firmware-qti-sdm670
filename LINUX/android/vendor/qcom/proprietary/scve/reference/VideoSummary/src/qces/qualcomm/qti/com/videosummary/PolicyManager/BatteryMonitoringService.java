/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.PolicyManager;

import android.app.IntentService;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.BatteryManager;
import android.util.Log;
import android.util.Pair;

import java.util.ArrayList;

import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.Util.CalendarHelper;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;
import qces.qualcomm.qti.com.videosummary.providers.VideoSummaryContract;
import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices.VidProcManagerService;

/**
 * Created by mithun on 7/25/2015.
 */
public class BatteryMonitoringService extends IntentService {
    private static final String ACTION_BATTERY_MONITOR = "qces.qualcomm.qti.com.videosummary.PolicyManager.action.BATTERY_MONITOR";

    private static final String TAG = BatteryMonitoringService.class.getSimpleName();
    private static float chargingRateMs = -1;
    private static int mLastLevel = -1;
    private static long mLastTime;

    public static boolean runningBatteryMonitorService = false;

    /**
     * Starts this service to perform action Foo with the given parameters. If
     * the service is already performing a task this action will be queued.
     *
     * @see IntentService
     */
    // TODO: Customize helper method
    public static void startActionBatteryMonitor(Context context, Intent batteryChangeIntent) {
        batteryChangeIntent.setClass(context, BatteryMonitoringService.class);
        batteryChangeIntent.setAction(ACTION_BATTERY_MONITOR);
        BatteryMonitoringService.runningBatteryMonitorService = true;
        context.startService(batteryChangeIntent);
    }

    public BatteryMonitoringService() {
        super("BatteryMonitoringService");
        Log.i(TAG, "constructor in batterymonitorservice");
    }

    @Override
    protected void onHandleIntent(Intent intent) {


        if (intent != null) {
            final String action = intent.getAction();
            if (ACTION_BATTERY_MONITOR.equals(action)) {
                final int level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
                final int scale = intent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);
                int chargingStatus = intent.getIntExtra(BatteryManager.EXTRA_STATUS, -1);
                final String status = Utils.getBatteryStatus(intent);
                int chargePlug = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1);
                final String plugType = Utils.getBatteryPluggedType(intent);
                Utils.sendMessage(this, TAG, "onhandleintent in batterymonitorservice, level=" +
                        level + ", scale=" + scale + ", status=" + status + ", plugtype=" + plugType);
                if (level == -1) {
                    throw new UnsupportedOperationException("cannot come to BatteryMonitoringService without battery change intent");
                }
                handleActionBatteryChange(level, scale, chargingStatus, chargingStatus);
            }
        }
        runningBatteryMonitorService = false;//thread is about to complete

    }

    private static void reset() {
        mLastLevel = -1;
        mLastTime = 0;
    }

    /**
     * Handle action BatteryChange in the provided background thread with the provided
     * parameters.
     */

    private void handleActionBatteryChange(int level, int scale, int chargingStatus, int
            chargePlug) {

        float timeRemaining = getTimeRemainingForBatteryCharging(level, scale, chargingStatus);
        if (timeRemaining != -1) {
            BatteryChangeEventReceiver.unregisterForBatteryChangeEvents(this);
            setupAlarmIfNecessary(timeRemaining);
        }
    }


    /*
     * calculates the time required for charging the battery to BATTERY_LEVEL_TO_START_VIDEO_SUMMARY percent(75%) in seconds
     */
    float getTimeRemainingForBatteryCharging(int currentLevel, int scale, int chargingStatus) {
        //TODO: function should not have multiple returns, change it later
        //charger got disconnected, so reset
        if (!(chargingStatus == BatteryManager.BATTERY_STATUS_CHARGING || chargingStatus ==
                BatteryManager.BATTERY_STATUS_FULL)) {
            Utils.sendMessage(this, TAG, "Not charging nor battery is full, so reset values & " +
                    "return");
            reset();
            //sendUnregisterRequestForBatteryChangeEvents();
            return -1;
        }

        long currentTime = System.currentTimeMillis();
        float currBatteryPercentage = currentLevel * 100 / (float) scale;
        float timeRemaining = 0;

        if (currBatteryPercentage >= Constants.BATTERY_LEVEL_TO_START_VIDEO_SUMMARY) {
            Utils.sendMessage(this, TAG, "currlevel = " + currentLevel + " >= required level, so returning time = 0");
            reset();
            //sendUnregisterRequestForBatteryChangeEvents();

            return 0;//already battery is charged to expected level;
        }

        if (currentLevel == mLastLevel) {
            Utils.sendMessage(this, TAG, "currlevel == mLastlevel =" + mLastLevel + " so return");
            return -1;//got an battery change event for the same level which is of no use.
        }

        //should happen only first time i.e when mLastLevel = -1, do not update this every time you come into this function and level didnt change
        if (mLastLevel == -1) {
            mLastLevel = currentLevel;
            mLastTime = 0;//for the first time the battery level will be in the middle eg: 65.5%, so wait till it jumps to next level to start timer.
            Utils.sendMessage(this, TAG, "1st time, so only saving level = " + mLastLevel + " and time = " + mLastTime + " & return");
            return -1;
        }

        if (mLastLevel < currentLevel && mLastTime == 0) {
            //this is the start of next level, so record the level and timestamp
            mLastLevel = currentLevel;
            mLastTime = currentTime;
            Utils.sendMessage(this, TAG, "2nd time, so only saving real level = " + mLastLevel + " and time = " + mLastTime + " & return");
            return -1;
        } else {
            //when battery level jumps to next level of mLastlevel

            float prevBatteryPercentage = mLastLevel * 100 / (float) scale;
            chargingRateMs = Math.abs(currBatteryPercentage - prevBatteryPercentage) / (float) Math.abs(currentTime - mLastTime);
            timeRemaining = ((Constants.BATTERY_LEVEL_TO_START_VIDEO_SUMMARY - currBatteryPercentage) / (float) chargingRateMs) / 1000;//in seconds
            String msg = "curBat=" + currBatteryPercentage + " prevBat=" + prevBatteryPercentage +
                    " currTime=" + currentTime + " prevTime=" + mLastTime + "charging rate = " +
                    chargingRateMs + " time remainig(s) = " + timeRemaining + " time string=" +
                    CalendarHelper.getTimeString((long) timeRemaining);
            Log.i(TAG, msg);
            Utils.sendMessage(this, TAG, msg);
            reset();
            Utils.sendMessage(this, TAG, "finished calculating time, send unregister request for battery change events");
            return timeRemaining;
        }
    }

    /**
     * input time remaining in seconds
     */
    void setupAlarmIfNecessary(float timeRemaining) {
        //timeRemaining = 0;//for testing purpose  only, remove this later
        if (timeRemaining == 0) {
            //start running video summary immediately
            Utils.sendMessage(this, TAG, "Start running Video Summary as time remaining is <= 0 " +
                    ":" +
                    " " +
                    timeRemaining);

            //TODO do scoring first
            boolean isScoring = runScoringJobs();
            if (!isScoring) {
                //no more scoring jobs, so do Highlights Assembly
                boolean isAssembling = runVideoSummaryNow();
                if (!isAssembling) {
                    //no more highlights, so do timelapse
                    runVideoTimelapseNow();
                }
            }

            //TODO: after performing the cpu intensive operations then do unregister power events

        } else if (timeRemaining <= 120) {
            //setAlarm for exact time remaining
            Utils.sendMessage(this, TAG, "time remaining is <= 60 : " + timeRemaining);
            BatteryAlarmReceiver.getInstance().setAlarmAtTime(this, timeRemaining);
        } else {
            Utils.sendMessage(this, TAG, "setting alarm as time remaining is : " + timeRemaining);
            timeRemaining *= 0.75f;//check after 75% of time remaining and recalculate the battery time remaining
            //timeRemaining > 120 seconds, so set up alarm for 75% of time and redo all calculations again until time remaining will be <= 60 seconds
            BatteryAlarmReceiver.getInstance().setAlarmAtTime(this, timeRemaining);
        }
    }


    private boolean runScoringJobs() {
        Utils.sendMessage(this, TAG, "running Scoring jobs in Automatic mode");

        Pair<ArrayList<MediaInfo>, Long> result = getNextUrisToBeScored();

        while (result != null) {
            final ArrayList<MediaInfo> mediaInfoArrayList = result.first;
            long summaryTimestamp = result.second;
            if (mediaInfoArrayList.size() > 0) {
                FrequencyScheduleAlarmReceiver.cancelTimeoutAlarm(this);
                FrequencyScheduleAlarmReceiver.setTimeoutAlarm(this, System.currentTimeMillis() + Utils.ALARM_TIMEOUT);

                Utils.sendMessage(this, TAG, "starting scoring process for " + CalendarHelper.timeStampMillisToString(summaryTimestamp));
                ArrayList<Uri> uris = MediaInfo.getArrayListOfUris(mediaInfoArrayList);
                VidProcManagerService.startActionScoreRecord(this, uris, summaryTimestamp);

                return true;//if atleast one scoring task is started then return true and it will call videoProcessingCompletedReceiver, which intern will call setalarm to come here to finish remaining scoring jobs

            } else {
                //there is no videos captured b/w this start and end timestamp so delete this row from db
                QueryHelper.deleteAssemblyTimeline(this, summaryTimestamp);
            }
            result = getNextUrisToBeScored();

        }
        Utils.sendMessage(this, TAG, "Nothing to score now");
        return false;
    }

    private boolean runVideoSummaryNow() {
        Utils.sendMessage(this, TAG, "running Video Summary now in Automatic mode");

        Pair<ArrayList<MediaInfo>, Long> result = getNextUrisToBeSummarized();
        final HighlightsModel.SummaryType summaryType = HighlightsModel.SummaryType.HIGHLIGHTS;
        while (result != null) {
            final ArrayList<MediaInfo> mediaInfoArrayList = result.first;
            long summaryTimestamp = result.second;
            if (mediaInfoArrayList.size() > 0) {
                FrequencyScheduleAlarmReceiver.cancelTimeoutAlarm(this);
                FrequencyScheduleAlarmReceiver.setTimeoutAlarm(this, System.currentTimeMillis() + Utils.ALARM_TIMEOUT);

                String mp4FilePath = Utils.getMp4FilePath(this, summaryTimestamp).toString();
                String thumbImgPath = Utils.getThumbImgFilePath(this, summaryTimestamp).toString();

                int speedOrLength = 0;
                int theme = Constants.DEFAULT_THEME;
                if (summaryType == HighlightsModel.SummaryType.HIGHLIGHTS) {
                    int length = SettingsActivity.getPrefDefaultLength(this);
                    speedOrLength = HighlightsModel.getSpeedOrLength(length, theme);
                } else {
                    int speed = SettingsActivity.getPrefSpeed(this);
                    boolean eventDetectEnabled = SettingsActivity.getPrefEventDetectionEnabled(this);
                    int eventSpeed = SettingsActivity.getPrefEventSpeed(this);
                    boolean motionEventDetectEnabled = SettingsActivity.getPrefMotionEventDetectionEnabled(this);
                    boolean faceEventDetectEnabled = SettingsActivity.getPrefFaceEventDetectionEnabled(this);
                    speedOrLength = HighlightsModel.getSpeedOrLength(speed, eventDetectEnabled, eventSpeed, motionEventDetectEnabled, faceEventDetectEnabled);
                }
                //using same theme for both Highlights and timelapse
                speedOrLength = HighlightsModel.updateTheme(speedOrLength, theme);

                final HighlightsModel item = new HighlightsModel(HighlightsModel.CardType.CREATING, summaryType,
                        mp4FilePath, thumbImgPath, summaryTimestamp, "Creating Movie", "", mediaInfoArrayList, speedOrLength, null);
                QueryHelper.insertIntoHighlights(this, item);

                //Update title in a thread which runs Geocoder (which inturn uses wifi or data) which is sometimes time consuming
                final Context context = this;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        item.updateTitle(context);
                        Utils.sendMessage(context, TAG, "Updating title as " + item.title + " & subTitle as " + item.subTitle);
                        QueryHelper.updateHighlights(context, item.videoFilePath, item.title, item.subTitle);
                    }
                }).start();

                Utils.sendMessage(this, TAG, "starting summary process for Title=" + item.title + " timestamp = " + CalendarHelper.timeStampMillisToString(summaryTimestamp));
                VidProcManagerService.startActionProcess(this, item);
                return true;//if atleast one summary task is started, just return and it will call videoProcessingCompletedReceiver, which intern will call setalarm to come here.

                 /*
              this broad cast has to be call from video processing engine once video hilight is
              ready
             */
                //use the corresponding timestamp as argument to identify the video instead of zero
                //VideoProcessingCompleteReceiver.sendVideoProcessingCompleteBroadcast(this, 0);

            } else {
                //there is no videos captured b/w this start and end timestamp so delete this row from db
                QueryHelper.deleteAssemblyTimeline(this, summaryTimestamp);
            }
            result = getNextUrisToBeSummarized();

        }
        Utils.sendMessage(this, TAG, "Nothing to summarize now, everything is already summarized or no media available ");
        //nothing to summarise, everything is already summarized or no media available
        //TODO: cancel timeOut alarm
        return false;
    }

    private void runVideoTimelapseNow() {
        Utils.sendMessage(this, TAG, "running Video Timelapse now in Automatic mode");

        /**
         * For timelapse we need to pick only media who's duration is > Constant (say 5 min)
         * so we need to delete any other small length videos from the ArrayList<MediaInfo>
         */
        Pair<ArrayList<MediaInfo>, Long> result = getNextUrisToBeTimelapsed();

        final HighlightsModel.SummaryType summaryType = HighlightsModel.SummaryType.TIMELAPSE;
        while (result != null) {
            ArrayList<MediaInfo> mediaInfoArrayList = result.first;

            for (int i = 0; i < mediaInfoArrayList.size(); ) {
                if (mediaInfoArrayList.get(i).duration < Utils.TIMELAPSE_MIN_DURATION) {
                    mediaInfoArrayList.remove(i);
                } else {
                    i++;
                }
            }//By completing this loop, the mediaInfoArrayList should have videos whose duration is >= TIMELAPSE_MIN_DURATION

            long summaryTimestamp = result.second;
            if (mediaInfoArrayList.size() > 0) {
                FrequencyScheduleAlarmReceiver.cancelTimeoutAlarm(this);
                FrequencyScheduleAlarmReceiver.setTimeoutAlarm(this, System.currentTimeMillis() + Utils.ALARM_TIMEOUT * 3);

                String mp4FilePath = Utils.getMp4FilePath(this, summaryTimestamp).toString();
                String thumbImgPath = Utils.getThumbImgFilePath(this, summaryTimestamp).toString();

                int speedOrLength = 0;
                int theme = Constants.DEFAULT_THEME;
                if (summaryType == HighlightsModel.SummaryType.HIGHLIGHTS) {
                    int length = SettingsActivity.getPrefDefaultLength(this);
                    speedOrLength = HighlightsModel.getSpeedOrLength(length, theme);
                } else {
                    int speed = SettingsActivity.getPrefSpeed(this);
                    boolean eventDetectEnabled = SettingsActivity.getPrefEventDetectionEnabled(this);
                    int eventSpeed = SettingsActivity.getPrefEventSpeed(this);
                    boolean motionEventDetectEnabled = SettingsActivity.getPrefMotionEventDetectionEnabled(this);
                    boolean faceEventDetectEnabled = SettingsActivity.getPrefFaceEventDetectionEnabled(this);
                    speedOrLength = HighlightsModel.getSpeedOrLength(speed, eventDetectEnabled, eventSpeed, motionEventDetectEnabled, faceEventDetectEnabled);
                }
                //using same theme for both Highlights and timelapse
                speedOrLength = HighlightsModel.updateTheme(speedOrLength, theme);

                final HighlightsModel item = new HighlightsModel(HighlightsModel.CardType.CREATING, summaryType,
                        mp4FilePath, thumbImgPath, summaryTimestamp, "Creating Movie", "", mediaInfoArrayList, speedOrLength, null);
                QueryHelper.insertIntoHighlights(this, item);

                //Update title in a thread which runs Geocoder (which inturn uses wifi or data) which is sometimes time consuming
                final Context context = this;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        item.updateTitle(context);
                        Utils.sendMessage(context, TAG, "Updating title as " + item.title + " & subTitle as " + item.subTitle);
                        QueryHelper.updateHighlights(context, item.videoFilePath, item.title, item.subTitle);
                    }
                }).start();

                Utils.sendMessage(this, TAG, "starting summary process for Title=" + item.title + " timestamp = " + CalendarHelper.timeStampMillisToString(summaryTimestamp));
                VidProcManagerService.startActionProcess(this, item);
                return;//if atleast one summary task is started, just return and it will call videoProcessingCompletedReceiver, which intern will call setalarm to come here.

                 /*
              this broad cast has to be call from video processing engine once video hilight is
              ready
             */
                //use the corresponding timestamp as argument to identify the video instead of zero
                //VideoProcessingCompleteReceiver.sendVideoProcessingCompleteBroadcast(this, 0);

            } else {
                //there is no videos captured b/w this start and end timestamp so delete this row from db
                QueryHelper.deleteAssemblyTimeline(this, summaryTimestamp);
            }
            result = getNextUrisToBeTimelapsed();

        }
        Utils.sendMessage(this, TAG, "Nothing to summarize now, everything is already timelapsed or no media available ");
        //nothing to summarise, everything is already timelapsed or no media available
        //TODO: cancel timeOut alarm
        return;
    }

    private Pair<ArrayList<MediaInfo>, Long> getNextUrisToBeTimelapsed() {
        Cursor cursor = QueryHelper.queryAssemblyTimeline(this, VideoSummaryContract.AssemblyTimeline.TYPE_TIMELAPSE);//for timelapse
        Utils.sendMessage(this, TAG, "pending timelapse jobs = ");
        return processCursor(cursor);
    }

    private Pair<ArrayList<MediaInfo>, Long> getNextUrisToBeSummarized() {
        Cursor cursor = QueryHelper.queryAssemblyTimeline(this, VideoSummaryContract.AssemblyTimeline.TYPE_HIGHLIGHTS_ASSEMBLY);//for assembly
        Utils.sendMessage(this, TAG, "pending summary jobs = ");
        return processCursor(cursor);
    }

    private Pair<ArrayList<MediaInfo>, Long> getNextUrisToBeScored() {
        Cursor cursor = QueryHelper.queryAssemblyTimeline(this, VideoSummaryContract.AssemblyTimeline.TYPE_SCORING_ONLY); //for scoring only
        Utils.sendMessage(this, TAG, "pending jobs for scoring = ");
        return processCursor(cursor);
    }

    private Pair<ArrayList<MediaInfo>, Long> processCursor(Cursor cursor) {
        if (cursor != null) {
            Utils.sendMessage(this, TAG, "Number of pending jobs = " + cursor.getCount());
            while (cursor.moveToNext()) {
                long startTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._START_TIMESTAMP));
                long endTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._END_TIMESTAMP));
                long summaryTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._SUMMARY_TIMESTAMP));
                int attempts = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._ATTEMPTS));
                int type = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._TYPE));
                if (attempts < 3) {
                    if (summaryTimestamp == 0) {
                        summaryTimestamp = System.currentTimeMillis();
                    }
                    QueryHelper.updateAssemblyTimeline(this, startTimestamp, endTimestamp, summaryTimestamp, attempts + 1, type);
                    return new Pair<>(QueryHelper.queryMediaStore(this, startTimestamp, endTimestamp), summaryTimestamp);
                } else {
                    //already tried 3 attempts, so delete this row and continue fetching next row
                    QueryHelper.deleteAssemblyTimeline(this, summaryTimestamp);
                }
            }
        }
        return null;
    }
}
