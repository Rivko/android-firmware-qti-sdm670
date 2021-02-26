/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.PolicyManager;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.util.Log;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Set;

import qces.qualcomm.qti.com.videosummary.Util.CalendarHelper;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;
import qces.qualcomm.qti.com.videosummary.providers.VideoSummaryContract;
import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;

public class FrequencyScheduleAlarmReceiver extends BroadcastReceiver {
    private static final int ALARM_REQUEST_CODE = 1;
    private static final int ALARM_TIMEOUT_REQUEST_CODE = 2;
    private static final String TAG = FrequencyScheduleAlarmReceiver.class.getSimpleName();
    private static final String ACTION_FREQUENCY_SCHEDULER = "qces.qualcomm.qti.com.videosummary.PolicyManager.action.FREQUENCY_SCHEDULER";
    private static final String ACTION_TIMEOUT = "qces.qualcomm.qti.com.videosummary.PolicyManager.action.TIMEOUT";

    public FrequencyScheduleAlarmReceiver() {
    }

    public static void cancelExistingAlarms(Context context) {
        Utils.sendMessage(context, TAG, "cancelling FrequencyScheduleAlarms");
        Intent intent = new Intent(context, FrequencyScheduleAlarmReceiver.class);
        intent.setAction(ACTION_FREQUENCY_SCHEDULER);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context.getApplicationContext(), ALARM_REQUEST_CODE, intent, 0);
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(context.getApplicationContext().ALARM_SERVICE);
        alarmManager.cancel(pendingIntent);
    }

    /**
     * reads the frequency value set in shared prefs and will set alarm based on the value.
     * No harm in calling this method multiple times as it will first cancel the existing alarm
     * and then will set it back to the required one.
     */
    public static void setupAlarmForSelectedFrequency(Context context) {


        //if the Assembly timeline table has still jobs pending, set alarm immediately
        Cursor cursor = QueryHelper.queryAssemblyTimeline(context, VideoSummaryContract.AssemblyTimeline.TYPE_SCORING_ONLY);//checking for ScoringOnly jobs
        boolean anyPendingJobsInAssemblyTimelineTable = false;
        if (cursor != null && cursor.getCount() > 0) {
            anyPendingJobsInAssemblyTimelineTable = true;
        }

        if (!anyPendingJobsInAssemblyTimelineTable) {
            cursor = QueryHelper.queryAssemblyTimeline(context, VideoSummaryContract.AssemblyTimeline.TYPE_HIGHLIGHTS_ASSEMBLY);//checking for Highlights Assembly jobs
            if (cursor != null && cursor.getCount() > 0) {
                anyPendingJobsInAssemblyTimelineTable = true;
            }
        }

        if (!anyPendingJobsInAssemblyTimelineTable) {
            cursor = QueryHelper.queryAssemblyTimeline(context, VideoSummaryContract.AssemblyTimeline.TYPE_TIMELAPSE);//checking for Timelapse jobs
            if (cursor != null && cursor.getCount() > 0) {
                anyPendingJobsInAssemblyTimelineTable = true;
            }
        }

        if (anyPendingJobsInAssemblyTimelineTable) {
            //set alarm immediately
            cancelExistingAlarms(context);
            setAlarm(context, Calendar.getInstance().getTimeInMillis());
        } else if (Constants.PERFORM_DAILY_SCORING) {
            Log.i(TAG, "setting alarm for Daily Scoring");
            cancelExistingAlarms(context);
            setUpcomingAlarmDay(context);
        } else
            //This part of code is useful only when PREFORM_DAILY_SCORING flag is false, then setup alarm based on closest frequency
            //If the CreateHighlightVideo toggle button is ON
            if (SettingsActivity.getPrefCreateHighlightVideos(context)) {
                //get the frequency value from pref
                Set<String> value = SettingsActivity.getPrefFrequency(context);
                Log.i(TAG, "Frequency value : " + value.toString());
                //cancelExistingAlarms any existing alarms before settingup new one
                cancelExistingAlarms(context);

                if (value.contains("Daily")) {
                    Log.i(TAG, "Daily ");
                    setUpcomingAlarmDay(context);
                } else if (value.contains("Weekly")) {
                    Log.i(TAG, "Weekly");
                    setUpcomingAlarmWeek(context);

                } else if (value.contains("Monthly")) {
                    Log.i(TAG, "Monthly");
                    setUpcomingAlarmMonth(context);
                }
            } else {
                Log.i(TAG, "CreateHighlights is OFF in settings");
            }
    }

    static void setUpcomingAlarmDay(Context context) {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.set(Calendar.HOUR_OF_DAY, 0);//midnight
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);

        if (calendar.compareTo(Calendar.getInstance()) <= 0) {
            calendar.add(Calendar.DATE, 1);
        }

        SimpleDateFormat sdf = new SimpleDateFormat("yyyy.MM.dd 'at' HH:mm:ss");
        Utils.sendMessage(context, TAG, " setting alarm for : " + sdf.format(calendar.getTime()));

        setAlarm(context, calendar.getTimeInMillis());
    }


    static void setUpcomingAlarmWeek(Context context) {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.set(Calendar.DAY_OF_WEEK, Calendar.MONDAY);
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);

        if (calendar.compareTo(Calendar.getInstance()) <= 0) {
            calendar.add(Calendar.DATE, 7);
        }
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy.MM.dd 'at' HH:mm:ss");
        Utils.sendMessage(context, TAG, " setting alarm for : " + sdf.format(calendar.getTime()));

        setAlarm(context, calendar.getTimeInMillis());
    }

    static void setUpcomingAlarmMonth(Context context) {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.set(Calendar.DAY_OF_MONTH, 1);
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);

        if (calendar.compareTo(Calendar.getInstance()) <= 0) {
            calendar.add(Calendar.MONTH, 1);
        }

        SimpleDateFormat sdf = new SimpleDateFormat("yyyy.MM.dd 'at' HH:mm:ss");
        Utils.sendMessage(context, TAG, " setting alarm for : " + sdf.format(calendar.getTime()));

        setAlarm(context, calendar.getTimeInMillis());
    }


    public static void setAlarm(Context context, long timeMillis) {

        //setting alarm for 5 seconds from now, just for testing
        //timeMillis = System.currentTimeMillis() + 5000;

        Intent alarmIntent = new Intent(context, FrequencyScheduleAlarmReceiver.class);
        alarmIntent.setAction(ACTION_FREQUENCY_SCHEDULER);
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(context.getApplicationContext().ALARM_SERVICE);

        alarmManager.set(AlarmManager.RTC_WAKEUP,
                timeMillis,
                PendingIntent.getBroadcast(context.getApplicationContext(), ALARM_REQUEST_CODE, alarmIntent, 0));

    }

    public static void setTimeoutAlarm(Context context, long timeMillis) {
        Utils.sendMessage(context, TAG, "setting TimeoutAlarm");
        Intent alarmIntent = new Intent(context, FrequencyScheduleAlarmReceiver.class);
        alarmIntent.setAction(ACTION_TIMEOUT);
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(context.getApplicationContext().ALARM_SERVICE);
        alarmManager.set(AlarmManager.RTC_WAKEUP,
                timeMillis,
                PendingIntent.getBroadcast(context.getApplicationContext(), ALARM_TIMEOUT_REQUEST_CODE, alarmIntent, 0));
    }

    public static void cancelTimeoutAlarm(Context context) {
        Utils.sendMessage(context, TAG, "cancelling TimeoutAlarm");
        Intent intent = new Intent(context, FrequencyScheduleAlarmReceiver.class);
        intent.setAction(ACTION_TIMEOUT);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context.getApplicationContext(), ALARM_TIMEOUT_REQUEST_CODE, intent, 0);
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(context.getApplicationContext().ALARM_SERVICE);
        alarmManager.cancel(pendingIntent);
    }


    @Override
    public void onReceive(Context context, Intent intent) {
        // TODO: This method is called when the BroadcastReceiver is receiving
        // an Intent broadcast.
        Utils.sendMessage(context, TAG, "onReceive of FrequencyScheduleAlarm with " + intent.getAction());
        if (intent.getAction().equals(ACTION_FREQUENCY_SCHEDULER) || (intent.getAction().equals(ACTION_TIMEOUT))) {

            if (intent.getAction().equals(ACTION_TIMEOUT)) {
                Utils.sendMessage(context, TAG, "video summary TIMED-OUT!!!!!");
            }
            //This method is responsible to update the AssemblyTimeline table with start and end timestamps of all days,weeks, months which are pending since last update
            queueTaskIntoAssemblyTimeline(context);


            /**
             * setup the next occurrence
             */

            //setupAlarmForSelectedFrequency(context);

            //here register for power connected and disconnedted events
            BatteryPowerEventReceiver.registerForPowerConnectedDisconnectedEvents(context);
            BatteryChangeEventReceiver.registerForBatteryChangeEvents(context);
        } else {
            throw new UnsupportedOperationException("Not yet implemented");
        }
    }

    void queueTaskIntoAssemblyTimeline(Context context) {
        Set<String> value = SettingsActivity.getPrefFrequency(context);

        long tableUpdatedTill = SettingsActivity.getAssemblyTimelineTableUpdateTimestamp(context);
        Calendar previouslyUpdatedTill = Calendar.getInstance();
        previouslyUpdatedTill.setTimeInMillis(tableUpdatedTill);

        long start, end;
        Calendar now = Calendar.getInstance();
        //TODO testing only
        //now.set(2015, Calendar.JULY, 17, 0, 0, 0);

        now.setTimeInMillis(getStartOfDayInMillis(now) - 1);//run till yesterday night

        String updatedTillStr = CalendarHelper.timeStampMillisToString(tableUpdatedTill);
        String nowStr = CalendarHelper.timeStampMillisToString(now.getTimeInMillis());

        while (getEndOfDayInMillis(previouslyUpdatedTill) <= getEndOfDayInMillis(now)) {

            //Check the Daily_Scoring flag enabled
            if (Constants.PERFORM_DAILY_SCORING) {
                //For daily scoring add entry into assemblytimline table with ScoringOnly flag set
                //add one row for daily summary
                Log.i(TAG, "Daily Scoring ");
                start = getStartOfDayInMillis(previouslyUpdatedTill);
                end = getEndOfDayInMillis(previouslyUpdatedTill);
                addEntryIntoAssemblyTimeline(context, start, end, VideoSummaryContract.AssemblyTimeline.TYPE_SCORING_ONLY);//scoringOnly
            }

            //if timelapse switch is enabled
            if (SettingsActivity.getPrefCreateTimelapseVideos(context)) {
                Log.i(TAG, "Daily timelapse");
                start = getStartOfDayInMillis(previouslyUpdatedTill);
                end = getEndOfDayInMillis(previouslyUpdatedTill);
                addEntryIntoAssemblyTimeline(context, start, end, VideoSummaryContract.AssemblyTimeline.TYPE_TIMELAPSE);//entry for Timelapse
            }


            //Check the highlights switch if turned on do below
            if (SettingsActivity.getPrefCreateHighlightVideos(context)) {

                if (value.contains("Daily")) {
                    Log.i(TAG, "Daily ");
                    //add one row for daily summary
                    start = getStartOfDayInMillis(previouslyUpdatedTill);
                    end = getEndOfDayInMillis(previouslyUpdatedTill);
                    addEntryIntoAssemblyTimeline(context, start, end, VideoSummaryContract.AssemblyTimeline.TYPE_HIGHLIGHTS_ASSEMBLY);
                }
                if (value.contains("Weekly")) {
                    Log.i(TAG, "Weekly");
                    if (getEndOfWeekInMillis(previouslyUpdatedTill) == getEndOfDayInMillis(previouslyUpdatedTill)) {
                        //this day is end of week, so add one row for weekly summary
                        start = getStartOfWeekInMillis(previouslyUpdatedTill);
                        end = getEndOfWeekInMillis(previouslyUpdatedTill);
                        addEntryIntoAssemblyTimeline(context, start, end, VideoSummaryContract.AssemblyTimeline.TYPE_HIGHLIGHTS_ASSEMBLY);
                    }
                }
                if (value.contains("Monthly")) {
                    Log.i(TAG, "Monthly");
                    if (getEndOfMonthInMillis(previouslyUpdatedTill) == getEndOfDayInMillis(previouslyUpdatedTill)) {
                        //today is end of month
                        start = getStartOfMonthInMillis(previouslyUpdatedTill);
                        end = getEndOfMonthInMillis(previouslyUpdatedTill);
                        addEntryIntoAssemblyTimeline(context, start, end, VideoSummaryContract.AssemblyTimeline.TYPE_HIGHLIGHTS_ASSEMBLY);
                    }

                }
            }
            //Check the timelapse switch if turned on do below
            if (SettingsActivity.getPrefCreateTimelapseVideos(context)) {
                //Todo: do auto run for timelapse, based on the spec
            }

            //increment loop counter, i.e add one day to calendar
            previouslyUpdatedTill.add(Calendar.DATE, 1);
            String nextDayStr = CalendarHelper.timeStampMillisToString(previouslyUpdatedTill.getTimeInMillis());
            Utils.sendMessage(context, TAG, "nextDay = " + nextDayStr);
        }
        Utils.sendMessage(context, TAG, "Finished inserting start, end tasks into AssemblyTimeline");
        //saving previously updated till timestamp
        String updatedTIllStr = CalendarHelper.timeStampMillisToString(getStartOfDayInMillis(previouslyUpdatedTill));
        SettingsActivity.setAssemblyTimelineTableUpdateTimestamp(context, getStartOfDayInMillis(previouslyUpdatedTill));

    }

    void addEntryIntoAssemblyTimeline(Context context, long start, long end, int type) {
        String startStr = CalendarHelper.timeStampMillisToString(start);
        String endStr = CalendarHelper.timeStampMillisToString(end);
        Utils.sendMessage(context, TAG, "start = " + startStr + " , end = " + endStr);
        QueryHelper.insertIntoAssemblyTimeline(context, start, end, type);
    }

    long getStartOfDayInMillis(Calendar cal) {
        Calendar calendar = (Calendar) cal.clone();
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);
        return calendar.getTimeInMillis();
    }

    long getEndOfDayInMillis(Calendar cal) {
        Calendar calendar = (Calendar) cal.clone();
        calendar.set(Calendar.HOUR_OF_DAY, 23);
        calendar.set(Calendar.MINUTE, 59);
        calendar.set(Calendar.SECOND, 59);
        calendar.set(Calendar.MILLISECOND, 999);
        return calendar.getTimeInMillis();
    }

    long getStartOfWeekInMillis(Calendar cal) {
        Calendar calendar = (Calendar) cal.clone();
        calendar.set(Calendar.DAY_OF_WEEK, 1);
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);
        return calendar.getTimeInMillis();
    }

    long getEndOfWeekInMillis(Calendar cal) {
        Calendar calendar = (Calendar) cal.clone();
        calendar.set(Calendar.DAY_OF_WEEK, 1);
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);
        calendar.add(Calendar.DATE, 7);//add 7 days which goes to next monday
        return calendar.getTimeInMillis() - 1;//minus 1 will take to previous day i.e saturday 23:59:59.999
    }

    long getStartOfMonthInMillis(Calendar cal) {
        Calendar calendar = (Calendar) cal.clone();
        calendar.set(Calendar.DAY_OF_MONTH, 1);
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);
        return calendar.getTimeInMillis();
    }

    long getEndOfMonthInMillis(Calendar cal) {
        Calendar calendar = (Calendar) cal.clone();
        calendar.set(Calendar.DAY_OF_MONTH, 1);
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);
        calendar.add(Calendar.MONTH, 1);//go to next month start day i.e 1 00;00:00.000
        return calendar.getTimeInMillis() - 1;//minus 1 will take to previous day i.e end of month 23:59:59.999
    }

}
