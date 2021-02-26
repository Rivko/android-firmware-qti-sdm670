/******************************************************************************
  @file    UimRemoteClientService.java

  ---------------------------------------------------------------------------
  Copyright (c) 2014,2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.uimremoteclient;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.HwBinder;
import android.os.Message;
import android.os.RemoteException;

import android.hidl.manager.V1_0.IServiceManager;
import android.hidl.manager.V1_0.IServiceNotification;

import android.content.Context;
import android.telephony.TelephonyManager;
import android.content.pm.Signature;
import android.content.pm.PackageManager;
import android.util.Log;
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
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import vendor.qti.hardware.radio.uim_remote_client.V1_0.IUimRemoteServiceClient;
import vendor.qti.hardware.radio.uim_remote_client.V1_0.IUimRemoteServiceClientResponse;
import vendor.qti.hardware.radio.uim_remote_client.V1_0.IUimRemoteServiceClientIndication;
import vendor.qti.hardware.radio.uim_remote_client.V1_0.UimRemoteEventReqType;

public class UimRemoteClientService extends Service {
    private final String LOG_TAG = "UimRemoteClientService";

    public static class UimRemoteError {

        public static final int UIM_REMOTE_SUCCESS = 0;

        public static final int UIM_REMOTE_ERROR = 1;
    }

    private Context context;
    private final AtomicInteger mToken = new AtomicInteger(0);

    private int simSlots = TelephonyManager.getDefault().getSimCount();

    private UimRemoteServiceClient[] mUimRemoteClientHals = new UimRemoteServiceClient[simSlots];
    IUimRemoteClientServiceCallback mCb = null;

    private Phone[] mPhone = new Phone[simSlots];
    private static final int EVENT_RADIO_AVAILABLE = 10;
    private static final int EVENT_RADIO_NOT_AVAILABLE = 11;

    private static class Application {
        public String name;
        public String key;
        public boolean parsingFail;
    }

    private Map UimRemoteClientWhiteList;

    private Handler mRadioStateHdlr = new Handler() {
        public void handleMessage (Message msg) {
            Log.i(LOG_TAG, "handleMessage()");
            if (mCb == null) {
                Log.d(LOG_TAG, "handleMessage() - null mCb");
                return;
            }

            try {
                int slotId = msg.arg1;

                switch(msg.what) {
                    case EVENT_RADIO_AVAILABLE:
                        Log.d(LOG_TAG, "EVENT_RADIO_AVAILABLE");
                        if(mPhone[slotId].mCi.getRadioState().isOn())
                            mCb.uimRemoteRadioStateIndication(slotId, 2);
                        else
                            mCb.uimRemoteRadioStateIndication(slotId, 1);
                        break;
                    case EVENT_RADIO_NOT_AVAILABLE:
                        Log.d(LOG_TAG, "EVENT_RADIO_NOT_AVAILABLE");
                        mCb.uimRemoteRadioStateIndication(slotId, 0);
                        break;
                    default:
                        break;
                }
            }
            catch (RemoteException e) {
                Log.e(LOG_TAG, "error occured when parsing the resp/ind");
            }
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(LOG_TAG, "onCreate()");
        context = this;

        Log.i(LOG_TAG, "simCount: "+ simSlots);

        for(int i = 0; i < simSlots; i++) {
            mPhone[i] = (Phone)(PhoneFactory.getPhones()[i]);
            mPhone[i].mCi.registerForAvailable(mRadioStateHdlr,
                                               EVENT_RADIO_AVAILABLE,
                                               new Integer(i));
            mPhone[i].mCi.registerForNotAvailable(mRadioStateHdlr,
                                                  EVENT_RADIO_NOT_AVAILABLE,
                                                  new Integer(i));
            mUimRemoteClientHals[i] = new UimRemoteServiceClient(i);
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
            mUimRemoteClientHals[i].toDestroy();
        }
        stopSelf();
        super.onDestroy();
    }

    private final IUimRemoteClientService.Stub mBinder = new IUimRemoteClientService.Stub() {
        public int registerCallback(IUimRemoteClientServiceCallback cb) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            UimRemoteClientService.this.mCb = cb;
            if (cb == null) {
                Log.d(LOG_TAG, "registerCallback() - null cb");
            }
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int deregisterCallback(IUimRemoteClientServiceCallback cb) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            UimRemoteClientService.this.mCb = null;
            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int uimRemoteEvent(int slot, int event, byte[] atr, int errCode,
                    boolean has_transport, int transport, boolean has_usage, int usage,
                    boolean has_apdu_timeout, int apdu_timeout, boolean has_disable_all_polling,
                    int disable_all_polling, boolean has_poll_timer, int poll_timer) {
            UimRemoteEventReqType mEventType = new UimRemoteEventReqType();
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if(slot >= simSlots) {
                Log.e(LOG_TAG, "Sim Slot not supported!" + slot);
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteClientHals[slot] == null) {
                Log.e(LOG_TAG, "Server is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteClientHals[slot].getService() == null)
            {
                Log.e(LOG_TAG, "Sevice is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }

            Log.d(LOG_TAG, "uimRemoteEvent() - slot: " + slot + "; event: " + event);
            mEventType.event = event;
            mEventType.atr.addAll(primitiveArrayToArrayList(atr));
            mEventType.errorCode = errCode;
            mEventType.has_transport = has_transport;
            mEventType.transport = transport;
            mEventType.has_usage = has_usage;
            mEventType.usage = usage;
            mEventType.has_apdu_timeout = has_apdu_timeout;
            mEventType.apduTimeout = apdu_timeout;
            mEventType.has_disable_all_polling = has_disable_all_polling;
            mEventType.disableAllPolling = disable_all_polling;
            mEventType.has_poll_timer = has_poll_timer;
            mEventType.pollTimer = poll_timer;

            try {
                mUimRemoteClientHals[slot].getService().UimRemoteServiceClientEventReq(
                                                       mToken.incrementAndGet(), mEventType);
            } catch(RemoteException ex) {
                Log.e(LOG_TAG, "uimRemoteEvent: exception" + ex);
            }

            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }

        public int uimRemoteApdu(int slot, int apduStatus, byte[] apduResp) {
            if(!verifyAuthenticity(mBinder.getCallingUid())) {
                Log.d(LOG_TAG, "Cannot perform! returning failure");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if(slot >= simSlots) {
                Log.e(LOG_TAG, "Sim Slot not supported!" + slot);
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteClientHals[slot] == null) {
                Log.e(LOG_TAG, "server is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }
            if (mUimRemoteClientHals[slot].getService() == null)
            {
                Log.e(LOG_TAG, "Sevice is not connected");
                return UimRemoteError.UIM_REMOTE_ERROR;
            }

            Log.d(LOG_TAG, "uimRemoteApdu() - slot: " + slot + "; adpuStatus: " + apduStatus);

            try {
                mUimRemoteClientHals[slot].getService().UimRemoteServiceClientApduReq(
                                                             mToken.incrementAndGet(),
                                                             apduStatus,
                                                             primitiveArrayToArrayList(apduResp));
            } catch(RemoteException ex) {
                Log.e(LOG_TAG, "uimRemoteApdu: exception" + ex);
            }

            return UimRemoteError.UIM_REMOTE_SUCCESS;
        }
    };

    private class UimRemoteServiceClient {
        private final String mInstanceName;
        private int mInstance;
        private final AtomicLong mUimRemoteClientCookie = new AtomicLong(0);
        private boolean mIsDisposed = false;

        private final UimRemoteClientDeathRecipient mUimRemotClientDeathRecipient =
                                                       new UimRemoteClientDeathRecipient();
        private final ServiceNotification mServiceNotification =
                                                       new ServiceNotification();
        private final IUimRemoteServiceClientResponse mUimRemoteClientResp =
                                                       new UimRemoteClientResponse();
        private final IUimRemoteServiceClientIndication mUimRemoteClientInd =
                                                       new UimRemoteClientIndication();
        private IUimRemoteServiceClient mUimremoteClient;

        private final class ServiceNotification extends IServiceNotification.Stub {
            @Override
            public void onRegistration(String fqName, String name, boolean preexisting) {
                if (!mInstanceName.equals(name) || mIsDisposed) {
                    Log.d(LOG_TAG, "onRegistration: Ignoring.");
                    return;
                }
                Log.d(LOG_TAG, "ServiceNotification - onRegistration");
                initUimRemoteClientService();
                if (mUimremoteClient != null &&
                    mCb != null &&
                    mInstance < simSlots && mPhone[mInstance] != null) {
                    try {
                        if(mPhone[mInstance].mCi.getRadioState().isOn()) {
                            mCb.uimRemoteRadioStateIndication(mInstance, 2);
                        }
                        else {
                            mCb.uimRemoteRadioStateIndication(mInstance, 1);
                        }
                    } catch(RemoteException ex) {
                        Log.e(LOG_TAG, "onRegistration: exception" + ex);
                    }
                }
            }
        }

        private final class UimRemoteClientDeathRecipient implements HwBinder.DeathRecipient {
            @Override
            public void serviceDied(long cookie) {
                if (cookie != mUimRemoteClientCookie.get()) {
                    return;
                }
                Log.e(LOG_TAG, "IUimRemoteServiceClient service died");
                mUimremoteClient = null;
                try {
                    if (mCb != null)
                    {
                        mCb.uimRemoteRadioStateIndication(mInstance, 0);
                    }
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "serviceDied: exception" + ex);
                }
            }
        }

        private final class UimRemoteClientResponse extends
                             IUimRemoteServiceClientResponse.Stub {
            @Override
            public void UimRemoteServiceClientEventResp(int token, int eventResp) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Client Response not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientEventResp No Callback");
                    return;
                }
                Log.d(LOG_TAG, "UimRemoteServiceClientEventResp, event_rsp: " +
                 String.valueOf(eventResp));
                try {
                    mCb.uimRemoteEventResponse(mInstance, eventResp);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientEventResp: exception" + ex);
                }
            }

            @Override
            public void UimRemoteServiceClientApduResp(int token, int apduResp) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Client Response not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientApduResp No Callback");
                    return;
                }
                Log.d(LOG_TAG, "UimRemoteServiceClientApduResp, apduResp: " +
                 String.valueOf(apduResp));
                try {
                    mCb.uimRemoteApduResponse(mInstance, apduResp);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientApduResp: exception" + ex);
                }
            }
        }

        private final class UimRemoteClientIndication extends
              IUimRemoteServiceClientIndication.Stub {
            @Override
            public void UimRemoteServiceClientApduInd(java.util.ArrayList<Byte> apduInd) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Client Indication not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientApduInd No Callback");
                    return;
                }
                Log.d(LOG_TAG, "UimRemoteServiceClientApduInd");
                try {
                    mCb.uimRemoteApduIndication(mInstance, arrayListToPrimitiveArray(apduInd));
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientApduInd: exception" + ex);
                }
            }

            @Override
            public void UimRemoteServiceClientConnectInd() {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Client Indication not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientConnectInd No Callback");
                    return;
                }
                Log.d(LOG_TAG, "UimRemoteServiceClientConnectInd");
                try
                {
                    mCb.uimRemoteConnectIndication(mInstance);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientConnectInd: exception" + ex);
                }
            }

            @Override
            public void UimRemoteServiceClientDisconnectInd() {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Client Indication not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientDisconnectInd No Callback");
                    return;
                }
                Log.d(LOG_TAG, "UimRemoteServiceClientDisconnectInd");
                try
                {
                    mCb.uimRemoteDisconnectIndication(mInstance);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientDisconnectInd: exception" + ex);
                }
            }

            @Override
            public void UimRemoteServiceClientPowerUpInd(boolean hasTimeOut, int timeOut,
                                         boolean hasVoltageClass, int powerUpVoltageClass) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Client Indication not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientPowerUpInd No Callback");
                    return;
                }
                Log.d(LOG_TAG, "UimRemoteServiceClientPowerUpInd");
                try
                {
                    mCb.uimRemotePowerUpIndication(mInstance);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientPowerUpInd: exception" + ex);
                }
            }

            @Override
            public void UimRemoteServiceClientPowerDownInd(boolean hasPowerDownMode,
                                                           int powerDownMode) {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Client Indication not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientPowerDownInd No Callback");
                    return;
                }
                Log.d(LOG_TAG, "UimRemoteServiceClientPowerDownInd");
                try
                {
                    mCb.uimRemotePowerDownIndication(mInstance);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientPowerDownInd: exception" + ex);
                }
            }

            @Override
            public void UimRemoteServiceClientResetInd() {
                if (mIsDisposed) {
                   Log.e(LOG_TAG, "Uim Remote Client Indication not exist");
                   return;
                }

                if (mCb == null) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientResetInd No Callback");
                    return;
                }
                Log.d(LOG_TAG, "UimRemoteServiceClientResetInd");
                try {
                    mCb.uimRemoteResetIndication(mInstance);
                } catch(RemoteException ex) {
                    Log.e(LOG_TAG, "UimRemoteServiceClientResetInd: exception" + ex);
                }
            }
        }

        public UimRemoteServiceClient(int slot) {
            mInstance = slot;
            mInstanceName = "uimRemoteClient" + slot;
            try {
                boolean ret = IServiceManager.getService().registerForNotifications
                    ("vendor.qti.hardware.radio.uim_remote_client@1.0::IUimRemoteServiceClient",
                     mInstanceName, mServiceNotification);
                if (!ret) {
                    Log.d(LOG_TAG, "Unable to register service start notification: ret" + ret);
                }
             } catch(RemoteException ex) {
                Log.e(LOG_TAG, "registerForServiceNotifications: exception" + ex);
             }
        }

        public synchronized IUimRemoteServiceClient getService() {
            if (mIsDisposed) {
                return null;
            }

            return mUimremoteClient;
        }

        public synchronized void toDestroy() {
            if (mIsDisposed) {
                return;
            }

            mIsDisposed = true;
            try {
                if (mUimremoteClient != null) {
                    mUimremoteClient.unlinkToDeath(mUimRemotClientDeathRecipient);
                    mUimremoteClient = null;
                }
            } catch(RemoteException e) {
                Log.e(LOG_TAG, "ToDestroy: Exception=" + e);
            }
        }

        private synchronized void initUimRemoteClientService() {
            Log.d(LOG_TAG, "initUimRemoteClientService");
            try {
                mUimremoteClient = IUimRemoteServiceClient.getService(mInstanceName);
                if (mUimremoteClient == null) {
                    Log.e(LOG_TAG, "initUimRemoteClientService: mUimremoteClient == null");
                    return;
                }
                mUimremoteClient.linkToDeath(mUimRemotClientDeathRecipient,
                                            mUimRemoteClientCookie.incrementAndGet());
                mUimremoteClient.setCallback(mUimRemoteClientResp, mUimRemoteClientInd);
            } catch(RemoteException e) {
                Log.e(LOG_TAG, "initUimRemoteClientService: Exception=" + e);
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
                    if(app.parsingFail){
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
                 UimRemoteClientWhiteList = Collections.unmodifiableMap(table);
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

        if(UimRemoteClientWhiteList == null) {
            return ret;
        }
        String[] packageNames = context.getPackageManager().getPackagesForUid(uid);
        for(String packageName : packageNames){
            if(UimRemoteClientWhiteList.containsKey(packageName)){
                String hash = (String)UimRemoteClientWhiteList.get(packageName);
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
