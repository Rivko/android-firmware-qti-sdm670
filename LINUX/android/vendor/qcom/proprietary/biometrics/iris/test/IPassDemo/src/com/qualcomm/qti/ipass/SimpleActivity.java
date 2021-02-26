/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.ipass;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.Typeface;
import com.qualcomm.qti.iris.Iris;
import com.qualcomm.qti.iris.IrisManager;
import com.qualcomm.qti.iris.IrisOperationStatus;
import com.qualcomm.qti.iris.Iris;
import com.qualcomm.qti.iris.IrisError;

import android.media.AudioManager;
import android.media.ToneGenerator;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.os.Handler;
import android.os.Vibrator;
import android.util.Log;
import android.util.Size;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class SimpleActivity extends Activity implements SurfaceHolder.Callback {

    private static final String TAG = "IrisActivity";
    boolean mStarted = false;

    private enum Status {
        STOPPED, ENROLLING, VERIFYING, NONMATCH
    }

    private Status mCurrentStatus;

    private enum AuthMode {
        SINGLE, CONTINUOUS, NONE
    }

    private final Handler mHandler = new Handler();
    private TextView mTextViewInfo;

    public static IrisManager IPassIrisManager;
    CancellationSignal mCancelSignal;

    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Surface mSurface;
    private View mParent;
    private int mWidth = 1920, mHeight = 960;
    private ToneGenerator mToneGen = new ToneGenerator(AudioManager.STREAM_MUSIC, 100);

    private Button mBtnEnroll;
    private ListView mEnrollListView;
    private ArrayAdapter<String> mEnrollListAdapter = null;
    private final ArrayList<String> mEnrollList = new ArrayList<String>();
    private int mEnrollmentProgress = -1;
    private byte[] mToken = new byte[69];
    private ProgressBar mEnrollLeftProgressBar;
    private ProgressBar mEnrollRightProgressBar;

    private Button mBtnSingleVerify;
    private Button mBtnContinueVerify;
    private int authCounter = 0;
    private int verifiedCounter = 0;
    private int notVerifiedCounter = 0;
    private AuthMode mAuthMode = AuthMode.NONE;

    private Button mBtnCancelAuth;

    private Toast mToast;
    private long mTic = 0;
    private long mToc = 0;
    private long mAuthStartTime = 0;
    private TextView mFpsInfo;
    private ProgressBar mTimerBar;


    // SurfaceHolder callbacks
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.v(TAG, "surfaceChanged: width = " + width + ", height = " + height);
        Log.v(TAG, "parent width=" + mParent.getWidth());
        if (mParent != null && mParent.getWidth() > 0) {
            FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) mSurfaceView.getLayoutParams();
            params.width = mParent.getWidth();
            params.height = params.width * mHeight / mWidth;
            Log.v(TAG, "layout width=" + params.width + " layout height=" + params.height);
            mSurfaceView.setLayoutParams(params);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.v(TAG, "surfaceCreated");
        mSurface = holder.getSurface();
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                //handle_authenticate();
                //updateButtons();
            }
        });
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.v(TAG, "surfaceDestroyed");
        mSurfaceHolder.setFormat(PixelFormat.TRANSPARENT);
        mSurface = null;
        Log.v(TAG, "set null surface");
        IPassIrisManager.setPreviewSurface(null);
    }

    public class TestAuthenticationCallback extends IrisManager.AuthenticationCallback {
        @Override
        public void onAuthenticationError(IrisError error) {
            postInfo(error.mErrorDesc.toString());
            mCurrentStatus = Status.STOPPED;
            resetViews();
        }

        @Override
        public void onAuthenticationStatus(IrisOperationStatus status) {
            if (status.mQuality != 0) {
                mToast.setText(status.mHelpString);
                Log.v(TAG, "quality=" + status.mQuality);
                if (status.mHelpString != null)
                    Log.v(TAG, status.mHelpString.toString());

                mToast.show();
            } else {
                authCounter++;
                mToast.cancel();
            }
            if (AuthMode.CONTINUOUS == mAuthMode) {
                if (status.mDesc.mVendorInfo.length == 4) {
                    byte[] irisData = status.mDesc.mVendorInfo;
                    int irisId = irisData[0] | (irisData[1] << 8) | (irisData[2] << 16) | (irisData[3] << 24);
                    List<Iris> enrolledIris = IPassIrisManager.getEnrolledIris();
                    for (int i = 0; i < enrolledIris.size(); i++) {
                        mEnrollListView.getChildAt(i).setBackgroundColor(Color.TRANSPARENT);
                        if (enrolledIris.get(i).getIrisId() == irisId) {
                            mEnrollListView.getChildAt(i).setBackgroundColor(Color.rgb(215, 120, 0));
                            mToneGen.startTone(ToneGenerator.TONE_CDMA_PIP, 30);
                            verifiedCounter++;
                            notVerifiedCounter = 0;
                        }
                    }
                    mToc = System.currentTimeMillis();
                    long latency = mToc - mTic;
                    mTic = mToc;
                    postFPSInfo(latency + "ms");
                    Log.v(TAG, irisId + " identified " + "with latency: " + latency + "ms");
                } else {
                    notVerifiedCounter++;
                    postInfo("Verifying...");
                    if (notVerifiedCounter >= 5) {
                        postFPSInfo("");
                        List<Iris> enrolledIris = IPassIrisManager.getEnrolledIris();
                        for (int i = 0; i < enrolledIris.size(); i++) {
                            mEnrollListView.getChildAt(i).setBackgroundColor(Color.TRANSPARENT);
                        }
                        notVerifiedCounter = 0;
                    }
                }

                if (authCounter >= 50 && verifiedCounter == 0) {
                    handle_cancel();
                    postInfo("Iris can not be verified!");
                    Vibrator v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
                    v.vibrate(100);
                    mToneGen.startTone(ToneGenerator.TONE_CDMA_ABBR_INTERCEPT, 500);
                    mCurrentStatus = Status.NONMATCH;
                }

                long timer = (System.currentTimeMillis() - mAuthStartTime) / 100;
                mTimerBar.setProgress(100 - (int) timer, true);
                if (timer >= 100) {
                    handle_cancel();
                }
            }

        }

        @Override
        public void onAuthenticationSucceeded(IrisManager.AuthenticationResult result) {
            mCurrentStatus = Status.STOPPED;
            updateButtons();
            resetViews();
            refreshEnrollList();
            Iris irisData = result.getIris();
            Log.v(TAG, "iris id " + irisData.getIrisId());
            CharSequence irisName = null;
            if (irisData != null) {
                List<Iris> enrolledIris = IPassIrisManager.getEnrolledIris();
                for (int i = 0; i < enrolledIris.size(); i++) {
                    mEnrollListView.getChildAt(i).setBackgroundColor(Color.TRANSPARENT);
                    if (enrolledIris.get(i).getIrisId() == irisData.getIrisId()) {
                        irisName = enrolledIris.get(i).getName();
                        mEnrollListView.getChildAt(i).setBackgroundColor(Color.rgb(215, 120, 0));
                        mToneGen.startTone(ToneGenerator.TONE_CDMA_PIP, 30);
                    }
                }
            }
            if (irisName != null) {
                Log.v(TAG, irisName + " Authenticatd");
                postInfo("Authentication is done");
            } else
                Log.v(TAG, "Authentication successful, unknown user name");
        }

        @Override
        public void onAuthenticationFailed() {
            postInfo("Fail to Authenticate");
            mCurrentStatus = Status.STOPPED;
            updateButtons();
            resetViews();
        }
    }

    private IrisManager.EnrollmentCallback mEnrollmentCallback
            = new IrisManager.EnrollmentCallback() {

        @Override
        public void onEnrollmentStatus(IrisOperationStatus status) {
            Log.v(TAG, "onEnrollmentStatus progress: " + status.mProgress);
            if (status.mQuality != 0) {
                mToast.setText(status.mHelpString);
                Log.v(TAG, "quality=" + status.mQuality);
                if (status.mHelpString != null)
                    Log.v(TAG, status.mHelpString.toString());
                mToast.show();
            } else {
                mToast.cancel();
            }

            mEnrollmentProgress = status.mProgress;
            mEnrollLeftProgressBar.setProgress(mEnrollmentProgress, true);
            mEnrollRightProgressBar.setProgress(mEnrollmentProgress, true);
            postInfo("Enrolling");
        }

        @Override
        public void onEnrollmentDone(int irisId) {
            Log.v(TAG, "onEnrollmentDone " + irisId);
            int result = IPassIrisManager.postEnroll();
            if (result < 0) {
                Log.v(TAG, "postEnroll failed: result = " + result);
            }
            mEnrollmentProgress = 100;
            mCurrentStatus = Status.STOPPED;
            mEnrollLeftProgressBar.setProgress(100, true);
            mEnrollRightProgressBar.setProgress(100, true);
            postInfo("Enrollment is done");
            refreshEnrollList();
            updateButtons();
            resetViews();
        }

        @Override
        public void onEnrollmentError(IrisError error) {
            Log.v(TAG, "onEnrollmentError ");
            postInfo(error.mErrorDesc.toString());
            mCurrentStatus = Status.STOPPED;
            mEnrollmentProgress = -1;
            mEnrollLeftProgressBar.setProgress(0, true);
            mEnrollRightProgressBar.setProgress(0, true);
            updateButtons();
            resetViews();
        }
    };

    public static IrisManager.RemovalCallback mRemoveCallback
            = new IrisManager.RemovalCallback() {
        @Override
        public void onRemovalSucceeded(Iris iris) {
            //TBD
        }

        @Override
        public void onRemovalError(Iris iris, IrisError error) {
            //TBD
        }
    };

    private void updateButtons() {
        if (mCurrentStatus == Status.STOPPED || mCurrentStatus == Status.NONMATCH) {
            mBtnSingleVerify.setEnabled(true);
            mBtnContinueVerify.setEnabled(true);
            mBtnEnroll.setEnabled(true);
            mBtnCancelAuth.setEnabled(false);
            mBtnCancelAuth.setBackgroundColor(Color.LTGRAY);
        } else {
            mBtnSingleVerify.setEnabled(false);
            mBtnContinueVerify.setEnabled(false);
            mBtnEnroll.setEnabled(false);
            mBtnCancelAuth.setEnabled(true);
            mBtnCancelAuth.setBackgroundColor(Color.rgb(215, 120, 0));
        }
    }

    @Override
    protected void onResume() {
        Log.v(TAG, "onResume");
        super.onResume();
        mEnrollListAdapter.notifyDataSetChanged();
        refreshEnrollList();
        resetViews();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        if (hasFocus) {
            refreshEnrollList();
        }
    }

    @Override
    protected void onPause() {
        if (mCancelSignal != null)
            mCancelSignal.cancel();
        super.onPause();
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.simple);

        IPassIrisManager = new IrisManager(this);
        Size previewSize = IPassIrisManager.getPreviewSize(IrisManager.IRIS_PREVIEW_SIZE_AUTH);

        if (previewSize != null)
            Log.v(TAG, previewSize.toString());
        else
            Log.v(TAG, "fail to get preview size");

        mParent = findViewById(R.id.surface_parent);
        mTextViewInfo = (TextView) findViewById(R.id.textViewInfo);
        mFpsInfo = (TextView) findViewById(R.id.fpsInfo);
        mFpsInfo.setTextColor(Color.rgb(215, 120, 0));
        mFpsInfo.setTextSize(30.0f);
        mFpsInfo.setTypeface(mFpsInfo.getTypeface(), Typeface.BOLD);
        mFpsInfo.setShadowLayer(10, 3, 3, Color.BLACK);
        mTimerBar = (ProgressBar) findViewById(R.id.timerbar);

        mSurfaceView = (SurfaceView) findViewById(R.id.texture);
        mSurfaceView.setVisibility(View.VISIBLE);
        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(this);
        mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        if (previewSize != null) {
            mWidth = previewSize.getWidth();
            mHeight = previewSize.getHeight();
        }
        mSurfaceHolder.setFixedSize(mWidth, mHeight);
        mSurfaceView.setVisibility(View.INVISIBLE);
        mCurrentStatus = Status.STOPPED;

        mBtnSingleVerify = (Button) findViewById(R.id.btnSingleVerify);
        mBtnSingleVerify.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mCurrentStatus == Status.STOPPED) {
                    mAuthMode = AuthMode.SINGLE;
                    handle_authenticate(mAuthMode);
                }
            }
        });

        mBtnContinueVerify = (Button) findViewById(R.id.btnContinueVerify);
        mBtnContinueVerify.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (Status.STOPPED == mCurrentStatus) {
                    mAuthStartTime = System.currentTimeMillis();
                    mAuthMode = AuthMode.CONTINUOUS;
                    handle_authenticate(mAuthMode);
                }
            }
        });

        mBtnEnroll = (Button) findViewById(R.id.btnEnroll);
        mBtnEnroll.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mCurrentStatus == Status.STOPPED) {
                    handle_enroll();
                }
            }
        });
        mEnrollLeftProgressBar = (ProgressBar) findViewById(R.id.enrollProgLeft);
        mEnrollRightProgressBar = (ProgressBar) findViewById(R.id.enrollProgRight);
        mEnrollLeftProgressBar.setVisibility(View.INVISIBLE);
        mEnrollRightProgressBar.setVisibility(View.INVISIBLE);

        mBtnCancelAuth = (Button) findViewById(R.id.btnCancelAuth);
        mBtnCancelAuth.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mCurrentStatus != Status.STOPPED) {
                    handle_cancel();
                }
            }
        });

        mToast = Toast.makeText(this, "", Toast.LENGTH_SHORT);
        mToast.setGravity(Gravity.TOP | Gravity.LEFT, 0, 0);

        mEnrollListView = (ListView) findViewById(R.id.enrolledList);
        mEnrollListAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, mEnrollList) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {

                View view = super.getView(position, convertView, parent);
                TextView textview = (TextView) view.findViewById(android.R.id.text1);
                textview.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
                textview.setGravity(Gravity.CENTER);

                return view;
            }
        };
        mEnrollListView.setOnItemClickListener(new OnItemClickListener() {
            public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {

                List<Iris> enrolledIris = IPassIrisManager.getEnrolledIris();

		if(enrolledIris.size() <= position){
			return;
		}

                Iris selectedIris = enrolledIris.get(position);

                FragmentTransaction ft = getFragmentManager().beginTransaction();
                Fragment prev = getFragmentManager().findFragmentByTag("dialog");
                if (prev != null) {
                    ft.remove(prev);
                }
                ft.addToBackStack(null);

                // Create and show the dialog.
                DialogFragment renameDeleteDialog = RenameDeleteDialog.newInstance(selectedIris);
                renameDeleteDialog.show(ft, RenameDeleteDialog.class.getName());
            }
        });

        mEnrollListView.setAdapter(mEnrollListAdapter);
        mEnrollListView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);

        refreshEnrollList();
        resetViews();
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        return super.onOptionsItemSelected(item);
    }

    public void handle_cancel() {
        if (mCurrentStatus == Status.STOPPED) {
            return;
        }
        cancelVerification();
        resetViews();
        updateButtons();
    }

    public void handle_authenticate(AuthMode authenticationMode) {
        if (mCurrentStatus == Status.VERIFYING || mCurrentStatus == Status.ENROLLING.ENROLLING) {
            return;
        }

        if (!IPassIrisManager.hasEnrolledIris()) {
            postInfo("No enrolled iris");
            return;
        }

        if (mSurface == null) {
            mSurfaceView.setVisibility(View.VISIBLE);
        }
        mCurrentStatus = Status.VERIFYING;
        resetEnrollListView();
        updateButtons();

        TestAuthenticationCallback authenticationResult = new TestAuthenticationCallback();
        mCancelSignal = new CancellationSignal();

        IPassIrisManager.setPreviewSurface(mSurface);
        byte[] param = new byte[1];
        if (authenticationMode == AuthMode.SINGLE) {
            param[0] = 0;
        } else if (authenticationMode == AuthMode.CONTINUOUS) {
            param[0] = 1;
        }
        IPassIrisManager.configure(IrisManager.IRIS_CONFIGURE_AUTH, param);
        IPassIrisManager.authenticate(null, mCancelSignal, 0, authenticationResult, null);
        postInfo("Verifying...");
    }

    public void handle_enroll() {
        if (mCurrentStatus == Status.VERIFYING || mCurrentStatus == Status.ENROLLING.ENROLLING) {
            return;
        }

        if (IPassIrisManager.hasEnrolledIris()) {
            List<Iris> enrolledIris = IPassIrisManager.getEnrolledIris();
            if (enrolledIris.size() >= 5) {
                postInfo("Cannot enroll more than 5 users.");
                return;
            }
        }

        if (mSurface == null) {
            mSurfaceView.setVisibility(View.VISIBLE);
        }

        mEnrollLeftProgressBar.setVisibility(View.VISIBLE);
        mEnrollRightProgressBar.setVisibility(View.VISIBLE);
        mCurrentStatus = Status.ENROLLING;
        mEnrollmentProgress = 0;
        resetEnrollListView();
        updateButtons();

        mCancelSignal = new CancellationSignal();
        new Random().nextBytes(mToken);

        IPassIrisManager.setPreviewSurface(mSurface);
        IPassIrisManager.enroll(mToken, mCancelSignal, 0, mEnrollmentCallback);
    }

    void postInfo(final String info) {
        mTextViewInfo.setText(info);
    }

    void postFPSInfo(final String fps) {
        mFpsInfo.setText(fps);
    }

    public void resetEnrollListView() {
        if (null == IPassIrisManager || null == mEnrollListView) {
            return;
        }

        List<Iris> enrolledIris = IPassIrisManager.getEnrolledIris();
        for (int i = 0; i < enrolledIris.size(); i++) {
            if (null != mEnrollListView.getChildAt(i)) {
                mEnrollListView.getChildAt(i).setBackgroundColor(Color.TRANSPARENT);
            }
        }
    }

    public void refreshEnrollList() {
        Log.v(TAG, "referesh enroll list");
        if (null == IPassIrisManager || null == mEnrollListAdapter) {
            return;
        }

        mEnrollList.clear();
        List<Iris> enrolledIris = IPassIrisManager.getEnrolledIris();
        CharSequence irisName = null;
        for (int i = 0; i < enrolledIris.size(); i++) {
            irisName = enrolledIris.get(i).getName();
            Log.v(TAG, "name=" + irisName);
            mEnrollList.add(irisName.toString());
        }
        mEnrollListAdapter.notifyDataSetChanged();
    }

    public void cancelVerification() {
        mSurfaceView.setVisibility(View.INVISIBLE);
        if (mCancelSignal != null)
            mCancelSignal.cancel();
        mToast.cancel();
    }

    public void resetViews() {
        postFPSInfo("");
        mCurrentStatus = Status.STOPPED;
        mAuthMode = AuthMode.NONE;
        authCounter = 0;
        verifiedCounter = 0;
        mTimerBar.setProgress(100, true);
        mEnrollLeftProgressBar.setProgress(0, true);
        mEnrollRightProgressBar.setProgress(0, true);
        mToast.cancel();
        mSurfaceView.setVisibility(View.INVISIBLE);
        mEnrollRightProgressBar.setVisibility(View.INVISIBLE);
        mEnrollLeftProgressBar.setVisibility(View.INVISIBLE);
        resetEnrollListView();
        updateButtons();
    }

    public static class RenameDeleteDialog extends DialogFragment {

        private Iris mIris;
        private EditText mDialogTextField;
        private String mIrisName;
        private Boolean mTextHadFocus;
        private int mTextSelectionStart;
        private int mTextSelectionEnd;

        public static RenameDeleteDialog newInstance(final Iris iris) {

            RenameDeleteDialog dialogFragment = new RenameDeleteDialog();
            Bundle bundle = new Bundle();
            bundle.putParcelable("iris", iris);
            dialogFragment.setArguments(bundle);

            return dialogFragment;

        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            mIris = getArguments().getParcelable("iris");
            if (savedInstanceState != null) {
                mIrisName = savedInstanceState.getString("irisName");
                mTextHadFocus = savedInstanceState.getBoolean("textHadFocus");
                mTextSelectionStart = savedInstanceState.getInt("startSelection");
                mTextSelectionEnd = savedInstanceState.getInt("endSelection");
            }
            final AlertDialog alertDialog = new AlertDialog.Builder(getActivity())
                    .setView(R.layout.iris_rename_dialog)
                    .setPositiveButton(
                            R.string.iris_enroll_dialog_rename,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    final String newName =
                                            mDialogTextField.getText().toString();
                                    final CharSequence name = mIris.getName();
                                    if (!newName.equals(name)) {
                                        SimpleActivity.IPassIrisManager.rename(mIris.getIrisId(), newName);
                                    }

                                    dialog.dismiss();
                                }
                            })
                    .setNeutralButton(
                            R.string.iris_enroll_dialog_delete,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    if (SimpleActivity.IPassIrisManager.getEnrolledIris().size() > 0) {
                                        SimpleActivity.IPassIrisManager.remove(mIris, null);
                                    }
                                    dialog.dismiss();
                                }
                            })
                    .create();
            alertDialog.setOnShowListener(new DialogInterface.OnShowListener() {
                @Override
                public void onShow(DialogInterface dialog) {
                    mDialogTextField = (EditText) alertDialog.findViewById(
                            R.id.iris_rename_field);
                    CharSequence name = mIrisName == null ? mIris.getName() : mIrisName;
                    mDialogTextField.setText(name);
                    if (mTextHadFocus == null) {
                        mDialogTextField.selectAll();
                    } else {
                        mDialogTextField.setSelection(mTextSelectionStart, mTextSelectionEnd);
                    }
                }
            });
            if (mTextHadFocus == null || mTextHadFocus) {
                // Request the IME
                alertDialog.getWindow().setSoftInputMode(
                        WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
            }
            return alertDialog;
        }

        @Override
        public void onSaveInstanceState(Bundle outState) {
            super.onSaveInstanceState(outState);
            if (mDialogTextField != null) {
                outState.putString("irisName", mDialogTextField.getText().toString());
                outState.putBoolean("textHadFocus", mDialogTextField.hasFocus());
                outState.putInt("startSelection", mDialogTextField.getSelectionStart());
                outState.putInt("endSelection", mDialogTextField.getSelectionEnd());
            }
        }
    }
}
