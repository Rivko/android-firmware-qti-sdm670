/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices;

import android.app.IntentService;
import android.content.Context;
import android.content.Intent;

import qces.qualcomm.qti.com.videosummary.AudioProcessing.AudioCodecManager;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;

/**
 * An {@link IntentService} subclass for handling asynchronous task requests in
 * a service on a separate handler thread.
 * <p/>
 */
public class AudioProcManagerService extends IntentService {

    private static final String TAG = AudioProcManagerService.class.getSimpleName();

    public static AudioCodecManager audioCodecManager;
    static Object audioProcTaskObject = new Object();

    /**
     * Creates an IntentService.  Invoked by your subclass's constructor.
     *
     * @param name Used to name the worker thread, important only for debugging.
     */
    public AudioProcManagerService() {
        super("audio");
    }

    private static void threadWait() {

        synchronized (audioProcTaskObject) {
            try {
                audioProcTaskObject.wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

    }

    private void threadContinue() {

        synchronized (audioProcTaskObject) {
            audioProcTaskObject.notifyAll();
        }

    }

    /**
     * Starts this service to perform action Change Music with the given parameters. If
     * the service is already performing a task this action will be queued.
     *
     * @see IntentService
     */
    public static void startActionChangeMusic(Context context, HighlightsModel item) {
        Intent intent = new Intent(context, AudioProcManagerService.class);
        intent.setAction(Constants.ACTION_CHANGE_MUSIC);
        intent.putExtra(Constants.EXTRA_HIGHLIGHTS_MODEL, item);
        context.startService(intent);
        threadWait();
    }

    public static void stopActionChangeMusic() {
        audioCodecManager.stop();
    }


    @Override
    protected void onHandleIntent(Intent intent) {

        if (intent != null) {

            final String action = intent.getAction();

            if (Constants.ACTION_CHANGE_MUSIC.equals(action)) {
                HighlightsModel highlightsItem = intent.getParcelableExtra(Constants.EXTRA_HIGHLIGHTS_MODEL);
                handleActionChangeMusic(highlightsItem);
            }

            threadContinue();

        }
    }

    /**
     * Handle action Change Music in the provided background thread with the provided
     * parameters.
     */
    private void handleActionChangeMusic(HighlightsModel item) {

        audioCodecManager = new AudioCodecManager(this, item);
        Thread processAudio = new Thread(audioCodecManager);
        processAudio.start();

        try {
            processAudio.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }


    }


}
