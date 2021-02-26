/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

import com.qualcomm.qti.qmmi.bean.TestCase;

public interface FunctionCallback {

    /**
     * run the method defined
     */
    int runFunction(TestCase testCase);

}
