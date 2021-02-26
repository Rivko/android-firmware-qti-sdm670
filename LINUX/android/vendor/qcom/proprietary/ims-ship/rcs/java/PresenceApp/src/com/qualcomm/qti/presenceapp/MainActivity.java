/*************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*************************************************************************/

package com.qualcomm.qti.presenceapp;

import android.opengl.Visibility;
import android.os.Bundle;
import android.view.Menu;
import android.telephony.TelephonyManager;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import com.qualcomm.qti.presenceapp.R;
import com.android.ims.internal.uce.uceservice.IUceService;
import com.android.ims.internal.uce.uceservice.ImsUceManager;
import com.android.ims.internal.uce.presence.IPresenceService;
import com.android.ims.internal.uce.options.IOptionsService;
import com.android.ims.internal.uce.common.StatusCode;
import com.android.ims.internal.uce.common.CapInfo;


import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ListActivity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.RemoteException;
import android.provider.ContactsContract;
import android.telephony.SubscriptionManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import android.content.pm.ResolveInfo;
import android.content.pm.ServiceInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import com.android.ims.ImsConfig;
import com.android.ims.ImsManager;
import com.android.ims.ImsConfigListener;
import android.telephony.TelephonyManager;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;

public class MainActivity extends ListActivity {
    private final static String TAG = "MainActivity";

    public Context mContext;
    ArrayList<Contact> mContacts;

    ContactArrayAdapter<Contact> mAdapter;
    ListenerHandler mListenerHandler;

    AsyncTask mPublishTask;
    AsyncTask mSubscribePollingTask;
    AsyncTask mImsEnablerTask;
    AsyncTask mUnPublishTask;

    public static MainActivity mainActivityObject;
    public static int AppStart = 1;
    public static int NewLaunch = 1;

    private static boolean isRCSServBound = false;
    private static boolean isIMSServBound = false;
    public static boolean appAbnormalClose = true;

    private BroadcastReceiver receiver;
    public  DataBroadcastReceiver dataBroadcastReceiver =  new DataBroadcastReceiver();

    public class ContactArrayAdapter<T> extends ArrayAdapter<T> {

        int layoutRes;
        ArrayList<Contact> contacts;

        public ContactArrayAdapter(Context context, int resource,
                int textViewResourceId, List<T> objects) {
            super(context, resource, textViewResourceId, objects);
            layoutRes = resource;
            contacts = (ArrayList<Contact>) objects;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            LinearLayout rowView;

            if (convertView == null) {
                rowView = new LinearLayout(getContext());

                String inflater = Context.LAYOUT_INFLATER_SERVICE;
                LayoutInflater vi = (LayoutInflater) getContext()
                        .getSystemService(inflater);
                vi.inflate(layoutRes, rowView, true);
            } else {
                rowView = (LinearLayout) convertView;
            }

            TextView nameView = (TextView) rowView.findViewById(R.id.name);
            TextView phoneView = (TextView) rowView.findViewById(R.id.phone);
            TextView noteView = (TextView) rowView.findViewById(R.id.Note);
            ImageView bmp = (ImageView) rowView.findViewById(R.id.icon);
            ImageView isMultiSelectedIcon = (ImageView) rowView
                    .findViewById(R.id.is_selected_icon);
            ImageView isExcludedIcon = (ImageView) rowView
                    .findViewById(R.id.is_excluded_icon);
            if(contacts != null && contacts.get(position) != null) {

            nameView.setText(contacts.get(position).getName());
            phoneView.setText(contacts.get(position).getPhone());
            noteView.setText(contacts.get(position).getNote());

            }
            updateIcon(bmp, position);
            updateExcludedIcon(isExcludedIcon, false);
            if (contacts != null && contacts.get(position) != null && contacts.get(position).isContactExcluded()) {
                updateExcludedIcon(isExcludedIcon, true);
                updateMultiSelectedIcon(isMultiSelectedIcon, false);

            } else {
                updateMultiSelectedIcon(isMultiSelectedIcon,
                        contacts.get(position).isMultiSelected());
            }
            return rowView;
        }

        private void updateExcludedIcon(ImageView isExcludedIcon,
                boolean visible) {
            if (visible) {
                isExcludedIcon.setVisibility(View.VISIBLE);
            } else {
                isExcludedIcon.setVisibility(View.INVISIBLE);
            }
        }

        private void updateMultiSelectedIcon(ImageView icon,
                boolean multiSelected) {
            if (multiSelected) {
                icon.setVisibility(View.VISIBLE);
            } else {
                icon.setVisibility(View.INVISIBLE);
            }
        }

        private void updateIcon(ImageView bmp, int position) {
            switch (contacts.get(position).getAvailability()) {
                case 0:
                    bmp.setImageResource(getResources().getIdentifier("icon",
                            "drawable", getPackageName()));
                    break;

                case 1:
                    bmp.setImageResource(getResources().getIdentifier("online",
                            "drawable", getPackageName()));
                    break;

                case 2:
                    bmp.setImageResource(getResources().getIdentifier("busy",
                            "drawable", getPackageName()));
                    break;

                default:
                    bmp.setImageResource(getResources().getIdentifier("icon",
                            "drawable", getPackageName()));
            }
        }
    }

    private SharedPreferences getSharedPrefHandle(String imsPresencePref) {
        SharedPreferences settings = getSharedPreferences(imsPresencePref, 0);

        return settings;
    }

    @Override
    protected void onRestart() {
        Log.d(TAG, "MainActivity onRestart hit");
        super.onRestart();
    }

    @Override
    protected void onResume() {
        Log.d(TAG, "MainActivity onResume hit");
        super.onResume();
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mContext = this;
        mainActivityObject = this;
        appAbnormalClose = true;

        Utility.initLiveLogging(mContext);
        Utility.initNotifyXMLFile(mContext);

        Log.d(TAG, "MainActivity onCreate hit");

        try
        {
            //adb shell setprop persist.OPERATOR_MODE 1
            Process proc = null;
            proc = Runtime.getRuntime().exec(new String[] {
                    "/system/bin/getprop", "persist.OPERATOR_MODE"
            });
            BufferedReader reader = new BufferedReader(new InputStreamReader(proc.getInputStream()));
            Log.d("PRESENCE_UI", "before my prop is:");
            String sMode = reader.readLine();
            Log.d("PRESENCE_UI", "my prop is: " + sMode);

            if(sMode != null && sMode.toString().equals(""))
            {
              sMode = "0";
            }

            if (sMode != null)
            {
                Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : sMode " + sMode);
                AppGlobalState.setOperatorMode(Integer.parseInt(sMode));
                Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : getOperatorMode " + AppGlobalState.getOperatorMode());
            }
            //adb shell setprop persist.CD_BY_OPTION 1
            Process procCd = null;
            procCd = Runtime.getRuntime().exec(new String[] {
                    "/system/bin/getprop", "persist.CD_BY_OPTION"
            });
            BufferedReader readerCd = new BufferedReader(new InputStreamReader(procCd.getInputStream()));
            Log.d("PRESENCE_UI", "before my sModeCd is:");

            String sModeCd = readerCd.readLine();
            if(sModeCd != null && sModeCd.toString().equals("")) {
               sModeCd = "0";
            }
            Log.d("PRESENCE_UI", "After my sModeCd is: " + sModeCd);

            if (sModeCd != null && Integer.parseInt(sModeCd) == 1)
            {
                Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : sModeCd " + sModeCd);
                AppGlobalState.setCdByOption(true);
                Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : isCdByOption " + AppGlobalState.isCdByOption());
            }
            else
            {
               AppGlobalState.setCdByOption(false);
               Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : isCdByOption " + AppGlobalState.isCdByOption());
            }

        } catch (IOException e) {

            e.printStackTrace();
        } catch (Exception e) {

            e.printStackTrace();
        }



        int dummyContactCount = getIntent().getIntExtra("COUNT", 0);
        Log.d(TAG, "dummyContactCount=" + dummyContactCount);
        if (dummyContactCount == 0) {
            mContacts = getContactsFromDB();
        } else {
            mContacts = getDummyContacts(dummyContactCount);
        }

        markExcludedContacts();

        AppGlobalState.setMainActivityContext(mContext);

        if (AppGlobalState.getTimerManager() == null) {
            Timer timerManager = new Timer();
            AppGlobalState.setTimerManager(timerManager);
        }

        AppGlobalState.setContacts(mContacts);

        linkArrayWithDisplayedList(mContacts);

        createListeningThread();

        SharedPreferences setting = getSharedPrefHandle(
                AppGlobalState.IMS_PRESENCE_MY_INFO);
        AppGlobalState.setMyInfoSettingHandle(setting);



        IntentFilter filter_available = new IntentFilter();
        filter_available.addAction("RCS_SERVICE_AVAILABLE");

        IntentFilter filter_unavailable = new IntentFilter();
        filter_unavailable.addAction("RCS_SERVICE_UNAVAILABLE");

        //Register for UCE feature
        IntentFilter filter_uceUp = new IntentFilter();
        filter_available.addAction(ImsUceManager.ACTION_UCE_SERVICE_UP);
        //uce DOwn indication
        IntentFilter filter_uceDown = new IntentFilter();
        filter_available.addAction(ImsUceManager.ACTION_UCE_SERVICE_DOWN);

        ImsManager imsManager = ImsManager.getInstance(mContext,0);
        if (imsManager != null) {
           Settings.isVt_calling_enabled = imsManager.isVtEnabledByUserForSlot();
           SharedPreferences preferences = mContext.getSharedPreferences("presencedata", Context.MODE_PRIVATE);
           SharedPreferences.Editor editor = preferences.edit();
           editor.putBoolean("VT_KEY", Settings.isVt_calling_enabled);
           editor.commit();
        }
        receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                // do something based on the intent's action

             Log.d("PRESENCE_UI", "intent.getAction():"+intent.getAction().toString());

                if(intent.getAction() != null && intent.getAction().equals("RCS_SERVICE_AVAILABLE"))
                {
                    Log.d("PRESENCE_UI", "MainActivity : onCreate : onReceive  RCS_SERVICE_AVAILABLE");
                    if(!isIMSServBound)
                    {
                        bindSettingService();
                        Log.d("PRESENCE_UI", "After Bind service isIMSServBound "+isIMSServBound);
                    }
                    }
                if(intent.getAction() != null && intent.getAction().equals("RCS_SERVICE_UNAVAILABLE"))
                {
                    Log.d("PRESENCE_UI", "MainActivity : onCreate : onReceive  RCS_SERVICE_UNAVAILABLE");

                }

                if(intent.getAction() != null && intent.getAction().equals(ImsUceManager.ACTION_UCE_SERVICE_DOWN))
                {
                    Log.d("PRESENCE_UI", "MainActivity : ACTION_UCE_SERVICE_DOWN");
                    releaseServices();

                    //ToDo: RcsService died retry service connection
                }

                if(intent.getAction() != null && intent.getAction().equals(ImsUceManager.ACTION_UCE_SERVICE_UP))
                {
                    //ToDo: RcsService died retry service connection
                    Log.d("PRESENCE_UI", "MainActivity : ACTION_UCE_SERVICE_UP");
                    bindRcsService();
                }
            }
        };
        registerReceiver(receiver, filter_available);
        registerReceiver(receiver, filter_unavailable);
        registerReceiver(receiver, filter_uceUp);
        registerReceiver(receiver, filter_uceDown);


        if (NewLaunch == 1)
        {

            Log.d("PRESENCE_UI", "LAUNCHING NOW... NewLaunch = " + NewLaunch);
            TelephonyManager teleMan = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
            int telephonyManagerNetworkType = teleMan.getNetworkType();
            Log.d("PRESENCE_UI", "MainActivity : onCreate : telephonyManagerNetworkType " + telephonyManagerNetworkType);

            switch (telephonyManagerNetworkType)
            {
                case TelephonyManager.NETWORK_TYPE_LTE:
                    Log.d("PRESENCE_UI", "MainActivity : onCreate : NETWORK_TYPE_LTE ");
                    ContactInfo.networkTypeLTE = true;
                    ContactInfo.vopsEnabled = true;
                    break;

                case TelephonyManager.NETWORK_TYPE_EHRPD:
                    Log.d("PRESENCE_UI", "MainActivity : onCreate : NETWORK_TYPE_EHRPD ");
                    ContactInfo.networkTypeLTE = false;
                    ContactInfo.vopsEnabled = false;
                    break;

                default:
                    ContactInfo.networkTypeLTE = false;
                    ContactInfo.vopsEnabled = false;
                    break;
            }

            NewLaunch = 0;
        }

        Log.d("PRESENCE_UI", "NewLaunch = " + NewLaunch);


    }


    void SetCustomFeatureTags() {

        try
        {
            Process procCd = null;
            String sModeCd = "0";
            procCd = Runtime.getRuntime().exec(new String[] {
                    "/system/bin/getprop", "persist.rcs.CustomFT"
            });
            BufferedReader readerCd = new BufferedReader(new InputStreamReader(procCd.getInputStream()));

            sModeCd = readerCd.readLine();

            if(sModeCd != null && sModeCd.equals("")) {
               sModeCd = "0";
            }
            Log.d("PRESENCE_UI", "SetCustomFeatureTags :After my sModeCd is: " + sModeCd);

            if (sModeCd != null && Integer.parseInt(sModeCd) == 1)
            {
                AppGlobalState.setCustomFTEnabled(true);
            }

        } catch (Exception e) {
            e.printStackTrace();
            Log.d("PRESENCE_UI", "RemoteException, Unable to set New Feature Tag");
        }
    }

    void startPresenceService(IUceService rcsService) {
        try {

            int presenceSerrviceHandle = AppGlobalState.getPresenceSerrviceHandle();
            if ( presenceSerrviceHandle == 0) {
                Log.d("PRESENCE_UI", "MainActivity : Creating New Presence Service");
                presenceSerrviceHandle = rcsService.createPresenceService(
                                            AppGlobalState.getListenerHandler().presListener,
                                            AppGlobalState.getPresenceListenerHandle());


            } else {
                Log.d("PRESENCE_UI", "MainActivity : PresenceServiceHandle already exists, destroy current handle");
                rcsService.destroyPresenceService(AppGlobalState.getPresenceSerrviceHandle());
                presenceSerrviceHandle = rcsService.createPresenceService(
                                            AppGlobalState.getListenerHandler().presListener,
                                            AppGlobalState.getPresenceListenerHandle());
            }
            Log.d("PRESENCE_UI", "MainActivity : presenceSerrviceHandle[" + presenceSerrviceHandle +"]");
      if(rcsService.getPresenceService() != null){
            Log.d("PRESENCE_UI","getPresenceService not null | handle: "+ String.valueOf(presenceSerrviceHandle));
            AppGlobalState.setPresenceService(rcsService.getPresenceService());
      }
            AppGlobalState.setPresenceSerrviceHandle(presenceSerrviceHandle);
            SetCustomFeatureTags();
        } catch (RemoteException e) {
            Log.d("PRESENCE_UI", " MainActivity : startOptionsService :  DeadObjectException dialog  ");
            e.printStackTrace();
        }
        catch (Exception e) {
            e.printStackTrace();
        }

    }

    void startOptionsService(IUceService rcsService){
        try {

            int optionsSrvHandle = AppGlobalState.getCdServiceHandle();
            if(optionsSrvHandle == 0) {
                Log.d("PRESENCE_UI", "MainActivity : Creating New Options Service");
                optionsSrvHandle = rcsService.createOptionsService(
                    AppGlobalState.getListenerHandler().cdServiceListener,
                    AppGlobalState.getOptionsListenerHandle());

            } else {
                Log.d("PRESENCE_UI", "MainActivity : OptionsServiceHandle already exists, destroy current handle");
                rcsService.destroyOptionsService(AppGlobalState.getCdServiceHandle());
                Log.d(TAG,"after QRCSDestroyCDService");
                optionsSrvHandle = rcsService.createOptionsService(
                    AppGlobalState.getListenerHandler().cdServiceListener,
                    AppGlobalState.getOptionsListenerHandle());
            }
            Log.d("PRESENCE_UI", "MainActivity : OptionsServiceHandle[" + optionsSrvHandle +"]");
            AppGlobalState.setCdServiceHandle(optionsSrvHandle);
            IOptionsService optionsService = rcsService.getOptionsService();
            AppGlobalState.setCdService(optionsService);
            optionsService.getMyInfo(optionsSrvHandle, AppGlobalState.getpUserData());

        } catch (RemoteException e) {
            Log.d("PRESENCE_UI", " MainActivity : startOptionsService :  DeadObjectException dialog  ");
            e.printStackTrace();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    void initAllServices(IUceService rcsService) {

        Log.d("PRESENCE_UI", "MainActivity initAllServices  : start");
    Settings.isVt_calling_enabled = getVtCallingStatus();
    Log.d("PRESENCE_UI","Settings.isVt_calling_enabled nonVZW" + String.valueOf(Settings.isVt_calling_enabled));

        //isRCSServBound = true;
        try {
            if (rcsService != null) {
                //isRCSServBound = true;
                boolean serviceStatus = false;
                serviceStatus = rcsService.getServiceStatus();
                if (true == serviceStatus) {
                    Log.d("PRESENCE_UI", "MainActivity : Rcs Service Status is TRUE");
                    startPresenceService(rcsService);
                    startOptionsService(rcsService);
                }
             Log.d("PRESENCE_UI", "MainActivity : setMobileDataContentObserver");
             dataBroadcastReceiver.setMobileDataContentObserver(mContext);
            } else {
                Log.d("PRESENCE_UI",
                        " MainActivity : initAllServices : QRCSStartRCSService  rcsListener.iQRCSListener is NULL  ");
            }
        } catch (RemoteException e) {
            Log.d("PRESENCE_UI", " MainActivity : initAllServices :  DeadObjectException dialog  ");
            e.printStackTrace();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
  /**
   ImsConfigSettings function to initialize the values and set it to settings fields.
   rcsimssettings now deprecated
   **/
  private ImsConfigListener.Stub imsConfigListener = new ImsConfigListener.Stub() {
        @Override
        public void onGetFeatureResponse(int feature, int network, int value, int status) {
      int vtCallEnabledVal = value;
      Log.d("PRESENCE_UI", " imsconfigsettings isVt_calling_enabled_valid " + vtCallEnabledVal);
      if(vtCallEnabledVal==1)
             Settings.isVt_calling_enabled=true;
           else
             Settings.isVt_calling_enabled=false;
      SharedPreferences preferences = mContext.getSharedPreferences("presencedata", Context.MODE_PRIVATE);
                     SharedPreferences.Editor editor = preferences.edit();
                     editor.putBoolean("VT_KEY", Settings.isVt_calling_enabled);
                     editor.commit();
                     if(AppGlobalState.getOperatorMode() != AppGlobalState.TMO_MODE)
                     {
             Log.d("PRESENCE_UI","publish was getting invoked from here | imsconfiglistener");
                     }


    }

        @Override
        public void onSetFeatureResponse(int feature, int network, int value, int status) {
        }

        @Override
        public void onGetVideoQuality(int status, int quality) {}

        @Override
        public void onSetVideoQuality(int status) {}

    };

  /**
  imsconfigsettings in place of rcsimssettings(deprecated now)
  */

    public void initImsConfigSettings(final int phoneId){
           new Thread(new Runnable(){
         public void run(){
       try{

       //phone id default 0
        Log.d("PRESENCE_UI", " imsconfigsettings | initializing settingsvariable for phone id 0");
        ImsConfig imsConfig = AppGlobalState.getImsConfig(0);

        //mobile data config settings
        TelephonyManager tm = (TelephonyManager)mContext.getSystemService(
                   Context.TELEPHONY_SERVICE);

        Settings.isMobile_data_enabled = false;
        int[] subIds = SubscriptionManager.getSubId(0);

        if (subIds != null && subIds.length != 0 &&
                SubscriptionManager.isUsableSubIdValue(subIds[0])) {
            Settings.isMobile_data_enabled = tm.getDataEnabled(subIds[0]);
        }

        Log.d("PRESENCE_UI", "imsconfigsettings isMobile_data_enabled " + Settings.isMobile_data_enabled);

        //presence settings
    Settings.volteUserOptInStatus = imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.VOLTE_USER_OPT_IN_STATUS);
    Log.d("PRESENCE_UI", " imsconfigsettings isPresenceEnabled " + Settings.volteUserOptInStatus);
            if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)
                         {
                         Log.d("PRESENCE_UI", "For ATT , make Presence check  = "
                                 + Settings.volteUserOptInStatus);
                           SharedPreferences preferences1 = mContext.getSharedPreferences("presencedata", Context.MODE_PRIVATE);
                           SharedPreferences.Editor editor1 = preferences1.edit();
               if(Settings.volteUserOptInStatus == 1)
                               editor1.putBoolean("PRESENCE_KEY", true);
               else
                   editor1.putBoolean("PRESENCE_KEY", false);
                           editor1.commit();
                         }

        //vtcalling settings
             Settings.isVt_calling_enabled = getVtCallingStatus();
             Log.d("PRESENCE_UI","Settings.isVt_calling_enabled VZW/ATT/TMO" + String.valueOf(Settings.isVt_calling_enabled));
             Log.d("PRESENCE_UI", "Current operator : " + AppGlobalState.getOperatorMode());
             if(AppGlobalState.getOperatorMode() != AppGlobalState.TMO_MODE)
                     {
            Log.d("PRESENCE_UI","publish was getting invoked from here | nonvzw");
                     }

        Log.d("PRESENCE_UI", " imsconfigsettings isVolte_enabled " + imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.VLT_SETTING_ENABLED ));

        Settings.publishExpiryTimer = imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.PUBLISH_TIMER);
                Log.d("PRESENCE_UI", " imsconfigsettings getPublish_expiry_timer " + Settings.publishExpiryTimer);

                Settings.publishExtendedExpiryTimer = imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.PUBLISH_TIMER_EXTENDED);
          Log.d("PRESENCE_UI", " imsconfigsettings getPublish_extended_expiry_timer " + Settings.publishExtendedExpiryTimer);

        Settings.listSubscriptionExpiryTimer = imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.CAPAB_POLL_LIST_SUB_EXP);
          Log.d("PRESENCE_UI", " imsconfigsettings getCapability_poll_list_subscription_expiry_timer " + Settings.listSubscriptionExpiryTimer);

        Settings.capabilityDiscoveryEnable = imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.CAPABILITY_DISCOVERY_ENABLED);
          Log.d("PRESENCE_UI", " imsconfigsettings isCapability_discovery_enable " + Settings.capabilityDiscoveryEnable);

        Settings.capabilityCacheExpiration = imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.CAPABILITIES_CACHE_EXPIRATION);
          Log.d("PRESENCE_UI", " imsconfigsettings getCapabilites_cache_expiration " + Settings.capabilityCacheExpiration);

        Settings.availabilityCacheExpiration = imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.AVAILABILITY_CACHE_EXPIRATION);
          Log.d("PRESENCE_UI", " imsconfigsettings getAvailability_cache_expiration " + Settings.availabilityCacheExpiration);

        Settings.capabilityPollInterval = imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.CAPABILITIES_POLL_INTERVAL);
          Log.d("PRESENCE_UI", " imsconfigsettings getCapability_poll_interval " + Settings.capabilityPollInterval);

        Settings.maxSubscriptionListEntries = imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.MAX_NUMENTRIES_IN_RCL);
          Log.d("PRESENCE_UI", " imsconfigsettings getMax_subcription_list_entries " + Settings.maxSubscriptionListEntries);

                Log.d("PRESENCE_UI", "Settings.inSettingScreen "
                            + Settings.inSettingScreen);
                    Runnable updateDataSetting = new Runnable() {
                        public void run() {
                            if (Settings.inSettingScreen) {
                                try {
                                    Log.d("PRESENCE_UI",
                                            " Before updateDataSetting");
                                    Settings.settingsObject.updateDataSetting();
                                    Log.d("PRESENCE_UI",
                                            " After updateDataSetting");

                                } catch (Exception e) {
                                    Log.d("PRESENCE_UI",
                                            " In Setting screen Exception");
                                    e.printStackTrace();
                                }
                            }

                            if (ContactInfo.inContactInfoScreen) {
                                try {
                                    Log.d("PRESENCE_UI",
                                            " Before updateVTCallButton");
                                    ContactInfo contactInfo = AppGlobalState.getContactInfo();
                                    if (contactInfo != null) {
                                        contactInfo.populateForm(contactInfo
                                                .getIndexOfDisplayedContact());
                                    }
                                    Log.d("PRESENCE_UI",
                                            " After updateVTCallButton");

                                } catch (Exception e) {
                                    Log.d("PRESENCE_UI",
                                            " In updateVTCallButton Exception");
                                    e.printStackTrace();
                                }
                            }
                        }
                    };
                    runOnUiThread(updateDataSetting);
                    Log.d("PRESENCE_UI", " runOnUiThread(updateDataSetting)");
                    Log.d("PRESENCE_UI", "For Video Settings.inSettingScreen "+ Settings.inSettingScreen);

                    //SharedPreferences preferences = mContext.getSharedPreferences(
                            //"ImsPresencePrefMyInfo", mContext.MODE_PRIVATE);
                    //SharedPreferences.Editor editor = preferences.edit();

                    /*if (Settings.isVt_calling_enabled) {
                        editor.putString("Description", "VoLTE Voice and Video Service");
                    } else {
                        editor.putString("Description", "VoLTE Service");
                    }
                    editor.commit();*/

                    if (Settings.inSettingScreen) {
                        Runnable updateVideoSetting = new Runnable() {
                            public void run() {
                                try {
                                    Log.d("PRESENCE_UI", " Before updateVideoSetting");
                                    Settings.settingsObject.updateVideoSetting();
                                    Log.d("PRESENCE_UI", " After updateVideoSetting");
                                } catch (Exception e) {
                                    Log.d("PRESENCE_UI", " In Setting screen Exception");
                                    e.printStackTrace();
                                }
                            }
                        };
                        runOnUiThread(updateVideoSetting);
                        Log.d("PRESENCE_UI", " runOnUiThread(updateVideoSetting)");
                    }


           Log.d("PRESENCE_UI", "AppStart = " + AppStart);
                    AppGlobalState.isDataSettingNvSame = true;


                    if (AppStart == 1 && (AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
                                      || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE)) {
                        boolean data = false;
                        ConnectivityManager cm = (ConnectivityManager) mContext
                                .getSystemService(Context.CONNECTIVITY_SERVICE);
                        try {
                            Class cmClass = Class.forName(cm.getClass().getName());
                            Method method = cmClass.getDeclaredMethod("getMobileDataEnabled");
                            method.setAccessible(true); // Make the method
                                                        // callback
                            // get the setting for "mobile data"
                            data = (Boolean) method.invoke(cm);
                            Log.d("PRESENCE_UI", "data from Settings  UI" + data);
                        } catch (Exception e) {
                            // Some problem accessible private API
                        }

                        Log.d("PRESENCE_UI", "Data value from setting UI = " + data);
            int temp;
            if(data)
              temp = 1;
            else
              temp = 0;


                        if (imsConfig.getProvisionedValue(ImsConfig.ConfigConstants.MOBILE_DATA_ENABLED) != temp) {
                            Log.d("PRESENCE_UI", "Data value from setting UI does not match with the resp_msg.isMobile_data_enabled()");
                            if (data) {
                                Log.d("PRESENCE_UI", "Setting Data Enabled value from Preference ");
                                imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.MOBILE_DATA_ENABLED,ImsConfig.FeatureValueConstants.ON);
                            } else {
                                Log.d("PRESENCE_UI", "Setting Data Disabled value from Preference ");
                                imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.MOBILE_DATA_ENABLED,ImsConfig.FeatureValueConstants.OFF);
                            }
                            Log.d("PRESENCE_UI", "Before QrcsImsSettings_SetQipcallConfig");
                            Log.d("PRESENCE_UI", "After QrcsImsSettings_SetQipcallConfig");
                            AppGlobalState.isDataSettingNvSame = false;
                            ContactInfo.firstPublish = false;
                        } else {
                            AppGlobalState.isDataSettingNvSame = true;
                            Log.d("PRESENCE_UI","Data value from setting UI match with the resp_msg.isMobile_data_enabled()");
                        }
                    }
                    Log.d(TAG, "QrcsImsSettings_QipcallConfigResponseCb : isDataSettingNvSame "+AppGlobalState.isDataSettingNvSame);
          AppStart = 0;

          Runnable invokePublishMethod = new Runnable() {
                            public void run() {
                                try {
                                    Log.d(TAG, "QrcsImsSettings_QipcallConfigResponseCb : before publish");
                            if(ContactInfo.firstPublish)
                            {
                Log.d("PRESENCE_UI","publish was getting invoked from here | contactfirstpublish");
                            }
                            else
                            {
                                ContactInfo.firstPublish = true;
                            }
                            Log.d(TAG, "QrcsImsSettings_QipcallConfigResponseCb : after publish");
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                            }
                        };
                        runOnUiThread(invokePublishMethod);



             Log.d("PRESENCE_UI", " getImsSettingsResp   2  ");

                         Runnable updateSetting = new Runnable() {
                             public void run() {
                                 try {
                                     Log.d("PRESENCE_UI", " In Setting screen"
                                             + Settings.inSettingScreen);
                                     if (Settings.inSettingScreen) {
                                         Log.d("PRESENCE_UI", " Before Populating values");
                                         Settings.settingsObject.updateSettingValues();
                                         Log.d("PRESENCE_UI", "After Populating values ");
                                     }

                                 } catch (Exception e) {
                                     Log.d("PRESENCE_UI", " In Setting screen Exception");
                                     e.printStackTrace();
                                 }
                             }
                         };
                         runOnUiThread(updateSetting);

                         Log.d("PRESENCE_UI", " runOnUiThread(updateSetting)");

       }catch(Exception e){
               Log.d("PRESENCE_UI", "imsconfig thread exception");
         e.printStackTrace();
       }
           }
             }).start();
  }

    private void markExcludedContacts() {
    try{
        Utility.prepareExcludedContactList();
        for (Contact c : mContacts) {
            if (Utility.isContactExcluded(c)) {
                c.setContactExcluded();
            }
        }
    } catch(Exception e){
      e.printStackTrace();
    }

    }

    @Override
    protected void onPause() {
        Log.d(TAG, "MainActivity onPause()");

        super.onPause();
    }

    @Override
    protected void onStop() {
        Log.d(TAG, "MainActivity onStop()");
        /*if (false) {
            // UT Only.
            for (Contact c : AppGlobalState.getContacts()) {
                TimerTask t = c.getSubscribeTimerTask();

                if (t != null) {
                    t.cancel();
                }
            }
        }*/
        super.onStop();
    }

    private ArrayList<Contact> getDummyContacts(int dummyContactCount) {
        ArrayList<Contact> contacts = new ArrayList<Contact>();
        for (int i = 0; i < dummyContactCount; i++) {
            if ((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
             || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE))
            {
                Contact c = new Contact("Test." + i, "555" + i, 0, "","");
                contacts.add(c);
            }
            else
            {
                Contact c = new Contact("Test." + i, "555" + i, 0, "","");
                contacts.add(c);
            }
        }
        return contacts;
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        AlertDialog.Builder closeApp = new AlertDialog.Builder(this);
        switch (id) {
            case 0:
                closeApp.setTitle(R.string.close_dialog_title)
                        .setMessage(R.string.close_dialog_message)
                        .setCancelable(false)
                        .setPositiveButton(R.string.close_dialog_positive_button,
                                new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                close();
                            }
                        })
                        .setNegativeButton(R.string.close_dialog_negative_button,
                                new DialogInterface.OnClickListener() {

                            public void onClick(DialogInterface dialog, int which) {
                                dialog.dismiss();
                            }
                        });
                break;
            default:
                break;
        }

        AlertDialog versionDialog = closeApp.create();
        return versionDialog;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            showDialog(0);
            return false;
        }
        return super.onKeyDown(keyCode, event);
    }

  private boolean getVtCallingStatus() {

       SharedPreferences preferences = mContext.getSharedPreferences(
                   "presencedata", Context.MODE_PRIVATE);

           boolean featureSupported = preferences.getBoolean("VT_KEY", false);
           Log.d(TAG, "vt_calling_from_sharedpref: "+featureSupported);

           return featureSupported;

    }

    private void close()
    {
        Log.d(TAG, "close hit");
        appAbnormalClose = false;
        ContactInfo.firstPublish = true;

        try
        {
            Log.d(TAG, "close before unregisterReceiver");
            unregisterReceiver(receiver);
            Log.d(TAG, "close After unregisterReceiver");
        } catch (Exception e)
        {
            e.printStackTrace();
        }

        dataBroadcastReceiver.removeMobileDataContentObserver();
        try
        {
            if (AppGlobalState.getIqRcsService() != null)
            {
                isRCSServBound = false;
                if(AppGlobalState.isCdByOption() && AppGlobalState.getCdServiceHandle() != 0)
                {
                  AppGlobalState.getIqRcsService().destroyOptionsService(AppGlobalState.getCdServiceHandle());
                  Log.d(TAG,"after QRCSDestroyCDService");
                }

                Log.d(TAG, "close PresServiceHandle:" + AppGlobalState.getPresenceSerrviceHandle());
                if (AppGlobalState.getPresenceSerrviceHandle() != 0)
                {
                    Log.d(TAG,
                            "close PresListenerHandle:"
                                    + AppGlobalState.getPresenceListenerHandle() + "Val:"
                                    + AppGlobalState.getPresenceListenerHandle().getUceLong());
                    if (AppGlobalState.getPresenceListenerHandle() != null
                            && AppGlobalState.getPresenceListenerHandle().getUceLong() != 0)
                    {
                        Log.d(TAG, "close Calling QPresService_RemoveListener");
                        AppGlobalState.getPresenceService().removeListener(
                                AppGlobalState.getPresenceSerrviceHandle(),
                                AppGlobalState.getPresenceListenerHandle());
                        Log.d(TAG, "close Calling QPresService_RemoveListener End");
                    }
                    AppGlobalState.getIqRcsService().destroyPresenceService(
                            AppGlobalState.getPresenceSerrviceHandle());
                    AppGlobalState.setPresenceSerrviceHandle(0);
                }
                else
                {
                    Log.d(TAG, "getPresenceSerrviceHandle null");
                }
                AppGlobalState.setPresenceService(null);
                Log.d(TAG, "Presence Service release");
            }
            Log.d(TAG, "close unbindService mQrcsServConn");
        } catch (RemoteException e1) {
            e1.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

        stopTimers();

        Utility.closeLiveLoggingFile();
        Utility.closeNotifyXMLFile();

        try
        {
            isIMSServBound = false;
            AppStart = 1;
            NewLaunch = 1;
            AppGlobalState.isDataSettingNvSame = false;
            Log.d(TAG, "close unbindService mQImsServConn");
        } catch (Exception e) {
            Log.d("PRESENCE_UI", "Exception " + e);
        }

        new Handler().postDelayed(new Runnable() {

            public void run() {

                finish();
            }
        }, 1000);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
    super.onConfigurationChanged(newConfig);
    Log.d(TAG, "onConfigurationChanged:newConfig:"+newConfig);
    }

    @Override
    public void onDestroy() {

        Log.d(TAG, "onDestroy hit appAbnormalClose " + appAbnormalClose);
        if (appAbnormalClose)
        {
            ContactInfo.firstPublish = true;
            try
            {
                Log.d(TAG, "onDestroy before unregisterReceiver");
                unregisterReceiver(receiver);
                Log.d(TAG, "onDestroy After unregisterReceiver");
            } catch (Exception e)
            {
                e.printStackTrace();
            }

            dataBroadcastReceiver.removeMobileDataContentObserver();
            try
            {
                if (AppGlobalState.getIqRcsService() != null)
                {
                  if(AppGlobalState.isCdByOption() && AppGlobalState.getCdServiceHandle() != 0)
                  {
                    AppGlobalState.getIqRcsService().destroyOptionsService(AppGlobalState.getCdServiceHandle());
                    Log.d(TAG,"after QRCSDestroyCDService");
                  }
                    isRCSServBound = false;
                    Log.d(TAG,
                            "onDestroy PresServiceHandle:"
                                    + AppGlobalState.getPresenceSerrviceHandle());
                    if (AppGlobalState.getPresenceSerrviceHandle() != 0)
                    {
                        Log.d(TAG,
                                "onDestroy PresListenerHandle:"
                                        + AppGlobalState.getPresenceListenerHandle() + "Val:"
                                        + AppGlobalState.getPresenceListenerHandle().getUceLong());
                        if (AppGlobalState.getPresenceListenerHandle() != null
                                && AppGlobalState.getPresenceListenerHandle().getUceLong() != 0)
                        {
                            Log.d(TAG, "onDestroy Calling QPresService_RemoveListener");
                            AppGlobalState.getPresenceService().removeListener(
                                    AppGlobalState.getPresenceSerrviceHandle(),
                                    AppGlobalState.getPresenceListenerHandle());
                            Log.d(TAG, "onDestroy Calling QPresService_RemoveListener End");
                        }
                        AppGlobalState.getIqRcsService().destroyPresenceService(
                                AppGlobalState.getPresenceSerrviceHandle());
                        AppGlobalState.setPresenceSerrviceHandle(0);
                    }
                    else
                    {
                        Log.d(TAG, "getPresenceSerrviceHandle null");
                    }
                    AppGlobalState.setPresenceService(null);
                    Log.d(TAG, "onDestroy after destroy presence service");
                }
                Log.d(TAG, "onDestroy after unbind rcs service");
            } catch (RemoteException e1) {
                e1.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }

            stopTimers();

            Utility.closeLiveLoggingFile();
            Utility.closeNotifyXMLFile();

            try
            {
                isIMSServBound = false;
                AppStart = 1;
                NewLaunch = 1;
                AppGlobalState.isDataSettingNvSame = false;
            } catch (Exception e) {
                Log.d("PRESENCE_UI", "Exception " + e);
            }
        }
        super.onDestroy();
    }

    private void stopTimers() {
        Log.d(TAG, "Stop timers");
        for (Contact c : AppGlobalState.getContacts()) {
            TimerTask t = c.getSubscribeTimerTask();

            if (t != null) {
                t.cancel();
            }
        }

    }

    private void createListeningThread() {

        if (AppGlobalState.getListenerLooper() == null) {
            HandlerThread listenerThread = new HandlerThread("Listener",
                    android.os.Process.THREAD_PRIORITY_BACKGROUND);

            listenerThread.start();
            Looper listenerLooper = listenerThread.getLooper();
            mListenerHandler = new ListenerHandler(mContext, listenerLooper);

            AppGlobalState.setListenerHandler(mListenerHandler);
            AppGlobalState.setListenerLooper(listenerLooper);

        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        super.onCreateOptionsMenu(menu);
        Log.d(TAG, "AppGlobalState.getIqRcsService() status : "+AppGlobalState.getIqRcsService());
        //if (AppGlobalState.getIqRcsService() != null) {
        getMenuInflater().inflate(R.menu.menu, menu);
        //}
        return true;

    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {

            case R.id.ims_enabler:
                sendLogMesg(mContext, "Menu option IMS Enabler selected.");
                mImsEnablerTask = new ImsEnablerTask(mContext).execute();
                return true;

            case R.id.publish_rich:
                sendLogMesg(mContext, "Menu option Publish Rich selected.");

                Log.d("PRESENCE_UI", "isCdByOption " + AppGlobalState.isCdByOption());

                if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)
                {
                    try {
                        if(AppGlobalState.getPresenceService() != null && AppGlobalState.getPresenceSerrviceHandle() != 0)
                        {
                            Log.d(TAG, "before calling QPresService_ReenableService");
                            AppGlobalState.getPresenceService().reenableService(
                                    AppGlobalState.getPresenceSerrviceHandle(), AppGlobalState.getpUserData());
                            Log.d(TAG, "after calling QPresService_ReenableService");
                        }
                        else
                        {
                            Log.d(TAG, "QPresService_ReenableService : service is null");
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

                if(AppGlobalState.isCdByOption()) {
          Log.d("PRESENCE_UI","invoking publish from isCdbyoption true");
                  invokePublish(true);
                } else {
          Log.d("PRESENCE_UI","invoking publish from isCdbyoption false");
                  invokePublish(false);
                }

                return true;

            case R.id.listsubscribepolling:
                invokeListSubscribePolling();
                return true;

            case R.id.listsubscribesimple:
                invokeListSubscribeSimple();
                return true;

            case R.id.myinfo:
                sendLogMesg(mContext, "Menu option MyInfo selected.");
                startMyInfoActivity();
                return true;

            case R.id.settings:
                sendLogMesg(mContext, "Menu option Settings selected.");
                startSettingsActivity();
                return true;

            case R.id.select_all:
                sendLogMesg(mContext, "Menu option SelectAll selected.");
                selectAllContacts(true);
                return true;

            case R.id.select_none:
                sendLogMesg(mContext, "Menu option SelectNone selected.");
                selectAllContacts(false);
                return true;

            case R.id.live_logging:
                startLiveLoggingActivity();
                return true;

            case R.id.refresh_contacts:

                int dummyContactCount = getIntent().getIntExtra("COUNT", 0);
                Log.d(TAG, "dummyContactCount=" + dummyContactCount);
                if (dummyContactCount == 0) {
                    mContacts = getContactsFromDB();
                } else {
                    mContacts = getDummyContacts(dummyContactCount);
                }

                markExcludedContacts();

                AppGlobalState.setMainActivityContext(mContext);

                AppGlobalState.setContacts(mContacts);

                linkArrayWithDisplayedList(mContacts);
                return true;
        }
        return false; // should never happen
    }

    private void invokePublish(Boolean manualPublish) {

        if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT && AppGlobalState.getIqRcsService() != null) {
            sendLogMesg(mContext, "Publish fmt=STRUCT based.");
            mPublishTask = new PublishTask().execute(manualPublish);
        }
    }

    private void invokeListSubscribePolling() {
        if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT) {
            sendLogMesg(mContext, "List subscribe polling fmt=STRUCT based.");
            mSubscribePollingTask = new ListAvailabilityFetchTask(mContext)
                    .execute();
        }
    }

    private void invokeListSubscribeSimple() {

        if((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
        || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE))
        {
            if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT) {
                sendLogMesg(mContext, "List subscribe simple fmt=STRUCT based.");
                mSubscribePollingTask = new ListCapabilityPollingTask(mContext).execute();
            }
        }
        else
        {
            Log.d(TAG, "AppGlobalState.ATT_MODE");
            ArrayList<Contact> contacts = AppGlobalState.getContacts();
            int listCount = 0;

            for (int i = 0; i < contacts.size(); i++) {
                if (contacts.get(i).isMultiSelected()) {
                    listCount++;
                }
            }

            Log.d(TAG, "invokeListSubscribeSimple listCount " + listCount);

            if (listCount > 0)
            {
                if (listCount == 1)
                {
                    if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT) {
                        sendLogMesg(mContext, "List subscribe polling fmt=STRUCT based.");
                        mSubscribePollingTask = new ListAvailabilityFetchTask(mContext).execute();
                    }
                }
                else
                {
                    if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT) {
                        sendLogMesg(mContext, "List subscribe simple fmt=STRUCT based.");
                        mSubscribePollingTask = new ListCapabilityPollingTask(mContext).execute();
                    }
                }
            }
            else
            {
                Toast.makeText(mContext, "None of the contact selected for list subscription.",
                        Toast.LENGTH_SHORT).show();
            }
        }
    }

    private void selectAllContacts(boolean flag) {
        for (Contact c : AppGlobalState.getContacts()) {
            if (Utility.isContactExcluded(c)) {
                c.setContactExcluded();
                c.setMultiSelected(false);
            } else {
                c.setMultiSelected(flag);
            }
        }
        AppGlobalState.getMainListAdapter().notifyDataSetChanged();
    }

    private void sendLogMesg(Context c, String string) {

        Utility.sendLogMesg(c, string);

    }

    private void startPublishActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.Publish");
        startActivity(i);
    }

    private void startMyInfoActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.MyInfo");
        startActivity(i);
    }

    private void startParActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.Par");
        startActivity(i);
    }

    private void startSettingsActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.Settings");
        startActivity(i);

    }

    private void dumpContacts(ArrayList<Contact> contacts) {
        for (Contact c : contacts) {
            Log.d(TAG, "Contact=" + c);
        }
    }

    private void startLiveLoggingActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.LiveLoggingActivity");
        startActivity(i);

    }

    private void linkArrayWithDisplayedList(ArrayList<Contact> contacts) {
        mAdapter = new ContactArrayAdapter<Contact>(this,
                R.layout.custom, R.id.name, contacts);

        setListAdapter(mAdapter);

        AppGlobalState.setMainListAdapter(mAdapter);
    }

    @Override
    protected void onListItemClick(ListView l, View v,
            int position, long id) {
        super.onListItemClick(l, v, position, id);
        Log.d(TAG, "selected=" + position);
        Log.d(TAG, "Contact=" + mContacts.get(position));
        startContactInfoActivity(position);
    }

    private void startContactInfoActivity(int index) {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.ContactInfo");
        i.putExtra("ContactIndex", index);

        sendLogMesg(mContext, "Starting ContactInfo");

        startActivity(i);
    }

    private ArrayList<Contact> getContactsFromDB() {
        ArrayList<Contact> contacts = new ArrayList<Contact>();

        /*
         * 1. Use the contacts.content_uri to get the id of the contacts, 2.
         * then query data.content_uri to get name and phone number. 3. Update
         * the arrayList contacts.
         */
        /*
         * Get a better way to get name and phone number from phoneDB.
         */
        TelephonyManager telMgr = (TelephonyManager)mContext.getSystemService(Context.TELEPHONY_SERVICE);
        String phoneNumber = telMgr.getLine1Number();
        Log.d("presence_ui", "Gnani: phoneNumber:" + phoneNumber);
    if(phoneNumber == null)
      phoneNumber = "";
        contacts.add(new Contact("Self", phoneNumber, 0, "<Not Subscribed>",""));

        Cursor contactsCursor = getContactsContentCursor();

        for (int i = 0; contactsCursor.moveToNext(); i++) {
            String id = getContactIdFromCursor(contactsCursor);
            Log.d(TAG, "getContactsFromDB() - "+id);
            Cursor dataCursor = getDataCursorRelatedToId(id);
            populateContactDataFromCursor(contacts, dataCursor);

            dataCursor.close();
        }
        Log.d("presence_ui", "data Before Cursor.close");
        contactsCursor.close();
        Log.d("presence_ui", "data After Cursor.close");
        return contacts;
    }

    private String getContactIdFromCursor(Cursor contactsCursor) {
        String id = contactsCursor.getString(contactsCursor
                .getColumnIndex(ContactsContract.Contacts._ID));
        return id;
    }

    private void populateContactDataFromCursor(ArrayList<Contact> contacts,
            Cursor dataCursor) {
        int nameIdx = dataCursor
                .getColumnIndex(ContactsContract.Data.DISPLAY_NAME);
        int phoneIdx = dataCursor
                .getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER);
        SharedPreferences preferences = this.getSharedPreferences(
                "presencedata", Context.MODE_PRIVATE);

        if (dataCursor.moveToFirst()) {
            // Extract the name.
            String name = dataCursor.getString(nameIdx);
            Log.d(TAG, "name from phonebook= " + name);
            // Extract the phone number.
            String rawNumber = dataCursor.getString(phoneIdx);
            Log.d(TAG, "rawNumber= " + rawNumber);
            if(rawNumber != null && rawNumber.length() > 0)
            {
                String number = getNormalizedNumber(rawNumber);
                Log.d(TAG, "number= " + number);
                if (number.length() > 0)
                {
                    Log.d(TAG, "number is not empty:Length:"+number.length());
                    if ((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
                     || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE))
                    {
                        contacts.add(new Contact(name, number, 0, "<Not Subscribed>",""));
                    }
                    else
                    {
                        Log.d(TAG, "setting " + preferences.getString("INDIVIDUAL_PRIFIX", ""));
                        Log.d(TAG, "setting " + preferences.getString("INDIVIDUAL_SUFFIX", ""));
                        if (!(preferences.getString("INDIVIDUAL_PRIFIX", "").equals(""))
                                || !(preferences.getString("INDIVIDUAL_SUFFIX", "").equals("")))
                        {
                            contacts.add(new Contact(name, number, 0, "<Not Subscribed>",preferences
                                    .getString("INDIVIDUAL_PRIFIX", "")
                                    + number
                                    + preferences.getString("INDIVIDUAL_SUFFIX", "")));
                        }
                        else
                        {
                            contacts.add(new Contact(name, number, 0, "<Not Subscribed>",""));
                        }
                    }
                }
            }
        }
    }

    private String getNormalizedNumber(String rawNumber) {
        Log.d(TAG, "getNormalizedNumber:RawNumber from phonebook= " + rawNumber);
        /*
         * remove "(", ")", "-" and "." from the number. Only integers or + is
         * allowed.
         */
        int len = rawNumber.length();
        String out = new String();
        Log.d(TAG, "getNormalizedNumber() : len= " + len);

        if(len > 0) {
        if (rawNumber.charAt(0) == '+') {
            out = "+";
        }

        for (int i = 0; i < len; i++) {
            char c = rawNumber.charAt(i);
            if (Character.isDigit(c)) {
                out += c;
            }
        }
        Log.d(TAG, "Normalized number= " + out);
        }
        return out;
    }

    private Cursor getDataCursorRelatedToId(String id) {
        String where = ContactsContract.Data.CONTACT_ID + " = " + id;

        Cursor dataCursor = getContentResolver().query(
                ContactsContract.Data.CONTENT_URI, null, where, null, null);
        return dataCursor;
    }

    private Cursor getContactsContentCursor() {
        Uri phoneBookContentUri = ContactsContract.Contacts.CONTENT_URI;
        String recordsWithPhoneNumberOnly = ContactsContract.Contacts.HAS_PHONE_NUMBER
                + "='1'";

        Cursor contactsCursor = getContentResolver().query(phoneBookContentUri, null,
                recordsWithPhoneNumberOnly, null, null);
        return contactsCursor;
    }

    static public class PresenceSolResponse {
        public int result;
        public Object data;
    }

    boolean bindSettingService()
    {
        if(AppGlobalState.getIqRcsService() != null){
        Log.d("PRESENCE_UI","starting imsconfigsetting");
    initImsConfigSettings(0);
        }
    return true;
    }

    boolean bindRcsService()
    {
        boolean bServStatus = false;
        Log.d("PRESENCE_UI", " bindRcsService Start");
        ImsUceManager uceMgr = AppGlobalState.GetUceManager();
        IUceService rcsService = null;
        if(uceMgr != null){
            Log.d("PRESENCE_UI", " bindRcsService found ImsUceManager");
            uceMgr.createUceService(false);
            rcsService = uceMgr.getUceServiceInstance();
            if(rcsService != null) {
                Log.d("PRESENCE_UI", " bindRcsService found IRCSService");
                AppGlobalState.setIqRcsService(rcsService);
        Log.d("PRESENCE_UI", "MainActivity : before bindsettingservice");
    //initializing the imsconfigsetting from here once rcs service has been set.
            bindSettingService();
        Log.d("PRESENCE_UI", "After bindsettingservice");
                initAllServices(rcsService);
                bServStatus = true;
            }
        }
        return bServStatus;

        /** Uncomment if using Proprietary RcsService

        PackageManager pm = this.getPackageManager();
        Intent i = new Intent();
        i.setAction("com.qualcomm.qti.rcsservice.QRCSService");
        List<ResolveInfo> l = pm.queryIntentServices(i, 0);
        Log.d("PRESENCE_UI", " bindRcsService l.size =   "+l.size());
        if(l.size() > 0)
        {
            ServiceInfo servInfo = l.get(0).serviceInfo;
            ComponentName name = new ComponentName(servInfo.applicationInfo.packageName, servInfo.name);
            Intent rcsServiceIntent = new Intent();
            rcsServiceIntent.setComponent(name);
            return bindService(rcsServiceIntent, mQrcsServConn, Context.BIND_AUTO_CREATE);
        }
        else
        {
            Log.d("PRESENCE_UI", " bindRcsService    no active setting service ");
            return false;
        } **/

    }


  void releaseServices() {

    if (AppGlobalState.getIqRcsService() != null) {

      // CDService
      if (AppGlobalState.isCdByOption()
          && AppGlobalState.getCdServiceHandle() != 0) {

        try {

          AppGlobalState.getIqRcsService().destroyOptionsService(
              AppGlobalState.getCdServiceHandle());
        } catch (Exception e) {
          e.printStackTrace();
        }

        AppGlobalState.setCdServiceHandle(0);

        AppGlobalState.setCdService(null);

      }

      // PresenceService
      if (AppGlobalState.getPresenceSerrviceHandle() != 0) {

        try {

          AppGlobalState.getPresenceService().removeListener(
              AppGlobalState.getPresenceSerrviceHandle(),
              AppGlobalState.getPresenceListenerHandle());
        } catch (Exception e) {
          e.printStackTrace();
        }

        try {
          AppGlobalState.getIqRcsService().destroyPresenceService(
              AppGlobalState.getPresenceSerrviceHandle());
        } catch (Exception e) {
          e.printStackTrace();
        }

        AppGlobalState.setPresenceSerrviceHandle(0);

        AppGlobalState.setPresenceService(null);

      }

      // RcsService
        AppGlobalState.setIqRcsService(null);
    }

  }

}
