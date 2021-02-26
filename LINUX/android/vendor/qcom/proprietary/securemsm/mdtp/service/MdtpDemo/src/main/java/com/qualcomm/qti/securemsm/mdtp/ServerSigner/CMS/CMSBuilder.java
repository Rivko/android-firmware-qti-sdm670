/*=============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.ServerSigner.CMS;

import android.util.Log;

import com.qualcomm.qti.securemsm.mdtp.ServerSigner.PKI.PKILoader;

public class CMSBuilder {
	private static final String TAG = "Mdtp.ServerSigner: CMS";

	static {
		System.loadLibrary("mdtpdemojni");
	}

	private static native byte[] nativeCreate(
			byte[] rootCACert, byte[] isvCACert, byte[] isvCert, byte[] isvPrivateKey, byte[] text);

	/**
	 * Create a signed CMS message, given a text.
	 * Note: PKILoader loaded must be called prior to using this function.
	 * @param[in]  text  Application context, needed for accessing resrouces.
	 */
	public static byte[] create(byte[] text) {
		Log.d(TAG, "Creating CMS message");
		byte[] tmp;

		try{
			tmp = nativeCreate(
					PKILoader.getRootCACert(),
					PKILoader.getIsvCACert(),
					PKILoader.getIsvCert(),
					PKILoader.getIsvPrivateKey(),
					text);
			Log.d(TAG, "CMS message created");
			return tmp;
		} catch(Exception e) {
			Log.d(TAG, e.getMessage());
			return null;
		}
	}
}