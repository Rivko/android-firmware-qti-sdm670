/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

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
import android.os.UserManager;
import android.os.BatteryManager;
import android.app.admin.SystemUpdatePolicy;

public class DeviceLockUtil {

	private static String TAG = "DeviceLockUtil";

	private Context mContext;
	private StatusBarManager mStatusBarManager;
	private KeyguardManager mKeyguardManager;
	private KeyguardManager.KeyguardLock mKeyguardLock;
	private DevicePolicyManager mDevicePolicyManager;
	private LockPatternUtils mLockPatternUtils;
	private ActivityManager mActivityManager;
	private IActivityManager mInternalActivityManager;
	private ComponentName adminComponentName;

	/** DeviceLockUtil Singleton */
	private static DeviceLockUtil instance = null;

	/** Get reference to singleton **/
	public static DeviceLockUtil getInstance(Context context){
		if(context == null)
			throw new RuntimeException();
		if(instance == null) {
			Log.d(TAG, "Instantiate DeviceLockUtil");
			instance = new DeviceLockUtil();
			instance.mContext = context;
		}
		instance.initSystemServices();
		return instance;
	}

	/** Init all needed services **/
	private void initSystemServices(){
		mStatusBarManager = (StatusBarManager) mContext.getSystemService(Context.STATUS_BAR_SERVICE);
		mKeyguardManager = (KeyguardManager) mContext.getSystemService(Context.KEYGUARD_SERVICE);
		mDevicePolicyManager = (DevicePolicyManager) mContext.getSystemService(Context.DEVICE_POLICY_SERVICE);
		mActivityManager = (ActivityManager) mContext.getSystemService(Context.ACTIVITY_SERVICE);
		mInternalActivityManager = ActivityManagerNative.getDefault();
		adminComponentName = LockReceiver.getComponentName(mContext);
		mLockPatternUtils = new LockPatternUtils(mContext);

		if (mStatusBarManager == null || mKeyguardManager == null || mDevicePolicyManager == null||
			mActivityManager == null || mInternalActivityManager == null || adminComponentName == null ||
			mLockPatternUtils == null)
			throw new RuntimeException();
	}

	/** Disable hardware buttons **/
	private void disableDeviceStatusBar(){
		mStatusBarManager.disable(
			StatusBarManager.DISABLE_EXPAND
			| StatusBarManager.DISABLE_NOTIFICATION_ICONS
			| StatusBarManager.DISABLE_NOTIFICATION_ALERTS
			| StatusBarManager.DISABLE_RECENT
			| StatusBarManager.DISABLE_BACK
			| StatusBarManager.DISABLE_SYSTEM_INFO
			| StatusBarManager.DISABLE_SEARCH);
	}

	/** Disable Keyguard and lock pattern\password **/
	private void disableDeviceKeyguard(){
		mKeyguardLock = mKeyguardManager.newKeyguardLock(Context.KEYGUARD_SERVICE);
		mKeyguardLock.disableKeyguard();

		mLockPatternUtils.clearLock(UserHandle.USER_OWNER);
		mLockPatternUtils.setLockScreenDisabled(true, UserHandle.USER_OWNER);
	}

	/** Adjust security and system settings **/
	private void adjustSecureSettings(boolean disable){
		int disableInt = disable ? 1 : 0;

		/** Disable camera gestures **/
		Settings.Secure.putInt(mContext.getContentResolver(),
			Settings.Secure.CAMERA_DOUBLE_TAP_POWER_GESTURE_DISABLED,
			disableInt /* Backwards because setting is for disabling */);
		Settings.Secure.putInt(mContext.getContentResolver(),
			Settings.Secure.CAMERA_GESTURE_DISABLED,
			disableInt /* Backwards because setting is for disabling */);
		Settings.System.putInt(mContext.getContentResolver(),
			Settings.System.LOCKSCREEN_DISABLED,
			disableInt /* Backwards because setting is for disabling */);
	}

	/** Adjust screen pinning settings **/
	private void adjustPinningSettings(){
		List<String> mLockTaskPackages = new ArrayList<>();
		mLockTaskPackages.add(mContext.getPackageName());
		String packages[] = new String[] {mContext.getPackageName()};
		try{
			mInternalActivityManager.updateLockTaskPackages(UserHandle.USER_OWNER, packages);
		}catch (Exception e) {
			/** Not gonna happen **/
			Log.d(TAG,"mdtp DevicePolicyManager updateLockTaskPackages RemoteException");
			throw new RuntimeException();
		}

		Log.d(TAG,"mdtp -----------------------");
		Log.d(TAG,"mdtp device owner is : " + mDevicePolicyManager.getDeviceOwner());
		Log.d(TAG,"mdtp device owner name is : " + mDevicePolicyManager.getDeviceOwnerNameOnAnyUser());
		Log.d(TAG,"mdtp Am I device owner? : " + mDevicePolicyManager.isDeviceOwnerApp(mContext.getPackageName()));
		Log.d(TAG,"mdtp -----------------------");
	}

	/** Enable hardware acceleration **/
	private void setHardwareAcceleration(){
		WindowManager.LayoutParams mLpChanged = new WindowManager.LayoutParams();
		mLpChanged.flags |=
			WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED;
		mLpChanged.privateFlags |=
			WindowManager.LayoutParams.PRIVATE_FLAG_FORCE_HARDWARE_ACCELERATED;
	}

	/** Set user restrictions **/
	private void setRestrictionPolicies(boolean active){
		setUserRestriction(UserManager.DISALLOW_SAFE_BOOT, active);
		setUserRestriction(UserManager.DISALLOW_FACTORY_RESET, active);
		setUserRestriction(UserManager.DISALLOW_ADD_USER, active);
		setUserRestriction(UserManager.DISALLOW_MOUNT_PHYSICAL_MEDIA, active);
		setUserRestriction(UserManager.DISALLOW_ADJUST_VOLUME, active);
		setUserRestriction(UserManager.DISALLOW_MODIFY_ACCOUNTS, active);
		setUserRestriction(UserManager.DISALLOW_INSTALL_APPS, active);
		setUserRestriction(UserManager.DISALLOW_UNINSTALL_APPS, active);
		setUserRestriction(UserManager.DISALLOW_USB_FILE_TRANSFER, active);
		setUserRestriction(UserManager.DISALLOW_OUTGOING_CALLS, active);

		/*
		// Recommended for OEMs:
		setUserRestriction(UserManager.DISALLOW_DEBUGGING_FEATURES, active);
		setUserRestriction(UserManager.DISALLOW_APPS_CONTROL, active);
		setUserRestriction(UserManager.DISALLOW_CONFIG_WIFI, active);
		setUserRestriction(UserManager.DISALLOW_SHARE_LOCATION, active);
		setUserRestriction(UserManager.DISALLOW_INSTALL_UNKNOWN_SOURCES, active);
		setUserRestriction(UserManager.DISALLOW_CONFIG_BLUETOOTH, active);
		setUserRestriction(UserManager.DISALLOW_CONFIG_CREDENTIALS, active);
		setUserRestriction(UserManager.DISALLOW_REMOVE_USER, active);
		setUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS, active);
		setUserRestriction(UserManager.DISALLOW_SMS, active);
		setUserRestriction(UserManager.DISALLOW_OUTGOING_BEAM, active);

		// Set system update policy
		if (active){
		    mDevicePolicyManager.setSystemUpdatePolicy(adminComponentName,
				SystemUpdatePolicy.createWindowedInstallPolicy(60, 120));
		} else {
		    mDevicePolicyManager.setSystemUpdatePolicy(adminComponentName,
				null);
		}
		*/

		/** set this Activity as a lock task package **/
		mDevicePolicyManager.setLockTaskPackages(adminComponentName, new String[]{mContext.getPackageName()});
		mDevicePolicyManager.setLockTaskPackages(adminComponentName,
				active ? new String[]{mContext.getPackageName()} : new String[]{});
	}

	/** Wrap function for enable\disable user restriction **/
	private void setUserRestriction(String restriction, boolean disallow){
		if (disallow) {
			mDevicePolicyManager.addUserRestriction(adminComponentName,
				restriction);
		} else {
			mDevicePolicyManager.clearUserRestriction(adminComponentName,
				restriction);
		}
	}

	/** Device will stay in while plugged in **/
	private void enableStayOnWhilePluggedIn(boolean enabled){
		if (enabled) {
			mDevicePolicyManager.setGlobalSetting(
				adminComponentName,
				Settings.Global.STAY_ON_WHILE_PLUGGED_IN,
				Integer.toString(BatteryManager.BATTERY_PLUGGED_AC
					| BatteryManager.BATTERY_PLUGGED_USB
					| BatteryManager.BATTERY_PLUGGED_WIRELESS));
		} else {
			mDevicePolicyManager.setGlobalSetting(
				adminComponentName,
				Settings.Global.STAY_ON_WHILE_PLUGGED_IN,
				"0"
				);
		}
	}

	/** Lock the device **/
	public void lockDevice(){
		disableDeviceStatusBar();
		setHardwareAcceleration();
		disableDeviceKeyguard();
		adjustSecureSettings(true);

		/** Only if device owner **/
		if(mDevicePolicyManager.isDeviceOwnerApp(mContext.getPackageName())){
			adjustPinningSettings();
			setRestrictionPolicies(true);
			enableStayOnWhilePluggedIn(true);
		}
	}

	/** Unlock the device **/
	public void unlockDevice(){
		mStatusBarManager.disable(StatusBarManager.DISABLE_NONE);

		/** Re-enable Keyguard **/
		//mKeyguardLock.reenableKeyguard();

		adjustSecureSettings(false);

		mLockPatternUtils.clearLock(UserHandle.USER_OWNER);
		mLockPatternUtils.setLockScreenDisabled(false, UserHandle.USER_OWNER);

		/** Only if device owner **/
		if(mDevicePolicyManager.isDeviceOwnerApp(mContext.getPackageName())){
			setRestrictionPolicies(false);
			enableStayOnWhilePluggedIn(false);
		}

		ComponentName lockerComponent =
			new ComponentName("com.qualcomm.qti.securemsm.mdtp.MdtpDemo",
				"com.qualcomm.qti.securemsm.mdtp.MdtpDemo.LockScreenActivity");
		mContext.getPackageManager().setComponentEnabledSetting(lockerComponent,
			PackageManager.COMPONENT_ENABLED_STATE_DISABLED, 1);
	}

}
