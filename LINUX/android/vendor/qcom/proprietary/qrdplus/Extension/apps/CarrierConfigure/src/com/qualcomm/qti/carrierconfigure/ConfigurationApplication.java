/**
 * Copyright (c) 2015-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import static com.android.internal.telephony.TelephonyIntents.SECRET_CODE_ACTION;

import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.IPackageInstallObserver;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Environment;
import android.os.RemoteException;
import android.text.TextUtils;
import android.util.Log;

import com.android.internal.os.RegionalizationEnvironment;
import com.android.internal.os.IRegionalizationService;
import com.qualcomm.qti.carrierconfigure.StartActivityReceiver;
import com.qualcomm.qti.carrierconfigure.SetTargetSlotService;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class ConfigurationApplication extends Application {
    private static final String TAG = "ConfigurationApplication";

    private static final String INTENTFILTER_SCHEME = "android_secret_code";
    private IRegionalizationService mRegionalizationService = null;

    private static Context mContext;
    private static ConfigurationApplication mConfigureApp = null;

    private void installCarrierApks(File file) {
        if (file.isDirectory()) {
            String[] children = file.list();
            if (children == null) return;

            for (int i = 0; i < children.length; i++) {
                installCarrierApks(new File(file, children[i]));
            }
        } else {
            if (file.isFile() && file.getName().endsWith(".apk")) {
                installOnBackground(file);
            }
        }
    }

    private void installOnBackground(File file) {
        int installFlags = 0;
        Uri packageUri = Uri.fromFile(file);

        PackageManager pm = mContext.getPackageManager();
        installFlags |= PackageManager.INSTALL_REPLACE_EXISTING;
        MyPackageInstallObserver observer = new MyPackageInstallObserver();
        pm.installPackage(packageUri, observer, installFlags, "com.qualcomm.qti.carrierconfigure");
    }

    private static class MyPackageInstallObserver extends IPackageInstallObserver.Stub {
        @Override
        public void packageInstalled(String packageName, int returnCode)
                throws RemoteException {
            Log.i(TAG, "packageName = " + packageName);
            Log.i(TAG, "returnCode = " + returnCode);
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mContext = this;

        StartActivityReceiver receiver = new StartActivityReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction(Utils.ACTION_TRIGGER);
        filter.setPriority(1000);
        registerReceiver(receiver, filter);

        filter = new IntentFilter();
        filter.addAction(SECRET_CODE_ACTION);
        filter.addDataScheme(INTENTFILTER_SCHEME);
        filter.addDataAuthority(StartActivityReceiver.CODE_SPEC_SWITCH, null);
        filter.addDataAuthority(StartActivityReceiver.CODE_SPEC_SWITCH_L, null);
        filter.setPriority(1000);
        registerReceiver(receiver, filter);

        filter = new IntentFilter();
        filter.addAction(Utils.ACTION_CARRIER_RESET);
        filter.setPriority(1000);
        registerReceiver(receiver, filter);

        filter = new IntentFilter();
        filter.addAction(Intent.ACTION_MASTER_CLEAR_NOTIFICATION);
        filter.setPriority(1000);
        registerReceiver(receiver, filter);

        Log.i(TAG, "register StartActivityReceiver.");

        if (TextUtils.isEmpty(Utils.getSharedPreference(this,
                Utils.SHARED_PREFERENCE_KEY_RESTORE_3RD_APKS))) {
            // Install 3rd apks in a new thread
            new Thread() {
                public void run() {
                    Log.d(TAG, "Install 3rd apks for Carrier ...");
                    ArrayList<String> carriers = Carrier.getCurrentCarriers();
                    for (String carrier : carriers) {
                        // Carrier's apk will preset to /data/app-regional/Carrier, if not,
                        // maybe a factory reset just happened.
                        File dataDir = Environment.getDataDirectory();
                        File vendorDir = Environment.getVendorDirectory();
                        if (RegionalizationEnvironment.isSupported()) {
                            vendorDir = new File(RegionalizationEnvironment.getStoragePos());
                        }
                        if (!(new File(dataDir, "app-regional/" + carrier).exists())) {
                            installCarrierApks(
                                    new File(vendorDir.getAbsolutePath(), carrier + "/data/app"));
                        }
                    }
                    Utils.setSharedPreference(mContext,
                            Utils.SHARED_PREFERENCE_KEY_RESTORE_3RD_APKS, "true");
                }
            }.start();
        }

        if (TextUtils.isEmpty(Utils.getSharedPreference(this,
                Utils.SHARED_PREFERENCE_KEY_IS_FIRST_BOOT))) {
            if (Utils.DEBUG) Log.i(TAG, "Start SetTargetSlotService.");
            startService(new Intent(this, SetTargetSlotService.class));
        }
    }

    public static ConfigurationApplication getInstance() {
        if(mConfigureApp == null){
            mConfigureApp = new ConfigurationApplication();
        }
        return mConfigureApp; 
    }

	/**
		* Save these carriers as not triggered carriers, and these carriers will never be triggered
		* until the user reset the phone.
		*/
    public void saveAsNotTriggeredCarrier(ArrayList<Carrier> carrierList) {
        if (carrierList == null || carrierList.size() < 1) return;
            for (Carrier carrier : carrierList) {
                if (carrier.mName.endsWith("2Default")) continue;
                saveAsNotTriggeredCarrier(carrier);
            }
    }

	 /**
     * Save this carrier as not triggered carrier, and this carrier will never be triggered
     * until the user reset the phone.
     */
    public void saveAsNotTriggeredCarrier(Carrier carrier) {
        if (carrier == null) return;

        if (RegionalizationEnvironment.isSupported()) {
            mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
        }
        if(mRegionalizationService != null){
            try{				
                mRegionalizationService.writeFile(Utils.NOT_TRIGGERED, carrier.mName + "\n", true);
                Log.d(TAG,".........write over ........"+Utils.NOT_TRIGGERED);
            }catch (Exception e) {
                Log.e(TAG, "IOException"+ e.getMessage());
            }		
        }
    }

    /**
     * Return the not triggered carriers' name.
     */
    public ArrayList<String> getNotTriggeredCarriersName() {
        ArrayList<String> list = new ArrayList<String>();

        if (RegionalizationEnvironment.isSupported()) {
            mRegionalizationService = RegionalizationEnvironment.getRegionalizationService();
        }
        if(mRegionalizationService != null){
            try{
                List<String> mList = mRegionalizationService.readFile(Utils.NOT_TRIGGERED, null);
                if(mList == null || mList.size() == 0 ) return null;
                for(int i = 0; i < mList.size(); i++)
                {
                    list.add(mList.get(i));
                    Log.d("WYM","....iii.....="+list.get(i));
                }
                Log.d("WYM","!!!!!!!!!read over ........NotTriggered");
            }catch (Exception e) {
                Log.e(TAG, "IOException"+ e.getMessage());
            }
        }

        return list;
    }
}
