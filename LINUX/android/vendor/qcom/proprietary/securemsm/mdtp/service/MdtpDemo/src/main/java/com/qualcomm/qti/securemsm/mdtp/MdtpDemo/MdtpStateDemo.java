/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.content.Context;
import java.io.Serializable;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;


public class MdtpStateDemo implements Serializable{
	private static final long serialVersionUID = 1L;
	public MdtpSystemStateDemo mSystemState;	/**< MDTP system state enumeration. */
	public MdtpAppStateDemo mAppState;		/**< MDTP applicative state. */

	/**
	 * MDTP system  state.
	 */
	public enum MdtpSystemStateDemo implements Serializable{
		DISABLED,	/**< MDTP was never activated on the device. */
		INACTIVE,	/**< MDTP was once activated, but now it is inactive. */
		ACTIVE,		/**< MDTP is active. */
		TAMPERED,	/**< MDTP storage is tampered and needs local deactivation with ISV PIN. */
		INVALID		/**< MDTP storage is badly corrupted, and needs recovery with HW recovery PIN. */
	}

	/**
	 * MDTP applicative state.
	 */
	public class MdtpAppStateDemo implements Serializable{
		public boolean mSimLocked; 	/**< Device is SIM locked or not. */
		public boolean mEmergencyOnly; 	/**< Device is in Emergency only mode or not. */
	}

	public MdtpStateDemo() {
		mSystemState = MdtpSystemStateDemo.DISABLED;
		mAppState = new MdtpAppStateDemo();
	}

	public MdtpStateDemo(MdtpSystemStateDemo systemState, MdtpAppStateDemo appState) {
		mSystemState = systemState;
		mAppState = appState;
	}

	public MdtpStateDemo(MdtpState state) {
		switch(state.mSystemState){
			case DISABLED:
				mSystemState = MdtpSystemStateDemo.DISABLED;
				break;
			case INACTIVE:
				mSystemState = MdtpSystemStateDemo.INACTIVE;
				break;
			case ACTIVE:
				mSystemState = MdtpSystemStateDemo.ACTIVE;
				break;
			case TAMPERED:
				mSystemState = MdtpSystemStateDemo.TAMPERED;
				break;
			case INVALID:
				mSystemState = MdtpSystemStateDemo.INVALID;
				break;
			default:
				mSystemState = MdtpSystemStateDemo.INVALID;
				break;
		}
		mAppState = new MdtpAppStateDemo();
		mAppState.mSimLocked = state.mAppState.mSimLocked;
		mAppState.mEmergencyOnly = state.mAppState.mEmergencyOnly;
	}
}
