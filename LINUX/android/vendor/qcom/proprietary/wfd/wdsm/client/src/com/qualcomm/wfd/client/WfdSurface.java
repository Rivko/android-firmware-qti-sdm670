/*
* Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/


package com.qualcomm.wfd.client;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Configuration;
import android.nfc.Tag;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.view.ActionMode;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;

import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdStatus;
import com.qualcomm.wfd.client.ServiceUtil;
import com.qualcomm.wfd.client.net.NetManagerIF;
import com.qualcomm.wfd.service.IWfdActionListener;
import com.qualcomm.wfd.service.IWfdSession;

import java.util.Arrays;
import java.util.concurrent.atomic.AtomicInteger;

import static com.qualcomm.wfd.client.WfdClientUtils.*;

//public class WfdSurface extends SurfaceView implements SurfaceHolder.Callback, Runnable {
public class WfdSurface extends SurfaceView implements SurfaceHolder.Callback {

    private static final String TAG = "Client.WfdSurface_";
    private static final AtomicInteger mSurfaceCounter = new AtomicInteger(1);
    private String mTAG;
    private boolean mCaptureEvents = false;
    private boolean bDisablePauseOnSurfaceDestroyed = false;
    private ScaleGestureDetector mScaleDetector;
    private int mHeight=0;
    private int mWidth=0;
    private int mOrientation =0;
    private WfdStatus beforeDestroy, currentStatus;
    private NetManagerIF netManager;
    private ActionMode mActionMode;
    private int mId;
    private IWfdSession mWfdSession;
    private IWfdActionListener mActionListener;
    private WfdSurfaceEventHandler mEventHandler;
    private WfdSurfaceListener mListener;
    private ProgressDialog startSessionProgressDialog;
    private boolean mStartingSession;
    private int mLastUIBCRotId = 0;
    private int mLastDecoderLatencyId = 0;

    public WfdSurface(Context context, IWfdSession wfdSession, WfdSurfaceListener listener) {
        super(context);
        mId = mSurfaceCounter.getAndIncrement();
        mTAG = TAG.concat(Integer.toString(mId));
        mWfdSession = wfdSession;
        Log.d(mTAG, "constructor called");
        getHolder().addCallback(this);
        beforeDestroy = new WfdStatus();
        currentStatus = new WfdStatus();
        beforeDestroy.state = WfdEnums.SessionState.INVALID.ordinal();
        currentStatus.state = WfdEnums.SessionState.INVALID.ordinal();
        netManager = Whisperer.getInstance().getNetManager();
        mEventHandler = new WfdSurfaceEventHandler();
        mListener = listener;
        mActionListener = WfdClientUtils.createActionListener(mEventHandler);
        try {
            mWfdSession.registerListener(mActionListener);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }
    public int getId() {
        return mId;
    }

    public void handleSurfacePress(View v) {
        if (mActionMode != null) {
            return;
        }
        // Start the CAB using the ActionMode.Callback defined above
        mActionMode = startActionMode(mActionModeCallback);
        setClickable(true);
    }

    @Override
    public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
        Log.d(mTAG, "surfaceChanged() called: " + arg1 + ", " + arg2 + ", " + arg3);
        if(!ServiceUtil.getmServiceAlreadyBound()) {
            Log.e(mTAG,"Service not avaialable yet!");
            return;
        }
        mWidth  = arg2;
        mHeight = arg3;
        Configuration config = getResources().getConfiguration();
        mOrientation = config.orientation;
        try {
            Bundle surfProp = new Bundle(1);
            int[] loc = new int[2];
            WfdSurface.this.getLocationInWindow(loc);
            /*
            Just
             */
            WfdSurface.this.getLocationOnScreen(loc);
            Log.d(mTAG,"Location on Screen: "+ Arrays.toString(loc));
            /*
            for debug compare
             */
            Log.d(mTAG,"Location in Window: "+ Arrays.toString(loc));
            //Populate in L,T,R,B format
            int[] data={loc[0],loc[1],
                        loc[0] + mWidth,
                        loc[1] + mHeight,
                        mOrientation};
            surfProp.putIntArray(WfdEnums.SURFACE_PROP_KEY,data);
            int ret = mWfdSession.setSurfacePropEx(surfProp);
            if (ret != 0) {
                Log.e(mTAG, "setSurfacePropEx Error: "
                        + WfdEnums.ErrorType.getValue(ret));
            }
        } catch (RemoteException e) {
            Log.e(mTAG, "RemoteException", e);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder arg0) {
        Log.d(mTAG, "surfaceCreated() called");
        Log.d(mTAG, "surfaceHolder: " + getHolder());
        Log.d(mTAG, "surface: " + getHolder().getSurface());
        setFocusable(true);
        setFocusableInTouchMode(true);
        boolean focusStatus = requestFocus();
        Log.d(mTAG, "focus status:" + focusStatus);

        Log.d(mTAG, "MIRACAST_SINK");
        netManager.setMiracastMode(NetManagerIF.MIRACAST_SINK);

        if (getHolder().getSurface().isValid()) {
            Log.d(mTAG, "surfaceHolder.getSurface().isValid() is true");
        } else {
            Log.d(mTAG, "surfaceHolder.getSurface().isValid() is false");
        }

        try {
            Log.d(mTAG, "setSurface() called on: " + mWfdSession);
            mWfdSession.setSurface(getHolder().getSurface());
        } catch (RemoteException e) {
            Log.e(mTAG, "setSurface()- Remote exception: ", e);
        }

        try {
            currentStatus = mWfdSession.getStatus();
            Log.d(mTAG, "Session state during surface creation is "+ currentStatus.state);
        } catch (RemoteException e) {
            Log.e(mTAG, "Remote Exception ",e);
        }

        if(!bDisablePauseOnSurfaceDestroyed &&
                currentStatus.state != WfdEnums.SessionState.STANDING_BY.ordinal()
                && currentStatus.state != WfdEnums.SessionState.STANDBY.ordinal()){
            currentStatus.state = WfdEnums.SessionState.INVALID.ordinal();
            if (beforeDestroy.state == WfdEnums.SessionState.PLAY.ordinal()) {
                Log.d(mTAG, "Calling play on surface creation");
                beforeDestroy.state = WfdEnums.SessionState.INVALID.ordinal();
                try {
                    mWfdSession.play();
                } catch (RemoteException e) {
                    Log.d(mTAG, "Remote Exception ", e);
                }
            }
        }
        if (!mStartingSession) {
            startSessionProgressDialog = ProgressDialog.show(getContext(),
                    "Starting Session on ".concat(mTAG), "Press back to cancel", true, true,
                    new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface dialog) {
                            Log.d(TAG, "startSessionProgressDialog onCancel called");
                        }
                    });
            mStartingSession = true;
        }
        try {
            mCaptureEvents = ServiceUtil.getInstance().getUIBCStatus();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder arg0) {
        Log.d(mTAG, "surfaceDestroyed() called");
        try {
            if(ServiceUtil.getmServiceAlreadyBound() != false) {
                beforeDestroy = mWfdSession.getStatus();
                Log.d(mTAG, "Session state is " + beforeDestroy.state);
            } else {
                Log.d(mTAG, "WFDSession Instance not available");
                return;
            }
        } catch (RemoteException e) {
            Log.e(mTAG, "setSurface()- Remote exception: ", e);
        }

        Log.d(mTAG, "MIRACAST_DISABLED");
        netManager.setMiracastMode(NetManagerIF.MIRACAST_DISABLED);

        try {
            Log.d(mTAG, "setSurface() called");
            mWfdSession.setSurface(null);
            //using null as a poison pointer
        } catch (RemoteException e) {
            Log.e(mTAG, "setSurface()- Remote exception: ", e);
        }

        if (!bDisablePauseOnSurfaceDestroyed  && beforeDestroy.state == WfdEnums.SessionState.PLAY.ordinal()) {
            Log.d(mTAG, "Calling pause on surface destruction");
            try {
                mWfdSession.pause();
            } catch (RemoteException e) {
                Log.d(mTAG, "Remote Exception ", e);
            }
        }
        if (startSessionProgressDialog != null) {
            startSessionProgressDialog.dismiss();
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        Log.d(TAG, "onTouchEvent() called");
        if (mCaptureEvents) {
            Log.d(TAG, "onTouchEvent()- capturing events");
            // Use ScaleGestureDetector to find scale gesture
            try {
                int ret = mWfdSession.sendEvent(event);
                if (ret != 0) {
                    Log.e(TAG, "Error: " + WfdEnums.ErrorType.getValue(ret));
                    return false;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "RemoteException", e);
                return false;
            }
            return true;
        }
        return super.onTouchEvent(event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        Log.d(mTAG, "onKeyUp() called with keycode: " + event);
        if (mCaptureEvents && !dropKey(keyCode)) {
            Log.d(mTAG, "onKeyUp() called while capturing events with keycode: " + keyCode);
            try {
                int ret = mWfdSession.sendEvent(event);
                if (ret != 0) {
                    Log.e(mTAG, "Error: " + WfdEnums.ErrorType.getValue(ret));
                }
            } catch (RemoteException e) {
                Log.e(mTAG, "RemoteException", e);
            }
        }

        //allow other key receivers to handle the event
        //based on the keyCode
        return dontHandleKeyFramework(keyCode);
    }

    private boolean dropKey(int keyCode) {
        Log.e(mTAG, "dropkey called with keycode: " + keyCode);
        switch (keyCode) {
            case KeyEvent.KEYCODE_VOLUME_MUTE:
            case KeyEvent.KEYCODE_VOLUME_DOWN:
            case KeyEvent.KEYCODE_VOLUME_UP:
            case KeyEvent.KEYCODE_POWER:
            case KeyEvent.KEYCODE_SLEEP:
            case KeyEvent.KEYCODE_WAKEUP:
            case KeyEvent.KEYCODE_CTRL_LEFT:
            case KeyEvent.KEYCODE_CTRL_RIGHT:
            case KeyEvent.KEYCODE_ALT_LEFT:
            case KeyEvent.KEYCODE_ALT_RIGHT:
            case KeyEvent.KEYCODE_SHIFT_LEFT:
            case KeyEvent.KEYCODE_SHIFT_RIGHT: return true;
        }
        return false;
    }

    private boolean dontHandleKeyFramework(int keyCode) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_MEDIA_NEXT:
            case KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE:
            case KeyEvent.KEYCODE_MEDIA_PREVIOUS: return true;
        }
        return false;
    }
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        Log.e(mTAG, "onKeyDown() called with keycode: " + event);
        if (mCaptureEvents && !dropKey(keyCode)) {
            try {
                int ret = mWfdSession.sendEvent(event);
                if (ret != 0) {
                    Log.e(mTAG, "Error: " + WfdEnums.ErrorType.getValue(ret));
                }
            } catch (RemoteException e) {
                Log.e(mTAG, "RemoteException", e);
            }
        }
        //allow other key receivers to handle the event
        //based on the keycode
        return dontHandleKeyFramework(keyCode);
    }

    /**
     * Function to start capturing UIBC event.  This function can only be called on WFD sink.
     * @return
     */
    public boolean startUIBCEventCapture() {
        mCaptureEvents = true;
        return true;
    }

    /**
     * Function to stop capturing UIBC event.  This function can only be called on WFD sink.
     * @return
     */
    public boolean stopUIBCEventCapture() {
        mCaptureEvents = false;
        return true;
    }
    private boolean handleUIBCRotation(int UIBCRotId, int angle) {
        Bundle cfgItem = new Bundle();
        try {
            mWfdSession.getConfigItems(cfgItem);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        // Rotation should be triggered only if UIBC is enabled
        int[] configArr = new int[WfdEnums.ConfigKeys.TOTAL_CFG_KEYS.ordinal()];
        configArr = cfgItem.getIntArray(WfdEnums.CONFIG_BUNDLE_KEY);
        if (configArr != null
                && 1 == configArr[WfdEnums.ConfigKeys.UIBC_VALID.ordinal()]) {
            int ret = -1;
            try {
                ret = mWfdSession.uibcRotate(angle);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
            String res = "failed!";
            if (ret == 0) {
                res = "Successful!";
            }
            Toast.makeText(getContext(),
                    "UIBC rotation with " + Integer.toString(angle) + " degrees " + res,
                    Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(getContext(), "UIBC not enabled!",
                    Toast.LENGTH_SHORT).show();
            return false;
        }
        if(mActionMode != null) {
            mActionMode.finish();
        }
        mLastUIBCRotId = UIBCRotId;
        return true;
    }

    private boolean setDecoderLatency(int decodeLatId,int latency) {
        int ret = -1;
        try {
            ret = mWfdSession.setDecoderLatency(latency);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        String res = "failed!";
        if (ret == 0) {
            res = "Successful!";
        }
        Toast.makeText(getContext(),
                "Decoder latency value " + Integer.toString(latency) + " msec " + res,
                Toast.LENGTH_SHORT).show();
        if(ret == 0) {
            mLastDecoderLatencyId = decodeLatId;
        }
        return ret==0;
    }

    private boolean mSessionAudioActive = true;
    private boolean mSessionPlaying;
    private ActionMode.Callback mActionModeCallback = new ActionMode.Callback() {

        // Called when the action mode is created; startActionMode() was called
        @Override
        public boolean onCreateActionMode(ActionMode mode, Menu menu) {
            // Inflate a menu resource providing context menu items
            MenuInflater inflater = mode.getMenuInflater();
            inflater.inflate(R.menu.surface_action_menu, menu);
            return true;
        }

        // Called each time the action mode is shown. Always called after onCreateActionMode, but
        // may be called multiple times if the mode is invalidated.
        @Override
        public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
            MenuItem item;
            item = menu.findItem(R.id.menu_audioControl);
            if(mSessionAudioActive) {
                item.setIcon(android.R.drawable.ic_lock_silent_mode);
            } else {
                item.setIcon(android.R.drawable.ic_lock_silent_mode_off);
            }
            item = menu.findItem(R.id.menu_start_stop_UIBC);
            if(mCaptureEvents) {
                item.setIcon(android.R.drawable.ic_menu_revert);
            } else {
                item.setIcon(android.R.drawable.ic_menu_edit);
            }
            item = menu.findItem(R.id.menu_play_pause);
            if(mSessionPlaying) {
                item.setIcon(android.R.drawable.ic_media_pause);
            } else {
                item.setIcon(android.R.drawable.ic_media_play);
            }
            if(mLastUIBCRotId > 0) {
                item = menu.findItem(mLastUIBCRotId);
                item.setChecked(true);
            }
            if(mLastDecoderLatencyId > 0) {
                item = menu.findItem(mLastDecoderLatencyId);
                item.setChecked(true);
            }
            return true; // Return false if nothing is done
        }

        // Called when the user selects a contextual menu item
        @Override
        public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
            try {
                int itemId = item.getItemId();
                switch(itemId) {
                    case R.id.menu_play_pause :
                        if(mSessionPlaying) {
                            startWfdOperationTask(getContext(), WfdOperation.PAUSE,mWfdSession);
                        } else {
                            startWfdOperationTask(getContext(), WfdOperation.PLAY,mWfdSession);
                        }
                        break;
                    case R.id.menu_start_stop_UIBC:
                        if(mCaptureEvents) {
                            startWfdOperationTask(getContext(), WfdOperation.STOP_UIBC,mWfdSession);
                        } else {
                            startWfdOperationTask(getContext(),WfdOperation.START_UIBC,mWfdSession);
                        }
                        break;
                    case R.id.menu_teardown:
                        startWfdOperationTask(getContext(), WfdOperation.TEARDOWN,mWfdSession);
                        break;
                    case R.id.menu_audioControl:
                        if(mSessionAudioActive) {
                            if (0 == mWfdSession.execRuntimeCommand(
                                    WfdEnums.RuntimecmdType.DISABLE_AUDIO.getCmdType())) {
                                mSessionAudioActive = false;
                            }
                        } else {
                            if (0 == mWfdSession.execRuntimeCommand(
                                    WfdEnums.RuntimecmdType.ENABLE_AUDIO.getCmdType())) {
                                mSessionAudioActive = true;
                            }
                        }
                        mode.finish();
                        break;
                    case R.id.UIBC_rotate_0:
                        return handleUIBCRotation(itemId,0);
                    case R.id.UIBC_rotate_90:
                        return handleUIBCRotation(itemId,90);
                    case R.id.UIBC_rotate_180:
                        return handleUIBCRotation(itemId,180);
                    case R.id.UIBC_rotate_270:
                        return handleUIBCRotation(itemId,270);
                    case R.id.Decoder_Latency_0:
                        return setDecoderLatency(itemId,0);
                    case R.id.Decoder_Latency_50:
                        return setDecoderLatency(itemId,50);
                    case R.id.Decoder_Latency_100:
                        return setDecoderLatency(itemId,100);
                    case R.id.Decoder_Latency_150:
                        return setDecoderLatency(itemId,150);
                    case R.id.Decoder_Latency_200:
                        return setDecoderLatency(itemId,200);
                    case R.id.Decoder_Latency_250:
                        return setDecoderLatency(itemId,250);
                    default:
                        return false;
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            } finally {
                return false;
            }
        }

        // Called when the user exits the action mode
        @Override
        public void onDestroyActionMode(ActionMode mode) {
            mActionMode = null;
        }
    };

    class WfdSurfaceEventHandler extends Handler {

        @Override
        public void handleMessage(Message msg) {
            Log.d(mTAG, "WfdSurfaceEventHandler received: " + msgCodeToString(msg));

            if (WfdClientUtils.wfdOperationProgressDialog != null) {
                WfdClientUtils.wfdOperationProgressDialog.dismiss();
            }
            if (mActionMode != null) {
                mActionMode.finish();
            }
            switch (msg.what) {

                case PLAY_CALLBACK: {
                    if (startSessionProgressDialog != null) {
                        startSessionProgressDialog.dismiss();
                    }
                    mSessionPlaying = true;
                }
                break;
                case PAUSE_CALLBACK: {
                    mSessionPlaying = false;
                }
                break;
                case STANDBY_CALLBACK: {
                    mSessionPlaying = false;
                }
                break;
                case UIBC_ACTION_COMPLETED: {
                    if (WfdClientUtils.UIBC_DISABLED == msg.arg1) {
                        mCaptureEvents = false;
                    } else if (WfdClientUtils.UIBC_ENABLED == msg.arg1) {
                        mCaptureEvents = true;
                    }
                }
                break;
                case CLEAR_UI:
                    //fall through to TEARDOWN_CALLBACK case for normal teardown cleanup
                case TEARDOWN_CALLBACK: {
                    try {
                        mWfdSession.deinit();
                        if(mListener != null) {
                            Bundle b = new Bundle();
                            b.putBinder(WFD_SESSION_BINDER_KEY,mWfdSession.asBinder());
                            mListener.WfdSurfaceNotify(mId,TEARDOWN_CALLBACK,b);
                        }
                    } catch (RemoteException e) {
                        Log.e(mTAG, "Remote exception when calling deinit()", e);
                    }
                }
                break;
                case AUDIO_ONLY_NOTIFICATION: {
                    bDisablePauseOnSurfaceDestroyed = true;
                }
                break;
                default:
                    Log.e(mTAG, "WfdSurfaceEventHandler: Unknown event received: " + msg.what);
            }
        }
    }

    public interface WfdSurfaceListener {
        int WfdSurfaceNotify(int Id, int event, Bundle b);
    }
}