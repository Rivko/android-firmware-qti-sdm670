/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.callenhancement;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

public class ThreadRequestHelper {
    private static final class ThreadRequest {
        Object argument;
        Object result;
        public ThreadRequest(Object args) {
            argument = args;
        }
    }

    public static Object sendRequest(Handler receiver, int cmd, Object args) {
        if (Looper.myLooper() == receiver.getLooper()) {
            throw new RuntimeException("This method will deadlock if called from main thread");
        }

        ThreadRequest request = new ThreadRequest(args);
        receiver.obtainMessage(cmd, request).sendToTarget();

        // Wait for the request to complete
        synchronized (request) {
            while (request.result == null) {
                try {
                    request.wait();
                } catch (InterruptedException e) {
                    // Do nothing, go back and wait until the request is complete
                }
            }
        }
        return request.result;
    }

    public static void populateResult(Message msg, Object result) {
        ThreadRequest request = (ThreadRequest) msg.obj;
        do {
            if (request == null) throw new RuntimeException("incorrect request with null");
            request.result = result;
            if (request.result == null) request.result = "";
        } while (false);

        // Wake up the requesting thread
        synchronized (request) {
            request.notifyAll();
        }
    }
}
