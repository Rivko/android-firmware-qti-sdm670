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
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.NotificationCompat;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.EditorInfo;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import android.app.admin.DevicePolicyManager;
import android.content.ComponentName;
import android.content.Context;



/**
 * A login screen that offers login via email/password.
 *
 */
public class WebLoginActivity extends Activity {

	/**
	 * Keep track of the login task to ensure we can cancel it if requested.
	 */
	private UserLoginTask mAuthTask = null;

	// UI references.
	private AutoCompleteTextView mEmailView;
	private EditText mPasswordView;
	private View mProgressView;
	private View mLoginFormView;
	private TextView mLoginText;

	NotificationCompat.Builder notification;
	private static final int uniqueID = 45612;

	private WebServerAPI mServerApi = null;
	boolean activityIsOn = false;
	private static final String TAG = "web_login";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_web_login);

		// Set up the login form.
		mEmailView = (AutoCompleteTextView) findViewById(R.id.web_email);
		mLoginText = (TextView) findViewById(R.id.web_login_status);
		mPasswordView = (EditText) findViewById(R.id.web_password);
		mPasswordView.setOnEditorActionListener(new TextView.OnEditorActionListener() {
			@Override
			public boolean onEditorAction(TextView textView, int id, KeyEvent keyEvent) {
				if (id == R.id.login || id == EditorInfo.IME_NULL) {
					attemptLogin();
					return true;
				}
				return false;
			}
		});

		Button mEmailSignInButton = (Button) findViewById(R.id.web_sign_in_button);
		if(mEmailSignInButton == null){
			Log.d(TAG, "onCreate shit");
		}
		mEmailSignInButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View view) {
				attemptLogin();
			}
		});
		Log.d(TAG, "onCreate");
		mLoginFormView = findViewById(R.id.login_form);
		mProgressView = findViewById(R.id.web_login_progress);

		mServerApi = WebServerAPI.getInstance(getBaseContext());
		activityIsOn = true;
		NotificationManager nm = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
		nm.cancel(uniqueID);
		notification = new NotificationCompat.Builder(this);
		notification.setAutoCancel(true);

		Log.d(TAG, "Web login activity started");
	}

	@Override
	protected void onStop() {
		super.onStop();
		activityIsOn = false; //User activity is not shown on screen anymore
	}

	@Override
	protected void onResume() {
		super.onResume();
		activityIsOn = true; //User activity is shown on screen
	}


	/**
	 * Attempts to sign in or register the account specified by the login form.
	 * If there are form errors (invalid email, missing fields, etc.), the
	 * errors are presented and no actual login attempt is made.
	 */
	public void attemptLogin() {
		if (mAuthTask != null) {
			return;
		}

		// Reset errors.
		mEmailView.setError(null);
		mPasswordView.setError(null);

		// Store values at the time of the login attempt.
		String email = mEmailView.getText().toString();
		String password = mPasswordView.getText().toString();

		boolean cancel = false;
		View focusView = null;

		// Check for a valid password, if the user entered one.
		if (!TextUtils.isEmpty(password) && !isPasswordValid(password)) {
			mPasswordView.setError(getString(R.string.error_invalid_password));
			focusView = mPasswordView;
			cancel = true;
		}

		// Check for a valid email address.
		if (TextUtils.isEmpty(email)) {
			mEmailView.setError(getString(R.string.error_field_required));
			focusView = mEmailView;
			cancel = true;
		} else if (!isEmailValid(email)) {
			mEmailView.setError(getString(R.string.error_invalid_email));
			focusView = mEmailView;
			cancel = true;
		}

		if (cancel) {
			// There was an error; don't attempt login and focus the first
			// form field with an error.
			focusView.requestFocus();
		} else {
			// Show a progress spinner, and kick off a background task to
			// perform the user login attempt.
			showProgress(true);
			mAuthTask = new UserLoginTask(email, password);
			mAuthTask.execute((Void) null);
			Log.d(TAG, "Login attemptLogin finished");
		}
	}

	private boolean isEmailValid(String email) {
		return email.contains("@");
	}

	private boolean isPasswordValid(String password) {
		return password.length() > 4;
	}

	/**
	 * Shows the progress UI and hides the login form.
	 */
	@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
	public void showProgress(final boolean show) {
		// On Honeycomb MR2 we have the ViewPropertyAnimator APIs, which allow
		// for very easy animations. If available, use these APIs to fade-in
		// the progress spinner.
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR2) {
			int shortAnimTime = getResources().getInteger(android.R.integer.config_shortAnimTime);

			mLoginFormView.setVisibility(show ? View.GONE : View.VISIBLE);
			mLoginFormView.animate().setDuration(shortAnimTime).alpha(show ? 0 : 1)
					.setListener(new AnimatorListenerAdapter() {
						@Override
						public void onAnimationEnd(Animator animation) {
							mLoginFormView.setVisibility(show ? View.GONE : View.VISIBLE);
						}
					});

			mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
			mProgressView.animate().setDuration(shortAnimTime).alpha(show ? 1 : 0)
					.setListener(new AnimatorListenerAdapter() {
						@Override
						public void onAnimationEnd(Animator animation) {
							mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
						}
					});
			mLoginText.setVisibility(show ? View.VISIBLE : View.GONE);
			mLoginText.animate().setDuration(shortAnimTime).alpha(show ? 1 : 0)
					.setListener(new AnimatorListenerAdapter() {
						@Override
						public void onAnimationEnd(Animator animation) {
							mLoginText.setVisibility(show ? View.VISIBLE : View.GONE);
						}
					});
		} else {
			// The ViewPropertyAnimator APIs are not available, so simply show
			// and hide the relevant UI components.
			mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
			mLoginFormView.setVisibility(show ? View.GONE : View.VISIBLE);
			mLoginText.setVisibility(show ? View.VISIBLE : View.GONE);
		}
	}


	/**
	 * Represents an asynchronous login/registration task used to authenticate
	 * the user.
	 */
	public class UserLoginTask extends AsyncTask<Void, Void, Boolean> {

		private final String mEmail;
		private final String mPassword;
		private static final String TAG_LOGIN = "WebLoginTask";
		private TextView login;
		String retval;

		UserLoginTask(String email, String password) {
			mEmail = email;
			mPassword = password;
		}

		@Override
		protected Boolean doInBackground(Void... params) {
			Log.d(TAG_LOGIN, "doInBackground start");
			runOnUiThread(new Runnable() {
			    public void run() {
			        /** Update UI elements **/
					login = (TextView) findViewById(R.id.web_login_status);
					login.setText(getString(R.string.validation_proc));
			    }
			});

			try {
				/** Simulate network access. **/
				Thread.sleep(2000);
			} catch (InterruptedException e) {
				return false;
			}

			/** Connecting to server **/
			MdtpMessage MsgHeader = new MdtpMessage("WebLogInRequest", "WebInterface", "Server");
			MdtpMessage.WebLogInRequest logInMsg = MsgHeader.new WebLogInRequest(mEmail,mPassword);

			while((retval = mServerApi.communicateServer(MsgHeader, logInMsg)).equals("Network Error")){
				try {
					Thread.sleep(200);
				} catch (InterruptedException e) {
					return false;
				}
			}
			if(retval.equals("Failure"))
				return false;

			/** Load device status from server **/
			MsgHeader = new MdtpMessage("UpdateStatusRequest", "WebInterface", "Server");
			MdtpMessage.UpdateStatusRequest updateStatusRequestMsg = MsgHeader.new UpdateStatusRequest("WebInterface");
			while(mServerApi.communicateServer(MsgHeader, updateStatusRequestMsg).equals("Network Error")){
				try {
					Thread.sleep(200);
				} catch (InterruptedException e) {
					throw new RuntimeException();
				}
			}

			return true;
		}

		@Override
		protected void onPostExecute(final Boolean success) {
			mAuthTask = null;
			showProgress(false);
			Intent launchNextActivity = new Intent(WebLoginActivity.this, WebMainActivity.class);
			launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
			launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);

			if (success) {
				if(!activityIsOn){
					this.publishNotification("Web Interface Login Successfully", true);
					return;
				}
				Log.d(TAG_LOGIN, "launchNextActivity");
				startActivity(launchNextActivity);
			} else {
				if(!activityIsOn){
					this.publishNotification("Web Interface Login Failed", false);
					return;
				}
				mPasswordView.setError(getString(R.string.error_incorrect_password));
				mPasswordView.requestFocus();
			}

		}

		@Override
		protected void onCancelled() {
			mAuthTask = null;
			showProgress(false);
		}

		private void publishNotification(CharSequence res, boolean state){
			Log.d(TAG, "publishNotification");
			notification.setSmallIcon(R.drawable.qualcomm_icon_clean);
			notification.setContentTitle("SafeSwitch Demo App");
			notification.setContentText(res);
			Intent intent;
			if(state){
				intent = new Intent(getBaseContext(), WebMainActivity.class);
			} else{
				intent = new Intent(getBaseContext(), LoginActivity.class);
			}

			PendingIntent pendingIntent = PendingIntent.getActivity(getBaseContext(), 0 , intent, PendingIntent.FLAG_UPDATE_CURRENT);
			notification.setContentIntent(pendingIntent);

			NotificationManager nm = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
			nm.notify(uniqueID, notification.build());
		}

	}

	public void startServerOnClick(View view){
		Log.d(TAG, "startServerOnClick");
		/** Start Server **/
		mServerApi.setLoadServerOnBoot(true);
		Intent serviceIntent = new Intent(this, ServerService.class);
		startService(serviceIntent);
	}

	public void stopServerOnClick(View view){
		Log.d(TAG, "stopServerOnClick");
		/** Stop Server **/
		mServerApi.setLoadServerOnBoot(false);
		Intent serviceIntent = new Intent(this, ServerService.class);
		stopService(serviceIntent);
	}

}
