/*=============================================================================
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import java.lang.reflect.Method;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.widget.EditText;
import android.os.Build;
import android.os.AsyncTask;
import android.content.ComponentName;
import android.content.Intent;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;

public class SetupWizardActivity extends FragmentActivity {
	private static final String MDTP_SETUPWIZARD_ENABLE_SYSPROP = "mdtp.setupwizard.enable";
	private static String TAG = "MdtpSetupWizardActivity";
	private ServerAPI mServerApi = null;
	private Context mContext;
	private BoundToService mAuthTask = null;
	private EditText serverIPAddrSign;
	private EditText serverIPAddrProtect;

/*********************************************************/

	/**
	 * Offload MDTP services to a non-UI thread
	 */
	private class BoundToService extends AsyncTask<Void, Void, Boolean> {

		private Context mBindContext;

		public BoundToService(Context context) {
			mBindContext = context;
		}

		protected Boolean doInBackground(Void... params) {
			try {
				/** Check if application has successfully bound to MDTP service */
				int i;
				// wait for 50*100 (5 seconds) - service initialization can take time on boot
				final int maxWaitIter = 50;
				for (i = 0; (!MdtpApiSingleton.getInstance().isBound()) && (i < maxWaitIter); i++) {
					Log.d(TAG, "Waiting for server bind...");
					Thread.sleep(100);
				}
				/** not bound to service */
				if (i == maxWaitIter) {
					Log.d(TAG, "Timeout while binding to the service");
					return false;
				}
				Log.d(TAG, "Succeffully binded to the service");

			} catch (Exception e) {
				Log.d(TAG, "Exception while accesing the service " + e.toString());
				return false;
			}
			return true;
		}

		protected void onPostExecute(Boolean sts) {
			Log.d(TAG, "onPostExecute - sts: " + sts);
			if (sts == false)
				throw new RuntimeException();

			Log.d(TAG, "Bounded - Comparing Mdtp states");
			mServerApi = ServerAPI.getInstance(getBaseContext());
			MdtpStateDemo device_state = new MdtpStateDemo(MdtpApiSingleton.getInstance().getState());
			MdtpStateDemo app_state = mServerApi.getDeviceState();

			//Check if application state is same as device state
			if((device_state.mSystemState != app_state.mSystemState) && (device_state.mSystemState == MdtpStateDemo.MdtpSystemStateDemo.ACTIVE)){
				Log.d(TAG, "Start wizard on factory reset protection");
				initFactoryResetProtView();
			}else{
				Log.d(TAG, "Start wizard on first boot");
				initSignUpView();
			}

		}

	}


/********************************************************/
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.d(TAG, "onCreate");

		/** Disable the lockscreen since it will try to take over the home button **/
		disableLockScreen();

		/** Finish activity right now if the demo wizard is not allowed to run **/
		boolean shouldUse = shouldUseDemoWizard();
		Log.d(TAG, "shouldUseDemoWizard:" + shouldUse);
		if (!shouldUse) {
			Log.d(TAG, "MDTP Setup Wizard is deiabled on system properties");
			disableWizardAndContinue();
		    finish();
		}

		setContentView(R.layout.activity_setup_wizard);

		/** Bound to Mdtp service **/
		mContext = getBaseContext();
		try{
			MdtpApiSingleton.getInstance().start(mContext);
			MdtpApiSingleton.getInstance().bind(mContext);
		}catch(Exception e){
			throw new RuntimeException();
		}

		mAuthTask = new BoundToService(getApplicationContext());
		mAuthTask.execute((Void) null);
	}

	/**
	 * Sign up and activate SafeSwitch
	 */
	public void oclSignUp(View v){
		serverIPAddrSign = (EditText) findViewById(R.id.insertServerIPOnWizardSign);
		mServerApi.setServerIP(serverIPAddrSign.getText().toString());
		/** Start Mdtp server**/
		mServerApi.setLoadServerOnBoot(true);
		Intent serviceIntent = new Intent(this, ServerService.class);
		startService(serviceIntent);

		Intent intent = new Intent(this, SignInActivity.class);
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
			| Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
		intent.putExtra("com.qualcomm.qti.securemsm.mdtp.MdtpDemo.isOnWizard", true);
		startActivity(intent);
	}

	/**
	 * Skip activation
	 */
	public void oclSkipActivation(View v) {
		Log.d(TAG, "Disabling setup wizard");
		disableWizardAndContinue();
		finish();
	}

	/**
	 * Deactivate mdtp
	 */
	public void oclDeactivate(View v) {
		serverIPAddrProtect = (EditText) findViewById(R.id.insertServerIPOnWizardProtect);
		mServerApi.setServerIP(serverIPAddrProtect.getText().toString());
		/** Start Mdtp server**/
		mServerApi.setLoadServerOnBoot(true);
		Intent serviceIntent = new Intent(this, ServerService.class);
		startService(serviceIntent);

		final Intent intent = new Intent(this, LoginActivity.class);
		intent.putExtra("com.qualcomm.qti.securemsm.mdtp.MdtpDemo.isOnWizard", true);
		startActivity(intent);
	}

	/**
	 * Launch emergency dialer activity
	 */
	public void oclEmergencyDialer(View v) {
		final Intent intent = new Intent("com.android.phone.EmergencyDialer.DIAL");
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
			| Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
		startActivity(intent);
	}

	/**
	 * Starts WiFi setup
	 * Can be used to enable remote services activation/authorization during the setup
	 */
	public void oclStartWifiWizard(View v) {
		Intent intent = new Intent(android.provider.Settings.ACTION_WIFI_SETTINGS);
		startActivity(intent);
	}


	/** Fragments management **/

	/**
	 * Initialize the MDTP factory reset protection screen
	 * This layout is used when MDTP was found active on the first/clean boot
	 */
	private void initFactoryResetProtView() {
		// Create a new Fragment to be placed in the activity layout
		ResetProtectionFragment resetProtectionFregment = new ResetProtectionFragment();

		// In case this activity was started with special instructions from an
		// Intent, pass the Intent's extras to the fragment as arguments
		resetProtectionFregment.setArguments(getIntent().getExtras());

		// Add the fragment to the 'fragment_container' FrameLayout
		getSupportFragmentManager().beginTransaction()
		        .add(R.id.fragment_container, resetProtectionFregment).commit();
	}


	/**
	 * Initialize the MDTP Sign-up screen
	 * This layout allows user optional MDTP activation on first device boot
	 */
	private void initSignUpView() {
		// Create a new Fragment to be placed in the activity layout
		SignUpFragment signUpFregment = new SignUpFragment();

		// In case this activity was started with special instructions from an
		// Intent, pass the Intent's extras to the fragment as arguments
		signUpFregment.setArguments(getIntent().getExtras());

		// Add the fragment to the 'fragment_container' FrameLayout
		getSupportFragmentManager().beginTransaction()
			.add(R.id.fragment_container, signUpFregment).commit();
	}

	/**
	 * ActivityFragment that implements user factory reset protection screen
	 */
	private class ResetProtectionFragment extends Fragment{
		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
			// Inflate the layout for this fragment
			return inflater.inflate(R.layout.wizard_factory_reset, container, false);
		}
	}

	/**
	 * ActivityFragment that implements first boot sign up screen
	 */
	private class SignUpFragment extends Fragment{
		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
			// Inflate the layout for this fragment
			return inflater.inflate(R.layout.wizard_sign_up, container, false);
		}
	}


	/********************** support functions *********************/

	/**
	 * Call this function when finishing the wizard.
	 * Finishing the wizard should disable and finish activity
	 * DONT_KILL_APP = 1
	 */
	private void disableWizardAndContinue() {
		getPackageManager().setComponentEnabledSetting(getComponentName(),
			PackageManager.COMPONENT_ENABLED_STATE_DISABLED, 1);
	}

	/**
	 * Call this function when finishing the lockscreen by the wizard.
	 * Finishing the wizard should disable activity
	 * DONT_KILL_APP = 1
	 */
	private void disableLockScreen() {
		ComponentName lockerComponent =
			new ComponentName("com.qualcomm.qti.securemsm.mdtp.MdtpDemo",
				"com.qualcomm.qti.securemsm.mdtp.MdtpDemo.LockScreenActivity");
		getPackageManager().setComponentEnabledSetting(lockerComponent,
			PackageManager.COMPONENT_ENABLED_STATE_DISABLED, 1);
	}

	/**
	 * Enable the wizard only if this is the first boot and the production system property is set
	 */
	private boolean shouldUseDemoWizard() {
		String propWizardEnabled = getSystemProperty(MDTP_SETUPWIZARD_ENABLE_SYSPROP, "undefined");
		Log.d(TAG, "shouldUseMDTPWizard " + MDTP_SETUPWIZARD_ENABLE_SYSPROP + " is " + propWizardEnabled);

		//we should check if the wizard is explicitly enabled
		if (propWizardEnabled.charAt(0) == '1') {
			return true;
		}
		//otherwise the demo wizard UI should never appear or block the boot
		return false;
	}

	/**
	 * Reads Android system property using undocumented framework class
	 * Should be used with care due potential future incompatibility
	 * @param propName - property name to read
	 * @param defaultValue - value to return if the property was defined in the system
	 */
	private String getSystemProperty(String propName,String defaultValue) {
		String prop = "";
		try {
			Class classFromName = null;
			classFromName = Class.forName("android.os.SystemProperties");
			Method method;
			method = classFromName.getDeclaredMethod("get", String.class);
			prop = (String)method.invoke(null, propName);
			if (prop.isEmpty())
				prop = defaultValue;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return prop;
	}
}
