/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.calendarwidget;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.provider.Settings;
import android.view.Window;
import android.widget.Toast;

import java.util.ArrayList;

public class PermissionActivity extends Activity {

    public static String PERMISSIONS[] = {Manifest.permission.READ_CALENDAR};
    private static final int PERMISSION_REQUEST_CODE = 1;
    private static final String PREFNAME = "config";
    private static final String NEEDREQUESTTAG = "needRequest";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
    }

    @Override
    protected void onStart() {
        super.onStart();
       if (checkPermissionsForRequest(this, PERMISSIONS, PERMISSION_REQUEST_CODE)) {
           finish();
       }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions,
            int[] grantResults) {
        boolean result = checkPermissionResult(permissions,grantResults);
        if (!result) {
            if (!shouldShowRequestPermissionRationale(
                    Manifest.permission.READ_CALENDAR)) {
                if (loadNeedRequest(this)) {
                    Toast.makeText(this, R.string.openPermissionTips, Toast.LENGTH_LONG).show();
                    openSettings();
                }
                saveNeedRequest(this, true);
            }
        }
        finish();
    }

    private boolean checkPermissionsForRequest(Activity activity, String[] permissions,
            int operationHandle) {
        if (permissions == null)
            return true;
        boolean isPermissionGranted = true;
        ArrayList<String> permissionList = new ArrayList<>();
        for (String permission : permissions) {
            if (PackageManager.PERMISSION_GRANTED != activity.checkSelfPermission(permission)) {
                permissionList.add(permission);
                isPermissionGranted = false;
            }
        }
        if (!isPermissionGranted) {
            String[] permissionArray = new String[permissionList.size()];
            permissionList.toArray(permissionArray);
            activity.requestPermissions(permissionArray, operationHandle);
        }
        return isPermissionGranted;
    }

    @Override
    protected void onStop() {
        Intent intent = new Intent();
        intent.setAction(Utility.UPDATE_PERMISSION);
        sendBroadcast(intent);
        super.onStop();
    }

    private void saveNeedRequest(Context context, boolean needRequest){
        SharedPreferences sp = context.getSharedPreferences(PREFNAME, MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putBoolean(NEEDREQUESTTAG, needRequest);
        editor.apply();
    }

    private boolean loadNeedRequest(Context context) {
        SharedPreferences sp = context.getSharedPreferences(PREFNAME, MODE_PRIVATE);
        return sp.getBoolean(NEEDREQUESTTAG,false);
    }

    private void openSettings(){
        Uri packageURI = Uri.parse("package:com.qualcomm.qti.calendarwidget");
        Intent intent =  new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS,packageURI);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(intent);
    }

    private static boolean checkPermissionResult(String[] permissions, int[] grantResults) {
        if (permissions == null || grantResults == null || permissions.length == 0
        || grantResults.length == 0) {
            return false;
        }
        for (int result : grantResults) {
            if (result != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

}