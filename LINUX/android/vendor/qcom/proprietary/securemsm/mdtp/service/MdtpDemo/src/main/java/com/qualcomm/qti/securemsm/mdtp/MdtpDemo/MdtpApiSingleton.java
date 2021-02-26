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

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.content.Context;
import android.util.Log;
import java.util.concurrent.locks.ReentrantLock;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApiImpl;
import java.io.File;

/**
 * This class implements a singleton around the MDTP API.
 * By doing so, only one instance of the MdtpApi object
 * is maintained, and shared amoungst the different activities.
 */
public class MdtpApiSingleton implements MdtpApi {
	private static final String TAG = "MdtpApiSingleton";

	private static MdtpApiSingleton instance = null;

	private MdtpApi mMapi;
	private boolean mIsInitialized = false;
	private static ReentrantLock mdtpApiLock;

	private MdtpApiSingleton() {
	}

	public static MdtpApiSingleton getInstance() {
		if(instance == null) {
			instance = new MdtpApiSingleton();
			mdtpApiLock = new ReentrantLock();
		}
		return instance;
	}

	public void lockAPI(Context ctx) throws RuntimeException {
		Log.d(TAG, "Locking...");
		mdtpApiLock.lock();
		Log.d(TAG, "Lock API completed");
	}

	public void unlockAPI(Context ctx) throws RuntimeException {
		Log.d(TAG, "Unlocking...");
		mdtpApiLock.unlock();
		Log.d(TAG, "Unlock API completed");
	}

	public void start(Context ctx) throws RuntimeException {
		Log.d(TAG, "Starting...");
		mMapi = new MdtpApiImpl();
		mMapi.start(ctx);
		mIsInitialized = true;
		Log.d(TAG, "Start completed");
	}

	public void stop(Context ctx) throws RuntimeException {
		Log.d(TAG, "Stopping...");
		mMapi = new MdtpApiImpl();
		mMapi.stop(ctx);
		mIsInitialized = false;
		Log.d(TAG, "Stop completed");
	}

	public void bind(Context ctx) throws RuntimeException {
		Log.d(TAG, "Binding...");
		mMapi.bind(ctx);
		Log.d(TAG, "Bind completed");
	}

	public void unBind(Context ctx) throws RuntimeException {
		Log.d(TAG, "unBinding...");
		mMapi.unBind(ctx);
		Log.d(TAG, "unBind completed");
	}

	public boolean isBound() {
		return mMapi.isBound();
	}

	public boolean isInitialized() {
		return mIsInitialized;
	}

	public MdtpState getState() throws RuntimeException {
		return mMapi.getState();
	}

	public MdtpVersion getVersion() throws RuntimeException {
		return mMapi.getVersion();
	}

	public void processSignedMsg(byte[] message) throws RuntimeException {
		mMapi.processSignedMsg(message);
	}

	public void deactivateLocal(String pin) throws RuntimeException {
		mMapi.deactivateLocal(pin);
	}

	public byte[] getIdToken() throws RuntimeException {
		return mMapi.getIdToken();
	}

	public void updateCrl(byte[] crl, byte[] certificate) throws RuntimeException {
		mMapi.updateCrl(crl, certificate);
	}

	public String getHwRecoveryPin() throws RuntimeException {
		return mMapi.getHwRecoveryPin();
	}
}
