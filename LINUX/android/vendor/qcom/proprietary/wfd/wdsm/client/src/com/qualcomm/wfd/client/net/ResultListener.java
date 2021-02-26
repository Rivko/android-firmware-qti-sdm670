/*
* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.util.Log;

public interface ResultListener<ResultType, ErrorType> {

    String TAG = "ResultListener";

    void onSuccess(ResultType result);
    void onFailure(ErrorType err);

    ResultListener NullListener = new ResultListener() {
        @Override
        public void onSuccess(Object result) {
            Log.v("NullListener", "onSuccess " + result);
        }

        @Override
        public void onFailure(Object err) {
            Log.e("NullListener", "onFailure " + err);
        }
    };

    class AnySuccessResultListener implements ResultListener {
        private final ResultListener mListener;
        private final int mCount;
        private boolean mCalled;
        private int mIndex;

        AnySuccessResultListener(ResultListener rl, int count) {
            mListener = rl;
            mCount = count;
        }

        @Override
        public void onSuccess(Object result) {
            if (!mCalled) {
                mListener.onSuccess(result);
                mCalled = true;
            }
        }

        @Override
        public void onFailure(Object err) {
            Log.w(TAG, "AnySuccessResultListener receives a error: " + err);
            if (mCalled) {
                Log.w(TAG, "Skip the error");
                return;
            }
            mIndex++;
            if (mIndex == mCount) {
                mListener.onFailure(err);
                mCalled = true;
            }
        }
    }
}
