/*=============================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.seccamsample;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.res.Configuration;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.LinearLayout;
import android.graphics.Bitmap;
import android.view.View;
import android.view.Gravity;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.support.v4.content.ContextCompat;
import android.support.v4.app.ActivityCompat;

import com.qualcomm.qti.seccamsample.R;
import com.qualcomm.qti.seccamapi.SecCamServiceClient;

public class MainActivity extends Activity implements SecCamServiceClient.ClientCallback {

    private static boolean isServiceActive_ = false;
    private static final Integer isServiceActiveDelay_ = 3000; // Time to wait (in ms) for the serviceConnected callback to be called
    private static final int REQUEST_CAMERA_PERMISSION = 0; // Id to identify a camera permission request

    protected static final String LOG_TAG = "SECCAM-SAMPLE-APP";
    //private static final String TZ_APP_NAME = "seccamfa"; //enable for 2dfa
    private static final String TZ_APP_NAME = "seccamdemo64";
    private static final int TZ_APP_BUFFER_SIZE = (8*1024);

    private static boolean is_2dfa = false; //enable for 2dfa
    //=========================================================================
    private OnClickListener startPreviewButtonClickListener = new OnClickListener() {
        @Override
        public void onClick(View v) {
            startPreview();
        }
    };

    //=========================================================================
    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {

        switch (requestCode) {
            case REQUEST_CAMERA_PERMISSION: {

                // In case the user declines to give camera permissions to the app, a notification is displayed,
                // and the application closes.
                if ((grantResults.length == 0) || (grantResults[0] != PackageManager.PERMISSION_GRANTED)) {
                    AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                    builder.setTitle("");
                    builder.setMessage("This app does not have critical permissions needed to run. Please check your permissions settings");
                    builder.setPositiveButton("OK", null);
                    AlertDialog dialog = builder.show();

                    TextView messageView = (TextView)dialog.findViewById(android.R.id.message);
                    messageView.setGravity(Gravity.CENTER);

                    final Button okButton = dialog.getButton(AlertDialog.BUTTON_POSITIVE);
                    okButton.setOnClickListener(new View.OnClickListener() {
                        public void onClick(View v) {
                            finish();
                            System.exit(0);
                        }
                    });
                    LinearLayout.LayoutParams okButtonLL = (LinearLayout.LayoutParams) okButton.getLayoutParams();
                    okButtonLL.width = ViewGroup.LayoutParams.MATCH_PARENT;
                    okButton.setLayoutParams(okButtonLL);

                }
                break;
            }
            default:
                break;
        }
    }

    //=========================================================================
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.start_secure_preview).setOnClickListener(startPreviewButtonClickListener);
        findViewById(R.id.start_secure_preview).setEnabled(false);
        findViewById(R.id.done_button).setOnClickListener(mExitButtonClickListener);

        int permissionCheck = ContextCompat.checkSelfPermission(MainActivity.this,
        android.Manifest.permission.CAMERA);

        // Validate camera permissions are granted, otherwise request it during runtime
        if (permissionCheck != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(MainActivity.this,
                new String[]{android.Manifest.permission.CAMERA},
                REQUEST_CAMERA_PERMISSION);
        }

        // It is required to recieve the instance of the Service client.
        // This results in binding to the Secure camera service.
        SecCamServiceClient.getInstance().start(this, this, TZ_APP_NAME, TZ_APP_BUFFER_SIZE, is_2dfa);

        // Handler to be called after isServiceActiveDelay milliseconds, showing an indication that binding
        // to the Secure camera service was not successful. This may be caused if the TA is unavailable.
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            public void run() {
                if (isServiceActive_ == false) {
                    AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                    builder.setTitle("");
                    builder.setMessage("Secure camera service is inactive\n\n Validate the TA (" + TZ_APP_NAME + ")\n is available");
                    builder.setPositiveButton("OK", null);
                    AlertDialog dialog = builder.show();

                    TextView messageView = (TextView)dialog.findViewById(android.R.id.message);
                    messageView.setGravity(Gravity.CENTER);

                    final Button okButton = dialog.getButton(AlertDialog.BUTTON_POSITIVE);
                    LinearLayout.LayoutParams okButtonLL = (LinearLayout.LayoutParams) okButton.getLayoutParams();
                    okButtonLL.width = ViewGroup.LayoutParams.MATCH_PARENT;
                    okButton.setLayoutParams(okButtonLL);
                }
            }
        }, isServiceActiveDelay_);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Release the instance of the Service client when the activity is destroyed.
        // This results in unbinding from the Secure camera service.
        SecCamServiceClient.getInstance().release();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        View view =  this.getWindow().getDecorView();
    }

    //=========================================================================
    public int getPlatformRelease() {
        String releaseStr = Build.VERSION.RELEASE;
        int sdkLevel = Build.VERSION.SDK_INT;

        String[] releaseStrs = TextUtils.split(releaseStr, "\\.");

        int major = 0;
        int minor = 0;
        int revision = 0;
        try {
            major = (releaseStrs.length > 0 &&  !releaseStrs[0].isEmpty() ) ? Integer.parseInt(releaseStrs[0]) : 0;
            minor = (releaseStrs.length > 1 &&  !releaseStrs[1].isEmpty() ) ? Integer.parseInt(releaseStrs[1]) : 0;
            revision = (releaseStrs.length > 2 &&  !releaseStrs[2].isEmpty() ) ? Integer.parseInt(releaseStrs[2]) : 0;
        } catch (Exception ex) {
            Log.e(LOG_TAG, "MainActivity::getPlatformRelease - Android Version parse ERROR: " + ex);
        }
        int release = 10000 * major + 100 * minor + revision;

        return release;
    }

    //=========================================================================
    private void startPreview() {
        if (SecCamServiceClient.getInstance().getMinSupportedHAL() == SecCamServiceClient.HAL1) {
            //Legacy Camera API to be used if secure camera2 based API is not supported
            startActivityForResult(new Intent(MainActivity.this, CameraAPIActivity.class), 1);
        }
        else {
            //Camera2 API
            startActivityForResult(new Intent(MainActivity.this, Camera2APIActivity.class), 1);
        }
    }

    //=========================================================================
    private OnClickListener mExitButtonClickListener = new OnClickListener() {
        @Override
        public void onClick(View v) {
            finish();
            System.exit(0);
        }
    };

    //=========================================================================
    @Override
    public void serviceConnected() {
        // Indicates that the service is available and connected.
        // Secure Camera can be activated after this callback occurs.
        findViewById(R.id.start_secure_preview).setEnabled(true);
        isServiceActive_ = true;
    }

    @Override
    public void serviceDisconnected() {
        // This callback is called if there is some issue connecting to the service. once it is triggered
        // the application must wait for the serviceConnected() callback to be called, before starting Secure Camera.
        findViewById(R.id.start_secure_preview).setEnabled(false);
        isServiceActive_ = false;
    }
}
