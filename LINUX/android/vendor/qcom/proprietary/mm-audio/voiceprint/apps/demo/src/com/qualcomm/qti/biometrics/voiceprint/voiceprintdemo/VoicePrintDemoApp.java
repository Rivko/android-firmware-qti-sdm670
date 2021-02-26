/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo;

import android.app.Application;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.PointF;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.ViewGroup;
import android.view.View;
import android.view.WindowManager;
import android.graphics.PixelFormat;
import android.content.Context;
import android.os.Bundle;
import android.widget.Toast;
import android.widget.ImageView;
import android.view.Gravity;
import android.os.Handler;
import android.view.animation.DecelerateInterpolator;
import android.view.animation.AccelerateInterpolator;
import com.qualcomm.qti.biometrics.voiceprint.sdk.VoicePrintManager;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.AudioRecordManager;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhrase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseAction;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultBase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintPreferences;
import java.io.File;

import java.util.List;

public class VoicePrintDemoApp extends Application {

    private final static String TAG = VoicePrintDemoApp.class.getSimpleName();

    //    private KeyPhraseList mKeyPhraseList = null;
    private KeyPhrase mCurrentKeyPhrase = new KeyPhrase();

    private KeyPhraseAction[] mActions;

    private static VoicePrintDemoApp mInstance;

    private VoicePrintManager mManager;

    private DebugView debugView;
    private ImageView spoofView;

    private boolean debugMode = false;

    private static final int SPOOF_VIEW_START = -200;

    public VoicePrintDemoApp() {

    }

    @Override
    public void onCreate() {
        super.onCreate();
        VoicePrintDatabase.instantiate(this);
        AudioRecordManager.instantiate(this);
        createActions();
        mInstance = this;
        mManager = VoicePrintManager.newInstance(this);

        // Debug View
        debugView = new DebugView(this);

        // Spoof View
        spoofView = new ImageView(this);
        spoofView.setImageDrawable(getResources().getDrawable(R.drawable.spoof));

        WindowManager windowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
        WindowManager.LayoutParams params = new WindowManager.LayoutParams(
            WindowManager.LayoutParams.FILL_PARENT,
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.TYPE_SYSTEM_OVERLAY,
            WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN,
            PixelFormat.TRANSLUCENT);
        params.x = SPOOF_VIEW_START;
        params.y = 0;
        params.gravity = Gravity.TOP;

        spoofView.setAlpha(0);

        windowManager.addView(
            spoofView,
            params);
    }

    public static final String DATA = "DATA";

    String mDebugText = null;

    public void update(Bundle data) {
        try {
            if ( data == null ) {
                mDebugText = null;
            } else {
                mDebugText = data.getString(OnResultBase.DEBUG);
                if ( mDebugText != null ) {
                    Log.d(TAG, mDebugText);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        // Log.d(TAG, "Try draw");
        debugView.invalidate();
    }

    public boolean processMatchScore(Bundle extra) {
        Log.d(TAG, "processMatchScore");

        float sentence_score = VoicePrintPreferences.getSentenceScore(this);
        float user_score = VoicePrintPreferences.getUserScore(this);
        float spoof_score = VoicePrintPreferences.getSpoofScore(this);


        Log.d(TAG, "sentence_score must be > " + sentence_score);
        Log.d(TAG, "user_score must be > " + user_score);
        Log.d(TAG, "spoof_score must be > " + spoof_score);

        String text = extra.getString(OnResultBase.DEBUG);

        boolean success = false;

        String newline = System.getProperty("line.separator");
        String[] tokens = text.split(newline);

        for( String s : tokens ){
            Log.d(TAG, "processing " + s);
            String[] kvp = s.split("=");

            if( (kvp[0] != null) && (kvp[1] != null) ) {

                if( kvp[0].equals("sentence_score")) {
                    success = (Float.parseFloat(kvp[1]) >= sentence_score);
                } else if( kvp[0].equals("user_score")) {
                    success = (Float.parseFloat(kvp[1]) >= user_score);
                } else if( kvp[0].equals("spoof_score")) {
                    success = (Float.parseFloat(kvp[1]) >= spoof_score);
                    if( false == success ){
                        //Toast.makeText(this, "Spoof Detected.", Toast.LENGTH_SHORT).show();
                        animateSpoof();
                    }
                } else if ( kvp[0].equals("index") ){
                    // Index has no affect on score
                    continue;
                }
            }

            Log.d(TAG, "success=" + success);

            // short-circuit if any below threshold
            if( false == success ) {
                break;
            }
        }

        return success;
    }

    public void onLongBackPress() {

        // Long Press back will switch mode
        debugMode = !debugMode;

        WindowManager windowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
        //Log.d(TAG, "debugMode=" + debugMode);

        try {
            if ( debugMode ) {

                windowManager.addView(
                    debugView,
                    new WindowManager.LayoutParams(
                        WindowManager.LayoutParams.FILL_PARENT,
                        WindowManager.LayoutParams.FILL_PARENT,
                        WindowManager.LayoutParams.TYPE_SYSTEM_OVERLAY,
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN,
                        PixelFormat.TRANSLUCENT));
            } else {
                windowManager.removeView(debugView);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void animateSpoof() {
        // Use alpha to set visibility. Do not use View.setVisibility as it is too slow.
        spoofView.setAlpha(0xFF);

        // The animation is nested to ensure that each animation starts at the correct time.

        // Fade in and then out of view
        spoofView.animate().alpha(1).setDuration(1000).setInterpolator(new DecelerateInterpolator()).withEndAction(new Runnable() {
            @Override
            public void run() {
                spoofView.animate().alpha(0).setDuration(1000).setInterpolator(new AccelerateInterpolator()).start();
            }
        }).start();
    }

    /**
    * The Class DebugView.
    */
    class DebugView extends View {


        /** The text paint. */
        private Paint mTextPaint;


        /**
         * Instantiates a new count down view.
         *
         * @param context
         *            the context
         */
        public DebugView(Context context) {
            super(context);

            init();

        }

        /**
         * Inits the.
         */
        private void init() {
            mTextPaint = new Paint();
            mTextPaint.setAntiAlias(true);
            mTextPaint.setTextSize(40);
            mTextPaint.setTypeface(Typeface.DEFAULT);
            mTextPaint.setTextAlign(Paint.Align.CENTER);
            mTextPaint.setColor(Color.BLACK);
            mTextPaint.setAlpha(0xff);

        }


        /*
        * (non-Javadoc)
        *
        * @see android.view.View#onDraw(android.graphics.Canvas)
        */
        @Override
        protected void onDraw(Canvas canvas) {
            try {
                //Log.d(TAG, "Draw");
                //canvas.drawText("HELLO!!!!", 100, 100, mTextPaint);

                if ( mDebugText != null ) {
                    canvas.drawText(mDebugText, 800, 300, mTextPaint);
                } else {
                    Log.d(TAG, "mDebugText is null");
                }

            } catch (Exception e) {
                e.printStackTrace();
                Log.d(TAG, "Excepting in drawing view " + e.getMessage());

            }


        }

    }

    public static VoicePrintDemoApp getInstance() {
        return mInstance;
    }

    public VoicePrintManager getManager() {
        return mManager;
    }

    public KeyPhraseAction getAction(int id) {
        if (id >= 0 && id < mActions.length) {
            return mActions[id];
        }
        return null;
    }


    public String getCurrentActionName() {
        KeyPhraseAction action = null;
        if (mCurrentKeyPhrase != null) {
            action = getAction(mCurrentKeyPhrase.getActionId());
        }
        return action == null ? "" : action.toString();
    }

    private void createActions() {
        mActions = new KeyPhraseAction[3];

        // Find by traversing list for email clients.
        // Use first one found.

        // Email
        Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_APP_EMAIL);
        PackageManager pkgManager = getPackageManager();
        List<ResolveInfo> activities = pkgManager.queryIntentActivities(intent, 0);
        if (activities.size() > 0) {
            ResolveInfo resolveInfo = activities.get(0);
            mActions[KeyPhraseAction.EMAIL] = new KeyPhraseAction(KeyPhraseAction.EMAIL,
                    getString(R.string.email), resolveInfo);

        }

        // Browser
        intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_APP_BROWSER);
        activities = pkgManager.queryIntentActivities(intent, 0);
        if (activities.size() > 0) {
            ResolveInfo resolveInfo = activities.get(0);
            mActions[KeyPhraseAction.BROWSER] = new KeyPhraseAction(KeyPhraseAction.BROWSER,
                    getString(R.string.google), resolveInfo);
        }

        // SMS
        intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_APP_MESSAGING);
        activities = pkgManager.queryIntentActivities(intent, 0);
        if (activities.size() > 0) {
            ResolveInfo resolveInfo = activities.get(0);
            mActions[KeyPhraseAction.SMS] = new KeyPhraseAction(KeyPhraseAction.SMS,
                    getString(R.string.sms), resolveInfo);
        }
    }

    public void setKeyPhrase(KeyPhrase phrase) {

        mCurrentKeyPhrase = phrase;

        if ( mCurrentKeyPhrase != null ) {
            Log.d(TAG, "Updating current keyphrase");
            Log.d(TAG, mCurrentKeyPhrase.toString());
        }
    }

    public void setKeyPhraseName(String name) {
        mCurrentKeyPhrase.setName(name);
    }

    public void setKeyPhrase(String phrase) {
        mCurrentKeyPhrase.setPhrase(phrase);
    }

    public KeyPhrase getCurrentKeyPhrase() {
        return mCurrentKeyPhrase;
    }

    public void setKeyPhraseAction(int action) {
        mCurrentKeyPhrase.setActionId(action);
    }

    public void setKeyPhraseAction(KeyPhraseAction action) {
        mCurrentKeyPhrase.setActionId(action.getId());
    }
}
