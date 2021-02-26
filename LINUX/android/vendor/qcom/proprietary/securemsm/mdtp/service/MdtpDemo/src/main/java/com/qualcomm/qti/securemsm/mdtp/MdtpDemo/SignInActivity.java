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
import android.app.admin.DevicePolicyManager;
import android.content.pm.PackageManager;
import android.content.ComponentName;
import android.content.Context;
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
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;

/**
 * A login screen that offers login via email/password.
 *
 */
public class SignInActivity extends Activity{
	/**
	 * Keep track of the login task to ensure we can cancel it if requested.
	 */
	private UserSignUpTask mAuthTask = null;
	private static String TAG = "SignUp";

	// UI references.
	private AutoCompleteTextView mEmailView;
	private EditText mPasswordView;
	private EditText mRepeatPasswordView;
	//private EditText mPasswordRepeatView;
	private View mProgressView;
	private View mLoginFormView;
	private TextView mProgressText;
	private CheckBox simlock_checkbox;
	Activity mActivity;

	NotificationCompat.Builder notification;
	private static final int uniqueID = 45612;
	public boolean activityIsOn = false;
	private boolean isOnWizard;
	private String mEmailAfterAdmin = null;
	private String mPasswordAfterAdmin = null;

	private ServerAPI mServerApi = null;
	final int ACTIVATION_REQUEST = 47;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_sign_inr);

		/** Set up the login form. **/
		mEmailView = (AutoCompleteTextView) findViewById(R.id.web_email);

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

		mRepeatPasswordView = (EditText) findViewById(R.id.password_repeat);
		mRepeatPasswordView.setOnEditorActionListener(new TextView.OnEditorActionListener() {
			@Override
			public boolean onEditorAction(TextView textView, int id, KeyEvent keyEvent) {
				if (id == R.id.login || id == EditorInfo.IME_NULL) {
					attemptLogin();
					return true;
				}
				return false;
			}
		});

		Button mEmailSignInButton = (Button) findViewById(R.id.new_account_button);
		mEmailSignInButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View view) {
				attemptLogin();
			}
		});

		mLoginFormView = findViewById(R.id.login_form);
		mProgressView = findViewById(R.id.signin_progress);
		mProgressText = (TextView)findViewById(R.id.signin_progress_text);
		simlock_checkbox = (CheckBox) findViewById(R.id.signin_checkBox);
		notification = new NotificationCompat.Builder(this);
		notification.setAutoCancel(true);
		activityIsOn = true;
		mServerApi = ServerAPI.getInstance(getBaseContext());

		mActivity = this;
		isOnWizard = false;
		Bundle wizardBundle = getIntent().getExtras();
		if(wizardBundle != null)
			isOnWizard = wizardBundle.getBoolean("com.qualcomm.qti.securemsm.mdtp.MdtpDemo.isOnWizard");
	}

	@Override
	protected void onStop() {
		super.onStop();
		activityIsOn = false; /** User activity is not shown on screen anymore **/
	}

	@Override
	protected void onResume() {
		super.onResume();
		activityIsOn = true; /** User activity is shown on screen **/
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
		/** Reset errors. **/
		mEmailView.setError(null);
		mPasswordView.setError(null);
		mRepeatPasswordView.setError(null);

		/** Store values at the time of the login attempt. **/
		String email = mEmailView.getText().toString();
		String password = mPasswordView.getText().toString();
		String repeatPassword = mRepeatPasswordView.getText().toString();

		boolean cancel = false;
		View focusView = null;

		/** Check for a valid password, if the user entered one. **/
		if (!TextUtils.isEmpty(password) && !isPasswordValid(password,repeatPassword)) {
			mPasswordView.setError(getString(R.string.error_invalid_password));
			focusView = mPasswordView;
			cancel = true;
		}
		/** Check for a valid email address. **/
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
			/** There was an error; don't attempt login and focus the first
			form field with an error. **/
			focusView.requestFocus();
		} else {
			DevicePolicyManager mDevicePolicyManager = (DevicePolicyManager) getSystemService(Context.DEVICE_POLICY_SERVICE);
			ComponentName adminComponentName = new ComponentName(this, LockReceiver.class);
			if(mDevicePolicyManager.isAdminActive(adminComponentName)) {
				/** Show a progress spinner, and kick off a background task to
				perform the user login attempt. **/
				showProgress(true);
				mAuthTask = new UserSignUpTask(email, password);
				mAuthTask.execute((Void) null);
			}else{
				/** Register as device admin **/
				mEmailAfterAdmin = email;
				mPasswordAfterAdmin = password;
				Intent intent = new Intent(DevicePolicyManager.ACTION_ADD_DEVICE_ADMIN);
				intent.putExtra(DevicePolicyManager.EXTRA_DEVICE_ADMIN, adminComponentName);
				intent.putExtra(DevicePolicyManager.EXTRA_ADD_EXPLANATION,
								"Enable MDTP Anti Theft Demo.");
				startActivityForResult(intent, ACTIVATION_REQUEST);
			}

		}
	}

	private boolean isEmailValid(String email) {
		return email.contains("@");
	}

	private boolean isPasswordValid(String password,String repeatPassword) {
		if (!password.equals(repeatPassword)){
			return false;
		}
		return password.length() > 4;
	}

	/**
	 * Shows the progress UI and hides the login form.
	 */
	@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
	public void showProgress(final boolean show) {
		/** On Honeycomb MR2 we have the ViewPropertyAnimator APIs, which allow
		for very easy animations. If available, use these APIs to fade-in
		the progress spinner. **/
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

			mProgressText.setVisibility(show ? View.VISIBLE : View.GONE);
		} else {
			/** The ViewPropertyAnimator APIs are not available, so simply show
			and hide the relevant UI components. **/
			mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
			mProgressText.setVisibility(show ? View.VISIBLE : View.GONE);
			mLoginFormView.setVisibility(show ? View.GONE : View.VISIBLE);
		}
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == ACTIVATION_REQUEST) {
			if(resultCode == Activity.RESULT_OK){
				showProgress(true);
				mAuthTask = new UserSignUpTask(mEmailAfterAdmin, mPasswordAfterAdmin);
				mAuthTask.execute((Void) null);
			}
			if (resultCode == Activity.RESULT_CANCELED) {
				return;
			}
		}else{
			super.onActivityResult(requestCode, resultCode, data);
		}
	}

	/**
	 * Represents an asynchronous login/registration task used to authenticate
	 * the user.
	 */
	public class UserSignUpTask extends AsyncTask<Void, Void, Boolean> {

		private final String mEmail;
		private final String mPassword;
		public boolean networkError;

		UserSignUpTask(String email, String password) {
			mEmail = email;
			mPassword = password;
		}

		@Override
		protected Boolean doInBackground(Void... params) {
			networkError = false;
			try {
				MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
				Thread.sleep(2000);
			} catch (Exception e) {
				e.printStackTrace();
				return false;
			}

			String retval;

			/** SignUpRequest **/
			MdtpMessage MsgHeader = new MdtpMessage("SignUpRequest", "Client", "Server");
			MdtpMessage.SignUpRequest signUpMsg = MsgHeader.new SignUpRequest(mEmail,mPassword);

			retval = mServerApi.sendMessageToServer(MsgHeader, signUpMsg, 60000);
			if(retval.equals("Network Error")){
				networkError = true;
				return false;
			}else if(!retval.equals("Success")){
				return false;
			}
			/** Update UI **/
			runOnUiThread(new Runnable() {
			    public void run() {
					mProgressText.setText(R.string.activation_proc);
			    }
			});

			/** Activating **/
			MsgHeader = new MdtpMessage("ActivationRequest", "Client", "Server");
			MdtpMessage.ActivationRequest activationRequest = MsgHeader.new ActivationRequest(mEmail, mPassword);
			activationRequest.setIMEI("TODO: IMEI");
			activationRequest.setUserToken(MdtpApiSingleton.getInstance().getIdToken());
			activationRequest.setSimLock(simlock_checkbox.isChecked());
			activationRequest.setIsvDeviceID("MyUniqueDeviceIDMyUniqueDeviceID");
			activationRequest.setHWRecoveryPin(MdtpApiSingleton.getInstance().getHwRecoveryPin());

			retval = mServerApi.sendMessageToServer(MsgHeader, activationRequest, 60000);
			if(retval.equals("Network Error")){
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				networkError = true;
				return false;
			}else if(!retval.equals("Success")){
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				return false;
			}

			/** Update server for new status **/
			MsgHeader = new MdtpMessage("UpdateStatus", "Client", "Server");
			MdtpMessage.UpdateStatus UpdateStatus = MsgHeader.new UpdateStatus("Client", "Server");
			MdtpState state = MdtpApiSingleton.getInstance().getState();
			UpdateStatus.setStatus(new MdtpStateDemo(state));

			retval = mServerApi.sendMessageToServer(MsgHeader, UpdateStatus, 60000);
			if(retval.equals("Network Error")){
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				networkError = true;
				return false;
			}else if(!retval.equals("Success")){
				MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				return false;
			}
			mServerApi.setServerConnectionStatus(true);
			MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());

			/** Activation succeed, start poller thread **/
			Intent clientPollerIntent = new Intent(SignInActivity.this, ClientService.class);
			startService(clientPollerIntent);
			return true;
		}

		@Override
		protected void onPostExecute(final Boolean success) {
			mAuthTask = null;
			showProgress(false);

			Intent launchNextActivity = new Intent(SignInActivity.this, ActivatedMode.class);
			launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
			if (success) {
				if(isOnWizard){
					Log.d(TAG, "Mdtp: disabling Setup Wizard...");
					ComponentName wizardComponent =
						new ComponentName("com.qualcomm.qti.securemsm.mdtp.MdtpDemo",
							"com.qualcomm.qti.securemsm.mdtp.MdtpDemo.SetupWizardActivity");
					getPackageManager().setComponentEnabledSetting(wizardComponent,
						PackageManager.COMPONENT_ENABLED_STATE_DISABLED, 1);
					mActivity.finish();
					this.publishNotification(getString(R.string.activation_notify_success), true);
					return;
				}
				if(!activityIsOn){
					this.publishNotification(getString(R.string.activation_notify_success), true);
					/** Close app and return to home screen **/
					Intent homeIntent = new Intent(Intent.ACTION_MAIN);
					homeIntent.addCategory(Intent.CATEGORY_HOME);
					homeIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
					homeIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
					startActivity(homeIntent);
					mActivity.finish();
					return;
				}

		        startActivity(launchNextActivity);
			} else {
				if(this.networkError){
					Toast.makeText(SignInActivity.this, getString(R.string.server_connection_error), Toast.LENGTH_SHORT).show();
				}else{
					mPasswordView.setError(getString(R.string.error_incorrect_credentials));
					mEmailView.requestFocus();
				}

			}
			mProgressText.setText(R.string.sign_in_creating);
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
				intent = new Intent(getBaseContext(), ActivatedMode.class);
			} else{
				intent = new Intent(getBaseContext(), LoginActivity.class);
			}
			intent.setFlags(Intent.FLAG_ACTIVITY_NEW_DOCUMENT);
			intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);

			PendingIntent pendingIntent = PendingIntent.getActivity(getBaseContext(), 0 , intent, PendingIntent.FLAG_UPDATE_CURRENT);
			notification.setContentIntent(pendingIntent);
			NotificationManager nm = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
			nm.notify(uniqueID, notification.build());
		}
	}


}
