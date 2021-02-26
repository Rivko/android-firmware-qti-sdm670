/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.LinkedHashMap;
import java.io.PrintWriter;
import java.lang.StringBuilder;

import android.app.Activity;
import android.app.PendingIntent;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.text.TextUtils;

import android.widget.Button;
import android.widget.TextView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.AdapterView;
import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.view.View;
import android.view.LayoutInflater;
import android.graphics.Color;
import android.location.Location;

import com.qti.location.sdk.IZatManager;
import com.qti.location.sdk.IZatTestService;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.BadParcelableException;


public class XtraThrottleActivity extends Activity {

    private static String TAG = "XtraThrottleActivity";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private IZatManager mIzatMgr;
    private IZatTestService mTestService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_xtrathrottle);
        doInit();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mIzatMgr.disconnectTestService(mTestService);
    }

    private void doInit() {
        mIzatMgr = IZatManager.getInstance(getApplicationContext());
        mTestService = mIzatMgr.connectTestService();

        String version = mIzatMgr.getVersion();
        if (VERBOSE) {
            Log.v(TAG, "SDK and Service Version:" + version);
        }

        final Button enableButton = (Button) findViewById(R.id.enableBtn);
        final Button disableButton = (Button) findViewById(R.id.disableBtn);

        enableButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (VERBOSE) {
                    Log.v(TAG, "enable XTRA throttle");
                }

                if (mTestService != null) {
                    mTestService.updateXtraThrottle(true);
                }
            }
        });

        disableButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (VERBOSE) {
                    Log.v(TAG, "disable XTRA throttle");
                }

                if (mTestService != null) {
                    mTestService.updateXtraThrottle(false);
                }
            }
        });
    }
}
