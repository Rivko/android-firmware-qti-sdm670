/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef QVOP_CALLBACK_LNX_H
#define QVOP_CALLBACK_LNX_H

#include "IQvopCallback.h"
#include "logfile.h"

namespace qvop {

    class QvopCallbackLinux : public IQvopCallback {
    public:
        QvopCallbackLinux() {
        }

        // error
        virtual void onError(int32_t error) {
            logfile_print_v("%s: LNX CB entry", QVOP_FN);
        }

        // On a match or match any successful search
        virtual void onMatchFound(const char* user_id,
            const char* identifier,
            int32_t index,
            double sentence_score,
            double ver_score,
            float spoofScore) {
            logfile_print_v("%s: LNX CB entry", QVOP_FN);
        }

        // On a match or match any failed
        virtual void onMatchNotFound(int32_t error,
            int32_t index,
            double sentence_score,
            double ver_score,
            float spoofScore)  {
            logfile_print_v("%s: LNX CB entry", QVOP_FN);
        }

        // Enrollment was successful
        virtual void onEnrollStatus(int32_t status) {
            logfile_print_v("%s: LNX CB entry", QVOP_FN);
        }


        // Delete Status - user/identifier, depending on request
        virtual void onDeleteStatus(int32_t status, const char* user_id,
            const char* identifier) {
            logfile_print_v("%s: LNX CB entry", QVOP_FN);
        }

        // Delete All status - identifier
        virtual void onDeleteAllStatus(int32_t status) {
            logfile_print_v("%s: LNX CB entry", QVOP_FN);
        }

        // Version info
        virtual void onVersionInfo(long major, long minor) {
            logfile_print_v("%s: LNX CB entry", QVOP_FN);
        }

        // Config setting success
        virtual void onConfigStatus(int32_t status) {
            logfile_print_v("%s: LNX CB entry", QVOP_FN);
        }

        // Rename successful
        virtual void onRenameStatus(int32_t status) {
            logfile_print_v("%s: LNX CB entry", QVOP_FN);
        }
    };

}; // namespace qvop

#endif  // QVOP_CALLBACK_LNX_H
