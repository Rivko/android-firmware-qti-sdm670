/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices;

import android.app.IntentService;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Environment;
import android.os.StatFs;
import android.provider.OpenableColumns;
import android.util.Log;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.Utils.FRDataParser;
import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.PolicyManager.FrequencyScheduleAlarmReceiver;
import qces.qualcomm.qti.com.videosummary.PolicyManager.VideoProcessingCompleteReceiver;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.debug.DebugMainActivity;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsActivity;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;
import qces.qualcomm.qti.com.videosummary.providers.FRQueryHelper;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoAssembly.VideoCodecManager;
import qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager.CVManagerJNILib;
import qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager.CVSummaryOutput;
import qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager.Shot;
import qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager.SummaryOutput;
import qces.qualcomm.qti.com.videosummary.watchdog.Watchdog;

/**
 * An {@link IntentService} subclass for handling asynchronous task requests in
 * a service on a separate handler thread.
 * <p/>
 */
public class VidProcManagerService extends IntentService {

    private static final String TAG = VidProcManagerService.class.getSimpleName();

    private static final String ACTION_PROCESS = "qces.qualcomm.qti.com.videosummary.VideoProcessing.action.PROCESS_VIDEOS";
    private static final String ACTION_PROCESS_MUSIC = "qces.qualcomm.qti.com.videosummary.VideoProcessing.action.ACTION_PROCESS_MUSIC";
    private static final String ACTION_SCORE_RECORD = "qces.qualcomm.qti.com.videosummary.VideoProcessing.action.SCORE_RECORD_VIDEOS";

    private boolean[] totalKeepFrameIndexes;
    private int numKeepFrames;
    private static long currentlyServing = 0;

    enum SERVICE_STATUS {STARTING, SCORING, ASSEMBLING, PROCESSING_AUDIO, INTERRUPTED, STOPPED}

    private static SERVICE_STATUS status = SERVICE_STATUS.STOPPED;

    private static HashMap<Long, Byte> toBeKilledProcessQueue = new HashMap<>();

    /**
     * Starts this service to perform action of processing a series of Video Uris. If
     * the service is already performing a task this action will be queued.
     *
     * @see IntentService
     */
    public static void startActionProcessMusic(Context context, HighlightsModel item) {
        status = SERVICE_STATUS.STARTING;
        currentlyServing = item.timeStampMillis;
        Intent intent = new Intent(context, VidProcManagerService.class);
        intent.setAction(ACTION_PROCESS_MUSIC);
        intent.putExtra(Constants.EXTRA_HIGHLIGHTS_MODEL, item);
        context.startService(intent);
    }

    public static void startActionScoreRecord(Context context, ArrayList<Uri> uris, long timeStampMillis) {
        Intent intent = new Intent(context, VidProcManagerService.class);
        intent.setAction(ACTION_SCORE_RECORD);
        intent.putParcelableArrayListExtra(Constants.EXTRA_URIS, uris);
        intent.putExtra(Constants.EXTRA_TIME_STAMP_MILLIS, timeStampMillis);
        context.startService(intent);
    }

    public static void startActionProcess(Context context, HighlightsModel item) {
        Intent intent = new Intent(context, VidProcManagerService.class);
        intent.setAction(ACTION_PROCESS);
        intent.putExtra(Constants.EXTRA_HIGHLIGHTS_MODEL, item);
        context.startService(intent);
    }

    public static void stopActionProcess(long timeStampMillis) {
        if (currentlyServing != timeStampMillis) {
            //the swipe/dismiss happened on a different CREATING card type item which is not the current one being serviced
            //so add it to the toBeKilledProcessQueue
            toBeKilledProcessQueue.put(timeStampMillis, (byte) 0);//insert a non null value
            return;
        }

        switch (status) {
            case STARTING:
            case SCORING:
                status = SERVICE_STATUS.INTERRUPTED;
                ScoreOrAssemblyService.stopActionScore();
                break;
            case ASSEMBLING:
                status = SERVICE_STATUS.INTERRUPTED;
                ScoreOrAssemblyService.stopActionAssemble();
                break;
            case PROCESSING_AUDIO:
                status = SERVICE_STATUS.INTERRUPTED;
                AudioProcManagerService.stopActionChangeMusic();
                break;
            case STOPPED:
                Log.i(TAG, "Already stopped");

        }
    }

    public VidProcManagerService() {
        super("VidProcManagerService");
    }


    @Override
    protected void onHandleIntent(Intent intent) {
        if (intent != null) {

            final String action = intent.getAction();


            if (ACTION_PROCESS.equals(action)) {
                status = SERVICE_STATUS.STARTING;


                do {
                    Watchdog.getWatchdogInstance().watchdogKick(this);
                    HighlightsModel highlightsItem = intent.getParcelableExtra(Constants.EXTRA_HIGHLIGHTS_MODEL);
                    long timeStampMillis = highlightsItem.timeStampMillis;
                    currentlyServing = timeStampMillis;
                    if (toBeKilledProcessQueue.remove(currentlyServing) != null) {
                        status = SERVICE_STATUS.INTERRUPTED;
                        break;
                    }
                    ArrayList<Uri> uris = MediaInfo.getArrayListOfUris(highlightsItem.compositonMediaList);

                    HighlightsActivity.sendProgressUpdate(this, timeStampMillis, 0, uris.size(), 0);//Scoring

                    //first score any unscored videos
                    int currentUriIndex = 1;
                    for (Uri u : uris) {
                        Watchdog.getWatchdogInstance().watchdogPet("VidProcMgrServ, action_process, start score for URI:" + u.toString());
                        Log.d(TAG, "VidProcManager: scoring URI: " + u.toString());
                        HighlightsActivity.sendProgressUpdate(this, timeStampMillis, currentUriIndex, uris.size(), 0);//Scoring
                        if (status != SERVICE_STATUS.INTERRUPTED) {
                            status = SERVICE_STATUS.SCORING;
                            ScoreOrAssemblyService.startActionScore(this, u, timeStampMillis, currentUriIndex, uris.size());
                        } else {
                            break;//break out of for loop
                        }

                        currentUriIndex++;
                        Log.d(TAG, "VidProcManager: finished scoring URI: " + u.toString());
                    }
                    if (status == SERVICE_STATUS.INTERRUPTED) {
                        break;//break to the end of this do while
                    }
                    Watchdog.getWatchdogInstance().watchdogPet("VidProcMgrServ, action_process, finished all scoring:");
                    int errorType = Constants.SUMMARY_ERROR_UNKNOWN;
                    boolean isEnoughSpace = checkEnoughSpace(uris);
                    if (!isEnoughSpace) {
                        errorType = Constants.SUMMARY_ERROR_DISK_SPACE_EXCEEDED;
                    }

                    boolean sufficientInputMedia = false;
                    if (isEnoughSpace) {
                        sufficientInputMedia = calculateShots(uris, highlightsItem);
                        if (!sufficientInputMedia) {
                            errorType = Constants.SUMMARY_ERROR_LENGTH_INSUFFICIENT;
                        }
                    }

                    Watchdog.getWatchdogInstance().watchdogPet("VidProcMgrServ, finished calculating shots");
                    if (isEnoughSpace && sufficientInputMedia) {
                        HighlightsActivity.sendProgressUpdate(this, timeStampMillis, -1, -1, 0);//Assembling
                        if (status == SERVICE_STATUS.INTERRUPTED) {
                            break;
                        }
                        status = SERVICE_STATUS.ASSEMBLING;
                        ScoreOrAssemblyService.startActionAssemble(this, highlightsItem, totalKeepFrameIndexes, numKeepFrames);
                        if (status == SERVICE_STATUS.INTERRUPTED) {
                            break;
                        }
                        status = SERVICE_STATUS.PROCESSING_AUDIO;
                        AudioProcManagerService.startActionChangeMusic(this, highlightsItem);
                        VideoProcessingCompleteReceiver.registerForVideoProcessingComplete(this);
                        VideoProcessingCompleteReceiver.sendVideoProcessingCompleteBroadcast(this, timeStampMillis);
                    } else {
                        //we don't have enough to assemble a video, so show an error card in highlights
                        QueryHelper.updateHighlightsAsError(this, Utils.getMp4FilePath(this, timeStampMillis).toString(), errorType);
                        VideoProcessingCompleteReceiver.registerForVideoProcessingComplete(this);
                        VideoProcessingCompleteReceiver.sendVideoProcessingCompleteBroadcast(this, timeStampMillis);
                    }
                } while (false);
                status = SERVICE_STATUS.STOPPED;
                Watchdog.getWatchdogInstance().watchdogSleep();


            } else if (ACTION_SCORE_RECORD.equals(action)) {
                ArrayList<Uri> uris = intent.getParcelableArrayListExtra(Constants.EXTRA_URIS);
                long timeStampMillis = intent.getLongExtra(Constants.EXTRA_TIME_STAMP_MILLIS, 0);
                int currentUriIndex = 1;
                for (Uri u : uris) {
                    FrequencyScheduleAlarmReceiver.cancelTimeoutAlarm(this);
                    FrequencyScheduleAlarmReceiver.setTimeoutAlarm(this, System.currentTimeMillis() + Utils.ALARM_TIMEOUT);

                    //passing 0 as timestamp for background scoring, this helps in not sending progress broadcast to UI thread.
                    ScoreOrAssemblyService.startActionScore(this, u, 0, currentUriIndex, uris.size());
                    Utils.sendMessage(this, TAG, "COMPLETED scoring uri: " + u.toString());
                    if (timeStampMillis == 0) {
                        DebugMainActivity.queryAndSaveScores(this, u);
                    }
                    currentUriIndex++;
                }
                VideoProcessingCompleteReceiver.registerForVideoProcessingComplete(this);
                VideoProcessingCompleteReceiver.sendVideoProcessingCompleteBroadcast(this, timeStampMillis);
            } else if (ACTION_PROCESS_MUSIC.equals(action)) {
                HighlightsModel highlightsItem = intent.getParcelableExtra(Constants.EXTRA_HIGHLIGHTS_MODEL);
                Watchdog.getWatchdogInstance().watchdogKick(this);
                do {
                    if (status == SERVICE_STATUS.INTERRUPTED) {
                        break;
                    }
                    status = SERVICE_STATUS.PROCESSING_AUDIO;
                    AudioProcManagerService.startActionChangeMusic(this, highlightsItem);
                } while (false);
                status = SERVICE_STATUS.STOPPED;
                Watchdog.getWatchdogInstance().watchdogSleep();
            }
        }
    }

    private boolean checkEnoughSpace(ArrayList<Uri> uris) {

        long totalUriBytes = 0;

        for (Uri u : uris) {
            Cursor mCursor = getContentResolver().query(u, null, null, null, null);
            int sizeIndex = mCursor.getColumnIndex(OpenableColumns.SIZE);
            mCursor.moveToFirst();
            totalUriBytes += mCursor.getLong(sizeIndex);
        }

        StatFs stat = new StatFs(Environment.getExternalStorageDirectory().getPath());
        long bytesAvailable = stat.getBlockSizeLong() * stat.getAvailableBlocksLong();

        if (bytesAvailable > 2 * totalUriBytes)
            return true;
        else
            return false;

    }

    private boolean calculateShots(ArrayList<Uri> uris, HighlightsModel item) {

        int targetLength = 0;
        int summaryMode = 0;
        byte compressRatio = 0;

        int timelapse_event_speed = 0;
        int timelapse_playback_speed = 0;

        boolean timelapseOn = false;

        // |-----------------|------------------|---------------|
        // |                 | Compress Ratio   | Target Length |
        // |    6 sec        |       0          |      6        |
        // |   15 sec        |       0          |      15       |
        // |   30 sec        |       0          |      30       |
        // | Long (low comp) |       1          |      0        |
        // | Short(High comp)|       2          |      0        |
        // | Manual(medium)  |       3          |      0        |
        // | Timelapse       |       0          |      0        |
        // |-----------------|------------------|---------------|

        if (item.summaryType == HighlightsModel.SummaryType.HIGHLIGHTS) {
            //we use all modes when summarizing for highlights
            summaryMode = 1 << Constants.MODE_BASIC_INDEX |
                    1 << Constants.MODE_FACE_INDEX |
                    1 << Constants.MODE_MOTION_INDEX |
                    1 << Constants.MODE_ASD_INDEX;
            if (item.getLength() > 3) {
                compressRatio = 0;
                targetLength = item.getLength() * VideoCodecManager.FINAL_ENCODED_FRAME_RATE;
            } else {
                compressRatio = (byte) item.getLength();
                targetLength = 0;
            }
        } else {
            //TIMELAPSE mode

            //in timelapse mode we summarize for at least basic, with face and motion user-controlled
            summaryMode = 1 << Constants.MODE_BASIC_INDEX; //always do at least basic
            if (item.getFaceEventDetectionEnabled())
                summaryMode |= 1 << Constants.MODE_FACE_INDEX;
            if (item.getMotionEventDetectionEnabled())
                summaryMode |= 1 << Constants.MODE_MOTION_INDEX;

            timelapse_event_speed = item.getEventSpeed();
            timelapse_playback_speed = item.getSpeed();
            timelapseOn = true;
            compressRatio = 0;
            targetLength = 0;
        }

        boolean doAssembly = true;

        //declare an array of videos, each having an array of scores
        int[][] allVideoScores = new int[uris.size()][];

        //declare an array of videos, each having an array of face tables
        byte[][] allVideoFRTables = new byte[uris.size()][];

        int totalAvailableFrames = 0;
        ArrayList<Integer> videoFrameCnt = new ArrayList<>();

        for (int i = 0; i < uris.size(); i++) {
            int[] videoScores = QueryHelper.getScores(this, uris.get(i));
            byte[] videoFRTable = QueryHelper.getFaceIds(this, uris.get(i));

            allVideoFRTables[i] = videoFRTable;
            allVideoScores[i] = videoScores;
            int currentVideoNumFrames = videoScores.length / Constants.MAX_SCORE_MODULES; //because there are MAX_SORE_MODULES values per frame
            totalAvailableFrames += currentVideoNumFrames;
            videoFrameCnt.add(currentVideoNumFrames);
        }

        totalKeepFrameIndexes = new boolean[totalAvailableFrames];
        numKeepFrames = 0;

        if (targetLength > totalAvailableFrames) //don't create highlights if more frames requested than available
            doAssembly = false;
        else if (item.summaryType == HighlightsModel.SummaryType.HIGHLIGHTS || (item.getEventDetectionEnabled() && timelapseOn)) {

            //we run the CVSummary algorithm if we're in highlights mode or if we're in timelapse mode and event detection is enabled

            Log.d("VideoSummary", "Calculating shots for total frames = " + totalAvailableFrames);

            // -- 2 -- Calculate shots based off of all scores of all videos
            CVSummaryOutput sumOutput = CVManagerJNILib.CVNativeSummary(allVideoScores, allVideoFRTables, summaryMode, targetLength, compressRatio, VideoCodecManager.FINAL_ENCODED_FRAME_RATE);

            FRDataParser parser = new FRDataParser(sumOutput.outputData);
            List<byte[]> faceIdList = parser.getFaceIDList();
            item.updateFaceIdsForStarring(this, faceIdList);
            List<Byte> asdCategoryList = parser.getASDCategoryList();
            if (asdCategoryList.size() > 0) {
                String catName = Utils.getASDCategoryName(asdCategoryList.get(0));
                Constants.THEME_TYPE theme = Utils.getThemeFromASDCategory(catName); //currently will only return 1 theme
                //TODO use this to set theme for the summarized video
                item.setTheme(theme.ordinal());
                QueryHelper.updateHighlightsTheme(this, item);
            }

            //TODO: delete this while loop, used only for debug purpose only
            int idx = 0;
            while (faceIdList != null && idx < faceIdList.size()) {
                String name = FRQueryHelper.queryEnrolledFaceName(this, faceIdList.get(idx));
                Log.i(TAG, "star name = " + name + " of size = " + faceIdList.size());
                idx++;
            }

            idx = 0;
            while (asdCategoryList != null && idx < asdCategoryList.size()) {
                Log.i(TAG, "ASD category = " + Utils.getASDCategoryName(asdCategoryList.get(idx)) + " of size = " + asdCategoryList.size());
                idx++;
            }

            int totalShotsFromSummary = 0;
            for (int output = 0; output < sumOutput.vidSumOutputs.size(); output++) {
                SummaryOutput sumOut = sumOutput.vidSumOutputs.get(output);
                Log.d("VideoSummary", "Output[" + output + "] # of shots = " + sumOut.getShots().size());
                totalShotsFromSummary += sumOut.getShots().size();
            }

            if (totalShotsFromSummary == 0) {
                doAssembly = false; //notify UI we can't assemble a video because no shots were calculated by summary
            } else {

                int frameOffsetFromLastVideo = 0;

                for (int output = 0; output < sumOutput.vidSumOutputs.size(); output++) {

                    SummaryOutput sumOut = sumOutput.vidSumOutputs.get(output);

                    for (int shot = 0; shot < sumOut.getShots().size(); shot++) {

                        Shot mShot = sumOut.getShots().get(shot);
                        Log.d("VideoSummary", "Output[" + output + "] Shot[" + shot + "] start: " + mShot.seg_start_frame + " end: " + mShot.seg_end_frame);

                        if (mShot.seg_start_frame < 0) {
                            Log.d("VideoSummary", "!!! Lib issue: returned start frame < 0 so forcing to zero");
                            mShot.seg_start_frame = 0;
                        }

                        if (mShot.seg_end_frame >= videoFrameCnt.get(output)) {
                            Log.d("VideoSummary", "!!! Lib issue: returned end frame >= frame count of this video so forcing it to last frame");
                            mShot.seg_end_frame = videoFrameCnt.get(output) - 1;
                        }

                        //turn on the keep frames within the range
                        int skipCount = 1;

                        for (int i = mShot.seg_start_frame; i < mShot.seg_end_frame; i++) {
                            if (!timelapseOn) {
                                //keep all frames if not in timelapse mode
                                totalKeepFrameIndexes[i + frameOffsetFromLastVideo] = true;
                            } else {
                                //keep 1 - every Nth frame if in timelapse mode
                                if (i == mShot.seg_start_frame || skipCount == timelapse_event_speed) {
                                    totalKeepFrameIndexes[i + frameOffsetFromLastVideo] = true;
                                    skipCount = 1;
                                } else
                                    skipCount++;
                            }
                        }
                    }

                    frameOffsetFromLastVideo += videoFrameCnt.get(output);
                }
            }
        }

        if (doAssembly) {
            //if timelapse, add extra frames outside of keep shots
            if (timelapseOn) {
                int skipCount = 0;
                for (int i = 0; i < totalKeepFrameIndexes.length; i++) {
                    if (skipCount == timelapse_playback_speed) {
                        skipCount = 0;
                        if (!totalKeepFrameIndexes[i]) {
                            totalKeepFrameIndexes[i] = true;
                        }
                    }
                    skipCount++;
                }
            }

            //count keep frames as separate step avoids overlapping shots where keep frames would be double incremented
            for (int i = 0; i < totalKeepFrameIndexes.length; i++) {
                if (totalKeepFrameIndexes[i])
                    numKeepFrames++;
            }
            Log.d("VideoSummary", "Keep frame total = " + numKeepFrames);
        }

        return doAssembly;
    }


}
