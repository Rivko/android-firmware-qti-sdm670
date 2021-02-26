/* Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package org.codeaurora.ims;

import com.android.ims.ImsCallForwardInfo;
import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsSsData;
import com.android.ims.ImsSsInfo;
import com.android.ims.internal.IImsUt;
import com.android.ims.internal.IImsUtListener;
import com.qualcomm.ims.utils.Log;

import android.os.Bundle;
import android.telephony.ims.stub.ImsUtListenerImplBase;

public class ImsUtListenerProxy extends ImsUtListenerImplBase {
    public IImsUtListener mListener;

    /**
     * Notifies the result of the supplementary service configuration update.
     */
    public void utConfigurationUpdated(final IImsUt ut, final int id) {
        if (mListener != null) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        mListener.utConfigurationUpdated(ut, id);
                    } catch (Throwable t) {
                        handleError(t, "utConfigurationUpdated()");
                    }
                }
            };
            startThread(r);
        }
    }

    public void utConfigurationUpdateFailed(final IImsUt ut,
                                            final int id,
                                            final ImsReasonInfo error) {
        if (mListener != null) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        mListener.utConfigurationUpdateFailed(ut, id, error);
                    } catch (Throwable t) {
                        handleError(t, "utConfigurationUpdateFailed()");
                    }
                }
            };
            startThread(r);
        }
    }

    /**
     * Notifies the result of the supplementary service configuration query.
     */
    public void utConfigurationQueried(final IImsUt ut,
                                       final int id,
                                       final Bundle ssInfo) {
        if (mListener != null) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        mListener.utConfigurationQueried(ut, id, ssInfo);
                    } catch (Throwable t) {
                        handleError(t, "utConfigurationQueried()");
                    }
                }
            };
            startThread(r);
        }
    }

    public void utConfigurationQueryFailed(final IImsUt ut,
                                           final int id,
                                           final ImsReasonInfo error) {
        if (mListener != null) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        mListener.utConfigurationQueryFailed(ut, id, error);
                    } catch (Throwable t) {
                        handleError(t, "utConfigurationQueryFailed()");
                    }
                }
            };
            startThread(r);
        }
    }

    /**
     * Notifies the status of the call barring supplementary service.
     */
    public void utConfigurationCallBarringQueried(final IImsUt ut,
                                                  final int id,
                                                  final ImsSsInfo[] cbInfo) {
        if (mListener != null) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        mListener.utConfigurationCallBarringQueried(ut, id, cbInfo);
                    } catch (Throwable t) {
                        handleError(t, "utConfigurationCallBarringQueried()");
                    }
                }
            };
            startThread(r);
        }
    }

    /**
     * Notifies the status of the call forwarding supplementary service.
     */
    public void utConfigurationCallForwardQueried(final IImsUt ut,
                                                  final int id,
                                                  final ImsCallForwardInfo[] cfInfo) {
        if (mListener != null) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        mListener.utConfigurationCallForwardQueried(ut, id, cfInfo);
                    } catch (Throwable t) {
                        handleError(t, "utConfigurationCallForwardQueried()");
                    }
                }
            };
            startThread(r);
        }
    }

    /**
     * Notifies the status of the call waiting supplementary service.
     */
    public void utConfigurationCallWaitingQueried(final IImsUt ut,
                                                  final int id,
                                                  final ImsSsInfo[] cwInfo) {
        if (mListener != null) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        mListener.utConfigurationCallWaitingQueried(ut, id, cwInfo);
                    } catch (Throwable t) {
                        handleError(t, "utConfigurationCallWaitingQueried()");
                    }
                }
            };
            startThread(r);
        }
    }

    /**
     * Notifies the supplementary service indication.
     */
    public void onSupplementaryServiceIndication(final ImsSsData ssData) {
        if (mListener != null) {
            final Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        mListener.onSupplementaryServiceIndication(ssData);
                    } catch (Throwable t) {
                        handleError(t, "onSupplementaryServiceIndication()");
                    }
                }
            };
            startThread(r);
        }
    }

    void startThread(Runnable r) {
        Thread t = new Thread(r, "ImsUtListenerProxyThread");
        t.start();
    }

    private void handleError(Throwable t, String message) {
        Log.e(this, t + " " + message);
     }
}
