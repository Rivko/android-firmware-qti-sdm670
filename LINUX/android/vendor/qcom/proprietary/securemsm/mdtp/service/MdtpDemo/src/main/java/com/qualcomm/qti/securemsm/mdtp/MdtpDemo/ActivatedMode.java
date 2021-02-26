/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

public class ActivatedMode extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_activated_mode);
	}

	public void deactivateOnClick(View view){
		Intent launchNextActivity = new Intent(ActivatedMode.this, LoginActivity.class);
		launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
		startActivity(launchNextActivity);
	}

}
