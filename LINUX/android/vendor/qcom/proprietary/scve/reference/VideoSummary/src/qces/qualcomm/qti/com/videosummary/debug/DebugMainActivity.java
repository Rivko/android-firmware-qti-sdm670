/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.debug;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.GridView;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;
import java.util.Set;

import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.TitleGenerator;
import qces.qualcomm.qti.com.videosummary.PolicyManager.FrequencyScheduleAlarmReceiver;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.CalendarHelper;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.VideoPicker.VideoPickerActivity;
import qces.qualcomm.qti.com.videosummary.highlights.FrameDebugDataAdapter;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;
import qces.qualcomm.qti.com.videosummary.providers.VideoSummaryContract;
import qces.qualcomm.qti.com.videosummary.settings.AboutActivity;
import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoDebug.FrameDebugData;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoDebug.VideoDecoderToDownscaledRGB;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices.VidProcManagerService;
import qces.qualcomm.qti.com.videosummary.videoprocessing.fastcv.FastCVJNILib;

public class DebugMainActivity extends AppCompatActivity {
    private static final String TAG = DebugMainActivity.class.getSimpleName();
    private static TextView logText;
    ScrollView scrollView;
    ArrayList<FrameDebugData> frameDataList;
    FrameDebugDataAdapter frameDataAdapter;
    private GridView gridview;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_debug_main);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        logText = (TextView) findViewById(R.id.logTextView);
        scrollView = (ScrollView) findViewById(R.id.scrollView);
        gridview = (GridView) findViewById(R.id.gridview);
        frameDataList = new ArrayList<FrameDebugData>();
        frameDataAdapter = new FrameDebugDataAdapter(this, frameDataList);

        gridview.setAdapter(frameDataAdapter);


    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_highlights, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        switch (item.getItemId()) {
            case R.id.action_create_highlights:
                //TODO: do stuff for create highlights video now
                break;
            case R.id.action_create_timelapse:
                //TODO: do stuff for create timelapse video now
                break;
            case R.id.action_settings:
                Intent settingsActivityIntent = new Intent(this, SettingsActivity.class);
                startActivityForResult(settingsActivityIntent, 0);
                break;
            case R.id.action_about:
                Intent aboutActivityIntent = new Intent(this, AboutActivity.class);
                startActivityForResult(aboutActivityIntent, 0);
                break;
            case R.id.action_debug:
                //Intent intent = new Intent(this, DebugMainActivity.class);
                //startActivityForResult(intent, 0);
                break;
            default:
                return super.onOptionsItemSelected(item);
        }
        return true;
    }

    @Override
    protected void onStart() {
        super.onStart();
        String fastCV_version = FastCVJNILib.getFastCVVersion();
        updateLog("Fast CV version = " + fastCV_version);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (SettingsActivity.getPrefExpandScrollView(this)) {
            LinearLayout.LayoutParams scrollParams = new LinearLayout.LayoutParams(ViewGroup
                    .LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT, 1.0f);
            scrollView.setLayoutParams(scrollParams);
        }
        LocalBroadcastManager.getInstance(this).registerReceiver(mMessageReceiver,
                new IntentFilter(Utils.ACTION_DIAG_MSG_RECEIVED));
    }

    @Override
    protected void onPause() {
        super.onPause();
        // Unregister since the activity is about to be closed.
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mMessageReceiver);
    }

    private void updateLog(String log) {

        StringBuilder currentText = new StringBuilder(logText.getText());
        currentText.append("\n" + log);
        logText.setText(currentText.toString());

        scrollView.post(new Runnable() {
            public void run() {
                scrollView.fullScroll(View.FOCUS_DOWN);
            }
        });
    }

    private void addDebugFrameInfo(Bitmap bmp, int[] scores, int frameCount) {

        FrameDebugData frameData = new FrameDebugData(bmp, scores, frameCount);
        frameDataList.add(frameData);

        frameDataAdapter.notifyDataSetChanged();
    }

    private void clearDebugFrameInfo() {
        frameDataList.clear();
        frameDataAdapter.notifyDataSetChanged();
        logText.setText("");
    }

    public void startDecodingVideo(View view) {
        Log.i(TAG, "starting video decode");
        selectVideoFromGalleryForScoring();
    }

    public void summarizeVideo(View view) {
        selectVideoFromGalleryForSummary();
    }

    public void startDecodingVideoFrames(View view) {
        selectVideoFromGalleryForFrameDebug();
    }

    public void setAlarm(View view) {
        updateLog("scheduling fake alarm");
        FrequencyScheduleAlarmReceiver.setAlarm(this, Calendar.getInstance().getTimeInMillis() + 5000);
    }


    String uriString, title, scoresString, starringIds;
    int numFrames, cardType, summaryType, speedOrLength;
    StringBuffer builder;
    boolean tableEmpty;
    long timeStamp;

    public void queryScoreKeeper(View view) {
        builder = new StringBuffer("URI STRING | NUM_FRAMES | SCORES | FaceIDs");
        logText.setText("");
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                Cursor cursor = QueryHelper.queryScoreKeeper(getApplicationContext());
                tableEmpty = true;
                if (cursor != null) {
                    int count = 1;
                    while (cursor.moveToNext()) {
                        tableEmpty = false;
                        uriString = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Scores._URI));
                        numFrames = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Scores._NUM_FRAMES));
                        scoresString = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Scores._SCORES));
                        starringIds = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Scores._STARRING_IDS));
                        builder.append("\n\nRow:" + count + " | " + uriString + " has " + numFrames + " frames, with scores => " + scoresString + " Starring Ids =" + starringIds + " Row:" + count + " END");
                        //QueryHelper.getFaceIdsAsArrayListOfByteArrayFromString(starringIds);
                        count++;
                    }
                }
                if (tableEmpty) {
                    builder.append("\n SCORE KEEPER TABLE IS EMPTY");
                }

                DebugMainActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateLog(builder.toString());
                    }
                });
            }
        });
        t.start();

    }

    public static void queryAndSaveScores(Context context, Uri uri) {
        int[] scores = QueryHelper.getScores(context, uri);
        String filepath = Utils.getAbsolutePathFromURI(context, uri);
        String str[] = filepath.split("/");
        String fileName = str[str.length - 1];
        String fileNameSplit[] = fileName.split("\\.");
        String resultTextFileName = fileNameSplit[0] + ".txt";
        try {
            Uri savedFileUri = writeToFile(resultTextFileName, scores);
            context.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, savedFileUri));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static Uri writeToFile(String outputFileName, int[] scores) throws IOException {

        OutputStream out = null;
        byte[] byteBuffer = scoresToPrintableFormat(scores);

        File outDir = new File(Utils.VIDEO_SUMMARY_DIRECTORY);
        outDir.mkdirs();
        Log.i(TAG, "saving scores to :" + outDir + "/" + outputFileName);
        File outFile = new File(outDir, outputFileName);
        try {
            out = new FileOutputStream(outFile);
            out.write(byteBuffer);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } finally {
            out.close();
            return Uri.fromFile(outFile);
        }
    }

    public static byte[] scoresToPrintableFormat(int[] scores) {
        StringBuilder scoreString = new StringBuilder();
        for (int i = 0; i < scores.length; i++) {
            scoreString.append(Integer.toString(scores[i]));
            if ((i + 1) % Constants.MAX_SCORE_MODULES == 0) {
                scoreString.append("\n");
            } else {
                scoreString.append(" ");
            }
        }
        return scoreString.toString().getBytes();
    }

    public void queryHighlights(View view) {
        builder = new StringBuffer("Card Type| SummaryType | URI STRING | Created Date | title | speedOrLength");
        logText.setText("");
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                Cursor cursor = QueryHelper.queryHighlights(getApplicationContext());
                tableEmpty = true;
                if (cursor != null) {
                    while (cursor.moveToNext()) {
                        tableEmpty = false;
                        cardType = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._CARD_TYPE));
                        summaryType = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._SUMMARY_TYPE));
                        uriString = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._URI));
                        timeStamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._CREATED_DATE));
                        title = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._TITLE));
                        speedOrLength = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._SPEED_LENGTH));
                        builder.append("\n" + HighlightsModel.CardType.values()[cardType].toString() +
                                " | " + HighlightsModel.SummaryType.values()[summaryType].toString() +
                                " | " + uriString +
                                " | " + CalendarHelper.timeStampMillisToString(timeStamp) +
                                " | " + title +
                                " | 0x" + Integer.toHexString(speedOrLength));
                    }
                }
                if (tableEmpty) {
                    builder.append("\n Highlgihts TABLE IS EMPTY");
                }

                DebugMainActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateLog(builder.toString());
                    }
                });
            }
        });
        t.start();

    }

    public void queryAssemblyTimeline(View view) {
        Log.i(TAG, "show location");
        //selectVideoFromGalleryForLocation();
        queryAssemblyTimeline();
    }


    private void queryAssemblyTimeline() {
        builder = new StringBuffer("Start | End | Summary Start time| Attempts");


        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                Cursor cursor = QueryHelper.queryAssemblyTimeline(getApplicationContext(), VideoSummaryContract.AssemblyTimeline.TYPE_SCORING_ONLY);
                tableEmpty = true;
                if (cursor != null) {
                    while (cursor.moveToNext()) {
                        tableEmpty = false;
                        long startTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._START_TIMESTAMP));
                        long endTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._END_TIMESTAMP));
                        long summaryTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._SUMMARY_TIMESTAMP));
                        int attempts = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._ATTEMPTS));

                        builder.append("\n Scoring:" + CalendarHelper.timeStampMillisToString(startTimestamp) + " | " +
                                CalendarHelper.timeStampMillisToString(endTimestamp) + " | " +
                                CalendarHelper.timeStampMillisToString(summaryTimestamp) + " | " +
                                attempts);
                    }
                }
                cursor = QueryHelper.queryAssemblyTimeline(getApplicationContext(), VideoSummaryContract.AssemblyTimeline.TYPE_HIGHLIGHTS_ASSEMBLY);
                if (cursor != null) {
                    while (cursor.moveToNext()) {
                        tableEmpty = false;
                        long startTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._START_TIMESTAMP));
                        long endTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._END_TIMESTAMP));
                        long summaryTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._SUMMARY_TIMESTAMP));
                        int attempts = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._ATTEMPTS));

                        builder.append("\n Assebmly:" + CalendarHelper.timeStampMillisToString(startTimestamp) + " | " +
                                CalendarHelper.timeStampMillisToString(endTimestamp) + " | " +
                                CalendarHelper.timeStampMillisToString(summaryTimestamp) + " | " +
                                attempts);
                    }
                }
                cursor = QueryHelper.queryAssemblyTimeline(getApplicationContext(), VideoSummaryContract.AssemblyTimeline.TYPE_TIMELAPSE);
                if (cursor != null) {
                    while (cursor.moveToNext()) {
                        tableEmpty = false;
                        long startTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._START_TIMESTAMP));
                        long endTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._END_TIMESTAMP));
                        long summaryTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._SUMMARY_TIMESTAMP));
                        int attempts = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.AssemblyTimeline._ATTEMPTS));

                        builder.append("\n Timelapse:" + CalendarHelper.timeStampMillisToString(startTimestamp) + " | " +
                                CalendarHelper.timeStampMillisToString(endTimestamp) + " | " +
                                CalendarHelper.timeStampMillisToString(summaryTimestamp) + " | " +
                                attempts);
                    }
                }
                if (tableEmpty) {
                    builder.append("\n AssemblyTimeline TABLE IS EMPTY");
                }

                DebugMainActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateLog(builder.toString());
                    }
                });
            }
        });
        t.start();
    }

    private static final int SELECT_VIDEO_FOR_SCORE = 1;
    private static final int SELECT_VIDEO_FOR_SUMMARY = 2;
    private static final int SELECT_VIDEO_FOR_FRAME_DEBUG = 3;
    private static final int SELECT_VIDEO_FOR_LOCATION = 4;


    private void selectVideoFromGalleryForScoring() {
        Intent videoPickerActivityIntent = new Intent(this, VideoPickerActivity.class);
        videoPickerActivityIntent.putExtra(VideoPickerActivity.MULTI_SELECT, false);
        startActivityForResult(videoPickerActivityIntent, SELECT_VIDEO_FOR_SCORE);
    }

    private void selectVideoFromGalleryForSummary() {
        Intent videoPickerActivityIntent = new Intent(this, VideoPickerActivity.class);
        videoPickerActivityIntent.putExtra(VideoPickerActivity.MULTI_SELECT, false);
        startActivityForResult(videoPickerActivityIntent, SELECT_VIDEO_FOR_SUMMARY);
    }

    private void selectVideoFromGalleryForFrameDebug() {
        Intent videoPickerActivityIntent = new Intent(this, VideoPickerActivity.class);
        videoPickerActivityIntent.putExtra(VideoPickerActivity.MULTI_SELECT, false);
        startActivityForResult(videoPickerActivityIntent, SELECT_VIDEO_FOR_FRAME_DEBUG);
    }

    private void selectVideoFromGalleryForLocation() {
        Intent videoPickerActivityIntent = new Intent(this, VideoPickerActivity.class);
        startActivityForResult(videoPickerActivityIntent, SELECT_VIDEO_FOR_LOCATION);
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode == Activity.RESULT_OK) {
            Uri returnedUri = null;

            if (data == null) {
                return;
            }


            clearDebugFrameInfo();

            switch (requestCode) {
                case SELECT_VIDEO_FOR_SUMMARY:
                    /* disabling this debug feature  for now */

                    /*

                    returnedUri = ((MediaInfo) data.getParcelableExtra(Constants.EXTRA_MEDIAINFO)).contentUri;
                    int scoreLen = QueryHelper.getNumFrames(this, returnedUri);

                    if (scoreLen == 0) {
                        updateLog("NO SCORE DATA FOUND in the Scores DB for the selected uri: " + returnedUri.toString());
                        return;
                    }

                    updateLog("scores byte array found for uri: " + returnedUri.toString());
                    updateLog("calling CVNativeSummary JNI on the score DB uri");

                    int fps = 30;
                    int desiredLength = SettingsActivity.getPrefDefaultLength(this);
                    int summary_length = desiredLength * fps;

                    updateLog("user desired length: " + desiredLength);
                    updateLog("summary length (frames) = frames/sec * desired length(sec) = " + fps + " * " + desiredLength + " = " + summary_length);

                    if (summary_length > scoreLen) {
                        updateLog("CHECK 1 FAIL: this video has " + scoreLen + " frames which isn't enough for summary length " + summary_length);
                        break;
                    }

                    int[][] allVideoScores = new int[1][];
                    int[] videoScores = QueryHelper.getScores(this, returnedUri);
                    allVideoScores[0] = videoScores;


                    CVSummaryOutput sumOutput = CVManagerJNILib.CVNativeSummary(allVideoScores, 7, 0, (byte) 0, fps);

                    if (sumOutput.vidSumOutputs.size() == 0) {
                        updateLog("CHECK 2 FAIL: the calculated summary has no output shots");
                        break;
                    }

                    for (int output_num = 0; output_num < sumOutput.vidSumOutputs.size(); output_num++) {

                        SummaryOutput sumOut = sumOutput.vidSumOutputs.get(output_num);
                        updateLog("Listing shots for output_num: " + output_num);

                        for (int shot = 0; shot < sumOut.getShots().size(); shot++) {
                            Shot mShot = sumOut.getShots().get(shot);
                            updateLog("Data for shot: " + shot);
                            updateLog("start frame: " + mShot.seg_start_frame);
                            updateLog("end frame: " + mShot.seg_end_frame);
                            updateLog("score: " + mShot.seg_score);
                        }
                    }

                    */

                    break;

                case SELECT_VIDEO_FOR_SCORE:
                    returnedUri = ((MediaInfo) data.getParcelableExtra(Constants.EXTRA_MEDIAINFO)).contentUri;
                    updateLog("STARTED Scoring Service for uri: " + returnedUri.toString());
                    ArrayList<Uri> uris = new ArrayList<Uri>();
                    uris.add(returnedUri);
                    VidProcManagerService.startActionScoreRecord(this, uris, 0);

                    break;

                case SELECT_VIDEO_FOR_FRAME_DEBUG:
                    returnedUri = ((MediaInfo) data.getParcelableExtra(Constants.EXTRA_MEDIAINFO)).contentUri;
                    updateLog("STARTED Frame Debug Service for uri: " + returnedUri.toString());
                    Thread frameDebug = new Thread(new VideoDecoderToDownscaledRGB(this, returnedUri));
                    frameDebug.start();

                    break;
                case SELECT_VIDEO_FOR_LOCATION:
                    ArrayList<MediaInfo> mediaInfoArrayList = data.getParcelableArrayListExtra(Constants.EXTRA_MEDIAINFO_ARRAYLIST);
                    if (mediaInfoArrayList == null || mediaInfoArrayList.size() == 0) {
                        String loc = Utils.getAllMediaStoreVideosWithLocInfo(this);
                        updateLog(loc);
                    } else {
                        TitleGenerator title = new TitleGenerator(this, mediaInfoArrayList, HighlightsModel.SummaryType.HIGHLIGHTS);
                        updateLog(title.getTitle());
                    }
                    break;
            }

        }
    }

    // Our handler for received Intents. This will be called whenever an Intent
    // with an action named "video-summary-local-events" is broadcasted.
    private BroadcastReceiver mMessageReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            // Get extra data included in the Intent

            String message = intent.getStringExtra(Utils.DIAG_EXTRA_MESSAGE);
            String toastMessage = intent.getStringExtra(Utils.TOAST_EXTRA_MESSAGE);
            String TAG = intent.getStringExtra(Utils.DIAG_EXTRA_TAG);
            if (toastMessage != null && toastMessage.length() > 0) {
                Toast.makeText(context, TAG + ":" + toastMessage, Toast.LENGTH_SHORT).show();
            } else if (message != null) {
                updateLog(TAG + ": " + message);
            } else {
                //check if frame debug info available

                int[] intArray = intent.getIntArrayExtra(Utils.DIAG_EXTRA_DBG_FRAME_DATA);

                if (intArray != null) {
                    //Bitmap bmp = BitmapFactory.decodeByteArray(byteArray,0,byteArray.length);

                    int width = intent.getIntExtra(Utils.DIAG_EXTRA_DBG_FRAME_WIDTH, 0);
                    int height = intent.getIntExtra(Utils.DIAG_EXTRA_DBG_FRAME_HEIGHT, 0);

                    Bitmap bmp = Bitmap.createBitmap(intArray, width, height, Bitmap.Config.ARGB_8888);
                    int[] scores = intent.getIntArrayExtra(Utils.DIAG_EXTRA_DBG_FRAME_SCORE);
                    int frameNum = intent.getIntExtra(Utils.DIAG_EXTRA_DBG_FRAME_NUM, 0);
                    addDebugFrameInfo(bmp, scores, frameNum);
                }

            }

        }
    };

   /* public void addSampleCards(View view) {
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                addCards();
            }
        });
        t.start();
    }

    static int count = 1;

    void addCards() {
        HighlightsModel item;
        for (int i = 1; i <= 10; i++) {
            try {
                Thread.sleep(2000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            long timeStampMillis = Calendar.getInstance().getTimeInMillis();
            String mp4FilePath = Utils.getMp4FilePath(this, timeStampMillis).toString();
            String thumbImgPath = Utils.getThumbImgFilePath(this, timeStampMillis).toString();
            item = new HighlightsModel(HighlightsModel.CardType.SUMMARY, HighlightsModel.SummaryType.HIGHLIGHTS, mp4FilePath, thumbImgPath, timeStampMillis, "Sample Card" + count++, null);
            QueryHelper.insertIntoHighlights(this, item);
        }
    }*/

    public void clearScoreKeeperTable(View view) {
        QueryHelper.deleteAllFromScoreKeeper(this);
        logText.setText("");
        updateLog("ScoreKeeper Table Cleared!!!");
    }

    public void status(View view) {
        Set<String> set = Utils.getPrefCvInitTime(this);
        Iterator<String> i = set.iterator();
        logText.setText("");
        updateLog("CVInit time:");
        while (i.hasNext()) {
            String s = i.next();
            updateLog(s);
        }
        updateLog("Last updated till :" + CalendarHelper.timeStampMillisToString(SettingsActivity.getAssemblyTimelineTableUpdateTimestamp(this)));
    }

    void sleep(long time) {
        try {
            Thread.sleep(time);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

}
