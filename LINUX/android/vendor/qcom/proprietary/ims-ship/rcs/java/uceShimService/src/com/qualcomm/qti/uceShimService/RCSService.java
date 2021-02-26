/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.uceShimService;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.NoSuchElementException;

import com.qualcomm.qti.uceservice.V1_0.*;

import com.android.ims.internal.uce.uceservice.IUceService;
import com.android.ims.internal.uce.UceServiceBase;
import com.android.ims.internal.uce.uceservice.IUceListener;
import com.android.ims.internal.uce.uceservice.ImsUceManager;
import com.android.ims.internal.uce.common.UceLong;
import android.app.Activity;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IHwBinder;
import android.os.Message;
import android.os.ServiceManager;
import android.os.SystemProperties;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.os.SystemClock;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.content.Intent;
import android.content.IntentFilter;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.IccCardConstants;
import android.content.BroadcastReceiver;
import android.hidl.manager.V1_0.IServiceManager;
import android.hidl.manager.V1_0.IServiceNotification;

import android.os.RemoteException;

public class RCSService extends Service {

    /** Service Initialize Parameters  */
    public final static String INTENT_BOOT_COMPLETE_STR = "android.intent.action.BOOT_COMPLETED";
    public final static String INTENT_SIM_STATE_CHANGED_STR = "android.intent.action.SIM_STATE_CHANGED";
    public final static String INTENT_DPL_ACTION = "dpl.init.action";
    public final static String INTENT_PROCESS_ID = "PROCESS_ID";
    public final static String INTENT_ONE_TIME = "ONE_TIME";
    public final static String INTENT_RCS_TIMER_ACTION = "rcs.timer.action";
    public final static String INTENT_MODEM_STATUS_ACTION = "rcs.check.modem.status";
    public final static String INTENT_RESTART_RCS_SERVICE_ACTION = "rcs.restart.service";
    public static final int DEFAULT_DPL_MAX_RETRY = 3;
    private static final long DEFAULT_DPL_INIT_TIMEOUT = 10000;
    private static final long DEFAULT_MODEM_STATUS_CHECK_RETRY =  12000;
    public static final long DEFAULT_SERVICE_RECOVERY_TIMER =  8000;

    public static final int RCS_CONFIG_DISABLED = 0;
    public static final int RCS_CONFIG_ENABLED = 1;
    public static final int RCS_MODEM_IMS_DISABLED = 2;
    public static final int RCS_PERSIST_DISABLED = 3;

    //Service Registered boolean variable
    private boolean isServiceStarted = false;
    //Death REcipient boolean variable
    private boolean isHIDLDead = true;
    //death Binder Cookie
    private final long mDeathBinderCookie = 1011010;

    /** check support Type */
    private static int qRcsSupportType = 0;

    /** Initialization Parameters */
    public static com.qualcomm.qti.uceservice.V1_0.IUceService mHidlService = null;
    public static RCSServiceStub objQRCSServiceStub = null;
    public static Context context; // context usage
    public static Context ctx;
    private static boolean isRCSServiceStartCalled = false;
    public Activity serviceConfigActivity = new Activity();
    private static boolean isLoadLibrary = false;
    private com.qualcomm.qti.uceservice.V1_0.IUceListener iQRCSListener = null;//hidl UceListener

    /** Logging Tag */
    private final static String logTAG = "QRCSService";

    /** Service Manager Identity to register with */
    private static final String UCE_SERVICE_NAME = "uce";
    private RCSListener mHidlUceListener = new RCSListener();

    private final Object mLock = new Object();

    private IServiceManager mIServiceManager = null;

   private HandlerThread mEventThread;
   private Handler mHandler;

    private final IServiceNotification mServiceNotificationCallback =
        new IServiceNotification.Stub() {
        public void onRegistration(String fqName,
                                   String name,
                                   boolean preexisting) {
            synchronized (mLock) {
                Log.i(logTAG,"IServiceNotification.onRegistration");
                startService();
            }
        }
    };

    private IHwBinder.DeathRecipient mDeathRecipient = new IHwBinder.DeathRecipient() {
        @Override
        public void serviceDied(long cookie) {
            Log.d(logTAG,"HIDL SErvice Died");
            if (cookie == mDeathBinderCookie) {
                try {
                    mHidlService.unlinkToDeath(mDeathRecipient);

                } catch (RemoteException e) {
                    Log.i(logTAG, "RemoteException : unable to unlink DeathRecipient");
                } catch (NoSuchElementException e) {
                    Log.i(logTAG, "NoSuchElementException : DeathRecipient has not been registered");
                }
                mHidlService = null;
                isHIDLDead = true;
                //Broadcast Service Intent : UceService DOWN
                Intent intent = new Intent();
                Log.d(logTAG,"Sending ACTION_UCE_SERVICE_DOWN");
                intent.setAction(ImsUceManager.ACTION_UCE_SERVICE_DOWN);
                objQRCSServiceStub.setServiceStatus(false);
                context.sendBroadcast(intent);
                //reset isServiceStarted
                isServiceStarted = false;
                mIServiceManager = null;
                registerForHidlServiceNotifications();
        }
        }
    };

    void registerForHidlServiceNotifications(){
                //register for Notifications
                try{
                    if(mIServiceManager != null) {
                        Log.i(logTAG, "Already have a Service Manager running");
                    }
                    else{
                        mIServiceManager = IServiceManager.getService();
                        if( mIServiceManager == null ) {
                            Log.e(logTAG, "Failed to obtain Service Manager");
                        }
                        else {
                            if (!mIServiceManager.registerForNotifications(
                                "com.qualcomm.qti.uceservice@1.0::IUceService",
                                "com.qualcomm.qti.uceservice",
                                mServiceNotificationCallback)) {
                                Log.e(logTAG,
                                      "Failed to register for notifications to IUceService");
                                }
                       }
                    }
                }catch (RemoteException e){
                    Log.i(logTAG, "Caught Exception:", e);
                }
    }

    synchronized private void startService() {
        if( !isServiceStarted ) {
            Log.i(logTAG, "Trying to get HIDL-IUceService");
            try {
                mHidlService = com.qualcomm.qti.uceservice.V1_0.IUceService.getService(
                                                         "com.qualcomm.qti.uceservice");
            }
            catch(RemoteException e) {
                Log.i(logTAG, "Caught Exception:", e);
            }
            catch(java.util.NoSuchElementException nSE) {
                Log.i(logTAG, "Caught Exception: Service not Registered yet", nSE);
                registerForHidlServiceNotifications();
            }
            catch (Exception e) {
                Log.i(logTAG, "Caught Exception:", e);
            }

            if(mHidlService != null) {
                try {
                    mHidlService.addUceListener(mHidlUceListener);
                }
                catch (RemoteException | RuntimeException e) {
                    Log.i(logTAG, "Unable to add HIDL Listener Exception :" + e);
                }

                Log.i(logTAG, "Received HIDL-UCEService");
                if (objQRCSServiceStub != null && objQRCSServiceStub.setHidlUceSerice(mHidlService)) {
                    try{
                        //register to Service Manager
                        ServiceManager.addService(UCE_SERVICE_NAME, objQRCSServiceStub.getBinder());
                        objQRCSServiceStub.setServiceStatus(true);

                        //Broadcast Service Intent
                        //Firing intent to Open source Client
                        isServiceStarted = true;
                        Intent uceServiceIntent = new Intent();
                        Log.d(logTAG,"Sending ACTION_UCE_SERVICE_UP");
                        uceServiceIntent.setAction(ImsUceManager.ACTION_UCE_SERVICE_UP);
                        context.sendStickyBroadcast(uceServiceIntent);

                        //Register to Death Recipient
                        boolean isLinkSuccesful= mHidlService.linkToDeath(mDeathRecipient,
                                                                     mDeathBinderCookie);
                        isHIDLDead = !(isLinkSuccesful);
                    } catch (RemoteException e){
                        Log.i(logTAG, "Caught Exception :" + e);
                    }
                } else {
                    Log.i(logTAG, "setHidlUceService failed");
                    mHidlService = null;
               }
            }
        }
        else{
            Log.i(logTAG, "Service is already started");
        }
    }

    private void postServiceDown() {
        if (isServiceStarted) {
            Intent intent = new Intent();
            Log.d(logTAG,"Sending ACTION_UCE_SERVICE_DOWN");
            intent.setAction(ImsUceManager.ACTION_UCE_SERVICE_DOWN);
            objQRCSServiceStub.setServiceStatus(false);
            this.sendBroadcast(intent);

            if(mHidlService != null) {
                try {
                    mHidlService.unlinkToDeath(mDeathRecipient);
                } catch (RemoteException e) {
                    Log.i(logTAG, "postServiceDown: RemoteException : unable to unlink DeathRecipient");
                } catch (NoSuchElementException e) {
                    Log.i(logTAG, "NoSuchElementException : DeathRecipient has not been registered");
                }
                mHidlService = null;
            }
            isServiceStarted = false;
        }
    }

    public static void setServiceStatus(boolean bStatus) {
        isRCSServiceStartCalled = bStatus;
    }

    public static boolean getServiceStatus() {
        return isRCSServiceStartCalled;
    }


    BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            if (intent == null) {
               Log.i(logTAG, "received null intent");
               return;
            }

            final String action = intent.getAction();
            if (action == null) {
               Log.i(logTAG, "received null intent action");
               return;
            }

            if (action.equals(INTENT_SIM_STATE_CHANGED_STR)) {
                Log.d(logTAG, "SIM State Changed Event Received");
                String state = intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);

                /* If SIM is inserted and ready then it should be either ICCID_READY
                   IMSI ready or LOADED any other state means that SIM is removed or
                   not READY yet  */
                boolean isSimInserted = (IccCardConstants.INTENT_VALUE_ICC_READY.equals(state)
                                         || IccCardConstants.INTENT_VALUE_ICC_IMSI.equals(state)
                                         || IccCardConstants.INTENT_VALUE_ICC_LOADED.equals(state));

                /* Start the service if SIM is inserted and ICCID is ready. */
                if (isSimInserted) {
                    startService();
                } else {
                    postServiceDown();
               }
            }
        }
    };

//refactor
    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(logTAG, "onCreate: Attempt to start service");
        ctx = getApplicationContext();
        context = this;

        /* Register for SIM state intent */
        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);

        mEventThread = new HandlerThread("MainEventThread");
        mEventThread.start();
        mHandler = new Handler(mEventThread.getLooper()){};

        context.registerReceiver(mIntentReceiver, filter, null, mHandler);

        if(iQRCSListener == null) {
            iQRCSListener = new RCSListener();
        } else {
            Log.i(logTAG, "onCreate: iQRCSListener already exists: check whether OnCreate Called Multiple Times");
        }

        if(objQRCSServiceStub == null) {
            objQRCSServiceStub = new RCSServiceStub();
        } else {
            Log.i(logTAG, "onCreate: objQRCSServiceStub already exists: check whether OnCreate Called Multiple Times");
        }
        objQRCSServiceStub.SetContext(context);
        Process proc = null;
    }

    @Override
    public void onStart(Intent intent, int startId) {
        Log.i(logTAG, "onStart: Attempt to start Service");
        super.onStart(intent, startId);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Log.i(logTAG, "onStartCommand- Call");
        Thread serviceStatusCheckThread = new Thread(){
                public void run(){
                    try {
                        if(!isServiceStarted) {
                            registerForHidlServiceNotifications();
                        }
                        else {
                            Log.i(logTAG, "Already HIDL-IUceService is Started ");
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
        };
        serviceStatusCheckThread.start();
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        //Broadcast Service Intent that Uce Service is Down
        Intent intent = new Intent();
        Log.d(logTAG,"Sending ACTION_UCE_SERVICE_DOWN");
        intent.setAction(ImsUceManager.ACTION_UCE_SERVICE_DOWN);
        objQRCSServiceStub.setServiceStatus(false);
        this.sendBroadcast(intent);

        Log.i(logTAG, "onDestroy: Call stopRCSService");
        try {
            if (getServiceStatus() == true) {
                RCSListener.isRCSRegistered = false;
            }
        } catch (UnsatisfiedLinkError e) {
            Log.i(logTAG, "onDestroy- stopRCSService error = " + e);
        } catch (Exception e) {
            Log.i(logTAG, "onDestroy- stopRCSService exception = " + e);
        }
        if(mHidlService != null) {
            try {
                mHidlService.unlinkToDeath(mDeathRecipient);
            } catch (RemoteException e) {
                Log.i(logTAG, "OnDestroy: RemoteException : unable to unlink DeathRecipient");
            } catch (NoSuchElementException e) {
                Log.i(logTAG, "NoSuchElementException : DeathRecipient has not been registered");
            }

            mHidlService = null;
        }

    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(logTAG, "onBind: Received: return NULL as this feature is not supported");
        return null;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        Log.i(logTAG, "onUnbind Received: Nothing to do");
        return false;
    }

    public IUceService.Stub getRCSServiceImpl() {
        try {
            return objQRCSServiceStub.getBinder();
        } catch (Exception e) {
            return null;
        }
    }

    public boolean getRcsServiceStatus() {
        Log.i(logTAG, "getRcsServiceStatus  isRCSRegistered ["+ RCSListener.isRCSRegistered +"]");
        return RCSListener.isRCSRegistered;
    }

}
