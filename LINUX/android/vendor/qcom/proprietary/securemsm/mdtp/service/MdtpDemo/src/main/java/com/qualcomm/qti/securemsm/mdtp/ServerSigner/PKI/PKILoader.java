/*=============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
package com.qualcomm.qti.securemsm.mdtp.ServerSigner.PKI;

import android.content.Context;
import android.util.Base64;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.Security;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.KeyFactory;
import java.security.cert.X509Certificate;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.PKCS8EncodedKeySpec;

public class PKILoader {
	private static final String TAG = "Mdtp.ServerSigner: PKI";
	private static byte[] IsvCert =  null;
	private static byte[] IsvCACert = null;
	private static byte[] RootCACert = null;
	private static byte[] IsvPrivateKey = null;

	/**
	 * Loads the MDTP certificate chain and the ISV private key from PEM resource files.
	 * @param[in]  context  Application context, needed for accessing resrouces.
	 */
	public static void load(Context context) {

		Log.d(TAG, "Loading server PKI");

		try {
			IsvCert = loadFromResource(context, "isv");
			IsvCACert = loadFromResource(context, "isv_ca");
			RootCACert = loadFromResource(context, "root_ca");
			IsvPrivateKey = loadFromResource(context, "isv_private");
		} catch (IOException e) {
			Log.d(TAG, e.getMessage());
			return;
		}

		Log.d(TAG, "Server PKI loaded");
	}

	/**
	 * Returns the ISV certificate.
	 * @Exception IOException
	 */
	public static byte[] getIsvCert() throws IOException {
		if (IsvCert == null)
			throw new IOException();
		else return IsvCert;
	}

	/**
	 * Returns the ISV CA certificate.
	 * @Exception IOException
	 */
	public static byte[] getIsvCACert() throws IOException {
		if (IsvCACert == null)
			throw new IOException();
		else return IsvCACert;
	}

	/**
	 * Returns the Root CA certificate.
	 * @Exception IOException
	 */
	public static byte[] getRootCACert() throws IOException {
		if (RootCACert == null)
			throw new IOException();
		else return RootCACert;
	}

	/**
	 * Returns the ISV's private key.
	 * @Exception IOException
	 */
	public static byte[] getIsvPrivateKey() throws IOException {
		if (IsvPrivateKey == null)
			throw new IOException();
		else return IsvPrivateKey;
	}

	private static byte[] loadFromResource(Context ctx, String resName) throws IOException {
		byte[] tmp;
		InputStream in = null;
		try {
			in = ctx.getResources().openRawResource(
					ctx.getResources().getIdentifier(resName, "raw", ctx.getPackageName()));
			tmp = new byte[in.available()];
			in.read(tmp);
		}
		finally {
			in.close();
		}

		return tmp;
	}
}

