/**
 * Copyright (c) 2013, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.calendarwidget;

import android.app.PendingIntent;
import android.appwidget.AppWidgetManager;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.provider.CalendarContract;
import android.text.format.Time;
import android.util.Log;
import android.view.View;
import android.widget.RemoteViews;

/**
 * We used this class to manager the CalendarWidget. It will control the widgets
 * date, and update the views of the widgets for the date.
 */
public class WidgetManager {
    private static final String TAG = "WidgetManager";

    private static int sYear = -1;
    private static int sMonth = -1;
    public static boolean mHasPermission = true;

    public static boolean checkPermission(Context context) {
        Cursor cursor = null;
        try {
            cursor = context.getContentResolver().query(
            CalendarContract.SyncState.CONTENT_URI, null, null, null, null);
            return true;
        } catch (java.lang.SecurityException e) {
            return false;
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }
    /**
     * To update the views (contains {@link CommonViews},
     * {@link WeekNumberViews} and {@link DateViews}) for all the widgets at the
     * saved date.
     *
     * @param context
     *            the application context
     * @param appWidgetManager
     *            the app widget manager
     * @param appWidgetIds
     *            the ids of this widget
     */
    public static void updateWidgets(Context context,
            AppWidgetManager appWidgetManager, int[] appWidgetIds) {
        mHasPermission = checkPermission(context);
        if (Utility.DEBUG) {
            Log.i(TAG, "WidgetManager will update the widgets.");
        }
        if (sYear == -1 || sMonth == -1) {
            gotoToday();
        }

        RemoteViews views = new RemoteViews(context.getPackageName(),
                R.layout.widget);
        if (!mHasPermission) {
            views.setViewVisibility(R.id.withoutPm, View.VISIBLE);
            views.setViewVisibility(R.id.content, View.GONE);
            Intent mRequestIntent = new Intent();
            mRequestIntent.setClass(context,PermissionActivity.class);
            views.setOnClickPendingIntent(R.id.widgetTop, PendingIntent
                    .getActivity(context, 0, mRequestIntent, PendingIntent.FLAG_CANCEL_CURRENT));
            CommonViews.update(context, appWidgetIds, views, sYear, sMonth);
            views.setViewVisibility(R.id.last_month,View.INVISIBLE);
            views.setViewVisibility(R.id.next_month,View.INVISIBLE);
            views.setViewVisibility(R.id.go_to_today,View.INVISIBLE);
        }
        else {
            views.setViewVisibility(R.id.withoutPm, View.GONE);
            views.setViewVisibility(R.id.content, View.VISIBLE);
            // the header views will be update with in WeekNameViews.
            CommonViews.update(context, appWidgetIds, views, sYear, sMonth);
            WeekNumberViews.update(context, appWidgetIds, views, sYear, sMonth);
            DateViews.update(context, appWidgetIds, views, sYear, sMonth);
            views.setViewVisibility(R.id.last_month,View.VISIBLE);
            views.setViewVisibility(R.id.next_month,View.VISIBLE);
            views.setViewVisibility(R.id.go_to_today,View.VISIBLE);
        }
        if (appWidgetManager != null) {
            appWidgetManager.updateAppWidget(appWidgetIds, views);
        }
    }

    /**
     * Set the date the widgets want to show. This will be called by
     * {@link ChooseDateActivity#onClick(android.view.View)}
     *
     * @param context
     *            the application context
     * @param year
     *            the year number which we want to changed as
     * @param month
     *            the month number which we want to changed as
     */
    public static void setDate(Context context, int year, int month) {
        if (sYear != year || sMonth != month) {
            sYear = year;
            sMonth = month;
            // we need notify calendar widget update the views.
            Intent intent = new Intent(Utility.COMMAND_DATE_CHANGED);
            context.sendBroadcast(intent);
        }
    }

    /**
     * Set the date as today.
     */
    public static void gotoToday() {
        Time time = new Time();
        time.setToNow();
        sYear = time.year;
        sMonth = time.month;
    }

    /**
     * Set the date to last month
     */
    public static void gotoLastMonth(){
        if(sMonth == 0){
            sYear = sYear - 1;
            sMonth = sMonth - 1 + 12;
        }
        else {
            sMonth = sMonth - 1;
        }
    }

    /**
     * Set the date to next month
     */
    public static void gotoNextMonth(){
        if(sMonth == 11){
            sYear = sYear + 1;
            sMonth = sMonth + 1 - 12;
        }
        else {
            sMonth = sMonth + 1;
        }
    }

    /**
     * @return true if saved date as today, otherwise false
     */
    public static boolean isToday() {
        Time time = new Time();
        time.setToNow();
        if (sYear == time.year && sMonth == time.month) {
            return true;
        } else {
            return false;
        }
    }
}
