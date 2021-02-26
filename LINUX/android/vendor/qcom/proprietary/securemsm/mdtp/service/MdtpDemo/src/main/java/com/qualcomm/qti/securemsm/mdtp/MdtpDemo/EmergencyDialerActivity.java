/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.content.Intent;
import android.net.Uri;

public class EmergencyDialerActivity extends Activity {

	private String dialedNumber;
	TextView dialedNumberOnScreen;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_emergency_dialer);

		dialedNumberOnScreen = (TextView)findViewById(R.id.dial_number);
		dialedNumber = null;
	}

	public void digitOneOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "1";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "1";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitTwoOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "2";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "2";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitThreeOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "3";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "3";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitFourOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "4";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "4";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitFiveOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "5";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "5";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitSixOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "6";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "6";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitSevenOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "7";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "7";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitEightOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "8";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "8";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitNineOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "9";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "9";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitZeroOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "0";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "0";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitStarOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "*";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "*";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}
	public void digitSulamitOnClick(View v){
		if(dialedNumber == null){
			dialedNumber = "#";
		}else if(dialedNumber.length() < 10){
			dialedNumber += "#";
		}
		dialedNumberOnScreen.setText(dialedNumber);
	}

	public void callOnClick(View v){
		Uri callUri = Uri.parse("tel://" + dialedNumber);
		Intent dialIntent = new Intent(Intent.ACTION_CALL,callUri);
		dialIntent.setFlags(Intent.FLAG_ACTIVITY_NO_USER_ACTION);
		startActivity(dialIntent);
		dialedNumber = null;
	}
	public void backOnClick(View v){
		//go back to main lock screen activity
		Intent intent = new Intent(EmergencyDialerActivity.this, LockScreenActivity.class);
		startActivity(intent);
	}
	public void deleteOnClick(View v){
		//go back to main lock screen activit
		String newString = null;
		if((dialedNumber != null) && (dialedNumber.length() > 1)){
			newString = dialedNumber.substring(0, dialedNumber.length()-1);
		}
		dialedNumber = newString;
		dialedNumberOnScreen.setText(dialedNumber);
	}

}
