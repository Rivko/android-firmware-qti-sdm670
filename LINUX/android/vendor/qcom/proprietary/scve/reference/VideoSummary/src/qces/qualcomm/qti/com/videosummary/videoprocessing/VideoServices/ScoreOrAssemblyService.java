/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices;

import android.app.IntentService;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoAssembly.VideoCodecManager;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoScore.VideoDecodeScore;
import qces.qualcomm.qti.com.videosummary.watchdog.Watchdog;

/**
 * An {@link IntentService} subclass for handling asynchronous task requests in
 * a service on a separate handler thread.
 * <p/>
 */
public class ScoreOrAssemblyService extends IntentService {

    private static final String TAG = ScoreOrAssemblyService.class.getSimpleName();

    public static VideoCodecManager videoCodecManager;
    public static VideoDecodeScore videoDecodeScore;
    static Object scoreOrAssemblyTaskObject = new Object();

    /**
     * Starts this service to perform action Video Score with the given parameters. If
     * the service is already performing a task this action will be queued.
     *
     * @see IntentService
     */
    public static void startActionScore(Context context, Uri uri, long timeStampMillis, int currentUriIndex, int numUris) {
        Intent intent = new Intent(context, ScoreOrAssemblyService.class);
        intent.setAction(Constants.ACTION_SCORE);
        intent.putExtra(Constants.EXTRA_URI, uri);
        intent.putExtra(Constants.EXTRA_TIME_STAMP_MILLIS, timeStampMillis);
        intent.putExtra(Constants.EXTRA_CURRENT_URI_INDEX, currentUriIndex);
        intent.putExtra(Constants.EXTRA_NUM_URIS, numUris);
        context.startService(intent);

        threadWait();

    }

    public static void stopActionScore() {
        videoDecodeScore.stop();
    }

    private static void threadWait() {

        synchronized (scoreOrAssemblyTaskObject) {
            try {
                scoreOrAssemblyTaskObject.wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

    }

    private void threadContinue() {

        synchronized (scoreOrAssemblyTaskObject) {
            scoreOrAssemblyTaskObject.notifyAll();
        }

    }

    /**
     * Starts this service to perform action Video Assemble with the given parameters. If
     * the service is already performing a task this action will be queued.
     *
     * @see IntentService
     */
    public static void startActionAssemble(Context context, HighlightsModel item, boolean[] keepFrames, int numKeepFrames) {
        Intent intent = new Intent(context, ScoreOrAssemblyService.class);
        intent.setAction(Constants.ACTION_ASSEMBLE);
        intent.putExtra(Constants.EXTRA_HIGHLIGHTS_MODEL, item);
        intent.putExtra(Constants.EXTRA_VIDEO_KEEP_FRAMES, keepFrames);
        intent.putExtra(Constants.EXTRA_NUM_KEEP_FRAMES, numKeepFrames);

        context.startService(intent);

        threadWait();
    }

    public static void stopActionAssemble() {
        videoCodecManager.stop();
    }


    public ScoreOrAssemblyService() {
        super("ScoreOrAssemblyService");
    }

    @Override
    protected void onHandleIntent(Intent intent) {

        if (intent != null) {

            final String action = intent.getAction();

            if (Constants.ACTION_SCORE.equals(action)) {
                Watchdog.getWatchdogInstance().watchdogPet("ScoreOrAssemblyService: handling ACTION_SCORE");
                final Uri uri = intent.getParcelableExtra(Constants.EXTRA_URI);
                long timeStampMillis = intent.getLongExtra(Constants.EXTRA_TIME_STAMP_MILLIS, 0);

                int currentUriIndex = intent.getIntExtra(Constants.EXTRA_CURRENT_URI_INDEX, -1);
                int numUris = intent.getIntExtra(Constants.EXTRA_NUM_URIS, -1);

                //get the first frame score in order to see if video has been scored yet
                boolean isUriScored = QueryHelper.isScored(getApplicationContext(), uri);

                //if not scored, then score it
                if (!isUriScored) {
                    Log.d(TAG, "Video uri index: " + currentUriIndex + " not scored URI: " + uri.toString() + ". Scoring module started");
                    handleActionScore(uri, timeStampMillis, currentUriIndex, numUris);
                } else {
                    Log.d(TAG, "Video uri index: " + currentUriIndex + " already scored for URI: " + uri.toString());
                }

                Log.d(TAG, "Video scoring completed");

            } else if (Constants.ACTION_ASSEMBLE.equals(action)) {
                Watchdog.getWatchdogInstance().watchdogPet("ScoreOrAssemblyService: handling ACTION_ASSEMBLY");
                HighlightsModel highlightsItem = intent.getParcelableExtra(Constants.EXTRA_HIGHLIGHTS_MODEL);
                boolean[] keepFrames = intent.getBooleanArrayExtra(Constants.EXTRA_VIDEO_KEEP_FRAMES);
                int numKeepFrames = intent.getIntExtra(Constants.EXTRA_NUM_KEEP_FRAMES, 1);
                handleActionAssemble(highlightsItem, keepFrames, numKeepFrames);

                //AudioProcManagerService.startActionChangeMusic(this, highlightsItem);

            }
            threadContinue();

        }
    }


    /**
     * Handle action Score Video in the provided background thread with the provided
     * parameters.
     */
    private void handleActionScore(Uri uri, long timeStampMillis, int currentUriIndex, int numUris) {
        //for single video scoring and saving to DB
        videoDecodeScore = new VideoDecodeScore(this, uri, timeStampMillis, currentUriIndex, numUris);
        Thread decodeScore = new Thread(videoDecodeScore);
        decodeScore.start();

        Log.d(TAG, "decodeScore thread started for uri " + uri.toString());

        try {
            decodeScore.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        Log.d(TAG, "decodeScore thread completed for uri + " + uri.toString());

    }

    /**
     * Handle action Assemble Video in the provided background thread with the provided
     * parameters.
     */
    private void handleActionAssemble(HighlightsModel item, boolean[] keepFrames, int numKeepFrames) {
        videoCodecManager = new VideoCodecManager(this, item, keepFrames, numKeepFrames);
        Thread assemble = new Thread(videoCodecManager);
        assemble.start();

        Log.d(TAG, "Video assembly started");

        try {
            assemble.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        Log.d(TAG, "Video assembly finished");


    }

}
