/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Battery;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.model.FunctionCallback;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class BatteryService extends BaseService {

    @Override
    public void register() {

        FunctionCallback callback1 = new FunctionCallback() {
            @Override
            public int runFunction(TestCase testCase) {
                LogUtils.logi( "callback1 start");
                return 0;
            }
        };

        FunctionCallback callback2 = new FunctionCallback() {
            @Override
            public int runFunction(TestCase testCase) {
                LogUtils.logi( "callback2 start");
                return 0;
            }
        };

        registerFunction("callback1", callback1);
        registerFunction("callback2", callback2);
    }

    @Override
    public int stop(TestCase testCase) {
        return 0;
    }

    @Override
    public int run(TestCase testCase) {
        String iccid = null;
        LogUtils.logi( "Battery service run");
        return 0;
    }

}
