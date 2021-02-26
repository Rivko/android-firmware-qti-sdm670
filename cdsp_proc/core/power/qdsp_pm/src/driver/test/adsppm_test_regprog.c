/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "mmpm.h"
#include "adsppm_test.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"


#define REGPROG_01_NAME    "regprog_01"
#define REGPROG_01_DETAILS "Req-Rel, all core/instance/reg speed combination"

static MMPM_STATUS TestAllCoresRegProgRequest(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    MmpmRegProgMatchType regProgMatch;
    uint32 clientId;

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, REGPROG_01_NAME);
                if(clientId)
                {
                    for(regProgMatch = MMPM_REG_PROG_NORM; regProgMatch < MMPM_REG_PROG_MAX; regProgMatch++)
                    {
                        sts = Test_RequestRegProg(clientId, regProgMatch);
                        if(MMPM_STATUS_SUCCESS != sts)
                        {
                            retSts = MMPM_STATUS_FAILED;
                        }
                        else
                        {
                            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_REG_PROG);
                            if(MMPM_STATUS_SUCCESS != sts)
                            {
                                retSts = MMPM_STATUS_FAILED;
                            }
                        }
                    }
                    sts = Test_Deregister(clientId);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
                    }
                }
                else
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }
    }

    return retSts;
}

/**
 * @ingroup RegProg
 * @test regprog_01
 * @brief Req-Rel, all core/instance/reg speed combination
 */
static MMPM_STATUS Test_RegProg_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(REGPROG_01_NAME, REGPROG_01_DETAILS);

    sts = TestAllCoresRegProgRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_RegProg, REGPROG_01_NAME, sts);
    return sts;
}


#define REGPROG_02_NAME    "regprog_02"
#define REGPROG_02_DETAILS "Req-Rel, 2 same coreID/instanceID clients, expect Rel to turn OFF AHB branch"

static AdsppmTestType testregprog02[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {REG, REQREGP}
    },
    {2, 2, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {REG, REQREGP}
    },
    {1, 2, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {RELREGP, DREG}
    },
    {2, 2, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {RELREGP, DREG}
    }
};

/**
 * @ingroup RegProg
 * @test regprog_02
 * @brief Req-Rel, 2 same coreID/instanceID clients, expect Rel to turn OFF AHB branch
 */
static MMPM_STATUS Test_RegProg_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(REGPROG_02_NAME, REGPROG_02_DETAILS);

    numTest = sizeof(testregprog02)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testregprog02, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_RegProg, REGPROG_02_NAME, sts);
    return sts;
}


#define REGPROG_03_NAME    "regprog_03"
#define REGPROG_03_DETAILS "Req-Rel-Rel, expect NO errors"

static AdsppmTestType testregprog03[] =
{
    {1, 5, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {REG, REQREGP, RELREGP, RELREGP, DREG}
    }
};

/**
 * @ingroup RegProg
 * @test regprog_03
 * @brief Req-Rel-Rel, expect NO errors
 */
static MMPM_STATUS Test_RegProg_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(REGPROG_03_NAME, REGPROG_03_DETAILS);

    numTest = sizeof(testregprog03)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testregprog03, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_RegProg, REGPROG_03_NAME, sts);
    return sts;
}


#define REGPROG_04_NAME    "regprog_04"
#define REGPROG_04_DETAILS "Rel with no Req, expect NO errors"

static AdsppmTestType testregprog04[] =
{
    {1, 3, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {REG, RELREGP, DREG}
    }
};

/**
 * @ingroup RegProg
 * @test regprog_04
 * @brief Rel with no Req, expect NO errors
 */
static MMPM_STATUS Test_RegProg_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(REGPROG_04_NAME, REGPROG_04_DETAILS);

    numTest = sizeof(testregprog04)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testregprog04, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_RegProg, REGPROG_04_NAME, sts);
    return sts;
}


#define REGPROG_05_NAME    "regprog_05"
#define REGPROG_05_DETAILS "Client 1 Req, client 2 Rel; client 2 Rel should not affect client 1 Req"

static AdsppmTestType testregprog05[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {REG, REQREGP}
    },
    {2, 2, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {REG, RELREGP}
    },
    {1, 1, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {DREG}
    },
    {2, 1, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {DREG}
    }
};

/**
 * @ingroup RegProg
 * @test regprog_05
 * @brief Client 1 Req, client 2 Rel; client 2 Rel should not affect client 1 Req
 */
static MMPM_STATUS Test_RegProg_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(REGPROG_05_NAME, REGPROG_05_DETAILS);

    numTest = sizeof(testregprog05)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testregprog05, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_RegProg, REGPROG_05_NAME, sts);
    return sts;
}


#define REGPROG_06_NAME    "regprog_06"
#define REGPROG_06_DETAILS "Req(NORM)-Req(FAST)-Req(NORM)-Rel; speed change"

static MMPM_STATUS TestRegProgSpeedChange(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_LPM;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, REGPROG_06_NAME);
    if(clientId)
    {
        sts = Test_RequestRegProg(clientId, MMPM_REG_PROG_NORM);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Error in 1st request");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_RequestRegProg(clientId, MMPM_REG_PROG_FAST);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Error in 2nd request");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_RequestRegProg(clientId, MMPM_REG_PROG_NORM);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Error in 3rd request");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_REG_PROG);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup RegProg
 * @test regprog_06
 * @brief Req(NORM)-Req(FAST)-Req(NORM)-Rel; speed change
 */
static MMPM_STATUS Test_RegProg_06(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(REGPROG_06_NAME, REGPROG_06_DETAILS);

    sts = TestRegProgSpeedChange();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_RegProg, REGPROG_06_NAME, sts);
    return sts;
}


#define REGPROG_07_NAME    "regprog_07"
#define REGPROG_07_DETAILS "Multiple different clients Reqs"

static AdsppmTestType testregprog07[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {REG, REQREGP}
    },
    {2, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {REG, REQREGP}
    },
    {3, 2, MMPM_CORE_ID_LPASS_AIF, MMPM_CORE_INSTANCE_0,
        {REG, REQREGP}
    },
    {4, 2, MMPM_CORE_ID_LPASS_SLIMBUS, MMPM_CORE_INSTANCE_0,
        {REG, REQREGP}
    },
    {1, 1, MMPM_CORE_ID_LPASS_LPM, MMPM_CORE_INSTANCE_0,
        {DREG}
    },
    {2, 1, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {DREG}
    },
    {3, 1, MMPM_CORE_ID_LPASS_AIF, MMPM_CORE_INSTANCE_0,
        {DREG}
    },
    {4, 1, MMPM_CORE_ID_LPASS_SLIMBUS, MMPM_CORE_INSTANCE_0,
        {DREG}
    }
};

/**
 * @ingroup RegProg
 * @test regprog_07
 * @brief Multiple different clients Reqs
 */
static MMPM_STATUS Test_RegProg_07(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(REGPROG_07_NAME, REGPROG_07_DETAILS);

    numTest = sizeof(testregprog07)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testregprog07, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_RegProg, REGPROG_07_NAME, sts);
    return sts;
}


#define REGPROG_08_NAME    "regprog_08"
#define REGPROG_08_DETAILS "Invalid client ID Req, expect failure Req"

static MMPM_STATUS TestRegProgInvalidClientId(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, REGPROG_08_NAME);

    if(clientId)
    {
        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_RequestRegProg(clientId, MMPM_REG_PROG_FAST);
        if(MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Request did not fail as intended");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[PASS] Failed return is intended");
        }
    }
    else
    {
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup RegProg
 * @test regprog_08
 * @brief Invalid client ID Req, expect failure Req
 */
static MMPM_STATUS Test_RegProg_08(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(REGPROG_08_NAME, REGPROG_08_DETAILS);

    sts = TestRegProgInvalidClientId();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_RegProg, REGPROG_08_NAME, sts);
    return sts;
}


#define REGPROG_09_NAME    "regprog_09"
#define REGPROG_09_DETAILS "Invalid parameters Req, expect failure Req"

static MMPM_STATUS TestRegProgInvalidRegProgParam(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_DML;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, REGPROG_09_NAME);

    if(clientId)
    {
        // Bad regProgMatch
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Test enum none regProgMatch");
        sts = Test_RequestRegProg(clientId, MMPM_REG_PROG_NONE);
        if(MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Request returned success when expecting failure");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[PASS] Failed return is intended");
        }

        // Bad regProgMatch
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Test enum max regProgMatch");
        sts = Test_RequestRegProg(clientId, MMPM_REG_PROG_MAX);
        if(MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Request returned success when expecting failure");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[PASS] Failed return is intended");
        }

        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup RegProg
 * @test regprog_09
 * @brief Invalid parameters Req, expect failure Req
 */
static MMPM_STATUS Test_RegProg_09(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(REGPROG_09_NAME, REGPROG_09_DETAILS);

    sts = TestRegProgInvalidRegProgParam();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_RegProg, REGPROG_09_NAME, sts);
    return sts;
}


void Test_RegisterProgramming(void)
{
    // Req-Rel, all core/instance/reg speed combination
    Test_RegProg_01();

    // Req-Rel, 2 same coreID/instanceID clients, expect Rel to turn OFF AHB branch
    Test_RegProg_02();

    // Req-Rel-Rel, expect NO errors
    Test_RegProg_03();

    // Rel with no Req, expect NO errors
    Test_RegProg_04();

    // Client 1 Req, client 2 Rel; client 2 Rel should not affect client 1 Req
    Test_RegProg_05();

    // Req(NORM)-Req(FAST)-Req(NORM)-Rel; speed change
    Test_RegProg_06();

    // Multiple different clients Reqs
    Test_RegProg_07();

    // Invalid client ID Req, expect failure Req
    Test_RegProg_08();

    // Invalid parameters Req, expect failure Req
    Test_RegProg_09();
}

