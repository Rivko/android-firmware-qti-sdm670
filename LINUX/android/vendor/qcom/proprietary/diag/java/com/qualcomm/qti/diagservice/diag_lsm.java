/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2016-2017 by Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag Java Lib Support

GENERAL DESCRIPTION

Implementation of Diag Java Interface

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
package com.qualcomm.qti.diagservice;

import android.util.Log;

public class diag_lsm {

	public static final int DIAG_PROC_MSM = 0;
	public static final int DIAG_PROC_MDM = 1;
	public static final int DIAG_CON_APSS = 0x0001; /* Bit mask for APSS */
	public static final int DIAG_CON_MPSS = 0x0002;  /* Bit mask for MPSS */
	public static final int DIAG_CON_LPASS = 0x0004;  /* Bit mask for LPASS */
	public static final int DIAG_CON_WCNSS = 0x0008; /* Bit mask for WCNSS */
	public static final int DIAG_CON_SENSORS =   0x0010; /* Bit mask for Sensors */
	public static final int DIAG_CON_WDSP = 0x0020;  /* Bit mask for WDSP */
	public static final int ENABLE =1;
	public static final int DISABLE = 0;
	public static final int IN_BUF_SIZE = 16384;
	/*===========================================================================
	FUNCTION   Diag_LSM_Init

	DESCRIPTION
	Initializes the Diag Legacy Mapping Layer. This should be called
	only once per process.

	DEPENDENCIES
	Successful initialization requires Diag CS component files to be present
	and accessible in the file system.

	RETURN VALUE
	FALSE = failure, else TRUE

	SIDE EFFECTS
	None

	===========================================================================*/
	public final static native boolean Diag_LSM_Init(long jarg1);

	/*===========================================================================

	FUNCTION    Diag_LSM_DeInit

	DESCRIPTION
	De-Initialize the Diag service.

	DEPENDENCIES
	None.

	RETURN VALUE
	FALSE = failure, else TRUE.
	Currently all the internal boolean return functions called by
	this function just returns TRUE w/o doing anything.

	SIDE EFFECTS
	None

	===========================================================================*/
	public final static native boolean Diag_LSM_DeInit();

}
