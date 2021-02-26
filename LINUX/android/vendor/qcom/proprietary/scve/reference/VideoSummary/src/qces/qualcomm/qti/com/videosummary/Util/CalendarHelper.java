/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.Util;

import android.content.Context;
import android.util.Log;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;

import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;

/**
 * Created by mithun on 8/26/15.
 */
public class CalendarHelper {
    private static final String TAG = CalendarHelper.class.getSimpleName();

    private static final String[] dayInWeek = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    private static final String[] monthInYear = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    public static void getAugustMonthMediaUris(Context context) {
        //long startDate = getCalendarForDate(2015, Calendar.JULY, 11).getTimeInMillis();
        //long endDate =  System.currentTimeMillis();

        Calendar startDate = Calendar.getInstance();
        startDate.set(Calendar.MONTH, Calendar.AUGUST);
        startDate.set(Calendar.DATE, 1);
        startDate.set(Calendar.HOUR, 0);
        startDate.set(Calendar.MINUTE, 0);
        startDate.set(Calendar.SECOND, 0);
        startDate.set(Calendar.MILLISECOND, 0);


        Calendar endDate = Calendar.getInstance();
        endDate.set(Calendar.MONTH, Calendar.AUGUST);
        endDate.set(Calendar.DATE, 31);
        endDate.set(Calendar.HOUR, 23);
        endDate.set(Calendar.MINUTE, 59);
        endDate.set(Calendar.SECOND, 59);
        endDate.set(Calendar.MILLISECOND, 999);


        ArrayList<MediaInfo> list = QueryHelper.queryMediaStore(context, startDate.getTimeInMillis(), endDate.getTimeInMillis());
        Log.i(TAG, "uriList length = " + list.size());

    }


    public static Calendar getCalendarForDate(int year, int month, int day) {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.set(Calendar.YEAR, year);
        calendar.set(Calendar.MONTH, month);
        calendar.set(Calendar.DATE, day);
        calendar.set(Calendar.HOUR_OF_DAY, 0);//midnight
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        return calendar;
    }

    public static String getTimeString(long seconds) {
        String retVal = "";

        long hours = seconds / 3600;
        long minutes = (seconds - hours * 3600) / 60;
        long sec = seconds - (hours * 3600 + minutes * 60);

        if (hours > 0) {
            retVal = hours + ":";
        }
        String minAndSec = String.format("%d:%02d", minutes, sec);
        return retVal + minAndSec;
    }

    public static String getTimeStringMillis(long elapsedMillis) {
        String retVal = "";
        long millisec = elapsedMillis % 1000;
        long seconds = elapsedMillis / 1000;
        long hours = seconds / 3600;
        long minutes = (seconds - hours * 3600) / 60;
        long sec = seconds - (hours * 3600 + minutes * 60);

        if (hours > 0) {
            retVal = hours + ":";
        }
        String minAndSec = String.format("%d:%02d.%03d", minutes, sec, millisec);
        return retVal + minAndSec;
    }

    public static String timeStampMillisToString(long timeStampMillis) {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(timeStampMillis);
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy.MM.dd 'at' HH:mm:ss.SSS");
        return sdf.format(calendar.getTime());
    }

    public static String timeStampToDateString(long timeStampMillis) {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(timeStampMillis);
        SimpleDateFormat sdf = new SimpleDateFormat("MM/dd/yyyy hh:mm a");
        return sdf.format(calendar.getTime());
    }

    public static String getDayOfWeekFromCalendar(Calendar calendar) {
        int day = calendar.get(Calendar.DAY_OF_WEEK);
        Log.i(TAG, "Day of week = " + day);
        return dayInWeek[day - 1];
    }

    public static String getMonthFromCalendar(Calendar calendar) {
        int month = calendar.get(Calendar.MONTH);
        Log.i(TAG, "month = " + month);
        return monthInYear[month];
    }

    public static int getStartMondayDateOfWeekFromCalendar(Calendar calendar) {
        calendar.set(Calendar.DAY_OF_WEEK, Calendar.MONDAY);
        int date = calendar.get(Calendar.DATE);
        Log.i(TAG, "start date = " + date);
        return date;
    }

    public static int getEndSundayDateOfWeekFromCalendar(Calendar calendar) {
        //get the start day i.e monday date and add 6 more days to get the next sundays date
        calendar.set(Calendar.DAY_OF_WEEK, Calendar.MONDAY);
        calendar.add(Calendar.DATE, 6);
        int date = calendar.get(Calendar.DATE);
        Log.i(TAG, "end date = " + date);
        return date;
    }

    public static long addOneWeek(long timeStampMillis) {
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(timeStampMillis);
        calendar.add(Calendar.DATE, 7);
        return calendar.getTimeInMillis();
    }

    public static long getInitialValForAssemblyTableUpdateTimestamp() {
        Calendar cal = Calendar.getInstance();
        //cal.setTimeInMillis(cal.getTimeInMillis() - 10 * 24 * 60 * 60 * 1000);//go back by 10 days;
        //cal.set(2015, Calendar.JUNE, 26);
        cal.set(Calendar.HOUR_OF_DAY, 0);
        cal.set(Calendar.MINUTE, 0);
        cal.set(Calendar.SECOND, 0);
        cal.set(Calendar.MILLISECOND, 0);
        return cal.getTimeInMillis();
    }


}
