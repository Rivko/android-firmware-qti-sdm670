/*
 * Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import android.Manifest;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.telephony.ims.stub.ImsConfigImplBase;

import com.android.ims.ImsConfig;
import com.android.ims.ImsConfigListener;
import com.android.ims.internal.IImsConfig;
import com.android.internal.os.SomeArgs;
import com.qualcomm.ims.utils.Log;

public class ImsConfigImpl extends ImsConfigImplBase {
    private static final int EVENT_ACCESS_PROVISIONED_VAL = 1;
    private static final int EVENT_SET_FEATURE_VALUE = 2;
    private static final int EVENT_GET_WIFI_CALLING_STATUS = 6;
    private static final int EVENT_SET_WIFI_CALLING_STATUS = 7;

    private ImsServiceSub mServiceSub;
    private ImsSenderRxr mCi;
    private Handler mHandler;
    private Context mContext;

    // Permissions used by class.
    private final String MODIFY_PHONE_STATE = Manifest.permission.MODIFY_PHONE_STATE;
    private final String READ_PRIVILEGED_PHONE_STATE = Manifest.permission.READ_PRIVILEGED_PHONE_STATE;
    private final String READ_PHONE_STATE = Manifest.permission.READ_PHONE_STATE;

    /**
     * Creates the Ims Config interface object for a sub.
     * @param senderRxr
     */
    public ImsConfigImpl(ImsServiceSub serviceSub, ImsSenderRxr senderRxr, Context context) {
        mServiceSub = serviceSub;
        mCi = senderRxr;
        mContext = context;
        HandlerThread handlerThread = new HandlerThread("ImsConfigImplHandler");
        handlerThread.start();
        mHandler = new ImsConfigImplHandler(handlerThread.getLooper());
    }

    /* Wrapper class to encapsulate the arguments and listener to the setFeatureValue and
     * getFeatureValue APIs
     */
    private static final class FeatureAccessWrapper {
        public int feature;
        public int network;
        public int value;
        public ImsConfigListener listener;
        public FeatureAccessWrapper(int feature, int network, int value,
                ImsConfigListener listener) {
            this.feature = feature;
            this.network = network;
            this.listener = listener;
            this.value = value;
        }
    }

    //Handler for tracking requests sent to ImsSenderRxr.
    private class ImsConfigImplHandler extends Handler {
        public ImsConfigImplHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            Log.i(this, "Message received: what = " + msg.what);
            AsyncResult ar = (AsyncResult) msg.obj;

            switch (msg.what) {
                case EVENT_GET_WIFI_CALLING_STATUS:
                    onGetWifiCallingStatusDone(getImsConfigListener(ar), ar);
                    break;
                case EVENT_SET_WIFI_CALLING_STATUS:
                    onSetWifiCallingStatusDone(getImsConfigListener(ar), ar);
                    break;
                case EVENT_SET_FEATURE_VALUE:
                    onSetFeatureResponseDone(getImsConfigListener(ar), ar);
                    break;
                case EVENT_ACCESS_PROVISIONED_VAL:
                    onAccessProvisionedValDone(ar, msg.what);
                    break;
                default:
                    Log.e(this, "handleMessage: unhandled message");
            }
        }
    }

    private void onAccessProvisionedValDone(AsyncResult ar, int accessType) {
        SomeArgs response = (SomeArgs)ar.userObj;
        response.arg2 = getOperationStatus(ar.exception == null);
        response.arg3 = ar.result;
        synchronized(response) {
            Log.i(this, "Notifyall");
            response.notifyAll();
        }
    }

    private void onGetPacketCountDone(ImsConfigListener imsConfigListener, AsyncResult ar) {
        if (imsConfigListener != null) {
            try {
                int status = getOperationStatus(ar.exception == null);
                long result = ar.result == null ? 0 : (long)ar.result;
                //TODO - Compilation stub imsConfigListener.onGetPacketCount(status, result);
            } catch (Throwable t) {
                Log.e(this, "onGetPacketCountDone " + t);
            }
        } else {
            Log.v(this, "onGetPacketCountDone listener is null");
        }
    }

    private void onGetPacketErrorCountDone(ImsConfigListener imsConfigListener, AsyncResult ar) {
        if (imsConfigListener != null) {
            try {
                int status = getOperationStatus(ar.exception == null);
                long result = ar.result == null ? 0 : (long)ar.result;
                //TODO - Compilation stub imsConfigListener.onGetPacketErrorCount(status, result);
            } catch (Throwable t) {
                Log.e(this, "onGetPacketErrorCountDone " + t);
            }
        } else {
            Log.v(this, "onGetPacketErrorCountDone listener is null");
        }
    }

    private void onGetWifiCallingStatusDone(ImsConfigListener imsConfigListener, AsyncResult ar) {
        try {
            int status = getOperationStatus(ar.exception == null);
            ImsQmiIF.WifiCallingInfo wifiCallingInfo = (ImsQmiIF.WifiCallingInfo)ar.result;
            //TODO - Compilation stub imsConfigListener.onGetWifiCallingPreference(status, wifiCallingInfo.getStatus(),
            //        wifiCallingInfo.getPreference());
        } catch (Throwable t) {
            Log.e(this, "onGetWifiCallingStatusDone " + t);
        }
    }

    private void onSetWifiCallingStatusDone(ImsConfigListener imsConfigListener, AsyncResult ar) {
        try {
            int status = getOperationStatus(ar.exception == null);
            //TODO - Compilation stub imsConfigListener.onSetWifiCallingPreference(status);
        } catch (Throwable t) {
            Log.e(this, "onSetWifiCallingStatusDone " + t);
        }
    }

    private void onSetFeatureResponseDone(ImsConfigListener imsConfigListener, AsyncResult ar) {
        if (imsConfigListener != null) {
            try {
                int status = getOperationStatus(ar.exception == null);
                FeatureAccessWrapper f = (FeatureAccessWrapper)ar.userObj;
                imsConfigListener.onSetFeatureResponse(f.feature, f.network, f.value, status);
            } catch (Throwable t) {
                Log.e(this, "onSetFeatureResponseDone " + t);
            }
        } else {
            Log.v(this, "onSetFeatureResponseDone listener is null");
        }
    }

    private int getOperationStatus(boolean status) {
        return status ? ImsConfig.OperationStatusConstants.SUCCESS :
                ImsConfig.OperationStatusConstants.FAILED;
    }

    private ImsConfigListener getImsConfigListener(AsyncResult ar) {
        if (ar == null ) {
            Log.e(this, "AsyncResult is null.");
        } else if (ar.userObj instanceof ImsConfigListener) {
            return (ImsConfigListener)ar.userObj;
        } else if (ar.userObj instanceof FeatureAccessWrapper &&
                ((FeatureAccessWrapper)(ar.userObj)).listener instanceof ImsConfigListener) {
            return (ImsConfigListener)((FeatureAccessWrapper)(ar.userObj)).listener;
        }

        Log.e(this, "getImsConfigListener returns null");
        return null;
    }

    private void enforceReadPhoneState(String fn) {
        if (mContext.checkCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE)
                != PackageManager.PERMISSION_GRANTED
                && mContext.checkCallingOrSelfPermission(READ_PHONE_STATE)
                != PackageManager.PERMISSION_GRANTED) {
            mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, fn);
        }
    }

    //TODO - Compilation stub @Override
    public void getVideoQuality(ImsConfigListener imsConfigListener) {
        enforceReadPhoneState("getVideoQuality");
        //Dummy implementation need to remove this once google removed this API
        Log.i(this, "getVideoQuality is deprecated API, please use getProvisionedValue");
    }

    //TODO - Compilation stub @Override
    public void setVideoQuality(int quality, ImsConfigListener imsConfigListener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setVideoQuality");
        //Dummy implementation need to remove this once google removed this API
        Log.i(this, "setVideoQuality is deprecated API, please use setProvisionedValue");
    }

    /**
     * Query for current wifi calling info.
     */
    //TODO - Compilation stub @Override
    public void getWifiCallingPreference(ImsConfigListener imsConfigListener) {
        enforceReadPhoneState("getWifiCallingPreference");
        Log.i(this, "getWifiCallingPreference");
        mCi.getWifiCallingPreference(mHandler.obtainMessage(EVENT_GET_WIFI_CALLING_STATUS,
                imsConfigListener));
    }

    /**
     * Set current wifi calling info.
     */
    //TODO - Compilation stub @Override
    public void setWifiCallingPreference(int wifiCallingStatus, int wifiCallingPreference,
            ImsConfigListener imsConfigListener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setWifiCallingPreference");
        Log.i(this, "setWifiCallingPreference");
        mCi.setWifiCallingPreference(wifiCallingStatus, wifiCallingPreference,
                mHandler.obtainMessage(EVENT_SET_WIFI_CALLING_STATUS, imsConfigListener));
    }

    // All below functions need to be implemented on demand basis
    //TODO: Remove Comment added for bringup @Override
    public int getMasterValue(int item) {
        //Dummy implementation
        return 0;
    }

    //TODO: Remove Comment added for bringup @Override
    public String getMasterStringValue(int item) {
        //Dummy implementation
        return null;
    }

    protected Object sendRequest(int requestType, int item, int event, boolean boolVal,
            int intVal, String strVal) {
        //SomeArgs semantics
        //arg1 - item
        //arg2 - result of operation - pass/fail
        //arg3 - value returned by function for get operation
        SomeArgs request = SomeArgs.obtain();
        try {
            request.arg1 = item;
            mCi.sendConfigRequest(requestType, ImsCallUtils.convertImsConfigToProto(item),
                    boolVal, intVal, strVal, ImsQmiIF.CONFIG_NO_ERR,
                    mHandler.obtainMessage(event, request));

            synchronized(request) {
                while(request.arg2 == null) {
                    try {
                        request.wait();
                    } catch (InterruptedException e) {
                        //Do Nothing and wait
                    }
                }
            }
        } catch (IllegalArgumentException e) {
            request.arg2 = ImsConfig.OperationStatusConstants.UNKNOWN;
        } finally {
            return request;
        }
    }

    public int getProvisionedValue(int item) {
        enforceReadPhoneState("getProvisionedValue");
        if (!ImsCallUtils.isConfigRequestValid(item, ImsCallUtils.CONFIG_TYPE_INT)) {
            Log.e(this, "Invalid API request for item");
            return ImsConfig.OperationStatusConstants.UNKNOWN;
        }
        SomeArgs result = (SomeArgs)sendRequest(ImsQmiIF.REQUEST_GET_IMS_CONFIG, item,
                EVENT_ACCESS_PROVISIONED_VAL, false, 0, null);
        int retVal = ((int)result.arg2 != ImsConfig.OperationStatusConstants.SUCCESS ?
                    ImsConfig.OperationStatusConstants.UNKNOWN : (int)result.arg3);
        result.recycle();
        return retVal;
    }

    public String getProvisionedStringValue(int item) {
        enforceReadPhoneState("getProvisionedStringValue");
        if (!ImsCallUtils.isConfigRequestValid(item, ImsCallUtils.CONFIG_TYPE_STRING)) {
            Log.e(this, "Invalid API request for item");
            return null;
        }
        SomeArgs result = (SomeArgs)sendRequest(ImsQmiIF.REQUEST_GET_IMS_CONFIG, item,
                EVENT_ACCESS_PROVISIONED_VAL, false, 0, null);
        String retVal = ((int)result.arg2 != ImsConfig.OperationStatusConstants.SUCCESS ?
                    null : (String)result.arg3);
        result.recycle();
        return retVal;
    }

    @Override
    public int setProvisionedValue(int item, int value) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setProvisionedValue");
        if (item == ImsConfig.ConfigConstants.VOICE_OVER_WIFI_ROAMING) {
            Log.w(this, "VoWiFi Roaming: value can't be changed");
            return ImsConfig.OperationStatusConstants.FAILED;
        }
        if (!ImsCallUtils.isConfigRequestValid(item, ImsCallUtils.CONFIG_TYPE_INT)) {
            Log.e(this, "Invalid API request for item");
            return ImsConfig.OperationStatusConstants.FAILED;
        }
        SomeArgs result = (SomeArgs)sendRequest(ImsQmiIF.REQUEST_SET_IMS_CONFIG, item,
                EVENT_ACCESS_PROVISIONED_VAL, false, value, null);
        int retVal = (int)result.arg2;
        result.recycle();
        return retVal;
    }

    @Override
    public int setProvisionedStringValue(int item, String value) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setProvisionedStringValue");
        if (!ImsCallUtils.isConfigRequestValid(item, ImsCallUtils.CONFIG_TYPE_STRING)) {
            Log.e(this, "Invalid API request for item");
            return ImsConfig.OperationStatusConstants.FAILED;
        }
        SomeArgs result = (SomeArgs)sendRequest(ImsQmiIF.REQUEST_SET_IMS_CONFIG, item,
                EVENT_ACCESS_PROVISIONED_VAL, false, 0, value);
        int retVal = (int)result.arg2;
        result.recycle();
        return retVal;
    }

    @Override
    public void getFeatureValue(int feature, int network, ImsConfigListener listener) {
        enforceReadPhoneState("getFeatureValue");
        //Dummy implementation
    }

    @Override
    public boolean getVolteProvisioned() {
        enforceReadPhoneState("getVolteProvisioned");
        //TODO: Dummy implementation added for bringup
        return true;
    }

    @Override
    public void setFeatureValue(int feature, int network, int value, ImsConfigListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setFeatureValue");
        int srvType = ImsQmiIF.CALL_TYPE_VOICE;
        if (feature == ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE) {
            srvType = ImsQmiIF.CALL_TYPE_VT;
        }
        int enabled = ImsQmiIF.STATUS_DISABLED;
        if (value == ImsConfig.FeatureValueConstants.ON) {
            enabled = ImsQmiIF.STATUS_ENABLED;
        }
        int act = ImsQmiIF.RADIO_TECH_LTE;
        if (network == TelephonyManager.NETWORK_TYPE_IWLAN) {
            act = ImsQmiIF.RADIO_TECH_IWLAN;
        }
        if (network == TelephonyManager.NETWORK_TYPE_LTE ||
                network == TelephonyManager.NETWORK_TYPE_IWLAN) {
            Log.i(this, "SetServiceStatus = " + srvType + " " + network + " " + enabled);
            mCi.setServiceStatus(mHandler.obtainMessage(EVENT_SET_FEATURE_VALUE,
                new FeatureAccessWrapper(feature, network, value, listener)),
                srvType, act, enabled, 0);
        }
    }
}
