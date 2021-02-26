/******************************************************************************
 * ---------------------------------------------------------------------------
 *  Copyright (C) 2014 Qualcomm Technologies, Inc.
 *  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
 *  ---------------------------------------------------------------------------
 *******************************************************************************/
package com.qti.ultrasound.penpairing;

import android.util.Log;
import java.io.FileInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import android.os.SystemService;
import android.os.SystemProperties;

public class DaemonWrapper {

    private boolean mIsDaemonRunning = false;

    private static final String mPidDirectory = "/data/usf/";
    private static final String mPidFileExtention = ".pid";
    private String mDaemonName;
    private static final String mExePath = "/system/bin/";

    public DaemonWrapper(String daemonName) {
        mDaemonName = daemonName;
    }

    /**
     * This function starts the daemon. If the service is currently
     * stopping (can happen with very quick stop/start), wait until
     * it is fully stopped before starting it.
     * <b>This function has a retry mechansim with a timeout</b>
     */
    public boolean start() {
        int numTries = 5;
        String state = null;

        while (numTries-- > 0) {
            state = SystemProperties.get("init.svc." + mDaemonName, "stopped");
            if(state.equals("stopped")) {
                SystemService.start(mDaemonName);
                mIsDaemonRunning = true;
                return true;
            }

            // Service is not fully stopped, wait and retry
            Log.w(this.toString(),
                  "Service not fully stopped (state: " + state + " ), wait and retry");
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                break;
            }
        }
        Log.e(this.toString(),
              "Failed to start " +  mDaemonName + ", service is in state " + state);
        return false;
    }

    /**
     * getPid() function reads the daemon's pid file and returns its
     * pid.
     * @return int the current daemon's pid
     *             -1 in case of an error
     *             -2 in case non-integer is read from the pid file
     */
    int getPid() {
        String str = "";
        StringBuffer buf = new StringBuffer();
        int retPid;
        BufferedReader reader = null;
        try {
            // Try to read pid file
            // This file should include one integer, which is the daemon's pid
            FileInputStream fStream = new FileInputStream(mPidDirectory +
                                                          mDaemonName +
                                                          mPidFileExtention);
            reader = new BufferedReader(new InputStreamReader(fStream));
            while (null != (str = reader.readLine())) {
                buf.append(str);
            }
        } catch (IOException e) {
            return -1;
        } finally {
            if (null != reader) {
                try {
                    reader.close();
                } catch (IOException e) {
                    return -1;
                }
            }
        }

        try {
            retPid = Integer.parseInt(buf.toString());
        } catch (NumberFormatException e) {
            Log.e(this.toString(), "Daemon pid file does not contain an integer");
            return -2;
        }
        return retPid;
    }

    /**
     * This function waits for the daemon to exit.
     */
    private void waitForDaemonToExit() {
        while(SystemProperties.get("init.svc." + mDaemonName, "stopped").equals("running")) {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                return;
            }
        }
    }
    /**
     * This function stops the daemon by setting a property. When
     * this property is set, the daemon stops and exits.
     */
    private void stopDaemon() {
        if (SystemProperties.get("init.svc." + mDaemonName, "stopped").equals("running")) {
            SystemProperties.set("sys.usf." + mDaemonName + ".stop", "1");
            waitForDaemonToExit();
        }
    }

    public void stop() {
        stopDaemon();
        mIsDaemonRunning = false;
    }

    public boolean isStarted() {
        return mIsDaemonRunning;
    }

}
