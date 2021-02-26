/* Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.telephony.ims.stub.ImsMultiEndpointImplBase;
import android.util.Log;
import android.util.Pair;

import com.android.ims.ImsExternalCallState;
import com.android.ims.internal.IImsMultiEndpoint;
import com.android.ims.internal.IImsExternalCallStateListener;

import org.codeaurora.ims.parser.ImsViceParser;

import java.util.List;

public class ImsMultiEndpointImpl extends ImsMultiEndpointImplBase {
    private static final String TAG = "ImsMultiEndpointImpl";

    private final int EVENT_REFRESH_VICE_INFO = 1;
    private final int EVENT_CAPABILITIES_CHANGED = 2;

    private ImsSenderRxr mCi;
    private Context mContext;
    private ImsServiceSub mServiceSub;
    private Handler mHandler = new ImsMultiEndpointImplHandler();
    private IImsExternalCallStateListener mListener;

    // ImsViceParser handles parsing of raw xml
    private ImsViceParser mImsViceParser;

    private boolean mIsVideoSupported = false;
    private boolean mIsVoiceSupported = false;

    public ImsMultiEndpointImpl(ImsSenderRxr ci, Context context, ImsServiceSub serviceSub) {
        mContext = context;
        mCi = ci;
        mServiceSub = serviceSub;
        // Initialize the VICE parser
        mImsViceParser = new ImsViceParser(mContext);
        mCi.registerForViceRefreshInfo(mHandler, EVENT_REFRESH_VICE_INFO, null);
        mServiceSub.registerForCapabilitiesChanged(mHandler, EVENT_CAPABILITIES_CHANGED, null);
    }

    @Override
    public void setListener(IImsExternalCallStateListener listener) {
        mListener = listener;
    }

    @Override
    public void requestImsExternalCallStateInfo() {
        // TODO: Will add this implementation in consequent changes
        Log.d(TAG, "requestImsExternalCallStateInfo not supported yet");
    }

    private class ImsMultiEndpointImplHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "Message received: what = " + msg.what);
            AsyncResult ar = (AsyncResult) msg.obj;

            switch (msg.what) {
                case EVENT_REFRESH_VICE_INFO:
                    Log.d(TAG, "EVENT_REFRESH_VICE_INFO");
                    handleRefreshViceInfo(ar);
                    break;
                case EVENT_CAPABILITIES_CHANGED:
                    Log.d(TAG, "EVENT_CAPABILITIES_CHANGED");
                    updateCapabilities(ar);
                    break;
                default:
                    Log.d(TAG, "Unknown message = " + msg.what);
                    break;
            }
        }
    }

    private void handleRefreshViceInfo(AsyncResult ar) {
        if ((ar == null) || (ar.exception != null) || (ar.result == null)) {
            Log.d(TAG, "handleRefreshViceInfo exception");
            return;
        }

        ImsQmiIF.ViceInfo viceInfo = (ImsQmiIF.ViceInfo) ar.result;
        final com.google.protobuf.micro.ByteStringMicro refreshViceInfoUri = viceInfo
                .getViceInfoUri();

        if (refreshViceInfoUri != null &&
                refreshViceInfoUri.size() >= 0) {
            byte[] viceInfoBytes = new byte[refreshViceInfoUri.size()];
            refreshViceInfoUri.copyTo(viceInfoBytes, 0);
            processViceInfo(viceInfoBytes);
        } else {
            Log.d(TAG, "handleRefreshViceInfo: refreshViceInfoUri null");
        }
    }

    private void processViceInfo(byte[] viceInfoBytes) {
        // Call the parser to parse this byte info
        // And then notify the listener
        mImsViceParser.updateViceXmlBytes(viceInfoBytes);
        notifyViceInfo();
    }

    private void notifyViceInfo() {
        List<ImsExternalCallState> dialogIds = mImsViceParser.getCallPullInfo(
                mIsVideoSupported, mIsVoiceSupported);

        if (mListener != null) {
            try {
                mListener.onImsExternalCallStateUpdate(dialogIds);
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            Log.e(TAG, "handleRefreshViceInfo listener null");
        }
    }

    private void updateCapabilities(AsyncResult ar) {
        Pair<Boolean, Boolean> capabilites = (Pair<Boolean, Boolean>) ar.result;
        mIsVideoSupported = ((Boolean) (capabilites.first)).booleanValue();
        mIsVoiceSupported = ((Boolean) (capabilites.second)).booleanValue();
        Log.d(TAG, "updateCapabilities:: Video = " + mIsVideoSupported +
                ", Voice = " + mIsVoiceSupported);
        notifyViceInfo();
    }
}
