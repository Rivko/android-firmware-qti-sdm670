/*
 *  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.wfd.client;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Point;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.util.SparseArray;
import android.view.Display;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdStatus;
import com.qualcomm.wfd.client.WfdClientUtils.WfdOperation;
import com.qualcomm.wfd.client.WfdClientUtils.WfdOperationTask;
import com.qualcomm.wfd.service.IWfdActionListener;
import com.qualcomm.wfd.service.IWfdSession;

import static com.qualcomm.wfd.client.WfdClientUtils.*;

public class SurfaceActivity extends Activity implements WfdSurface.WfdSurfaceListener{

    public static final String TAG = "Client.SurfaceActivity";
    private SurfaceEventHandler mEventHandler;
    private LinearLayout mSurfaceActivityFrame;
    private Boolean modePlay = true;
    private Boolean modeStandby = false;
    private Boolean modeUibc = false;
    private Boolean modeDS = false;
    private MenuItem playPauseMenuItem;
    private MenuItem standbyResumeMenuItem;
    private MenuItem startStopUibcMenuItem;
    private MenuItem enableDisableDSMenuItem;
    private SparseArray<WfdSurface> mSurfaceArray;
    private WindowManager mWindowMgr;
    private boolean mInitDone;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "surfaceActivity onCreate called");
        super.onCreate(savedInstanceState);
        init();
        IWfdSession wfdSession = IWfdSession.Stub.asInterface(getIntent().getIBinderExtra(WFD_SESSION_BINDER_KEY));
        addSurface(wfdSession);
        Log.d(TAG, "surface instantiated");
        serviceUtilInitWrapper(wfdSession);
    }

    private void init() {
        if (!mInitDone) {
            requestWindowFeature(Window.FEATURE_NO_TITLE);
            setContentView(R.layout.surface_layout);
            mSurfaceActivityFrame = (LinearLayout) findViewById(R.id.surfaceFrame);
            mSurfaceArray = new SparseArray<WfdSurface>();
            mWindowMgr = ((WindowManager) this.getSystemService(Context.WINDOW_SERVICE));
            mEventHandler = new SurfaceEventHandler();
            Whisperer.getInstance().registerHandler(mEventHandler,SurfaceActivity.class.getName());
            mInitDone = true;
        }
    }
    /** To prevent dynamic configuration changes from destroying activity */
    @Override
    public void onConfigurationChanged (Configuration newConfig) {
        Log.e(TAG, "onConfigurationChanged called due to"+ newConfig.toString());
        super.onConfigurationChanged(newConfig);
    }

    @Override
    protected void onResume() {
        super.onResume();
        final View decorView = getWindow().getDecorView();
        decorView.setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
            @Override
            public void onSystemUiVisibilityChange(int visibility) {
                if((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
                    setUIVisibility(false);
                }
            }
        });
        setUIVisibility(false);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Whisperer.getInstance().unRegisterHandler(SurfaceActivity.class.getName());
        mEventHandler = null;
        if(mSurfaceArray.size() != 0) {
            Log.wtf(TAG,"Surface Activity destroyed while surfaces exist");
        }
        mSurfaceArray = null;
        mSurfaceActivityFrame = null;
        mWindowMgr = null;
        Log.d(TAG, "onDestroy() called");
    }

    private void setUIVisibility(boolean showUI) {
        final View decorView = getWindow().getDecorView();
        if(!showUI) {
            int uiOptions = decorView.getSystemUiVisibility();
            uiOptions |= View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
            uiOptions |= View.SYSTEM_UI_FLAG_FULLSCREEN;
            uiOptions |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
            uiOptions|= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
            decorView.setSystemUiVisibility(uiOptions);
        }
    }

    private void serviceUtilInitWrapper(IWfdSession wfdSession) {
        IWfdActionListener mActionListener = WfdClientUtils.createActionListener(mEventHandler);

        try {
            int initReturn = wfdSession.registerListener(mActionListener);
            Log.d(TAG, "onCreate: init returned- " + initReturn);
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception", e);
        }
        Toast.makeText(getApplicationContext(), "WFD service connected", Toast.LENGTH_SHORT).show();
    }


    public void enableDisableDSSurfaceActivity(MenuItem i) {
        Log.d(TAG, "enableDisableDSSurfaceActivity() called");
        Log.d(TAG, "Enable/Disable DS button clicked");

        if(!ServiceUtil.getmServiceAlreadyBound()) {
            Log.e(TAG,"toggleDSModeSurfaceActivity() called when not in WFD Session!");
            Toast.makeText(getApplicationContext(), "Not in WFD Session!",
                Toast.LENGTH_SHORT).show();
            return;
        }

        try {
            WfdStatus wfdStatus = ServiceUtil.getInstance().getStatus();
            if(wfdStatus.state != WfdEnums.SessionState.PLAY.ordinal()) {
                Log.d(TAG, "Toggle DS not allowed in non-PLAY state");
                Toast.makeText(getApplicationContext(),
                    "Not honouring DirStr toggle in non-PLAY state",
                    Toast.LENGTH_SHORT).show();
                return;
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }

        int ret = -1;
        try {
            ret = ServiceUtil.getInstance().toggleDSMode(!modeDS);
        } catch (RemoteException e) {
            e.printStackTrace();
        }

        if(ret == 1) {
            if(!modeDS) {
                modeDS = true;
                enableDisableDSMenuItem.setTitle("Disable DS");
            } else {
                modeDS = false;
                enableDisableDSMenuItem.setTitle("Enable DS");
            }
        }
    }

    @Override
        public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            Log.e(TAG, "Back button pressed in Surface Activity");
            WfdStatus status = null;
            try {
                status = ServiceUtil.getInstance().getStatus();
            } catch (RemoteException e) {
                Log.e(TAG,"Remote Exception when retrieving status");
            }
            if (status.state == WfdEnums.SessionState.INVALID.ordinal()
             || status.state == WfdEnums.SessionState.INITIALIZED.ordinal()
             || status.state == WfdEnums.SessionState.IDLE.ordinal()) {
                Log.d(TAG, "No session in progress , call finish on Surface activity");
                setResult(RESULT_CANCELED);
                finish();
            } else {
                WfdOperationTask task = new WfdOperationTask(this, "Tearing down session", WfdOperation.TEARDOWN);
                task.execute();
            }
        }
        return false;
    }

    @Override
    public int WfdSurfaceNotify(int Id, int event, Bundle b) {
        if(event == TEARDOWN_CALLBACK) {
            Whisperer.getInstance().getHandler(WFDClientActivity.class.getName()).
                    obtainMessage(REMOVE_WFD_SESSION,b.getBinder(WFD_SESSION_BINDER_KEY)).sendToTarget();
            removeSurface(Id);
        }
        return 0;
    }

    /**
     * Class for internal event handling in SurfaceActivity. Must run on UI thread.
     */
    private class SurfaceEventHandler extends Handler {

        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "Event handler received: " + msg.what);

            switch (msg.what) {
                case 42:
                    Log.e(TAG,"Request for new surface received");
                    IWfdSession wfd = (IWfdSession) msg.obj;
                    addSurface(wfd);
                    break;
                default:
                    Log.e(TAG, "SurfaceEventHandler: Unknown event received: " + msg.what);
            }
        }
    }

    private void addSurface(IWfdSession wfd) {
        if(wfd!=null) {
            int currentSurfaceCount = mSurfaceArray.size();
            Display display = mWindowMgr.getDefaultDisplay();
            //Point realSize = new Point();
            Point size = new Point();
            //display.getRealSize(realSize);
            display.getSize(size);
            // First decide the layout parameters for each surface by considering the extra surface to be added
            LinearLayout.LayoutParams surfaceLayoutParam
                    = new LinearLayout.LayoutParams(size.x/(currentSurfaceCount + 1),
                    ViewGroup.LayoutParams.MATCH_PARENT);
            surfaceLayoutParam.gravity = Gravity.END;
            // Iterate over the existing surfaces and lay them out
            for(int i =0 ; i < currentSurfaceCount; i++) {
                WfdSurface s = mSurfaceArray.valueAt(i);
                //Get the cached frame layout from the surface view
                FrameLayout parentFrame = (FrameLayout) s.getParent();
                //Layout the container frame layout
                parentFrame.setLayoutParams(surfaceLayoutParam);
            }
            // Now create the new surface
            WfdSurface surface = new WfdSurface(this,wfd,this);
            // Add the newly created surface to the array
            mSurfaceArray.put(surface.getId(),surface);
            //Now create the frame layout that will hold the new surface
            FrameLayout surfaceContainer = new FrameLayout(this);
            // Add the frame layout to the parent container (a horizontal Linear Layout). Use the layout params just calculated
            mSurfaceActivityFrame.addView(surfaceContainer, surfaceLayoutParam);
            // Add the newly created surface to this frame layout
            surfaceContainer.addView(surface);
            // Now create the navigation button to overlay this surface view
            ImageButton ib= new ImageButton(this);
            //Cache the surface in this button for later access
            ib.setTag(surface);
            //Set the long click listener to invoke the surface's action mode
            ib.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    WfdSurface s = (WfdSurface) (v.getTag());
                    s.handleSurfacePress(v);
                }
            });
            ib.setImageResource(android.R.drawable.ic_menu_compass);
            //Prepare the layout for the button to be added
            FrameLayout.LayoutParams buttonLayoutParams =
                new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT);
            // Add the button to the frame layout (since its a Frame Layout,
            // it will automatically overlay the surface if its added after it)
            surfaceContainer.addView(ib, buttonLayoutParams);
        } else {
            Log.wtf(TAG,"Null WFD session binder passed!");
        }
    }

        private void removeSurface(int id) {
        //We are removing a surface, so the new count is decremented
        Log.d(TAG,"Removing surface with Id: " + id);
        int newSurfaceCount = mSurfaceArray.size() - 1;
        Display display = mWindowMgr.getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        // Get the surface from the array and remove it
        WfdSurface surface = mSurfaceArray.get(id);
        mSurfaceArray.remove(id);
        // Get the frame layout container from the surface
        FrameLayout surfaceContainer = (FrameLayout) surface.getParent();
        // Iterate over the children to find the image button
        for(int i = 0; i< surfaceContainer.getChildCount();i++) {
            View v = surfaceContainer.getChildAt(i);
            if(v instanceof ImageButton) {
                ImageButton button = (ImageButton) v;
                //Lose the reference to the surface
                button.setTag(null);
            }
        }
        // Remove the frame layout from the parent container
        mSurfaceActivityFrame.removeView(surfaceContainer);
        if (newSurfaceCount != 0) {
            // Decide the layout parameters for each surface by considering the surface to be removed
            LinearLayout.LayoutParams layoutParamsControl
                    = new LinearLayout.LayoutParams(size.x/(newSurfaceCount),
                    ViewGroup.LayoutParams.MATCH_PARENT);
            layoutParamsControl.gravity = Gravity.END;
            // Iterate over the existing surfaces and lay them out
            for(int i =0 ; i < newSurfaceCount; i++) {
                WfdSurface s = mSurfaceArray.valueAt(i);
                FrameLayout parentContainer = (FrameLayout) s.getParent();
                parentContainer.setLayoutParams(layoutParamsControl);
            }
        } else {
            Log.d(TAG,"All surfaces are removed, finishing activity");
            finish();//All variables will be cleared in onDestroy() callback
        }
    }
}
