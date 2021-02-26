/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.net.Uri;
import android.content.pm.PackageManager;
import android.content.ComponentName;
import android.app.KeyguardManager;
import android.app.StatusBarManager;
import android.content.Context;
import android.provider.Settings;
import java.lang.System;
import android.os.UserHandle;
import com.android.internal.widget.LockPatternUtils;
import android.app.admin.DevicePolicyManager;
import com.android.server.devicepolicy.DevicePolicyManagerService;
import android.app.admin.DeviceAdminReceiver;
import android.app.IActivityManager;
import android.app.ActivityManagerNative;
import java.util.ArrayList;
import java.util.List;
import android.content.IntentFilter;

import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;


public class LockScreenActivity extends Activity {

	private static String TAG = "LockScreenActivity";

	private View mProgressView;
	private Button unlockButton;
	private Button emergencyCallButton;
	private TextView unlockScreenDesc;
	private EditText unlockUsername;
	private EditText unlockPassword;

	private static String deviceIMEI = "AM29DN0Q2LV95NF";

	private String username;
	private String password;

	private ServerAPI mServerApi;
	private UnlockTask mAuthTask;
	private DeviceLockUtil mDeviceLockUtil;
	private ActivityManager mActivityManager;
	private DevicePolicyManager mDevicePolicyManager;
	private BoundToService mAuthTaskBoundService;

/*********************************************************/

	/** Offload MDTP services to a non-UI thread **/
	private class BoundToService extends AsyncTask<Void, Void, Boolean> {

		private Context mBindContext;

		public BoundToService(Context context) {
			mBindContext = context;
		}

		protected Boolean doInBackground(Void... params) {
			Log.d(TAG, "MDTP: LockScreen - doInBackground start");
			try {
				MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
				if (!MdtpApiSingleton.getInstance().isInitialized())
					MdtpApiSingleton.getInstance().start(getBaseContext());

				if (!MdtpApiSingleton.getInstance().isBound())
					MdtpApiSingleton.getInstance().bind(getBaseContext());
				/** Check if application has successfully bound to MDTP service */
				int i;
				// wait for 50*100 (5 seconds) - service initialization can take time on boot
				final int maxWaitIter = 50;
				for (i = 0; (!MdtpApiSingleton.getInstance().isBound()) && (i < maxWaitIter); i++) {
					Log.d(TAG, "MDTP: LockScreen - Waiting for server bind...");
					Thread.sleep(100);
				}
				/** not bound to service */
				if (i == maxWaitIter) {
					Log.d(TAG, "MDTP: LockScreen -Timeout while binding to the service");
					MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
					return false;
				}
				Log.d(TAG, "MDTP: LockScreen - Successfully binded to the service");

			} catch (Exception e) {
				Log.d(TAG, "MDTP: LockScreen - Exception while accesing the service " + e.toString());
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				return false;
			}
			MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
			return true;
		}

		protected void onPostExecute(Boolean sts) {
			Log.d(TAG, "MDTP: LockScreen - onPostExecute - mdtp bounded: " + sts);
			if (sts == false)
				throw new RuntimeException();

			MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
			MdtpState deviceState = MdtpApiSingleton.getInstance().getState();
			if(deviceState.mAppState.mEmergencyOnly){
				Log.d(TAG, "MDTP: LockScreen - Locked");
				mDeviceLockUtil.lockDevice();
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
			} else{
				Log.d(TAG, "MDTP: LockScreen - Unlocked");
				mDeviceLockUtil.unlockDevice();
				mServerApi.setDeviceState(new MdtpStateDemo(deviceState));
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				finish();
			}

			int state = mActivityManager.getLockTaskModeState();
			switch(state){
				case 0: Log.d(TAG,"doInBackground mdtp getLockTaskModeState NONE");
						break;
				case 1: Log.d(TAG,"doInBackground mdtp getLockTaskModeState LOCKED");
						break;
				case 2: Log.d(TAG,"doInBackground mdtp getLockTaskModeState PINNED");
						break;
				default: Log.d(TAG,"doInBackground mdtp getLockTaskModeState defualt");
						break;
			}
			Log.d(TAG, "MDTP: onPostExecute - done");
		}

	}

	/***********************************************************************/


	@SuppressWarnings("deprecation")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.d(TAG,"onCreate");
		setContentView(R.layout.activity_lock_screen);
		initActivityViews();

		mServerApi = ServerAPI.getInstance(getBaseContext());
		mDeviceLockUtil = DeviceLockUtil.getInstance(getBaseContext());
		mActivityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
		mDevicePolicyManager = (DevicePolicyManager) getSystemService(Context.DEVICE_POLICY_SERVICE);

		mAuthTaskBoundService = new BoundToService(getApplicationContext());
		mAuthTaskBoundService.execute((Void) null);
	}

	@Override
	protected void onStart() {
		super.onStart();
		// Start lock task mode if its not already active
		if(mDevicePolicyManager.isLockTaskPermitted(this.getPackageName())){
			Log.d(TAG, "MDTP: isLockTaskPermitted - yes");
			ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
			if (am.getLockTaskModeState() == ActivityManager.LOCK_TASK_MODE_NONE) {
				startLockTask();
			}
		}else{
			Log.d(TAG, "MDTP: isLockTaskPermitted - no");
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		getWindow().addFlags(
			WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
			| WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD
			| WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
			| WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON
			| WindowManager.LayoutParams.FLAG_SHOW_WALLPAPER);

		int state = mActivityManager.getLockTaskModeState();
		switch(state){
			case 0: Log.d(TAG,"onResume mdtp getLockTaskModeState NONE");
					break;
			case 1: Log.d(TAG,"onResume mdtp getLockTaskModeState LOCKED");
					break;
			case 2: Log.d(TAG,"onResume mdtp getLockTaskModeState PINNED");
					break;
			default: Log.d(TAG,"onResume mdtp getLockTaskModeState defualt");
					break;
		}
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		View decorView = getWindow().getDecorView();
		if (hasFocus) {
			decorView.setSystemUiVisibility(
				View.SYSTEM_UI_FLAG_LAYOUT_STABLE
				| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
				| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
				| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
				| View.SYSTEM_UI_FLAG_FULLSCREEN
				| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY );
		}
	}

	/** Init all activity's view components **/
	private void initActivityViews(){
		unlockButton = (Button) findViewById(R.id.lockscreen_unlock);
		emergencyCallButton = (Button) findViewById(R.id.emergency_call_button);
		mProgressView = findViewById(R.id.unlock_progressBar);
		unlockUsername = (EditText) findViewById(R.id.unlock_username);
		unlockPassword = (EditText) findViewById(R.id.unlock_password);
		unlockScreenDesc = (TextView) findViewById(R.id.unlock_desc);
		unlockScreenDesc.setText(R.string.lockscreen_desc);
	}

	/** Unlock screen button onClick **/
	public void unlockOnClick(View view){
		username = null;
		password = null;
		username = unlockUsername.getText().toString();
		password = unlockPassword.getText().toString();
		if(username.equals("") || password.equals("")) {
			Toast.makeText(this, "Please enter valid username and password.", Toast.LENGTH_LONG).show();
			return;
		}

		showProgress(true);
		mAuthTask = new UnlockTask();
		mAuthTask.execute((Void) null);
	}

	/** Emergency dialer button onClick **/
	public void emergencyCallClick(View view){
		Log.d(TAG,"mdtp emergencyCallClick");
		//Intent intent = new Intent("com.android.phone.EmergencyDialer.DIAL");
		Intent intent = new Intent(LockScreenActivity.this, EmergencyDialerActivity.class);
		startActivity(intent);
	}

	@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
	private void showProgressPerView(final View v, final boolean show){
		// On Honeycomb MR2 we have the ViewPropertyAnimator APIs, which allow
		// for very easy animations. If available, use these APIs to fade-in
		// the progress spinner.
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR2) {
			int shortAnimTime = getResources().getInteger(android.R.integer.config_shortAnimTime);
			v.setVisibility(show ? View.GONE : View.VISIBLE);
			v.animate().setDuration(shortAnimTime).alpha(show ? 0 : 1)
					.setListener(new AnimatorListenerAdapter() {
						@Override
						public void onAnimationEnd(Animator animation) {
							v.setVisibility(show ? View.GONE : View.VISIBLE);
						}
					});
		}else {
			// The ViewPropertyAnimator APIs are not available, so simply show
			// and hide the relevant UI components.
			v.setVisibility(show ? View.GONE : View.VISIBLE);
		}
	}

	/**
	* Shows the progress UI and hides the login form.
	*/
	@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
	public void showProgress(final boolean show) {
		showProgressPerView(unlockButton, show);
		showProgressPerView(emergencyCallButton, show);
		showProgressPerView(unlockUsername, show);
		showProgressPerView(unlockPassword, show);
		showProgressPerView(mProgressView, !show);

	}

	/**
	 * Represents an asynchronous unlock task.
	 */
	public class UnlockTask extends AsyncTask<Void, Void, Boolean> {

		private static final String TAG_LOGIN = "UnlockTask";
		private TextView status;
		private boolean networkFailure;

		UnlockTask() {}

		@Override
		protected Boolean doInBackground(Void... params) {
			String retval;

			MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
			networkFailure = false;
			runOnUiThread(new Runnable() {
				public void run() {
					/** Update UI elements **/
					status = (TextView) findViewById(R.id.unlock_desc);
					status.setText("Recovering Device...");
				}
			});

			try {
				Thread.sleep(2000);
			} catch (InterruptedException e) {
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				return false;
			}

			/** Request unlock approval**/
			MdtpMessage MsgHeader = new MdtpMessage("RemoteRestoreRequest", "Client", "Server");
			MdtpMessage.RemoteRestoreRequest unlockRequest = MsgHeader.new RemoteRestoreRequest(deviceIMEI, username, password);

			retval = mServerApi.sendMessageToServer(MsgHeader, unlockRequest, 60000);
			if(retval.equals("Network Error")){
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				networkFailure = true;
				return false;
			}
			if(!retval.equals("Success")){
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				return false;
			}

			/** Update server for new status**/
			MsgHeader = new MdtpMessage("UpdateStatus", "Client", "Server");
			MdtpMessage.UpdateStatus UpdateStatus = MsgHeader.new UpdateStatus("Client", "Server");
			UpdateStatus.setStatus(
				new MdtpStateDemo(MdtpApiSingleton.getInstance().getState())
				);

			retval = mServerApi.sendMessageToServer(MsgHeader, UpdateStatus, 60000);
			if(retval.equals("Network Error")){
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				networkFailure = true;
				return false;
			}
			if(!retval.equals("Success")){
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				return false;
			}
			MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
			return true;
		}

		@Override
		protected void onPostExecute(final Boolean success) {
			Log.d(TAG_LOGIN, "onPostExecute started");
			mAuthTask = null;
			showProgress(false);
			Intent launchNextActivity;
			Log.d(TAG_LOGIN, "mdtp onPostExecute unlock res = "+success);
			if (success) {
				MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
				mServerApi.setDeviceState(
					new MdtpStateDemo(MdtpApiSingleton.getInstance().getState())
				);
				getWindow().getDecorView().setSystemUiVisibility(0);
				mDeviceLockUtil.unlockDevice();
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				stopLockTask();
				finish();
			} else {
				runOnUiThread(new Runnable() {
					public void run() {
						/** Update UI elements **/
						status = (TextView) findViewById(R.id.unlock_desc);
						status.setText(R.string.lockscreen_desc);
					}
				});

				/** On network connection error **/
				if(networkFailure){
					launchNextActivity = new Intent(LockScreenActivity.this, LocalDeactivationActivity.class);
					launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
					startActivity(launchNextActivity);
				}
			}
		}

		@Override
		protected void onCancelled() {
			mAuthTask = null;
			showProgress(false);
		}

	}

}
