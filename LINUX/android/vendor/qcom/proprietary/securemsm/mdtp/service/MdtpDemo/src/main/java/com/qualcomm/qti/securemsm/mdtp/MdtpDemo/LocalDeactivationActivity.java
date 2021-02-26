/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Build;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;
import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.util.Log;
import android.widget.TextView;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;

public class LocalDeactivationActivity extends Activity {

	private static final String TAG = "LocalDeactivationActivity";
	private ServerAPI mServerApi = null;
	private EditText deactivatePin;
	private TextView emergencyBackButton;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_local_deactivation);
		deactivatePin = (EditText) findViewById(R.id.local_deac_pin);
		emergencyBackButton = (TextView) findViewById(R.id.local_deac_back_button);
		mServerApi = ServerAPI.getInstance(getBaseContext());

		MdtpStateDemo state = mServerApi.getDeviceState();
		if (state.mAppState.mEmergencyOnly){
			emergencyBackButton.setVisibility(View.VISIBLE);
		}else{
			emergencyBackButton.setVisibility(View.GONE);
		}
	}

	/** Performs local deactivation **/
	public void localDeactivateOnClick(View view){
		String pin = deactivatePin.getText().toString();
		Toast.makeText(this, "Please wait for a few secondes.", Toast.LENGTH_SHORT).show();
		/** A backgorund thread that tries to update server about Mdtp status change.
		Will keep running until it succeed to connect to the server	**/
		Runnable r = new Runnable(){
			@Override
			public void run() {
				MdtpMessage MsgHeader = new MdtpMessage("UpdateStatus", "Client", "Server");
				MdtpMessage.UpdateStatus updateStatus = MsgHeader.new UpdateStatus("Client", "Server");
				MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
				MdtpState state = MdtpApiSingleton.getInstance().getState();
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				updateStatus.setStatus(new MdtpStateDemo(state));
				String retval;

				/** Try to reach server every 10 secs **/
				do {
					retval = mServerApi.sendMessageToServer(MsgHeader, updateStatus, 60000);
					try {
						Thread.sleep(10000);
					} catch (InterruptedException e) {
						continue;
					}
				} while (retval.equals("Network Error"));
			}
		};
		/** Local deactivation **/
		MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
		MdtpApiSingleton.getInstance().deactivateLocal(pin);
		MdtpState state = MdtpApiSingleton.getInstance().getState();
		MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
		if (state.mSystemState == MdtpSystemState.ACTIVE){
			Toast.makeText(this, "Wrong deactivation PIN", Toast.LENGTH_LONG).show();
			return;
		}
		mServerApi.setDeviceState(new MdtpStateDemo(state));
		/** Update server about Mdtp status change if local deactivation succeed **/
		Thread updateServerThread = new Thread(r);
		updateServerThread.start();

		Intent launchNextActivity = new Intent(LocalDeactivationActivity.this, MainActivity.class);
		launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
		launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
		startActivity(launchNextActivity);

		/** Stop Poller service when Mdtp is deactivated **/
		Intent clientPollerIntent = new Intent(this, ClientService.class);
		this.stopService(clientPollerIntent);

		/** Unlock device if needed **/
		DeviceLockUtil mDeviceLockUtil;
		mDeviceLockUtil = DeviceLockUtil.getInstance(getBaseContext());
		mDeviceLockUtil.unlockDevice();

	}

	/** Back button when the device is locked **/
	public void onEmergencyBackPressed(View v) {
		Log.d(TAG,"LocalDeactivationActivity mdtp - STATE_E911_MODE");
		Intent launchNextActivity = new Intent(LocalDeactivationActivity.this, LockScreenActivity.class);
		launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
		startActivity(launchNextActivity);
	}

}
