/*
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Based on NFC Handset Requirements Document v6.0
 */

package com.gsma.services.utils;

import com.gsma.services.nfc.ServiceUtils;
import java.lang.Exception;
import java.lang.reflect.Method;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.content.Context;
import android.util.Log;
import android.content.Intent;
import com.gsma.services.nfc.*;
import android.content.ServiceConnection;
import android.app.Activity;
import android.app.ActivityThread;
import android.os.IBinder;
import com.gsma.services.nfc.IGsmaService;
import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.io.IOException;

public class Handset
{
    private static final String TAG = "GsmaHandset";
    final static boolean DBG = true;

    public static final int BATTERY_LOW_MODE = 144;
    public static final int BATTERY_POWER_OFF_MODE = 145;
    public static final int BATTERY_OPERATIONAL_MODE = 146;
    public static final int FELICA = 32;
    public static final int HCI_SWP = 0;
    public static final int MIFARE_CLASSIC = 33;
    public static final int MIFARE_DESFIRE = 34;
    public static final int MULTIPLE_ACTIVE_CEE = 1;
    public static final int NFC_FORUM_TYPE3 = 35;
    public static final int OMAPI = 80;

    public static final String SE_NAME_SIM1 = "SIM1";
    public static final String SE_NAME_SIM2 = "SIM2";
    public static final String SE_NAME_ESE1 = "eSE1";
    public static final String SE_NAME_ESE2 = "eSE2";
    public static final String SE_NAME_SD1  = "SD1";

    private volatile IGsmaService mGsmaService;

    boolean mIsBound =false;
    String SEName  = null;
    String[] SENameList = null;
    boolean isConnected = false;
    Context mContext = null;
    MyConnection mConn = null;
    String mPackName = null;

    public class MyConnection implements ServiceConnection
    {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service)
        {
            if (DBG) Log.d(TAG, "onServiceConnected");
            mGsmaService = IGsmaService.Stub.asInterface(service);
        }

        @Override
        public void onServiceDisconnected(ComponentName className)
        {
            if (DBG) Log.d(TAG, "onServiceDiscnnected");
            mGsmaService = null;
        }
    };

    void bindService(Context context)
    {
        if (DBG) Log.d(TAG, "bindService()");
        mIsBound = ServiceUtils.bindService(context, mConn);
    }

    void unbindService(Context context)
    {
        if (DBG) Log.d(TAG, "unbindService()");
        if (mIsBound) {
            context.unbindService(mConn);
            mIsBound = false;
        }
    }

    public Handset()
    {
        mContext = getContext();
        mPackName = getPName();
        Log.d(TAG, "The current activity is " + mPackName);
        if (mConn == null ) {
            mConn = new MyConnection();
            bindService(mContext);
        }
    }

    // returns api version
    public int getVersion()
    {
        int version = 9000;
        return version;
    }

    private String getPName()
    {
        String packName = null;
        try {
            Class<?> activityThreadClass = Class.forName("android.app.ActivityThread");
            Class[] params = new Class[0];
            Method currentPackageName = activityThreadClass.getDeclaredMethod("currentPackageName", params);
            Object obj = currentPackageName.invoke(activityThreadClass);
            if (obj == null) {
                Log.d(TAG, "The current activity thread is null!");
                return null;
            }
            packName = (String)obj;
        }
        catch (Exception e) {
            Log.d(TAG, Log.getStackTraceString(e));
        }
        return packName;
    }

    protected static Context getContext()
    {
        Context context = null;
        try {
            Class<?> activityThreadClass = Class.forName("android.app.ActivityThread");
            Class[] params = new Class[0];
            Method currentActivityThread = activityThreadClass.getDeclaredMethod("currentActivityThread", params);
            Boolean accessible = currentActivityThread.isAccessible();
            currentActivityThread.setAccessible(true);
            Object obj = currentActivityThread.invoke(activityThreadClass);
            if (obj == null) {
                Log.d(TAG, "The current activity thread is null!");
                return null;
            }
            currentActivityThread.setAccessible(accessible);
            Method getSystemContext = activityThreadClass.getDeclaredMethod("getSystemContext", params);
            accessible = getSystemContext.isAccessible();
            getSystemContext.setAccessible(true);
            context = (Context) getSystemContext.invoke(obj);
            getSystemContext.setAccessible(accessible);
        }
        catch (Exception e) {
            Log.d(TAG, Log.getStackTraceString(e));
        }
        return context;
    }

    // returns true if feature is supported
    // all features are supported, just for debugging
    public boolean getProperty(int feature)
    {
        switch (feature)
        {
            case HCI_SWP:
            case FELICA:
            case MIFARE_DESFIRE:
            case NFC_FORUM_TYPE3:
            case OMAPI:
            case BATTERY_LOW_MODE:
            case MULTIPLE_ACTIVE_CEE:
                return true;
            case BATTERY_POWER_OFF_MODE:
            case MIFARE_CLASSIC:
                return false;
            default:
                throw new IllegalArgumentException("illegal argument");
        }
    }

    public List<String> getAvailableSecureElements(int batteryLevel)
    {
        String pkg = mContext.getPackageName();
        String[] secureElemArray = null;
        List<String> secureElementList = new ArrayList<String>();
        if (DBG) Log.d(TAG, "getAvailableSecureElements()");
        switch(batteryLevel)
        {
            case BATTERY_LOW_MODE:
            case BATTERY_POWER_OFF_MODE:
            case BATTERY_OPERATIONAL_MODE:
                try {
                    secureElemArray =  mGsmaService.getActiveSecureElementList(pkg);
                }
                catch(Exception e) {
                    secureElemArray = null;
                }
                break;
            default:
                if (DBG) Log.d(TAG, "In Default getAvailableSecureElements()");
                throw new IllegalArgumentException("illegal argument");
                //break;
        }
        if(secureElemArray != null && secureElemArray.length > 0x00) {
            Collections.addAll(secureElementList , secureElemArray);
            return secureElementList;
        }
         if (DBG) Log.d(TAG, "In empty getAvailableSecureElements()");
         return Collections.emptyList();
    }

    /**
     * after a call to this method, any authorized/registered components can receive a transaction
     *  event. This call shall not imply a power cycle of the mobile device, the modem or the UICC.
     *               enableMultiEvt_transactionReception
     */
    public void enableMultiEvt_transactionReception()
    {
        try {
            mGsmaService.mgetPname(mPackName);
        }
        catch (Exception e) {
            throw new SecurityException("Unable to get the third party app package name");
        }
        if (DBG) Log.d(TAG, "enableMultiEvt_transactionReception() ");
        try {
            SENameList = mGsmaService.getActiveSecureElementList(mPackName);
        } catch (Exception e) {
            Log.e(TAG, "mGsmaService.getActiveSecureElement() : " + e.getMessage());
            // SecurityException - if the application is not allowed to use this API
            throw new SecurityException("application is not allowed");
        }
        for(int i=0;i<SENameList.length;i++) {
            if ((SENameList[i].equals(SE_NAME_SIM1)==true) || (SENameList[i].equals(SE_NAME_SIM2)==true)) {
            SEName = SENameList[i];
            Log.e(TAG, " SENamelist[" + i+ "] : " + SENameList[i]);
            }
        }
        try {
            if (mGsmaService != null)
                mGsmaService.enableMultiReception(SEName);
        }
        catch (Exception e) {
            Log.e(TAG, "mGsmaService.enableMultiReception() : " + e.getMessage());
            // SecurityException - if the application is not allowed to use this API
            throw new SecurityException("application is not allowed");
        }
    }
}
