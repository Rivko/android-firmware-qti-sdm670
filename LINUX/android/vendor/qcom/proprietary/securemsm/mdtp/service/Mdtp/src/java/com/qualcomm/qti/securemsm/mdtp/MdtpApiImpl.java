/*=============================================================================
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*===========================================================================

 EDIT HISTORY FOR FILE


 when         who      what, where, why
 ----------   ---      -------------------------------------------------------
 12/26/2016   ysreekan Removed Firmware lock related sources

 ===========================================================================*/

package com.qualcomm.qti.securemsm.mdtp;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

public class MdtpApiImpl implements MdtpApi {
	private static final String TAG = "MdtpApiImpl";
	private static final String SERVICE_NAME = "mdtp";
	private Intent mServiceIntent;
	private IMdtpService mIMdtpApi = null;  /** Binder class created by AIDL */
	private boolean mIsStarted = false;
	private boolean mIsBound = false;

	static final int STATE_DISABLED = 0; /** Bitmask offset of activated state in mdtp state_mask */
	static final int STATE_INACTIVE = 1; /** Bitmask offset of activated state in mdtp state_mask */
	static final int STATE_ACTIVE = 2; /** Bitmask offset of activated state in mdtp state_mask */
	static final int STATE_TAMPERED = 3; /** Bitmask offset of activated state in mdtp state_mask */
	static final int STATE_INVALID = 4; /** Bitmask offset of activated state in mdtp state_mask */

	static final int STATE_SIMLOCKED_MSK = 1; /** Bitmask offset of activated state in mdtp state_mask */
	static final int STATE_EMERGENCY_ONLY_MODE_MSK = 2; /** Bitmask offset of activated state in mdtp state_mask */

	/**
	 * This class represents the actual service connection. It casts the bound
	 * stub implementation of the service to the AIDL interface.
	 */
	private ServiceConnection mConnection = new ServiceConnection() {

		/** Called when the connection with the service is established */
		public void onServiceConnected(ComponentName className, IBinder service) {

			/** Gets an instance of the IRemoteInterface, which we can use to call on the service */
			Log.d(TAG, "MdtpService connected");
			if (mIMdtpApi == null) {
				mIMdtpApi = IMdtpService.Stub.asInterface(service);
			}
		}

		/** Called when the connection with the service disconnects unexpectedly */
		public void onServiceDisconnected(ComponentName className) {
			Log.e(TAG, "MdtpService disconnected");
			mIMdtpApi = null;
		}
	};

	/*
	 *  Constructor
	 */
	public MdtpApiImpl() {
		Log.d(TAG, "MdtpApiImpl CTOR");
		mServiceIntent = new Intent();
		mServiceIntent.setClassName("com.qualcomm.qti.securemsm.mdtp",
				"com.qualcomm.qti.securemsm.mdtp.MdtpService");
	}

	/**
	 * Starts the MDTP Service application
	 * @param[in]  ctx  context of the calling activity
	 * @Exception RuntimeException
	 */
	public void start(Context ctx) throws RuntimeException {
		Log.d(TAG, "Starting service, mIsStarted="+mIsStarted);
		if (ctx == null)
			throw new RuntimeException(new NullPointerException());

		if (mServiceIntent == null)
			throw new RuntimeException(new NullPointerException());

		if (mIsStarted)
			throw new RuntimeException("MdtpService already started");

		ctx.startService(mServiceIntent);
		mIsStarted = true;
	}

	/**
	 * Stops the MDTP Service application
	 * @param[in]  ctx  context of the calling activity
	 * @Exception RuntimeException
	 */
	public void stop(Context ctx) throws RuntimeException {
		Log.d(TAG, "Stopping service, mIsStarted="+mIsStarted);
		if (ctx == null)
			throw new RuntimeException(new NullPointerException());

		if (mServiceIntent == null)
			throw new RuntimeException(new NullPointerException());

		if (!mIsStarted)
			throw new RuntimeException("MdtpService not started");

		ctx.stopService(mServiceIntent);
		mIsBound = false;
		mIsStarted = false;
	}

	/**
	 * Binds the connection to the system service application
	 * @param[in]  ctx  context of the calling activity
	 * @Exception RuntimeException
	 */
	public void bind(Context ctx) throws RuntimeException {
		if (mServiceIntent == null || mConnection == null)
			throw new RuntimeException(new NullPointerException());

		if (ctx == null)
			throw new RuntimeException(new NullPointerException());

		if (!mIsStarted)
			throw new RuntimeException("MdtpService not started");

		if (mIsBound)
			throw new RuntimeException("MdtpService already bound");

		if (ctx.bindService(mServiceIntent, mConnection, Context.BIND_AUTO_CREATE)) {
			Log.d(TAG, "Binding successfull");
			mIsBound = true;
		}
		else {
			Log.d(TAG, "Binding FAILED !!!!");
			throw new RuntimeException("MdtpService bind failed");
		}
	}

	/**
	 * Unbinds the connection from the system service application
	 * @param[in]  ctx  context of the calling activity
	 * @Exception RuntimeException
	 */
	public void unBind(Context ctx) throws RuntimeException {
		if ((mConnection == null) || (mIMdtpApi == null))
			throw new RuntimeException(new NullPointerException());

		if (ctx == null)
			throw new RuntimeException(new NullPointerException());

		if (!mIsBound)
			throw new RuntimeException("MdtpService not bound");
		ctx.unbindService(mConnection);
		mIsBound = false;
	}

	/**
	 * Checks if the API has been successfully bound to the MDTP service.
	 * @return true / false
	 */
	public boolean isBound() {
		if (!mIsBound)
			return false;
		return(mIMdtpApi != null);
	}

	/**
	 * Returns the current state of the MDTP.
	 * @return MdtpState
	 * @Exception RuntimeException
	 */
	@Override
	public MdtpState getState() throws RuntimeException {
		if (!mIsBound)
			throw new RuntimeException("MdtpService not bound");
		try {
			int[] system_state = new int[1];
			int[] app_state = new int[1];
			MdtpSystemState systemState;
			MdtpAppState appState = new MdtpAppState();

			if (mIMdtpApi.getState(system_state, app_state) != 0)
				throw new RuntimeException("getState failed");
			else {
				switch (system_state[0]) {
				    case STATE_DISABLED:
					systemState = MdtpSystemState.DISABLED;
					break;
				    case STATE_INACTIVE:
					systemState = MdtpSystemState.INACTIVE;
					break;
				    case STATE_ACTIVE:
					systemState = MdtpSystemState.ACTIVE;
					break;
				    case STATE_TAMPERED:
					systemState = MdtpSystemState.TAMPERED;
					break;
				    case STATE_INVALID:
					systemState = MdtpSystemState.INVALID;
					break;
				    default:
					throw new RuntimeException("Wrong system_state");
				}

				appState.mSimLocked = ((app_state[0] & STATE_SIMLOCKED_MSK) != 0);
				appState.mEmergencyOnly = ((app_state[0] & STATE_EMERGENCY_ONLY_MODE_MSK) != 0);

				MdtpState state = new MdtpState(systemState, appState);

				return state;
			}
		} catch (RemoteException e) {
			throw new RuntimeException(e);
		}
	}

	/**
	 * Returns the version of the MDTP library.
	 * @return MdtpVersion
	 * @Exception RuntimeException
	 */
	@Override
	public MdtpVersion getVersion() throws RuntimeException {
		if (!mIsBound)
			throw new RuntimeException("MdtpService not bound");
		try {
			int[] libV = new int[1];
			int[] tzV = new int[1];
			if (mIMdtpApi.getVersion(libV, tzV) != 0)
				throw new RuntimeException("getVersion failed");
			else {
				MdtpVersion version = new MdtpVersion();
				version.mLibVersion = libV[0];
				version.mTzVersion = tzV[0];
				return version;
			}
		} catch (RemoteException e) {
			throw new RuntimeException(e);
		}
	}

	/**
	 * Process a signed message sent to device by MDTP services. Signed messages are used for sensitive operations,
	 * such as MDTP activation, deactivation, etc. The structure of the content and the different messages supported
	 * by the MDTP is described in "MDTP Interface Specification" document. The function will process the message,
	 * validate its authenticity, and execute the command it carries.
	 * @param[in]    message signed message.
	 * @Exception RuntimeException
	 */
	@Override
	public void processSignedMsg(byte[] message) throws RuntimeException {
		if (!mIsBound)
			throw new RuntimeException("MdtpService not bound");
		try {
			if (mIMdtpApi.processSignedMsg(message) != 0)
				throw new RuntimeException("processSignedMsg failed");
		} catch (RemoteException e) {
			throw new RuntimeException(e);
		}
	}

	/**
	 * Deactivates the MDTP protection, using local PIN authentication instead of a signed deactivation message arriving
	 * from MDTP services. The pin is generated in the cloud, and received as part of a signed activation message.
	 * during MDTP activation. (See processSignedMsg).
	 * @param[in]  PIN of MDTP_PIN_LEN digits.
	 * @Exception RuntimeException
	 */
	@Override
	public void deactivateLocal(String pin) throws RuntimeException {
		if (!mIsBound)
			throw new RuntimeException("MdtpService not bound");
		try {
			if (mIMdtpApi.deactivateLocal(pin.getBytes()) != 0)
				throw new RuntimeException("deactivateLocal failed");
		} catch (RemoteException e) {
			throw new RuntimeException(e);
		}
	}

	/**
	 * Get an identity token to uniquely identify a specific instance of MDTP activation/deactivation.
	 * The API will be available only When MDTP state is inactive. On each call, a new unique token shall
	 * be generated and persisted.
	 * Upon MDTP deactivation the identity token is cleared.
	 * @return byte[]
	 * @Exception RuntimeException
	 */
	@Override
	public byte[] getIdToken() throws RuntimeException {
		if (!mIsBound)
			throw new RuntimeException("MdtpService not bound");
		try {
			byte[] token = new byte[MDTP_ID_TOKEN_LEN];
			if (mIMdtpApi.getIdToken(token) != 0)
				throw new RuntimeException("getIdToken failed");
			else
				return token;
		} catch (RemoteException e) {
			throw new RuntimeException(e);
		}
	}

	/**
	 * Updates the MDTP with a given RFC5280 CRL
	 * @param[in]  crl  Certificate Revocation List (CRL), according to RFC5280.
	 * @param[in]  cert  The X509 certificate the CRL is signed with. This is to identify who signed the CRL (the root CA or its
	 * subordinates CAs)
	 * @Exception RuntimeException
	 */
	@Override
	public void updateCrl(byte[] crl, byte[] certificate) throws RuntimeException {
		if (!mIsBound)
			throw new RuntimeException("MdtpService not bound");
		try {
			if (mIMdtpApi.updateCrl(crl, certificate) != 0)
				throw new RuntimeException("updateCrl failed");
		} catch (RemoteException e) {
			throw new RuntimeException(e);
		}
	}

	/**
	 * Gets the HW recovery PIN of the device. The HW recovery PIN is used by the Bootloader, for recovering MDTP when
	 * its storage is badly damaged, so ISV PIN canoot be available.
	 * @return byte[]
	 * @Exception RuntimeException
	 */
	public String getHwRecoveryPin() throws RuntimeException {
		if (!mIsBound)
			throw new RuntimeException("MdtpService not bound");
		try {
			byte[] HwRecoveryPin = new byte[MDTP_PIN_LEN];
			if (mIMdtpApi.getHwRecoveryPin(HwRecoveryPin) != 0)
				throw new RuntimeException("getHwRecoveryPin failed");
			else
				return new String(HwRecoveryPin);
		} catch (RemoteException e) {
			throw new RuntimeException(e);
		}
	}

}
