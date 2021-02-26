/*************************************************************************
 Copyright (c)2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
*************************************************************************/

package com.qualcomm.qti.presenceapp;

import com.qualcomm.qti.presenceapp.R;
import com.android.ims.internal.uce.common.CapInfo;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.opengl.Visibility;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import com.android.ims.ImsConfig;
import com.android.ims.ImsManager;
import com.android.ims.ImsConfigListener;
import android.telephony.TelephonyManager;


public class Settings extends Activity {
    final String TAG = "Settings";

    int FIELD_TAG = 0;
    int FIELD_VALUE = 1;
    Context mContext;
    AsyncTask mNotifyFormatTask;
    AsyncTask mEventReportTask;
    public static Settings settingsObject;
    public static Context settingsContext;
    public static boolean inSettingScreen = false;
    EditText availabilityCacheView;
    CheckBox videoSetting;
    TextView dataStatusViewTitle;
    TextView dataStatusView;
    public static Context context;
    AsyncTask mPublishTask;

    public static class EventReport {
        int publishTriggerInd;
        int notifyInd;
        int enablerInd;

        @Override
        public String toString() {
            return "EventReport [publishTriggerInd=" + publishTriggerInd
                    + ", notifyInd=" + notifyInd + ", enablerInd=" + enablerInd
                    + "]";
        }

        public EventReport(int publishTriggerInd, int notifyInd, int enablerInd) {
            this.publishTriggerInd = publishTriggerInd;
            this.notifyInd = notifyInd;
            this.enablerInd = enablerInd;
        }

        public EventReport() {
            // TODO Auto-generated constructor stub
        }
    }

    public static int video_on_off;
    public static int availabilityCacheExpiration;
    public static int capabilityCacheExpiration;
    public static int capabilityDiscoveryEnable;
    public static int capabilityPollInterval;
    public static int listSubscriptionExpiryTimer;
    public static int maxSubscriptionListEntries;
    public static int minimumPublishInterval;
    public static int publishExpiryTimer;
    public static int publishExtendedExpiryTimer;
    public static int volteUserOptInStatus;
    public static boolean isAvailability_cache_expiration_valid;
    public static boolean isCapabilites_cache_expiration_valid;
    public static boolean isCapability_discovery_enable_valid;
    public static boolean isCapability_poll_interval_valid;
    public static boolean isCapability_poll_list_subscription_expiry_timer_valid;
    public static boolean isMax_subcription_list_entries_valid;
    public static boolean isMinimum_publish_interval_valid;
    public static boolean isPublish_expiry_timer_valid;
    public static boolean isPublish_extended_expiry_timer_valid;
    public static boolean isVolte_user_opted_in_status_valid;
    public static boolean isVt_calling_enabled_valid;
    public static boolean isVt_calling_enabled;
    public static boolean isMobile_data_enabled;
    public static boolean isMobile_data_enabled_valid;


    private boolean isVtCheckBoxStateChange;
    private boolean isFtCheckBoxStateChange;
    private boolean isChatCheckBoxStateChange;
    private boolean isPresenceCheckBoxStateChange;

    int formMap[][] = {

            {
                    R.string.set_notify_fmt_text, R.id.notify_fmt_Spinner
            },

            {
                    R.string.availability_cache_exp_text,
                    R.id.availability_cache_exp_value
            },
            {
                    R.string.capability_cache_exp_text,
                    R.id.capability_cache_exp_value
            },
            {
                    R.string.capability_poll_interval_text,
                    R.id.capability_poll_interval_value
            },
            {
                    R.string.capability_poll_list_exp_text,
                    R.id.capability_poll_list_exp_value
            },
            {
                    R.string.source_throttle_publish_text,
                    R.id.source_throttle_publish_value
            },
            {
                    R.string.subscribe_retry_text, R.id.subscribe_retry_value
            },
            {
                    R.string.publish_retry_text, R.id.publish_retry_value
            },
            {
                    R.string.max_list_entries_text, R.id.max_list_entries_value
            },

    };

    class SettingsMainThreadHandler extends Handler {

        final static int SET_NOTIFY_FORMAT_RESPONSE = 0;
        final static int SET_EVENT_REPORT_RESPONSE = 1;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case SET_NOTIFY_FORMAT_RESPONSE: {
                    String val = (String) msg.obj;
                    Log.d(TAG,
                            "Received msg in Setting activity main thread. msg.obj="
                                    + val);
                    updateNotifyFmtValue(val);
                    break;
                }
                case SET_EVENT_REPORT_RESPONSE: {
                    EventReport rep = (EventReport) msg.obj;
                    Log.d(TAG, rep.toString());
                    break;
                }

                default:
                    Log.e(TAG, "Unknown msg");
            }

            super.handleMessage(msg);
        }

        private void updateNotifyFmtValue(String val) {
            Spinner spinner = (Spinner) findViewById(R.id.notify_fmt_Spinner);
            ArrayAdapter adapter = (ArrayAdapter) spinner.getAdapter();
            int pos = adapter.getPosition(val);
            spinner.setSelection(pos);
        }
    }

    private void sendLogMesg(Context c, String string) {

        Utility.sendLogMesg(c, string);

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        settingsObject = this;
        settingsContext = getApplicationContext();
        mContext = this;
        inSettingScreen = true;
        this.setContentView(R.layout.settings);
        CheckBox chatSetting = (CheckBox) findViewById(R.id.chat_checkbox);

    Log.d("Presence.Settings","Operator mode:"+AppGlobalState.getOperatorMode());
        availabilityCacheView = (EditText) findViewById(R.id.availability_cache_exp_value);

        isVtCheckBoxStateChange = false;
        isChatCheckBoxStateChange = false;
        isFtCheckBoxStateChange = false;
        isPresenceCheckBoxStateChange = false;

        dataStatusViewTitle = (TextView) findViewById(R.id.data_textview_title);
        dataStatusView = (TextView) findViewById(R.id.data_textview);


        if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)
        {
            Log.d(TAG, "AppGlobalState.ATT_MODE");
            try
            {
                dataStatusViewTitle.setText("");
                dataStatusViewTitle.setHeight(0);
                dataStatusViewTitle.setVisibility(View.INVISIBLE);
                dataStatusView.setText("");
                dataStatusView.setHeight(0);
                dataStatusView.setVisibility(View.INVISIBLE);
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }
        }

        videoSetting = (CheckBox) findViewById(R.id.vt_checkbox);
        handleVtCheckBox(videoSetting);

            Log.d(TAG, "AppGlobalState.getOperatorMode():"+AppGlobalState.getOperatorMode());


            chatSetting.setChecked(getCheckboxStatus("CHAT_KEY"));

            if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE) {
                   isVt_calling_enabled = getCheckboxStatus("VT_KEY");
                   videoSetting.setChecked(isVt_calling_enabled);
                   videoSetting.setText(R.string.atnt_vt_settings_text);
                   chatSetting.setText(R.string.atnt_chat_text);
            }

            Log.d(TAG, "OnCreate:isVt_calling_enabled:"+isVt_calling_enabled+":isMobile_data_enabled:"+isMobile_data_enabled);

            if(isVt_calling_enabled && isMobile_data_enabled) {
              videoSetting.setChecked(true);
            }


        SettingsMainThreadHandler settingsHandler = new SettingsMainThreadHandler();
        AppGlobalState.setSettingsHandler(settingsHandler);

        initButtons();
        populateInitialValues();
    updateDataSetting();
    updateVideoSetting();
    updateSettingValues();
    }


    private boolean getCheckboxStatus(String key) {

       SharedPreferences preferences = mContext.getSharedPreferences(
                   "presencedata", Context.MODE_PRIVATE);

           boolean featureSupported = preferences.getBoolean(key, false);
           Log.d("Presence.Settings", "Key:"+key+":featureSupported:"+featureSupported);

           return featureSupported;

    }




    @Override
    protected void onStart() {
        super.onStart();
        inSettingScreen = true;
    }

    public void updateDataSetting() {

        CheckBox ftSetting = (CheckBox) findViewById(R.id.ft_checkbox);
        CheckBox chatSetting = (CheckBox) findViewById(R.id.chat_checkbox);

        if((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE) || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE))
        {
            Log.d("PRESENCE_UI", "Inside updateDataSetting isMobile_data_enabled =  "
                    + isMobile_data_enabled);
            if (isMobile_data_enabled) {
                dataStatusView.setText("Mobile Data : Enabled");
                ftSetting.setEnabled(true);
                chatSetting.setEnabled(true);
            }
            else {
                dataStatusView.setText("Mobile Data : Disabled");
                ftSetting.setEnabled(false);
                chatSetting.setEnabled(false);
            }
        }
        else
        {
            Log.d(TAG, "AppGlobalState.ATT_MODE");
            try
            {
                LinearLayout checkBoxLayout = (LinearLayout)findViewById(R.id.checkbox_layout);
                if(ftSetting != null)
                {
                    checkBoxLayout.removeView(ftSetting);
                }
                dataStatusViewTitle.setText("");
                dataStatusViewTitle.setHeight(0);
                dataStatusViewTitle.setVisibility(View.INVISIBLE);
                dataStatusView.setText("");
                dataStatusView.setHeight(0);
                dataStatusView.setVisibility(View.INVISIBLE);
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }
        }
    }

    public void updateVideoSetting() {

    Log.d(TAG, "updateVideoSetting():OpMode:"+AppGlobalState.getOperatorMode());
    Log.d(TAG, "isVt_calling_enabled:"+isVt_calling_enabled+":isMobile_data_enabled:"+isMobile_data_enabled);

        videoSetting = (CheckBox) findViewById(R.id.vt_checkbox);
        if((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE) || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE)
        || (AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE))
        {
           if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE) {
              isVt_calling_enabled = getCheckboxStatus("VT_KEY");
           }

            if (isVt_calling_enabled && !isMobile_data_enabled) {
                Log.d("PRESENCE_UI", "Data is disabled ... video was checked, video = "
                        + isVt_calling_enabled);
                videoSetting.setChecked(true);
                videoSetting.setEnabled(false);
            }
            else if (isVt_calling_enabled && isMobile_data_enabled) {
                Log.d("PRESENCE_UI", "Data is enabled ... video was checked , video = "
                        + isVt_calling_enabled);
                videoSetting.setChecked(true);
                videoSetting.setEnabled(true);
            }
            else if (!isVt_calling_enabled && !isMobile_data_enabled)
            {
                Log.d("PRESENCE_UI", " Data is disabled ...video was unchecked, video = "
                        + isVt_calling_enabled);
                videoSetting.setChecked(false);
                videoSetting.setEnabled(false);
            }
            else if (!isVt_calling_enabled && isMobile_data_enabled)
            {
                Log.d("PRESENCE_UI", "Data is enabled ... video was unchecked, video = "
                        + isVt_calling_enabled);
                videoSetting.setChecked(false);
                videoSetting.setEnabled(true);
            }
        }

    }

    public void SavePreference(Context ctx, String key, int value) {

        SharedPreferences preferences = ctx.getSharedPreferences("presencedata",
                MODE_PRIVATE);

        SharedPreferences.Editor editor = preferences.edit();
        editor.putInt(key, value);
        Log.d("PRESENCE_UI", "Saving ....Data in preference " + value);
        editor.commit();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.d("Presence.Settings", "Settings:ONPAUSE");
        inSettingScreen = false;
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d("PRESENCE_UI", "Settings:ON RESUME");
        inSettingScreen = true;
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d("PRESENCE_UI", "Settings:ON STOP");
        finish();
    }

    private void initButtons() {
        CheckBox chatSetting = (CheckBox) findViewById(R.id.chat_checkbox);

        if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)
        {
            CheckBox ftSetting = (CheckBox) findViewById(R.id.ft_checkbox);
            LinearLayout checkBoxLayout = (LinearLayout)findViewById(R.id.checkbox_layout);
            if(ftSetting != null)
            {
                checkBoxLayout.removeView(ftSetting);
            }
        }
        else
        {
            CheckBox ftSetting = (CheckBox) findViewById(R.id.ft_checkbox);
            handleFtCheckBox(ftSetting);
        }
//For AT, VZW and TMO
        handleChatCheckBox(chatSetting);


        CheckBox presenceSetting = (CheckBox) findViewById(R.id.presence_checkbox);
        handlePresenceCheckBox(presenceSetting);

        Button okButton = (Button) findViewById(R.id.ok);
        handleOkButtonClick(okButton);

        Button cancelButton = (Button) findViewById(R.id.cancel);
        handleCancelButtonClick(cancelButton);

        Button setNotifyFmtButton = (Button) findViewById(R.id.set_notify_fmt_button);
        handleSetNotifyButtonClick(setNotifyFmtButton);
        Button getNotifyFmtButton = (Button) findViewById(R.id.get_notify_current_fmt_button);
        handleGetNotifyButtonClick(getNotifyFmtButton);

    }

    private void handleSetNotifyButtonClick(Button setNotifyFmtButton) {
        setNotifyFmtButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                SharedPreferences.Editor editor = getSharedPrefEditor(
                        AppGlobalState.IMS_PRESENCE_SETTINGS);
                storeFormValuesToSharedPreferences(editor);
            }
        });
    }

    private void handleGetNotifyButtonClick(Button getNotifyFmtButton) {
        getNotifyFmtButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {

            }
        });
    }


 private void handleVtCheckBox(CheckBox vtCheckBox) {

     vtCheckBox.setOnClickListener(new OnClickListener() {

         public void onClick(View v) {

             if(isVtCheckBoxStateChange)
             {
                 isVtCheckBoxStateChange = false;
                 Log.d("PRESENCE_UI", "Setting : handleVtCheckBox : "+isVtCheckBoxStateChange);
             }
             else
             {
                 isVtCheckBoxStateChange = true;
                 Log.d("PRESENCE_UI", "Setting : handleVtCheckBox : "+isVtCheckBoxStateChange);
             }
         }
     });
 }

 private void handleFtCheckBox(CheckBox ftCheckBox) {
        ftCheckBox.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                if(isFtCheckBoxStateChange)
                {
                    isFtCheckBoxStateChange = false;
                }
                else
                {
                    isFtCheckBoxStateChange = true;
                }
            }
        });
    }


    private void handleChatCheckBox(CheckBox chatCheckBox) {
        chatCheckBox.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {

                if(isChatCheckBoxStateChange)
                {
                    isChatCheckBoxStateChange = false;
                }
                else
                {
                    isChatCheckBoxStateChange = true;
                }
            }
        });
    }

    private void handlePresenceCheckBox(CheckBox chatCheckBox) {

        try
        {
            if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)
            {
                Log.d(TAG, "AppGlobalState.ATT_MODE");
                chatCheckBox.setChecked(getCheckboxStatus("PRESENCE_KEY"));
        Log.d(TAG, "AppGlobalState.ATT_MODE | presenceCheckBox | PRESENCE_KEY: "+String.valueOf(getCheckboxStatus("PRESENCE_KEY")));
                chatCheckBox.setEnabled(true);
                chatCheckBox.setVisibility(View.VISIBLE);
                chatCheckBox.setOnClickListener(new OnClickListener() {

                    @Override
                    public void onClick(View v) {
                        if(isPresenceCheckBoxStateChange)
                        {
                            isPresenceCheckBoxStateChange = false;
                        }
                        else
                        {
                            isPresenceCheckBoxStateChange = true;
                        }
                    }
                });
            }
            else
            {
               Log.d(TAG, "Remove Presence checkbox");
                LinearLayout checkBoxLayout = (LinearLayout)findViewById(R.id.checkbox_layout);
                checkBoxLayout.removeView(chatCheckBox);
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }

   private ImsConfigListener.Stub imsConfigListener = new ImsConfigListener.Stub() {
        @Override
        public void onGetFeatureResponse(int feature, int network, int value, int status) {
      Log.d("PRESENCE_UI", "Settings| inside getfeatureresp imsconfiglistener " + String.valueOf(feature) );
    }

        @Override
        public void onSetFeatureResponse(int feature, int network, int value, int status) {
      CheckBox videoCheckBox = (CheckBox) findViewById(R.id.vt_checkbox);
      boolean isVTBoxChecked = videoCheckBox.isChecked();
            Log.d("PRESENCE_UI", "if setting vt call config response | status " + String.valueOf(status) );
      if(status == ImsConfig.OperationStatusConstants.SUCCESS){
                                isVt_calling_enabled = isVTBoxChecked;
                Log.d("PRESENCE_UI", "if setting vt call config response value" + String.valueOf(value));
                                Log.d("PRESENCE_UI", "if setting vt call config success | isVt_calling_enabled" + String.valueOf(isVt_calling_enabled));
                  }
                  else{
                                videoCheckBox.setChecked((!isVTBoxChecked));
                                Log.d("PRESENCE_UI", "if QrcsImsSettings_SetQipcallConfig failed | phoneId 0");
                                Toast.makeText(mContext, "Video Setting Failed", Toast.LENGTH_SHORT).show();
                            }
                  Log.d("PRESENCE_UI", "After QrcsImsSettings_SetQipcallConfig");
        }

        @Override
        public void onGetVideoQuality(int status, int quality) {}

        @Override
        public void onSetVideoQuality(int status) {}

    };


    private void handleVtImFtCheckBox()
    {
        CheckBox videoCheckBox = (CheckBox) findViewById(R.id.vt_checkbox);
        CheckBox ftCheckBox = (CheckBox) findViewById(R.id.ft_checkbox);
        CheckBox chatCheckBox = (CheckBox) findViewById(R.id.chat_checkbox);
        CheckBox presenceCheckBox = (CheckBox) findViewById(R.id.presence_checkbox);

        Log.d("Settings", "handleVtImFtCheckBox()");
        Log.d("Settings", "isFtCheckBoxStateChange:"+isFtCheckBoxStateChange);
        Log.d("Settings", "isChatCheckBoxStateChange:"+isChatCheckBoxStateChange);
        Log.d("Settings", "isVtCheckBoxStateChange:"+isVtCheckBoxStateChange);
        Log.d("Settings", "isPresenceCheckBoxStateChange:"+isPresenceCheckBoxStateChange);

        Log.d(TAG, "AppGlobalState.isCdByOption():"+AppGlobalState.isCdByOption());

        if(isFtCheckBoxStateChange || isChatCheckBoxStateChange)
        {
            SharedPreferences preferences = mContext.getSharedPreferences(
                    "presencedata", mContext.MODE_PRIVATE);
            SharedPreferences.Editor editor = preferences.edit();

            if(isFtCheckBoxStateChange)
            {
                if (ftCheckBox.isChecked() == true)
                {
                    Log.d("PRESENCE_UI", "FT is Checked");
                    editor.putBoolean("FT_KEY", true);
                    editor.putBoolean("FT_THUMB_KEY", true);
                    editor.putBoolean("FT_SNF_KEY", true);
                } else {
                    Log.d("PRESENCE_UI", "FT is UnChecked");
                    editor.putBoolean("FT_KEY", false);
                    editor.putBoolean("FT_THUMB_KEY", false);
                    editor.putBoolean("FT_SNF_KEY", false);
                }
            }

            if(isChatCheckBoxStateChange)
            {
                if (chatCheckBox.isChecked() == true)
                {
                    Log.d("PRESENCE_UI", "Chat is Checked");
                    editor.putBoolean("CHAT_KEY", true);
                    editor.putBoolean("IPME_KEY", true);
                } else {
                    Log.d("PRESENCE_UI", "Chat is UnChecked");
                    editor.putBoolean("CHAT_KEY", false);
                    editor.putBoolean("IPME_KEY", false);
                }
            }

            if(isPresenceCheckBoxStateChange)
            {
    Log.d("PRESENCE_UI", "presenceCheckBox.isChecked():"+presenceCheckBox.isChecked());

                if (presenceCheckBox.isChecked())
                {
                    Log.d("PRESENCE_UI", "Presence is Checked");
                    editor.putBoolean("PRESENCE_KEY", true);
                } else {
                    Log.d("PRESENCE_UI", "Presence is UnChecked");
                    editor.putBoolean("PRESENCE_KEY", false);
                }
            }

            editor.commit();

            Log.d("PRESENCE_UI", "Trigerring publish for FT or IM change ");
            sendLogMesg(mContext,"File transfer or Chat selected ...Menu option Publish Rich selected.");
        }

        if(isVtCheckBoxStateChange)
        {
            ContactInfo.firstPublish = true;
            boolean isVTBoxChecked = videoCheckBox.isChecked();
      Log.d("PRESENCE_UI", "Before setting vt calling prop | isVTBoxChecked " + String.valueOf(isVTBoxChecked));
                try
                {
                    Log.d("PRESENCE_UI", "Before setting vt calling prop");
          ImsConfig imsConfig = AppGlobalState.getImsConfig(0);
                    new Thread(new Runnable() {
                        public void run() {
            // calling on background thread

              try{

                //updating the setfeaturevalue api to toggle vtcalling nv for all operator modes.
                Log.d("PRESENCE_UI", "Current operator : " + AppGlobalState.getOperatorMode());
                imsConfig.setFeatureValue(ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE,TelephonyManager.NETWORK_TYPE_LTE,isVTBoxChecked?ImsConfig.FeatureValueConstants.ON:ImsConfig.FeatureValueConstants.OFF,imsConfigListener);
                } catch(Exception e){
                        e.printStackTrace();

                }

            }
            }).start();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
          if(isVtCheckBoxStateChange && (AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)) {

           isVt_calling_enabled = videoCheckBox.isChecked();
           Log.d("PRESENCE_UI", "Update VT Prefs val :isVt_calling_enabled:"+isVt_calling_enabled);
           updatePresencePrefs("VT_KEY", isVt_calling_enabled);
        }


        if(isVtCheckBoxStateChange || isFtCheckBoxStateChange || isChatCheckBoxStateChange)
        {
         Log.d("PRESENCE_UI", "AppGlobalState.getOperatorMode():"+AppGlobalState.getOperatorMode());

         Log.d("PRESENCE_UI", "isVtCheckBoxStateChange:"+isVtCheckBoxStateChange);
         Log.d("PRESENCE_UI", "isFtCheckBoxStateChange:"+isFtCheckBoxStateChange);
         Log.d("PRESENCE_UI", "isChatCheckBoxStateChange"+isChatCheckBoxStateChange);

     if(isVtCheckBoxStateChange)
       isVt_calling_enabled = videoCheckBox.isChecked();

        // Enabling publish trigger whenever there is a capability change

        if ((Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT))
                {
                    Log.d("PRESENCE_UI", "Before Call PublishTask from Settings ");
                    sendLogMesg(mContext, "Publish fmt=STRUCT based.");
                    mPublishTask = new PublishTask().execute(false);
                    Log.d("PRESENCE_UI", "After  Call PublishTask from Settings ");
                }

        }
        else
        {
            Log.d("PRESENCE_UI", "No Capability change for Video, im and ft ");
            Toast.makeText(mContext, "No Capability change", Toast.LENGTH_SHORT).show();
        }

        if((isVtCheckBoxStateChange || isFtCheckBoxStateChange || isChatCheckBoxStateChange || isPresenceCheckBoxStateChange))  //CD by OPTION & Setting changes
        {
            try
            {
                Log.d("PRESENCE_UI", "Before QCDService_GetMyCdInfo");

                CapInfo myCdInfo = AppGlobalState.GetCdInfo();
        if(AppGlobalState.getCdService() != null){
                AppGlobalState.getCdService().getMyInfo(AppGlobalState.getCdServiceHandle(), AppGlobalState.getpUserData());
                }
                if(AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE) {
                   Log.d("PRESENCE_UI", "AppGlobalState.VZW_MODE");
                   myCdInfo.setFtSupported(ftCheckBox.isChecked());
                   myCdInfo.setImSupported(chatCheckBox.isChecked());
                   myCdInfo.setFullSnFGroupChatSupported(chatCheckBox.isChecked());
                }

                if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE) {
                  Log.d("PRESENCE_UI", "AppGlobalState.ATT_MODE");
                  myCdInfo.setSmSupported(chatCheckBox.isChecked());
                  myCdInfo.setFtSupported(chatCheckBox.isChecked());
                  myCdInfo.setImSupported(chatCheckBox.isChecked());
                  myCdInfo.setCdViaPresenceSupported(presenceCheckBox.isChecked());
                }
                if(AppGlobalState.getCdService() != null){
                AppGlobalState.getCdService().setMyInfo(AppGlobalState.getCdServiceHandle(), myCdInfo, AppGlobalState.getpUserData());
        }
                Log.d("PRESENCE_UI", "After QCDService_SetMyCdInfo");
            }   catch (Exception e) {
                   e.printStackTrace();
            }
        }

        if(isPresenceCheckBoxStateChange)
        {
    updatePresencePrefs("PRESENCE_KEY", presenceCheckBox.isChecked());
    try{
                Log.d(TAG, "presence check box state is changed");
      Log.d("PRESENCE_UI", "before SetPresenceConfig ");
      final boolean presenceBoxChecked = presenceCheckBox.isChecked();
        new Thread(new Runnable() {
                        public void run() {
            // calling on background thread
                            try{
                    ImsConfig imsConfig = AppGlobalState.getImsConfig(0);
              if(presenceBoxChecked)
                    {
                        int res1 = imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.VOLTE_USER_OPT_IN_STATUS,ImsConfig.FeatureValueConstants.ON);
                        if(res1 == 0)
                        {
                            volteUserOptInStatus = 1;
                            Log.d("PRESENCE_UI", "After SetPresenceConfig | volteuseroptinstatus" + " " + String.valueOf(volteUserOptInStatus));
                        }
                    }
                    else
                    {
                        int res2 = imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.VOLTE_USER_OPT_IN_STATUS,ImsConfig.FeatureValueConstants.OFF);
                        if(res2 == 0)
                        {
                            volteUserOptInStatus = 0;
                            Log.d("PRESENCE_UI", "After SetPresenceConfig | volteuseroptinstatus" + " " + String.valueOf(volteUserOptInStatus));
                        }
                    }
          Log.d("PRESENCE_UI", "After SetPresenceConfig | PhoneId 0" + " " + String.valueOf(volteUserOptInStatus));
                }catch(Exception e){
                            e.printStackTrace();
                }
            }
            }).start();
    }catch(Exception e){
                e.printStackTrace();
            }
        }
    }

    private void handleOkButtonClick(Button cancelButton) {
        cancelButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                Log.d("PRESENCE_UI","On OK button clicked");
                handleVtImFtCheckBox();
                sendFormValues(0);
            }
        });
    }


    public void updatePresencePrefs(String key, boolean value) {

        SharedPreferences preferences = mContext.getSharedPreferences("presencedata",
                MODE_PRIVATE);

        SharedPreferences.Editor editor = preferences.edit();
        editor.putBoolean(key, value);
        Log.d("PRESENCE_UI", "Update prefs: key:" + key+":value:"+value);

        editor.commit();
    }


    private void handleCancelButtonClick(Button cancelButton) {
        cancelButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                isVtCheckBoxStateChange = false;
                isChatCheckBoxStateChange = false;
                isFtCheckBoxStateChange = false;
                finish();
            }
        });
    }

    public void updateSettingValues() {

        if (!isAvailability_cache_expiration_valid)
        {
            Log.d("PRESENCE_UI", "isAvailability_cache_expiration_valid View  "
                    + findViewById(R.id.availability_cache_exp_value));
            EditText availabilityCacheView = (EditText) findViewById(R.id.availability_cache_exp_value);
            Log.d("PRESENCE_UI", "isAvailability_cache_expiration_valid View  "
                    + availabilityCacheView);
            availabilityCacheView.setText("" + availabilityCacheExpiration);
            availabilityCacheView.invalidate();
        }

        if (!isCapabilites_cache_expiration_valid) {
            Log.d("PRESENCE_UI", "isCapabilites_cache_expiration_valid ");
            EditText capabilityCacheView = (EditText) findViewById(R.id.capability_cache_exp_value);
            capabilityCacheView.setText("" + capabilityCacheExpiration);
            capabilityCacheView.invalidate();

        }

        if (!isCapability_poll_interval_valid) {
            Log.d("PRESENCE_UI", "isCapability_poll_interval_valid ");
            EditText capabilityPollView = (EditText) findViewById(R.id.capability_poll_interval_value);
            capabilityPollView.setText("" + capabilityPollInterval);
            capabilityPollView.invalidate();
        }

        if (!isCapability_poll_list_subscription_expiry_timer_valid) {
            Log.d("PRESENCE_UI", "isCapability_poll_list_subscription_expiry_timer_valid ");
            EditText listSubscriptionExpiryTimerView = (EditText) findViewById(R.id.capability_poll_list_exp_value);
            listSubscriptionExpiryTimerView.setText("" + listSubscriptionExpiryTimer);
            listSubscriptionExpiryTimerView.invalidate();

        }

        if (!isMinimum_publish_interval_valid) {
            Log.d("PRESENCE_UI", "isMinimum_publish_interval_valid ");
            EditText sourceThrottlelView = (EditText) findViewById(R.id.source_throttle_publish_value);
            sourceThrottlelView.setText("" + minimumPublishInterval);
            sourceThrottlelView.invalidate();
        }

        if (!isMax_subcription_list_entries_valid) {
            Log.d("PRESENCE_UI", "isMax_subcription_list_entries_valid ");
            EditText maxSubscriptionListEntriesView = (EditText) findViewById(R.id.max_list_entries_value);
            maxSubscriptionListEntriesView.setText("" + maxSubscriptionListEntries);
            maxSubscriptionListEntriesView.invalidate();

        }

    }

    private void populateInitialValues() {
        SharedPreferences setting = getSharedPrefHandle(
                AppGlobalState.IMS_PRESENCE_SETTINGS);

        for (int i = 0; i < 4; i++) {

            String temp = getString(formMap[i][FIELD_TAG]);
            Log.d("PRESENCE_UI", temp);
            String uriValue = setting.getString(
                    getString(formMap[i][FIELD_TAG]), "");

            if (formMap[i][FIELD_VALUE] == R.id.notify_fmt_Spinner) {

                Spinner spinner = (Spinner) findViewById(formMap[i][FIELD_VALUE]);
                ArrayAdapter adapter = (ArrayAdapter) spinner.getAdapter();
                int pos = adapter.getPosition(uriValue);
                spinner.setSelection(pos);

            } else {
                Log.d("PRESENCE_UI", "DO NOTHING");
            }
        }

        SharedPreferences preferences = mContext.getSharedPreferences(
                "presencedata", Context.MODE_PRIVATE);
        boolean ftSupported = preferences.getBoolean("FT_KEY", false);
        boolean chatSupported = preferences.getBoolean("CHAT_KEY", false);
        Log.d("PRESENCE_UI", "ftSupported =" + ftSupported + "chatSupported =" + chatSupported);

        if((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE) || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE))
        {
            CheckBox ftSetting = (CheckBox) findViewById(R.id.ft_checkbox);
            ftSetting.setChecked(ftSupported);
        }

        CheckBox chatSetting = (CheckBox) findViewById(R.id.chat_checkbox);
        chatSetting.setChecked(chatSupported);
    }

    private SharedPreferences getSharedPrefHandle(String imsPresencePref) {
        SharedPreferences settings = getSharedPreferences(imsPresencePref, 0);

        return settings;
    }

    private Editor getSharedPrefEditor(String imsPresencePref) {
        SharedPreferences settings = getSharedPrefHandle(imsPresencePref);
        SharedPreferences.Editor editor = settings.edit();
        return editor;
    }

    private void storeFormValuesToSharedPreferences(Editor editor) {
        for (int i = 0; i < 4; i++) {
            if (formMap[i][FIELD_VALUE] == R.id.notify_fmt_Spinner) {

                Spinner spinner = (Spinner) findViewById(formMap[i][FIELD_VALUE]);
                editor.putString(getString(formMap[i][FIELD_TAG]), spinner
                        .getSelectedItem().toString());

            }
            else {
                Log.d("PRESENCE_UI", "DO NOTHING");
            }
        }
        editor.commit();
    }

    private void sendFormValues(final int phoneId) {

        Log.d("PRESENCE_UI", "sendFormValues ");
    final TextView availabilityCacheExpiry = (TextView) findViewById(R.id.availability_cache_exp_value);
    final TextView capabilityCacheExpiry = (TextView) findViewById(R.id.capability_cache_exp_value);
    final TextView capabilityPollInterval = (TextView) findViewById(R.id.capability_poll_interval_value);
    final TextView listSubscriptionExpiryTimer = (TextView) findViewById(R.id.capability_poll_list_exp_value);
    final TextView maxSubscriptionListEntries = (TextView) findViewById(R.id.max_list_entries_value);
    final TextView minPublishInterval = (TextView) findViewById(R.id.source_throttle_publish_value);

    final String availabilityCacheExpiryStr = availabilityCacheExpiry.getText().toString();
    final String capabilityCacheExpiryStr = capabilityCacheExpiry.getText().toString();
    final String capabilityPollIntervalStr = capabilityPollInterval.getText().toString();
    final String listSubscriptionExpiryTimerStr = listSubscriptionExpiryTimer.getText().toString();
    final String maxSubscriptionListEntriesStr = maxSubscriptionListEntries.getText().toString();
//    final String minPublishIntervalStr = minPublishInterval.getText().toString();

    new Thread(new Runnable() {
            public void run() {
        try{
          ImsConfig imsConfig = AppGlobalState.getImsConfig(0);
          if(AppGlobalState.objectValidator(availabilityCacheExpiryStr)){
          int resp1 = imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.AVAILABILITY_CACHE_EXPIRATION,Integer.parseInt(availabilityCacheExpiryStr));
            Log.d(TAG,"availabilityCacheExpiry "+ availabilityCacheExpiryStr + "Response: " + String.valueOf(resp1));
          }
          if(AppGlobalState.objectValidator(capabilityCacheExpiryStr)){
          int resp2 = imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.CAPABILITIES_CACHE_EXPIRATION,Integer.parseInt(capabilityCacheExpiryStr));
            Log.d(TAG,"capabilityCacheExpiry "+ capabilityCacheExpiryStr + "Response: " + String.valueOf(resp2));
          }
          if(AppGlobalState.objectValidator(capabilityPollIntervalStr)){
          int resp3 = imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.CAPABILITIES_POLL_INTERVAL,Integer.parseInt(capabilityPollIntervalStr));
            Log.d(TAG,"capabilityPollInterval "+ capabilityPollIntervalStr + "Response: " + String.valueOf(resp3));
          }
          if(AppGlobalState.objectValidator(listSubscriptionExpiryTimerStr)){
          int resp4 = imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.CAPAB_POLL_LIST_SUB_EXP,Integer.parseInt(listSubscriptionExpiryTimerStr));
            Log.d(TAG,"listSubscriptionExpiryTimer "+ listSubscriptionExpiryTimerStr + "Response: " + String.valueOf(resp4));
          }
          if(AppGlobalState.objectValidator(maxSubscriptionListEntriesStr)){
          int resp5 = imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.MAX_NUMENTRIES_IN_RCL,Integer.parseInt(maxSubscriptionListEntriesStr));
            Log.d(TAG,"maxSubscriptionListEntries "+ maxSubscriptionListEntriesStr + "Response: " + String.valueOf(resp5));
          }
        } catch(Exception e){
          e.printStackTrace();
        }
    //addding finish activity code from background after all the values are set
    try{
    runOnUiThread(new Runnable() {

                        @Override
                        public void run() {
                            finish();
                        }
                    });

              } catch(Exception e){
          e.printStackTrace();
          Log.d(TAG,"Exception inside finishactivity " );
        }
      }

      }).start();
    }
}

