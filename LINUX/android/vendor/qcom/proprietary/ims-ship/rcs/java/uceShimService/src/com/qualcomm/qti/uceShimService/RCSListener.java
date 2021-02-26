/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.uceShimService;

import com.android.ims.internal.uce.uceservice.ImsUceManager;
import com.qualcomm.qti.uceservice.V1_0.*;

import android.util.Log;
import android.os.RemoteException;

public class RCSListener extends IUceListener.Stub { //extends HIDL IUceListener

    public static boolean isRCSRegistered = false;
    com.android.ims.internal.uce.uceservice.IUceListener mAidlUceListener = null;
    private final static String logTAG = "RCSService Listener";

    public void onStatusChange(UceStatus status, String iccId) {
        /*
         //TODO: notify UI of iccId supported/UP/DOWN
           if(subId == 0) {
            // if UCE_SERVICE_UNAVAILABLE, HIDL is not working properly.
			try {
			mAidlUceListener.setStatus(status.status);
			}
			catch (RemoteException e) {
				Log.d(logTAG,"Remote Exception");
			}
        } */
    }

    public void setAidlUceListener(com.android.ims.internal.uce.uceservice.IUceListener aidlUceListener) {
	mAidlUceListener = aidlUceListener;
    }
}
