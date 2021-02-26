/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.MeshViewer;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.v4.app.NavUtils;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.Editable;
import android.text.InputFilter;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Choreographer;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.qualcomm.qti.threedr.GL2JNILib;
import com.qualcomm.qti.threedr.MeshViewer.utils.AboutBox;
import com.qualcomm.qti.threedr.R;
import com.qualcomm.qti.threedr.modelbrowser.BrowserUtils;
import com.qualcomm.qti.threedr.modelbrowser.EmailComposer;
import com.qualcomm.qti.threedr.modelbrowser.GalleryViewActivity;
import com.qualcomm.qti.threedr.modelbrowser.ModelInfo;

//import org.apache.commons.io.FileUtils;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;


public class MeshViewerActivity extends AppCompatActivity implements SurfaceHolder.Callback, Choreographer.FrameCallback {

    private static final int MESHMODE_TEXTURED = 0;
    private static final int MESHMODE_NO_TEXTURE = 1;
    private static int CURRENT_MESHMODE = 0;

    static String TAG = "Scan3D";

    int mCount = 1;

    Toolbar mToolbar;
    ImageButton imageSolidMesh;
    ImageButton imageTextureMesh;
    ToggleButton lockRotationToggleBtn;
    LinearLayout mTopButtonLayout;

    private int[] m_pointerArray;

    private ModelInfo mModelInfo;

    boolean isLaunchFromGallery = false;
    int modelPosInGallery = -1;

    private RenderThread mRenderThread;

    private EglCore mEglCore;

    public static final int PROGBAR_MODE_INDETERMINATE = -1;
    public static final int PROGBAR_MODE_DETERMINATE = -2;
    public static final int PROGBAR_MODE_DISMISS = -3;

    private boolean discardConfirmed = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (savedInstanceState == null) {
            Intent intent = getIntent();
            handleIntent(intent);
        }

        setContentView(R.layout.model_viewer_activity);
        mToolbar = (Toolbar) findViewById(R.id.toolbar);

        lockRotationToggleBtn = (ToggleButton) findViewById(R.id.toggleButton);
        lockRotationToggleBtn.setText(R.string.action_toggle_rotation_lock);
        lockRotationToggleBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                GLViewerJNILib.lockRotationAngle();
                if (lockRotationToggleBtn.isChecked())
                    lockRotationToggleBtn.setText(R.string.action_toggle_rotation_unlock);
                else
                    lockRotationToggleBtn.setText(R.string.action_toggle_rotation_lock);
            }
        });

        imageSolidMesh = (ImageButton) findViewById(R.id.imageSolidMesh);
        imageSolidMesh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                imageSolidMesh.setSelected(true);
                if (CURRENT_MESHMODE == MESHMODE_TEXTURED) {
                    unselectTextureMeshSelectorButton();
                    GLViewerJNILib.setMeshMode(MESHMODE_NO_TEXTURE);
                    CURRENT_MESHMODE = MESHMODE_NO_TEXTURE;
                }

            }
        });
        imageTextureMesh = (ImageButton) findViewById(R.id.imageTextureMesh);
        imageTextureMesh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                imageTextureMesh.setSelected(true);
                if (CURRENT_MESHMODE == MESHMODE_NO_TEXTURE) {
                    unselectSolidMeshSelectorButton();
                    GLViewerJNILib.setMeshMode(MESHMODE_TEXTURED);
                    CURRENT_MESHMODE = MESHMODE_TEXTURED;
                }
            }
        });

        setSupportActionBar(mToolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        setTitle(mModelInfo.getName());

        mTopButtonLayout = (LinearLayout) findViewById(R.id.top_button_layout);
        if (isLaunchFromGallery) {
            mTopButtonLayout.setVisibility(View.GONE);
        } else {
            mToolbar.setVisibility(View.GONE);
            mTopButtonLayout.setVisibility(View.VISIBLE);

            Button saveButton = (Button) findViewById(R.id.save_button);
            Button discardButton = (Button) findViewById(R.id.discard_button);

            saveButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Log.i(TAG, "saveButton clicked");
                    showNameDialog();
                }
            });

            discardButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Log.i(TAG, "discardButton clicked");
                    showDiscardConfirmationDialog();
                }
            });
        }

        //initially we show the Texture model
        CURRENT_MESHMODE = MESHMODE_TEXTURED;
        ((ImageButton) findViewById(R.id.imageTextureMesh)).setSelected(true);

        SurfaceView sv = (SurfaceView) findViewById(R.id.modelViewerSurfaceView);
        sv.getHolder().addCallback(this);

        InitGestures();
    }

    void unselectTextureMeshSelectorButton() {
        ImageButton imageTextureMeshButton = (ImageButton) findViewById(R.id.imageTextureMesh);
        imageTextureMeshButton.setSelected(false);
    }

    void unselectSolidMeshSelectorButton() {
        ImageButton imageSolidMeshButton = (ImageButton) findViewById(R.id.imageSolidMesh);
        imageSolidMeshButton.setSelected(false);
    }

    void handleIntent(Intent intent) {
        String action = intent.getAction();
        if (action != null && action.equals("android.intent.action.VIEW")) {
            Uri uri = intent.getData();
            if (uri != null) {
                mModelInfo = ModelInfo.extractModelInfo(uri, this);
                if (mModelInfo == null) {
                    Toast.makeText(getApplicationContext(), "please download the file and open it from downloads or file explorer", Toast.LENGTH_LONG).show();
                    onBackPressed();
                    return;
                }
            }
        } else {
            mModelInfo = intent.getParcelableExtra(BrowserUtils.EXTRA_MODELINFO);
        }
        modelPosInGallery = intent.getIntExtra(BrowserUtils.EXTRA_MODELINFO_POS, -1);
        isLaunchFromGallery = intent.getBooleanExtra(BrowserUtils.EXTRA_LAUNCH_FROM_GALLERY, false);


    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "onPause unhooking choreographer");
        Choreographer.getInstance().removeFrameCallback(this);
    }

    @Override
    protected void onResume() {
        super.onResume();

        for (int i = 0; i < 10; i++) {
            m_pointerArray[i] = -1;
        }

        if (mRenderThread != null) {
            Log.d(TAG, "onResume re-hooking choreographer");
            Choreographer.getInstance().postFrameCallback(this);
        }
    }

    ProgressDialog progressDialog;

    private void createProgDialog() {
        if (progressDialog == null) {
            progressDialog = new ProgressDialog(this);
            progressDialog.setCancelable(false);
        }
    }

    public void setProgBar(int progMode) {

        switch (progMode) {
            case PROGBAR_MODE_DISMISS:
                if (progressDialog != null) {
                    progressDialog.dismiss();
                    progressDialog = null;
                }
                break;
            case PROGBAR_MODE_INDETERMINATE:
                createProgDialog();
                progressDialog.setIndeterminate(true);
                progressDialog.setTitle(R.string.viewer_loading_title);
                progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
                progressDialog.setMessage(getString(R.string.viewer_loading_message));
                progressDialog.show();
                break;
            case PROGBAR_MODE_DETERMINATE:
                createProgDialog();
                progressDialog.setIndeterminate(false);
                progressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
                progressDialog.setTitle(R.string.creating_mp4_title);
                progressDialog.setMax(MeshViewerUtils.TOTAL_FRAMES);
                progressDialog.show();
                break;
            default:
                progressDialog.setProgress(progMode);
                break;

        }

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.model_viewer_menu, menu);
        return true;
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        boolean isEnabled = (mCount > 0) ? true : false;

        setMenuItemEnabled(menu, R.id.action_share, isEnabled);
        setMenuItemEnabled(menu, R.id.action_rename, isEnabled);
        setMenuItemEnabled(menu, R.id.action_delete, isEnabled);

        return super.onPrepareOptionsMenu(menu);
    }

    // Set menu item enabled state
    void setMenuItemEnabled(Menu menu, int itemId, boolean enabled) {
        MenuItem menuItem = menu.findItem(itemId);
        if (menuItem != null) {
            menuItem.setEnabled(enabled);
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        boolean found = false;
        int id = item.getItemId();

        List<File> folderList;

        switch (id) {
            case android.R.id.home:
                onBackPressed();
                found = true;
                break;
            case R.id.action_share:
                break;
            case R.id.action_video_share:
                Log.i(TAG, "action video share clicked");

                RenderHandler rh = mRenderThread.getHandler();
                if (rh != null) {
                    rh.sendCreateMP4();
                }

                found = true;
                break;
            case R.id.action_raw_share:
                Log.i(TAG, "action raw share clicked");
                folderList = new ArrayList<>();
                folderList.add(mModelInfo.getFile());
                BrowserUtils.shareRawObjDataFiles(this, folderList);
                found = true;
                break;

            case R.id.action_rename:
                showNameDialog();
                found = true;
                break;
            case R.id.action_delete:
                actionDeleteCurrentModel();
                setResult(RESULT_OK, GalleryViewActivity.OPERATION_MODEL_DELETED);
                found = true;
                break;
            default:
                found = super.onOptionsItemSelected(item);
        }

        return found;
    }

    void actionDeleteCurrentModel() {
        List<File> folderList = new ArrayList<File>();
        folderList.add(mModelInfo.getFile());
        BrowserUtils.delete(this, folderList);
    }

    void saveCurrentModel(String newName) {

        if (!newName.equals(mModelInfo.getName())) {
            mModelInfo.setName(newName);
            setTitle(newName);
        }

        //appending "_done" to model dir name
        if (!mModelInfo.getFile().getName().endsWith(BrowserUtils.FolderSuffix)) {
            String fileParent = mModelInfo.getFile().getParent();
            String fileName = mModelInfo.getFile().getName() + BrowserUtils.FolderSuffix;
            mModelInfo.getFile().renameTo(new File(fileParent + "/" + fileName));
            mModelInfo = new ModelInfo(fileParent, fileName);
        }

        //if model is in temp/ dir then move it to sdcard/3dr/<model_name>_done
        File modelFile = mModelInfo.getFile();
        if (!modelFile.getParent().endsWith("/3DR")) {
            //check if this model exist in our parentfolder
            String newModelFolderPath = BrowserUtils.ParentFolderPath + "/" + mModelInfo.getFile().getName();
            String modelFolderName = mModelInfo.getFile().getName();
            File newModelFolder = new File(newModelFolderPath);
            //TODO: handle if this model or a model with same name already exist in the parent folder
            int i = 0;
            while (newModelFolder.exists()) {
                modelFolderName = mModelInfo.getModelID() + "_" + ++i + BrowserUtils.FolderSuffix;
                newModelFolderPath = BrowserUtils.ParentFolderPath + "/" + modelFolderName;
                newModelFolder = new File(newModelFolderPath);
            }
            //// first do a rename anfd then create a model out of it and then call move with new pointers
            //if model with same name exist then rename it with appending _1 or _2 or _3 ...
            if (i > 0) {
                BrowserUtils.rename(mModelInfo.getFile(), modelFolderName.substring(0, modelFolderName.lastIndexOf(BrowserUtils.FolderSuffix)));
                mModelInfo = new ModelInfo(mModelInfo.getFile().getParent(), newModelFolder.getName());
            }
            if ( !mModelInfo.getFile().renameTo(newModelFolder) ){
               Log.e(TAG, "Could not move model from "+modelFolderName+" to "+newModelFolderPath);
            }
            //try {
            //    FileUtils.moveDirectory(mModelInfo.getFile(), newModelFolder);
            //} catch (IOException e) {
            //    e.printStackTrace();
            //}
        }
    }

    void showNameDialog() {
        final Context context = this;
        final EditText inputName = new EditText(context);
        if (!mModelInfo.getFile().getParent().equals(BrowserUtils.ParentFolderPath) ||
                !(mModelInfo.getModelID().equals(mModelInfo.getName()))) {
            inputName.setText(mModelInfo.getName());
            inputName.setSelectAllOnFocus(true);
        } else {
            inputName.setHint("My Model");
        }
        inputName.setInputType(InputType.TYPE_TEXT_FLAG_CAP_WORDS);
        inputName.setPaddingRelative(70, 33, 70, 30);
        inputName.setFilters(new InputFilter[]{new InputFilter.LengthFilter(30)});

        final AlertDialog.Builder renameAlert = new AlertDialog.Builder(context)
                .setTitle("Name this model")
                .setView(inputName)
                .setPositiveButton("SAVE", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        String newName = inputName.getText().toString();
                        saveCurrentModel(newName);
                        mTopButtonLayout.setVisibility(View.GONE);
                        mToolbar.setVisibility(View.VISIBLE);
                        //if we are saving the model (adding newly) then hitting save should close the viewer and take us to gallery.
                        //But for new scan setResult wont work because we didnt come here directly from gallery.
                        if (!isLaunchFromGallery) {
                            setResult(RESULT_OK, GalleryViewActivity.OPERATION_MODEL_ADDED);
                            onBackPressed();
                        } else {
                            //it was a rename
                            setResult(RESULT_OK, GalleryViewActivity.OPERATION_MODEL_RENAMED);
                        }
                    }
                })
                .setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
        final AlertDialog dialog = renameAlert.create();
        dialog.show();
        if (inputName.getText().length() == 0) {
            dialog.getButton(DialogInterface.BUTTON_POSITIVE).setEnabled(false);
        }

        inputName.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                if (s.length() > 0) {
                    dialog.getButton(DialogInterface.BUTTON_POSITIVE).setEnabled(true);
                } else {
                    dialog.getButton(DialogInterface.BUTTON_POSITIVE).setEnabled(false);
                }
            }
        });
    }

    void showDiscardConfirmationDialog() {
        final Context context = this;

        final AlertDialog.Builder renameAlert = new AlertDialog.Builder(context)
                .setTitle("Discard this model")
                .setMessage("Are you sure you want to discard this model? This action is not reversible.")
                .setPositiveButton("DISCARD", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        discardConfirmed = true;//this flag helps in exiting this activity in onBackpressed()
                        actionDeleteCurrentModel();
                    }
                })
                .setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
        final AlertDialog dialog = renameAlert.create();
        dialog.show();
    }

    // Show the about box
    void showAboutBox() {
        AboutBox.Show(this);
    }

    // Show the settings screen
    void showSettings() {
        //Intent intent = new Intent(this, SettingsActivity.class);
        // TODO startActivity(intent);
    }

    void setResult(int resCode, int operationId) {
        Intent data = new Intent();
        data.putExtra(GalleryViewActivity.OPERATIONPERFORMED, operationId);
        data.putExtra(BrowserUtils.EXTRA_MODELINFO_POS, modelPosInGallery);
        setResult(resCode, data);
    }

    @Override
    public void onBackPressed() {
        //super.onBackPressed();
        Log.i(TAG, "ModelViewerActivity's onBackPressed");
        if ((mTopButtonLayout.getVisibility() == View.VISIBLE) && !discardConfirmed) {
            //if the topbuttonlayout i.e Save & Discard buttons are visible, and user hit backbutton,
            //then show discard confirm dialog, if we came here from onback press called due to
            //discard, then discardConfirmation flag will be true.
            showDiscardConfirmationDialog();
        } else {
            // Go back to gallery
            if (!isLaunchFromGallery) {
                NavUtils.navigateUpFromSameTask(this);
               /* AlarmManager alm = (AlarmManager) getSystemService(Context.ALARM_SERVICE);
                alm.setExact(AlarmManager.RTC_WAKEUP, System.currentTimeMillis() + 0, PendingIntent.getActivity(getApplicationContext(), 0, new Intent(getApplicationContext(), GalleryViewActivity.class), 0));*/
                finish();
                System.exit(0);
            } else {
                super.onBackPressed();
            }
        }
    }


    /**
     * Override the touch screen listener.
     * React to moves and presses on the touchscreen.
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int index = event.getActionIndex();
        int x = (int) event.getX();
        int y = (int) event.getY();
        int button = -1;
        int numPointers = (int) event.getPointerCount();
        int pointerID = (int) event.getPointerId(index);


        //
        // ACTION_DOWN and ACTION_UP only are generated by the first finger.
        // Therefore button = 0 in these cases.
        // We define button state as:  0 - up   and 1 - down
        //
        if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
            for (int i = 0; i < 10; i++) {
                m_pointerArray[i] = -1;
            }

            button = 0;
            m_pointerArray[0] = pointerID;
            //Log.i(TAG, "Button FIRST DOWN: " + button + " " + x + " " + y );
            GLViewerJNILib.mouseButtonFunc(button, 1, x, y);
        }

        if (event.getActionMasked() == MotionEvent.ACTION_UP) {
            // find the index, given the ID. Clear out array, all fingers are off.
            for (int i = 0; i < 10; i++) {
                if (m_pointerArray[i] == pointerID) {
                    button = i;
                    m_pointerArray[i] = -1;
                }
            }

            //Log.i(TAG, "Button LAST UP: " + button);
            GLViewerJNILib.mouseButtonFunc(button, 0, x, y);
        }

        if (event.getActionMasked() == MotionEvent.ACTION_POINTER_DOWN) {
            // find empty slot in pointerArray, assign ID, retrieve index as button number.
            for (int i = 0; i < 10; i++) {
                if (m_pointerArray[i] == -1) {
                    m_pointerArray[i] = pointerID;
                    button = i;
                    break;
                }
            }

            int ptrIndex = event.findPointerIndex(pointerID);
            x = (int) event.getX(ptrIndex);
            y = (int) event.getY(ptrIndex);

            //Log.i(TAG, "Button  DOWN: " + button + " " + x + " " + y );
            GLViewerJNILib.mouseButtonFunc(button, 1, x, y);
        }

        if (event.getActionMasked() == MotionEvent.ACTION_POINTER_UP) {
            // find slot in pointerArray with pointerID, clear it out.
            for (int i = 0; i < 10; i++) {
                if (m_pointerArray[i] == pointerID) {
                    m_pointerArray[i] = -1;
                    button = i;
                    break;
                }
            }

            int ptrIndex = event.findPointerIndex(pointerID);
            x = (int) event.getX(ptrIndex);
            y = (int) event.getY(ptrIndex);

            //Log.i(TAG, "Button UP: " + button);
            GLViewerJNILib.mouseButtonFunc(button, 0, x, y);
        }

        //If a touch is moved on the screen
        if (event.getActionMasked() == MotionEvent.ACTION_MOVE) {
            for (int i = 0; i < numPointers; i++) {
                x = (int) event.getX(i);
                y = (int) event.getY(i);
                pointerID = (int) event.getPointerId(i);

                // find button index
                for (int pointerIndex = 0; pointerIndex < 10; pointerIndex++) {
                    if (m_pointerArray[pointerIndex] == pointerID) {
                        button = pointerIndex;
                        break;
                    }
                }

                GLViewerJNILib.mousePosFunc(button, x, y);
                //Log.i(TAG, "MOVE: " + button + " " + x + " " + y);
            }

        }

        return true;
    }

    private void InitGestures() {
        m_pointerArray = new int[10];
        for (int i = 0; i < 10; i++) {
            m_pointerArray[i] = -1;
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        SurfaceView sv = (SurfaceView) findViewById(R.id.modelViewerSurfaceView);

        mEglCore = new EglCore(null, EglCore.FLAG_RECORDABLE | EglCore.FLAG_TRY_GLES3);
        mRenderThread = new RenderThread(sv.getHolder(), new ActivityHandler(this), mModelInfo, mEglCore);
        mRenderThread.setName("modelViewer GL render thread");
        mRenderThread.start();
        mRenderThread.waitUntilReady();

        RenderHandler rh = mRenderThread.getHandler();
        if (rh != null) {
            rh.sendSurfaceCreated();
        }

        // start the draw events
        Choreographer.getInstance().postFrameCallback(this);

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged fmt=" + format + " size=" + width + "x" + height +
                " holder=" + holder);

        RenderHandler rh = mRenderThread.getHandler();
        if (rh != null) {
            rh.sendSurfaceChanged(format, width, height);
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

        Log.d(TAG, "surfaceDestroyed holder=" + holder);

        // We need to wait for the render thread to shut down before continuing because we
        // don't want the Surface to disappear out from under it mid-render.  The frame
        // notifications will have been stopped back in onPause(), but there might have
        // been one in progress.

        mEglCore.makeNothingCurrent();
        mEglCore.release();

        shutdownRenderThread();

        // If the callback was posted, remove it.  Without this, we could get one more
        // call on doFrame().
        Choreographer.getInstance().removeFrameCallback(this);
        Log.d(TAG, "surfaceDestroyed complete");

        //GLViewerJNILib.shutdown();

    }

    private void shutdownRenderThread() {
        RenderHandler rh = mRenderThread.getHandler();
        if (rh != null) {
            rh.sendShutdown();
            try {
                mRenderThread.join();
            } catch (InterruptedException ie) {
                // not expected
                throw new RuntimeException("join was interrupted", ie);
            }
        }
        mRenderThread = null;
    }

    @Override
    public void doFrame(long l) {
        RenderHandler rh = mRenderThread.getHandler();
        if (rh != null) {
            Choreographer.getInstance().postFrameCallback(this);
            rh.sendDoFrame();
        }
    }

    /**
     * Handles messages sent from the render thread to the UI thread.
     * <p/>
     * The object is created on the UI thread, and all handlers run there.
     */
    static class ActivityHandler extends Handler {
        private static final int MSG_UPDATE_FPS = 1;
        private static final int MSG_SET_PROGRESS_BAR = 2;

        // Weak reference to the Activity; only access this from the UI thread.
        private WeakReference<MeshViewerActivity> mWeakActivity;

        public ActivityHandler(MeshViewerActivity activity) {
            mWeakActivity = new WeakReference<MeshViewerActivity>(activity);
        }

        /**
         * Send an FPS update.  "fps" should be in thousands of frames per second
         * (i.e. fps * 1000), so we can get fractional fps even though the Handler only
         * supports passing integers.
         * <p/>
         * Call from non-UI thread.
         */
        public void sendFpsUpdate(int tfps, int dropped) {
            sendMessage(obtainMessage(MSG_UPDATE_FPS, tfps, dropped));
        }

        public void setProgressBar(int progSetting) {
            sendMessage(obtainMessage(MSG_SET_PROGRESS_BAR, progSetting, 0));
        }

        @Override  // runs on UI thread
        public void handleMessage(Message msg) {
            int what = msg.what;
            //Log.d(TAG, "ActivityHandler [" + this + "]: what=" + what);

            MeshViewerActivity activity = mWeakActivity.get();
            if (activity == null) {
                Log.w(TAG, "ActivityHandler.handleMessage: activity is null");
                return;
            }

            switch (what) {
                case MSG_UPDATE_FPS:
                    //activity.handleUpdateFps(msg.arg1, msg.arg2);
                    break;
                case MSG_SET_PROGRESS_BAR:
                    activity.setProgBar(msg.arg1);
                    break;
                default:
                    throw new RuntimeException("unknown msg " + what);
            }
        }
    }

    /**
     * This class handles all OpenGL rendering.
     * <p/>
     * We use Choreographer to coordinate with the device vsync.  We deliver one frame
     * per vsync.  We can't actually know when the frame we render will be drawn, but at
     * least we get a consistent frame interval.
     * <p/>
     * Start the render thread after the Surface has been created.
     */
    private static class RenderThread extends Thread {
        // Object must be created on render thread to get correct Looper, but is used from
        // UI thread, so we need to declare it volatile to ensure the UI thread sees a fully
        // constructed object.
        private volatile RenderHandler mHandler;

        // Handler we can send messages to if we want to update the app UI.
        private ActivityHandler mActivityHandler;

        private ModelInfo mRenderModelInfo;

        // Used to wait for the thread to start.
        private Object mStartLock = new Object();
        private boolean mReady = false;

        private volatile SurfaceHolder mSurfaceHolder;  // may be updated by UI thread
        private EglCore mEglCore;
        private WindowSurface mWindowSurface;

        private int frameIndex = 0;
        private boolean mViewer_Initialized = false;


        /**
         * Pass in the SurfaceView's SurfaceHolder.  Note the Surface may not yet exist.
         */
        public RenderThread(SurfaceHolder holder,
                            ActivityHandler ahandler,
                            ModelInfo modelInfo,
                            EglCore eglCore) {

            mSurfaceHolder = holder;
            mActivityHandler = ahandler;
            mRenderModelInfo = modelInfo;
            mEglCore = eglCore;
        }

        /**
         * Thread entry point.
         * <p/>
         * The thread should not be started until the Surface associated with the SurfaceHolder
         * has been created.  That way we don't have to wait for a separate "surface created"
         * message to arrive.
         */
        @Override
        public void run() {
            Looper.prepare();
            mHandler = new RenderHandler(this);
            synchronized (mStartLock) {
                mReady = true;
                mStartLock.notify();    // signal waitUntilReady()
            }

            Looper.loop();

            Log.d(TAG, "looper quit");

            if (mWindowSurface != null) {
                mWindowSurface.release();
                mWindowSurface = null;
            }

            synchronized (mStartLock) {
                mReady = false;
            }
        }

        /**
         * Waits until the render thread is ready to receive messages.
         * <p/>
         * Call from the UI thread.
         */
        public void waitUntilReady() {
            synchronized (mStartLock) {
                while (!mReady) {
                    try {
                        mStartLock.wait();
                    } catch (InterruptedException ie) { /* not expected */ }
                }
            }
        }

        /**
         * Shuts everything down.
         */
        private void shutdown() {
            Log.d(TAG, "shutdown");
            GLViewerJNILib.shutdown();
            Looper.myLooper().quit();
        }

        /**
         * Returns the render thread's Handler.  This may be called from any thread.
         */
        public RenderHandler getHandler() {
            return mHandler;
        }

        /**
         * Prepares the surface.
         */
        private void surfaceCreated() {
            Surface surface = mSurfaceHolder.getSurface();
            mWindowSurface = new WindowSurface(mEglCore, surface, false);
            mWindowSurface.makeCurrent();

            frameIndex = 0;
        }

        /**
         * Handles changes to the size of the underlying surface.  Adjusts viewport as needed.
         * Must be called before we start drawing.
         * (Called from RenderHandler.)
         */
        private void surfaceChanged(int width, int height) {
            Log.d(TAG, "surfaceChanged " + width + "x" + height);

            frameIndex = 0;

            if (!mViewer_Initialized) {
                mViewer_Initialized = true;
                mActivityHandler.setProgressBar(PROGBAR_MODE_INDETERMINATE);
                GLViewerJNILib.init(width, height, mRenderModelInfo.getFile().getAbsolutePath(),
                        mRenderModelInfo.getModelID(), mRenderModelInfo.getNumTextures());

            }
        }

        /**
         * Advance state and draw frame in response to a vsync event.
         */
        private void doFrame(long timeStampNanos) {

            boolean swapResult;
            //this part for display rendering
            GLViewerJNILib.step(); //this does native opengl drawing
            swapResult = mWindowSurface.swapBuffers(); //publishes

            if (frameIndex == 0)
                mActivityHandler.setProgressBar(PROGBAR_MODE_DISMISS);

            frameIndex++;

            if (!swapResult) {
                // This can happen if the Activity stops without waiting for us to halt.
                Log.w(TAG, "swapBuffers failed, killing renderer thread");
                shutdown();
                return;
            }
        }

        public void createMP4ofModel() {

            int progress = 0;

            //below is a hack to handle case where mModelInfo was updated with the suffix "_done"
            //after the user saved, but our local copy mRenderModelInfo wasn't updated with that info
            //TODO: see if the whole _done mechanism can be removed
            if (!mRenderModelInfo.getFile().getName().endsWith(BrowserUtils.FolderSuffix)) {
                String fileParent = mRenderModelInfo.getFile().getParent();
                String fileName = mRenderModelInfo.getFile().getName() + BrowserUtils.FolderSuffix;
                mRenderModelInfo = new ModelInfo(fileParent, fileName);
            }

            //if the mp4 doesn't exist then create it and send, otherwise just send
            if (!mRenderModelInfo.checkMP4Created()) {

                mActivityHandler.setProgressBar(PROGBAR_MODE_DETERMINATE);

                MeshViewerUtils.setupGLandMP4forMeshModel(mRenderModelInfo, true, mEglCore);

                for (int i = 0; i < MeshViewerUtils.TOTAL_FRAMES; i++) {
                    MeshViewerUtils.renderAndEncodeSingleFrame(i);
                    mActivityHandler.setProgressBar(i);
                }

                MeshViewerUtils.cleanupAfterEncoding();

                //restore the viewer after mp4 creation is done
                GLViewerJNILib.setViewer(0, 0, mWindowSurface.getWidth(), mWindowSurface.getHeight());
                mWindowSurface.makeCurrent(); //set back
                mActivityHandler.setProgressBar(PROGBAR_MODE_DISMISS);

            }

            List<File> selectedFolderList = new ArrayList<>();
            selectedFolderList.add(mRenderModelInfo.getFile());
            EmailComposer.Send(mActivityHandler.mWeakActivity.get(),
                    "3d_models",
                    BrowserUtils.getVideoUrisFromFolderList(selectedFolderList));

        }
    }

    /**
     * Handler for RenderThread.  Used for messages sent from the UI thread to the render thread.
     * <p/>
     * The object is created on the render thread, and the various "send" methods are called
     * from the UI thread.
     */
    private static class RenderHandler extends Handler {
        private static final int MSG_SURFACE_CREATED = 0;
        private static final int MSG_SURFACE_CHANGED = 1;
        private static final int MSG_DO_FRAME = 2;
        private static final int MSG_SHUTDOWN = 3;
        private static final int MSG_CREATE_MP4 = 4;

        // This shouldn't need to be a weak ref, since we'll go away when the Looper quits,
        // but no real harm in it.
        private WeakReference<RenderThread> mWeakRenderThread;

        /**
         * Call from render thread.
         */
        public RenderHandler(RenderThread rt) {
            mWeakRenderThread = new WeakReference<RenderThread>(rt);
        }

        /**
         * Sends the "surface created" message.
         * <p/>
         * Call from UI thread.
         */
        public void sendSurfaceCreated() {
            sendMessage(obtainMessage(RenderHandler.MSG_SURFACE_CREATED));
        }

        /**
         * Sends the "surface changed" message, forwarding what we got from the SurfaceHolder.
         * <p/>
         * Call from UI thread.
         */
        public void sendSurfaceChanged(@SuppressWarnings("unused") int format,
                                       int width, int height) {
            // ignore format
            sendMessage(obtainMessage(RenderHandler.MSG_SURFACE_CHANGED, width, height));
        }

        /**
         * Sends the "create an mp4" message to the render thread
         * <p/>
         * Call from UI thread.
         */
        public void sendCreateMP4() {
            sendMessageAtFrontOfQueue(obtainMessage(RenderHandler.MSG_CREATE_MP4));
        }

        /**
         * Sends the "do frame" message, forwarding the Choreographer event.
         * <p/>
         * Call from UI thread.
         */
        public void sendDoFrame() {
            sendMessage(obtainMessage(RenderHandler.MSG_DO_FRAME));
        }

        /**
         * Sends the "shutdown" message, which tells the render thread to halt.
         * <p/>
         * Call from UI thread.
         */
        public void sendShutdown() {
            sendMessage(obtainMessage(RenderHandler.MSG_SHUTDOWN));
        }

        @Override  // runs on RenderThread
        public void handleMessage(Message msg) {
            int what = msg.what;
            //Log.d(TAG, "RenderHandler [" + this + "]: what=" + what);

            RenderThread renderThread = mWeakRenderThread.get();
            if (renderThread == null) {
                Log.w(TAG, "RenderHandler.handleMessage: weak ref is null");
                return;
            }

            switch (what) {
                case MSG_SURFACE_CREATED:
                    renderThread.surfaceCreated();
                    break;
                case MSG_SURFACE_CHANGED:
                    renderThread.surfaceChanged(msg.arg1, msg.arg2);
                    break;
                case MSG_DO_FRAME:
                    long timestamp = (((long) msg.arg1) << 32) |
                            (((long) msg.arg2) & 0xffffffffL);
                    renderThread.doFrame(timestamp);
                    break;
                case MSG_CREATE_MP4:
                    renderThread.createMP4ofModel();
                    break;
                case MSG_SHUTDOWN:
                    renderThread.shutdown();
                    break;
                default:
                    throw new RuntimeException("unknown message " + what);
            }
        }


    }


}

