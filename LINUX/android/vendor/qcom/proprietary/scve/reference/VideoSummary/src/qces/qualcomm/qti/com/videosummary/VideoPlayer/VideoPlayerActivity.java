/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.VideoPlayer;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.SurfaceTexture;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.File;
import java.io.IOException;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces.DetectedFaceModel;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.FacesInbox.FacesListViewAdapter;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.VideoPicker.VideoPickerActivity;
import qces.qualcomm.qti.com.videosummary.highlights.CreatingStatus;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;
import qces.qualcomm.qti.com.videosummary.providers.FRQueryHelper;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices.VidProcManagerService;

public class VideoPlayerActivity extends AppCompatActivity implements MediaPlayer.OnPreparedListener, MediaPlayer.OnErrorListener, VideoControllerView.MediaPlayerControl {

    private static final String TAG = VideoPlayerActivity.class.getSimpleName();
    private static final long CONTROLLER_MILLIS_UNTIL_AUTOHIDE = 3000;
    TextureView videoTextureView;
    MediaPlayer player;
    VideoControllerView controller;
    private Thread timer;
    private Handler hideHandler;
    private boolean monitorProgress = true;
    private LinearLayout videoContainerLayout;

    private boolean softKeboardShown = false;
    private FrameLayout videoTextureViewHolder;

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        if (softKeboardShown)
            hideSoftKeyboard(this);

        //show the media controls and start the count to hide them
        showMediaControls();

        resizeVideoTextureView();

        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            hideAllExceptVideo();
            if (!fullScreen)
                controller.showFullScreenBtn(false);
            else
                controller.showFullScreenBtn(true);
        } else
            showAll();

    }

    private void showAll() {

        controller.showFullScreenBtn(true);

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE);

        toolbar.setVisibility(View.VISIBLE);
        contentInfoHolder.setVisibility(View.VISIBLE);

        videoContainerLayout.setBackgroundColor(Color.parseColor("#FFFFFF"));
    }

    private void hideAllExceptVideo() {
        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
                        | View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
                        | View.SYSTEM_UI_FLAG_IMMERSIVE);

        toolbar.setVisibility(View.GONE);
        contentInfoHolder.setVisibility(View.GONE);

        removeTitleFocus();

        videoContainerLayout.setBackgroundColor(Color.parseColor("#000000"));
    }

    public HighlightsModel highlightsItem;
    EditText summaryTitle;

    TextView mediaCompositionText;
    private Toolbar toolbar;

    private boolean fullScreen;
    private int videoOrientation;

    private static final int ORIENTATION_LANDSCAPE = 1;
    private static final int ORIENTATION_PORTRAIT = 2;
    private FrameLayout contentInfoHolder;

    public static final int MEDIA_CHANGED = 0x01;
    public static final int LENGTH_CHANGED = 0x02;
    public static final int THEME_CHANGED = 0x04;
    public static final int SPEED_CHANGED = 0x08;
    public int onSharedPrefChangedBitMask = 0;

    private void resizeVideoTextureView() {

        int videoWidth = player.getVideoWidth();
        int videoHeight = player.getVideoHeight();
        float playerAspect = (float) videoWidth / (float) videoHeight;

        videoOrientation = (playerAspect > 1) ? ORIENTATION_LANDSCAPE : ORIENTATION_PORTRAIT;

        DisplayMetrics displaymetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displaymetrics);
        int displayWidth = displaymetrics.widthPixels;
        int displayHeight = displaymetrics.heightPixels;

        //force a video height that is 16x9 aspect ratio based off of the display width
        float newHeight = displayWidth * 9F / 16F;

        if (videoOrientation == ORIENTATION_PORTRAIT && isFullScreen()) //override
            newHeight = displayHeight;

        ViewGroup.LayoutParams videoTextureLayoutParams = videoTextureView.getLayoutParams();
        videoTextureLayoutParams.height = (int) newHeight;

        if (videoOrientation == ORIENTATION_PORTRAIT) {
            float newWidth = newHeight * playerAspect;
            videoTextureLayoutParams.width = (int) newWidth;
        }

        videoTextureView.setLayoutParams(videoTextureLayoutParams);

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_player);

        hideHandler = new Handler(Looper.getMainLooper());

        toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        videoContainerLayout = (LinearLayout) findViewById(R.id.video_container);

        videoTextureViewHolder = (FrameLayout) findViewById(R.id.videoTextureViewHolder);

        videoTextureView = (TextureView) findViewById(R.id.videoTextureView);
        videoTextureView.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
            @Override
            public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
                Surface s = new Surface(surface);
                player.setSurface(s);
                player.prepareAsync();
            }

            @Override
            public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

            }

            @Override
            public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
                return false;
            }

            @Override
            public void onSurfaceTextureUpdated(SurfaceTexture surface) {

            }
        });

        videoTextureViewHolder.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                showMediaControls();
                return false;
            }
        });

        player = new MediaPlayer();

        player.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            }
        });

        controller = new VideoControllerView(this);
        controller.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                showMediaControls();
                return false;
            }
        });

        highlightsItem = getIntent().getParcelableExtra(Constants.EXTRA_HIGHLIGHTS_MODEL);
        Uri videoUri = Uri.fromFile(new File(highlightsItem.videoFilePath));

        try {
            player.setDataSource(this, videoUri);
            player.setOnPreparedListener(this);
            player.setOnErrorListener(this);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (IllegalStateException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }


        timer = new Thread() {
            public void run() {
                while (monitorProgress) {
                    // do stuff in a separate thread
                    uiCallback.sendEmptyMessage(0);
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        };
        timer.start();


        contentInfoHolder = (FrameLayout) findViewById(R.id.edit_video_info_content_frame);
        LayoutInflater inflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        //depending on highlights or timelapse inflate the corresponding layout
        View v = inflater.inflate(R.layout.highlights_edit_video_info_layout, null);
        contentInfoHolder.removeAllViews();
        contentInfoHolder.addView(v);

        getFragmentManager().beginTransaction().replace(R.id.content_frame_pref, new VideoEditSettingsFragment()).commit();

        summaryTitle = (EditText) v.findViewById(R.id.summary_title);
        summaryTitle.setText(highlightsItem.title);

        summaryTitle.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                summaryTitle.setFocusableInTouchMode(true);
                softKeboardShown = true;
                return false;
            }
        });

        final LinearLayout media_info_layout = (LinearLayout) v.findViewById(R.id.edit_media_layout);
        mediaCompositionText = (TextView) v.findViewById(R.id.media_composition_text);
        mediaCompositionText.setText(highlightsItem.compositonMediaList.size() + " videos");

        media_info_layout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i(TAG, "edit media clicked");
                Intent videoPickerActivityIntent = new Intent(getApplicationContext(), VideoPickerActivity.class);
                videoPickerActivityIntent.putParcelableArrayListExtra(Constants.EXTRA_MEDIAINFO_ARRAYLIST, highlightsItem.compositonMediaList);
                startActivityForResult(videoPickerActivityIntent, highlightsItem.summaryType.ordinal());
            }
        });

        if (highlightsItem.faceIdList != null && highlightsItem.faceIdList.size() > 0) {
            loadStarringFaces();
        } else {
            LinearLayout ll = (LinearLayout) findViewById(R.id.starring_layout);
            ll.setVisibility(View.GONE);
        }
    }

    public void hideSoftKeyboard(Activity activity) {
        softKeboardShown = false;
        InputMethodManager inputMethodManager = (InputMethodManager) activity.getSystemService(Activity.INPUT_METHOD_SERVICE);
        inputMethodManager.hideSoftInputFromWindow(activity.getCurrentFocus().getWindowToken(), 0);
    }

    private void removeTitleFocus() {

        summaryTitle.setFocusable(false);

    }

    private Runnable hideMediaControlThread = new Runnable() {

        public void run() {
            controller.setVisibility(View.GONE);
        }
    };


    public void startDelayToHideMediaControls() {
        controller.postDelayed(hideMediaControlThread, CONTROLLER_MILLIS_UNTIL_AUTOHIDE);
    }

    public void showMediaControls() {
        controller.setVisibility(View.VISIBLE);
        hideHandler.removeCallbacks(hideMediaControlThread);
        startDelayToHideMediaControls();
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_video_edit, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        int id = item.getItemId();
        if (id == R.id.action_done) {
            //update title if changed
            if (!summaryTitle.getText().toString().equals(highlightsItem.title)) {
                highlightsItem.title = summaryTitle.getText().toString();
                QueryHelper.updateHighlights(this, highlightsItem);
            }
            if (onSharedPrefChangedBitMask == THEME_CHANGED) {
                //only music changed
                highlightsItem.cardType = HighlightsModel.CardType.CREATING;
                //DONOT delete the thumbimg and video file
                QueryHelper.updateHighlights(this, highlightsItem);
                VidProcManagerService.startActionProcessMusic(this, highlightsItem);
            } else if (onSharedPrefChangedBitMask != 0) {
                //this could be LENGTH_CHANGED | MEDIA_CHANGED | SPEED_CHANGED
                highlightsItem.cardType = HighlightsModel.CardType.CREATING;
                QueryHelper.updateHighlights(this, highlightsItem);
                startVideoSummaryProcess();
            }
            //if media composition changed or speed/length changed, then cardtype will be CREATING, so run video summary process
            // if (highlightsItem.cardType == HighlightsModel.CardType.CREATING) {
            //  QueryHelper.updateHighlights(this, highlightsItem);
            // startVideoSummaryProcess();
            //  }
            finish();
            return true;
        } else if (id == android.R.id.home) {
            onBackPressed();
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode == RESULT_OK) {
            HighlightsModel.SummaryType summaryType = HighlightsModel.SummaryType.values()[requestCode];
            switch (summaryType) {
                case HIGHLIGHTS:
                case TIMELAPSE:
                    highlightsItem.compositonMediaList = data.getParcelableArrayListExtra(Constants.EXTRA_MEDIAINFO_ARRAYLIST);
                    mediaCompositionText.setText(highlightsItem.compositonMediaList.size() + " videos");
                    onSharedPrefChangedBitMask |= MEDIA_CHANGED;
                    highlightsItem.status = new CreatingStatus();//TODO:make status as parcalable and package it
                    break;
                default:
                    throw new UnsupportedOperationException("Mode " + summaryType.toString() + " not supported");
            }
        }
    }

    public void startVideoSummaryProcess() {
        deleteCurrentSummaryVideoFromFileSystem();//delete the existing summary video and thumbnail
        VidProcManagerService.startActionProcess(this, highlightsItem);
    }

    void deleteCurrentSummaryVideoFromFileSystem() {
        String mp4FilePath = highlightsItem.videoFilePath;
        String thumbImgPath = highlightsItem.thumbImgFilePath;
        Utils.deleteFile(mp4FilePath);
        Utils.deleteFile(thumbImgPath);
    }

    @Override
    public void onPause() {
        super.onPause();
        stopPlaying();
    }

    @Override
    public void onStop() {
        super.onStop();
        stopPlaying();
    }

    void stopPlaying() {
       if ( player != null ) {
          player.stop();
          player.reset();
          player.release();
          player = null;
       }
       controller.onStop();
    }

    @Override
    protected void onDestroy() {

        super.onDestroy();
        stopPlaying();
        monitorProgress = false;
    }


    private Handler uiCallback = new Handler() {
        public void handleMessage(Message msg) {
            // do stuff with UI
            controller.setProgress();
        }
    };


    @Override
    public void onPrepared(MediaPlayer mp) {
        controller.setMediaPlayer(this);
        controller.setAnchorView((ViewGroup) findViewById(R.id.videoTextureViewHolder));
        resizeVideoTextureView();
        startMediaPlayer();
        startDelayToHideMediaControls();
    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
       Log.i(TAG, "MediaPlayer ERROR!! what=" + what + "extra=" + extra);
       stopPlaying();
       return false;
    }

    private void startMediaPlayer() {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        player.start();
    }

    @Override
    public int getCurrentPosition() {
        return player.getCurrentPosition();
    }

    @Override
    public int getDuration() {
        return player.getDuration();
    }

    @Override
    public boolean isPlaying() {
        return player.isPlaying();
    }

    @Override
    public void pause() {
        player.pause();
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public void seekTo(int i) {
        player.seekTo(i);
    }

    @Override
    public void start() {
        startMediaPlayer();
    }

    @Override
    public boolean isFullScreen() {
        return fullScreen;
    }

    @Override
    public void toggleFullScreen() {

        fullScreen = !fullScreen;

        if (isFullScreen()) {

            if (videoOrientation == ORIENTATION_LANDSCAPE)
                forceOrientationLandscape();

            resizeVideoTextureView();

            hideAllExceptVideo();

        } else {

            //go back to default (portrait)
            this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

            resizeVideoTextureView();
            showAll();

            //re-enable the sensor
            this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR);


        }

    }

    private void forceOrientationLandscape() {
        this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }

    public static LinearLayoutManager llm;
    public static FacesListViewAdapter mAdapter;
    RecyclerView mRecyclerView;

    void loadStarringFaces() {
        mRecyclerView = (RecyclerView) findViewById(R.id.recycler_view);
        mRecyclerView.setHasFixedSize(true);

        llm = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(llm);
        mRecyclerView.setItemAnimator(new DefaultItemAnimator());
        mAdapter = new FacesListViewAdapter(VideoPlayerActivity.this);
        mRecyclerView.setAdapter(mAdapter);
        ViewGroup.LayoutParams params = mRecyclerView.getLayoutParams();
        params.height = 400 * highlightsItem.faceIdList.size();//based on trial and error on LG G4, 400dp is the height of each face item in the recycler view.
        mRecyclerView.requestLayout();

        new Thread(new Runnable() {
            @Override
            public void run() {
                for (int i = 0; i < highlightsItem.faceIdList.size(); i++) {

                    String frDatasetId = new String(highlightsItem.faceIdList.get(i));
                    String name = FRQueryHelper.queryEnrolledFaceName(VideoPlayerActivity.this, highlightsItem.faceIdList.get(i));
                    Bitmap photo = FRQueryHelper.queryEnrolledFaceBitmap(VideoPlayerActivity.this, highlightsItem.faceIdList.get(i));

                    DetectedFaceModel faceItem = new DetectedFaceModel(frDatasetId.getBytes(), name, photo, DetectedFaceModel.FaceType.KNOWN);
                    mAdapter.addFaceItem(faceItem);
                }
            }
        }).start();

    }


}
