/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.os.Handler;

import com.android.internal.telephony.SmsUsageMonitor;

import com.qualcomm.qti.internal.telephony.QtiSmsSecurityService.SmsSecurityServiceCallback;

public class QtiSmsUsageMonitor extends SmsUsageMonitor {

    private final QtiSmsSecurityService mSmsSecurityService;

    public QtiSmsUsageMonitor(final Context context, final QtiSmsSecurityService service) {
        super(context);
        mSmsSecurityService = service;
    }

    @Override
    public void authorizeOutgoingSms(final PackageInfo packageInfo,
            final String destinationAddress,
            final String message,
            final SmsAuthorizationCallback callback,
            final Handler callbackHandler) {
        mSmsSecurityService.requestAuthorization(packageInfo, destinationAddress, message,
                new CallBackAdapter(callback), callbackHandler);
    }

    private static final class CallBackAdapter implements SmsSecurityServiceCallback {
        private final SmsAuthorizationCallback mTarget;
        public CallBackAdapter(final SmsAuthorizationCallback callback) {
            mTarget = callback;
        }

        @Override
        public void onAuthorizationResult(final boolean authorized) {
            mTarget.onAuthorizationResult(authorized);
        }
    }
}
