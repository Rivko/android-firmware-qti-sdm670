/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.content.Intent;
import android.widget.EditText;
import android.view.View;

public class MainActivity extends Activity {
	private EditText serverIPAddr;
	private ServerAPI mServerApi = null;
	private static String TAG = "MainActivity";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		serverIPAddr = (EditText) findViewById(R.id.insertServerIP);
		mServerApi = ServerAPI.getInstance(getBaseContext());

		serverIPAddr.setHint(mServerApi.getServerIP());
		serverIPAddr.setText(mServerApi.getServerIP());
	}

	/** Turn on SafeSwitch by creating a new account **/
	public void turnOnClick(View view){
		mServerApi.setServerIP(serverIPAddr.getText().toString());
		Log.d(TAG, "turnOnClick: "+serverIPAddr.getText().toString());
		Intent intent = new Intent(this, SignInActivity.class);
		startActivity(intent);
    }

	/** log on to SafeSwitch with an existing account **/
	public void loginOnClick(View view){
		mServerApi.setServerIP(serverIPAddr.getText().toString());
		Log.d(TAG, "loginOnClick: "+serverIPAddr.getText().toString());
		Intent intent = new Intent(this, LoginActivity.class);
		startActivity(intent);
	}

}
