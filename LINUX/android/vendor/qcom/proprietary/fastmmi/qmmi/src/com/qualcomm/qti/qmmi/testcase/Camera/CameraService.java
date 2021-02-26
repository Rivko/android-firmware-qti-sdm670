/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Camera;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;

public class CameraService extends BaseService {
    public static String[] supported_methods = {"preview", "snapshot", "face_detect"};

    public static boolean isMethodSupported(String method) {
        for (String m : supported_methods) {
            if (method.equalsIgnoreCase(m))
                return true;
        }
        return false;
    }

    @Override
    public void register() {
    }

    @Override
    public int run(TestCase testCase) {
        return 0;
    }

    @Override
    public int stop(TestCase testCase) {
        return 0;
    }
}
