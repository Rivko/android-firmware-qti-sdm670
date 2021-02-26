/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.TouchPanel;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class TouchPanelService extends BaseService {

    @Override
    public void register() {

    }

    @Override
    public int run(TestCase testCase) {
        String iccid = null;
//        if ("dsds"
//                .equals(Utils.getSystemProperties(Values.PROP_MULTISIM, null))) {
//
//            int[] subId = SubscriptionManager.getSubId(SLOT_ID);
//            iccid = TelephonyManager.getDefault().getSimSerialNumber(subId[0]);
//        } else {
//            iccid = TelephonyManager.getDefault().getSimSerialNumber();
//        }
//
//        if (iccid != null && !iccid.equals("")) {
//            result = true;
//            toastString = "iccid: " + iccid;
//        }
        LogUtils.logi("simservice run");
        return 0;
    }

    @Override
    public int stop(TestCase testCase) {
        return 0;
    }

}
