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

import android.content.Context;
/** @addtogroup fwlockEnumtype
@{ */
public interface MdtpApi {

	public static final int MDTP_ID_TOKEN_LEN = 16;				/**< MDTP token length. */
	public static final int MDTP_PIN_LEN  = 8;				/**< Allowed length of PIN. */
	public static final int MDTP_MAX_MSG_LEN  = 4096;			/**< Maximum message length. */
	public static final int MDTP_MAX_CRL_LEN  = 2048;			/**< Maximum CRL length. */
	public static final int MDTP_MAX_CERT_LEN = 2048;			/**< Maximum certificate length. */

	/**
	 * MDTP version.
	 */
	public class MdtpVersion {
		public int mLibVersion; /**< MDTP version number for MDTP library. */
		public int mTzVersion;  /**< MDTP version number for the TrustZone component. */
	}

	/**
	 * MDTP system  state.
	 */
	public enum MdtpSystemState {
		DISABLED,	/**< MDTP was never activated on the device. */
		INACTIVE,	/**< MDTP was once activated, but now it is inactive. */
		ACTIVE,		/**< MDTP is active. */
		TAMPERED,	/**< MDTP storage is tampered and needs local deactivation with ISV PIN. */
		INVALID		/**< MDTP storage is badly corrupted, and needs recovery with HW recovery PIN. */
	}

	/**
	 * MDTP applicative state.
	 */
	public class MdtpAppState {
		public boolean mSimLocked; 	/**< Device is SIM locked or not. */
		public boolean mEmergencyOnly; 	/**< Device is in Emergency only mode or not. */
	}

	/**
	 * MDTP version.
	 */
	public class MdtpState {
		public MdtpSystemState mSystemState;	/**< MDTP system state enumeration. */
		public MdtpAppState mAppState;		/**< MDTP applicative state. */

		public MdtpState() {
			mSystemState = MdtpSystemState.DISABLED;
			mAppState = new MdtpAppState();
		}

		public MdtpState(MdtpSystemState systemState, MdtpAppState appState) {
			mSystemState = systemState;
			mAppState = appState;
		}
	}

	/**@ingroup fwlockfunctions
	 * Starts the MDTP Service application.
	 *
	 * @param[in]  ctx  Context of the calling activity.
	 *
	 * @return None.
	 */
	void start(Context ctx) throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Stops the MDTP Service application.
	 *
	 * @param[in]  ctx  Context of the calling activity.
	 *
	 * @return None.
	 */
	void stop(Context ctx) throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Binds the connection to the system service application.
	 *
	 * @param[in]  ctx  Context of the calling activity.
	 *
	 * @return None.
	 */
	void bind(Context ctx) throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Unbinds the connection from the system service application.
	 *
	 * @param[in]  ctx  Context of the calling activity.
	 *
	 * @return None.
	 */
	void unBind(Context ctx) throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Checks whether the API has successfully bound to the MDTP service.
	 *
	 * @return TRUE \n
	 *         FALSE
	 */
	boolean isBound();

	/**@ingroup fwlockfunctions
	 * Returns the current state of the MDTP.
	 *
	 * @return MdtpState: \n
	 *           MdtpSystemState: MDTP system state. Either one of: DISABLED/INACTIVE/ACTIVE/TAMPERED/INVALID.\n
	 *           MdtpAppState: MDTP applicative state. SIMLOCKED: true/false, EmergencyOnly: true/false. \n
	 */
	MdtpState getState() throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Returns the version of the MDTP library.
	 *
	 * @return MdtpVersion
	 *
	 */
	MdtpVersion getVersion() throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Processes a signed message sent to device by MDTP services. Signed messages are used for sensitive operations,
	 * such as MDTP activation and deactivation. The structure of the content and the different messages supported
	 * by the MDTP is described in @xhyperref{80NR0991,80-NR099-1}. This function processes the message,
	 * validates its authenticity, and executes the command it carries.
	 *
	 * @param[in]    message Signed message.
	 *
	 * @return None.
	 */
	void processSignedMsg(byte[] message) throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Deactivates the MDTP protection, using local PIN authentication instead of a signed deactivation message arriving
	 * from MDTP services. The pin is generated in the cloud, and received as part of a signed activation message.
	 * during MDTP activation. (See processSignedMsg).
	 *
	 * @param[in]  PIN of #MDTP_PIN_LEN digits.
	 *
	 * @return None.
	 */
	void deactivateLocal(String pin) throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Gets an identity token to identify a specific instance of MDTP activation or deactivation. The API will be available
	 * only When MDTP state is inactive. On each call, a new unique token shall be generated and persisted.
	 * Upon MDTP deactivation, the identity token is cleared.
	 *
	 * @return byte[] -- Buffer holding the identity token.
	 *
	 */
	byte[] getIdToken() throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Updates the MDTP with a given @xhyperref{RFC5280, RFC 5280} CRL.
	 *
	 * @param[in]  crl  Certificate Revocation List (CRL), according to RFC 5280.
	 * @param[in]  certificate  X509 certificate the CRL is signed with; identifies the signer of the CRL (the root CA or its
	 * subordinates CAs)
	 *
	 */
	void updateCrl(byte[] crl, byte[] certificate) throws RuntimeException;

	/**@ingroup fwlockfunctions
	 * Gets the HW recovery PIN of the device. The HW recovery PIN is used by the Bootloader, for recovering MDTP when
	 * its storage is badly damaged, so ISV PIN canoot be available.
	 *
	 * @return String -- A string holding the HW recovery PIN.
	 *
	 */
	String getHwRecoveryPin() throws RuntimeException;
}
