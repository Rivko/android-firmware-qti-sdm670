/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qmmi.utils;

import java.text.DecimalFormat;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class NumericUtils {

    public static boolean isMatch(String regex, String orginal){
        if (orginal == null || orginal.trim().equals("")) {
            return false;
        }
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(orginal);
        return matcher.matches();
    }

    /**
     * Check if number is more than zero
     * @param orginal
     * @return
     */
    public static boolean isPositiveInteger(String orginal) {
        return isMatch("^[1-9]\\d*$", orginal);
    }

    public static String formatDecimals(float value, int retainNums){
        StringBuffer params = new StringBuffer();
        params.append("0.");

        for( int i = 0; i < retainNums; i++){
            params.append("0");
        }

        DecimalFormat df = new DecimalFormat(params.toString());
        return df.format(value);
    }

}
