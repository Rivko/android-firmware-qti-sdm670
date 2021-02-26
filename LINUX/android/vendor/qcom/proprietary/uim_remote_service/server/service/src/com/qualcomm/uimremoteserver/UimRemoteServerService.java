 /******************************************************************************
  @file    UimRemoteServerService.java

  ---------------------------------------------------------------------------
  Copyright (c) 2014,2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.uimremoteserver;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.HwBinder;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;

import android.hidl.manager.V1_0.IServiceManager;
import android.hidl.manager.V1_0.IServiceNotification;

import android.content.Context;
import android.telephony.TelephonyManager;
import android.content.pm.Signature;
import android.content.pm.PackageManager;
import java.util.Map;
import java.util.HashMap;
import java.util.Collections;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.ArrayList;
import java.io.IOException;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import android.content.res.XmlResourceParser;

import java.io.*;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.security.MessageDigest;

import vendor.qti.hardware.radio.uim_remote_server.V1_0.IUimRemoteServiceServer;
import vendor.qti.hardware.radio.uim_remote_server.V1_0.IUimRemoteServiceServerResponse;
import vendor.qti.hardware.radio.uim_remote_server.V1_0.IUimRemoteServiceServerIndication;
import vendor.qti.hardware.radio.uim_remote_server.V1_0.UimRemoteServiceServerApduType;
import vendor.qti.hardware.radio.uim_remote_server.V1_0.UimRemoteServiceServerResultCode;

public class UimRemoteServerService extends Service {
    private final String LOG_TAG = "UimRemoteServerService";

    public static class UimRemoteError {

        public static final int UIM_REMOTE_SUCCESS = 0;

        public static final int UIM_REMOTE_ERROR = 1;
    }

    private static class Application {
        public String name;
        public String key;
        public boolean parsingFail;
    }

    private Map UimRemoteServerWhiteList;

    private Context context;

    private final AtomicInteger mToken = new AtomicInteger(0);

    private int simSlots = TelephonyManager.getDefault().getSimCount();

    private UimRemoteServiceServer[] mUimRemoteServerHals = new UimRemoteServiceServer[simSlots];
    IUimRemoteServerServiceCallback mCb = null;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(LOG_TAG, "onCreate()");
        context = this;
        Log.i(LOG_TAG, "simCount: "+ simSlots);
        for(int i = 0; i < simSlots; i++) {
            mUimRemoteServerHals[i] = new UimRemoteServiceServer(i);
        }

        //initing whitelist
        getWhiteList();
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(LOG_TAG, "onBind()");
        return mBinder;
    }

    @Override
    public void onDestroy() {
        Log.i(LOG_TAG, "onDestroy()");
        for(int i = 0; i < simSlots; i++) {
            mUimRemoteServerHals[i].toDestroy();
        }
        stopSelf();
        super.onDestroy();
    }

    private final IUimRemoteServerService.Stub mBinder = new IUimRemoteServerService.Stub() {
        public int registerCallback(IUimRemoteServerServiceCallback cb) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.i(LOG_TAG, "register callback: Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            UimRemoteServerService.this.mCb = cb;
            if (cb == null) {
                Log.d(LOG_TAG, "registerCallback() - null cb");
            }
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int deregisterCallback(IUimRemoteServerServiceCallback cb) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "deregister Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            UimRemoteServerService.this.mCb = null;
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int uimRemoteServerConnectReq(int slot, int maxMessageSize) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "Connect Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if(slot >= simSlots) {
                Log.e(LOG_TAG, "Sim Slot not supported!" + slot);
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot] == null) {
                Log.e(LOG_TAG, "server is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot].getService() == null) {
                Log.e(LOG_TAG, "service is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }

            Log.d(LOG_TAG, "uimRemoteServerConnectReq() - maxMessageSize: "
             + maxMessageSize + "slot: " + slot);

            try {
                mUimRemoteServerHals[slot].getService().uimRemoteServiceServerConnectReq(
                 mToken.incrementAndGet(), maxMessageSize);
            } catch(RemoteException ex) {
                Log.e(LOG_TAG, "uimRemoteEvent: exception" + ex);
            }
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int uimRemoteServerDisconnectReq(int slot) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "Disconnect Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if(slot >= simSlots) {
                Log.e(LOG_TAG, "Sim Slot not supported!" + slot);
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot] == null) {
                Log.e(LOG_TAG, "server is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot].getService() == null) {
                Log.e(LOG_TAG, "service is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }

            Log.d(LOG_TAG, "uimRemoteServerDisconnectReq() slot: " + slot);

            try {
                mUimRemoteServerHals[slot].getService().uimRemoteServiceServerDisconnectReq(
                 mToken.incrementAndGet());
            } catch(RemoteException ex) {
                Log.e(LOG_TAG, "uimRemoteEvent: exception" + ex);
            }
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int uimRemoteServerApduReq(int slot, byte[] cmd) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "Apdu Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if(slot >= simSlots) {
                Log.e(LOG_TAG, "Sim Slot not supported!" + slot);
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot] == null) {
                Log.e(LOG_TAG, "server is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot].getService() == null) {
                Log.e(LOG_TAG, "service is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }

            Log.d(LOG_TAG, "uimRemoteServerApduReq() - cmd length: " + cmd.length + " slot: " + slot);

            try {
                mUimRemoteServerHals[slot].getService().uimRemoteServiceServerApduReq(
                 mToken.incrementAndGet(), UimRemoteServiceServerApduType.UIM_REMOTE_SERVICE_SERVER_APDU,
                 primitiveArrayToArrayList(cmd));
            } catch(RemoteException ex) {
                Log.e(LOG_TAG, "uimRemoteEvent: exception" + ex);
            }
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int uimRemoteServerTransferAtrReq(int slot) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "ATR Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if(slot >= simSlots) {
                Log.e(LOG_TAG, "Sim Slot not supported!" + slot);
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot] == null) {
                Log.e(LOG_TAG, "server is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot].getService() == null) {
                Log.e(LOG_TAG, "service is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }

            Log.d(LOG_TAG, "uimRemoteServerTransferAtrReq() slot: " + slot);

            try {
                mUimRemoteServerHals[slot].getService().uimRemoteServiceServerTransferAtrReq(
                 mToken.incrementAndGet());
            } catch(RemoteException ex) {
                Log.e(LOG_TAG, "uimRemoteEvent: exception" + ex);
            }
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int uimRemoteServerPowerReq(int slot, boolean state) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "Power Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if(slot >= simSlots) {
                Log.e(LOG_TAG, "Sim Slot not supported!" + slot);
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot] == null) {
                Log.e(LOG_TAG, "server is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot].getService() == null) {
                Log.e(LOG_TAG, "service is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }

            Log.d(LOG_TAG, "uimRemoteServerPowerReq() - state: " + state + " slot: " + slot);

            try {
                mUimRemoteServerHals[slot].getService().uimRemoteServiceServerPowerReq(
                 mToken.incrementAndGet(), state);
            } catch(RemoteException ex) {
                Log.e(LOG_TAG, "uimRemoteEvent: exception" + ex);
            }
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int uimRemoteServerResetSimReq(int slot) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "Reset Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if(slot >= simSlots) {
                Log.e(LOG_TAG, "Sim Slot not supported!" + slot);
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot] == null) {
                Log.e(LOG_TAG, "server is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteServerHals[slot].getService() == null) {
                Log.e(LOG_TAG, "service is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }

            Log.d(LOG_TAG, "uimRemoteServerResetSimReq() slot: " + slot);

            try {
                mUimRemoteServerHals[slot].getService().uimRemoteServiceServerResetSimReq(
                 mToken.incrementAndGet());
            } catch(RemoteException ex) {
                Log.e(LOG_TAG, "uimRemoteEvent: exception" + ex);
            }
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }
    };

    private class UimRemoteServiceServer {
        private final String mInstanceName;
        private int mInstance;
        private final AtomicLong mUimRemoteServerCookie = new AtomicLong(0);
        private boolean mIsDisposed = false;

        private final UimRemoteServerDeathRecipient mUimRemotServerDeathRecipient =
                          new UimRemoteServerDeathRecipient();
        private final ServiceNotification mServiceNotification = new ServiceNotification();
        private final IUimRemoteServiceServerResponse mUimRemoteServerResp =
                          new UimRemoteServerResponse();
        private final IUimRemoteServiceServerIndication mUimRemoteServerInd =
                          new UimRemoteServerIndication();
        private IUimRemoteServiceServer mUimremoteServer;

        private final class ServiceNotification extends IServiceNotification.Stub {
            @Override
            public void onRegistration(String fqName, String name, boolean preexisting) {
                if (!mInstanceName.equals(name) || mIsDisposed) {
                    Log.d(LOG_TAG, "onRegistration: Ignoring.");
                    return;
                }
                Log.d(LOG_TAG, "ServiceNotification - onRegistration");
                initUimRemoteServerService();
            }
        }

        private final class UimRemoteServerDeathRecipient implements HwBinder.DeathRecipient {
            @Override
            public void serviceDied(long cookie) {
                if (cookie != mUimRemoteServerCookie.get()) {
                    return;
                }
                Log.e(LOG_TAG, "IUimRemoteServiceServer service died");
                mUimremoteServer = null;
            }
        }

        private final class UimRemoteServerResponse extends IUimRemoteServiceServerResponse.Stub {
            @Override
            public void uimRemoteServiceServerConnectResponse(int token, int sapConnectRsp,
                         int maxMsgSize) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Server Response not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerConnectResponse No Callback");
                    return;
                }
                Log.d(LOG_TAG, "uimRemoteServiceServerConnectResponse, sapConnectRsp: " +
                 String.valueOf(sapConnectRsp));
                try {
                    mCb.uimRemoteServerConnectResp(mInstance, sapConnectRsp, maxMsgSize);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerConnectResponse: exception" + ex);
                }
            }

            @Override
            public void uimRemoteServiceServerDisconnectResponse(int token) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote server Response not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerDisconnectResponse No Callback");
                    return;
                }
                Log.d(LOG_TAG, "uimRemoteServiceServerDisconnectResponse: ");
                try {
                    mCb.uimRemoteServerDisconnectResp(mInstance, 0);  // success
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerDisconnectResponse: exception" + ex);
                }
            }

            @Override
            public void uimRemoteServiceServerApduResponse(int token, int resultCode,
                         java.util.ArrayList<Byte> apduRsp) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote server Response not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerApduResponse No Callback");
                    return;
                }
                Log.d(LOG_TAG, "uimRemoteServiceServerApduResponse, resultCode: " +
                 String.valueOf(resultCode));
                try {
                    // UIM_REMOTE_SERVER_SUCCESS
                    int respStatus = 0;

                    switch(resultCode)
                    {
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_SUCCESS:
                            // UIM_REMOTE_SERVER_SUCCESS
                            respStatus = 0;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_NOT_ACCESSSIBLE:
                            // UIM_REMOTE_SERVER_SIM_NOT_READY
                            respStatus = 2;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_OFF:
                            // UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_OFF
                            respStatus = 3;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_REMOVED:
                            // UIM_REMOTE_SERVER_SIM_ABSENT
                            respStatus = 4;
                            break;
                        default:
                            // UIM_REMOTE_SERVER_INTERNAL_FAILURE
                            respStatus = 1;
                            break;
                    }
                    mCb.uimRemoteServerApduResp(mInstance, respStatus,
                     arrayListToPrimitiveArray(apduRsp));
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerApduResponse: exception" + ex);
                }
            }

            @Override
            public void uimRemoteServiceServerTransferAtrResponse(int token,
                         int resultCode, java.util.ArrayList<Byte> atr) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote server Response not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerTransferAtrResponse No Callback");
                    return;
                }
                Log.d(LOG_TAG, "uimRemoteServiceServerTransferAtrResponse, resultCode: " +
                 String.valueOf(resultCode));
                try {
                    // UIM_REMOTE_SERVER_SUCCESS
                    int respStatus = 0;

                    switch(resultCode)
                    {
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_SUCCESS:
                            // UIM_REMOTE_SERVER_SUCCESS
                            respStatus = 0;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_NOT_ACCESSSIBLE:
                            // UIM_REMOTE_SERVER_SIM_NOT_READY
                            respStatus = 2;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_OFF:
                            // UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_OFF
                            respStatus = 3;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_ON:
                            // UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_ON
                            respStatus = 18;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_REMOVED:
                            // UIM_REMOTE_SERVER_SIM_ABSENT
                            respStatus = 4;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_DATA_NOT_AVAILABLE:
                            // UIM_REMOTE_SERVER_SIM_DATA_NOT_AVAILABLE
                            respStatus = 6;
                            break;
                        default:
                            // UIM_REMOTE_SERVER_INTERNAL_FAILURE
                            respStatus = 1;
                            break;
                    }
                    mCb.uimRemoteServerTransferAtrResp(mInstance, respStatus,
                     arrayListToPrimitiveArray(atr));
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerTransferAtrResponse: exception" + ex);
                }
            }

            @Override
            public void uimRemoteServiceServerPowerResponse(int token, int resultCode) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote server Response not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerPowerResponse No Callback");
                    return;
                }
                Log.d(LOG_TAG, "uimRemoteServiceServerPowerResponse, resultCode: " +
                 String.valueOf(resultCode));
                try {
                    // UIM_REMOTE_SERVER_SUCCESS
                    int respStatus = 0;

                    switch(resultCode)
                    {
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_SUCCESS:
                            // UIM_REMOTE_SERVER_SUCCESS
                            respStatus = 0;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_OFF:
                            // UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_OFF
                            respStatus = 17;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_ON:
                            // UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_ON
                            respStatus = 18;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_REMOVED:
                            // UIM_REMOTE_SERVER_SIM_ABSENT
                            respStatus = 11;
                            break;
                        default:
                            // UIM_REMOTE_SERVER_GENERIC_FAILURE
                            respStatus = 2;
                            break;
                    }
                    mCb.uimRemoteServerPowerResp(mInstance, respStatus);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerPowerResponse: exception" + ex);
                }
            }

            @Override
            public void uimRemoteServiceServerResetSimResponse(int token, int resultCode) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote server Response not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerResetSimResponse No Callback");
                    return;
                }
                Log.d(LOG_TAG, "uimRemoteServiceServerResetSimResponse, resultCode: " +
                 String.valueOf(resultCode));
                try {
                    // UIM_REMOTE_SERVER_SUCCESS
                    int respStatus = 0;

                    switch(resultCode)
                    {
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_SUCCESS:
                            // UIM_REMOTE_SERVER_SUCCESS
                            respStatus = 0;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_OFF:
                            // UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_OFF
                            respStatus = 17;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_ON:
                            // UIM_REMOTE_SERVER_SIM_ALREADY_POWERED_ON
                            respStatus = 18;
                            break;
                        case UimRemoteServiceServerResultCode.UIM_REMOTE_SERVICE_SERVER_CARD_REMOVED:
                            // UIM_REMOTE_SERVER_SIM_ABSENT
                            respStatus = 11;
                            break;
                        default:
                            // UIM_REMOTE_SERVER_GENERIC_FAILURE
                            respStatus = 2;
                            break;
                    }
                    mCb.uimRemoteServerResetSimResp(mInstance, respStatus);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerResetSimResponse: exception" + ex);
                }
            }

            @Override
            public void uimRemoteServiceServerTransferCardReaderStatusResponse(int token,
                         int resultCode, int cardReaderStatus) {
                Log.e(LOG_TAG, "uimRemoteServiceServerTransferCardReaderStatusResponse Not Supported, dummy function");
            }

            @Override
            public void uimRemoteServiceServerErrorResponse(int token) {
                Log.e(LOG_TAG, "uimRemoteServiceServerErrorResponse Not Supported, dummy function");
            }

            @Override
            public void uimRemoteServiceServerTransferProtocolResponse(int token, int resultCode) {
                Log.e(LOG_TAG, "uimRemoteServiceServerTransferProtocolResponse Not Supported, dummy function");
            }
        }

        private final class UimRemoteServerIndication extends IUimRemoteServiceServerIndication.Stub {
            @Override
            public void uimRemoteServiceServerDisconnectIndication(int disconnectType) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Server Indication not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerDisconnectIndication No Callback");
                    return;
                }
                Log.d(LOG_TAG, "uimRemoteServiceServerDisconnectIndication : disconnect type: " +
                 String.valueOf(disconnectType));
                try
                {
                    mCb.uimRemoteServerDisconnectInd(mInstance, disconnectType);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerDisconnectIndication: exception" + ex);
                }
            }

            @Override
            public void uimRemoteServiceServerStatusIndication(int status) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Server Indication not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerStatusIndication No Callback");
                    return;
                }
                Log.d(LOG_TAG, "uimRemoteServiceServerStatusIndication  status : " +
                 String.valueOf(status));
                try
                {
                    mCb.uimRemoteServerStatusInd(mInstance, status);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "uimRemoteServiceServerStatusIndication: exception" + ex);
                }
            }
        }

        public UimRemoteServiceServer(int slot) {
            mInstance = slot;
            mInstanceName = "uimRemoteServer" + slot;
            try {
                boolean ret = IServiceManager.getService().registerForNotifications
                    ("vendor.qti.hardware.radio.uim_remote_server@1.0::IUimRemoteServiceServer",
                     mInstanceName, mServiceNotification);
                if (!ret)
                {
                    Log.d(LOG_TAG, "Unable to register service start notification: ret: " + ret);
                }
             } catch(RemoteException ex) {
                Log.e(LOG_TAG, "registerForServiceNotifications: exception" + ex);
             }
        }

        public synchronized IUimRemoteServiceServer getService() {
            if (mIsDisposed) {
                return null;
            }
            return mUimremoteServer;
        }

        public synchronized void toDestroy() {
            if (mIsDisposed) {
                return;
            }

            mIsDisposed = true;
            try {
                if (mUimremoteServer != null) {
                    mUimremoteServer.unlinkToDeath(mUimRemotServerDeathRecipient);
                    mUimremoteServer = null;
                }
            } catch(RemoteException e) {
                Log.e(LOG_TAG, "ToDestroy: Exception=" + e);
            }
        }

        private synchronized void initUimRemoteServerService() {
            Log.d(LOG_TAG, "initUimRemoteServerService");
            try {
                mUimremoteServer = IUimRemoteServiceServer.getService(mInstanceName);
                if (mUimremoteServer == null) {
                    Log.e(LOG_TAG, "initUimRemoteServerService: mUimremoteServer == null");
                    return;
                }
                mUimremoteServer.linkToDeath(mUimRemotServerDeathRecipient,
                                             mUimRemoteServerCookie.incrementAndGet());
                mUimremoteServer.setCallback(mUimRemoteServerResp, mUimRemoteServerInd);
            } catch(RemoteException e) {
                Log.e(LOG_TAG, "initUimRemoteServerService: Exception=" + e);
            }
        }
    }

    private Application readApplication(XmlResourceParser parser) throws XmlPullParserException, IOException {
        parser.require(XmlPullParser.START_TAG, null, "Application");
        Application app = new Application();
        int eventType = parser.next();
        while(eventType != XmlPullParser.END_TAG) {
            if(eventType != XmlPullParser.START_TAG) {
                app.parsingFail = true;
                Log.e(LOG_TAG, "parse fail");
                break;
            }
            String tagName = parser.getName();
            if(tagName.equals("PackageName")){
                eventType = parser.next();
                if(eventType == XmlPullParser.TEXT){
                    app.name = parser.getText();
                    eventType = parser.next();
                }
                if((eventType != XmlPullParser.END_TAG) || !(parser.getName().equals("PackageName"))){
                     //Invalid tag or invalid xml format
                    app.parsingFail = true;
                    Log.e(LOG_TAG, "parse fail");
                    break;
                }
            }
            else if(tagName.equals("SignatureHash")){
                eventType = parser.next();
                if(eventType == XmlPullParser.TEXT){
                    app.key = parser.getText();
                    eventType = parser.next();
                }
                if((eventType != XmlPullParser.END_TAG) || !(parser.getName().equals("SignatureHash"))){
                     //Invalid tag or invalid xml format
                    app.parsingFail = true;
                    Log.e(LOG_TAG, "parse fail");
                    break;
                }
            }
            else{
                 app.parsingFail = true;
                 Log.e(LOG_TAG, "parse fail" + tagName);
                 break;
            }
            eventType = parser.next();
        }
        if((eventType != XmlPullParser.END_TAG) || !(parser.getName().equals("Application"))){
            //End Tag that ended the loop is not Application
            app.parsingFail = true;
        }
        return app;
    }

    private void getWhiteList(){
        try {
            String name = null;
            Application app = null;
            boolean fail = false;
            int eventType;
            HashMap<String, String> table = new HashMap<String, String>();

            XmlResourceParser parser = this.getResources().getXml(R.xml.applist);
            parser.next();

            //Get the parser to point to Entries Tag.
            if(parser.getEventType() == XmlPullParser.START_DOCUMENT){
                name = parser.getName();
                while(name == null ||
                       (name != null && !name.equals("Entries"))){
                    parser.next();
                    name = parser.getName();
                }
            }

            parser.require(XmlPullParser.START_TAG, null, "Entries");
            eventType = parser.next();

            //Loop until END_TAG is encountered
            while(eventType != XmlPullParser.END_TAG) {

                //If the TAG is not a START_TAG, break the loop
                //with Failure.
                if(eventType != XmlPullParser.START_TAG) {
                    fail = true;
                    Log.e(LOG_TAG, "parse fail");
                    break;
                }

                name = parser.getName();
                if(name.equals("Application")) {
                    app = readApplication(parser);
                    if(app.parsingFail)
                    {
                        fail = true;
                        Log.e(LOG_TAG, "parse fail");
                        break;
                    }
                    else if(app.name != null || app.key != null){
                        table.put(app.name, app.key);
                    }
                }
                else {
                    fail = true;
                    Log.e(LOG_TAG, "parse fail" + name);
                    break;
                }
                eventType = parser.next();
            }
            if(fail || eventType != XmlPullParser.END_TAG ||
                     !(parser.getName().equals("Entries"))){
                //parsing failure
                Log.e(LOG_TAG, "FAIL");
            }
            else if(!table.isEmpty()) {
                 UimRemoteServerWhiteList = Collections.unmodifiableMap(table);
            }
        }
        catch(Exception e) {
            Log.e(LOG_TAG, "Exception: "+ e);
        }
    }

	private static String bytesToHex(byte[] inputBytes) {
        final StringBuilder sb = new StringBuilder();
        for(byte b : inputBytes) {
            sb.append(String.format("%02x", b));
        }
        return sb.toString();
    }
    private boolean verifyAuthenticity(int uid){
        boolean ret = false;

        if(UimRemoteServerWhiteList == null) {
            Log.e(LOG_TAG, "empty white list");
            return ret;
        }
        String[] packageNames = context.getPackageManager().getPackagesForUid(uid);
        for(String packageName : packageNames){
            if(UimRemoteServerWhiteList.containsKey(packageName)){
                String hash = (String)UimRemoteServerWhiteList.get(packageName);
                String compareHash = new String();
                try {
                    Signature[] sigs = context.getPackageManager().getPackageInfo(packageName, PackageManager.GET_SIGNATURES).signatures;
                    for(Signature sig: sigs) {

                        //get the raw certificate into input stream
                        final byte[] rawCert = sig.toByteArray();
                        InputStream certStream = new ByteArrayInputStream(rawCert);

                        //Read the X.509 Certificate into certBytes
                        final CertificateFactory certFactory = CertificateFactory.getInstance("X509");
                        final X509Certificate x509Cert = (X509Certificate)certFactory.generateCertificate(certStream);
                        byte[] certBytes = x509Cert.getEncoded();

                        //get the fixed SHA-1 cert
                        MessageDigest md = MessageDigest.getInstance("SHA-1");
	                    md.update(certBytes);
	                    byte[] certThumbprint = md.digest();

                        //cert in hex format
                        compareHash = bytesToHex(certThumbprint);

                        if(hash.equals(compareHash)) {
                            ret = true;
                            break;
                        }
                    }
                }
                catch(Exception e) {
                    Log.e(LOG_TAG, "Exception reading client data!" + e);
                }
            }
        }
        return ret;
    }

    /* Utility method to convert byte array to ArrayList of Bytes */
    private static ArrayList<Byte> primitiveArrayToArrayList(byte[] arr) {
        if (arr == null) return null;
        ArrayList<Byte> arrayList = new ArrayList<>(arr.length);
        for (byte b : arr) {
            arrayList.add(b);
        }
        return arrayList;
    }
    /* Utility method to convert ArrayList of Bytes to byte array */
    private static byte[] arrayListToPrimitiveArray(ArrayList<Byte> bytes) {
        if (bytes == null) return null;
        byte[] ret = new byte[bytes.size()];
        for (int i = 0; i < ret.length; i++) {
            ret[i] = bytes.get(i);
        }
        return ret;
    }
}
