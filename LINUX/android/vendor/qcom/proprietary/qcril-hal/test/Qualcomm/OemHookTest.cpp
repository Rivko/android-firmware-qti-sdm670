/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#include "gtest/gtest.h"

#include <framework/legacy.h>
#include <framework/Log.h>
#include "../../qcril_qmi/oem_socket/qcril_qmi_oem_reqlist.h"
#include "../../qcril_qmi/oem_socket/qcril_qmi_oem_events.h"


using namespace std;

#define TAG "OEM_TEST"

/* Validate that every EMBMS HOOK REQ has an event defined */
TEST(OemHookSanity, TestEventForAllEmbmsRequests) {

    /* initialize all the oem events map */
    qcril_qmi_oem_events_init();

    int request;
    qcril_evt_e_type event;
    for (request = QCRIL_REQ_HOOK_EMBMS_BASE + 1; request < QCRIL_REQ_HOOK_EMBMS_MAX; request++) {
        event = qcril_qmi_oem_get_event(request);
        if (event == QCRIL_EVT_HOOK_BASE) {
            QCRIL_LOG_ERROR("Error: no event found for request = 0x%08x", request);
            ASSERT_FALSE (event == QCRIL_EVT_HOOK_BASE);
        }
    }
}
#endif