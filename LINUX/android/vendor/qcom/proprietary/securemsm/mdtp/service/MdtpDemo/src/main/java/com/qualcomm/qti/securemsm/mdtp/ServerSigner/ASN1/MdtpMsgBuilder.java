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

package com.qualcomm.qti.securemsm.mdtp.ServerSigner.ASN1;

import android.util.Log;

public class MdtpMsgBuilder {
	private static final String TAG = "Mdtp.ServerSigner: ASN1";

	static {
		System.loadLibrary("mdtpdemojni");
	}

	private static native byte[] nativeBuildSetStateMsg(
			boolean active, boolean simLockSelect,
			byte[] idToken, byte[] isvPin, byte[] isvDeviceId, byte[] isvFriendlyName);

	private static native byte[] nativeBuildSetLockMsg(
			boolean lock, boolean simLockSelect, byte[] idToken);

	private static native byte[] nativeBuildSetKillMsg(boolean kill, boolean SimSelect, byte[] idToken);

	/**
	 * Build MDTP Set_State message in ASN1 format.
	 * The Set_State message is used for MDTP activation and deactivation.
	 * @param[in]  active  true for activation, false for deactivation.
	 * @param[in]  simLockSelect  true if sim lock should be activated as well.
	 * @param[in]  idToken the ID token.
	 * @param[in]  isvPin the local deactivation PIN.
	 * @param[in]  isvDeviceId A unique device ID.
	 * @param[in]  isvFriendlyName the ISV ID token.
	 */
	public static byte[] buildSetStateMsg(boolean active, boolean simLockSelect,
					      byte[] idToken, byte[] isvPin, byte[] isvDeviceId, byte[] isvFriendlyName) {
		return nativeBuildSetStateMsg(active, simLockSelect, idToken, isvPin, isvDeviceId, isvFriendlyName);
	}

	/**
	 * Build MDTP Set_Lock message in ASN1 format.
	 * The Set_Lock message is used mainly for MDTP SIM lock and unlock.
	 * @param[in]  lock  true for lock, false for unlock.
	 * @param[in]  simLockSelect  true if sim should be locked/unlocked.
	 * @param[in]  idToken the ID token.
	 */
	public static byte[] buildSetLockMsg(boolean lock, boolean simLockSelect, byte[] idToken) {
		return nativeBuildSetLockMsg(lock, simLockSelect, idToken);
	}

	/**
	 * Build MDTP Set_Kill message in ASN1 format.
	 * The Set_Kill message is used for MDTP Kill (E911 mode) and Restore.
	 * @param[in]  kill  true for Kill (E911 mode), false for Restore.
	 * @param[in]  SimSelect  true if SIM functionality should be killed (currently: always).
	 * @param[in]  idToken the ID token.
	 */
	public static byte[] buildSetKillMsg(boolean kill, boolean SimSelect, byte[] idToken) {
		return nativeBuildSetKillMsg(kill, SimSelect, idToken);
	}
}
