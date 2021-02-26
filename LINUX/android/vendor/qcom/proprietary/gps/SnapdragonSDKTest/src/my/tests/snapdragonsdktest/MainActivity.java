/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import android.app.Activity;
import android.app.TabActivity;

import android.os.Bundle;
import android.os.Build;

import android.util.Log;
import java.util.ArrayList;
import android.content.Intent;
import android.Manifest;
import android.content.pm.PackageManager;
import android.widget.TabHost;
import android.widget.TabHost.TabSpec;

public class MainActivity extends TabActivity {
    private static String TAG = "SnapdragonSDKTest";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private boolean mIsInitialized = false;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        boolean permissionGranted = false;
        Log.d(TAG, "ininit" + mIsInitialized);

        if (Build.VERSION.SDK_INT >= 23) {
            // if SDK version > 23, check for required runtime permission.
            String[] operationPermissionNames = getOperationPermissionName();

            if (operationPermissionNames == null ||
                        checkOperationPermission(operationPermissionNames)) {
                permissionGranted = true;
            }
            else {
                // if permission not granted, don't continue
                permissionGranted = false;
            }
        }
        else {
            permissionGranted = true;
        }

        if (permissionGranted) {
            doInit();
        }
    }

    private void doInit() {
        if (mIsInitialized == true) {
            return;
        }

        TabHost tabHost=getTabHost();
        // no need to call TabHost.Setup()

        //First Tab
        TabSpec spec1=tabHost.newTabSpec("DebugReport");
        spec1.setIndicator("DebugReport");
        Intent in1=new Intent(this, DebugReportActivity.class);
        spec1.setContent(in1);

        TabSpec spec2=tabHost.newTabSpec("Geofence");
        spec2.setIndicator("Geofence");
        Intent in2=new Intent(this, FullscreenActivity.class);
        spec2.setContent(in2);

        TabSpec spec3=tabHost.newTabSpec("FLP");
        spec3.setIndicator("FLP");
        Intent in3=new Intent(this, FlpActivity.class);
        spec3.setContent(in3);

        TabSpec spec4=tabHost.newTabSpec("XTRA");
        spec4.setIndicator("XTRA");
        Intent in4=new Intent(this, XtraThrottleActivity.class);
        spec4.setContent(in4);

        tabHost.addTab(spec1);
        tabHost.addTab(spec2);
        tabHost.addTab(spec3);
        tabHost.addTab(spec4);

        mIsInitialized = true;
    }

    private boolean checkOperationPermission(String[] permissionName) {
        ArrayList<String> needRequestPermission = new ArrayList<String>();
        for (String tmp : permissionName) {
            if (!(PackageManager.PERMISSION_GRANTED == checkSelfPermission(tmp))) {
                needRequestPermission.add(tmp);
            }
        }

        if (needRequestPermission.size() == 0) {
            return true;
        } else {
            String[] needRequestPermissionArray = new String[needRequestPermission.size()];
            needRequestPermission.toArray(needRequestPermissionArray);
            requestPermissions(needRequestPermissionArray, 0);
            return false;
        }
    }

    private String[] getOperationPermissionName() {
        return new String[]{ Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.WRITE_EXTERNAL_STORAGE};
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (permissions == null || grantResults == null ||
            permissions.length == 0 || grantResults.length == 0) {
            return;
        }

        boolean bPass = true;
        for (int i = 0 ; i < permissions.length ; i++) {
            Log.d(TAG, "PermissionsResult: " + permissions[i] + ":" + grantResults[i]);
            if (grantResults[i] != PackageManager.PERMISSION_GRANTED) {
                bPass = false;
            }
        }
        if(!bPass) {
            finish();
        } else {
            doInit();
        }
    }
}
