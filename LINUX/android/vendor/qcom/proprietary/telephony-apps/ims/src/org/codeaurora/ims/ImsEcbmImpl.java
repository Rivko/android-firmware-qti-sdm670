/* Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package org.codeaurora.ims;

import com.android.ims.internal.IImsEcbm;
import com.android.ims.internal.IImsEcbmListener;
import com.qualcomm.ims.utils.Log;

import java.util.Map;

import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.telephony.ims.stub.ImsEcbmImplBase;

public class ImsEcbmImpl extends ImsEcbmImplBase {

    private final int EVENT_ENTER_EMERGENCY_CALLBACK_MODE = 1;
    private final int EVENT_EXIT_EMERGENCY_CALLBACK_MODE = 2;

    private ImsServiceSub mServiceSub;
    private ImsSenderRxr mCi;
    private Handler mHandler = new ImsEcbmImplHandler();
    private IImsEcbmListener mListener;

    public ImsEcbmImpl(ImsServiceSub serviceSub, ImsSenderRxr ci) {
        mServiceSub = serviceSub;
        mCi = ci;
        mCi.setEmergencyCallbackMode(mHandler, EVENT_ENTER_EMERGENCY_CALLBACK_MODE, null);
        mCi.registerForExitEmergencyCallbackMode(mHandler, EVENT_EXIT_EMERGENCY_CALLBACK_MODE,
                null);
    }

    /**
     * Set the ECBM listener
     * @param listener - registration listener
     */
    public void setListener(IImsEcbmListener listener) {
        mListener = listener;
    }

    public void exitEmergencyCallbackMode() {
        mCi.exitEmergencyCallbackMode(Message.obtain(mHandler, EVENT_EXIT_EMERGENCY_CALLBACK_MODE));
    }

    /**
     * Local utility to start a new thread and then call ECBM call back
     * @param tracker object that contains handle to ECBM call back
     * @param isEntered
     */
    private void createEcbmCallBackThread(final IImsEcbmListener listener,
            final boolean isEntered) {
        final Runnable r = new Runnable() {
            @Override
            public void run() {
                try {
                    if (isEntered) {
                        listener.enteredECBM();
                    } else {
                        listener.exitedECBM();
                    }
                } catch (RemoteException e) {
                    Log.i(this, "RemoteException @createEcbmCallBackThread " + e);
                }
            }
        };
        Thread t = new Thread(r, this + "ImsEcbmImpl:EcbmCallBackThread");
        t.start();
    }

    // Handler for tracking requests/UNSOLs to/from ImsSenderRxr.
    private class ImsEcbmImplHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Log.i(this, "Message received: what = " + msg.what);

            switch (msg.what) {
                case EVENT_ENTER_EMERGENCY_CALLBACK_MODE:
                    Log.i(this, "EVENT_ENTER_EMERGENCY_CALLBACK_MODE");
                    createEcbmCallBackThread(mListener, true);
                    break;
                case EVENT_EXIT_EMERGENCY_CALLBACK_MODE:
                    Log.i(this, "EVENT_EXIT_EMERGENCY_CALLBACK_MODE");
                    createEcbmCallBackThread(mListener, false);
                    break;
                default:
                    Log.i(this, "Unknown message = " + msg.what);
                    break;
            }
        }
    }
}
