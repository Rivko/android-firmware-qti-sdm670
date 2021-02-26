/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.orbbec;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.support.v7.app.AlertDialog;
import android.util.Log;

import com.qualcomm.qti.threedr.utils.Constants;

import org.openni.Device;
import org.openni.DeviceInfo;
import org.openni.OpenNI;
import org.openni.android.OpenNIHelper;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.List;

import static com.qualcomm.qti.threedr.OrbecNative.getOBCmosParams;
import static com.qualcomm.qti.threedr.OrbecNative.getSerialNumberFromJNI;

public class OBNativeHelper {
    private static final String TAG = OBNativeHelper.class.getCanonicalName();

    private static OBNativeHelper instance = null;

    private OpenNIHelper mOpenNIHelper;
    private Context mContext;
    private Device mDevice = null;
    public boolean deviceOpenFailed = false;

    public static OBNativeHelper getInstance(Context context) {
        if (instance == null) {
            instance = new OBNativeHelper(context);
        }
        return instance;
    }

    private OBNativeHelper(Context context) {
        mContext = context;
    }

    public void openOBSensor() {
        if (mOpenNIHelper != null) {
            closeOBSensor();
        }
        try {
            mOpenNIHelper = new OpenNIHelper(mContext);
        } catch (IOException e) {
            e.printStackTrace();
        }
        OpenNI.setLogAndroidOutput(true);
        OpenNI.setLogMinSeverity(0);
        OpenNI.initialize();
        initOrbbec();
    }

    public void closeOBSensor() {
        if (mDevice != null) {
            mDevice.close();
            mDevice = null;
        }

        if (mOpenNIHelper != null) {
            mOpenNIHelper.shutdown();
            OpenNI.shutdown();
            mOpenNIHelper = null;
        }
    }

    private void initOrbbec() {
        // Request opening the first OpenNI-compliant device found
        String uri;

        List<DeviceInfo> devices = OpenNI.enumerateDevices();
        if (devices.isEmpty()) {
            Log.d(TAG, "No OpenNI-compliant device found.");
            showAlertAndExit("No OpenNI-compliant device found.");
        }

        uri = devices.get(devices.size() - 1).getUri();

        mOpenNIHelper.requestDeviceOpen(uri, new OpenNIHelper.DeviceOpenListener() {
            @Override
            public void onDeviceOpened(Device device) {
                mDevice = device;
            }

            @Override
            public void onDeviceOpenFailed(String uri) {
                deviceOpenFailed = true;
                Log.i(TAG, "Device open Failed for uri=" + uri);
            }
        });
    }

    public void readOBCmosParams() {
        while (mDevice == null && !deviceOpenFailed) {
            try {
                Thread.sleep(1000);
                Log.i(TAG, "waiting for device open");
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        if (deviceOpenFailed) {
            return;
        }
        String serialNo = getSerialNumberFromJNI();
        Log.i(TAG, " read serial no in java as " + serialNo);
        if (isNewSensor(serialNo)) {
            OBCameraParams obCameraParams = getOBCmosParams();
            updateCalibrationLR_HR_files(obCameraParams);
            updateSerialnoFile(serialNo);
        }
    }

    /**
     * creates new if not exist or will overwrite the existing one with the new serial no
     *
     * @param serialNo
     */
    private void updateSerialnoFile(String serialNo) {
        File file = new File(Constants.CALIBRATION_FOLDER + "/orbbec_serial_no.txt");
        try {
            file.createNewFile();
            FileOutputStream fos = new FileOutputStream(file, false);
            fos.write(serialNo.getBytes());
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * this function will return true if connected sensor is new one, i.e if the device doesnt have the serial_no.txt file
     * or if the file exist but the serial no in the file doesnt match the serial no passed as argument we will return true.
     * if the file exist and serial no matches that means its a old sensor hence we already has its info
     *
     * @param serialNo
     * @return true if new device, false otherwise
     */
    private boolean isNewSensor(String serialNo) {
        boolean retVal = true;
        File file = new File(Constants.CALIBRATION_FOLDER + "/orbbec_serial_no.txt");
        if (file.exists()) {
            BufferedReader reader = null;
            String line = null;
            try {
                reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
                line = reader.readLine();
                reader.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            if (line != null && line.equals(serialNo)) {
                retVal = false;
            }
        }
        return retVal;
    }

    /**
     * creates new files if not exist or will overwrite on existing ones the LR and HR values
     *
     * @param obCameraParams
     */
    private void updateCalibrationLR_HR_files(OBCameraParams obCameraParams) {
        File QD_cam_calibration_LR = new File(Constants.CALIBRATION_FOLDER + "/QD_cam_calibration_LR.txt");
        File QD_cam_calibration_HR = new File(Constants.CALIBRATION_FOLDER + "/QD_cam_calibration_HR.txt");

         if (!QD_cam_calibration_LR.getParentFile().exists()) {
             QD_cam_calibration_LR.getParentFile().mkdirs();
         }        

        try {
            //create if doesnt exist
            QD_cam_calibration_LR.createNewFile();
            QD_cam_calibration_HR.createNewFile();

            //write LR file
            FileOutputStream fos = new FileOutputStream(QD_cam_calibration_LR, false);
            BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(fos));
            /**
             *For LR
             * Line 1 = RGB camera intrinsic i.e r_intr_p[0] r_intr_p[1] r_intr_p[2] r_intr_p[3]
             * Line 2 = IR Intrinsic         i.e l_intr_p[0] l_intr_p[1] l_intr_p[2] l_intr_p[3]
             * Line 3 = Rotate Matrix organized row wise i.e r2l_r[0] r2l_r[1] r2l_r[2] r2l_r[3] r2l_r[4] r2l_r[5] r2l_r[6] r2l_r[7] r2l_r[8]
             * Line 4 = Translate Matrix in meters (div by 1000) i.e r2l_t[0]/1000 r2l_t[1]/1000 r2l_t[2]/1000
             */
            bw.write(obCameraParams.r_intr_p[0] + " " + obCameraParams.r_intr_p[1] + " " + obCameraParams.r_intr_p[2] + " " + obCameraParams.r_intr_p[3]);
            bw.newLine();
            bw.write(obCameraParams.l_intr_p[0] + " " + obCameraParams.l_intr_p[1] + " " + obCameraParams.l_intr_p[2] + " " + obCameraParams.l_intr_p[3]);
            bw.newLine();
            bw.write(obCameraParams.r2l_r[0] + " " + obCameraParams.r2l_r[1] + " " + obCameraParams.r2l_r[2]
                    + " " + obCameraParams.r2l_r[3] + " " + obCameraParams.r2l_r[4] + " " + obCameraParams.r2l_r[5]
                    + " " + obCameraParams.r2l_r[6] + " " + obCameraParams.r2l_r[7] + " " + obCameraParams.r2l_r[8]);
            bw.newLine();
            bw.write(obCameraParams.r2l_t[0] / (float) 1000 + " " + obCameraParams.r2l_t[1] / (float) 1000 + " " + obCameraParams.r2l_t[2] / (float) 1000);
            bw.close();


            //write LR file
            fos = new FileOutputStream(QD_cam_calibration_HR, false);
            bw = new BufferedWriter(new OutputStreamWriter(fos));
            /**
             * For HR
             * Line 1 = RGB Camera intrensic i.e r_intr_p[0] r_intr_p[1] r_intr_p[2] r_intr_p[3]
             * Line 2 & 3 = always like this as we are not using any other camera.
             * Line 2 = 1 0 0 0 1 0 0 0 1
             * Line 3 = 0 0 0
             */
            bw.write(obCameraParams.r_intr_p[0] + " " + obCameraParams.r_intr_p[1] + " " + obCameraParams.r_intr_p[2] + " " + obCameraParams.r_intr_p[3]);
            bw.newLine();
            bw.write("1 0 0 0 1 0 0 0 1");
            bw.newLine();
            bw.write("0 0 0");
            bw.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void showAlertAndExit(String message) {
        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setMessage(message);
        builder.setNeutralButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                ((Activity) mContext).finish();
            }
        });
        builder.show();
    }
}
