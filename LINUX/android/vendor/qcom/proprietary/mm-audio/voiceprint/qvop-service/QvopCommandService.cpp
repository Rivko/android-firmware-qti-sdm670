/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "QvopCommandService.h"
#include "QvopNullCallback.h"
#include "logfile.h"
#include "qvop_qsee_interface.h"

namespace qvop {

    QvopCommandService::QvopCommandService() :
        mComplete(false),
        mCanceled(false)

    {
        logfile_print_v("%s: entry", QVOP_FN);
    }

    QvopCommandService::~QvopCommandService()
    {
        logfile_print_v("%s: entry", QVOP_FN);
    }

    bool QvopCommandService::isCanceled() const {
        return mCanceled;
    }

    QvopStatus QvopCommandService::tryInitSession() {

        QvopStatus res = QvopSessionInit();

        if (res == QVOP_SUCCESS) {
            return res;
        }

        // pton TODO Subhash will return session in progress.
        logfile_print_e("%s: Session init failed. Try closing the session first.", QVOP_FN);
        res = QvopSessionEnd();

        if (QVOP_SUCCESS != res) {
            logfile_print_v("%s: QvopSessionEnd failed", QVOP_FN);
        }

        // Retry opening session
        res = QvopSessionInit();

        return res;
    }

    QvopStatus QvopCommandService::tryCloseSession() {
        QvopStatus res = QvopSessionEnd();
        if (QVOP_SUCCESS != res) {
            logfile_print_v("%s QvopSessionEnd failed", QVOP_FN);
        }

        logfile_print_v("%s: ret=%d", QVOP_FN, res);

        return res;
    }

    void QvopCommandService::executeCmd(IQvopCallback& callback) {
        logfile_print_v("%s: entry", QVOP_FN);
        if (mComplete) {
            logfile_print_w("%s: attempt to rerun thread after completion", QVOP_FN);
            return;
        }

        // Execute target command
        execute(callback);

        //task complete, mark state as such
        mComplete = true;

        logfile_print_v("%s: exit", QVOP_FN);
    }
}
