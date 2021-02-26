/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

/**
 * Created by muluhe on 4/7/2017.
 */

public interface ControllerInterface {
    //
    //* purpose
    //*      get the fail module count
    //*
    //* requires
    //*       None
    //* guarantees
    //*       return Fail module number
    int getFailCount(String params);

    //* purpose
    //*      get the sequence module count
    //*
    //* requires
    //*       None
    //* guarantees
    //*       return sequence module number
    int getSequenceCount();

    //
    //* purpose
    //*      get MMI status
    //* requires
    //*       None
    //* guarantees
    //*       return 1: busy, 0:idle
    int getAppStatus(String params);

    //
    //* purpose
    //*      reconfig mmi using the specified config file
    //*
    //* requires
    //*       Input config file name
    //* guarantees
    //*       return module number
    int selectSequence(String cfgFileName);

    //
    //* purpose
    //*      clear the current test result for each module
    //*
    //* requires
    //*       None
    //* guarantees
    //*       clear result
    int clearResults(String params);

    //
    //* purpose
    //*      run the module in PCBA
    //*
    //* requires
    //*       module name
    //* guarantees
    //*
    int runSingleTest(String caseName);

    //
    //* purpose
    //*      run all module
    //*
    //* requires
    //*       None
    //* guarantees
    //*
    int runAllTests(String params);

    //
    //* purpose
    //*      stop the specified module
    //*
    //* requires
    //*       None
    //* guarantees
    //*
    int exitTest(String caseName);

    //
    //* purpose
    //*      stop the specified module
    //*
    //* requires
    //*       None
    //* guarantees
    //*
    int doUtility(String params);
    //
    //* purpose
    //*      save the module list to file
    //*
    //* requires
    //*       None
    //* guarantees
    //*
    int saveTestListToFile(String params);

    //
    //* purpose
    //*      save the module list to file
    //*
    //* requires
    //*       None
    //* guarantees
    //*
    int appendToResult(String params);

    //
    //* purpose
    //*      save the module list to file
    //*
    //* requires
    //*       None
    //* guarantees
    //*
    int getAppVersion(String params);

    //
    //* purpose
    //*      set token for testcase
    //*
    //* requires
    //*       None
    //* guarantees
    //*
    int setToken(String params);

}
