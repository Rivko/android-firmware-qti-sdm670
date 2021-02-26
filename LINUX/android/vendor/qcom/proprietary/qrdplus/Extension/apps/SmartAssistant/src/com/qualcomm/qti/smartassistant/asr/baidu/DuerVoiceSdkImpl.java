/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.asr.baidu;

import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.AsyncTask;
import android.provider.AlarmClock;
import android.provider.ContactsContract;
import android.text.TextUtils;
import android.util.ArrayMap;
import android.util.Log;
import android.widget.Toast;

import com.baidu.duer.dcs.api.DcsSdkBuilder;
import com.baidu.duer.dcs.api.IConnectionStatusListener;
import com.baidu.duer.dcs.api.IDcsSdk;
import com.baidu.duer.dcs.api.IDialogStateListener;
import com.baidu.duer.dcs.oauth.api.credentials.BaiduOauthClientCredentialsImpl;
import com.baidu.duer.dcs.sample.sdk.devicemodule.alarms.AlarmsDeviceModule;
import com.baidu.duer.dcs.sample.sdk.devicemodule.alarms.message.SetAlarmPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.alarms.message.SetTimerPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.alarms.message.ShowAlarmsPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.alarms.message.ShowTimersPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.applauncher.AppLauncherDeviceModule;
import com.baidu.duer.dcs.sample.sdk.devicemodule.applauncher.AppLauncherImpl;
import com.baidu.duer.dcs.sample.sdk.devicemodule.applauncher.message.LaunchAppPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.phonecall.PhoneCallDeviceModule;
import com.baidu.duer.dcs.sample.sdk.devicemodule.phonecall.message.CandidateCallee;
import com.baidu.duer.dcs.sample.sdk.devicemodule.phonecall.message.PhonecallByNamePayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.phonecall.message.PhonecallByNumberPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.phonecall.message.SelectCalleePayload;

import com.baidu.duer.dcs.sample.sdk.devicemodule.screen.ScreenDeviceModule;
import com.baidu.duer.dcs.sample.sdk.devicemodule.screen.message.HtmlPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.screen.message.RenderCardPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.screen.message.RenderHintPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.screen.message.RenderVoiceInputTextPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.sms.SmsDeviceModule;
import com.baidu.duer.dcs.sample.sdk.devicemodule.sms.message.CandidateRecipient;
import com.baidu.duer.dcs.sample.sdk.devicemodule.sms.message.SelectRecipientPayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.sms.message.SendSmsByNamePayload;
import com.baidu.duer.dcs.sample.sdk.devicemodule.sms.message.SendSmsByNumberPayload;
import com.baidu.duer.dcs.framework.DcsSdkImpl;
import com.baidu.duer.dcs.framework.ILoginListener;
import com.baidu.duer.dcs.framework.IMessageSender;
import com.baidu.duer.dcs.framework.InternalApi;
import com.baidu.duer.dcs.framework.internalapi.DcsConfig;
import com.baidu.duer.dcs.framework.internalapi.IErrorListener;
import com.baidu.duer.dcs.framework.location.Location;
import com.baidu.duer.dcs.framework.upload.contact.UploadPreference;
import com.baidu.duer.dcs.framework.upload.contact.IUpload.IUploadListener;
import com.baidu.duer.dcs.systeminterface.BaseAudioRecorder;
import com.baidu.duer.dcs.systeminterface.IOauth;
import com.baidu.duer.dcs.api.IVoiceRequestListener;
import com.baidu.duer.dcs.common.util.CommonUtil;
import com.baidu.duer.dcs.util.NetWorkUtil;
import com.qualcomm.qti.smartassistant.R;
import com.qualcomm.qti.smartassistant.asr.CandidateInfo;
import com.qualcomm.qti.smartassistant.asr.IVoiceSdk;
import com.qualcomm.qti.smartassistant.asr.IVoiceSdkListener;
import com.qualcomm.qti.smartassistant.service.device.SmsSender;

import com.qualcomm.qti.dueros.DuerosAccount;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.Map;

public class DuerVoiceSdkImpl implements IVoiceSdk {
    public static final String TAG = "DuerVoiceSdkImpl";

    //Please replace below CLIENT_ID and CLIENT_SECRET with your content
    private static final String CLIENT_ID = DuerosAccount.getClientId();
    static final String CLIENT_SECRET = DuerosAccount.getClientSecret();
    private List<IVoiceSdkListener> mVoiceSdkListeners;
    private Context mContext;
    private IDcsSdk dcsSdk;
    private DuerAudioRecordImpl mAudioRecorder;
    private ScreenDeviceModule screenDeviceModule;
    private PhoneCallDeviceModule phoneCallDeviceModule;
    private SmsDeviceModule smsDeviceModule;
    private AppLauncherDeviceModule appLauncherDeviceModule;
    private AlarmsDeviceModule alarmsDeviceModule;

    private PhoneCallDeviceModule.IPhoneCallListener phoneCallListener;
    private SmsDeviceModule.ISmsListener smsListener;
    private AppLauncherDeviceModule.IAppLauncherListener appLauncherListener;
    private AlarmsDeviceModule.IAlarmDirectiveListener alarmListener;
    private IDialogStateListener dialogStateListener;
    private boolean isStopListenReceiving;

    private QueryContactsTask queryContactsTask;

    private static final Map<String, Integer> calendar = new ArrayMap<>();
    static {
        calendar.put("MON", Calendar.MONDAY);
        calendar.put("TUE", Calendar.TUESDAY);
        calendar.put("WED", Calendar.WEDNESDAY);
        calendar.put("THU", Calendar.THURSDAY);
        calendar.put("FRI", Calendar.FRIDAY);
        calendar.put("SAT", Calendar.SATURDAY);
        calendar.put("SUN", Calendar.SUNDAY);
    }

    private IConnectionStatusListener connectionStatusListener = new IConnectionStatusListener() {
        @Override
        public void onConnectStatus(ConnectionStatus connectionStatus) {
            Log.d(TAG, "onConnectionStatusChange: " + connectionStatus);
        }
    };

    private IErrorListener errorListener = new IErrorListener() {
        @Override
        public void onErrorCode(ErrorCode errorCode) {
            Log.d(TAG, "onErrorCode:" + errorCode);
        }
    };

    private ScreenDeviceModule.IScreenListener screenListener = new ScreenDeviceModule.IScreenListener() {
        @Override
        public void onRenderVoiceInputText(RenderVoiceInputTextPayload payload) {
            handleRenderVoiceInputText(payload.text);
            if (payload.type == RenderVoiceInputTextPayload.Type.FINAL) {
                Log.d(TAG,"ASR-FINAL-RESULT:" + payload.text + "," + System.currentTimeMillis());
            }
        }

        @Override
        public void onHtmlPayload(HtmlPayload htmlPayload, int id) {
            for (IVoiceSdkListener listener: mVoiceSdkListeners){
                listener.onHtmlPayload(htmlPayload.getUrl());
            }
        }

        @Override
        public void onRenderCard(RenderCardPayload renderCardPayload, int id) {

        }

        @Override
        public void onRenderHint(RenderHintPayload renderHintPayload, int id) {

        }
    };

    /**
     * You should implement the location function by yourself.
     * Here is hardcode the location to "Beijing" for testing.
     **/
    protected Location.LocationHandler locationHandler = new Location.LocationHandler() {
        @Override
        public double getLongitude() {
            return 116.404;
        }

        @Override
        public double getLatitude() {
            return 39.915;
        }

        @Override
        public String getCity() {
            return "北京";
        }

        @Override
        public Location.EGeoCoordinateSystem getGeoCoordinateSystem() {
            return Location.EGeoCoordinateSystem.WGS84;
        }
    };

    public DuerVoiceSdkImpl(Context context){
        mContext = context;
        mVoiceSdkListeners = new ArrayList<>();
    }

    @Override
    public void initializeSdk(Context context) {
        initSdk();

        dcsSdk.addConnectionStatusListener(connectionStatusListener);

        getInternalApi().addErrorListener(errorListener);
        getInternalApi().setLocationHandler(locationHandler);

        initVoiceRequestListener();
    }

    @Override
    public void addVoiceSdkListener(IVoiceSdkListener listener) {
        if (!mVoiceSdkListeners.contains(listener)){
            mVoiceSdkListeners.add(listener);
        }
    }

    @Override
    public void removeVoiceSdkListener(IVoiceSdkListener listener) {
        mVoiceSdkListeners.remove(listener);
    }

    @Override
    public void startRecognition(boolean captureAvailable, int captureSession,
                                 int soundModelHandle) {
        if (!NetWorkUtil.isNetworkConnected(mContext)) {
            Toast.makeText(mContext,
                    mContext.getResources().getString(R.string.err_net_msg),
                    Toast.LENGTH_SHORT).show();
            for (IVoiceSdkListener listener : mVoiceSdkListeners){
                listener.onNetworkException();
            }
            Log.e(TAG, "startRecognition network exception");
            return;
        }
        if (CommonUtil.isFastDoubleClick()) {
            return;
        }
        if (isStopListenReceiving) {
            endVoiceRequest();
            return;
        }
        mAudioRecorder.setAudioRecordMode(captureAvailable ?
                DuerAudioRecordImpl.AudioRecordMode.ONE_SHOT :
                DuerAudioRecordImpl.AudioRecordMode.NORMAL,
                captureSession, soundModelHandle);
        beginVoiceRequest();
    }

    @Override
    public void stopRecognition() {
        dcsSdk.getVoiceRequest().endVoiceRequest(new IVoiceRequestListener() {
            @Override
            public void onSucceed() {
            }
        });
        isStopListenReceiving = false;
    }

    @Override
    public void updateContacts(String contacts) {
        final String trimContacts = contacts.trim();
        String lastUpMd5 = UploadPreference.getLastUploadPhoneContacts(mContext);
        if (trimContacts.equals(lastUpMd5)){
            Log.e(TAG, "updateContacts rejected since the same");
        }else {
            ((DcsSdkImpl) dcsSdk).getUpload().uploadPhoneContacts(mContext, trimContacts, new IUploadListener(){
                @Override
                public void onSucceed(int status){
                    Log.d(TAG, "updateContacts succeed! status = " + status);
                }
                @Override
                public void onFailed(){
                    Log.e(TAG, "updateContacts failed!");
                }
            });
        }
    }

    @Override
    public void releaseSdk() {
        if (screenDeviceModule != null) {
            screenDeviceModule.removeScreenListener(screenListener);
        }
        if (phoneCallDeviceModule != null){
            phoneCallDeviceModule.removePhoneCallListener(phoneCallListener);
        }
        if (smsDeviceModule != null){
            smsDeviceModule.removeSmsListener(smsListener);
        }
        if (appLauncherDeviceModule != null){
            appLauncherDeviceModule.removeAppLauncherListener(appLauncherListener);
        }
        dcsSdk.getVoiceRequest().removeDialogStateListener(dialogStateListener);
        dcsSdk.removeConnectionStatusListener(connectionStatusListener);
        getInternalApi().removeErrorListener(errorListener);
        getInternalApi().setLocationHandler(null);
        dcsSdk.release();
    }

    private void beginVoiceRequest(){
        if (!isStopListenReceiving){
            isStopListenReceiving = true;
            handleRenderVoiceInputText("");
            handleStartVoiceInput();
            dcsSdk.getVoiceRequest().cancelVoiceRequest(new IVoiceRequestListener() {
                @Override
                public void onSucceed() {
                    Log.d(TAG, "--beginVoiceRequest--");
                    dcsSdk.getVoiceRequest().beginVoiceRequest(true);
                }
            });
        }
    }

    private void endVoiceRequest(){
        dcsSdk.getVoiceRequest().endVoiceRequest(new IVoiceRequestListener() {
            @Override
            public void onSucceed() {
            }
        });
        handleStopVoiceInput();
        isStopListenReceiving = false;
    }

    private void handleStartVoiceInput(){
        for (IVoiceSdkListener listener: mVoiceSdkListeners){
            listener.onVoiceStart();
        }
        Log.d(TAG, "handleStartVoiceInput");
    }

    private void handleStopVoiceInput(){
        for (IVoiceSdkListener listener: mVoiceSdkListeners){
            listener.onVoiceStop();
        }
        Log.d(TAG, "handleStopVoiceInput");
    }

    private void handleRenderVoiceInputText(String text) {
        for (IVoiceSdkListener listener: mVoiceSdkListeners){
            listener.onRenderVoiceInputText(text);
        }
    }

    private InternalApi getInternalApi() {
        return ((DcsSdkImpl) dcsSdk).getInternalApi();
    }

    private int getAsrType() {
        return DcsConfig.ASR_MODE_ONLINE;
    }

    private void initSdk() {
        mAudioRecorder = new DuerAudioRecordImpl();
        mAudioRecorder.addRecorderListener(new BaseAudioRecorder.SimpleRecorderListener() {
            @Override
            public void onData(byte[] data) {

            }
        });

        IOauth oauth =  new BaiduOauthClientCredentialsImpl(CLIENT_ID, CLIENT_SECRET);
        DcsSdkBuilder builder = new DcsSdkBuilder();
        dcsSdk = builder.withClientId(CLIENT_ID)
                .withOauth(oauth)
                .withAudioRecorder(mAudioRecorder)
                .withPid(729)
                .build();

        getInternalApi().setDebug(true);
        getInternalApi().setAsrMode(getAsrType());

        IMessageSender messageSender = getInternalApi().getMessageSender();
        // add screen capacity
        screenDeviceModule = new ScreenDeviceModule(messageSender);
        screenDeviceModule.addScreenListener(screenListener);
        dcsSdk.putDeviceModule(screenDeviceModule);
        // add phone call capacity
        phoneCallDeviceModule = new PhoneCallDeviceModule(messageSender);
        initPhoneCallListener();
        phoneCallDeviceModule.addPhoneCallListener(phoneCallListener);
        dcsSdk.putDeviceModule(phoneCallDeviceModule);
        // add message capacity
        smsDeviceModule = new SmsDeviceModule(messageSender);
        initSmsListener();
        smsDeviceModule.addSmsListener(smsListener);
        dcsSdk.putDeviceModule(smsDeviceModule);
        // add lunch app capacity
        appLauncherDeviceModule = new AppLauncherDeviceModule(messageSender,new AppLauncherImpl(mContext));
        initAppLauncherListener();
        appLauncherDeviceModule.addAppLauncherListener(appLauncherListener);
        dcsSdk.putDeviceModule(appLauncherDeviceModule);
        // add set alarm capacity
        alarmsDeviceModule = new AlarmsDeviceModule(messageSender);
        initAlarmListener();
        alarmsDeviceModule.addAlarmListener(alarmListener);
        dcsSdk.putDeviceModule(alarmsDeviceModule);

        ((DcsSdkImpl) dcsSdk).getInternalApi().login(new ILoginListener() {
            @Override
            public void onSucceed(String accessToken) {
                dcsSdk.run();
                Log.d(TAG, "Login succeed.");
            }

            @Override
            public void onFailed(String errorMessage) {
                Log.d(TAG, "Login failed.");
            }

            @Override
            public void onCancel() {

            }
        });
    }

    private void initVoiceRequestListener() {
        dialogStateListener = new IDialogStateListener() {
            @Override
            public void onDialogStateChanged(DialogState dialogState) {
                switch (dialogState) {
                    case IDLE:
                        Log.d(TAG, "onDialogStateChanged: IDLE");
                        isStopListenReceiving = false;
                        handleStopVoiceInput();
                        break;
                    case LISTENING:
                        Log.d(TAG, "onDialogStateChanged: LISTENING");
                        isStopListenReceiving = true;
                        handleStartVoiceInput();
                        break;
                    case SPEAKING:
                        Log.d(TAG, "onDialogStateChanged: SPEAKING");
                        break;
                    case THINKING:
                        Log.d(TAG, "onDialogStateChanged: THINKING");
                        break;
                    default:
                        break;
                }
            }
        };
        dcsSdk.getVoiceRequest().addDialogStateListener(dialogStateListener);
    }

    private void initPhoneCallListener() {
        phoneCallListener = new PhoneCallDeviceModule.IPhoneCallListener() {
            @Override
            public void onPhoneCallByName(PhonecallByNamePayload payload) {
                List<String> contacts = new ArrayList<>();
                for (CandidateCallee callee: payload.getCandidateCallees()){
                    contacts.add(callee.getContactName());
                }
                String[] callees =  contacts.toArray(new String[contacts.size()]);
                queryContactsTask = new QueryContactsTask("");
                queryContactsTask.execute(callees);
            }

            @Override
            public void onSelectCallee(SelectCalleePayload payload) {
                Log.d(TAG, "PhoneCall: onSelectCallee");
            }

            @Override
            public void onPhoneCallByNumber(PhonecallByNumberPayload payload) {
                Uri uri = Uri.parse("tel:" + payload.getCallee().getPhoneNumber());
                Intent intent = new Intent(Intent.ACTION_CALL, uri);
                mContext.startActivity(intent);
            }
        };
    }

    private void initSmsListener() {
        smsListener = new SmsDeviceModule.ISmsListener() {
            @Override
            public void onSendSmsByName(SendSmsByNamePayload payload) {
                List<String> contacts = new ArrayList<>();
                for (CandidateRecipient callee: payload.getCandidateRecipients()){
                    contacts.add(callee.getContactName());
                }
                String[] callees =  contacts.toArray(new String[contacts.size()]);
                queryContactsTask = new QueryContactsTask(payload.getMessageContent());
                queryContactsTask.execute(callees);
            }

            @Override
            public void onSelectRecipient(SelectRecipientPayload payload) {
                Log.d(TAG, "Message : onSelectRecipient");
            }

            @Override
            public void onSendSmsByNumber(SendSmsByNumberPayload payload) {
                SmsSender smsSender = new SmsSender(mContext);
                smsSender.sendMessage(payload.getRecipient().getPhoneNumber(),
                        payload.getMessageContent());
            }
        };
    }

    private void initAppLauncherListener() {
        appLauncherListener = new AppLauncherDeviceModule.IAppLauncherListener() {
            @Override
            public void onLaunchApp(LaunchAppPayload payload) {
                if (!TextUtils.isEmpty(payload.getDeepLink())) {
                    appLauncherDeviceModule.getAppLauncher().launchAppByDeepLink(mContext,
                            payload.getDeepLink());
                } else if (!TextUtils.isEmpty(payload.getPackageName())) {
                    appLauncherDeviceModule.getAppLauncher().launchAppByPackageName(mContext,
                            payload.getPackageName());
                } else if (!TextUtils.isEmpty(payload.getAppName())) {
                    appLauncherDeviceModule.getAppLauncher().launchAppByName(mContext,
                            payload.getAppName());
                }
            }
        };
    }

    private void initAlarmListener() {
        alarmListener = new AlarmsDeviceModule.IAlarmDirectiveListener() {
            @Override
            public void onSetAlarmDirectiveReceived(SetAlarmPayload setAlarmPayload) {
                ArrayList<Integer> alarmDays = new ArrayList<>();
                for (String s : setAlarmPayload.getDays()) {
                    alarmDays.add(calendar.get(s));
                }
                Intent intent = new Intent(AlarmClock.ACTION_SET_ALARM);
                intent.putExtra(AlarmClock.EXTRA_MESSAGE, setAlarmPayload.getMessage());
                intent.putExtra(AlarmClock.EXTRA_DAYS, alarmDays);
                intent.putExtra(AlarmClock.EXTRA_HOUR, setAlarmPayload.getHour());
                intent.putExtra(AlarmClock.EXTRA_MINUTES, setAlarmPayload.getMinutes());
                if (intent.resolveActivity(mContext.getPackageManager()) != null) {
                    mContext.startActivity(intent);
                }
            }

            @Override
            public void onShowAlarmsDirectiveReceived(ShowAlarmsPayload showAlarmsPayload) {
                Log.d(TAG, "Alarm : onShowAlarmsDirectiveReceived");
            }

            @Override
            public void onSetTimerDirectiveReceived(SetTimerPayload setTimerPayload) {
                Log.d(TAG, "Alarm : onSetTimerDirectiveReceived");
            }

            @Override
            public void onShowTimersDirectiveReceived(ShowTimersPayload showTimersPayload) {
                Log.d(TAG, "Alarm : onShowTimersDirectiveReceived");
            }
        };
    }


    private class QueryContactsTask extends AsyncTask<String, Void, Integer> {
        private final String message;
        private final List<CandidateInfo> mCandidateInfos;
        private String logs = "";

        public QueryContactsTask(String message){
            this.message = message;
            mCandidateInfos = new ArrayList<>();
        }

        @Override
        protected Integer doInBackground(String... strings) {
            Log.v(TAG, "QueryContactsTask.doInBackground "+strings.length);

            for (String name: strings){
                ContentResolver cr = mContext.getContentResolver();
                Cursor cursor = cr.query(ContactsContract.Contacts.CONTENT_URI,null,
                        ContactsContract.PhoneLookup.DISPLAY_NAME + "=?",
                        new String[]{name},null);

                while(cursor.moveToNext()) {
                    int nameFieldColumnIndex = cursor.getColumnIndex(ContactsContract.PhoneLookup.DISPLAY_NAME);
                    String contact = cursor.getString(nameFieldColumnIndex);
                    String ContactId = cursor.getString(cursor.getColumnIndex(ContactsContract.Contacts._ID));
                    Cursor phone = cr.query(ContactsContract.CommonDataKinds.Phone.CONTENT_URI,
                            null, ContactsContract.CommonDataKinds.Phone.CONTACT_ID +
                                    "=" + ContactId, null, null);
                    CandidateInfo candidateInfo = new CandidateInfo(name, ContactId);
                    while(phone.moveToNext()) {
                        String phoneNumber = phone.getString(phone.
                                getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER));
                        String phoneType = phone.getString(phone.
                                getColumnIndex(ContactsContract.CommonDataKinds.Phone.TYPE));
                        CharSequence phoneTypeLabel = ContactsContract.CommonDataKinds.Phone.
                                getTypeLabel(mContext.getResources(),
                                Integer.parseInt(phoneType),"");
                        candidateInfo.addContactPhoneInfo(new CandidateInfo.Phone(phoneNumber,
                                phoneTypeLabel));
                        logs += (contact +":"+ phoneNumber +" "+ phoneTypeLabel +"\n");
                    }
                    mCandidateInfos.add(candidateInfo);
                }
                cursor.close();
            }
            Log.d(TAG, "QueryContactsTask.doInBackground result = "+logs);
            return 0;
        }

        @Override
        protected void onPostExecute(Integer returnStatus) {
            Log.v(TAG, "QueryContactsTask.onPostExecute returnStatus = "+returnStatus);
            if (returnStatus == 0) {
                if (TextUtils.isEmpty(message)){
                    for (IVoiceSdkListener listener: mVoiceSdkListeners){
                        listener.onPhoneCallByName(mCandidateInfos);
                    }
                }else {
                    for (IVoiceSdkListener listener: mVoiceSdkListeners){
                        listener.onSendSmsByName(mCandidateInfos, message);
                    }
                }
            }
        }
    }
}
