/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti.poweroffalarm;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.media.AudioAttributes;
import android.media.AudioManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.UserHandle;
import android.os.Vibrator;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

public class PowerOffAlarmActivity extends Activity {
    private static final String TAG = "PowerOffAlarm";

    private static final String DEFAULT_RINGTONE = "content://media/internal/audio/media/1";

    // Default snooze time 10 minutes
    private static final long SNOOZE_TIME = 600000L;

    private static final long[] VIBRATE_PATTERN = {500, 500};

    private static final int MESSAGE_UPDATE_TIME = 1;

    private Ringtone mRingtone;

    private Vibrator mVibrator;

    private Button snoozeButton;

    private Button dismissButton;

    private TextView timeTextView;

    private boolean mIsAlarmBoot = false;

    private Context mContext;

    private TimeThread timeThread;

    private Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case MESSAGE_UPDATE_TIME:
                    long sysTime = System.currentTimeMillis();
                    CharSequence sysTimeStr = DateFormat.format("hh:mm:ss", sysTime);
                    timeTextView.setText(sysTimeStr);
                    break;
                default:
                    break;

            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_power_off_alarm);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
                | WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD
                | WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
                | WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON
                | WindowManager.LayoutParams.FLAG_ALLOW_LOCK_WHILE_SCREEN_ON);

        timeTextView = (TextView) findViewById(R.id.timeTextView);
        snoozeButton = (Button) findViewById(R.id.snoozeButton);

        mIsAlarmBoot = PowerOffAlarmUtils.isAlarmBoot();
        mContext = getApplicationContext();
        PowerOffAlarmPersistData persistData = new PowerOffAlarmPersistData();

        snoozeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                recover();

                // If snooze, set snooze time to rtc alarm and power off.
                long currentTime = System.currentTimeMillis();

                persistData.setSnoozeTime(currentTime + SNOOZE_TIME);
                persistData.setAlarmStatus(PowerOffAlarmUtils.ALARM_STATUS_SNOOZE);
                persistData.writeDataToFile();

                long result = PowerOffAlarmUtils.setAlarmToRtc(currentTime + SNOOZE_TIME);
                if (result == -1) {
                    Log.d(TAG, "Set snooze time to rtc register failed!");
                }

                if (mIsAlarmBoot) {
                    PowerOffAlarmUtils.powerOff(getApplicationContext());
                } else {
                    AlarmManager am = (AlarmManager) mContext.
                            getSystemService(Context.ALARM_SERVICE);
                    Intent activityIntent = new Intent(mContext, PowerOffAlarmActivity.class);
                    PendingIntent pendingIntent = PendingIntent.getActivity(mContext, 1,
                            activityIntent, PendingIntent.FLAG_UPDATE_CURRENT);
                    am.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, currentTime + SNOOZE_TIME,
                            pendingIntent);
                }

                finish();
            }
        });

        dismissButton = (Button) findViewById(R.id.dismissButton);
        dismissButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                recover();

                persistData.setAlarmStatus(PowerOffAlarmUtils.ALARM_STATUS_DISMISS);
                persistData.writeDataToFile();

                if (mIsAlarmBoot) {
                    // If dismiss, start alarm dialog
                    Intent dialogIntent = new Intent(PowerOffAlarmActivity.this,
                            PowerOffAlarmDialog.class);
                    dialogIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    PowerOffAlarmActivity.this.startActivityAsUser(dialogIntent,
                            UserHandle.CURRENT);
                }

                finish();
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        playRingTone();
        vibrate();

        if (timeThread == null) {
            timeThread = new TimeThread();
        }
        timeThread.start();
    }

    private void playRingTone() {
       try {
           if (mRingtone == null) {
               Uri ringtoneUri = Uri.parse(DEFAULT_RINGTONE);
               mRingtone = RingtoneManager.getRingtone(getApplicationContext(), ringtoneUri);
               mRingtone.setStreamType(AudioManager.STREAM_ALARM);
               mRingtone.setLooping(true);
               mRingtone.play();
           }
       } catch (Exception ex) {
           ex.printStackTrace();
       }
    }

    private void vibrate() {
        if (mVibrator == null) {
            mVibrator = (Vibrator) this.getSystemService(Context.VIBRATOR_SERVICE);
            mVibrator.vibrate(VIBRATE_PATTERN, 0, new AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_ALARM)
                    .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                    .build());
        }
    }

    private void recover() {
        if (mRingtone != null && mRingtone.isPlaying()) {
            mRingtone.stop();
        }

        if (mVibrator != null) {
            mVibrator.cancel();
        }

        if (timeThread != null) {
            timeThread.interrupt();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        recover();
    }

    class TimeThread extends Thread {
        @Override
        public void run() {
            try {
                do {
                    // update the time every second
                    Thread.sleep(1000);
                    Message msg = new Message();
                    msg.what = MESSAGE_UPDATE_TIME;
                    mHandler.sendMessage(msg);
                } while (!isInterrupted());
            } catch (InterruptedException ie) {
                Log.e(TAG, ie.toString());
            }
        }
    }
}
