/*************************************************************************
 Copyright (c)2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
*************************************************************************/

package com.qualcomm.qti.presenceapp;

import java.lang.reflect.Method;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;
import android.content.SharedPreferences;
import android.os.RemoteException;
import android.database.ContentObserver;
import android.provider.Settings;
import android.os.Handler;
import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager;
import android.net.Uri;
import com.android.ims.ImsConfig;

public class DataBroadcastReceiver  {

    private static final String TAG = "DataBroadcastReceiver";
    public static boolean AppStart = true;

    private ConnectivityManager cm;
    private Context mContext;
    private int mPhoneCount = 0;

    /** Observer to get notified when Mobile data enabled status changes */
    private MobileDataContentObserver mObserver;

    public void setMobileDataContentObserver(Context context) {

     Log.d(TAG , "DataBroadcastReceiver(): setMobileDataContentObserver() : operator: "+AppGlobalState.getOperatorMode());
     //Register only for VZW mode .
         mContext = context;
         mPhoneCount = TelephonyManager.getDefault().getPhoneCount();

            Log.d(TAG, "mPhoneCount : "+mPhoneCount);

            mObserver = new MobileDataContentObserver();

               for (int i = 0; i < mPhoneCount; i++) {
                int[] subIds = SubscriptionManager.getSubId(i);
                Log.d(TAG ,"subIds length : "+subIds.length);
                 if (SubscriptionManager.isUsableSubIdValue(subIds[0])) {
                    mContext.getContentResolver().registerContentObserver(setUpUri(),false, mObserver);
                }
               }

    }

    public void removeMobileDataContentObserver() {

               for (int i = 0; i < mPhoneCount; i++) {
                int[] subIds = SubscriptionManager.getSubId(i);
                Log.d(TAG ,"subIds length : "+subIds.length);
                 if (SubscriptionManager.isUsableSubIdValue(subIds[0])) {
                    mContext.getContentResolver().unregisterContentObserver(mObserver);
                }
               }
    }

     private Uri setUpUri() {

       Uri dataUri = null;
       Log.d(TAG ,"setUpUri()");
       for (int i = 0; i < mPhoneCount; i++) {

         int[] subIds = SubscriptionManager.getSubId(i);
         Log.d(TAG ,"subIds length : "+subIds.length);

        if (SubscriptionManager.isUsableSubIdValue(subIds[0])) {
            if (mPhoneCount > 1) {
              dataUri = Settings.Global.getUriFor(Settings.Global.MOBILE_DATA + subIds[0]);
            } else {
              dataUri = Settings.Global.getUriFor(Settings.Global.MOBILE_DATA);
            }
        }
      }
      Log.d(TAG ,"setUpUri():dataUri:"+dataUri.toString());
      return dataUri;
    }


    private class MobileDataContentObserver extends ContentObserver {

         public MobileDataContentObserver() {
            super(new Handler());
         }

         @Override
         public void onChange(final boolean selfChange, Uri uri) {

             int enabled = 0;
             int phoneId = 0;
             int subId = 0;

             if (uri != null) {

               String authority = uri.getAuthority();
               String uriLastSegment = uri.getLastPathSegment();

               if (authority.equals("settings"))
               { // authority for mobile_data
                   if (mPhoneCount > 1) {
                       /*
                        * For multi-sim, the URI last segment contains subId in the format
                        * mobile_data0
                        */
                       String[] lastSegmentParts = uriLastSegment.split("\\d");
                       int uriLength = uriLastSegment.length();
                       int keyLength = lastSegmentParts[0].length();
                       subId = Integer.parseInt(uriLastSegment.substring(keyLength, uriLength));
                       phoneId = SubscriptionManager.getPhoneId(subId);
                       uriLastSegment = uriLastSegment.substring(0, keyLength);
                       Log.d(TAG, "MultiSim onChange(): subId = " + subId);
                   }
               }
             }

             try {
              if (mPhoneCount > 1)
               {
                enabled = Settings.Global.getInt(mContext.getContentResolver(),
                                                      Settings.Global.MOBILE_DATA + subId );
               }
               else
               {
                enabled = Settings.Global.getInt(mContext.getContentResolver(),
                                                      Settings.Global.MOBILE_DATA);
               }
             } catch (Exception e) {
               e.printStackTrace();
             }

             Log.d(TAG , "onChange:enabled:"+enabled);
             Log.d(TAG , "onChange Mobile data enabled status:"+((enabled !=0) ? "ON" : "OFF"));
             toggleQipcallConfig((enabled !=0),0);
             PublishTask publishMethod = new PublishTask();
             publishMethod.execute(false);
        }
    }

    private void toggleQipcallConfig(final boolean bInMobileDataEnabled,final int phoneId) {

        try
        {
            Log.d(TAG, "toggleQipcallConfig:bInMobileDataEnabled:"+ bInMobileDataEnabled);
            SavePreference(mContext, "DATA_KEY", bInMobileDataEnabled);
            com.qualcomm.qti.presenceapp.Settings.isMobile_data_enabled = bInMobileDataEnabled;
      new Thread(new Runnable(){
        @Override
        public void run(){
          ImsConfig imsConfig = AppGlobalState.getImsConfig(phoneId);
          try{
            imsConfig.setProvisionedValue(ImsConfig.ConfigConstants.MOBILE_DATA_ENABLED,bInMobileDataEnabled?ImsConfig.FeatureValueConstants.ON:ImsConfig.FeatureValueConstants.OFF);
            } catch(Exception e){
              e.printStackTrace();
              }
          }
        }).start();
    } catch (Exception exception) {
          Log.d(TAG, "Exception = " + exception);
          }
    }

    public void SavePreference(Context ctx, String key, boolean value) {

        SharedPreferences preferences = ctx.getSharedPreferences(
                "presencedata", Context.MODE_PRIVATE);

        SharedPreferences.Editor editor = preferences.edit();
        editor.putBoolean(key, value);
        Log.d(TAG, "Saving ....Data in preference " + value);
        editor.commit();
    }

    public void SaveNetworkPreference(Context ctx, String key, String value) {

        SharedPreferences preferences = ctx.getSharedPreferences(
                "presencedata", Context.MODE_PRIVATE);

        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(key, value);
        Log.d(TAG, "Saving ....Data in preference " + value);
        editor.commit();
    }
}
