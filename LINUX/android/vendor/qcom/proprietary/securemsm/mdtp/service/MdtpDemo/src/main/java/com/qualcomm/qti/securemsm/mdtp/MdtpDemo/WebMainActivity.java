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
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;


public class WebMainActivity extends Activity {
	private static String TAG = "WebMainActivity";

	private UserRemoteTask mAuthTask;
	private Button lockButton;
	private Button unlockButton;
	private Button showPinButton;
	private Button showHWPinButton;
	private Button refreshButton;
	private static TextView device_status;
	private static TextView mdtp_status;
	private static TextView isv_device_id;
	private static TextView isv_name;
	private static TextView locking_status;
	private static TextView service_addr;
	private static TextView server_ip;
	private View mProgressView;

	private MdtpStateDemo deviceStatus = null;
	private boolean pendingLock = false;
	private boolean pendingUnlock = false;

	private WebServerAPI mServerApi = null;
	private boolean lockRequset = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_web_main);

		Log.d(TAG, "onCreate");
		initActivityViews();
		mServerApi = WebServerAPI.getInstance(getBaseContext());

		deviceStatus = mServerApi.getStatus();
		pendingLock = mServerApi.getLockingStatus();
		pendingUnlock = mServerApi.getUnlockingStatus();

		publishSystemState();
		publishAppState();
		publishButtonState();
		publishIsvInfo();

		try {
			service_addr.setText(mServerApi.getLocalIpAddress());
		} catch (Exception e) {
			e.printStackTrace();
			throw new RuntimeException();
		}

	}

	public void lockOnClick(View view){
		Log.d(TAG, "lockOnClick");
		if(deviceStatus.mAppState.mEmergencyOnly){
			Toast.makeText(this, getString(R.string.already_locked), Toast.LENGTH_LONG).show();
			return;
		}
		if(deviceStatus.mSystemState != MdtpStateDemo.MdtpSystemStateDemo.ACTIVE){
			Toast.makeText(this, getString(R.string.mdtp_off_cannot_lock), Toast.LENGTH_LONG).show();
			return;
		}
		lockRequset = true;
		showProgress(true);
		mAuthTask = new UserRemoteTask();
		mAuthTask.execute((Void) null);
	}

	public void unlockOnClick(View view){
		Log.d(TAG, "unlockOnClick");
		if(!deviceStatus.mAppState.mEmergencyOnly){
			Toast.makeText(this, getString(R.string.already_unlocked), Toast.LENGTH_LONG).show();
			return;
		}
		if(deviceStatus.mSystemState != MdtpStateDemo.MdtpSystemStateDemo.ACTIVE){
			Toast.makeText(this, getString(R.string.mdtp_off_cannot_unlock), Toast.LENGTH_LONG).show();
			return;
		}
		lockRequset = false;
		showProgress(true);
		mAuthTask = new UserRemoteTask();
		mAuthTask.execute((Void) null);
	}

	public void showPinOnClick(View view){
		Log.d(TAG, "showPinOnClick");
		if(deviceStatus.mSystemState == MdtpStateDemo.MdtpSystemStateDemo.ACTIVE)
			Toast.makeText(this, "SafeSwitch Deactivation PIN is: " + mServerApi.getMdtpPin(), Toast.LENGTH_LONG).show();
		else
			Toast.makeText(this, getString(R.string.mdtp_off), Toast.LENGTH_SHORT).show();
	}

	public void showHWRecoveryPinOnClick(View view){
		Log.d(TAG, "showHWRecoveryPinOnClick");
		if(deviceStatus.mSystemState == MdtpStateDemo.MdtpSystemStateDemo.ACTIVE)
			Toast.makeText(this, "SafeSwitch HW Recovery PIN is: " + mServerApi.getMdtpHWRecoveryPin(), Toast.LENGTH_LONG).show();
		else
			Toast.makeText(this, getString(R.string.mdtp_off), Toast.LENGTH_SHORT).show();
	}

	public void refreshOnClick(View view){
		Log.d(TAG, "refreshOnClick");

		Runnable r = new Runnable(){
		@Override
		public void run() {
			MdtpMessage MsgHeader = new MdtpMessage("UpdateStatusRequest", "WebInterface", "Server");
			MdtpMessage.UpdateStatusRequest updateStatusRequestMsg = MsgHeader.new UpdateStatusRequest("WebInterface");
			while(mServerApi.communicateServer(MsgHeader, updateStatusRequestMsg).equals("Network Error")){
				try {
					Thread.sleep(200);
				} catch (InterruptedException e) {
					throw new RuntimeException();
				}
			}
		}

		};
		Thread refreshThread = new Thread(r);
		refreshThread.start();
		try {
		refreshThread.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		deviceStatus = mServerApi.getStatus();
		pendingLock = mServerApi.getLockingStatus();
		pendingUnlock = mServerApi.getUnlockingStatus();

		publishSystemState();
		publishAppState();
		publishButtonState();
		publishIsvInfo();

		Toast.makeText(this, getString(R.string.refresh_web), Toast.LENGTH_SHORT).show();
	}

	private void publishSystemState(){
		if(deviceStatus.mSystemState == MdtpStateDemo.MdtpSystemStateDemo.ACTIVE){
			mdtp_status.setText(R.string.web_mdtp_status_on);
		}else if(deviceStatus.mSystemState == MdtpStateDemo.MdtpSystemStateDemo.INACTIVE){
			mdtp_status.setText(R.string.web_mdtp_status_off);
		}else if(deviceStatus.mSystemState == MdtpStateDemo.MdtpSystemStateDemo.TAMPERED){
			mdtp_status.setText(R.string.web_mdtp_status_tampered);
		}else if(deviceStatus.mSystemState == MdtpStateDemo.MdtpSystemStateDemo.DISABLED){
			mdtp_status.setText(R.string.web_mdtp_status_disabled);
		}else if(deviceStatus.mSystemState == MdtpStateDemo.MdtpSystemStateDemo.INVALID){
			mdtp_status.setText(R.string.web_mdtp_status_invalid);
		}
	}
	private void publishAppState(){
		if(deviceStatus.mAppState.mEmergencyOnly){
			device_status.setText(R.string.web_device_status_on);
		}else{
			device_status.setText(R.string.web_device_status_off);
		}
	}
	private void publishButtonState(){
		lockButton.setText(R.string.web_lock);
		unlockButton.setText(R.string.web_unlock);

		if(pendingLock){
			device_status.setText(R.string.web_device_status_locking);
			lockButton.setText(R.string.web_in_locking);
		}
		if(pendingUnlock){
			device_status.setText(R.string.web_device_status_unlocking);
			unlockButton.setText(R.string.web_in_unlocking);
		}
	}
	private void publishIsvInfo(){
		isv_device_id.setText(R.string.web_isv_device_id);
		isv_name.setText(R.string.web_isv_name);

		isv_device_id.append(mServerApi.getIsvDeviceID());
		isv_name.append(mServerApi.getIsvFreindlyName());
	}

	private void initActivityViews(){
		lockButton = (Button) findViewById(R.id.web_lock_button);
		unlockButton = (Button) findViewById(R.id.web_unlock_button);
		showPinButton = (Button) findViewById(R.id.show_pin_button);
		showHWPinButton = (Button) findViewById(R.id.show_hw_pin_button);
		refreshButton = (Button) findViewById(R.id.web_refresh_button);
		device_status = (TextView) findViewById(R.id.web_device_Status);
		mdtp_status = (TextView) findViewById(R.id.web_mdtp_status);
		locking_status = (TextView) findViewById(R.id.web_task_desc);
		mProgressView =  findViewById(R.id.web_progress_bar);
		service_addr = (TextView) findViewById(R.id.server_addr);
		server_ip = (TextView) findViewById(R.id.server_ip);
		isv_device_id = (TextView) findViewById(R.id.web_isv_device_id);
		isv_name = (TextView) findViewById(R.id.web_isv_name);
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
		showProgressPerView(mProgressView, !show);
		showProgressPerView(locking_status, !show);
		showProgressPerView(device_status, show);
		showProgressPerView(mdtp_status, show);
		showProgressPerView(lockButton, show);
		showProgressPerView(unlockButton, show);
		showProgressPerView(service_addr, show);
		showProgressPerView(server_ip, show);
		showProgressPerView(showPinButton, show);
		showProgressPerView(showHWPinButton, show);
		showProgressPerView(refreshButton, show);
		showProgressPerView(isv_device_id, show);
		showProgressPerView(isv_name, show);
	}


	/**
	 * Represents an asynchronous lock(unlock) screen task.
	 */
	public class UserRemoteTask extends AsyncTask<Void, Void, Boolean> {

		private static final String TAG_LOGIN = "UserRemoteTask";
		private TextView status;

		UserRemoteTask() {}

		@Override
		protected Boolean doInBackground(Void... params) {
			status = (TextView) findViewById(R.id.web_task_desc);
			Log.d(TAG_LOGIN, "doInBackground started");
			runOnUiThread(new Runnable() {
				public void run() {
					status.setText(getString(R.string.web_connecting_server));
				}
			});
			try {
				/** Simulate network access. **/
				Thread.sleep(2000);
			} catch (InterruptedException e) {
				return false;
			}

			String retval;
			MdtpMessage MsgHeader;
			MdtpMessage.StopPendingRequests stopPendings;
			if(pendingLock || pendingUnlock){
				runOnUiThread(new Runnable() {
					public void run() {
						/** Update UI elements **/
						if(pendingLock) status.setText(getString(R.string.remote_lock_stop_proc));
						else if(pendingUnlock) status.setText(getString(R.string.remote_unlock_stop_proc));
					}
				});
				MsgHeader = new MdtpMessage("StopPendingRequests", "WebInterface", "Server");
				stopPendings = MsgHeader.new StopPendingRequests();
				while((retval = mServerApi.communicateServer(MsgHeader, stopPendings)).equals("Network Error")){
					try {
						Thread.sleep(200);
					} catch (InterruptedException e) {
						return false;
					}
				}
				if(retval.equals("Failure"))
					return false;

				return true;
			}

			runOnUiThread(new Runnable() {
				public void run() {
					/** Update UI elements **/
					if(lockRequset) status.setText(getString(R.string.remote_lock_proc));
					else status.setText(getString(R.string.remote_unlock_proc));
				}
			});

			try {
				/** Simulate network access. **/
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				return false;
			}

			if(lockRequset){
				MsgHeader = new MdtpMessage("RemoteLockRequest", "WebInterface", "Server");
				MdtpMessage.RemoteLockRequest lockMsg = MsgHeader.new RemoteLockRequest();
				while((retval = mServerApi.communicateServer(MsgHeader, lockMsg)).equals("Network Error")){
					try {
						Thread.sleep(200);
					} catch (InterruptedException e) {
						return false;
					}
				}
				if(retval.equals("Failure"))
					return false;
			}
			else{
				MsgHeader = new MdtpMessage("WebRestoreRequest", "WebInterface", "Server");
				MdtpMessage.WebRestoreRequest unlockMsg = MsgHeader.new WebRestoreRequest();
			while((retval = mServerApi.communicateServer(MsgHeader, unlockMsg)).equals("Network Error")){
				try {
					Thread.sleep(200);
				} catch (InterruptedException e) {
					return false;
				}
			}
			if(retval.equals("Failure"))
				return false;
			}

			return true;
		}

		@Override
		protected void onPostExecute(final Boolean success) {
			Log.d(TAG_LOGIN, "onPostExecute started");
			mAuthTask = null;
			showProgress(false);
			if (success) {
				deviceStatus = mServerApi.getStatus();
				pendingLock = mServerApi.getLockingStatus();
				pendingUnlock = mServerApi.getUnlockingStatus();

				publishSystemState();
				publishAppState();
				publishButtonState();
				publishIsvInfo();
			}
		}

		@Override
		protected void onCancelled() {
			mAuthTask = null;
			showProgress(false);
		}

	}

}
