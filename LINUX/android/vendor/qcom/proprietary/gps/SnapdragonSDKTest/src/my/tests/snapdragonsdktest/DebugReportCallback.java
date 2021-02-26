/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import com.qti.location.sdk.IZatDebugReportingService;
import com.qti.debugreport.*;

import android.util.Log;
import java.util.List;
import java.util.ArrayList;
import java.util.Date;
import java.text.SimpleDateFormat;
import java.io.File;
import java.io.PrintWriter;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import android.os.Bundle;
import android.os.Environment;

public class DebugReportCallback implements IZatDebugReportingService.IZatDebugReportCallback {
    private static String TAG = "DebugReportCallbackInApp";

    private boolean mExternalStorageAvailable = false;
    private boolean mExternalStorageWritable = false;
    File mFileReport;
    FileOutputStream mFileOutputStream;
    PrintWriter mFileWriter;
    private static DebugReportActivity mDebugReportActivityObj;
    private int mReportNum;

    public DebugReportCallback(DebugReportActivity objDebugReportActivity) {
        mDebugReportActivityObj = objDebugReportActivity;
        mReportNum = 0;

        checkExternalMedia();

        if (mExternalStorageWritable) {
            File root = Environment.getExternalStorageDirectory();
            File systemReportDir = new File(root.getAbsolutePath() + "/SystemStatusReport");
            Log.d(TAG, "root dir:" + systemReportDir);
            systemReportDir.mkdir();
            mFileReport = new File(systemReportDir, "statusReport.txt");
            try {
                mFileOutputStream = new FileOutputStream(mFileReport);
            } catch (FileNotFoundException e) {
                Log.e(TAG, "File not found");
                e.printStackTrace();
            }
            mFileWriter = new PrintWriter(mFileOutputStream, true);
        }
    }

    public void onDebugReportAvailable(Bundle debugReport) {
        Log.v(TAG, "Got debug report in app");
        SimpleDateFormat s = new SimpleDateFormat("hh:mm:ss.SSS");
        String sDate = s.format(new Date());
        String sReportNum = "---- Debug Report :" + mReportNum;

        mFileWriter.println();
        mFileWriter.println(sDate + ":" + sReportNum);
        mReportNum ++;

        mDebugReportActivityObj.printReport(debugReport, mFileWriter);
    }

    public void checkExternalMedia() {
         // check if external media is available
        String state = Environment.getExternalStorageState();

        if (Environment.MEDIA_MOUNTED.equals(state)) {
            // Can read and write the media
            mExternalStorageAvailable = mExternalStorageWritable = true;
        } else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
            // Can only read the media
            mExternalStorageAvailable = true;
            mExternalStorageWritable = false;
        } else {
            // Can't read or write
            mExternalStorageAvailable = mExternalStorageWritable = false;
        }
        Log.d(TAG, "\n\nExternal Media: readable="
                +mExternalStorageAvailable+" writable="+mExternalStorageWritable);
    }

    public void saveReport() {
        mFileWriter.flush();
        mFileWriter.close();

        try {
            mFileOutputStream.close();
        } catch (IOException e) {
            Log.e(TAG, "IOException when closing FileOutput stream");
            e.printStackTrace();
        }

        mFileWriter = null;
        mFileOutputStream = null;
        mFileReport = null;
    }
}
