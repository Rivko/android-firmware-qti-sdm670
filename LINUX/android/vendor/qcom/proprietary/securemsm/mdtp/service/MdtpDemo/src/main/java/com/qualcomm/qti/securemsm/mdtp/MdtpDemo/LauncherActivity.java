/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.content.Context;
import android.view.Window;
import android.content.pm.PackageManager;
import android.content.ComponentName;

import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;

public class LauncherActivity extends Activity {

	private static final int SPLASH_DISPLAY_TIME = 2000; // splash screen delay time
	private static final String SPLASH_TAG = "LauncherActivity";

	private ServerAPI mServerApi = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_launcher);
        Log.d(SPLASH_TAG, "onCreate");
        mServerApi = ServerAPI.getInstance(getBaseContext());
        Thread timerThread = new Thread(){
            public void run(){
                MdtpState state = new MdtpState();
                try {
                    /** Bound to Mdtp service **/
                    MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
                    if(!MdtpApiSingleton.getInstance().isInitialized())
                        MdtpApiSingleton.getInstance().start(getBaseContext());
                    if (!MdtpApiSingleton.getInstance().isBound())
                        MdtpApiSingleton.getInstance().bind(getBaseContext());
                    /** Check if application has successfully bound to MDTP service */
                    int i;
                    // wait for 50*100 (5 seconds) - service initialization can take time on boot
                    final int maxWaitIter = 50;
                    for (i = 0; (!MdtpApiSingleton.getInstance().isBound()) && (i < maxWaitIter); i++) {
                        Log.d(SPLASH_TAG, "Waiting for server bind...");
                        Thread.sleep(100);
                    }
                    /** not bound to service */
                    if (i == maxWaitIter) {
                        Log.d(SPLASH_TAG, "Timeout while binding to the service");
                        MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
                        throw new RuntimeException();
                    }
                    Log.d(SPLASH_TAG, "Succeffully binded to the service");


                    sleep(SPLASH_DISPLAY_TIME);
                    /** Check for Mdtp state to decide which screen to load **/
                    state = MdtpApiSingleton.getInstance().getState();
                    MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
                } catch (Exception e) {
                    Log.d(SPLASH_TAG, "Exception while accesing the service " + e.toString());
                    throw new RuntimeException();
                } finally{
                    Intent launchNextActivity;
                    if(state.mSystemState == MdtpSystemState.ACTIVE){
                        Log.d(SPLASH_TAG, "state.mSystemState == MdtpSystemState.ACTIVE");
                        launchNextActivity = new Intent(LauncherActivity.this, ActivatedMode.class);
                    } else{
                        Log.d(SPLASH_TAG, "state.mSystemState != MdtpSystemState.ACTIVE");
                        launchNextActivity = new Intent(LauncherActivity.this, MainActivity.class);
                    }
                    launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
                    startActivity(launchNextActivity);
                }
                finish();
            }
        };

		timerThread.start();
	}

}
