/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.utils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class ShellUtils {

    public static class CmdResult {
        /**
         * result of cmd
         **/
        public int result;
        /**
         * success message of cmd result
         **/
        public String successMsg;
        /**
         * error message of cmd result
         **/
        public String errorMsg;

        public CmdResult(int result) {
            this.result = result;
        }

        public CmdResult(int result, String successMsg, String errorMsg) {
            this.result = result;
            this.successMsg = successMsg;
            this.errorMsg = errorMsg;
        }
    }

    /**
     * execute the command
     */
    public static CmdResult exec(String cmd) {
        int result = -1;
        Process proc = null;
        BufferedReader successResult = null;
        BufferedReader errorResult = null;
        StringBuilder successMsg = null;
        StringBuilder errorMsg = null;

        Runtime runtime = Runtime.getRuntime();
        LogUtils.logi("exec command: " + cmd);
        try {
            proc = runtime.exec(cmd);
            result = proc.waitFor();
            successMsg = new StringBuilder();
            errorMsg = new StringBuilder();
            successResult = new BufferedReader(new InputStreamReader(proc.getInputStream()));
            errorResult = new BufferedReader(new InputStreamReader(proc.getErrorStream()));
            String s;
            while ((s = successResult.readLine()) != null) {
                LogUtils.logi("exec command: " + s);
                successMsg.append(s);
            }
            while ((s = errorResult.readLine()) != null) {
                LogUtils.logi("exec command: " + s);
                errorMsg.append(s);
            }
        } catch (InterruptedException e) {
            LogUtils.loge(e);
        } catch (IOException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (successResult != null) {
                    successResult.close();
                }
                if (errorResult != null) {
                    errorResult.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            if (proc != null) {
                proc.destroy();
            }
        }
        return new CmdResult(result, successMsg == null ? null : successMsg.toString(), errorMsg == null ? null
                : errorMsg.toString());
    }
}
