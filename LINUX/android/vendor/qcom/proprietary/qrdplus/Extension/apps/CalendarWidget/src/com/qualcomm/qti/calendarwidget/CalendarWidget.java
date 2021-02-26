/**
 * Copyright (c) 2013, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


package com.qualcomm.qti.calendarwidget;

import android.app.Service;
import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.content.*;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.CalendarContract.Calendars;
import android.util.Log;
import android.widget.RemoteViewsService;
import java.util.Calendar;

public class CalendarWidget extends AppWidgetProvider {

    private static final String TAG = "CalendarWidget";
    public static final Uri uri = Uri.parse("content://com.android.calendar/week_start_day");
    public static final String START_DAY = "start_day";
    public static final int UPDATE_START_DAY = 1;
    public static final int DEFAULT_START_DAY_TAG= -1;
    Handler mHandler;
    private int old_startDay = -2;

    private class WeekStartDayObserver extends ContentObserver {
        Context mContext=null;

        public WeekStartDayObserver(Context context,Handler handler) {
            super(handler);
            mHandler = handler;
            mContext =context;
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);
            ContentResolver contentResolver = mContext.getContentResolver();
            try {
                Cursor cursor = contentResolver.query(uri, null, null, null, null);
                if (cursor != null && cursor.moveToFirst()) {
                    int startDay = Integer.parseInt(cursor
                            .getString(cursor.getColumnIndex(START_DAY)));
                    if( startDay == old_startDay){
                        return;
                    }
                    else {
                        old_startDay = startDay;
                    }
                    if (startDay == DEFAULT_START_DAY_TAG) {
                        setStartDayPreference(Calendar.getInstance().getFirstDayOfWeek());
                    } else {
                        setStartDayPreference(startDay);
                    }
                    mHandler.sendEmptyMessage(UPDATE_START_DAY);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }

        }

        private  void setStartDayPreference(int startDay){
            SharedPreferences sharedPreferences = mContext.getSharedPreferences(START_DAY, Context
                    .MODE_PRIVATE);
            SharedPreferences.Editor editor = sharedPreferences.edit();
            editor.putInt(START_DAY, startDay);
            editor.commit();
        }
    }

    @Override
    public void onEnabled(Context context) {
        if (Utility.DEBUG) {
            Log.i(TAG, "Enable this widget.");
        }

        // If we enable this widget, we will set the date to today.
        WidgetManager.gotoToday();
        super.onEnabled(context);
    }


    @Override
    public void onDisabled(Context context) {
        if (Utility.DEBUG) {
            Log.i(TAG, "Disable this widget.");
        }

        // If we disable this widget, we need stop the services.
        context.stopService(new Intent(context, DateService.class));
        context.stopService(new Intent(context, CommonService.class));
        context.stopService(new Intent(context, WeekNumberService.class));
        super.onDisabled(context);
    }

    @Override
    public void onReceive(final Context context, Intent intent) {
        String action = intent.getAction();
        if (Utility.DEBUG) {
            Log.i(TAG, "receive the action:" + action);
        }

        final AppWidgetManager awm = AppWidgetManager.getInstance(context);
        final int[] ids = awm.getAppWidgetIds(new ComponentName(context, CalendarWidget.class));
        mHandler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what){
                    case UPDATE_START_DAY:
                        WidgetManager.updateWidgets(context,awm,ids);
                        break;
                    default:
                        break;
                }
            }
        };
        context.getContentResolver().registerContentObserver(uri,true,
                new WeekStartDayObserver(context,mHandler));


        if (Intent.ACTION_TIME_CHANGED.equals(action)
                || Intent.ACTION_DATE_CHANGED.equals(action)
                || Intent.ACTION_TIMEZONE_CHANGED.equals(action)) {
            // we need to go to today if we changed date manually.
            if (!WidgetManager.isToday()) {
                WidgetManager.gotoToday();
            }
            WidgetManager.updateWidgets(context, awm, ids);
        } else if (Utility.COMMAND_DATE_CHANGED.equals(action)
                || Utility.UPDATE_PERMISSION.equals(action)) {
            WidgetManager.updateWidgets(context, awm, ids);
        } else if (Utility.COMMAND_GOTO_TODAY.equals(action)) {
            if (!WidgetManager.isToday()) {
                WidgetManager.gotoToday();
                WidgetManager.updateWidgets(context, awm, ids);
            }
        }else if (Utility.COMMAND_GOTO_LAST_MONTH.equals(action)) {
            WidgetManager.gotoLastMonth();
            WidgetManager.updateWidgets(context, awm, ids);
        }else if (Utility.COMMAND_GOTO_NEXT_MONTH.equals(action)) {
            WidgetManager.gotoNextMonth();
            WidgetManager.updateWidgets(context, awm, ids);
        } else if (Utility.COMMAND_REFRESH.equals(action)) {
            // we need to request sync.
            if (Utility.getAccountCount(context) > 0) {
                Bundle extras = new Bundle();
                extras.putBoolean(ContentResolver.SYNC_EXTRAS_MANUAL, true);
                extras.putBoolean("metafeedonly", true);
                ContentResolver.requestSync(null /* all accounts */,
                        Calendars.CONTENT_URI.getAuthority(), extras);
                // update the refresh image state
                WidgetManager.updateWidgets(context, awm, ids);
            } else {
                Intent noAccount = new Intent();
                noAccount.setClass(context, NoAccountAlert.class);
                noAccount.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(noAccount);
            }
        } else if (Intent.ACTION_PACKAGE_CHANGED.equals(action)) {
            String packageName = intent.getData().getSchemeSpecificPart();
            if (Utility.CALENDAR_PACKAGE_NAME.equals(packageName)) {
                Log.d(TAG,
                        "Receive the calendar application update action. Update the widget state.");
                // we need update the add event button's state.
                WidgetManager.updateWidgets(context, awm, ids);
            }
        } else {
            super.onReceive(context, intent);
        }
    }

    @Override
    public void onUpdate(Context context, AppWidgetManager appWidgetManager,
            int[] appWidgetIds) {
        WidgetManager.updateWidgets(context, appWidgetManager, appWidgetIds);
        super.onUpdate(context, appWidgetManager, appWidgetIds);
    }

    /**
     * We use the DateService for: 1) To provide a widget factory for
     * RemoteViews. 2) Catch our command, and make the DateViews to process the
     * intent.
     */
    public static class DateService extends RemoteViewsService {

        @Override
        public RemoteViewsFactory onGetViewFactory(Intent intent) {
            int[] widgetIds = intent == null ? null : intent
                    .getIntArrayExtra(AppWidgetManager.EXTRA_APPWIDGET_ID);
            if (widgetIds == null) {
                return null;
            }

            // Find the existing widget or create it.
            return DateViews.getOrCreateViews(this.getApplicationContext(), widgetIds);
        }

        @Override
        public int onStartCommand(Intent intent, int flags, int startId) {
            if (intent != null && intent.getData() != null) {
                // DateViews creates intents, so it knows how to handle them.
                DateViews.processIntent(this, intent);
            }
            return Service.START_NOT_STICKY;
        }

    }

    public static class CommonService extends RemoteViewsService {

        @Override
        public RemoteViewsFactory onGetViewFactory(Intent intent) {
            int[] widgetIds = intent == null ? null : intent
                    .getIntArrayExtra(AppWidgetManager.EXTRA_APPWIDGET_ID);
            if (widgetIds == null) {
                return null;
            }

            // Find the existing widget or create it.
            return CommonViews.getOrCreateViews(this.getApplicationContext(), widgetIds);
        }

        @Override
        public int onStartCommand(Intent intent, int flags, int startId) {
            if (intent != null && intent.getData() != null) {
                // CommonViews creates intents, so it knows how to handle them.
                CommonViews.processIntent(this, intent);
            }
            return Service.START_NOT_STICKY;
        }

    }

    public static class WeekNumberService extends RemoteViewsService {

        @Override
        public RemoteViewsFactory onGetViewFactory(Intent intent) {
            int[] widgetIds = intent == null ? null : intent
                    .getIntArrayExtra(AppWidgetManager.EXTRA_APPWIDGET_ID);
            if (widgetIds == null) {
                return null;
            }

            // Find the existing widget or create it.
            return WeekNumberViews.getOrCreateViews(this.getApplicationContext(), widgetIds);
        }

    }

}
