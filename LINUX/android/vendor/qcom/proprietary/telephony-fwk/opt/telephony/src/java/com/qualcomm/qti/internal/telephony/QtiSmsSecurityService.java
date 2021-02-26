/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.internal.telephony;

import android.Manifest.permission;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.RemoteException;
import android.telephony.Rlog;

import com.android.internal.R;
import com.android.internal.telephony.ISmsSecurityAgent;
import com.android.internal.telephony.ISmsSecurityService;
import com.android.internal.telephony.SmsAuthorizationRequest;

import java.util.concurrent.ConcurrentHashMap;

public class QtiSmsSecurityService extends ISmsSecurityService.Stub {

    public interface SmsSecurityServiceCallback {
        void onAuthorizationResult(boolean authorized);
    }

    public static final String SERVICE_NAME = "sms-sec";

    private static final String LOG_TAG = QtiSmsSecurityService.class.getSimpleName();

    private final ConcurrentHashMap<IBinder, PendingRequestRecord> mPendingRequests;

    private final Context mContext;

    private volatile SecurityAgentRecord mAgentRecord;

    private final long mTimeoutMs;

    public QtiSmsSecurityService(final Context context) {
        mPendingRequests = new ConcurrentHashMap<>();
        mTimeoutMs = context.getResources().getInteger(R.integer.config_sms_authorization_timeout_ms);
        mContext = context;
    }

    @Override
    public boolean register(final ISmsSecurityAgent agent) throws RemoteException {
        mContext.enforceCallingOrSelfPermission(permission.AUTHORIZE_OUTGOING_SMS, LOG_TAG);
        boolean registered = false;
        synchronized (this) {
            if (mAgentRecord != null && !mAgentRecord.mAgent.asBinder().equals(agent.asBinder())) {
                unregister(mAgentRecord.mAgent);
                mAgentRecord = null;
            }

            if (mAgentRecord == null) {
                mAgentRecord = new SecurityAgentRecord(agent, this);
                registered = true;
            }
        }
        return registered;
    }

    @Override
    public boolean unregister(final ISmsSecurityAgent agent) throws RemoteException {
        mContext.enforceCallingOrSelfPermission(permission.AUTHORIZE_OUTGOING_SMS, LOG_TAG);
        return doUnregisterSafe(agent);
    }

    @Override
    public boolean sendResponse(final SmsAuthorizationRequest request, final boolean authorized)
            throws RemoteException {
        mContext.enforceCallingOrSelfPermission(permission.AUTHORIZE_OUTGOING_SMS, LOG_TAG);
        final PendingRequestRecord record = mPendingRequests.remove(request.getToken());
        if (record != null) {
            record.invokeCallback(authorized);
        }
        return record != null;
    }

    public void requestAuthorization(final PackageInfo packageInfo,
            final String destinationAddress,
            final String message,
            final SmsSecurityServiceCallback callback,
            final Handler callbackHandler) {
        boolean requested = false;

        final IBinder token = new Binder();
        final SmsAuthorizationRequest request = new SmsAuthorizationRequest(this,
                token, packageInfo.packageName, destinationAddress, message);

        final PendingRequestRecord requestRecord = new PendingRequestRecord(this,
                request, callback, callbackHandler);
        mPendingRequests.put(token, requestRecord);

        final SecurityAgentRecord record = mAgentRecord;
        if (record != null) {
            requestRecord.scheduleTimeout(mTimeoutMs);
            try {
                record.mAgent.onAuthorize(request);
                requested = true;
            } catch (RemoteException e) {
                Rlog.e(LOG_TAG, "Unable to request SMS authentication.", e);
                requestRecord.cancelTimeout();
            }
        }

        if (!requested) {
            final PendingRequestRecord failedRequest = mPendingRequests.remove(request.getToken());
            if (failedRequest != null) {
                failedRequest.invokeCallback(true);
            }
        }
    }

    protected void onRequestTimeout(final SmsAuthorizationRequest request) {
        final PendingRequestRecord record = mPendingRequests.remove(request.getToken());
        if (record != null) {
            record.invokeTimeout();
        }
    }

    private boolean doUnregisterSafe(final ISmsSecurityAgent agent) {
        boolean unregistered = false;
        synchronized (this) {
            if (mAgentRecord != null && mAgentRecord.mAgent.asBinder().equals(agent.asBinder())) {
                mAgentRecord.mAgent.asBinder().unlinkToDeath(mAgentRecord, 0);
                mAgentRecord = null;
                unregistered = true;
            }
        }
        return unregistered;
    }

    private static final class SecurityAgentRecord implements DeathRecipient {

        private final ISmsSecurityAgent mAgent;
        private final QtiSmsSecurityService mService;

        public SecurityAgentRecord(final ISmsSecurityAgent agent,
                final QtiSmsSecurityService monitor) throws RemoteException {
            mAgent = agent;
            mService = monitor;
            mAgent.asBinder().linkToDeath(this, 0);
        }

        @Override
        public void binderDied() {
            mService.doUnregisterSafe(mAgent);
        }
    }

    private static final class PendingRequestRecord {

        private final SmsSecurityServiceCallback mCallback;
        private final Handler mHandler;
        private final Runnable mTimeoutCallback;

        public PendingRequestRecord(final QtiSmsSecurityService service,
                final SmsAuthorizationRequest request,
                final SmsSecurityServiceCallback callback,
                final Handler callbackHandler) {
            mCallback = callback;
            mHandler = callbackHandler;
            mTimeoutCallback = new Runnable() {
                @Override
                public void run() {
                    service.onRequestTimeout(request);
                }
            };
        }

        public void invokeCallback(boolean authorized) {
            cancelTimeout();
            mHandler.post(new Runnable() {
                @Override
                public void run() {
                    mCallback.onAuthorizationResult(authorized);
                }
            });
        }

        public void invokeTimeout() {
            mCallback.onAuthorizationResult(true);
        }

        public void scheduleTimeout(final long delayMillis) {
            mHandler.postDelayed(mTimeoutCallback, delayMillis);
        }

        public void cancelTimeout() {
            mHandler.removeCallbacks(mTimeoutCallback);
        }
    }
}
