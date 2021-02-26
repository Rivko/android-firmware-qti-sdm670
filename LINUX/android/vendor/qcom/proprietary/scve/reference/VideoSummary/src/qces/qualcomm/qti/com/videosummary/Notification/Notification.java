/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.Notification;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.media.RingtoneManager;
import android.preference.PreferenceManager;
import android.support.v4.app.NotificationCompat;
import android.util.Log;

import java.util.Calendar;
import java.util.LinkedHashSet;
import java.util.Set;

import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsActivity;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;
import qces.qualcomm.qti.com.videosummary.providers.VideoSummaryContract;
import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;

/**
 * Created by mithun on 10/1/15.
 */
public class Notification {
    public static final int NOTIFICATION_ID = 1;
    private static final String TAG = Notification.class.getSimpleName();

    Context context;
    long timeStampMillis;

    public Notification(Context context, long timestampMillis) {
        this.context = context;
        this.timeStampMillis = timestampMillis;
    }

    public void showNotification(boolean autoMode) {
        if (HighlightsActivity.isInForeground()) {
            //Since the activity is in foreground, do not show notification
            return;
        }

        String notificationTitle = context.getResources().getString(R.string.notification_multi_title);
        String notificationSummary = "Show title here as content";


        String summaryTitle = "";
        NotificationCompat.InboxStyle inboxStyle = null;
        String mp4FilePath = Utils.getMp4FilePath(context, timeStampMillis).toString();
        String thumbImgPath = Utils.getThumbImgFilePath(context, timeStampMillis).toString();
        Cursor cursor = QueryHelper.queryHighlights(context, mp4FilePath);
        HighlightsModel.SummaryType summaryType = HighlightsModel.SummaryType.HIGHLIGHTS;

        //if cursor is null i.e was not able to find an entry for this timestamp key in Highlights db, then we came here during ScoringOnly completed, so just return
        if (cursor != null && cursor.moveToFirst()) {
            summaryType = HighlightsModel.SummaryType.values()[cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._SUMMARY_TYPE))];
            summaryTitle = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._TITLE));


            //get titles if any and then add current title & save the set, if set was previously empty we wil have only one now,so show Highlights or timelapse else show generic message
            Set<String> notificationSummarySet = getPrefNotificationSummary(context);

            /*since Set<> is a collection of unique objects and cannot have duplicates, when we have
          multiple summary with same title it wont insert the duplicate, so appending the timestamp
          to make string unique. Later when printing it in inboxStyle stripping off the "<timestamp>#"*/
            notificationSummarySet.add(timeStampMillis + "#" + summaryTitle);
            setPrefNotificationSummary(context, notificationSummarySet);

            //TODO: if its auto mode then check time is night then set alarm (save the current highlight/timelapse timestamp or pass as intent) for morning 9 am and return.
            //this will keep accumulating the notifications in the shared pref.
            //once the alarm fires, then use the saved timestamp (passed as intentExtra) and call Notification.show with this timestamp and it would flush out all the pending notifications

            if (autoMode) {
                Calendar currentTime = Calendar.getInstance();
                int hourOfTheDay = currentTime.get(Calendar.HOUR_OF_DAY);

                //This "if condition" will work only for night silent time i.e > 9pm and < 9am, as the time will roll over after 23:59:59 to 0:0:0 so using "OR" condition
                if ((hourOfTheDay >= Constants.AFTER_HOURS_START) || (hourOfTheDay < Constants.AFTER_HOURS_END) || true) {
                    //Its night time, do not disturb user if its auto mode creation
                    AutoPendingNotificationTrigger.getInstance().setAlarmMorning(context, timeStampMillis);
                    return;
                }
            }
            int count = notificationSummarySet.size();
            if (count == 1) {
                if (summaryType == HighlightsModel.SummaryType.HIGHLIGHTS) {
                    notificationTitle = context.getResources().getString(R.string.notification_highlights_title);
                } else {
                    notificationTitle = context.getResources().getString(R.string.notification_timelapse_title);
                }
                notificationSummary = "Play " + summaryTitle;

            } else {

                inboxStyle = new NotificationCompat.InboxStyle();

                //setBigContentTitle is the title "Your videos are ready" shown when notification is showing expanded view i.e list of titles
                inboxStyle.setBigContentTitle(notificationTitle);

                for (String summaryLine : notificationSummarySet) {
                    String summaryLineSplit[] = summaryLine.split("#");
                    inboxStyle.addLine(summaryLineSplit[1]);
                }

                notificationSummary = count + " " + context.getResources().getString(R.string.videos_available);

                //inboxSet.setSummaryText will show up at the bottom as summary "3 videos available" after the list of titles
                inboxStyle.setSummaryText(notificationSummary);
            }

            Log.i(TAG, "Status - notification count = " + count);
            sendSummaryCompletedNotification(context, notificationTitle, notificationSummary, thumbImgPath, count, inboxStyle);
        }
    }

    public void sendSummaryCompletedNotification(Context context, String title, String notificationSummary, String thumbImgPath, int count, NotificationCompat.InboxStyle inboxStyle) {

        //Bitmap largeIcon = BitmapFactory.decodeResource(context.getResources(), R.drawable.ic_videosummary_appicon);
        Bitmap largeIcon = null;
        if (count == 1) {
            //If we have only one summary video to notify, then show the thumb img of the summarized video as notification big icon.
            // And this icon has to be square
            largeIcon = BitmapFactory.decodeFile(thumbImgPath);
            int largeIconSize = Math.min(largeIcon.getWidth(), largeIcon.getHeight());
            int width = largeIcon.getWidth();
            int height = largeIcon.getHeight();
            int x, y;
            if (width > height) {
                x = (width - largeIconSize) / 2;
                y = 0;
            } else {
                y = (height - largeIconSize) / 2;
                x = 0;
            }

            largeIcon = Bitmap.createBitmap(largeIcon, x, y, largeIconSize, largeIconSize);
        }

        NotificationCompat.Builder mBuilder =
                new NotificationCompat.Builder(context)
                        .setSmallIcon(R.drawable.ic_videosummary_notification)
                        .setContentTitle(title)
                        .setLargeIcon(largeIcon)
                        .setContentText(notificationSummary)
                        .setNumber(count)
                        .setColor(context.getResources().getColor(R.color.primaryColor))
                        .setAutoCancel(true);
        if (inboxStyle != null) {
            mBuilder.setStyle(inboxStyle);
        }

        // Creates an explicit intent for an Activity in your app
        Intent resultIntent = new Intent(context, HighlightsActivity.class);

        PendingIntent resultPendingIntent = PendingIntent.getActivity(context, 0, resultIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        mBuilder.setContentIntent(resultPendingIntent);

        if ((SettingsActivity.getPrefCreateHighlightVideos(context) && !SettingsActivity.getPrefSendHighlightsNotifications(context))
                || (SettingsActivity.getPrefCreateTimelapseVideos(context) && !SettingsActivity.getPrefSendTimelapseNotifications(context))) {
            //dont disturb user with notification
        } else {
            //notification sound
            mBuilder.setSound(RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION));

            //light blink and viberation
            mBuilder.setLights(Color.BLUE, 500, 500);
            long[] pattern = {500, 500, 500, 500, 500, 500, 500, 500, 500};
            mBuilder.setVibrate(pattern);
        }
        NotificationManager mNotificationManager =
                (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        // mId allows you to update the notification later on.
        mNotificationManager.notify(NOTIFICATION_ID, mBuilder.build());
    }

    public static void cancelNotifications(Context context) {
        NotificationManager mNotificationManager =
                (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.cancel(NOTIFICATION_ID);

        //clear off the saved notification summary
        setPrefNotificationSummary(context, new LinkedHashSet<String>());

    }

    /**
     * This pref would store the title's of all the summary that are being notified, this title set
     * is shown when user expands the notification in case of multiple notifications.
     * this set will be deleted when the notification is cancelled, so next time when we are about
     * to show notification we see if this set is empty we have only one notification else append
     * the current notification title to this set and show all the titles in expanded view of
     * notification.
     *
     * @param context
     * @return set of strings which is the summary titles
     */
    public Set<String> getPrefNotificationSummary(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getStringSet(context.getResources().getString(R.string.pref_notification_title_set), new LinkedHashSet<String>());
    }

    public static void setPrefNotificationSummary(Context context, Set<String> stringSet) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putStringSet(context.getResources().getString(R.string.pref_notification_title_set), stringSet);
        editor.commit();
    }


}
